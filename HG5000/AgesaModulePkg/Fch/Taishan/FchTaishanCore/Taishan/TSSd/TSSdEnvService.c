/* $NoKeywords:$ */
/**
 * @file
 *
 * Config TAISHAN SD
 *
 * Init SD Controller.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309083 $   @e \$Date: 2014-12-09 09:28:24 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*
*****************************************************************************
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
****************************************************************************
*/
#include "FchPlatform.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_TAISHAN_TSSD_TSSDENVSERVICE_FILECODE
/**
 * FchInitEnvSdProgram - Config SD controller before PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvSdProgram (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  // SD Configuration
  //
  if (LocalCfgPtr->Sd.SdConfig == SdDisable) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGD3, AccessWidth8, 0xBF, 0x00);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGE8, AccessWidth8, 0xFE, 0x00);
    ACPIMMIO8 (FCH_AOACx72_SD1_D3_CONTROL) &= ~ AOAC_PWR_ON_DEV;
  } else if (LocalCfgPtr->Sd.SdConfig != SdDump) {
    //ACPIMMIO8 (FCH_PMxE8_SDFlashCntrl) |= FCH_PMxE8_SDFlashCntrl_SdCd2Acpi; //Enable SD_CD0
    //ACPIMMIO8 (FCH_IOMUXx66_SD0_PWR_CTRL_AGPIO102) = 0; //ENH448923
    //ACPIMMIO32(FCH_MISCx68_MEMORY_POWER_SAVING_CONTROL) &= ~ BIT18; //SdBypassMemDsd

    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGD3, AccessWidth8, 0xBF, 0x40);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGE8, AccessWidth8, 0xFE, BIT0);

    if (LocalCfgPtr->Sd.SdSsid != NULL ) {
      RwPci ((TAISHAN_SD_BUS_DEV_FUN << 16) + SD_PCI_REG2C, AccessWidth32, 0, LocalCfgPtr->Sd.SdSsid, StdHeader);
    }

    RwPci ((TAISHAN_SD_BUS_DEV_FUN << 16) + SD_PCI_REGA4, AccessWidth32, 0x3FFFFFFF, ((UINT32) (LocalCfgPtr->Sd.SdSlotType) << 30), StdHeader);

    if ( LocalCfgPtr->Sd.SdConfig == SdVer2) {
// SD 2.0
//The following programming sequence sets up SD controller to operate in SD 2.0 mode. (UBTS538428).
//A. Program D14F7xA4[BaseFreq]= 32h.
//B. Program D14F7xA8=[Ddr50Sup]=0.
//C. Program D14F7xA8=[Sdr100Sup]=0.
//D. Program D14F7xA8=[Sdr50Sup]=0.
//E. Program D14F7xB0=[HostVersionSel]=1h.
      RwPci ((TAISHAN_SD_BUS_DEV_FUN << 16) + SD_PCI_REGA4, AccessWidth32, ~ (UINT32) (0xFF << 8), (UINT32) (0x32 << 8), StdHeader);
      RwPci ((TAISHAN_SD_BUS_DEV_FUN << 16) + SD_PCI_REGA8, AccessWidth32, ~ (UINT32) (0x01 << 2), (UINT32) (0x00 << 2), StdHeader);
      RwPci ((TAISHAN_SD_BUS_DEV_FUN << 16) + SD_PCI_REGA8, AccessWidth32, ~ (UINT32) (0x01 << 1), (UINT32) (0x00 << 1), StdHeader);
      RwPci ((TAISHAN_SD_BUS_DEV_FUN << 16) + SD_PCI_REGA8, AccessWidth32, ~ (UINT32) (0x01 << 0), (UINT32) (0x00 << 0), StdHeader);
      RwPci ((TAISHAN_SD_BUS_DEV_FUN << 16) + SD_PCI_REGB0, AccessWidth32, ~ (UINT32) (0xFF << 24), (UINT32) (0x01 << 24), StdHeader);
    } else {
      //  no SD3.0 POR on TS
    }
    if ( LocalCfgPtr->Sd.SdForce18 ) {
      RwPci ((TAISHAN_SD_BUS_DEV_FUN << 16) + SD_PCI_REGD0, AccessWidth32, ~ ((UINT32) (0x01 << 17)), ((UINT32) (0x01) << 17), StdHeader);
    }

    if (LocalCfgPtr->Sd.SdDbgConfig == 1) {
      //DMA clear BIT19,BIT20
      RwPci ((TAISHAN_SD_BUS_DEV_FUN << 16) + SD_PCI_REGA4, AccessWidth32, ~ (UINT32) (BIT19 + BIT20), 0, StdHeader);
    }

    if (LocalCfgPtr->Sd.SdDbgConfig == 2) {
      //PIO clear BIT19,BIT20,BIT22
      RwPci ((TAISHAN_SD_BUS_DEV_FUN << 16) + SD_PCI_REGA4, AccessWidth32, ~ (UINT32) (BIT19 + BIT20 + BIT22), 0, StdHeader);
    }
  }
}
