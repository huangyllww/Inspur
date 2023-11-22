/* $NoKeywords:$ */

/**
 * @file
 *
 * NbioTopMem - Set top of memory for NBIO
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonNbioBasePei
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
#include <HYGON.h>
#include <PiPei.h>
#include <Filecode.h>
#include <GnbRegisters.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Guid/HygonMemoryInfoHob.h>
#include <Library/BaseLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/PcieConfigLib.h>
#include <Ppi/NbioPcieServicesPpi.h>
#include <Ppi/HygonMemoryInfoHobPpi.h>

#define FILECODE        NBIO_NBIOBASE_HYEX_HYGONNBIOBASEPEI_NBIOTOPMEM_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                    P P I   N O T I F Y   D E S C R I P T O R S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/

/**
 * Memory config complete callback
 * This function should use the information from memory config to initialize NBIO top of memory
 *
 *
 *
 * @param[in]  PeiServices        EFI_PEI_SERVICES pointer
 * @param[in]  NotifyDescriptor   EFI_PEI_NOTIFY_DESCRIPTOR pointer
 * @param[in]  Ppi                VOID pointer to the PPI that generated this callback
 * @retval     EFI_STATUS
 */
EFI_STATUS
EFIAPI
MemoryConfigDoneCallbackPpi (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                        Status;
  GNB_HANDLE                        *GnbHandle;
  HYGON_CONFIG_PARAMS               *StdHeader;
  PCIe_PLATFORM_CONFIG              *Pcie;
  PEI_HYGON_NBIO_PCIE_SERVICES_PPI  *PcieServicesPpi;
  GNB_PCIE_INFORMATION_DATA_HOB     *PciePlatformConfigHob;
  UINT64                            MsrData;
  UINT64                            GnbTom2;
  NB_TOP_OF_DRAM_SLOT1_STRUCT       NB_TOP_OF_DRAM_SLOT1;
  NBMISC_0064_STRUCT                NBMISC_0064;
  NBMISC_0068_STRUCT                NBMISC_0068;
  NBMISC_0138_STRUCT                NBMISC_0138;
  NBMISC_013C_STRUCT                NBMISC_013C;
  NB_MMIOBASE_STRUCT                NB_MMIOBASE;
  NB_MMIOLIMIT_STRUCT               NB_MMIOLIMIT;

  HGPI_TESTPOINT (TpMemoryConfigDoneCallbackPpiEntry, NULL);
  GnbHandle = NULL;
  StdHeader = NULL;
  // Need topology structure to get GnbHandle
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonNbioPcieServicesPpiGuid,
                             0,
                             NULL,
                             (VOID **)&PcieServicesPpi
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PcieServicesPpi->PcieGetTopology (PcieServicesPpi, &PciePlatformConfigHob);
  Pcie = &(PciePlatformConfigHob->PciePlatformConfigHob);
  GnbHandle = NbioGetHandle (Pcie);

  while (GnbHandle != NULL) {
    // Read TOP_OF_DRAM registers
    NbioRegisterRead (GnbHandle, NB_TOP_OF_DRAM_SLOT1_TYPE, NB_TOP_OF_DRAM_SLOT1_ADDRESS, &NB_TOP_OF_DRAM_SLOT1.Value, 0);
    NbioRegisterRead (GnbHandle, NB_LOWER_TOP_OF_DRAM2_TYPE, NBIO_SPACE(GnbHandle, NB_LOWER_TOP_OF_DRAM2_ADDRESS_HYEX), &NBMISC_0064.Value, 0);
    NbioRegisterRead (GnbHandle, NB_UPPER_TOP_OF_DRAM2_TYPE, NBIO_SPACE(GnbHandle, NB_UPPER_TOP_OF_DRAM2_ADDRESS_HYEX), &NBMISC_0068.Value, 0);

    // In IOHC, program TOM3_ENABLE=1 and TOM3_LIMIT to 0x3FFFFFF in IOHCMISC0x00000138 (NB_TOP_OF_DRAM3).
    // This will force IOHC to see the full encrypted address space as part of the DRAM range.
    // Setting this should get IOMMU to start up.
    NBMISC_013C.Field.TOM3_EXT_BASE  = 0;
    NBMISC_013C.Field.TOM3_EXT_LIMIT = 0x7;
    NBMISC_0138.Field.TOM3_LIMIT     = 0x3FFFFFF;
    NBMISC_0138.Field.TOM3_ENABLE    = 1;
    // Set MMIOBase and MMIOLimit to cover any MMIO that exists between 1TB and 128TB-1.
    // For example, if this entire range was MMIO (ie. System had 256GB of DRAM) set MMIOBase=0x100_0000 in
    // IOHCMISC0x0000005C (NB_MMIOBASE) and MMIOLimit=0x7FFF_FFFF in IOHCMISC0x00000060 (NB_MMIOLIMIT).
    // This should allow IOHC to see this range as a non-DRAM range and do proper p2p decoding again.
    NB_MMIOBASE.Value  = 0x01000000;
    NB_MMIOLIMIT.Value = 0x7FFFFFFF;

    // Read memory size below 4G from MSR C001_001A
    MsrData = AsmReadMsr64 (TOP_MEM);
    // Write to NB register 0x90
    NB_TOP_OF_DRAM_SLOT1.Field.TOP_OF_DRAM = ((UINT32)MsrData & 0xFF800000) >> 23;     // Keep bits 31:23

    GnbTom2 = 0;
    MsrData = AsmReadMsr64 (SYS_CFG);
    if ((MsrData & BIT21) != 0) {
      // If SYS_CFG(MtrrTom2En) then configure GNB TOM2
      // Read memory size above 4G from TOP_MEM2 (MSR C001_001D)
      MsrData = AsmReadMsr64 (TOP_MEM2);
      IDS_HDT_CONSOLE (GNB_TRACE, "MSR TOP_MEM2[63:32] is 0x%08x\n", RShiftU64 (MsrData, 32));
      IDS_HDT_CONSOLE (GNB_TRACE, "MSR TOP_MEM2[31:0] is 0x%08x\n", MsrData);
      GnbTom2 = MsrData & (UINT64)0xFFFFFFFFFF800000;

      if (GnbTom2 < 0x10000000000) {
        GnbTom2 = MsrData & (UINT64)0x000000FFFFC00000;  // Keep bits 39:22
        NBMISC_0068.Field.UPPER_TOM2 = (UINT32)(RShiftU64 (GnbTom2, 32) & 0xFF);
        NBMISC_0064.Field.LOWER_TOM2 = ((UINT32)GnbTom2 & 0xFF800000) >> 23;
        NBMISC_0064.Field.ENABLE     = 1;
      } else {
        NBMISC_0068.Field.UPPER_TOM2 = 0x1FF;
        NBMISC_0064.Field.LOWER_TOM2 = 0x1FF;
        NBMISC_0064.Field.ENABLE     = 1;
        NB_MMIOBASE.Value = (UINT32)RShiftU64 (GnbTom2, 16);
      }
    }

    IDS_HDT_CONSOLE (GNB_TRACE, "GnbSetTomST setting GnbTom2 to 0x%x\n", GnbTom2);

      // Write TOP_OF_DRAM registers
    NbioRegisterWrite (GnbHandle, NB_TOP_OF_DRAM_SLOT1_TYPE, NB_TOP_OF_DRAM_SLOT1_ADDRESS, &NB_TOP_OF_DRAM_SLOT1.Value, 0);
    NbioRegisterWrite (GnbHandle, NB_LOWER_TOP_OF_DRAM2_TYPE, NBIO_SPACE(GnbHandle, NB_LOWER_TOP_OF_DRAM2_ADDRESS_HYEX), &NBMISC_0064.Value, 0);
    NbioRegisterWrite (GnbHandle, NB_UPPER_TOP_OF_DRAM2_TYPE, NBIO_SPACE(GnbHandle, NB_UPPER_TOP_OF_DRAM2_ADDRESS_HYEX), &NBMISC_0068.Value, 0);
    NbioRegisterWrite (GnbHandle, NB_TOP_OF_DRAM3_EXT_TYPE, NBIO_SPACE(GnbHandle, NB_TOP_OF_DRAM3_EXT_ADDRESS_HYEX), &NBMISC_013C.Value, 0);
    NbioRegisterWrite (GnbHandle, NB_TOP_OF_DRAM3_TYPE, NBIO_SPACE(GnbHandle, NB_TOP_OF_DRAM3_ADDRESS_HYEX), &NBMISC_0138.Value, 0);
    NbioRegisterWrite (GnbHandle, NB_MMIOBASE_TYPE, NBIO_SPACE(GnbHandle, NB_MMIOBASE_ADDRESS_HYEX), &NB_MMIOBASE.Value, 0);
    NbioRegisterWrite (GnbHandle, NB_MMIOLIMIT_TYPE, NBIO_SPACE(GnbHandle, NB_MMIOLIMIT_ADDRESS_HYEX), &NB_MMIOLIMIT.Value, 0);
    NbioRegisterRMW (GnbHandle,
                     NB_PCI_CTRL_TYPE,
                     NBIO_SPACE(GnbHandle, NB_PCI_CTRL_ADDRESS_HYEX),
                     (UINT32) ~(NB_PCI_CTRL_MMIOEnable_MASK),
                     (1 << NB_PCI_CTRL_MMIOEnable_OFFSET),
                     0
                     );

    GnbHandle = GnbGetNextHandle (GnbHandle);
  }
  HGPI_TESTPOINT (TpMemoryConfigDoneCallbackPpiExit, NULL);

  return EFI_SUCCESS;
}


