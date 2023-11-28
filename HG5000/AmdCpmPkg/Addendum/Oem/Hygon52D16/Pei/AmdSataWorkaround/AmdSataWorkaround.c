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
#include <Ppi/AmdBoardIdPpi.h>
#include <Ppi/Pca9545aPpi.h>
#include <Ppi/Pca9535aPpi.h>
#include <Ppi/Ds125Br401aPpi.h>
#include "AmdSataWorkaround.h"

SATA_WA_PLATFORM SataWaPlatformList[] = {
    {{DIESEL      , 0, REVISION_A, 0}, 0x0F00},
    {{DIESEL      , 0, REVISION_B, 0}, 0x0F00},
    {{DIESEL_DEBUG, 0, REVISION_A, 0}, 0x0F00},
    {{DIESEL_SLT  , 0, REVISION_A, 0}, 0x0F00},
    {{DIESEL_SLT  , 0, REVISION_B, 0}, 0x0F00},
    {{DIESEL_DAP  , 0, REVISION_A, 0}, 0x0F00},
    {{DIESEL_DAP  , 0, REVISION_B, 0}, 0x0F00},
    {{OCTANE      , 0, REVISION_A, 0}, 0x0F00},
    {{OCTANE_DAP  , 0, REVISION_A, 0}, 0x0F00}
};

#define PLATFORM_LIST_COUNT  (sizeof (SataWaPlatformList) / sizeof (SATA_WA_PLATFORM))

EFI_STATUS
PlatformSearch (
    IN       AMD_PLATFORM_ID     *AmdPlatformId,
    OUT      UINT16              *BitMap
  )
{
  UINT32    Index;
  AMD_PLATFORM_ID *SataWaPlatformId;

  for (Index = 0; Index < PLATFORM_LIST_COUNT; Index++) {
    SataWaPlatformId = &SataWaPlatformList[Index].PlatformID;
    if (AmdPlatformId->BoardId == SataWaPlatformId->BoardId) {
        if (AmdPlatformId->RevisionId == SataWaPlatformId->RevisionId) {
            *BitMap = SataWaPlatformList[Index].SataInstanceBitMap;
            break;
        }
    }
  }
  if (Index == PLATFORM_LIST_COUNT) {
      DEBUG((EFI_D_ERROR, "Platform not found !!!\n"));
    return RETURN_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
SataWrokaround (
    IN      UINT16    BitMap
  )
{
  EFI_STATUS Status = EFI_SUCCESS;


  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD Sata Workaround Init PEIM driver
 *
 * This function defines CPM OEM definition tables and installs AmdCpmOemTablePpi.
 * It also defines callback function to update these definition table on run time.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
AmdSataWorkaroundPeiInit (
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  EFI_PEI_AMDBOARDID_PPI *AmdBoardIdPpi;
  EFI_PEI_PCA9545A_PPI   *Pca9545aPpi;
  EFI_PEI_DS125BR401A_PPI *Ds125Br401aPpi;
  UINT16 BitMap;
  AMD_EEPROM_ROOT_TABLE *AmdEepromRootTable;

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gAmdBoardIdPpiGuid,
     0,
     NULL,
     &AmdBoardIdPpi
     );

  AmdEepromRootTable = AmdBoardIdPpi->AmdEepromRootTable;
  Status = PlatformSearch(&AmdEepromRootTable->PlatformId, &BitMap);
  if (Status != EFI_SUCCESS) {
      //the platform not in the list, return success
      return EFI_SUCCESS;
  }

  DEBUG((EFI_D_ERROR, "SATA workaround Required !!! BitMap : %04x\n", BitMap));

  SataWrokaround(BitMap);

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gPca9545aPpiGuid,
     0,
     NULL,
     &Pca9545aPpi
     );

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gDs125Br401aPpiGuid,
     0,
     NULL,
     &Ds125Br401aPpi
     );

  //enable first field card I2C MUX channel 1
//  ControlByte = 0x01; //enable channel 1 
  Pca9545aPpi->Set(PeiServices, 0, 0x70, 0x01);

  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x06, 0x18);
  
  //Channel B 0-3 EQ

  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x04, 0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x0F, 0);  
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x16, 0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x1D, 0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x24, 0);
  

  //Channel A 0-3 EQ
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x2C, 0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x33, 0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x3A, 0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x41, 0);

  
  //Channel B 0-3 VOD
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x10, 0x0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x17, 0x0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x1E, 0x0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x25, 0x0);
  
  
  //Channel A 0-3 VOD
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x2D, 0x0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x34, 0x0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x3B, 0x0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x42, 0x0);

  
  //Channel B 0-3 DEM
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x11, 0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x18, 0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x1F, 0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x26, 0);

  
  //Channel A 0-3 DEM
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x2E, 0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x35, 0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x3C, 0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x43, 0);
  
//  Ds125Br401aPpi->Get(PeiServices, 0, 0xB0 >> 1, 0x2E, &data);
 // DEBUG((EFI_D_ERROR, "SATA Ch A DEM data   : %04x\n", data));
  
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x0D, 0x02);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x14, 0x02);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x1B, 0x02);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x22, 0x02);

  Pca9545aPpi->Set(PeiServices, 0, 0x70, 0x00);

  //Second Field card
  Pca9545aPpi->Set(PeiServices, 0, 0x71, 0x01);

  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x06, 0x18);
  //Channel B 0-3 EQ
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x0F, 0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x16, 0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x1D, 0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x24, 0);


  
  //Channel A 0-3 EQ
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x2C, 2);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x33, 2);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x3A, 2);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x41, 2);

  
  //Channel B 0-3 VOD
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x10, 0xAD);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x17, 0xAD);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x1E, 0xAD);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x25, 0xAD);

  //Channel A 0-3 VOD
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x2D, 0xA8);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x34, 0xA8);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x3B, 0xA8);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x42, 0xA8);

  //Channel B 0-3 DEM
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x11, 3);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x18, 3);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x1F, 3);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x26, 3);

  //Channel A 0-3 DEM
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x2E, 0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x35, 0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x3C, 0);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x43, 0);

  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x2A, 0x02);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x31, 0x02);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x38, 0x02);
  Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x3F, 0x02);
  Pca9545aPpi->Set(PeiServices, 0, 0x71, 0x00);

#if 0
  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gPca9535aPpiGuid,
     0,
     NULL,
     &Pca9535APpi
     );

  Pca9535APpi->Get(PeiServices, 0, 0x20, 0, &Data);
  DEBUG((EFI_D_ERROR, "PCA9535A Slave : 0x20 Register 0, 1 : %02x\n", Data));
  Pca9535APpi->Get(PeiServices, 0, 0x20, 2, &Data);
  DEBUG((EFI_D_ERROR, "PCA9535A Slave : 0x20 Register 2, 3 : %02x\n", Data));
  Pca9535APpi->Get(PeiServices, 0, 0x20, 4, &Data);
  DEBUG((EFI_D_ERROR, "PCA9535A Slave : 0x20 Register 4, 5 : %02x\n", Data));
  Pca9535APpi->Get(PeiServices, 0, 0x20, 6, &Data);
  DEBUG((EFI_D_ERROR, "PCA9535A Slave : 0x20 Register 6, 7 : %02x\n", Data));
#endif
  return Status;
}
