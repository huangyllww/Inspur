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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Filecode.h>
#include <Guid/EventGroup.h>
#include "HYGON.h"
#include "HPOB.h"
#include <Library/DebugLib.h>
#include "Library/IdsLib.h"
#include <Library/UefiDriverEntryPoint.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/HygonPspHpobLib.h>
#include <Library/RasBaseLib.h>
#include <Library/SmnAccessLib.h>
#include <Protocol/FabricTopologyServices.h>
#include <Protocol/HygonCoreTopologyProtocol.h>
#include "HygonRasDxe.h"
#include <Fch.h>
#include <TSFch.h>
#include <GnbRegisters.h>
#include <CddRegistersDm.h>
#include <Library/HygonSmnAddressLib.h>
#include <Library/HygonSocBaseLib.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  RAS_HYGONRASDXE_HYGONRASDXE_FILECODE

#define RAS_HYEX_MAX_CORES  (CCX_MAX_SOCKETS * CCX_MAX_DIES_PER_SOCKET_HYEX * CCX_MAX_COMPLEXES_PER_DIE_HYEX * CCX_MAX_CORES_PER_COMPLEX_HYEX * CCX_MAX_THREADS_PER_CORE)
#define RAS_HYGX_MAX_CORES  (CCX_MAX_SOCKETS * CCX_MAX_DIES_PER_SOCKET_HYGX * CCX_MAX_COMPLEXES_PER_DIE_HYGX * CCX_MAX_CORES_PER_COMPLEX_HYGX * CCX_MAX_THREADS_PER_CORE)

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

extern  EFI_BOOT_SERVICES       *gBS;
ADDR_DATA         *gAddrData;
HYGON_RAS_POLICY  *mHygonRasPolicy;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
CollectDimmMap (
  VOID
  );

EFI_STATUS
CollectCpuMap (
  VOID
  );

EFI_STATUS
HygonRasPolicyInit (
  VOID
  );

// EFI_STATUS
// NbioErrThresInit(VOID);
VOID
NbioErrThresInit (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  );

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          T A B L E    D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/

/**
 * Read related register for UmcSmnRegisterRead
 *
 * @param[in] BusNumber         BusNumber of locate Socket
 * @param[in] CddNumber         Cdd ID
 * @param[in] UmcNumber         UMC ID
 * @param[in] SubChannelNumber  DDR4 always 0, DDR 5 support 0 or 1
 * @param[in] UmcSmnOffset      UMC SMN offset
 * @retval    VOID
 *----------------------------------------------------------------------------------------*/
UINT32
UmcSmnRegisterRead (
  UINTN   BusNumber,
  UINTN   CddNumber,
  UINTN   UmcNumber,
  UINTN   SubChannelNumber,
  UINTN   UmcSmnOffset
  )
{
  UINT32  SmnAddress;
  UINT32  Value;
  UINT32  UMCBaseReg;

  UMCBaseReg = (SubChannelNumber == 0) ? UMC0_CH_REG_BASE : UMC0_SUBCH1_REG_BASE;

  SmnAddress = (UINT32)UMC_SPACE (CddNumber, UmcNumber, UMCBaseReg) + (UINT32)UmcSmnOffset;
  SmnRegisterRead ((UINT32)BusNumber, SmnAddress, &Value);
  return Value;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Read related register and save to global variable
 *
 * @param[in] pkgnoe            Socket ID
 * @param[in] mpuno             Cdd ID
 * @param[in] umcno             UMC ID
 * @param[in] umcchno           always = 0 in DDR4
 * @param[in] BusNumberBase     PCI bus number
 * @retval    VOID
 *----------------------------------------------------------------------------------------*/
STATIC
VOID
retrieve_regs_DDR4 (
  UINTN   pkgno,
  UINTN   mpuno,
  UINTN   umcno,
  UINTN   umcchno,
  UINTN   BusNumberBase
  )
{
  UINTN  channel;

  channel = convert_to_addr_trans_index (pkgno, mpuno, umcno, umcchno);

  // UMC0CHx00000000 [DRAM CS Base Address] (BaseAddr), ch0_cs[3:0]_aliasSMN; UMC0CHx0000_000[[C,8,4,0]];
  // [31:1]BaseAddr: Base Address [39:9]
  gAddrData->CSBASE[channel][0] = ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x0) >> 1) << 1);
  gAddrData->CSBASE[channel][1] = ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x4) >> 1) << 1);
  gAddrData->CSBASE[channel][2] = ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x8) >> 1) << 1);
  gAddrData->CSBASE[channel][3] = ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0xC) >> 1) << 1);
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tBaseAddr-x00\t\tChannel %x cs0 = %08x cs1 = %08x cs2 = %08x cs3 = %08x\n",
    channel,
    gAddrData->CSBASE[channel][0],
    gAddrData->CSBASE[channel][1],
    gAddrData->CSBASE[channel][2],
    gAddrData->CSBASE[channel][3]
    );

  // UMC0CHx00000020 [DRAM CS Mask Address] (AddrMask), ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_002[4,0];
  // [31:1]AddrMask: Address Mask [39:9]
  gAddrData->CSMASK[channel][0] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x20) | 0x1;
  gAddrData->CSMASK[channel][1] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x24) | 0x1;
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tAddrMask-x20\t\tChannel %x Dimm0 = %08x Dimm1 = %08x\n",
    channel,
    gAddrData->CSMASK[channel][0],
    gAddrData->CSMASK[channel][1]
    );

  // UMC0CHx00000010 [DRAM CS Base Secondary Address] (BaseAddrSec), ch0_cs[3:0]_aliasSMN; UMC0CHx0000_001[[C,8,4,0]];
  // [31:1]BaseAddr: Base Address [39:9]
  gAddrData->CSBASESEC[channel][0] = ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x10) >> 1) << 1);
  gAddrData->CSBASESEC[channel][1] = ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x14) >> 1) << 1);
  gAddrData->CSBASESEC[channel][2] = ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x18) >> 1) << 1);
  gAddrData->CSBASESEC[channel][3] = ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x1C) >> 1) << 1);
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tBaseAddrSec-x10\t\tChannel %x cs0 = %08x cs1 = %08x cs2 = %08x cs3 = %08x\n",
    channel,
    gAddrData->CSBASESEC[channel][0],
    gAddrData->CSBASESEC[channel][1],
    gAddrData->CSBASESEC[channel][2],
    gAddrData->CSBASESEC[channel][3]
    );

  // UMC0CHx00000028 [DRAM CS Mask Secondary Address] (AddrMaskSec), _ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_002[C,8];
  // [31:1]AddrMask: Address Mask [39:9]
  gAddrData->CSMASKSEC[channel][0] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x28) | 0x1;
  gAddrData->CSMASKSEC[channel][1] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x2C) | 0x1;
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tAddrMaskSec-x28\t\tChannel %x Dimm0 = %08x Dimm1 = %08x\n",
    channel,
    gAddrData->CSMASKSEC[channel][0],
    gAddrData->CSMASKSEC[channel][1]
    );

  // UMC0CHx00000080 [DIMM Configuration] (DimmCfg), ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_008[4,0];
  // [11]DDR4eEn
  //
  // UMC0CHx00000200 [DRAM Configuration] (DramConfiguration), umc0_ch0_mp[3:0]_aliasSMN; UMC0CHx0000_0[5:2]00;
  // [8]BankGroupEn. This register is per channel, not per DIMM. Following read from 0x200 twice
  gAddrData->CTRLREG[channel][0] =
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x80)) >> 11) & 0x1) << 13) /*bit13*/ |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x200)) >> 8) & 0x1) << 5); /*bit5*/
  gAddrData->CTRLREG[channel][1] =
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x84)) >> 11) & 0x1) << 13) /*bit13*/ |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x200)) >> 8) & 0x1) << 5); /*bit5*/
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tDimmCfg-x28[11]-x200[8]\tChannel %x Dimm0 DDR4eEn = %x BankGroupEn = %x, Dimm1 DDR4eEn = %x BankGroupEn = %x\n",
    channel,
    ((gAddrData->CTRLREG[channel][0] & BIT13) ? 1 : 0),
    ((gAddrData->CTRLREG[channel][0] & BIT5) ? 1 : 0),
    ((gAddrData->CTRLREG[channel][1] & BIT13) ? 1 : 0),
    ((gAddrData->CTRLREG[channel][1] & BIT5) ? 1 : 0)
    );

  // UMC0CHx00000010 [DRAM CS Base Secondary Address] (BaseAddrSec), ch0_cs[3:0]_aliasSMN; UMC0CHx0000_001[[C,8,4,0]];
  // [0]CSEnable
  //
  // UMC0CHx00000030 [DRAM Address Configuration] (AddrCfg), ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_003[4,0];
  // [21:20]NumBanks
  // [19:16]NumCol
  // [15:12]NumRowHi
  // [11:8]NumRowLo
  // [5:4]NumRM
  // [3:2]NumBankGroups
  //
  // UMC0CHx00000000 [DRAM CS Base Address] (BaseAddr), ch0_cs[3:0]_aliasSMN; UMC0CHx0000_000[[C,8,4,0]];
  // [0]CSEnable
  // DIMM 0
  gAddrData->CONFIGDIMM[channel][0] =
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x14)) >> 0) & 0x1) << 25) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x10)) >> 0) & 0x1) << 24) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x30)) >> 2) & 0x3) << 20) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x30)) >> 16) & 0xf) << 16) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x30)) >> 12) & 0xf) << 12) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x30)) >> 8) & 0xf) << 8) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x30)) >> 4) & 0x3) << 6) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x30)) >> 20) & 0x3) << 4) |
    (((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x4)) & 0x1) << 1) |
    ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x0)) & 0x1);
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tAddrCfg-x30\t\tChannel %x Dimm0 = %08x ([25:24]BaseAddrSec CSEnable, [1:0]BaseAddr CSEnable)\n",
    channel,
    gAddrData->CONFIGDIMM[channel][0]
    );

  // As above
  // DIMM 1
  gAddrData->CONFIGDIMM[channel][1] =
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x1C)) >> 0) & 0x1) << 25) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x18)) >> 0) & 0x1) << 24) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x34)) >> 2) & 0x3) << 20) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x34)) >> 16) & 0xf) << 16) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x34)) >> 12) & 0xf) << 12) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x34)) >> 8) & 0xf) << 8) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x34)) >> 4) & 0x3) << 6) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x34)) >> 20) & 0x3) << 4) |
    (((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0xc)) & 0x1) << 1) |
    ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x8)) & 0x1);
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tAddrCfg-x30\t\tChannel %x Dimm1 = %08x ([25:24]BaseAddrSec CSEnable, [1:0]BaseAddr CSEnable)\n",
    channel,
    gAddrData->CONFIGDIMM[channel][1]
    );

  // UMC0CHx00000040 [DRAM Bank Address Select] (AddrSel), ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_004[4,0];
  // [19:16]BankBit4
  // [15:12]BankBit3
  // [11:8]BankBit2
  // [7:4]BankBit1
  // [3:0]BankBit0
  gAddrData->BANKSELDIMM[channel][0] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x40) & 0xfffff;

  // As above
  gAddrData->BANKSELDIMM[channel][1] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x44) & 0xfffff;
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tAddrSel-x40\t\tChannel %x BankBit Dimm0 = %08x Dimm1 = %08x\n",
    channel,
    gAddrData->BANKSELDIMM[channel][0],
    gAddrData->BANKSELDIMM[channel][1]
    );

  // UMC0CHx00000040 [DRAM Bank Address Select] (AddrSel), ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_004[4,0];
  // [31:28]RowHi
  // [27:24]RowLo
  gAddrData->ROWSELDIMM[channel][0] = (((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x40) >> 24) & 0xff));
  gAddrData->ROWSELDIMM[channel][1] = (((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x44) >> 24) & 0xff));
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tAddrSel-x40\t\tChannel %x RowHiLo Dimm0 = %08x Dimm1 = %08x\n",
    channel,
    gAddrData->ROWSELDIMM[channel][0],
    gAddrData->ROWSELDIMM[channel][1]
    );

  // UMC0CHx00000050 [DRAM Column Address Select Low] (ColSelLo), _ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_005[[8,0]];
  gAddrData->COL0SELDIMM[channel][0] = (UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x50));
  gAddrData->COL0SELDIMM[channel][1] = (UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x58));
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tColSelLo-x50\t\tChannel %x Dimm0 = %08x Dimm1 = %08x\n",
    channel,
    gAddrData->COL0SELDIMM[channel][0],
    gAddrData->COL0SELDIMM[channel][1]
    );

  // UMC0CHx00000054 [DRAM Column Address Select High] (ColSelHi), _ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_005[[C,4]];
  gAddrData->COL1SELDIMM[channel][0] = (UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x54));
  gAddrData->COL1SELDIMM[channel][1] = (UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x5C));
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tColSelHi-x54\t\tChannel %x Dimm0 = %08x Dimm1 = %08x\n",
    channel,
    gAddrData->COL1SELDIMM[channel][0],
    gAddrData->COL1SELDIMM[channel][1]
    );

  // UMC0CHx00000070 [DRAM Rank Multiply Address Select] (RmSel), _ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_007[4,0];
  gAddrData->RMSELDIMM[channel][0] = (UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x70));
  gAddrData->RMSELDIMM[channel][1] = (UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x74));
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tRmSel-x70\t\tChannel %x Dimm0 = %08x Dimm1 = %08x\n",
    channel,
    gAddrData->RMSELDIMM[channel][0],
    gAddrData->RMSELDIMM[channel][1]
    );

  // UMC0CHx00000078 [DRAM Rank Multiply Secondary Address Select] (RmSelSec), _ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_007[C,8];
  gAddrData->RMSELDIMMSEC[channel][0] = (UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x78));
  gAddrData->RMSELDIMMSEC[channel][1] = (UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x7C));
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tRmSelSec-x78\t\tChannel %x Dimm0 = %08x Dimm1 = %08x\n",
    channel,
    gAddrData->RMSELDIMMSEC[channel][0],
    gAddrData->RMSELDIMMSEC[channel][1]
    );

  // UMC0CHx000000C8 [Address Hash Bank] (AddrHashBank), _ch0_n[4:0]_aliasSMN; UMC0CHx0000_00[D8,D4,D0,CC,C8];
  // [31:14]RowXor: RowXor[17:0]
  // [13:1]ColXor: ColXor[12:0]
  // [0]XorEnable
  gAddrData->ADDRHASHBANK0[channel] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0xC8);
  gAddrData->ADDRHASHBANK1[channel] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0xCC);
  gAddrData->ADDRHASHBANK2[channel] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0xD0);
  gAddrData->ADDRHASHBANK3[channel] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0xD4);
  gAddrData->ADDRHASHBANK4[channel] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0xD8);
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tAddrHashBank-xC8\tChannel %x n0 = %08x n1 = %08x n2 = %08x n3 = %08x n4 = %08x\n",
    channel,
    gAddrData->ADDRHASHBANK0[channel],
    gAddrData->ADDRHASHBANK1[channel],
    gAddrData->ADDRHASHBANK2[channel],
    gAddrData->ADDRHASHBANK3[channel],
    gAddrData->ADDRHASHBANK4[channel]
    );

  // UMC0CHx000000E8 [Address Hash CS] (AddrHashCS), _ch0_n[1:0]_aliasSMN; UMC0CHx0000_00E[C,8];
  // [31:1]NormAddrXor: NormAddrXor[39:9]
  // [0]XorEn
  gAddrData->ADDRHASHNORMADDR[channel][0] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0xE8);
  gAddrData->ADDRHASHNORMADDR[channel][1] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0xEC);
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tAddrHashCS-xE8\t\tChannel %x n0 = %08x n1 = %08x\n",
    channel,
    gAddrData->ADDRHASHNORMADDR[channel][0],
    gAddrData->ADDRHASHNORMADDR[channel][1]
    );
}

/*----------------------------------------------------------------------------------------*/

/**
 * Read related register and save to global variable
 *
 * @param[in] pkgnoe            Socket ID
 * @param[in] mpuno             Cdd ID
 * @param[in] umcno             UMC ID
 * @param[in] umcchno           Sub channel ID, 0 or 1 in DDR5
 * @param[in] BusNumberBase     PCI bus number
 * @retval    VOID
 *----------------------------------------------------------------------------------------*/
STATIC
VOID
retrieve_regs_DDR5 (
  UINTN   pkgno,
  UINTN   mpuno,
  UINTN   umcno,
  UINTN   umcchno,
  UINTN   BusNumberBase
  )
{
  UINTN  channel;

  channel = convert_to_addr_trans_index (pkgno, mpuno, umcno, umcchno);

  // UMC0CHx00000000 [DRAM CS Base Address] (BaseAddr), ch0_cs[3:0]_aliasSMN; UMC0CHx0000_000[[C,8,4,0]];
  // [31:1]BaseAddr: Base Address [39:9]
  gAddrData->CSBASE[channel][0] = ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x0) >> 1) << 1);
  gAddrData->CSBASE[channel][1] = ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x4) >> 1) << 1);
  gAddrData->CSBASE[channel][2] = ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x8) >> 1) << 1);
  gAddrData->CSBASE[channel][3] = ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0xC) >> 1) << 1);
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tBaseAddr-x00\t\tChannel %x cs0 = %08x cs1 = %08x cs2 = %08x cs3 = %08x\n",
    channel,
    gAddrData->CSBASE[channel][0],
    gAddrData->CSBASE[channel][1],
    gAddrData->CSBASE[channel][2],
    gAddrData->CSBASE[channel][3]
    );

  // UMC0CHx00000020 [DRAM CS Mask Address] (AddrMask), ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_002[4,0];
  // [31:1]AddrMask: Address Mask [39:9]
  gAddrData->CSMASK[channel][0] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x20) | 0x1;
  gAddrData->CSMASK[channel][1] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x24) | 0x1;
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tAddrMask-x20\t\tChannel %x Dimm0 = %08x Dimm1 = %08x\n",
    channel,
    gAddrData->CSMASK[channel][0],
    gAddrData->CSMASK[channel][1]
    );

  // UMC0CHx00000010 [DRAM CS Base Secondary Address] (BaseAddrSec), ch0_cs[3:0]_aliasSMN; UMC0CHx0000_001[[C,8,4,0]];
  // [31:1]BaseAddr: Base Address [39:9]
  gAddrData->CSBASESEC[channel][0] = ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x10) >> 1) << 1);
  gAddrData->CSBASESEC[channel][1] = ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x14) >> 1) << 1);
  gAddrData->CSBASESEC[channel][2] = ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x18) >> 1) << 1);
  gAddrData->CSBASESEC[channel][3] = ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x1C) >> 1) << 1);
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tBaseAddrSec-x10\t\tChannel %x cs0 = %08x cs1 = %08x cs2 = %08x cs3 = %08x\n",
    channel,
    gAddrData->CSBASESEC[channel][0],
    gAddrData->CSBASESEC[channel][1],
    gAddrData->CSBASESEC[channel][2],
    gAddrData->CSBASESEC[channel][3]
    );

  // UMC0CHx00000028 [DRAM CS Mask Secondary Address] (AddrMaskSec), _ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_002[C,8];
  // [31:1]AddrMask: Address Mask [39:9]
  gAddrData->CSMASKSEC[channel][0] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x28) | 0x1;
  gAddrData->CSMASKSEC[channel][1] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x2C) | 0x1;
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tAddrMaskSec-x28\t\tChannel %x Dimm0 = %08x Dimm1 = %08x\n",
    channel,
    gAddrData->CSMASKSEC[channel][0],
    gAddrData->CSMASKSEC[channel][1]
    );

  // UMC0CHx00000080 [DIMM Configuration] (DimmCfg), ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_008[4,0];
  // [11]DDR4eEn
  //
  // UMC0CHx00000200 [DRAM Configuration] (DramConfiguration), umc0_ch0_mp[3:0]_aliasSMN; UMC0CHx0000_0[5:2]00;
  // [8]BankGroupEn. This register is per channel, not per DIMM. Following read from 0x200 twice
  gAddrData->CTRLREG[channel][0] =
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x80)) >> 11) & 0x1) << 13) /*bit13*/ |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x200)) >> 8) & 0x1) << 5); /*bit5*/
  gAddrData->CTRLREG[channel][1] =
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x84)) >> 11) & 0x1) << 13) /*bit13*/ |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x200)) >> 8) & 0x1) << 5); /*bit5*/
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tDimmCfg-x28[11]-x200[8]\tChannel %x Dimm0 DDR4eEn = %x BankGroupEn = %x, Dimm1 DDR4eEn = %x BankGroupEn = %x\n",
    channel,
    ((gAddrData->CTRLREG[channel][0] & BIT13) ? 1 : 0),
    ((gAddrData->CTRLREG[channel][0] & BIT5) ? 1 : 0),
    ((gAddrData->CTRLREG[channel][1] & BIT13) ? 1 : 0),
    ((gAddrData->CTRLREG[channel][1] & BIT5) ? 1 : 0)
    );

  // UMC0CHx00000010 [DRAM CS Base Secondary Address] (BaseAddrSec), ch0_cs[3:0]_aliasSMN; UMC0CHx0000_001[[C,8,4,0]];
  // [0]CSEnable
  //
  // UMC0CHx00000030 [DRAM Address Configuration] (AddrCfg), ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_003[4,0];
  // [21:20]NumBanks
  // [19:16]NumCol
  // [15:12]NumRowHi
  // [11:8]NumRowLo
  // [5:4]NumRM
  // [3:2]NumBankGroups
  //
  // UMC0CHx00000000 [DRAM CS Base Address] (BaseAddr), ch0_cs[3:0]_aliasSMN; UMC0CHx0000_000[[C,8,4,0]];
  // [0]CSEnable
  // DIMM 0
  gAddrData->CONFIGDIMM[channel][0] =
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x14)) >> 0) & 0x1) << 25) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x10)) >> 0) & 0x1) << 24) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x30)) >> 2) & 0x3) << 20) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x30)) >> 16) & 0xf) << 16) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x30)) >> 12) & 0xf) << 12) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x30)) >> 8) & 0xf) << 8) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x30)) >> 4) & 0x3) << 6) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x30)) >> 20) & 0x3) << 4) |
    (((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x4)) & 0x1) << 1) |
    ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x0)) & 0x1);
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tAddrCfg-x30\t\tChannel %x Dimm0 = %08x ([25:24]BaseAddrSec CSEnable, [1:0]BaseAddr CSEnable)\n",
    channel,
    gAddrData->CONFIGDIMM[channel][0]
    );

  // As above
  // DIMM 1
  gAddrData->CONFIGDIMM[channel][1] =
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x1C)) >> 0) & 0x1) << 25) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x18)) >> 0) & 0x1) << 24) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x34)) >> 2) & 0x3) << 20) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x34)) >> 16) & 0xf) << 16) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x34)) >> 12) & 0xf) << 12) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x34)) >> 8) & 0xf) << 8) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x34)) >> 4) & 0x3) << 6) |
    ((((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x34)) >> 20) & 0x3) << 4) |
    (((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0xc)) & 0x1) << 1) |
    ((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x8)) & 0x1);
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tAddrCfg-x30\t\tChannel %x Dimm1 = %08x ([25:24]BaseAddrSec CSEnable, [1:0]BaseAddr CSEnable)\n",
    channel,
    gAddrData->CONFIGDIMM[channel][1]
    );

  // UMC0CHx00000040 [DRAM Bank Address Select] (AddrSel), ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_004[4,0];
  // [19:16]BankBit4
  // [15:12]BankBit3
  // [11:8]BankBit2
  // [7:4]BankBit1
  // [3:0]BankBit0
  gAddrData->BANKSELDIMM[channel][0] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x40) & 0xfffff;

  // As above
  gAddrData->BANKSELDIMM[channel][1] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x44) & 0xfffff;
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tAddrSel-x40\t\tChannel %x BankBit Dimm0 = %08x Dimm1 = %08x\n",
    channel,
    gAddrData->BANKSELDIMM[channel][0],
    gAddrData->BANKSELDIMM[channel][1]
    );

  // UMC0CHx00000040 [DRAM Bank Address Select] (AddrSel), ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_004[4,0];
  // [31:28]RowHi
  // [27:24]RowLo
  gAddrData->ROWSELDIMM[channel][0] = (((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x40) >> 24) & 0xff));
  gAddrData->ROWSELDIMM[channel][1] = (((UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x44) >> 24) & 0xff));
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tAddrSel-x40\t\tChannel %x RowHiLo Dimm0 = %08x Dimm1 = %08x\n",
    channel,
    gAddrData->ROWSELDIMM[channel][0],
    gAddrData->ROWSELDIMM[channel][1]
    );

  // UMC0CHx00000050 [DRAM Column Address Select Low] (ColSelLo), _ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_005[[8,0]];
  gAddrData->COL0SELDIMM[channel][0] = (UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x50));
  gAddrData->COL0SELDIMM[channel][1] = (UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x58));
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tColSelLo-x50\t\tChannel %x Dimm0 = %08x Dimm1 = %08x\n",
    channel,
    gAddrData->COL0SELDIMM[channel][0],
    gAddrData->COL0SELDIMM[channel][1]
    );

  // UMC0CHx00000054 [DRAM Column Address Select High] (ColSelHi), _ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_005[[C,4]];
  gAddrData->COL1SELDIMM[channel][0] = (UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x54));
  gAddrData->COL1SELDIMM[channel][1] = (UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x5C));
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tColSelHi-x54\t\tChannel %x Dimm0 = %08x Dimm1 = %08x\n",
    channel,
    gAddrData->COL1SELDIMM[channel][0],
    gAddrData->COL1SELDIMM[channel][1]
    );

  // UMC0CHx00000070 [DRAM Rank Multiply Address Select] (RmSel), _ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_007[4,0];
  gAddrData->RMSELDIMM[channel][0] = (UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x70));
  gAddrData->RMSELDIMM[channel][1] = (UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x74));
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tRmSel-x70\t\tChannel %x Dimm0 = %08x Dimm1 = %08x\n",
    channel,
    gAddrData->RMSELDIMM[channel][0],
    gAddrData->RMSELDIMM[channel][1]
    );

  // UMC0CHx00000078 [DRAM Rank Multiply Secondary Address Select] (RmSelSec), _ch0_dimm[1:0]_aliasSMN; UMC0CHx0000_007[C,8];
  gAddrData->RMSELDIMMSEC[channel][0] = (UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x78));
  gAddrData->RMSELDIMMSEC[channel][1] = (UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0x7C));
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tRmSelSec-x78\t\tChannel %x Dimm0 = %08x Dimm1 = %08x\n",
    channel,
    gAddrData->RMSELDIMMSEC[channel][0],
    gAddrData->RMSELDIMMSEC[channel][1]
    );

  // UMC0CHx000000C8 [Address Hash Bank] (AddrHashBank), _ch0_n[4:0]_aliasSMN; UMC0CHx0000_00[D8,D4,D0,CC,C8];
  // [31:14]RowXor: RowXor[17:0]
  // [13:1]ColXor: ColXor[12:0]
  // [0]XorEnable
  gAddrData->ADDRHASHBANK0[channel] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0xC8);
  gAddrData->ADDRHASHBANK1[channel] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0xCC);
  gAddrData->ADDRHASHBANK2[channel] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0xD0);
  gAddrData->ADDRHASHBANK3[channel] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0xD4);
  gAddrData->ADDRHASHBANK4[channel] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0xD8);
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tAddrHashBank-xC8\tChannel %x n0 = %08x n1 = %08x n2 = %08x n3 = %08x n4 = %08x\n",
    channel,
    gAddrData->ADDRHASHBANK0[channel],
    gAddrData->ADDRHASHBANK1[channel],
    gAddrData->ADDRHASHBANK2[channel],
    gAddrData->ADDRHASHBANK3[channel],
    gAddrData->ADDRHASHBANK4[channel]
    );

  // UMC0CHx000000E0 [Address Hash CS] (AddrHashCS), _ch0_n[1:0]_aliasSMN; UMC0CHx0000_00E[4,0];
  // [31:1]NormAddrXor: NormAddrXor[39:9]
  // [0]XorEn
  gAddrData->ADDRHASHNORMADDR[channel][0] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0xE0);
  gAddrData->ADDRHASHNORMADDR[channel][1] = UmcSmnRegisterRead (BusNumberBase, mpuno, umcno, umcchno, 0xE4);
  IDS_HDT_CONSOLE (
    MAIN_FLOW,
    "\t\tAddrHashCS-xE8\t\tChannel %x n0 = %08x n1 = %08x\n",
    channel,
    gAddrData->ADDRHASHNORMADDR[channel][0],
    gAddrData->ADDRHASHNORMADDR[channel][1]
    );
}

/*********************************************************************************
 * Name: HygonRasDxeInit
 *
 * Description
 *   Entry point of the HYGON RAS DXE driver
 *   Perform the configuration init, resource reservation, early post init
 *   and install all the supported protocol
 *
 * Input
 *   ImageHandle : EFI Image Handle for the DXE driver
 *   SystemTable : pointer to the EFI system table
 *
 * Output
 *   EFI_SUCCESS : Module initialized successfully
 *   EFI_ERROR   : Initialization failed (see error for more details)
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
HygonRasDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_HANDLE  Handle = NULL;
  EFI_EVENT   ExitBootServicesEvent;

  HygonRasPolicyInit ();

  CollectDimmMap ();

  CollectCpuMap ();

  Handle = ImageHandle;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gHygonRasInitDataProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  mHygonRasPolicy
                  );
  if (EFI_ERROR (Status)) {
    return (Status);
  }

  //
  // Register the event handling function
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  NbioErrThresInit,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &ExitBootServicesEvent
                  );

  return (Status);
}

VOID
NbioErrThresInit (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  PARITY_CONTROL_0_REG                     ParityControl0;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  UINTN                                    NumberOfSockets;
  UINTN                                    NumberOfPhysicalDies;
  UINTN                                    NumberOfLogicalDies;
  UINTN                                    RbNumberOfLogicalDie;
  UINTN                                    BusNumberBase;
  UINTN                                    SocketId;
  UINTN                                    RbId;
  UINTN                                    LogicalDieId;
  UINTN                                    IohubPhysicalDieId;
  UINT32                                   CpuModel;
  EFI_STATUS                               Status;

  gBS->CloseEvent(Event);                             // byo230922 +

  // Init reset value
  ParityControl0.Fields.ParityUCPThreshold  = 1;
  ParityControl0.Fields.ParityCorrThreshold = 1;
  
  CpuModel = GetHygonSocModel();

  // Read pcd setting
  if (mHygonRasPolicy->NbioDeferredErrThreshEn) {
    ParityControl0.Fields.ParityUCPThreshold = mHygonRasPolicy->NbioDeferredErrThreshCount;
    DEBUG ((EFI_D_ERROR, "[RAS]NBIO Deferred Error Threshold : %d\n", ParityControl0.Fields.ParityUCPThreshold));
  }

  if (mHygonRasPolicy->NbioCorrectedErrThreshEn) {
    ParityControl0.Fields.ParityCorrThreshold = mHygonRasPolicy->NbioCorrectedErrThreshCount;
    DEBUG ((EFI_D_ERROR, "[RAS]NBIO Corrected Error Threshold : %d\n", ParityControl0.Fields.ParityCorrThreshold));
  }

  Status = gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, (VOID **)&FabricTopology);
  if (!EFI_ERROR (Status)) {
    FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL);

    for (SocketId = 0; SocketId < NumberOfSockets; SocketId++) {
      FabricTopology->GetProcessorInfo (FabricTopology, SocketId, &NumberOfPhysicalDies, &NumberOfLogicalDies, NULL);
      for(LogicalDieId = 0; LogicalDieId < NumberOfLogicalDies; LogicalDieId++) {
        IohubPhysicalDieId = FabricTopologyGetIohubPhysicalDieId (LogicalDieId);

        FabricTopology->GetDieInfo (FabricTopology, SocketId, LogicalDieId, &RbNumberOfLogicalDie, NULL, NULL);
        for (RbId = 0; RbId < RbNumberOfLogicalDie; RbId++) {
          FabricTopology->GetRootBridgeInfo (FabricTopology, SocketId, LogicalDieId, RbId, NULL, &BusNumberBase, NULL);
          DEBUG ((EFI_D_ERROR, "[RAS]Bus: 0x%0x, Parity Control 0 : 0x%08x\n", BusNumberBase, ParityControl0.Value));
          SmnRegisterWrite ((UINT32)BusNumberBase, NBIO_SPACE2 (IohubPhysicalDieId, RbId, (CpuModel==HYGON_EX_CPU)?RAS_PARITY_CONTROL_0_HYEX:RAS_PARITY_CONTROL_0_HYGX), &ParityControl0.Value, 0);
        }
      }
    }
  }
}

EFI_STATUS
HygonRasPolicyInit (
  VOID
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  BYO_RAS_POLICY_DATA  *ByoRasPolicy;           // byo231109 +

  DEBUG((EFI_D_INFO, "HygonRasPolicyInit\n"));  // byo231109 +

  //
  // Allocate memory and Initialize for Data block
  //
  Status = gBS->AllocatePool (
                  EfiReservedMemoryType,
                  sizeof (HYGON_RAS_POLICY),
                  (VOID **)&mHygonRasPolicy
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (mHygonRasPolicy, sizeof (HYGON_RAS_POLICY));

// byo231109 + >>
  Status = gBS->LocateProtocol (
                  &gByoRasPolicyDataProtocolGuid,
                  NULL,
                  (VOID**)&ByoRasPolicy
                  );
  if(!EFI_ERROR(Status) && ByoRasPolicy->Tag == BYO_RAS_POLICY_DATA_TAG && ByoRasPolicy->Version == BYO_RAS_POLICY_DATA_VER){
    mHygonRasPolicy->McaErrThreshSwCount = ByoRasPolicy->McaErrThreshSwCount;
    mHygonRasPolicy->McaErrThreshCount   = ByoRasPolicy->McaErrThreshCount;
    mHygonRasPolicy->McaNonMemErrThresh  = ByoRasPolicy->McaNonMemErrThresh;
  } else {
    DEBUG((EFI_D_INFO, "Default %r %x %x\n", Status, ByoRasPolicy->Tag, ByoRasPolicy->Version));
    mHygonRasPolicy->McaErrThreshSwCount = PcdGet16(PcdMcaErrThreshCount);
    mHygonRasPolicy->McaErrThreshCount   = mHygonRasPolicy->McaErrThreshSwCount;
    mHygonRasPolicy->McaNonMemErrThresh  = 4096;
  }
// byo231109 + <<

  mHygonRasPolicy->PFEHEnable     = PcdGetBool (PcdHygonCcxCfgPFEHEnable);
  mHygonRasPolicy->MceSwSmiData   = PcdGet8 (PcdMceSwSmiData);
  mHygonRasPolicy->McaErrThreshEn = PcdGetBool (PcdMcaErrThreshEn);

  mHygonRasPolicy->NbioCorrectedErrThreshEn    = PcdGetBool (PcdNbioCorrectedErrThreshEn);
  mHygonRasPolicy->NbioCorrectedErrThreshCount = PcdGet16 (PcdNbioCorrectedErrThreshCount);
  mHygonRasPolicy->NbioDeferredErrThreshEn     = PcdGetBool (PcdNbioDeferredErrThreshEn);
  mHygonRasPolicy->NbioDeferredErrThreshCount  = PcdGet16 (PcdNbioDeferredErrThreshCount);

  // FCH software SMI command port
  mHygonRasPolicy->SwSmiCmdPortAddr = MmioRead16 ((ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6A));

  return Status;
}

EFI_STATUS
CollectDimmMap (
  VOID
  )
{
  EFI_STATUS                               Status = EFI_SUCCESS;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  UINTN                                    NumberOfSockets;
  UINTN                                    NumberOfCdds;
  UINTN                                    CddsPresent;
  UINTN                                    BusNumberBase;
  UINTN                                    SocketId;
  UINTN                                    CddId;
  UINTN                                    ChannelsPerCdd;
  UINTN                                    channelId;
  UINTN                                    channelIdInSystem;
  UINT32                                   CpuModel;

  Status = gBS->AllocatePool (
                  EfiReservedMemoryType, // IN EFI_MEMORY_TYPE PoolType
                  sizeof (ADDR_DATA),    // IN UINTN Size
                  &gAddrData             // OUT VOID **Buffer
                  );
  if (EFI_ERROR (Status)) {
    ASSERT (!EFI_ERROR (Status));
    return Status;
  }

  ZeroMem (gAddrData, sizeof (ADDR_DATA));

  NumberOfSockets = 0;
  NumberOfCdds    = 0;
  BusNumberBase   = 0;
  ChannelsPerCdd = 2;

  Status = gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, (VOID **)&FabricTopology);
  FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL);

  CpuModel = GetHygonSocModel ();
  if ((CpuModel == HYGON_EX_CPU) && PcdGetBool(PcdDDR5Enable)) {
    ChannelsPerCdd = 3;
  }
  for (SocketId = 0; SocketId < NumberOfSockets; SocketId++) {
    FabricTopology->GetRootBridgeInfo (FabricTopology, SocketId, 0, 0, NULL, &BusNumberBase, NULL);
    FabricTopology->GetCddInfo (FabricTopology, SocketId, &NumberOfCdds, &CddsPresent);
    for (CddId = 0; CddId < MAX_CDDS_PER_SOCKET; CddId++) {
      if (!IS_CDD_PRESENT (CddId, CddsPresent)) {
        continue;
      }

      for (channelId = 0; channelId < ChannelsPerCdd; channelId++) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "\nSocket %x Cdd %x channel %x pci bus = %x\n", SocketId, CddId, channelId, BusNumberBase);
        if (PcdGetBool (PcdDDR5Enable)) {
          retrieve_regs_DDR5 (SocketId, CddId, channelId, 0, BusNumberBase);
        } else {
          retrieve_regs_DDR4 (SocketId, CddId, channelId, 0, BusNumberBase);
        }

        channelIdInSystem = convert_to_addr_trans_index (SocketId, CddId, channelId, 0);

        // UMC0CHx00000000 [DRAM CS Base Address] (BaseAddr)
        // - _umc0_ch0_cs[3:0]_aliasSMN; UMC0CHx0000_000[[C,8,4,0]]; UMC0CH=0005_0000h
        // - _umc1_ch0_cs[3:0]_aliasSMN; UMC1CHx0000_000[[C,8,4,0]]; UMC1CH=0015_0000h
        // Figure out how many chip select is enabled in a channel
        gAddrData->TOTAL_NUM_RANKS_PER_UMCCH_ADDR_TRANS[channelIdInSystem] = (((UmcSmnRegisterRead (BusNumberBase, CddId, channelId, 0, 0x4)) & 0x1)) +
                                                                             ((UmcSmnRegisterRead (BusNumberBase, CddId, channelId, 0, 0x0)) & 0x1) +
                                                                             ((UmcSmnRegisterRead (BusNumberBase, CddId, channelId, 0, 0x8)) & 0x1) +
                                                                             ((UmcSmnRegisterRead (BusNumberBase, CddId, channelId, 0, 0xC)) & 0x1);
        // IDS_HDT_CONSOLE (MAIN_FLOW, "\t\tTotal chip select enabled in this channel = %x\n", gAddrData->TOTAL_NUM_RANKS_PER_UMCCH_ADDR_TRANS[channelIdInSystem]);

        // Mark enabled chip select in bit position in RANK_ENABLE_PER_UMCCH_ADDR_TRANS
        gAddrData->RANK_ENABLE_PER_UMCCH_ADDR_TRANS[channelIdInSystem] =  ((((UmcSmnRegisterRead (BusNumberBase, CddId, channelId, 0, 0xC)) & 0x1))<<3)|
                                                                         ((((UmcSmnRegisterRead (BusNumberBase, CddId, channelId, 0, 0x8)) & 0x1)) <<2)|
                                                                         ((((UmcSmnRegisterRead (BusNumberBase, CddId, channelId, 0, 0x4)) & 0x1))<<1) |
                                                                         ((UmcSmnRegisterRead (BusNumberBase, CddId, channelId, 0, 0x0)) & 0x1);
        // IDS_HDT_CONSOLE (MAIN_FLOW, "\t\tChip select enabled in this channel (bit position) = %x\n", gAddrData->RANK_ENABLE_PER_UMCCH_ADDR_TRANS[channelIdInSystem]);
      }
    }
  }

  // Save Memory Address Data pointer to Hygon Ras Policy
  mHygonRasPolicy->AddrData = gAddrData;

  return Status;
}

EFI_STATUS
CollectCpuMap (
  VOID
  )
{
  HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL         *CoreTopologyServices;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL       *FabricTopologyServices;
  HPOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE_STRUCT_HYGX  HpobCcxLogToPhysMap;
  HPOB_TYPE_HEADER                              *HpobEntry;
  CPU_INFO                                      *RasCpuMap;
  UINT32                                        HpobInstanceId;
  UINT32                                        Index;
  UINTN                                         SocketLoop;
  UINTN                                         CddLoop;
  UINTN                                         CddsPresent;
  UINTN                                         ComplexLoop;
  UINTN                                         CoreLoop;
  UINTN                                         NumberOfSockets;
  UINTN                                         NumberOfSystemDies;
  UINTN                                         NumberOfCdds;
  UINTN                                         NumberOfComplexes;
  UINTN                                         NumberOfCores;
  UINTN                                         NumberOfThreads;
  UINT32                                        CpuModel;
  UINTN                                         MaxCores;
  EFI_STATUS                                    Status = EFI_SUCCESS;

  gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, &FabricTopologyServices);
  gBS->LocateProtocol (&gHygonCoreTopologyServicesProtocolGuid, NULL, &CoreTopologyServices);

  CpuModel = GetHygonSocModel();
  MaxCores = (CpuModel == HYGON_EX_CPU) ? RAS_HYEX_MAX_CORES : RAS_HYGX_MAX_CORES;

  //
  // Allocate memory and Initialize for Data block
  //
  Status = gBS->AllocatePool (
                  EfiReservedMemoryType,
                  sizeof (CPU_INFO) * MaxCores,
                  (VOID **)&RasCpuMap
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (RasCpuMap, sizeof (CPU_INFO) * MaxCores);

  if (FabricTopologyServices->GetSystemInfo (FabricTopologyServices, &NumberOfSockets, &NumberOfSystemDies, NULL) != EFI_SUCCESS) {
    return (Status);
  }

  Index = 0;
  for (SocketLoop = 0; SocketLoop < NumberOfSockets; SocketLoop++) {
    if (FabricTopologyServices->GetCddInfo (FabricTopologyServices, SocketLoop, &NumberOfCdds, &CddsPresent) != EFI_SUCCESS) {
      return (Status);
    }

    for (CddLoop = 0; CddLoop < MAX_CDDS_PER_SOCKET; CddLoop++) {
      if (!IS_CDD_PRESENT (CddLoop, CddsPresent)) {
        continue;
      }

      if (CoreTopologyServices->GetCoreTopologyOnCdd (CoreTopologyServices, SocketLoop, CddLoop, &NumberOfComplexes, &NumberOfCores, &NumberOfThreads) != EFI_SUCCESS) {
        return (Status);
      }

      HpobInstanceId = HygonPspGetHpobCddInstanceId ((UINT32)SocketLoop, (UINT32)CddLoop);
      if (HygonPspGetHpobEntryInstance (HPOB_GROUP_CCX, HPOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE, HpobInstanceId, FALSE, &HpobEntry) != EFI_SUCCESS) {
        return (Status);
      }

      CopyMem (&HpobCcxLogToPhysMap, HpobEntry, sizeof (HPOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE_STRUCT_HYGX));

      for (ComplexLoop = 0; ComplexLoop < NumberOfComplexes; ComplexLoop++) {
        if (HpobCcxLogToPhysMap.ComplexMap[ComplexLoop].PhysComplexNumber == CCX_NOT_PRESENT) {
          return (Status);
        }

        for (CoreLoop = 0; CoreLoop < NumberOfCores; CoreLoop++) {
          if (HpobCcxLogToPhysMap.ComplexMap[ComplexLoop].CoreInfo[CoreLoop].PhysCoreNumber  == CCX_NOT_PRESENT) {
            return (Status);
          }

          if (Index == MaxCores) {
            return EFI_BUFFER_TOO_SMALL;                // Should not be here
          }

          RasCpuMap[Index].ProcessorNumber = Index;    // CPU Logic Number
          RasCpuMap[Index].SocketId = (UINT8)(SocketLoop & 0xFF);
          RasCpuMap[Index].CddId    = (UINT8)(CddLoop & 0xFF);
          RasCpuMap[Index].CcxId    = (UINT8)(ComplexLoop & 0xFF);
          RasCpuMap[Index].CoreId   = (UINT8)(CoreLoop & 0xFF);
          RasCpuMap[Index].ThreadID = 0;
          Index++;
          if (NumberOfThreads > 1) {
            if (Index == MaxCores) {
              return EFI_BUFFER_TOO_SMALL;
            }

            RasCpuMap[Index].ProcessorNumber = Index; // CPU Logic Number
            RasCpuMap[Index].SocketId = (UINT8)(SocketLoop & 0xFF);
            RasCpuMap[Index].CddId    = (UINT8)(CddLoop & 0xFF);
            RasCpuMap[Index].CcxId    = (UINT8)(ComplexLoop & 0xFF);
            RasCpuMap[Index].CoreId   = (UINT8)(CoreLoop & 0xFF);
            RasCpuMap[Index].ThreadID = 1;
            Index++;
          }
        }
      }
    }
  }

  mHygonRasPolicy->TotalNumberOfProcessors = Index;

  // Update Ras CPU map pointer to HYGON RAS Policy buffer.
  mHygonRasPolicy->RasCpuMap = RasCpuMap;

  return (Status);
}
