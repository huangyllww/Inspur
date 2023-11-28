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

#include <Library/BaseLib.h>
#include "CbsHookPeiLib.h"
#include <Ppi/AmdSocPcdInitPpi.h>

typedef struct _CBS_CONFIG AMD_SYSTEM_CONFIGURATION;

STATIC PEI_CBS_HOOK_PPI mCbsHookInstance =
{
  CbsHookFunc
};

EFI_GUID SystemConfigurationGuid = AMD_SYSTEM_CONFIGURATION_GUID;

STATIC EFI_PEI_PPI_DESCRIPTOR mCbsHookInstancePpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCbsHookPpiGuid,
  &mCbsHookInstance
};

EFI_STATUS
EFIAPI
AgesaPcdInitReadyCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );

EFI_PEI_NOTIFY_DESCRIPTOR   mAgesaPcdInitReadyCallback = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdPcdInitReadyPpiGuid,
  AgesaPcdInitReadyCallback
};


EFI_STATUS
CbsHookFunc (
  IN       EFI_PEI_SERVICES     **PeiServices,
  IN OUT   VOID                 *AMD_PARAMS,
  IN       UINTN                IdsHookId
  )
{
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *Variable;
  CBS_CONFIG                      CbsVariable;
  UINTN                           VariableSize;
  EFI_STATUS                      Status;
  PEI_CBS_COMPONENT_STRUCT        *pCbsFunctionTable;
  PEI_CBS_FUNCTION_STRUCT         *pCurTable;
  CPUID_DATA                      CpuId;
  UINT64                          SocFamilyID;

  (**PeiServices).SetMem (&CbsVariable, sizeof (AMD_SYSTEM_CONFIGURATION), 0);

  Status = (**PeiServices).LocatePpi (PeiServices,&gEfiPeiReadOnlyVariable2PpiGuid,0,NULL,&Variable);
  if (!EFI_ERROR(Status)) {
    VariableSize = 0;
    Status = Variable->GetVariable (Variable,L"AmdSetup", &SystemConfigurationGuid, NULL, &VariableSize, &CbsVariable);
    if ((Status == EFI_BUFFER_TOO_SMALL) && (VariableSize == sizeof (AMD_SYSTEM_CONFIGURATION))) {
      Status = Variable->GetVariable (Variable,L"AmdSetup", &SystemConfigurationGuid, NULL, &VariableSize, &CbsVariable);
      if (EFI_ERROR(Status)) {
        // Set default value if fail to locate AmdSetup EfiVariable
         CbsWriteDefalutValue((UINT8 *)&CbsVariable);
      }
    } else {
      CbsWriteDefalutValue((UINT8 *)&CbsVariable);
    }
  } else {
    CbsWriteDefalutValue((UINT8 *)&CbsVariable);
  }

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
      // Dispatch CBS function in PEI_CBS_FUNCTION_STRUCT.
      pCurTable = pCbsFunctionTable->pFuncTable;
      while (pCurTable->CBSFuncPtr != NULL) {
        if ((pCurTable->IdsHookId == IdsHookId) &&
            ((pCurTable->Family == SocFamilyID) || (pCurTable->Family == 0xFFFFFFFF))) {
          pCurTable->CBSFuncPtr(PeiServices, AMD_PARAMS, &CbsVariable);
        }
        pCurTable++;
      }
    }
    pCbsFunctionTable++;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AgesaPcdInitReadyCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *Variable;
  UINTN                           VariableSize;
  EFI_STATUS                      Status;
  CBS_CONFIG                      CbsVariable;

  (**PeiServices).SetMem (&CbsVariable, sizeof (AMD_SYSTEM_CONFIGURATION), 0);

  Status = (**PeiServices).LocatePpi (PeiServices,&gEfiPeiReadOnlyVariable2PpiGuid,0,NULL,&Variable);
  if (!EFI_ERROR(Status)) {
    VariableSize = 0;
    Status = Variable->GetVariable (Variable, L"AmdSetup", &SystemConfigurationGuid, NULL, &VariableSize, &CbsVariable);
    if ((Status == EFI_BUFFER_TOO_SMALL) && (VariableSize == sizeof (AMD_SYSTEM_CONFIGURATION))) {
      Status = Variable->GetVariable (Variable, L"AmdSetup", &SystemConfigurationGuid, NULL, &VariableSize, &CbsVariable);
      if (EFI_ERROR(Status)) {
         CbsWriteDefalutValue((UINT8 *)&CbsVariable);
      }
    } else {
      CbsWriteDefalutValue((UINT8 *)&CbsVariable);
    }
  } else {
    CbsWriteDefalutValue((UINT8 *)&CbsVariable);
  }

  //Call Auto Gen code to set AGESA PCDs
  CbsSetAgesaPcds (&CbsVariable);
  return (EFI_SUCCESS);
}

EFI_STATUS CbsInitEntry(
  IN CONST  EFI_PEI_SERVICES       **PeiServices
  )
{
  EFI_STATUS   Status;

  Status = (**PeiServices).InstallPpi (PeiServices, &mCbsHookInstancePpiList);
  Status = (**PeiServices).NotifyPpi (PeiServices, &mAgesaPcdInitReadyCallback);

  return Status;
}

EFI_STATUS
CbsInterfaceFunc (
  IN  EFI_PEI_SERVICES       **PeiServices,
  IN  OUT VOID               *AMD_PARAMS,
  IN      UINTN              IdsHookId
  )
{
  EFI_STATUS          Status;
  PEI_CBS_HOOK_PPI    *CbsHookPtr;
  
  Status = EFI_SUCCESS;
  if (PeiServices) {
    Status = (*PeiServices)->LocatePpi (
                               PeiServices,
                               &gAmdCbsHookPpiGuid,
                               0,
                               NULL,
                               &CbsHookPtr
                               );
    if (!EFI_ERROR (Status)) {
      Status = CbsHookPtr->CbsHookInterface (PeiServices, AMD_PARAMS, IdsHookId);
    }
  }
  return Status;
}
