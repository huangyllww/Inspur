/* $NoKeywords:$ */

/**
 * @file
 *
 * FCH DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project   FCH DXE Driver
 *
 */
/*****************************************************************************
 *
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 *
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 *
 ******************************************************************************
 */
#include "FchDxe.h"
#include <Library/IoLib.h>
extern EFI_GUID        gFchResetDataHobGuid;
extern EFI_GUID        gEfiPciIoProtocolGuid;
extern FCH_DATA_BLOCK  InitEnvCfgDefault;

EFI_STATUS
EFIAPI
FchUsbDisablePort (
  IN       CONST FCH_INIT_PROTOCOL   *This,
  IN       UINT8                     Socket,
  IN       UINT8                     PhysicalDieId,
  IN       UINT8                     UsbIndex,
  IN       UINT32                    USB3DisableMap,
  IN       UINT32                    USB2DisableMap
  );

EFI_STATUS
EFIAPI
FchUsbOvercurrentControl (
  IN       CONST FCH_INIT_PROTOCOL   *This,
  IN       UINT8                     Socket,
  IN       UINT8                     PhysicalDieId,
  IN       UINT8                     UsbIndex,
  IN       UINT8                     Port,
  IN       UINT8                     OCPin
  );

//
// Private Function Declaration
//
VOID
InvokeFchInitMid (
  IN       EFI_EVENT        Event,
  IN       VOID             *Context
  )
{
  FCH_DATA_BLOCK  *FchPolicy;
  HGPI_STATUS     HgpiStatus;

  DEBUG ((DEBUG_INFO, "[FchInitMid] Fch Init - After PCI Scan ...Start\n"));
  FchPolicy  = (FCH_DATA_BLOCK *)Context;
  HgpiStatus = FchInitMid (FchPolicy);
  DEBUG ((DEBUG_INFO, "[FchInitMid] Fch Init - After PCI Scan ...Complete\n"));

  gBS->CloseEvent (Event);
}

VOID
InvokeFchInitLate (
  IN       EFI_EVENT        Event,
  IN       VOID             *Context
  )
{
  FCH_DATA_BLOCK  *FchPolicy;
  HGPI_STATUS     HgpiStatus;

  DEBUG ((DEBUG_INFO, "[FchInitLate] Fch Init - Before Boot ...Start\n"));
  FchPolicy  = (FCH_DATA_BLOCK *)Context;
  HgpiStatus = FchInitLate (FchPolicy);
  DEBUG ((DEBUG_INFO, "[FchInitLate] Fch Init - Before Boot ...Complete\n"));

  gBS->CloseEvent (Event);
}

FCH_DATA_BLOCK *
FchInitDataBlock (
  IN OUT   FCH_DATA_BLOCK  *FchParams
  )
{
  EFI_STATUS                Status;
  EFI_HOB_GUID_TYPE         *FchHob;
  FCH_RESET_DATA_BLOCK      *FchResetParams;
  VOID                      *PcdDataPtr;
  UINT16                    IoEnable16;
  UINT16                    UartChannel;
  UINT8                     UartLegacy[4];
  UINT8                     PortIndex;
  UINT8                     RootBridgeIndex;
  UINT8                     RootBridgeCount;
  UINT64                    SataPortMdControl;
  UINT32                    SataEspPortControl;
  UINT32                    SataPortPowerControl;

  // load default
  gBS->CopyMem (
         FchParams,
         &InitEnvCfgDefault,
         sizeof (FCH_DATA_BLOCK)
         );
  // find HOB and update with reset data block
  FchHob = GetFirstGuidHob (&gFchResetDataHobGuid);
  FchHob++;
  FchResetParams = (FCH_RESET_DATA_BLOCK *)((UINTN)FchHob - sizeof (UINT32));

  FchParams->Usb.Xhci0Enable              = FchResetParams->FchReset.Xhci0Enable;
  FchParams->Usb.Xhci1Enable              = FchResetParams->FchReset.Xhci1Enable;
  FchParams->Spi.SpiFastSpeed             = FchResetParams->Spi.SpiFastSpeed;
  FchParams->Spi.WriteSpeed               = FchResetParams->Spi.WriteSpeed;
  FchParams->Spi.SpiMode                  = FchResetParams->Spi.SpiMode;
  FchParams->Spi.SpiSpeed                 = FchResetParams->Spi.SpiSpeed;
  FchParams->Spi.AutoMode                 = FchResetParams->Spi.AutoMode;
  FchParams->Spi.SpiBurstWrite            = FchResetParams->Spi.SpiBurstWrite;
  FchParams->Sata.SataMode.Sata6AhciCap   = (UINT8) FchResetParams->SataMode.Sata6AhciCap;
  FchParams->Misc.Cg2Pll                  = FchResetParams->Misc2.Cg2Pll;
  FchParams->Sata.SataMode.SataEnable     = FchResetParams->FchReset.SataEnable;
  FchParams->Sata.SataMode.SataSetMaxGen2 = FchResetParams->SataMode.SataSetMaxGen2;
  FchParams->Sata.SataMode.SataClkMode    = FchResetParams->SataMode.SataClkMode;
  FchParams->Sata.SataMode.SataModeReg    = FchResetParams->SataMode.SataModeReg;
  FchParams->Sata.SataInternal100Spread   = (UINT8) FchResetParams->Misc2.SataInternal100Spread;

  Status = FchPlatformOemDxeInit ((VOID *)FchParams);

  ASSERT (!EFI_ERROR (Status));

  IDS_HOOK (IDS_HOOK_FCH_INIT_ENV, NULL, (VOID *)FchParams);

  //Dynamic PCDs
  FchParams->Ab.AbClockGating             = PcdGet8 (PcdAbClockGating);
  FchParams->Ab.ALinkClkGateOff           = PcdGet8 (PcdALinkClkGateOff);
  FchParams->Ab.BLinkClkGateOff           = PcdGet8 (PcdBLinkClkGateOff);
  FchParams->Ab.SbgMemoryPowerSaving      = PcdGetBool (PcdSbgMemoryPowerSaving);
  FchParams->Ab.SbgClockGating            = PcdGetBool (PcdSbgClockGating);
  FchParams->Ab.XdmaDmaWrite16ByteMode    = PcdGetBool (PcdXdmaDmaWrite16ByteMode);
  FchParams->Ab.XdmaMemoryPowerSaving     = PcdGetBool (PcdXdmaMemoryPowerSaving);
  FchParams->Ab.XdmaPendingNprThreshold   = PcdGet8 (PcdXdmaPendingNprThreshold);
  FchParams->Ab.XdmaDncplOrderDis         = PcdGetBool (PcdXdmaDncplOrderDis);
  //FchParams->Ab.GppClockRequest0          = PcdGet8 (PcdGppClockRequest0);
  //FchParams->Ab.GppClockRequest1          = PcdGet8 (PcdGppClockRequest1);
  //FchParams->Ab.GppClockRequest2          = PcdGet8 (PcdGppClockRequest2);
  //FchParams->Ab.GppClockRequest3          = PcdGet8 (PcdGppClockRequest3);
  //FchParams->Ab.SltGfxClockRequest0       = PcdGet8 (PcdSltGfxClockRequest0);
  //FchParams->Ab.SltGfxClockRequest1       = PcdGet8 (PcdSltGfxClockRequest1);
  FchParams->Ab.SdphostBypassDataPack     = PcdGetBool (PcdSdphostBypassDataPack);
  FchParams->Ab.SdphostDisNpmwrProtect    = PcdGetBool (PcdSdphostDisNpmwrProtect);
  FchParams->Ab.ResetCpuOnSyncFlood       = PcdGetBool (PcdResetCpuOnSyncFlood);

  FchParams->Sata.SataClass               = PcdGet8 (PcdSataClass);
  FchParams->Sata.SataAggrLinkPmCap       = PcdGet8 (PcdSataAggrLinkPmCap);
  FchParams->Sata.SataPortMultCap         = PcdGet8 (PcdSataPortMultCap);
  FchParams->Sata.SataPscCap              = PcdGet8 (PcdSataPscCap);
  FchParams->Sata.SataSscCap              = PcdGet8 (PcdSataSscCap);
  FchParams->Sata.SataClkAutoOff          = PcdGet8 (PcdSataClkAutoOff);
  FchParams->Sata.SataFisBasedSwitching   = PcdGet8 (PcdSataFisBasedSwitching);
  FchParams->Sata.SataCccSupport          = PcdGet8 (PcdSataCccSupport);
  FchParams->Sata.SataDisableGenericMode  = PcdGet8 (PcdSataDisableGenericMode);
  RootBridgeCount = (UINT8)FabricTopologyGetNumberOfRootBridgesOnSocket(0);
  SataPortMdControl                       = (UINT64) PcdGet64 (PcdSataModePortHyEx);
  SataEspPortControl                      = (UINT32) PcdGet32 (PcdSataEspPortHyEx);
  SataPortPowerControl                    = (UINT32) PcdGet32 (PcdSataPortPowerPortHyEx);

  for (RootBridgeIndex = 0; RootBridgeIndex < RootBridgeCount; RootBridgeIndex++){
    for (PortIndex = 0; PortIndex < TAISHAN_SATA_PORT_NUM; PortIndex++)  {
      //Record sata port mode for each port
      FchParams->Sata.SataPortMd.Port[RootBridgeIndex][PortIndex] = 
                              (UINT8) (((SataPortMdControl >> (RootBridgeIndex * TAISHAN_SATA_PORT_NUM * 2)) >> (PortIndex *2)) & 0x3);
      
      //Record ESP for each port
      FchParams->Sata.SataEspPort.Port[RootBridgeIndex][PortIndex] = 
                              (UINT8) (((SataEspPortControl >> (RootBridgeIndex * TAISHAN_SATA_PORT_NUM)) >> PortIndex) & 0x01);

      //Record sata port power for each port
      FchParams->Sata.SataPortPower.Port[RootBridgeIndex][PortIndex] = 
                              (UINT8) (((SataPortPowerControl >> (RootBridgeIndex * TAISHAN_SATA_PORT_NUM)) >> PortIndex) & 0xFF);
    }

    //Record sata port mode for each NBIO
    FchParams->Sata.SataPortMd.SataPortMode[RootBridgeIndex] = (UINT16) ((SataPortMdControl >> (RootBridgeIndex * TAISHAN_SATA_PORT_NUM * 2)) & 0xFFFF);
    
    //Record ESP for each NBIO
    FchParams->Sata.SataEspPort.SataPortReg[RootBridgeIndex] = (UINT8) ((SataEspPortControl >> (RootBridgeIndex * TAISHAN_SATA_PORT_NUM)) & 0xFF);

    //Record sata port power setting for each NBIO
    FchParams->Sata.SataPortPower.SataPortReg[RootBridgeIndex] = (UINT8) ((SataPortPowerControl >> (RootBridgeIndex * TAISHAN_SATA_PORT_NUM)) & 0xFF);
  }

  FchParams->Sata.SataTargetSupport8Device          = PcdGet8 (PcdSataTargetSupport8Device);
  FchParams->Sata.SataAhciEnclosureManagement       = PcdGet8 (PcdSataAhciEnclosureManagement);
  FchParams->Sata.SataMsiEnable           = PcdGetBool (PcdSataMsiEnable);
  FchParams->Sata.SataRasSupport          = PcdGetBool (PcdSataRasSupport);
  FchParams->Sata.SataAhciDisPrefetchFunction       = PcdGetBool (PcdSataAhciDisPrefetchFunction);
  FchParams->Sata.SataDevSlpPort0         = PcdGetBool (PcdSataDevSlpPort0);
  FchParams->Sata.SataDevSlpPort1         = PcdGetBool (PcdSataDevSlpPort1);
  FchParams->Sata.SataDevSlpPort0Num      = PcdGet8 (PcdSataDevSlpPort0Num);
  FchParams->Sata.SataDevSlpPort1Num      = PcdGet8 (PcdSataDevSlpPort1Num);
  FchParams->Sata.SataPhyPllShutDown      = PcdGet8 (PcdSataPhyPllShutDown);
  FchParams->Sata.SataSgpio               = (UINT8)PcdGet32 (PcdSataSgpioHyEx);

  FchParams->Hpet.HpetEnable              = PcdGetBool (PcdHpetEnable);
  FchParams->Hpet.HpetMsiDis              = PcdGetBool (PcdHpetMsiDis);
  FchParams->Hpet.HpetBase                = 0xFED00000;

  FchParams->HwAcpi.SpreadSpectrum        = PcdGetBool (PcdSpreadSpectrum);
  FchParams->HwAcpi.WatchDogTimerBase     = 0xFEB00000;
  FchParams->Misc.NoneSioKbcSupport       = PcdGetBool (PcdNoneSioKbcSupport);
  FchParams->HwAcpi.PwrFailShadow         = PcdGet8 (PcdPwrFailShadow);

  FchParams->HwAcpi.StressResetMode       = PcdGet8 (PcdStressResetMode);
  FchParams->HwAcpi.NoClearThermalTripSts = PcdGetBool (PcdNoClearThermalTripSts);
  FchParams->HwAcpi.OemProgrammingTablePtr     = PcdGetPtr (PcdOemProgrammingTablePtr);
  FchParams->Gpp.SerialDebugBusEnable     = PcdGetBool (PcdSerialDebugBusEnable);
  FchParams->Gcpu.TimerTickTrack          = PcdGet8 (PcdTimerTickTrack);
  FchParams->Gcpu.ClockInterruptTag       = PcdGet8 (PcdClockInterruptTag);
  FchParams->Misc.NativePcieSupport       = PcdGetBool (PcdNativePcieSupport);

  FchParams->Emmc.EmmcEnable              = PcdGetBool (PcdEmmcEnable);
  FchParams->Emmc.BusSpeedMode            = PcdGet8 (PcdEmmcBusSpeedMode);
  FchParams->Emmc.BusWidth                = PcdGet8 (PcdEmmcBusWidth);
  FchParams->Emmc.RetuneMode              = PcdGet8 (PcdEmmcRetuneMode);
  FchParams->Emmc.ClockMultiplier         = PcdGetBool (PcdEmmcClockMultiplier);

  FchParams->Xgbe.XgbeMdio0Enable         = PcdGetBool (PcdXgbeMdio0);
  FchParams->Xgbe.XgbeMdio1Enable         = PcdGetBool (PcdXgbeMdio1);
  FchParams->Xgbe.XgbeSfpEnable           = PcdGetBool (PcdXgbeSfp);
  FchParams->Xgbe.Port[0].XgbePortConfig  = PcdGetBool (PcdXgbePort0ConfigEn);
  if (FchParams->Xgbe.Port[0].XgbePortConfig) {
    PcdDataPtr = PcdGetPtr (PcdXgbePort0Table);
    gBS->CopyMem (
           &FchParams->Xgbe.Port[0],
           PcdDataPtr,
           sizeof (FCH_XGBE_PORT)
           );
  }

  FchParams->Xgbe.Mac[0].XgbePortMacAddressLow  = (UINT32) PcdGet64 (PcdXgbePort0MAC);
  FchParams->Xgbe.Mac[0].XgbePortMacAddressHigh = (UINT32) RShiftU64 (PcdGet64 (PcdXgbePort0MAC), 32);

  FchParams->Xgbe.Port[1].XgbePortConfig  = PcdGetBool (PcdXgbePort1ConfigEn);
  if (FchParams->Xgbe.Port[1].XgbePortConfig) {
    PcdDataPtr = PcdGetPtr (PcdXgbePort1Table);
    gBS->CopyMem (
           &FchParams->Xgbe.Port[1],
           PcdDataPtr,
           sizeof (FCH_XGBE_PORT)
           );
  }

  FchParams->Xgbe.Mac[1].XgbePortMacAddressLow  = (UINT32) PcdGet64 (PcdXgbePort1MAC);
  FchParams->Xgbe.Mac[1].XgbePortMacAddressHigh = (UINT32) RShiftU64 (PcdGet64 (PcdXgbePort1MAC), 32);

  FchParams->Xgbe.Port[2].XgbePortConfig  = PcdGetBool (PcdXgbePort2ConfigEn);
  if (FchParams->Xgbe.Port[2].XgbePortConfig) {
    PcdDataPtr = PcdGetPtr (PcdXgbePort2Table);
    gBS->CopyMem (
           &FchParams->Xgbe.Port[2],
           PcdDataPtr,
           sizeof (FCH_XGBE_PORT)
           );
  }

  FchParams->Xgbe.Mac[2].XgbePortMacAddressLow  = (UINT32) PcdGet64 (PcdXgbePort2MAC);
  FchParams->Xgbe.Mac[2].XgbePortMacAddressHigh = (UINT32) RShiftU64 (PcdGet64 (PcdXgbePort2MAC), 32);

  FchParams->Xgbe.Port[3].XgbePortConfig  = PcdGetBool (PcdXgbePort3ConfigEn);
  if (FchParams->Xgbe.Port[3].XgbePortConfig) {
    PcdDataPtr = PcdGetPtr (PcdXgbePort3Table);
    gBS->CopyMem (
           &FchParams->Xgbe.Port[3],
           PcdDataPtr,
           sizeof (FCH_XGBE_PORT)
           );
  }

  FchParams->Xgbe.Mac[3].XgbePortMacAddressLow  = (UINT32) PcdGet64 (PcdXgbePort3MAC);
  FchParams->Xgbe.Mac[3].XgbePortMacAddressHigh = (UINT32) RShiftU64 (PcdGet64 (PcdXgbePort3MAC), 32);

  DEBUG ((
    EFI_D_INFO,                                      // byo230914 -
    "PcdFchRTDeviceEnableMap=0x%x in %a\n",
    PcdGet32(PcdFchRTDeviceEnableMap),
    __FUNCTION__
    ));
  DEBUG ((
    EFI_D_INFO,                                      // byo230914 -
    "PcdFchAl2AhbLegacyUartIoEnable=0x%x in %a\n",
    PcdGet16(PcdFchAl2AhbLegacyUartIoEnable),
    __FUNCTION__
    ));

  FchParams->FchRunTime.FchDeviceEnableMap = PcdGet32 (PcdFchRTDeviceEnableMap);
  FchParams->FchRunTime.PcieMmioBase       = (UINT32) PcdGet64 (PcdPciExpressBaseAddress);
  if (FchParams->FchRunTime.FchDeviceEnableMap & BIT11) {
    UartLegacy[0] = PcdGet8 (PcdFchUart0LegacyEnable);
  } else {
    UartLegacy[0] = 0;
  }

  if (FchParams->FchRunTime.FchDeviceEnableMap & BIT12) {
    UartLegacy[1] = PcdGet8 (PcdFchUart1LegacyEnable);
  } else {
    UartLegacy[1] = 0;
  }

  if (FchParams->FchRunTime.FchDeviceEnableMap & BIT16) {
    UartLegacy[2] = PcdGet8 (PcdFchUart2LegacyEnable);
  } else {
    UartLegacy[2] = 0;
  }

  if (FchParams->FchRunTime.FchDeviceEnableMap & BIT26) {
    UartLegacy[3] = PcdGet8 (PcdFchUart3LegacyEnable);
  } else {
    UartLegacy[3] = 0;
  }

  IoEnable16 = 0;
  for (UartChannel = 0; UartChannel < 4; UartChannel++ ) {
    if (UartLegacy[UartChannel]) {
      IoEnable16 |=
        (BIT0 << (UartLegacy[UartChannel] - 1)) + (UartChannel << (8 + ((UartLegacy[UartChannel] - 1) * 2)));
    }
  }

  FchParams->FchRunTime.Al2AhbLegacyUartIoEnable = IoEnable16;

  FchParams->Smbus.SmbusSsid              = PcdGet32 (PcdSmbusSsid);
  FchParams->Sata.SataAhciSsid            = PcdGet32 (PcdSataAhciSsid);
  FchParams->Sata.SataIdeSsid             = PcdGet32 (PcdSataIdeSsid);
  FchParams->Sata.SataRaid5Ssid           = PcdGet32 (PcdSataRaid5Ssid);
  FchParams->Sata.SataRaidSsid            = PcdGet32 (PcdSataRaidSsid);
  FchParams->Spi.LpcSsid                  = PcdGet32 (PcdLpcSsid);
  FchParams->Usb.XhciSsid                 = PcdGet32 (PcdXhciSsid);

  return FchParams;
}

//
// Driver Global Data
//

/*********************************************************************************
 * Name: FchDxeHyExInit
 *
 * Description
 *   Entry point of the HYGON FCH DXE driver
 *   Perform the configuration init, resource reservation, early post init
 *   and install all the supported protocol
 *
 * Input
 *   ImageHandle : EFI Image Handle for the DXE driver
 *   SystemTable : pointer to the EFI system table
 *
 * Output
 *   EFI_SUCCESS : Module initialized successfully
 *   EFI_ERROR   : Initialization failed (see error for more details)
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
FchDxeHyExInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  FCH_DXE_PRIVATE     *FchPrivate;
  EFI_STATUS          Status;
  EFI_HANDLE          Handle;
  FCH_DATA_BLOCK      *FchDataBlock;
  EFI_EVENT           PciIoEvent;
  VOID                *Registration;

  HGPI_TESTPOINT (TpFchDxeEntry, NULL);
  //
  // Initialize EFI library
  //

  //
  // Initialize the configuration structure and private data area
  //
  // Allocate memory for the private data
  Status = gBS->AllocatePool (
                  EfiACPIMemoryNVS,
                  sizeof (FCH_DXE_PRIVATE),
                  &FchPrivate
                  );

  ASSERT (!EFI_ERROR (Status));

  Status = gBS->AllocatePool (
                  EfiACPIMemoryNVS,
                  sizeof (FCH_DATA_BLOCK),
                  &FchDataBlock
                  );

  ASSERT (!EFI_ERROR (Status));

  FchDataBlock = FchInitDataBlock (FchDataBlock);
  // Initialize the private data structure
  FchPrivate->Signature = FCH_DXE_PRIVATE_DATA_SIGNATURE;
  // Initialize the FCHInit protocol
  FchPrivate->FchInit.Revision             = FCH_INIT_REV;
  FchPrivate->FchInit.FchRev               = FCH_VERSION;
  FchPrivate->FchInit.FchPolicy            = (VOID*) FchDataBlock;
  FchPrivate->FchInit.FpUsbPortDisable2    = FchUsbDisablePort;
  FchPrivate->FchInit.FpUsbOverCurrent2    = FchUsbOvercurrentControl;

  //
  // Publish the FCHInit protocol
  //
  Handle = ImageHandle;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gFchInitProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &FchPrivate->FchInit
                  );

  if (EFI_ERROR (Status)) {
    return (Status);
  }

  //
  // Enviroment Init Entry
  //
  DEBUG ((DEBUG_INFO, "[FchInitEnv] Fch Init - Before PCI Scan ...Start\n"));
  Status = FchInitEnv (FchDataBlock);
  DEBUG ((DEBUG_INFO, "[FchInitPei] Fch Init - Before PCI Scan ...complete\n"));

  //
  // Register the event handling function for FchInitMid to be launched after
  // PciIo protocol
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  InvokeFchInitMid,
                  FchPrivate->FchInit.FchPolicy,
                  NULL,
                  &PciIoEvent
                  );

  Status = gBS->RegisterProtocolNotify (
                  &gEfiPciIoProtocolGuid,
                  PciIoEvent,
                  &Registration
                  );

  //
  // Register the event handling function for FchInitLate to be launched after
  // Ready to Boot
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             InvokeFchInitLate,
             FchPrivate->FchInit.FchPolicy,
             &FchPrivate->EventReadyToBoot
             );

  //
  // Install gFchInitDonePolicyProtocolGuid to signal Platform
  //
  Handle = ImageHandle;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gFchInitDonePolicyProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );

  HGPI_TESTPOINT (TpFchDxeExit, NULL);
  return (Status);
}

EFI_STATUS
EFIAPI
FchUsbDisablePort (
  IN       CONST FCH_INIT_PROTOCOL   *This,
  IN       UINT8                     Socket,
  IN       UINT8                     PhysicalDieId,
  IN       UINT8                     UsbIndex,
  IN       UINT32                    USB3DisableMap,
  IN       UINT32                    USB2DisableMap
  )
{
  if (FchXhciDisablePortSoc (Socket, PhysicalDieId, UsbIndex, USB3DisableMap, USB2DisableMap)) {
    return EFI_SUCCESS;
  } else {
    return EFI_UNSUPPORTED;
  }
}

EFI_STATUS
EFIAPI
FchUsbOvercurrentControl (
  IN       CONST FCH_INIT_PROTOCOL   *This,
  IN       UINT8                     Socket,
  IN       UINT8                     PhysicalDieId,
  IN       UINT8                     UsbIndex,
  IN       UINT8                     Port,
  IN       UINT8                     OCPin
  )
{
  if (FchXhciOCmappingSoc (Socket, PhysicalDieId, UsbIndex, Port, OCPin)) {
    return EFI_SUCCESS;
  } else {
    return EFI_UNSUPPORTED;
  }
}
