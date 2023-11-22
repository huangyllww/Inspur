/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  NvmeSanitization.c

Abstract:
  for Hdd Sanitization driver.

Revision History:

**/

#include "HddSanitizationDxe.h"
#include "HddSanitizationCommon.h"


UINTN                           gNvmeCount = 0;

//
//Identify NVMe controller Identifier data, check if ATA_SEC is supported.
//
EFI_STATUS  
AtaSecSupportCheck (
  IN  NVME_DEVICE_INFO  *NvmeInfo
  )
{
  EFI_STATUS                        Status;
  NVME_ADMIN_CONTROLLER_DATA        *ControllerData = NULL;
  UINT8                             *Identify3279;

  ControllerData = AllocateZeroPool(sizeof(NVME_ADMIN_CONTROLLER_DATA));
  if(ControllerData == NULL){
    Status = EFI_OUT_OF_RESOURCES;
    return Status;
  }
  NvmeInfo->AtaSecPayload = AllocateZeroPool(512);
  if(NvmeInfo->AtaSecPayload == NULL){
    Status = EFI_OUT_OF_RESOURCES;
    return Status;
  }  
  NvmeInfo->AtaSecPayloadSize = 512;
  //
  //For NVMe device, get NVMe Controller data structure and check ATA_SEC support
  //
  Status = NvmeGetIdentifyControllerData (NvmeInfo->NvmePassThru, ControllerData);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_INFO, "(L%d)\n", __LINE__));
    FreePool (ControllerData);
    return Status;
  }
  Identify3279 = (UINT8 *)ControllerData;
  if (!(Identify3279[3279] & BIT0)) {
    DEBUG((EFI_D_INFO, "AtaSec not support\n"));
    Status = EFI_UNSUPPORTED;
    FreePool (ControllerData);
    return Status;
  } else {
    Status = EFI_SUCCESS;
    FreePool (ControllerData);
    return Status;
  }
}

VOID 
InitOpalSession (
  OPAL_FEATURE_INFO   *OpalInfo
  ) 
{
  ZeroMem(&OpalInfo->Session, sizeof(OpalInfo->Session));
  OpalInfo->Session.Sscp          = OpalInfo->Sscp;
  OpalInfo->Session.MediaId       = OpalInfo->MediaId;
  OpalInfo->Session.OpalBaseComId = OpalInfo->OpalBaseComId;
}

//
//Update a Opal device's security status(Enable/Locked or Disable/Unlocked) to NVME_DEVICE_INFO.
//
EFI_STATUS
OpalDeviceUpdateStatus (
  VOID                   *DevInfo,
  HDD_SECURITY_INFO      *HddSecInfo
  )
{
  TCG_RESULT                  TcgResult;
  OPAL_FEATURE_INFO           *OpalInfo;

  DEBUG((DEBUG_INFO,"%a\n",__FUNCTION__));
  //
  // Get/Update Opal Device Info
  //
  OpalInfo = GetOpalInfo(DevInfo);
  InitOpalSession(OpalInfo);

  TcgResult = OpalGetLockingInfo(&OpalInfo->Session, &OpalInfo->LockingFeature);
  if (TcgResult != TcgResultSuccess) {
    return EFI_DEVICE_ERROR;
  }

  if (OpalFeatureSupported (&OpalInfo->SupportedAttributes)) {
    HddSecInfo->Supported = 0x01;
  }else{
    HddSecInfo->Supported = 0x00;
  }

  HddSecInfo->Enabled = OpalFeatureEnabled (
                       &OpalInfo->SupportedAttributes,
                       &OpalInfo->LockingFeature
                       );

  if (HddSecInfo->Enabled) {
    HddSecInfo->Locked = OpalDeviceLocked (
                         &OpalInfo->SupportedAttributes,
                         &OpalInfo->LockingFeature
                         );
  }

  HddSecInfo->Frozen = OpalInfo->SIDBlocked;

  return EFI_SUCCESS;
}

/*
  Get a ATA_SEC Device's security status for Identify Data.
*/
EFI_STATUS  
AtaSecCheckSecurityStatus (
  IN  NVME_DEVICE_INFO   *NvmeInfo,
  OUT ATA_IDENTIFY_DATA  *IdentifyData
  )
{
  UINTN                 TransferSize;
  EFI_STATUS            Status;
  NVME_ATA_SECU_STS     *AtaStatus;

  Status = NvmeInfo->Sscp->ReceiveData (
                             NvmeInfo->Sscp,
                             NvmeInfo->OpalInfo->MediaId,
                             EFI_TIMER_PERIOD_SECONDS(10),
                             TCG_SECURITY_PROTOCOL_ATA_DEVICE_SERVER_PASS,
                             0,
                             0x10,
                             NvmeInfo->AtaSecPayload,
                             &TransferSize
                             );
  if (EFI_ERROR(Status)) {
    goto ProcExit;
  }

  AtaStatus = (NVME_ATA_SECU_STS *)NvmeInfo->AtaSecPayload;

  ZeroMem(IdentifyData, sizeof(ATA_IDENTIFY_DATA));

  IdentifyData->command_set_supported_82   = (AtaStatus->Status & BIT0)?BIT1:0;
  IdentifyData->security_status            = AtaStatus->Status & (BIT1 | BIT2 | BIT3 | BIT4);
  IdentifyData->master_password_identifier = SwapBytes16(AtaStatus->MasterIdentify);

  CopyMem(IdentifyData->SerialNo, NvmeInfo->SerialNo, sizeof(IdentifyData->SerialNo));

  if (NvmeInfo->EraseUnitTimeOut != 0x0) {
    NvmeInfo->EraseUnitTimeOut = SwapBytes16(AtaStatus->EraseTime) * 60; //In Seconds
  } else {
    //
    //SECURITY ERASE TIME: Shall be 01h (<= 2 minutes) (Spec: Lenovo Purchase Spec-HDD Password via ATA Pass-through 1.1.1.2 & ACS-3)
    //
    DEBUG((DEBUG_INFO, "If the Security erase time is zero,force set the value be 1 minute according to Spec.\n"));
    NvmeInfo->EraseUnitTimeOut = 0x3C;
  }

ProcExit:
  return Status;
}

//
//Get a ATA_SEC devices' security status with IdentifyData, and sync them to NVME_DEVICE_INFO Structure.
//
EFI_STATUS
AtaSecDeviceUpdateStatus (
  IN NVME_DEVICE_INFO      *NvmeInfo,
  IN HDD_SECURITY_INFO     *HddSecInfo
  )
{
  EFI_STATUS                  Status;
  ATA_IDENTIFY_DATA           *IdentifyData;
  //
  // Get/Update ATA_SEC device Info
  //
  IdentifyData = AllocateZeroPool(sizeof(ATA_IDENTIFY_DATA));
  if(IdentifyData == NULL){
    Status = EFI_OUT_OF_RESOURCES;
    return Status;
  }

  Status = AtaSecCheckSecurityStatus(NvmeInfo,IdentifyData);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "AtaSecCheckSecurityStatus failed!\n"));
    FreePool (IdentifyData);
    return EFI_DEVICE_ERROR;
  }

  GetAtaDevicePasswordSecurityStatus(IdentifyData, HddSecInfo);

  FreePool (IdentifyData);
  return Status;
}

EFI_STATUS
InitNvmeInfo (
  NVME_DEVICE_INFO           *NvmeInfo,
  NVME_SSCP_HOOK_PARAMETER   *p
)
{
  EFI_STATUS                       Status = EFI_SUCCESS;
  TCG_RESULT                       TcgResult;
  NVME_ADMIN_CONTROLLER_DATA       *ControllerData = NULL;
  UINT8                            SanitizeCapabilites[4] = {0x0};

  DEBUG((DEBUG_INFO,"%a\n",__FUNCTION__));
  NvmeInfo->IsAtaSec          = FALSE;
  NvmeInfo->Sign              = NVME_DEVICE_INFO_SIGN;
  NvmeInfo->NvmePassThru      = p->Passthru;
  NvmeInfo->Sscp              = p->Sscp;
  NvmeInfo->OpalInfo->Sscp    = p->Sscp;
  NvmeInfo->OpalInfo->MediaId = p->BlockIo->Media->MediaId;
  NvmeInfo->NamespaceId       = p->NamespaceId;
  NvmeInfo->BlockIo           = p->BlockIo;
  CopyMem(NvmeInfo->SerialNo, p->Sn, sizeof(p->Sn));
  CopyMem(NvmeInfo->Mn, p->Mn, sizeof(p->Mn));

  ZeroMem(&NvmeInfo->OpalInfo->Session, sizeof(NvmeInfo->OpalInfo->Session));
  NvmeInfo->OpalInfo->Session.Sscp    = NvmeInfo->Sscp;
  NvmeInfo->OpalInfo->Session.MediaId = NvmeInfo->OpalInfo->MediaId;

  ControllerData = AllocateZeroPool(sizeof(NVME_ADMIN_CONTROLLER_DATA));
  if (ControllerData == NULL) {
    DEBUG((DEBUG_INFO, "Allocate Error!"));
    ASSERT(FALSE);
  }
  /**
   * According to NVM Express Base Specification,
   * Read the Sanitize Operation supporting information which is in the low three bits of Sanitize Capabilites field.
   *
  */
  Status = NvmeGetIdentifyControllerData (NvmeInfo->NvmePassThru, ControllerData);
  if (Status == EFI_SUCCESS) {
    //
    //Init NVMe Sanitize Capabilites from the Bytes 328 in Identify Controller Data field.
    //
    SanitizeCapabilites[0] = ((UINT8 *)ControllerData)[328];
    NvmeInfo->SanitizeCap  = SanitizeCapabilites[0] & (BIT0 | BIT1 | BIT2);
    DEBUG((DEBUG_INFO, "NvmeInfo->SanitizeCap:0x%x\n", NvmeInfo->SanitizeCap));
    //
    //Init NVMe Format Attributes form the  Bytes 524 in Identify Controller Data field.
    //
    NvmeInfo->FormatAttributes = ControllerData->Fna;
    DEBUG((DEBUG_INFO, "NvmeInfo->FormatAttributes:0x%x\n", NvmeInfo->FormatAttributes));
  }

  //
  // First check if device support Opal, then check ATA_SEC support 
  //
  TcgResult = OpalGetSupportedAttributesInfo (
                &NvmeInfo->OpalInfo->Session,
                &NvmeInfo->OpalInfo->SupportedAttributes,
                &NvmeInfo->OpalInfo->OpalBaseComId
                );
  if (TcgResult != TcgResultSuccess) {
    //
    // check ATA_SEC support
    //
    Status = AtaSecSupportCheck (NvmeInfo);
    if (!EFI_ERROR (Status)) {
      NvmeInfo->IsAtaSec = TRUE;
    }
  }

  FreePool(ControllerData);
  ControllerData = NULL;

  return Status;
}

VOID
NvmeSscpHookForHddSanitize (
  VOID  *Param
  )
{
  NVME_SSCP_HOOK_PARAMETER         *p;
  EFI_STATUS                       Status;
  LIST_ENTRY                       *Entry;
  HDD_SANITIZE_DEVICE_ENTRY        *HddSanDevEntry;
  BOOLEAN                          EntryExisted;
  HDD_PASSWORD_INFO                *HdpInfo;
  NVME_DEVICE_INFO                 *NvmeInfo;
  UINTN                            Index;
  UINTN                            SegNum;
  UINTN                            BusNum;
  UINTN                            DevNum;
  UINTN                            FuncNum;
  HDD_SECURITY_INFO                *HddSecInfo;
  EFI_DEVICE_PATH_PROTOCOL         *NvmeControllerDevicePath;

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  p = (NVME_SSCP_HOOK_PARAMETER*)Param;

  ASSERT(p->Sign == NVME_SSCP_HOOK_PARAMETER_SIGN);

  Status = gBS->OpenProtocol (
                  p->Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID**) &NvmeControllerDevicePath,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR(Status)) {
    NvmeControllerDevicePath = NULL;
  }

  EntryExisted = FALSE;
  EFI_LIST_FOR_EACH (Entry, &mHddSanitizationCfgFormList) {
    HddSanDevEntry = BASE_CR (Entry, HDD_SANITIZE_DEVICE_ENTRY, Link);
    //
    // When reconnecting locked storage device, NvmeSscpHook should only check NVMe device
    //
    if ((*(UINT32 *)HddSanDevEntry->DevInfo) != NVME_DEVICE_INFO_SIGN) {
      continue;
    }
    NvmeInfo = (NVME_DEVICE_INFO *)HddSanDevEntry->DevInfo;
    HdpInfo  = &NvmeInfo->HdpInfo;

    if (NvmeControllerDevicePath   != NULL && 
        HdpInfo->ControllerDevPath != NULL &&
        UefiCompareDevicePath(NvmeControllerDevicePath, HdpInfo->ControllerDevPath)) {
      //
      // If Control device path matches again. It is caused by driver reconnection. Refresh NvmeInfo
      //
      EntryExisted           = TRUE;
      NvmeInfo->NvmePassThru = p->Passthru;
      NvmeInfo->Sscp         = p->Sscp;
      HdpInfo->Controller    = p->Controller;
      break;
    }
  }

  if (EntryExisted) {
    DEBUG((EFI_D_INFO, "%a exist\n", __FUNCTION__));
    goto ProcExit;
  }

  NvmeInfo = AllocateZeroPool(sizeof(NVME_DEVICE_INFO));
  if (NvmeInfo == NULL) {
    DEBUG((EFI_D_ERROR, "(L%d) malloc error\n", __LINE__));
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }

  NvmeInfo->OpalInfo = AllocateZeroPool(sizeof(OPAL_FEATURE_INFO));
  if (NvmeInfo->OpalInfo == NULL) {
    DEBUG((EFI_D_ERROR, "(L%d) malloc error\n", __LINE__));
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }

  Status  = InitNvmeInfo(NvmeInfo, p);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "(L%d) InitNvmeInfo error\n", __LINE__));
    goto ProcExit;
  }

  HddSanDevEntry = AllocateZeroPool(sizeof(HDD_SANITIZE_DEVICE_ENTRY));
  ASSERT(HddSanDevEntry != NULL);
  if (HddSanDevEntry == NULL) {
    DEBUG((EFI_D_ERROR, "(L%d) malloc error\n", __LINE__));
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }

  HdpInfo                    = &NvmeInfo->HdpInfo;
  HddSanDevEntry->DevInfo    = NvmeInfo;
  HddSecInfo                 = &HddSanDevEntry->HddSecInfo;
  HdpInfo->Controller        = p->Controller;
  HdpInfo->ControllerDevPath = DuplicateDevicePath(NvmeControllerDevicePath);

  InitializeListHead(&HddSanDevEntry->Link);
  Index = gNvmeCount++;

  HdpInfo->HddIndex = 0x80 + (UINT8)Index;

  if (NvmeInfo->IsAtaSec) {
    Status = AtaSecDeviceUpdateStatus(NvmeInfo, HddSecInfo);
  } else {
    Status = OpalDeviceUpdateStatus(NvmeInfo, HddSecInfo);
  }

  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "%a (L%d) %r\n", __FUNCTION__, __LINE__, Status));
    Status = EFI_NOT_READY;
    goto ProcExit;
  }

  HddSecInfo->SanitizeCap   = NvmeInfo->SanitizeCap; 
  
  Status = p->PciIo->GetLocation (
                       p->PciIo,
                       &SegNum,
                       &BusNum,
                       &DevNum,
                       &FuncNum
                       );
  ASSERT_EFI_ERROR(Status);
  DEBUG((EFI_D_INFO, "(%X,%X,%X)\n", BusNum, DevNum, FuncNum));

  HdpInfo->Bus    = (UINT8)BusNum;
  HdpInfo->Dev    = (UINT8)DevNum;
  HdpInfo->Func   = (UINT8)FuncNum;
  HdpInfo->PciIo  = p->PciIo;
  HdpInfo->PciScc = 0x08;
  TrimStr8((CHAR8 *) NvmeInfo->Mn);
  //
  //Print NVMe device Info(Bus Device Function  Protocol:ATA_SEC/TCG_OPAL) in Setup page.
  //
  if (NvmeInfo->IsAtaSec) {
    UnicodeSPrint(
      HddSanDevEntry->HddString,
      sizeof(HddSanDevEntry->HddString),
      L"NVME(PCI%x-%x-%x): %a ",
      BusNum,
      DevNum,
      FuncNum,
      NvmeInfo->Mn
      );
  } else {
    UnicodeSPrint(
      HddSanDevEntry->HddString,
      sizeof(HddSanDevEntry->HddString),
      L"NVME(PCI%x-%x-%x): %a ",
      BusNum,
      DevNum,
      FuncNum,
      NvmeInfo->Mn
      );
  }

  HddSanDevEntry->TitleToken     = HiiSetString(gPrivate->HiiHandle, 0, HddSanDevEntry->HddString, NULL);
  HddSanDevEntry->TitleHelpToken = (EFI_STRING_ID) STRING_TOKEN (STR_HDD_SANITIZE_HELP);
  InsertTailList (&mHddSanitizationCfgFormList, &HddSanDevEntry->Link);
  mNumberOfHddDevices++;

  DEBUG((EFI_D_INFO, "Port:%X HddString:%s En:%d Lock:%d Frozen:%d Count:%d\n", \
    HdpInfo->HddIndex, HddSanDevEntry->HddString, HddSecInfo->Enabled, HddSecInfo->Locked, \
    HddSecInfo->Frozen, mNumberOfHddDevices));

ProcExit:
  return;
}

/**
 * @brief Get NVMe Controller Identify data with opcode NVME_ADMIN_IDENTIFY_CMD
 *        These operations refer to NVM-Express-2.0a Spec.
 * 
 * @param[in] NvmePassThru                A pointer to the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL instance.
 * @param[in] Buffer                      A pointer to the Controller Data Buffer.
 * 
*/
EFI_STATUS
NvmeGetIdentifyControllerData (
  IN     EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL *NvmePassThru,
  IN OUT VOID                               *Buffer
  )
{
  EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET CommandPacket;
  EFI_NVM_EXPRESS_COMMAND                  Command;
  EFI_NVM_EXPRESS_COMPLETION               Completion;
  EFI_STATUS                               Status;

  DEBUG((EFI_D_INFO, "%a() Start.\n", __FUNCTION__));
  ZeroMem (&CommandPacket, sizeof(EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET));
  ZeroMem (&Command, sizeof(EFI_NVM_EXPRESS_COMMAND));
  ZeroMem (&Completion, sizeof(EFI_NVM_EXPRESS_COMPLETION));

  Command.Cdw0.Opcode          = NVME_ADMIN_IDENTIFY_CMD;
  //
  // According to Nvm Express 1.1 spec Figure 38, When not used, the field shall be cleared to 0h.
  // For the Identify command, the Namespace Identifier is only used for the Namespace data structure.
  //
  Command.Nsid                 = 0;
  CommandPacket.NvmeCmd        = &Command;
  CommandPacket.NvmeCompletion = &Completion;
  CommandPacket.TransferBuffer = Buffer;
  CommandPacket.TransferLength = sizeof(NVME_ADMIN_CONTROLLER_DATA);
  CommandPacket.CommandTimeout = EFI_TIMER_PERIOD_SECONDS(5);
  CommandPacket.QueueType      = NVME_ADMIN_QUEUE;
  //
  // Set bit 0 (Cns bit) to 1 to identify a controller
  //
  Command.Cdw10                = 1;
  Command.Flags                = CDW10_VALID;

  Status = NvmePassThru->PassThru (
                           NvmePassThru,
                           0,
                           &CommandPacket,
                           NULL
                           );

  return Status;
}

/**
 * @brief Get the status associated with the most recent sanitize operation about NVMe Device.
 *        These operations refer to NVM-Express-2.0a Spec.
 * 
 * @param[in] NvmePassThru                A pointer to the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL instance.
 * @param[in] GetLogCmd           A pointer to the Get Log Page command.
 * @param[in] SanitizeSatus               A pointer to the Sanitize Satus.
 * 
*/
EFI_STATUS
NvmeGetSanitizeStatus(
  IN   EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL *NvmePassThru,
  IN   NVME_GET_SINITIZE_LOG_CMD          *GetLogCmd,
  OUT  UINTN                              *SanitizeSatus
  )
{
  EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET  CommandPacket;
  EFI_NVM_EXPRESS_COMMAND                   Command;
  EFI_NVM_EXPRESS_COMPLETION                Completion;
  EFI_STATUS                                Status;
	UINT32                                    Ndw;
  UINTN                                     LogLength = 0x200;
  UINTN                                     *ReadBuffer;
  UINTN                                     SanitizePercent;
  UINTN                                     SanitizeProgress;
  UINT64                                    Remainder;
  
  DEBUG((EFI_D_INFO, "%a() Start.\n", __FUNCTION__));
  ReadBuffer = AllocateZeroPool(LogLength);
  if (ReadBuffer == NULL) {
    DEBUG((EFI_D_ERROR, "(L%d) malloc error\n", __LINE__));
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (&CommandPacket, sizeof(EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET));
  ZeroMem (&Command, sizeof(EFI_NVM_EXPRESS_COMMAND));
  ZeroMem (&Completion, sizeof(EFI_NVM_EXPRESS_COMPLETION));
  Ndw = (0x200 >> 2) - 1;

  Command.Cdw0.Opcode          = NVME_ADMIN_GET_LOG_PAGE_CMD;
  Command.Nsid                 = NVME_CONTROLLER_NSID;
  CommandPacket.NvmeCmd        = &Command;
  CommandPacket.NvmeCompletion = &Completion;
  CommandPacket.TransferBuffer = ReadBuffer;
  CommandPacket.TransferLength = 512;
  CommandPacket.CommandTimeout = EFI_TIMER_PERIOD_SECONDS(5);
  CommandPacket.QueueType      = NVME_ADMIN_QUEUE;

  /**
   * Bits 31:16 This field specifies the least significant 16 bits of the number of dwords to return unless otherwise specified.
   * Bits 15    This bit specifies when to retain or clear an Asynchronous event.
   * Bits 14:08 Log Specific Field:If not defined for the log specified by the Log Page Identifier field, this  field is reserved.
   * 07:00      This field specifies the identifier of the log page to retrieve.
   * 
  */
  Command.Cdw10                = (Ndw & 0xffff) << 16           |
				                         ((GetLogCmd->Rae) ? 1 : 0) << 15 |
				                         GetLogCmd->Lsp << 8              |
				                         (GetLogCmd->Lid & 0xff);

  /**
   * Bits 31:16 This field specifies an identifier that is required for a particular log page
   * 
   * Bits 15:00 : This field specifies the most significant 16 bits of the number of dwords to return unless otherwise specified.
  */
  Command.Cdw11                = (GetLogCmd->Lsi << 16 | Ndw >> 16);

  /**
   * Bits 31:00 Log Page Offset Lower,
   * The log page offset specifies the location within a log page to  start returning data from unless otherwise specified.
  */
  Command.Cdw12                = GetLogCmd->Lpo & 0xffffffff;

  /**
   * Bits 31:00 Log Page Offset Upper (LPOU): 
   * This field specifies the most significant 32 bits of either the log page offset or 
   * the index into the list of data structures unless otherwise specified. Refer to the Log Page Offset Lower definition.
  */
  Command.Cdw13                = GetLogCmd->Lpo >> 32;

  /**
   * Bits 31:24 Command Set Identifier
   * Bits 23    This bit specifies the Log Page Offset which will be returned.
   * Bits 22:07 Reserved 
   * Bits 06:00 UUID Index
   * 
  */
  Command.Cdw14                = GetLogCmd->Csi << 24 | (GetLogCmd->Ot ? 1 : 0) << 23 | GetLogCmd->Uuidx;

  Command.Flags                = CDW10_VALID | CDW11_VALID | CDW12_VALID | CDW13_VALID | CDW14_VALID;

  Status = NvmePassThru->PassThru (
                           NvmePassThru,
                           0,
                           &CommandPacket,
                           NULL
                           );

  /**
   * The progress bar is refreshed dynamically according to the Sanitize operation status information.
  */
  DumpMem8(ReadBuffer, LogLength);
  *SanitizeSatus  = (*ReadBuffer >> 0x10) & 0x7;
  DEBUG((EFI_D_ERROR, "%a SanitizeSatus:0x%x\n", __FUNCTION__, SanitizeSatus));
  //The value is a numerator of the fraction complete that has 65,536 (10000h) as its denominator defined in Nvm Express 2.0 spec.
  SanitizeProgress = *ReadBuffer & 0xFFFF;
  DEBUG((EFI_D_ERROR, "%a SanitizeProgress:0x%x\n", __FUNCTION__, SanitizeProgress));
  if (SanitizeProgress != 0xFFFF) {
    SanitizePercent  = DivU64x64Remainder(MultU64x32(SanitizeProgress, 100), 0x10000, &Remainder);
    DEBUG((EFI_D_ERROR, "%a SanitizePercent:0x%x\n", __FUNCTION__, SanitizePercent));
    if (PcdGetBool (PcdGuiEnable)) {
      ShowGuiProgress(HDD_ERASE_DRAW_TYPE_REFRESH, SanitizePercent, NULL, NULL);
    } else {
      ShowTextProgress(HDD_ERASE_DRAW_TYPE_REFRESH, SanitizePercent, NULL, NULL);
    }
  }

  return Status;
}

/**
 * @brief start a sanitize operation via Sanitize command.
 *       All sanitize operations (i.e., Block Erase, Crypto Erase, and Overwrite) are performed in the background 
 *       (i.e., Sanitize command completion does not indicate sanitize operation completion)
 * 
 * @param[in] NvmePassThru                A pointer to the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL instance.
 * @param[in] SanitizeCmd        A pointer to the Get Log Page command.
 * 
*/
EFI_STATUS
NvmeStartSanitize (
  IN  EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL *NvmePassThru,
  IN  NVME_SANITIZE_OPERATION_CMD        *SanitizeCmd
)
{
  EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET CommandPacket;
  EFI_NVM_EXPRESS_COMMAND                  Command;
  EFI_NVM_EXPRESS_COMPLETION               Completion;
  EFI_STATUS                               Status;

  DEBUG((EFI_D_INFO, "%a() Start.\n", __FUNCTION__));

  ZeroMem (&CommandPacket, sizeof(EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET));
  ZeroMem (&Command, sizeof(EFI_NVM_EXPRESS_COMMAND));
  ZeroMem (&Completion, sizeof(EFI_NVM_EXPRESS_COMPLETION));
  
  Command.Cdw0.Opcode          = NVME_ADMIN_SANITIZE_CMD;

  //
  // According to Nvm Express 2.0 spec Figure 303, 
  //The Sanitize command uses Command Dword 10 and Command Dword 11.  All other command specific  fields are reserved.
  // 
  Command.Nsid                 = NVME_CONTROLLER_NSID; 
  CommandPacket.NvmeCmd        = &Command;
  CommandPacket.NvmeCompletion = &Completion;
  CommandPacket.TransferBuffer = 0;
  CommandPacket.TransferLength = 0;
  CommandPacket.CommandTimeout = EFI_TIMER_PERIOD_SECONDS(5);
  CommandPacket.QueueType      = NVME_ADMIN_QUEUE;

  /**
   * Bit 31:10  Reserved
   * Bit 09 determines whether the controller should deallocate user data as a result of successfully completing the sanitize operation.     
   * Bit 08 determines whether the Overwrite Invert Pattern Between Passes.
   * Bit 07:04 determines indicate the Overwrite Overwrite Pass Count.
   * Bit 03 determines whether the Sanitize operation is performed in unrestricted or restricted completion mode.   
   * Bit 02:01 specifies the sanitize action to perform:
   *                                                     000b Reserved
                                                         001b Exit Failure Mode
                                                         010b Start a Block Erase sanitize operation
                                                         011b Start an Overwrite sanitize operation
                                                         100b Start a Crypto Erase sanitize operation
                                                         101b to 111b Reserved
  */
  Command.Cdw10                = ((SanitizeCmd->Nodas ? 1 : 0) << 9)   
				                          | ((SanitizeCmd->Oipbp ? 1 : 0) << 8)
				                          | ((SanitizeCmd->Owpass & 0xf) << 4) 
				                          | ((SanitizeCmd->Ause ? 1 : 0) << 3) 
				                          | ((SanitizeCmd->Sanact & 0x7) << 0);

  //
  //This field specifies a 32-bit pattern that is used for the Overwrite sanitize operation.
  //
  Command.Cdw11                = SanitizeCmd->Ovrpat;

  Command.Flags                = CDW10_VALID | CDW11_VALID;

  Status = NvmePassThru->PassThru (
                           NvmePassThru,
                           NVME_CONTROLLER_NSID,
                           &CommandPacket,
                           NULL
                           );

  DEBUG((DEBUG_INFO, " %a Sanitize Status: %r", __FUNCTION__, Status));

  return Status;
}

/**
 * @brief start a sanitize operation via Sanitize command.
 *        1.All sanitize operations (i.e., Block Erase, Crypto Erase, and Overwrite) are performed in the background 
 *       (i.e., Sanitize command completion does not indicate sanitize operation completion)
 *        2.So, need to get log page for tracking progress about Sanitize operation.
 * 
 * @param[in] NvmePassThru                A pointer to the EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL instance.
 * @param[in] SanitizeMethodType          A pointer to the Sanitize Operation.
 * 
*/
EFI_STATUS
NvmeDeviceSanitize (
  IN  NVME_DEVICE_INFO     *NvmeInfo,
  IN  UINT8                *SanitizeMethodType
  )
{
  EFI_STATUS                       Status = EFI_SUCCESS;
  CHAR16                           *StrTitle;
  CHAR16                           *StrSucceed;
  CHAR16                           *StrFailed;
  NVME_SANITIZE_OPERATION_CMD      SanitizeCmd;
  NVME_GET_SINITIZE_LOG_CMD        GetLogCmd;
  UINTN                            *SanitizeSatus = NULL;
  
  DEBUG((EFI_D_INFO, "%a() Start.\n", __FUNCTION__));
  if (!NvmeInfo->SanitizeCap) {
    return EFI_UNSUPPORTED;
  }
  //
  //According NVM-Express-2.0a Spec,the three types Sanitize Erase Types are supported,
  //They are respectively Overwrite(BIT4), Block Erase(BIT2),Crypto Erase(BIT1).
  //So,need to check the Sanitize capabilities for Verifies that the type being executed is supported.
  //
  DEBUG((EFI_D_ERROR, "SanitizeMethodType:0x%x\n",*SanitizeMethodType));
  if (!(*SanitizeMethodType & NvmeInfo->SanitizeCap)) {
    DEBUG((EFI_D_ERROR, "The currently selected  Sanitize method is not be Supported.\n"));
    return EFI_UNSUPPORTED;
  }

  /**
   * 1. Init Sanitize Command 
  */

  // Sanitize Operation Command
  SanitizeCmd.Ause        = FALSE;
  SanitizeCmd.Nodas       = FALSE;
  SanitizeCmd.Oipbp       = FALSE;
  SanitizeCmd.Ovrpat      = FALSE;

  if (*SanitizeMethodType == 0x1) {
    SanitizeCmd.Sanact      = NvmeSanitizeCryptoErase;
  } else if (*SanitizeMethodType == 0x2) {
    SanitizeCmd.Sanact      = NvmeSanitizeBlockErase;
  } else {
    SanitizeCmd.Sanact      = NvmeSanitizeOverwrite;
  }

  // Get log sanitize log page command.
  GetLogCmd.Csi   = NvmeCsiNVM;
  GetLogCmd.Ot    = FALSE;
  GetLogCmd.Uuidx = 0x0;
  GetLogCmd.Lsi   = 0x0;
  GetLogCmd.Lid   = NVME_LOG_LID_SANITIZE;
  GetLogCmd.Lsp   = 0x0;
  GetLogCmd.Rae   = FALSE;
  GetLogCmd.Lpo   = 0x0;

  /**
   * 2. Start a NVMe Sanitize
  */
  if (PcdGetBool (PcdGuiEnable)) {
    ShowGuiProgress(HDD_ERASE_DRAW_TYPE_ENTRY, 0, gSanitizeTitleStr, NULL);
  } else {
    ShowTextProgress(HDD_ERASE_DRAW_TYPE_ENTRY, 0, gSanitizeTitleStr, NULL);
  }

  Status = NvmeStartSanitize(NvmeInfo->NvmePassThru, &SanitizeCmd);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  /**
   * 3. Query the progress of Hdd Sanitization operation via the Sanitize Status Log, and show it.
  */
  StrTitle   = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(STR_HDD_SANITIZE_PROMPT), NULL);
  StrSucceed = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(STR_HDD_SANITIZATION_SUCCESS), NULL);
  StrFailed  = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(STR_HDD_SANITIZATION_FAILED), NULL);

  *SanitizeSatus = 0xFF;
  while (*SanitizeSatus != Sanitizecompletion) {
    gBS->Stall (1000000);
    Status = NvmeGetSanitizeStatus(NvmeInfo->NvmePassThru, &GetLogCmd, SanitizeSatus);
    DEBUG((EFI_D_INFO, "SanitizeSatus : 0x%x\n", *SanitizeSatus));
    //
    //The most recent sanitize operation completed successfully including any additional media modification.
    //
    if (*SanitizeSatus == Sanitizecompletion) {
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
      DEBUG((EFI_D_INFO, "The most recent sanitize operation completed successfully including any additional media modification.\n"));
    } else if (*SanitizeSatus == SanitizeFailed) {
      UiConfirmDialogEx(DIALOG_ATTRIBUTE_RECOVER, DIALOG_INFO, StrTitle, NULL, TEXT_ALIGIN_CENTER, StrFailed, NULL);
      DEBUG((EFI_D_INFO, "The most recent sanitize operation failed."));
    }
  }

  FreePool(StrTitle);
  FreePool(StrSucceed);
  FreePool(StrFailed);
  return Status;
}

/**
 * @brief  Erase a Unencrypted NVMe device with BlockIo->WriteBlocks()
 * 
 * @param  BlkIo   A pointer to the EFI_BLOCK_IO_PROTOCOL instance
*/
EFI_STATUS
NvmeBlockIoErase (
  EFI_BLOCK_IO_PROTOCOL   *BlkIo
  )
{
  EFI_STATUS             Status = EFI_SUCCESS;
  UINT8                  *Buffer;
  UINTN                  BufferSize;                 
  UINTN                  Lba = 0;
  UINT64                 CurrentPercent;
  UINT64                 OldPercent;
  CHAR16                 *StrTitle   = NULL;
  CHAR16                 *StrSucceed = NULL;
  CHAR16                 *StrFailed  = NULL;

  DEBUG((EFI_D_INFO, "BlkIo->Media->MediaId:0x%x \n",BlkIo->Media->MediaId));
  DEBUG((EFI_D_INFO, "BlkIo->BlockIo->Media->BlockSize:0x%x BlkIo->BlockIo->Media->LastBlock:0x%lx\n",BlkIo->Media->BlockSize, BlkIo->Media->LastBlock));

  StrTitle   = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(STR_HDD_SANITIZE_PROMPT), NULL);
  StrSucceed = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(STR_HDD_SANITIZATION_SUCCESS), NULL);
  StrFailed  = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(STR_HDD_SANITIZATION_FAILED), NULL);

  BufferSize = BlkIo->Media->BlockSize * ERASE_BLOCKS_COUNT;//Write a size of 2048 blocks at a time
  Buffer     = AllocateZeroPool(BufferSize);
  if (Buffer == NULL) {
    return EFI_ABORTED;
  }

  DEBUG((EFI_D_INFO, "%a Start:\n", __FUNCTION__));
  if (PcdGetBool (PcdGuiEnable)) {
    ShowGuiProgress(HDD_ERASE_DRAW_TYPE_ENTRY, 0, gSanitizeTitleStr, NULL);
  } else {
    ShowTextProgress(HDD_ERASE_DRAW_TYPE_ENTRY, 0, gSanitizeTitleStr, NULL);
  }

  while(Lba <= BlkIo->Media->LastBlock){
    //If the block will be exceeded the last time, change the buffersize
    if ((Lba + ERASE_BLOCKS_COUNT - 1) > BlkIo->Media->LastBlock) {
      BufferSize = (BlkIo->Media->LastBlock + 1 - Lba) * BlkIo->Media->BlockSize;
    }
    Status = BlkIo->WriteBlocks (
                     BlkIo,
                     BlkIo->Media->MediaId,
                     Lba,
                     BufferSize,
                     Buffer
                     );
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_INFO, "Write blocks Status:%r Current Lba:%x\n", Status, Lba));
      break;
    }

    OldPercent     = DivU64x32(MultU64x32(Lba, 100), (UINT32)BlkIo->Media->LastBlock);
    Lba           += ERASE_BLOCKS_COUNT;
    CurrentPercent = DivU64x32(MultU64x32(Lba, 100), (UINT32)BlkIo->Media->LastBlock);
    //
    //Refresh HDD erase percentage in every 1 seconds.
    //
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