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
#include <CcxRegistersDm.h>

#define FILECODE        NBIO_NBIOBASE_HYEX_HYGONNBIOBASEPEI_NBIORASCONTROL_FILECODE

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
#define ECC_correction_and_reporting                0x01
// 5h - enable error reporting
#define Error_Reporting                             0x05
// Ch - enable UCP reporting
#define UCP_Reporting                               0x0C

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

typedef struct {
  IN      UINT32        GroupType;       ///< Parity Group ID
  IN      UINT32        GroupID;         ///< Group Type
  IN      UINT32        StructureID;     ///< Structure ID
  IN      UINT32        ErrGenCmd;       ///< Error command
} NBIO_PARITY_TABLE;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

NBIO_PARITY_TABLE NbioParityTableST[] = {
    // NBIO Correctable Parity
    { CORRECTABLE_GROUP_TYPE,12, 0,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,12, 1,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,13, 0,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,13, 1,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,14, 0,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,14, 1,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,15, 0,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,15, 1,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16, 0,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16, 1,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16, 2,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16, 3,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16, 4,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16, 5,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16, 6,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16, 7,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16, 8,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16, 9,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16,10,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16,11,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16,12,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16,13,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16,14,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16,15,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16,16,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16,17,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16,18,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,16,19,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,17, 0,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,17, 1,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,17, 2,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,17, 3,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,17, 4,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,17, 5,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,17, 6,Error_Reporting },
    { CORRECTABLE_GROUP_TYPE,17, 7,Error_Reporting },
    // NBIO Uncorrectable Parity
    //PCIE core 0
    { UNCORRECTABLE_GROUP_TYPE,12, 0,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,12, 1,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,12, 2,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,13, 0,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,13, 1,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,13, 2,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,13, 3,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,13, 4,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,13, 5,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,13, 6,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,13, 7,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,13, 8,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,13, 9,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,13,13,Error_Reporting },
    //PCIE core 1
    { UNCORRECTABLE_GROUP_TYPE,14, 0,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,14, 1,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,14, 2,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,15, 0,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,15, 1,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,15, 2,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,15, 3,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,15, 4,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,15, 5,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,15, 6,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,15, 7,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,15, 8,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,15, 9,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,15,13,Error_Reporting },
    //PCIE core 2
    { UNCORRECTABLE_GROUP_TYPE,17, 0,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,17, 1,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,17, 2,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,18, 0,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,18, 1,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,18, 2,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,18, 3,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,18, 4,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,18, 5,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,18, 6,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,18, 7,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,18, 8,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,18, 9,Error_Reporting },
    { UNCORRECTABLE_GROUP_TYPE,18,13,Error_Reporting },
    // NBIO ECC
    { CORRECTABLE_GROUP_TYPE,0,0,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,0,1,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,0,2,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,0,3,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,0,4,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,0,5,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,1,0,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,1,1,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,1,2,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,1,3,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,1,4,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,1,5,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,2,0,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,3,0,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,4,0,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,5,0,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,6,0,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,6,1,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,7,0,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,7,1,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,8,1,ECC_correction_and_reporting },
    //PCie core 2 TxEcc
    { CORRECTABLE_GROUP_TYPE,18,0,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,18,1,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,18,2,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,18,3,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,18,4,ECC_correction_and_reporting },
    { CORRECTABLE_GROUP_TYPE,18,5,ECC_correction_and_reporting },
    // NBIO ECC+UCP
    { ECC_UCP_GROUP_TYPE,2,1,ECC_correction_and_reporting },
    { ECC_UCP_GROUP_TYPE,3,1,ECC_correction_and_reporting },
    { ECC_UCP_GROUP_TYPE,4,1,ECC_correction_and_reporting },
    { ECC_UCP_GROUP_TYPE,5,1,ECC_correction_and_reporting },
    // NBIO DDP+UCP
    { DDP_UCP_GROUP_TYPE, 2,2,UCP_Reporting },
    { DDP_UCP_GROUP_TYPE, 3,2,UCP_Reporting },
    { DDP_UCP_GROUP_TYPE, 4,2,UCP_Reporting },
    { DDP_UCP_GROUP_TYPE, 5,2,UCP_Reporting },
    { DDP_UCP_GROUP_TYPE, 6,2,UCP_Reporting },
    { DDP_UCP_GROUP_TYPE, 7,2,UCP_Reporting },
    { DDP_UCP_GROUP_TYPE, 7,3,UCP_Reporting },
    { DDP_UCP_GROUP_TYPE, 7,4,UCP_Reporting },
    { DDP_UCP_GROUP_TYPE, 8,2,UCP_Reporting },
    { DDP_UCP_GROUP_TYPE, 8,3,UCP_Reporting },
    { DDP_UCP_GROUP_TYPE, 8,4,UCP_Reporting },
    { DDP_UCP_GROUP_TYPE, 9,0,UCP_Reporting },
    { DDP_UCP_GROUP_TYPE, 9,1,UCP_Reporting },  //NBIF0 SMN RAS
    { DDP_UCP_GROUP_TYPE,10,0,UCP_Reporting },
    { DDP_UCP_GROUP_TYPE,10,1,UCP_Reporting },  //NBIF1 SMN RAS
    { DDP_UCP_GROUP_TYPE,11,0,UCP_Reporting },
    { DDP_UCP_GROUP_TYPE,11,1,UCP_Reporting },
    { DDP_UCP_GROUP_TYPE,11,2,UCP_Reporting },
};

/*----------------------------------------------------------------------------------------
 *                    P P I   N O T I F Y   D E S C R I P T O R S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Identify active PCIe core for NBIO RAS Control
 *
 *
 *
 * @param[in]  GnbHandle      GNB Handle
 * @param[in]  PcieCore       Pcie core number
 */

VOID
NbioRASIdentifyPcieCore (
    IN      GNB_HANDLE              *GnbHandle,
    IN      UINT8                   *PcieCore
  )
{
  PCIe_ENGINE_CONFIG             *PcieEngine;
  PCIe_WRAPPER_CONFIG            *PcieWrapper;

  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if (PcieLibIsEngineAllocated (PcieEngine) &&
          PcieLibIsPcieEngine(PcieEngine))
      {
        *PcieCore = (UINT8) (1 << PcieWrapper->WrapId);
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
  UINTN                     i;
  UINT32                    Value;
  NBRASCFG_0004_STRUCT      NBRASCFG_0004;
  PCI_ADDR                  IommuPciAddress;
  UINT8                     PcieCore;

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioRASControl Enter\n");

  // IOHCRASx00000000 (PARITY_CONTROL_0): 0x0001_xxxx
  NbioRegisterRead ( GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, PARITY_CONTROL_0_ADDRESS_HYEX), &Value, 0);
  Value = (Value & 0xFFFF) | 0x00010000;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, PARITY_CONTROL_0_ADDRESS_HYEX), &Value, 0);
  // IOHCRASx00000008 (PARITY_SEVERITY_CONTROL_UNCORR_0): 0xAAAA_AAAA
  Value = 0xAAAAAAAA;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, PARITY_SEVERITY_CONTROL_UNCORR_0_ADDRESS_HYEX), &Value, 0);
  // IOHCRASx0000000C (PARITY_SEVERITY_CONTROL_UNCORR_1): 0x0000_002A
  Value = 0x0000002A;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, PARITY_SEVERITY_CONTROL_UNCORR_1_ADDRESS_HYEX), &Value, 0);
  // IOHCRASx00000010 (PARITY_SEVERITY_CONTROL_CORR_0): 0x0000_0000
  // IOHCRASx00000014 (PARITY_SEVERITY_CONTROL_CORR_1): 0x0000_0000
  // IOHCRASx00000018 (PARITY_SEVERITY_CONTROL_UCP_0): 0x0000_0000
  // IOHCRASx00000158 (MISC_SEVERITY_CONTROL): 0x0000_0000
  Value = 0;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, PARITY_SEVERITY_CONTROL_CORR_0_ADDRESS_HYEX), &Value, 0);
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, PARITY_SEVERITY_CONTROL_CORR_1_ADDRESS_HYEX), &Value, 0);
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, PARITY_SEVERITY_CONTROL_UCP_0_ADDRESS_HYEX), &Value, 0);
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, MISC_SEVERITY_CONTROL_ADDRESS_HYEX), &Value, 0);

  Value = PcdGet32( PcdEgressPoisonSeverityLo);
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, EGRESS_POISON_SEVERITY_LO_ADDRESS_HYEX), &Value, 0);
  Value = PcdGet32( PcdEgressPoisonSeverityHi);
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, EGRESS_POISON_SEVERITY_HI_ADDRESS_HYEX), &Value, 0);

  // IOHCRASx00000168 (ErrEvent_ACTION_CONTROL): 0x0000_0018
  // IOHCRASx0000016C (ParitySerr_ACTION_CONTROL): 0x0000_001F
  // IOHCRASx00000170 (ParityFatal_ACTION_CONTROL): 0x0000_001F
  // IOHCRASx00000174 (ParityNonFatal_ACTION_CONTROL): 0x0000_0006
  // IOHCRASx00000178 (ParityCorr_ACTION_CONTROL): 0x0000_0006
  NbioRegisterRead ( GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, ErrEvent_ACTION_CONTROL_ADDRESS_HYEX), &Value, 0);
  Value |= BIT0;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, ErrEvent_ACTION_CONTROL_ADDRESS_HYEX), &Value, 0);
  Value = 0x0000001F;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, ParitySerr_ACTION_CONTROL_ADDRESS_HYEX), &Value, 0);
  Value = 0x0000001F;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, ParityFatal_ACTION_CONTROL_ADDRESS_HYEX), &Value, 0);
  Value = 0x00000006;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, ParityNonFatal_ACTION_CONTROL_ADDRESS_HYEX), &Value, 0);
  Value = 0x00000006;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, ParityCorr_ACTION_CONTROL_ADDRESS_HYEX), &Value, 0);

  // IOMMUL1:
  // L1_CNTRL_1: 10 L1CacheParityEn.
  NbioRegisterRead ( GnbHandle, TYPE_D0F0xBC, L1IMU_SPACE_HYEX(GnbHandle, L1_CNTRL_1_PCIE0_ADDRESS_HYEX), &Value, 0);
  Value |= BIT10;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, L1IMU_SPACE_HYEX(GnbHandle, L1_CNTRL_1_PCIE0_ADDRESS_HYEX), &Value, 0);
  NbioRegisterRead ( GnbHandle, TYPE_D0F0xBC, L1IMU_SPACE_HYEX(GnbHandle, L1_CNTRL_1_PCIE1_ADDRESS_HYEX), &Value, 0);
  Value |= BIT10;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, L1IMU_SPACE_HYEX(GnbHandle, L1_CNTRL_1_PCIE1_ADDRESS_HYEX), &Value, 0);
  NbioRegisterRead ( GnbHandle, TYPE_D0F0xBC, L1IMU_SPACE_HYEX(GnbHandle, L1_CNTRL_1_NBIF0_ADDRESS_HYEX), &Value, 0);
  Value |= BIT10;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, L1IMU_SPACE_HYEX(GnbHandle, L1_CNTRL_1_NBIF0_ADDRESS_HYEX), &Value, 0);
  NbioRegisterRead ( GnbHandle, TYPE_D0F0xBC, L1IMU_SPACE_HYEX(GnbHandle, L1_CNTRL_1_IOAGR_ADDRESS_HYEX), &Value, 0);
  Value |= BIT10;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, L1IMU_SPACE_HYEX(GnbHandle, L1_CNTRL_1_IOAGR_ADDRESS_HYEX), &Value, 0);

  // L1_CNTRL_2: 3 L1ATSDataErrorSignalEn.
  NbioRegisterRead ( GnbHandle, TYPE_D0F0xBC, L1IMU_SPACE_HYEX(GnbHandle, L1_CNTRL_2_PCIE0_ADDRESS_HYEX), &Value, 0);
  Value |= BIT3;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, L1IMU_SPACE_HYEX(GnbHandle, L1_CNTRL_2_PCIE0_ADDRESS_HYEX), &Value, 0);
  NbioRegisterRead ( GnbHandle, TYPE_D0F0xBC, L1IMU_SPACE_HYEX(GnbHandle, L1_CNTRL_2_PCIE1_ADDRESS_HYEX), &Value, 0);
  Value |= BIT3;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, L1IMU_SPACE_HYEX(GnbHandle, L1_CNTRL_2_PCIE1_ADDRESS_HYEX), &Value, 0);
  NbioRegisterRead ( GnbHandle, TYPE_D0F0xBC, L1IMU_SPACE_HYEX(GnbHandle, L1_CNTRL_2_NBIF0_ADDRESS_HYEX), &Value, 0);
  Value |= BIT3;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, L1IMU_SPACE_HYEX(GnbHandle, L1_CNTRL_2_NBIF0_ADDRESS_HYEX), &Value, 0);
  NbioRegisterRead ( GnbHandle, TYPE_D0F0xBC, L1IMU_SPACE_HYEX(GnbHandle, L1_CNTRL_2_IOAGR_ADDRESS_HYEX), &Value, 0);
  Value |= BIT3;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, L1IMU_SPACE_HYEX(GnbHandle, L1_CNTRL_2_IOAGR_ADDRESS_HYEX), &Value, 0);

  // IOMMUL2:
  // IOMMU_COMMAND: 6 PARITY_ERROR_EN.
  IommuPciAddress = NbioGetHostPciAddress (GnbHandle);
  IommuPciAddress.Address.Function = 0x2;
  GnbLibPciRead (IommuPciAddress.AddressValue | 0x4 , AccessWidth32, &Value, NULL);
  Value |= BIT6;

  GnbLibPciWrite (IommuPciAddress.AddressValue | 0x4, AccessS3SaveWidth32, &Value, NULL); 

  // L2_DTC_CONTROL: 4 DTCParityEn.
  NbioRegisterRead ( GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, 0x15700040), &Value, 0);
  Value |= BIT4;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, 0x15700040), &Value, 0);
  // L2_ITC_CONTROL: 4 ITCParityEn.
  NbioRegisterRead ( GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, 0x15700050), &Value, 0);
  Value |= BIT4;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, 0x15700050), &Value, 0);
  // L2_PTC_A_CONTROL: 4 PTCAParityEn.
  NbioRegisterRead ( GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, 0x15700060), &Value, 0);
  Value |= BIT4;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, 0x15700060), &Value, 0);
  // L2_PDC_CONTROL: 4 PDCParityEn.
  NbioRegisterRead ( GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, 0x13F01140), &Value, 0);
  Value |= BIT4;
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, 0x13F01140), &Value, 0);
  // L2B_SDP_PARITY_ERROR_EN: 2 TWW_PARITY_ERROR_EN.
  // L2B_SDP_PARITY_ERROR_EN: 1 CP_PARITY_ERROR_EN.
  NbioRegisterRead ( GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, 0x13F01288), &Value, 0);
  Value |=(BIT1 | BIT2);
  NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, 0x13F01288), &Value, 0);

  PcieCore = 0;
  NbioRASIdentifyPcieCore (GnbHandle, &PcieCore);
  IDS_HDT_CONSOLE (GNB_TRACE, "Used PcieCore = 0x%X\n", PcieCore);

  // execute the programming sequence given in PPR section 9.1.7.1 NBIO Internal Error Reporting Configuration.
  for ( i = 0; i < sizeof (NbioParityTableST) / sizeof (NBIO_PARITY_TABLE); i++) {     
    NBRASCFG_0004.Value = 0x0;

    // only enable PCIe RAS if the PCIe core is in use
    // These groups should only be enabled if PcieCore 0 is in use
    if ((PcieCore & BIT0) == 0) {      
        //Ignore UNCORR, CORR, UCP Group 0 pcie0.TxEcc
        if(NbioParityTableST[i].GroupID == 0) {
            continue;   //Do nothing
        }
        
        //Ignore UNCORR group 12 and 13
        if (NbioParityTableST[i].GroupType == UNCORRECTABLE_GROUP_TYPE) {
            if((NbioParityTableST[i].GroupID == 12) ||
               (NbioParityTableST[i].GroupID == 13)) {
                continue;
            }
        }     
    }
    
    // These groups should only be enabled if PcieCore 1 is in use
    if ((PcieCore & BIT1) == 0) {
        //Ignore UNCORR, CORR, UCP Group 1 pcie1.TxEcc
        if(NbioParityTableST[i].GroupID == 1) {
            continue;
        }
        
        //Ignore UNCORR group 14 and 15
        if (NbioParityTableST[i].GroupType == UNCORRECTABLE_GROUP_TYPE) {
            if((NbioParityTableST[i].GroupID == 14) ||
               (NbioParityTableST[i].GroupID == 15)) {
                continue;
            }
        }        
    }
    
    // These groups should only be enabled if PcieCore 2 is in use
    if ((PcieCore & BIT2) == 0) {
        //Ignore UNCORR group 16, 17 and 18
        if (NbioParityTableST[i].GroupType == UNCORRECTABLE_GROUP_TYPE) {
            if((NbioParityTableST[i].GroupID == 16) ||
               (NbioParityTableST[i].GroupID == 17) ||
               (NbioParityTableST[i].GroupID == 18)) {
                continue;
            }
        }
        
        //Ignore CORR group 18
        if (NbioParityTableST[i].GroupType == CORRECTABLE_GROUP_TYPE) {
            if(NbioParityTableST[i].GroupID == 18) {
                continue;
            }
        }
        
        //Ignore UCP group 12
        if ((NbioParityTableST[i].GroupType == ECC_UCP_GROUP_TYPE) ||
            (NbioParityTableST[i].GroupType == DDP_UCP_GROUP_TYPE)) {
            if(NbioParityTableST[i].GroupID == 12) {
                continue;
            }
        }                
    }
    
    IDS_HDT_CONSOLE (GNB_TRACE, "Init group %02d - %02d - %02d - %02d begin \n",
                     NbioParityTableST[i].GroupType,
                     NbioParityTableST[i].GroupID,
                     NbioParityTableST[i].StructureID,
                     NbioParityTableST[i].ErrGenCmd);
    
    IDS_HDT_CONSOLE (GNB_TRACE, "Wait for NBRASCFG_0004.Field.ParityErrGenInjectAllow to 1 ...\n");
    NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, PARITY_CONTROL_1_ADDRESS_HYEX), &NBRASCFG_0004.Value, 0);
    do {
      NbioRegisterRead ( GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, PARITY_CONTROL_1_ADDRESS_HYEX), &NBRASCFG_0004.Value, 0);
    } while (NBRASCFG_0004.Field.ParityErrGenInjectAllow != 0x1);

    //Ignore CORR group 16: structure 8,9,10,11 
    if (PcdGetBool(PcdX2ApicMode)) {
       if (NbioParityTableST[i].GroupType == CORRECTABLE_GROUP_TYPE) {
          if(NbioParityTableST[i].GroupID == 16) {
            if((NbioParityTableST[i].StructureID == 8)||
               (NbioParityTableST[i].StructureID == 9)||
               (NbioParityTableST[i].StructureID == 10)||
               (NbioParityTableST[i].StructureID == 11)) {
                continue;
            }
          }
        }
    }
    
    NBRASCFG_0004.Field.ParityErrGenGroupSel = NbioParityTableST[i].GroupID;
    NBRASCFG_0004.Field.ParityErrGenIdSel = NbioParityTableST[i].StructureID;
    NBRASCFG_0004.Field.ParityErrGenCmd = NbioParityTableST[i].ErrGenCmd;
    NBRASCFG_0004.Field.ParityErrGenTrigger = 1;

    switch (NbioParityTableST[i].GroupType) {
    case UNCORRECTABLE_GROUP_TYPE:
        NBRASCFG_0004.Field.ParityErrGenGroupTypeSel = 0;
        break;
    case CORRECTABLE_GROUP_TYPE:
    case ECC_UCP_GROUP_TYPE:
    case DDP_UCP_GROUP_TYPE:
        NBRASCFG_0004.Field.ParityErrGenGroupTypeSel = 1;
        break;
    }
    NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, PARITY_CONTROL_1_ADDRESS_HYEX), &NBRASCFG_0004.Value, 0);

    if (NbioParityTableST[i].GroupType == ECC_UCP_GROUP_TYPE) {
      IDS_HDT_CONSOLE (GNB_TRACE, "ECC_UCP: Wait for NBRASCFG_0004.Field.ParityErrGenInjectAllow to 1 ...\n");  
      do {
        NbioRegisterRead ( GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, PARITY_CONTROL_1_ADDRESS_HYEX), &NBRASCFG_0004.Value, 0);
      } while (NBRASCFG_0004.Field.ParityErrGenInjectAllow != 0x1);
      
      NBRASCFG_0004.Field.ParityErrGenCmd = UCP_Reporting;
      NBRASCFG_0004.Field.ParityErrGenTrigger = 1;
      NbioRegisterWrite (GnbHandle, TYPE_D0F0xBC, NBIO_SPACE(GnbHandle, PARITY_CONTROL_1_ADDRESS_HYEX), &NBRASCFG_0004.Value, 0);
    }
    
    IDS_HDT_CONSOLE (GNB_TRACE, "Init group end \n");
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioRASControl Exit\n");
}

