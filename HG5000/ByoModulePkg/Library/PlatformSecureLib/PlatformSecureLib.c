/** @file
  Provides a platform-specific method to enable Secure Boot Custom Mode setup.

  Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <Uefi.h>
#include <Guid/AuthenticatedVariableFormat.h>
#include <Guid/ImageAuthentication.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/ByoSecureLib.h>
#include <Protocol/FirmwareVolume2.h>


STATIC AUTHVAR_KEY_NAME gSecureKeyVariableList[] = {
  {&gEfiGlobalVariableGuid,        EFI_PLATFORM_KEY_NAME},          // PK    0
  {&gEfiGlobalVariableGuid,        EFI_KEY_EXCHANGE_KEY_NAME},      // KEK  1
  {&gEfiImageSecurityDatabaseGuid, EFI_IMAGE_SECURITY_DATABASE},    // db    2
  {&gEfiImageSecurityDatabaseGuid, EFI_IMAGE_SECURITY_DATABASE1},   // dbx  3
};

STATIC RESTORED_KEY_FILE gSecureKeyTable[] = {
  {(EFI_GUID*)PcdGetPtr(PcdSecureKeyMSDBXFile), L"MSDBX", &gSecureKeyVariableList[3], NULL, 0, &gMicrosoftSignatureOwnerGuid},
  {(EFI_GUID*)PcdGetPtr(PcdSecureKeyMSKEKFile), L"MSKEK", &gSecureKeyVariableList[1], NULL, 0, &gMicrosoftSignatureOwnerGuid},
  {(EFI_GUID*)PcdGetPtr(PcdSecureKeyMSProFile), L"MSPro", &gSecureKeyVariableList[2], NULL, 0, &gMicrosoftSignatureOwnerGuid},
  {(EFI_GUID*)PcdGetPtr(PcdSecureKeyMSUEFFile), L"MSUEF", &gSecureKeyVariableList[2], NULL, 0, &gMicrosoftSignatureOwnerGuid},
  {(EFI_GUID*)PcdGetPtr(PcdSecureKeyPKFile),    L"PK",    &gSecureKeyVariableList[0], NULL, 0, &gMySignatureOwnerGuid},
  //{&gByoUosDbKeyFileGuid, 					    L"DB",	  &gSecureKeyVariableList[2], NULL, 0, &gMySignatureOwnerGuid},
};

UINT32  *gSecurePPFlag = NULL;
  
EFI_STATUS
LibAuthVarSetPhysicalPresent(
  BOOLEAN Present
)
{
  if(Present){
    *gSecurePPFlag = PP_VALID_FLAG;
  } else {
    *gSecurePPFlag = 0;
  }

  return EFI_SUCCESS;  
} 

/**

  This function provides a platform-specific method to detect whether the platform
  is operating by a physically present user. 

  Programmatic changing of platform security policy (such as disable Secure Boot,
  or switch between Standard/Custom Secure Boot mode) MUST NOT be possible during
  Boot Services or after exiting EFI Boot Services. Only a physically present user
  is allowed to perform these operations.

  NOTE THAT: This function cannot depend on any EFI Variable Service since they are
  not available when this function is called in AuthenticateVariable driver.
  
  @retval  TRUE       The platform is operated by a physically present user.
  @retval  FALSE      The platform is NOT operated by a physically present user.

**/
BOOLEAN
EFIAPI
UserPhysicalPresent (
  VOID
  )
{
  return (BOOLEAN)(*gSecurePPFlag == PP_VALID_FLAG);  
}

VOID
FreeLoadedKeys()
{
  UINTN   Index; 
  
  for (Index = 0; Index < sizeof(gSecureKeyTable)/sizeof(RESTORED_KEY_FILE); Index++) {
	if (gSecureKeyTable[Index].Data != NULL) {
      FreePool(gSecureKeyTable[Index].Data);
	}
    gSecureKeyTable[Index].DataSize = 0;
  }
}

EFI_STATUS
LoadKeysFromFv(
  VOID
)
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  UINTN                         Index;  
  EFI_FV_FILETYPE               FileType;
  UINT32                        AuthStatus;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINTN                         FileSize;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *ptFV2;
  UINT8                         *FileBuffer;

  
  HandleBuffer = NULL;
  ptFV2        = NULL;
  
  Status = gBS->LocateHandleBuffer (
                   ByProtocol,
                   &gEfiFirmwareVolume2ProtocolGuid,
                   NULL,
                   &NumberOfHandles,
                   &HandleBuffer
                   );
  if (EFI_ERROR(Status)) {
    goto ProcExit;
  }
  
  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                     HandleBuffer[Index],
                     &gEfiFirmwareVolume2ProtocolGuid,
                     (VOID**)&ptFV2
                     );
    ASSERT(!EFI_ERROR(Status));
    Status = ptFV2->ReadFile (
                      ptFV2,
                      gSecureKeyTable[0].File,
                      NULL,        // get info only
                      &FileSize,
                      &FileType,
                      &Attributes,
                      &AuthStatus
                      );
    if (!EFI_ERROR(Status)) {
      break;
    }
  }
  if (Index == NumberOfHandles) {
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }
  
  for (Index = 0; Index < sizeof(gSecureKeyTable)/sizeof(RESTORED_KEY_FILE); Index++) {
    FileBuffer = NULL;
    FileSize   = 0;
    Status = ptFV2->ReadSection(
                      ptFV2,
                      gSecureKeyTable[Index].File,
                      EFI_SECTION_RAW,
                      0,              // SectionInstance
                      (VOID**)&FileBuffer,
                      &FileSize,
                      &AuthStatus
                      );
    if (EFI_ERROR(Status)) {
      goto ProcExit;
    }               
    
    gSecureKeyTable[Index].Data     = FileBuffer;
    gSecureKeyTable[Index].DataSize = FileSize;
  }
  
ProcExit:
  if (HandleBuffer != NULL) {
    FreePool(HandleBuffer);
  }
  if (EFI_ERROR(Status)) {
    FreeLoadedKeys();
  }
  return Status;   
}

EFI_STATUS
GetTimeStamp(
  EFI_TIME *Time
)
{
  EFI_STATUS  Status;
  
  ASSERT(Time != NULL);
  
  Status = gRT->GetTime(Time, NULL);
  if (EFI_ERROR(Status)) {
    goto ProcExit;
  }
	
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
  
  
  ASSERT(Data != NULL && DataSize != NULL);
  Status  = EFI_SUCCESS;
  NewData = NULL;  
  
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
  DescriptorSize = OFFSET_OF(EFI_VARIABLE_AUTHENTICATION_2, AuthInfo) +   // TimeStamp 
                   OFFSET_OF(WIN_CERTIFICATE_UEFI_GUID, CertData);        // Hdr + CertType
  NewData = AllocateZeroPool(DescriptorSize + PayloadSize);
  if (NewData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }

  if ((Payload != NULL) && (PayloadSize != 0)) {
    CopyMem(NewData + DescriptorSize, Payload, PayloadSize);
  }

  DescriptorData = (EFI_VARIABLE_AUTHENTICATION_2*)NewData;
  Status = GetTimeStamp(&Time);
  if (EFI_ERROR(Status)) {
    goto ProcExit;
  }
  CopyMem(&DescriptorData->TimeStamp, &Time, sizeof(EFI_TIME));
 
  DescriptorData->AuthInfo.Hdr.dwLength         = OFFSET_OF(WIN_CERTIFICATE_UEFI_GUID, CertData);
  DescriptorData->AuthInfo.Hdr.wRevision        = 0x0200;
  DescriptorData->AuthInfo.Hdr.wCertificateType = WIN_CERT_TYPE_EFI_GUID;
  CopyGuid (&DescriptorData->AuthInfo.CertType, &gEfiCertPkcs7Guid);
  
  *DataSize = DescriptorSize + PayloadSize;
  *Data     = NewData;
  
ProcExit:
  if (EFI_ERROR(Status) && NewData != NULL) {
    FreePool(NewData);
  }
  return Status;
}

EFI_STATUS
DelSecureBootKeyVariable (
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
  
  Status = gRT->GetVariable(
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
  Variable = AllocatePool(VarSize);
  if (Variable == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }
  Status = gRT->GetVariable(
                  VariableName,
                  VendorGuid,
                  &Attribute,   // now attribute is correct.
                  &VarSize,
                  Variable       
                  );
  FreePool(Variable);
  if (EFI_ERROR(Status)) {
    goto ProcExit;
  }
  
  Data     = NULL;
  DataSize = 0;
  Status = CreateDummyTimeBasedPayload(&DataSize, &Data);
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "(L%d) %r 0x%X 0x%X\n", Status, Data, DataSize));
    goto ProcExit;
  }

  Status = gRT->SetVariable (
                  VariableName,
                  VendorGuid,
                  Attribute,
                  DataSize,
                  Data
                  );
  if (Data != NULL) {
    FreePool(Data);
  }

ProcExit:  
  return Status;
}

EFI_STATUS 
AddSecureBootKeyVariable(
        CHAR16        *VarName, 
        EFI_GUID      *VarGuid, 
  CONST UINT8         *CertData, 
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
  
  if (StrCmp(VarName, EFI_IMAGE_SECURITY_DATABASE1) == 0) {
  	DEBUG((EFI_D_INFO, "AddSecureBootKeyVariable: %s - 0x%x\n", VarName, CertDataSize));
  	VarDataSize = CertDataSize;
	VarData = AllocateZeroPool(VarDataSize);
	if (VarData == NULL) {
	  Status = EFI_OUT_OF_RESOURCES;
	  goto ProcExit;
	}
	CopyMem (VarData, CertData, VarDataSize);
  } else {
    DEBUG((EFI_D_INFO, "AddSecureBootKeyVariable: 0x%x\n", CertDataSize));
    SignListSize = sizeof(EFI_SIGNATURE_LIST) + OFFSET_OF(EFI_SIGNATURE_DATA, SignatureData) + CertDataSize;
    SignList     = (EFI_SIGNATURE_LIST*)AllocateZeroPool(SignListSize);
    if (SignList == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ProcExit;
    }
 
    CopyMem(&SignList->SignatureType, &gEfiCertX509Guid, sizeof(EFI_GUID));
    SignList->SignatureListSize   = (UINT32)SignListSize;
    SignList->SignatureHeaderSize = 0;
    SignList->SignatureSize       = (UINT32)(OFFSET_OF(EFI_SIGNATURE_DATA, SignatureData) + CertDataSize);
    SignData = (EFI_SIGNATURE_DATA*)((UINT8*)SignList + sizeof(EFI_SIGNATURE_LIST));
    CopyMem(&SignData->SignatureOwner, SignatureOwner, sizeof(EFI_GUID));
    CopyMem(&SignData->SignatureData[0], CertData, CertDataSize);

    VarData     = (UINT8*)SignList;
    VarDataSize = SignListSize;
    Status = CreateDummyTimeBasedPayload(&VarDataSize, &VarData);
    if (EFI_ERROR(Status)) {
      if ((UINTN)VarData == (UINTN)SignList) {
        VarData = NULL;
      }  
      goto ProcExit;
    }
  }

  Attribute = EFI_VARIABLE_NON_VOLATILE | 
              EFI_VARIABLE_BOOTSERVICE_ACCESS | 
              EFI_VARIABLE_RUNTIME_ACCESS |
              EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
  
  if (StrCmp(VarName, EFI_PLATFORM_KEY_NAME) != 0) {
    DataSize = 0;
    Status = gRT->GetVariable(
                    VarName,
                    VarGuid,
                    NULL,
                    &DataSize,
                    NULL
                    );
	DEBUG((EFI_D_INFO, "AddSecureBootKeyVariable: GetVariable - %r\n", Status));
    if (Status == EFI_BUFFER_TOO_SMALL) {
      Attribute |= EFI_VARIABLE_APPEND_WRITE;
    }
  }  
  
  Status = gRT->SetVariable (
                  VarName,
                  VarGuid,
                  Attribute,
                  VarDataSize,
                  VarData
                  );
  DEBUG((EFI_D_INFO, "AddSecureBootKeyVariable: SetVariable - %r\n", Status));

ProcExit:
  if (SignList) {
    FreePool(SignList);
  }
  if (VarData) {
    FreePool(VarData);
  }
  return Status;
}

EFI_STATUS
SetCustomMode(
  UINT8       Mode
)
{
  EFI_STATUS  Status;
  UINT8       CustomMode;
  UINTN       DataSize;
  UINT32      Attribute;

  DEBUG((EFI_D_INFO, "%a(%d)\n", __FUNCTION__, Mode)); 
  
  Status = EFI_SUCCESS;  
  DataSize = sizeof(CustomMode);
  Status = gRT->GetVariable(
                  EFI_CUSTOM_MODE_NAME,
                  &gEfiCustomModeEnableGuid,
                  &Attribute,
                  &DataSize,
                  &CustomMode
                  );
  if (EFI_ERROR(Status)) {
    goto ProcExit;
  }
  
  if (CustomMode != Mode) {
    CustomMode = Mode;
    Status = gRT->SetVariable(                          
                    EFI_CUSTOM_MODE_NAME,
                    &gEfiCustomModeEnableGuid,
                    Attribute,
                    sizeof(CustomMode),
                    &CustomMode
                    );
  }

ProcExit:
  return Status;
}

EFI_STATUS
AuthRemoveAllCertKeys(
  VOID
)
{
  UINTN           Index;
  EFI_STATUS      Status;
  
  Status = EFI_SUCCESS;
  
  LibAuthVarSetPhysicalPresent(TRUE);
  SetCustomMode(CUSTOM_SECURE_BOOT_MODE);
  for (Index = 0; Index < sizeof(gSecureKeyVariableList)/sizeof(AUTHVAR_KEY_NAME); Index++) {
    Status = DelSecureBootKeyVariable(gSecureKeyVariableList[Index].VarName, gSecureKeyVariableList[Index].VarGuid);
	DEBUG((EFI_D_INFO, "%a: DelSecureBootKeyVariable %r\n", __FUNCTION__, Status)); 
  }
  SetCustomMode(STANDARD_SECURE_BOOT_MODE);
  LibAuthVarSetPhysicalPresent(FALSE);
  
  return Status;
}

EFI_STATUS
AuthRestoreFactoryKeys (
  VOID
)
{
  UINTN           Index;
  EFI_STATUS      Status;
  
  Status = LoadKeysFromFv();
  if (EFI_ERROR(Status)) {
  	DEBUG((EFI_D_ERROR, "LoadKeysFromFv %r\n", Status));
	return Status;
  }
  
  LibAuthVarSetPhysicalPresent(TRUE);  
  SetCustomMode(CUSTOM_SECURE_BOOT_MODE);  
  for (Index = 0; Index < sizeof(gSecureKeyTable)/sizeof(RESTORED_KEY_FILE); Index++) {
	Status = AddSecureBootKeyVariable(
			   gSecureKeyTable[Index].KeyName->VarName,
			   gSecureKeyTable[Index].KeyName->VarGuid,
			   gSecureKeyTable[Index].Data,
			   gSecureKeyTable[Index].DataSize,
			   gSecureKeyTable[Index].SignatureOwner
			   );
	if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "AddSecureBootKeyVariable [%d]-%r\n", Index, Status));
      break;
	}
  }
  SetCustomMode(STANDARD_SECURE_BOOT_MODE);
  LibAuthVarSetPhysicalPresent(FALSE);
  
  FreeLoadedKeys();  
  //DeleteNoNeededNVData();

  return Status;
}

EFI_STATUS
EFIAPI
PlatformSecureLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS    Status;
  UINT32        *Data32;
  
  gSecurePPFlag = (UINT32*)(UINTN)PcdGet64(PcdSecurePPMemAddress);
  if(gSecurePPFlag == NULL){  
    Status = gBS->AllocatePool(
                    EfiReservedMemoryType,
                    sizeof(UINT32),
                    (VOID**)&Data32
                    );
    ASSERT(!EFI_ERROR(Status));
    PcdSet64(PcdSecurePPMemAddress, (UINTN)Data32);
    gSecurePPFlag = Data32;
    *gSecurePPFlag = 0;
  }
  DEBUG((EFI_D_INFO, "gSecurePP:%X\n", gSecurePPFlag));
  
  return EFI_SUCCESS;
}



