/* $NoKeywords:$ */
/**
 * @file
 *
 * NTB Feature Initialization
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioPcieZPPei
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
#include <PiPei.h>
#include <Filecode.h>
#include <GnbDxio.h>
#include <GnbRegistersZP.h>

//In AgesaPkg
#include <AmdPcieComplex.h>

#include "AmdNbioPcieZPPei.h"

#include <Ppi/NbioSmuServicesPpi.h>
#include <Ppi/NbioBaseServicesZPPpi.h>
#include <Ppi/NbioPcieServicesPpi.h>

#include <Library/AmdBaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/IdsLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>

#include <Library/DxioLibV1.h>
#include <Library/NbioRegisterAccLib.h>

#include <Guid/GnbPcieInfoHob.h>
#include <Guid/GnbNbioBaseZPInfoHob.h>


#define FILECODE        NBIO_PCIE_AMDNBIOPCIEZPPEI_NTBFEATURE_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define DxioInputParsetGetNextDescriptor(Descriptor) (Descriptor != NULL ? ((((Descriptor->Flags & DESCRIPTOR_TERMINATE_LIST) != 0) ? NULL : (++Descriptor))) : NULL)

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
 * @retval    AGESA_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
NtbHotplugPortUserConfig (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       DXIO_COMPLEX_DESCRIPTOR    *ComplexDescriptor,
  IN       UINT32                     PcieCore,
  IN       UINT32                     GenSpeed
  )
{
  PCIe_WRAPPER_CONFIG       *PcieWrapper;
  BOOLEAN                   WrapperFound;
  BOOLEAN                   EngineFound;
  DXIO_PORT_DESCRIPTOR      *EngineDescriptor;
  DXIO_COMPLEX_DESCRIPTOR   *LocalComplex;


  IDS_HDT_CONSOLE (GNB_TRACE, "NtbHotplugPortUserConfig Enter\n");
  LocalComplex = ComplexDescriptor;

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
    IDS_HDT_CONSOLE (GNB_TRACE, "Looking for an engine descriptor that fits here...\n");
    IDS_HDT_CONSOLE (GNB_TRACE, "  This die start lane     = %d\n", GnbHandle->StartLane);
    IDS_HDT_CONSOLE (GNB_TRACE, "  This wrapper start lane = %d\n", GnbHandle->StartLane + PcieWrapper->StartPhyLane);
    IDS_HDT_CONSOLE (GNB_TRACE, "  This wrapper end lane   = %d\n", GnbHandle->StartLane + PcieWrapper->EndPhyLane);
    if (GnbHandle->SocketId != 0) {
      LocalComplex = DxioInputParsetGetNextDescriptor (LocalComplex);
    }
    if (LocalComplex != NULL) {
      EngineDescriptor = LocalComplex->PciePortList;
      while ((EngineDescriptor != NULL)  && (EngineFound == FALSE)) {
        IDS_HDT_CONSOLE (GNB_TRACE, "\nEngineData.StartLane = %d\n", EngineDescriptor->EngineData.StartLane);
        IDS_HDT_CONSOLE (GNB_TRACE, "EngineData.EndLane   = %d\n", EngineDescriptor->EngineData.EndLane);
        if ((EngineDescriptor->EngineData.StartLane >= (PcieWrapper->StartPhyLane + GnbHandle->StartLane)) &&
            (EngineDescriptor->EngineData.EndLane <= (PcieWrapper->EndPhyLane + GnbHandle->StartLane))) {
          EngineFound = TRUE;
          if (EngineDescriptor->Port.LinkHotplug == DxioHotplugDisabled) {
            IDS_HDT_CONSOLE (GNB_TRACE, "Found the right entry here...\n");
            EngineDescriptor->Port.LinkHotplug = DxioHotplugEnhanced;
          } else {
            IDS_HDT_CONSOLE (GNB_TRACE, "Hotplug already enabled\n");

          }
          IDS_HDT_CONSOLE (GNB_TRACE, "Setting NTB GenSpeed to %d\n", GenSpeed);
          EngineDescriptor->Port.LinkSpeedCapability = (UINT8) GenSpeed;
        } else {
          EngineDescriptor = DxioInputParsetGetNextDescriptor (EngineDescriptor);
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
  IN       DXIO_COMPLEX_DESCRIPTOR    *PcieTopologyData,
  IN       PCIe_PLATFORM_CONFIG       *Pcie
  )
{
  EFI_STATUS                        Status;
  GNB_HANDLE                        *GnbHandle;
  PEI_AMD_NBIO_BASE_SERVICES_PPI    *NbioBaseServicesPpi;
  GNB_BUILD_OPTIONS_ZP_DATA_HOB     *GnbBuildOptionsZPDataHob;
  NTBPCIE_LOCATION_STRUCT           NTBLocation;
  UINT32                            Value;

  IDS_HDT_CONSOLE (GNB_TRACE, "InitPCIeNTB Entry\n");
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gAmdNbioBaseServicesZpPpiGuid,
                             0,
                             NULL,
                             (VOID **)&NbioBaseServicesPpi
                             );
  if (Status == EFI_SUCCESS) {
    Status = NbioBaseServicesPpi->DebugOptions (NbioBaseServicesPpi, (GNB_BUILD_OPTIONS_ZP_DATA_HOB **)&GnbBuildOptionsZPDataHob);
    if (Status == EFI_SUCCESS) {
      IDS_HDT_CONSOLE (GNB_TRACE, " CfgNTBLocation = %d\n", GnbBuildOptionsZPDataHob->CfgNTBLocation);
      IDS_HDT_CONSOLE (GNB_TRACE, " CfgNTBPcieCoreSel = %d\n", GnbBuildOptionsZPDataHob->CfgNTBPcieCoreSel);
      IDS_HDT_CONSOLE (GNB_TRACE, " CfgNTBMode = %d\n", GnbBuildOptionsZPDataHob->CfgNTBMode);
      IDS_HDT_CONSOLE (GNB_TRACE, " CfgNTBLinkSpeed = %d\n", GnbBuildOptionsZPDataHob->CfgNTBLinkSpeed);

      if (GnbBuildOptionsZPDataHob->CfgNTBEnable == TRUE) {
        IDS_HDT_CONSOLE (GNB_TRACE, " CfgNTBEnable is TRUE\n");
        GnbHandle = NbioGetHandle (Pcie);
        while (GnbHandle != NULL) {
          if (GnbBuildOptionsZPDataHob->CfgNTBLocation == GnbHandle->InstanceId) {
            IDS_HDT_CONSOLE (GNB_TRACE, "  Found InstanceId = %d\n", GnbHandle->InstanceId);

            switch (GnbBuildOptionsZPDataHob->CfgNTBMode) {

            case 0:           // NTB Disabled
              NTBLocation.Field.Core = 0x0000;
              NTBLocation.Field.Port = 0x0000;
              NbioRegisterWrite (GnbHandle, TYPE_SMN, NTBPCIE_LOCATION_ADDRESS, &NTBLocation, 0);
              NbioRegisterRMW (GnbHandle,
                               TYPE_SMN,
                               NTBPCIE_UNITID_ADDRESS,
                               (UINT32) ~(NTBPCIE_UnitID_MASK),
                               0 << NTBPCIE_UnitID_OFFSET,
                               0
                               );
              break;

            case 1:           // NTB Primary
              // Update the user config for this port
              NtbHotplugPortUserConfig (GnbHandle,
                                        PcieTopologyData,
                                        (GnbBuildOptionsZPDataHob->CfgNTBPcieCoreSel == 0 ? 0 : 1),
                                        GnbBuildOptionsZPDataHob->CfgNTBLinkSpeed
                                        );
              // Enable NTB devices on this die
              NbioEnableNbifDevice (GnbHandle, DxioNTBDevice, 0);
              if (GnbBuildOptionsZPDataHob->CfgNTBPcieCoreSel == 0) {
                // GnbBuildOptionsZPDataHob->CfgNTBPcieCoreSel == 0
                NTBLocation.Field.Core = 0x0001;
                NTBLocation.Field.Port = 0x0001;
                NbioRegisterWrite (GnbHandle, TYPE_SMN, NTBPCIE_LOCATION_ADDRESS, &NTBLocation.Value, 0);
                NbioRegisterRMW (GnbHandle,
                                 TYPE_SMN,
                                 NTBPCIE_UNITID_ADDRESS,
                                 (UINT32) ~(NTBPCIE_UnitID_MASK),
                                 0x10 << NTBPCIE_UnitID_OFFSET,
                                 0
                                 );
              } else {
                // GnbBuildOptionsZPDataHob->CfgNTBPcieCoreSel == 1
                NTBLocation.Field.Core = 0x0002;
                NTBLocation.Field.Port = 0x0001;
                NbioRegisterWrite (GnbHandle, TYPE_SMN, NTBPCIE_LOCATION_ADDRESS, &NTBLocation.Value, 0);
                NbioRegisterRMW (GnbHandle,
                                 TYPE_SMN,
                                 NTBPCIE_UNITID_ADDRESS,
                                 (UINT32) ~(NTBPCIE_UnitID_MASK),
                                 0x18 << NTBPCIE_UnitID_OFFSET,
                                 0
                                 );
                NbioRegisterRMW (GnbHandle,
                                 BIFC_MISC_CTRL0_TYPE,
                                 NBIF2_BIFC_MISC_CTRL0_TYPE,
                                 (UINT32) ~(BIFC_MISC_CTRL0_PCIESWUS_SELECTION_MASK),
                                 (UINT32) 0x1 << BIFC_MISC_CTRL0_PCIESWUS_SELECTION_OFFSET,
                                 0
                                 );
              }

              //#step 2
              //#program nBIF2's STRAP_CFG_CRS_EN_DEV0 to 1 to wait for initial primary side
              NbioRegisterRMW (GnbHandle,
                               RCC_DEV0_PORT_STRAP6_TYPE,
                               NBIF0RCC_DEV0_PORT_STRAP6_ADDRESS,
                               (UINT32) ~(STRAP_CFG_CRS_EN_DEV0_MASK),
                               1 << STRAP_CFG_CRS_EN_DEV0_OFFSET,
                               0
                               );
              NbioRegisterRMW (GnbHandle,
                               RCC_DEV0_PORT_STRAP6_TYPE,
                               NBIF2RCC_DEV0_PORT_STRAP6_ADDRESS,
                               (UINT32) ~(STRAP_CFG_CRS_EN_DEV0_MASK),
                               1 << STRAP_CFG_CRS_EN_DEV0_OFFSET,
                               0
                               );

              //# initialize internal EP device
              //#Step 3
              //# program strap_chip_bif_mode[7:0] to 0 to enable PCIE port to sw.us

			  Value = 0xFFFE0086;
              NbioRegisterWrite (GnbHandle,
                                 TYPE_SMN,
                                 (GnbBuildOptionsZPDataHob->CfgNTBPcieCoreSel == 0) ?  PCIE0_STRAP_INDEX_ADDRESS : PCIE1_STRAP_INDEX_ADDRESS,
                                 &Value,
                                 0
                                 );
              Value = 0xFE;
              NbioRegisterWrite (GnbHandle,
                                 TYPE_SMN,
                                 (GnbBuildOptionsZPDataHob->CfgNTBPcieCoreSel == 0) ?  PCIE0_STRAP_DATA_ADDRESS : PCIE1_STRAP_DATA_ADDRESS,
                                 &Value,
                                 0
                                 );

              //#set target speed
              //"PPR::NBIFRCCFG::socket0::die0::NBIF0::LINK_CNTL2"
              NbioRegisterRMW (GnbHandle,
                               NBIF_LINK_CNTL2_TYPE,
                               NBIF0_LINK_CNTL2_ADDRESS,
                               (UINT32) ~(NBIF_LINK_CNTL2_TARGET_LINK_SPEED_MASK),
                               GnbBuildOptionsZPDataHob->CfgNTBLinkSpeed << NBIF_LINK_CNTL2_TARGET_LINK_SPEED_OFFSET,
                               0
                               );
              NbioRegisterRMW (GnbHandle,
                               NBIF_LINK_CNTL2_TYPE,
                               NBIF2_LINK_CNTL2_ADDRESS,
                               (UINT32) ~(NBIF_LINK_CNTL2_TARGET_LINK_SPEED_MASK),
                               GnbBuildOptionsZPDataHob->CfgNTBLinkSpeed << NBIF_LINK_CNTL2_TARGET_LINK_SPEED_OFFSET,
                               0
                               );
              //#step 5
              //#program nBIF2's STRAP_CFG_CRS_EN_DEV0 to 0 to enable sw.ds
              NbioRegisterRMW (GnbHandle,
                               RCC_DEV0_PORT_STRAP6_TYPE,
                               NBIF0RCC_DEV0_PORT_STRAP6_ADDRESS,
                               (UINT32) ~(STRAP_CFG_CRS_EN_DEV0_MASK),
                               0 << STRAP_CFG_CRS_EN_DEV0_OFFSET,
                               0
                                );
              NbioRegisterRMW (GnbHandle,
                               RCC_DEV0_PORT_STRAP6_TYPE,
                               NBIF2RCC_DEV0_PORT_STRAP6_ADDRESS,
                               (UINT32) ~(STRAP_CFG_CRS_EN_DEV0_MASK),
                               0 << STRAP_CFG_CRS_EN_DEV0_OFFSET,
                               0
                               );
			  //set bar23/bar45 prefetchable
              NbioRegisterRMW (GnbHandle,
                               RCC_DEV0_EPF1_STRAP6_TYPE,
                               NBIF0RCC_DEV0_EPF1_STRAP6_ADDRESS,
                               (UINT32) ~( STRAP_APER2_PREFETCHABLE_EN_DEV0_F1_MASK | STRAP_APER3_PREFETCHABLE_EN_DEV0_F1_MASK ),
                               (UINT32)((1 << STRAP_APER2_PREFETCHABLE_EN_DEV0_F1_OFFSET)|(1<<STRAP_APER3_PREFETCHABLE_EN_DEV0_F1_OFFSET)),
                               0
                                );	
              NbioRegisterRMW (GnbHandle,
                               RCC_DEV0_EPF1_STRAP6_TYPE,
                               NBIF2RCC_DEV0_EPF1_STRAP6_ADDRESS,
                               (UINT32) ~( STRAP_APER2_PREFETCHABLE_EN_DEV0_F1_MASK | STRAP_APER3_PREFETCHABLE_EN_DEV0_F1_MASK ),
                               (UINT32)((1 << STRAP_APER2_PREFETCHABLE_EN_DEV0_F1_OFFSET)|(1<<STRAP_APER3_PREFETCHABLE_EN_DEV0_F1_OFFSET)),
                               0
                                );				  
              //nbif0 enable bar sizing
              IDS_HDT_CONSOLE (GNB_TRACE, "enable bar sizing\n");
              NbioRegisterRMW (GnbHandle,
                               RCC_DEV0_EPF1_STRAP2_TYPE,
                               NBIF0RCC_DEV0_EPF1_STRAP2_ADDRESS,
                               (UINT32) ~( STRAP_RESIZE_BAR_EN_DEV0_F1_MASK ),
                               1 << STRAP_RESIZE_BAR_EN_DEV0_F1_OFFSET,
                               0
                                );	
              NbioRegisterRMW (GnbHandle,
                               RCC_DEV0_EPF1_STRAP2_TYPE,
                               NBIF2RCC_DEV0_EPF1_STRAP2_ADDRESS,
                               (UINT32) ~( STRAP_RESIZE_BAR_EN_DEV0_F1_MASK ),
                               1 << STRAP_RESIZE_BAR_EN_DEV0_F1_OFFSET,
                               0
                                );				  
              NbioRegisterRMW (GnbHandle,
                               RCC_DEV0_EPF1_STRAP10_TYPE,
                               NBIF0RCC_DEV0_EPF1_STRAP10_ADDRESS,
                               (UINT32) ~(STRAP_APER1_RESIZE_EN_DEV0_F1_MASK | STRAP_APER1_RESIZE_SUPPORT_DEV0_F1_MASK),
                               1 << STRAP_APER1_RESIZE_EN_DEV0_F1_OFFSET | 0xFFFFF << STRAP_APER1_RESIZE_SUPPORT_DEV0_F1_OFFSET,
                               0
                                );
              NbioRegisterRMW (GnbHandle,
                               RCC_DEV0_EPF1_STRAP11_TYPE,
                               NBIF0RCC_DEV0_EPF1_STRAP11_ADDRESS,
                               (UINT32) ~(STRAP_APER2_RESIZE_EN_DEV0_F1_MASK | STRAP_APER2_RESIZE_SUPPORT_DEV0_F1_MASK),
                               1 << STRAP_APER2_RESIZE_EN_DEV0_F1_OFFSET | 0xFFFFF << STRAP_APER2_RESIZE_SUPPORT_DEV0_F1_OFFSET,
                               0
                                );
              NbioRegisterRMW (GnbHandle,
                               RCC_DEV0_EPF1_STRAP12_TYPE,
                               NBIF0RCC_DEV0_EPF1_STRAP12_ADDRESS,
                               (UINT32) ~(STRAP_APER3_RESIZE_EN_DEV0_F1_MASK | STRAP_APER3_RESIZE_SUPPORT_DEV0_F1_MASK),
                               1 << STRAP_APER3_RESIZE_EN_DEV0_F1_OFFSET | 0xFFFFF << STRAP_APER3_RESIZE_SUPPORT_DEV0_F1_OFFSET,
                               0
                                );
              //nbif2 enable bar sizing
              NbioRegisterRMW (GnbHandle,
                               RCC_DEV0_EPF1_STRAP10_TYPE,
                               NBIF2RCC_DEV0_EPF1_STRAP10_ADDRESS,
                               (UINT32) ~(STRAP_APER1_RESIZE_EN_DEV0_F1_MASK | STRAP_APER1_RESIZE_SUPPORT_DEV0_F1_MASK),
                               1 << STRAP_APER1_RESIZE_EN_DEV0_F1_OFFSET | 0xFFFFF << STRAP_APER1_RESIZE_SUPPORT_DEV0_F1_OFFSET,
                               0
                                );
              NbioRegisterRMW (GnbHandle,
                               RCC_DEV0_EPF1_STRAP11_TYPE,
                               NBIF2RCC_DEV0_EPF1_STRAP11_ADDRESS,
                               (UINT32) ~(STRAP_APER2_RESIZE_EN_DEV0_F1_MASK | STRAP_APER2_RESIZE_SUPPORT_DEV0_F1_MASK),
                               1 << STRAP_APER2_RESIZE_EN_DEV0_F1_OFFSET | 0xFFFFF << STRAP_APER2_RESIZE_SUPPORT_DEV0_F1_OFFSET,
                               0
                                );
              NbioRegisterRMW (GnbHandle,
                               RCC_DEV0_EPF1_STRAP12_TYPE,
                               NBIF2RCC_DEV0_EPF1_STRAP12_ADDRESS,
                               (UINT32) ~(STRAP_APER3_RESIZE_EN_DEV0_F1_MASK | STRAP_APER3_RESIZE_SUPPORT_DEV0_F1_MASK),
                               1 << STRAP_APER3_RESIZE_EN_DEV0_F1_OFFSET | 0xFFFFF << STRAP_APER3_RESIZE_SUPPORT_DEV0_F1_OFFSET,
                               0
                                );
              break;

            case 2:           // NTB Secondary
              // Update the user config for this port
              NtbHotplugPortUserConfig (GnbHandle,
                                        PcieTopologyData,
                                        (GnbBuildOptionsZPDataHob->CfgNTBPcieCoreSel == 0 ? 0 : 1),
                                        GnbBuildOptionsZPDataHob->CfgNTBLinkSpeed
                                        );
              NbioRegisterRMW (GnbHandle,
                               NBIFMISC0_INTR_LINE_ENABLE_TYPE,
                               NBIFMISC0_INTR_LINE_ENABLE_ADDRESS,
                               (UINT32) ~(1 << 1),
                               (1 << 1),
                               0
                               );
              NbioRegisterRMW (GnbHandle,
                               NBIFMISC2_INTR_LINE_ENABLE_TYPE,
                               NBIFMISC2_INTR_LINE_ENABLE_ADDRESS,
                               (UINT32) ~(1 << 1),
                               (1 << 1),
                               0
                               );
              break;

            case 3:           // NTB Random
              ///@todo Notify DXIO/SMU of NTB location
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


/*----------------------------------------------------------------------------------------*/

UINT64
GetBarXSize(
 IN UINT16 NTBBarSizeIndex
)
{
  UINT32 BarSize;
  UINT64 PhyBarSize;
  switch(NTBBarSizeIndex){
    case 0x00:
		BarSize = 1;
		break;
	case 0x01:
		BarSize = 2;
		break;
	case 0x02:
		BarSize = 4;
		break;
	case 0x03:
		BarSize = 8;
		break;
	case 0x04:
		BarSize = 16;
		break;
	case 0x05:
		BarSize = 32;
		break;
	case 0x06:
		BarSize = 64;
		break;
    case 0x07:
		BarSize = 128;
		break;
	case 0x08:
		BarSize = 256;
		break;
	case 0x09:
		BarSize = 512;
		break;
	case 0x0a:
		BarSize = 1024;
		break;
	case 0xb:
		BarSize = 2048;
		break;
	case 0xc:
		BarSize = 4096;
	case 0xd:
		BarSize = 8192;
		break;
	case 0xe:
		BarSize = 16384;
		break;
	case 0xf:
		BarSize = 32768;
		break;
	case 0x10:
		BarSize = 65536;
		break;
	case 0x11:
		BarSize = 131072;
		break;
	case 0x12:
		BarSize = 262144;
		break;
	case 0x13:
		BarSize = 524288;
		break;
	default:
		BarSize = 0xff;
		break;
  }
  if(BarSize == 0xff)
  {
     return 0xff;
  }
  PhyBarSize = BarSize*1024*1024-1UL;
  return PhyBarSize;
}
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
  IN       DXIO_COMPLEX_DESCRIPTOR    *PcieTopologyData,
  IN       PCIe_PLATFORM_CONFIG       *Pcie
  )
{
  EFI_STATUS                        Status;
  GNB_HANDLE                        *GnbHandle;
  PEI_AMD_NBIO_BASE_SERVICES_PPI    *NbioBaseServicesPpi;
  GNB_BUILD_OPTIONS_ZP_DATA_HOB     *GnbBuildOptionsZPDataHob;
  PCIe_WRAPPER_CONFIG               *PcieWrapper;
  PCIe_ENGINE_CONFIG                *PcieEngine;
  BOOLEAN                           WrapperFound;
  BOOLEAN                           EngineFound;
  UINT32                            PcieCore;
  UINT32                            PhysicalBar1Size;
  UINT64                            PhysicalBar23Size;
  UINT64                            PhysicalBar45Size;
  UINT32                            PhysicalBar23SizeLo;
  UINT32                            PhysicalBar23SizeHi;
  UINT32                            PhysicalBar45SizeLo;
  UINT32                            PhysicalBar45SizeHi;

  IDS_HDT_CONSOLE (GNB_TRACE, "InitNTBAfterTraining Entry\n");
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gAmdNbioBaseServicesZpPpiGuid,
                             0,
                             NULL,
                             (VOID **)&NbioBaseServicesPpi
                             );
  if (Status == EFI_SUCCESS) {
    Status = NbioBaseServicesPpi->DebugOptions (NbioBaseServicesPpi, (GNB_BUILD_OPTIONS_ZP_DATA_HOB **)&GnbBuildOptionsZPDataHob);
    if (Status == EFI_SUCCESS) {
      if (GnbBuildOptionsZPDataHob->CfgNTBEnable == TRUE) {
        IDS_HDT_CONSOLE (GNB_TRACE, " CfgNTBEnable is TRUE\n");
        GnbHandle = NbioGetHandle (Pcie);
        while (GnbHandle != NULL) {
          if (GnbBuildOptionsZPDataHob->CfgNTBLocation == GnbHandle->InstanceId) {
            IDS_HDT_CONSOLE (GNB_TRACE, "  Found InstanceId = %d\n", GnbHandle->InstanceId);

            // Special case for NTB primary
            // Device is hotplug configured, but it needs to be hidden
            if (GnbBuildOptionsZPDataHob->CfgNTBMode == 1) {

              PcieCore = GnbBuildOptionsZPDataHob->CfgNTBPcieCoreSel == 0 ? 0 : 1;

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
                  if (PcieEngine->Type.Port.PortData.PortPresent == TRUE) {
                    NbioRegisterRMW (GnbHandle,
                                   TYPE_SMN,
                                   SMN_0x13B31004_ADDRESS + (PcieEngine->Type.Port.LogicalBridgeId << 10),
                                   (UINT32)~(SMN_0x13B31004_BridgeDis_MASK | SMN_0x13B31004_CfgDis_MASK | SMN_0x13B31004_CrsEnable_MASK),
                                   (UINT32) (SMN_0x13B31004_BridgeDis_MASK | SMN_0x13B31004_CfgDis_MASK | SMN_0x13B31004_CrsEnable_MASK),
                                   0
                                   );
                    EngineFound = TRUE;
                  }
                  PcieEngine = PcieConfigGetNextDescriptor (PcieEngine);
                }
              }
              //set nibf0 bar resource
              IDS_HDT_CONSOLE (GNB_TRACE, " Set bar resource bar1size Index:%x  bar23size Index:%x bar45size Index:%x\n",
                      GnbBuildOptionsZPDataHob->CfgNTBBAR1Size,GnbBuildOptionsZPDataHob->CfgNTBBAR23Size,GnbBuildOptionsZPDataHob->CfgNTBBAR45Size);
              //BAR1 SIZE
              NbioRegisterRMW (GnbHandle,
                                TYPE_SMN,
                                NBIF0D00F01PCIE_BAR1_CNTL_ADDRSS,
                                (UINT32)~(BAR_SIZE_MASK),
                                (UINT32) (GnbBuildOptionsZPDataHob->CfgNTBBAR1Size << BAR_SIZE_OFFSET),
                                0
                                );
              //BAR23 SIZE
              NbioRegisterRMW (GnbHandle,
                                TYPE_SMN,
                                NBIF0D00F01PCIE_BAR2_CNTL_ADDRSS,
                                (UINT32)~(BAR_SIZE_MASK),
                                (UINT32) (GnbBuildOptionsZPDataHob->CfgNTBBAR23Size << BAR_SIZE_OFFSET),
                                0
                                );
              //BAR45 SIZE
              NbioRegisterRMW (GnbHandle,
                                TYPE_SMN,
                                NBIF0D00F01PCIE_BAR3_CNTL_ADDRSS,
                                (UINT32)~(BAR_SIZE_MASK),
                                (UINT32) (GnbBuildOptionsZPDataHob->CfgNTBBAR45Size << BAR_SIZE_OFFSET),
                                0
                                );
              //set nbif2 bar resource
              //BAR1 SIZE
              NbioRegisterRMW (GnbHandle,
                                TYPE_SMN,
                                NBIF2D00F01PCIE_BAR1_CNTL_ADDRSS,
                                (UINT32)~(BAR_SIZE_MASK),
                                (UINT32) (GnbBuildOptionsZPDataHob->CfgNTBBAR1Size << BAR_SIZE_OFFSET),
                                0
                                );             
              //BAR23 SIZE
              NbioRegisterRMW (GnbHandle,
                                TYPE_SMN,
                                NBIF2D00F01PCIE_BAR2_CNTL_ADDRSS,
                                (UINT32)~(BAR_SIZE_MASK),
                                (UINT32) (GnbBuildOptionsZPDataHob->CfgNTBBAR23Size << BAR_SIZE_OFFSET),
                                0
                                );
              //BAR45 SIZE
              NbioRegisterRMW (GnbHandle,
                                TYPE_SMN,
                                NBIF2D00F01PCIE_BAR3_CNTL_ADDRSS,
                                (UINT32)~(BAR_SIZE_MASK),
                                (UINT32) (GnbBuildOptionsZPDataHob->CfgNTBBAR45Size << BAR_SIZE_OFFSET),
                                0
                                );

              PhysicalBar1Size = (UINT32)GetBarXSize(GnbBuildOptionsZPDataHob->CfgNTBBAR1Size);
			  PhysicalBar23Size = GetBarXSize(GnbBuildOptionsZPDataHob->CfgNTBBAR23Size);
			  PhysicalBar45Size = GetBarXSize(GnbBuildOptionsZPDataHob->CfgNTBBAR45Size);
			  PhysicalBar23SizeLo = PhysicalBar23Size & 0xffffffff;
			  PhysicalBar23SizeHi = (PhysicalBar23Size >> 32) & 0xffffffff;
			  PhysicalBar45SizeLo = PhysicalBar45Size & 0xffffffff;
			  PhysicalBar45SizeHi = (PhysicalBar45Size >> 32) & 0xffffffff;
			  IDS_HDT_CONSOLE (GNB_TRACE, " Physical BAR1 Size:%x\n Physical BAR23 Size:%lx | BAR23LO:%x | BAR23Hi:%x\n Physical BAR45 Size:%lx | BAR45LO:%x | BAR45HI:%x\n",
                      PhysicalBar1Size,PhysicalBar23Size,PhysicalBar23SizeLo,PhysicalBar23SizeHi,PhysicalBar45Size,PhysicalBar45SizeLo,PhysicalBar45SizeHi);
			  //BAR LMT SET
              NbioRegisterRMW (GnbHandle,
                                TYPE_SMN,
                                NTB_CTRL_ADDRESS,
                                (UINT32)~(PMM_REG_CTRL_MASK | SMM_REG_CTRL_MASK),
                                (UINT32) ((1 << PMM_REG_CTRL_OFFSET)|(1 << SMM_REG_CTRL_OFFSET)),
                                0
                                );
			  //Primary 
              NbioRegisterRMW (GnbHandle,
                                TYPE_SMN,
                                NTB_PBAR1LMT_ADDRESS,
                                (UINT32)~(NTB_PBAR1LMT_MASK),
                                (UINT32) (PhysicalBar1Size & 0xfffff000),
                                0
                                );
              NbioRegisterRMW (GnbHandle,
                                TYPE_SMN,
                                NTB_PBAR23LMT_LO_ADDRESS,
                                (UINT32)~(NTB_PBAR23LMT_LO_MASK),
                                (UINT32) (PhysicalBar23SizeLo & 0xfffff000),
                                0
                                );
              NbioRegisterRMW (GnbHandle,
                                TYPE_SMN,
                                NTB_PBAR23LMT_HI_ADDRESS,
                                (UINT32)~(NTB_PBAR23LMT_HI_MASK),
                                (UINT32) (PhysicalBar23SizeHi),
                                0
                                );			  
              NbioRegisterRMW (GnbHandle,
                                TYPE_SMN,
                                NTB_PBAR45LMT_LO_ADDRESS,
                                (UINT32)~(NTB_PBAR45LMT_LO_MASK),
                                (UINT32) (PhysicalBar45SizeLo  & 0xfffff000),
                                0
                                );
              NbioRegisterRMW (GnbHandle,
                                TYPE_SMN,
                                NTB_PBAR45LMT_HI_ADDRESS,
                                (UINT32)~(NTB_PBAR45LMT_HI_MASK),
                                (UINT32) (PhysicalBar45SizeHi),
                                0
                                );			  
			 //Secondary
              NbioRegisterRMW (GnbHandle,
                                TYPE_SMN,
                                NTB_SBAR1LMT_ADDRESS,
                                (UINT32)~(NTB_SBAR1LMT_MASK),
                                (UINT32) (PhysicalBar1Size & 0xfffff000),
                                0
                                );
              NbioRegisterRMW (GnbHandle,
                                TYPE_SMN,
                                NTB_SBAR23LMT_LO_ADDRESS,
                                (UINT32)~(NTB_SBAR23LMT_LO_MASK),
                                (UINT32) (PhysicalBar23SizeLo & 0xfffff000),
                                0
                                );
              NbioRegisterRMW (GnbHandle,
                                TYPE_SMN,
                                NTB_SBAR23LMT_HI_ADDRESS,
                                (UINT32)~(NTB_SBAR23LMT_HI_MASK),
                                (UINT32) (PhysicalBar23SizeHi),
                                0
                                );			  
              NbioRegisterRMW (GnbHandle,
                                TYPE_SMN,
                                NTB_SBAR45LMT_LO_ADDRESS,
                                (UINT32)~(NTB_SBAR45LMT_LO_MASK),
                                (UINT32) (PhysicalBar45SizeLo  & 0xfffff000),
                                0
                                );
              NbioRegisterRMW (GnbHandle,
                                TYPE_SMN,
                                NTB_SBAR45LMT_HI_ADDRESS,
                                (UINT32)~(NTB_SBAR45LMT_HI_MASK),
                                (UINT32) (PhysicalBar45SizeHi),
                                0
                                );				  
            }else if((GnbBuildOptionsZPDataHob->CfgNTBMode == 2)){
              PcieCore = GnbBuildOptionsZPDataHob->CfgNTBPcieCoreSel == 0 ? 0 : 1;

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
                  if (PcieEngine->Type.Port.PortData.PortPresent == TRUE) {
                    NbioRegisterRMW (GnbHandle,
                                   TYPE_SMN,
                                   SMN_0x13B31004_ADDRESS + (PcieEngine->Type.Port.LogicalBridgeId << 10),
                                   (UINT32)~(SMN_0x13B31004_BridgeDis_MASK | SMN_0x13B31004_CfgDis_MASK | SMN_0x13B31004_CrsEnable_MASK),
                                   (UINT32) 0,
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


