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

#include <Filecode.h>
#include "HGPI.h"
#include <Library/CcxSetMmioCfgBaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/PeiSocLogicalIdServicesLib.h>
#include <Library/PeiSocBistLoggingLib.h>
#include <Library/HygonIdsHookLib.h>
#include <Library/FabricResourceInitLib.h>
#include <Ppi/HygonSocPpi.h>
#include <Ppi/HygonSocPcdInitPpi.h>
#include <Ppi/HygonCcxPpi.h>
#include "HygonSocPei.h"

#define FILECODE  SOC_HYGONSOCSL2STPEI_HYGONSOCSL2STPEI_FILECODE

EFI_STATUS
EFIAPI
SocAfterPeiInitCompleteCallback (
  IN  CONST EFI_PEI_SERVICES                **PeiServices,
  IN        EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN        VOID                            *InvokePpi
  );

STATIC PEI_HYGON_SOC_PPI  mHygonSocInstallPpiTemplate = {
  HYGON_SOC_PPI_REVISION,
};

STATIC PEI_HYGON_PCD_INIT_READY_PPI  mHygonPcdInitReadyPpi = {
  PEI_HYGON_PCD_INIT_READY_PPI_REVISION,
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mHygonPcdInitReady = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonPcdInitReadyPpiGuid,
  &mHygonPcdInitReadyPpi
};

EFI_PEI_NOTIFY_DESCRIPTOR  mAfterCcxPeiInitCompletePpiCallback = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonCcxPeiInitCompletePpiGuid,
  SocAfterPeiInitCompleteCallback
};

EFI_STATUS
SocPublishSiliconPpi (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN CONST EFI_GUID            *SiliconDriverId
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  EFI_PEI_PPI_DESCRIPTOR  *SocPpiDescriptorTemplate;

  // Allocate memory for the PPI descriptor
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             (VOID **)&SocPpiDescriptorTemplate
                             );
  ASSERT (!EFI_ERROR (Status));

  SocPpiDescriptorTemplate->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  SocPpiDescriptorTemplate->Guid  = (EFI_GUID *)SiliconDriverId;
  SocPpiDescriptorTemplate->Ppi   = &mHygonSocInstallPpiTemplate;

  Status = (*PeiServices)->InstallPpi (
                             PeiServices,
                             SocPpiDescriptorTemplate
                             );
  return (Status);
}

/*++

Routine Description:

  SOC Driver Entry. Initialize SOC device and publish silicon driver installation PPI

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
HygonSocPeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  UINT8                                  i;
  EFI_STATUS                             Status = EFI_SUCCESS;
  PEI_HYGON_HGPI_PARAMETER_GROUP_ID_PPI  *PeiHygonHgpiParameterGroupIdPpi;
  HYGON_PCD_PLATFORM_GROUP_LIST          *HygonPcdPlatformGroupList;
  EFI_GUID                               *PlalformGroupGuid = NULL;
  UINT32                                 CpuModel;

  DEBUG ((EFI_D_INFO, "*****************************PEI SOC SL2 ST Driver Entry*********************\n"));    // byo230914 -

  // Init PCIE Configuration Space
  // CcxSetMmioCfgBaseLib ();
  // Init DF resource
  //FabricResourceInit ();

  //
  // Parameter initailization
  //
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonSocHgpiGroupIdentificationPpiGuid,
                             0,
                             NULL,
                             (VOID **)&PeiHygonHgpiParameterGroupIdPpi
                             );

  if (EFI_SUCCESS == Status) {
    HygonPcdPlatformGroupList = PeiHygonHgpiParameterGroupIdPpi->HygonConfigurationPcdGroupList;

    // Pass SOC ID to platform BIOS before PCD init.
    Status = PeiHygonHgpiParameterGroupIdPpi->HygonSocCallBackPei (PeiServices, &PlalformGroupGuid);
    if (NULL == PlalformGroupGuid) {
      PlalformGroupGuid = HygonPcdPlatformGroupList->PlalformGroupGuid;
    }

    // find Group Hgpi configuration list
    for (i = 0; i < HygonPcdPlatformGroupList->NumberOfGroupsList; i++) {
      if (CompareGuid (PlalformGroupGuid, (EFI_GUID *)HygonPcdPlatformGroupList->GroupList[i].PlalformGroupGuid)) {
        // Update PCD
        HygonPcdInit (HygonPcdPlatformGroupList->GroupList[i].NumberOfPcdEntries, (HYGON_PCD_LIST *)HygonPcdPlatformGroupList->GroupList[i].ListEntry);
      }
    }
  }

  //
  // Publish the logical ID protocol
  //
  Status = SocLogicalIdServicePpiInstall (PeiServices);

  IDS_HOOK (IDS_HOOK_BEGINNING_OF_HGPI, NULL, NULL);

  //
  // Publish Silicon Driver Installation PPI
  //
  Status = (*PeiServices)->InstallPpi (
                             PeiServices,
                             &mHygonPcdInitReady
                             );
  ASSERT (!EFI_ERROR (Status));
  
  // Init DF resource
  FabricResourceInit ();

  //
  // Publish Silicon Driver Installation PPI
  //
  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_EX_CPU) {
    for (i = 0; i < HyExSiliconDriverPpiListNumber; i++) {
      SocPublishSiliconPpi (PeiServices, HyExSiliconDriverPpiList[i]);
    }
  }
  if (CpuModel == HYGON_GX_CPU) {
    for (i = 0; i < HyGxSiliconDriverPpiListNumber; i++) {
      SocPublishSiliconPpi (PeiServices, HyGxSiliconDriverPpiList[i]);
    }
  }
  
  (**PeiServices).NotifyPpi (PeiServices, &mAfterCcxPeiInitCompletePpiCallback);
  DEBUG ((EFI_D_INFO, "*****************************PEI SOC SL2 ST Driver Exit*********************\n"));     // byo230914 -
  return (Status);
}

/*++

Routine Description:

  Callback routine once gHygonCcxPeiInitCompletePpiGuid has been installed to ensure the following services have been published
  - Core Topology Services
  - Fabric Topology Services
  - Nbio SMU Services

Arguments:
  **PeiServices
  *NotifyDesc
  *InvokePpi

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
SocAfterPeiInitCompleteCallback (
  IN  CONST EFI_PEI_SERVICES                **PeiServices,
  IN        EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN        VOID                            *InvokePpi
  )
{
  LogBistStatus (PeiServices);

  return EFI_SUCCESS;
}
