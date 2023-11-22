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

/*****************************************************************************
 *
 * This software package can be used to enable the Overclocking of certain
 * HYGON processors and its use is subject to the terms and conditions of the
 * HYGON Overclocking Waiver. Enabling overclocking through use of the low-level
 * routines included in this package and operating an HYGON processor outside of
 * the applicable HYGON product specifications will void any HYGON warranty and can
 * result in damage to the processor or the system into which the processor has
 * been integrated. The user of this software assumes, and HYGON disclaims, all
 * risk, liability, costs and damages relating to or arising from the overclocking
 * of HYGON processors.
 *
 ******************************************************************************
 */

#include <Base.h>
#include <Uefi.h>

#include <Protocol/HiiConfigRouting.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiString.h>

#include <Guid/MdeModuleHii.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HiiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include <Library/HygonSocBaseLib.h>

#include <Protocol/HygonNbioSmuServicesProtocol.h>

#include <Library/HygonCbsSetupLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Protocol/HygonCbsHookProtocol.h>
#include "HygonCbsVariable.h"
#include "HygonCbsFormID.h"
#include "CbsCustomCorePstates.h"

#include "PiDxe.h"
#include <HygonFuse.h>
#include <CddRegistersDm.h>

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
CustomCorePstate (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest,
  OUT CBS_CONFIG                             *pSetup_Config,
  IN  EFI_HII_HANDLE                         HiiHandle
  )
{
  switch (QuestionId) {
    case KEY_CUSTOM_PSTATE0:
      CustomCorePstate0 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
      break;
    case KEY_CUSTOM_PSTATE1:
      CustomCorePstate1 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
      break;
    case KEY_CUSTOM_PSTATE2:
      CustomCorePstate2 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
      break;
    case KEY_PSTATE0_FID:
    case KEY_PSTATE0_DID:
    case KEY_PSTATE0_VID:
      UpdatePstInfo0 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
      break;
    case KEY_PSTATE1_FID:
    case KEY_PSTATE1_DID:
    case KEY_PSTATE1_VID:
      UpdatePstInfo1 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
      break;
    case KEY_PSTATE2_FID:
    case KEY_PSTATE2_DID:
    case KEY_PSTATE2_VID:
      UpdatePstInfo2 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
      break;
    default:
      break;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
CustomCorePstate0 (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest,
  OUT CBS_CONFIG                             *pSetup_Config,
  IN  EFI_HII_HANDLE                         HiiHandle
  )
{
  PSTATE_MSR  PstateMsr;

  // Custom
  if (pSetup_Config->CbsCpuPstCustomP0 == CUSTOM_PSTATE) {
    PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_0);
    if ((PstateMsr.Field.PstateEn == 1) &&
        (pSetup_Config->CbsCpuPst0Fid == DEFAULT_FID) &&
        (pSetup_Config->CbsCpuPst0Did == DEFAULT_DID) &&
        (pSetup_Config->CbsCpuPst0Vid == DEFAULT_VID)) {
      pSetup_Config->CbsCpuPst0Fid = (UINT8)PstateMsr.Field.CpuFid_7_0;
      pSetup_Config->CbsCpuPst0Did = (UINT8)PstateMsr.Field.CpuDid;
      pSetup_Config->CbsCpuPst0Vid = (UINT8)PstateMsr.Field.CpuVid;
    }

    UpdatePstInfo0 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
  }

  // Auto
  if (pSetup_Config->CbsCpuPstCustomP0 == AUTO_PSTATE) {
    SetFidDidVidToDefault (pSetup_Config, PSTATE_0);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
CustomCorePstate1 (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest,
  OUT CBS_CONFIG                             *pSetup_Config,
  IN  EFI_HII_HANDLE                         HiiHandle
  )
{
  PSTATE_MSR  PstateMsr;

  // Disable
  if (pSetup_Config->CbsCpuPstCustomP1 == DISABLE_PSTATE) {
    SetFollowingCustomPstateOptions (pSetup_Config, PSTATE_2, DISABLE_PSTATE);
    SetFidDidVidToDefault (pSetup_Config, PSTATE_1);
  }

  // Custom
  if (pSetup_Config->CbsCpuPstCustomP1 == CUSTOM_PSTATE) {
    PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_1);
    if ((PstateMsr.Field.PstateEn == 1) &&
        (pSetup_Config->CbsCpuPst1Fid == DEFAULT_FID) &&
        (pSetup_Config->CbsCpuPst1Did == DEFAULT_DID) &&
        (pSetup_Config->CbsCpuPst1Vid == DEFAULT_VID)) {
      pSetup_Config->CbsCpuPst1Fid = (UINT8)PstateMsr.Field.CpuFid_7_0;
      pSetup_Config->CbsCpuPst1Did = (UINT8)PstateMsr.Field.CpuDid;
      pSetup_Config->CbsCpuPst1Vid = (UINT8)PstateMsr.Field.CpuVid;
    }

    UpdatePstInfo1 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
  }

  // Auto
  if (pSetup_Config->CbsCpuPstCustomP1 == AUTO_PSTATE) {
    PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_1);
    if ((pSetup_Config->CbsCpuPstCustomP2 == DISABLE_PSTATE) ||
        (PstateMsr.Field.PstateEn == 0)) {
      SetFollowingCustomPstateOptions (pSetup_Config, PSTATE_2, AUTO_PSTATE);
    }

    SetFidDidVidToDefault (pSetup_Config, PSTATE_1);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
CustomCorePstate2 (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest,
  OUT CBS_CONFIG                             *pSetup_Config,
  IN  EFI_HII_HANDLE                         HiiHandle
  )
{
  BOOLEAN     AllowCustom;
  PSTATE_MSR  PstateMsr;

  // Disable
  if (pSetup_Config->CbsCpuPstCustomP2 == DISABLE_PSTATE) {
    SetFollowingCustomPstateOptions (pSetup_Config, PSTATE_3, DISABLE_PSTATE);
    SetFidDidVidToDefault (pSetup_Config, PSTATE_2);
  }

  // Custom
  if (pSetup_Config->CbsCpuPstCustomP2 == CUSTOM_PSTATE) {
    AllowCustom = FALSE;
    if (pSetup_Config->CbsCpuPstCustomP1 == CUSTOM_PSTATE) {
      AllowCustom = TRUE;
    }

    if (pSetup_Config->CbsCpuPstCustomP1 == AUTO_PSTATE) {
      PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_1);
      if (PstateMsr.Field.PstateEn == 1) {
        AllowCustom = TRUE;
      }
    }

    if (AllowCustom) {
      PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_2);
      if ((PstateMsr.Field.PstateEn == 1) &&
          (pSetup_Config->CbsCpuPst2Fid == DEFAULT_FID) &&
          (pSetup_Config->CbsCpuPst2Did == DEFAULT_DID) &&
          (pSetup_Config->CbsCpuPst2Vid == DEFAULT_VID)) {
        pSetup_Config->CbsCpuPst2Fid = (UINT8)PstateMsr.Field.CpuFid_7_0;
        pSetup_Config->CbsCpuPst2Did = (UINT8)PstateMsr.Field.CpuDid;
        pSetup_Config->CbsCpuPst2Vid = (UINT8)PstateMsr.Field.CpuVid;
      }

      UpdatePstInfo2 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    } else {
      pSetup_Config->CbsCpuPstCustomP2 = AUTO_PSTATE;
    }
  }

  // Auto
  if (pSetup_Config->CbsCpuPstCustomP2 == AUTO_PSTATE) {
    SetFidDidVidToDefault (pSetup_Config, PSTATE_2);
  }

  return EFI_SUCCESS;
}

VOID
UpdatePstInfo0 (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest,
  OUT CBS_CONFIG                             *pSetup_Config,
  IN  EFI_HII_HANDLE                         HiiHandle
  )
{
  UINT8   CurrentFid;
  UINT8   AdjacentFid;
  UINT8   CurrentDid;
  UINT8   AdjacentDid;
  UINT8   CurrentVid;
  UINT8   AdjacentVid;
  UINT32  CurrentFreq;
  UINT32  AdjacentFreq;

  if (pSetup_Config->CbsCpuPst0Did > 0x1A) {
    pSetup_Config->CbsCpuPst0Did &= 0xFE;
  }

  // Check settings
  GetPstateSetting (pSetup_Config, PSTATE_0, &CurrentFreq, &CurrentFid, &CurrentDid, &CurrentVid);
  GetPstateSetting (pSetup_Config, PSTATE_1, &AdjacentFreq, &AdjacentFid, &AdjacentDid, &AdjacentVid);
  if (AdjacentFreq != 0) {
    if (CurrentFreq < AdjacentFreq) {
      pSetup_Config->CbsCpuPst0Fid = AdjacentFid;
      pSetup_Config->CbsCpuPst0Did = AdjacentDid;
    }
  }

  if (AdjacentVid != DEFAULT_VID) {
    if (CurrentVid > AdjacentVid) {
      pSetup_Config->CbsCpuPst0Vid = AdjacentVid;
    }
  }

  // Display frequency
  if (pSetup_Config->CbsCpuPst0Did == 0) {
    pSetup_Config->CbsCpuCofP0 = 0;
  } else {
    pSetup_Config->CbsCpuCofP0 = GetPstateInfoFeq (pSetup_Config->CbsCpuPst0Fid, pSetup_Config->CbsCpuPst0Did);
  }

  // Display voltage
  if ((pSetup_Config->CbsCpuPst0Vid >= 0xF8) && (pSetup_Config->CbsCpuPst0Vid <= 0xFF)) {
    pSetup_Config->CbsCpuVoltageP0 = 0;
  } else {
    pSetup_Config->CbsCpuVoltageP0 = GetPstateInfoVoltage (pSetup_Config->CbsCpuPst0Vid);
  }
}

VOID
UpdatePstInfo1 (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest,
  OUT CBS_CONFIG                             *pSetup_Config,
  IN  EFI_HII_HANDLE                         HiiHandle
  )
{
  UINT8   CurrentFid;
  UINT8   AdjacentFid;
  UINT8   CurrentDid;
  UINT8   AdjacentDid;
  UINT8   CurrentVid;
  UINT8   AdjacentVid;
  UINT32  CurrentFreq;
  UINT32  AdjacentFreq;

  if (pSetup_Config->CbsCpuPst1Did > 0x1A) {
    pSetup_Config->CbsCpuPst1Did &= 0xFE;
  }

  // Check settings
  GetPstateSetting (pSetup_Config, PSTATE_1, &CurrentFreq, &CurrentFid, &CurrentDid, &CurrentVid);
  GetPstateSetting (pSetup_Config, PSTATE_0, &AdjacentFreq, &AdjacentFid, &AdjacentDid, &AdjacentVid);
  if (CurrentFreq != 0) {
    if (CurrentFreq > AdjacentFreq) {
      pSetup_Config->CbsCpuPst1Fid = AdjacentFid;
      pSetup_Config->CbsCpuPst1Did = AdjacentDid;
    }
  }

  if (CurrentVid != DEFAULT_VID) {
    if (CurrentVid < AdjacentVid) {
      pSetup_Config->CbsCpuPst1Vid = AdjacentVid;
    }
  }

  GetPstateSetting (pSetup_Config, PSTATE_2, &AdjacentFreq, &AdjacentFid, &AdjacentDid, &AdjacentVid);
  if (AdjacentFreq != 0) {
    if (CurrentFreq < AdjacentFreq) {
      pSetup_Config->CbsCpuPst1Fid = AdjacentFid;
      pSetup_Config->CbsCpuPst1Did = AdjacentDid;
    }
  }

  if (AdjacentVid != DEFAULT_VID) {
    if (CurrentVid > AdjacentVid) {
      pSetup_Config->CbsCpuPst1Vid = AdjacentVid;
    }
  }

  // Display frequency
  if (pSetup_Config->CbsCpuPst1Did == 0) {
    pSetup_Config->CbsCpuCofP1 = 0;
  } else {
    pSetup_Config->CbsCpuCofP1 = GetPstateInfoFeq (pSetup_Config->CbsCpuPst1Fid, pSetup_Config->CbsCpuPst1Did);
  }

  // Display voltage
  if ((pSetup_Config->CbsCpuPst1Vid >= 0xF8) && (pSetup_Config->CbsCpuPst1Vid <= 0xFF)) {
    pSetup_Config->CbsCpuVoltageP1 = 0;
  } else {
    pSetup_Config->CbsCpuVoltageP1 = GetPstateInfoVoltage (pSetup_Config->CbsCpuPst1Vid);
  }
}

VOID
UpdatePstInfo2 (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest,
  OUT CBS_CONFIG                             *pSetup_Config,
  IN  EFI_HII_HANDLE                         HiiHandle
  )
{
  UINT8   CurrentFid;
  UINT8   AdjacentFid;
  UINT8   CurrentDid;
  UINT8   AdjacentDid;
  UINT8   CurrentVid;
  UINT8   AdjacentVid;
  UINT32  CurrentFreq;
  UINT32  AdjacentFreq;

  if (pSetup_Config->CbsCpuPst2Did > 0x1A) {
    pSetup_Config->CbsCpuPst2Did &= 0xFE;
  }

  // Check settings
  GetPstateSetting (pSetup_Config, PSTATE_2, &CurrentFreq, &CurrentFid, &CurrentDid, &CurrentVid);
  GetPstateSetting (pSetup_Config, PSTATE_1, &AdjacentFreq, &AdjacentFid, &AdjacentDid, &AdjacentVid);
  if (AdjacentFreq == 0) {
    pSetup_Config->CbsCpuPstCustomP2 = AUTO_PSTATE;
    return;
  }

  if (CurrentFreq != 0) {
    if (CurrentFreq > AdjacentFreq) {
      pSetup_Config->CbsCpuPst2Fid = AdjacentFid;
      pSetup_Config->CbsCpuPst2Did = AdjacentDid;
    }
  }

  if (CurrentVid != DEFAULT_VID) {
    if (CurrentVid < AdjacentVid) {
      pSetup_Config->CbsCpuPst2Vid = AdjacentVid;
    }
  }

  GetPstateSetting (pSetup_Config, PSTATE_3, &AdjacentFreq, &AdjacentFid, &AdjacentDid, &AdjacentVid);
  if (AdjacentFreq != 0) {
    if (CurrentFreq < AdjacentFreq) {
      pSetup_Config->CbsCpuPst2Fid = AdjacentFid;
      pSetup_Config->CbsCpuPst2Did = AdjacentDid;
    }
  }

  if (AdjacentVid != DEFAULT_VID) {
    if (CurrentVid > AdjacentVid) {
      pSetup_Config->CbsCpuPst2Vid = AdjacentVid;
    }
  }

  // Display frequency
  if (pSetup_Config->CbsCpuPst2Did == 0) {
    pSetup_Config->CbsCpuCofP2 = 0;
  } else {
    pSetup_Config->CbsCpuCofP2 = GetPstateInfoFeq (pSetup_Config->CbsCpuPst2Fid, pSetup_Config->CbsCpuPst2Did);
  }

  // Display voltage
  if ((pSetup_Config->CbsCpuPst2Vid >= 0xF8) && (pSetup_Config->CbsCpuPst2Vid <= 0xFF)) {
    pSetup_Config->CbsCpuVoltageP2 = 0;
  } else {
    pSetup_Config->CbsCpuVoltageP2 = GetPstateInfoVoltage (pSetup_Config->CbsCpuPst2Vid);
  }
}

VOID
SetFidDidVidToDefault (
  OUT CBS_CONFIG *pSetup_Config,
  IN  UINT8 Pstate
  )
{
  switch (Pstate) {
    case PSTATE_0:
      pSetup_Config->CbsCpuPst0Fid = DEFAULT_FID;
      pSetup_Config->CbsCpuPst0Did = DEFAULT_DID;
      pSetup_Config->CbsCpuPst0Vid = DEFAULT_VID;
      break;
    case PSTATE_1:
      pSetup_Config->CbsCpuPst1Fid = DEFAULT_FID;
      pSetup_Config->CbsCpuPst1Did = DEFAULT_DID;
      pSetup_Config->CbsCpuPst1Vid = DEFAULT_VID;
      break;
    case PSTATE_2:
      pSetup_Config->CbsCpuPst2Fid = DEFAULT_FID;
      pSetup_Config->CbsCpuPst2Did = DEFAULT_DID;
      pSetup_Config->CbsCpuPst2Vid = DEFAULT_VID;
      break;
    default:
      break;
  }
}

VOID
SetFollowingCustomPstateOptions (
  OUT CBS_CONFIG *pSetup_Config,
  IN  UINT8 Pstate,
  IN  UINT8 Choice
  )
{
  if (Pstate == PSTATE_0) {
    pSetup_Config->CbsCpuPstCustomP0 = Choice;
  }

  if (Pstate <= PSTATE_1) {
    pSetup_Config->CbsCpuPstCustomP1 = Choice;
  }

  if (Pstate <= PSTATE_2) {
    pSetup_Config->CbsCpuPstCustomP2 = Choice;
  }
}

UINT32
GetPstateInfoFeq (
  IN  UINT8 Fid,
  IN  UINT8 Did
  )
{
  return (UINT32)(DivU64x32 (MultU64x64 (200, Fid), Did));
}

UINT32
GetPstateInfoVoltage (
  IN  UINT8 Vid
  )
{
  return (UINT32)(1550000L - MultU64x64 (6250, Vid));
}

VOID
GetPstateSetting (
  OUT CBS_CONFIG *pSetup_Config,
  IN  UINT8       Pstate,
  OUT UINT32     *CpuFreq,
  OUT UINT8      *CpuFid,
  OUT UINT8      *CpuDid,
  OUT UINT8      *CpuVid
  )
{
  UINT8       CustomPstateX;
  UINT8       CustomPstateXFid;
  UINT8       CustomPstateXDid;
  UINT8       CustomPstateXVid;
  PSTATE_MSR  PstateMsr;

  switch (Pstate) {
    case PSTATE_0:
      CustomPstateX    = pSetup_Config->CbsCpuPstCustomP0;
      CustomPstateXFid = pSetup_Config->CbsCpuPst0Fid;
      CustomPstateXDid = pSetup_Config->CbsCpuPst0Did;
      CustomPstateXVid = pSetup_Config->CbsCpuPst0Vid;
      break;
    case PSTATE_1:
      CustomPstateX    = pSetup_Config->CbsCpuPstCustomP1;
      CustomPstateXFid = pSetup_Config->CbsCpuPst1Fid;
      CustomPstateXDid = pSetup_Config->CbsCpuPst1Did;
      CustomPstateXVid = pSetup_Config->CbsCpuPst1Vid;
      break;
    case PSTATE_2:
      CustomPstateX    = pSetup_Config->CbsCpuPstCustomP2;
      CustomPstateXFid = pSetup_Config->CbsCpuPst2Fid;
      CustomPstateXDid = pSetup_Config->CbsCpuPst2Did;
      CustomPstateXVid = pSetup_Config->CbsCpuPst2Vid;
      break;
    default:
      *CpuFreq = 0;
      *CpuVid  = 0xFF;
      return;
      break;
  }

  if (CustomPstateX == DISABLE_PSTATE) {
    *CpuFreq = 0;
    *CpuFid  = DEFAULT_FID;
    *CpuDid  = DEFAULT_DID;
    *CpuVid  = DEFAULT_VID;
  } else if (CustomPstateX == CUSTOM_PSTATE) {
    *CpuFreq = GetPstateInfoFeq (CustomPstateXFid, CustomPstateXDid);
    *CpuFid  = CustomPstateXFid;
    *CpuDid  = CustomPstateXDid;
    *CpuVid  = CustomPstateXVid;
  } else {
    PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_0 + Pstate);
    if (PstateMsr.Field.PstateEn == 1) {
      *CpuFreq = GetPstateInfoFeq ((UINT8)PstateMsr.Field.CpuFid_7_0, (UINT8)PstateMsr.Field.CpuDid);
      *CpuFid  = (UINT8)PstateMsr.Field.CpuFid_7_0;
      *CpuDid  = (UINT8)PstateMsr.Field.CpuDid;
      *CpuVid  = (UINT8)PstateMsr.Field.CpuVid;
    } else {
      *CpuFreq = 0;
      *CpuFid  = DEFAULT_FID;
      *CpuDid  = DEFAULT_DID;
      *CpuVid  = DEFAULT_VID;
    }
  }
}

UINT64
HexToDec (
  IN  UINT64 Hex
  )
{
  UINT8   i;
  UINT64  Dec;
  UINT64  Temp;

  Dec = 0;
  i   = 0;
  while (Hex != 0) {
    Temp = Hex % 10;
    Temp = LShiftU64 (Temp, i);
    Dec |= Temp;
    Hex  = Hex / 10;
    i    = i + 4;
  }

  return Dec;
}
