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

#include <AmdCpmPei.h>
#include <Library/DebugLib.h>
#include <AmdBoardId.h>
#include <AmdSoc.h>
#include "HygonBoardIdPei.h"


UINT8                   Hygon35N16ChanXLat[]      = {0x3,0x3, 0x2,0x2, 0x0,0x0, 0x1,0x1, 0x7,0x7, 0x6,0x6, 0x4,0x4, 0x5,0x5, 0xFF,0xFF};
UINT8                   Hygon35N16Sl1R2ChanXLat[] = {0x3,0x3, 0x2,0x2, 0x7,0x7, 0x6,0x6, 0xFF,0xFF};
UINT8                   Hygon65N32ChanXLat[]      = {0x3,0x3, 0x2,0x2, 0x0,0x0, 0x1,0x1, 0x7,0x7, 0x6,0x6, 0x4,0x4, 0x5,0x5, 0xFF,0xFF};
UINT8                   Hygon65N32Sl1R2ChanXLat[] = {0x3,0x3, 0x2,0x2, 0x7,0x7, 0x6,0x6, 0xFF,0xFF};
UINT8                   HygonDM1SLTChanXLat[]     = {0x0,0x0, 0x1,0x1, 0xFF,0xFF};
UINT8                   Hygon52D16ChanXLat[]      = {0x3,0x1, 0x2,0x0, 0x7,0x3, 0x6,0x2, 0xFF,0xFF};

UINTN                   Sizeof35N16ChanXLat = sizeof(Hygon35N16ChanXLat)/sizeof(UINT8);
UINTN                   Sizeof35N16Sl1R2ChanXLat = sizeof(Hygon35N16Sl1R2ChanXLat)/sizeof(UINT8);
UINTN                   Sizeof65N32ChanXLat = sizeof(Hygon65N32ChanXLat)/sizeof(UINT8);
UINTN                   Sizeof65N32Sl1R2ChanXLat = sizeof(Hygon65N32Sl1R2ChanXLat)/sizeof(UINT8);
UINTN                   SizeofDM1SLTChanXLat = sizeof(HygonDM1SLTChanXLat)/sizeof(UINT8);
UINTN                   Sizeof52D16ChanXLat = sizeof(Hygon52D16ChanXLat)/sizeof(UINT8);


EFI_STATUS InitHygonProjectEnv(
    IN CONST EFI_PEI_SERVICES     **PeiServices)
{
    // 35N16:      4
    // 65N32:      5
    // DM1SLT:     11
    // 52D16:      13
    // 35N16SL1r2: 16
    // 65N32SL1r2: 17
    HYGON_BOARD_ID          PlatformSelect;
    EFI_PEI_PPI_DESCRIPTOR  *mPpiProjectInstall;
    EFI_STATUS              Status;
    UINTN                   SizeofBuffer;
    UINT8                   PkgType;
    UINT32                  RegEbx;
    
    Status = (*PeiServices)->AllocatePool (PeiServices,sizeof (EFI_PEI_PPI_DESCRIPTOR),(VOID **)&mPpiProjectInstall);
    mPpiProjectInstall->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
    mPpiProjectInstall->Ppi   = NULL;
    
    PlatformSelect = PcdGet8(PcdPlatformSelect);
    AsmCpuid (0x80000001, NULL, &RegEbx, NULL, NULL);
    PkgType = (UINT8)(RegEbx >> 28);
    switch (PkgType)
    {
    case ZP_PKG_SP3:
        if (PlatformSelect == HYGON_35N16_2DIE_4DIE) {
            PlatformSelect = HYGON_35N16; //35N16
        } else if (PlatformSelect == HYGON_65N32_2DIE_4DIE) {
            PlatformSelect = HYGON_65N32; //65N32
        }
        break;
        
    case ZP_PKG_DM1:
        break;
        
    case ZP_PKG_SP3r2:
        if (PlatformSelect == HYGON_35N16_2DIE_4DIE) {
            PlatformSelect = HYGON_35N16SL1R2; //35N16SL1r2
        } else if (PlatformSelect == HYGON_65N32_2DIE_4DIE) {
            PlatformSelect = HYGON_65N32SL1R2; //65N32SL1r2
        }
        break;
        
    default:
        ASSERT(FALSE);
        break;
    }
    DEBUG((DEBUG_INFO, "PkgType = %x, PlatformSelect=%d\n", PkgType, PlatformSelect));
    
    switch (PlatformSelect)
    {
    //35N16
    case HYGON_35N16:
        SizeofBuffer = Sizeof35N16ChanXLat;
        PcdSetPtr (PcdApcbToBoardChanXLatTab, &SizeofBuffer, (VOID*)Hygon35N16ChanXLat);
        mPpiProjectInstall->Guid  = &gHygon35N16ProjectInstallGuid;
        break;

        //35N16SL1r2
    case HYGON_35N16SL1R2:
        SizeofBuffer = Sizeof35N16Sl1R2ChanXLat;
        PcdSetPtr (PcdApcbToBoardChanXLatTab, &SizeofBuffer, (VOID*)Hygon35N16Sl1R2ChanXLat);
        mPpiProjectInstall->Guid  = &gHygon35N16SL1r2ProjectInstallGuid;
        break;

        //65N32
    case HYGON_65N32:
        SizeofBuffer = Sizeof65N32ChanXLat;
        PcdSetPtr (PcdApcbToBoardChanXLatTab, &SizeofBuffer, (VOID*)Hygon65N32ChanXLat);
        mPpiProjectInstall->Guid  = &gHygon65N32ProjectInstallGuid;
        break;

        //65N32SL1r2
    case HYGON_65N32SL1R2:
        SizeofBuffer = Sizeof65N32Sl1R2ChanXLat;
        PcdSetPtr (PcdApcbToBoardChanXLatTab, &SizeofBuffer, (VOID*)Hygon65N32Sl1R2ChanXLat);
        mPpiProjectInstall->Guid  = &gHygon65N32SL1r2ProjectInstallGuid;
        break;

        //DM1SLT
    case HYGON_DM1SLT:
    case SUGON_W550_DM1SLT:
        SizeofBuffer = SizeofDM1SLTChanXLat;
        PcdSetPtr (PcdApcbToBoardChanXLatTab, &SizeofBuffer, (VOID*)HygonDM1SLTChanXLat);
        mPpiProjectInstall->Guid  = &gHygonDM1SLTProjectInstallGuid;
        break;

        //52D16      
    case HYGON_52D16:
        SizeofBuffer = Sizeof52D16ChanXLat;
        PcdSetPtr (PcdApcbToBoardChanXLatTab, &SizeofBuffer, (VOID*)Hygon52D16ChanXLat);
        mPpiProjectInstall->Guid  = &gHygon52D16ProjectInstallGuid;
        break;

    default:
        ASSERT(FALSE);
        break;
    }
    Status = (*PeiServices)->InstallPpi(PeiServices, mPpiProjectInstall);
    PcdSet8(PcdPlatformSelect, (UINT8)PlatformSelect);
    
    return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the Hygon CPM BoardID PEIM driver
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
HygonBoardIdPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{

  EFI_STATUS  Status;
  
  Status = InitHygonProjectEnv(PeiServices);

  return (Status);
}
