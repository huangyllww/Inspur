/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  KcsBmc.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/


#include "IpmiRedirFru.h"

EFI_IPMI_FRU_GLOBAL *mIpmiFruGlobal;
UINT16               mDeviceID = 0;

UINT8
GetEmptyFruSlot (
  VOID
  )
/*++

Routine Description:

  Get Empty Fru Slot

Arguments:

  None

Returns:

  UINT8

--*/
{
  UINT8 i;

  for (i = 1; i < mIpmiFruGlobal->MaxFruSlots; i++) {
    if (!mIpmiFruGlobal->FruDeviceInfo[i].Valid) {
      return i;
    }
  }

  return 0xFF;
}

EFI_STATUS
EfiGetFruRedirInfo (
  IN  EFI_SM_FRU_REDIR_PROTOCOL           *This,
  IN  UINTN                               FruSlotNumber,
  OUT EFI_GUID                            *FruFormatGuid,
  OUT UINTN                               *DataAccessGranularity,
  OUT CHAR16                              **FruInformationString
  )
/*++

Routine Description:

  Get Fru Redir Info
 
Arguments:

  This
  FruSlotNumber
  FruFormatGuid
  DataAccessGranularity
  FruInformationString

Returns:

  EFI_NO_MAPPING
  EFI_SUCCESS

--*/
{
  return EFI_SUCCESS;
}

EFI_STATUS
EfiGetFruSlotInfo (
  IN  EFI_SM_FRU_REDIR_PROTOCOL           *This,
  OUT EFI_GUID                            *FruTypeGuid,
  OUT UINTN                               *StartFruSlotNumber,
  OUT UINTN                               *NumSlots
  )
/*++

Routine Description:

  Get Fru Slot Info

Arguments:

  This
  FruTypeGuid
  StartFruSlotNumber
  NumSlots

Returns:

  EFI_SUCCESS

--*/
{

  return EFI_SUCCESS;
}

EFI_STATUS
EfiGetFruRedirData (
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This,
  IN UINTN                                FruSlotNumber,
  IN UINTN                                FruDataOffset,
  IN UINTN                                FruDataSize,
  IN UINT8                                *FruData
  )
/*++

Routine Description:

  Get Fru Redir Data 

Arguments:

  This
  FruSlotNumber
  FruDataOffset
  FruDataSize
  FruData

Returns:

  EFI_STATUS

--*/
{
  EFI_IPMI_FRU_GLOBAL *FruPrivate;
  FRU_READ_COMMAND    Command;
  UINT8               ResponseData[20];
  UINT32              ResponseDataSize;
  UINT8               PointerOffset;
  EFI_STATUS          Status;
  INT32               RemainingCount = 0;

  FruPrivate    = NULL;
  PointerOffset = 0;

  FruPrivate    = INSTANCE_FROM_EFI_SM_IPMI_FRU_THIS (This);

  if ((FruSlotNumber + 1) > FruPrivate->NumSlots) {
    return EFI_NO_MAPPING;
  }

  if (FruPrivate->FruDeviceInfo[FruSlotNumber].FruDevice.LogicalFruDevice) {
    //
    // Create the FRU Read Command for the logical FRU Device.
    //
    Command.Data.FruDeviceId  = (UINT8)mDeviceID;// FruPrivate->FruDeviceInfo[FruSlotNumber].FruDevice.FruDeviceId;
    Command.Data.FruOffset    = (UINT16) FruDataOffset;
    RemainingCount            = (INT32) FruDataSize;

    //
    // Collect the data till it is completely retrieved.
    //
    while (RemainingCount > 0) {
      if (RemainingCount > 0x10) {
        Command.Count = 0x10;
      } else {
        Command.Count = (UINT8)RemainingCount;
      }

      ResponseDataSize = 20;
      Status = EfiSendCommandToBMC (
                EFI_SM_NETFN_STORAGE,
                EFI_STORAGE_READ_FRU_DATA,
                (UINT8 *) &Command,
                sizeof (FRU_READ_COMMAND),
                (UINT8 *) &ResponseData,
                (UINT8 *) &ResponseDataSize
                );
      DEBUG ((EFI_D_INFO, "EfiSendCommandToBMC() %r,ResponseData[0]:%x\n",Status,ResponseData[0]));
      ASSERT (Status != EFI_BUFFER_TOO_SMALL);
      if (EFI_ERROR (Status)) {
        return Status;
      }

      //
      // If the read FRU command returns a count of 0, then no FRU data was found, so exit.
      //
      if ( ResponseData[0] == 0x00 ) {
        return EFI_NOT_FOUND;
      }
      if ( ResponseData[0] > Command.Count) {
        return EFI_NOT_FOUND;
      }
      if ( ResponseData[0] > 0x10) {
        return EFI_NOT_FOUND;
      }

      //
      // In case of partial retrieval; Data[0] contains the retrieved data size;
      //
      RemainingCount =  (RemainingCount - ResponseData[0]);  // Remaining Count
      if (RemainingCount < 0) {
        return EFI_NOT_FOUND;
      }
      Command.Data.FruOffset = (UINT16) (Command.Data.FruOffset + ResponseData[0]);  // Next Offset to retrieve
      CopyMem (&FruData[PointerOffset], &ResponseData[1], ResponseData[0]);  // Copy the partial data
      PointerOffset = (UINT8) (PointerOffset + ResponseData[0]);  // Next offset to the iput pointer.
    }
  } else {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EfiSetFruRedirData (
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This,
  IN UINTN                                FruSlotNumber,
  IN UINTN                                FruDataOffset,
  IN UINTN                                FruDataSize,
  IN UINT8                                *FruData
  )
/*++

Routine Description:

  Set Fru Redir Data

Arguments:

  This
  FruSlotNumber
  FruDataOffset
  FruDataSize
  FruData

Returns:

  EFI_STATUS

--*/
{
  EFI_IPMI_FRU_GLOBAL *FruPrivate;
  FRU_WRITE_COMMAND   Command;
  UINT8               Count;
  UINT8               BackupCount;
  UINT8               ResponseData[10];
  UINT32              ResponseDataSize;
  UINT8               PointerOffset;
  EFI_STATUS          Status;

  FruPrivate    = NULL;
  PointerOffset = 0;

  FruPrivate    = INSTANCE_FROM_EFI_SM_IPMI_FRU_THIS (This);

  if ((FruSlotNumber + 1) > FruPrivate->NumSlots) {
    return EFI_NO_MAPPING;
  }

  if (FruPrivate->FruDeviceInfo[FruSlotNumber].FruDevice.LogicalFruDevice) {
    //
    // Create the FRU Read Command for the logical FRU Device.
    //
    Command.Data.FruDeviceId  = FruPrivate->FruDeviceInfo[FruSlotNumber].FruDevice.FruDeviceId;
    Command.Data.FruOffset    = (UINT16) FruDataOffset;
    Count                     = (UINT8) FruDataSize;
    PointerOffset             = 0;

    //
    // Collect the data till it is completely retrieved.
    //
    while (Count != 0) {
      //
      // Backup the count, since we are going to perform fragmanted reads
      //
      BackupCount = Count;
      if (Count > 0x10) {
        Count = 0x10;
      }
      CopyMem (Command.FruData, &FruData[PointerOffset], Count);

      ResponseDataSize = 10;
      Status = EfiSendCommandToBMC (
                EFI_SM_NETFN_STORAGE,
                EFI_STORAGE_WRITE_FRU_DATA,
                (UINT8 *) &Command,
                (sizeof (FRU_COMMON_DATA) + Count),
                (UINT8 *) &ResponseData,
                (UINT8 *) &ResponseDataSize
                );

      ASSERT (Status != EFI_BUFFER_TOO_SMALL);
      if (EFI_ERROR (Status)) {
        return Status;
      }

      Count = BackupCount;

      //
      // In case of partial retrieval; Data[0] contains the retrieved data size;
      //
      Count = (UINT8) (Count - ResponseData[0]);  // Remaining Count
      Command.Data.FruOffset = (UINT16) (Command.Data.FruOffset + ResponseData[0]);  // Next Offset to set
      PointerOffset = (UINT8) (PointerOffset + ResponseData[0]);  // Next offset to the iput pointer.
    }
  } else {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
InitializeSmRedirFruLayer (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
/*++

Routine Description:

  Initialize SM Redirection Fru Layer

Arguments:

  ImageHandle - ImageHandle of the loaded driver
  SystemTable - Pointer to the System Table

Returns:

  EFI_STATUS

--*/
{
  EFI_HANDLE        NewHandle;
  EFI_STATUS        Status;  
  EFI_SM_CTRL_INFO  ControllerInfo;  
  UINT8             ResponseDataSize;

  gST = SystemTable;
  gBS = gST->BootServices;

  //
  // Initialize COM layer Library as we will use IPMI COM Library functions.
  //
  EfiInitializeIpmiBase ();
  //
  // Initialize Global memory
  //
  mIpmiFruGlobal = AllocateRuntimePool (sizeof (EFI_IPMI_FRU_GLOBAL));
  ASSERT(mIpmiFruGlobal != NULL);
  mIpmiFruGlobal->NumSlots  = 0;
  mIpmiFruGlobal->IpmiRedirFruProtocol.GetFruRedirInfo  = EfiGetFruRedirInfo;
  mIpmiFruGlobal->IpmiRedirFruProtocol.GetFruSlotInfo   = EfiGetFruSlotInfo;
  mIpmiFruGlobal->IpmiRedirFruProtocol.GetFruRedirData  = EfiGetFruRedirData;
  mIpmiFruGlobal->IpmiRedirFruProtocol.SetFruRedirData  = EfiSetFruRedirData;
  mIpmiFruGlobal->Signature = EFI_SM_FRU_REDIR_SIGNATURE;
  mIpmiFruGlobal->MaxFruSlots = MAX_FRU_SLOT;
  //
  //  Get all the SDR Records from BMC and retrieve the Record ID from the structure for future use.
  //
  ResponseDataSize = sizeof (ControllerInfo);
  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_APP,
            EFI_APP_GET_DEVICE_ID,
            (UINT8 *) NULL,
            0,
            (UINT8 *) &ControllerInfo,
            (UINT8 *) &ResponseDataSize
            );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (ControllerInfo.FruInventorySupport) {
    //
    // Add a mandatory FRU Inventory device attached to the controller.
    //
    mIpmiFruGlobal->FruDeviceInfo[mIpmiFruGlobal->NumSlots].Valid = TRUE;
    ZeroMem (&mIpmiFruGlobal->FruDeviceInfo[mIpmiFruGlobal->NumSlots].FruDevice, sizeof (EFI_FRU_DATA_INFO));
    mIpmiFruGlobal->FruDeviceInfo[mIpmiFruGlobal->NumSlots].FruDevice.LogicalFruDevice = 1;
    mIpmiFruGlobal->NumSlots++;
  }

  GenerateFruSmbiosData (&mIpmiFruGlobal->IpmiRedirFruProtocol);

  //
  // Install the FRU Ipmi Redir protocol.
  //
  NewHandle = NULL;
  Status = gBS->InstallProtocolInterface (
                 &NewHandle,
                 &gEfiRedirFruProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &mIpmiFruGlobal->IpmiRedirFruProtocol
                 );

  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
