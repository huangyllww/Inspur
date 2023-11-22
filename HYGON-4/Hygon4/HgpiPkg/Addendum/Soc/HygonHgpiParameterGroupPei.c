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
/*++
Module Name:

  HygonSocDhm1Pei.c
  Init Soc interface

Abstract:
--*/

#include <Library/DebugLib.h>
#include <HygonSoc.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Ppi/HygonSocPcdInitPpi.h>
#include "HygonHgpiParameterGroupPei.h"

STATIC HYGON_PCD_PTR  mPcdHygonHgpiConfigParameter6 = { sizeof (EFI_GUID), &gHygonSocHgpiGroupIdentificationPpiGuid };

// the PCDs list here are simple for the reference. platform BIOS need to replace with the cofiguration PCD provide by IP driver.
HYGON_PCD_LIST  HygonConfigurationParameterListA[] = {
  { &gEfiHygonHgpiPkgTokenSpaceGuid, 0x0003001A, (UINT64)TRUE,               PCD_BOOL   },
  { &gEfiHygonHgpiPkgTokenSpaceGuid, 0x0003001B, (UINT64)0xAA,               PCD_UINT8  },
  { &gEfiHygonHgpiPkgTokenSpaceGuid, 0x0003001C, (UINT64)0x4321,             PCD_UINT16 },
  { &gEfiHygonHgpiPkgTokenSpaceGuid, 0x0003001D, (UINT64)0x12345678,         PCD_UINT32 },
  { &gEfiHygonHgpiPkgTokenSpaceGuid, 0x0003001E, (UINT64)0xDEADBEEFDEADBEEF, PCD_UINT64 }
};

HYGON_PCD_LIST  HygonConfigurationParameterListB[] = {
  { &gEfiHygonHgpiPkgTokenSpaceGuid, 0x0003001A, (UINT64)FALSE,                                PCD_BOOL   },
  { &gEfiHygonHgpiPkgTokenSpaceGuid, 0x0003001B, (UINT64)0xBB,                                 PCD_UINT8  },
  { &gEfiHygonHgpiPkgTokenSpaceGuid, 0x0003001C, (UINT64)0x7654,                               PCD_UINT16 },
  { &gEfiHygonHgpiPkgTokenSpaceGuid, 0x0003001D, (UINT64)0x45678123,                           PCD_UINT32 },
  { &gEfiHygonHgpiPkgTokenSpaceGuid, 0x0003001E, (UINT64)0xABCDEF0123456789,                   PCD_UINT64 },
  { &gEfiHygonHgpiPkgTokenSpaceGuid, 0x0003001F, (UINT64)(UINTN)&mPcdHygonHgpiConfigParameter6, PCD_PTR    }
};

HYGON_PCD_LIST  HygonConfigurationParameterListC[] = {
  { &gEfiHygonHgpiPkgTokenSpaceGuid, 0x0003001B, (UINT64)0xCC,       PCD_UINT8  },
  { &gEfiHygonHgpiPkgTokenSpaceGuid, 0x0003001C, (UINT64)0xAA55,     PCD_UINT16 },
  { &gEfiHygonHgpiPkgTokenSpaceGuid, 0x0003001D, (UINT64)0xDEADBEEF, PCD_UINT32 }
};

// the number of the list depends on platform design. the number can be vary.
HYGON_GROUP_LIST_HEADER  HygonConfigurationParameterGroupList[] = {
  { &gHygonConfigurationParameterListAGuid, (sizeof (HygonConfigurationParameterListA) / sizeof (HygonConfigurationParameterListA[0])), &HygonConfigurationParameterListA[0] },
  { &gHygonConfigurationParameterListBGuid, (sizeof (HygonConfigurationParameterListB) / sizeof (HygonConfigurationParameterListB[0])), &HygonConfigurationParameterListB[0] },
  { &gHygonConfigurationParameterListCGuid, (sizeof (HygonConfigurationParameterListC) / sizeof (HygonConfigurationParameterListC[0])), &HygonConfigurationParameterListC[0] }
};

STATIC HYGON_PCD_PLATFORM_GROUP_LIST  mHygonPcdPlatformGroupList = {
  0,
  (sizeof (HygonConfigurationParameterGroupList) / sizeof (HygonConfigurationParameterGroupList[0])),
  &HygonConfigurationParameterGroupList[0],
  &gHygonConfigurationParameterListAGuid
};

STATIC PEI_HYGON_HGPI_PARAMETER_GROUP_ID_PPI  mHygonHgpiParameterGroupIdPpi = {
  HYGON_HGPI_PARAMETER_GROUP_ID_PPI_REVISION,
  &mHygonPcdPlatformGroupList,
  HygonSocCallBackPei,
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mPpiHgpiParameterGroupIdService = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonSocHgpiGroupIdentificationPpiGuid,
  &mHygonHgpiParameterGroupIdPpi
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
HygonHgpiParameterGroupPeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  DEBUG ((EFI_D_ERROR, "***************PEI HGPI Parameter Group Identification Driver Entry***************\n"));

  // Publish Hgpi Parameter Group Identification Ppi
  // this ppi need to be installed before SOC driver
  Status = (*PeiServices)->InstallPpi (
                             PeiServices,
                             &mPpiHgpiParameterGroupIdService
                             );
  ASSERT (!EFI_ERROR (Status));

  DEBUG ((EFI_D_ERROR, "***************PEI HGPI Parameter Group Identification Driver Exit***************\n"));
  return (Status);
}

/*---------------------------------------------------------------------------------------*/

/**
 *
 * This function use for change HGPI configuration parameter PCD for multiplatform support.
 *
 * It will prepare HGPI configuration group list and return to SOC driver.
 * then the SOC driver will update the PCD value based on the list provide by this function.
 *
 * @param[in]   PeiServices
 * @param[in]   SocId
 * @param[out]  PlalformGroupGuid
 *
 */
EFI_STATUS
EFIAPI
HygonSocCallBackPei (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  OUT      EFI_GUID           **PlalformGroupGuid
  )
{
  UINTN  CddPerSocket;

  CddPerSocket = FabricTopologyGetNumberOfCddsOnSocket (0);

  *PlalformGroupGuid = &gHygonConfigurationParameterListBGuid;
  if (CddPerSocket >= 4) {
    *PlalformGroupGuid = &gHygonConfigurationParameterListCGuid;
  }

  if (CddPerSocket == 2) {
    *PlalformGroupGuid = &gHygonConfigurationParameterListAGuid;
  }

  if (CddPerSocket == 1) {
    *PlalformGroupGuid = &gHygonConfigurationParameterListBGuid;
  }

  return EFI_SUCCESS;
}
