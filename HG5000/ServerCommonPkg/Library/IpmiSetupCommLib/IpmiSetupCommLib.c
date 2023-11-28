/** @file
 Byosoft Impi Setup common library implement code.
*/
#include <Library/IpmiSetupCommLib.h>

STATIC IPMI_INTERFACE_PROTOCOL      *mIpmi = NULL;

/**
 Get BMC LAN IPv4 Address and update the string in BIOS Setup.
*/
EFI_STATUS
GetBmcIpv4AddressValue (
  IN  UINT8                         LanChannel,
  IN  EFI_HII_HANDLE                HiiHandle,
  IN  EFI_STRING_ID                 TokenToUpdate
  )
{
  EFI_STATUS                            Status;
  UINT8                                 SendData[4];
  UINT8                                 RecvBuf[5];
  UINT8                                 RecvSize;
  IPMI_CMD_HEADER                       Request;
  CHAR16                                TempString[50];

  if (mIpmi == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SendData[0] = LanChannel;
  SendData[1] = LAN_IP_ADDRESS;
  SendData[2] = 0;
  SendData[3] = 0;

  Request.NetFn = IPMI_NETFN_TRANSPORT;
  Request.Cmd = IPMI_CMD_GET_LAN_PARAMETERS;
  Request.Lun = IPMI_BMC_LUN;

  Status = mIpmi->ExecuteIpmiCmd (
                    mIpmi,
                    Request,
                    SendData,
                    4,
                    RecvBuf,
                    &RecvSize,
                    NULL
                    );
  
  if (!EFI_ERROR (Status)) {
    UnicodeSPrint (TempString, sizeof (TempString),
                     L"%d.%d.%d.%d",
                     RecvBuf[1],RecvBuf[2],RecvBuf[3],RecvBuf[4]
                     );
    HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);
  } else {
    HiiSetString (HiiHandle, TokenToUpdate, L"N/A", NULL);
  }

  return Status;
}

/**
 Update BMC Status string in BIOS Setup.
*/
EFI_STATUS
UpdateBmcStatusValue (
  IN  EFI_HII_HANDLE            HiiHandle,
  IN  EFI_STRING_ID             TokenToUpdate
  )
{
  CHAR16                          TempString[30];
  IPMI_BMC_STATUS                 BmcStatus;
  
  BmcStatus = mIpmi->GetBmcStatus (mIpmi);

  ZeroMem (TempString, sizeof (TempString));

  switch (BmcStatus) {
    case BMC_STATUS_OK:
      UnicodeSPrint (TempString, sizeof (TempString), L"OK");
      break;

    case BMC_STATUS_ERROR:
      UnicodeSPrint (TempString, sizeof (TempString), L"Error");
      break;

    case BMC_STATUS_NOT_READY:
      UnicodeSPrint (TempString, sizeof (TempString), L"Not Ready");
      break;

    case BMC_STATUS_NOT_FOUND:
      UnicodeSPrint (TempString, sizeof (TempString), L"Not Found");
      break;

    case BMC_STATUS_UNKNOWN:
      UnicodeSPrint (TempString, sizeof (TempString), L"Unknown");
      break;
  }

  HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

  return EFI_SUCCESS;
}

/**
 Update BMC Firmware Version string in BIOS Setup
*/
EFI_STATUS
UpdateBmcFirmwareVersionValue (  
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  EFI_STRING_ID                     TokenToUpdate
  )
{
  CHAR16                                TempString[100];
  UINT16                                FwVersion;
  
  if (mIpmi == NULL) {
    return EFI_UNSUPPORTED;
  }
  
  ZeroMem (TempString, sizeof (TempString));
  FwVersion = mIpmi->GetBmcFirmwareVersion (mIpmi);
  
#if BC_TENCENT
  UnicodeSPrint (
    TempString,
    sizeof (TempString),
    L"%d.%02x.%02d",
    FwVersion >> 12, 
    (FwVersion >> 4) & 0xff, 
    FwVersion & 0xf
    );
#else
  UnicodeSPrint (
      TempString,
      sizeof (TempString),
      L"%d.%x.%d",
      FwVersion >> 12, 
      (FwVersion >> 4) & 0xff, 
      FwVersion & 0xf
      );

#endif

  HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);
  return EFI_SUCCESS;
}

/**
  Ipmi Setup CommLib Constructor.

  @param[in]  ImageHandle       The firmware allocated handle for the EFI image.
  @param[in]  SystemTable       A pointer to the EFI System Table.

  @retval EFI_SUCCESS  Constructor returns successfully.
**/
EFI_STATUS
EFIAPI
IpmiSetupCommLibConstructor (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = gBS->LocateProtocol (
                  &gByoIpmiInterfaceProtocolGuid,
                  NULL,
                  (VOID **)&mIpmi);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "LocateProtocol gByoIpmiInterfaceProtocolGuid Status = %r \n", Status));
  }

  return Status;
}
