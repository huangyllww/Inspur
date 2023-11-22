/** @file
==========================================================================================
      NOTICE: Copyright (c) 2006 - 2018 Byosoft Corporation. All rights reserved.
              This program and associated documentation (if any) is furnished
              under a license. Except as permitted by such license,no part of this
              program or documentation may be reproduced, stored divulged or used
              in a public system, or transmitted in any form or by any means
              without the express written consent of Byosoft Corporation.
==========================================================================================

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MultiPlatSupportLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Ppi/MemoryDiscovered.h>
#include <Guid/VariableFormat.h>
#include <SystemPasswordVariable.h>
#include <SysMiscCfg.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/PeiServicesLib.h>
#include <Uefi.h>
#include <DhcpGetNetworldCfg.h>


#pragma pack(1)

typedef struct {
  UINT16 DefaultId;
  UINT8  BoardId;
} DEFAULT_INFO;

typedef struct {
  //
  // HeaderSize includes HeaderSize fields and DefaultInfo arrays
  //
  UINT16 HeaderSize;
  //
  // DefaultInfo arrays those have the same default setting.
  //
  DEFAULT_INFO DefaultInfo[1];
  //
  // Default data is stored as variable storage.
  // VARIABLE_STORE_HEADER  VarStorageHeader;
  //
} DEFAULT_DATA;

#pragma pack()



#define DEFAULT_DATA_FILE_GUID \
  { 0x1ae42876, 0x008f, 0x4161, { 0xb2, 0xb7, 0x1c, 0xd, 0x15, 0xc5, 0xef, 0x43 }}

EFI_GUID gDefaultDataFileGuid = DEFAULT_DATA_FILE_GUID;


CONST EFI_PEI_PPI_DESCRIPTOR mSetupDefaultSettingPatchPpiList = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gSetupDefaultSettingPatchPpiGuid,
  NULL
};


STATIC
UINTN
GetVariableHeaderSize (
  IN  BOOLEAN       IsAuth
  )
{
  UINTN Value;

  if (IsAuth) {
    Value = sizeof (AUTHENTICATED_VARIABLE_HEADER);
  } else {
    Value = sizeof (VARIABLE_HEADER);
  }

  return Value;
}


STATIC
CHAR16 *
GetVariableNamePtr (
  IN VARIABLE_HEADER    *Variable,
  IN BOOLEAN            IsAuth
  )
{
  return (CHAR16*)((UINTN)Variable + GetVariableHeaderSize(IsAuth));
}

STATIC
EFI_GUID *
GetVendorGuidPtr (
  IN VARIABLE_HEADER    *Variable,
  IN BOOLEAN            IsAuth
  )
{
  AUTHENTICATED_VARIABLE_HEADER *AuthVariable;

  AuthVariable = (AUTHENTICATED_VARIABLE_HEADER *) Variable;
  if (IsAuth) {
    return &AuthVariable->VendorGuid;
  } else {
    return &Variable->VendorGuid;
  }
}


STATIC
UINTN
NameSizeOfVariable (
  IN  VARIABLE_HEADER   *Variable,
  IN  BOOLEAN           IsAuth
  )
{
  AUTHENTICATED_VARIABLE_HEADER *AuthVariable;

  AuthVariable = (AUTHENTICATED_VARIABLE_HEADER *) Variable;
  if (IsAuth) {
    if (AuthVariable->State == (UINT8) (-1) ||
       AuthVariable->DataSize == (UINT32) (-1) ||
       AuthVariable->NameSize == (UINT32) (-1) ||
       AuthVariable->Attributes == (UINT32) (-1)) {
      return 0;
    }
    return (UINTN) AuthVariable->NameSize;
  } else {
    if (Variable->State == (UINT8) (-1) ||
       Variable->DataSize == (UINT32) (-1) ||
       Variable->NameSize == (UINT32) (-1) ||
       Variable->Attributes == (UINT32) (-1)) {
      return 0;
    }
    return (UINTN) Variable->NameSize;
  }
}


STATIC
UINTN
DataSizeOfVariable (
  IN  VARIABLE_HEADER   *Variable,
  IN  BOOLEAN           IsAuth
  )
{
  AUTHENTICATED_VARIABLE_HEADER *AuthVariable;

  AuthVariable = (AUTHENTICATED_VARIABLE_HEADER *) Variable;
  if (IsAuth) {
    if (AuthVariable->State == (UINT8) (-1) ||
       AuthVariable->DataSize == (UINT32) (-1) ||
       AuthVariable->NameSize == (UINT32) (-1) ||
       AuthVariable->Attributes == (UINT32) (-1)) {
      return 0;
    }
    return (UINTN) AuthVariable->DataSize;
  } else {
    if (Variable->State == (UINT8) (-1) ||
       Variable->DataSize == (UINT32) (-1) ||
       Variable->NameSize == (UINT32) (-1) ||
       Variable->Attributes == (UINT32) (-1)) {
      return 0;
    }
    return (UINTN) Variable->DataSize;
  }
}



STATIC
UINT8 *
GetVariableDataPtr (
  IN  VARIABLE_HEADER   *Variable,
  IN  BOOLEAN           IsAuth
  )
{
  UINTN Value;
  UINTN Size;

  Value = (UINTN) GetVariableNamePtr (Variable, IsAuth);
  Size  = NameSizeOfVariable(Variable, IsAuth);
  Value += Size;
  Value += GET_PAD_SIZE(Size);

  return (UINT8*)Value;
}


STATIC
VARIABLE_HEADER *
GetNextVariablePtr (
  IN  VARIABLE_HEADER       *Variable,
  IN  BOOLEAN               IsAuth
  )
{
  UINTN                 Value;
  UINTN                 Size;

  Value = (UINTN) GetVariableDataPtr(Variable, IsAuth);
  Size  = DataSizeOfVariable(Variable, IsAuth);
  Value += Size;
  Value += GET_PAD_SIZE (Size);
  Value = HEADER_ALIGN (Value);

  return (VARIABLE_HEADER *) Value;
}





typedef struct {
  EFI_GUID  *Guid;
  CHAR16    *Name;
} VAR_NAME_GUID;


// 6339D487-26BA-424B-9A5D-687E25D740BC TCG2_CONFIGURATION

STATIC VAR_NAME_GUID gIgnoreVariableList[] = {
 
#if FixedPcdGetBool(PcdKeepSysPasswordWhenLoadFCE)
  {&gEfiSystemPasswordVariableGuid, SYSTEM_PASSWORD_NAME},
#endif
  {&gByoPasswordNetworkAuthConfigGuid, PASSWORD_NETWORK_AUTH_VAR_NAME},
  {NULL,NULL},
};

#define IGNORE_VAR_LIST_COUNT    (sizeof(gIgnoreVariableList)/sizeof(gIgnoreVariableList[0]))



STATIC BOOLEAN IsVarIgnore(EFI_GUID *Guid, CHAR16 *Name)
{
  UINTN     Index;
  
  for (Index = 0; Index < IGNORE_VAR_LIST_COUNT; Index ++) {
    if (gIgnoreVariableList[Index].Guid == NULL || gIgnoreVariableList[Index].Name == NULL) {
      break;
    }
    if (CompareGuid(Guid, gIgnoreVariableList[Index].Guid) && 
        StrCmp(gIgnoreVariableList[Index].Name, Name) == 0) {
      return TRUE;
    }
  }
  return FALSE;
}


STATIC
VOID
UpdateVariableInHob (
  IN   VARIABLE_STORE_HEADER      *VarStoreHeader,
  OUT  VOID                       **SetupData,
  IN   BOOLEAN                    VariableIsExist    
  )
{
  BOOLEAN                       IsAuth = FALSE;
  VARIABLE_HEADER               *VarHdr;
  VARIABLE_HEADER               *VarEnd;
  EFI_GUID                      *Guid;
  CHAR16                        *Name;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if(CompareGuid(&VarStoreHeader->Signature, &gEfiAuthenticatedVariableGuid)){
    //
    // The generated default variable storage must be normal variable storage
    // When meet with this issue, please remove Edk2\BaseTools\Bin\Win32, and try again
    //
    ASSERT (FALSE);
    IsAuth = TRUE;
  }

  VarHdr = (VARIABLE_HEADER*)HEADER_ALIGN(VarStoreHeader + 1);
  VarEnd = (VARIABLE_HEADER*)HEADER_ALIGN((UINTN) VarStoreHeader + VarStoreHeader->Size);

  *SetupData = NULL;

  while ((VarHdr < VarEnd) && VarHdr->StartId == VARIABLE_DATA) {
    Guid = GetVendorGuidPtr(VarHdr, IsAuth);
    Name = GetVariableNamePtr(VarHdr, IsAuth);
    DEBUG((EFI_D_INFO, "%g %s\n", Guid, Name));
    if(VariableIsExist && IsVarIgnore(Guid, Name)) {
      VarHdr->State = VAR_DELETED;
      DEBUG((EFI_D_INFO, "Ignore\n"));
    } else if(StrCmp(L"Setup", Name) == 0) {
      *SetupData = (VOID*)GetVariableDataPtr(VarHdr, IsAuth);
      DEBUG((EFI_D_INFO, "Setup\n"));
    }
    VarHdr = GetNextVariablePtr(VarHdr, IsAuth);
  }

}


EFI_STATUS
EFIAPI
CreateDefaultVariableHob (
  IN  UINT16    DefaultId,
  IN  UINT8     BoardId,
  OUT VOID      **pSetupData     OPTIONAL
  )
/*++
Description:

  This function finds the matched default data and create GUID hob for it. 

Arguments:

  DefaultId - Specifies the type of defaults to retrieve.
  BoardId   - Specifies the platform board of defaults to retrieve.
  
Returns:

  EFI_SUCCESS - The matched default data is found.
  EFI_NOT_FOUND - The matched default data is not found.
  EFI_OUT_OF_RESOURCES - No enough resource to create HOB.

--*/
{
  UINTN                      FvInstance;
  EFI_FIRMWARE_VOLUME_HEADER *FvHeader;
  EFI_FFS_FILE_HEADER        *FfsHeader;
  UINT32                     FileSize;
  EFI_COMMON_SECTION_HEADER  *Section;
  UINT32                     SectionLength;
  BOOLEAN                    DefaultFileIsFound;
  DEFAULT_DATA               *DefaultData;
  DEFAULT_INFO               *DefaultInfo;
  VARIABLE_STORE_HEADER      *VarStoreHeader;
  VARIABLE_STORE_HEADER      *VarStoreHeaderHob;
  UINT8                      *VarHobPtr;
  UINT8                      *VarPtr;
  UINT32                     VarDataOffset;
  UINT32                     VarHobDataOffset;
  CONST EFI_PEI_SERVICES     **PeiServices;
  VOID                       *SetupData;
  EFI_STATUS                 Status;
  UINTN                      Size;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariablePpi;
  BOOLEAN                    VariableIsExist = TRUE;
	
  //
  // Get PeiService pointer
  //
  PeiServices = GetPeiServicesTablePointer ();

  //
  // Find the FFS file that stores all default data.
  //
  DefaultFileIsFound = FALSE;
  FvInstance         = 0;
  FfsHeader          = NULL;
  while (((*PeiServices)->FfsFindNextVolume (PeiServices, FvInstance, (VOID **)&FvHeader) == EFI_SUCCESS) &&
         (!DefaultFileIsFound)) {
    FfsHeader = NULL;
    while ((*PeiServices)->FfsFindNextFile (PeiServices, EFI_FV_FILETYPE_FREEFORM, FvHeader, (VOID **)&FfsHeader) == EFI_SUCCESS) {
      if (CompareGuid ((EFI_GUID *) FfsHeader, &gDefaultDataFileGuid)) {
        DefaultFileIsFound = TRUE;
        break;
      }
    }
    FvInstance ++;
  }
  
  //
  // FFS file is not found.
  //
  if (!DefaultFileIsFound) {
    return EFI_NOT_FOUND;
  }

  Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &VariablePpi);
  ASSERT_EFI_ERROR (Status);

  Size = 0;
  Status = VariablePpi->GetVariable (
                          VariablePpi,
                          SYSTEM_PASSWORD_NAME,
                          &gEfiSystemPasswordVariableGuid,
                          NULL,
                          &Size,
                          NULL
                          );
  if (Status == EFI_NOT_FOUND) {
    VariableIsExist = FALSE;
  }
  
  //
  // Find the matched default data for the input default ID. plat ID is ignored as temporary solution
  // as we assume there is only for a single platform. The purpose is to reduce the size of default data file.
  // 
  VarStoreHeader = NULL;
  Section  = (EFI_COMMON_SECTION_HEADER *)(FfsHeader + 1);
  FileSize = *(UINT32 *)(FfsHeader->Size) & 0x00FFFFFF;
  while (((UINTN) Section < (UINTN) FfsHeader + FileSize) && (VarStoreHeader == NULL)) {
    DefaultData = (DEFAULT_DATA *) (Section + 1);
    DefaultInfo = &(DefaultData->DefaultInfo[0]);
    while ((UINTN) DefaultInfo < (UINTN) DefaultData + DefaultData->HeaderSize) {
      if (DefaultInfo->DefaultId == DefaultId) {
        VarStoreHeader = (VARIABLE_STORE_HEADER *) ((UINT8 *) DefaultData + DefaultData->HeaderSize);
        break;
      }
      DefaultInfo ++;
    }
    //
    // Size is 24 bits wide so mask upper 8 bits. 
    // SectionLength is adjusted it is 4 byte aligned.
    // Go to the next section
    //
    SectionLength = *(UINT32 *)Section->Size & 0x00FFFFFF;
    SectionLength = (SectionLength + 3) & (~3);
    ASSERT (SectionLength != 0);
    Section = (EFI_COMMON_SECTION_HEADER *)((UINT8 *)Section + SectionLength);
  }
  //
  // Matched default data is not found.
  //
  if (VarStoreHeader == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Create HOB to store default data so that Variable driver can use it.
  // Allocate more data for header alignment. 
  //
  VarStoreHeaderHob = (VARIABLE_STORE_HEADER *) BuildGuidHob (&VarStoreHeader->Signature, VarStoreHeader->Size + HEADER_ALIGNMENT - 1);
  if (VarStoreHeaderHob == NULL) {
    //
    // No enough hob resource.
    //
    return EFI_OUT_OF_RESOURCES;
  }
  
  //
  // Copy variable storage header.
  //
  CopyMem (VarStoreHeaderHob, VarStoreHeader, sizeof (VARIABLE_STORE_HEADER));
  //
  // Copy variable data.
  //
  VarPtr           = (UINT8 *) HEADER_ALIGN ((UINTN) (VarStoreHeader + 1));
  VarDataOffset    = (UINT32) ((UINTN) VarPtr - (UINTN) VarStoreHeader);
  VarHobPtr        = (UINT8 *) HEADER_ALIGN ((UINTN) (VarStoreHeaderHob + 1));
  VarHobDataOffset = (UINT32) ((UINTN) VarHobPtr - (UINTN) VarStoreHeaderHob);
  CopyMem (VarHobPtr, VarPtr, VarStoreHeader->Size - VarDataOffset);
  //
  // Update variable size.
  //
  VarStoreHeaderHob->Size = VarStoreHeader->Size - VarDataOffset + VarHobDataOffset;

  //
  // Patch Setup Default Setting
  //
  Status = PeiServicesInstallPpi(&mSetupDefaultSettingPatchPpiList);
  ASSERT_EFI_ERROR(Status);

  UpdateVariableInHob(VarStoreHeaderHob, &SetupData, VariableIsExist);

  if(pSetupData != NULL){
    *pSetupData = SetupData;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PlatformDefaultEndOfPei (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_HOB_GUID_TYPE      *GuidHob;
  EFI_BOOT_MODE          BootMode;
  EFI_STATUS             Status;

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  if (BootMode == BOOT_ON_S3_RESUME) {
    //
    // Do nothing on S3 resume
    //
    return EFI_SUCCESS;
  }

  GuidHob = GetFirstGuidHob (&gEfiVariableGuid);
  if (GuidHob == NULL) {
    GuidHob = GetFirstGuidHob (&gVariableDefaultGuid);
    if (GuidHob == NULL) {
      //
      // This HOB is for HiiDataBase module to retrieve the default setting for setup page
      //
      CreateDefaultVariableHob(EFI_HII_DEFAULT_CLASS_STANDARD, 0, NULL);
      GuidHob = GetFirstGuidHob (&gEfiVariableGuid);
      if (GuidHob != NULL) {
        CopyGuid (&GuidHob->Name, &gVariableDefaultGuid);
      }
    }
  }
  return EFI_SUCCESS;
}

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mPlatformDefaultEndOfPeiNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  PlatformDefaultEndOfPei
};

EFI_STATUS
EFIAPI
PeiMultiPlatSupportLibConstructor (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  PeiServicesNotifyPpi (&mPlatformDefaultEndOfPeiNotifyList);

  return EFI_SUCCESS;
}
