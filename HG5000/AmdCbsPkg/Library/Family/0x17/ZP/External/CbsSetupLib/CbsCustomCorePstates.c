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

/*****************************************************************************
 *
 * This software package can be used to enable the Overclocking of certain
 * AMD processors and its use is subject to the terms and conditions of the
 * AMD Overclocking Waiver. Enabling overclocking through use of the low-level
 * routines included in this package and operating an AMD processor outside of
 * the applicable AMD product specifications will void any AMD warranty and can
 * result in damage to the processor or the system into which the processor has
 * been integrated. The user of this software assumes, and AMD disclaims, all
 * risk, liability, costs and damages relating to or arising from the overclocking
 * of AMD processors.
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

#include <Protocol/AmdNbioSmuServicesProtocol.h>

#include <Library/AmdCbsSetupLib.h>
#include <Protocol/AmdCbsHookProtocol.h>
#include "AmdCbsVariable.h"
#include "AmdCbsFormID.h"
#include "CbsCustomCorePstates.h"

#include "PiDxe.h"

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
  case KEY_OC_MODE:
    OcMode (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    break;
  case KEY_CUSTOM_PSTATE0:
    CustomCorePstate0 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    break;
  case KEY_CUSTOM_PSTATE1:
    CustomCorePstate1 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    break;
  case KEY_CUSTOM_PSTATE2:
    CustomCorePstate2 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    break;
  case KEY_CUSTOM_PSTATE3:
    CustomCorePstate3 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    break;
  case KEY_CUSTOM_PSTATE4:
    CustomCorePstate4 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    break;
  case KEY_CUSTOM_PSTATE5:
    CustomCorePstate5 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    break;
  case KEY_CUSTOM_PSTATE6:
    CustomCorePstate6 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    break;
  case KEY_CUSTOM_PSTATE7:
    CustomCorePstate7 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
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
  case KEY_PSTATE3_FID:
  case KEY_PSTATE3_DID:
  case KEY_PSTATE3_VID:
    UpdatePstInfo3 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    break;
  case KEY_PSTATE4_FID:
  case KEY_PSTATE4_DID:
  case KEY_PSTATE4_VID:
    UpdatePstInfo4 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    break;
  case KEY_PSTATE5_FID:
  case KEY_PSTATE5_DID:
  case KEY_PSTATE5_VID:
    UpdatePstInfo5 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    break;
  case KEY_PSTATE6_FID:
  case KEY_PSTATE6_DID:
  case KEY_PSTATE6_VID:
    UpdatePstInfo6 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    break;
  case KEY_PSTATE7_FID:
  case KEY_PSTATE7_DID:
  case KEY_PSTATE7_VID:
    UpdatePstInfo7 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    break;
  default:
    break;
  }

  return EFI_SUCCESS;
}

CBS_OC_TABLE mCbsOcMode180W [] = {
  {   0,    0,    0,    0,    0}, // Normal Operation
  {   1,    0, 0x90, 0x08, 0x22}, // OC1 - 16C 1.33750V 3.6G
  {   2,    5, 0x94, 0x08, 0x1D}, // OC2 -  8C 1.36875V 3.7G
  {   3,    2, 0x96, 0x08, 0x1C}, // OC3 -  4C 1.37500V 3.75G
  {   4,    0, 0x98, 0x08, 0x18}, // Max Stress Mode - 16C 1.40000V 3.8G
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

CBS_OC_TABLE mCbsOcMode140W [] = {
  {   0,    0,    0,    0,    0}, // Normal Operation
  {   1,    0, 0x8C, 0x08, 0x1F}, // OC1 - 12C 1.35625V 3.5G
  {   2,    5, 0x8E, 0x08, 0x1D}, // OC2 -  8C 1.36875V 3.55G
  {   3,    2, 0x8F, 0x08, 0x1C}, // OC3 -  4C 1.37500V 3.575G
  {   4,    0, 0x92, 0x08, 0x18}, // Max Stress Mode - 12C 1.40000V 3.65G
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

CBS_OC_TABLE_SELECTION mCbsOcModeTableSelection [] = {
  {ZP_SOCKET_SP3r2, 8, 180, mCbsOcMode180W}, // 16C 180W
  {ZP_SOCKET_SP3r2, 6, 140, mCbsOcMode140W}, // 12C 140W
  {0, 0, 0, NULL}
};

EFI_STATUS
OcMode (
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
  UINT32                                RegEbx;
  UINT32                                CoreDisFusePerDie;
  UINT32                                CoreDisByFuseCountPerDie;
  UINT32                                OpnCoreNumberPerDie;
  UINT32                                FuseValue;
  UINT32                                PackageType;
  UINT32                                OpnMaxTdp;
  CBS_OC_TABLE_SELECTION               *TableSelection;
  CBS_OC_TABLE                         *OcModeTable;
  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL   *NbioSmuServices;

  // Locate gAmdNbioSmuServicesProtocolGuid
  NbioSmuServices = NULL;
  gBS->LocateProtocol (&gAmdNbioSmuServicesProtocolGuid, NULL, &NbioSmuServices);
  ASSERT (NbioSmuServices != NULL);

  // Get PackageType
  AsmCpuid (0x80000001, NULL, &RegEbx, NULL, NULL);
  PackageType = (RegEbx >> 28) & 0xF; // bit 31:28

  if (PackageType == ZP_SOCKET_SP4) {
    NbioSmuServices->AmdSmuRegisterRead (NbioSmuServices, 0, 0x5D258, &FuseValue);
    if ((FuseValue & BIT30) != 0) {
      PackageType = ZP_SOCKET_SP3r2;
    }
  }

  // Get OPN core number
  // OpnCoreNumberPerDie = 8 - CoreDisByFuseCountPerDie
  CoreDisByFuseCountPerDie = 0;
  NbioSmuServices->AmdSmuRegisterRead (NbioSmuServices, 0, 0x5D25C, &CoreDisFusePerDie);
  CoreDisFusePerDie &= 0xFF; // bits[7:0]
  while (CoreDisFusePerDie != 0) {
    if ((CoreDisFusePerDie & 1) == 1) {
      CoreDisByFuseCountPerDie++;
    }
    CoreDisFusePerDie = CoreDisFusePerDie >> 1;
  }

  OpnCoreNumberPerDie = 8 - CoreDisByFuseCountPerDie;

  // Get OPN TDP
  NbioSmuServices->AmdSmuRegisterRead (NbioSmuServices, 0, 0x5D378, &FuseValue);
  OpnMaxTdp = (FuseValue >> 1) & 0xFF;

  // Find out OC mode table
  TableSelection = mCbsOcModeTableSelection;
  OcModeTable = NULL;

  while (TableSelection->OcTable != NULL) {
    if ((TableSelection->PackageType == PackageType) &&
        (TableSelection->CoreNumberPerDie == OpnCoreNumberPerDie) &&
        (TableSelection->Tdp == OpnMaxTdp)) {
      OcModeTable = TableSelection->OcTable;
      break;
    }
    TableSelection++;
  }

  // Apply OC table
  if (OcModeTable != NULL ) {
    while (OcModeTable->OcMode != 0xFF) {
      if (pSetup_Config->CbsCmnCpuOcMode == OcModeTable->OcMode) {
        // Update down core mode
        pSetup_Config->CbsCmnCpuGenDowncoreCtrl = OcModeTable->DownCoreMode;

        // Update P0
        if (OcModeTable->P0Fid == 0) {
          pSetup_Config->CbsCpuPstCustomP0 = AUTO_PSTATE;
          SetFidDidVidToDefault (pSetup_Config, PSTATE_0);
        } else {
          pSetup_Config->CbsCpuPstCustomP0 = CUSTOM_PSTATE;
          pSetup_Config->CbsCpuPst0Fid = OcModeTable->P0Fid;
          pSetup_Config->CbsCpuPst0Did = OcModeTable->P0Did;
          pSetup_Config->CbsCpuPst0Vid = OcModeTable->P0Vid;
          UpdatePstInfo0 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
        }
        break;
      }
      OcModeTable++;
    }
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
  PSTATE_MSR PstateMsr;

  // Custom
  if (pSetup_Config->CbsCpuPstCustomP0 == CUSTOM_PSTATE) {
    PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_0);
    if ((PstateMsr.Field.PstateEn == 1) &&
        (pSetup_Config->CbsCpuPst0Fid == DEFAULT_FID) &&
        (pSetup_Config->CbsCpuPst0Did == DEFAULT_DID) &&
        (pSetup_Config->CbsCpuPst0Vid == DEFAULT_VID)) {
      pSetup_Config->CbsCpuPst0Fid = (UINT8) PstateMsr.Field.CpuFid_7_0;
      pSetup_Config->CbsCpuPst0Did = (UINT8) PstateMsr.Field.CpuDid;
      pSetup_Config->CbsCpuPst0Vid = (UINT8) PstateMsr.Field.CpuVid;
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
  PSTATE_MSR PstateMsr;

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
      pSetup_Config->CbsCpuPst1Fid = (UINT8) PstateMsr.Field.CpuFid_7_0;
      pSetup_Config->CbsCpuPst1Did = (UINT8) PstateMsr.Field.CpuDid;
      pSetup_Config->CbsCpuPst1Vid = (UINT8) PstateMsr.Field.CpuVid;
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
  BOOLEAN    AllowCustom;
  PSTATE_MSR PstateMsr;

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
        pSetup_Config->CbsCpuPst2Fid = (UINT8) PstateMsr.Field.CpuFid_7_0;
        pSetup_Config->CbsCpuPst2Did = (UINT8) PstateMsr.Field.CpuDid;
        pSetup_Config->CbsCpuPst2Vid = (UINT8) PstateMsr.Field.CpuVid;
      }
      UpdatePstInfo2 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    } else {
      pSetup_Config->CbsCpuPstCustomP2 = AUTO_PSTATE;
    }
  }

  // Auto
  if (pSetup_Config->CbsCpuPstCustomP2 == AUTO_PSTATE) {
    PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_2);
    if ((pSetup_Config->CbsCpuPstCustomP3 == DISABLE_PSTATE) ||
        (PstateMsr.Field.PstateEn == 0)) {
      SetFollowingCustomPstateOptions (pSetup_Config, PSTATE_3, AUTO_PSTATE);
    }
    SetFidDidVidToDefault (pSetup_Config, PSTATE_2);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
CustomCorePstate3 (
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
  BOOLEAN    AllowCustom;
  PSTATE_MSR PstateMsr;

  // Disable
  if (pSetup_Config->CbsCpuPstCustomP3 == DISABLE_PSTATE) {
    SetFollowingCustomPstateOptions (pSetup_Config, PSTATE_4, DISABLE_PSTATE);
    SetFidDidVidToDefault (pSetup_Config, PSTATE_3);
  }

  // Custom
  if (pSetup_Config->CbsCpuPstCustomP3 == CUSTOM_PSTATE) {
    AllowCustom = FALSE;
    if (pSetup_Config->CbsCpuPstCustomP2 == CUSTOM_PSTATE) {
      AllowCustom = TRUE;
    }
    if (pSetup_Config->CbsCpuPstCustomP2 == AUTO_PSTATE) {
      PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_2);
      if (PstateMsr.Field.PstateEn == 1) {
        AllowCustom = TRUE;
      }
    }
    if (AllowCustom) {
      PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_3);
      if ((PstateMsr.Field.PstateEn == 1) &&
          (pSetup_Config->CbsCpuPst3Fid == DEFAULT_FID) &&
          (pSetup_Config->CbsCpuPst3Did == DEFAULT_DID) &&
          (pSetup_Config->CbsCpuPst3Vid == DEFAULT_VID)) {
        pSetup_Config->CbsCpuPst3Fid = (UINT8) PstateMsr.Field.CpuFid_7_0;
        pSetup_Config->CbsCpuPst3Did = (UINT8) PstateMsr.Field.CpuDid;
        pSetup_Config->CbsCpuPst3Vid = (UINT8) PstateMsr.Field.CpuVid;
      }
      UpdatePstInfo3 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    } else {
      pSetup_Config->CbsCpuPstCustomP3 = AUTO_PSTATE;
    }
  }

  // Auto
  if (pSetup_Config->CbsCpuPstCustomP3 == AUTO_PSTATE) {
    PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_3);
    if ((pSetup_Config->CbsCpuPstCustomP4 == DISABLE_PSTATE) ||
        (PstateMsr.Field.PstateEn == 0)) {
      SetFollowingCustomPstateOptions (pSetup_Config, PSTATE_4, AUTO_PSTATE);
    }
    SetFidDidVidToDefault (pSetup_Config, PSTATE_3);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
CustomCorePstate4 (
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
  BOOLEAN    AllowCustom;
  PSTATE_MSR PstateMsr;

  // Disable
  if (pSetup_Config->CbsCpuPstCustomP4 == DISABLE_PSTATE) {
    SetFollowingCustomPstateOptions (pSetup_Config, PSTATE_5, DISABLE_PSTATE);
    SetFidDidVidToDefault (pSetup_Config, PSTATE_4);
  }

  // Custom
  if (pSetup_Config->CbsCpuPstCustomP4 == CUSTOM_PSTATE) {
    AllowCustom = FALSE;
    if (pSetup_Config->CbsCpuPstCustomP3 == CUSTOM_PSTATE) {
      AllowCustom = TRUE;
    }
    if (pSetup_Config->CbsCpuPstCustomP3 == AUTO_PSTATE) {
      PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_3);
      if (PstateMsr.Field.PstateEn == 1) {
        AllowCustom = TRUE;
      }
    }
    if (AllowCustom) {
      PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_4);
      if ((PstateMsr.Field.PstateEn == 1) &&
          (pSetup_Config->CbsCpuPst4Fid == DEFAULT_FID) &&
          (pSetup_Config->CbsCpuPst4Did == DEFAULT_DID) &&
          (pSetup_Config->CbsCpuPst4Vid == DEFAULT_VID)) {
        pSetup_Config->CbsCpuPst4Fid = (UINT8) PstateMsr.Field.CpuFid_7_0;
        pSetup_Config->CbsCpuPst4Did = (UINT8) PstateMsr.Field.CpuDid;
        pSetup_Config->CbsCpuPst4Vid = (UINT8) PstateMsr.Field.CpuVid;
      }
      UpdatePstInfo4 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    } else {
      pSetup_Config->CbsCpuPstCustomP4 = AUTO_PSTATE;
    }
  }

  // Auto
  if (pSetup_Config->CbsCpuPstCustomP4 == AUTO_PSTATE) {
    PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_4);
    if ((pSetup_Config->CbsCpuPstCustomP5 == DISABLE_PSTATE) ||
        (PstateMsr.Field.PstateEn == 0)) {
      SetFollowingCustomPstateOptions (pSetup_Config, PSTATE_5, AUTO_PSTATE);
    }
    SetFidDidVidToDefault (pSetup_Config, PSTATE_4);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
CustomCorePstate5 (
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
  BOOLEAN    AllowCustom;
  PSTATE_MSR PstateMsr;

  // Disable
  if (pSetup_Config->CbsCpuPstCustomP5 == DISABLE_PSTATE) {
    SetFollowingCustomPstateOptions (pSetup_Config, PSTATE_6, DISABLE_PSTATE);
    SetFidDidVidToDefault (pSetup_Config, PSTATE_5);
  }

  // Custom
  if (pSetup_Config->CbsCpuPstCustomP5 == CUSTOM_PSTATE) {
    AllowCustom = FALSE;
    if (pSetup_Config->CbsCpuPstCustomP4 == CUSTOM_PSTATE) {
      AllowCustom = TRUE;
    }
    if (pSetup_Config->CbsCpuPstCustomP4 == AUTO_PSTATE) {
      PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_4);
      if (PstateMsr.Field.PstateEn == 1) {
        AllowCustom = TRUE;
      }
    }
    if (AllowCustom) {
      PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_5);
      if ((PstateMsr.Field.PstateEn == 1) &&
          (pSetup_Config->CbsCpuPst5Fid == DEFAULT_FID) &&
          (pSetup_Config->CbsCpuPst5Did == DEFAULT_DID) &&
          (pSetup_Config->CbsCpuPst5Vid == DEFAULT_VID)) {
        pSetup_Config->CbsCpuPst5Fid = (UINT8) PstateMsr.Field.CpuFid_7_0;
        pSetup_Config->CbsCpuPst5Did = (UINT8) PstateMsr.Field.CpuDid;
        pSetup_Config->CbsCpuPst5Vid = (UINT8) PstateMsr.Field.CpuVid;
      }
      UpdatePstInfo5 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    } else {
      pSetup_Config->CbsCpuPstCustomP5 = AUTO_PSTATE;
    }
  }

  // Auto
  if (pSetup_Config->CbsCpuPstCustomP5 == AUTO_PSTATE) {
    PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_5);
    if ((pSetup_Config->CbsCpuPstCustomP6 == DISABLE_PSTATE) ||
        (PstateMsr.Field.PstateEn == 0)) {
      SetFollowingCustomPstateOptions (pSetup_Config, PSTATE_6, AUTO_PSTATE);
    }
    SetFidDidVidToDefault (pSetup_Config, PSTATE_5);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
CustomCorePstate6 (
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
  BOOLEAN    AllowCustom;
  PSTATE_MSR PstateMsr;

  // Disable
  if (pSetup_Config->CbsCpuPstCustomP6 == DISABLE_PSTATE) {
    SetFollowingCustomPstateOptions (pSetup_Config, PSTATE_7, DISABLE_PSTATE);
    SetFidDidVidToDefault (pSetup_Config, PSTATE_6);
  }

  // Custom
  if (pSetup_Config->CbsCpuPstCustomP6 == CUSTOM_PSTATE) {
    AllowCustom = FALSE;
    if (pSetup_Config->CbsCpuPstCustomP5 == CUSTOM_PSTATE) {
      AllowCustom = TRUE;
    }
    if (pSetup_Config->CbsCpuPstCustomP5 == AUTO_PSTATE) {
      PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_5);
      if (PstateMsr.Field.PstateEn == 1) {
        AllowCustom = TRUE;
      }
    }
    if (AllowCustom) {
      PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_6);
      if ((PstateMsr.Field.PstateEn == 1) &&
          (pSetup_Config->CbsCpuPst6Fid == DEFAULT_FID) &&
          (pSetup_Config->CbsCpuPst6Did == DEFAULT_DID) &&
          (pSetup_Config->CbsCpuPst6Vid == DEFAULT_VID)) {
        pSetup_Config->CbsCpuPst6Fid = (UINT8) PstateMsr.Field.CpuFid_7_0;
        pSetup_Config->CbsCpuPst6Did = (UINT8) PstateMsr.Field.CpuDid;
        pSetup_Config->CbsCpuPst6Vid = (UINT8) PstateMsr.Field.CpuVid;
      }
      UpdatePstInfo6 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    } else {
      pSetup_Config->CbsCpuPstCustomP6 = AUTO_PSTATE;
    }
  }

  // Auto
  if (pSetup_Config->CbsCpuPstCustomP6 == AUTO_PSTATE) {
    PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_6);
    if ((pSetup_Config->CbsCpuPstCustomP7 == DISABLE_PSTATE) ||
        (PstateMsr.Field.PstateEn == 0)) {
      SetFollowingCustomPstateOptions (pSetup_Config, PSTATE_7, AUTO_PSTATE);
    }
    SetFidDidVidToDefault (pSetup_Config, PSTATE_6);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
CustomCorePstate7 (
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
  BOOLEAN    AllowCustom;
  PSTATE_MSR PstateMsr;

  // Disable
  if (pSetup_Config->CbsCpuPstCustomP7 == DISABLE_PSTATE) {
    SetFidDidVidToDefault (pSetup_Config, PSTATE_7);
  }

  // Custom
  if (pSetup_Config->CbsCpuPstCustomP7 == CUSTOM_PSTATE) {
    AllowCustom = FALSE;
    if (pSetup_Config->CbsCpuPstCustomP6 == CUSTOM_PSTATE) {
      AllowCustom = TRUE;
    }
    if (pSetup_Config->CbsCpuPstCustomP6 == AUTO_PSTATE) {
      PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_6);
      if (PstateMsr.Field.PstateEn == 1) {
        AllowCustom = TRUE;
      }
    }
    if (AllowCustom) {
      PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_7);
      if ((PstateMsr.Field.PstateEn == 1) &&
          (pSetup_Config->CbsCpuPst7Fid == DEFAULT_FID) &&
          (pSetup_Config->CbsCpuPst7Did == DEFAULT_DID) &&
          (pSetup_Config->CbsCpuPst7Vid == DEFAULT_VID)) {
        pSetup_Config->CbsCpuPst7Fid = (UINT8) PstateMsr.Field.CpuFid_7_0;
        pSetup_Config->CbsCpuPst7Did = (UINT8) PstateMsr.Field.CpuDid;
        pSetup_Config->CbsCpuPst7Vid = (UINT8) PstateMsr.Field.CpuVid;
      }
      UpdatePstInfo7 (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
    } else {
      pSetup_Config->CbsCpuPstCustomP7 = AUTO_PSTATE;
    }
  }

  // Auto
  if (pSetup_Config->CbsCpuPstCustomP7 == AUTO_PSTATE) {
    SetFidDidVidToDefault (pSetup_Config, PSTATE_7);
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
  UINT8  CurrentFid;
  UINT8  AdjacentFid;
  UINT8  CurrentDid;
  UINT8  AdjacentDid;
  UINT8  CurrentVid;
  UINT8  AdjacentVid;
  UINT32 CurrentFreq;
  UINT32 AdjacentFreq;

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
  UINT8  CurrentFid;
  UINT8  AdjacentFid;
  UINT8  CurrentDid;
  UINT8  AdjacentDid;
  UINT8  CurrentVid;
  UINT8  AdjacentVid;
  UINT32 CurrentFreq;
  UINT32 AdjacentFreq;

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
  UINT8  CurrentFid;
  UINT8  AdjacentFid;
  UINT8  CurrentDid;
  UINT8  AdjacentDid;
  UINT8  CurrentVid;
  UINT8  AdjacentVid;
  UINT32 CurrentFreq;
  UINT32 AdjacentFreq;

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

  GetPstateSetting (pSetup_Config, PSTATE_3,  &AdjacentFreq, &AdjacentFid, &AdjacentDid, &AdjacentVid);
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
UpdatePstInfo3 (
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
  UINT8  CurrentFid;
  UINT8  AdjacentFid;
  UINT8  CurrentDid;
  UINT8  AdjacentDid;
  UINT8  CurrentVid;
  UINT8  AdjacentVid;
  UINT32 CurrentFreq;
  UINT32 AdjacentFreq;

  if (pSetup_Config->CbsCpuPst3Did > 0x1A) {
    pSetup_Config->CbsCpuPst3Did &= 0xFE;
  }

  // Check settings
  GetPstateSetting (pSetup_Config, PSTATE_3, &CurrentFreq, &CurrentFid, &CurrentDid, &CurrentVid);
  GetPstateSetting (pSetup_Config, PSTATE_2, &AdjacentFreq, &AdjacentFid, &AdjacentDid, &AdjacentVid);
  if (AdjacentFreq == 0) {
    pSetup_Config->CbsCpuPstCustomP3 = AUTO_PSTATE;
    return;
  }
  if (CurrentFreq != 0) {
    if (CurrentFreq > AdjacentFreq) {
      pSetup_Config->CbsCpuPst3Fid = AdjacentFid;
      pSetup_Config->CbsCpuPst3Did = AdjacentDid;
    }
  }
  if (CurrentVid != DEFAULT_VID) {
    if (CurrentVid < AdjacentVid) {
      pSetup_Config->CbsCpuPst3Vid = AdjacentVid;
    }
  }

  GetPstateSetting (pSetup_Config, PSTATE_4, &AdjacentFreq, &AdjacentFid, &AdjacentDid, &AdjacentVid);
  if (AdjacentFreq != 0) {
    if (CurrentFreq < AdjacentFreq) {
      pSetup_Config->CbsCpuPst3Fid = AdjacentFid;
      pSetup_Config->CbsCpuPst3Did = AdjacentDid;
    }
  }
  if (AdjacentVid != DEFAULT_VID) {
    if (CurrentVid > AdjacentVid) {
      pSetup_Config->CbsCpuPst3Vid = AdjacentVid;
    }
  }

  // Display frequency
  if (pSetup_Config->CbsCpuPst3Did == 0) {
    pSetup_Config->CbsCpuCofP3 = 0;
  } else {
    pSetup_Config->CbsCpuCofP3 = GetPstateInfoFeq (pSetup_Config->CbsCpuPst3Fid, pSetup_Config->CbsCpuPst3Did);
  }

  // Display voltage
  if ((pSetup_Config->CbsCpuPst3Vid >= 0xF8) && (pSetup_Config->CbsCpuPst3Vid <= 0xFF)) {
    pSetup_Config->CbsCpuVoltageP3 = 0;
  } else {
    pSetup_Config->CbsCpuVoltageP3 = GetPstateInfoVoltage (pSetup_Config->CbsCpuPst3Vid);
  }
}

VOID
UpdatePstInfo4 (
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
  UINT8  CurrentFid;
  UINT8  AdjacentFid;
  UINT8  CurrentDid;
  UINT8  AdjacentDid;
  UINT8  CurrentVid;
  UINT8  AdjacentVid;
  UINT32 CurrentFreq;
  UINT32 AdjacentFreq;

  if (pSetup_Config->CbsCpuPst4Did > 0x1A) {
    pSetup_Config->CbsCpuPst4Did &= 0xFE;
  }

  // Check settings
  GetPstateSetting (pSetup_Config, PSTATE_4, &CurrentFreq, &CurrentFid, &CurrentDid, &CurrentVid);
  GetPstateSetting (pSetup_Config, PSTATE_3, &AdjacentFreq, &AdjacentFid, &AdjacentDid, &AdjacentVid);
  if (AdjacentFreq == 0) {
    pSetup_Config->CbsCpuPstCustomP4 = AUTO_PSTATE;
    return;
  }
  if (CurrentFreq != 0) {
    if (CurrentFreq > AdjacentFreq) {
      pSetup_Config->CbsCpuPst4Fid = AdjacentFid;
      pSetup_Config->CbsCpuPst4Did = AdjacentDid;
    }
  }
  if (CurrentVid != DEFAULT_VID) {
    if (CurrentVid < AdjacentVid) {
      pSetup_Config->CbsCpuPst4Vid = AdjacentVid;
    }
  }

  GetPstateSetting (pSetup_Config, PSTATE_5, &AdjacentFreq, &AdjacentFid, &AdjacentDid, &AdjacentVid);
  if (AdjacentFreq != 0) {
    if (CurrentFreq < AdjacentFreq) {
      pSetup_Config->CbsCpuPst4Fid = AdjacentFid;
      pSetup_Config->CbsCpuPst4Did = AdjacentDid;
    }
  }
  if (AdjacentVid != DEFAULT_VID) {
    if (CurrentVid > AdjacentVid) {
      pSetup_Config->CbsCpuPst4Vid = AdjacentVid;
    }
  }

  // Display frequency
  if (pSetup_Config->CbsCpuPst4Did == 0) {
    pSetup_Config->CbsCpuCofP4 = 0;
  } else {
    pSetup_Config->CbsCpuCofP4 = GetPstateInfoFeq (pSetup_Config->CbsCpuPst4Fid, pSetup_Config->CbsCpuPst4Did);
  }

  // Display voltage
  if ((pSetup_Config->CbsCpuPst4Vid >= 0xF8) && (pSetup_Config->CbsCpuPst4Vid <= 0xFF)) {
    pSetup_Config->CbsCpuVoltageP4 = 0;
  } else {
    pSetup_Config->CbsCpuVoltageP4 = GetPstateInfoVoltage (pSetup_Config->CbsCpuPst4Vid);
  }
}

VOID
UpdatePstInfo5 (
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
  UINT8  CurrentFid;
  UINT8  AdjacentFid;
  UINT8  CurrentDid;
  UINT8  AdjacentDid;
  UINT8  CurrentVid;
  UINT8  AdjacentVid;
  UINT32 CurrentFreq;
  UINT32 AdjacentFreq;

  if (pSetup_Config->CbsCpuPst5Did > 0x1A) {
    pSetup_Config->CbsCpuPst5Did &= 0xFE;
  }

  // Check settings
  GetPstateSetting (pSetup_Config, PSTATE_5, &CurrentFreq, &CurrentFid, &CurrentDid, &CurrentVid);
  GetPstateSetting (pSetup_Config, PSTATE_4, &AdjacentFreq, &AdjacentFid, &AdjacentDid, &AdjacentVid);
  if (AdjacentFreq == 0) {
    pSetup_Config->CbsCpuPstCustomP5 = AUTO_PSTATE;
    return;
  }
  if (CurrentFreq != 0) {
    if (CurrentFreq > AdjacentFreq) {
      pSetup_Config->CbsCpuPst5Fid = AdjacentFid;
      pSetup_Config->CbsCpuPst5Did = AdjacentDid;
    }
  }
  if (CurrentVid != DEFAULT_VID) {
    if (CurrentVid < AdjacentVid) {
      pSetup_Config->CbsCpuPst5Vid = AdjacentVid;
    }
  }

  GetPstateSetting (pSetup_Config, PSTATE_6, &AdjacentFreq, &AdjacentFid, &AdjacentDid, &AdjacentVid);
  if (AdjacentFreq != 0) {
    if (CurrentFreq < AdjacentFreq) {
      pSetup_Config->CbsCpuPst5Fid = AdjacentFid;
      pSetup_Config->CbsCpuPst5Did = AdjacentDid;
    }
  }
  if (AdjacentVid != DEFAULT_VID) {
    if (CurrentVid > AdjacentVid) {
      pSetup_Config->CbsCpuPst5Vid = AdjacentVid;
    }
  }

  // Display frequency
  if (pSetup_Config->CbsCpuPst5Did == 0) {
    pSetup_Config->CbsCpuCofP5 = 0;
  } else {
    pSetup_Config->CbsCpuCofP5 = GetPstateInfoFeq (pSetup_Config->CbsCpuPst5Fid, pSetup_Config->CbsCpuPst5Did);
  }

  // Display voltage
  if ((pSetup_Config->CbsCpuPst5Vid >= 0xF8) && (pSetup_Config->CbsCpuPst5Vid <= 0xFF)) {
    pSetup_Config->CbsCpuVoltageP5 = 0;
  } else {
    pSetup_Config->CbsCpuVoltageP5 = GetPstateInfoVoltage (pSetup_Config->CbsCpuPst5Vid);
  }
}

VOID
UpdatePstInfo6 (
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
  UINT8  CurrentFid;
  UINT8  AdjacentFid;
  UINT8  CurrentDid;
  UINT8  AdjacentDid;
  UINT8  CurrentVid;
  UINT8  AdjacentVid;
  UINT32 CurrentFreq;
  UINT32 AdjacentFreq;

  if (pSetup_Config->CbsCpuPst6Did > 0x1A) {
    pSetup_Config->CbsCpuPst6Did &= 0xFE;
  }

  // Check settings
  GetPstateSetting (pSetup_Config, PSTATE_6, &CurrentFreq, &CurrentFid, &CurrentDid, &CurrentVid);
  GetPstateSetting (pSetup_Config, PSTATE_5, &AdjacentFreq, &AdjacentFid, &AdjacentDid, &AdjacentVid);
  if (AdjacentFreq == 0) {
    pSetup_Config->CbsCpuPstCustomP6 = AUTO_PSTATE;
    return;
  }
  if (CurrentFreq != 0) {
    if (CurrentFreq > AdjacentFreq) {
      pSetup_Config->CbsCpuPst6Fid = AdjacentFid;
      pSetup_Config->CbsCpuPst6Did = AdjacentDid;
    }
  }
  if (CurrentVid != DEFAULT_VID) {
    if (CurrentVid < AdjacentVid) {
      pSetup_Config->CbsCpuPst6Vid = AdjacentVid;
    }
  }

  GetPstateSetting (pSetup_Config, PSTATE_7, &AdjacentFreq, &AdjacentFid, &AdjacentDid, &AdjacentVid);
  if (AdjacentFreq != 0) {
    if (CurrentFreq < AdjacentFreq) {
      pSetup_Config->CbsCpuPst6Fid = AdjacentFid;
      pSetup_Config->CbsCpuPst6Did = AdjacentDid;
    }
  }
  if (AdjacentVid != DEFAULT_VID) {
    if (CurrentVid > AdjacentVid) {
      pSetup_Config->CbsCpuPst6Vid = AdjacentVid;
    }
  }

  // Display frequency
  if (pSetup_Config->CbsCpuPst6Did == 0) {
    pSetup_Config->CbsCpuCofP6 = 0;
  } else {
    pSetup_Config->CbsCpuCofP6 = GetPstateInfoFeq (pSetup_Config->CbsCpuPst6Fid, pSetup_Config->CbsCpuPst6Did);
  }

  // Display voltage
  if ((pSetup_Config->CbsCpuPst6Vid >= 0xF8) && (pSetup_Config->CbsCpuPst6Vid <= 0xFF)) {
    pSetup_Config->CbsCpuVoltageP6 = 0;
  } else {
    pSetup_Config->CbsCpuVoltageP6 = GetPstateInfoVoltage (pSetup_Config->CbsCpuPst6Vid);
  }
}

VOID
UpdatePstInfo7 (
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
  UINT8  CurrentFid;
  UINT8  AdjacentFid;
  UINT8  CurrentDid;
  UINT8  AdjacentDid;
  UINT8  CurrentVid;
  UINT8  AdjacentVid;
  UINT32 CurrentFreq;
  UINT32 AdjacentFreq;

  if (pSetup_Config->CbsCpuPst7Did > 0x1A) {
    pSetup_Config->CbsCpuPst7Did &= 0xFE;
  }

  // Check settings
  GetPstateSetting (pSetup_Config, PSTATE_7, &CurrentFreq, &CurrentFid, &CurrentDid, &CurrentVid);
  GetPstateSetting (pSetup_Config, PSTATE_6, &AdjacentFreq, &AdjacentFid, &AdjacentDid, &AdjacentVid);
  if (AdjacentFreq == 0) {
    pSetup_Config->CbsCpuPstCustomP7 = AUTO_PSTATE;
    return;
  }
  if (CurrentFreq != 0) {
    if (CurrentFreq > AdjacentFreq) {
      pSetup_Config->CbsCpuPst7Fid = AdjacentFid;
      pSetup_Config->CbsCpuPst7Did = AdjacentDid;
    }
  }
  if (CurrentVid != DEFAULT_VID) {
    if (CurrentVid < AdjacentVid) {
      pSetup_Config->CbsCpuPst7Vid = AdjacentVid;
    }
  }

  // Display frequency
  if (pSetup_Config->CbsCpuPst7Did == 0) {
    pSetup_Config->CbsCpuCofP7 = 0;
  } else {
    pSetup_Config->CbsCpuCofP7 = GetPstateInfoFeq (pSetup_Config->CbsCpuPst7Fid, pSetup_Config->CbsCpuPst7Did);
  }

  // Display voltage
  if ((pSetup_Config->CbsCpuPst7Vid >= 0xF8) && (pSetup_Config->CbsCpuPst7Vid <= 0xFF)) {
    pSetup_Config->CbsCpuVoltageP7 = 0;
  } else {
    pSetup_Config->CbsCpuVoltageP7 = GetPstateInfoVoltage (pSetup_Config->CbsCpuPst7Vid);
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
  case PSTATE_3:
    pSetup_Config->CbsCpuPst3Fid = DEFAULT_FID;
    pSetup_Config->CbsCpuPst3Did = DEFAULT_DID;
    pSetup_Config->CbsCpuPst3Vid = DEFAULT_VID;
    break;
  case PSTATE_4:
    pSetup_Config->CbsCpuPst4Fid = DEFAULT_FID;
    pSetup_Config->CbsCpuPst4Did = DEFAULT_DID;
    pSetup_Config->CbsCpuPst4Vid = DEFAULT_VID;
    break;
  case PSTATE_5:
    pSetup_Config->CbsCpuPst5Fid = DEFAULT_FID;
    pSetup_Config->CbsCpuPst5Did = DEFAULT_DID;
    pSetup_Config->CbsCpuPst5Vid = DEFAULT_VID;
    break;
  case PSTATE_6:
    pSetup_Config->CbsCpuPst6Fid = DEFAULT_FID;
    pSetup_Config->CbsCpuPst6Did = DEFAULT_DID;
    pSetup_Config->CbsCpuPst6Vid = DEFAULT_VID;
    break;
  case PSTATE_7:
    pSetup_Config->CbsCpuPst7Fid = DEFAULT_FID;
    pSetup_Config->CbsCpuPst7Did = DEFAULT_DID;
    pSetup_Config->CbsCpuPst7Vid = DEFAULT_VID;
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
  if (Pstate <= PSTATE_3) {
    pSetup_Config->CbsCpuPstCustomP3 = Choice;
  }
  if (Pstate <= PSTATE_4) {
    pSetup_Config->CbsCpuPstCustomP4 = Choice;
  }
  if (Pstate <= PSTATE_5) {
    pSetup_Config->CbsCpuPstCustomP5 = Choice;
  }
  if (Pstate <= PSTATE_6) {
    pSetup_Config->CbsCpuPstCustomP6 = Choice;
  }
  if (Pstate <= PSTATE_7) {
    pSetup_Config->CbsCpuPstCustomP7 = Choice;
  }
}

UINT32
GetPstateInfoFeq (
  IN  UINT8 Fid,
  IN  UINT8 Did
  )
{
  return (UINT32) (DivU64x32 (MultU64x64 (200, Fid), Did));
}

UINT32
GetPstateInfoVoltage (
  IN  UINT8 Vid
  )
{
  return (UINT32) (1550000L - MultU64x64 (6250, Vid));
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
  UINT8 CustomPstateX;
  UINT8 CustomPstateXFid;
  UINT8 CustomPstateXDid;
  UINT8 CustomPstateXVid;
  PSTATE_MSR PstateMsr;

  switch (Pstate) {
  case PSTATE_0:
    CustomPstateX = pSetup_Config->CbsCpuPstCustomP0;
    CustomPstateXFid = pSetup_Config->CbsCpuPst0Fid;
    CustomPstateXDid = pSetup_Config->CbsCpuPst0Did;
    CustomPstateXVid = pSetup_Config->CbsCpuPst0Vid;
    break;
  case PSTATE_1:
    CustomPstateX = pSetup_Config->CbsCpuPstCustomP1;
    CustomPstateXFid = pSetup_Config->CbsCpuPst1Fid;
    CustomPstateXDid = pSetup_Config->CbsCpuPst1Did;
    CustomPstateXVid = pSetup_Config->CbsCpuPst1Vid;
    break;
  case PSTATE_2:
    CustomPstateX = pSetup_Config->CbsCpuPstCustomP2;
    CustomPstateXFid = pSetup_Config->CbsCpuPst2Fid;
    CustomPstateXDid = pSetup_Config->CbsCpuPst2Did;
    CustomPstateXVid = pSetup_Config->CbsCpuPst2Vid;
    break;
  case PSTATE_3:
    CustomPstateX = pSetup_Config->CbsCpuPstCustomP3;
    CustomPstateXFid = pSetup_Config->CbsCpuPst3Fid;
    CustomPstateXDid = pSetup_Config->CbsCpuPst3Did;
    CustomPstateXVid = pSetup_Config->CbsCpuPst3Vid;
    break;
  case PSTATE_4:
    CustomPstateX = pSetup_Config->CbsCpuPstCustomP4;
    CustomPstateXFid = pSetup_Config->CbsCpuPst4Fid;
    CustomPstateXDid = pSetup_Config->CbsCpuPst4Did;
    CustomPstateXVid = pSetup_Config->CbsCpuPst4Vid;
    break;
  case PSTATE_5:
    CustomPstateX = pSetup_Config->CbsCpuPstCustomP5;
    CustomPstateXFid = pSetup_Config->CbsCpuPst5Fid;
    CustomPstateXDid = pSetup_Config->CbsCpuPst5Did;
    CustomPstateXVid = pSetup_Config->CbsCpuPst5Vid;
    break;
  case PSTATE_6:
    CustomPstateX = pSetup_Config->CbsCpuPstCustomP6;
    CustomPstateXFid = pSetup_Config->CbsCpuPst6Fid;
    CustomPstateXDid = pSetup_Config->CbsCpuPst6Did;
    CustomPstateXVid = pSetup_Config->CbsCpuPst6Vid;
    break;
  case PSTATE_7:
    CustomPstateX = pSetup_Config->CbsCpuPstCustomP7;
    CustomPstateXFid = pSetup_Config->CbsCpuPst7Fid;
    CustomPstateXDid = pSetup_Config->CbsCpuPst7Did;
    CustomPstateXVid = pSetup_Config->CbsCpuPst7Vid;
    break;
  default:
    *CpuFreq = 0;
    *CpuVid = 0xFF;
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
      *CpuFreq = GetPstateInfoFeq ((UINT8) PstateMsr.Field.CpuFid_7_0, (UINT8) PstateMsr.Field.CpuDid);
      *CpuFid  = (UINT8) PstateMsr.Field.CpuFid_7_0;
      *CpuDid  = (UINT8) PstateMsr.Field.CpuDid;
      *CpuVid  = (UINT8) PstateMsr.Field.CpuVid;
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
  UINT8  i;
  UINT64 Dec;
  UINT64 Temp;

  Dec = 0;
  i = 0;
  while (Hex != 0) {
    Temp = Hex % 10;
    Temp = LShiftU64 (Temp, i);
    Dec |= Temp;
    Hex = Hex / 10;
    i = i + 4;
  }

  return Dec;
}
