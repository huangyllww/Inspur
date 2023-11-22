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
#include "PiPei.h"
#include <Filecode.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/HygonIdsHookLib.h>
#include <Library/PcdLib.h>
#include <IdsHookId.h>
#include <IdsNvIdSAT.h>
#include <IdsNvDefSAT.h>
#include <HygonSmuBiosIf.h>

#define FILECODE  LIBRARY_NBIOIDSHOOKSTLIB_NBIOIDSHOOKSTLIB_FILECODE

/**
 *
 * IDS HOOK for SMU feature enable
 *
 *
 *
 * @param[in] HookId          IDS HOOK ID
 * @param[in] Handle          Handle
 * @param[in] Data            Data
 * @retval    HGPI_STATUS
 *
 **/
IDS_HOOK_STATUS
SmuSTBeforeFeatureEnable (
  HOOK_ID   HookId,
  VOID      *Handle,
  VOID      *Data
  )
{
  UINT64      IdsNvValue;
  UINT32      Value;
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;

  Value = PcdGet32 (PcdSmuFeatureControlDefines);

  // Set DF C-STATES
  if (PcdGet8 (PcdHygonDfCStateMode) == 1) {
    Value |= (UINT32)(FEATURE_DF_CSTATES_MASK);
  } else {
    Value &= (UINT32)(~FEATURE_DF_CSTATES_MASK);
  }

  // Set CORE C-STATES
  if (PcdGet8 (PcdHygonCStateMode) == 1) {
    // Enable CC1, disable CC6
    Value |= (UINT32)(FEATURE_CC1_MASK);
    Value &= (UINT32)(~FEATURE_CC6_MASK);
  } else if (PcdGet8 (PcdHygonCStateMode) == 2) {
    // Enable CC1 and CC6
    Value |= (UINT32)(FEATURE_CC1_MASK + FEATURE_CC6_MASK);
  } else {
    // Disable CC1 and CC6
    Value &= (UINT32)(~(FEATURE_CC1_MASK + FEATURE_CC6_MASK));
  }

  PcdSet32S (PcdSmuFeatureControlDefines, Value);

  // Disable CORE C-STATES if IBS is enabled
  IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_EN_IBS, &IdsNvValue) {
    switch (IdsNvValue) {
      case IDSOPT_CMN_CPU_EN_IBS_AUTO:
        break;
      case IDSOPT_CMN_CPU_EN_IBS_ENABLED:
        // SMU Feature Enable/Disable
        Value = PcdGet32 (PcdSmuFeatureControlDefines);
        // Core Cstates Disable
        Value &= (UINT32)(~FEATURE_CC6_MASK);
        PcdSet32S (PcdSmuFeatureControlDefines, Value);
        break;
      case IDSOPT_CMN_CPU_EN_IBS_DISABLED:
        break;
      default:
        ASSERT (FALSE);
        break;
    }
  }

  // cTDP Control
  IDS_NV_READ_SKIP (IDSNVID_CMNC_TDP_CTL, &IdsNvValue) {
    switch (IdsNvValue) {
      case IDSOPT_CMNC_TDP_CTL_MANUAL:
        IDS_NV_READ_SKIP (IDSNVID_CMNC_TDP_LIMIT, &IdsNvValue) {
          PcdSet32S (PcdHygoncTDP, (UINT32)IdsNvValue);
        }
        break;
      case IDSOPT_CMNC_TDP_CTL_AUTO:
        break;
      default:
        ASSERT (FALSE);
        break;
    }
  }

  //TDP Boost
  IDS_NV_READ_SKIP (IDSNVID_CMN_TDP_BOOST, &IdsNvValue) {
    if (IdsNvValue != IDSOPT_CMN_TDP_BOOST_AUTO) {
      PcdSet8S (PcdHygonTDPBoost, (UINT8)IdsNvValue);
    }
  }

  return Status;
}

/**
 *
 * IDS HOOK for NBIO load build options
 *
 *
 *
 * @param[in] HookId          IDS HOOK ID
 * @param[in] Handle          Handle
 * @param[in] Data            Data
 * @retval    HGPI_STATUS
 *
 **/
IDS_HOOK_STATUS
IdsHookNbioLoadBuildOption (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
  )
{
  UINT64  IdsNvValue;

  // IOMMU
  IDS_NV_READ_SKIP (IDSNVID_CMN_GNB_NB_IOMMU, &IdsNvValue) {
    if (IdsNvValue != IDSOPT_CMN_GNB_NB_IOMMU_AUTO) {
      PcdSetBoolS (PcdCfgIommuSupport, (BOOLEAN)IdsNvValue);
    }
  }

  // NBIO Internal Poison Consumption
  IDS_NV_READ_SKIP (IDSNVID_DBG_POISON_CONSUMPTION, &IdsNvValue) {
    if (IdsNvValue != IDSOPT_DBG_POISON_CONSUMPTION_AUTO) {
      PcdSetBoolS (PcdHygonNbioPoisonConsumption, (BOOLEAN)IdsNvValue);
    }
  }

  // NBIO RAS Control
  IDS_NV_READ_SKIP (IDSNVID_NBIO_RAS_CONTROL, &IdsNvValue) {
    if (IdsNvValue != IDSOPT_DBG_RAS_CONTROL_AUTO) {
      PcdSetBoolS (PcdHygonNbioRASControl, (BOOLEAN)IdsNvValue);
    }
  }

  // HD Audio Enable
  IDS_NV_READ_SKIP (IDSNVID_CMN_GNB_HD_AUDIO_EN, &IdsNvValue) {
    if (IdsNvValue != IDSOPT_CMN_GNB_HD_AUDIO_EN_AUTO) {
      PcdSetBoolS (PcdCfgHdAudioEnable, (BOOLEAN)IdsNvValue);
    }
  }

  return IDS_HOOK_SUCCESS;
}

#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define NBIO_ST_IDS_HOOKS_INT
#else
  #include <Internal/NbioIdsHookSTLibInt.h>
#endif

IDS_HOOK_ELEMENT  NbioSTIdsHooks[] = {
  {
    IDS_HOOK_NBIO_SMU_INIT_CMN,
    &SmuSTBeforeFeatureEnable
  },
  {
    IDS_HOOK_NBIO_LOAD_BUILD_OPTION,
    &IdsHookNbioLoadBuildOption
  },
  NBIO_ST_IDS_HOOKS_INT
  IDS_HOOKS_END
};

IDS_HOOK_TABLE  NbioSTIdsHookTable = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  NbioSTIdsHooks
};

HGPI_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &NbioSTIdsHookTable;
  return HGPI_SUCCESS;
}
