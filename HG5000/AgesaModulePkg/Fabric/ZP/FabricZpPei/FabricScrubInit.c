/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric DRAM Scrub initialization.
 *
 * This funtion initializes the scrub features of PIE.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ****************************************************************************
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Porting.h"
#include "AMD.h"
#include <Library/IdsLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersZP.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/AmdPspApobLib.h>
#include <Library/BaseMemoryLib.h>
#include "Filecode.h"
#include "FabricScrubInit.h"

#define FILECODE FABRIC_ZP_FABRICZPPEI_FABRICSCRUBINIT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define NUM_SCRUB_RATE_SETTINGS             16
#define mGetIntLvNumSockets(DramLimitAddr)  (DramLimitAddr.Field.IntLvNumSockets + 1)
#define mGetIntLvNumDies(DramLimitAddr)     (1 << DramLimitAddr.Field.IntLvNumDies)

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
UINT64 GetDramScrubSize (UINTN Socket, UINTN Die);
UINTN  MatchScrubRate (UINT64 ScrubRateInNSec);

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
// In nano seconds
UINT64 DramScrubRateEncodings[NUM_SCRUB_RATE_SETTINGS] = {
  640,      // 0, 640ns
  1280,     // 1, 1.28us
  2560,     // 2, 2.56us
  5120,     // 3, 5.12us
  10200,    // 4, 10.2us
  20500,    // 5, 20.5us
  41000,    // 6, 41.0us
  81900,    // 7, 81.9us
  163800,   // 8, 163.8us
  327700,   // 9, 327.7us
  655400,   // A, 655.4us
  1310000,  // B, 1.31ms
  2620000,  // C, 2.62ms
  5240000,  // D, 5.24ms
  10490000, // E, 10.49ms
  20970000, // F, 20.97ms
 };

/* -----------------------------------------------------------------------------*/
/**
 *
 *  FabricScrubInit
 *
 *  Description:
 *    This funtion initializes the DRAM scrub features of PIE.
 *
 */
VOID
FabricScrubInit (
  IN       CONST EFI_PEI_SERVICES       **PeiServices,
  IN       AMD_PEI_SOC_LOGICAL_ID_PPI   *SocLogicalIdPpi
  )
{
  UINTN                                            Socket;
  UINTN                                            Die;
  UINTN                                            NumOfProcessorPresent;
  UINTN                                            NumOfDiesOnSocket;
  UINTN                                            ScrubRateIndex;
  UINT64                                           DramScrubSizeOnDie;
  UINTN                                            ScrubTimeInHour;
  UINTN                                            i;
  UINT64                                           ScrubTimeInNSec;
  UINT64                                           ScrubRateInNSec;
  UINT32                                           ApobInstanceId;
  BOOLEAN                                          ScrubRedirection;
  BOOLEAN                                          IsEccEnabled;
  APOB_TYPE_HEADER                                 *ApobEntry;
  REDIRECT_SCRUB_CTRL_REGISTER                     RedirectScrubCtrl;
  DRAM_SCRUB_BASE_ADDR_REGISTER                    DramScrubBaseAddr;
  DRAM_SCRUB_LIMIT_ADDR_REGISTER                   DramScrubLimitAddr;
  APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE_STRUCT  ApobMemGenCfg;

  // Time = (MemSizeInBlk) * (DramScrubRate).
  //
  // Calculation methods:
  // 1. TimeHr * 1.1
  // 2. round to closest and faster time with a scrub rate

  ScrubRedirection = PcdGetBool (PcdAmdFabricEccScrubRedirection);
  IDS_HOOK (IDS_HOOK_PIE_REDIRECT_SCRUB_CTRL_INIT, NULL, (VOID *) &ScrubRedirection);

  ScrubTimeInHour = PcdGet8 (PcdAmdFabricDramScrubTime);
  IDS_HOOK (IDS_HOOK_PIE_DRAM_SCRUB_TIME_INIT, NULL, (VOID *) &ScrubTimeInHour);

  NumOfProcessorPresent = FabricTopologyGetNumberOfProcessorsPresent ();
  for (Socket = 0; Socket < NumOfProcessorPresent; Socket++) {
    NumOfDiesOnSocket = FabricTopologyGetNumberOfDiesOnSocket (Socket);
    for (Die = 0; Die < NumOfDiesOnSocket; Die++) {
      DramScrubSizeOnDie = GetDramScrubSize (Socket, Die);
      if (DramScrubSizeOnDie == 0) {
        continue;
      }

      ApobInstanceId = ((UINT32) Socket << 8) | (UINT32) Die;
      if (AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE, ApobInstanceId, FALSE, &ApobEntry) != EFI_SUCCESS) {
        continue;
      }
      CopyMem (&ApobMemGenCfg, ApobEntry, sizeof (APOB_MEM_GENERAL_CONFIGURATION_INFO_TYPE_STRUCT));

      IsEccEnabled = FALSE;
      for (i = 0; i < ABL_APOB_MAX_CHANNELS_PER_DIE; i++) {
        if (ApobMemGenCfg.EccEnable[i]) {
          IsEccEnabled = TRUE;
        }
      }

      if (!IsEccEnabled) {
        continue;
      }

      if (ScrubRedirection) {
        for (i = 0; i < ABL_APOB_MAX_CHANNELS_PER_DIE; i++) {
          RedirectScrubCtrl.Value = FabricRegisterAccRead (Socket, Die, REDIRECTSCRUBCTRL_FUNC, REDIRECTSCRUBCTRL_REG, (ZP_CS0_INSTANCE_ID + i));
          RedirectScrubCtrl.Fields.EnRedirScrub = (ScrubRedirection && ApobMemGenCfg.EccEnable[i]) ? 1 : 0;
          FabricRegisterAccWrite (Socket, Die, REDIRECTSCRUBCTRL_FUNC, REDIRECTSCRUBCTRL_REG, (ZP_CS0_INSTANCE_ID + i), RedirectScrubCtrl.Fields.EnRedirScrub, FALSE);
        }
      }

      if (ScrubTimeInHour == 0) {
        // Disable periodic scrubber
        continue;
      }

      ScrubTimeInNSec = MultU64x32 (3960000000000ull, (UINT32) ScrubTimeInHour); // 60 * 60 * 1000 * 1000 * 1000 * 11 Add 10% (* 11 / 10)
      ScrubRateInNSec = DivU64x64Remainder (ScrubTimeInNSec, RShiftU64 (DramScrubSizeOnDie, 6), NULL); // 64 bytes per block
      ScrubRateIndex = MatchScrubRate (ScrubRateInNSec);

      DramScrubLimitAddr.Value = FabricRegisterAccRead (Socket, Die, DRAMSCRUBLIMITADDR_FUNC, DRAMSCRUBLIMITADDR_REG, FABRIC_REG_ACC_BC);
      DramScrubLimitAddr.Fields.DramScrubRate = ScrubRateIndex;
      FabricRegisterAccWrite (Socket, Die, DRAMSCRUBLIMITADDR_FUNC, DRAMSCRUBLIMITADDR_REG, FABRIC_REG_ACC_BC, DramScrubLimitAddr.Value, FALSE);
      IDS_HDT_CONSOLE (CPU_TRACE, "    DramScrubRate is set to 0x%x on Socket %d Die %d\n", ScrubRateIndex, Socket, Die);

      // Enable scrubber
      DramScrubBaseAddr.Value = FabricRegisterAccRead (Socket, Die, DRAMSCRUBBASEADDR_FUNC, DRAMSCRUBBASEADDR_REG, FABRIC_REG_ACC_BC);
      DramScrubBaseAddr.Fields.DramScrubEn = 1;
      FabricRegisterAccWrite (Socket, Die, DRAMSCRUBBASEADDR_FUNC, DRAMSCRUBBASEADDR_REG, FABRIC_REG_ACC_BC, DramScrubBaseAddr.Value, FALSE);
    }
  }
}


UINT64
GetDramScrubSize (
  IN       UINTN Socket,
  IN       UINTN Die
  )
{

  DRAM_SCRUB_BASE_ADDR_REGISTER   DramScrubBaseAddr;
  DRAM_SCRUB_LIMIT_ADDR_REGISTER  DramScrubLimitAddr;
  DRAM_BASE_ADDRESS_REGISTER      DramBaseAddr;
  DRAM_LIMIT_ADDRESS_REGISTER     DramLimitAddr;
  DRAM_HOLE_CONTROL_REGISTER      DramHoleControl;
  UINT64                          DramScrubSize;
  UINT32                          AddrMapPair;

  DramScrubBaseAddr.Value = FabricRegisterAccRead (Socket, Die, DRAMSCRUBBASEADDR_FUNC, DRAMSCRUBBASEADDR_REG, FABRIC_REG_ACC_BC);
  DramScrubLimitAddr.Value = FabricRegisterAccRead (Socket, Die, DRAMSCRUBLIMITADDR_FUNC, DRAMSCRUBLIMITADDR_REG, FABRIC_REG_ACC_BC);
  if (DramScrubLimitAddr.Value == 0) {
    return (0);
  }
  DramScrubSize = LShiftU64 ((UINT64) ((DramScrubLimitAddr.Fields.DramScrubLimitAddr + 1) - DramScrubBaseAddr.Fields.DramScrubBaseAddr), 20);

  for (AddrMapPair = 0; AddrMapPair < 2; AddrMapPair++) {
    DramBaseAddr.Value = FabricRegisterAccRead (Socket, Die, DRAMBASEADDR0_FUNC, DRAMBASEADDR0_REG + (AddrMapPair << 3), ZP_PIE_INSTANCE_ID);
    if (DramBaseAddr.Field.AddrRngVal == 0) {
      continue;
    }
    DramLimitAddr.Value = FabricRegisterAccRead (Socket, Die, DRAMLIMITADDR0_FUNC, DRAMLIMITADDR0_REG + (AddrMapPair << 3), ZP_PIE_INSTANCE_ID);
    if ((DramLimitAddr.Field.DstFabricID & 0xE0) != (Socket << 7 | Die << 5)) {
      continue;
    }

    // There should be only one of LgcyMmioHoleEn set in the maps.
    if (DramBaseAddr.Field.LgcyMmioHoleEn == 1) {
      DramHoleControl.Value = FabricRegisterAccRead (Socket, Die, DRAMHOLECTRL_FUNC, DRAMHOLECTRL_REG, ZP_PIE_INSTANCE_ID);
      if (DramHoleControl.Field.DramHoleValid == 1) {
        DramScrubSize -= (0x100 - DramHoleControl.Field.DramHoleBase) << 24;
      }
    }
  }

  return (DramScrubSize);
}


UINTN
MatchScrubRate (
  IN       UINT64 ScrubRateInNSec
  )
{
  UINTN   Index;
  UINTN   ScrubRateIndex;

  ScrubRateIndex = 0;
  for (Index =   0; Index < NUM_SCRUB_RATE_SETTINGS; Index++) {
    if (ScrubRateInNSec >= DramScrubRateEncodings[Index]) {
      ScrubRateIndex = Index;
    } else {
      break;
    }
  }
  return (ScrubRateIndex);
}
