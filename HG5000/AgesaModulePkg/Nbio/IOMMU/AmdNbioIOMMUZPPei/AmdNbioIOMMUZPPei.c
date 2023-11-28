/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdNbioIOMMUZPPei Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioIOMMUZPPei
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
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
#include <AMD.h>
#include <Filecode.h>
#include <PiPei.h>
#include <GnbDxio.h>
#include <Library/PeiServicesLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/PcdLib.h>
#include <Library/AmdPspBaseLibV1.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/IdsLib.h>
#include <Library/GnbHeapLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/FabricResourceManagerLib.h>
#include <Library/GnbPciLib.h>
#include <Library/GnbLib.h>
#include <Library/GnbPciAccLib.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Guid/GnbNbioBaseZPInfoHob.h>
#include <Ppi/NbioPcieServicesPpi.h>

#define FILECODE   NBIO_IOMMU_AMDNBIOIOMMUZPPEI_AMDNBIOIOMMUZPPEI_FILECODE

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
 * AmdNbio IOMMU ZP PEI driver entry point
 *
 *
 *
 * @param[in]  FileHandle  Standard configuration header
 * @param[in]  PeiServices Pointer to EFI_PEI_SERVICES pointer
 * @retval     EFI_STATUS
 */

EFI_STATUS
EFIAPI
AmdNbioIOMMUZPPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{

  PEI_AMD_NBIO_PCIE_SERVICES_PPI     *PcieServicesPpi;
  GNB_PCIE_INFORMATION_DATA_HOB      *PciePlatformConfigHob;
  FABRIC_TARGET                      MmioTarget;
  FABRIC_MMIO_ATTRIBUTE              MmioAttr;
  UINT64                             IommMmioSize;
  UINT64                             IommMmioBase;
  GNB_HANDLE                         *GnbHandle;
  EFI_STATUS                         Status;
  PCIe_PLATFORM_CONFIG               *Pcie;
  PCI_ADDR                           IommuPciAddress;
  UINT32                             Value;

  if (PcdGetBool (PcdCfgIommuMMIOAddressReservedEnable) == 0x00) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "We don't need reserved IOMMU MMIO space from GNB PEIM \n");
    return EFI_SUCCESS;
  }

  AGESA_TESTPOINT (TpNbioIommuPEIEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioIOMMUZPPEIEntry Entry\n");

  // Need topology structure
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gAmdNbioPcieServicesPpiGuid,
                             0,
                             NULL,
                             (VOID **)&PcieServicesPpi
                             );

  if (Status == EFI_SUCCESS) {
    PcieServicesPpi->PcieGetTopology (PcieServicesPpi, &PciePlatformConfigHob);
    Pcie = &(PciePlatformConfigHob->PciePlatformConfigHob);
    GnbHandle = NbioGetHandle (Pcie);
    while (GnbHandle != NULL) {

      MmioTarget.TgtType = TARGET_DIE;
      MmioTarget.SocketNum = GnbHandle->SocketId;
      MmioTarget.DieNum = GnbHandle->DieNumber;
      IommMmioSize = SIZE_512KB;
      MmioAttr.MmioType = NON_PCI_DEVICE_BELOW_4G;
      FabricAllocateMmio (&IommMmioBase, &IommMmioSize, ALIGN_512K, MmioTarget, &MmioAttr);
      IDS_HDT_CONSOLE (MAIN_FLOW, "IOMMU MMIO at address 0x%x for Socket 0x%x Silicon 0x%x\n", IommMmioBase, GnbHandle->SocketId, GnbHandle->DieNumber);

      Value = (UINT32)IommMmioBase;
      IommuPciAddress = NbioGetHostPciAddress (GnbHandle);
      IommuPciAddress.Address.Function = 0x2;
      GnbLibPciWrite (IommuPciAddress.AddressValue | 0x44, AccessS3SaveWidth32, &Value, NULL);

      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioIOMMUZPPEIEntry Exit\n");

  AGESA_TESTPOINT (TpNbioIommuPEIExit, NULL);
  return EFI_SUCCESS;
}
