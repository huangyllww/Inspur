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

#include <HygonCpmPei.h>
#include <Library/DebugLib.h>
#include <Ppi/HygonBoardIdPpi.h>
#include <Ppi/HygonCpuInfoPpi.h>
#include <HygonSoc.h>
#include <Library/HygonSocBaseLib.h>
#include <SocLogicalId.h>
#include "HygonProjectInfoPei.h"

STATIC PEI_HYGON_CPU_INFO_PPI  mHygonExCpuInfoPpi = {
  HYGONCPUINFO_PPI_REVISION,
};

STATIC PEI_HYGON_CPU_INFO_PPI  mHygonGxCpuInfoPpi = {
  HYGONCPUINFO_PPI_REVISION,
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mHygonExCpuInfoPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonExCpuInfoInstallGuid,
  &mHygonExCpuInfoPpi
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mHygonGxCpuInfoPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonGxCpuInfoInstallGuid,
  &mHygonGxCpuInfoPpi
};

// Channel A = 0, Channel B = 1, Channel C = 2, Channel D = 3
// Channel E = 4, Channel F = 5, Channel G = 6, Channel H = 7
// Channel I = 8, Channel J = 9, Channel K = 0xA, Channel L = 0xB
// Requested   Translated
UINT8  HygonNanHaiVtb1Xlat[] = {
  0xA,  0xA, 0xB, 0xB, 0x9, 0x9, \
  0x4,  0x4, 0x5, 0x5, 0x3, 0x3, \
  0x0,  0x0, 0x1, 0x1, 0x2, 0x2, \
  0x6,  0x6, 0x7, 0x7, 0x8, 0x8, \
  0xFF, 0xFF
};

UINT8  HygonNanHaiVtb3Xlat[] = {
  0xA,  0xA, 0xB, 0xB, 0x9, 0x9, \
  0x4,  0x4, 0x5, 0x5, 0x3, 0x3, \
  0x0,  0x0, 0x1, 0x1, 0x2, 0x2, \
  0x6,  0x6, 0x7, 0x7, 0x8, 0x8, \
  0xFF, 0xFF
};

UINTN  SizeofNanHaiVtb1Xlat   = sizeof (HygonNanHaiVtb1Xlat)/sizeof (UINT8);
UINTN  SizeofNanHaiVtb3Xlat   = sizeof (HygonNanHaiVtb3Xlat)/sizeof (UINT8);

EFI_STATUS
InitHygonProjectEnv (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  HYGON_BOARD_ID          PlatformSelect;
  EFI_PEI_PPI_DESCRIPTOR  *mPpiProjectInstall;
  EFI_STATUS              Status;
  UINTN                   SizeofBuffer;
  UINT8                   PkgType;

  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (EFI_PEI_PPI_DESCRIPTOR), (VOID **)&mPpiProjectInstall);
  mPpiProjectInstall->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  mPpiProjectInstall->Ppi   = NULL;

  PlatformSelect = PcdGet8 (PcdPlatformSelect);
  PkgType = GetSocPkgType ();
  DEBUG ((DEBUG_INFO, "PkgType = %x, PlatformSelect=%d\n", PkgType, PlatformSelect));

  switch (PlatformSelect) {

    case HYGON_NANHAIVTB1:
      SizeofBuffer = SizeofNanHaiVtb1Xlat;
      PcdSetPtrS (PcdHpcbToBoardChanXLatTab, &SizeofBuffer, (VOID *)HygonNanHaiVtb1Xlat);
      mPpiProjectInstall->Guid = &gHygonNanHaiVtb1ProjectInstallGuid;
      break;

    case HYGON_NANHAIVTB3:
      SizeofBuffer = SizeofNanHaiVtb3Xlat;
      PcdSetPtrS (PcdHpcbToBoardChanXLatTab, &SizeofBuffer, (VOID *)HygonNanHaiVtb3Xlat);
      mPpiProjectInstall->Guid = &gHygonNanHaiVtb3ProjectInstallGuid;
      break;

    default:
      ASSERT (FALSE);
      break;
  }

  Status = (*PeiServices)->InstallPpi (PeiServices, mPpiProjectInstall);
  PcdSet8S(PcdPlatformSelect, (UINT8)PlatformSelect);
  
  return Status;
}


EFI_STATUS
PeiInitInstallHygonCpuInfo (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  UINT32              CpuModel;
  EFI_STATUS          Status;

  CpuModel = GetHygonSocModel();
  switch (CpuModel){
    case HYGON_EX_CPU:
      Status = (*PeiServices)->InstallPpi (PeiServices, &mHygonExCpuInfoPpiList);
      break;

    case HYGON_GX_CPU:
      Status = (*PeiServices)->InstallPpi (PeiServices, &mHygonGxCpuInfoPpiList);
      break;

    default:
      DEBUG ((DEBUG_INFO, "PeiInitInstallHygonCpuInfo: CpuModel = 0x%x, Not Match the Corrent Guid\n", CpuModel));
      ASSERT (FALSE);
  }
  
  DEBUG ((DEBUG_INFO, "PeiInitInstallHygonCpuInfo: CpuModel = 0x%x, Guid installed\n", CpuModel));
  return Status;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Entry point of the HYGON CPM BoardID PEIM driver
 *
 * This function registers the function to Get BoardId information.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
HygonProjectInfoPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = PeiInitInstallHygonCpuInfo (PeiServices);
  Status = InitHygonProjectEnv (PeiServices);

  return (Status);
}
