/** @file

  Copyright (c) 2020, Byosoft Corporation.<BR>
  All rights reserved.This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.

  File Name:
    SecureBootRestoreLib.c

  Abstract:
    Restore or remove build-in secure boot key.

  Revision History:

  TIME:
  $AUTHOR:
  $REVIEWERS:
  $SCOPE:
  $TECHNICAL:


  T.O.D.O

  $END

--*/
#include "SecureBootRestoreLibPrivate.h"

EFI_STATUS LibAuthVarSetPhysicalPresent(BOOLEAN Present);


#define ARRAY_ELEM_COUNT(a) (sizeof(a) / sizeof((a)[0]))

#define _FREE_NON_NULL(POINTER) \
  do{ \
    if((POINTER) != NULL) { \
      FreePool((POINTER)); \
      (POINTER) = NULL; \
    } \
  } while(FALSE)

typedef struct{
  EFI_GUID *VarGuid;
  CHAR16   *VarName;
} AUTHVAR_KEY_NAME;

AUTHVAR_KEY_NAME gSecureKeyVariableList[] = {
  {&gEfiGlobalVariableGuid, EFI_PLATFORM_KEY_NAME},                 // PK   0
  {&gEfiGlobalVariableGuid, EFI_KEY_EXCHANGE_KEY_NAME},             // KEK  1
  {&gEfiImageSecurityDatabaseGuid, EFI_IMAGE_SECURITY_DATABASE},    // db   2
  {&gEfiImageSecurityDatabaseGuid, EFI_IMAGE_SECURITY_DATABASE1},   // dbx  3
};

// {77fa9abd-0359-4d32-bd60-28f4e78f784b}.
EFI_GUID gMicrosoftSignatureOwnerGuid = \
{0x77fa9abd, 0x0359, 0x4d32, {0xBD, 0x60, 0x28, 0xF4, 0xE7, 0x8F, 0x78, 0x4b}};

// {8db4a6d7-ad6a-4aed-972f-6d1401c81050}
EFI_GUID gMySignatureOwnerGuid = \
{0x8db4a6d7, 0xad6a, 0x4aed, {0x97, 0x2f, 0x6d, 0x14, 0x01, 0xc8, 0x10, 0x50}};

struct{
  EFI_GUID          *File;
  CHAR16            *UiName;
  AUTHVAR_KEY_NAME  *KeyName;
  UINT8             *Data;
  UINTN             DataSize;
  EFI_GUID          *SignatureOwner;
} gSecureKeyTable[] = {
  {(EFI_GUID*)PcdGetPtr(PcdSecureKeyMSDBXFile), L"DBX",   &gSecureKeyVariableList[3], NULL, 0, &gMySignatureOwnerGuid},
  {&gByoUosDbKeyFileGuid,                       L"UOSDB", &gSecureKeyVariableList[2], NULL, 0, &gMySignatureOwnerGuid},
  {(EFI_GUID*)PcdGetPtr(PcdSecureKeyMSKEKFile), L"MSKEK", &gSecureKeyVariableList[1], NULL, 0, &gMicrosoftSignatureOwnerGuid},
  {(EFI_GUID*)PcdGetPtr(PcdSecureKeyMSProFile), L"MSPro", &gSecureKeyVariableList[2], NULL, 0, &gMicrosoftSignatureOwnerGuid},
  {(EFI_GUID*)PcdGetPtr(PcdSecureKeyMSUEFFile), L"MSUEF", &gSecureKeyVariableList[2], NULL, 0, &gMicrosoftSignatureOwnerGuid},
  {(EFI_GUID*)PcdGetPtr(PcdSecureKeyPKFile),    L"PK",    &gSecureKeyVariableList[0], NULL, 0, &gMySignatureOwnerGuid},
#if FixedPcdGetBool(PcdByoKylinDbKeyEnable)
  {&gByoKylinDbKeyFileGuid,                     L"KOSDB", &gSecureKeyVariableList[2], NULL, 0, &gMySignatureOwnerGuid},
#endif

#if FixedPcdGetBool(PcdByoOemDbKeyEnable)
  {&gByoOemDbKeyFileGuid,                       L"OemDB", &gSecureKeyVariableList[2], NULL, 0, &gMySignatureOwnerGuid}
#endif
};


EFI_STATUS
GetTimeStamp (
  EFI_TIME      *Time
  )
{
  EFI_STATUS  Status;

  if (Time == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gRT->GetTime (Time, NULL);
  if (EFI_ERROR (Status)) {
    goto ProcExit;
  };

// Pad1, Nanosecond, TimeZone, Daylight and Pad2 components 
// of the TimeStamp value must be set to zero.
  Time->Pad1       = 0;
  Time->Nanosecond = 0;
  Time->TimeZone   = 0;
  Time->Daylight   = 0;
  Time->Pad2       = 0;

ProcExit:
  return Status;
}

EFI_STATUS
CreateTimeBasedPayload2 (
  IN OUT UINTN            *DataSize,
  IN OUT UINT8            **Data
  )
{
  EFI_STATUS                       Status;
  UINT8                            *NewData;
  UINT8                            *Payload;
  UINTN                            PayloadSize;
  EFI_VARIABLE_AUTHENTICATION_2    *DescriptorData;
  UINTN                            DescriptorSize;
  EFI_TIME                         Time;

  NewData        = NULL;
  Payload        = NULL;
  DescriptorData = NULL;
  if (Data == NULL || DataSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // In Setup mode or Custom mode, the variable does not need to be signed but the
  // parameters to the SetVariable() call still need to be prepared as authenticated
  // variable. So we create EFI_VARIABLE_AUTHENTICATED_2 descriptor without certificate
  // data in it.
  //
  Payload     = *Data;
  PayloadSize = *DataSize;

  DescriptorSize    = OFFSET_OF (EFI_VARIABLE_AUTHENTICATION_2, AuthInfo) + OFFSET_OF (WIN_CERTIFICATE_UEFI_GUID, CertData);
  NewData = (UINT8*) AllocateZeroPool (DescriptorSize + PayloadSize);
  if (NewData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if ((Payload != NULL) && (PayloadSize != 0)) {
    CopyMem (NewData + DescriptorSize, Payload, PayloadSize);
  }

  DescriptorData = (EFI_VARIABLE_AUTHENTICATION_2 *) (NewData);

  ZeroMem (&Time, sizeof (EFI_TIME));
  Status = gRT->GetTime (&Time, NULL);
  if (EFI_ERROR (Status)) {
    FreePool(NewData);
    return Status;
  }
  Time.Pad1       = 0;
  Time.Nanosecond = 0;
  Time.TimeZone   = 0;
  Time.Daylight   = 0;
  Time.Pad2       = 0;
  CopyMem (&DescriptorData->TimeStamp, &Time, sizeof (EFI_TIME));

  DescriptorData->AuthInfo.Hdr.dwLength         = OFFSET_OF (WIN_CERTIFICATE_UEFI_GUID, CertData);
  DescriptorData->AuthInfo.Hdr.wRevision        = 0x0200;
  DescriptorData->AuthInfo.Hdr.wCertificateType = WIN_CERT_TYPE_EFI_GUID;
  CopyGuid (&DescriptorData->AuthInfo.CertType, &gEfiCertPkcs7Guid);

  if (Payload != NULL) {
    FreePool(Payload);
  }

  *DataSize = DescriptorSize + PayloadSize;
  *Data     = NewData;
  return EFI_SUCCESS;
}

EFI_STATUS
ReadFileContent2 (
IN      EFI_FILE_HANDLE           FileHandle,
  IN OUT  VOID                      **BufferPtr,
     OUT  UINTN                     *FileSize,
  IN      UINTN                     AdditionAllocateSize
  )

{
  UINTN      BufferSize;
  UINT64     SourceFileSize;
  VOID       *Buffer;
  EFI_STATUS Status;

  Buffer = NULL;
  if ((FileHandle == NULL) || (FileSize == NULL)) {
    return EFI_INVALID_PARAMETER;
  }


  //
  // Get the file size
  //
  Status = FileHandle->SetPosition (FileHandle, (UINT64) -1);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  Status = FileHandle->GetPosition (FileHandle, &SourceFileSize);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  Status = FileHandle->SetPosition (FileHandle, 0);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  BufferSize = (UINTN) SourceFileSize + AdditionAllocateSize;
  Buffer =  AllocateZeroPool(BufferSize);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  BufferSize = (UINTN) SourceFileSize;
  *FileSize  = BufferSize;

  Status = FileHandle->Read (FileHandle, &BufferSize, Buffer);
  if (EFI_ERROR (Status) || BufferSize != *FileSize) {
    FreePool (Buffer);
    Buffer = NULL;
    Status  = EFI_BAD_BUFFER_SIZE;
    goto ON_EXIT;
  }

ON_EXIT:

  *BufferPtr = Buffer;
  return Status;
}

VOID
FreeLoadedKeys (
  VOID
  )
{
  UINTN   Index;
  
  for (Index = 0; Index < ARRAY_ELEM_COUNT(gSecureKeyTable); Index++) {
    _FREE_NON_NULL(gSecureKeyTable[Index].Data);
    gSecureKeyTable[Index].DataSize = 0;
  }
}

EFI_STATUS
LoadKeysInFv (
  VOID
  )
{
  EFI_STATUS   Status;
  UINTN        FileSize;
  UINT8        *FileBuffer;
  UINTN        Index;

  for (Index = 0; Index < ARRAY_ELEM_COUNT(gSecureKeyTable); Index++) {
    FileBuffer = NULL;
    FileSize   = 0;

    Status = GetSectionFromAnyFv (
               gSecureKeyTable[Index].File,
               EFI_SECTION_RAW,
               0,
               (VOID**)&FileBuffer,
               &FileSize
               );
    if (EFI_ERROR (Status)) {
      goto ProcExit;
    }

    gSecureKeyTable[Index].Data     = FileBuffer;
    gSecureKeyTable[Index].DataSize = FileSize;
  }

ProcExit:

  if (EFI_ERROR (Status)) {
    FreeLoadedKeys ();
  }
  return Status;
}

EFI_STATUS
AppendX509FromFV (
  IN EFI_GUID                         *CertificateGuid,
  IN  CHAR16                          *VariableName,
  IN EFI_GUID                         *VendorGuid,
  IN EFI_GUID                         *SignatureOwner
  )
{
  EFI_STATUS                        Status;
  VOID                              *Data;
  UINTN                             DataSize;
  UINTN                             SigDBSize;
  UINT32                            Attr;
  UINTN                             X509DataSize;
  VOID                              *X509Data;

  X509DataSize  = 0;
  X509Data      = NULL;
  SigDBSize     = 0;
  DataSize      = 0;
  Data          = NULL;

  Status = GetSectionFromAnyFv (
              CertificateGuid,
              EFI_SECTION_RAW,
              0,
              &X509Data,
              &X509DataSize
              );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  SigDBSize = X509DataSize;

  Data = AllocateZeroPool (SigDBSize);
  if (Data == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  CopyMem ((UINT8* )Data, X509Data, X509DataSize);

  Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS 
         | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;

  //
  // Check if signature database entry has been already existed.
  // If true, use EFI_VARIABLE_APPEND_WRITE attribute to append the
  // new signature data to original variable
  //

  Status = gRT->GetVariable (
                  VariableName,
                  VendorGuid,
                  NULL,
                  &DataSize,
                  NULL
                  );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    Attr |= EFI_VARIABLE_APPEND_WRITE;
  } else if (Status != EFI_NOT_FOUND) {
    goto ON_EXIT;
  }  

  Status = gRT->SetVariable (
                  VariableName,
                  VendorGuid,
                  Attr,
                  SigDBSize,
                  Data
                  );

  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

ON_EXIT:
  
  if (Data != NULL) {
    FreePool (Data);
  }

  if (X509Data != NULL) {
    FreePool (X509Data);
  }

  return Status;
}

EFI_STATUS
CreateDummyTimeBasedPayload (
  IN OUT UINTN            *DataSize,
  IN OUT UINT8            **Data
  )
{
  EFI_STATUS                       Status;
  UINT8                            *NewData;
  UINT8                            *Payload;
  UINTN                            PayloadSize;
  EFI_VARIABLE_AUTHENTICATION_2    *DescriptorData;
  UINTN                            DescriptorSize;
  EFI_TIME                         Time;

  Status         = EFI_SUCCESS;
  NewData        = NULL;
  DescriptorData = NULL;
  Payload        = NULL;
  if ((Data == NULL) || (DataSize == NULL)) {
    return EFI_INVALID_PARAMETER;
  }


  //
  // In Setup mode or Custom mode, the variable does not need to be signed but the 
  // parameters to the SetVariable() call still need to be prepared as authenticated
  // variable. So we create EFI_VARIABLE_AUTHENTICATED_2 descriptor without certificate
  // data in it.
  //
  Payload     = *Data;
  PayloadSize = *DataSize;

// EFI_VARIABLE_AUTHENTICATION_2
//   TimeStamp(EFI_TIME)
//   AuthInfo(WIN_CERTIFICATE_UEFI_GUID) 
//     Hdr(WIN_CERTIFICATE) 
//       dwLength
//       wRevision
//       wCertificateType
//     CertType(EFI_GUID)
//     CertData(UINT8[1])
  DescriptorSize = OFFSET_OF (EFI_VARIABLE_AUTHENTICATION_2, AuthInfo) +   // TimeStamp
                   OFFSET_OF (WIN_CERTIFICATE_UEFI_GUID, CertData);        // Hdr + CertType
  NewData = AllocateZeroPool (DescriptorSize + PayloadSize);
  if (NewData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }

  if ((Payload != NULL) && (PayloadSize != 0)) {
    CopyMem (NewData + DescriptorSize, Payload, PayloadSize);
  }

  DescriptorData = (EFI_VARIABLE_AUTHENTICATION_2*) NewData;
  Status = GetTimeStamp (&Time);
  if (EFI_ERROR (Status)) {
    goto ProcExit;
  };
  CopyMem (&DescriptorData->TimeStamp, &Time, sizeof (EFI_TIME));
 
  DescriptorData->AuthInfo.Hdr.dwLength         = OFFSET_OF (WIN_CERTIFICATE_UEFI_GUID, CertData);
  DescriptorData->AuthInfo.Hdr.wRevision        = 0x0200;
  DescriptorData->AuthInfo.Hdr.wCertificateType = WIN_CERT_TYPE_EFI_GUID;
  CopyGuid (&DescriptorData->AuthInfo.CertType, &gEfiCertPkcs7Guid);

  *DataSize = DescriptorSize + PayloadSize;
  *Data     = NewData;

ProcExit:
  if (EFI_ERROR (Status) && NewData!=NULL) {
    gBS->FreePool (NewData);
  }
  return Status;
}

EFI_STATUS
DeleteSecureBootVariableNoAuth (
  IN  CHAR16     *VariableName,
  IN  EFI_GUID   *VendorGuid
  )
{
  EFI_STATUS              Status;
  VOID*                   Variable;
  UINTN                   VarSize;
  UINT8                   *Data;
  UINTN                   DataSize;
  UINT32                  Attribute;

  Status   = EFI_SUCCESS;
  Variable = NULL;
  VarSize  = 0;
  Data     = NULL;

  Status = gRT->GetVariable (
                  VariableName,
                  VendorGuid,
                  &Attribute,   // bug:if DataSize is not big enough, Attribute will not be updated.
                  &VarSize,
                  Variable
                  );
  if (Status != EFI_BUFFER_TOO_SMALL) {   // not found
    Status = EFI_SUCCESS;
    goto ProcExit;
  }
  Variable = AllocatePool (VarSize);
  if (Variable == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }
  Status = gRT->GetVariable (
                  VariableName,
                  VendorGuid,
                  &Attribute,   // now attribute is correct.
                  &VarSize,
                  Variable
                  );
  gBS->FreePool (Variable);
  if (EFI_ERROR (Status)) {
    goto ProcExit;
  }

  DataSize = 0;
  Status = CreateDummyTimeBasedPayload (&DataSize, &Data);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "(L%d) %r 0x%X 0x%X\n", __LINE__, Status, Data, DataSize));
    goto ProcExit;
  }

  Status = gRT->SetVariable (
                  VariableName,
                  VendorGuid,
                  Attribute,
                  DataSize,
                  Data
                  );
  _FREE_NON_NULL (Data);

ProcExit:
  return Status;
}



EFI_STATUS 
AddSecureBootVarNoAuth(
        CHAR16        *VarName, 
        EFI_GUID      *VarGuid, 
        UINT8         *CertData, 
        UINTN         CertDataSize,
        EFI_GUID      *SignatureOwner
  )
{
  EFI_STATUS                     Status;
  EFI_SIGNATURE_LIST             *SignList;
  UINTN                          SignListSize;
  UINT8                          *VarData;
  UINTN                          VarDataSize;
  UINT32                         Attribute;
  UINTN                          DataSize;
  EFI_SIGNATURE_DATA             *SignData;

  SignList = NULL;
  VarData  = NULL;
  SignData = NULL; 

  SignListSize = sizeof (EFI_SIGNATURE_LIST) + OFFSET_OF (EFI_SIGNATURE_DATA, SignatureData) + CertDataSize;
  SignList     = (EFI_SIGNATURE_LIST*) AllocatePool (SignListSize);
  if (SignList == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }
 
  CopyMem (&SignList->SignatureType, &gEfiCertX509Guid, sizeof (EFI_GUID));
  SignList->SignatureListSize   = (UINT32) SignListSize;
  SignList->SignatureHeaderSize = 0;
  SignList->SignatureSize       = (UINT32) (OFFSET_OF (EFI_SIGNATURE_DATA, SignatureData) + CertDataSize);
  SignData = (EFI_SIGNATURE_DATA*) ((UINT8*) SignList + sizeof (EFI_SIGNATURE_LIST));
  CopyMem (&SignData->SignatureOwner, SignatureOwner, sizeof (EFI_GUID));
  CopyMem (&SignData->SignatureData[0], CertData, CertDataSize);

  VarData     = (UINT8*) SignList;
  VarDataSize = SignListSize;
  Status = CreateDummyTimeBasedPayload (&VarDataSize, &VarData);
  if (EFI_ERROR (Status)) {
    if ((UINTN) VarData == (UINTN) SignList) {
      VarData = NULL;
    }
    goto ProcExit;
  }

  Attribute = EFI_VARIABLE_NON_VOLATILE | 
              EFI_VARIABLE_BOOTSERVICE_ACCESS | 
              EFI_VARIABLE_RUNTIME_ACCESS |
              EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;

  if (StrCmp (VarName, EFI_PLATFORM_KEY_NAME) != 0) {
    DataSize = 0;
    Status = gRT->GetVariable(
                    VarName,
                    VarGuid,
                    NULL,
                    &DataSize,
                    NULL
                    );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      Attribute |= EFI_VARIABLE_APPEND_WRITE;
    }
  }

  if (StrCmp (VarName, EFI_IMAGE_SECURITY_DATABASE1) != 0) {
    Status = gRT->SetVariable (
                    VarName,
                    VarGuid,
                    Attribute,
                    VarDataSize,
                    VarData
                    );
  DEBUG ((EFI_D_INFO, "Set: VarName = %s, VarGuid = %g, DataSize=%x, Status = %r\n", VarName, VarGuid, VarDataSize, Status));
  } else {
  //
  // Directly set AUTHENTICATION_2 data to SetVariable
  //
    Status = gRT->SetVariable (
                    VarName,
                    VarGuid,
                    Attribute,
                    CertDataSize,
                    CertData
                    );
  DEBUG ((EFI_D_INFO, "Set: VarName = %s, VarGuid = %g, DataSize=%x, Status = %r\n", VarName, VarGuid, CertDataSize, Status));
  }

ProcExit:
  _FREE_NON_NULL(SignList);
  _FREE_NON_NULL(VarData);
  return Status;
}

EFI_STATUS
SetCustomMode (
  IN     UINT8         SecureBootMode
  )
{
  return gRT->SetVariable (
                EFI_CUSTOM_MODE_NAME,
                &gEfiCustomModeEnableGuid,
                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                sizeof (UINT8),
                &SecureBootMode
                );
}

EFI_STATUS
SetVariableInDeployedMode (
  IN UINT8                         DeployedVarValue
  )
{
  EFI_STATUS                       Status = EFI_SUCCESS;
  
  Status = gRT->SetVariable(
                    EFI_DEPLOYED_MODE_NAME,
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof(UINT8),
                    &DeployedVarValue
                    );
  if(EFI_ERROR (Status)){
    DEBUG((DEBUG_INFO,"Deployed Var Set Failed!\n"));
    ASSERT(Status);
  }

  return Status;
}


EFI_STATUS
AddMfgKeys (
  VOID
  )
{
  UINTN         Index;
  CHAR16        *VarName;
  EFI_GUID      *VarGuid;
  EFI_STATUS    Status;
  UINT8         *CertData;
  UINTN         CertDataSize;
  EFI_GUID      *SignatureOwner;
  UINTN         DataSize;
  UINT8         *Data = NULL;

  //UINT8         *SecureBootMode;

  Status         = EFI_SUCCESS;
  VarName        = NULL;
  VarGuid        = NULL;
  SignatureOwner = NULL;
  CertData       = NULL;
  //SecureBootMode = NULL;


  LibAuthVarSetPhysicalPresent(TRUE);  
  SetCustomMode (CUSTOM_SECURE_BOOT_MODE);
  for (Index = 0; Index < ARRAY_ELEM_COUNT(gSecureKeyTable); Index++) {
    VarName        = gSecureKeyTable[Index].KeyName->VarName;
    VarGuid        = gSecureKeyTable[Index].KeyName->VarGuid;
    CertData       = gSecureKeyTable[Index].Data;
    CertDataSize   = gSecureKeyTable[Index].DataSize;
    SignatureOwner = gSecureKeyTable[Index].SignatureOwner;

    if(CertDataSize == 0){
      DEBUG((EFI_D_ERROR, "[%d](%s) not found\n", Index, gSecureKeyTable[Index].UiName));
      continue;
    }
    
    Status = AddSecureBootVarNoAuth (VarName, VarGuid, CertData, CertDataSize, SignatureOwner);
    DEBUG((EFI_D_INFO,"CUSTOM_SECURE_BOOT_MODE "));
    DEBUG((EFI_D_INFO,"VarName = %s, VarGuid = %g, CertDataSize=%x, Status = %r\n", VarName, VarGuid, CertDataSize, Status));
  }
  SetCustomMode (STANDARD_SECURE_BOOT_MODE);
  LibAuthVarSetPhysicalPresent(FALSE);  

  for (Index = 0; Index < ARRAY_ELEM_COUNT(gSecureKeyTable); Index++) {
    DataSize = 0;
    VarName  = gSecureKeyTable[Index].KeyName->VarName;
    VarGuid  = gSecureKeyTable[Index].KeyName->VarGuid;
    Status   = gRT->GetVariable (
                      VarName,
                      VarGuid,
                      NULL,
                      &DataSize,
                      NULL
                      );
    DEBUG((EFI_D_INFO,"STANDARD_SECURE_BOOT_MODE "));
    DEBUG ((EFI_D_INFO, "VarName = %s, VarGuid = %g, DataSize=%x, Status = %r\n", VarName, VarGuid, DataSize, Status));
    if (Status == EFI_BUFFER_TOO_SMALL) {
      Data = (UINT8 *) AllocateZeroPool (DataSize);
      if (Data != NULL) {
        Status = gRT->GetVariable (
                        VarName,
                        VarGuid,
                        NULL,
                        &DataSize,
                        Data
                        );
        FreePool (Data);
        DEBUG ((EFI_D_INFO, "VarName = %s, VarGuid = %g, DataSize=%x, Status 1 = %r\n", VarName, VarGuid, DataSize, Status));
      }
    }
  }

  //
  // SecureBoot Auto Provision
  // SetupMode/UserMode/AuditMode->DeployedMode
  //
  //*SecureBootMode = SECURE_BOOT_MODE_DEPLOYED_MODE;

  Status = SetVariableInDeployedMode(1); // SetupModeVar=0,AuditModeVar=0,DeployedModeVar=1 => SecureBootMode is DeployedMode
  DEBUG ((EFI_D_INFO, "AddMfgKeys Deployed Mode\n"));

  return Status;
}

EFI_STATUS
DelVariable (
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid
  )
{
  EFI_STATUS              Status;
  VOID*                   Variable;
  UINT8                   *Data;
  UINTN                   DataSize;
  UINT32                  Attr;
  Data     = NULL;
  Variable = NULL;

  GetVariable2 (VariableName, VendorGuid, &Variable, NULL);
  if (Variable == NULL) {
    return EFI_SUCCESS;
  }
  FreePool (Variable);

  DataSize = 0;
  Attr     = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS
             | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;

  Status = CreateTimeBasedPayload2 (&DataSize, &Data);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
    return Status;
  }

  Status = gRT->SetVariable (
                  VariableName,
                  VendorGuid,
                  Attr,
                  DataSize,
                  Data
                  );
  if (Data != NULL) {
    FreePool (Data);
  }
  return Status;
}

EFI_STATUS
RemoveAllCertKey (
  VOID
  )
{
  EFI_STATUS      Status;

  Status = EFI_SUCCESS;

  LibAuthVarSetPhysicalPresent(TRUE);
  SetCustomMode (CUSTOM_SECURE_BOOT_MODE);
  Status = DelVariable (
             EFI_PLATFORM_KEY_NAME,
             &gEfiGlobalVariableGuid
             );
  Status = DelVariable (
             EFI_KEY_EXCHANGE_KEY_NAME,
             &gEfiGlobalVariableGuid
             );
  Status = DelVariable (
             EFI_IMAGE_SECURITY_DATABASE,
             &gEfiImageSecurityDatabaseGuid
             );
  Status = DelVariable (
             EFI_IMAGE_SECURITY_DATABASE1,
             &gEfiImageSecurityDatabaseGuid
             );
  Status = DelVariable (
             EFI_IMAGE_SECURITY_DATABASE2,
             &gEfiImageSecurityDatabaseGuid
             );
  SetCustomMode (STANDARD_SECURE_BOOT_MODE);
  LibAuthVarSetPhysicalPresent(FALSE);  
  DEBUG((DEBUG_INFO,"RemoveAllCertKey() Status is %r\n",Status));
  return Status;
}

EFI_STATUS
AuthVarMfgReset (
  VOID
  )
{
  EFI_STATUS  Status;
  DEBUG((DEBUG_INFO,"AuthVarMfgReset START"));
  Status = LoadKeysInFv ();
  if (EFI_ERROR (Status)) {
    goto ProcExit;
  }
  RemoveAllCertKey ();
  AddMfgKeys ();
  FreeLoadedKeys ();
  DEBUG((DEBUG_INFO,"AuthVarMfgReset end"));
ProcExit:
  return Status;
}
