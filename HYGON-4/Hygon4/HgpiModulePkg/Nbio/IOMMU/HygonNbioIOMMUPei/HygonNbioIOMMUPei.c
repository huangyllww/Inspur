/* $NoKeywords:$ */

/**
 * @file
 *
 * HygonNbioIOMMUPei Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonNbioIOMMUSATPei
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
#include <Filecode.h>
#include <PiPei.h>
#include <GnbHsio.h>
#include <Library/PeiServicesLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/PcdLib.h>
#include <Library/HygonPspBaseLib.h>
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
#include <Guid/GnbNbioBaseSTInfoHob.h>
#include <Ppi/NbioPcieServicesPpi.h>
#include <GnbRegisters.h>
#include <Library/NbioTable.h>
#include <Library/HygonSocBaseLib.h>
#include <Ppi/NbioBaseServicesSTPpi.h>

#define FILECODE  NBIO_IOMMU_HYGONNBIOIOMMUSTPEI_HYGONNBIOIOMMUSTPEI_FILECODE
extern GNB_TABLE ROMDATA  IommuInitSequenceHyEx[];
extern GNB_TABLE ROMDATA  IommuInitSequenceHyGx[];
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
 * HygonNbio IOMMU PEI driver entry point
 *
 *
 *
 * @param[in]  FileHandle  Standard configuration header
 * @param[in]  PeiServices Pointer to EFI_PEI_SERVICES pointer
 * @retval     EFI_STATUS
 */
EFI_STATUS
EFIAPI
HygonNbioIOMMUPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  PEI_HYGON_NBIO_PCIE_SERVICES_PPI  *PcieServicesPpi;
  GNB_PCIE_INFORMATION_DATA_HOB     *PciePlatformConfigHob;
  FABRIC_TARGET                     MmioTarget;
  FABRIC_MMIO_ATTRIBUTE             MmioAttr;
  UINT64                            IommMmioSize;
  UINT64                            IommMmioBase;
  GNB_HANDLE                        *GnbHandle;
  EFI_STATUS                        Status;
  PCIe_PLATFORM_CONFIG              *Pcie;
  PCI_ADDR                          IommuPciAddress;
  UINT32                            Value;
  BOOLEAN                           ReserveIommuMmioResource;
  UINT32                            Property = 0;
  IOMMU_MMIO_CONTROL0_W_STRUCT      MmioControl0;
  UINT16                            CapabilityOffset;
  UINT32                            HygonCpuModel;
  PEI_HYGON_NBIO_BASE_SERVICES_PPI  *NbioBaseServices;
  GNB_BUILD_OPTIONS_ST_DATA_HOB     *GnbBuildOptionData;

  if (PcdGetBool (PcdCfgIommuMMIOAddressReservedEnable) == FALSE) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "We don't need reserved IOMMU MMIO space from GNB PEIM \n");
    ReserveIommuMmioResource = FALSE;
  } else {
    IDS_HDT_CONSOLE (MAIN_FLOW, "We need reserved IOMMU MMIO space from GNB PEIM \n");
    ReserveIommuMmioResource = TRUE;
  }

  HGPI_TESTPOINT (TpNbioIommuPEIEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioIOMMUPeiEntry Entry\n");

  if (PcdGetBool (PcdX2ApicMode)) {
    Property |= TABLE_PROPERTY_IOMMU_X2APIC_SUPPORT;
    IDS_HDT_CONSOLE (MAIN_FLOW, "Set TABLE_PROPERTY_IOMMU_X2APIC_SUPPORT \n");
  }

  HygonCpuModel = GetHygonSocModel();

  Status = (*PeiServices)->LocatePpi (
                           PeiServices,
                           &gHygonNbioBaseServicesStPpiGuid,
                           0,
                           NULL,
                           (VOID **)&NbioBaseServices
                           );
  
  NbioBaseServices->DebugOptions (NbioBaseServices, &GnbBuildOptionData);

  Property |= GnbBuildOptionData->CfgIommuSupport ? 0 : TABLE_PROPERTY_IOMMU_DISABLED;
  Property |= GnbBuildOptionData->GnbCommonOptions.CfgIommuL2ClockGatingEnable ? TABLE_PROPERTY_IOMMU_L2_CLOCK_GATING : 0;
  Property |= GnbBuildOptionData->GnbCommonOptions.CfgIommuL1ClockGatingEnable ? TABLE_PROPERTY_IOMMU_L1_CLOCK_GATING : 0;

  // Need topology structure
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonNbioPcieServicesPpiGuid,
                             0,
                             NULL,
                             (VOID **)&PcieServicesPpi
                             );

  if (Status == EFI_SUCCESS) {
    PcieServicesPpi->PcieGetTopology (PcieServicesPpi, &PciePlatformConfigHob);
    Pcie = &(PciePlatformConfigHob->PciePlatformConfigHob);
    GnbHandle = NbioGetHandle (Pcie);
    while (GnbHandle != NULL) {
      if (!GnbHandle->IohubPresent) {
        GnbHandle = GnbGetNextHandle (GnbHandle);
        continue;
      }

      if (ReserveIommuMmioResource) {
        // allocate 512KB MMIO space below 4G for IOMMU
        MmioTarget.TgtType   = TARGET_RB;
        MmioTarget.SocketNum = GnbHandle->SocketId;
        MmioTarget.DieNum    = GnbHandle->LogicalDieId;
        MmioTarget.RbNum     = GnbHandle->RbId;
        IommMmioSize = SIZE_512KB;
        MmioAttr.MmioType = NON_PCI_DEVICE_BELOW_4G;
        FabricAllocateMmio (&IommMmioBase, &IommMmioSize, ALIGN_512K, MmioTarget, &MmioAttr);
        IDS_HDT_CONSOLE (
          MAIN_FLOW,
          "IOMMU MMIO at address 0x%x for Socket %d Die P%d L%d Rb %d\n",
          IommMmioBase,
          GnbHandle->SocketId,
          GnbHandle->PhysicalDieId,
          GnbHandle->LogicalDieId,
          GnbHandle->RbId
          );

        Value = (UINT32)IommMmioBase;
        IommuPciAddress = NbioGetHostPciAddress (GnbHandle);
        IommuPciAddress.Address.Function = 0x2;
        GnbLibPciWrite (IommuPciAddress.AddressValue | 0x44, AccessS3SaveWidth32, &Value, NULL);   // IOMMU_CAP_BASE_LO: BXXD00F2x044

        if (HygonCpuModel == HYGON_GX_CPU) {
          // vIOMMU base address
          CapabilityOffset = GnbLibFindPciCapability (IommuPciAddress.AddressValue, VIOMMU_CAP_ID, NULL);
          if (CapabilityOffset == 0) {
            IDS_HDT_CONSOLE (MAIN_FLOW, "vIOMMU Cap Not Found\n");
          } else {
            IommMmioSize = SIZE_256MB;
            MmioAttr.MmioType = NON_PCI_DEVICE_ABOVE_4G;
            FabricAllocateMmio (&IommMmioBase, &IommMmioSize, ALIGN_256M, MmioTarget, &MmioAttr);
            IDS_HDT_CONSOLE (
              MAIN_FLOW,
              "vIOMMU MMIO at address 0x%lx for Socket %d Die P%d L%d Rb %d\n",
              IommMmioBase,
              GnbHandle->SocketId,
              GnbHandle->PhysicalDieId,
              GnbHandle->LogicalDieId,
              GnbHandle->RbId
              );

            Value = (UINT32)IommMmioBase;
            IommuPciAddress = NbioGetHostPciAddress (GnbHandle);
            IommuPciAddress.Address.Function = 0x2;
            GnbLibPciWrite (IommuPciAddress.AddressValue | 0xD0, AccessS3SaveWidth32, &Value, NULL);   // IOMMU_VF_BASE_LO: BXXD00F2x0D0
            Value = (UINT32)(IommMmioBase >> 32);
            GnbLibPciWrite (IommuPciAddress.AddressValue | 0xD4, AccessS3SaveWidth32, &Value, NULL);   // IOMMU_VF_BASE_HI: BXXD00F2x0D4
            // vIOMMU control base address
            IommMmioBase = 0;
            IommMmioSize = SIZE_4MB;
            MmioAttr.MmioType = NON_PCI_DEVICE_ABOVE_4G;
            FabricAllocateMmio (&IommMmioBase, &IommMmioSize, ALIGN_4M, MmioTarget, &MmioAttr);
            IDS_HDT_CONSOLE (
              MAIN_FLOW,
              "vIOMMU control MMIO at address 0x%lx for Socket %d Die P%d L%d Rb %d\n",
              IommMmioBase,
              GnbHandle->SocketId,
              GnbHandle->PhysicalDieId,
              GnbHandle->LogicalDieId,
              GnbHandle->RbId
              );

            Value = (UINT32)IommMmioBase;
            IommuPciAddress = NbioGetHostPciAddress (GnbHandle);
            IommuPciAddress.Address.Function = 0x2;
            GnbLibPciWrite (IommuPciAddress.AddressValue | 0xD8, AccessS3SaveWidth32, &Value, NULL);   // IOMMU_VFCNTL_BASE_LO: BXXD00F2x0D8
            Value = (UINT32)(IommMmioBase >> 32);
            GnbLibPciWrite (IommuPciAddress.AddressValue | 0xDC, AccessS3SaveWidth32, &Value, NULL);   // IOMMU_VFCNTL_BASE_HI: BXXD00F2x0DC
          }
        }
      }

      // Do IOMMU initialize sequence according to satori iommu spec
      if (HygonCpuModel == HYGON_EX_CPU) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "Init IommuInitSequenceHyEx table start\n");
        Status = GnbProcessTable (
                  GnbHandle,
                  IommuInitSequenceHyEx,
                  Property,
                  0,
                  NULL
                  );
        IDS_HDT_CONSOLE (MAIN_FLOW, "Init IommuInitSequenceHyEx table end\n");

      } else {
        IDS_HDT_CONSOLE (MAIN_FLOW, "Init IommuInitSequenceHyGx table start\n");
        Status = GnbProcessTable (
                  GnbHandle,
                  IommuInitSequenceHyGx,
                  Property,
                  0,
                  NULL
                  );
        IDS_HDT_CONSOLE (MAIN_FLOW, "Init IommuInitSequenceHyGx table end\n");
      }

      if (PcdGetBool (PcdX2ApicMode)) {
        IommuPciAddress = NbioGetHostPciAddress (GnbHandle);
        IommuPciAddress.Address.Function = 0x2;
        GnbLibPciRead (IommuPciAddress.AddressValue | PCICFG_IOMMU_MMIO_CONTROL0_W_OFFSET, AccessWidth32, &(MmioControl0.Value), NULL);
        MmioControl0.Field.XT_SUP_W = 1;
        MmioControl0.Field.GA_SUP_W = 1;
        GnbLibPciWrite (IommuPciAddress.AddressValue | PCICFG_IOMMU_MMIO_CONTROL0_W_OFFSET, AccessS3SaveWidth32, &(MmioControl0.Value), NULL);
      }

      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioIOMMUPeiEntry Exit\n");

  HGPI_TESTPOINT (TpNbioIommuPEIExit, NULL);
  return EFI_SUCCESS;
}
