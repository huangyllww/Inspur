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
#include "PiPei.h"
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/PcdLib.h>
#include <IdsHookId.h>
#include <IdsNvIdZP.h>
#include <IdsNvDefZP.h>
//BYO Modify for RAS >>
#include <GnbPcieAer.h>
//BYO Modify for RAS <<

#define FILECODE LIBRARY_NBIOIDSHOOKZPLIB_NBIOIDSHOOKZPLIB_FILECODE

/**
 *
 * IDS HOOK for SMU feature enable
 *
 *
 *
 * @param[in] HookId          IDS HOOK ID
 * @param[in] Handle          Handle
 * @param[in] Data            Data
 * @retval    AGESA_STATUS
 *
 **/
IDS_HOOK_STATUS
SmuZPBeforeFeatureEnable (
  HOOK_ID   HookId,
  VOID      *Handle,
  VOID      *Data
  )
{
  UINT64            IdsNvValue;
  UINT32            Value;
  EFI_STATUS        Status;

  Status = EFI_SUCCESS;

  //Disable CORE C-STATES if IBS is enabled
  IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_EN_IBS, &IdsNvValue) {
    switch (IdsNvValue) {
    case IDSOPT_CMN_CPU_EN_IBS_AUTO:
      break;
    case IDSOPT_CMN_CPU_EN_IBS_ENABLED:
      //SMU Feature Enable/Disable
      Value = PcdGet32 (PcdSmuFeatureControlDefines);
      //Core Cstates Disable
      Value &= (UINT32) (~BIT8);
      PcdSet32 (PcdSmuFeatureControlDefines, Value);
      break;
    case IDSOPT_CMN_CPU_EN_IBS_DISABLED:
      break;
    default:
      ASSERT (FALSE);
      break;
    }
  }

  //cTDP Control
  IDS_NV_READ_SKIP (IDSNVID_CMNC_TDP_CTL, &IdsNvValue) {
    switch (IdsNvValue) {
    case IDSOPT_CMNC_TDP_CTL_MANUAL:
      IDS_NV_READ_SKIP (IDSNVID_CMNC_TDP_LIMIT, &IdsNvValue) {
        PcdSet32 (PcdAmdcTDP, (UINT32)IdsNvValue);
      }
      break;
    case IDSOPT_CMNC_TDP_CTL_AUTO:
      break;
    default:
      ASSERT (FALSE);
      break;
    }
  }

  //Fan Control
  IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_CTL, &IdsNvValue) {
    switch (IdsNvValue) {
    case IDSOPT_DBG_FAN_CTL_MANUAL:
      IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_CTL, &IdsNvValue) {
        PcdSet8 (PcdFanTableOverride, (UINT8)IdsNvValue);
        IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_TEMP_LOW, &IdsNvValue) {
          PcdSet8 (PcdFanTableTempLow, (UINT8)IdsNvValue);
        }
        IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_TEMP_MED, &IdsNvValue) {
          PcdSet8 (PcdFanTableTempMed, (UINT8)IdsNvValue);
        }
        IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_TEMP_HIGH, &IdsNvValue) {
          PcdSet8 (PcdFanTableTempHigh, (UINT8)IdsNvValue);
        }
        IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_TEMP_CRITICAL, &IdsNvValue) {
          PcdSet8 (PcdFanTableTempCritical, (UINT8)IdsNvValue);
        }
        IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_T_PWM_LOW, &IdsNvValue) {
          PcdSet8 (PcdFanTablePwmLow, (UINT8)IdsNvValue);
        }
        IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_PWM_MED, &IdsNvValue) {
          PcdSet8 (PcdFanTablePwmMed, (UINT8)IdsNvValue);
        }
        IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_PWM_HIGH, &IdsNvValue) {
          PcdSet8 (PcdFanTablePwmHigh, (UINT8)IdsNvValue);
        }
        IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_HYST, &IdsNvValue) {
          PcdSet8 (PcdFanTableHysteresis, (UINT8)IdsNvValue);
        }
        //Fan Polarity
        IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_POLARITY, &IdsNvValue) {
        PcdSet8 (PcdFanTablePolarity, (UINT8)IdsNvValue);
        }
      }
      //Pwm Frequency
      IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_PWM_FREQ, &IdsNvValue) {
        PcdSet8 (PcdFanTablePwmFreq, (UINT8)IdsNvValue);
      }

      //Force PWM Control
      IDS_NV_READ_SKIP (IDSNVID_DBG_FORCE_PWM_CTL, &IdsNvValue) {
        switch (IdsNvValue) {
        case IDSOPT_DBG_FORCE_PWM_CTL_FORCE:
          PcdSet8 (PcdForceFanPwmEn, (UINT8)IdsNvValue);
          IDS_NV_READ_SKIP (IDSNVID_DBG_FORCE_PWM, &IdsNvValue) {
            PcdSet8 (PcdForceFanPwm, (UINT8)IdsNvValue);
          }
          break;
        case IDSOPT_DBG_FORCE_PWM_CTL_UNFORCE:
          break;
        default:
          ASSERT (FALSE);
          break;
        }
      }

      break;
    case IDSOPT_DBG_FAN_CTL_AUTO:
      break;
    default:
      ASSERT (FALSE);
      break;
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
 * @retval    AGESA_STATUS
 *
 **/
IDS_HOOK_STATUS
IdsHookNbioLoadBuildOption (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
  )
{
  UINT64     IdsNvValue;

  //IOMMU
  IDS_NV_READ_SKIP (IDSNVID_CMN_GNB_NB_IOMMU, &IdsNvValue) {
    if (IdsNvValue) {
      PcdSetBool (PcdCfgIommuSupport, (BOOLEAN)IdsNvValue);
    }
  }

  //NBIO Internal Poison Consumption
  IDS_NV_READ_SKIP (IDSNVID_DBG_POISON_CONSUMPTION, &IdsNvValue) {
    if (IdsNvValue != IDSOPT_DBG_POISON_CONSUMPTION_AUTO) {
      PcdSetBool (PcdAmdNbioPoisonConsumption, (BOOLEAN)IdsNvValue);
    }
  }

  //NBIO RAS Control
  IDS_NV_READ_SKIP (IDSNVID_DBG_RAS_CONTROL, &IdsNvValue) {
    if (IdsNvValue != IDSOPT_DBG_RAS_CONTROL_AUTO) {
      PcdSetBool (PcdAmdNbioRASControl, (BOOLEAN)IdsNvValue);
    }
  }

  //HD Audio Enable
  IDS_NV_READ_SKIP (IDSNVID_CMN_GNB_HD_AUDIO_EN, &IdsNvValue) {
    if (IdsNvValue != IDSOPT_CMN_GNB_HD_AUDIO_EN_AUTO) {
      PcdSetBool (PcdCfgHdAudioEnable, (BOOLEAN)IdsNvValue);
    }
  }

  return IDS_HOOK_SUCCESS;
}

//BYO Modify for RAS >>
IDS_HOOK_STATUS
IdsHookNbioPcieAerInit (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
  )
{
    AMD_NBIO_PCIe_AER_CONFIG *Config;
    UINT32   Index;
    
    Config = (AMD_NBIO_PCIe_AER_CONFIG*) Data;
    for (Index = 0; Index < Config->NumberOfPorts; Index++) {
        Config->PortAerConfig[Index].PortAerEnable = 1;
        Config->PortAerConfig[Index].CorrectableMask.Value = MAX_UINT32;
        Config->PortAerConfig[Index].UncorrectableMask.Value = MAX_UINT32;
        Config->PortAerConfig[Index].UncorrectableSeverity.Value = MAX_UINT32;
    }
    return IDS_HOOK_SUCCESS;
}
//BYO Modify for RAS <<

#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define NBIO_ZP_IDS_HOOKS_INT
#else
  #include <Internal/NbioIdsHookZPLibInt.h>
#endif

IDS_HOOK_ELEMENT NbioZPIdsHooks[] = {
  {
    IDS_HOOK_NBIO_SMUV9_INIT_CMN,
    &SmuZPBeforeFeatureEnable
  },
  {
    IDS_HOOK_NBIO_LOAD_BUILD_OPTION,
    &IdsHookNbioLoadBuildOption
  },
  //BYO Modify for RAS >>
  {
    IDS_HOOK_NBIO_PCIE_AER_INIT,
    &IdsHookNbioPcieAerInit
  },
  //BYO Modify for RAS <<
  NBIO_ZP_IDS_HOOKS_INT
  IDS_HOOKS_END
};

IDS_HOOK_TABLE NbioZPIdsHookTable = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  NbioZPIdsHooks
};


AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &NbioZPIdsHookTable;
  return AGESA_SUCCESS;
}
