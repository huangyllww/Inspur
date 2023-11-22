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
#include <PiDxe.h>                          // byo230831 +
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>               // byo230831 +
#include <Pi/PiBootMode.h>                  // byo230831 +
#include <Pi/PiHob.h>                       // byo230831 +
#include <Library/HobLib.h>                 // byo230831 +
#include "CbsHookDxeLib.h"

typedef struct _CBS_CONFIG HYGON_SYSTEM_CONFIGURATION;
extern EFI_RUNTIME_SERVICES  *gRT;

static CBS_DXE_PROTOCOL  mCbsProtocolInstance =
{
  CbsHookFunc,
  0
};

EFI_STATUS
CbsHookFunc (
  IN       EFI_BOOT_SERVICES    *BootServices,
  IN OUT   VOID                 *HYGON_PARAMS,
  IN       UINTN                IdsHookId
  )
{
  CBS_CONFIG                *pCbsConfig;
  DXE_CBS_COMPONENT_STRUCT  *pCbsFunctionTable;
  DXE_CBS_FUNCTION_STRUCT   *pCurTable;
  CPUID_DATA                CpuId;
  UINT64                    SocFamilyID;
  EFI_PEI_HOB_POINTERS      GuidHob;                           // byo230831 +
  
// byo230831 - >>
  GuidHob.Raw = GetFirstGuidHob(&gHygonCbsVariableHobGuid);
  ASSERT(GuidHob.Raw != NULL);
  pCbsConfig = (CBS_CONFIG*)(GuidHob.Guid + 1);
// byo230831 - <<

  AsmCpuid (
    0x80000001,
    &(CpuId.EAX_Reg),
    &(CpuId.EBX_Reg),
    &(CpuId.ECX_Reg),
    &(CpuId.EDX_Reg)
    );
  SocFamilyID = CpuId.EAX_Reg & RAW_FAMILY_ID_MASK;

  pCbsFunctionTable = &CbsCommonFuncTable[0];
  while (pCbsFunctionTable->pFuncTable != NULL) {
    if ((pCbsFunctionTable->Family == SocFamilyID) || (pCbsFunctionTable->Family == 0xFFFFFFFF)) {
      // Dispatch CBS function in DXE_CBS_FUNCTION_STRUCT.
      pCurTable = pCbsFunctionTable->pFuncTable;
      while (pCurTable->CBSFuncPtr != NULL) {
        if ((pCurTable->IdsHookId == IdsHookId) &&
            ((pCurTable->Family == SocFamilyID) || (pCurTable->Family == 0xFFFFFFFF))) {
          pCurTable->CBSFuncPtr (BootServices, HYGON_PARAMS, pCbsConfig);
        }

        pCurTable++;
      }
    }

    pCbsFunctionTable++;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
CbsInitEntry (
  IN  EFI_BOOT_SERVICES *BootServices
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;

  Handle = NULL;
  Status = BootServices->InstallProtocolInterface (
                           &Handle,
                           &gCbsProtocolGuid,
                           EFI_NATIVE_INTERFACE,
                           &mCbsProtocolInstance
                           );
  return Status;
}

EFI_STATUS
CbsInterfaceFunc (
  IN  EFI_BOOT_SERVICES      *BootServices,
  IN  OUT VOID               *HYGON_PARAMS,
  IN      UINTN              IdsHookId
  )
{
  EFI_STATUS        Status;
  CBS_DXE_PROTOCOL  *pCbsProtocol;

  Status = BootServices->LocateProtocol (&gCbsProtocolGuid, NULL, &pCbsProtocol);
  if (!EFI_ERROR (Status)) {
    Status = pCbsProtocol->CbsHookInterface (BootServices, HYGON_PARAMS, IdsHookId);
  }

  return Status;
}
