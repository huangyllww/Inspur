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

#include "HygonCbsVariable.h"
#include "IdsNvIdSAT.h"
#include "IdsNvIntIdSAT.h"
#include "IdsNvTable.h"

UINT32
GetIdsNvRecordsSize (
  );

BOOLEAN
PrepareIdsNvTable (
  IN       VOID *CbsVariable,
  IN OUT   VOID *IdsNvTable,
  IN OUT   UINT32 *IdsNvTableSize
  )
{
  IDS_NV_TABLE_HEADER  *IdsNvTblHdr;
  UINT8                *IdsNvRecord;
  CBS_CONFIG           *Setup_Config;

  // Check if IdsNvTableSize size satisfied
  if ((*IdsNvTableSize) <  sizeof (IDS_NV_TABLE_HEADER) + GetIdsNvRecordsSize ()) {
    *IdsNvTableSize = sizeof (IDS_NV_TABLE_HEADER) + GetIdsNvRecordsSize ();
    return FALSE;
  }

  Setup_Config = (CBS_CONFIG *)CbsVariable;

  // Fill the IDS_NV_TABLE_HEADER
  IdsNvTblHdr = IdsNvTable;
  IdsNvTblHdr->Signature = IDS_NV_TABLE_SIGNATURE; // $INV
  IdsNvTblHdr->Revision  = IDS_NV_TABLE_REV_1; // 0x00000001ul

  // Fill the IDS_NV_RECORD
  IdsNvRecord = &(((IDS_NV_TABLE *)IdsNvTable)->NvRecords);

  // Platform First Error Handling
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_PFEH;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnCpuPfeh;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Core Performance Boost
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_CPB;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnCpuCpb;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Enable IBS
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_EN_IBS;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnCpuEnIbs;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // CPU C-state Control
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_CSTATE_CTRL;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnCpuCstateCtrl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // SMT Mode Control
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_SMT_MODE_CTRL;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnSmtMode;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Opcache Control
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_OPCACHE_CTRL;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnCpuOpcacheCtrl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // OC Mode
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_OC_MODE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnCpuOcMode;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // CSV-ES ASID Space Limit
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_CSV_ASID_SPACE_LIMIT;
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Value = Setup_Config->CbsCmnCpuCsvAsidSpaceLimit;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  // Streaming Stores Control
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_STREAMING_STORES_CTRL;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnCpuStreamingStoresCtrl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Local APIC Mode Control
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_LAPIC_MODE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnCpuLApicMode;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Custom Pstate0
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CPU_PST_CUSTOM_P0;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCpuPstCustomP0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Frequency (MHz)
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Id = IDSNVID_CPU_COF_P0;
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Value = Setup_Config->CbsCpuCofP0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  // Voltage (uV)
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Id = IDSNVID_CPU_VOLTAGE_P0;
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Value = Setup_Config->CbsCpuVoltageP0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  // Pstate0 FID
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CPU_PST0_FID;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCpuPst0Fid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Pstate0 DID
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CPU_PST0_DID;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCpuPst0Did;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Pstate0 VID
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CPU_PST0_VID;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCpuPst0Vid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Custom Pstate1
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CPU_PST_CUSTOM_P1;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCpuPstCustomP1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Frequency (MHz)
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Id = IDSNVID_CPU_COF_P1;
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Value = Setup_Config->CbsCpuCofP1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  // Voltage (uV)
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Id = IDSNVID_CPU_VOLTAGE_P1;
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Value = Setup_Config->CbsCpuVoltageP1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  // Pstate1 FID
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CPU_PST1_FID;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCpuPst1Fid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Pstate1 DID
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CPU_PST1_DID;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCpuPst1Did;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Pstate1 VID
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CPU_PST1_VID;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCpuPst1Vid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Custom Pstate2
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CPU_PST_CUSTOM_P2;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCpuPstCustomP2;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Frequency (MHz)
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Id = IDSNVID_CPU_COF_P2;
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Value = Setup_Config->CbsCpuCofP2;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  // Voltage (uV)
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Id = IDSNVID_CPU_VOLTAGE_P2;
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Value = Setup_Config->CbsCpuVoltageP2;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  // Pstate2 FID
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CPU_PST2_FID;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCpuPst2Fid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Pstate2 DID
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CPU_PST2_DID;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCpuPst2Did;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Pstate2 VID
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CPU_PST2_VID;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCpuPst2Vid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Downcore control
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_GEN_DOWNCORE_CTRL;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnCpuGenDowncoreCtrl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // L1 Stream HW Prefetcher
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_L1_STREAM_HW_PREFETCHER;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnCpuL1StreamHwPrefetcher;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // L2 Stream HW Prefetcher
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_L2_STREAM_HW_PREFETCHER;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnCpuL2StreamHwPrefetcher;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // DRAM scrub time
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DF_CMN_DRAM_SCRUB_TIME;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDfCmnDramScrubTime;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Redirect scrubber control
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DF_CMN_REDIR_SCRUB_CTRL;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDfCmnRedirScrubCtrl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Disable DF sync flood propagation
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DF_CMN_SYNC_FLOOD_PROP;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDfCmnSyncFloodProp;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Freeze DF module queues on error
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DF_CMN_FREEZE_QUEUE_ERROR;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDfCmnFreezeQueueError;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // HMI encryption control
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DF_CMN_HMI_ENCRYPTION;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDfCmnHmiEncryption;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // xHMI encryption control
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DF_CMN_X_HMI_ENCRYPTION;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDfCmnXHmiEncryption;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // CC6 memory region encryption
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DF_CMN_CC6_MEM_ENCRYPTION;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDfCmnCc6MemEncryption;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Location of private memory regions
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DF_CMN_CC6_ALLOCATION_SCHEME;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDfCmnCc6AllocationScheme;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // System probe filter
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DF_CMN_SYS_PROBE_FILTER;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDfCmnSysProbeFilter;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Memory interleaving
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DF_CMN_MEM_INTLV;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDfCmnMemIntlv;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Memory interleaving size
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DF_CMN_MEM_INTLV_SIZE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDfCmnMemIntlvSize;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Channel interleaving hash
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DF_CMN_CHNL_INTLV_HASH;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDfCmnChnlIntlvHash;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Memory Clear
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DF_CMN_MEM_CLEAR;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDfCmnMemClear;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // DF C-state Control
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_DF_CSTATE_CTRL;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDfCmnCstateCtrl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Overclock
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_OVERCLOCK_DDR5;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemOverclockDdr5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Memory Clock Speed
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_SPEED_DDR5;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemSpeedDdr5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Cmd2T
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER2_T_MODE_DDR5;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrller2TModeDdr5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Gear Down Mode
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_GEAR_DOWN_MODE_DDR5;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemGearDownModeDdr5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Power Down Enable
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_PWR_DN_EN_DDR5;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerPwrDnEnDdr5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Data Bus Configuration User Controls
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_DATA_BUS_CONFIG_CTL_DDR5;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemDataBusConfigCtlDdr5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //RttNomRD
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_RTT_NOM_DDR5;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerRttNomRDDdr5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // RttWr
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_RTT_WR_DDR5;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerRttWrDdr5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // RttPark
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_RTT_PARK_DDR5;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerRttParkDdr5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Data Poisoning
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_DATA_POISONING_DDR5;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemDataPoisoningDdr5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // DRAM ECC Symbol Size
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_DRAM_ECC_SYMBOL_SIZE_DDR5;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerDramEccSymbolSizeDdr5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // DRAM ECC Enable
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_DRAM_ECC_EN_DDR5;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerDramEccEnDdr5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // TSME
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_TSME_DDR5;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemTsmeDdr5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Data Scramble
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_DATA_SCRAMBLE_DDR5;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerDataScrambleDdr5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Chipselect Interleaving
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_MAPPING_BANK_INTERLEAVE_DDR5;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemMappingBankInterleaveDdr5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // BankGroupSwap
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_DDR5;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemCtrllerBankGroupSwapDdr5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Address Hash Bank
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_ADDRESS_HASH_BANK_DDR5;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemAddressHashBankDdr5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Address Hash CS
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_ADDRESS_HASH_CS_DDR5;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemAddressHashCsDdr5;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // MBIST Enable
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_MBIST_EN;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemMbistEn;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // MBIST Test Mode
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_MBIST_TESTMODE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemMbistTestmode;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // MBIST Aggressors
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_MBIST_AGGRESSORS;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemMbistAggressors;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // MBIST Per Bit Slave Die Reporting
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnMemMbistPerBitSlaveDieReport;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // NBIO Internal Poison Consumption
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DBG_POISON_CONSUMPTION;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDbgPoisonConsumption;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // NBIO RAS Control
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_NBIO_RAS_CONTROL;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsNbioRASControl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // cTDP Control
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMNC_TDP_CTL;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmncTDPCtl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // cTDP
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Id = IDSNVID_CMNC_TDP_LIMIT;
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
  ((IDS_NV_RECORD_U32 *)IdsNvRecord)->Value = Setup_Config->CbsCmncTDPLimit;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U32);

  // TDP Boost
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_TDP_BOOST;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnTDPBoost;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Efficiency Optimized Mode
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_NBIO_EFFICIENCY_OPTIMIZED_MODE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnNbioEfficiencyOptimizedMode;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // PSI
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_NBIO_PSI_DISABLE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnNbioPSIDisable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // ACS Enable
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DBG_GNB_DBG_ACS_ENABLE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDbgGnbDbgACSEnable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // PCIe ARI Support
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_GNB_DBG_PCIE_ARI_SUPPORT;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsGnbDbgPcieAriSupport;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // HD Audio Enable
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_GNB_HD_AUDIO_EN;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnGnbHdAudioEn;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Block PCIe Loopback
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CFG_PCIE_LOOPBACK_MODE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCfgPcieLoopbackMode;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // CRS Delay
  ((IDS_NV_RECORD_U16 *)IdsNvRecord)->Id = IDSNVID_CFG_PCIE_CRS_DELAY;
  ((IDS_NV_RECORD_U16 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_WORD;
  ((IDS_NV_RECORD_U16 *)IdsNvRecord)->Value = Setup_Config->CbsCfgPcieCrsDelay;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U16);

  // CRS Limit
  ((IDS_NV_RECORD_U16 *)IdsNvRecord)->Id = IDSNVID_CFG_PCIE_CRS_LIMIT;
  ((IDS_NV_RECORD_U16 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_WORD;
  ((IDS_NV_RECORD_U16 *)IdsNvRecord)->Value = Setup_Config->CbsCfgPcieCrsLimit;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U16);

  // IOMMU
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_GNB_NB_IOMMU;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnGnbNbIOMMU;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // xHMI Link Width
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_XHMI_LINK_WIDTH;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsXhmiLinkWidth;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);
  
  // PCIe ASPM Control
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_PCIE_ASPM_CONTROL;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsPcieAspmControl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);
  
  // PCIe Eq Mode
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_PCIE_EQ_MODE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsPcieEqMode;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // TCDX Routing Method
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_TCDX_ROUTING_METHOD;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsTcdxRouting;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Ignore sideband
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_DISABLE_SIDEBAND;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnDisableSideband;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Disable L1 w/a
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_DISABLE_L1WA;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnDisableL1wa;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Disable BridgeDis
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_DISABLE_BRIDGE_DIS;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnDisableBridgeDis;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Disable irq polling
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_DISABLE_IRQ_POLL;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnDisableIrqPoll;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // IRQ sets BridgeDis
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_IRQ_SETS_BRIDGE_DIS;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnIrqSetsBridgeDis;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // PCIE hotplug support
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CBS_PCIE_HOTPLUG_SUPPORT;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsPcieHotplugSupport;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // DJ0 SATA Controller 0
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_SATA_ENABLE0;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchSataEnable0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);
  //SATA Controller 1
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_SATA_ENABLE1;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchSataEnable1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);
  //SATA Controller 2
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_SATA_ENABLE2;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchSataEnable2;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);
  //SATA Controller 3
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_SATA_ENABLE3;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchSataEnable3;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);
  
  // SATA Mode
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_SATA_CLASS;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchSataClass;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Sata RAS Support
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_SATA_RAS_SUPPORT;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchSataRasSupport;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Sata Disabled AHCI Prefetch Function
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_SATA_AHCI_DIS_PREFETCH_FUNCTION;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchSataAhciDisPrefetchFunction;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Aggresive SATA Device Sleep Port 0
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P0;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDbgFchSataAggresiveDevSlpP0;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // DevSleep0 Port Number
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DBG_FCH_SATA_DEV_SLP_PORT0_NUM;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDbgFchSataDevSlpPort0Num;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Aggresive SATA Device Sleep Port 1
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P1;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDbgFchSataAggresiveDevSlpP1;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // DevSleep1 Port Number
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DBG_FCH_SATA_DEV_SLP_PORT1_NUM;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsDbgFchSataDevSlpPort1Num;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //XHCI0 controller enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_USB_XHC_I0_ENABLE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUsbXHCI0Enable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //XHCI1 controller enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_USB_XHC_I1_ENABLE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUsbXHCI1Enable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //XHCI2 controller enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_USB_XHC_I2_ENABLE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUsbXHCI2Enable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //XHCI3 controller enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_USB_XHC_I3_ENABLE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUsbXHCI3Enable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //XHCI4 controller enable
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_USB_XHC_I4_ENABLE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUsbXHCI4Enable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //XHCI5 Controller enable 
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_USB_XHC_I5_ENABLE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUsbXHCI5Enable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //XHCI6 controller enable 
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_USB_XHC_I6_ENABLE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUsbXHCI6Enable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  //XHCI7 controller enable 
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_FCH_USB_XHC_I7_ENABLE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsCmnFchUsbXHCI7Enable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Ac Loss Control
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_SYSTEM_PWR_FAIL_SHADOW;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchSystemPwrFailShadow;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // I2C 0 Enable
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_I2_C0_CONFIG;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchI2C0Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // I2C 1 Enable
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_I2_C1_CONFIG;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchI2C1Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // I2C 2 Enable
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_I2_C2_CONFIG;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchI2C2Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // I2C 3 Enable
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_I2_C3_CONFIG;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchI2C3Config;
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

  // I3C 0 Enable
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_I3_C0_CONFIG;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchI3C0Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // I3C 1 Enable
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_I3_C1_CONFIG;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchI3C1Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Uart 0 Enable
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_UART0_CONFIG;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchUart0Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Uart 0 Legacy Options
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_UART0_LEGACY_CONFIG;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchUart0LegacyConfig;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Uart 1 Enable
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_UART1_CONFIG;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchUart1Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Uart 1 Legacy Options
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_UART1_LEGACY_CONFIG;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchUart1LegacyConfig;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Uart 2 Enable (no HW FC)
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_UART2_CONFIG;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchUart2Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Uart 2 Legacy Options
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_UART2_LEGACY_CONFIG;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchUart2LegacyConfig;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Uart 3 Enable (no HW FC)
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_UART3_CONFIG;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchUart3Config;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Uart 3 Legacy Options
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_FCH_UART3_LEGACY_CONFIG;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnFchUart3LegacyConfig;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // NTB Enable
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_NTB_ENABLE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnNtbEnable;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // NTB Location
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_NTB_LOCATION;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnNtbLocation;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // NTB active on PCIeCore
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_NTB_PCIE_CORE_INDEX;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnNtbPCIeCore;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // NTB Mode
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_NTB_MODE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnNtbMode;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Link Speed
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_NTB_LINK_SPEED;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnNtbLinkSpeed;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // BAR1 size
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DBG_S_P3_NTB_BAR1_SIZE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnNtbBAR1Window;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // BAR23 size
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DBG_S_P3_NTB_BAR23_SIZE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnNtbBAR23Window;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // BAR45 size
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_DBG_S_P3_NTB_BAR45_SIZE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnNtbBAR45Window;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_RDSEED_RDRAND_CTRL;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnCpuRdseedRdrandCtrl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_LOAD_UCODE_CONTROL;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnCpuLoadUcodeCtrl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_FORCE_VDDCR_CPU_VID_EN;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnCpuForceVddcrCpuVidEn;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  ((IDS_NV_RECORD_U16 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_FORCE_VDDCR_CPU_VID;
  ((IDS_NV_RECORD_U16 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_WORD;
  ((IDS_NV_RECORD_U16 *)IdsNvRecord)->Value = Setup_Config->CbsCmnCpuForceVddcrCpuVid;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U16);

  ((IDS_NV_RECORD_U16 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_VDDCR_CPU_VOLTAGE_MARGIN;
  ((IDS_NV_RECORD_U16 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_WORD;
  ((IDS_NV_RECORD_U16 *)IdsNvRecord)->Value = Setup_Config->CbsCmnVddcrCpuVoltageMargin;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U16);

  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_FORCE_CCLK_FREQUENCY_EN;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnForceCclkFrequencyEn;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  ((IDS_NV_RECORD_U16 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_FORCE_CCLK_FREQUENCY;
  ((IDS_NV_RECORD_U16 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_WORD;
  ((IDS_NV_RECORD_U16 *)IdsNvRecord)->Value = Setup_Config->CbsCmnForceCclkFrequency;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U16);

  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_DLDO_BYPASS;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnDldoBypass;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_DISPLAY_RDRAND_RDSEED;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnCpuRdseedRdrandCap;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Enable or Disable SMEE
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_SMEE_CONTROL;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnSmeeCtrl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);
  // SMEE CAP
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_SMEE_CAP_CONTROL;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnCpuSmeeCap;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);
  
  //Above 4G MMIO limit bit
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IDSNVID_CMN_CPU_ABOVE4G_MMIO_LIMIT_BIT;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = Setup_Config->CbsAbove4GMmioLimitBit;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8); 

  // Enable or Disable SVM
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CMN_CPU_SVM_CONTROL;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsCmnSVMCtrl;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Validation - DDR Parity
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CBS_VALIDATION_DDR_PATRITY;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsValidationDdrParity;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Validation - PSPCCP VQ Count
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Id = IDSNVID_CBS_VALIDATION_PSPCCP_VQ_COUNT;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
  ((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value = Setup_Config->CbsValidationPspCcpVqCount;
  IdsNvRecord += sizeof (IDS_NV_RECORD_U8);

  // Fill the end of IDS_NV_RECORD
  ((IDS_NV_RECORD_CMN *)IdsNvRecord)->Id = IDS_NVID_END;

  return TRUE;
}

#define IDS_NV_RECORDS_SIZE  (1086)

UINT32
GetIdsNvRecordsSize (
  )
{
  return IDS_NV_RECORDS_SIZE;
}
