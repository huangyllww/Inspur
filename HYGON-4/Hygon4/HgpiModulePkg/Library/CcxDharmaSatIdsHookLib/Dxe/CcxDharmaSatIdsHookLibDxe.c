/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CCX Dharma - Satori API, and related functions.
 *
 * Contains code that initializes the core complex
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  CCX
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
#include "PiDxe.h"
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include "CcxDharmaSatIdsSyncMsr.h"
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/HygonIdsHookLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/CcxRolesLib.h>
#include <Library/UefiLib.h>
#include <Protocol/MpService.h>
#include <Protocol/SmmControl2.h>
#include "Guid/EventGroup.h"
#include <IdsHookId.h>
#include "IdsNvIdSAT.h"
#include "IdsNvDefSAT.h"
#include "CcxDharmaSatIdsCustomPstates.h"
#include <Library/HygonSocBaseLib.h>
#include "Filecode.h"
#include <Protocol/SocLogicalIdProtocol.h>
#include <SocLogicalId.h>
extern  EFI_BOOT_SERVICES  *gBS;

#define FILECODE  LIBRARY_CCXDHARMASATIDSHOOKLIB_DXE_CCXDHARMASATIDSHOOKLIBDXE_FILECODE

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
EFI_EVENT  CcxIdsDharmaDxeMpServiceEvent;

VOID  *CcxIdsDharmaForMpServiceEvent;

#define IDS_MAX_NUM_OF_SYNC_MSR  20

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
EFIAPI
CcxIdsDharmaSatDxeMpServiceCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
IDS_HOOK_STATUS
CcxDharmaSatDxeAfterApLaunch (
  HOOK_ID   HookId,
  VOID      *Handle,
  VOID      *Data
  )
{
  UINT64               IdsNvValue;
  HYGON_CONFIG_PARAMS  StdHeader;
  BOOLEAN              CtrlOpCacheThruIcCfg;

  if (CcxIsBsp (&StdHeader)) {
    IDS_HDT_CONSOLE (CPU_TRACE, "CcxDharmaSatDxeAfterApLaunch External Options\n");

    // Disable uop Cache
    CtrlOpCacheThruIcCfg = TRUE;

    if (CtrlOpCacheThruIcCfg) {
      IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_OPCACHE_CTRL, &IdsNvValue) {
        switch (IdsNvValue) {
          case IDSOPT_CMN_CPU_OPCACHE_CTRL_AUTO:
            break;
          case IDSOPT_CMN_CPU_OPCACHE_CTRL_DISABLED:
            // MSR C001_1021[5] = 1
            AsmMsrOr64 (0xC0011021, BIT5);
            break;
          case IDSOPT_CMN_CPU_OPCACHE_CTRL_ENABLED:
            // MSR C001_1021[5] = 0
            AsmMsrAnd64 (0xC0011021, ~((UINT64)BIT5));
            break;
          default:
            ASSERT (FALSE);
            break;
        }
      }
    }

    // Enable IBS
    IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_EN_IBS, &IdsNvValue) {
      switch (IdsNvValue) {
        case IDSOPT_CMN_CPU_EN_IBS_AUTO:
          break;
        case IDSOPT_CMN_CPU_EN_IBS_ENABLED:
          // MSRC001_1005[42]=1
          AsmMsrOr64 (0xC0011005, BIT42);
          // MSRC001_1020[54]=1
          AsmMsrOr64 (0xC0011020, BIT54);
          //// MSRC001_1029[19]=1
          // AsmMsrOr64 (0xC0011029, BIT19);
          break;
        case IDSOPT_CMN_CPU_EN_IBS_DISABLED:
          // MSRC001_1005[42]=0
          AsmMsrAnd64 (0xC0011005, ~((UINT64)BIT42));
          // MSRC001_1020[54]=0
          AsmMsrAnd64 (0xC0011020, ~((UINT64)BIT54));
          //// MSRC001_1029[19]=0
          // AsmMsrAnd64 (0xC0011029, ~((UINT64)BIT19));
          break;
        default:
          ASSERT (FALSE);
          break;
      }
    }

    // Streaming Stores Control
    IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_STREAMING_STORES_CTRL, &IdsNvValue) {
      switch (IdsNvValue) {
        case IDSOPT_CMN_CPU_STREAMING_STORES_CTRL_AUTO:
          break;
        case IDSOPT_CMN_CPU_STREAMING_STORES_CTRL_ENABLED:
          // MSRC001_1020[28]=0
          AsmMsrAnd64 (0xC0011020, ~((UINT64)BIT28));
          break;
        case IDSOPT_CMN_CPU_STREAMING_STORES_CTRL_DISABLED:
          // MSRC001_1020[28]=1
          AsmMsrOr64 (0xC0011020, BIT28);
          break;
        default:
          ASSERT (FALSE);
          break;
      }
    }    
  }

  //
  // Set up call back after gEfiMpServiceProtocolGuid.
  //
  CcxIdsDharmaDxeMpServiceEvent = EfiCreateProtocolNotifyEvent (
                                    &gEfiMpServiceProtocolGuid,
                                    TPL_NOTIFY,
                                    CcxIdsDharmaSatDxeMpServiceCallBack,
                                    NULL,
                                    &CcxIdsDharmaForMpServiceEvent
                                    );

  IDS_HDT_CONSOLE (CPU_TRACE, "Create  CcxIdsDharmaDxeMpServiceEvent status \n");
  if (CcxIdsDharmaDxeMpServiceEvent != NULL) {
    return IDS_HOOK_SUCCESS;
  } else {
    return IDS_HOOK_ERROR;
  }
}

/*---------------------------------------------------------------------------------------*/

/**
 * CcxIdsDharmaSatDxeMpServiceCallBack
 *
 *
 *  Parameters:
 *    @param[in]     Event
 *    @param[in]     *Context
 *
 *    @retval        VOID
 */
VOID
EFIAPI
CcxIdsDharmaSatDxeMpServiceCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                Status;
  EFI_MP_SERVICES_PROTOCOL  *MpServices;
  UINT16                    i;
  IDS_BSC_AP_MSR_SYNC       ApMsrSync[IDS_MAX_NUM_OF_SYNC_MSR];
  HYGON_CONFIG_PARAMS       StdHeader;

  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &MpServices);
  if (EFI_ERROR (Status)) {
    return;
  }

  gBS->CloseEvent (Event);

  ZeroMem (ApMsrSync, sizeof (ApMsrSync));

  i = 0;

  ApMsrSync[i].RegisterAddress = 0xC0011029;
  ApMsrSync[i++].RegisterMask  = (BIT19 | BIT14);

  ApMsrSync[i].RegisterAddress = 0xC0011023;
  ApMsrSync[i++].RegisterMask  = BIT45;

  ApMsrSync[i].RegisterAddress = 0xC0011005;
  ApMsrSync[i++].RegisterMask  = BIT42;

  ApMsrSync[i].RegisterAddress = 0xC0011020;
  ApMsrSync[i++].RegisterMask  = BIT54 | BIT28;

  ApMsrSync[i].RegisterAddress = 0xC0011021;
  ApMsrSync[i++].RegisterMask  = BIT5;

  ApMsrSync[i].RegisterAddress = 0xC0011022;
  ApMsrSync[i++].RegisterMask  = BIT16;

  ApMsrSync[i].RegisterAddress = 0xC001102B;
  ApMsrSync[i++].RegisterMask  = BIT0;

  // End of sync-up list

  ASSERT (i < IDS_MAX_NUM_OF_SYNC_MSR);
  if (i >= IDS_MAX_NUM_OF_SYNC_MSR) {
    IDS_HDT_CONSOLE (CPU_TRACE, "ERROR: IDS ApMsrSync overflow\n");
    IDS_DEADLOOP ()
  }

  CcxIdsDharmaSatDxeSyncMsr (ApMsrSync, &StdHeader);
}

/*---------------------------------------------------------------------------------------*/

/**
 * CcxDharmaSatDxeSkipOcEnable
 *
 *
 *  Parameters:
 *    @param[in]     HookId
 *    @param[in]     *Handle
 *    @param[in]     *Data
 *
 *    @retval        IDS_HOOK_STATUS
 */
IDS_HOOK_STATUS
CcxDharmaSatDxeSkipOcEnable (
  IN       HOOK_ID HookId,
  IN       VOID *Handle,
  IN       VOID *Data
  )
{
  UINT64  IdsNvValue;

  IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_OPCACHE_CTRL, &IdsNvValue) {
    if (IdsNvValue == IDSOPT_CMN_CPU_OPCACHE_CTRL_DISABLED) {
      return IDS_HOOK_NO_SKIP;
    }
  }
  return IDS_HOOK_SKIP;
}

IDS_HOOK_STATUS
SmeeEnable (
  IN       HOOK_ID HookId,
  IN       VOID *Handle,
  IN       VOID *Data
  )
{
  HYGON_SOC_LOGICAL_ID_PROTOCOL  *SocLogicalId;
  SOC_LOGICAL_ID                 LogicalId;
  EFI_STATUS                     Status;

  // get cpu model/stepping

  // Ax do not support SMEE
  Status = gBS->LocateProtocol (&gHygonSocLogicalIdProtocolGuid, NULL, (VOID **)&SocLogicalId);

  if (!EFI_ERROR (Status)) {
    SocLogicalId->GetLogicalIdOnCurrentCore (SocLogicalId, &LogicalId);
    if ((LogicalId.Family == HYGON_FAMILY_18) && (LogicalId.Revision != HYGON_REV_F18_DN_Ax)) {
      return IDS_HOOK_NO_SKIP;
    }
  }

  return IDS_HOOK_SKIP;   // If Ax or fail to get gHygonSocLogicalIdProtocolGuid, keep smee as default value
}

#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define CCX_IDS_HOOKS_INT
#else
  #include <Internal/CcxDharmaSatIdsHookLibIntDxe.h>
#endif

IDS_HOOK_ELEMENT  CcxDharmaSatIdsHooks[] = {
  {
    IDS_HOOK_CCX_AFTER_AP_LAUNCH,
    &CcxDharmaSatDxeAfterApLaunch
  },
  {
    IDS_HOOK_CCX_CUSTOM_PSTATES,
    &CcxDharmaSatIdsDxeCustomPstates
  },
  {
    IDS_HOOK_CCX_SKIP_OC_ENABLE,
    &CcxDharmaSatDxeSkipOcEnable
  },
  {// Check SMEE
    IDS_HOOK_CCX_SKIP_SMEE,
    &SmeeEnable
  },

  CCX_IDS_HOOKS_INT
  IDS_HOOKS_END
};

IDS_HOOK_TABLE  CcxDharmaSatIdsHookTable = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  CcxDharmaSatIdsHooks
};

HGPI_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &CcxDharmaSatIdsHookTable;
  return HGPI_SUCCESS;
}
