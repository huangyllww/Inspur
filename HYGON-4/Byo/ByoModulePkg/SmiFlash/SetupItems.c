/** @file

Copyright (c) 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SetupItems

Abstract:
  .

Revision History:

TIME:       2019-12-19
$AUTHOR:    Fuming
$REVIEWERS:
$SCOPE:     All Byosoft IA32/X64 Platforms
$TECHNICAL:


T.O.D.O

$END--------------------------------------------------------------------------------------------------------------------

--*/

#include "SmiFlash.h"
#include <Library/HobLib.h>
#include <Guid/SmmVariableCommon.h>

SETUP_ITEM_INFO                 *mSetupItem = NULL;
SETUP_VARIABLE_TABLE            *mSetupVariableTable = NULL;
VOID                            **mSetupVariableData = NULL;
UINT32                          mSetupVariableNumber = 0;
UINT32                          mSetupItemNumber = 0;
EFI_SMM_VARIABLE_PROTOCOL       *mSmmVariable = NULL;
CHAR8                           *mSetupName = NULL;
UINT32                          mSetupNameSize = 0;
CHAR8                           *mSetupOptions = NULL;
UINT32                          mSetupOptionsSize = 0;
OPTIONS_DATA                    **mOptionsSplit = NULL;
CHAR8                           *mSetupItemString = NULL;
UINT32                          mSetupItemStringSize = 0;
CHAR8                           *mSetupItemOneOfOptionString = NULL;
UINT32                          mSetupItemOneOfOptionStringSize = 0;
UINT32                          mOrderedListNum = 0;


//Split the mSetupOptions got in LocatSetupItemsDb
EFI_STATUS
OptionSplit()  
{
  UINT32 IndexI;
  mOptionsSplit = AllocatePool(mSetupItemNumber*sizeof(VOID *));
  if (mOptionsSplit == NULL){
    Print(L"AllocatePool failed\n");
    return EFI_OUT_OF_RESOURCES;
  }
  OPTIONS_DATA        *OptionsTemp = (OPTIONS_DATA*)mSetupOptions;

  for ( IndexI = 0; IndexI < mSetupItemNumber; IndexI++){
    switch (OptionsTemp->DataType){
      case DATA_TYPE_UINT8:
        mOptionsSplit[IndexI] = OptionsTemp;
        OptionsTemp = (OPTIONS_DATA*) ( (UINT8*)OptionsTemp + 3 + (OptionsTemp->Count)*1);
        break;
      case DATA_TYPE_UINT16:
        mOptionsSplit[IndexI] = OptionsTemp;
        OptionsTemp = (OPTIONS_DATA*) ( (UINT8*)OptionsTemp + 3 + (OptionsTemp->Count)*2 );
        break;
      case DATA_TYPE_UINT32:
        mOptionsSplit[IndexI] = OptionsTemp;
        OptionsTemp = (OPTIONS_DATA*) ( (UINT8*)OptionsTemp + 3 + (OptionsTemp->Count)*4);
        break;
      case DATA_TYPE_UINT64:
        mOptionsSplit[IndexI] = OptionsTemp;
        OptionsTemp = (OPTIONS_DATA*) ( (UINT8*)OptionsTemp + 3 + (OptionsTemp->Count)*8);
        break;
      default:
        break;
    }
  }
  return EFI_SUCCESS;
}


VOID
LocatSetupItemsDb (
  )
{
  VOID  *Buffer;
  VOID  *SmmBuffer;
  UINTN BufferSize;
  SETUP_DATABASE_LAYOUT *SetupDB;
  EFI_STATUS Status;
  UINT32 Index;

  //
  // Get Setup Item DataBase
  //
  Buffer = NULL;
  Status = GetSectionFromAnyFv (
            &gByoSetupItemsDBGuid, 
            EFI_SECTION_RAW, 
            0, 
            &Buffer,
            &BufferSize
          );

  if (Status == EFI_SUCCESS) {
    ASSERT(BufferSize >= sizeof(SETUP_DATABASE_LAYOUT));
    //
    // Copy Data from Boot Service Memory to SMM Memory
    //
    SmmBuffer = AllocatePool (BufferSize);
    if (SmmBuffer == NULL) {
      goto Done;
    }
    CopyMem (SmmBuffer, Buffer, BufferSize);
    SetupDB = (SETUP_DATABASE_LAYOUT *) SmmBuffer;
    mSetupVariableNumber = SetupDB->SetupVariableNumber;
    mSetupItemNumber = SetupDB->SetupItemNumber;
    mSetupVariableTable  = SetupDB->VariableTable;
    mSetupVariableData  = AllocatePool (mSetupVariableNumber * sizeof (VOID*));
    if (mSetupVariableData == NULL) {
      goto Done;
    }
    for (Index = 0; Index < mSetupVariableNumber; Index ++) {
      mSetupVariableData[Index] = AllocatePool (mSetupVariableTable[Index].VariableLen);
      if (mSetupVariableData[Index] == NULL) {
        goto Done;
      }
      DEBUG((DEBUG_INFO, "Allocate Address_%d: %p\n", Index, mSetupVariableData[Index]));
    }
    mSetupItem           = (SETUP_ITEM_INFO *) (mSetupVariableTable + mSetupVariableNumber);

  }
  //
  // Get Setup Items' Name Field
  //
  if (Buffer != NULL) {
    gBS->FreePool (Buffer);
    Buffer = NULL;
  }
  Status = GetSectionFromAnyFv (
            &gByoSetupItemsDBGuid, 
            EFI_SECTION_RAW, 
            1, 
            &Buffer,
            &BufferSize
          );

  if (Status == EFI_SUCCESS) {
    ASSERT(BufferSize > 0);
    //
    // Copy Data from Boot Service Memory to SMM Memory
    //
    SmmBuffer = AllocatePool (BufferSize);
    if (SmmBuffer == NULL) {
      goto Done;
    }
    CopyMem (SmmBuffer, Buffer, BufferSize);
    mSetupName           = (CHAR8 *)SmmBuffer;
    mSetupNameSize       = (UINT32)BufferSize;
  }
  //
  // Get Setup Items' option information
  //
  if (Buffer != NULL) {
    gBS->FreePool (Buffer);
    Buffer = NULL;
  }
  Status = GetSectionFromAnyFv (
            &gByoSetupItemsDBGuid, 
            EFI_SECTION_RAW, 
            2, 
            &Buffer,
            &BufferSize
          );

  if (Status == EFI_SUCCESS) {
    ASSERT(BufferSize > sizeof(OPTIONS_DATA));
    //
    // Copy Data from Boot Service Memory to SMM Memory
    //
    SmmBuffer = AllocatePool (BufferSize);
    if (SmmBuffer == NULL) {
      goto Done;
    }
    CopyMem (SmmBuffer, Buffer, BufferSize);
    mSetupOptions           = (CHAR8 *)SmmBuffer;
    mSetupOptionsSize       = (UINT32)BufferSize;
  }
  if (Buffer != NULL) {
    gBS->FreePool (Buffer);
    Buffer = NULL;
  }
  Status = GetSectionFromAnyFv (
            &gByoSetupItemsDBGuid, 
            EFI_SECTION_RAW, 
            3, 
            &Buffer,
            &BufferSize
          );

  if (Status == EFI_SUCCESS) {
    //
    // Copy Data from Boot Service Memory to SMM Memory
    //
    SmmBuffer = AllocatePool (BufferSize);
    if (SmmBuffer == NULL) {
      goto Done;
    }
    CopyMem (SmmBuffer, Buffer, BufferSize);
    mSetupItemString           = (CHAR8 *)SmmBuffer;
    mSetupItemStringSize       = (UINT32)BufferSize;
  }

  //
  // Get Setup Items' one of option string
  //
  if (Buffer != NULL) {
    gBS->FreePool (Buffer);
    Buffer = NULL;
  }
  Status = GetSectionFromAnyFv (
             &gByoSetupItemsDBGuid, 
             EFI_SECTION_RAW, 
             4, 
             &Buffer,
             &BufferSize
             );
  if (Status == EFI_SUCCESS) {
    //
    // Copy Data from Boot Service Memory to SMM Memory
    //
    SmmBuffer = AllocatePool (BufferSize);
    if (SmmBuffer == NULL) {
      goto Done;
    }
    CopyMem (SmmBuffer, Buffer, BufferSize);
    mSetupItemOneOfOptionString     = (CHAR8 *)SmmBuffer;
    mSetupItemOneOfOptionStringSize = (UINT32)BufferSize;
  } 

  Status = OptionSplit();

Done:
  if (Buffer != NULL) {
    gBS->FreePool (Buffer);
    return;
  }

  return;
}

VOID
GetSingleOptionLength(
  UINT8  DataType,
  UINT32 *SingleOptionLength
)
{
  switch (DataType){
    case DATA_TYPE_UINT8:
      *SingleOptionLength = 1;
      break;
    case DATA_TYPE_UINT16:
      *SingleOptionLength = 2;
      break;
    case DATA_TYPE_UINT32:
      *SingleOptionLength = 4;
      break;
    case DATA_TYPE_UINT64:
      *SingleOptionLength = 8;
      break;
    default:
      *SingleOptionLength = 0;
      break;
  }
}



EFI_STATUS
SetValue (
  UINT32                          ItemNameHash,
  UINT64                          ModifyValue,
  BOOLEAN                         IsCustomDefault
)
{
  VOID                            *PrefixAddr;
  UINT32                          Attributes;
  UINT32                          Index;
  UINT32                          VariableIndex;
  EFI_STATUS                      Status;
  CHAR16                          *PrefixValueName;
  UINTN                           VariableLength;
  UINT32                          SingleOptionLength;

  Status = EFI_UNSUPPORTED;
  PrefixAddr = NULL;
  PrefixValueName = NULL;
  Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS;
  
  if (mSetupItem == NULL) {
    DEBUG ((DEBUG_INFO, "%a() L%d, Not Found SetupItemDbInCurrentBIOS\n", __FUNCTION__, __LINE__));
    return Status;
  }

  for (Index = 0; mSetupItem[Index].VariableIndex != MAX_UINT32; Index++) {
    if (mSetupItem[Index].ItemNameHash == 0) {
      continue;
    }

    if (mSetupItem[Index].ItemNameHash == ItemNameHash) {
      VariableIndex = mSetupItem[Index].VariableIndex;
      
      //
      // Variable is not in the stored table.
      // Variable attribute is not NV + BS + RT.
      // Then, this variable is invalid
      //
      if ((VariableIndex >= mSetupVariableNumber) || (mSetupVariableTable[VariableIndex].Attribute != Attributes)) {
        return EFI_INVALID_PARAMETER;
      }
      VariableLength = (UINTN) mSetupVariableTable[VariableIndex].VariableLen;
      Status = mSmmVariable->SmmGetVariable (
                      mSetupVariableTable[VariableIndex].VariableName,
                      &(mSetupVariableTable[VariableIndex].VariableGuid),
                      &Attributes,
                      &VariableLength,
                      mSetupVariableData[VariableIndex]
                      );

      if (EFI_ERROR(Status)) {
        return EFI_INVALID_PARAMETER;
      }

      Status = gSmst->SmmAllocatePool (
                  EfiRuntimeServicesData,
                  0x200,
                  (VOID **)&PrefixValueName
                  );
      SetMem (PrefixValueName, 0x200, 0);
      if (IsCustomDefault == TRUE) {
        Status = gSmst->SmmAllocatePool (
                EfiRuntimeServicesData,
                VariableLength,
                (VOID **)&PrefixAddr
                );
        SetMem (PrefixAddr, VariableLength, 0);
        Status = StrCatS (PrefixValueName, 0x200/sizeof(CHAR16), L"Prefix");
        if (EFI_ERROR (Status)){
          DEBUG ((EFI_D_INFO, "%a Line %d StrCatS Failed. \n ", __FUNCTION__, __LINE__));
        }
        Status = StrCatS (PrefixValueName, 0x200/sizeof(CHAR16), mSetupVariableTable[VariableIndex].VariableName); 	
        if (EFI_ERROR (Status)){
          DEBUG ((EFI_D_INFO, "%a Line %d StrCatS Failed. \n ", __FUNCTION__, __LINE__));
        }
        Status = mSmmVariable->SmmGetVariable (
                        PrefixValueName,
                        &(mSetupVariableTable[VariableIndex].VariableGuid),
                        &Attributes,
                        &VariableLength,
                        (VOID *)PrefixAddr
                        );
        if (!EFI_ERROR(Status)) {
          CopyMem (mSetupVariableData[VariableIndex], PrefixAddr, VariableLength);
          if (EFI_ERROR (Status)){
            DEBUG ((EFI_D_INFO, "%a Line %d MemCpyS Failed. \n ", __FUNCTION__, __LINE__));
          }
        }
      } else {
        Status = StrCatS (PrefixValueName, 0x200/sizeof(CHAR16), mSetupVariableTable[VariableIndex].VariableName);	
        if (EFI_ERROR (Status)){
          DEBUG ((EFI_D_INFO, "%a Line %d StrCatS Failed. \n ", __FUNCTION__, __LINE__));
        }
      }
      if (mOrderedListNum > (UINT8)(mOptionsSplit[Index-1]->Count - 2)){
        mOrderedListNum = 0;
        return EFI_INVALID_PARAMETER;
      }
      
      if (mOptionsSplit[Index-1]->OpCode == EFI_IFR_ORDERED_LIST_OP){
        GetSingleOptionLength(mOptionsSplit[Index-1]->DataType,&SingleOptionLength);
        if (SingleOptionLength == 0){
          return EFI_INVALID_PARAMETER;
        }
        CopyMem ( (VOID*)((UINT8*)mSetupVariableData[VariableIndex] + mSetupItem[Index].ItemOffset + mOrderedListNum*SingleOptionLength), 
                  (VOID *)&ModifyValue, 
                  (UINTN)SingleOptionLength);
        if (mOrderedListNum == (UINT8)(mOptionsSplit[Index-1]->Count - 2)){
          mOrderedListNum = 0;
        } else {
          mOrderedListNum++;
        }
      } else {
        CopyMem ( (VOID*)((UINT8*)mSetupVariableData[VariableIndex] + mSetupItem[Index].ItemOffset), (VOID *)&ModifyValue, (UINTN)mSetupItem[Index].ItemLength);
      }
      Status = mSmmVariable->SmmSetVariable (
                      PrefixValueName, 
                      &(mSetupVariableTable[VariableIndex].VariableGuid), 
                      Attributes, 
                      VariableLength, 
                      mSetupVariableData[VariableIndex]
      );

      if(PrefixValueName != NULL) {
        gSmst->SmmFreePool (PrefixValueName);
      }
      if(PrefixAddr != NULL) {
        gSmst->SmmFreePool (PrefixAddr);
      }
      DEBUG ((DEBUG_INFO, "%a() L%d, Patch setup ItemOffset: %X\n", __FUNCTION__, __LINE__, mSetupItem[Index].ItemOffset));
      DEBUG ((DEBUG_INFO, "%a() L%d, Patch setup ItemLength: %X\n", __FUNCTION__, __LINE__, mSetupItem[Index].ItemLength));
      break;
    }
  }
  return Status;
}

EFI_STATUS
GetValue (
  UINT32                          ItemNameHash,
  UPDATE_VALUE_PARAMETER          *VariableParam
)
{
  UINT32                          Attributes;
  UINT32                          Index;
  UINT32                          VariableIndex;
  EFI_STATUS                      Status;
  UINTN                           VariableLength;
  UINT32                          SingleOptionLength;

  Status = EFI_UNSUPPORTED;
  Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS;
  
  if (mSetupItem == NULL) {
    DEBUG ((DEBUG_ERROR, "%a() L%d, Not Found SetupItemDbInCurrentBIOS\n", __FUNCTION__, __LINE__));
    return Status;
  }
  for (Index = 0; mSetupItem[Index].VariableIndex != MAX_UINT32; Index++) {
    if (mSetupItem[Index].ItemNameHash == 0) {
      continue;
    }
    if (mSetupItem[Index].ItemNameHash == ItemNameHash) {
      VariableIndex = mSetupItem[Index].VariableIndex;

      //
      // Variable is not in the stored table.
      // Variable attribute is not NV + BS + RT.
      // Then, this variable is invalid
      //
      if ((VariableIndex >= mSetupVariableNumber) || (mSetupVariableTable[VariableIndex].Attribute != Attributes)) {
        return EFI_INVALID_PARAMETER;
      }
      VariableLength = (UINTN)mSetupVariableTable[VariableIndex].VariableLen;
      Status = mSmmVariable->SmmGetVariable (
                      mSetupVariableTable[VariableIndex].VariableName,
                      &(mSetupVariableTable[VariableIndex].VariableGuid),
                      &Attributes,
                      &VariableLength,
                      mSetupVariableData[VariableIndex]
                      );
      DEBUG ((DEBUG_INFO, "SmmGetVariable:%r\n",Status));
      if (EFI_ERROR(Status)) {
        return EFI_INVALID_PARAMETER;
      }
      if (mOrderedListNum > (UINT8)(mOptionsSplit[Index-1]->Count - 2)){
        mOrderedListNum = 0;
        return EFI_INVALID_PARAMETER;
      }
      if (mOptionsSplit[Index-1]->OpCode == EFI_IFR_ORDERED_LIST_OP){
        GetSingleOptionLength(mOptionsSplit[Index-1]->DataType,&SingleOptionLength);
        if (SingleOptionLength == 0){
          return EFI_INVALID_PARAMETER;
        }
        CopyMem ( (VOID *)&VariableParam->ModifyValue, 
                  (VOID*)((UINT8*)mSetupVariableData[VariableIndex] + mSetupItem[Index].ItemOffset+ mOrderedListNum*SingleOptionLength), 
                  (UINTN)SingleOptionLength);
        if (mOrderedListNum == (UINT8)(mOptionsSplit[Index-1]->Count - 2)){
          mOrderedListNum = 0;
        } else {
          mOrderedListNum++;
        }
      } else {
        CopyMem ( (VOID *)&VariableParam->ModifyValue, (VOID*)((UINT8*)mSetupVariableData[VariableIndex] + mSetupItem[Index].ItemOffset), (UINTN)mSetupItem[Index].ItemLength);
      }
      DEBUG ((DEBUG_INFO, "%a() L%d, Patch setup ItemOffset: %X\n", __FUNCTION__, __LINE__, mSetupItem[Index].ItemOffset));
      DEBUG ((DEBUG_INFO, "%a() L%d, Patch setup ItemLength: %X\n", __FUNCTION__, __LINE__, mSetupItem[Index].ItemLength));
      break;
    }
  }
  return Status;
}

EFI_STATUS
UniToolGetValueOffset (
    UPDATE_VALUE_PARAMETER *VariableParam
)
{
  UINT32                ItemNameHash;
  UINT8                 *String;

  ItemNameHash = 0;
  String = (UINT8*)VariableParam;
  BKDRHASH(String, ItemNameHash);

  DEBUG ((DEBUG_INFO, "VariableParam->NameString:%a\n", VariableParam->NameString));
  DEBUG ((DEBUG_INFO, "VariableParam->ModifyValue:%lx\n", VariableParam->ModifyValue));
  DEBUG ((DEBUG_INFO, "ItemNameHash:%x\n", ItemNameHash));
  return GetValue(ItemNameHash, VariableParam);
}

EFI_STATUS
UniToolSetValueOffset (
    UPDATE_VALUE_PARAMETER *VariableParam
)
{
  UINT32                ItemNameHash;
  UINT8                 *String;

  ItemNameHash = 0;
  String = (UINT8*)VariableParam;
  BKDRHASH(String, ItemNameHash);

  DEBUG ((DEBUG_INFO, "VariableParam->NameString:%a\n", VariableParam->NameString));
  DEBUG ((DEBUG_INFO, "VariableParam->ModifyValue:%lx\n", VariableParam->ModifyValue));
  DEBUG ((DEBUG_INFO, "ItemNameHash:%x\n", ItemNameHash));
  return SetValue(ItemNameHash, VariableParam->ModifyValue, FALSE);
}

EFI_STATUS
GetSetupItemsName(
  UINT32 Offset, UINT32 Size, UINT8 *Buffer)
{
  EFI_STATUS Status = EFI_UNSUPPORTED;
  if (mSetupName == NULL) {
    DEBUG ((DEBUG_ERROR, "%a() L%d, Not Found SetupItemNameInCurrentBIOS\n", __FUNCTION__, __LINE__));
    return Status;
  }
  if (Offset > mSetupNameSize) {
    DEBUG((DEBUG_ERROR, "Offset larger than Setup Items Name Size\n"));
    return EFI_ABORTED;
  } else if (Offset + Size > mSetupNameSize) {
    CopyMem (Buffer, mSetupName + Offset, mSetupNameSize - Offset);
  } else if (Offset + Size < mSetupNameSize) {
    CopyMem (Buffer, mSetupName + Offset, Size);
  }
  return EFI_SUCCESS;
}

EFI_STATUS
GetSetupItemsOption(
  UINT32 Offset, UINT32 Size, UINT8 *Buffer)
{
  EFI_STATUS Status = EFI_UNSUPPORTED;
  if (mSetupOptions == NULL) {
    DEBUG ((DEBUG_ERROR, "%a() L%d, Not Found Setup Items Option  In Current BIOS\n", __FUNCTION__, __LINE__));
    return Status;
  }
  if (Offset > mSetupOptionsSize) {
    DEBUG((DEBUG_ERROR, "Offset larger than Setup Items Option Size\n"));
    return EFI_ABORTED;
  } else if (Offset + Size > mSetupOptionsSize) {
    CopyMem (Buffer, mSetupOptions + Offset, mSetupOptionsSize - Offset);
  } else if (Offset + Size < mSetupOptionsSize) {
    CopyMem (Buffer, mSetupOptions + Offset, Size);
  }
  return EFI_SUCCESS;
}

EFI_STATUS
GetSetupItemsNameString(
  UINT32 Offset, UINT32 Size, UINT8 *Buffer)
{
  EFI_STATUS Status = EFI_UNSUPPORTED;
  if (mSetupItemString == NULL) {
    DEBUG ((DEBUG_ERROR, "%a() L%d, Not Found Setup Items Name String  In Current BIOS\n", __FUNCTION__, __LINE__));
    return Status;
  }
  if (Offset > mSetupItemStringSize) {
    DEBUG((DEBUG_ERROR, "Offset larger than Setup Items Name String Size\n"));
    return EFI_ABORTED;
  } else if (Offset + Size > mSetupItemStringSize) {
    CopyMem (Buffer, mSetupItemString + Offset, mSetupItemStringSize - Offset);
  } else if (Offset + Size < mSetupItemStringSize) {
    CopyMem (Buffer, mSetupItemString + Offset, Size);
  }
  return EFI_SUCCESS;
}

EFI_STATUS
GetSetupItemsOneofString(
  UINT32 Offset, UINT32 Size, UINT8 *Buffer)
{
  EFI_STATUS Status = EFI_UNSUPPORTED;
  if (mSetupItemOneOfOptionString == NULL) {
    DEBUG ((DEBUG_ERROR, "%a() L%d, Not Found Setup Items Oneof Name String  In Current BIOS\n", __FUNCTION__, __LINE__));
    return Status;
  }
  if (Offset > mSetupItemOneOfOptionStringSize) {
    DEBUG((DEBUG_ERROR, "Offset larger than Setup Oneof Items Name String Size\n"));
    return EFI_ABORTED;
  } else if (Size > mSetupItemOneOfOptionStringSize - Offset) {
    CopyMem (Buffer, mSetupItemOneOfOptionString + Offset, mSetupItemOneOfOptionStringSize - Offset);
  } else {
    CopyMem (Buffer, mSetupItemOneOfOptionString + Offset, Size);
  }

  return EFI_SUCCESS;
}


EFI_STATUS
GetSetupValueGroup (
  UPDATE_VALUE_PARAMETER *VariableParam, UINT32 Offset)
{
  UINT32                          Attributes;
  UINT32                          Index;
  UINT32                          VariableIndex;
  EFI_STATUS                      Status;
  UINTN                           VariableLength;
  UINT32                          ItemNameHash;
  UINT8                           *String;
  UPDATE_VALUE_PARAMETER          *pUpdateVariableParam;
  UINT32                          SingleOptionLength;

  Status = EFI_UNSUPPORTED;
  Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS;
  
  if (mSetupItem == NULL) {
    DEBUG ((DEBUG_ERROR, "%a() L%d, Not Found SetupItemDbInCurrentBIOS\n", __FUNCTION__, __LINE__));
    return Status;
  }

  //
  //if Offset = 0, Get all setup variable data
  //
  if (Offset == 0) {
    for (VariableIndex = 0; VariableIndex < mSetupVariableNumber; VariableIndex++) {
      VariableLength = (UINTN)mSetupVariableTable[VariableIndex].VariableLen;
      Status = mSmmVariable->SmmGetVariable (
                      mSetupVariableTable[VariableIndex].VariableName,
                      &(mSetupVariableTable[VariableIndex].VariableGuid),
                      &Attributes,
                      &VariableLength,
                      mSetupVariableData[VariableIndex]
                      );
      DEBUG ((DEBUG_INFO, "SmmGetVariable:%r\n",Status));
      if (EFI_ERROR(Status)) {
        return EFI_INVALID_PARAMETER;
      }
    }
  }
  for (pUpdateVariableParam = VariableParam; pUpdateVariableParam->NameString[0] != 0; pUpdateVariableParam++) {
    ItemNameHash = 0;
    String = (UINT8*)pUpdateVariableParam;
    BKDRHASH(String, ItemNameHash);
    for (Index = 0; mSetupItem[Index].VariableIndex != MAX_UINT32; Index++) {
      if (mSetupItem[Index].ItemNameHash == 0) {
        continue;
      }
      if (mSetupItem[Index].ItemNameHash == ItemNameHash) {
        VariableIndex = mSetupItem[Index].VariableIndex;

        //
        // Variable is not in the stored table.
        // Variable attribute is not NV + BS + RT.
        // Then, this variable is invalid
        //
        if ((VariableIndex >= mSetupVariableNumber) || (mSetupVariableTable[VariableIndex].Attribute != Attributes)) {
          return EFI_INVALID_PARAMETER;
        }
        if (mOrderedListNum > (UINT8)(mOptionsSplit[Index-1]->Count - 2)){
          mOrderedListNum = 0;
          return EFI_INVALID_PARAMETER;
        }
        if (mOptionsSplit[Index-1]->OpCode == EFI_IFR_ORDERED_LIST_OP){
          GetSingleOptionLength(mOptionsSplit[Index-1]->DataType,&SingleOptionLength);
          if (SingleOptionLength == 0){
            return EFI_INVALID_PARAMETER;
          }
          CopyMem ( (VOID *)&pUpdateVariableParam->ModifyValue, 
                    (VOID*)((UINT8*)mSetupVariableData[VariableIndex] + mSetupItem[Index].ItemOffset+ mOrderedListNum*SingleOptionLength), 
                    (UINTN)SingleOptionLength);
          if (mOrderedListNum == (UINT8)(mOptionsSplit[Index-1]->Count - 2)){
            mOrderedListNum = 0;
          } else {
            mOrderedListNum++;
          }
        } else {
          CopyMem ( (VOID *)&pUpdateVariableParam->ModifyValue, (VOID*)((UINT8*)mSetupVariableData[VariableIndex] + mSetupItem[Index].ItemOffset), (UINTN)mSetupItem[Index].ItemLength);
        }
        break;
      }
    }
    if (mSetupItem[Index].VariableIndex == MAX_UINT32) {
      DEBUG ((DEBUG_ERROR, "%a NOT FOUND\n", pUpdateVariableParam->NameString));
    }
  }
  return EFI_SUCCESS;
}

EFI_STATUS
SetSetupValueGroup (
  UPDATE_VALUE_PARAMETER *VariableParam, UINT32 Offset, UINT32 Size)
{
  UINT32                          Attributes;
  UINT32                          Index;
  UINT32                          VariableIndex;
  EFI_STATUS                      Status;
  UINTN                           VariableLength;
  UINT32                          ItemNameHash;
  UINT8                           *String;
  UPDATE_VALUE_PARAMETER          *pUpdateVariableParam;
  UINT32                          SingleOptionLength;

  Status = EFI_UNSUPPORTED;
  Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS;
  
  if (mSetupItem == NULL) {
    DEBUG ((DEBUG_ERROR, "%a() L%d, Not Found SetupItemDbInCurrentBIOS\n", __FUNCTION__, __LINE__));
    return Status;
  }

  //
  //if Offset = 0, Get all setup variable data
  //
  if (Offset == 0) {
    for (VariableIndex = 0; VariableIndex < mSetupVariableNumber; VariableIndex++) {
      VariableLength = (UINTN)mSetupVariableTable[VariableIndex].VariableLen;
      Status = mSmmVariable->SmmGetVariable (
                      mSetupVariableTable[VariableIndex].VariableName,
                      &(mSetupVariableTable[VariableIndex].VariableGuid),
                      &Attributes,
                      &VariableLength,
                      mSetupVariableData[VariableIndex]
                      );
      DEBUG ((DEBUG_INFO, "SmmGetVariable:%r\n",Status));
      if (EFI_ERROR(Status)) {
        return EFI_INVALID_PARAMETER;
      }
    }
  }
  for (pUpdateVariableParam = VariableParam; pUpdateVariableParam->NameString[0] != 0; pUpdateVariableParam++) {
    ItemNameHash = 0;
    String = (UINT8*)pUpdateVariableParam;
    BKDRHASH(String, ItemNameHash);
    for (Index = 0; mSetupItem[Index].VariableIndex != MAX_UINT32; Index++) {
      if (mSetupItem[Index].ItemNameHash == 0) {
        continue;
      }
      if (mSetupItem[Index].ItemNameHash == ItemNameHash) {
        VariableIndex = mSetupItem[Index].VariableIndex;

        //
        // Variable is not in the stored table.
        // Variable attribute is not NV + BS + RT.
        // Then, this variable is invalid
        //
        if ((VariableIndex >= mSetupVariableNumber) || (mSetupVariableTable[VariableIndex].Attribute != Attributes)) {
          return EFI_INVALID_PARAMETER;
        }
        if (mOrderedListNum > (UINT8)(mOptionsSplit[Index-1]->Count - 2)){
          mOrderedListNum = 0;
          return EFI_INVALID_PARAMETER;
        }
        
        if (mOptionsSplit[Index-1]->OpCode == EFI_IFR_ORDERED_LIST_OP){
          GetSingleOptionLength(mOptionsSplit[Index-1]->DataType,&SingleOptionLength);
          if (SingleOptionLength == 0){
            return EFI_INVALID_PARAMETER;
          }
          CopyMem ((VOID*)((UINT8*)mSetupVariableData[VariableIndex] + mSetupItem[Index].ItemOffset+ mOrderedListNum*SingleOptionLength), 
                    (VOID *)&pUpdateVariableParam->ModifyValue, 
                    (UINTN)SingleOptionLength);
          if (mOrderedListNum == (UINT8)(mOptionsSplit[Index-1]->Count - 2)){
            mOrderedListNum = 0;
          } else {
            mOrderedListNum++;
          }
        } else {
          CopyMem ((VOID*)((UINT8*)mSetupVariableData[VariableIndex] + mSetupItem[Index].ItemOffset), (VOID *)&pUpdateVariableParam->ModifyValue, (UINTN)mSetupItem[Index].ItemLength);
        }
        break;
      }
    }
    if (mSetupItem[Index].VariableIndex == MAX_UINT32) {
      DEBUG ((DEBUG_ERROR, "%a NOT FOUND\n", pUpdateVariableParam->NameString));
    }
  }
  //
  //if Size < SIZE_4KB, set variable, tool control the size
  //
  if (Size < SIZE_4KB) {
    for (VariableIndex = 0; VariableIndex < mSetupVariableNumber; VariableIndex++) {
      VariableLength = (UINTN)mSetupVariableTable[VariableIndex].VariableLen;
      Status = mSmmVariable->SmmSetVariable(
          mSetupVariableTable[VariableIndex].VariableName,
          &(mSetupVariableTable[VariableIndex].VariableGuid),
          Attributes,
          VariableLength,
          mSetupVariableData[VariableIndex]);
      if (EFI_ERROR(Status)) {
        return EFI_INVALID_PARAMETER;
      }
    }
  }
  return EFI_SUCCESS;
}


UINT8 *
FindVariableData (
  IN  VARIABLE_STORE_HEADER  *VariableStorage,
  IN  EFI_GUID               *VarGuid,
  IN  UINT32                 VarAttribute,
  IN  CHAR16                 *VarName
  )
{
  VARIABLE_HEADER *VariableHeader;
  VARIABLE_HEADER *VariableEnd;
  UINT8           *VariableData;

  VariableEnd    = (VARIABLE_HEADER *) ((UINT8 *) VariableStorage + VariableStorage->Size);
  VariableHeader = (VARIABLE_HEADER *) (VariableStorage + 1);
  VariableHeader = (VARIABLE_HEADER *) HEADER_ALIGN (VariableHeader);
  while (VariableHeader < VariableEnd) {
    if (CompareGuid (&VariableHeader->VendorGuid, VarGuid) &&
        VariableHeader->Attributes == VarAttribute &&
        StrCmp (VarName, (CHAR16 *) (VariableHeader + 1)) == 0) {
      VariableData = (UINT8 *)VariableHeader + sizeof (VARIABLE_HEADER) + VariableHeader->NameSize;
      return VariableData;
    }
    VariableHeader = (VARIABLE_HEADER *) ((UINT8 *) VariableHeader + sizeof (VARIABLE_HEADER) + VariableHeader->NameSize + VariableHeader->DataSize);
    VariableHeader = (VARIABLE_HEADER *) HEADER_ALIGN (VariableHeader);
  }

  return NULL;
}

EFI_STATUS
ChangeVariableDefault(
  IN OPTIONS_DATA                 *pOptions,
  IN VOID                         *Value,           //only in orderedlist this is decided by pOptions->DataType,others this is uint64
  IN UINT32                       Length
)
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  UINT32                        Index,Index2;
  UINT64                        DefaultVaule;
  UINT32                        OrderListNum;
  if (pOptions->OpCode == EFI_IFR_NUMERIC_OP){
    DefaultVaule = *((UINT64 *)Value);
    switch (pOptions->DataType) {
      case DATA_TYPE_UINT8:
        if ((DefaultVaule >= *(((UINT8 *)pOptions->Value) + 1)) && (DefaultVaule <= *(((UINT8 *)pOptions->Value) + 2)) ){
          *((UINT8 *)pOptions->Value) = (UINT8)DefaultVaule;
        } else {
          DEBUG((EFI_D_ERROR, "Value:%d should be %d - %d\n",DefaultVaule, *(((UINT8 *)pOptions->Value) + 1), *(((UINT8 *)pOptions->Value) + 2) ));
          Status = EFI_INVALID_PARAMETER;
        }
        break;
      case DATA_TYPE_UINT16:
        if ((DefaultVaule >= *(((UINT16 *)pOptions->Value) + 1)) && (DefaultVaule <= *(((UINT16 *)pOptions->Value) + 2)) ){
          *((UINT16 *)pOptions->Value) = (UINT16)DefaultVaule;
        } else {
          DEBUG((EFI_D_ERROR, "Value:%d should be %d - %d\n",DefaultVaule, *(((UINT16 *)pOptions->Value) + 1), *(((UINT16 *)pOptions->Value) + 2) ));
          Status = EFI_INVALID_PARAMETER;
        }
        break;
      case DATA_TYPE_UINT32:
        if ((DefaultVaule >= *(((UINT32 *)pOptions->Value) + 1)) && (DefaultVaule <= *(((UINT32 *)pOptions->Value) + 2)) ){
          *((UINT32 *)pOptions->Value) = (UINT32)DefaultVaule;
        } else {
          DEBUG((EFI_D_ERROR, "Value:%d should be %d - %d\n",DefaultVaule, *(((UINT32 *)pOptions->Value) + 1), *(((UINT32 *)pOptions->Value) + 2) ));
          Status = EFI_INVALID_PARAMETER;
        }
        break;
      case DATA_TYPE_UINT64:
        if ((DefaultVaule >= *(((UINT64 *)pOptions->Value) + 1)) && (DefaultVaule <= *(((UINT64 *)pOptions->Value) + 2)) ){
          *((UINT64 *)pOptions->Value) = DefaultVaule;
        } else {
          DEBUG((EFI_D_ERROR, "Value:%d should be %d - %d\n",DefaultVaule, *(((UINT64 *)pOptions->Value) + 1), *(((UINT64 *)pOptions->Value) + 2) ));
          Status = EFI_INVALID_PARAMETER;
        }
        break;
      default:
        break;
    }
  } else if (pOptions->OpCode == EFI_IFR_ONE_OF_OP || pOptions->OpCode == EFI_IFR_CHECKBOX_OP){
    //
    //Value[0] is for default value, ignore
    //
    DefaultVaule = *((UINT64 *)Value);
    for (Index = 1; Index < pOptions->Count; Index++) {
      switch (pOptions->DataType) {
        case DATA_TYPE_UINT8:
          if ( *((UINT8*)(pOptions->Value) + Index) == (UINT8)DefaultVaule){
            *((UINT8 *)pOptions->Value) = (UINT8)DefaultVaule;
            return Status;
          }
          break;
        case DATA_TYPE_UINT16:
          if ( *((UINT16*)(pOptions->Value) + Index) == (UINT16)DefaultVaule){
            *((UINT16 *)pOptions->Value) = (UINT16)DefaultVaule;
            return Status;
          }
          break;
        case DATA_TYPE_UINT32:
          if ( *((UINT32*)(pOptions->Value) + Index) == (UINT32)DefaultVaule){
            *((UINT32 *)pOptions->Value) = (UINT32)DefaultVaule;
            return Status;
          }
          break;
        case DATA_TYPE_UINT64:
          if ( *((UINT64*)(pOptions->Value) + Index) == (UINT64)DefaultVaule){
            *((UINT64 *)pOptions->Value) = (UINT64)DefaultVaule;
            return Status;
          }
          break;
      }
    }
    if (Index == pOptions->Count){
      Status = EFI_INVALID_PARAMETER;
    }
  } else if (pOptions->OpCode == EFI_IFR_ORDERED_LIST_OP){
    Status = EFI_SUCCESS;
    
    switch (pOptions->DataType) {
      case DATA_TYPE_UINT8:
        OrderListNum = Length/sizeof(UINT8);
        break;
      case DATA_TYPE_UINT16:
        OrderListNum = Length/sizeof(UINT16);
        break;
      case DATA_TYPE_UINT32:
        OrderListNum = Length/sizeof(UINT32);
        break;
      case DATA_TYPE_UINT64:
        OrderListNum = Length/sizeof(UINT64);
        break;
    }
    if (pOptions->Count != (OrderListNum+1)){                         //check orderedlist length
      Status = EFI_INVALID_PARAMETER;
      return Status;
    }

    for ( Index = 0; Index < OrderListNum; Index++){
      switch (pOptions->DataType) {
        case DATA_TYPE_UINT8:
          for ( Index2 = 1; Index2 < pOptions->Count; Index2++){      //check orderedlist default value whether exist
            if ( ((UINT8 *)Value)[Index] == ((UINT8*)(pOptions->Value))[Index2] ){
              break;
            }
          }
          if (Index2 == pOptions->Count){
            Status = EFI_INVALID_PARAMETER;
            return Status;
          }
          for ( Index2 = 0; Index2 < Index; Index2++){                //check orderedlist default value whether repeat
            if ( ((UINT8 *)Value)[Index2] == ((UINT8 *)Value)[Index]){
              Status = EFI_INVALID_PARAMETER;
              return Status;
            }
          }
          break;
        case DATA_TYPE_UINT16:
          for ( Index2 = 1; Index2 < pOptions->Count; Index2++){
            if ( ((UINT16 *)Value)[Index] == ((UINT16 *)(pOptions->Value))[Index2] ){
              break;
            }
          }
          if (Index2 == pOptions->Count){
            Status = EFI_INVALID_PARAMETER;
            return Status;
          }
          for ( Index2 = 0; Index2 < Index; Index2++){
            if ( ((UINT16 *)Value)[Index2] == ((UINT16 *)Value)[Index]){
              Status = EFI_INVALID_PARAMETER;
              return Status;
            }
          }
          break;
        case DATA_TYPE_UINT32:
          for ( Index2 = 1; Index2 < pOptions->Count; Index2++){
            if ( ((UINT32 *)Value)[Index] == ((UINT32 *)(pOptions->Value))[Index2] ){
              break;
            }
          }
          if (Index2 == pOptions->Count){
            Status = EFI_INVALID_PARAMETER;
            return Status;
          }
          for ( Index2 = 0; Index2 < Index; Index2++){
            if ( ((UINT32 *)Value)[Index2] == ((UINT32 *)Value)[Index]){
              Status = EFI_INVALID_PARAMETER;
              return Status;
            }
          }
          break;
        case DATA_TYPE_UINT64:
          for ( Index2 = 1; Index2 < pOptions->Count; Index2++){
            if ( ((UINT64 *)Value)[Index] == ((UINT64 *)(pOptions->Value))[Index2] ){
              break;
            }
          }
          if (Index2 == pOptions->Count){
            Status = EFI_INVALID_PARAMETER;
            return Status;
          }
          for ( Index2 = 0; Index2 < Index; Index2++){
            if ( ((UINT64 *)Value)[Index2] == ((UINT64 *)Value)[Index]){
              Status = EFI_INVALID_PARAMETER;
              return Status;
            }
          }
          break;
      }
    }

    for ( Index = 0; Index < OrderListNum; Index++){
      switch (pOptions->DataType) {
        case DATA_TYPE_UINT8:
          ((UINT8 *)pOptions->Value)[Index+1] = ((UINT8 *)Value)[Index];
          break;
        case DATA_TYPE_UINT16:
          ((UINT16 *)pOptions->Value)[Index+1] = ((UINT16 *)Value)[Index];
          break;
        case DATA_TYPE_UINT32:
          ((UINT32 *)pOptions->Value)[Index+1] = ((UINT32 *)Value)[Index];
          break;
        case DATA_TYPE_UINT64:
          ((UINT64 *)pOptions->Value)[Index+1] = ((UINT64 *)Value)[Index];
          break;
      }
    }
  }

  return Status;
}

EFI_STATUS
UpdateVariableDefault()
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  EFI_STATUS                    TempStatus = EFI_SUCCESS;
  VARIABLE_STORE_HEADER         *gVariableStorage;
  EFI_PEI_HOB_POINTERS          GuidHob;
  UINT32                        Index,VariableIndex;
  UINT64                        DefaultValue = 0;
  VOID                          *OrderedListDefaultValue;
  OPTIONS_DATA                  *pOptions;
  UINT8                         **VariableHeaderData = NULL;
  GuidHob.Raw = GetFirstGuidHob (&gEfiVariableGuid);
  if (GuidHob.Raw == NULL) {
    GuidHob.Raw = GetFirstGuidHob (&gVariableDefaultGuid);
  }
  if (GuidHob.Raw == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  gVariableStorage = GET_GUID_HOB_DATA (GuidHob.Guid);
  VariableHeaderData = AllocatePool(mSetupVariableNumber*sizeof(VOID *));
  OrderedListDefaultValue = AllocatePool(SIZE_4KB);
  for ( Index = 0; Index < mSetupVariableNumber; Index++){
    VariableHeaderData[Index] = FindVariableData(gVariableStorage, 
                          &mSetupVariableTable[Index].VariableGuid, 
                          mSetupVariableTable[Index].Attribute, 
                          mSetupVariableTable[Index].VariableName
                        );
    if (VariableHeaderData[Index] == NULL){
      continue;
    }
  }
  
  for ( Index = 1; Index <= mSetupItemNumber; Index++){
    VariableIndex = mSetupItem[Index].VariableIndex;
    if (VariableHeaderData[VariableIndex] == NULL){
      continue;
    }
    DefaultValue = 0;
    pOptions = mOptionsSplit[Index-1];
    if (pOptions->OpCode != EFI_IFR_ORDERED_LIST_OP){
      CopyMem ( (VOID *)&DefaultValue, (VOID*)(VariableHeaderData[VariableIndex] + mSetupItem[Index].ItemOffset), (UINTN)mSetupItem[Index].ItemLength);
      TempStatus = ChangeVariableDefault(pOptions, (VOID *)&DefaultValue, sizeof(UINT64));
      if (EFI_ERROR(TempStatus)){
        Status = EFI_INVALID_PARAMETER;
        DEBUG((EFI_D_ERROR, "Offset %d to variable:%s change Default to %d is not allowed;ItemNumber:%d\n",mSetupItem[Index].ItemOffset,mSetupVariableTable[VariableIndex].VariableName,DefaultValue,Index));
        continue;
      } 
    } else {
      CopyMem ( OrderedListDefaultValue, (VOID*)(VariableHeaderData[VariableIndex] + mSetupItem[Index].ItemOffset), (UINTN)mSetupItem[Index].ItemLength);
      TempStatus = ChangeVariableDefault(pOptions, OrderedListDefaultValue, (UINT32)mSetupItem[Index].ItemLength);
      if (EFI_ERROR(TempStatus)){
        Status = EFI_INVALID_PARAMETER;
        DEBUG((EFI_D_ERROR, "Offset %d to variable:%s change orderedlist Default failed\n ",mSetupItem[Index].ItemOffset,mSetupVariableTable[VariableIndex].VariableName));
        continue;
      } 
    }
  }

    FreePool(VariableHeaderData);
    FreePool(OrderedListDefaultValue);
    return Status;
}





EFI_STATUS
FindOptionOffset(
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  OUT UINT32                      *OptionOffset
  )
{
  UINT32                        Index,VariableIndex;
  for ( Index = 0; Index < mSetupVariableNumber; Index++){
    if (StrCmp(VariableName, mSetupVariableTable[Index].VariableName) == 0){
      if (CompareMem(VendorGuid, &mSetupVariableTable[Index].VariableGuid, sizeof(EFI_GUID)) == 0) {
        break;
      }
    }
  }
  if (Index == mSetupVariableNumber){
    return EFI_NOT_FOUND;
  }
  
  VariableIndex = Index;
  for ( Index = 1; Index <= mSetupItemNumber; Index++){
    if (mSetupItem[Index].VariableIndex == VariableIndex && mSetupItem[Index].ItemOffset == Offset){
      *OptionOffset = Index-1;
      return EFI_SUCCESS;
    }
  }
  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
UpdateMinValue(
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT64                       Value
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  UINT32                        OptionOffset = 0;
  OPTIONS_DATA                  *pOptions;

  Status = FindOptionOffset(VariableName,VendorGuid,Offset,&OptionOffset);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "Offset %d to variable:%s not found\n",Offset,VariableName));
    return Status;
  }
  pOptions = mOptionsSplit[OptionOffset];
  if (pOptions->OpCode == EFI_IFR_NUMERIC_OP){
    switch (pOptions->DataType) {
      case DATA_TYPE_UINT8:
        if (Value <= *(((UINT8 *)pOptions->Value) + 2)) {
          *(((UINT8 *)pOptions->Value) + 1) = (UINT8)Value;
          Status = EFI_SUCCESS;
        } else {
          DEBUG((EFI_D_ERROR, "Offset %d to variable:%s change min to %d is not allowed\n",Offset,VariableName,Value));
          Status = EFI_INVALID_PARAMETER;
        }
        break;
      case DATA_TYPE_UINT16:
        if (Value <= *(((UINT16 *)pOptions->Value) + 2)) {
          *(((UINT16 *)pOptions->Value) + 1) = (UINT16)Value;
          Status = EFI_SUCCESS;
        } else {
          DEBUG((EFI_D_ERROR, "Offset %d to variable:%s change min to %d is not allowed\n",Offset,VariableName,Value));
          Status = EFI_INVALID_PARAMETER;
        }
        break;
      case DATA_TYPE_UINT32:
        if (Value <= *(((UINT32 *)pOptions->Value) + 2)) {
          *(((UINT32 *)pOptions->Value) + 1) = (UINT32)Value;
          Status = EFI_SUCCESS;
        } else {
          DEBUG((EFI_D_ERROR, "Offset %d to variable:%s change min to %d is not allowed\n",Offset,VariableName,Value));
          Status = EFI_INVALID_PARAMETER;
        }
        break;
      case DATA_TYPE_UINT64:
        if (Value <= *(((UINT64 *)pOptions->Value) + 2)) {
          *(((UINT64 *)pOptions->Value) + 1) = (UINT64)Value;
          Status = EFI_SUCCESS;
        } else {
          DEBUG((EFI_D_ERROR, "Offset %d to variable:%s change min to %d is not allowed\n",Offset,VariableName,Value));
          Status = EFI_INVALID_PARAMETER;
        }
        break;
      default:
        break;
    }
  } else {
    DEBUG((EFI_D_INFO, "%s is not a numeric variable\n",VariableName));
    Status = EFI_INVALID_PARAMETER;
  }
  if (Status == EFI_SUCCESS){
    DEBUG((EFI_D_ERROR, "Offset %d to variable:%s change min to %d\n",Offset,VariableName,Value));
  }
  return Status;
  
}

EFI_STATUS
EFIAPI
UpdateMaxValue(
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT64                       Value
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  UINT32                        OptionOffset = 0;
  OPTIONS_DATA                  *pOptions;

  Status = FindOptionOffset(VariableName,VendorGuid,Offset,&OptionOffset);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "Offset %d to variable:%s not found\n",Offset,VariableName));
    return Status;
  }
  pOptions = mOptionsSplit[OptionOffset];
  if (pOptions->OpCode == EFI_IFR_NUMERIC_OP){
    switch (pOptions->DataType) {
      case DATA_TYPE_UINT8:
        if (Value >= *(((UINT8 *)pOptions->Value) + 1)) {
          *(((UINT8 *)pOptions->Value) + 2) = (UINT8)Value;
          Status = EFI_SUCCESS;
        } else {
          DEBUG((EFI_D_ERROR, "Offset %d to variable:%s change max to %d is not allowed\n",Offset,VariableName,Value));
          Status = EFI_INVALID_PARAMETER;
        }
        break;
      case DATA_TYPE_UINT16:
        if (Value >= *(((UINT16 *)pOptions->Value) + 1)) {
          *(((UINT16 *)pOptions->Value) + 2) = (UINT16)Value;
          Status = EFI_SUCCESS;
        } else {
          DEBUG((EFI_D_ERROR, "Offset %d to variable:%s change max to %d is not allowed\n",Offset,VariableName,Value));
          Status = EFI_INVALID_PARAMETER;
        }
        break;
      case DATA_TYPE_UINT32:
        if (Value >= *(((UINT32 *)pOptions->Value) + 1)) {
          *(((UINT32 *)pOptions->Value) + 2) = (UINT32)Value;
          Status = EFI_SUCCESS;
        } else {
          DEBUG((EFI_D_ERROR, "Offset %d to variable:%s change max to %d is not allowed\n",Offset,VariableName,Value));
          Status = EFI_INVALID_PARAMETER;
        }
        break;
      case DATA_TYPE_UINT64:
        if (Value >= *(((UINT64 *)pOptions->Value) + 1)) {
          *(((UINT64 *)pOptions->Value) + 2) = (UINT64)Value;
          Status = EFI_SUCCESS;
        } else {
          DEBUG((EFI_D_ERROR, "Offset %d to variable:%s change max to %d is not allowed\n",Offset,VariableName,Value));
          Status = EFI_INVALID_PARAMETER;
        }
        break;
      default:
        break;
    }
  } else {
    DEBUG((EFI_D_INFO, "Offset %d to variable:%s is not a numeric variable\n",Offset,VariableName));
    Status = EFI_INVALID_PARAMETER;
  }
  if (Status == EFI_SUCCESS){
    DEBUG((EFI_D_ERROR, "Offset %d to variable:%s change max to %d\n",Offset,VariableName,Value));
  }
  
  return Status;
}



EFI_STATUS
EFIAPI
UpdateDefaultValue(
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT64                       Value
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  UINT32                        OptionOffset = 0;
  OPTIONS_DATA                  *pOptions;

  Status = FindOptionOffset(VariableName,VendorGuid,Offset,&OptionOffset);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "Offset %d to variable:%s not found\n",Offset,VariableName));
    return Status;
  }
  pOptions = mOptionsSplit[OptionOffset];
  if (pOptions->OpCode == EFI_IFR_ORDERED_LIST_OP){
    DEBUG((EFI_D_ERROR, "UpdateDefaultValue change orderedlist default value is not allowed\n"));
    return EFI_UNSUPPORTED;
  }
  
  Status = ChangeVariableDefault(pOptions, (VOID *)&Value, sizeof(UINT64));
  if (EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "Offset %d to variable:%s change Default to %d is not allowed\n",Offset,VariableName,Value));
  } else {
    DEBUG((EFI_D_ERROR, "Offset %d to variable:%s change Default to %d\n",Offset,VariableName,Value));
  }
  
  return Status;

}

EFI_STATUS
EFIAPI
UpdateAttribute(
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT16                       Attribute
  )
{
  return EFI_UNSUPPORTED;
}


EFI_STATUS
FindMatchedOptionValue(
  IN  OPTIONS_DATA  *pOptions,
  IN  UINT64         Value,
  OUT UINT64        *OptionIndex
)
{
  EFI_STATUS                    Status = EFI_INVALID_PARAMETER;
  UINT64                        Index;

  for ( Index = 1; Index < pOptions->Count; Index++){
    switch (pOptions->DataType){
      case DATA_TYPE_UINT8:
        if ( ((UINT8 *)pOptions->Value)[Index] == Value){
          *OptionIndex = Index;
          Status = EFI_SUCCESS;
        }
        break;

      case DATA_TYPE_UINT16:
        if ( ((UINT16 *)pOptions->Value)[Index] == Value){
          *OptionIndex = Index;
          Status = EFI_SUCCESS;
        }
        break;

      case DATA_TYPE_UINT32:
        if ( ((UINT32 *)pOptions->Value)[Index] == Value){
          *OptionIndex = Index;
          Status = EFI_SUCCESS;
        }
        break;

      case DATA_TYPE_UINT64:
        if ( ((UINT64 *)pOptions->Value)[Index] == Value){
          *OptionIndex = Index;
          Status = EFI_SUCCESS;
        }
        break;
      
      default:
        break;
    }
    if (Status == EFI_SUCCESS){
      break;
    }
  }

  return Status;
}


EFI_STATUS
EFIAPI
UpdateOptionValue (
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINTN                        Count,
  IN UINT64                       *OptionValueArray
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  UINT32                        OptionOffset = 0;
  OPTIONS_DATA                  *pOptions;
  UINT64                        Index;
  UINT64                        OptionIndex;
  UINT8                         *OptionValueExist = NULL;

  Status = FindOptionOffset(VariableName,VendorGuid,Offset,&OptionOffset);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "Offset %d to variable:%s not found\n",Offset,VariableName));
    return Status;
  }
  pOptions = mOptionsSplit[OptionOffset];
  if (pOptions->OpCode != EFI_IFR_ONE_OF_OP){
    DEBUG((EFI_D_ERROR, "UpdateOptionValue only support change one of option value\n"));
    return EFI_INVALID_PARAMETER;
  }
  
  //After get pOptions,we set OptionValueExist to know new option value place in old option value
  OptionValueExist = AllocatePool(pOptions->Count * sizeof(UINT8) + 1);
  SetMem(OptionValueExist, pOptions->Count * sizeof(UINT8) + 1, 0);
  OptionValueExist[1] = 1;

  for ( Index = 0; Index < Count; Index++){
    Status = FindMatchedOptionValue(pOptions, OptionValueArray[Index], &OptionIndex);
    if (EFI_ERROR (Status)){
      DEBUG((EFI_D_ERROR, "Option value:%llx is not supported\n",OptionValueArray[Index]));
      Status = EFI_INVALID_PARAMETER;
      FreePool(OptionValueExist);
      return Status;
    } else {
      OptionValueExist[OptionIndex] = 1;
    }
  }

  //After get OptionValueExist,we set pOptions->Value[Index] to the value of pOptions->Value[Index-1] if OptionValueExist[Index] is zero

  for ( Index = 1; Index < pOptions->Count; Index++){
    switch (pOptions->DataType){
      case DATA_TYPE_UINT8:
        if (OptionValueExist[Index] == 0){
          ((UINT8 *)pOptions->Value)[Index] = ((UINT8 *)pOptions->Value)[Index-1];
        }
        break;

      case DATA_TYPE_UINT16:
        if (OptionValueExist[Index] == 0){
          ((UINT16 *)pOptions->Value)[Index] = ((UINT16 *)pOptions->Value)[Index-1];
        }
        break;

      case DATA_TYPE_UINT32:
        if (OptionValueExist[Index] == 0){
          ((UINT32 *)pOptions->Value)[Index] = ((UINT32 *)pOptions->Value)[Index-1];
        }
        break;

      case DATA_TYPE_UINT64:
        if (OptionValueExist[Index] == 0){
          ((UINT64 *)pOptions->Value)[Index] = ((UINT64 *)pOptions->Value)[Index-1];
        }
        break;
      
      default:
        break;
    }
  }
  
  if (OptionValueExist != NULL){
    FreePool(OptionValueExist);
  }
  
  return Status;
}


EFI_STATUS
EFIAPI
CheckVariableValid(
  CHECK_VALUE_PARAMETER  *VariableParameter
  )
{
  UINT32           VariableTableIndex;
  UINT32           ItemNumberIndex;

  //Set mDumpWhetherHidden[ItemNumberIndex-1] = FALSE;
  for ( VariableTableIndex = 0; VariableTableIndex < mSetupVariableNumber; VariableTableIndex++){
    if (StrCmp(VariableParameter->VariableName, mSetupVariableTable[VariableTableIndex].VariableName) == 0 &&
        CompareGuid(&VariableParameter->Guid, &mSetupVariableTable[VariableTableIndex].VariableGuid) == TRUE){
      break;
    }
  }
  if (VariableTableIndex == mSetupVariableNumber){
    return EFI_NOT_FOUND;
  }

  for ( ItemNumberIndex = 1; ItemNumberIndex <= mSetupItemNumber; ItemNumberIndex++){
    if (VariableParameter->Offset == mSetupItem[ItemNumberIndex].ItemOffset && VariableParameter->Length == mSetupItem[ItemNumberIndex].ItemLength){
      if (mSetupItem[ItemNumberIndex].VariableIndex == VariableTableIndex){
        VariableParameter->ItemNumber = ItemNumberIndex;
        return EFI_SUCCESS;
      }
    }
  }
  return EFI_NOT_FOUND;
}


