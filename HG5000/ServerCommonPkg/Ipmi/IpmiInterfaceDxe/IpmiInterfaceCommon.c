/** @file
 Byosoft IPMI DXE Interface module.
*/

#include <IpmiInterfaceCommon.h>


/**
 Empty function for internal use.

 @param[in]        This                 A pointer to IPMI_INTERFACE_PROTOCOL structure.
 @param[in]        Request              IPMI_CMD_HEADER structure, storing Network Function, LUN
                                        and various IPMI command, to send to BMC.
 @param[in]        SendData             Optional arguments, if an IPMI command is required to
                                        send with data, this argument is a pointer to the data buffer.
                                        If no data is required, set this argument as NULL.
 @param[in]        SendLength           When sending command with data, this argument is the length of the data,
                                        otherwise set this argument as 0.
 @param[out]       RecvData             Data buffer to put the data read from BMC.
 @param[out]       RecvLength           Length of Data readed from BMC.
 @param[out]       StatusCodes          The bit 15 of StatusCodes means this argument is valid or not:
                                        1. If bit 15 set to 1, this is a valid Status Code,
                                        and the Status Code is in low byte.
                                        2. If bit 15 set to 0, there is no Status Code
                                        For KCS system interface:
                                        StatusCodes is valid when return value is EFI_ABORTED. If the return
                                        value is EFI_DEVICE_ERROR,it does not guarantee StatusCodes is valid,
                                        the caller must check bit 15.
                                        For SMIC system interface: It is always valid.
                                        For BT system interface: It is no function.

 @retval EFI_UNSUPPORTED                Always return EFI_UNSUPPORTED since this is an empty function.
*/
EFI_STATUS
EFIAPI
EmptyExecuteIpmiCmd (
  IN  IPMI_INTERFACE_PROTOCOL           *This,
  IN  IPMI_CMD_HEADER                   Request,
  IN  VOID                              *SendData OPTIONAL,
  IN  UINTN                             SendLength,
  OUT VOID                              *RecvData,
  OUT UINT8                             *RecvLength,
  OUT UINT16                            *StatusCodes OPTIONAL
  )
{
  return EFI_UNSUPPORTED;
}


/**
 Return system interface address that BMC currently use.

 @param[in]         This                A pointer to IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         Index               Type of base address, one of three types:
                                        "IPMI_OS_BASE_ADDRESS_INDEX"
                                        "IPMI_POST_BASE_ADDRESS_INDEX"

 @retval The base address that BMC current use.
*/
UINT64
EFIAPI
GetBaseAddress (
  IN  IPMI_INTERFACE_PROTOCOL           *This,
  IN  UINT8                             Index
  )
{
  IPMI_PRIVATE_DATA                 *IpmiData;

  if (This == NULL) {
    return 0;
  }

  IpmiData = IPMI_PRIVATE_DATA_FROM_THIS (This);

  return IpmiData->IpmiBaseAddress;
}


/**
 Return system interface offset that BMC currently use.

 @param[in]         This                A pointer to IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         Index               Type of base address, one of three types:
                                        "IPMI_OS_BASE_ADDRESS_INDEX"
                                        "IPMI_POST_BASE_ADDRESS_INDEX"

 @retval The base address offset that BMC current use.
*/
UINT64
EFIAPI
GetBaseAddressOffset (
  IN  IPMI_INTERFACE_PROTOCOL           *This,
  IN  UINT8                             Index
  )
{
  IPMI_PRIVATE_DATA                 *IpmiData;

  if (This == NULL) {
    return 0;
  }

  IpmiData = IPMI_PRIVATE_DATA_FROM_THIS (This);

  return IpmiData->IpmiBaseAddressOffset;
}

/**
 Return IPMI version that BMC current supports.

 @param[in]         This                Pointer to IPMI_INTERFACE_PROTOCOL instance.

 @retval IPMI version. Upper four bits are msb, lower four bits are lsb. For example, if
         IPMI version is 2.0, this function return 0x20h.
*/
UINT8
EFIAPI
GetIpmiVersion (
  IN  IPMI_INTERFACE_PROTOCOL       *This
  )
{
  IPMI_PRIVATE_DATA                 *IpmiData;

  if (This == NULL) {
    return 0;
  }

  IpmiData = IPMI_PRIVATE_DATA_FROM_THIS (This);

  return IpmiData->IpmiVersion;

}


/**
 Return BMC firmware version.

 @param[in]         This                Pointer to IPMI_INTERFACE_PROTOCOL instance.

 @retval Major revision is in high byte, minor revision is in low byte.
*/
UINT16
EFIAPI
GetBmcFirmwareVersion (
  IN  IPMI_INTERFACE_PROTOCOL       *This
  )
{
  IPMI_PRIVATE_DATA                 *IpmiData;

  if (This == NULL) {
    return 0;
  }

  IpmiData = IPMI_PRIVATE_DATA_FROM_THIS (This);

  return IpmiData->BmcFirmwareVersion;

}


/**
 Return BMC status.

 @param[in]         This                A pointer to IPMI_INTERFACE_PROTOCOL structure.

 @retval The enum IPMI_BMC_STATUS of BMC current status.
*/
IPMI_BMC_STATUS
EFIAPI
GetBmcStatus (
  IN  IPMI_INTERFACE_PROTOCOL       *This
  )
{
  IPMI_PRIVATE_DATA                 *IpmiData;

  if (This == NULL) {
    return BMC_STATUS_UNKNOWN;
  }

  IpmiData = IPMI_PRIVATE_DATA_FROM_THIS (This);

  return IpmiData->BmcStatus;

}

UINT8
EFIAPI
GetLastCompleteCode (
  IN  IPMI_INTERFACE_PROTOCOL       *This
  )
{
  IPMI_PRIVATE_DATA                 *IpmiData;

  if (This == NULL) {
    return BMC_STATUS_UNKNOWN;
  }

  IpmiData = IPMI_PRIVATE_DATA_FROM_THIS (This);

  return IpmiData->LastCompleteCode;

}


/**
 Get basic BMC information.

 This function use "GetDeviceId" IPMI command to get BMC information, 
especially
 BMC Firmware Version and IPMI Specification Version.

 @param[in]         Ipmi                A pointer to IPMI_INTERFACE_PPI 
structure.
 @param[in]         BmcInfo             Structure for storing data from Get 
Device ID command.

 @retval EFI_SUCCESS                    Execute "GetDeviceId" IPMI command 
success
                                        and the information is correct.
 @retval EFI_TIMEOUT                    BMC no response or BMC is busy.
 @retval EFI_NOT_FOUND                  Corresponding register is not be 
decode.
 @retval EFI_DEVICE_ERROR               BMC has other issue other then no 
response and busy.
*/
EFI_STATUS
GetBmcInfo (
  IN  IPMI_INTERFACE_PROTOCOL       *Ipmi,
  IN  IPMI_BMC_INFO                 *BmcInfo
  )
{
  EFI_STATUS                            Status;
  UINT8                                 RecvSize;
  IPMI_CMD_HEADER                       Request;
  UINT8                                 Retry;
  UINT8                                 Temp;


  DEBUG ((EFI_D_INFO,"%a(),line:%d,\n",__FUNCTION__,__LINE__));

  //
  // Some BMCs will return garbage data after BMC just ready.
  // Retry several times to read correct data
  //
  Request.NetFn = EFI_SM_NETFN_APP;
  Request.Cmd = EFI_APP_GET_DEVICE_ID;
  Request.Lun = IPMI_BMC_LUN;

  for (Retry = 0; Retry < 3; ++Retry) {
    Status = Ipmi->ExecuteIpmiCmd (
                     Ipmi,
                     Request,
                     NULL,
                     0,
                     BmcInfo,
                     &RecvSize,
                     NULL
                     );
    if (Status == EFI_SUCCESS) {
      DEBUG ((DEBUG_INFO, "DeviceId:%x,DeviceRevision:%x\n",BmcInfo->DeviceId, BmcInfo->DeviceRevision));
      DEBUG ((DEBUG_INFO, "MajorFmRevision:%x,MinorFmRevision:%x\n",BmcInfo->MajorFmRevision, BmcInfo->MinorFmRevision));
      Temp = BmcInfo->IpmiVersionMostSigBits << 4 | BmcInfo->IpmiVersionLeastSigBits;
      if (Temp == 0x20 || Temp == 0x15) {
        DEBUG ((DEBUG_INFO, "Get BMC infor SUCCESS, IpmiVersion:%x\n",Temp));
        return EFI_SUCCESS;
      }
      DEBUG ((DEBUG_ERROR, "[IpmiInterface] Strange IPMI Version! Maybe BMC Firmware Bug!\n"));
    } else if (Status == EFI_TIMEOUT) {
      return EFI_TIMEOUT;
    } else if (Status == EFI_NOT_FOUND) {
      return EFI_NOT_FOUND;
    }
  }

  return EFI_DEVICE_ERROR;
}


/**
 Fill private data content

 @param[in]         IpmiHob             A pointer to IPMI Hob
 @param[in]         InSmm               In SMM or not. It is possible to use different base address in SMM.
 @param[out]        IpmiData            A pointer to IPMI_PRIVATE_DATA
*/
VOID
InitialIpmiInterfaceContent (
  OUT IPMI_PRIVATE_DATA             *IpmiData
  )
{
  EFI_STATUS                            Status;
  IPMI_BMC_INFO                         BmcInfo;
  UINT8                                 Retry;

  
  DEBUG ((EFI_D_INFO,"%a(),line:%d,\n",__FUNCTION__,__LINE__));
  //
  // Initial Context Structure
  //
  IpmiData->Signature = IPMI_PRIVATE_DATA_SIGNATURE;
  IpmiData->IpmiProtocol.GetIpmiBaseAddress = GetBaseAddress;
  IpmiData->IpmiProtocol.GetIpmiBaseAddressOffset = GetBaseAddressOffset;
  IpmiData->IpmiProtocol.GetIpmiVersion = GetIpmiVersion;
  IpmiData->IpmiProtocol.GetBmcFirmwareVersion = GetBmcFirmwareVersion;
  IpmiData->IpmiProtocol.GetBmcStatus = GetBmcStatus;
  IpmiData->IpmiProtocol.GetBmcStatus = GetBmcStatus;
  IpmiData->IpmiProtocol.GetLastCompleteCode = GetLastCompleteCode;
  IpmiData->BmcFirmwareVersion = 0xFFFF;
  IpmiData->IpmiVersion = IPMI_VERSION_DEFAULT;
  IpmiData->BmcStatus = BMC_STATUS_UNKNOWN;
  IpmiData->ProtocolHandle = NULL;
  IpmiData->IpmiBaseAddress = FixedPcdGet16 (PcdIpmiPostBaseAddress);
  IpmiData->IpmiBaseAddressOffset = FixedPcdGet16 (PcdIpmiPostRegOffset);

  InitialKcsSystemInterface (IpmiData);

  for (Retry = 0; Retry < BMC_TIMEOUT / BMC_KCS_TIMEOUT; Retry ++) {
    Status = GetBmcInfo (&IpmiData->IpmiProtocol, &BmcInfo);
    if (Status == EFI_SUCCESS) {
      IpmiData->BmcFirmwareVersion = BmcInfo.MajorFmRevision << 12 | BmcInfo.MinorFmRevision << 4 | BmcInfo.AuxInfo[0];
      IpmiData->IpmiVersion = BmcInfo.IpmiVersionMostSigBits << 4 | BmcInfo.IpmiVersionLeastSigBits;
      IpmiData->BmcStatus = BMC_STATUS_OK;
      break;
    } else {
      //Retry ++;
      gBS->Stall (1000000);// 1s
      DEBUG((EFI_D_INFO, "Cannot get BMC infor, retry:%d\n", Retry));
    }
  }

  //
  // If BMC is not ready or something else, use empty function since we cannot
  // do anything.
  //
  if (IPMI_VERSION_DEFAULT == IpmiData->IpmiVersion) {
    IpmiData->IpmiProtocol.ExecuteIpmiCmd = EmptyExecuteIpmiCmd;
  }

}
