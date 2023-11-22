/* $NoKeywords:$ */
/**
 * @file
 *
 * NTB Feature Initialization
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonNbioPciePei
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
#include <PiPei.h>
#include <Filecode.h>
#include <GnbHsio.h>
#include <GnbRegisters.h>
//In HgpiPkg
#include <HygonPcieComplex.h>

#include "HygonNbioPciePei.h"

#include <Ppi/NbioSmuServicesPpi.h>
#include <Ppi/NbioBaseServicesSTPpi.h>
#include <Ppi/NbioPcieServicesPpi.h>

#include <Library/HygonBaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/IdsLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>

#include <Hsio.h>
#include <Library/NbioRegisterAccLib.h>

#include <Guid/GnbPcieInfoHob.h>
#include <Guid/GnbNbioBaseSTInfoHob.h>

#define FILECODE  NBIO_PCIE_HYGONNBIOPCIESTPEI_NTBFEATURE_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define HsioInputParsetGetNextDescriptor(Descriptor)  (Descriptor != NULL ? ((((Descriptor->Flags & DESCRIPTOR_TERMINATE_LIST) != 0) ? NULL : (++Descriptor))) : NULL)

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

/**----------------------------------------------------------------------------------------*/

/**
 * PCIE interface to configure register setting
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    HGPI_STATUS
 */
/*----------------------------------------------------------------------------------------*/

VOID
NtbHotplugPortUserConfig (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       HSIO_COMPLEX_DESCRIPTOR    *ComplexDescriptor,
  IN       UINT32                     PcieCore,
  IN       UINT32                     GenSpeed
  )
{
  PCIe_WRAPPER_CONFIG      *PcieWrapper;
  BOOLEAN                  WrapperFound;
  BOOLEAN                  EngineFound;
  HSIO_PORT_DESCRIPTOR     *EngineDescriptor;
  HSIO_COMPLEX_DESCRIPTOR  *LocalComplex;

  IDS_HDT_CONSOLE (GNB_TRACE, "NtbHotplugPortUserConfig Enter\n");
  LocalComplex = ComplexDescriptor;

  PcieWrapper  = PcieConfigGetChildWrapper (GnbHandle);
  WrapperFound = FALSE;

  IDS_HDT_CONSOLE (GNB_TRACE, "Looking for WrapId == %d\n", PcieCore);

  while ((PcieWrapper != NULL) && (WrapperFound == FALSE)) {
    IDS_HDT_CONSOLE (GNB_TRACE, "  Checking wrapper %d\n", PcieWrapper->WrapId);
    if (PcieWrapper->WrapId == PcieCore) {
      WrapperFound = TRUE;
    } else {
      PcieWrapper = PcieConfigGetNextDescriptor (PcieWrapper);
    }
  }

  if (WrapperFound == TRUE) {
    IDS_HDT_CONSOLE (GNB_TRACE, "Wrapper Found\n");
    EngineFound = FALSE;
    IDS_HDT_CONSOLE (GNB_TRACE, "Looking for an engine descriptor that fits here...\n");
    IDS_HDT_CONSOLE (GNB_TRACE, "  This socket %d start lane = %d, end lane = %d \n", GnbHandle->SocketId, GnbHandle->SocketStartLane, GnbHandle->SocketEndLane);
    IDS_HDT_CONSOLE (GNB_TRACE, "  This wrapper start lane = %d\n", PcieWrapper->StartPhyLane);
    IDS_HDT_CONSOLE (GNB_TRACE, "  This wrapper end lane   = %d\n", PcieWrapper->EndPhyLane);
    if (GnbHandle->SocketId != 0) {
      LocalComplex = HsioInputParsetGetNextDescriptor (LocalComplex);
    }

    if (LocalComplex != NULL) {
      EngineDescriptor = LocalComplex->PciePortList;
      while ((EngineDescriptor != NULL)  && (EngineFound == FALSE)) {
        IDS_HDT_CONSOLE (GNB_TRACE, "\nEngineData.StartLane = %d\n", EngineDescriptor->EngineData.StartLane);
        IDS_HDT_CONSOLE (GNB_TRACE, "EngineData.EndLane   = %d\n", EngineDescriptor->EngineData.EndLane);
        if ((EngineDescriptor->EngineData.StartLane >= PcieWrapper->StartPhyLane) &&
            (EngineDescriptor->EngineData.EndLane <= PcieWrapper->EndPhyLane)) {
          EngineFound = TRUE;
          if (EngineDescriptor->Port.LinkHotplug == HsioHotplugDisabled) {
            IDS_HDT_CONSOLE (GNB_TRACE, "Found the right entry here...\n");
            EngineDescriptor->Port.LinkHotplug = HsioHotplugEnhanced;
          } else {
            IDS_HDT_CONSOLE (GNB_TRACE, "Hotplug already enabled\n");
          }

          IDS_HDT_CONSOLE (GNB_TRACE, "Setting NTB GenSpeed to %d\n", GenSpeed);
          EngineDescriptor->Port.LinkSpeedCapability = (UINT8)GenSpeed;
        } else {
          EngineDescriptor = HsioInputParsetGetNextDescriptor (EngineDescriptor);
        }
      }
    }
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "NtbHotplugPortUserConfig Exit\n");
}

/*----------------------------------------------------------------------------------------*/

/**
 * Initialize NTB on PCIe core
 *
 *
 * @param[in]  PeiServices       Pointer to EFI_PEI_SERVICES pointer
 */
VOID
InitNTBFeature (
  IN CONST EFI_PEI_SERVICES           **PeiServices,
  IN       HSIO_COMPLEX_DESCRIPTOR    *PcieTopologyData,
  IN       PCIe_PLATFORM_CONFIG       *Pcie
  )
{
  EFI_STATUS                        Status;
  GNB_HANDLE                        *GnbHandle;
  PEI_HYGON_NBIO_BASE_SERVICES_PPI  *NbioBaseServicesPpi;
  GNB_BUILD_OPTIONS_ST_DATA_HOB     *GnbBuildOptionsSTDataHob;
  NTBPCIE_LOCATION_STRUCT           NTBLocation;
  UINT32                            Value;
  UINT32                            Address;

  IDS_HDT_CONSOLE (GNB_TRACE, "InitPCIeNTB Entry\n");
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonNbioBaseServicesStPpiGuid,
                             0,
                             NULL,
                             (VOID **)&NbioBaseServicesPpi
                             );
  if (Status == EFI_SUCCESS) {
    Status = NbioBaseServicesPpi->DebugOptions (NbioBaseServicesPpi, (GNB_BUILD_OPTIONS_ST_DATA_HOB **)&GnbBuildOptionsSTDataHob);
    if (Status == EFI_SUCCESS) {
      IDS_HDT_CONSOLE (GNB_TRACE, " CfgNTBLocation = %d\n", GnbBuildOptionsSTDataHob->CfgNTBLocation);
      IDS_HDT_CONSOLE (GNB_TRACE, " CfgNTBPcieCoreSel = %d\n", GnbBuildOptionsSTDataHob->CfgNTBPcieCoreSel);
      IDS_HDT_CONSOLE (GNB_TRACE, " CfgNTBMode = %d\n", GnbBuildOptionsSTDataHob->CfgNTBMode);
      IDS_HDT_CONSOLE (GNB_TRACE, " CfgNTBLinkSpeed = %d\n", GnbBuildOptionsSTDataHob->CfgNTBLinkSpeed);

      if (GnbBuildOptionsSTDataHob->CfgNTBEnable == TRUE) {
        IDS_HDT_CONSOLE (GNB_TRACE, " CfgNTBEnable is TRUE\n");
        GnbHandle = NbioGetHandle (Pcie);
        while (GnbHandle != NULL) {
          if (GnbBuildOptionsSTDataHob->CfgNTBLocation == GnbHandle->InstanceId){
            IDS_HDT_CONSOLE (GNB_TRACE, "  Found InstanceId = %d\n", GnbHandle->InstanceId);

            switch (GnbBuildOptionsSTDataHob->CfgNTBMode) {

            case 0:           // NTB Disabled
              NTBLocation.Field.Core = 0x0000;
              NTBLocation.Field.Port = 0x0000;
              NbioRegisterWrite (GnbHandle, TYPE_SMN, SDPMUX_SPACE_HYEX(GnbHandle, NTBPCIE_LOCATION_ADDRESS_HYEX), &NTBLocation, 0);
              NbioRegisterRMW (GnbHandle,
                               TYPE_SMN,
                               SDPMUX_SPACE_HYEX(GnbHandle, NTBPCIE_UNITID_ADDRESS_HYEX),
                               (UINT32) ~(NTBPCIE_UnitID_MASK),
                               0 << NTBPCIE_UnitID_OFFSET,
                               0
                               );
              break;

            case 1:           // NTB Primary
              // Update the user config for this port
              NtbHotplugPortUserConfig (GnbHandle,
                                        PcieTopologyData,
                                        (GnbBuildOptionsSTDataHob->CfgNTBPcieCoreSel == 0 ? 0 : 1),
                                        GnbBuildOptionsSTDataHob->CfgNTBLinkSpeed
                                        );
              // Enable NTB devices on this die
              NbioEnableNbifDevice (GnbHandle, HsioNTBDevice, 0);
              if (GnbBuildOptionsSTDataHob->CfgNTBPcieCoreSel == 0) {
                // GnbBuildOptionsSTDataHob->CfgNTBPcieCoreSel == 0
                NTBLocation.Field.Core = 0x0001;
                NTBLocation.Field.Port = 0x0001;
                NbioRegisterWrite (GnbHandle, TYPE_SMN, SDPMUX_SPACE_HYEX(GnbHandle, NTBPCIE_LOCATION_ADDRESS_HYEX), &NTBLocation.Value, 0);
                NbioRegisterRMW (GnbHandle,
                                 TYPE_SMN,
                                 SDPMUX_SPACE_HYEX(GnbHandle, NTBPCIE_UNITID_ADDRESS_HYEX),
                                 (UINT32) ~(NTBPCIE_UnitID_MASK),
                                 0x10 << NTBPCIE_UnitID_OFFSET,
                                 0
                                 );
              } else {
                // GnbBuildOptionsSTDataHob->CfgNTBPcieCoreSel == 1
                NTBLocation.Field.Core = 0x0002;
                NTBLocation.Field.Port = 0x0001;
                NbioRegisterWrite (GnbHandle, TYPE_SMN, SDPMUX_SPACE_HYEX(GnbHandle, NTBPCIE_LOCATION_ADDRESS_HYEX), &NTBLocation.Value, 0);
                NbioRegisterRMW (GnbHandle,
                                 TYPE_SMN,
                                 SDPMUX_SPACE_HYEX(GnbHandle, NTBPCIE_UNITID_ADDRESS_HYEX),
                                 (UINT32) ~(NTBPCIE_UnitID_MASK),
                                 0x18 << NTBPCIE_UnitID_OFFSET,
                                 0
                                 );
                NbioRegisterRMW (GnbHandle,
                                 BIFC_MISC_CTRL0_TYPE,
                                 NBIO_SPACE(GnbHandle, NBIF2_BIFC_MISC_CTRL0_ADDRESS_HYEX),
                                 (UINT32) ~(BIFC_MISC_CTRL0_PCIESWUS_SELECTION_MASK),
                                 (UINT32) 0x1 << BIFC_MISC_CTRL0_PCIESWUS_SELECTION_OFFSET,
                                 0
                                 );
              }

              //#step 2
              //#program nBIF2's STRAP_CFG_CRS_EN_DEV0 to 1 to wait for initial primary side
              NbioRegisterRMW (GnbHandle,
                               RCC_DEV0_PORT_STRAP6_TYPE,
                               NBIO_SPACE(GnbHandle, NBIF0RCC_DEV0_PORT_STRAP6_ADDRESS_HYEX),
                               (UINT32) ~(STRAP_CFG_CRS_EN_DEV0_MASK),
                               1 << STRAP_CFG_CRS_EN_DEV0_OFFSET,
                               0
                               );
              NbioRegisterRMW (GnbHandle,
                               RCC_DEV0_PORT_STRAP6_TYPE,
                               NBIO_SPACE(GnbHandle, NBIF2RCC_DEV0_PORT_STRAP6_ADDRESS_HYEX),
                               (UINT32) ~(STRAP_CFG_CRS_EN_DEV0_MASK),
                               1 << STRAP_CFG_CRS_EN_DEV0_OFFSET,
                               0
                               );

              //# initialize internal EP device
              //#Step 3
              //# program strap_chip_bif_mode[7:0] to 0 to enable PCIE port to sw.us
              Value = STRAP_CHIP_BIF_MODE_ADDR; 
              if (GnbBuildOptionsSTDataHob->CfgNTBPcieCoreSel == 0) {
                  Address = PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE0_STRAP_INDEX_ADDRESS);
              } else {
                  Address = PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE1_STRAP_INDEX_ADDRESS);
              }                      
              NbioRegisterWrite (GnbHandle,
                                 TYPE_SMN,
                                 Address,
                                 &Value,
                                 0
                                 );
              
              Value = 0xFE;
              if (GnbBuildOptionsSTDataHob->CfgNTBPcieCoreSel == 0) {
                  Address = PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE0_STRAP_DATA_ADDRESS);
              } else {
                  Address = PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE1_STRAP_DATA_ADDRESS);       
              }    
              NbioRegisterWrite (GnbHandle,
                                 TYPE_SMN,
                                 Address,
                                 &Value,
                                 0
                                 );

              //#set target speed
              //"PPR::NBIFRCCFG::socket0::die0::NBIF0::LINK_CNTL2"
              NbioRegisterRMW (GnbHandle,
                               NBIF_LINK_CNTL2_TYPE,
                               NBIO_SPACE(GnbHandle, NBIF0_LINK_CNTL2_ADDRESS_HYEX),
                               (UINT32) ~(NBIF_LINK_CNTL2_TARGET_LINK_SPEED_MASK),
                               GnbBuildOptionsSTDataHob->CfgNTBLinkSpeed << NBIF_LINK_CNTL2_TARGET_LINK_SPEED_OFFSET,
                               0
                               );
              NbioRegisterRMW (GnbHandle,
                               NBIF_LINK_CNTL2_TYPE,
                               NBIO_SPACE(GnbHandle, NBIF2_LINK_CNTL2_ADDRESS_HYEX),
                               (UINT32) ~(NBIF_LINK_CNTL2_TARGET_LINK_SPEED_MASK),
                               GnbBuildOptionsSTDataHob->CfgNTBLinkSpeed << NBIF_LINK_CNTL2_TARGET_LINK_SPEED_OFFSET,
                               0
                               );
              //#step 5
              //#program nBIF2's STRAP_CFG_CRS_EN_DEV0 to 0 to enable sw.ds
              NbioRegisterRMW (GnbHandle,
                               RCC_DEV0_PORT_STRAP6_TYPE,
                               NBIO_SPACE(GnbHandle, NBIF0RCC_DEV0_PORT_STRAP6_ADDRESS_HYEX),
                               (UINT32) ~(STRAP_CFG_CRS_EN_DEV0_MASK),
                               0 << STRAP_CFG_CRS_EN_DEV0_OFFSET,
                               0
                                );
              NbioRegisterRMW (GnbHandle,
                               RCC_DEV0_PORT_STRAP6_TYPE,
                               NBIO_SPACE(GnbHandle, NBIF2RCC_DEV0_PORT_STRAP6_ADDRESS_HYEX),
                               (UINT32) ~(STRAP_CFG_CRS_EN_DEV0_MASK),
                               0 << STRAP_CFG_CRS_EN_DEV0_OFFSET,
                               0
                               );
              //set bar23/bar45 prefetchable
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NBIF0RCC_DEV0_EPF1_STRAP6_ADDRESS_HYEX),
                (UINT32) ~( STRAP_APER2_PREFETCHABLE_EN_DEV0_F1_MASK | STRAP_APER3_PREFETCHABLE_EN_DEV0_F1_MASK ),
                (UINT32)((1 << STRAP_APER2_PREFETCHABLE_EN_DEV0_F1_OFFSET)|(1<<STRAP_APER3_PREFETCHABLE_EN_DEV0_F1_OFFSET)),
                0
                );  
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NBIF2RCC_DEV0_EPF1_STRAP6_ADDRESS_HYEX),
                (UINT32) ~( STRAP_APER2_PREFETCHABLE_EN_DEV0_F1_MASK | STRAP_APER3_PREFETCHABLE_EN_DEV0_F1_MASK ),
                (UINT32)((1 << STRAP_APER2_PREFETCHABLE_EN_DEV0_F1_OFFSET)|(1<<STRAP_APER3_PREFETCHABLE_EN_DEV0_F1_OFFSET)),
                0
                );          
              //nbif0 enable bar sizing
              IDS_HDT_CONSOLE (GNB_TRACE, "enable bar sizing\n");
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NBIF0RCC_DEV0_EPF1_STRAP2_ADDRESS_HYEX),
                (UINT32) ~( STRAP_RESIZE_BAR_EN_DEV0_F1_MASK ),
                1 << STRAP_RESIZE_BAR_EN_DEV0_F1_OFFSET,
                0
                );  
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NBIF2RCC_DEV0_EPF1_STRAP2_ADDRESS_HYEX),
                (UINT32) ~( STRAP_RESIZE_BAR_EN_DEV0_F1_MASK ),
                1 << STRAP_RESIZE_BAR_EN_DEV0_F1_OFFSET,
                0
                );          
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NBIF0RCC_DEV0_EPF1_STRAP10_ADDRESS_HYEX),
                (UINT32) ~(STRAP_APER1_RESIZE_EN_DEV0_F1_MASK | STRAP_APER1_RESIZE_SUPPORT_DEV0_F1_MASK),
                1 << STRAP_APER1_RESIZE_EN_DEV0_F1_OFFSET | 0xFFFFF << STRAP_APER1_RESIZE_SUPPORT_DEV0_F1_OFFSET,
                0
                );
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NBIF0RCC_DEV0_EPF1_STRAP11_ADDRESS_HYEX),
                (UINT32) ~(STRAP_APER2_RESIZE_EN_DEV0_F1_MASK | STRAP_APER2_RESIZE_SUPPORT_DEV0_F1_MASK),
                1 << STRAP_APER2_RESIZE_EN_DEV0_F1_OFFSET | 0xFFFFF << STRAP_APER2_RESIZE_SUPPORT_DEV0_F1_OFFSET,
                0
                );
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NBIF0RCC_DEV0_EPF1_STRAP12_ADDRESS_HYEX),
                (UINT32) ~(STRAP_APER3_RESIZE_EN_DEV0_F1_MASK | STRAP_APER3_RESIZE_SUPPORT_DEV0_F1_MASK),
                1 << STRAP_APER3_RESIZE_EN_DEV0_F1_OFFSET | 0xFFFFF << STRAP_APER3_RESIZE_SUPPORT_DEV0_F1_OFFSET,
                0
                );
              //nbif2 enable bar sizing
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NBIF2RCC_DEV0_EPF1_STRAP10_ADDRESS_HYEX),
                (UINT32) ~(STRAP_APER1_RESIZE_EN_DEV0_F1_MASK | STRAP_APER1_RESIZE_SUPPORT_DEV0_F1_MASK),
                1 << STRAP_APER1_RESIZE_EN_DEV0_F1_OFFSET | 0xFFFFF << STRAP_APER1_RESIZE_SUPPORT_DEV0_F1_OFFSET,
                0
                );
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NBIF2RCC_DEV0_EPF1_STRAP11_ADDRESS_HYEX),
                (UINT32) ~(STRAP_APER2_RESIZE_EN_DEV0_F1_MASK | STRAP_APER2_RESIZE_SUPPORT_DEV0_F1_MASK),
                1 << STRAP_APER2_RESIZE_EN_DEV0_F1_OFFSET | 0xFFFFF << STRAP_APER2_RESIZE_SUPPORT_DEV0_F1_OFFSET,
                0
                );
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NBIF2RCC_DEV0_EPF1_STRAP12_ADDRESS_HYEX),
                (UINT32) ~(STRAP_APER3_RESIZE_EN_DEV0_F1_MASK | STRAP_APER3_RESIZE_SUPPORT_DEV0_F1_MASK),
                1 << STRAP_APER3_RESIZE_EN_DEV0_F1_OFFSET | 0xFFFFF << STRAP_APER3_RESIZE_SUPPORT_DEV0_F1_OFFSET,
                0
                );
              break;

            case 2:           // NTB Secondary
              // Update the user config for this port
              NtbHotplugPortUserConfig (GnbHandle,
                                        PcieTopologyData,
                                        (GnbBuildOptionsSTDataHob->CfgNTBPcieCoreSel == 0 ? 0 : 1),
                                        GnbBuildOptionsSTDataHob->CfgNTBLinkSpeed
                                        );
              NbioRegisterRMW (GnbHandle,
                               TYPE_SMN,
                               NBIO_SPACE(GnbHandle, NBIFMISC0_INTR_LINE_ENABLE_ADDRESS_HYEX),
                               (UINT32) ~(1 << 1),
                               (1 << 1),
                               0
                               );
              NbioRegisterRMW (GnbHandle,
                               TYPE_SMN,
                               NBIO_SPACE(GnbHandle, NBIFMISC2_INTR_LINE_ENABLE_ADDRESS_HYEX),
                               (UINT32) ~(1 << 1),
                               (1 << 1),
                               0
                               );
              break;

            default:
              break;
            }
          }
          GnbHandle = GnbGetNextHandle (GnbHandle);
        }
      }
    }
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "InitPCIeNTB Exit\n");
}

VOID
GetBarXSize(
 IN  UINT16 NTBBarSizeIndex,
 OUT UINT32 *BarXSizeLo,
 OUT UINT32 *BarXSizeHi
)
{
  if (NTBBarSizeIndex > 0x1b) {
    *BarXSizeLo = 0;
    if (BarXSizeHi != NULL) {
      *BarXSizeHi = 0;
    }
    return;
  }
  if (NTBBarSizeIndex < 0xc) {
    *BarXSizeLo = (1 << NTBBarSizeIndex) * 1024 * 1024 - 1;
    if (BarXSizeHi != NULL) {
      *BarXSizeHi = 0;
    }
  } else {
    *BarXSizeLo = 0xFFFFFFFF;
    if (BarXSizeHi != NULL) {
      *BarXSizeHi = (1 << (NTBBarSizeIndex - 0xc)) - 1;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize NTB after training
 *
 *
 * @param[in]  PeiServices       Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  PcieTopologyData  Pointer to UserConfig
 * @param[in]  Pcie              Pointer to Pcie topology structure
 */
VOID
InitNTBAfterTraining (
  IN CONST EFI_PEI_SERVICES           **PeiServices,
  IN       HSIO_COMPLEX_DESCRIPTOR    *PcieTopologyData,
  IN       PCIe_PLATFORM_CONFIG       *Pcie
  )
{
  EFI_STATUS                        Status;
  GNB_HANDLE                        *GnbHandle;
  PEI_HYGON_NBIO_BASE_SERVICES_PPI    *NbioBaseServicesPpi;
  GNB_BUILD_OPTIONS_ST_DATA_HOB     *GnbBuildOptionsSTDataHob;
  PCIe_WRAPPER_CONFIG               *PcieWrapper;
  PCIe_ENGINE_CONFIG                *PcieEngine;
  BOOLEAN                           WrapperFound;
  BOOLEAN                           EngineFound;
  UINT32                            PcieCore;
  UINT32                            PhysicalBar1Size;
  UINT32                            PhysicalBar23SizeLo;
  UINT32                            PhysicalBar23SizeHi;
  UINT32                            PhysicalBar45SizeLo;
  UINT32                            PhysicalBar45SizeHi;
  
  IDS_HDT_CONSOLE (GNB_TRACE, "InitNTBAfterTraining Entry\n");
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonNbioBaseServicesStPpiGuid,
                             0,
                             NULL,
                             (VOID **)&NbioBaseServicesPpi
                             );
  if (Status == EFI_SUCCESS) {
    Status = NbioBaseServicesPpi->DebugOptions (NbioBaseServicesPpi, (GNB_BUILD_OPTIONS_ST_DATA_HOB **)&GnbBuildOptionsSTDataHob);
    if (Status == EFI_SUCCESS) {
      if (GnbBuildOptionsSTDataHob->CfgNTBEnable == TRUE) {
        IDS_HDT_CONSOLE (GNB_TRACE, " CfgNTBEnable is TRUE\n");
        GnbHandle = NbioGetHandle (Pcie);
        while (GnbHandle != NULL) {
          if (GnbBuildOptionsSTDataHob->CfgNTBLocation == GnbHandle->InstanceId) {
            IDS_HDT_CONSOLE (GNB_TRACE, "  Found InstanceId = %d\n", GnbHandle->InstanceId);

            // Special case for NTB primary
            // Device is hotplug configured, but it needs to be hidden
            if (GnbBuildOptionsSTDataHob->CfgNTBMode == 1) {
              PcieCore = GnbBuildOptionsSTDataHob->CfgNTBPcieCoreSel == 0 ? 0 : 1;

              PcieWrapper  = PcieConfigGetChildWrapper (GnbHandle);
              WrapperFound = FALSE;

              IDS_HDT_CONSOLE (GNB_TRACE, "Looking for WrapId == %d\n", PcieCore);

              while ((PcieWrapper != NULL) && (WrapperFound == FALSE)) {
                IDS_HDT_CONSOLE (GNB_TRACE, "  Checking wrapper %d\n", PcieWrapper->WrapId);
                if (PcieWrapper->WrapId == PcieCore) {
                  WrapperFound = TRUE;
                } else {
                  PcieWrapper = PcieConfigGetNextDescriptor (PcieWrapper);
                }
              }

              if (WrapperFound == TRUE) {
                IDS_HDT_CONSOLE (GNB_TRACE, "Wrapper Found\n");
                EngineFound = FALSE;
                PcieEngine  = PcieConfigGetChildEngine (PcieWrapper);
                while ((PcieEngine != NULL) && (EngineFound == FALSE)) {
                  if ((PcieEngine->Type.Port.PortData.PortPresent == TRUE) && (PcieEngine->Type.Port.PortId == 0)) {
                    NbioRegisterRMW (
                      GnbHandle,
                      TYPE_SMN,
                      IOHC_BRIDGE_SPACE (GnbHandle, PcieEngine, NB_DEVINDCFG0_IOHC_Bridge_CNTL_ADDRESS_HYEX),
                      (UINT32) ~(IOHC_BRIDGE_CNTL_BridgeDis_MASK | IOHC_BRIDGE_CNTL_CfgDis_MASK |
                                 IOHC_BRIDGE_CNTL_CrsEnable_MASK),
                      (UINT32)(IOHC_BRIDGE_CNTL_BridgeDis_MASK | IOHC_BRIDGE_CNTL_CfgDis_MASK |
                               IOHC_BRIDGE_CNTL_CrsEnable_MASK),
                      0
                      );
                    EngineFound = TRUE;
                  }

                  PcieEngine = PcieConfigGetNextDescriptor (PcieEngine);
                }
              }
              
              //set nibf0 bar resource
              IDS_HDT_CONSOLE (GNB_TRACE, " Set bar resource bar1size Index:%x  bar23size Index:%x bar45size Index:%x\n",
                  GnbBuildOptionsSTDataHob->CfgNTBBAR1Size, GnbBuildOptionsSTDataHob->CfgNTBBAR23Size, GnbBuildOptionsSTDataHob->CfgNTBBAR45Size);
              //BAR1 SIZE
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NBIF0D00F01PCIE_BAR1_CNTL_ADDRSS_HYEX),
                (UINT32)~(BAR_SIZE_MASK),
                (UINT32) (GnbBuildOptionsSTDataHob->CfgNTBBAR1Size << BAR_SIZE_OFFSET),
                0
                );
              //BAR23 SIZE
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NBIF0D00F01PCIE_BAR2_CNTL_ADDRSS_HYEX),
                (UINT32)~(BAR_SIZE_MASK),
                (UINT32) (GnbBuildOptionsSTDataHob->CfgNTBBAR23Size << BAR_SIZE_OFFSET),
                0
                );
              //BAR45 SIZE
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NBIF0D00F01PCIE_BAR3_CNTL_ADDRSS_HYEX),
                (UINT32)~(BAR_SIZE_MASK),
                (UINT32) (GnbBuildOptionsSTDataHob->CfgNTBBAR45Size << BAR_SIZE_OFFSET),
                0
                );
              //set nbif2 bar resource
              //BAR1 SIZE
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NBIF2D00F01PCIE_BAR1_CNTL_ADDRSS_HYEX),
                (UINT32)~(BAR_SIZE_MASK),
                (UINT32) (GnbBuildOptionsSTDataHob->CfgNTBBAR1Size << BAR_SIZE_OFFSET),
                0
                );             
              //BAR23 SIZE
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NBIF2D00F01PCIE_BAR2_CNTL_ADDRSS_HYEX),
                (UINT32)~(BAR_SIZE_MASK),
                (UINT32) (GnbBuildOptionsSTDataHob->CfgNTBBAR23Size << BAR_SIZE_OFFSET),
                0
                );
              //BAR45 SIZE
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NBIF2D00F01PCIE_BAR3_CNTL_ADDRSS_HYEX),
                (UINT32)~(BAR_SIZE_MASK),
                (UINT32) (GnbBuildOptionsSTDataHob->CfgNTBBAR45Size << BAR_SIZE_OFFSET),
                0
                );

              GetBarXSize(GnbBuildOptionsSTDataHob->CfgNTBBAR1Size, &PhysicalBar1Size, NULL);
              GetBarXSize(GnbBuildOptionsSTDataHob->CfgNTBBAR23Size, &PhysicalBar23SizeLo, &PhysicalBar23SizeHi);
              GetBarXSize(GnbBuildOptionsSTDataHob->CfgNTBBAR45Size, &PhysicalBar45SizeLo, &PhysicalBar45SizeHi);

              IDS_HDT_CONSOLE (GNB_TRACE, " NTB BAR SIZE: BAR1:%x - BAR23LO:%x - BAR23Hi:%x - BAR45LO:%x - BAR45HI:%x\n",
                PhysicalBar1Size,PhysicalBar23SizeLo,PhysicalBar23SizeHi,PhysicalBar45SizeLo,PhysicalBar45SizeHi);
              //BAR LMT SET
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NTB_CTRL_ADDRESS_HYEX),
                (UINT32)~(PMM_REG_CTRL_MASK | SMM_REG_CTRL_MASK),
                (UINT32) ((1 << PMM_REG_CTRL_OFFSET)|(1 << SMM_REG_CTRL_OFFSET)),
                0
                );
              //Primary 
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NTB_PBAR1LMT_ADDRESS_HYEX),
                (UINT32)~(NTB_PBAR1LMT_MASK),
                (UINT32) (PhysicalBar1Size & 0xfffff000),
                0
                );
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NTB_PBAR23LMT_LO_ADDRESS_HYEX),
                (UINT32)~(NTB_PBAR23LMT_LO_MASK),
                (UINT32) (PhysicalBar23SizeLo & 0xfffff000),
                0
                );
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NTB_PBAR23LMT_HI_ADDRESS_HYEX),
                (UINT32)~(NTB_PBAR23LMT_HI_MASK),
                (UINT32) (PhysicalBar23SizeHi),
                0
                );        
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NTB_PBAR45LMT_LO_ADDRESS_HYEX),
                (UINT32)~(NTB_PBAR45LMT_LO_MASK),
                (UINT32) (PhysicalBar45SizeLo  & 0xfffff000),
                0
                );
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NTB_PBAR45LMT_HI_ADDRESS_HYEX),
                (UINT32)~(NTB_PBAR45LMT_HI_MASK),
                (UINT32) (PhysicalBar45SizeHi),
                0
                );        
              //Secondary
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NTB_SBAR1LMT_ADDRESS_HYEX),
                (UINT32)~(NTB_SBAR1LMT_MASK),
                (UINT32) (PhysicalBar1Size & 0xfffff000),
                0
                );
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NTB_SBAR23LMT_LO_ADDRESS_HYEX),
                (UINT32)~(NTB_SBAR23LMT_LO_MASK),
                (UINT32) (PhysicalBar23SizeLo & 0xfffff000),
                0
                );
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NTB_SBAR23LMT_HI_ADDRESS_HYEX),
                (UINT32)~(NTB_SBAR23LMT_HI_MASK),
                (UINT32) (PhysicalBar23SizeHi),
                0
                );        
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NTB_SBAR45LMT_LO_ADDRESS_HYEX),
                (UINT32)~(NTB_SBAR45LMT_LO_MASK),
                (UINT32) (PhysicalBar45SizeLo  & 0xfffff000),
                0
                );
              NbioRegisterRMW (
                GnbHandle,
                TYPE_SMN,
                NBIO_SPACE (GnbHandle, NTB_SBAR45LMT_HI_ADDRESS_HYEX),
                (UINT32)~(NTB_SBAR45LMT_HI_MASK),
                (UINT32) (PhysicalBar45SizeHi),
                0
                );
            } else if (GnbBuildOptionsSTDataHob->CfgNTBMode == 2) {
              PcieCore = GnbBuildOptionsSTDataHob->CfgNTBPcieCoreSel == 0 ? 0 : 1;

              PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
              WrapperFound = FALSE;

              IDS_HDT_CONSOLE (GNB_TRACE, "Looking for WrapId == %d\n", PcieCore);

              while ((PcieWrapper != NULL) && (WrapperFound == FALSE)) {
                IDS_HDT_CONSOLE (GNB_TRACE, "  Checking wrapper %d\n", PcieWrapper->WrapId);
                if (PcieWrapper->WrapId == PcieCore) {
                  WrapperFound = TRUE;
                } else {
                  PcieWrapper = PcieConfigGetNextDescriptor (PcieWrapper);
                }
              }
              if (WrapperFound == TRUE) {
                IDS_HDT_CONSOLE (GNB_TRACE, "Wrapper Found\n");
                EngineFound = FALSE;
                PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
                while ((PcieEngine != NULL) && (EngineFound == FALSE)) {
                  if ((PcieEngine->Type.Port.PortData.PortPresent == TRUE) && (PcieEngine->Type.Port.PortId == 0)) {
                    NbioRegisterRMW (GnbHandle,
                                   TYPE_SMN,
                                   IOHC_BRIDGE_SPACE(GnbHandle, PcieEngine, NB_DEVINDCFG0_IOHC_Bridge_CNTL_ADDRESS_HYEX),
                                   (UINT32)~(IOHC_BRIDGE_CNTL_BridgeDis_MASK | IOHC_BRIDGE_CNTL_CfgDis_MASK | IOHC_BRIDGE_CNTL_CrsEnable_MASK),
                                   0,
                                   0
                                   );
                    EngineFound = TRUE;
                  }
                  PcieEngine = PcieConfigGetNextDescriptor (PcieEngine);
                }
              }
            }
          }
          GnbHandle = GnbGetNextHandle (GnbHandle);
        }
      }
    }
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "InitNTBAfterTraining Exit\n");
}