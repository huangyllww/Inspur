/** @file
  Opal Password PEI driver which is used to unlock Opal Password for S3.

Copyright (c) 2022, Byosoft Corporation. All rights reserved.<BR>

This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.


**/

#include "OpalPasswordPei.h"

EFI_GUID mOpalDeviceLockBoxGuid = OPAL_DEVICE_LOCKBOX_GUID;

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
  Send a security protocol command to a device that receives data and/or the result
  of one or more commands sent by SendData.

  The ReceiveData function sends a security protocol command to the given MediaId.
  The security protocol command sent is defined by SecurityProtocolId and contains
  the security protocol specific data SecurityProtocolSpecificData. The function
  returns the data from the security protocol command in PayloadBuffer.

  For devices supporting the SCSI command set, the security protocol command is sent
  using the SECURITY PROTOCOL IN command defined in SPC-4.

  For devices supporting the ATA command set, the security protocol command is sent
  using one of the TRUSTED RECEIVE commands defined in ATA8-ACS if PayloadBufferSize
  is non-zero.

  If the PayloadBufferSize is zero, the security protocol command is sent using the
  Trusted Non-Data command defined in ATA8-ACS.

  If PayloadBufferSize is too small to store the available data from the security
  protocol command, the function shall copy PayloadBufferSize bytes into the
  PayloadBuffer and return EFI_WARN_BUFFER_TOO_SMALL.

  If PayloadBuffer or PayloadTransferSize is NULL and PayloadBufferSize is non-zero,
  the function shall return EFI_INVALID_PARAMETER.

  If the given MediaId does not support security protocol commands, the function shall
  return EFI_UNSUPPORTED. If there is no media in the device, the function returns
  EFI_NO_MEDIA. If the MediaId is not the ID for the current media in the device,
  the function returns EFI_MEDIA_CHANGED.

  If the security protocol fails to complete within the Timeout period, the function
  shall return EFI_TIMEOUT.

  If the security protocol command completes without an error, the function shall
  return EFI_SUCCESS. If the security protocol command completes with an error, the
  function shall return EFI_DEVICE_ERROR.

  @param  This                         Indicates a pointer to the calling context.
  @param  MediaId                      ID of the medium to receive data from.
  @param  Timeout                      The timeout, in 100ns units, to use for the execution
                                       of the security protocol command. A Timeout value of 0
                                       means that this function will wait indefinitely for the
                                       security protocol command to execute. If Timeout is greater
                                       than zero, then this function will return EFI_TIMEOUT
                                       if the time required to execute the receive data command
                                       is greater than Timeout.
  @param  SecurityProtocolId           The value of the "Security Protocol" parameter of
                                       the security protocol command to be sent.
  @param  SecurityProtocolSpecificData The value of the "Security Protocol Specific" parameter
                                       of the security protocol command to be sent.
  @param  PayloadBufferSize            Size in bytes of the payload data buffer.
  @param  PayloadBuffer                A pointer to a destination buffer to store the security
                                       protocol command specific payload data for the security
                                       protocol command. The caller is responsible for having
                                       either implicit or explicit ownership of the buffer.
  @param  PayloadTransferSize          A pointer to a buffer to store the size in bytes of the
                                       data written to the payload data buffer.

  @retval EFI_SUCCESS                  The security protocol command completed successfully.
  @retval EFI_WARN_BUFFER_TOO_SMALL    The PayloadBufferSize was too small to store the available
                                       data from the device. The PayloadBuffer contains the truncated data.
  @retval EFI_UNSUPPORTED              The given MediaId does not support security protocol commands.
  @retval EFI_DEVICE_ERROR             The security protocol command completed with an error.
  @retval EFI_NO_MEDIA                 There is no media in the device.
  @retval EFI_MEDIA_CHANGED            The MediaId is not for the current media.
  @retval EFI_INVALID_PARAMETER        The PayloadBuffer or PayloadTransferSize is NULL and
                                       PayloadBufferSize is non-zero.
  @retval EFI_TIMEOUT                  A timeout occurred while waiting for the security
                                       protocol command to execute.

**/
EFI_STATUS
EFIAPI
SecurityReceiveData (
  IN  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    *This,
  IN  UINT32                                   MediaId,
  IN  UINT64                                   Timeout,
  IN  UINT8                                    SecurityProtocolId,
  IN  UINT16                                   SecurityProtocolSpecificData,
  IN  UINTN                                    PayloadBufferSize,
  OUT VOID                                     *PayloadBuffer,
  OUT UINTN                                    *PayloadTransferSize
  )
{
  OPAL_PEI_DEVICE               *PeiDev;

  PeiDev = OPAL_PEI_DEVICE_FROM_THIS (This);
  if (PeiDev == NULL) {
    return EFI_DEVICE_ERROR;
  }

  return PeiDev->SscPpi->ReceiveData (
                           PeiDev->SscPpi,
                           PeiDev->DeviceIndex,
                           SSC_PPI_GENERIC_TIMEOUT,
                           SecurityProtocolId,
                           SecurityProtocolSpecificData,
                           PayloadBufferSize,
                           PayloadBuffer,
                           PayloadTransferSize
                           );
}

/**
  Send a security protocol command to a device.

  The SendData function sends a security protocol command containing the payload
  PayloadBuffer to the given MediaId. The security protocol command sent is
  defined by SecurityProtocolId and contains the security protocol specific data
  SecurityProtocolSpecificData. If the underlying protocol command requires a
  specific padding for the command payload, the SendData function shall add padding
  bytes to the command payload to satisfy the padding requirements.

  For devices supporting the SCSI command set, the security protocol command is sent
  using the SECURITY PROTOCOL OUT command defined in SPC-4.

  For devices supporting the ATA command set, the security protocol command is sent
  using one of the TRUSTED SEND commands defined in ATA8-ACS if PayloadBufferSize
  is non-zero. If the PayloadBufferSize is zero, the security protocol command is
  sent using the Trusted Non-Data command defined in ATA8-ACS.

  If PayloadBuffer is NULL and PayloadBufferSize is non-zero, the function shall
  return EFI_INVALID_PARAMETER.

  If the given MediaId does not support security protocol commands, the function
  shall return EFI_UNSUPPORTED. If there is no media in the device, the function
  returns EFI_NO_MEDIA. If the MediaId is not the ID for the current media in the
  device, the function returns EFI_MEDIA_CHANGED.

  If the security protocol fails to complete within the Timeout period, the function
  shall return EFI_TIMEOUT.

  If the security protocol command completes without an error, the function shall return
  EFI_SUCCESS. If the security protocol command completes with an error, the function
  shall return EFI_DEVICE_ERROR.

  @param  This                         Indicates a pointer to the calling context.
  @param  MediaId                      ID of the medium to receive data from.
  @param  Timeout                      The timeout, in 100ns units, to use for the execution
                                       of the security protocol command. A Timeout value of 0
                                       means that this function will wait indefinitely for the
                                       security protocol command to execute. If Timeout is greater
                                       than zero, then this function will return EFI_TIMEOUT
                                       if the time required to execute the send data command
                                       is greater than Timeout.
  @param  SecurityProtocolId           The value of the "Security Protocol" parameter of
                                       the security protocol command to be sent.
  @param  SecurityProtocolSpecificData The value of the "Security Protocol Specific" parameter
                                       of the security protocol command to be sent.
  @param  PayloadBufferSize            Size in bytes of the payload data buffer.
  @param  PayloadBuffer                A pointer to a destination buffer to store the security
                                       protocol command specific payload data for the security
                                       protocol command.

  @retval EFI_SUCCESS                  The security protocol command completed successfully.
  @retval EFI_UNSUPPORTED              The given MediaId does not support security protocol commands.
  @retval EFI_DEVICE_ERROR             The security protocol command completed with an error.
  @retval EFI_NO_MEDIA                 There is no media in the device.
  @retval EFI_MEDIA_CHANGED            The MediaId is not for the current media.
  @retval EFI_INVALID_PARAMETER        The PayloadBuffer is NULL and PayloadBufferSize is non-zero.
  @retval EFI_TIMEOUT                  A timeout occurred while waiting for the security
                                       protocol command to execute.

**/
EFI_STATUS
EFIAPI
SecuritySendData (
  IN EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    *This,
  IN UINT32                                   MediaId,
  IN UINT64                                   Timeout,
  IN UINT8                                    SecurityProtocolId,
  IN UINT16                                   SecurityProtocolSpecificData,
  IN UINTN                                    PayloadBufferSize,
  IN VOID                                     *PayloadBuffer
  )
{
  OPAL_PEI_DEVICE               *PeiDev;

  PeiDev = OPAL_PEI_DEVICE_FROM_THIS (This);
  if (PeiDev == NULL) {
    return EFI_DEVICE_ERROR;
  }

  return PeiDev->SscPpi->SendData (
                           PeiDev->SscPpi,
                           PeiDev->DeviceIndex,
                           SSC_PPI_GENERIC_TIMEOUT,
                           SecurityProtocolId,
                           SecurityProtocolSpecificData,
                           PayloadBufferSize,
                           PayloadBuffer
                           );
}

/**

  The function returns whether or not the device is Opal Locked.
  TRUE means that the device is partially or fully locked.
  This will perform a Level 0 Discovery and parse the locking feature descriptor

  @param[in]      OpalDev             Opal object to determine if locked.
  @param[out]     BlockSidSupported   Whether device support BlockSid feature.

**/
BOOLEAN
IsOpalDeviceLocked(
  OPAL_PEI_DEVICE    *OpalDev,
  BOOLEAN            *BlockSidSupported,
  UINT16             *BaseComId
  )
{
  OPAL_SESSION                   Session;
  OPAL_DISK_SUPPORT_ATTRIBUTE    SupportedAttributes;
  TCG_LOCKING_FEATURE_DESCRIPTOR LockingFeature;
  UINT16                         OpalBaseComId;
  TCG_RESULT                     Ret;

  Session.Sscp = &OpalDev->Sscp;
  Session.MediaId = 0;

  Ret = OpalGetSupportedAttributesInfo (&Session, &SupportedAttributes, &OpalBaseComId);
  if (Ret != TcgResultSuccess) {
    return FALSE;
  }

  Session.OpalBaseComId  = OpalBaseComId;
  *BaseComId             = OpalBaseComId;
  *BlockSidSupported     = SupportedAttributes.BlockSid == 1 ? TRUE : FALSE;
  
  Ret = OpalGetLockingInfo(&Session, &LockingFeature);
  if (Ret != TcgResultSuccess) {
    return FALSE;
  }

  return OpalDeviceLocked (&SupportedAttributes, &LockingFeature);
}

/**
  Unlock OPAL password for S3.

  @param[in] OpalDev            Opal object to unlock.

**/
VOID
UnlockOpalPassword (
  IN OPAL_PEI_DEVICE            *OpalDev
  )
{
  TCG_RESULT                    Result;
  OPAL_SESSION                  Session;
  BOOLEAN                       BlockSidSupport;
  UINT32                        PpStorageFlags;
  BOOLEAN                       BlockSIDEnabled;
  UINT16                        OpalBaseComId;

  BlockSidSupport = FALSE;
  if (IsOpalDeviceLocked (OpalDev, &BlockSidSupport, &OpalBaseComId)) {
    ZeroMem(&Session, sizeof (Session));
    Session.Sscp = &OpalDev->Sscp;
    Session.MediaId = 0;
    Session.OpalBaseComId = OpalBaseComId;

    Result = OpalUtilUpdateGlobalLockingRange (
               &Session,
               OpalDev->Device->Password,
               OpalDev->Device->PasswordLength,
               FALSE,
               FALSE
               );
    DEBUG ((
      DEBUG_INFO,
      "%a() OpalUtilUpdateGlobalLockingRange() Result = 0x%x\n",
      __FUNCTION__,
      Result
      ));
  }

  PpStorageFlags = Tcg2PhysicalPresenceLibGetManagementFlags ();
  if ((PpStorageFlags & TCG2_BIOS_STORAGE_MANAGEMENT_FLAG_ENABLE_BLOCK_SID) != 0) {
    BlockSIDEnabled = TRUE;
  } else {
    BlockSIDEnabled = FALSE;
  }
  if (BlockSIDEnabled && BlockSidSupport) {
    DEBUG ((DEBUG_INFO, "OpalPassword: S3 phase send BlockSid command to device!\n"));
    ZeroMem(&Session, sizeof (Session));
    Session.Sscp = &OpalDev->Sscp;
    Session.MediaId = 0;
    Session.OpalBaseComId = OpalDev->Device->OpalBaseComId;

    //
    //Set the SID Authentication Blocked State and Locking SP Freeze Lock State bits,
    //To block normal authentication of SID permissions and use of Locking SP.
    //(TCG_Storage-Feature_Set_Block_SID_Authentication_v1.01_r1.00 -> Section 4.2 Block SID Authentication Command)
    //
    Result = OpalBlockSid (&Session, FALSE);
    DEBUG ((
      DEBUG_INFO,
      "%a() OpalBlockSid() Result = 0x%x\n",
      __FUNCTION__,
      Result
      ));
  }
}
/**
 Unlock device in system S3 path through ATA Pass-thru Security Protocol (ATA_SEC)
**/
EFI_STATUS
EFIAPI
UnlockAtaPassword(
  IN EDKII_PEI_STORAGE_SECURITY_CMD_PPI    *SscPpi,
  IN HDP_PORT_INFO                         *PortInfo
)
{
  VOID                                  *AtaBuffer = NULL;
  UINTN                                 AtaBufferSize;
  UINT16                                *Point;
  EFI_STATUS                            Status;

  DEBUG((EFI_D_INFO, "[%a] Start!\n", __FUNCTION__));
  
  AtaBufferSize = 512;
  AtaBuffer     = AllocateZeroPool(AtaBufferSize);
  
  if(AtaBuffer == NULL){
    Status = EFI_OUT_OF_RESOURCES;
    return Status;
  }

  Point = (UINT16*)AtaBuffer;
  ZeroMem(Point, 0x24);
  Point[0] = SwapBytes16(PortInfo->IsUser?0:1);
  CopyMem(&Point[1], PortInfo->Password, 32);
  
  //
  // When ATA_SEC device security feature is not enabled(User Password is not set), 
  // in S3 path, device unlock is not required.
  //
  if (!IsZeroBuffer (PortInfo->Password, HDD_PASSWORD_MAX_LENGTH)) {
    //Send a security protocol command to a device.
    Status = SscPpi->SendData (
                    SscPpi,
                    0,
                    EFI_TIMER_PERIOD_SECONDS(10),
                    TCG_SECURITY_PROTOCOL_ATA_DEVICE_SERVER_PASS,
                    SwapBytes16(SPSP_UNLOCK),
                    0x24,
                    AtaBuffer
                    );
    ZeroMem(AtaBuffer, AtaBufferSize);
  }

  //Send security freeze cmd to a device.
  Status = SscPpi->SendData (
                  SscPpi,
                  0,
                  EFI_TIMER_PERIOD_SECONDS(10),
                  TCG_SECURITY_PROTOCOL_ATA_DEVICE_SERVER_PASS,
                  SwapBytes16(SPSP_FREEZE),
                  0,
                  NULL
                  );

    DEBUG((EFI_D_INFO, "[%a] End!\n", __FUNCTION__));
    return Status;
}
/**
  Unlock the OPAL NVM Express and ATA devices for S3.

  @param[in] SscPpi    Pointer to the EDKII_PEI_STORAGE_SECURITY_CMD_PPI instance.

**/
VOID
UnlockOpalPasswordDevices (
  IN EDKII_PEI_STORAGE_SECURITY_CMD_PPI    *SscPpi
  )
{
  EFI_STATUS                            Status;
  HDP_S3_DATA                           *HdpS3Data;
  UINTN                                 VarDataSize;
  HDP_PORT_INFO                         *PortInfo;
  UINTN                                 SscDeviceNum;
  UINTN                                 SscDeviceIndex;
  EFI_DEVICE_PATH_PROTOCOL              *SscDevicePath;
  UINTN                                 SscDevicePathLength;
  UINTN                                 Index;
  OPAL_DEVICE_LOCKBOX_DATA              DevInfo;
  OPAL_PEI_DEVICE                       OpalDev;
  PEI_NVME_CONTROLLER_PRIVATE_DATA      *NvmePrivate;
  UINT8                                 Sn[21];         // 1-Byte longer than real Sn Size(20) to hold '\0' at tail for trim

  // Get NVMe Controller PrivateData from this Ssc PPI
  NvmePrivate = GET_NVME_PEIM_HC_PRIVATE_DATA_FROM_THIS_STROAGE_SECURITY(SscPpi);

  //
  // Get BYO NVMe/HDD Password Info from Variable
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
    DEBUG((DEBUG_INFO, "[%a] Fail to get Variable %s\n", __FUNCTION__, L"HddUnlockInfo"));
    goto Exit;
  }


  //
  // Go through all the devices managed by the SSC PPI instance.
  //
  Status = SscPpi->GetNumberofDevices (SscPpi, &SscDeviceNum);
  if (EFI_ERROR (Status)) {
    goto Exit;
  }
  for (SscDeviceIndex = 1; SscDeviceIndex <= SscDeviceNum; SscDeviceIndex++) {
    Status = SscPpi->GetDevicePath (
                       SscPpi,
                       SscDeviceIndex,
                       &SscDevicePathLength,
                       &SscDevicePath
                       );
    if (SscDevicePathLength <= sizeof (EFI_DEVICE_PATH_PROTOCOL)) {
      //
      // Device path validity check.
      //
      continue;
    }

    //
    // Search the device in the restored.
    //
    for (Index = 0; Index < HdpS3Data->EnCount; Index++) {
      PortInfo = &HdpS3Data->Port[Index];

      DEBUG((EFI_D_INFO, "[%a] Sn: %a\n",  __FUNCTION__, PortInfo->Sn));
      DEBUG((EFI_D_INFO, "[%a] Password:", __FUNCTION__));
      DumpMem8(PortInfo->Password,32);   
      //
      // Unlock NVMe OPAL Device (Security SATA is not supported here)
      //
      if (PortInfo->Scc == 0x08) {  //Check PCI reg SubClassCode (Offset 0xA) to determine NVMe device

        if(PortInfo->IsAtaSec == TRUE) {
          UnlockAtaPassword(SscPpi, PortInfo);
        } else {
          DevInfo.Length = sizeof(OPAL_DEVICE_LOCKBOX_DATA);
      
          DevInfo.Device.Segment = 0;
          DevInfo.Device.Bus = PortInfo->Bus;
          DevInfo.Device.Device = PortInfo->Dev;
          DevInfo.Device.Function = PortInfo->Func;
          DevInfo.Device.Reserved = 0;
  
          DevInfo.PasswordLength = OPAL_MAX_PASSWORD_SIZE;
          CopyMem(DevInfo.Password, PortInfo->Password, OPAL_MAX_PASSWORD_SIZE);
          DevInfo.DevicePathLength = 0;
  
          OpalDev.Signature        = OPAL_PEI_DEVICE_SIGNATURE;
          OpalDev.Sscp.ReceiveData = SecurityReceiveData;
          OpalDev.Sscp.SendData    = SecuritySendData;
          OpalDev.Device           = &DevInfo;
          OpalDev.Context          = NULL;
          OpalDev.SscPpi           = SscPpi;
          OpalDev.DeviceIndex      = SscDeviceIndex;

          //
          // Sn saved by HddPassword Module is trimmed version. For the Sn queried here, same trim must be applied 
          //
          CopyMem(Sn, NvmePrivate->ControllerData->Sn, sizeof(Sn));
          // 1-Byte longer than real Sn Size(20) to hold '\0' at tail for trim
          Sn[20] = 0;
          TrimStr8((CHAR8 *)Sn);

          //
          // When multiple NVMe devices are installed on platform, check Opal Device SN is correct then unlock
          //
          if (CompareMem(Sn, PortInfo->Sn, sizeof(PortInfo->Sn)) == 0) {
            UnlockOpalPassword (&OpalDev);
          }
        }
      }
    }
  }

Exit:
  ZeroMem (HdpS3Data, VarDataSize);
  FreePool(HdpS3Data);
}

/**
  One notified function at the installation of EDKII_PEI_STORAGE_SECURITY_CMD_PPI.
  It is to unlock OPAL password for S3.

  @param[in] PeiServices         Indirect reference to the PEI Services Table.
  @param[in] NotifyDescriptor    Address of the notification descriptor data structure.
  @param[in] Ppi                 Address of the PPI that was installed.

  @return Status of the notification.
          The status code returned from this function is ignored.

**/
EFI_STATUS
EFIAPI
OpalPasswordStorageSecurityPpiNotify (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDesc,
  IN VOID                         *Ppi
  )
{
  DEBUG ((DEBUG_INFO, "%a entered at S3 resume!\n", __FUNCTION__));

  UnlockOpalPasswordDevices ((EDKII_PEI_STORAGE_SECURITY_CMD_PPI *) Ppi);

  DEBUG ((DEBUG_INFO, "%a exit at S3 resume!\n", __FUNCTION__));

  return EFI_SUCCESS;
}


EFI_PEI_NOTIFY_DESCRIPTOR mOpalPasswordStorageSecurityPpiNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEdkiiPeiStorageSecurityCommandPpiGuid,
  OpalPasswordStorageSecurityPpiNotify
};


/**
  Main entry for this module.

  @param FileHandle             Handle of the file being invoked.
  @param PeiServices            Pointer to PEI Services table.

  @return Status from PeiServicesNotifyPpi.

**/
EFI_STATUS
EFIAPI
OpalPasswordPeiInit (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS       Status;
  EFI_BOOT_MODE    BootMode;

  Status = PeiServicesGetBootMode (&BootMode);
  if ((EFI_ERROR (Status)) || (BootMode != BOOT_ON_S3_RESUME)) {
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "%a: Nvme Enters in S3 path.\n", __FUNCTION__));

  Status = PeiServicesNotifyPpi (&mOpalPasswordStorageSecurityPpiNotifyDesc);
  ASSERT_EFI_ERROR (Status);
  return Status;
}
