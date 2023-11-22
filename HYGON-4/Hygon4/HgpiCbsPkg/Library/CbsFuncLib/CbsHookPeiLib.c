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

#include <Library/BaseLib.h>
#include "CbsHookPeiLib.h"
#include <Ppi/HygonSocPcdInitPpi.h>

typedef struct _CBS_CONFIG HYGON_SYSTEM_CONFIGURATION;

STATIC PEI_CBS_HOOK_PPI  mCbsHookInstance =
{
  CbsHookFunc
};

EFI_GUID  SystemConfigurationGuid = HYGON_SYSTEM_CONFIGURATION_GUID;

STATIC EFI_PEI_PPI_DESCRIPTOR  mCbsHookInstancePpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonCbsHookPpiGuid,
  &mCbsHookInstance
};

EFI_STATUS
EFIAPI
HgpiPcdInitReadyCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );

EFI_PEI_NOTIFY_DESCRIPTOR  mHgpiPcdInitReadyCallback = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonPcdInitReadyPpiGuid,
  HgpiPcdInitReadyCallback
};


// byo230831 + >>
VOID 
GetHygonSetupData (
  IN       EFI_PEI_SERVICES     **PeiServices,
  OUT      VOID                 **SetupData
  )
{
  EFI_PEI_HOB_POINTERS             GuidHob;
  CBS_CONFIG                       *CbsData;
  UINT8                            *HgCbsDefData;                  // byo231012 +
  EFI_STATUS                       Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *Variable;
  UINTN                            VariableSize;
  BOOLEAN                          VarValid = FALSE;
  UINTN                            Index;                          // byo231012 +
  UINT8                            *p;                             // byo231012 +
  

  GuidHob.Raw = GetFirstGuidHob(&gHygonCbsVariableHobGuid);
  if(GuidHob.Raw != NULL){
    *SetupData = (VOID*)(GuidHob.Guid + 1);
    return;
  }

  DEBUG((EFI_D_INFO, "GetHygonSetupData\n"));

  CbsData = (CBS_CONFIG*)BuildGuidHob(&gHygonCbsVariableHobGuid, sizeof(CBS_CONFIG));
  ASSERT(CbsData != NULL);  
  ZeroMem(CbsData, sizeof(CBS_CONFIG));
  p = (UINT8*)CbsData;                                             // byo231012 +

// byo231012 + >>
  HgCbsDefData = (UINT8*)BuildGuidHob(&gHygonCbsVariableHgDefValHobGuid, sizeof(CBS_CONFIG));
  ASSERT(HgCbsDefData != NULL);  
  ZeroMem(HgCbsDefData, sizeof(CBS_CONFIG));
  CbsWriteDefalutValue(HgCbsDefData);
// byo231012 + <<
    
  Status = (**PeiServices).LocatePpi (PeiServices, &gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, &Variable);
  if (!EFI_ERROR (Status)) {
    VariableSize = sizeof(CBS_CONFIG);
    Status = Variable->GetVariable (
                         Variable,
                         L"HygonSetup",
                         &SystemConfigurationGuid,
                         NULL,
                         &VariableSize,
                         CbsData
                         );
    DEBUG((EFI_D_INFO, "GetVariable(HygonSetup):%r, L:%d %d\n", Status, VariableSize, sizeof(CBS_CONFIG)));
    if (!EFI_ERROR(Status) && VariableSize == sizeof(CBS_CONFIG)){
      VarValid = TRUE;
    }    
  }

  if(!VarValid){
    CbsWriteDefalutValue(p);                                                                                                    // byo231012 -
  } else if(PcdGet8(PcdIsPlatformCmosBad)){                                                                                     // byo231012 +
    for(Index=0;Index<sizeof(CBS_CONFIG);Index++){                                                                              // byo231012 +
      if(HgCbsDefData[Index] != p[Index]){                                                                                      // byo231012 +
        DEBUG((EFI_D_ERROR, "[ERROR] Cbs default value not equal [%d] %x vs %x\n", Index, HgCbsDefData[Index], p[Index]));      // byo231012 +
      }                                                                                                                         // byo231012 +
    }                                                                                                                           // byo231012 +
  }

  CbsData->CbsCmnCpuRdseedRdrandCap = 1;
  CbsData->CbsCmnCpuSmeeCap = 1;                               // byo230928 -
  CbsData->CbsCpuModel = (UINT8)GetHygonSocModel();
  CbsData->CbsCpuPkgType = GetSocPkgType();                    // byo230928 +
// byo230914 + >>
  if(!IsZeroBuffer(CbsData->Reserved, sizeof(CbsData->Reserved))){
    DEBUG((EFI_D_ERROR, "[ERROR] cbs reserved data should be zero.\n"));
  }
// byo230914 + <<  
  
  *SetupData = CbsData;  
}
// byo230831 + <<

EFI_STATUS
CbsHookFunc (
  IN       EFI_PEI_SERVICES     **PeiServices,
  IN OUT   VOID                 *HYGON_PARAMS,
  IN       UINTN                IdsHookId
  )
{
  CBS_CONFIG                       *CbsVariable;              // byo230831 -
  PEI_CBS_COMPONENT_STRUCT         *pCbsFunctionTable;
  PEI_CBS_FUNCTION_STRUCT          *pCurTable;
  CPUID_DATA                       CpuId;
  UINT64                           SocFamilyID;

// byo230831 - >>
  GetHygonSetupData(PeiServices, &CbsVariable);
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
      // Dispatch CBS function in PEI_CBS_FUNCTION_STRUCT.
      pCurTable = pCbsFunctionTable->pFuncTable;
      while (pCurTable->CBSFuncPtr != NULL) {
        if ((pCurTable->IdsHookId == IdsHookId) &&
            ((pCurTable->Family == SocFamilyID) || (pCurTable->Family == 0xFFFFFFFF))) {
          pCurTable->CBSFuncPtr (PeiServices, HYGON_PARAMS, CbsVariable);                   // byo230831 -
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
HgpiPcdInitReadyCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{
  CBS_CONFIG      *CbsVariable;                             // byo230831 -

// byo230831 - >>
  GetHygonSetupData(PeiServices, &CbsVariable);
  CbsSetHgpiPcds (CbsVariable);      // Call Auto Gen code to set HGPI PCDs
// byo230831 - <<
  
  return (EFI_SUCCESS);
}

EFI_STATUS
CbsInitEntry (
  IN CONST  EFI_PEI_SERVICES       **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = (**PeiServices).InstallPpi (PeiServices, &mCbsHookInstancePpiList);
  Status = (**PeiServices).NotifyPpi (PeiServices, &mHgpiPcdInitReadyCallback);

  return Status;
}

EFI_STATUS
CbsInterfaceFunc (
  IN  EFI_PEI_SERVICES       **PeiServices,
  IN  OUT VOID               *HYGON_PARAMS,
  IN      UINTN              IdsHookId
  )
{
  EFI_STATUS        Status;
  PEI_CBS_HOOK_PPI  *CbsHookPtr;

  Status = EFI_SUCCESS;
  if (PeiServices) {
    Status = (*PeiServices)->LocatePpi (
                               PeiServices,
                               &gHygonCbsHookPpiGuid,
                               0,
                               NULL,
                               &CbsHookPtr
                               );
    if (!EFI_ERROR (Status)) {
      Status = CbsHookPtr->CbsHookInterface (PeiServices, HYGON_PARAMS, IdsHookId);
    }
  }

  return Status;
}
