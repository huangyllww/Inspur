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
/*++
Module Name:

  AmdSocSp3ZpPei.c
  Init Soc interface

Abstract:
--*/

#include <Filecode.h>
#include "AGESA.h"
#include <Library/CcxSetMmioCfgBaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/AmdSocBaseLib.h>
#include <Library/PeiSocLogicalIdServicesLib.h>
#include <Library/PeiSocBistLoggingLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/FabricResourceInitLib.h>
#include <Ppi/AmdSocPpi.h>
#include <Ppi/AmdSocPcdInitPpi.h>
#include <Ppi/AmdCcxPpi.h>
#include "AmdSocSp3ZpPei.h"

#define FILECODE SOC_AMDSOCSP3ZPPEI_AMDSOCSP3ZPPEI_FILECODE

EFI_STATUS
EFIAPI
SocAfterPeiInitCompleteCallback (
  IN  CONST EFI_PEI_SERVICES                **PeiServices,
  IN        EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN        VOID                            *InvokePpi
  );

STATIC PEI_AMD_SOC_PPI mAmdSocInstallPpiTemplate = {
  AMD_SOC_PPI_REVISION,
};

STATIC PEI_AMD_PCD_INIT_READY_PPI mAmdPcdInitReadyPpi = {
  PEI_AMD_PCD_INIT_READY_PPI_REVISION,
};

STATIC EFI_PEI_PPI_DESCRIPTOR mAmdPcdInitReady = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdPcdInitReadyPpiGuid,
  &mAmdPcdInitReadyPpi
};

EFI_PEI_NOTIFY_DESCRIPTOR   mAfterCcxPeiInitCompletePpiCallback = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCcxPeiInitCompletePpiGuid,
  SocAfterPeiInitCompleteCallback
};

EFI_STATUS
SocPublishSiliconPpi (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN CONST EFI_GUID            *SiliconDriverId
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_PEI_PPI_DESCRIPTOR    *SocPpiDescriptorTemplate;

  // Allocate memory for the PPI descriptor
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             (VOID **)&SocPpiDescriptorTemplate
                             );
  ASSERT_EFI_ERROR ( Status);

  SocPpiDescriptorTemplate->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  SocPpiDescriptorTemplate->Guid  = (EFI_GUID*)SiliconDriverId;
  SocPpiDescriptorTemplate->Ppi   = &mAmdSocInstallPpiTemplate;

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
AmdSocSp3ZpPeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  UINT8               i;
  SOC_ID_STRUCT       SocIdSp3, SocIdAm4, SocIdDm1, SocIdSp4, HygonSp3, HygonDM1, HygonAm4, HygonSL1R2, *SocId;
  EFI_STATUS          Status = EFI_SUCCESS;
  PEI_AMD_AGESA_PARAMETER_GROUP_ID_PPI *PeiAmdAgesaParameterGroupIdPpi;
  AMD_PCD_PLATFORM_GROUP_LIST *AmdPcdPlatformGroupList;
  EFI_GUID *PlalformGroupGuid = NULL;

  DEBUG ((EFI_D_ERROR, "*****************************PEI SOC SP3 ZP Driver Entry*********************\n"));

  // Check Hardware Identification
  SocId = NULL;
  SocIdSp3.SocFamilyID = F17_ZP_RAW_ID;
  SocIdSp3.PackageType = ZP_SP3;
  SocIdAm4.SocFamilyID = F17_ZP_RAW_ID;
  SocIdAm4.PackageType = ZP_AM4;
  SocIdDm1.SocFamilyID = F17_ZP_RAW_ID;
  SocIdDm1.PackageType = ZP_DM1;
  SocIdSp4.SocFamilyID = F17_ZP_RAW_ID;
  SocIdSp4.PackageType = ZP_SP4;

  HygonSp3.SocFamilyID = F18_DN_RAW_ID;
  HygonSp3.PackageType = ZP_SP3;
  HygonDM1.SocFamilyID = F18_DN_RAW_ID;
  HygonDM1.PackageType = ZP_DM1;
  HygonAm4.SocFamilyID = F18_DN_RAW_ID; 
  HygonAm4.PackageType = ZP_AM4;
  HygonSL1R2.SocFamilyID = F18_DN_RAW_ID; 
  HygonSL1R2.PackageType = TR_SP3r2;

  SocId = NULL;
  if (SocHardwareIdentificationCheck (&SocIdSp3)){
    SocId = &SocIdSp3;
  }
  else if (SocHardwareIdentificationCheck (&SocIdAm4)){
    SocId = &SocIdAm4;
  }
  else if (SocHardwareIdentificationCheck (&SocIdDm1))
  {
    SocId = &SocIdDm1;
  }	
  else if (SocHardwareIdentificationCheck (&SocIdSp4)){
    SocId = &SocIdSp4;
  }
  else if (SocHardwareIdentificationCheck (&HygonSp3)){
    SocId = &HygonSp3;
  }
  else if (SocHardwareIdentificationCheck (&HygonDM1)){
    SocId = &HygonDM1;
  }
  else if (SocHardwareIdentificationCheck (&HygonAm4)){
    SocId = &HygonAm4;
  }
  else if (SocHardwareIdentificationCheck (&HygonSL1R2)){
    SocId = &HygonSL1R2;
  }
  if (SocId != NULL) {
    // Init PCIE Configuration Space
    CcxSetMmioCfgBaseLib ();
    // Init DF resource
    FabricResourceInit ();
    //
    // Parameter initailization
    //
    Status = (*PeiServices)->LocatePpi (
                PeiServices,
                &gAmdSocAgesaGroupIdentificationPpiGuid,
                0,
                NULL,
                (VOID**)&PeiAmdAgesaParameterGroupIdPpi
                );

    if (EFI_SUCCESS == Status) {
      AmdPcdPlatformGroupList = PeiAmdAgesaParameterGroupIdPpi->AmdConfigurationPcdGroupList;

      //Pass SOC ID to platform BIOS before PCD init.
      Status = PeiAmdAgesaParameterGroupIdPpi->AmdSocCallBackPei (PeiServices, SocId, &PlalformGroupGuid);
      if (NULL == PlalformGroupGuid) {
        PlalformGroupGuid = AmdPcdPlatformGroupList->PlalformGroupGuid;
      }

      //find Group Agesa configuration list
      for (i = 0; i < AmdPcdPlatformGroupList->NumberOfGroupsList; i++) {
        if (CompareGuid (PlalformGroupGuid, (EFI_GUID *)AmdPcdPlatformGroupList->GroupList[i].PlalformGroupGuid)) {
          // Update PCD
          AmdPcdInit (AmdPcdPlatformGroupList->GroupList[i].NumberOfPcdEntries, (AMD_PCD_LIST *)AmdPcdPlatformGroupList->GroupList[i].ListEntry);
        }
      }
    }

    //
    // Publish the logical ID protocol
    //
    Status = SocLogicalIdServicePpiInstall (PeiServices);

    IDS_HOOK (IDS_HOOK_BEGINNING_OF_AGESA, NULL, NULL);

    //
    //Publish Silicon Driver Installation PPI
    //
    Status = (*PeiServices)->InstallPpi (
                             PeiServices,
                             &mAmdPcdInitReady
                             );
    ASSERT_EFI_ERROR (Status);

    //
    //Publish Silicon Driver Installation PPI
    //
    for (i = 0 ; i < SiliconDriverPpiListNumber; i++) {
      SocPublishSiliconPpi (PeiServices, SiliconDriverPpiList[i]);
    }
  }

  (**PeiServices).NotifyPpi (PeiServices, &mAfterCcxPeiInitCompletePpiCallback);
  DEBUG ((EFI_D_ERROR, "*****************************PEI SOC SP3 ZP Driver Exit*********************\n"));
  return (Status);
}

/*++

Routine Description:

  Callback routine once gAmdCcxPeiInitCompletePpiGuid has been installed to ensure the following services have been published
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