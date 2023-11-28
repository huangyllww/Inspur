/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric IDS hook implementation
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision$   @e \$Date$
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
#include "FabricRegistersZP.h"
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "Library/AmdIdsHookLib.h"
#include "Library/BaseFabricTopologyLib.h"
#include "Library/FabricRegisterAccLib.h"
#include "IdsHookId.h"
#include "IdsNvIdZP.h"
#include "IdsNvDefZP.h"
#include "Filecode.h"

#define FILECODE LIBRARY_FABRICIDSHOOKZPLIB_PEI_FABRICIDSHOOKZPLIBPEI_FILECODE


/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

IDS_HOOK_STATUS
FabricZpPeiDfSyncFlood (
  HOOK_ID HookId,
  VOID    *Handle,
  VOID    *Data
  )
{
  UINTN   i;
  UINTN   j;
  UINT32  RegData;
  UINT32  RegValue;
  UINT64  IdsNvValue;

  IDS_HDT_CONSOLE (CPU_TRACE, "FabricZpPeiSyncFlood Option\n");

  RegData = 0x00000000;
  //Disable DF sync flood propagation
  IDS_NV_READ_SKIP (IDSNVID_DF_CMN_SYNC_FLOOD_PROP, &IdsNvValue) {
    switch (IdsNvValue) {
    case IDSOPT_DF_CMN_SYNC_FLOOD_PROP_SYNCFLOODDISABLED:
      RegData = 0x00080000;
      break;
    case IDSOPT_DF_CMN_SYNC_FLOOD_PROP_SYNCFLOODENABLED:
      RegData = 0x00000000;
      break;
    case IDSOPT_DF_CMN_SYNC_FLOOD_PROP_AUTO:
      return IDS_HOOK_SUCCESS;
    default:
      ASSERT (FALSE);
      break;
    }
  }

  // Disable DF sync flood propagation
  // D18F5x104[19]
  for (i = 0; i < FabricTopologyGetNumberOfProcessorsPresent (); i++) {
    for (j = 0; j < FabricTopologyGetNumberOfDiesOnSocket (i); j++) {
      RegValue = FabricRegisterAccRead (i, j, 0x5, 0x104, FABRIC_REG_ACC_BC);
      RegValue &= 0xFFF7FFFF;
      RegValue |= RegData;
      FabricRegisterAccWrite (i, j, 0x5, 0x104, FABRIC_REG_ACC_BC, RegValue, FALSE);
    }
  }
  return IDS_HOOK_SUCCESS;
}

IDS_HOOK_STATUS
FabricZpPeiDfFreezeModuleQueuesOnError (
  HOOK_ID HookId,
  VOID    *Handle,
  VOID    *Data
  )
{
  UINT64                IdsNvValue;
  DF_GLBL_CTRL_REGISTER *PDfGlblCtrl;

  IDS_HDT_CONSOLE (CPU_TRACE, "FabricZpFreezeModuleQueuesOnError Option\n");

  //Disable DF sync flood propagation
  // D18F03F8[3]
  PDfGlblCtrl = (DF_GLBL_CTRL_REGISTER *) Data;
  IDS_NV_READ_SKIP (IDSNVID_DF_CMN_FREEZE_QUEUE_ERROR, &IdsNvValue) {
    switch (IdsNvValue) {
    case IDSOPT_DF_CMN_FREEZE_QUEUE_ERROR_DISABLED:
      PDfGlblCtrl->Field.DisImmSyncFloodOnFatalErr = 1;
      break;
    case IDSOPT_DF_CMN_FREEZE_QUEUE_ERROR_ENABLED:
      break;
    case IDSOPT_DF_CMN_FREEZE_QUEUE_ERROR_AUTO:
      break;
    default:
      ASSERT (FALSE);
      break;
    }
  }
  return IDS_HOOK_SUCCESS;
}


IDS_HOOK_STATUS
FabricZpPeiDfRedirectScrubCtrl (
  HOOK_ID HookId,
  VOID    *Handle,
  VOID    *Data
  )
{
  UINT64                IdsNvValue;
  BOOLEAN               *PDfRedirScrubCtrl;

  IDS_HDT_CONSOLE (CPU_TRACE, "FabricZpPeiRedirectScrubCtrl Option\n");

  //Redirect scrubber control
  // D18F6x58[0]
  PDfRedirScrubCtrl = (BOOLEAN *) Data;
  IDS_NV_READ_SKIP (IDSNVID_DF_CMN_REDIR_SCRUB_CTRL, &IdsNvValue) {
    switch (IdsNvValue) {
    case IDSOPT_DF_CMN_REDIR_SCRUB_CTRL_DISABLED:
      *PDfRedirScrubCtrl = 0;
      break;
    case IDSOPT_DF_CMN_REDIR_SCRUB_CTRL_ENABLED:
      *PDfRedirScrubCtrl = 1;
      break;
    case IDSOPT_DF_CMN_REDIR_SCRUB_CTRL_AUTO:
      break;
    default:
      ASSERT (FALSE);
      break;
    }
  }
  return IDS_HOOK_SUCCESS;
}


IDS_HOOK_STATUS
FabricZpPeiDfDramScrubTime (
  HOOK_ID HookId,
  VOID    *Handle,
  VOID    *Data
  )
{
  UINT64                IdsNvValue;
  UINT8                 *PDfDramScrubTime;

  IDS_HDT_CONSOLE (CPU_TRACE, "FabricZpPeiDramScrubTime Option\n");

  //DRAM scrub time
  PDfDramScrubTime = (UINT8 *) Data;
  IDS_NV_READ_SKIP (IDSNVID_DF_CMN_DRAM_SCRUB_TIME, &IdsNvValue) {
    switch (IdsNvValue) {
    case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_DISABLED:
      *PDfDramScrubTime = 0;
      break;
    case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_1HOUR:
      *PDfDramScrubTime = 1;
      break;
    case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_4HOURS:
      *PDfDramScrubTime = 4;
      break;
    case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_8HOURS:
      *PDfDramScrubTime = 8;
      break;
    case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_16HOURS:
      *PDfDramScrubTime = 16;
      break;
    case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_24HOURS:
      *PDfDramScrubTime = 24;
      break;
    case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_48HOURS:
      *PDfDramScrubTime = 48;
      break;
    case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_AUTO:
      break;
    default:
      ASSERT (FALSE);
      break;
    }
  }
  return IDS_HOOK_SUCCESS;
}


#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define FABRIC_ZP_IDS_HOOKS_INT_PEI
#else
  #include "Internal/FabricIdsHookZpLibIntPei.h"
#endif

IDS_HOOK_ELEMENT FabricZpIdsHooksPei[] = {
  {
    IDS_HOOK_DF_RAS_INIT,
    &FabricZpPeiDfSyncFlood
  },
  {
    IDS_HOOK_DF_RAS_INIT2,
    &FabricZpPeiDfFreezeModuleQueuesOnError
  },
  {
    IDS_HOOK_PIE_REDIRECT_SCRUB_CTRL_INIT,
    &FabricZpPeiDfRedirectScrubCtrl
  },
  {
    IDS_HOOK_PIE_DRAM_SCRUB_TIME_INIT,
    &FabricZpPeiDfDramScrubTime
  },
  FABRIC_ZP_IDS_HOOKS_INT_PEI
  IDS_HOOKS_END
};

IDS_HOOK_TABLE FabricZpIdsHookTablePei = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  FabricZpIdsHooksPei
};


AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &FabricZpIdsHookTablePei;
  return AGESA_SUCCESS;
}