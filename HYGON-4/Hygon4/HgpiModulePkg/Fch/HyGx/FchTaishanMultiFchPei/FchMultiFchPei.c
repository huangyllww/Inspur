/* $NoKeywords:$ */

/**
 * @file
 *
 * FCH PEIM
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project   FCH PEIM
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
#include "FchMultiFchPei.h"

#define DF_X86IOBASE_FUNC    0x00
#define DF_X86IOBASE_OFFSET  0x220

extern EFI_GUID  gFchMultiFchResetDataHobGuid;

/*********************************************************************************
 * Name: MultiFchPeiHyGxInit
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
MultiFchPeiHyGxInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  UINT8                                   FchBusNum;
  UINT32                                  SataEnable32;
  UINT64                                  XhciEnable64;
  UINT32                                  IOBaseRegVal_Org;
  UINT32                                  IOLimitRegVal_Org;
  UINT32                                  IOBaseRegVal_New;
  UINT32                                  IOLimitRegVal_New;
  EFI_STATUS                              Status;
  EFI_HOB_GUID_TYPE                       *FchHob;
  HYGON_FCH_INIT_PPI                      *FchInitPpi;
  FCH_RESET_DATA_BLOCK                    *FchResetDataPtr;
  FCH_MULITI_FCH_RESET_DATA_BLOCK         *FchMfResetData;
  FCH_MULTI_FCH_PEI_PRIVATE               *FchMultiFchPeiPrivate;
  EFI_PEI_PPI_DESCRIPTOR                  *PpiListMultiFchInit;
  HYGON_PEI_FABRIC_TOPOLOGY_SERVICES_PPI  *FabricTopologyServicesPpi;
  UINTN                                   NumberOfSockets;
  UINTN                                   NumberOfPhysicalDie;
  UINTN                                   SocketId;
  UINTN                                   PhysicalDieId;
  UINTN                                   LogicalDieId;
  UINTN                                   DieType;
  UINTN                                   NumberOfFch;
  UINT8                                   SataIndex;
  UINT8                                   UsbIndex;

  HGPI_TESTPOINT (TpFchMultiFchPeiEntry, NULL);
  DEBUG ((DEBUG_INFO, "MultiFchPeiHyGxInit Entry \n"));

  Status = (**PeiServices).LocatePpi (
          PeiServices,
          &gHygonFabricTopologyServicesPpiGuid,
          0,
          NULL,
          &FabricTopologyServicesPpi
  );
  FabricTopologyServicesPpi->GetSystemInfo (&NumberOfSockets, NULL, NULL);
  NumberOfPhysicalDie = FabricTopologyGetNumberOfPhysicalDiesOnSocket (0);
  DEBUG ((DEBUG_INFO, "  Socket number=%d, Physical Die number per socket=%d \n", NumberOfSockets, NumberOfPhysicalDie));

  NumberOfFch = 0;
  for (SocketId = 0; SocketId < NumberOfSockets; SocketId++) {
    if (NumberOfPhysicalDie == 1) {
        NumberOfFch++;  // Count Satori FCH
    } else {
      for (PhysicalDieId = 0; PhysicalDieId < NumberOfPhysicalDie; PhysicalDieId++) {
        FabricTopologyGetPhysicalIodDieInfo (PhysicalDieId, &LogicalDieId, &DieType);
        if (DieType == IOD_DUJIANG) {
          NumberOfFch++;  // Count Dujiang FCH
        }
      } 
    }
  }

  DEBUG ((DEBUG_INFO, "  System total Fch Number=%d \n", NumberOfFch));
  if (NumberOfFch < 2) {
    return EFI_SUCCESS;
  }

  // Create Fch GUID HOB to save RESET_DATA_BLOCK
  Status = (*PeiServices)->CreateHob (
                             PeiServices,
                             EFI_HOB_TYPE_GUID_EXTENSION,
                             sizeof (EFI_HOB_GUID_TYPE) + sizeof (FCH_MULITI_FCH_RESET_DATA_BLOCK),
                             &FchHob
                             );

  ASSERT (!EFI_ERROR (Status));
  CopyMem (&FchHob->Name, &gFchMultiFchResetDataHobGuid, sizeof (EFI_GUID));
  FchHob++;
  FchMfResetData = (FCH_MULITI_FCH_RESET_DATA_BLOCK *)FchHob;

  // Update DATA BLOCK
  ZeroMem (FchMfResetData, sizeof (FCH_MULITI_FCH_RESET_DATA_BLOCK));
  FchMfResetData->FchAcpiMmioBase[0][0] = 0xFED80000ul;

  // Set Master Fch Warm Reboot Sequence
  FchSetWarmRebootSequence ((UINT32)FchMfResetData->FchAcpiMmioBase[0][0]);

  // Init FCH_MULTI_FCH_PEI_PRIVATE
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (FCH_MULTI_FCH_PEI_PRIVATE),
                             &FchMultiFchPeiPrivate
                             );
  ASSERT (!EFI_ERROR (Status));

  ZeroMem (FchMultiFchPeiPrivate, sizeof (FCH_MULTI_FCH_PEI_PRIVATE));

  FchMultiFchPeiPrivate->Signature                          = MULTI_FCH_PEI_PRIVATE_DATA_SIGNATURE;
  FchMultiFchPeiPrivate->FchMultiFchInitPpi.Revision        = HYGON_MULTI_FCH_INIT_PPI_REV;

  //
  // Locate Fch Init PPI
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonFchInitPpiGuid,
                             0,
                             NULL,
                             (VOID **)&FchInitPpi
                             );

  ASSERT (!EFI_ERROR (Status));

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
  SataEnable32 = PcdGet32 (PcdSataEnableHyGx);
  XhciEnable64 = PcdGet64 (PcdXhciEnableHyGx);
  for (SocketId = 0; SocketId < NumberOfSockets; SocketId++) {
    for (PhysicalDieId = 0; PhysicalDieId < NumberOfPhysicalDie; PhysicalDieId++) {
      if ((SocketId == 0) && (PhysicalDieId == 0)) {
        continue;           // Bypass master dujiang or Bypass master Satori
      }

      FabricTopologyGetPhysicalIodDieInfo (PhysicalDieId, &LogicalDieId, &DieType);
      if (DieType == IOD_DUJIANG) {
        DEBUG ((DEBUG_INFO, "  Configure socket %d physical die %d FCH: \n", SocketId, PhysicalDieId));

        // enable IO 0xCxx on the Socket N Die N
        IOBaseRegVal_Org = CddFabricRegisterAccRead (
                             0,                                  // Socket0
                             0,                                  // Cdd0
                             DF_X86IOBASE_FUNC,                  // 0x00
                             DF_X86IOBASE_OFFSET,                //
                             FABRIC_REG_ACC_BC                   // BC
                             );
        IOBaseRegVal_New = 0x00000003;
        CddFabricRegisterAccWrite (
          0,                                                     // Socket0
          0,                                                     // Cdd0
          DF_X86IOBASE_FUNC,                                     // 0x00
          DF_X86IOBASE_OFFSET,                                   //
          FABRIC_REG_ACC_BC,                                     // BC
          IOBaseRegVal_New,
          FALSE
          );

        IOLimitRegVal_Org = CddFabricRegisterAccRead (
                              0,                                 // Socket0
                              0,                                 // Cdd0
                              DF_X86IOBASE_FUNC,                 // 0x00
                              DF_X86IOBASE_OFFSET + 4,           //
                              FABRIC_REG_ACC_BC                  // BC
                              );
        IOLimitRegVal_New = IOMS0_FABRIC_ID + FabricTopologyGetDieSystemOffset(SocketId, LogicalDieId);
        CddFabricRegisterAccWrite (
          0,                                                     // Socket0
          0,                                                     // Cdd0
          DF_X86IOBASE_FUNC,                                     // 0x00
          DF_X86IOBASE_OFFSET + 4,                               //
          FABRIC_REG_ACC_BC,                                     // BC
          IOLimitRegVal_New,
          FALSE
          );

        // Secondary Fch init and MMIO relocation
        FchBusNum = (UINT8)ReadSocDieBusNum (SocketId, LogicalDieId, 0);
        DEBUG ((DEBUG_INFO, "  FchBusNum = 0x%x \n", FchBusNum));

        FchTSSecondaryFchInitPei (FchBusNum, FchResetDataPtr);

        // Disable IO 0xCxx on DieN
        CddFabricRegisterAccWrite (
          0,                                                     // Socket0
          0,                                                     // Cdd0
          DF_X86IOBASE_FUNC,                                     // 0x00
          DF_X86IOBASE_OFFSET,                                   // 0xC0
          FABRIC_REG_ACC_BC,                                     // BC
          IOBaseRegVal_Org,
          FALSE
          );

        CddFabricRegisterAccWrite (
          0,                                                     // Socket0
          0,                                                     // Cdd0
          DF_X86IOBASE_FUNC,                                     // 0x00
          DF_X86IOBASE_OFFSET + 4,                               // 0xC0
          FABRIC_REG_ACC_BC,                                     // BC
          IOLimitRegVal_Org,
          FALSE
          );

        // USB
        FchResetDataPtr->FchReset.XhciEnable = ((XhciEnable64 >> ((SocketId * 8) + (PhysicalDieId * 2))) & 0x03);
        for (UsbIndex = 0; UsbIndex < USB_NUMBER_PER_DUJIANG; UsbIndex++) {
          if (PcdGetBool (PcdHygonUsbSupport) == TRUE) {
            DEBUG ((DEBUG_INFO, "  Configure USB %d \n", UsbIndex));
            FchTSSecondaryFchInitUsbPei ((UINT8)SocketId, (UINT8)PhysicalDieId, UsbIndex, FchBusNum, FchResetDataPtr);
          }
        }

        // SATA
        FchResetDataPtr->FchReset.SataEnable = ((SataEnable32 >> ((SocketId * MAX_SATA_NUMBER_PER_SOCKET) + (PhysicalDieId * SATA_NUMBER_PER_DUJIANG))) & 0x1);
        for (SataIndex = 0; SataIndex < SATA_NUMBER_PER_DUJIANG; SataIndex++) {
          DEBUG ((DEBUG_INFO, "  Configure SATA %d \n", SataIndex));
          FchTSSecondaryFchInitSataPei ((UINT8)PhysicalDieId, SataIndex, FchBusNum, FchResetDataPtr);
        }

        // Update Global Data
        FchMfResetData->FchAcpiMmioBase[SocketId][PhysicalDieId] = (UINT64)FchResetDataPtr->Misc2.FchAcpiMmioBase;
        
        // Set Slave Fch Warm Reboot Sequence
        FchSetWarmRebootSequence (FchResetDataPtr->Misc2.FchAcpiMmioBase);
      }
    }
  }

  // Update PPI data
  CopyMem (
    &FchMultiFchPeiPrivate->FchMultiFchInitPpi.FchMfResetData,
    FchMfResetData,
    sizeof (FCH_MULITI_FCH_RESET_DATA_BLOCK)
    );

  // Allocate memory for the PPI descriptor
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             &PpiListMultiFchInit
                             );
  ASSERT (!EFI_ERROR (Status));

  PpiListMultiFchInit->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiListMultiFchInit->Guid  = &gHygonFchMultiFchInitPpiGuid;
  PpiListMultiFchInit->Ppi   = &FchMultiFchPeiPrivate->FchMultiFchInitPpi;

  Status = (*PeiServices)->InstallPpi (
                             PeiServices,
                             PpiListMultiFchInit
                             );

  DEBUG ((DEBUG_INFO, "MultiFchPeiHyGxInit Exit \n"));
  HGPI_TESTPOINT (TpFchMultiFchPeiExit, NULL);
  return Status;
}

VOID
FchTSSecondaryFchInitPei (
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK  *LocalCfgPtr;

  DEBUG ((DEBUG_INFO, "FchTSSecondaryFchInitPei Entry \n"));
  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  FchTSSecondaryFchMmioRelocate (DieBusNum, FchDataPtr);
  FchTSSecondaryFchInitWakeWA (DieBusNum, FchDataPtr);
  FchTSSecondaryFchIORegInit (DieBusNum, FchDataPtr);
  FchTSSecondaryFchSpreadSpectrum (DieBusNum, FchDataPtr);
  FchTSSecondaryFchDisableSD (DieBusNum, FchDataPtr);
  DEBUG ((DEBUG_INFO, "FchTSSecondaryFchInitPei Exit \n"));
}

VOID
FchTSSecondaryFchInitUsbPei (
  IN  UINT8       Socket,
  IN  UINT8       PhysicalDieId,
  IN  UINT8       UsbIndex,
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  UINT32                DieBusNum32;
  FCH_RESET_DATA_BLOCK  *LocalCfgPtr;

  DieBusNum32 = (UINT32)DieBusNum;
  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  // For Boundary Scan
  FchSmnRW (DieBusNum32, USB_SPACE_HYGX (PhysicalDieId, UsbIndex, FCH_TS_USB_SPARE2_REG_HYGX), ~(UINT32)BIT31, BIT31, NULL);
  //Raise 200M to 400 of USB AXI Clock
  FchSmnRW (DieBusNum32, USB_SPACE_HYGX (PhysicalDieId, UsbIndex, (FCH_SMN_MISC_BASE_HYGX + FCH_MISC_REG40)), ~(UINT32)BIT2, 0, NULL);
  //
  // Disable USB controller according to PCD
  //
  if ((LocalCfgPtr->FchReset.XhciEnable & (UINT64)BIT (UsbIndex)) == 0) {
    FchSmnRW (DieBusNum32, NBIO_SPACE2 (PhysicalDieId, 0, (UsbIndex == 0 ? NBIF2_USB_STRAP0_ADDRESS_HYGX : NBIF3_USB_STRAP0_ADDRESS_HYGX)), ~(UINT32)BIT28, BIT28, NULL);
    return;
  }

  LocalCfgPtr->Xhci.XhciLaneParaCtl0 = 0x01B3CD56;

  if ((FchReadSleepType () != HYGON_ACPI_S3)) {
    FchTSXhciInitBootProgram (Socket, PhysicalDieId, UsbIndex, DieBusNum32, FchDataPtr);
  } else {
    FchTSXhciInitS3ExitProgram (Socket, PhysicalDieId, UsbIndex, DieBusNum32, FchDataPtr);
  }
}

VOID
FchTSSecondaryFchInitSataPei (
  IN  UINT8       PhysicalDieId,
  IN  UINT8       SataIndex,
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  UINT32                DieBusNum32;
  FCH_RESET_DATA_BLOCK  *LocalCfgPtr;

  DieBusNum32 = (UINT32)DieBusNum;
  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  FchInitResetSataProgram (PhysicalDieId, SataIndex, DieBusNum32, FchDataPtr);
}

VOID
FchTSSecondaryFchMmioRelocate (
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  UINT32                 AltMmioBase;
  UINT64                 Length;
  UINT64                 FchMmioBase;
  FABRIC_TARGET          MmioTarget;
  FABRIC_MMIO_ATTRIBUTE  Attributes;
  FCH_RESET_DATA_BLOCK   *LocalCfgPtr;
  EFI_STATUS             Status;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  Length                 = 0x2000;
  MmioTarget.TgtType     = TARGET_PCI_BUS;
  MmioTarget.PciBusNum   = DieBusNum;
  Attributes.ReadEnable  = 1;
  Attributes.WriteEnable = 1;
  Attributes.NonPosted   = 0;
  Attributes.MmioType    = NON_PCI_DEVICE_BELOW_4G;

  Status = FabricAllocateMmio (&FchMmioBase, &Length, 16, MmioTarget, &Attributes);
  if (Status == EFI_SUCCESS) {
    AltMmioBase = (UINT32)((FchMmioBase >> 16) & 0xFFFF);
    RwPmio (FCH_PMIOA_REGD6, AccessWidth16, 0x00, AltMmioBase, NULL);
    RwPmio (FCH_PMIOA_REGD5, AccessWidth8, 0xFC, 1, NULL);
    LocalCfgPtr->Misc2.FchAcpiMmioBase = (UINT32)FchMmioBase;
    DEBUG ((DEBUG_INFO, "    Alt ACPI MMIO Base = 0x%X \n", FchMmioBase));
  }

  DEBUG ((DEBUG_INFO, "    FchTSSecondaryFchMmioRelocate status (%r) \n", Status));
}

VOID
FchTSSecondaryFchInitWakeWA (
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  UINT32                FchAcpiMmio;
  FCH_RESET_DATA_BLOCK  *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  FchAcpiMmio = LocalCfgPtr->Misc2.FchAcpiMmioBase;

  //
  // BIOS should configure other socket WAKE_L pin to become GPIO
  //
  // Write (address=SLAVE_IOMUX_02, value=01b);
  RwMem (FchAcpiMmio + IOMUX_BASE + 0x02, AccessWidth8, 0x00, 0x1);
}

VOID
FchTSSecondaryFchIORegInit (
  IN  UINT8       DieBusNum,
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
  if (PcdGetBool(PcdResetCpuOnSyncFlood)) {
    // Enable Syncflood
    RwAlink (FCH_ABCFG_REG10050 | (UINT32) (ABCFG << 29), ~(UINT32) BIT2, BIT2, NULL);
  } else {
    // Disable Syncflood
    RwAlink (FCH_ABCFG_REG10050 | (UINT32) (ABCFG << 29), ~(UINT32) BIT2, 0, NULL);
  }
  RwPmio (FCH_PMIOA_REGE0, AccessWidth16, 0x00, 0x00, NULL);
}

VOID
FchTSSecondaryFchSpreadSpectrum (
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  UINT32                FchAcpiMmio;
  FCH_RESET_DATA_BLOCK  *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;
  FchAcpiMmio = LocalCfgPtr->Misc2.FchAcpiMmioBase;
  //
  // Step 4 - If it is slave die (SL2/SP4/SL2r2) or SL2/SP4 master die in socket1 package: (100M refclk)
  // Misc_Reg x10[1:0] (refclk_div[1:0]) value from 2'b00 to 2'b01
  // Misc_Reg x10[12:4] (FCW0_int[8:0]) value from 9'h000 to 9'h004
  // Misc_Reg x34[31:23] (FCW1_int[8:0]) value from 9'h000 to 9'h03C
  // Misc_Reg x14[23:8] (FCW1_frac[8:0]) value from 16'h0000 to 16'h03D7
  // Misc_Reg x18[31:16] (FCW_slew_frac[15:0]) value from 16'h0000 to 16'h003E
  // Misc_Reg x1C[16:13] (gp_coarse_mant[3:0]) value from 16'h0000 to 16'h0001
  // Misc_Reg x1C[20:17](gp_coarse_exp[3:0]) value from 2'b00 to 2'b00 (no change)
  // Misc_Reg x1C[27:26] (gi_coarse_mant[1:0]) value from 2'b00 to 2'b10
  // Misc_Reg x1C[12:9] (gi_coarse_exp[3:0]) value from 4'b0000 to  4'b0010
  RwMem (FchAcpiMmio + MISC_BASE + 0x10, AccessWidth32, ~(UINT32)(0x3 << 0), (UINT32)(1 << 0));
  RwMem (FchAcpiMmio + MISC_BASE + 0x10, AccessWidth32, ~(UINT32)(0x1FF << 4), (UINT32)(4 << 4));
  RwMem (FchAcpiMmio + MISC_BASE + 0x34, AccessWidth32, ~(UINT32)(0x1FF << 23), (UINT32)(0x3C << 23));
  RwMem (FchAcpiMmio + MISC_BASE + 0x14, AccessWidth32, ~(UINT32)(0xFFFF << 8), (UINT32)(0x03D7 << 8));
  RwMem (FchAcpiMmio + MISC_BASE + 0x18, AccessWidth32, ~(UINT32)(0xFFFF << 16), (UINT32)(0x003E << 16));
  RwMem (FchAcpiMmio + MISC_BASE + 0x1C, AccessWidth32, ~(UINT32)(0xF << 13), (UINT32)(1 << 13));
  RwMem (FchAcpiMmio + MISC_BASE + 0x1C, AccessWidth32, ~(UINT32)(0xF << 17), (UINT32)(0 << 17));
  RwMem (FchAcpiMmio + MISC_BASE + 0x1C, AccessWidth32, ~(UINT32)(0x3 << 26), (UINT32)(2 << 26));
  RwMem (FchAcpiMmio + MISC_BASE + 0x1C, AccessWidth32, ~(UINT32)(0xF << 9), (UINT32)(2 << 9));

  //
  // Step 5 - Misc_Reg x40[25] (FBDIV_LoadEN for loading register) value from 1'b0 to 1'b1
  //
  RwMem (FchAcpiMmio + MISC_BASE + 0x40, AccessWidth32, ~(UINT32)BIT25, BIT25);

  //
  // Step 6 - Misc_ Reg x40[30]=1 (cfg_update_req)
  //
  RwMem (FchAcpiMmio + MISC_BASE + 0x40, AccessWidth32, ~(UINT32)BIT30, BIT30);
}

VOID
FchTSSecondaryFchDisableSD (
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  )
{
  UINT32                FchAcpiMmio;
  FCH_RESET_DATA_BLOCK  *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;
  FchAcpiMmio = LocalCfgPtr->Misc2.FchAcpiMmioBase;

  RwMem (FchAcpiMmio + PMIO_BASE + FCH_PMIOA_REGD3, AccessWidth8, 0xBF, 0x00);
  RwMem (FchAcpiMmio + PMIO_BASE + FCH_PMIOA_REGE8, AccessWidth8, 0xFE, 0x00);
  RwMem (FchAcpiMmio + AOAC_BASE + 0x72, AccessWidth8, 0xF7, 0x00);
}

/**
 * FchSetWarmRebootSequence  - Set fch warm reboot sequence
 *
 * @param[in] FchMmioBaseAddress  Fch MMIO Base Address
 *
 */
VOID
FchSetWarmRebootSequence (
  IN UINT32       FchMmioBaseAddress
  )
{
  UINT8  FchResetSequence;

  // 1. Set PMIO Warm Reset Control Register PMx70[2](En_slv_cpurst).
  // When set to 1, Master FCH will do reset when slave FCH asserts CpuRst=0. Slave FCH will do warm
  // reset for sync_flood.Master and slave fch should set the same value.
  RwMem (FchMmioBaseAddress + PMIO_BASE + FCH_PMIOA_REG70, AccessWidth8, ~(UINT8)(1 << 2), PcdGet8 (PcdSlaveCpuResetEnable) << 2);

  // 3. Select warm reset sequence
  // PMx74[14:13] (S0ResetB_sel)
  // 00:warm reset select normal sequence
  // 01:add 360us time delay for fch internal reset after receive warm reset command
  // 1x:fch internal reset directly use Soc Cpl_resetn
  FchResetSequence = PcdGet8 (PcdWarmResetSequence);
  RwMem (FchMmioBaseAddress + PMIO_BASE + FCH_PMIOA_REG74, AccessWidth32, ~(UINT32)(3 << 13), FchResetSequence << 13);

  // 4. set PMIO Warm reset delay register
  // PMx3C[31:24](S0ResetDlyTmr) Specifies the timer value for delay fch internal reset after receive warm reset command
  // unit = 16us
  if (FchResetSequence == 1) {
    RwMem (
      FchMmioBaseAddress + PMIO_BASE + FCH_PMIOA_REG3C,
      AccessWidth32,
      ~(UINT32)(0xFF << 24),
      PcdGet8 (PcdS0ResetDelayTimerCount) << 24
      );
  }
}
