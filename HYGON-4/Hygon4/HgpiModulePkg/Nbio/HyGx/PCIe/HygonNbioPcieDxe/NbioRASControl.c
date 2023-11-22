/* $NoKeywords:$ */
/**
 * @file
 *
 * NbioRASControl - NBIO RAS Control
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
#include <GnbHsio.h>
#include <GnbRegisters.h>
#include <Library/PeiServicesLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/GnbLib.h>
#include <Library/GnbPciLib.h>
#include <Library/GnbPciAccLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <CcxRegistersDm.h>

#define FILECODE  NBIO_NBIOBASE_HYGX_HYGONNBIOBASEPEI_NBIORASCONTROL_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define UNCORRECTABLE_GROUP_TYPE  0
#define CORRECTABLE_GROUP_TYPE    1
#define ECC_UCP_GROUP_TYPE        3
#define DDP_UCP_GROUP_TYPE        4

// 1h - enable ECC correction and reporting
#define ECC_correction_and_reporting  0x01
// 5h - enable error reporting
#define Error_Reporting  0x05
// Ch - enable UCP reporting
#define UCP_Reporting  0x0C

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

typedef struct {
  IN      UINT32    GroupType;              ///< Parity Group ID
  IN      UINT32    GroupID;                ///< Group Type
  IN      UINT32    StructureIDMask;        ///< Structure ID
  IN      BOOLEAN   EmNbio0Supported;       ///< Emei Nbio 0 Supported
  IN      BOOLEAN   EmNbio2Supported;       ///< Emei Nbio 2 Supported
  IN      BOOLEAN   EmNbio1And3Supported;   ///< Emei Nbio 1/3 Supported
  IN      BOOLEAN   DjSupported;            ///< Du Jiang Supported
  IN      UINT32    ErrGenCmd;              ///< Error command
} NBIO_PARITY_TABLE;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
NBIO_PARITY_TABLE  NbioParityTable[] = {
  ///
  ///RAS Parity Group ID For UNCORR(DED)
  ///
    // PCIE0 TX ECC (Group 0, Struct 0~8)
    { UNCORRECTABLE_GROUP_TYPE,   0,  0x1FF, TRUE, TRUE, TRUE, FALSE, ECC_correction_and_reporting },
    // PCIE1 TX ECC (Group 1, Struct 0~6)
    { UNCORRECTABLE_GROUP_TYPE,   1,  0x7F, TRUE, TRUE, FALSE, FALSE, ECC_correction_and_reporting },
    // PCIE2 TX ECC (Group 2, Struct 0~8)
    { UNCORRECTABLE_GROUP_TYPE,   2,  0x1FF, TRUE, FALSE, FALSE, TRUE, ECC_correction_and_reporting },
    // IOMMU L1 PCIe 0 (Group 3, Struct 0~2)
    { UNCORRECTABLE_GROUP_TYPE,   3,  0x7, TRUE, TRUE, TRUE, FALSE, ECC_correction_and_reporting },
    // IOMMU L1 PCIe 1 (Group 4, Struct 0~2)
    { UNCORRECTABLE_GROUP_TYPE,   4,  0x7, TRUE, TRUE, FALSE, FALSE, ECC_correction_and_reporting },
    // IOMMU L1 nbif 0 (Group 5, Struct 0~2)
    { UNCORRECTABLE_GROUP_TYPE,   5,  0x7, TRUE, TRUE, FALSE, FALSE, ECC_correction_and_reporting },
    // IOMMU L1 ioagr (Group 6, Struct 0~2)
    { UNCORRECTABLE_GROUP_TYPE,   6,  0x7, TRUE, FALSE, FALSE, TRUE, ECC_correction_and_reporting },
    // IOHC (Group 7, Struct 0~3)
    { UNCORRECTABLE_GROUP_TYPE,   7,  0xF, TRUE, TRUE, TRUE, TRUE, ECC_correction_and_reporting },
    // nbif0 gdc (Group 8, Struct 0~6)
    { UNCORRECTABLE_GROUP_TYPE,   8,  0x7F, TRUE, TRUE, FALSE, FALSE, ECC_correction_and_reporting },
    // nbif2 gdc (Group 9, Struct 0~5)
    { UNCORRECTABLE_GROUP_TYPE,   9,  0x3F, TRUE, FALSE, FALSE, TRUE, ECC_correction_and_reporting },
    // nbif3 gdc (Group 10, Struct 0~7)
    { UNCORRECTABLE_GROUP_TYPE,   10,  0xFF, TRUE, FALSE, FALSE, TRUE, ECC_correction_and_reporting }, 
    // nbif0 bifc (Group 11, Struct 0~1)
    { UNCORRECTABLE_GROUP_TYPE,   11,  0x3, TRUE, TRUE, FALSE, FALSE, ECC_correction_and_reporting },
    // nbif2 bifc (Group 12, Struct 0~2)
    { UNCORRECTABLE_GROUP_TYPE,   12,  0x7, TRUE, FALSE, FALSE, TRUE, ECC_correction_and_reporting }, 
    // nbif3 bifc (Group 13, Struct 0~1)
    { UNCORRECTABLE_GROUP_TYPE,   13,  0x3, TRUE, FALSE, FALSE, TRUE, ECC_correction_and_reporting }, 
    // NTB bifc (Group 14, Struct 0~2)
    { UNCORRECTABLE_GROUP_TYPE,   14,  0x7, TRUE, TRUE, FALSE, FALSE, ECC_correction_and_reporting },  
    // PCIe0.RdTxClk (Group 15, Struct 0~21)
    { UNCORRECTABLE_GROUP_TYPE,   15,  0x3FFFFF, TRUE, TRUE, TRUE, FALSE, Error_Reporting }, 
    // PCIe0.RdLClk (Group 16, Struct 0~22, skip 20,21)
    { UNCORRECTABLE_GROUP_TYPE,   16,  0x4FFFFF, TRUE, TRUE, TRUE, FALSE, Error_Reporting }, 
    // PCIe1.RdTxClk (Group 17, Struct 0~1)
    { UNCORRECTABLE_GROUP_TYPE,   17,  0x3, TRUE, TRUE, FALSE, FALSE, Error_Reporting }, 
    // PCIe1.RdLClk (Group 18, Struct 0~22, skip 20,21)
    { UNCORRECTABLE_GROUP_TYPE,   18,  0x4FFFFF, TRUE, TRUE, FALSE, FALSE, Error_Reporting }, 
    // PCIe2.RdTxClk (Group 19, Struct 0~21)
    { UNCORRECTABLE_GROUP_TYPE,   19,  0x3FFFFF, TRUE, FALSE, FALSE, TRUE, Error_Reporting }, 
    // PCIe2.RdLClk (Group 20, Struct 0~22, skip 20,21)
    { UNCORRECTABLE_GROUP_TYPE,   20,  0x4FFFFF, TRUE, FALSE, FALSE, TRUE, Error_Reporting },   
  ///
  ///RAS Parity Group ID For CORR(SEC)
  ///
    // PCIE0 TX ECC (Group 0, Struct 0~8)
    { CORRECTABLE_GROUP_TYPE,   0,  0x1FF, TRUE, TRUE, TRUE, FALSE, ECC_correction_and_reporting },
    // PCIE1 TX ECC (Group 1, Struct 0~6)
    { CORRECTABLE_GROUP_TYPE,   1,  0x7F, TRUE, TRUE, FALSE, FALSE, ECC_correction_and_reporting },
    // PCIE2 TX ECC (Group 2, Struct 0~8)
    { CORRECTABLE_GROUP_TYPE,   2,  0x1FF, TRUE, FALSE, FALSE, TRUE, ECC_correction_and_reporting },
    // IOMMU L1 PCIe 0 (Group 3, Struct 0~2)
    { CORRECTABLE_GROUP_TYPE,   3,  0x7, TRUE, TRUE, TRUE, FALSE, ECC_correction_and_reporting },
    // IOMMU L1 PCIe 1 (Group 4, Struct 0~2)
    { CORRECTABLE_GROUP_TYPE,   4,  0x7, TRUE, TRUE, FALSE, FALSE, ECC_correction_and_reporting },
    // IOMMU L1 nbif 0 (Group 5, Struct 0~2)
    { CORRECTABLE_GROUP_TYPE,   5,  0x7, TRUE, TRUE, FALSE, FALSE, ECC_correction_and_reporting },
    // IOMMU L1 ioagr (Group 6, Struct 0~2)
    { CORRECTABLE_GROUP_TYPE,   6,  0x7, TRUE, FALSE, FALSE, TRUE, ECC_correction_and_reporting },
    // IOHC (Group 7, Struct 0~3)
    { CORRECTABLE_GROUP_TYPE,   7,  0xF, TRUE, TRUE, TRUE, TRUE, ECC_correction_and_reporting },
    // nbif0 gdc (Group 8, Struct 0~6)
    { CORRECTABLE_GROUP_TYPE,   8,  0x7F, TRUE, TRUE, FALSE, FALSE, ECC_correction_and_reporting },
    // nbif2 gdc (Group 9, Struct 0~5)
    { CORRECTABLE_GROUP_TYPE,   9,  0x3F, TRUE, FALSE, FALSE, TRUE, ECC_correction_and_reporting },
    // nbif3 gdc (Group 10, Struct 0~7)
    { CORRECTABLE_GROUP_TYPE,   10,  0xFF, TRUE, FALSE, FALSE, TRUE, ECC_correction_and_reporting }, 
    // nbif0 bifc (Group 11, Struct 0~1)
    { CORRECTABLE_GROUP_TYPE,   11,  0x3, TRUE, TRUE, FALSE, FALSE, ECC_correction_and_reporting },
    // nbif2 bifc (Group 12, Struct 0~2)
    { CORRECTABLE_GROUP_TYPE,   12,  0x7, TRUE, FALSE, FALSE, TRUE, ECC_correction_and_reporting }, 
    // nbif3 bifc (Group 13, Struct 0~1)
    { CORRECTABLE_GROUP_TYPE,   13,  0x3, TRUE, FALSE, FALSE, TRUE, ECC_correction_and_reporting }, 
    // NTB bifc (Group 14, Struct 0~2)
    { CORRECTABLE_GROUP_TYPE,   14,  0x7, TRUE, TRUE, FALSE, FALSE, ECC_correction_and_reporting },  
    // Iommu_L1_PCIE0 (Group 15, Struct 0~1)
    { CORRECTABLE_GROUP_TYPE,   15,  0x3, TRUE, TRUE, TRUE, FALSE, Error_Reporting }, 
    // Iommu_L1_PCIE1 (Group 16, Struct 0~1)
    { CORRECTABLE_GROUP_TYPE,   16,  0x3, TRUE, TRUE, FALSE, FALSE, Error_Reporting }, 
    // Iommu_L1_nbif0 (Group 17, Struct 0~1)
    { CORRECTABLE_GROUP_TYPE,   17,  0x3, TRUE, TRUE, FALSE, FALSE, Error_Reporting }, 
    // Iommu_L1_ioagr (Group 18, Struct 0~1)
    { CORRECTABLE_GROUP_TYPE,   18,  0x3, TRUE, FALSE, FALSE, TRUE, Error_Reporting }, 
    // Iommu_L1_l2a (Group 19, Struct 0~19)
    { CORRECTABLE_GROUP_TYPE,   19,  0xFFFFF, TRUE, TRUE, TRUE, TRUE, Error_Reporting }, 
    // Iommu_L1_l2b (Group 20, Struct 0~7)
    { CORRECTABLE_GROUP_TYPE,   20,  0xFF, TRUE, TRUE, TRUE, TRUE, Error_Reporting }, 
  ///
  ///RAS Parity Group ID For UCP
  ///
    // PCIE0 TX ECC (Group 0, Struct 0~8)
    { ECC_UCP_GROUP_TYPE,   0,  0x1FF, TRUE, TRUE, TRUE, FALSE, UCP_Reporting },
    // PCIE1 TX ECC (Group 1, Struct 0~6)
    { ECC_UCP_GROUP_TYPE,   1,  0x7F, TRUE, TRUE, FALSE, FALSE, UCP_Reporting },
    // PCIE2 TX ECC (Group 2, Struct 0~8)
    { ECC_UCP_GROUP_TYPE,   2,  0x1FF, TRUE, FALSE, FALSE, TRUE, UCP_Reporting },
    // IOMMU L1 PCIe 0 (Group 3, Struct 0~2)
    { ECC_UCP_GROUP_TYPE,   3,  0x7, TRUE, TRUE, TRUE, FALSE, UCP_Reporting },
    // IOMMU L1 PCIe 1 (Group 4, Struct 0~2)
    { ECC_UCP_GROUP_TYPE,   4,  0x7, TRUE, TRUE, FALSE, FALSE, UCP_Reporting },
    // IOMMU L1 nbif 0 (Group 5, Struct 0~2)
    { ECC_UCP_GROUP_TYPE,   5,  0x7, TRUE, TRUE, FALSE, FALSE, UCP_Reporting },
    // IOMMU L1 ioagr (Group 6, Struct 0~2)
    { ECC_UCP_GROUP_TYPE,   6,  0x7, TRUE, FALSE, FALSE, TRUE, UCP_Reporting },
    // IOHC (Group 7, Struct 0~3)
    { ECC_UCP_GROUP_TYPE,   7,  0xF, TRUE, TRUE, TRUE, TRUE, UCP_Reporting },
    // nbif0 gdc (Group 8, Struct 0~6)
    { ECC_UCP_GROUP_TYPE,   8,  0x7F, TRUE, TRUE, FALSE, FALSE, UCP_Reporting },
    // nbif2 gdc (Group 9, Struct 0~5)
    { ECC_UCP_GROUP_TYPE,   9,  0x3F, TRUE, FALSE, FALSE, TRUE, UCP_Reporting },
    // nbif3 gdc (Group 10, Struct 0~7)
    { ECC_UCP_GROUP_TYPE,   10,  0xFF, TRUE, FALSE, FALSE, TRUE, UCP_Reporting }, 
    // nbif0 bifc (Group 11, Struct 0~1)
    { ECC_UCP_GROUP_TYPE,   11,  0x3, TRUE, TRUE, FALSE, FALSE, UCP_Reporting },
    // nbif2 bifc (Group 12, Struct 0~2)
    { ECC_UCP_GROUP_TYPE,   12,  0x7, TRUE, FALSE, FALSE, TRUE, UCP_Reporting }, 
    // nbif3 bifc (Group 13, Struct 0~1)
    { ECC_UCP_GROUP_TYPE,   13,  0x3, TRUE, FALSE, FALSE, TRUE, UCP_Reporting }, 
    // NTB bifc (Group 14, Struct 0~2)
    { ECC_UCP_GROUP_TYPE,   14,  0x7, TRUE, TRUE, FALSE, FALSE, UCP_Reporting }
};

/*----------------------------------------------------------------------------------------
 *                    P P I   N O T I F Y   D E S C R I P T O R S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/

/**
 * Identify active PCIe core for NBIO RAS Control
 * 
 * @param[in]  GnbHandle      GNB Handle
 * @param[in]  PcieCore       Pcie core number
 */
VOID
IsActivedPcieCore (
  IN      GNB_HANDLE              *GnbHandle,
  IN      UINT8                   *PcieCore
  )
{
  UINT32                       SmnAddress;
  UINT32                       Value;
  PCIe_ENGINE_CONFIG           *PcieEngine;
  PCIe_WRAPPER_CONFIG          *PcieWrapper;

  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      //get iohc_bridge_cntl Setting
      SmnAddress = GetIohcBridgeCntlAddress (GnbHandle, PcieEngine);
      NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &Value, 0);   
      if ((Value & IOHC_BRIDGE_CNTL_BridgeDis_MASK) == 0) {
        //Pcie core has actived port
        *PcieCore |= (BIT0 << PcieWrapper->WrapId);
        break;
      }
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine); 
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Enable NBIO RAS Control
 *
 *
 *
 * @param[in]  GnbHandle      GNB Handle
 */
VOID
NbioRASControl (
  IN GNB_HANDLE                 *GnbHandle
  )
{
  UINT32                i, j;
  UINT32                Count;
  UINT32                Value;
  NBRASCFG_0004_STRUCT  NBRASCFG_0004;
  PCI_ADDR              IommuPciAddress;
  UINT8                 ActivePcieCore;
  UINT32                GroupID;

  if (!GnbHandle->IohubPresent) {
    return;
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioRASControl Enter\n");
  IDS_HDT_CONSOLE (GNB_TRACE, "DieType:%02x PhysicalDieId:%02x RbId:%02x \n", GnbHandle->DieType, GnbHandle->PhysicalDieId, GnbHandle->RbId);
  // IOHCRASx00000000 (PARITY_CONTROL_0): 0x0001_xxxx
  NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, PARITY_CONTROL_0_ADDRESS_HYGX), 0xFFFF, 0x00010000, 0);

  // IOHCRASx00000008 (PARITY_SEVERITY_CONTROL_UNCORR_0): 0xAAAA_AAAA
  Value = 0xAAAAAAAA;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, PARITY_SEVERITY_CONTROL_UNCORR_0_ADDRESS_HYGX), &Value, 0);

  // IOHCRASx0000000C (PARITY_SEVERITY_CONTROL_UNCORR_1): 0x0000_002A
  Value = 0x000002AA;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, PARITY_SEVERITY_CONTROL_UNCORR_1_ADDRESS_HYGX), &Value, 0);

  // IOHCRASx00000010 (PARITY_SEVERITY_CONTROL_CORR_0): 0x0000_0000
  // IOHCRASx00000014 (PARITY_SEVERITY_CONTROL_CORR_1): 0x0000_0000
  // IOHCRASx00000018 (PARITY_SEVERITY_CONTROL_UCP_0): 0x0000_0000
  // IOHCRASx00000158 (MISC_SEVERITY_CONTROL): 0x0000_0000
  Value = 0;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, PARITY_SEVERITY_CONTROL_CORR_0_ADDRESS_HYGX), &Value, 0);
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, PARITY_SEVERITY_CONTROL_CORR_1_ADDRESS_HYGX), &Value, 0);
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, PARITY_SEVERITY_CONTROL_UCP_0_ADDRESS_HYGX), &Value, 0);
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, MISC_SEVERITY_CONTROL_ADDRESS_HYGX), &Value, 0);

  Value = PcdGet32 (PcdEgressPoisonSeverityLo);
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, EGRESS_POISON_SEVERITY_LO_ADDRESS_HYGX), &Value, 0);

  Value = PcdGet32 (PcdEgressPoisonSeverityHi);
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, EGRESS_POISON_SEVERITY_HI_ADDRESS_HYGX), &Value, 0);

  // IOHCRASx00000168 (ErrEvent_ACTION_CONTROL): 0x0000_0018
  // IOHCRASx0000016C (ParitySerr_ACTION_CONTROL): 0x0000_001F
  // IOHCRASx00000170 (ParityFatal_ACTION_CONTROL): 0x0000_001F
  // IOHCRASx00000174 (ParityNonFatal_ACTION_CONTROL): 0x0000_0006
  // IOHCRASx00000178 (ParityCorr_ACTION_CONTROL): 0x0000_0006
  NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, ErrEvent_ACTION_CONTROL_ADDRESS_HYGX), 0xFFFFFFFF, BIT0, 0);

  Value = 0x0000001F;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, ParitySerr_ACTION_CONTROL_ADDRESS_HYGX), &Value, 0);

  Value = 0x0000001F;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, ParityFatal_ACTION_CONTROL_ADDRESS_HYGX), &Value, 0);

  Value = 0x00000006;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, ParityNonFatal_ACTION_CONTROL_ADDRESS_HYGX), &Value, 0);

  Value = 0x00000006;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, ParityCorr_ACTION_CONTROL_ADDRESS_HYGX), &Value, 0);

  // IOMMUL1:
  if (GnbHandle->DieType == IOD_EMEI) {
    if (GnbHandle->RbId == 0 || GnbHandle->RbId == 2) {
      //Emei IOD NBIO0/2 have PCIe0/PCIe1/NBIF0/IOAGR IOMMU L1 Instance

      // L1_CNTRL_1: 10 L1CacheParityEn.
      NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, L1_CNTRL_1_PCIE0_ADDRESS_HYGX), 0xFFFFFFFF, BIT10, 0);

      NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, L1_CNTRL_1_PCIE1_ADDRESS_HYGX), 0xFFFFFFFF, BIT10, 0);

      NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, L1_CNTRL_1_NBIF0_ADDRESS_HYGX), 0xFFFFFFFF, BIT10, 0);

      NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, L1_CNTRL_1_IOAGR_ADDRESS_HYGX), 0xFFFFFFFF, BIT10, 0);

      // L1_CNTRL_2: 3 L1ATSDataErrorSignalEn.
      NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, L1_CNTRL_2_PCIE0_ADDRESS_HYGX), 0xFFFFFFFF, BIT3, 0);

      NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, L1_CNTRL_2_PCIE1_ADDRESS_HYGX), 0xFFFFFFFF, BIT3, 0);

      NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, L1_CNTRL_2_NBIF0_ADDRESS_HYGX), 0xFFFFFFFF, BIT3, 0);

      NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, L1_CNTRL_2_IOAGR_ADDRESS_HYGX), 0xFFFFFFFF, BIT3, 0);

      // Enable FSTus error report
      if (GnbHandle->RbId == 0) {
        NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, FST_LINK_CONTROL_HYGX), 0x87FFFFFF, BIT27+BIT28+BIT30, 0);
      }
    } else {
      //Emei IOD NBIO1/3 have PCIe0 IOMMU L1 Instance
      // L1_CNTRL_1: 10 L1CacheParityEn.
      NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, L1_CNTRL_1_PCIE0_ADDRESS_HYGX), 0xFFFFFFFF, BIT10, 0);

      // L1_CNTRL_2: 3 L1ATSDataErrorSignalEn.
      NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, L1_CNTRL_2_PCIE0_ADDRESS_HYGX), 0xFFFFFFFF, BIT3, 0);   
    }
  } else {
    //Du Jiang IOD have IOAGR IOMMU L1 Instance
    // L1_CNTRL_1: 10 L1CacheParityEn.
    NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, L1_CNTRL_1_IOAGR_ADDRESS_HYGX), 0xFFFFFFFF, BIT10, 0);

    // L1_CNTRL_2: 3 L1ATSDataErrorSignalEn.
    NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, L1_CNTRL_2_IOAGR_ADDRESS_HYGX), 0xFFFFFFFF, BIT10, 0);
    
    if (IsEmeiPresent()) {
      // Enable FSTds error report
      NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, FST_LINK_CONTROL_HYGX), 0x87FFFFFF, BIT27+BIT28+BIT30, 0);
    }
  }

  // IOMMUL2:
  // IOMMU_COMMAND: 6 PARITY_ERROR_EN.
  IommuPciAddress = NbioGetHostPciAddress (GnbHandle);
  IommuPciAddress.Address.Function = 0x2;
  GnbLibPciRead (IommuPciAddress.AddressValue | 0x4, AccessWidth32, &Value, NULL);
  Value |= BIT6;
  GnbLibPciWrite (IommuPciAddress.AddressValue | 0x4, AccessS3SaveWidth32, &Value, NULL);

  // L2_DTC_CONTROL: 4 DTCParityEn.
  NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, L2_DTC_CONTROL_ADDRESS_HYGX), 0xFFFFFFFF, BIT4, 0);

  // L2_ITC_CONTROL: 4 ITCParityEn.
  NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, L2_ITC_CONTROL_ADDRESS_HYGX), 0xFFFFFFFF, BIT4, 0);

  // L2_PTC_A_CONTROL: 4 PTCAParityEn.
  NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, L2_PTC_A_CONTROL_ADDRESS_HYGX), 0xFFFFFFFF, BIT4, 0);

  // L2_PDC_CONTROL: 4 PDCParityEn.
  NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, L2_PDC_CONTROL_ADDRESS_HYGX), 0xFFFFFFFF, BIT4, 0);

  // L2B_SDP_PARITY_ERROR_EN: 2 TWW_PARITY_ERROR_EN.
  // L2B_SDP_PARITY_ERROR_EN: 1 CP_PARITY_ERROR_EN.
  NbioRegisterRMW (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, L2B_SDP_PARITY_ERROR_EN_ADDRESS_HYGX), 0xFFFFFFFF, (BIT1 | BIT2), 0);

  // execute the programming sequence given in PPR section 9.1.7.1 NBIO Internal Error Reporting Configuration.
  for ( i = 0; i < sizeof (NbioParityTable) / sizeof (NBIO_PARITY_TABLE); i++) {

    if (GnbHandle->DieType == IOD_EMEI) {
      if (((GnbHandle->RbId == 0) && (!NbioParityTable[i].EmNbio0Supported)) ||
          ((GnbHandle->RbId == 2) && (!NbioParityTable[i].EmNbio2Supported)) ||
          (((GnbHandle->RbId == 1) || (GnbHandle->RbId == 3)) && (!NbioParityTable[i].EmNbio1And3Supported)) ||
          ((GnbHandle->PhysicalDieId != 1) && (NbioParityTable[i].EmNbio0Supported) && (!NbioParityTable[i].EmNbio2Supported) && (!NbioParityTable[i].EmNbio1And3Supported) && (NbioParityTable[i].DjSupported)) 
          ) {
        continue;   
      }
    } else {
      if (!NbioParityTable[i].DjSupported) {
        continue;
      }
    }

    IsActivedPcieCore (GnbHandle, &ActivePcieCore);
    GroupID = NbioParityTable[i].GroupID;

    if (NbioParityTable[i].GroupType == UNCORRECTABLE_GROUP_TYPE) {
      if (
          (((ActivePcieCore & BIT0) == 0) && ((GroupID == 0) || (GroupID == 3) || (GroupID == 15) || (GroupID == 16))) ||
          (((ActivePcieCore & BIT1) == 0) && ((GroupID == 1) || (GroupID == 4) || (GroupID == 17) || (GroupID == 18))) ||
          (((ActivePcieCore & BIT2) == 0) && ((GroupID == 2) || (GroupID == 19) || (GroupID == 20)))
        ) {
        continue;
      }
    } else if (NbioParityTable[i].GroupType == CORRECTABLE_GROUP_TYPE) {
      if (
          (((ActivePcieCore & BIT0) == 0) && ((GroupID == 0) || (GroupID == 3) || (GroupID == 15))) ||
          (((ActivePcieCore & BIT1) == 0) && ((GroupID == 1) || (GroupID == 4) || (GroupID == 16))) ||
          (((ActivePcieCore & BIT2) == 0) && ((GroupID == 2)))
        ) {
        continue;
      }
    } else {
      if (
          (((ActivePcieCore & BIT0) == 0) && ((GroupID == 0) || (GroupID == 3))) ||
          (((ActivePcieCore & BIT1) == 0) && ((GroupID == 1) || (GroupID == 4))) ||
          (((ActivePcieCore & BIT2) == 0) && ((GroupID == 2)))
        ) {
        continue;
      }
    }
    
    Count = sizeof (NbioParityTable[i].StructureIDMask) * 8;
    for (j = 0; j < Count; j++) {
      if (((NbioParityTable[i].StructureIDMask >> j) & BIT0) == BIT0) {
        IDS_HDT_CONSOLE (
          GNB_TRACE,
          "Init group %02d - %02d - %02d - %02d begin \n",
          NbioParityTable[i].GroupType,
          NbioParityTable[i].GroupID,
          j,
          NbioParityTable[i].ErrGenCmd
          );

        IDS_HDT_CONSOLE (GNB_TRACE, "Wait for NBRASCFG_0004.Field.ParityErrGenInjectAllow to 1 ...\n");
        NBRASCFG_0004.Value = 0x0;
        NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, PARITY_CONTROL_1_ADDRESS_HYGX), &NBRASCFG_0004.Value, 0);
        do {
          NbioRegisterRead (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, PARITY_CONTROL_1_ADDRESS_HYGX), &NBRASCFG_0004.Value, 0);
        } while (NBRASCFG_0004.Field.ParityErrGenInjectAllow != 0x1);

        NBRASCFG_0004.Field.ParityErrGenGroupSel = NbioParityTable[i].GroupID;
        NBRASCFG_0004.Field.ParityErrGenIdSel    = j;
        NBRASCFG_0004.Field.ParityErrGenCmd      = NbioParityTable[i].ErrGenCmd;
        NBRASCFG_0004.Field.ParityErrGenTrigger  = 1;

        switch (NbioParityTable[i].GroupType) {
          case UNCORRECTABLE_GROUP_TYPE:
            NBRASCFG_0004.Field.ParityErrGenGroupTypeSel = 0;
            break;
          case CORRECTABLE_GROUP_TYPE:
          case ECC_UCP_GROUP_TYPE:
          case DDP_UCP_GROUP_TYPE:
            NBRASCFG_0004.Field.ParityErrGenGroupTypeSel = 1;
            break;
        }

        NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE (GnbHandle, PARITY_CONTROL_1_ADDRESS_HYGX), &NBRASCFG_0004.Value, 0);
      }
    }

    IDS_HDT_CONSOLE (GNB_TRACE, "Init group end \n");
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioRASControl Exit\n");
}
