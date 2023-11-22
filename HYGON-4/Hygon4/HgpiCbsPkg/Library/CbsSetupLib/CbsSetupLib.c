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
#include <Library/HygonCbsSetupLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Protocol/HygonCbsHookProtocol.h>
#include "HygonCbsVariable.h"
#include "HygonCbsFormID.h"
#include "HygonSoc.h"

#include "PiDxe.h"
#include "HGPI.h"
#include <HPCB.h>
#include "HpcbCommon.h"
#include "Protocol/HygonHpcbProtocol.h"
#include "Library/HygonPspBaseLib.h"
#include "CbsCustomCorePstates.h"

VOID
PrepareCmnTypeAttrib (
  UINT32               *SizeOfAttrib,
  HPCB_PARAM_ATTRIBUTE *HpcbParaAttrib,
  UINT32               *SizeOfValue,
  UINT8                *HpcbParaValue,
  VOID                 *CbsVariable
  );

extern  EFI_BOOT_SERVICES  *gBS;
extern  EFI_GUID           gHygonHpcbDxeServiceProtocolGuid;

UINTN  CbsVariableSize = sizeof (CBS_CONFIG);

EFI_STATUS
HygonCbsSetupCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest,
  OUT UINT8                                  *IfrData,
  IN  EFI_HII_HANDLE                         HiiHandle
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  CBS_CONFIG  *pSetup_Config;

  pSetup_Config = (CBS_CONFIG *)IfrData;

  switch (Action) {
    case EFI_BROWSER_ACTION_CHANGING:
    case EFI_BROWSER_ACTION_CHANGED:
      switch (QuestionId) {
        case KEY_CUSTOM_PSTATE0:
        case KEY_CUSTOM_PSTATE1:
        case KEY_CUSTOM_PSTATE2:
        case KEY_PSTATE0_FID:
        case KEY_PSTATE0_DID:
        case KEY_PSTATE0_VID:
        case KEY_PSTATE1_FID:
        case KEY_PSTATE1_DID:
        case KEY_PSTATE1_VID:
        case KEY_PSTATE2_FID:
        case KEY_PSTATE2_DID:
        case KEY_PSTATE2_VID:
          CustomCorePstate (This, Action, QuestionId, Type, Value, ActionRequest, pSetup_Config, HiiHandle);
          break;
        default:
          break;
      }

      break;
    case EFI_BROWSER_ACTION_DEFAULT_STANDARD:
      CbsWriteDefalutValue (IfrData);
      switch (QuestionId) {
        default:
          break;
      }

      break;
    default:
      break;
  }

  return Status;
}

VOID
HygonSaveCbsConfigData (
  IN UINT8   *IfrData
  )
{
  EFI_STATUS                   Status;
  BOOLEAN                      HpcbEntryExist;
  TYPE_ATTRIB                  TypeAttrib;
  UINT64                       OldHpcbPtr;
  UINT32                       EntrySize;
  UINT64                       EntryDest;
  UINT32                       AttributeSize;
  UINT32                       ValueSize;
  UINT32                       TotalSizeExt;
  UINT8                        *DataStream;
  UINT8                        *ValueStream;
  UINT8                        *NewHpcbPtr;
  HYGON_HPCB_SERVICE_PROTOCOL  *mHpcbDxeServiceProtocol;

  DEBUG ((EFI_D_INFO, "[CbsSetupLib] HygonSaveCbsConfigData\n"));            // byo231012 +

  NewHpcbPtr = NULL;
  // Check the existence and size of HPCB  entry 0x60
  HpcbEntryExist = BIOSEntryInfo (BIOS_HPCB_INFO, INSTANCE_IGNORED, &TypeAttrib, &OldHpcbPtr, &EntrySize, &EntryDest);
  ASSERT (HpcbEntryExist == TRUE);

  // Calculate the buffer needed to store the external byte stream
  AttributeSize = 0;
  ValueSize     = 0;
  DataStream    = NULL;
  PrepareCmnTypeAttrib (&AttributeSize, (HPCB_PARAM_ATTRIBUTE *)DataStream, &ValueSize, DataStream, (CBS_CONFIG *)IfrData);
  DEBUG ((EFI_D_INFO, "PrepareCmnTypeAttrib: AttributeSize=%d, ValueSize=%d\n", AttributeSize, ValueSize));
  TotalSizeExt  = AttributeSize + ValueSize;
  TotalSizeExt += ((ALIGN_SIZE_OF_TYPE - (TotalSizeExt % ALIGN_SIZE_OF_TYPE)) & (ALIGN_SIZE_OF_TYPE - 1));

  if (0 != TotalSizeExt) {
    DEBUG ((EFI_D_INFO, "Replace CBS Common Options\n"));
    // Allocate buffer for new Type data stream
    DataStream = NULL;
    DataStream = AllocateZeroPool (TotalSizeExt);
    ASSERT (DataStream != NULL);
    if (DataStream == NULL) {
      return;
    }

    // Create new Type data stream
    ValueStream = &DataStream[AttributeSize];
    PrepareCmnTypeAttrib (&AttributeSize, (HPCB_PARAM_ATTRIBUTE *)DataStream, &ValueSize, ValueStream, (CBS_CONFIG *)IfrData);

    Status = gBS->LocateProtocol (&gHygonHpcbDxeServiceProtocolGuid, NULL, &mHpcbDxeServiceProtocol);
    if (EFI_ERROR (Status)) {
      ASSERT (FALSE);
    }

    Status = mHpcbDxeServiceProtocol->HpcbUpdateCbsData (mHpcbDxeServiceProtocol, DataStream, TotalSizeExt, FALSE);
    if (EFI_ERROR (Status)) {
      ASSERT (FALSE);
    }

    Status = mHpcbDxeServiceProtocol->HpcbFlushData (mHpcbDxeServiceProtocol);
    if (EFI_ERROR (Status)) {
      ASSERT (FALSE);
    }

    // Free temporary buffer
    Status = gBS->FreePool (DataStream);
    ASSERT (EFI_SUCCESS == Status);
  }

  return;
}

VOID
CbsOptionControlDependOnStepping (
  IN UINT8   *IfrData
  )
{
  SOC_LOGICAL_ID  LogicalId;
  CBS_CONFIG      *Setup_Config;

  Setup_Config = (CBS_CONFIG *)IfrData;
  Setup_Config->CbsCmnCpuRdseedRdrandCap = 1;
  Setup_Config->CbsCmnCpuSmeeCap = 0;
  Setup_Config->CbsCpuModel = (UINT8)GetHygonSocModel();
  Setup_Config->CbsCpuPkgType = GetSocPkgType();

  if(!BaseGetLogicalIdOnExecutingCore (&LogicalId)) {
    DEBUG ((EFI_D_ERROR, "call BaseGetLogicalIdOnExecutingCore() error\n"));
    return;
  }

  if ((LogicalId.Family == HYGON_FAMILY_18) && (LogicalId.Revision == HYGON_REV_F18_DN_A0)) {
    Setup_Config->CbsCmnCpuRdseedRdrandCap = 0;
  }

  if ((LogicalId.Family == HYGON_FAMILY_18) && (LogicalId.Revision >= HYGON_REV_F18_DN_B0)) {
    Setup_Config->CbsCmnCpuSmeeCap = 1;
  }
}
