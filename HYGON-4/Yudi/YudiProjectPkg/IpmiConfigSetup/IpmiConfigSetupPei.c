/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PeiIpmiConfigSetup.c

Abstract:
  Source file for the IPMI PEIM.

Revision History:

**/

#include <Base.h>
#include <PiPei.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IpmiBaseLib.h>
#include <IpmiCommon.h>
#include <BoardIdType.h>


EFI_STATUS
IpmiGetSetupConfig (
  OUT BMC_CONFIG_SETUP *IpmiConfigSetup
  )
{
  EFI_STATUS                        Status;
  UINT8                             ResponseSize;

  ResponseSize = sizeof (BMC_CONFIG_SETUP);
  Status = EfiSendCommandToBMC (
             SM_BYOSOFT_NETFN_APP,
             0xc2,
             NULL,
             0,
             (UINT8 *) IpmiConfigSetup,
             &ResponseSize
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG_CODE(
    UINTN Index;
    UINT8 *ResponeBuf;

    ResponeBuf = (UINT8 *)IpmiConfigSetup;
    DEBUG((DEBUG_INFO, "Ipmi config setup Repsonse: "));
    for (Index = 0; Index < ResponseSize; Index++) {
      DEBUG((DEBUG_INFO, "0x%x ", ResponeBuf[Index]));
    }
    DEBUG((DEBUG_INFO, "\n"));
  );

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), ReadFlag :%d.\n", __LINE__, IpmiConfigSetup->ReadFlag));
  if (!IpmiConfigSetup->ReadFlag) {
    return EFI_NOT_READY;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
GetBmcConfigSetupInfo(
  OUT BMC_CONFIG_SETUP *IpmiConfigSetup
)
{
  EFI_STATUS                Status = EFI_SUCCESS;

  Status = IpmiGetSetupConfig (IpmiConfigSetup);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}

VOID
DumpIpmiResponse(
  BMC_CONFIG_SETUP IpmiConfigSetup
  )
{
  DEBUG((DEBUG_INFO,"IPMI Config Setup Options:\n"));
  DEBUG((DEBUG_INFO,"  Byte 0:\n"));
  DEBUG((DEBUG_INFO,"    ReadFlag - %x\n", IpmiConfigSetup.ReadFlag));

  DEBUG((DEBUG_INFO,"  Byte 1:\n"));
  DEBUG((DEBUG_INFO,"    CpuSmtMode - %x\n", IpmiConfigSetup.CpuSmtMode));
  DEBUG((DEBUG_INFO,"    SriovDis - %x\n", IpmiConfigSetup.SriovDis));
  DEBUG((DEBUG_INFO,"    CpuSvm - %x\n", IpmiConfigSetup.CpuSvm));
  DEBUG((DEBUG_INFO,"    CbsCmnGnbNbIOMMU - %x\n", IpmiConfigSetup.CbsCmnGnbNbIOMMU));
  DEBUG((DEBUG_INFO,"    CbsCmnCpuRdseedRdrandCtrl - %x\n", IpmiConfigSetup.CbsCmnCpuRdseedRdrandCtrl));
  DEBUG((DEBUG_INFO,"    PXEBootPriority - %x\n", IpmiConfigSetup.PXEBootPriority));

  DEBUG((DEBUG_INFO,"  Byte 2:\n"));
  DEBUG((DEBUG_INFO,"    CbsDfCmnMemIntlv - %x\n", IpmiConfigSetup.CbsDfCmnMemIntlv));
  DEBUG((DEBUG_INFO,"    CbsCmnCpuCpb - %x\n", IpmiConfigSetup.CbsCmnCpuCpb));
  DEBUG((DEBUG_INFO,"    LanBoot - %x\n", IpmiConfigSetup.LanBoot));
  DEBUG((DEBUG_INFO,"    BIOSLock - %x\n", IpmiConfigSetup.BIOSLock));
  DEBUG((DEBUG_INFO,"    HideBrandLogo - %x\n", IpmiConfigSetup.HideBrandLogo));

  DEBUG((DEBUG_INFO,"  Byte 3:\n"));
  DEBUG((DEBUG_INFO,"    BootModeType - %x\n", IpmiConfigSetup.BootModeType));
  DEBUG((DEBUG_INFO,"    Ipv4Support - %x\n", IpmiConfigSetup.Ipv4Support));
  DEBUG((DEBUG_INFO,"    Ipv6Support - %x\n", IpmiConfigSetup.Ipv6Support));
  DEBUG((DEBUG_INFO,"    PerfMode2 - %x\n", IpmiConfigSetup.PerfMode2));
  DEBUG((DEBUG_INFO,"    FRB2WatchDog - %x\n", IpmiConfigSetup.FRB2WatchDog));
  DEBUG((DEBUG_INFO,"    AesSupportDis - %x\n", IpmiConfigSetup.AesSupportDis));

  DEBUG((DEBUG_INFO,"  Byte 4-5-6:\n"));
  DEBUG((DEBUG_INFO,"    FirstBoot - %x\n", IpmiConfigSetup.FirstBoot));
  DEBUG((DEBUG_INFO,"    SecondBoot - %x\n", IpmiConfigSetup.SecondBoot));
  DEBUG((DEBUG_INFO,"    ThirdBoot - %x\n", IpmiConfigSetup.ThirdBoot));
  DEBUG((DEBUG_INFO,"    FourthBoot - %x\n", IpmiConfigSetup.FourthBoot));
  DEBUG((DEBUG_INFO,"    FifthBoot - %x\n", IpmiConfigSetup.FifthBoot));
  DEBUG((DEBUG_INFO,"    SixthBoot - %x\n", IpmiConfigSetup.SixthBoot));

  DEBUG((DEBUG_INFO,"  Byte 7:\n"));
  DEBUG((DEBUG_INFO,"    Language - %x\n", IpmiConfigSetup.Language));
  DEBUG((DEBUG_INFO,"    UCREnable - %x\n", IpmiConfigSetup.UCREnable));
  DEBUG((DEBUG_INFO,"    SerialBaudrate - %x\n", IpmiConfigSetup.SerialBaudrate));
  DEBUG((DEBUG_INFO,"    CbsCmnSmeeCtrl - %x\n", IpmiConfigSetup.CbsCmnSmeeCtrl));
  DEBUG((DEBUG_INFO,"    TerminalType - %x\n", IpmiConfigSetup.TerminalType));

  DEBUG((DEBUG_INFO,"  Byte 8:\n"));
  DEBUG((DEBUG_INFO,"    Pfeh - %x\n", IpmiConfigSetup.Pfeh));
  DEBUG((DEBUG_INFO,"    McaErrThreshCount - %x\n", IpmiConfigSetup.McaErrThreshCount));
  DEBUG((DEBUG_INFO,"    ObLanEn - %x\n", IpmiConfigSetup.ObLanEn));
  DEBUG((DEBUG_INFO,"    WakeOnLan - %x\n", IpmiConfigSetup.WakeOnLan));
  DEBUG((DEBUG_INFO,"    VideoPrimaryAdapter - %x\n", IpmiConfigSetup.VideoPrimaryAdapter));

  DEBUG((DEBUG_INFO,"  Byte 9:\n"));
  DEBUG((DEBUG_INFO,"    USBRearCtrl - %x\n", IpmiConfigSetup.USBRearCtrl));
  DEBUG((DEBUG_INFO,"    USBFronCtrl - %x\n", IpmiConfigSetup.USBFronCtrl));
  DEBUG((DEBUG_INFO,"    UsbMassSupport - %x\n", IpmiConfigSetup.UsbMassSupport));
  DEBUG((DEBUG_INFO,"    PcieMaxPayloadSize - %x\n", IpmiConfigSetup.PcieMaxPayloadSize));
  DEBUG((DEBUG_INFO,"    PcieAspm - %x\n", IpmiConfigSetup.PcieAspm));

  DEBUG((DEBUG_INFO,"  Byte 10:\n"));
  DEBUG((DEBUG_INFO,"    PerfMode - %x\n", IpmiConfigSetup.PerfMode));
  DEBUG((DEBUG_INFO,"    AMDPstate - %x\n", IpmiConfigSetup.CpuPstateEn));
  DEBUG((DEBUG_INFO,"    AMDCstate - %x\n", IpmiConfigSetup.CpuCstate));
  DEBUG((DEBUG_INFO,"    CpuSpeed - %x\n", IpmiConfigSetup.CpuSpeed));

  DEBUG((DEBUG_INFO,"  Byte 11-12:\n"));
  DEBUG((DEBUG_INFO,"    BootTimeout - %x\n", (IpmiConfigSetup.BootTimeout[1] << 8) + (IpmiConfigSetup.BootTimeout[0]) ));

  DEBUG((DEBUG_INFO,"  Byte 13:\n"));
  DEBUG((DEBUG_INFO,"    QuietBoot - %x\n", IpmiConfigSetup.QuietBoot));
  DEBUG((DEBUG_INFO,"    Numlock - %x\n", IpmiConfigSetup.Numlock));
  DEBUG((DEBUG_INFO,"    OpromMessage - %x\n", IpmiConfigSetup.LegacyOpromMessage));
  DEBUG((DEBUG_INFO,"    ShellEn - %x\n", IpmiConfigSetup.ShellEn));
  DEBUG((DEBUG_INFO,"    UefiNetworkStack - %x\n", IpmiConfigSetup.UefiNetworkStack));
  DEBUG((DEBUG_INFO,"    RetryPxeBoot - %x\n", IpmiConfigSetup.RetryPxeBoot));
}

EFI_STATUS
CreatIpmiSetInfoHob (
  VOID
  )
{
  EFI_STATUS               Status;
  BMC_CONFIG_SETUP         ConfigInfo;
  BMC_CONFIG_SETUP         *ConfigInfoHobDataPtr;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n\n", __LINE__));
  Status = IpmiGetSetupConfig (&ConfigInfo);
  //
  // Save IPMI config setup infomation into HOB
  //
  if (Status == EFI_SUCCESS) {		
    DumpIpmiResponse (ConfigInfo);

    ConfigInfoHobDataPtr = BuildGuidHob(&gIpmiConfigSetupInfoHobGuid, sizeof(BMC_CONFIG_SETUP));
    ASSERT (ConfigInfoHobDataPtr != NULL);

    CopyMem(ConfigInfoHobDataPtr, &ConfigInfo, sizeof(BMC_CONFIG_SETUP));
    DEBUG((EFI_D_INFO,"CreatIpmiSetInfoHob CopyMem end,size : %d\n",sizeof(BMC_CONFIG_SETUP)));
  }

  return Status;
}


EFI_STATUS
EFIAPI
PeiIpmiConfigSetupEntryPoint (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = CreatIpmiSetInfoHob();
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "CreatIpmiSetInfoHob:%r\n", Status));
  }

  return Status;
}

