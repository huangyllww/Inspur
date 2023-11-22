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
 *
 ***************************************************************************/

// This file is auto generated, don't edit it manually

#include <Base.h>
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <HygonCbsVariable.h>
#include <Library/HpcbLib.h>
#include <HGPI.h>
#include <HPCB.h>
#include <HPOB.h>
#include <HpcbAutoGenSAT.h>

VOID SetCmnTypeAttribData (
  IN OUT UINT32                  *SizeAttrib,
  IN OUT HPCB_PARAM_ATTRIBUTE    **HpcbParaAttrib,
  IN OUT UINT32                  *SizeValue,
  IN OUT UINT8                   **HpcbParaValue,
  IN UINT16                      Token,
  IN UINT64                      Value,
  IN UINT8                       TokenSize
)
{
  HPCB_PARAM_ATTRIBUTE *ParaAttrib;
  UINT8                *ParaValue;
  
  ASSERT (TokenSize <= sizeof(UINT64));
  
  ParaAttrib  = *HpcbParaAttrib;
  ParaValue   = *HpcbParaValue;

  if (ParaAttrib != NULL)
  {
    if (Token == HPCB_TOKEN_CBS_DBG_LIMIT) {
      ParaAttrib->TimePoint = 0;
      ParaAttrib->Token     = HPCB_TOKEN_CBS_DBG_LIMIT;
      ParaAttrib->Size      = 0;
      ParaAttrib->Reserved  = 0;
    }
    else {
      ParaAttrib->TimePoint = HPCB_TIME_POINT_ANY;
      ParaAttrib->Token     = Token;
      ParaAttrib->Size      = (TokenSize - 1);
      ParaAttrib++;

      // Value
      switch (TokenSize)
      {
      case sizeof(UINT8):
        *((UINT8 *)ParaValue) = (UINT8)Value;
        ParaValue += sizeof(UINT8);
        break;

      case sizeof(UINT16):
        *((UINT16 *)ParaValue) = (UINT16)Value;
        ParaValue += sizeof(UINT16);
        break;

      case sizeof(UINT32):
        *((UINT32 *)ParaValue) = (UINT32)Value;
        ParaValue += sizeof(UINT32);
        break;

      case sizeof(UINT64):
        *((UINT64 *)ParaValue) = (UINT64)Value;
        ParaValue += sizeof(UINT64);
        break;

      default:
        ASSERT (FALSE);
        break;
      }
    }
  }
  *SizeAttrib += sizeof(HPCB_PARAM_ATTRIBUTE);
  *SizeValue  += TokenSize;

  *HpcbParaAttrib = ParaAttrib;
  *HpcbParaValue  = ParaValue;
}

VOID
PrepareCmnTypeAttrib (
  UINT32               *SizeOfAttrib,
  HPCB_PARAM_ATTRIBUTE *HpcbParaAttrib,
  UINT32               *SizeOfValue,
  UINT8                *HpcbParaValue,
  VOID                 *CbsVariable
  )
{
  CBS_CONFIG  *Setup_Config;
  UINT32      SizeAttrib;
  UINT32      SizeValue;
  UINT8       IODHmi;
  UINT8       CDDHmi;
  UINT8       CDDDDR;
  UINT8       DjHmiVid;
  UINT8       DjS5Vid;
  UINT8       EmHmiVid;
  UINT8       EmCfopVid;
  UINT8       CddHmiVid;
  UINT8       CddDDRVid;
  UINT64      CbsMemberValue;
  UINT32      CbsMemberSize;

  if (CbsVariable == NULL) {
    ASSERT(FALSE);
    return;
  }

  if (SizeOfAttrib == NULL || SizeOfValue == NULL) {
    ASSERT(FALSE);
    return;
  }

  SizeAttrib = 0;
  SizeValue = 0;
  Setup_Config = (CBS_CONFIG *)CbsVariable;

  // CSV-ES ASID Space Limit
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_CPU_CSV_ASID_SPACE_LIMIT,
      Setup_Config->CbsCmnCpuCsvAsidSpaceLimit,
      sizeof(UINT32)
      );

  // HMI encryption control
  if (Setup_Config->CbsDfCmnHmiEncryption != 3) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_DF_CMN_HMI_ENCRYPTION,
        Setup_Config->CbsDfCmnHmiEncryption,
        sizeof(UINT8)
        );

  }

  // xHMI encryption control
  if (Setup_Config->CbsDfCmnXHmiEncryption != 3) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_DF_CMN_X_HMI_ENCRYPTION,
        Setup_Config->CbsDfCmnXHmiEncryption,
        sizeof(UINT8)
        );

  }

  // CC6 memory region encryption
  if (Setup_Config->CbsDfCmnCc6MemEncryption != 3) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_DF_CMN_CC6_MEM_ENCRYPTION,
        Setup_Config->CbsDfCmnCc6MemEncryption,
        sizeof(UINT8)
        );

  }

  // Location of private memory regions
  if (Setup_Config->CbsDfCmnCc6AllocationScheme != 3) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_DF_CMN_CC6_ALLOCATION_SCHEME,
        Setup_Config->CbsDfCmnCc6AllocationScheme,
        sizeof(UINT8)
        );

  }

  // System probe filter
  if (Setup_Config->CbsDfCmnSysProbeFilter != 3) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_DF_CMN_SYS_PROBE_FILTER,
        Setup_Config->CbsDfCmnSysProbeFilter,
        sizeof(UINT8)
        );

  }

  // Memory interleaving
  if (Setup_Config->CbsDfCmnMemIntlv != 7) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_DF_CMN_MEM_INTLV,
        Setup_Config->CbsDfCmnMemIntlv,
        sizeof(UINT8)
        );

  }

  // Memory interleaving size
  if (Setup_Config->CbsDfCmnMemIntlvSize != 7) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_DF_CMN_MEM_INTLV_SIZE,
        Setup_Config->CbsDfCmnMemIntlvSize,
        sizeof(UINT8)
        );

  }

  // Channel interleaving hash
  if (Setup_Config->CbsDfCmnChnlIntlvHash != 3) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_DF_CMN_CHNL_INTLV_HASH,
        Setup_Config->CbsDfCmnChnlIntlvHash,
        sizeof(UINT8)
        );

  }

  // Subchannel interleaving bit
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_SUBCHNL_INTLV_BIT,
      Setup_Config->CbsDfCmnSubchannelIntlvBit,
      sizeof(UINT8)
      );

  // Memory Clear
  if (Setup_Config->CbsDfCmnMemClear != 3) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_DF_CMN_MEM_CLEAR,
        Setup_Config->CbsDfCmnMemClear,
        sizeof(UINT8)
        );

  }

  // FclkUclkSyncMode
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_FCLK_UCLK_SYNC_MODE,
      Setup_Config->CbsFclkUclkSyncMode,
      sizeof(UINT8)
      );

  // Overclock
  if (Setup_Config->CbsCmnMemOverclockDdr5 != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_OVERCLOCK_DDR5,
        Setup_Config->CbsCmnMemOverclockDdr5,
        sizeof(UINT8)
        );

  }

  // Memory Clock Speed
  if (Setup_Config->CbsCmnMemSpeedDdr5 != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_SPEED_DDR5,
        Setup_Config->CbsCmnMemSpeedDdr5,
        sizeof(UINT8)
        );

  }

  // Cmd2T
  if (Setup_Config->CbsCmnMemCtrller2TModeDdr5 != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_CTRLLER2_T_MODE_DDR5,
        Setup_Config->CbsCmnMemCtrller2TModeDdr5,
        sizeof(UINT8)
        );

  }

  // Gear Down Mode
  if (Setup_Config->CbsCmnMemGearDownModeDdr5 != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_GEAR_DOWN_MODE_DDR5,
        Setup_Config->CbsCmnMemGearDownModeDdr5,
        sizeof(UINT8)
        );

  }

  // Power Down Enable
  if (Setup_Config->CbsCmnMemCtrllerPwrDnEnDdr5 != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_CTRLLER_PWR_DN_EN_DDR5,
        Setup_Config->CbsCmnMemCtrllerPwrDnEnDdr5,
        sizeof(UINT8)
        );

  }

  // Data Bus Configuration User Controls
  if (Setup_Config->CbsCmnMemDataBusConfigCtlDdr5 != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_DATA_BUS_CONFIG_CTL_DDR5,
        Setup_Config->CbsCmnMemDataBusConfigCtlDdr5,
        sizeof(UINT8)
        );

  }

  // phydrvstrenQqdqs
  if (Setup_Config->CbsCmnMemCtrllerPhyDrvStrenDqdqs != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_CTRLLER_PHY_DRV_STREN_DQ_DQS_DDR5,
        Setup_Config->CbsCmnMemCtrllerPhyDrvStrenDqdqs,
        sizeof(UINT8)
        );

  }
  
  // PhyOdtStrenDqDqs
  if (Setup_Config->CbsCmnMemCtrllerPhyOdtStrenDqDqs != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_CTRLLER_PHY_ODT_STREN_DQ_DQS_DDR5,
        Setup_Config->CbsCmnMemCtrllerPhyOdtStrenDqDqs,
        sizeof(UINT8)
        );

  }
  
  // RttNom
  if (Setup_Config->CbsCmnMemCtrllerRttNomRDDdr5 != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_CTRLLER_DQ_RTT_NOM_RD_DDR5,
        Setup_Config->CbsCmnMemCtrllerRttNomRDDdr5,
        sizeof(UINT8)
        );

  }

  // RttWr
  if (Setup_Config->CbsCmnMemCtrllerRttWrDdr5 != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_CTRLLER_DQ_RTT_WR_DDR5,
        Setup_Config->CbsCmnMemCtrllerRttWrDdr5,
        sizeof(UINT8)
        );

  }

  // RttNomWr
  if (Setup_Config->CbsCmnMemCtrllerRttNomWrDdr5 != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_CTRLLER_DQ_RTT_NOM_WR_DDR5,
        Setup_Config->CbsCmnMemCtrllerRttNomWrDdr5,
        sizeof(UINT8)
        );

  }
  
  // RttPark
  if (Setup_Config->CbsCmnMemCtrllerRttParkDdr5 != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_CTRLLER_DQ_RTT_PARK_DDR5,
        Setup_Config->CbsCmnMemCtrllerRttParkDdr5,
        sizeof(UINT8)
        );

  }

  // Data Poisoning
  if (Setup_Config->CbsCmnMemDataPoisoningDdr5 != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_DATA_POISONING_DDR5,
        Setup_Config->CbsCmnMemDataPoisoningDdr5,
        sizeof(UINT8)
        );

  }

  // TSME
  if (Setup_Config->CbsCmnMemTsmeDdr5 != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_TSME_DDR5,
        Setup_Config->CbsCmnMemTsmeDdr5,
        sizeof(UINT8)
        );

  }

  // Data Scramble
  if (Setup_Config->CbsCmnMemCtrllerDataScrambleDdr5 != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_CTRLLER_DATA_SCRAMBLE_DDR5,
        Setup_Config->CbsCmnMemCtrllerDataScrambleDdr5,
        sizeof(UINT8)
        );

  }

  // Chipselect Interleaving
  if (Setup_Config->CbsCmnMemMappingBankInterleaveDdr5 != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_MAPPING_BANK_INTERLEAVE_DDR5,
        Setup_Config->CbsCmnMemMappingBankInterleaveDdr5,
        sizeof(UINT8)
        );

  }

  // BankGroupSwap
  if (Setup_Config->CbsCmnMemCtrllerBankGroupSwapDdr5 != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_DDR5,
        Setup_Config->CbsCmnMemCtrllerBankGroupSwapDdr5,
        sizeof(UINT8)
        );

  }

  // Address Hash Bank
  if (Setup_Config->CbsCmnMemAddressHashBankDdr5 != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_ADDRESS_HASH_BANK_DDR5,
        Setup_Config->CbsCmnMemAddressHashBankDdr5,
        sizeof(UINT8)
        );

  }

  // Address Hash CS
  if (Setup_Config->CbsCmnMemAddressHashCsDdr5 != 0xFF) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_ADDRESS_HASH_CS_DDR5,
        Setup_Config->CbsCmnMemAddressHashCsDdr5,
        sizeof(UINT8)
        );

  }

  // MBIST Enable
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_MEM_MBIST_EN,
      Setup_Config->CbsCmnMemMbistEn,
      sizeof(UINT8)
      );

  // MBIST Test Mode

  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_MEM_MBIST_SUB_TEST,
      Setup_Config->CbsCmnMemMbistTestmode,
      sizeof(UINT8)
      );

  // MBIST Aggressors
  if (Setup_Config->CbsCmnMemMbistAggressors != 0xff) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_MBIST_AGGRESSORS,
        Setup_Config->CbsCmnMemMbistAggressors,
        sizeof(UINT8)
        );

  }

  // MBIST Per Bit Slave Die Reporting
  if (Setup_Config->CbsCmnMemMbistPerBitSlaveDieReport != 0xff) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT,
        Setup_Config->CbsCmnMemMbistPerBitSlaveDieReport,
        sizeof(UINT8)
        );

  }
  // HYGON_EX_CPU
  // CLDO_VDDP Control
  if (Setup_Config->CbsCmnCLDO_VDDPCtl != 0) {
    SetCmnTypeAttribData(
        &SizeAttrib,
        &HpcbParaAttrib,
        &SizeValue,
        &HpcbParaValue,
        HPCB_TOKEN_CBS_CMN_CLDO_VDDP_CTL,
        Setup_Config->CbsCmnCLDO_VDDPCtl,
        sizeof(UINT8)
        );

  }

  // CLDO_VDDP voltage

  IODHmi = (UINT8)((Setup_Config->CbsCmnCLDOVDDPvoltageIODHmiPHY + 4 - 245) / 5);
  CDDHmi = (UINT8)((Setup_Config->CbsCmnCLDOVDDPvoltageCDDHmiCFOPPHY + 4 - 245) / 5);
  CDDDDR = (UINT8)((Setup_Config->CbsCmnCLDOVDDPvoltageCDDDDRPHY + 4 - 245) / 5);

  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_CLDOVDD_PVOLTAGE,
      ((IODHmi << 16) + (CDDHmi << 8) + CDDDDR),
      sizeof(UINT32)
      );

  // HYGON_GX_CPU
  // DJ CLDO_VDDP Control
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_DJ_CLDO_VDDP_CTL,
      Setup_Config->CbsCmnDjCLDO_VDDPCtl,
      sizeof(UINT8)
      );

  // DJ CLDO_VDDP voltage
  DjHmiVid = (UINT8)((Setup_Config->CbsCmnDjCLDOVDDPvoltageHmi + 4 - 245) / 5);
  DjS5Vid = (UINT8)((Setup_Config->CbsCmnDjCLDOVDDPvoltageS5 + 4 - 245) / 5);

  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_DJ_CLDOVDD_PVOLTAGE,
      ((HYGX_DJ_DIE_TYPE << 16) + (DjHmiVid << 8) + DjS5Vid),
      sizeof(UINT32)
      );

  // EM CLDO_VDDP Control
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_EM_CLDO_VDDP_CTL,
      Setup_Config->CbsCmnEmCLDO_VDDPCtl,
      sizeof(UINT8)
      );

  // EM CLDO_VDDP voltage
  EmHmiVid = (UINT8)((Setup_Config->CbsCmnEmCLDOVDDPvoltageHmi + 4 - 245) / 5);
  EmCfopVid = (UINT8)((Setup_Config->CbsCmnEmCLDOVDDPvoltageCfop + 4 - 245) / 5);

  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_EM_CLDOVDD_PVOLTAGE,
      ((HYGX_EMEI_DIE_TYPE << 16) + (EmHmiVid << 8) + EmCfopVid),
      sizeof(UINT32)
      );

  // CDD CLDO_VDDP Control
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_CDD_CLDO_VDDP_CTL,
      Setup_Config->CbsCmnCddCLDO_VDDPCtl,
        sizeof(UINT8)
      );

  // CDD CLDO_VDDP voltage
  CddHmiVid = (UINT8)((Setup_Config->CbsCmnCddCLDOVDDPvoltageHmi + 4 - 245) / 5);
  CddDDRVid = (UINT8)((Setup_Config->CbsCmnCddCLDOVDDPvoltageDDR + 4 - 245) / 5);

  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_CDD_CLDOVDD_PVOLTAGE,
      ((HYGX_CORE_DIE_TYPE << 16) + (CddHmiVid << 8) + CddDDRVid),
      sizeof(UINT32)
      );

  // Mca Bank Control
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_MCA_BANK_CTL,
      Setup_Config->CbsDfCmnMcaBankCtrl,
      sizeof(UINT8)
      );

  // Socket Number
  HygonPspHpcbGetCbsCmnParameter (
    HPCB_TOKEN_CBS_CMN_SOCKET_NUMBER,
    &CbsMemberSize,
    &CbsMemberValue
  );
  
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_SOCKET_NUMBER,
      CbsMemberValue,
      (UINT8)CbsMemberSize
      );
  
  // Xhmi Connect Type
  HygonPspHpcbGetCbsCmnParameter (
    HPCB_TOKEN_CBS_CMN_XHMI_CONNECT_TYPE,
    &CbsMemberSize,
    &CbsMemberValue
  );
  
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_XHMI_CONNECT_TYPE,
      CbsMemberValue,
      (UINT8)CbsMemberSize
      );
  
  // GNB xHMI PHY Rate
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_GNB_XHMI_PHY_RATE_CFG,
      Setup_Config->CbsGnbxHmiPHYRateCFG,
      sizeof(UINT8)
      );

  // GNB HMI PHY Rate
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_GNB_HMI_PHY_RATE_CFG,
      Setup_Config->CbsGnbHmiPHYRateCFG,
      sizeof(UINT8)
      );

  // HBL log cntl
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_FCH_CONSOLE_OUT_ENABLE,
      Setup_Config->CbsHBLCntl,
      sizeof(UINT8)
      );

  // HBL die type
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_FCH_CONSOLE_OUT_DIE_TYPE,
      Setup_Config->CbsHBLDieType,
      sizeof(UINT8)
      );

  // SOCKET ID type
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_FCH_CONSOLE_OUT_SOCKET_ID,
      Setup_Config->CbsHBLSOCKETID,
      sizeof(UINT8)
      );

  // IOD ID type
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_FCH_CONSOLE_OUT_IOD_ID,
      Setup_Config->CbsHBLIODID,
      sizeof(UINT8)
      );

  // CDD ID type
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_FCH_CONSOLE_OUT_CDD_ID,
      Setup_Config->CbsHBLCDDID,
      sizeof(UINT8)
      );

  // Psp Log category
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_PSP_LOG_CATEGORY,
      Setup_Config->CbsPspLogCategory,
      sizeof(UINT8)
      );

  // Psp log Level
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_PSP_LOG_LEVEL,
      Setup_Config->CbsPspLogLevel,
      sizeof(UINT8)
      );

  // mem eye test
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_MEM_EYE_TEST,
      Setup_Config->CbsMemEyeTest,
      sizeof(UINT8)
      );

  // Refresh Mode
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_AUTOREFFINEGRANMODE,
      Setup_Config->AutoRefFineGranMode,
      sizeof(UINT8)
      );

  // CLDO_VDDM Control
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_CLDO_VDDM_CTL,
      Setup_Config->CbsCmnCLDO_VDDMCtl,
      sizeof(UINT8)
      );

  // CLDO_VDDM voltage
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_CLDOVDD_MVOLTAGE,
      Setup_Config->CbsCmnCLDOVDDMvoltage,
      sizeof(UINT32)
      );

  // HYGON_EX_CPU
  // BypassVddp_Cldo_IOD_HMI
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_CLDO_VDDP_IOD_HMI_VID,
      Setup_Config->CbsCmnCldoVDDPBypassIODHMI_MODE,
      sizeof(UINT8)
      );

  // BypassVddp_Cldo_CDD_HMI
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_CLDO_VDDP_CDD_HMI_VID,
      Setup_Config->CbsCmnCldoVDDPBypassCDDHMI_MODE,
      sizeof(UINT8)
      );

  // BypassVddp_Cldo_CDD_DDR
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_CLDO_VDDP_CDD_DDR_VID,
      Setup_Config->CbsCmnCldoVDDPBypassCDDDDR_MODE,
      sizeof(UINT8)
      );

  // HYGON_GX_CPU
  // Cldo_Vddp DJ Bypass Mode
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_CLDO_VDDP_DJ_BYPASS_MODE,
      ((HYGX_DJ_DIE_TYPE << 16) + (Setup_Config->CbsCmnCldoVDDPDjHmiBypassMode << 8) + Setup_Config->CbsCmnCldoVDDPDjS5BypassMode),
      sizeof(UINT32)
      );

  // Cldo_Vddp EM Bypass Mode
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_CLDO_VDDP_EM_BYPASS_MODE,
      ((HYGX_EMEI_DIE_TYPE << 16) + (Setup_Config->CbsCmnCldoVDDPEmHmiBypassMode << 8) + Setup_Config->CbsCmnCldoVDDPEmCfopBypassMode),
      sizeof(UINT32)
      );
  // Cldo_Vddp CDD Bypass Mode
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_CLDO_VDDP_CDD_BYPASS_MODE,
      ((HYGX_CORE_DIE_TYPE << 16) + (Setup_Config->CbsCmnCldoVDDPCddHmiBypassMode << 8) + Setup_Config->CbsCmnCldoVDDPCddDdrBypassMode),
      sizeof(UINT32)
      );

  // Cldo_Vddm Bypass Mode
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_CLDO_VDDM_BYPASS_MODE,
      Setup_Config->CbsCmnCldoVDDMBypass_MODE,
      sizeof(UINT8)
      );

  // AVS_CTL
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_AVS_CTL,
      Setup_Config->CbsCmnAvsCtl,
      sizeof(UINT8)
      );

  // TPCM Control
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_TPCM_CONTROL,
      Setup_Config->CbsTpcmControl,
      sizeof(UINT8)
      );

  // PSF Control(CSV)
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_PSF_CONTROL,
      Setup_Config->CbsPsfControl,
      sizeof(UINT8)
      );

  // POST complete pin
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_POST_COMPLETE_PIN_CTRL,
      Setup_Config->CbsPostCompletePin,
      sizeof(UINT32)
      );

  // Serial Port Select
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_CONSOLE_OUT_SERIAL_PORT,
      Setup_Config->CbsSerialPortCtl,
      sizeof(UINT8)
      );

  //SPI Speed
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_BIOS_SPI_SPEED_MODE,
      Setup_Config->CbsCmnSPISpeedCtrl,
      sizeof(UINT8)
      );

  // QoS Control
  SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_CMN_QOS,
      Setup_Config->CbsQoSControl,
      sizeof(UINT8)
      );

  // End
  SetCmnTypeAttribData (
    &SizeAttrib,
    &HpcbParaAttrib,
    &SizeValue,
    &HpcbParaValue,
    HPCB_TOKEN_CBS_DBG_LIMIT,
    0,
    0
  );

  *SizeOfAttrib = SizeAttrib;
  *SizeOfValue = SizeValue;
}

VOID
PrepareDbgTypeAttrib (
  UINT32               *SizeOfAttrib,
  HPCB_PARAM_ATTRIBUTE *HpcbParaAttrib,
  UINT32               *SizeOfValue,
  UINT8                *HpcbParaValue,
  VOID                 *CbsVariable
  )
{
  CBS_CONFIG  *Setup_Config;
  UINT32      SizeAttrib;
  UINT32      SizeValue;

  if (CbsVariable == NULL) {
    ASSERT (FALSE);
    return;
  }

  if (SizeOfAttrib == NULL || SizeOfValue == NULL) {
    ASSERT (FALSE);
    return;
  }

  SizeAttrib = 0;
  SizeValue  = 0;
  Setup_Config = (CBS_CONFIG *)CbsVariable;

  //DRAM ECC Symbol Size
  if (Setup_Config->CbsCmnMemCtrllerDramEccSymbolSizeDdr5 != 0xFF) {
    SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_DRAM_ECC_SYMBOL_SIZE_DDR5,
      Setup_Config->CbsCmnMemCtrllerDramEccSymbolSizeDdr5,
      sizeof(UINT8)
      );
  }

  //DRAM ECC Enable
  if (Setup_Config->CbsCmnMemCtrllerDramEccEnDdr5 != 0xFF) {
    SetCmnTypeAttribData(
      &SizeAttrib,
      &HpcbParaAttrib,
      &SizeValue,
      &HpcbParaValue,
      HPCB_TOKEN_CBS_DBG_MEM_CTRLLER_DRAM_ECC_EN_DDR5,
      Setup_Config->CbsCmnMemCtrllerDramEccEnDdr5,
      sizeof(UINT8)
      );

  }

  //End
  SetCmnTypeAttribData (
    &SizeAttrib,
    &HpcbParaAttrib,
    &SizeValue,
    &HpcbParaValue,
    HPCB_TOKEN_CBS_DBG_LIMIT,
    0,
    0
  );
  
  *SizeOfAttrib  = SizeAttrib;
  *SizeOfValue   = SizeValue;
}

