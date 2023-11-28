/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH PEIM
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH PEIM
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
#include "FchMultiFchPei.h"

#define MAX_DIE_SUPPORT 8
#define MAX_DIES_PER_SOCKET     4
//#define AMD_VENDOR_ID   0x1022

#define DF_CFGADDRESSCNTL_FUNC    0x00
#define DF_CFGADDRESSCNTL_OFFSET  0x84

#define DF_X86IOBASE_FUNC    0x00
#define DF_X86IOBASE_OFFSET  0xC0

#define DF_SPECIALSYSFUNCFBID_FUNC    0x01
#define DF_SPECIALSYSFUNCFBID_OFFSET  0x60

extern EFI_GUID gFchMultiFchResetDataHobGuid;


//
// Module globals
//

/*********************************************************************************
 * Name: MultiFchPeiInit
 *
 * Description:
 *
 * Input
 *   FfsHeader   : pointer to the firmware file system header
 *   PeiServices : pointer to the PEI service table
 *
 * Output
 *   EFI_SUCCESS : Module initialized successfully
 *   EFI_ERROR   : Initialization failed (see error for more details)
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
MultiFchPeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  UINT8                     Die;
  UINT8                     DieBusNum;
  UINT8                     SataMultiDieEnable;
  UINT16                    VendorID;
  UINT32                    IOBaseRegVal_Org;
  UINT32                    IOLimitRegVal_Org;
  UINT32                    IOBaseRegVal_New;
  UINT32                    IOLimitRegVal_New;
  UINT64                    PeiPciAddress;
  EFI_STATUS                Status;
  EFI_HOB_GUID_TYPE         *FchHob;
  AMD_FCH_INIT_PPI          *FchInitPpi;
  FCH_RESET_DATA_BLOCK      *FchResetDataPtr;
  FCH_MULITI_FCH_RESET_DATA_BLOCK  *FchMfResetData;
  FCH_MULTI_FCH_PEI_PRIVATE *FchMultiFchPeiPrivate;
  EFI_PEI_PPI_DESCRIPTOR    *PpiListMultiFchInit;

  AGESA_TESTPOINT (TpFchMultiFchPeiEntry, NULL);
  //
  // Check MCM support
  //
  // Create Fch GUID HOB to save RESET_DATA_BLOCK
  Status = (*PeiServices)->CreateHob (
                             PeiServices,
                             EFI_HOB_TYPE_GUID_EXTENSION,
                             sizeof (EFI_HOB_GUID_TYPE) + sizeof (FCH_MULITI_FCH_RESET_DATA_BLOCK),
                             &FchHob
                             );

  ASSERT_EFI_ERROR (Status);
  CopyMem (&FchHob->Name, &gFchMultiFchResetDataHobGuid, sizeof (EFI_GUID));
  FchHob++;
  FchMfResetData = (FCH_MULITI_FCH_RESET_DATA_BLOCK *)FchHob;

  //Update DATA BLOCK
  ZeroMem (FchMfResetData, sizeof (FCH_MULITI_FCH_RESET_DATA_BLOCK));
  FchMfResetData->FchAcpiMmioBase[0] = 0xFED80000ul;

  //Init FCH_MULTI_FCH_PEI_PRIVATE
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (FCH_MULTI_FCH_PEI_PRIVATE),
                             &FchMultiFchPeiPrivate
                             );
  ASSERT_EFI_ERROR ( Status);

  ZeroMem (FchMultiFchPeiPrivate, sizeof (FCH_MULTI_FCH_PEI_PRIVATE));

  FchMultiFchPeiPrivate->Signature                          = MULTI_FCH_PEI_PRIVATE_DATA_SIGNATURE;
  FchMultiFchPeiPrivate->FchMultiFchInitPpi.Revision        = AMD_MULTI_FCH_INIT_PPI_REV;

  //
  // Locat Fch Init PPI
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gAmdFchInitPpiGuid,
                             0,
                             NULL,
                             (VOID **)&FchInitPpi
                             );

  ASSERT_EFI_ERROR ( Status);

  //
  // Init local Data structure
  //
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (FCH_RESET_DATA_BLOCK),
                             &FchResetDataPtr
                             );

  CopyMem (FchResetDataPtr, FchInitPpi->FchResetData, sizeof (FCH_RESET_DATA_BLOCK));

  //
  // Update local Data Structure
  //
  SataMultiDieEnable = PcdGet8 (PcdSataMultiDieEnable);

  //
  // Check Die#
  //
  for (Die = 1; Die < MAX_DIE_SUPPORT; Die++) {
    //
    // Update local Data Structure for each Die
    //
    SataMultiDieEnable = SataMultiDieEnable >> 1;
    FchResetDataPtr->FchReset.SataEnable = SataMultiDieEnable & BIT0;

    //DF devices on bus0
    PeiPciAddress = EFI_PEI_PCI_CFG_ADDRESS (0, (Die + 24), 0, 0);
    LibFchPciCfgRead (PeiServices, EfiPeiPciCfgWidthUint16, PeiPciAddress, &VendorID);

//    if (VendorID != AMD_VENDOR_ID) {
	  if (VendorID != AMD_VID && VendorID != HYGON_VID ) {
      continue;
    }

    PeiPciAddress = EFI_PEI_PCI_CFG_ADDRESS (0, (Die + 24), DF_CFGADDRESSCNTL_FUNC, DF_CFGADDRESSCNTL_OFFSET);
    LibFchPciCfgRead (PeiServices, EfiPeiPciCfgWidthUint8, PeiPciAddress, &DieBusNum);

    //enable IO 0xCxx on the dieN
    PeiPciAddress = EFI_PEI_PCI_CFG_ADDRESS (0, 24, DF_X86IOBASE_FUNC, DF_X86IOBASE_OFFSET);
    LibFchPciCfgRead (PeiServices, EfiPeiPciCfgWidthUint32, PeiPciAddress, &IOBaseRegVal_Org);
    IOBaseRegVal_New = 0x00000003;
    LibFchPciCfgWrite (PeiServices, EfiPeiPciCfgWidthUint32, PeiPciAddress, &IOBaseRegVal_New);

    PeiPciAddress = EFI_PEI_PCI_CFG_ADDRESS (0, 24, DF_X86IOBASE_FUNC, DF_X86IOBASE_OFFSET + 4);
    LibFchPciCfgRead (PeiServices, EfiPeiPciCfgWidthUint32, PeiPciAddress, &IOLimitRegVal_Org);
    IOLimitRegVal_New = 0x00000004 + (Die << 5);
    LibFchPciCfgWrite (PeiServices, EfiPeiPciCfgWidthUint32, PeiPciAddress, &IOLimitRegVal_New);


    //Secondary Fch init and MMIO relocation
    FchTSSecondaryFchInitPei (Die, DieBusNum, FchResetDataPtr);

    // Disable IO 0xCxx on DieN
    PeiPciAddress = EFI_PEI_PCI_CFG_ADDRESS (0, 24, DF_X86IOBASE_FUNC, DF_X86IOBASE_OFFSET);
    LibFchPciCfgWrite (PeiServices, EfiPeiPciCfgWidthUint32, PeiPciAddress, &IOBaseRegVal_Org);

    PeiPciAddress = EFI_PEI_PCI_CFG_ADDRESS (0, 24, DF_X86IOBASE_FUNC, DF_X86IOBASE_OFFSET + 4);
    LibFchPciCfgWrite (PeiServices, EfiPeiPciCfgWidthUint32, PeiPciAddress, &IOLimitRegVal_Org);

    //USB
    FchTSSecondaryFchInitUsbPei (Die, DieBusNum, FchResetDataPtr);
    //SATA
    FchTSSecondaryFchInitSataPei (Die, DieBusNum, FchResetDataPtr);
    //Ethernet

    //Update Globle Data
    FchMfResetData->FchAcpiMmioBase[Die] = (UINT64) FchResetDataPtr->FchAcpiMmioBase;
  }

  // Update PPI data
  CopyMem (&FchMultiFchPeiPrivate->FchMultiFchInitPpi.FchMfResetData, FchMfResetData, sizeof (FCH_MULITI_FCH_RESET_DATA_BLOCK));

  // Allocate memory for the PPI descriptor
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             &PpiListMultiFchInit
                             );
  ASSERT_EFI_ERROR ( Status);

  PpiListMultiFchInit->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiListMultiFchInit->Guid  = &gAmdFchMultiFchInitPpiGuid;
  PpiListMultiFchInit->Ppi   = &FchMultiFchPeiPrivate->FchMultiFchInitPpi;

  Status = (*PeiServices)->InstallPpi (
                             PeiServices,
                             PpiListMultiFchInit
                             );

  AGESA_TESTPOINT (TpFchMultiFchPeiExit, NULL);
  return Status;
}

VOID
FchTSSecondaryFchInitPei (
  IN  UINT8       Die,
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  FchTSSecondaryFchMmioRelocate (Die, FchDataPtr);
  FchTSSecondaryFchInitWakeWA (DieBusNum, FchDataPtr);
  FchTSSecondaryFchIORegInit (Die, FchDataPtr);
  FchTSSecondaryFchSpreadSpectrum (Die, FchDataPtr);
  FchTSSecondaryFchDisableSD (Die, DieBusNum, FchDataPtr);
}

VOID
FchTSSecondaryFchInitUsbPei (
  IN  UINT8       Die,
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  UINT32                    DieBusNum32;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;

  DieBusNum32 = (UINT32)DieBusNum;
  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  //For Boundary Scan
  FchSmnRW (DieBusNum32, 0x16D8014C, ~ (UINT32) BIT31, BIT31, NULL);

  //
  // Disable USB controller on SP3-2 and SP3-3
  //
  if (Die & BIT1) {
    FchSmnRW (DieBusNum32, FCH_TS_USB_NBIF_STRAP0, ~(UINT32) BIT28, 0, NULL);
    // Put controller back to reset
    FchSmnRW (DieBusNum32, FCH_TS_USB_RESET, ~ ((UINT32) (FCH_TS_USB_CONTROLLER_RESET + FCH_TS_USB_PHY_RESET + FCH_TS_USB_S5_RESET)), (FCH_TS_USB_CONTROLLER_RESET + FCH_TS_USB_PHY_RESET + FCH_TS_USB_S5_RESET), NULL);
    return;
  }

  //
  // Disable USB controller according to PCD
  //
  switch (Die) {
  case 1:
    if (!(PcdGetBool (PcdXhci1Enable))) {
      FchSmnRW (DieBusNum32, FCH_TS_USB_NBIF_STRAP0, ~(UINT32) BIT28, 0, NULL);
      return;
    }
    break;

  case 4:
    if (!(PcdGetBool (PcdXhci2Enable))) {
      FchSmnRW (DieBusNum32, FCH_TS_USB_NBIF_STRAP0, ~(UINT32) BIT28, 0, NULL);
      return;
    }
    break;

  case 5:
    if (!(PcdGetBool (PcdXhci3Enable))) {
      FchSmnRW (DieBusNum32, FCH_TS_USB_NBIF_STRAP0, ~(UINT32) BIT28, 0, NULL);
      return;
    }
    break;

  default:
    break;
  }

  if (Die == 1) {
    LocalCfgPtr->XhciLaneParaCtl0 = 0x01B3CD56;
  }

  if ((FchReadSleepType () != AMD_ACPI_S3)) {
    FchTSXhciInitBootProgram (DieBusNum32, FchDataPtr);
  } else {
    FchTSXhciInitS3ExitProgram (DieBusNum32, FchDataPtr);
  }
}

VOID
FchTSSecondaryFchInitSataPei (
  IN  UINT8       Die,
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  UINT32                    DieBusNum32;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;

  DieBusNum32 = (UINT32)DieBusNum;
  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  FchInitResetSataProgram (DieBusNum32, FchDataPtr);
}

VOID
FchTSSecondaryFchMmioRelocate (
  IN  UINT8       Die,
  IN  VOID        *FchDataPtr
  )
{

  UINT32                    AltMmioBase;
  UINT64                    Length;
  UINT64                    FchMmioBase;
  FABRIC_TARGET             MmioTarget;
  FABRIC_MMIO_ATTRIBUTE     Attributes;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  Length = 0x2000;
  MmioTarget.TgtType = TARGET_DIE;
  MmioTarget.SocketNum = Die / MAX_DIES_PER_SOCKET;
  MmioTarget.DieNum = Die % MAX_DIES_PER_SOCKET;
  Attributes.ReadEnable = 1;
  Attributes.WriteEnable = 1;
  Attributes.NonPosted = 0;
  Attributes.MmioType = NON_PCI_DEVICE_BELOW_4G;
  if (FabricAllocateMmio (&FchMmioBase, &Length, 16, MmioTarget, &Attributes) == EFI_SUCCESS) {
    AltMmioBase = (UINT32) ((FchMmioBase >> 16) & 0xFFFF);
    RwPmio (FCH_PMIOA_REGD6, AccessWidth16, 0x00, AltMmioBase, NULL);
    RwPmio (FCH_PMIOA_REGD5, AccessWidth8, 0xFC, 1, NULL);
    LocalCfgPtr->FchAcpiMmioBase = (UINT32)FchMmioBase;
    DEBUG ((DEBUG_INFO, "Die %d Alt ACPI MMIO Base = 0x%X \n", Die, FchMmioBase));
  }
}

VOID
FchTSSecondaryFchInitWakeWA (
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  BOOLEAN                   CheckHigh;
  BOOLEAN                   CheckLow;
  UINT32                    GpioReg32;
  UINT32                    FchAcpiMmio;
  UINT32                    FchAcpiMmioDie0;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  UINT8                     PkgType;
  UINT32                    RegEbx;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  FchAcpiMmio = LocalCfgPtr->FchAcpiMmioBase;
  FchAcpiMmioDie0 = ACPI_MMIO_BASE;

  //
  // MCM checking
  //
  AsmCpuid (0x80000001, NULL, &RegEbx, NULL, NULL);
  PkgType = (UINT8)(RegEbx >> 28);
  DEBUG((EFI_D_ERROR,"[FchMutiFch]PkgType : %x\n",PkgType));
  if ((PkgType == PKG_SP3) || (PkgType == PKG_SP4)) {
    //
    // For SP3 and SP4, BIOS should configure WAKE_L pin to become GPIO
    //
    // Write (address=SLAVE_IOMUX_02, value=01b);
    RwMem (FchAcpiMmio + IOMUX_BASE + 0x02, AccessWidth8, 0x00, 0x1);
  } else if (PkgType == PKG_SP3_R2) {
    //
    // For SP3-r2 package, there is a new substrate being made to work around the bug.
    // BIOS will need to detect whether it is the new or old substrate.
    //
    // enable pull-down resistor on GPIO40 pin on master die
    //  Write (address=MASTER_BANK0_GPIO40, PullDownEnable=1 PullUpEnable=0);
    RwMem (FchAcpiMmioDie0 + IOMUX_BASE + FCH_GPIO_REG40, AccessWidth8, 0x00, 0x00);
    RwMem (FchAcpiMmioDie0 + GPIO_BANK0_BASE + 0xA0, AccessWidth32, ~(UINT32) (BIT20 + BIT21), 0);

    // First configure slave's WAKE pin as GPIO to drive high
    //  Write (address=SLAVE_IOMUX_02, value = 01b);
    //  Write (address=SLAVE_GPIO2, OutputEnable = 1, OutputValue=1);
    RwMem (FchAcpiMmio + IOMUX_BASE + 0x02, AccessWidth8, 0x00, 0x1);
    RwMem (FchAcpiMmio + GPIO_BANK0_BASE + 0x08, AccessWidth32, ~(UINT32) (BIT22 + BIT23), BIT22 + BIT23);

    // read master's GPIO40 pin status to see if it is high or low
    //  Read (address=MASTER_BANK0_GPIO40.PinSts)
    ReadMem (FchAcpiMmioDie0 + GPIO_BANK0_BASE + 0xA0, AccessWidth32, &GpioReg32);

    //  # if slave die's WAKE# is connected to GPIO40, it will cause the pin to be high
    if (GpioReg32 & BIT16) {
      CheckHigh = TRUE;
    } else {
      CheckHigh = FALSE;
    }

    // # Next configure slave's WAKE pin as GPIO to drive low
    //   Write (address=SLAVE_GPIO2, OutputEnable = 1, OutputValue=0);
    RwMem (FchAcpiMmio + GPIO_BANK0_BASE + 0x08, AccessWidth32, ~(UINT32) (BIT22 + BIT23), BIT23);
    // # if slave die's WAKE# is connected to GPIO40, it will cause the pin to be low
    ReadMem (FchAcpiMmioDie0 + GPIO_BANK0_BASE + 0xA0, AccessWidth32, &GpioReg32);
    if (GpioReg32 & BIT16) {
      CheckLow = FALSE;
    } else {
      CheckLow = TRUE;
    }

    //     # if master.GPIO40 follows slave.GPIO2, it must be new substrate
    //     if (CheckHigh && CheckLow ) {
    //       # configure GPIO40 to GEVENT; report GEVENT bit as USB_WAKE to OS
    //       # in ACPI table
    //      # program GPIO40 as GEVENT
    //      # set WAKE_L pin back to original
    //      Write (SLAVE_IOMUX_02 = 00b);
    //     } else {
    //     # otherwise, we have an old substrate.  In this case, we should tristate slave's WAKE_L pin
    //      Write (address=SLAVE_IOMUX_02, value=01b);
    //     }
    if (CheckHigh && CheckLow) {
      RwMem (FchAcpiMmioDie0 + SMI_BASE + FCH_SMI_Gevent20, AccessWidth8, 0x00, 0x14);
      RwMem (FchAcpiMmio + IOMUX_BASE + 0x02, AccessWidth8, 0x00, 0x00);
      // Map XHCI PME# to BIT25 in Event_Status on slave die
      RwMem (FchAcpiMmio + SMI_BASE + FCH_SMI_xHC1Pme, AccessWidth8, 0x00, 0x19);
      // Set SciLevl[25] = Level Trigger
      RwMem (FchAcpiMmio + SMI_BASE + FCH_SMI_REG0C, AccessWidth32, 0xFFFFFFFF, BIT25);
      // Enable Event_Enable[25]
      RwMem (FchAcpiMmio + SMI_BASE + FCH_SMI_REG04, AccessWidth32, 0x00, BIT25);
      // Clear Event_Status on slave die
      RwMem (FchAcpiMmio + SMI_BASE + FCH_SMI_REG00, AccessWidth32, 0x00, 0xFFFFFFFF);
    } else {
      RwMem (FchAcpiMmio + IOMUX_BASE + 0x02, AccessWidth8, 0x00, 0x1);
      RwMem (FchAcpiMmio + GPIO_BANK0_BASE + 0x08, AccessWidth32, ~(UINT32) (BIT22 + BIT23), 0x00);
    }

    // After detection, BIOS should enable the pull-up in GPIO40 of the master die
    // This is to keep this pin pulled high even when there is no work around; or when slave
    // die should tristate the pin during the "workaround period" prior to sleep entry 
    //  Write (address=MASTER_BANK0_GPIO40, PullDownEnable=0 PullUpEnable=1);
    RwMem (FchAcpiMmioDie0 + GPIO_BANK0_BASE + 0xA0, AccessWidth32, ~(UINT32) (BIT20 + BIT21), BIT20);
  }
}

VOID
FchTSSecondaryFchIORegInit (
  IN  UINT8       Die,
  IN  VOID        *FchDataPtr
  )
{
  // Init AB registers
  FchTSSecondaryFchInitABPei ();
}

VOID
FchTSSecondaryFchInitABPei (
  VOID
  )
{
  RwPmio (FCH_PMIOA_REGE0, AccessWidth16, 0x00, ALINK_ACCESS_INDEX, NULL);
  // Disable Syncflood
  RwAlink (FCH_ABCFG_REG10050 | (UINT32) (ABCFG << 29), ~(UINT32) BIT2, 0, NULL);

  RwPmio (FCH_PMIOA_REGE0, AccessWidth16, 0x00, 0x00, NULL);
}

VOID
FchTSSecondaryFchSpreadSpectrum (
  IN  UINT8       Die,
  IN  VOID        *FchDataPtr
  )
{
  UINT32                    FchAcpiMmio;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;
  FchAcpiMmio = LocalCfgPtr->FchAcpiMmioBase;
  //
  //Step 4 - If it is slave die (SP3/SP4/SP3r2) or SP3/SP4 master die in socket1 package: (100M refclk)
  //  Misc_Reg x10[1:0] (refclk_div[1:0]) value from 2'b00 to 2'b01
  //  Misc_Reg x10[12:4] (FCW0_int[8:0]) value from 9'h000 to 9'h004
  //  Misc_Reg x34[31:23] (FCW1_int[8:0]) value from 9'h000 to 9'h03C
  //  Misc_Reg x14[23:8] (FCW1_frac[8:0]) value from 16'h0000 to 16'h03D7
  //  Misc_Reg x18[31:16] (FCW_slew_frac[15:0]) value from 16'h0000 to 16'h003E
  //  Misc_Reg x1C[16:13] (gp_coarse_mant[3:0]) value from 16'h0000 to 16'h0001
  //  Misc_Reg x1C[20:17](gp_coarse_exp[3:0]) value from 2'b00 to 2'b00 (no change)
  //  Misc_Reg x1C[27:26] (gi_coarse_mant[1:0]) value from 2'b00 to 2'b10
  //  Misc_Reg x1C[12:9] (gi_coarse_exp[3:0]) value from 4'b0000 to  4'b0010
  RwMem (FchAcpiMmio + MISC_BASE + 0x10, AccessWidth32, ~(UINT32) (0x3 << 0), (UINT32) (1 << 0));
  RwMem (FchAcpiMmio + MISC_BASE + 0x10, AccessWidth32, ~(UINT32) (0x1FF << 4), (UINT32) (4 << 4));
  RwMem (FchAcpiMmio + MISC_BASE + 0x34, AccessWidth32, ~(UINT32) (0x1FF << 23), (UINT32) (0x3C << 23));
  RwMem (FchAcpiMmio + MISC_BASE + 0x14, AccessWidth32, ~(UINT32) (0xFFFF << 8), (UINT32) (0x03D7 << 8));
  RwMem (FchAcpiMmio + MISC_BASE + 0x18, AccessWidth32, ~(UINT32) (0xFFFF << 16), (UINT32) (0x003E << 16));
  RwMem (FchAcpiMmio + MISC_BASE + 0x1C, AccessWidth32, ~(UINT32) (0xF << 13), (UINT32) (1 << 13));
  RwMem (FchAcpiMmio + MISC_BASE + 0x1C, AccessWidth32, ~(UINT32) (0xF << 17), (UINT32) (0 << 17));
  RwMem (FchAcpiMmio + MISC_BASE + 0x1C, AccessWidth32, ~(UINT32) (0x3 << 26), (UINT32) (2 << 26));
  RwMem (FchAcpiMmio + MISC_BASE + 0x1C, AccessWidth32, ~(UINT32) (0xF << 9), (UINT32) (2 << 9));


  //
  //Step 5 - Misc_Reg x40[25] (FBDIV_LoadEN for loading register) value from 1'b0 to 1'b1
  //
  RwMem (FchAcpiMmio + MISC_BASE + 0x40, AccessWidth32, ~(UINT32) BIT25, BIT25);

  //
  //Step 6 - Misc_ Reg x40[30]=1 (cfg_update_req)
  //
  RwMem (FchAcpiMmio + MISC_BASE + 0x40, AccessWidth32, ~(UINT32) BIT30, BIT30);
}

VOID
FchTSSecondaryFchDisableSD (
  IN  UINT8       Die,
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  UINT32                    FchAcpiMmio;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;
  FchAcpiMmio = LocalCfgPtr->FchAcpiMmioBase;

  RwMem (FchAcpiMmio + PMIO_BASE + FCH_PMIOA_REGD3, AccessWidth8, 0xBF, 0x00);
  RwMem (FchAcpiMmio + PMIO_BASE + FCH_PMIOA_REGE8, AccessWidth8, 0xFE, 0x00);
  RwMem (FchAcpiMmio + AOAC_BASE + 0x72, AccessWidth8, 0xF7, 0x00);
}
