/** @file
  HddPassword PEI module which is used to unlock HDD password for S3.

  Copyright (c) 2022, Byosoft Corporation. All rights reserved.<BR>

  This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.


**/

#include "HddPasswordPei.h"


EFI_STATUS
EFIAPI
VarLibGetVariable (
  IN     CHAR16                      *VariableName,
  IN     EFI_GUID                    *VendorGuid,
  OUT    UINT32                      *Attributes,    OPTIONAL
  IN OUT UINTN                       *DataSize,
  OUT    VOID                        *Data           OPTIONAL
  )
{
  EFI_STATUS                          Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI     *VariablePpi;

  //
  // Locate the variable PPI.
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariablePpi
             );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (VariablePpi != NULL) {
    Status = VariablePpi->GetVariable (
              VariablePpi,
              VariableName,
              VendorGuid,
              Attributes,
              DataSize,
              Data
              );
  } else {
    Status = EFI_UNSUPPORTED;
  }
  return Status;
}
/**
  Send unlock hdd password cmd through ATA PassThru PPI.

  @param[in] AtaPassThru           The pointer to the ATA PassThru PPI.
  @param[in] Port                  The port number of the ATA device.
  @param[in] PortMultiplierPort    The port multiplier port number of the ATA device.
  @param[in] Identifier            The identifier to set user or master password.
  @param[in] Password              The hdd password of attached ATA device.

  @retval EFI_SUCCESS              Successful to send unlock hdd password cmd.
  @retval EFI_INVALID_PARAMETER    The parameter passed-in is invalid.
  @retval EFI_OUT_OF_RESOURCES     Not enough memory to send unlock hdd password cmd.
  @retval EFI_DEVICE_ERROR         Can not send unlock hdd password cmd.

**/
EFI_STATUS
UnlockDevice (
  IN EDKII_PEI_ATA_PASS_THRU_PPI    *AtaPassThru,
  IN UINT16                         Port,
  IN UINT16                         PortMultiplierPort,
  IN CHAR8                          Identifier,
  IN CHAR8                          *Password
  )
{
  EFI_STATUS                          Status;
  EFI_ATA_COMMAND_BLOCK               Acb;
  EFI_ATA_STATUS_BLOCK                *Asb;
  EFI_ATA_PASS_THRU_COMMAND_PACKET    Packet;
  UINT8                               Buffer[ATA_SECURITY_BUFFER_LENGTH];

  if ((AtaPassThru == NULL) || (Password == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // The 'Asb' field (a pointer to the EFI_ATA_STATUS_BLOCK structure) in
  // EFI_ATA_PASS_THRU_COMMAND_PACKET is required to be aligned specified by
  // the 'IoAlign' field in the EFI_ATA_PASS_THRU_MODE structure. Meanwhile,
  // the structure EFI_ATA_STATUS_BLOCK is composed of only UINT8 fields, so it
  // may not be aligned when allocated on stack for some compilers. Hence, we
  // use the API AllocateAlignedPages to ensure this structure is properly
  // aligned.
  //
  Asb = AllocateAlignedPages (
          EFI_SIZE_TO_PAGES (sizeof (EFI_ATA_STATUS_BLOCK)),
          AtaPassThru->Mode->IoAlign
          );
  if (Asb == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Prepare for ATA command block.
  //
  ZeroMem (&Acb, sizeof (Acb));
  ZeroMem (Asb, sizeof (EFI_ATA_STATUS_BLOCK));
  Acb.AtaCommand    = ATA_CMD_SECURITY_UNLOCK;
  Acb.AtaDeviceHead = (UINT8) (PortMultiplierPort == 0xFFFF ? 0 : (PortMultiplierPort << 4));

  //
  // Prepare for ATA pass through packet.
  //
  ZeroMem (&Packet, sizeof (Packet));
  Packet.Protocol = EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_OUT;
  Packet.Length   = EFI_ATA_PASS_THRU_LENGTH_BYTES;
  Packet.Asb      = Asb;
  Packet.Acb      = &Acb;

  ((CHAR16 *) Buffer)[0] = Identifier & BIT0;
  CopyMem (&((CHAR16 *) Buffer)[1], Password, HDD_PASSWORD_MAX_LENGTH);

  Packet.OutDataBuffer     = Buffer;
  Packet.OutTransferLength = sizeof (Buffer);
  Packet.Timeout           = ATA_TIMEOUT;

  Status = AtaPassThru->PassThru (
                          AtaPassThru,
                          Port,
                          PortMultiplierPort,
                          &Packet
                          );
  if (!EFI_ERROR (Status) &&
      ((Asb->AtaStatus & ATA_STSREG_ERR) != 0) &&
      ((Asb->AtaError & ATA_ERRREG_ABRT) != 0)) {
    Status = EFI_DEVICE_ERROR;
  }

  FreeAlignedPages (Asb, EFI_SIZE_TO_PAGES (sizeof (EFI_ATA_STATUS_BLOCK)));

  ZeroMem (Buffer, sizeof (Buffer));

  DEBUG ((DEBUG_INFO, "%a() - %r\n", __FUNCTION__, Status));
  return Status;
}

/**
  Send security freeze lock cmd through ATA PassThru PPI.

  @param[in] AtaPassThru           The pointer to the ATA PassThru PPI.
  @param[in] Port                  The port number of the ATA device.
  @param[in] PortMultiplierPort    The port multiplier port number of the ATA device.

  @retval EFI_SUCCESS              Successful to send security freeze lock cmd.
  @retval EFI_INVALID_PARAMETER    The parameter passed-in is invalid.
  @retval EFI_OUT_OF_RESOURCES     Not enough memory to send unlock hdd password cmd.
  @retval EFI_DEVICE_ERROR         Can not send security freeze lock cmd.

**/
EFI_STATUS
FreezeLockDevice (
  IN EDKII_PEI_ATA_PASS_THRU_PPI    *AtaPassThru,
  IN UINT16                         Port,
  IN UINT16                         PortMultiplierPort
  )
{
  EFI_STATUS                          Status;
  EFI_ATA_COMMAND_BLOCK               Acb;
  EFI_ATA_STATUS_BLOCK                *Asb;
  EFI_ATA_PASS_THRU_COMMAND_PACKET    Packet;

  if (AtaPassThru == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // The 'Asb' field (a pointer to the EFI_ATA_STATUS_BLOCK structure) in
  // EFI_ATA_PASS_THRU_COMMAND_PACKET is required to be aligned specified by
  // the 'IoAlign' field in the EFI_ATA_PASS_THRU_MODE structure. Meanwhile,
  // the structure EFI_ATA_STATUS_BLOCK is composed of only UINT8 fields, so it
  // may not be aligned when allocated on stack for some compilers. Hence, we
  // use the API AllocateAlignedPages to ensure this structure is properly
  // aligned.
  //
  Asb = AllocateAlignedPages (
          EFI_SIZE_TO_PAGES (sizeof (EFI_ATA_STATUS_BLOCK)),
          AtaPassThru->Mode->IoAlign
          );
  if (Asb == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Prepare for ATA command block.
  //
  ZeroMem (&Acb, sizeof (Acb));
  ZeroMem (Asb, sizeof (EFI_ATA_STATUS_BLOCK));
  Acb.AtaCommand    = ATA_CMD_SECURITY_FREEZE_LOCK;
  Acb.AtaDeviceHead = (UINT8) (PortMultiplierPort == 0xFFFF ? 0 : (PortMultiplierPort << 4));

  //
  // Prepare for ATA pass through packet.
  //
  ZeroMem (&Packet, sizeof (Packet));
  Packet.Protocol = EFI_ATA_PASS_THRU_PROTOCOL_ATA_NON_DATA;
  Packet.Length   = EFI_ATA_PASS_THRU_LENGTH_NO_DATA_TRANSFER;
  Packet.Asb      = Asb;
  Packet.Acb      = &Acb;
  Packet.Timeout  = ATA_TIMEOUT;

  Status = AtaPassThru->PassThru (
                          AtaPassThru,
                          Port,
                          PortMultiplierPort,
                          &Packet
                          );
  if (!EFI_ERROR (Status) &&
      ((Asb->AtaStatus & ATA_STSREG_ERR) != 0) &&
      ((Asb->AtaError & ATA_ERRREG_ABRT) != 0)) {
    Status = EFI_DEVICE_ERROR;
  }

  FreeAlignedPages (Asb, EFI_SIZE_TO_PAGES (sizeof (EFI_ATA_STATUS_BLOCK)));

  DEBUG ((DEBUG_INFO, "%a() - %r\n", __FUNCTION__, Status));
  return Status;
}

/**
  Unlock HDD password for S3.

  @param[in] AtaPassThruPpi    Pointer to the EDKII_PEI_ATA_PASS_THRU_PPI instance.

**/
VOID
UnlockHddPassword (
  IN EDKII_PEI_ATA_PASS_THRU_PPI    *AtaPassThruPpi
  )
{
  EFI_STATUS                     Status;
  HDP_S3_DATA                    *HdpS3Data;
  UINTN                          VarDataSize;
  HDP_PORT_INFO                  *PortInfo;
  UINTN                          Index;

  //
  // Get BYO HddPassword Info from Variable
  //
  VarDataSize = 0;
  Status = VarLibGetVariable (L"HddUnlockInfo", &gEfiHddPasswordSecurityVariableGuid, NULL, &VarDataSize, NULL);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    DEBUG((DEBUG_INFO, "[%a] Fail to find Variable %s, it may not exist\n", __FUNCTION__, L"HddUnlockInfo"));
    return;
  }

  HdpS3Data = AllocateZeroPool(VarDataSize);
  if (HdpS3Data == NULL) {
    return;
  }

  Status = VarLibGetVariable (
              L"HddUnlockInfo",
              &gEfiHddPasswordSecurityVariableGuid,
              NULL,
              &VarDataSize,
              HdpS3Data
              );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_INFO, "[%a] Fail to get Variable %s\n",  __FUNCTION__, L"HddUnlockInfo"));
    goto Exit;
  }

  for (Index = 0; Index < HdpS3Data->EnCount; Index++) {
    PortInfo = &HdpS3Data->Port[Index];
    //
    // Unlock Hdd Only
    //
    if (PortInfo->Scc != 0x08) {
      DEBUG((DEBUG_INFO, "Unlock SATA Port:%x Mp:%x Identifier: %x\n", PortInfo->Port, PortInfo->PortMp, PortInfo->IsUser));
      DEBUG((EFI_D_INFO, "[%a] Sn: %a\n",  __FUNCTION__, PortInfo->Sn));
      DEBUG((EFI_D_INFO, "[%a] Password:", __FUNCTION__));
      DumpMem8(PortInfo->Password,HDD_PASSWORD_MAX_LENGTH);
      //
      // it means HDD device security feature is not enabled(Ownership is not taken), 
      // so in S3 path, device unlock is not required.
      //
      if (!IsZeroBuffer (PortInfo->Password, HDD_PASSWORD_MAX_LENGTH)) {
        UnlockDevice (AtaPassThruPpi, PortInfo->Port, PortInfo->PortMp, !PortInfo->IsUser, (CHAR8 *)PortInfo->Password);
      }
      //
      // Freeze lock the device.
      //
      FreezeLockDevice (AtaPassThruPpi, PortInfo->Port, PortInfo->PortMp);
    }
  }

Exit:
  ZeroMem (HdpS3Data, VarDataSize);
  FreePool(HdpS3Data);
}

/**
  Entry point of the notification callback function itself within the PEIM.
  It is to unlock HDD password for S3.

  @param  PeiServices      Indirect reference to the PEI Services Table.
  @param  NotifyDescriptor Address of the notification descriptor data structure.
  @param  Ppi              Address of the PPI that was installed.

  @return Status of the notification.
          The status code returned from this function is ignored.
**/
EFI_STATUS
EFIAPI
HddPasswordAtaPassThruNotify (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN VOID                      *Ppi
  )
{
  DEBUG ((DEBUG_INFO, "%a() - enter at S3 resume\n", __FUNCTION__));

  UnlockHddPassword ((EDKII_PEI_ATA_PASS_THRU_PPI *) Ppi);

  DEBUG ((DEBUG_INFO, "%a() - exit at S3 resume\n", __FUNCTION__));

  return EFI_SUCCESS;
}


EFI_PEI_NOTIFY_DESCRIPTOR mHddPasswordAtaPassThruPpiNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEdkiiPeiAtaPassThruPpiGuid,
  HddPasswordAtaPassThruNotify
};


/**
  Main entry for this module.

  @param FileHandle             Handle of the file being invoked.
  @param PeiServices            Pointer to PEI Services table.

  @return Status from PeiServicesNotifyPpi.

**/
EFI_STATUS
EFIAPI
HddPasswordPeiInit (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                              Status;
  EFI_BOOT_MODE                           BootMode;

  Status = PeiServicesGetBootMode (&BootMode);
  if ((EFI_ERROR (Status)) || (BootMode != BOOT_ON_S3_RESUME)) {
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "%a: SATA Enters in S3 path.\n", __FUNCTION__));

  Status = PeiServicesNotifyPpi (&mHddPasswordAtaPassThruPpiNotifyDesc);
  ASSERT_EFI_ERROR (Status);
  return Status;
}

