

#include <PiPei.h>
#include <Pi/PiBootMode.h>      // HobLib.h +
#include <Pi/PiHob.h>           // HobLib.h +
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PlatformCommLib.h>
#include <Ppi/IpmiTransportPpi.h>
#include <EfiServerManagement.h>
#include <Library/ByoCommLib.h>


#define EFI_SM_NETFN_APP         0x06
#define EFI_SM_NETFN_LAN         0x0C
#define GET_LAN_CONFIG_CMD       0x02
#define GET_DEVICE_ID_CMD        0x01



EFI_STATUS
EFIAPI
PlatIpmiPeiCallBack (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                 Status;
  PEI_IPMI_TRANSPORT_PPI     *IpmiTransport;
  UINT8                      InBuffer[4];
  UINT8                      OutBuffer[32];
  UINT8                      OutBufferSize;
  PLATFORM_COMM_INFO         *Info;
  EFI_SM_CTRL_INFO           *SmCtrlInfo;
  

  DEBUG((EFI_D_INFO, "PlatIpmiPeiCallBack\n"));

  Status = PeiServicesLocatePpi (
              &gPeiIpmiTransportPpiGuid,
              0,
              NULL,
              (VOID **) &IpmiTransport
              );
  if(EFI_ERROR(Status)){
    return Status;
  }

  Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();
  
  InBuffer[0] = FixedPcdGet8(PcdBmcOnboardLanChannelNumber);
  InBuffer[1] = 3; //LanParam(IP Address)
  InBuffer[2] = 0;
  InBuffer[3] = 0;
  OutBufferSize = sizeof(OutBuffer);
  Status = IpmiTransport->SendIpmiCommand (
                             IpmiTransport,
                             EFI_SM_NETFN_LAN,
                             0,
                             GET_LAN_CONFIG_CMD,
                             InBuffer,
                             sizeof(InBuffer),
                             OutBuffer,
                             &OutBufferSize
                             );
  DEBUG((EFI_D_INFO, "<1> %r %d.%d.%d.%d\n", Status, OutBuffer[1], OutBuffer[2], OutBuffer[3], OutBuffer[4]));

  if (!IsZeroBuffer(&OutBuffer[1], 4)) {
    CopyMem(&Info->BmcIp.v4, &OutBuffer[1], sizeof(EFI_IPv4_ADDRESS));
    Info->BmcIpType = 4;
    
  } else {
    
    InBuffer[1] = 0x40; //GATEWAY_IPV6_CONFIG_PARM
    InBuffer[2] = 0;
    InBuffer[3] = 0;
    OutBufferSize = sizeof(OutBuffer);
    Status = IpmiTransport->SendIpmiCommand (
                               IpmiTransport,
                               EFI_SM_NETFN_LAN,
                               0,
                               GET_LAN_CONFIG_CMD,
                               InBuffer,
                               sizeof(InBuffer),
                               OutBuffer,
                               &OutBufferSize
                               );
    DEBUG((EFI_D_INFO, "<2> %r %d\n", Status, OutBuffer[1]));
    if(OutBuffer[1] == 2){
      InBuffer[1] = 0x3b;
    } else {
      InBuffer[1] = 0x38;
    }
    InBuffer[2] = 0;
    InBuffer[3] = 0;
    OutBufferSize = sizeof(OutBuffer);
    Status = IpmiTransport->SendIpmiCommand (
                               IpmiTransport,
                               EFI_SM_NETFN_LAN,
                               0,
                               GET_LAN_CONFIG_CMD,
                               InBuffer,
                               sizeof(InBuffer),
                               OutBuffer,
                               &OutBufferSize
                               );
    DEBUG((EFI_D_INFO, "<3> %r %x %x %x %x\n", Status, OutBuffer[3], OutBuffer[4], OutBuffer[5], OutBuffer[6]));    
    if(!IsZeroBuffer(&OutBuffer[3], 16)){
      CopyMem(&Info->BmcIp.v6, &OutBuffer[3], sizeof(EFI_IPv6_ADDRESS));
      Info->BmcIpType = 6;
    }

  }

  OutBufferSize = sizeof(OutBuffer);
  Status = IpmiTransport->SendIpmiCommand (
                             IpmiTransport,
                             EFI_SM_NETFN_APP,
                             0,
                             GET_DEVICE_ID_CMD,
                             NULL,
                             0,
                             OutBuffer,
                             &OutBufferSize
                             );
  if(!EFI_ERROR(Status)){
    SmCtrlInfo = (EFI_SM_CTRL_INFO*)OutBuffer;
    Info->BmcVersion[2] = SmCtrlInfo->MajorFirmwareRev;
    Info->BmcVersion[1] = SmCtrlInfo->MinorFirmwareRev;             // BCD
    Info->BmcVersion[1] = ((Info->BmcVersion[1] >> 4) & 0xF) * 10 + (Info->BmcVersion[1] & 0xF);
    Info->BmcVersion[0] = (UINT8)(SmCtrlInfo->AuxFirmwareRevInfo & 0xFF);
    DEBUG((EFI_D_INFO, "<4> %d.%d.%d\n", Info->BmcVersion[2], Info->BmcVersion[1], Info->BmcVersion[0])); 
  }

  return Status;
}  


