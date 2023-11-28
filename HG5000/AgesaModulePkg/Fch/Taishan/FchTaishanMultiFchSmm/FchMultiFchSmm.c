/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH SMM Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH SMM Driver
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
#include "FchMultiFchSmm.h"

#define MAX_DIE_SUPPORT 8
#define AMD_VENDOR_ID   0x1022

#define DF_CFGADDRESSCNTL_FUNC    0x00
#define DF_CFGADDRESSCNTL_OFFSET  0x84
//
// Driver Global Data
//
UINT32 UsbDieBusNum[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
UINT8  SmmPcdSataMultiDieEnable;
UINT8  SmmPcdSataSgpioMultiDieEnable;
UINT64 SmmPcdSataMultiDiePortShutDown;
UINT64 SmmPcdSataMultiDiePortESP;
UINT64 SmmPcdSataMultiDieDevSlp;
UINT64 SmmPcdSataMultiDiePortMode;                              // winddy +
FCH_SMM_INIT_PROTOCOL *pFchSmmInitProtocol;
FCH_MULITI_FCH_DATA_BLOCK  FchSmmMfData;
/*********************************************************************************
 * Name: FchSmmEntryPoint
 *
 * Description
 *   Entry point of the AMD FCH SMM driver
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
MultiFchSmmInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  FCH_MULTI_FCH_INIT_PROTOCOL *FchMfInit;
  EFI_STATUS                  Status;

  Status = gBS->LocateProtocol (
                  &gFchMultiFchInitProtocolGuid,
                  NULL,
                  &FchMfInit
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmInitProtocolGuid,
                  NULL,
                  &pFchSmmInitProtocol
                  );
  if (EFI_ERROR (Status)) {
	return Status;
  }

  SmmPcdSataMultiDieEnable = PcdGet8 (PcdSataMultiDieEnable);
  SmmPcdSataMultiDiePortShutDown = PcdGet64 (PcdSataMultiDiePortShutDown);
  SmmPcdSataMultiDiePortESP = PcdGet64 (PcdSataMultiDiePortESP);
  SmmPcdSataMultiDieDevSlp = PcdGet64 (PcdSataMultiDieDevSlp);
  SmmPcdSataSgpioMultiDieEnable = PcdGet8 (PcdSataSgpioMultiDieEnable);
  SmmPcdSataMultiDiePortMode = PcdGet64 (PcdSataMultiDiePortMode);             // winddy +

  gBS->CopyMem (
         &FchSmmMfData,
         &FchMfInit->FchMfData,
         sizeof (FCH_MULITI_FCH_DATA_BLOCK)
         );

  Status = MultiFchSmmRegisterSxSmi ();

  Status = MultiFchSmmRegisterSwSmi ();

  return Status;
}

EFI_STATUS
MultiFchSmmRegisterSxSmi (
  VOID
  )
{
  EFI_STATUS                               Status;
  FCH_SMM_SX_DISPATCH2_PROTOCOL            *AmdSxDispatch;
  FCH_SMM_SX_REGISTER_CONTEXT              SxRegisterContext;
  EFI_HANDLE                               SxHandle;

  MultiFchSmmDieBusInfo ();

  //
  // Register AMD SX SMM
  //
  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmSxDispatch2ProtocolGuid,
                  NULL,
                  &AmdSxDispatch
                  );
  ASSERT_EFI_ERROR (Status);

  SxRegisterContext.Type  = SxS3;
  SxRegisterContext.Phase = SxEntry;
  SxRegisterContext.Order = 1;

  Status = AmdSxDispatch->Register (
                             AmdSxDispatch,
                             MultiFchS3SleepEntryCallback,
                             &SxRegisterContext,
                             &SxHandle
                             );
  return Status;
}

EFI_STATUS
MultiFchSmmRegisterSwSmi (
  VOID
  )
{
  EFI_STATUS                               Status;
  FCH_SMM_SW_DISPATCH2_PROTOCOL            *AmdSwDispatch;
  FCH_SMM_SW_REGISTER_CONTEXT              SwRegisterContext;
  EFI_HANDLE                               SwHandle;

  //
  //  Locate SMM SW dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmSwDispatch2ProtocolGuid,
                  NULL,
                  &AmdSwDispatch
                  );
  ASSERT_EFI_ERROR (Status);

  SwRegisterContext.AmdSwValue  = PcdGet8(PcdFchOemBeforePciRestoreSwSmi); // use of PCD in place of FCHOEM_BEFORE_PCI_RESTORE_SWSMI    0xD3
  SwRegisterContext.Order       = 0x90;
  Status = AmdSwDispatch->Register (
                            AmdSwDispatch,
                            MultiFchBeforePciS3RestoreCallback,
                            &SwRegisterContext,
                            &SwHandle
                            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SwRegisterContext.AmdSwValue  = PcdGet8(PcdFchOemAfterPciRestoreSwSmi); // use of PCD in place of FCHOEM_AFTER_PCI_RESTORE_SWSMI    0xD4
  SwRegisterContext.Order       = 0x90;
  Status = AmdSwDispatch->Register (
                            AmdSwDispatch,
                            MultiFchAfterPciS3RestoreCallback,
                            &SwRegisterContext,
                            &SwHandle
                            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}

/*++

Routine Description:
    USB S3entry programming for Slave die

--*/
EFI_STATUS
EFIAPI
MultiFchS3SleepEntryCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SX_REGISTER_CONTEXT *DispatchContext,
  IN OUT   VOID                              *CommBuffer OPTIONAL,
  IN OUT   UINTN                             *CommBufferSize  OPTIONAL
  )
{
  UINT8                     Index;
  FCH_RESET_DATA_BLOCK      FchPolicy;

  for (Index = 0; Index < 8; Index++) {
    if (UsbDieBusNum[Index] == 0xFF) {
      return EFI_SUCCESS;
    }
    FchTSXhciInitS3EntryProgram (UsbDieBusNum[Index], &FchPolicy);
  }
  return EFI_SUCCESS;
}

VOID
MultiFchSmmDieBusInfo (
  VOID
  )
{
  UINT8               Die;
  UINT8               DieBusNum;
  UINT8               Index;
  UINT16              VendorID;
  UINT32              UsbNbifStrap0;
  UINTN               PciAddress;

  Index = 0;
  //
  // Check Die#
  //
  for (Die = 1; Die < MAX_DIE_SUPPORT; Die++) {
    //DF devices on bus0
    PciAddress = ((Die + 24) << 15) + 0x000;
    VendorID = PciRead16 (PciAddress);
    if ((VendorID != AMD_VENDOR_ID) && (VendorID != HYGON_VID)) {
      continue;
    }

    PciAddress = ((Die + 24) << 15) + (DF_CFGADDRESSCNTL_FUNC << 12) + DF_CFGADDRESSCNTL_OFFSET;
    DieBusNum = PciRead8 (PciAddress);

    //
    // Save Usb Die Bus info
    //
    FchSmnRead ((UINT32)DieBusNum, FCH_TS_USB_NBIF_STRAP0, &UsbNbifStrap0, NULL);
    if (UsbNbifStrap0 & BIT28) {
      UsbDieBusNum [Index] = (UINT32)DieBusNum;
      Index++;
    }
  }
}

/*++

Routine Description:
  SMI handler to restore Slave Fch

--*/
EFI_STATUS
EFIAPI
MultiFchBeforePciS3RestoreCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{
  FCH_DATA_BLOCK      *FchDataPtr;
  EFI_STATUS          Status;
  UINT8               Die;
  UINT8               DieBusNum;
  UINT16              VendorID;
  UINT8               SataMultiDieEnable;
  UINT8               SataSgpioMultiDieEnable;
  UINT64              SataMultiDiePortShutDown;
  UINT64              SataMultiDiePortESP;
  UINT64              SataMultiDieDevSlp;
  UINT64              SataMultiDiePortMode;                               // winddy +
  UINT64              SataMultiDiePortMode2;                              // winddy +
  UINTN               PciAddress;

  Status = EFI_SUCCESS;

  SataMultiDieEnable = SmmPcdSataMultiDieEnable;
  SataMultiDiePortShutDown = SmmPcdSataMultiDiePortShutDown;
  SataMultiDiePortESP = SmmPcdSataMultiDiePortESP;
  SataMultiDieDevSlp = SmmPcdSataMultiDieDevSlp;
  SataSgpioMultiDieEnable = SmmPcdSataSgpioMultiDieEnable;
  SataMultiDiePortMode = SmmPcdSataMultiDiePortMode;                      // winddy +

  SataMultiDiePortMode2 = (SataMultiDiePortMode >> 32) & 0xFFFFFFFF;      // winddy +
  SataMultiDiePortMode  = SataMultiDiePortMode & 0xFFFFFFFF;              // winddy +

  FchDataPtr = (FCH_DATA_BLOCK *)(pFchSmmInitProtocol->FchSmmPolicy);
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
    FchDataPtr->HwAcpi.FchAcpiMmioBase = (UINT32) FchSmmMfData.FchAcpiMmioBase[Die];
    FchDataPtr->Sata.SataPortMd.SataPortMode = (UINT16)SataMultiDiePortMode;           // winddy +

    //DF devices on bus0
    PciAddress = ((Die + 24) << 15) + 0x000;
    VendorID = PciRead16 (PciAddress);
    if ((VendorID != AMD_VENDOR_ID) && (VendorID != HYGON_VID)) {
      continue;
    }

    PciAddress = ((Die + 24) << 15) + (DF_CFGADDRESSCNTL_FUNC << 12) + DF_CFGADDRESSCNTL_OFFSET;
    DieBusNum = PciRead8 (PciAddress);

    // nBIF1 at dev8 function1
    PciAddress = (DieBusNum << 20) + (0x8 << 15) + (0x1 << 12);
    VendorID = PciRead16 (PciAddress);
    if ((VendorID != AMD_VENDOR_ID) && (VendorID != HYGON_VID)) {
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

    //SATA
    MultiFchSataInitSmm (Die, DieBusNum, FchDataPtr);
    //Ethernet
    MultiFchXgbeInitSmm (Die, DieBusNum, FchDataPtr);
  }
  return Status;
}

/*++

Routine Description:
  SMI handler to restore Slave Fch

--*/
EFI_STATUS
EFIAPI
MultiFchAfterPciS3RestoreCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{
  FCH_DATA_BLOCK      *FchDataPtr;
  EFI_STATUS          Status;
  UINT8               Die;
  UINT8               DieBusNum;
  UINT16              VendorID;
  UINT8               SataMultiDieEnable;
  UINT8               SataSgpioMultiDieEnable;
  UINT64              SataMultiDiePortShutDown;
  UINT64              SataMultiDiePortESP;
  UINT64              SataMultiDieDevSlp;
  UINTN               PciAddress;

  Status = EFI_SUCCESS;
  SataMultiDieEnable = SmmPcdSataMultiDieEnable;
  SataMultiDiePortShutDown = SmmPcdSataMultiDiePortShutDown;
  SataMultiDiePortESP = SmmPcdSataMultiDiePortESP;
  SataMultiDieDevSlp = SmmPcdSataMultiDieDevSlp;
  SataSgpioMultiDieEnable = SmmPcdSataSgpioMultiDieEnable;
  FchDataPtr = (FCH_DATA_BLOCK *)(pFchSmmInitProtocol->FchSmmPolicy);

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
    FchDataPtr->HwAcpi.FchAcpiMmioBase = (UINT32) FchSmmMfData.FchAcpiMmioBase[Die];

    //DF devices on bus0
    PciAddress = ((Die + 24) << 15) + 0x000;
    VendorID = PciRead16 (PciAddress);
    if ((VendorID != AMD_VENDOR_ID) && (VendorID != HYGON_VID)) {
      continue;
    }

    PciAddress = ((Die + 24) << 15) + (DF_CFGADDRESSCNTL_FUNC << 12) + DF_CFGADDRESSCNTL_OFFSET;
    DieBusNum = PciRead8 (PciAddress);

    //SATA
    MultiFchSataInit2Smm (Die, DieBusNum, FchDataPtr);
  }
  return Status;
}

VOID
MultiFchSataInitSmm (
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
MultiFchXgbeInitSmm (
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
MultiFchSataInit2Smm (
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

  FchTSSataShutdownUnconnectedSataPortClock (DieBusNum32, LocalCfgPtr);

  FchTSSataInitMMC (DieBusNum32, LocalCfgPtr);
  //
  // Set Sata PCI Configuration Space Write disable
  //
  SataDisableWriteAccessTS (DieBusNum32, LocalCfgPtr);
}

VOID
MultiFchAcpiClearStatusOnSxEntry (
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT8       PkgType;
  UINT32      RegEbx;

  //
  // MCM checking
  //
  AsmCpuid (0x80000001, NULL, &RegEbx, NULL, NULL);
  PkgType = (UINT8) (RegEbx >> 28);
  if (PkgType == PKG_SP3_R2) {
  // Clear SCI Status on slave die
    RwMem ((UINT32) (FchSmmMfData.FchAcpiMmioBase[1] + SMI_BASE + FCH_SMI_REG84), AccessWidth32, 0x00, 0xFFFFFFFF);
    RwMem ((UINT32) (FchSmmMfData.FchAcpiMmioBase[1] + SMI_BASE + FCH_SMI_REG00), AccessWidth32, 0x00, 0xFFFFFFFF);
  }
}

