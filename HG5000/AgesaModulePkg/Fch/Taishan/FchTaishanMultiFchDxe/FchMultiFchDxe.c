/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH DXE Driver
 * @e \$Revision: 309083 $   @e \$Date: 2014-12-09 09:28:24 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 ******************************************************************************
 */
#include "FchMultiFchDxe.h"

#define MAX_DIE_SUPPORT 8
//#define AMD_VENDOR_ID   0x1022

#define DF_CFGADDRESSCNTL_FUNC    0x00
#define DF_CFGADDRESSCNTL_OFFSET  0x84

extern EFI_GUID gFchMultiFchResetDataHobGuid;

//
// Driver Global Data
//


/*********************************************************************************
 * Name: MultiFchDxeInit
 *
 * Description
 *   Entry point of the AMD FCH MCM DXE driver
 *   Perform the configuration init, resource reservation, early post init
 *   and install all the supported protocol for FCH MCM support
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
MultiFchDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  UINT8               Die;
  UINT8               DieBusNum;
  UINT16              VendorID;
  UINT8               SataMultiDieEnable;
  UINT8               SataSgpioMultiDieEnable;
  UINT64              SataMultiDiePortShutDown;
  UINT64              SataMultiDiePortESP;
  UINT64              SataMultiDieDevSlp;
  UINT64              SataMultiDiePortMode;            // winddy +
  UINT64              SataMultiDiePortMode2;           // winddy +
  FCH_INIT_PROTOCOL   *FchInit;
  FCH_DATA_BLOCK      *FchDataPtr;
  EFI_STATUS          Status;
  EFI_HANDLE          Handle;
  EFI_HOB_GUID_TYPE          *FchHob;
  FCH_MULTI_FCH_DXE_PRIVATE  *FchMfPrivate;
  FCH_MULITI_FCH_DATA_BLOCK  *FchMfData;

  AGESA_TESTPOINT (TpFchMultiFchDxeEntry, NULL);
  //
  // Initialize EFI library
  //

  //
  // Initialize the configuration structure and private data area
  //
  //find HOB and update with reset data block
  FchHob = GetFirstGuidHob (&gFchMultiFchResetDataHobGuid);
  FchHob++;

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (FCH_MULTI_FCH_DXE_PRIVATE),
                  &FchMfPrivate
                  );

  ASSERT_EFI_ERROR (Status);

  // Update Private Data
  ZeroMem (FchMfPrivate, sizeof (FCH_MULTI_FCH_DXE_PRIVATE));
  FchMfPrivate->Signature = FCH_MULTI_FCH_DXE_PRIVATE_DATA_SIGNATURE;
  FchMfPrivate->FchMultiFchInit.Revision     = FCH_MULTI_FCH_INIT_REV;
  FchMfData = &FchMfPrivate->FchMultiFchInit.FchMfData;
  gBS->CopyMem (
         FchMfData,
         FchHob,
         sizeof (FCH_MULITI_FCH_DATA_BLOCK)
         );

  //
  // Locate Fch INIT Protocol
  //
  Status = gBS->LocateProtocol (
                  &gFchInitProtocolGuid,
                  NULL,
                  &FchInit
                  );
  ASSERT_EFI_ERROR (Status);

  // Allocate memory for the local data
  Status = gBS->AllocatePool (
                  EfiRuntimeServicesData,   //review for memory type
                  sizeof (FCH_DATA_BLOCK),
                  &FchDataPtr
                  );
  ASSERT_EFI_ERROR (Status);

  gBS->CopyMem (
         FchDataPtr,
         FchInit->FchPolicy,
         sizeof (FCH_DATA_BLOCK)
         );

  //
  // Update local Data Structure
  //
  SataMultiDieEnable = PcdGet8 (PcdSataMultiDieEnable);
  SataMultiDiePortShutDown = PcdGet64 (PcdSataMultiDiePortShutDown);
  SataMultiDiePortESP = PcdGet64 (PcdSataMultiDiePortESP);
  SataMultiDieDevSlp = PcdGet64 (PcdSataMultiDieDevSlp);
  SataSgpioMultiDieEnable = PcdGet8 (PcdSataSgpioMultiDieEnable);
  SataMultiDiePortMode  = PcdGet64 (PcdSataMultiDiePortMode);             // winddy +
  SataMultiDiePortMode2 = (SataMultiDiePortMode >> 32) & 0xFFFFFFFF;      // winddy +
  SataMultiDiePortMode  = SataMultiDiePortMode & 0xFFFFFFFF;              // winddy +
  
  //
  // Check Die#
  //
  for (Die = 1; Die < MAX_DIE_SUPPORT; Die++) {
    //
    // Update local Data Structure for each Die
    //
    SataMultiDieEnable = SataMultiDieEnable >> 1;
    SataMultiDiePortShutDown = SataMultiDiePortShutDown >> 8;
    SataMultiDiePortESP = SataMultiDiePortESP >> 8;
    SataMultiDieDevSlp = SataMultiDieDevSlp >> 8;
    SataSgpioMultiDieEnable = SataSgpioMultiDieEnable >> 1;

// winddy + >>
    if(Die == 4){
      SataMultiDiePortMode = SataMultiDiePortMode2;
    } else {
      SataMultiDiePortMode = SataMultiDiePortMode >> 16;
    }
// winddy + <<

    FchDataPtr->Sata.SataMode.SataEnable = SataMultiDieEnable & BIT0;
    FchDataPtr->Sata.SataPortPower.SataPortReg = (UINT8)SataMultiDiePortShutDown;
    FchDataPtr->Sata.SataEspPort.SataPortReg = (UINT8)SataMultiDiePortESP;
    FchDataPtr->Sata.SataDevSlpPort0 = SataMultiDieDevSlp & BIT0;
    FchDataPtr->Sata.SataDevSlpPort0Num = (UINT8)((SataMultiDieDevSlp >> 1) & 0x7);
    FchDataPtr->Sata.SataDevSlpPort1 = SataMultiDieDevSlp & BIT4;
    FchDataPtr->Sata.SataDevSlpPort1Num = (UINT8)((SataMultiDieDevSlp >> 5) & 0x7);
    FchDataPtr->Sata.SataSgpio0 = SataSgpioMultiDieEnable & BIT0;
    FchDataPtr->HwAcpi.FchAcpiMmioBase = (UINT32) FchMfData->FchAcpiMmioBase[Die];
    FchDataPtr->Sata.SataPortMd.SataPortMode = (UINT16)SataMultiDiePortMode;             // winddy +

    //DF devices on bus0
    LibFchPciRead (EfiPciWidthUint16, ((((Die + 24) << 3) + 0) << 16) + 0x00, &VendorID);

    //if (VendorID != AMD_VENDOR_ID) {
    if (VendorID != AMD_VID && VendorID != HYGON_VID ) {
      continue;
    }

    LibFchPciRead (EfiPciWidthUint8, ((((Die + 24) << 3) + DF_CFGADDRESSCNTL_FUNC) << 16) + DF_CFGADDRESSCNTL_OFFSET, &DieBusNum);

    // nBIF1 at dev8 function1
    LibFchPciRead (EfiPciWidthUint16, (((DieBusNum << 8) + (0x8 << 3) + 0x1) << 16) + 0x00, &VendorID);
    //if (VendorID != AMD_VENDOR_ID) {
    if (VendorID != AMD_VID && VendorID != HYGON_VID ) {
      //
      // Disable Sata and xGbE if nBIF1 is disabled
      //
      FchSmnRW ((UINT32)DieBusNum, FCH_SMN_SATA_NBIF_STRAP0, ~(UINT32)BIT28, 0, NULL);

      FchSmnRW ((UINT32)DieBusNum, FCH_SMN_XGBE0_NBIF_STRAP0, ~(UINT32)BIT28, 0, NULL);
      FchSmnRW ((UINT32)DieBusNum, FCH_SMN_XGBE0_NBIF_STRAP0 + 0x200, ~(UINT32)BIT28, 0, NULL);
      FchSmnRW ((UINT32)DieBusNum, FCH_SMN_XGBE0_NBIF_STRAP0 + 0x400, ~(UINT32)BIT28, 0, NULL);
      FchSmnRW ((UINT32)DieBusNum, FCH_SMN_XGBE0_NBIF_STRAP0 + 0x600, ~(UINT32)BIT28, 0, NULL);
      continue;
    }

    //Secondary Fch init and MMIO relocation

    //USB
    FchTSSecondaryFchInitUsbDxe (Die, DieBusNum, FchDataPtr);      // wz200307 +
    //SATA
    FchTSSecondaryFchInitSataDxe (Die, DieBusNum, FchDataPtr);
    //Ethernet
    FchTSSecondaryFchInitXgbeDxe (Die, DieBusNum, FchDataPtr);

  }

  //
  // Install gFchMultiFchInitProtocolGuid to signal Platform
  //
  Handle = ImageHandle;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gFchMultiFchInitProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &FchMfPrivate->FchMultiFchInit
                  );
  ASSERT_EFI_ERROR (Status);
  //
  // Register the event handling function for FchInitLate to be launched after
  // Ready to Boot
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             MultiFchInitRtb,
             NULL,
             &FchMfPrivate->EventReadyToBoot
             );

  Status = gBS->FreePool (FchDataPtr);
  ASSERT_EFI_ERROR (Status);

  AGESA_TESTPOINT (TpFchMultiFchDxeExit, NULL);
  return (Status);
}



// wz200307 + >>
VOID
FchTSSecondaryFchInitUsbDxe (
  IN  UINT8       Die,
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  FCH_DATA_BLOCK            *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;

  if (LocalCfgPtr->Usb.XhciSsid != 0) {
    FchSmnRW (DieBusNum, FCH_TS_USB_SMN_PCICFG + 0x4C, 0x00, LocalCfgPtr->Usb.XhciSsid, NULL);
  }
}
// wz200307 + <<



VOID
FchTSSecondaryFchInitSataDxe (
  IN  UINT8       Die,
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  UINT8                     PortNum;
  UINT32                    DieBusNum32;
  UINT32                    SataNbifStrap0;
  FCH_DATA_BLOCK            *LocalCfgPtr;

  DieBusNum32 = (UINT32)DieBusNum;
  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;

  //
  // Check if Sata is enabled by NBIO
  //
  FchSmnRead (DieBusNum32, FCH_SMN_SATA_NBIF_STRAP0, &SataNbifStrap0, NULL);

  if ((SataNbifStrap0 & BIT28) == 0) {
    FchSmnRW (DieBusNum32, FCH_TS_SATA_MISC_CONTROL, 0xfffffffe, BIT0, NULL);
    FchSmnRW (DieBusNum32, FCH_TS_SATA_MISC_CONTROL, ~(UINT32)BIT11, BIT11, NULL);
    return;
  }

  if ( LocalCfgPtr->Sata.SataMode.SataEnable == 0 ) {
    FchSmnRW (DieBusNum32, FCH_SMN_SATA_NBIF_STRAP0, ~(UINT32)BIT28, 0, NULL);
    return;                                                //return if SATA controller is disabled.
  }

  //
  // Init Sata
  //
  SataEnableWriteAccessTS (DieBusNum32, LocalCfgPtr);

  for (PortNum = 0; PortNum < TAISHAN_SATA_PORT_NUM; PortNum++) {
    if (((LocalCfgPtr->Sata.SataPortPower.SataPortReg >> PortNum) & BIT0) == 0) {
      FchTSSataInitPortActive (DieBusNum32, PortNum, FchDataPtr);
    }
  }

  // Do Sata init
  FchTSSataInitRsmuCtrl (DieBusNum32, LocalCfgPtr);

  FchTSSataInitCtrlReg (DieBusNum32, LocalCfgPtr);

  FchTSSataInitEsata (DieBusNum32, LocalCfgPtr);

  FchTSSataInitDevSlp (DieBusNum32, LocalCfgPtr);

  FchTSSataInitMpssMap (DieBusNum32, LocalCfgPtr);

  FchTSSataInitEnableErr (DieBusNum32, LocalCfgPtr);

  FchTSSataSetPortGenMode (DieBusNum32, LocalCfgPtr);

  if (LocalCfgPtr->Sata.SataSgpio0) {
    FchTSSataGpioInitial (DieBusNum32, LocalCfgPtr);
  }

  if (( LocalCfgPtr->Sata.SataClass == SataAhci7804) || (LocalCfgPtr->Sata.SataClass == SataAhci )) {
    FchInitEnvSataAhciTS (DieBusNum32, LocalCfgPtr );
  }

  if ( LocalCfgPtr->Sata.SataClass == SataRaid) {
    FchInitEnvSataRaidTS (DieBusNum32, LocalCfgPtr );
  }

  SataDisableWriteAccessTS (DieBusNum32, LocalCfgPtr);
}

VOID
FchTSSecondaryFchInitXgbeDxe (
  IN  UINT8       Die,
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  UINT8                     PortNum;
  UINT32                    DieBusNum32;
  UINT32                    SystemPort;
  UINT32                    XgbeNbifStrap0;
  FCH_XGBE_PORT             *FchXgbePort;
  FCH_XGBE_MAC              *FchXgbePortMac;
  FCH_DATA_BLOCK            *LocalCfgPtr;

  DieBusNum32 = (UINT32)DieBusNum;
  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  FchXgbePort = &LocalCfgPtr->Xgbe.Port[0];
  FchXgbePortMac = &LocalCfgPtr->Xgbe.Mac[0];

  for (PortNum = 0; PortNum < 4; PortNum++) {
    //
    // Check if GMAC is enabled
    //
    FchSmnRead (DieBusNum32, FCH_SMN_XGBE0_NBIF_STRAP0 + PortNum * 0x200, &XgbeNbifStrap0, NULL);

    if (XgbeNbifStrap0 & BIT28) {
      SystemPort = LocalCfgPtr->Xgbe.MaxPortNum;
      LocalCfgPtr->Xgbe.MaxPortNum++;
      if (FchXgbePort->XgbePortConfig) {
        FchTsXgbePortPlatformConfig (DieBusNum32, PortNum, SystemPort, FchXgbePort);
        FchTsXgbePortMacAddress (DieBusNum32, PortNum, FchXgbePortMac);
        FchTsXgbePortPtpInit (DieBusNum32, PortNum, FchXgbePort);
      }
    } else {
      FchTsXgbePortClockGate (DieBusNum32, PortNum);
    }
    FchXgbePort++;
    FchXgbePortMac++;
  }

  FchTsXgbePadEnable (DieBusNum32, LocalCfgPtr);
}

VOID
MultiFchInitRtb (
  IN       EFI_EVENT        Event,
  IN       VOID             *Context
  )
{
  UINT8               Die;
  UINT8               DieBusNum;
  UINT16              VendorID;
  UINT8               SataMultiDieEnable;
  UINT8               SataSgpioMultiDieEnable;
  UINT64              SataMultiDiePortShutDown;
  UINT64              SataMultiDiePortESP;
  UINT64              SataMultiDieDevSlp;
  FCH_INIT_PROTOCOL   *FchInit;
  FCH_DATA_BLOCK      *FchDataPtr;
  EFI_STATUS          Status;
  FCH_MULTI_FCH_INIT_PROTOCOL  *FchMfInit;

  //
  // Locate Fch INIT Protocol
  //
  Status = gBS->LocateProtocol (
                  &gFchInitProtocolGuid,
                  NULL,
                  &FchInit
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (
                  &gFchMultiFchInitProtocolGuid,
                  NULL,
                  &FchMfInit
                  );
  ASSERT_EFI_ERROR (Status);

  // Allocate memory for the local data
  Status = gBS->AllocatePool (
                  EfiRuntimeServicesData,   //review for memory type
                  sizeof (FCH_DATA_BLOCK),
                  &FchDataPtr
                  );
  ASSERT_EFI_ERROR (Status);

  gBS->CopyMem (
         FchDataPtr,
         FchInit->FchPolicy,
         sizeof (FCH_DATA_BLOCK)
         );

  //
  // Update local Data Structure
  //
  SataMultiDieEnable = PcdGet8 (PcdSataMultiDieEnable);
  SataMultiDiePortShutDown = PcdGet64 (PcdSataMultiDiePortShutDown);
  SataMultiDiePortESP = PcdGet64 (PcdSataMultiDiePortESP);
  SataMultiDieDevSlp = PcdGet64 (PcdSataMultiDieDevSlp);
  SataSgpioMultiDieEnable = PcdGet8 (PcdSataSgpioMultiDieEnable);

  //
  // Check Die#
  //
  for (Die = 1; Die < MAX_DIE_SUPPORT; Die++) {
    //
    // Update local Data Structure for each Die
    //
    SataMultiDieEnable = SataMultiDieEnable >> 1;
    SataMultiDiePortShutDown = SataMultiDiePortShutDown >> 8;
    SataMultiDiePortESP = SataMultiDiePortESP >> 8;
    SataMultiDieDevSlp = SataMultiDieDevSlp >> 8;
    SataSgpioMultiDieEnable = SataSgpioMultiDieEnable >> 1;

    FchDataPtr->Sata.SataMode.SataEnable = SataMultiDieEnable & BIT0;
    FchDataPtr->Sata.SataPortPower.SataPortReg = (UINT8)SataMultiDiePortShutDown;
    FchDataPtr->Sata.SataEspPort.SataPortReg = (UINT8)SataMultiDiePortESP;
    FchDataPtr->Sata.SataDevSlpPort0 = SataMultiDieDevSlp & BIT0;
    FchDataPtr->Sata.SataDevSlpPort0Num = (UINT8)((SataMultiDieDevSlp >> 1) & 0x7);
    FchDataPtr->Sata.SataDevSlpPort1 = SataMultiDieDevSlp & BIT4;
    FchDataPtr->Sata.SataDevSlpPort1Num = (UINT8)((SataMultiDieDevSlp >> 5) & 0x7);
    FchDataPtr->Sata.SataSgpio0 = SataSgpioMultiDieEnable & BIT0;
    FchDataPtr->HwAcpi.FchAcpiMmioBase = (UINT32) FchMfInit->FchMfData.FchAcpiMmioBase[Die];

    //DF devices on bus0
    LibFchPciRead (EfiPciWidthUint16, ((((Die + 24) << 3) + 0) << 16) + 0x00, &VendorID);

    //if (VendorID != AMD_VENDOR_ID) {
    if (VendorID != AMD_VID && VendorID != HYGON_VID ) {
      continue;
    }

    LibFchPciRead (EfiPciWidthUint8, ((((Die + 24) << 3) + DF_CFGADDRESSCNTL_FUNC) << 16) + DF_CFGADDRESSCNTL_OFFSET, &DieBusNum);

    //
    // Update local Data Structure for each Die
    //

    //Secondary Fch init and MMIO relocation

    //USB

    //SATA
    FchTSSecondaryFchInitSataRtb (Die, DieBusNum, FchDataPtr);
    //Ethernet

  }

  gBS->CloseEvent (Event);
}

VOID
FchTSSecondaryFchInitSataRtb (
  IN  UINT8       Die,
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  UINT32                    DieBusNum32;
  UINT32                    SataNbifStrap0;
  FCH_DATA_BLOCK            *LocalCfgPtr;

  DieBusNum32 = (UINT32)DieBusNum;
  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;

  //
  // Check if Sata is enabled by NBIO
  //
  FchSmnRead (DieBusNum32, FCH_SMN_SATA_NBIF_STRAP0, &SataNbifStrap0, NULL);

  if ((SataNbifStrap0 & BIT28) == 0) {
    return;
  }

  //
  // Set Sata PCI Configuration Space Write enable
  //
  SataEnableWriteAccessTS (DieBusNum32, LocalCfgPtr);
  //
  // Call Sub-function for each Sata mode
  //
  if (( LocalCfgPtr->Sata.SataClass == SataAhci7804) || (LocalCfgPtr->Sata.SataClass == SataAhci )) {
    FchInitLateSataAhci (DieBusNum32, LocalCfgPtr );
  }

  if ( LocalCfgPtr->Sata.SataClass == SataRaid) {
    FchInitLateSataRaid (DieBusNum32, LocalCfgPtr );
  }

  FchTSSataInitMMC (DieBusNum32, LocalCfgPtr);
  //
  // Set Sata PCI Configuration Space Write disable
  //
  SataDisableWriteAccessTS (DieBusNum32, LocalCfgPtr);
}
