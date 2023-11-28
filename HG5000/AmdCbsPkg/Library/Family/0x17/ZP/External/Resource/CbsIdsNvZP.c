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

#include "AmdCbsVariable.h"
#include "IdsNvIdZP.h"
#include "IdsNvIntIdZP.h"
#include "IdsNvTable.h"
#include <Library/PcdLib.h>

UINT32 GetIdsNvRecordsSize ();

BOOLEAN
PrepareIdsNvTable (
  IN       VOID *CbsVariable,
  IN OUT   VOID *IdsNvTable,
  IN OUT   UINT32 *IdsNvTableSize
  )
{
  IDS_NV_TABLE_HEADER *IdsNvTblHdr;
  UINT8 *IdsNvRecord;
  CBS_CONFIG *Setup_Config;

  //Check if IdsNvTableSize size satisfied
  if ((*IdsNvTableSize) <  sizeof (IDS_NV_TABLE_HEADER) + GetIdsNvRecordsSize () ) {
    *IdsNvTableSize = sizeof (IDS_NV_TABLE_HEADER) + GetIdsNvRecordsSize ();
    return FALSE;
  }

  Setup_Config = (CBS_CONFIG *) CbsVariable;

  //Fill the IDS_NV_TABLE_HEADER
  IdsNvTblHdr = IdsNvTable;
  IdsNvTblHdr->Signature = IDS_NV_TABLE_SIGNATURE; //$INV
  IdsNvTblHdr->Revision = IDS_NV_TABLE_REV_1; //0x00000001ul

  //Fill the IDS_NV_RECORD
  IdsNvRecord = &(((IDS_NV_TABLE *)IdsNvTable)->NvRecords);

  //RedirectForReturnDis
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_GEN_W_A05;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCpuGenWA05;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //L2 TLB Associativity
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_L2_TLB_WAY_ALLOC;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCpuL2TlbWayAlloc;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Platform First Error Handling
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_PFEH;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCpuPfeh;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Core Performance Boost
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_CPB;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCpuCpb;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Enable IBS
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_EN_IBS;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCpuEnIbs;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Global C-state Control
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_GLOBAL_CSTATE_CTRL;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCpuGlobalCstateCtrl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Opcache Control
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_OPCACHE_CTRL;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCpuOpcacheCtrl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //OC Mode
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_OC_MODE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCpuOcMode;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //SEV-ES ASID Space Limit
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_SEV_ASID_SPACE_LIMIT;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCpuSevAsidSpaceLimit;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //Streaming Stores Control
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_STREAMING_STORES_CTRL;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCpuStreamingStoresCtrl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Custom Core Pstates Legal Disclaimer
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_LEGAL_DISCLAIMER;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuLegalDisclaimer;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Custom Core Pstates Legal Disclaimer 1
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_LEGAL_DISCLAIMER1;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuLegalDisclaimer1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Custom Pstate0
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST_CUSTOM_P0;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPstCustomP0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Frequency (MHz)
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CPU_COF_P0;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCpuCofP0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //  Voltage (uV)
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CPU_VOLTAGE_P0;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCpuVoltageP0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //  Pstate0 FID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST0_FID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst0Fid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Pstate0 DID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST0_DID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst0Did;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Pstate0 VID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST0_VID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst0Vid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Custom Pstate1
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST_CUSTOM_P1;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPstCustomP1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Frequency (MHz)
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CPU_COF_P1;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCpuCofP1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //  Voltage (uV)
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CPU_VOLTAGE_P1;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCpuVoltageP1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //  Pstate1 FID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST1_FID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst1Fid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Pstate1 DID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST1_DID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst1Did;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Pstate1 VID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST1_VID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst1Vid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Custom Pstate2
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST_CUSTOM_P2;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPstCustomP2;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Frequency (MHz)
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CPU_COF_P2;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCpuCofP2;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //  Voltage (uV)
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CPU_VOLTAGE_P2;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCpuVoltageP2;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //  Pstate2 FID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST2_FID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst2Fid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Pstate2 DID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST2_DID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst2Did;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Pstate2 VID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST2_VID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst2Vid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Custom Pstate3
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST_CUSTOM_P3;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPstCustomP3;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Frequency (MHz)
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CPU_COF_P3;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCpuCofP3;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //  Voltage (uV)
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CPU_VOLTAGE_P3;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCpuVoltageP3;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //  Pstate3 FID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST3_FID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst3Fid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Pstate3 DID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST3_DID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst3Did;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Pstate3 VID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST3_VID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst3Vid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Custom Pstate4
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST_CUSTOM_P4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPstCustomP4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Frequency (MHz)
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CPU_COF_P4;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCpuCofP4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //  Voltage (uV)
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CPU_VOLTAGE_P4;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCpuVoltageP4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //  Pstate4 FID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST4_FID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst4Fid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Pstate4 DID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST4_DID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst4Did;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Pstate4 VID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST4_VID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst4Vid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Custom Pstate5
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST_CUSTOM_P5;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPstCustomP5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Frequency (MHz)
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CPU_COF_P5;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCpuCofP5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //  Voltage (uV)
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CPU_VOLTAGE_P5;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCpuVoltageP5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //  Pstate5 FID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST5_FID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst5Fid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Pstate5 DID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST5_DID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst5Did;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Pstate5 VID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST5_VID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst5Vid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Custom Pstate6
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST_CUSTOM_P6;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPstCustomP6;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Frequency (MHz)
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CPU_COF_P6;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCpuCofP6;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //  Voltage (uV)
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CPU_VOLTAGE_P6;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCpuVoltageP6;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //  Pstate6 FID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST6_FID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst6Fid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Pstate6 DID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST6_DID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst6Did;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Pstate6 VID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST6_VID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst6Vid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Custom Pstate7
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST_CUSTOM_P7;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPstCustomP7;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Frequency (MHz)
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CPU_COF_P7;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCpuCofP7;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //  Voltage (uV)
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CPU_VOLTAGE_P7;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCpuVoltageP7;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //  Pstate7 FID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST7_FID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst7Fid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Pstate7 DID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST7_DID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst7Did;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Pstate7 VID
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_PST7_VID;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuPst7Vid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Core/Thread Enablement Legal Disclaimer
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CPU_CORE_THREAD_LEGAL_DISCLAIMER;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCpuCoreThreadLegalDisclaimer;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Downcore control
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_GEN_DOWNCORE_CTRL;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCpuGenDowncoreCtrl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //L1 Stream HW Prefetcher
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_L1_STREAM_HW_PREFETCHER;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCpuL1StreamHwPrefetcher;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //L2 Stream HW Prefetcher
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_L2_STREAM_HW_PREFETCHER;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCpuL2StreamHwPrefetcher;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //DRAM scrub time
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DF_CMN_DRAM_SCRUB_TIME;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDfCmnDramScrubTime;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Redirect scrubber control
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DF_CMN_REDIR_SCRUB_CTRL;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDfCmnRedirScrubCtrl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Disable DF sync flood propagation
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DF_CMN_SYNC_FLOOD_PROP;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDfCmnSyncFloodProp;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Freeze DF module queues on error
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DF_CMN_FREEZE_QUEUE_ERROR;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDfCmnFreezeQueueError;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //GMI encryption control
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DF_CMN_GMI_ENCRYPTION;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDfCmnGmiEncryption;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //xGMI encryption control
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DF_CMN_X_GMI_ENCRYPTION;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDfCmnXGmiEncryption;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //CC6 memory region encryption
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DF_CMN_CC6_MEM_ENCRYPTION;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDfCmnCc6MemEncryption;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Location of private memory regions
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DF_CMN_CC6_ALLOCATION_SCHEME;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDfCmnCc6AllocationScheme;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //System probe filter
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DF_CMN_SYS_PROBE_FILTER;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDfCmnSysProbeFilter;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Memory interleaving
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DF_CMN_MEM_INTLV;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDfCmnMemIntlv;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Memory interleaving size
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DF_CMN_MEM_INTLV_SIZE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDfCmnMemIntlvSize;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Channel interleaving hash
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DF_CMN_CHNL_INTLV_HASH;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDfCmnChnlIntlvHash;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Memory Clear
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DF_CMN_MEM_CLEAR;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDfCmnMemClear;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //DRAM Timing Configuration Legal Disclaimer
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_LEGAL_DISCLAIMER;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingLegalDisclaimer;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //DRAM Timing Configuration Legal Disclaimer 1
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_LEGAL_DISCLAIMER1;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingLegalDisclaimer1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Overclock
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_OVERCLOCK_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemOverclockDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Memory Clock Speed
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_SPEED_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemSpeedDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Tcl
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TCL_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTclDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Trcdrd
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRCDRD_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrcdrdDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Trcdwr
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRCDWR_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrcdwrDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Trp
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRP_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrpDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Tras
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRAS_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrasDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Trc Ctrl
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRC_CTRL_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrcCtrlDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Trc
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRC_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrcDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //TrrdS
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRRD_S_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrrdSDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //TrrdL
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRRD_L_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrrdLDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Tfaw Ctrl
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TFAW_CTRL_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTfawCtrlDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Tfaw
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TFAW_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTfawDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //TwtrS
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TWTR_S_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTwtrSDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //TwtrL
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TWTR_L_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTwtrLDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Twr Ctrl
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TWR_CTRL_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTwrCtrlDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Twr
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TWR_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTwrDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Trcpage Ctrl
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRCPAGE_CTRL_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrcpageCtrlDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Trcpage
  ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRCPAGE_DDR4;
  ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_WORD;
  ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrcpageDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U16);

  //TrdrdScL Ctrl
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRDRD_SC_L_CTRL_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrdrdScLCtrlDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //TrdrdScL
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRDRD_SC_L_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrdrdScLDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //TwrwrScL Ctrl
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TWRWR_SC_L_CTRL_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTwrwrScLCtrlDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //TwrwrScL
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TWRWR_SC_L_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTwrwrScLDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Trfc Ctrl
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRFC_CTRL_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrfcCtrlDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Trfc
  ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRFC_DDR4;
  ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_WORD;
  ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrfcDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U16);

  //Trfc2 Ctrl
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRFC2_CTRL_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrfc2CtrlDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Trfc2
  ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRFC2_DDR4;
  ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_WORD;
  ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrfc2Ddr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U16);

  //Trfc4 Ctrl
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRFC4_CTRL_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrfc4CtrlDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Trfc4
  ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRFC4_DDR4;
  ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_WORD;
  ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrfc4Ddr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U16);

  //ProcODT
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_PROC_ODT_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerProcOdtDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Tcwl
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TCWL_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTcwlDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Trtp
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRTP_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrtpDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Trdwr
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRDWR_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrdwrDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Twrrd
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TWRRD_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTwrrdDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //TwrwrSc
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TWRWR_SC_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTwrwrScDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //TwrwrSd
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TWRWR_SD_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTwrwrSdDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //TwrwrDd
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TWRWR_DD_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTwrwrDdDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //TrdrdSc
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRDRD_SC_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrdrdScDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //TrdrdSd
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRDRD_SD_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrdrdSdDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //TrdrdDd
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TRDRD_DD_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTrdrdDdDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Tcke
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TIMING_TCKE_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTimingTckeDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Cmd2T
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER2_T_MODE_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrller2TModeDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Gear Down Mode
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_GEAR_DOWN_MODE_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemGearDownModeDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Specifies the DDR4 refresh mode.
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_AUTO_REFRESH_RATE_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerAutoRefreshRateDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);
  
  //Power Down Enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_PWR_DN_EN_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerPwrDnEnDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Data Bus Configuration User Controls
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_DATA_BUS_CONFIG_CTL_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemDataBusConfigCtlDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //RttNom
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_RTT_NOM_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerRttNomDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //RttWr
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_RTT_WR_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerRttWrDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //RttPark
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_RTT_PARK_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerRttParkDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Data Poisoning
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_DATA_POISONING_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemDataPoisoningDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //DRAM ECC Symbol Size
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_DRAM_ECC_SYMBOL_SIZE_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerDramEccSymbolSizeDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //DRAM ECC Enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_DRAM_ECC_EN_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerDramEccEnDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //TSME
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_TSME_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemTsmeDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Data Scramble
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_DATA_SCRAMBLE_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerDataScrambleDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Chipselect Interleaving
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_MAPPING_BANK_INTERLEAVE_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemMappingBankInterleaveDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //BankGroupSwap
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerBankGroupSwapDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //BankGroupSwapAlt
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_ALT_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerBankGroupSwapAltDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Address Hash Bank
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_ADDRESS_HASH_BANK_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemAddressHashBankDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Address Hash CS
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_ADDRESS_HASH_CS_DDR4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemAddressHashCsDdr4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //MBIST Enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_MBIST_EN;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemMbistEn;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //MBIST Test Mode
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_MBIST_TESTMODE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemMbistTestmode;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //MBIST Aggressors
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_MBIST_AGGRESSORS;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemMbistAggressors;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //MBIST Per Bit Slave Die Reporting
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemMbistPerBitSlaveDieReport;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //System Configuration
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_GNB_SMU_SYSTEM_CONFIG;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnGnbSMUSystemConfig;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //NBIO Internal Poison Consumption
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_POISON_CONSUMPTION;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPoisonConsumption;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //NBIO RAS Control
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_RAS_CONTROL;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgRASControl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Determinism Slider
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_DETERMINISM_SLIDER;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnDeterminismSlider;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //cTDP Control
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMNC_TDP_CTL;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmncTDPCtl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //cTDP
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CMNC_TDP_LIMIT;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCmncTDPLimit;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //Efficiency Optimized Mode
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_NBIO_EFFICIENCY_OPTIMIZED_MODE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnNbioEfficiencyOptimizedMode;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PSI
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_NBIO_PSI_DISABLE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnNbioPSIDisable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //ACS Enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_GNB_DBG_ACS_ENABLE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgGnbDbgACSEnable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PCIe ARI Support
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_GNB_DBG_PCIE_ARI_SUPPORT;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsGnbDbgPcieAriSupport;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //CLDO_VDDP Control
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CLDO_VDDP_CTL;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCLDO_VDDPCtl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //CLDO_VDDP voltage
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IDSNVID_CMN_CLDOVDD_PVOLTAGE;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCLDOVDDPvoltage;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  //HD Audio Enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_GNB_HD_AUDIO_EN;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnGnbHdAudioEn;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Block PCIe Loopback
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CFG_PCIE_LOOPBACK_MODE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCfgPcieLoopbackMode;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //CRS Delay
  ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Id = IDSNVID_CFG_PCIE_CRS_DELAY;
  ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_WORD;
  ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Value = Setup_Config->CbsCfgPcieCrsDelay;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U16);

  //CRS Limit
  ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Id = IDSNVID_CFG_PCIE_CRS_LIMIT;
  ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_WORD;
  ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Value = Setup_Config->CbsCfgPcieCrsLimit;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U16);

  //IOMMU
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_GNB_NB_IOMMU;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = PcdGet8(PcdAmdNbIOMMU);
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Concurrent Training
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_PCIE_SYNC_RESET;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsPcieSyncReset;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Fan Control
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_FAN_CTL;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgFanCtl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Force PWM Control
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_FORCE_PWM_CTL;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgForcePwmCtl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Force PWM
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_FORCE_PWM;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgForcePwm;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Fan Table Control
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_FAN_TABLE_CTL;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgFanTableCtl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Low Temperature
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_FAN_TABLE_TEMP_LOW;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgFanTableTempLow;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Medium Temperature
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_FAN_TABLE_TEMP_MED;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgFanTableTempMed;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //High Temperature
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_FAN_TABLE_TEMP_HIGH;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgFanTableTempHigh;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Critical Temperature
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_FAN_TABLE_TEMP_CRITICAL;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgFanTableTempCritical;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Low Pwm
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_FAN_TABLE_T_PWM_LOW;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgFanTableTPwmLow;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Medium Pwm
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_FAN_TABLE_PWM_MED;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgFanTablePwmMed;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //High Pwm
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_FAN_TABLE_PWM_HIGH;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgFanTablePwmHigh;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Temperature Hysteresis
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_FAN_TABLE_HYST;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgFanTableHyst;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Pwm Frequency
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_FAN_TABLE_PWM_FREQ;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgFanTablePwmFreq;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Fan Polarity
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_FAN_POLARITY;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgFanPolarity;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Ignore sideband
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_DISABLE_SIDEBAND;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnDisableSideband;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Disable L1 w/a
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_DISABLE_L1WA;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnDisableL1wa;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Disable BridgeDis
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_DISABLE_BRIDGE_DIS;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnDisableBridgeDis;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Disable irq polling
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_DISABLE_IRQ_POLL;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnDisableIrqPoll;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //IRQ sets BridgeDis
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_IRQ_SETS_BRIDGE_DIS;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnIrqSetsBridgeDis;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);
  
  //PCIE hotplug support
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CBS_PCIE_HOTPLUG_SUPPORT;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsPcieHotplugSupport;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //SATA Controller
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_SATA_ENABLE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchSataEnable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //SATA Mode
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_SATA_CLASS;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchSataClass;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Sata RAS Support
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_SATA_RAS_SUPPORT;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchSataRasSupport;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Sata Disabled AHCI Prefetch Function
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_SATA_AHCI_DIS_PREFETCH_FUNCTION;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchSataAhciDisPrefetchFunction;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Aggresive SATA Device Sleep Port 0
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P0;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgFchSataAggresiveDevSlpP0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //DevSleep0 Port Number
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_FCH_SATA_DEV_SLP_PORT0_NUM;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgFchSataDevSlpPort0Num;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Aggresive SATA Device Sleep Port 1
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P1;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgFchSataAggresiveDevSlpP1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //DevSleep1 Port Number
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_FCH_SATA_DEV_SLP_PORT1_NUM;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgFchSataDevSlpPort1Num;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //XHCI controller enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_USB_XHC_I0_ENABLE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUsbXHCI0Enable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //XHCI Controller1 enable (Die1)
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_USB_XHC_I1_ENABLE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUsbXHCI1Enable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //XHCI2 enable (MCM1/Die0)
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_USB_XHC_I2_ENABLE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUsbXHCI2Enable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //XHCI3 enable (MCM1/Die1)
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_USB_XHC_I3_ENABLE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUsbXHCI3Enable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //SD Configuration Mode
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_SD_CONFIG;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchSdConfig;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Ac Loss Control
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_SYSTEM_PWR_FAIL_SHADOW;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchSystemPwrFailShadow;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //I2C 0 Enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_I2_C0_CONFIG;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchI2C0Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //I2C 1 Enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_I2_C1_CONFIG;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchI2C1Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //I2C 2 Enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_I2_C2_CONFIG;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchI2C2Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //I2C 3 Enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_I2_C3_CONFIG;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchI2C3Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //I2C 4 Enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_I2_C4_CONFIG;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchI2C4Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //I2C 5 Enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_I2_C5_CONFIG;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchI2C5Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Uart 0 Enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_UART0_CONFIG;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUart0Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Uart 0 Legacy Options
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_UART0_LEGACY_CONFIG;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUart0LegacyConfig;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Uart 1 Enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_UART1_CONFIG;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUart1Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Uart 1 Legacy Options
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_UART1_LEGACY_CONFIG;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUart1LegacyConfig;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Uart 2 Enable (no HW FC)
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_UART2_CONFIG;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUart2Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Uart 2 Legacy Options
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_UART2_LEGACY_CONFIG;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUart2LegacyConfig;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Uart 3 Enable (no HW FC)
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_UART3_CONFIG;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUart3Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //Uart 3 Legacy Options
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_UART3_LEGACY_CONFIG;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUart3LegacyConfig;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //ESPI Enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_ESPI_CONFIG;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchEspiConfig;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT SATA Port Enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_PT_SATA_PORT_ENABLE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnPTSataPortEnable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT SATA Mode
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_PT_SATA_CLASS;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnPTSataClass;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT Aggresive SATA Device Sleep Port 0
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P0;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTSataAggresiveDevSlpP0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT Aggresive SATA Device Sleep Port 1
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P1;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTSataAggresiveDevSlpP1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT XHCI GEN1
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_XHCI_GEN1;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTXhciGen1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT XHCI GEN2
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_XHCI_GEN2;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTXhciGen2;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB Equalization4
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_USB_EQUALIZATION4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTUsbEqualization4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB Redriver
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_USB_REDRIVER;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTUsbRedriver;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB31 PORT0
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_USB31_P0;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTUsb31P0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB31 PORT1
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_USB31_P1;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTUsb31P1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB30 PORT0
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_USB30_P0;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTUsb30P0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB30 PORT1
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_USB30_P1;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTUsb30P1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB30 PORT2
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_USB30_P2;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTUsb30P2;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB30 PORT3
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_USB30_P3;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTUsb30P3;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB30 PORT4
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_USB30_P4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTUsb30P4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB30 PORT5
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_USB30_P5;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTUsb30P5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB20 PORT0
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_USB20_P0;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTUsb20P0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB20 PORT1
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_USB20_P1;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTUsb20P1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB20 PORT2
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_USB20_P2;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTUsb20P2;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB20 PORT3
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_USB20_P3;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTUsb20P3;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB20 PORT4
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_USB20_P4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTUsb20P4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB20 PORT5
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_USB20_P5;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTUsb20P5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB31 PORT0
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM2_USB31_P0;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm2Usb31P0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB31 PORT1
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM2_USB31_P1;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm2Usb31P1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB30 PORT0
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM2_USB30_P0;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm2Usb30P0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB30 PORT1
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM2_USB30_P1;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm2Usb30P1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB20 PORT0
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM2_USB20_P0;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm2Usb20P0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB20 PORT1
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM2_USB20_P1;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm2Usb20P1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB20 PORT2
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM2_USB20_P2;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm2Usb20P2;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB20 PORT3
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM2_USB20_P3;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm2Usb20P3;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB20 PORT4
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM2_USB20_P4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm2Usb20P4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB20 PORT5
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM2_USB20_P5;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm2Usb20P5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB31 PORT0
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM1_USB31_P0;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm1Usb31P0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB31 PORT1
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM1_USB31_P1;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm1Usb31P1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB30 PORT0
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM1_USB30_P0;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm1Usb30P0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB20 PORT0
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM1_USB20_P0;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm1Usb20P0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB20 PORT1
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM1_USB20_P1;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm1Usb20P1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB20 PORT2
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM1_USB20_P2;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm1Usb20P2;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB20 PORT3
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM1_USB20_P3;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm1Usb20P3;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB20 PORT4
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM1_USB20_P4;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm1Usb20P4;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //PT USB20 PORT5
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_PT_PROM1_USB20_P5;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgPTProm1Usb20P5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //NTB Enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_S_P3_NTB_ENABLE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgSP3NtbEnable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //NTB Location
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_S_P3_NTB_LOCATION;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgSP3NtbLocation;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //NTB active on PCIeCore
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_S_P3_NTB_PC_IE_CORE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgSP3NtbPCIeCore;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //NTB Mode
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_S_P3_NTB_MODE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgSP3NtbMode;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  Link Speed
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_S_P3_NTB_LINK_SPEED;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgSP3NtbLinkSpeed;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //  BAR1 size
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_S_P3_NTB_BAR1_SIZE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgSP3NtbBAR1Window;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);
  
  //  BAR23 size
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_S_P3_NTB_BAR23_SIZE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgSP3NtbBAR23Window;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);
  
  //  BAR45 size
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_DBG_S_P3_NTB_BAR45_SIZE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsDbgSP3NtbBAR45Window;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);
  
  //hzp20170921 Enables or disables CPU rdseed and rdrand feature, add start
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_RDSEED_RDRAND_CTRL;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCpuRdseedRdrandCtrl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);
  //hzp20170921 Enables or disables CPU rdseed and rdrand feature, add end
  
  //<PS-315+>
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_LOAD_UCODE_CONTROL;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCpuLoadUcodeCtrl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);
  //<PS-315->
  
   ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_AVAILABLE_VQ_COUNT;
   ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
   ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnAvailableVqCount;
   IdsNvRecord += sizeof (IDS_NV_RECORD_U8);
  //<PS-625+>
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_DISPLAY_RDRAND_RDSEED;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCpuRdseedRdrandCap;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);
  
  //Enable or Disable SMEE
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_SMEE_CONTROL;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnSmeeCtrl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);  
  //SMEE CAP
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_SMEE_CAP_CONTROL;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnCpuSmeeCap;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);  
  
  //Enable or Disable SVM
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_SVM_CONTROL;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = PcdGet8(PcdAmdSVMCtrl);
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);    
  
  //Fill the end of IDS_NV_RECORD
  ((IDS_NV_RECORD_CMN *) IdsNvRecord)->Id = IDS_NVID_END;

  return TRUE;
}

#define IDS_NV_RECORDS_SIZE (1086)

UINT32
GetIdsNvRecordsSize (
  )
{
  return IDS_NV_RECORDS_SIZE;
}
