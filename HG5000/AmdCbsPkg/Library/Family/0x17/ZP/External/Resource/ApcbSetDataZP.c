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
 *
 ***************************************************************************/


//This file is auto generated, don't edit it manually


#include <Base.h>
#include <Library/DebugLib.h>
#include <AmdCbsVariable.h>
#include <AGESA.h>
#include <Firmwares/ZP/APCB.h>
#include <ApcbAutoGenZP.h>

VOID
PrepareCmnTypeAttrib (
  UINT32 *Size,
  APCB_PARAM_ATTRIBUTE *ApcbParaAttrib,
  VOID *CbsVariable
  )
{
  CBS_CONFIG *Setup_Config;
  UINT32     _Size;

  if (CbsVariable == NULL) {
    ASSERT (FALSE);
    return;
  }

  if (Size == NULL) {
    ASSERT (FALSE);
    return;
  }

  _Size = 0;
  Setup_Config = (CBS_CONFIG *) CbsVariable;


  //SEV-ES ASID Space Limit
  if (ApcbParaAttrib != NULL) {
    ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
    ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_CPU_SEV_ASID_SPACE_LIMIT;
    ApcbParaAttrib->Size = (sizeof (UINT32) -1);
    ApcbParaAttrib++;
  }
  _Size += sizeof (APCB_PARAM_ATTRIBUTE);

  //GMI encryption control
  if (Setup_Config->CbsDfCmnGmiEncryption != 3) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_DF_CMN_GMI_ENCRYPTION;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //xGMI encryption control
  if (Setup_Config->CbsDfCmnXGmiEncryption != 3) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_DF_CMN_X_GMI_ENCRYPTION;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //CC6 memory region encryption
  if (Setup_Config->CbsDfCmnCc6MemEncryption != 3) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_DF_CMN_CC6_MEM_ENCRYPTION;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Location of private memory regions
  if (Setup_Config->CbsDfCmnCc6AllocationScheme != 3) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_DF_CMN_CC6_ALLOCATION_SCHEME;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //System probe filter
  if (Setup_Config->CbsDfCmnSysProbeFilter != 3) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_DF_CMN_SYS_PROBE_FILTER;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Memory interleaving
  if (Setup_Config->CbsDfCmnMemIntlv != 7) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_DF_CMN_MEM_INTLV;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Memory interleaving size
  if (Setup_Config->CbsDfCmnMemIntlvSize != 7) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_DF_CMN_MEM_INTLV_SIZE;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Channel interleaving hash
  if (Setup_Config->CbsDfCmnChnlIntlvHash != 3) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_DF_CMN_CHNL_INTLV_HASH;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Memory Clear
  if (Setup_Config->CbsDfCmnMemClear != 3) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_DF_CMN_MEM_CLEAR;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Overclock
  if (Setup_Config->CbsCmnMemOverclockDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_OVERCLOCK_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Memory Clock Speed
  if (Setup_Config->CbsCmnMemSpeedDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_SPEED_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Tcl
  if (Setup_Config->CbsCmnMemTimingTclDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TCL_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Trcdrd
  if (Setup_Config->CbsCmnMemTimingTrcdrdDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRCDRD_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Trcdwr
  if (Setup_Config->CbsCmnMemTimingTrcdwrDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRCDWR_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Trp
  if (Setup_Config->CbsCmnMemTimingTrpDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRP_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Tras
  if (Setup_Config->CbsCmnMemTimingTrasDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRAS_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Trc Ctrl
  if (Setup_Config->CbsCmnMemTimingTrcCtrlDdr4 != 0) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRC_CTRL_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Trc
  if (ApcbParaAttrib != NULL) {
    ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
    ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRC_DDR4;
    ApcbParaAttrib->Size = (sizeof (UINT8) -1);
    ApcbParaAttrib++;
  }
  _Size += sizeof (APCB_PARAM_ATTRIBUTE);

  //TrrdS
  if (Setup_Config->CbsCmnMemTimingTrrdSDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRRD_S_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //TrrdL
  if (Setup_Config->CbsCmnMemTimingTrrdLDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRRD_L_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Tfaw Ctrl
  if (Setup_Config->CbsCmnMemTimingTfawCtrlDdr4 != 0) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TFAW_CTRL_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Tfaw
  if (ApcbParaAttrib != NULL) {
    ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
    ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TFAW_DDR4;
    ApcbParaAttrib->Size = (sizeof (UINT8) -1);
    ApcbParaAttrib++;
  }
  _Size += sizeof (APCB_PARAM_ATTRIBUTE);

  //TwtrS
  if (Setup_Config->CbsCmnMemTimingTwtrSDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TWTR_S_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //TwtrL
  if (Setup_Config->CbsCmnMemTimingTwtrLDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TWTR_L_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Twr Ctrl
  if (Setup_Config->CbsCmnMemTimingTwrCtrlDdr4 != 0) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TWR_CTRL_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Twr
  if (ApcbParaAttrib != NULL) {
    ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
    ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TWR_DDR4;
    ApcbParaAttrib->Size = (sizeof (UINT8) -1);
    ApcbParaAttrib++;
  }
  _Size += sizeof (APCB_PARAM_ATTRIBUTE);

  //Trcpage Ctrl
  if (Setup_Config->CbsCmnMemTimingTrcpageCtrlDdr4 != 0) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRCPAGE_CTRL_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Trcpage
  if (ApcbParaAttrib != NULL) {
    ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
    ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRCPAGE_DDR4;
    ApcbParaAttrib->Size = (sizeof (UINT16) -1);
    ApcbParaAttrib++;
  }
  _Size += sizeof (APCB_PARAM_ATTRIBUTE);

  //TrdrdScL Ctrl
  if (Setup_Config->CbsCmnMemTimingTrdrdScLCtrlDdr4 != 0) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRDRD_SC_L_CTRL_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //TrdrdScL
  if (ApcbParaAttrib != NULL) {
    ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
    ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRDRD_SC_L_DDR4;
    ApcbParaAttrib->Size = (sizeof (UINT8) -1);
    ApcbParaAttrib++;
  }
  _Size += sizeof (APCB_PARAM_ATTRIBUTE);

  //TwrwrScL Ctrl
  if (Setup_Config->CbsCmnMemTimingTwrwrScLCtrlDdr4 != 0) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TWRWR_SC_L_CTRL_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //TwrwrScL
  if (ApcbParaAttrib != NULL) {
    ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
    ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TWRWR_SC_L_DDR4;
    ApcbParaAttrib->Size = (sizeof (UINT8) -1);
    ApcbParaAttrib++;
  }
  _Size += sizeof (APCB_PARAM_ATTRIBUTE);

  //Trfc Ctrl
  if (Setup_Config->CbsCmnMemTimingTrfcCtrlDdr4 != 0) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRFC_CTRL_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Trfc
  if (ApcbParaAttrib != NULL) {
    ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
    ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRFC_DDR4;
    ApcbParaAttrib->Size = (sizeof (UINT16) -1);
    ApcbParaAttrib++;
  }
  _Size += sizeof (APCB_PARAM_ATTRIBUTE);

  //Trfc2 Ctrl
  if (Setup_Config->CbsCmnMemTimingTrfc2CtrlDdr4 != 0) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRFC2_CTRL_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Trfc2
  if (ApcbParaAttrib != NULL) {
    ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
    ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRFC2_DDR4;
    ApcbParaAttrib->Size = (sizeof (UINT16) -1);
    ApcbParaAttrib++;
  }
  _Size += sizeof (APCB_PARAM_ATTRIBUTE);

  //Trfc4 Ctrl
  if (Setup_Config->CbsCmnMemTimingTrfc4CtrlDdr4 != 0) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRFC4_CTRL_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Trfc4
  if (ApcbParaAttrib != NULL) {
    ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
    ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRFC4_DDR4;
    ApcbParaAttrib->Size = (sizeof (UINT16) -1);
    ApcbParaAttrib++;
  }
  _Size += sizeof (APCB_PARAM_ATTRIBUTE);

  //ProcODT
  if (Setup_Config->CbsCmnMemCtrllerProcOdtDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_CTRLLER_PROC_ODT_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Tcwl
  if (Setup_Config->CbsCmnMemTimingTcwlDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TCWL_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Trtp
  if (Setup_Config->CbsCmnMemTimingTrtpDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRTP_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Trdwr
  if (Setup_Config->CbsCmnMemTimingTrdwrDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRDWR_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Twrrd
  if (Setup_Config->CbsCmnMemTimingTwrrdDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TWRRD_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //TwrwrSc
  if (Setup_Config->CbsCmnMemTimingTwrwrScDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TWRWR_SC_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //TwrwrSd
  if (Setup_Config->CbsCmnMemTimingTwrwrSdDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TWRWR_SD_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //TwrwrDd
  if (Setup_Config->CbsCmnMemTimingTwrwrDdDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TWRWR_DD_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //TrdrdSc
  if (Setup_Config->CbsCmnMemTimingTrdrdScDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRDRD_SC_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //TrdrdSd
  if (Setup_Config->CbsCmnMemTimingTrdrdSdDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRDRD_SD_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //TrdrdDd
  if (Setup_Config->CbsCmnMemTimingTrdrdDdDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TRDRD_DD_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Tcke
  if (Setup_Config->CbsCmnMemTimingTckeDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TIMING_TCKE_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Cmd2T
  if (Setup_Config->CbsCmnMemCtrller2TModeDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_CTRLLER2_T_MODE_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Gear Down Mode
  if (Setup_Config->CbsCmnMemGearDownModeDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_GEAR_DOWN_MODE_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Power Down Enable
  if (Setup_Config->CbsCmnMemCtrllerPwrDnEnDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_CTRLLER_PWR_DN_EN_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Data Bus Configuration User Controls
  if (Setup_Config->CbsCmnMemDataBusConfigCtlDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_DATA_BUS_CONFIG_CTL_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //RttNom
  if (Setup_Config->CbsCmnMemCtrllerRttNomDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_CTRLLER_RTT_NOM_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //RttWr
  if (Setup_Config->CbsCmnMemCtrllerRttWrDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_CTRLLER_RTT_WR_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //RttPark
  if (Setup_Config->CbsCmnMemCtrllerRttParkDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_CTRLLER_RTT_PARK_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Data Poisoning
  if (Setup_Config->CbsCmnMemDataPoisoningDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_DATA_POISONING_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //DRAM ECC Symbol Size
  if (Setup_Config->CbsCmnMemCtrllerDramEccSymbolSizeDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_CTRLLER_DRAM_ECC_SYMBOL_SIZE_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //DRAM ECC Enable
  if (Setup_Config->CbsCmnMemCtrllerDramEccEnDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_CTRLLER_DRAM_ECC_EN_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //TSME
  if (Setup_Config->CbsCmnMemTsmeDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_TSME_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Data Scramble
  if (Setup_Config->CbsCmnMemCtrllerDataScrambleDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_CTRLLER_DATA_SCRAMBLE_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Chipselect Interleaving
  if (Setup_Config->CbsCmnMemMappingBankInterleaveDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_MAPPING_BANK_INTERLEAVE_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //BankGroupSwap
  if (Setup_Config->CbsCmnMemCtrllerBankGroupSwapDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //BankGroupSwapAlt
  if (Setup_Config->CbsCmnMemCtrllerBankGroupSwapAltDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_ALT_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Address Hash Bank
  if (Setup_Config->CbsCmnMemAddressHashBankDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_ADDRESS_HASH_BANK_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //Address Hash CS
  if (Setup_Config->CbsCmnMemAddressHashCsDdr4 != 0xFF) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_ADDRESS_HASH_CS_DDR4;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //MBIST Enable
  if (ApcbParaAttrib != NULL) {
    ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
    ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_MBIST_EN;
    ApcbParaAttrib->Size = (sizeof (UINT8) -1);
    ApcbParaAttrib++;
  }
  _Size += sizeof (APCB_PARAM_ATTRIBUTE);

  //MBIST Test Mode
  if (ApcbParaAttrib != NULL) {
    ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
    ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_MBIST_TESTMODE;
    ApcbParaAttrib->Size = (sizeof (UINT8) -1);
    ApcbParaAttrib++;
  }
  _Size += sizeof (APCB_PARAM_ATTRIBUTE);

  //MBIST Aggressors
  if (Setup_Config->CbsCmnMemMbistAggressors != 0xff) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_MBIST_AGGRESSORS;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //MBIST Per Bit Slave Die Reporting
  if (Setup_Config->CbsCmnMemMbistPerBitSlaveDieReport != 0xff) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //CLDO_VDDP Control
  if (Setup_Config->CbsCmnCLDO_VDDPCtl != 0) {
    if (ApcbParaAttrib != NULL) {
      ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
      ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_CLDO_VDDP_CTL;
      ApcbParaAttrib->Size = (sizeof (UINT8) -1);
      ApcbParaAttrib++;
    }
    _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  }

  //CLDO_VDDP voltage
  if (ApcbParaAttrib != NULL) {
    ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
    ApcbParaAttrib->Token = APCB_TOKEN_CBS_CMN_CLDOVDD_PVOLTAGE;
    ApcbParaAttrib->Size = (sizeof (UINT32) -1);
    ApcbParaAttrib++;
  }
  _Size += sizeof (APCB_PARAM_ATTRIBUTE);

  //End
  if (ApcbParaAttrib != NULL) {
    ApcbParaAttrib->TimePoint = 0;
    ApcbParaAttrib->Token = APCB_TOKEN_CBS_DBG_LIMIT;
    ApcbParaAttrib->Size = 0;
    ApcbParaAttrib->Reserved = 0;
  }

  _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  *Size = _Size;
}

VOID
PrepareCmnTypeValue (
  UINT32 *Size,
  UINT8 *ApcbParaValue,
  VOID *CbsVariable
  )
{
  CBS_CONFIG *Setup_Config;
  UINT32     _Size;

  if (CbsVariable == NULL) {
    ASSERT (FALSE);
    return;
  }

  if (Size == NULL) {
    ASSERT (FALSE);
    return;
  }

  _Size = 0;
  Setup_Config = (CBS_CONFIG *) CbsVariable;

  //SEV-ES ASID Space Limit
  if (ApcbParaValue != NULL) {
    * ((UINT32 *) ApcbParaValue) = Setup_Config->CbsCmnCpuSevAsidSpaceLimit;
    ApcbParaValue += sizeof (UINT32);
  }
  _Size += sizeof (UINT32);

  //GMI encryption control
  if (Setup_Config->CbsDfCmnGmiEncryption != 3) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsDfCmnGmiEncryption;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //xGMI encryption control
  if (Setup_Config->CbsDfCmnXGmiEncryption != 3) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsDfCmnXGmiEncryption;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //CC6 memory region encryption
  if (Setup_Config->CbsDfCmnCc6MemEncryption != 3) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsDfCmnCc6MemEncryption;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Location of private memory regions
  if (Setup_Config->CbsDfCmnCc6AllocationScheme != 3) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsDfCmnCc6AllocationScheme;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //System probe filter
  if (Setup_Config->CbsDfCmnSysProbeFilter != 3) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsDfCmnSysProbeFilter;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Memory interleaving
  if (Setup_Config->CbsDfCmnMemIntlv != 7) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsDfCmnMemIntlv;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Memory interleaving size
  if (Setup_Config->CbsDfCmnMemIntlvSize != 7) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsDfCmnMemIntlvSize;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Channel interleaving hash
  if (Setup_Config->CbsDfCmnChnlIntlvHash != 3) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsDfCmnChnlIntlvHash;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Memory Clear
  if (Setup_Config->CbsDfCmnMemClear != 3) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsDfCmnMemClear;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Overclock
  if (Setup_Config->CbsCmnMemOverclockDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemOverclockDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Memory Clock Speed
  if (Setup_Config->CbsCmnMemSpeedDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemSpeedDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Tcl
  if (Setup_Config->CbsCmnMemTimingTclDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTclDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Trcdrd
  if (Setup_Config->CbsCmnMemTimingTrcdrdDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrcdrdDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Trcdwr
  if (Setup_Config->CbsCmnMemTimingTrcdwrDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrcdwrDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Trp
  if (Setup_Config->CbsCmnMemTimingTrpDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrpDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Tras
  if (Setup_Config->CbsCmnMemTimingTrasDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrasDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Trc Ctrl
  if (Setup_Config->CbsCmnMemTimingTrcCtrlDdr4 != 0) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrcCtrlDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Trc
  if (ApcbParaValue != NULL) {
    * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrcDdr4;
    ApcbParaValue += sizeof (UINT8);
  }
  _Size += sizeof (UINT8);

  //TrrdS
  if (Setup_Config->CbsCmnMemTimingTrrdSDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrrdSDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //TrrdL
  if (Setup_Config->CbsCmnMemTimingTrrdLDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrrdLDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Tfaw Ctrl
  if (Setup_Config->CbsCmnMemTimingTfawCtrlDdr4 != 0) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTfawCtrlDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Tfaw
  if (ApcbParaValue != NULL) {
    * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTfawDdr4;
    ApcbParaValue += sizeof (UINT8);
  }
  _Size += sizeof (UINT8);

  //TwtrS
  if (Setup_Config->CbsCmnMemTimingTwtrSDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTwtrSDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //TwtrL
  if (Setup_Config->CbsCmnMemTimingTwtrLDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTwtrLDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Twr Ctrl
  if (Setup_Config->CbsCmnMemTimingTwrCtrlDdr4 != 0) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTwrCtrlDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Twr
  if (ApcbParaValue != NULL) {
    * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTwrDdr4;
    ApcbParaValue += sizeof (UINT8);
  }
  _Size += sizeof (UINT8);

  //Trcpage Ctrl
  if (Setup_Config->CbsCmnMemTimingTrcpageCtrlDdr4 != 0) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrcpageCtrlDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Trcpage
  if (ApcbParaValue != NULL) {
    * ((UINT16 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrcpageDdr4;
    ApcbParaValue += sizeof (UINT16);
  }
  _Size += sizeof (UINT16);

  //TrdrdScL Ctrl
  if (Setup_Config->CbsCmnMemTimingTrdrdScLCtrlDdr4 != 0) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrdrdScLCtrlDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //TrdrdScL
  if (ApcbParaValue != NULL) {
    * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrdrdScLDdr4;
    ApcbParaValue += sizeof (UINT8);
  }
  _Size += sizeof (UINT8);

  //TwrwrScL Ctrl
  if (Setup_Config->CbsCmnMemTimingTwrwrScLCtrlDdr4 != 0) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTwrwrScLCtrlDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //TwrwrScL
  if (ApcbParaValue != NULL) {
    * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTwrwrScLDdr4;
    ApcbParaValue += sizeof (UINT8);
  }
  _Size += sizeof (UINT8);

  //Trfc Ctrl
  if (Setup_Config->CbsCmnMemTimingTrfcCtrlDdr4 != 0) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrfcCtrlDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Trfc
  if (ApcbParaValue != NULL) {
    * ((UINT16 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrfcDdr4;
    ApcbParaValue += sizeof (UINT16);
  }
  _Size += sizeof (UINT16);

  //Trfc2 Ctrl
  if (Setup_Config->CbsCmnMemTimingTrfc2CtrlDdr4 != 0) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrfc2CtrlDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Trfc2
  if (ApcbParaValue != NULL) {
    * ((UINT16 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrfc2Ddr4;
    ApcbParaValue += sizeof (UINT16);
  }
  _Size += sizeof (UINT16);

  //Trfc4 Ctrl
  if (Setup_Config->CbsCmnMemTimingTrfc4CtrlDdr4 != 0) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrfc4CtrlDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Trfc4
  if (ApcbParaValue != NULL) {
    * ((UINT16 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrfc4Ddr4;
    ApcbParaValue += sizeof (UINT16);
  }
  _Size += sizeof (UINT16);

  //ProcODT
  if (Setup_Config->CbsCmnMemCtrllerProcOdtDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemCtrllerProcOdtDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Tcwl
  if (Setup_Config->CbsCmnMemTimingTcwlDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTcwlDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Trtp
  if (Setup_Config->CbsCmnMemTimingTrtpDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrtpDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Trdwr
  if (Setup_Config->CbsCmnMemTimingTrdwrDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrdwrDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Twrrd
  if (Setup_Config->CbsCmnMemTimingTwrrdDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTwrrdDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //TwrwrSc
  if (Setup_Config->CbsCmnMemTimingTwrwrScDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTwrwrScDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //TwrwrSd
  if (Setup_Config->CbsCmnMemTimingTwrwrSdDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTwrwrSdDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //TwrwrDd
  if (Setup_Config->CbsCmnMemTimingTwrwrDdDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTwrwrDdDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //TrdrdSc
  if (Setup_Config->CbsCmnMemTimingTrdrdScDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrdrdScDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //TrdrdSd
  if (Setup_Config->CbsCmnMemTimingTrdrdSdDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrdrdSdDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //TrdrdDd
  if (Setup_Config->CbsCmnMemTimingTrdrdDdDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTrdrdDdDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Tcke
  if (Setup_Config->CbsCmnMemTimingTckeDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTimingTckeDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Cmd2T
  if (Setup_Config->CbsCmnMemCtrller2TModeDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemCtrller2TModeDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Gear Down Mode
  if (Setup_Config->CbsCmnMemGearDownModeDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemGearDownModeDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Power Down Enable
  if (Setup_Config->CbsCmnMemCtrllerPwrDnEnDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemCtrllerPwrDnEnDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Data Bus Configuration User Controls
  if (Setup_Config->CbsCmnMemDataBusConfigCtlDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemDataBusConfigCtlDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //RttNom
  if (Setup_Config->CbsCmnMemCtrllerRttNomDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemCtrllerRttNomDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //RttWr
  if (Setup_Config->CbsCmnMemCtrllerRttWrDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemCtrllerRttWrDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //RttPark
  if (Setup_Config->CbsCmnMemCtrllerRttParkDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemCtrllerRttParkDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Data Poisoning
  if (Setup_Config->CbsCmnMemDataPoisoningDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemDataPoisoningDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //DRAM ECC Symbol Size
  if (Setup_Config->CbsCmnMemCtrllerDramEccSymbolSizeDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemCtrllerDramEccSymbolSizeDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //DRAM ECC Enable
  if (Setup_Config->CbsCmnMemCtrllerDramEccEnDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemCtrllerDramEccEnDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //TSME
  if (Setup_Config->CbsCmnMemTsmeDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemTsmeDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Data Scramble
  if (Setup_Config->CbsCmnMemCtrllerDataScrambleDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemCtrllerDataScrambleDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Chipselect Interleaving
  if (Setup_Config->CbsCmnMemMappingBankInterleaveDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemMappingBankInterleaveDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //BankGroupSwap
  if (Setup_Config->CbsCmnMemCtrllerBankGroupSwapDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemCtrllerBankGroupSwapDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //BankGroupSwapAlt
  if (Setup_Config->CbsCmnMemCtrllerBankGroupSwapAltDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemCtrllerBankGroupSwapAltDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Address Hash Bank
  if (Setup_Config->CbsCmnMemAddressHashBankDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemAddressHashBankDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //Address Hash CS
  if (Setup_Config->CbsCmnMemAddressHashCsDdr4 != 0xFF) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemAddressHashCsDdr4;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //MBIST Enable
  if (ApcbParaValue != NULL) {
    * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemMbistEn;
    ApcbParaValue += sizeof (UINT8);
  }
  _Size += sizeof (UINT8);

  //MBIST Test Mode
  if (ApcbParaValue != NULL) {
    * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemMbistTestmode;
    ApcbParaValue += sizeof (UINT8);
  }
  _Size += sizeof (UINT8);

  //MBIST Aggressors
  if (Setup_Config->CbsCmnMemMbistAggressors != 0xff) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemMbistAggressors;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //MBIST Per Bit Slave Die Reporting
  if (Setup_Config->CbsCmnMemMbistPerBitSlaveDieReport != 0xff) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemMbistPerBitSlaveDieReport;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //CLDO_VDDP Control
  if (Setup_Config->CbsCmnCLDO_VDDPCtl != 0) {
    if (ApcbParaValue != NULL) {
      * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnCLDO_VDDPCtl;
      ApcbParaValue += sizeof (UINT8);
    }
    _Size += sizeof (UINT8);
  }

  //CLDO_VDDP voltage
  if (ApcbParaValue != NULL) {
    * ((UINT32 *) ApcbParaValue) = Setup_Config->CbsCmnCLDOVDDPvoltage;
    ApcbParaValue += sizeof (UINT32);
  }
  _Size += sizeof (UINT32);

  //End
  *Size = _Size;
}


VOID
PrepareDbgTypeAttrib (
  UINT32 *Size,
  APCB_PARAM_ATTRIBUTE *ApcbParaAttrib,
  VOID *CbsVariable
  )
{
  CBS_CONFIG *Setup_Config;
  UINT32     _Size;

  if (CbsVariable == NULL) {
    ASSERT (FALSE);
    return;
  }

  if (Size == NULL) {
    ASSERT (FALSE);
    return;
  }

  _Size = 0;
  Setup_Config = (CBS_CONFIG *) CbsVariable;

  //Auto Refersh Rate
  if (ApcbParaAttrib != NULL) {
    ApcbParaAttrib->TimePoint = APCB_TIME_POINT_ANY;
    ApcbParaAttrib->Token = APCB_TOKEN_CBS_DBG_MEM_FINE_GRAN_REF_DDR4;
    ApcbParaAttrib->Size = (sizeof (UINT8) -1);
    ApcbParaAttrib++;
  }
  _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  
  //End
  if (ApcbParaAttrib != NULL) {
    ApcbParaAttrib->TimePoint = 0;
    ApcbParaAttrib->Token = APCB_TOKEN_CBS_DBG_LIMIT;
    ApcbParaAttrib->Size = 0;
    ApcbParaAttrib->Reserved = 0;
  }

  _Size += sizeof (APCB_PARAM_ATTRIBUTE);
  *Size = _Size;
}

VOID
PrepareDbgTypeValue (
  UINT32 *Size,
  UINT8 *ApcbParaValue,
  VOID *CbsVariable
  )
{
  CBS_CONFIG *Setup_Config;
  UINT32     _Size;

  if (CbsVariable == NULL) {
    ASSERT (FALSE);
    return;
  }

  if (Size == NULL) {
    ASSERT (FALSE);
    return;
  }

  _Size = 0;
  Setup_Config = (CBS_CONFIG *) CbsVariable;

  //Auto Refresh Rate
  if (ApcbParaValue != NULL) {
    * ((UINT8 *) ApcbParaValue) = Setup_Config->CbsCmnMemCtrllerAutoRefreshRateDdr4;
    ApcbParaValue += sizeof (UINT8);
  }
  _Size += sizeof (UINT8);
  
  //End
  *Size = _Size;
}

