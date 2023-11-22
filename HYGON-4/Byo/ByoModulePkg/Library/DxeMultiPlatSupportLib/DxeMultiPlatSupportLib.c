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

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MultiPlatSupportLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Guid/VariableFormat.h>
#include <SystemPasswordVariable.h>
#include <Library/DxeServicesLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

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

STATIC
CHAR16 *
GetVariableNamePtr (
  IN VARIABLE_HEADER    *Variable
  )
{
  return (CHAR16*)((UINTN)Variable + sizeof (VARIABLE_HEADER));
}

STATIC
EFI_GUID *
GetVendorGuidPtr (
  IN VARIABLE_HEADER    *Variable
  )
{
  return &Variable->VendorGuid;
}


STATIC
UINTN
NameSizeOfVariable (
  IN  VARIABLE_HEADER   *Variable
  )
{

  if (Variable->State == (UINT8) (-1) ||
      Variable->DataSize == (UINT32) (-1) ||
      Variable->NameSize == (UINT32) (-1) ||
      Variable->Attributes == (UINT32) (-1)) {
    return 0;
  }
  return (UINTN) Variable->NameSize;
}


STATIC
UINTN
DataSizeOfVariable (
  IN  VARIABLE_HEADER   *Variable
  )
{
  if (Variable->State == (UINT8) (-1) ||
      Variable->DataSize == (UINT32) (-1) ||
      Variable->NameSize == (UINT32) (-1) ||
      Variable->Attributes == (UINT32) (-1)) {
    return 0;
  }
  return (UINTN) Variable->DataSize;
}



STATIC
UINT8 *
GetVariableDataPtr (
  IN  VARIABLE_HEADER   *Variable
  )
{
  UINTN Value;
  UINTN Size;

  Value = (UINTN) GetVariableNamePtr (Variable);
  Size  = NameSizeOfVariable(Variable);
  Value += Size;
  Value += GET_PAD_SIZE(Size);

  return (UINT8*)Value;
}


STATIC
VARIABLE_HEADER *
GetNextVariablePtr (
  IN  VARIABLE_HEADER       *Variable
  )
{
  UINTN                 Value;
  UINTN                 Size;

  Value = (UINTN) GetVariableDataPtr(Variable);
  Size  = DataSizeOfVariable(Variable);
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
  VARIABLE_HEADER               *VarHdr;
  VARIABLE_HEADER               *VarEnd;
  EFI_GUID                      *Guid;
  CHAR16                        *Name;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if(CompareGuid(&VarStoreHeader->Signature, &gEfiAuthenticatedVariableGuid)){
    //
    // Variable storage must be normal variable storage
    //
    ASSERT (FALSE);
  }

  VarHdr = (VARIABLE_HEADER*)HEADER_ALIGN(VarStoreHeader + 1);
  VarEnd = (VARIABLE_HEADER*)HEADER_ALIGN((UINTN) VarStoreHeader + VarStoreHeader->Size);

  *SetupData = NULL;

  while ((VarHdr < VarEnd) && VarHdr->StartId == VARIABLE_DATA) {
    Guid = GetVendorGuidPtr(VarHdr);
    Name = GetVariableNamePtr(VarHdr);
    DEBUG((EFI_D_INFO, "%g %s\n", Guid, Name));
    if(VariableIsExist && IsVarIgnore(Guid, Name)) {
      VarHdr->State = VAR_DELETED;
      DEBUG((EFI_D_INFO, "Ignore\n"));
    } else if(StrCmp(L"Setup", Name) == 0) {
      *SetupData = (VOID*)GetVariableDataPtr(VarHdr);
      DEBUG((EFI_D_INFO, "Setup\n"));
    }
    VarHdr = GetNextVariablePtr(VarHdr);
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
  UINT8                      *SectionData;
  UINTN                      SectionLength;
  UINTN                      Index;
  DEFAULT_DATA               *DefaultData;
  DEFAULT_INFO               *DefaultInfo;
  VARIABLE_STORE_HEADER      *VarStoreHeader;
  VARIABLE_STORE_HEADER      *VarStoreHeaderHob;
  UINT8                      *VarHobPtr;
  UINT8                      *VarPtr;
  UINT32                     VarDataOffset;
  UINT32                     VarHobDataOffset;
  VOID                       *SetupData;
  EFI_STATUS                 Status;
  UINTN                      Size;
  BOOLEAN                    VariableIsExist = TRUE;
  EFI_HOB_GUID_TYPE          *GuidHob;

  Size = 0;
  Status = gRT->GetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &Size,
                  NULL
                  );
  //
  // Variable service must be ready now.
  //
  ASSERT (Status != EFI_NOT_AVAILABLE_YET);
  if (Status == EFI_NOT_FOUND) {
    VariableIsExist = FALSE;
  }

  //
  // Find the matched default data for the input default ID. plat ID is ignored as temporary solution
  // as we assume there is only for a single platform. The purpose is to reduce the size of default data file.
  // 
  VarStoreHeader = NULL;

  Index = 0; 
  while (VarStoreHeader == NULL) {
    Status = GetSectionFromAnyFv (
              &gDefaultDataFileGuid,
              EFI_SECTION_RAW,
              Index,
              (VOID **)&SectionData,
              &SectionLength
              );
    if (EFI_ERROR (Status)) {
      break;
    }

    DefaultData = (DEFAULT_DATA *) SectionData;
    DefaultInfo = &(DefaultData->DefaultInfo[0]);
    while ((UINTN) DefaultInfo < (UINTN) DefaultData + DefaultData->HeaderSize) {
      if (DefaultInfo->DefaultId == DefaultId) {
        VarStoreHeader = (VARIABLE_STORE_HEADER *) ((UINT8 *) DefaultData + DefaultData->HeaderSize);
        break;
      }
      DefaultInfo ++;
    }

    Index ++;
  }

  //
  // Matched default data is not found.
  //
  if (VarStoreHeader == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Use pre allocated HOB to store default data so that Variable driver can use it.
  //
  GuidHob = GetFirstGuidHob (&gPreVarialeDefaultDataGuid);
  ASSERT (GuidHob != NULL);
  ASSERT (GET_HOB_LENGTH (GuidHob) >= (VarStoreHeader->Size + HEADER_ALIGNMENT - 1 + sizeof (EFI_HOB_GUID_TYPE)));

  //
  // Copy variable storage guid
  //
  CopyGuid (&GuidHob->Name, &VarStoreHeader->Signature);
  VarStoreHeaderHob = (VARIABLE_STORE_HEADER *) GET_GUID_HOB_DATA (GuidHob);

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

  UpdateVariableInHob(VarStoreHeaderHob, &SetupData, VariableIsExist);

  if(pSetupData != NULL){
    *pSetupData = SetupData;
  }

  return EFI_SUCCESS;
}

