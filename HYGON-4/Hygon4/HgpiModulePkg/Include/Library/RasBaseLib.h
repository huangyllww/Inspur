/*
*****************************************************************************
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
****************************************************************************
*/

#ifndef _RAS_BASE_LIB_H_
#define _Ras_BASE_LIB_H_

#include "HygonRas.h"

#define MYDIETYPELO      21
#define MYDIETYPEHI      22
#define SOCKETIDSHIFTLO  28
#define SOCKETIDSHIFTHI  31
#define SOCKETIDMASKLO   16
#define SOCKETIDMASKHI_HYEX 25
#define SOCKETIDMASKHI_HYGX 26
#define DIEIDSHIFTLO     12
#define DIEIDSHIFTHI     15
#define DIEIDMASKLO      0
#define DIEIDMASKHI_HYEX  9
#define DIEIDMASKHI_HYGX  10
#define BLKINSTCOUNTLO   0
#define BLKINSTCOUNTHI   7
#define BLOCKFABRICIDLO  8
#define BLOCKFABRICIDHI_HYEX 17
#define BLOCKFABRICIDHI_HYGX 18
#define HIADDROFFSETEN   0
#define HIADDROFFSETLO   18
#define HIADDROFFSETHI   31
#define ADDRRNGVAL       0
#define DRAMBASEADDRLO   12
#define DRAMBASEADDRHI   31
#define DRAMLIMITADDRLO  12
#define DRAMLIMITADDRHI  31
#define LGCYMMIOHOLEEN   1
#define DRAMHOLEBASELO   24
#define DRAMHOLEBASEHI   31
#define INTLVADDRSELLO   8
#define INTLVADDRSELHI   10
#define INTLVNUMCHANLO   4
#define INTLVNUMCHANHI   7
#define INTLVNUMDIESLO   10
#define INTLVNUMDIESHI   11
#define INTLVNUMSKTSLO   2
#define INTLVNUMSKTSHI   3
#define DSTFABRICIDLO    0
#define DSTFABRICIDHI_HYEX 9
#define DSTFABRICIDHI_HYGX 10
#define NUM_DRAM_MAPS    32
#define BCAST            0xffffffff
#define CDD_PER_SOCKET   4
#define UMC_PER_CDD      2
#define CHANNEL_PER_UMC  1

#define DF_SYSCFG_ADDR         ((1<<10)|(0x200))
#define DF_SYSFABIDMASK_ADDR   ((1<<10)|(0x208))
#define DF_FABBLKINSTCNT_ADDR  ((0<<10)|(0x040))
#define DF_FABBLKINFO3_ADDR    ((0<<10)|(0x050))
#define DF_DRAMBASE0_ADDR      ((0<<10)|(0x110))
#define DF_DRAMLIMIT0_ADDR     ((0<<10)|(0x114))
#define DF_DRAMOFFSET1_ADDR    ((0<<10)|(0x210)) // DramOffset0 doesn't technically exist.
#define DF_DRAMHOLECTRL_ADDR   ((0<<10)|(0x104))
#define FABRIC_REG_ACC_BC      (0xFF)

UINT64
pow_ras (
  UINTN   input,
  UINTN   exp
  );

UINT64
getBits64 (
  UINT32 lowBit,
  UINT32 highBit,
  UINT64 data
  );

UINT32
getBits (
  UINT32 lowBit,
  UINT32 highBit,
  UINT32 data
  );

UINT32
getBit (
  UINT32 bit,
  UINT32 data
  );

UINTN
convert_to_addr_trans_index (
  UINTN  pkg_no,
  UINTN  mpu_no,
  UINTN  umc_inst_num,
  UINTN  umc_chan_num
  );

BOOLEAN
internal_bit_wise_xor (
  UINT32  inp
  );

VOID
NormalizedToBankAddrMap (
  UINT64  ChannelAddr,
  UINT32 CSMask,
  UINT8 *Bank,
  UINT32 *Row,
  UINT16 *Col,
  UINT8 *Rankmul,
  UINT8 numbankbits,
  UINT8 bank4,
  UINT8 bank3,
  UINT8 bank2,
  UINT8 bank1,
  UINT8 bank0,
  UINT8 numrowlobits,
  UINT8 numrowhibits,
  UINT8 numcolbits,
  UINT8 row_lo0,
  UINT8 row_hi0,
  UINT32 COL0REG,
  UINT32 COL1REG,
  UINT8 numcsbits,
  UINT8 rm0,
  UINT8 rm1,
  UINT8 rm2,
  UINT8 chan,
  UINT8 vcm_en,
  UINT8 numbgbits,
  UINT8 bankgroupen,
  UINT8 invertmsbse,
  UINT8 invertmsbso,
  UINT8 rm0sec,
  UINT8 rm1sec,
  UINT8 rm2sec,
  UINT8 chansec,
  UINT8 invertmsbsesec,
  UINT8 invertmsbsosec,
  UINT64 CSMasksec,
  UINT8 SEC,
  UINT8 cs,
  UINT32 addrhashbank0,
  UINT32 addrhashbank1,
  UINT32 addrhashbank2,
  UINT32 addrhashbank3,
  UINT32 addrhashbank4,
  UINT32 addrhashbankpc,
  UINT32 addrhashbankpc2,
  UINT32 addrhashnormaddr0,
  UINT32 addrhashnormaddr1
  );

VOID
translate_norm_to_dram_addr (
  UINT64  ChannelAddr,
  UINT8   pkg_no,
  UINT8   mpu_no,
  UINT8   umc_inst_num,
  UINT8   umc_chan_num,
  UINT8   *cs_num,
  UINT8   *bank,
  UINT32  *row,
  UINT16  *col,
  UINT8   *rankmul
  );

UINT32
checkDramHit (
  UINT64  sysAddr,
  UINT32  ccmInstanceId
  );

UINT64
calcSysAddr (
  UINT64  normAddr,
  UINT32  mySocketNum,
  UINT32  myCddNum,
  UINT32  myChannelNum,
  UINT32  mySubChannelNum
  );

NORMALIZED_ADDRESS
calcNormAddr (
  UINT64 sysAddr
  );

UINT64
InsertBits64 (
  IN UINT64 SrcData,
  IN UINT64 InsertData,
  IN UINT64 InsertStartBit,
  IN UINT64 InsertBitWidth
  );

UINT64
RemoveBits64 (
  IN UINT64 SrcData,
  IN UINT64 RemoveStartBit,
  IN UINT64 RemoveBitWidth
  );

extern  ADDR_DATA  *gAddrData;

#endif //_Ras_BASE_LIB_H_
