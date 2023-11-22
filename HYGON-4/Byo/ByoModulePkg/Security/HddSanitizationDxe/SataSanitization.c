/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SataSanitization.c

Abstract:
  for Hdd Sanitization driver.

Revision History:

**/

#include "HddSanitizationDxe.h"
#include "HddSanitizationCommon.h"

/**
  Get a ATA device Security Status with the device Identify Data,and report to Hdd password DXE driver
**/
EFI_STATUS
GetAtaDevicePasswordSecurityStatus (
  IN     ATA_IDENTIFY_DATA    *IdentifyData,
  IN OUT HDD_SECURITY_INFO    *HddSecInfo
  )
{
  UINT16  RevisionCode;

  HddSecInfo->Supported           = (IdentifyData->command_set_supported_82 & BIT1) ? 1 : 0;
  HddSecInfo->Enabled             = (IdentifyData->security_status & BIT1) ? 1 : 0;
  HddSecInfo->Locked              = (IdentifyData->security_status & BIT2) ? 1 : 0;
  HddSecInfo->Frozen              = (IdentifyData->security_status & BIT3) ? 1 : 0;

  RevisionCode = IdentifyData->master_password_identifier;
  DEBUG((EFI_D_INFO, "RevCode:%X\n", RevisionCode));

  DEBUG((EFI_D_INFO, "En:%d Lock:%d Fz:%d RC:%X Supported:%X\n", \
                      HddSecInfo->Enabled,              \
                      HddSecInfo->Locked,               \
                      HddSecInfo->Frozen,               \
                      RevisionCode,                  \
                      HddSecInfo->Supported             \
                      ));
  return EFI_SUCCESS;
}


//
//Get ATA device Identify Data with IDENTIFY DEVICE command (0xec), Incomplete data may be returned by this command.
//Spec ata-atapi-8 -> Section 7.12 IDENTIFY DEVICE - ECh, PIO Data-In.
//
EFI_STATUS
GetAtaDeviceIdentifyData (
  IN ATA_DEVICE_INFO     *AtaInfo
  )
{
  EFI_STATUS                        Status;
  EFI_ATA_COMMAND_BLOCK             Acb;
  EFI_ATA_PASS_THRU_COMMAND_PACKET  Packet;


  ZeroMem(&Acb, sizeof (Acb));
  ZeroMem(AtaInfo->Asb, sizeof(EFI_ATA_STATUS_BLOCK));
  ZeroMem(AtaInfo->IdentifyData, sizeof(ATA_IDENTIFY_DATA));

  Acb.AtaCommand    = ATA_CMD_IDENTIFY_DRIVE;
  Acb.AtaDeviceHead = (UINT8) (BIT7 | BIT6 | BIT5 | (AtaInfo->PortMp == 0xFFFF ? 0 : (AtaInfo->PortMp << 4)));

  ZeroMem(&Packet, sizeof (Packet));
  Packet.Protocol         = EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_IN;
  Packet.Length           = EFI_ATA_PASS_THRU_LENGTH_BYTES;
  Packet.Asb              = AtaInfo->Asb;
  Packet.Acb              = &Acb;
  Packet.InDataBuffer     = (VOID*)AtaInfo->IdentifyData;
  Packet.InTransferLength = sizeof(ATA_IDENTIFY_DATA);
  Packet.Timeout          = ATA_TIMEOUT;

  Status = AtaInfo->AtaPassThru->PassThru (
                                   AtaInfo->AtaPassThru,
                                   AtaInfo->Port,
                                   AtaInfo->PortMp,
                                   &Packet,
                                   NULL
                                   );

  return Status;
}

EFI_STATUS
HandleSataDevice (
    ATA_PASSTHRU_HOOK_PARAMETER   *p,
    UINT16                        Port,
    UINT16                        PortMp
  )
{
  LIST_ENTRY                        *Entry;
  HDD_SANITIZE_DEVICE_ENTRY         *HddSanDevEntry;
  BOOLEAN                           EntryExisted;
  EFI_STATUS                        Status;
  CHAR8                             ModelName[40+1];
  HDD_PASSWORD_INFO                 *HdpInfo;
  ATA_DEVICE_INFO                   *AtaInfo;
  EFI_PCI_IO_PROTOCOL               *PciIo;
  UINT8                             PciScc;
  HDD_SECURITY_INFO                *HddSecInfo;
  UINTN                             SegNum;
  UINTN                             BusNum;
  UINTN                             DevNum;
  UINTN                             FuncNum;
  EFI_DEVICE_PATH_PROTOCOL          *SataControllerDevicePath;
  UINT16                            PlatSataHostIndex;
  PLAT_HOST_INFO_PROTOCOL           *PlatHostInfo = NULL;
  UINT16                            IndexBase1 = 0;
  UINT32                            PciId;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePathNode = NULL;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;

  Status = gBS->OpenProtocol (
                  p->Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID**) &SataControllerDevicePath,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR(Status)) {
    SataControllerDevicePath = NULL;
  }

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  EntryExisted = FALSE;
  EFI_LIST_FOR_EACH (Entry, &mHddSanitizationCfgFormList) {
    HddSanDevEntry = BASE_CR (Entry, HDD_SANITIZE_DEVICE_ENTRY, Link);
    //
    // When reconnecting locked storage device, HandleSataDevice should only check ATA device
    //
    if ((*(UINT32 *)HddSanDevEntry->DevInfo) != ATA_DEVICE_INFO_SIGN) {
      continue;
    }
    AtaInfo         = (ATA_DEVICE_INFO *)HddSanDevEntry->DevInfo;
    HdpInfo         = &AtaInfo->HdpInfo;

    if (AtaInfo->Port              == Port   &&
        AtaInfo->PortMp            == PortMp && 
        SataControllerDevicePath   != NULL   &&
        HdpInfo->ControllerDevPath != NULL   &&
        UefiCompareDevicePath(SataControllerDevicePath, HdpInfo->ControllerDevPath)) {
      //
      // If Control device path matches again. It is caused by driver reconnection. Try to Refresh AtaPassThru Protocol and controller Handler 
      //
      EntryExisted         = TRUE;
      AtaInfo->AtaPassThru = p->AtaPassThru;
      AtaInfo->Port        = Port;
      AtaInfo->PortMp      = PortMp;
      HdpInfo->Controller  = p->Controller;
      break;
    }
  }

  if (EntryExisted) {
    DEBUG((EFI_D_INFO, "%a exist\n", __FUNCTION__));
    return EFI_ALREADY_STARTED;
  }

  HddSanDevEntry = AllocateZeroPool(sizeof(HDD_SANITIZE_DEVICE_ENTRY));
  ASSERT(HddSanDevEntry != NULL);
  if (HddSanDevEntry == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  AtaInfo = AllocateZeroPool(sizeof(ATA_DEVICE_INFO));
  if (AtaInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  HdpInfo = &AtaInfo->HdpInfo;
  HddSanDevEntry->DevInfo = AtaInfo;

  InitializeListHead (&HddSanDevEntry->Link);
  AtaInfo->Sign               = ATA_DEVICE_INFO_SIGN; 
  AtaInfo->AtaPassThru        = p->AtaPassThru;
  AtaInfo->Port               = Port;
  AtaInfo->PortMp             = PortMp;

  Status = AtaInfo->AtaPassThru->BuildDevicePath (AtaInfo->AtaPassThru, Port, PortMp, &DevicePathNode);
  DEBUG((EFI_D_INFO, "AtaInfo->AtaPassThru->BuildDevicePath: %r\n", Status));

  DevicePath = AppendDevicePathNode (SataControllerDevicePath, DevicePathNode);

  AtaInfo->DevicePath         = DevicePath;
  HdpInfo->Controller         = p->Controller;
  HdpInfo->ControllerDevPath  = DuplicateDevicePath(SataControllerDevicePath);
  //
  // Asb is aligned to IoAlign in allocation. Use FreeAlignedPages when Free
  //
  AtaInfo->Asb          = AllocateAlignedPages(EFI_SIZE_TO_PAGES (sizeof (EFI_ATA_STATUS_BLOCK)), p->AtaPassThru->Mode->IoAlign);
  ASSERT(AtaInfo->Asb != NULL);
  //
  // IdentifyData is aligned to IoAlign in allocation. Use FreeAlignedPages when Free
  //
  AtaInfo->IdentifyData = AllocateAlignedPages(EFI_SIZE_TO_PAGES (sizeof (ATA_IDENTIFY_DATA)), p->AtaPassThru->Mode->IoAlign);
  ASSERT(AtaInfo->IdentifyData != NULL);
  //
  // PayLoad is aligned to IoAlign in allocation. Use FreeAlignedPages when Free
  //
  AtaInfo->PayLoadSize  = 512;
  AtaInfo->PayLoad      = AllocateAlignedPages(EFI_SIZE_TO_PAGES (AtaInfo->PayLoadSize), p->AtaPassThru->Mode->IoAlign);
  ASSERT(AtaInfo->PayLoad != NULL);

  Status = GetAtaDeviceIdentifyData(AtaInfo);
  /**
   * Init ATA Device Sanitize supported Information.
  */
  AtaInfo->SanitizeSupported = (UINT8)((AtaInfo->IdentifyData->multi_sector_setting >> 12) & BIT0);
  DEBUG((EFI_D_INFO, "multi_sector_setting 0x%x AtaInfo->SanitizeSupported:0x%x\n", AtaInfo->IdentifyData->multi_sector_setting, AtaInfo->SanitizeSupported));
  /**
   * Init ATA Sanitize SanitizeCap (Word 59 BIT13 | BIT14 | BIT15)
  */
  AtaInfo->SanitizeCap = (UINT8)((AtaInfo->IdentifyData->multi_sector_setting >> 13) & (BIT0 | BIT1 | BIT2));
  DEBUG((EFI_D_INFO, "multi_sector_setting 0x%x AtaInfo->SanitizeCap:0x%x\n", AtaInfo->IdentifyData->multi_sector_setting, AtaInfo->SanitizeCap));

  Status = gBS->HandleProtocol (
                  p->Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID**)&PciIo
                  );
  ASSERT_EFI_ERROR (Status);
  Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0x0A, 1, &PciScc);
  ASSERT_EFI_ERROR (Status);

  if(PciScc == 0x01){         // IDE
    HdpInfo->HddIndex = Port * 2 + PortMp;
  } else {
    HdpInfo->HddIndex = Port;
  }

  Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0, 1, &PciId);
  ASSERT_EFI_ERROR (Status);
  AtaInfo->PciId = PciId;

  Status = PciIo->GetLocation (
                    PciIo,
                    &SegNum,
                    &BusNum,
                    &DevNum,
                    &FuncNum
                    );
  ASSERT_EFI_ERROR(Status);

  HdpInfo->Bus    = (UINT8)BusNum;
  HdpInfo->Dev    = (UINT8)DevNum;
  HdpInfo->Func   = (UINT8)FuncNum;
  HdpInfo->PciIo  = PciIo;
  HdpInfo->PciScc = PciScc;

  HddSecInfo = &HddSanDevEntry->HddSecInfo;
  GetAtaDevicePasswordSecurityStatus(AtaInfo->IdentifyData, HddSecInfo);


  CopyMem(ModelName, AtaInfo->IdentifyData->ModelName, 40);
  SwapWordArray((UINT8 *) ModelName, 40);
  ModelName[40] = 0;
  TrimStr8(ModelName);

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, (VOID **)&PlatHostInfo);
  
  if ((!EFI_ERROR(Status)) && (PlatHostInfo != NULL) && (PlatHostInfo->GetSataHostIndex != NULL )) {
    PlatSataHostIndex = PlatHostInfo->GetSataHostIndex(p->Controller);
    DEBUG((EFI_D_INFO, "PlatSataHostIndex: %d\n", PlatSataHostIndex));
  } else {
    PlatSataHostIndex = 0xFFFF;
  }

  if (PlatSataHostIndex == 0xFFFF || PlatSataHostIndex == 0x8000) {
    UnicodeSPrint(
      HddSanDevEntry->HddString,
      sizeof(HddSanDevEntry->HddString),
      L"SATA %d: %a",
      HdpInfo->HddIndex + IndexBase1,
      ModelName
      );
  } else {
    UnicodeSPrint(
      HddSanDevEntry->HddString,
      sizeof(HddSanDevEntry->HddString),
      L"SATA %d-%d: %a",
      PlatSataHostIndex+1,
      HdpInfo->HddIndex + IndexBase1,
      ModelName
      );
  }

  CopyMem(AtaInfo->SerialNo, AtaInfo->IdentifyData->SerialNo, sizeof(AtaInfo->SerialNo));

  HddSanDevEntry->TitleToken = HiiSetString(gPrivate->HiiHandle, 0, HddSanDevEntry->HddString, NULL);
  HddSanDevEntry->TitleHelpToken = (EFI_STRING_ID) STRING_TOKEN (STR_HDD_SANITIZE_HELP);

  InsertTailList (&mHddSanitizationCfgFormList, &HddSanDevEntry->Link);

  mNumberOfHddDevices++;

  DEBUG((EFI_D_INFO, "Port:%d ModelName:%a En:%d Lock:%d Frozen:%d Count:%d\n", \
    HdpInfo->HddIndex, ModelName, HddSecInfo->Enabled, HddSecInfo->Locked, \
    HddSecInfo->Frozen, mNumberOfHddDevices));

  return EFI_SUCCESS;
}

VOID
SataPassThruHookForHddSanitize (
  VOID  *Param
  )
{
  ATA_PASSTHRU_HOOK_PARAMETER   *p;
  UINT16                        Port;
  UINT16                        PortMp;
  EFI_STATUS                    Status;


  p = (ATA_PASSTHRU_HOOK_PARAMETER*)Param;
  ASSERT(p->Sign == ATA_PASSTHRU_HOOK_PARAMETER_SIGN);

  DEBUG((EFI_D_INFO, "SataMode:%X\n", p->SataMode));

  Port = 0xFFFF;
  while (TRUE) {
    Status = p->AtaPassThru->GetNextPort(p->AtaPassThru, &Port);
    if (EFI_ERROR(Status)) {
      break;
    }
    PortMp = 0xFFFF;
    while (TRUE) {
      Status = p->AtaPassThru->GetNextDevice(p->AtaPassThru, Port, &PortMp);
      if (EFI_ERROR (Status)) {
        break;
      }

      DEBUG((EFI_D_INFO, "AtaDev(%X,%X)\n", Port, PortMp));
      HandleSataDevice(p, Port, PortMp);
    }
  }
}

/**
 * @brief  Erase a Unencrypted ATA device with BlockIo->WriteBlocks()
 * 
 * @param  AtaInfo   A pointer to the SATA device 
*/
EFI_STATUS
SataDevBlockIoErase(
  ATA_DEVICE_INFO      *AtaInfo
)
{
  EFI_STATUS                 Status = EFI_SUCCESS;
  EFI_HANDLE                 DevHandle;
  EFI_BLOCK_IO_PROTOCOL      *BlockIo;
  UINT8                      *Buffer;
  UINTN                      BufferSize;                 
  UINTN                      Lba = 0;
  UINT64                     CurrentPercent;
  UINT64                     OldPercent;
  CHAR16                     *StrTitle   = NULL;
  CHAR16                     *StrSucceed = NULL;
  CHAR16                     *StrFailed  = NULL;

  DEBUG((EFI_D_INFO, "%a Enter\n", __FUNCTION__));
  Status = gBS->LocateDevicePath(&gEfiBlockIoProtocolGuid, &AtaInfo->DevicePath, &DevHandle);
  if (Status != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  Status = gBS->HandleProtocol(DevHandle, &gEfiBlockIoProtocolGuid, (VOID **) &BlockIo);
  if (Status != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  DEBUG((EFI_D_INFO, "AtaInfo->BlockIo->Media->MediaId:0x%x \n",BlockIo->Media->MediaId));
  DEBUG((EFI_D_INFO, "AtaInfo->BlockIo->Media->BlockSize:0x%x AtaInfo->BlockIo->Media->LastBlock:0x%lx\n",BlockIo->Media->BlockSize, BlockIo->Media->LastBlock));

  StrTitle   = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(STR_HDD_SANITIZE_PROMPT), NULL);
  StrSucceed = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(STR_HDD_SANITIZATION_SUCCESS), NULL);
  StrFailed  = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(STR_HDD_SANITIZATION_FAILED), NULL);

  BufferSize = BlockIo->Media->BlockSize * ERASE_BLOCKS_COUNT;//Write a size of 2048 blocks at a time
  Buffer     = AllocateZeroPool(BufferSize);
  if (Buffer == NULL) {
    return EFI_ABORTED;
  }

  if (PcdGetBool (PcdGuiEnable)) {
    ShowGuiProgress(HDD_ERASE_DRAW_TYPE_ENTRY, 0, gSanitizeTitleStr, NULL);
  } else {
    ShowTextProgress(HDD_ERASE_DRAW_TYPE_ENTRY, 0, gSanitizeTitleStr, NULL);
  }
  DEBUG((EFI_D_INFO, "Erase SATA Start:\n"));
  while (Lba <= BlockIo->Media->LastBlock) {
    //If the block will be exceeded the last time, change the buffersize
    if ((Lba + ERASE_BLOCKS_COUNT - 1) > BlockIo->Media->LastBlock) {
       BufferSize = (BlockIo->Media->LastBlock + 1 - Lba) * BlockIo->Media->BlockSize;
    }
    Status = BlockIo->WriteBlocks (
                        BlockIo,
                        BlockIo->Media->MediaId,
                        Lba,
                        BufferSize,
                        Buffer
                        );
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_INFO, "Write blocks Status:%r Current Lba:%x\n", Status, Lba));
      break;
    }
    //
    //Unencrypted ATA device uses Block write to erase, so the progress bar is calculated by the erase ratio.
    //
    OldPercent     = DivU64x32(MultU64x32(Lba, 100), (UINT32)BlockIo->Media->LastBlock);
    Lba           += ERASE_BLOCKS_COUNT;
    CurrentPercent = DivU64x32(MultU64x32(Lba, 100), (UINT32)BlockIo->Media->LastBlock);

    if (CurrentPercent > OldPercent) {
      if (PcdGetBool (PcdGuiEnable)) {
        ShowGuiProgress(HDD_ERASE_DRAW_TYPE_REFRESH, CurrentPercent, NULL, NULL);
      } else {
        ShowTextProgress(HDD_ERASE_DRAW_TYPE_REFRESH, CurrentPercent, NULL, NULL);
      }
    }
  }

  if (Status == EFI_SUCCESS) {
    gBS->Stall (1000000);
    if (PcdGetBool (PcdGuiEnable)) {
      ShowGuiProgress(HDD_ERASE_DRAW_TYPE_REFRESH, 100, NULL, NULL);
    } else {
      ShowTextProgress(HDD_ERASE_DRAW_TYPE_REFRESH, 100, NULL, NULL);
    }
    gBS->Stall (1000000);
    //
    //After Sanitize progress, Close the progress bar for avoiding overlapping popovers.
    //
    if (PcdGetBool (PcdGuiEnable)) {
      ShowGuiProgress(HDD_ERASE_DRAW_TYPE_EXIT, 100, NULL, NULL);
    } else {
      ShowTextProgress(HDD_ERASE_DRAW_TYPE_EXIT, 100, NULL, NULL);
    }
    UiConfirmDialogEx(DIALOG_ATTRIBUTE_RECOVER, DIALOG_INFO, StrTitle, NULL, TEXT_ALIGIN_CENTER, StrSucceed, NULL);
  } else {
    UiConfirmDialogEx(DIALOG_ATTRIBUTE_RECOVER, DIALOG_INFO, StrTitle, NULL, TEXT_ALIGIN_CENTER, StrFailed, NULL);
  }

  DEBUG((EFI_D_INFO, "End!\n"));

  FreePool(Buffer);
  FreePool(StrTitle);
  FreePool(StrSucceed);
  FreePool(StrFailed);

  return Status;
}

EFI_STATUS
SataDeviceSanitize (
  IN  ATA_DEVICE_INFO       *AtaInfo,
  IN  UINT8                 *SanitizeMethodType
  )
{
  DEBUG((EFI_D_INFO, "%a() Start.\n", __FUNCTION__));

  if (!AtaInfo->SanitizeSupported || !AtaInfo->SanitizeCap) {
    return EFI_UNSUPPORTED;
  }
  //
  //According NVM-Express-2.0a Spec,the three types Sanitize Erase Types are supported,
  //They are respectively Overwrite(BIT4), Block Erase(BIT2),Crypto Erase(BIT1).
  //So,need to check the Sanitize capabilities for Verifies that the type being executed is supported.
  //
  DEBUG((EFI_D_ERROR, "SanitizeMethodType:0x%x\n",*SanitizeMethodType));
  if (!(*SanitizeMethodType & AtaInfo->SanitizeCap)) {
    DEBUG((EFI_D_ERROR, "The currently selected  Sanitize method is not be Supported.\n"));
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}