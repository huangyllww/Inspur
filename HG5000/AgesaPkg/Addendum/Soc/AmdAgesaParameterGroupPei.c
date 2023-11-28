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

  AmdSocAm4Pei.c
  Init Soc interface

Abstract:
--*/

#include <Library/DebugLib.h>
#include <AmdSoc.h>
#include <Ppi/AmdSocPcdInitPpi.h>
#include "AmdAgesaParameterGroupPei.h"


STATIC AMD_PCD_PTR mPcdAmdAgesaConfigParameter6 = {sizeof (EFI_GUID), &gAmdSocAgesaGroupIdentificationPpiGuid};

// the PCDs list here are simple for the reference. platform BIOS need to replace with the cofiguration PCD provide by IP driver.
AMD_PCD_LIST AmdConfigurationParameterListA[] = {
  {&gEfiAmdAgesaPkgTokenSpaceGuid, 0x0003001A, (UINT64)TRUE, PCD_BOOL},
  {&gEfiAmdAgesaPkgTokenSpaceGuid, 0x0003001B, (UINT64)0xAA, PCD_UINT8},
  {&gEfiAmdAgesaPkgTokenSpaceGuid, 0x0003001C, (UINT64)0x4321, PCD_UINT16},
  {&gEfiAmdAgesaPkgTokenSpaceGuid, 0x0003001D, (UINT64)0x12345678, PCD_UINT32},
  {&gEfiAmdAgesaPkgTokenSpaceGuid, 0x0003001E, (UINT64)0xDEADBEEFDEADBEEF, PCD_UINT64}
};

AMD_PCD_LIST AmdConfigurationParameterListB[] = {
  {&gEfiAmdAgesaPkgTokenSpaceGuid, 0x0003001A, (UINT64)FALSE, PCD_BOOL},
  {&gEfiAmdAgesaPkgTokenSpaceGuid, 0x0003001B, (UINT64)0xBB, PCD_UINT8},
  {&gEfiAmdAgesaPkgTokenSpaceGuid, 0x0003001C, (UINT64)0x7654, PCD_UINT16},
  {&gEfiAmdAgesaPkgTokenSpaceGuid, 0x0003001D, (UINT64)0x45678123, PCD_UINT32},
  {&gEfiAmdAgesaPkgTokenSpaceGuid, 0x0003001E, (UINT64)0xABCDEF0123456789, PCD_UINT64},
  {&gEfiAmdAgesaPkgTokenSpaceGuid, 0x0003001F, (UINT64)(UINTN)&mPcdAmdAgesaConfigParameter6, PCD_PTR}
};

AMD_PCD_LIST AmdConfigurationParameterListC[] = {
  {&gEfiAmdAgesaPkgTokenSpaceGuid, 0x0003001B, (UINT64)0xCC, PCD_UINT8},
  {&gEfiAmdAgesaPkgTokenSpaceGuid, 0x0003001C, (UINT64)0xAA55, PCD_UINT16},
  {&gEfiAmdAgesaPkgTokenSpaceGuid, 0x0003001D, (UINT64)0xDEADBEEF, PCD_UINT32}
};

//the number of the list depends on platform design. the number can be vary.
AMD_GROUP_LIST_HEADER AmdConfigurationParameterGroupList[] = {
{&gAmdConfigurationParameterListAGuid, (sizeof (AmdConfigurationParameterListA) / sizeof (AmdConfigurationParameterListA[0])), &AmdConfigurationParameterListA[0]},
{&gAmdConfigurationParameterListBGuid, (sizeof (AmdConfigurationParameterListB) / sizeof (AmdConfigurationParameterListB[0])), &AmdConfigurationParameterListB[0]},
{&gAmdConfigurationParameterListCGuid, (sizeof (AmdConfigurationParameterListC) / sizeof (AmdConfigurationParameterListC[0])), &AmdConfigurationParameterListC[0]}
};

STATIC AMD_PCD_PLATFORM_GROUP_LIST mAmdPcdPlatformGroupList = {
  0,
  (sizeof (AmdConfigurationParameterGroupList) / sizeof (AmdConfigurationParameterGroupList[0])),
  &AmdConfigurationParameterGroupList[0],
  &gAmdConfigurationParameterListAGuid
};

STATIC PEI_AMD_AGESA_PARAMETER_GROUP_ID_PPI mAmdAgesaParameterGroupIdPpi = {
  AMD_AGESA_PARAMETER_GROUP_ID_PPI_REVISION,
  &mAmdPcdPlatformGroupList,
  AmdSocCallBackPei,
};

STATIC EFI_PEI_PPI_DESCRIPTOR mPpiAgesaParameterGroupIdService = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdSocAgesaGroupIdentificationPpiGuid,
  &mAmdAgesaParameterGroupIdPpi
};

/*++

Routine Description:

  SOC Driver Entry. Initialize SOC device and publish silicon driver installation PPI

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
AmdAgesaParameterGroupPeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;

  DEBUG ((EFI_D_ERROR, "***************PEI AGESA Parameter Group Identification Driver Entry***************\n"));

  // Publish Agesa Parameter Group Identification Ppi
  // this ppi need to be installed before SOC driver
  Status = (*PeiServices)->InstallPpi (
                               PeiServices,
                               &mPpiAgesaParameterGroupIdService
                               );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_ERROR, "***************PEI AGESA Parameter Group Identification Driver Exit***************\n"));
  return (Status);
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function use for change AGESA configuration parameter PCD for multiplatform support.
 *
 * It will prepare AGESA configuration group list and return to SOC driver.
 * then the SOC driver will update the PCD value based on the list provide by this function.
 *
 * @param[in]   PeiServices
 * @param[in]   SocId
 * @param[out]  PlalformGroupGuid
 *
 */
EFI_STATUS
EFIAPI
AmdSocCallBackPei (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN       SOC_ID_STRUCT *SocId,
  OUT      EFI_GUID **PlalformGroupGuid
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;

  *PlalformGroupGuid = &gAmdConfigurationParameterListBGuid;

  if (SocId->SocFamilyID == F17_ZP_RAW_ID && SocId->PackageType == ZP_AM4) {
    *PlalformGroupGuid = &gAmdConfigurationParameterListBGuid;
  }

  if (SocId->SocFamilyID == F17_ZP_RAW_ID && SocId->PackageType == ZP_DM1) {
    *PlalformGroupGuid = &gAmdConfigurationParameterListBGuid;
  }
  if (SocId->SocFamilyID == F18_DN_RAW_ID && SocId->PackageType == ZP_DM1) {
    *PlalformGroupGuid = &gAmdConfigurationParameterListBGuid;
  }
    
  if (SocId->SocFamilyID == F17_ZP_RAW_ID && SocId->PackageType == ZP_SP3) {
    *PlalformGroupGuid = &gAmdConfigurationParameterListCGuid;
  }

  if (SocId->SocFamilyID == F17_ZP_RAW_ID && SocId->PackageType == ZP_SP4) {
    *PlalformGroupGuid = &gAmdConfigurationParameterListAGuid;
  }

  //HYGON
  if (SocId->SocFamilyID == F18_DN_RAW_ID && SocId->PackageType == ZP_SP3) {
    *PlalformGroupGuid = &gAmdConfigurationParameterListCGuid;
  }

  if (SocId->SocFamilyID == F18_DN_RAW_ID && SocId->PackageType == ZP_DM1) {
    *PlalformGroupGuid = &gAmdConfigurationParameterListBGuid;
	
  if (SocId->SocFamilyID == F18_DN_RAW_ID && SocId->PackageType == TR_SP3r2) {
    *PlalformGroupGuid = &gAmdConfigurationParameterListAGuid;
  }

  return Status;
}
