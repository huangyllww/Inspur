/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch HwAcpi controller
 *
 * Init Spread Spectrum features.
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
#define FILECODE PROC_FCH_TAISHAN_TSHWACPI_TSSSSERVICE_FILECODE

/**
 * FchInitResetAcpiMmioTable - Fch ACPI MMIO initial
 * during the power on stage.
 *
 *
 *
 *
 */
ACPI_REG_WRITE FchInitResetAcpiMmioTable[] =
{
  {00, 00, 0xB0, 0xAC},                                         /// Signature

  {PMIO_BASE >> 8,  FCH_PMIOA_REG5D, 0xFC, BIT1},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGD2, 0xCF, 0x00},
  {SMBUS_BASE >> 8, FCH_SMBUS_REG12, 0x00, BIT0},
  //{PMIO_BASE >> 8,  FCH_PMIOA_REG44 + 3, 0x67, 0x88},            /// Stop Boot timer
  {PMIO_BASE >> 8,  FCH_PMIOA_REG44 + 3, 0x67, 0x80},            /// Disable boot timer
  {PMIO_BASE >> 8,  FCH_PMIOA_REG20 + 2, 0x00, 0xFF},            /// Set CpuRstBTmr to 0x7FF
  {PMIO_BASE >> 8,  FCH_PMIOA_REG20 + 3, 0xF8, 0x07},            /// Set CpuRstBTmr to 0x7FF
  {PMIO_BASE >> 8,  FCH_PMIOA_REG00, 0xF7, 0x77},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGEC, 0xFB, BIT2},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG08, 0xFE, BIT2 + BIT4},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG08 + 1, 0xFF, BIT0},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG10, 0xFC, 0x00},
  //Byosoft- {PMIO_BASE >> 8,  FCH_PMIOA_REG54, 0x00, BIT4 + BIT6},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG54, 0x00, BIT4},          /// Byosoft+
  {PMIO_BASE >> 8,  FCH_PMIOA_REG74, 0xF6, BIT0},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGC4, 0xEE, 0x04},                 /// Release NB_PCIE_RST
  {PMIO_BASE >> 8, FCH_PMIOA_REGC0 +1, 0xF8, BIT2},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGC0 + 2, 0xBF, 0x40},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGBE, 0xDF, BIT5},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGC4, 0xFB, 0},

  {MISC_BASE >> 8,  FCH_MISC_REG6C + 2, 0x7F, BIT7},             // MISC 0x6C BIT23
  {MISC_BASE >> 8,  FCH_MISC_REG6C + 3, 0xF7, BIT3},             // MISC 0x6C BIT27

  {0xFF, 0xFF, 0xFF, 0xFF},
};

ACPI_REG_WRITE FchTaishanInitResetRtcextTable[] =
{
  {00, 00, 0xB0, 0xAC},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x01},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x02},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x03},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x04},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x10},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x11},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x12},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x13},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x14},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {0xFF, 0xFF, 0xFF, 0xFF},
};

/**
 * ProgramFchHwAcpiResetP  - Config SpreadSpectrum before PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ProgramFchHwAcpiResetP (
  IN VOID  *FchDataPtr
  )
{
  UINT32                    RegEax;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  // Clear UseAcpiStraps, PMIO_C8[4]
  //
  RwPmio (FCH_PMIOA_REGC8, AccessWidth8, 0xEF, 0x0, StdHeader);

  // PLAT-12314 APIC INIT may hit PIC/SMI/NMI/DMA
  AsmCpuid (0x80000001, &RegEax, NULL, NULL, NULL);
  if ((RegEax & 0xF0) == 0) {
    RwPmio (FCH_PMIOA_REG08, AccessWidth8, 0x9F, 0x60, StdHeader);
  }
}

VOID
FchInitEnableWdt (
  IN  VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;

  if (LocalCfgPtr->WdtEnable) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG00, AccessWidth8, 0x7F, BIT7);
  } else {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG00, AccessWidth8, 0x7F, 0);
  }
}

/**
 * ProgramResetRtcExt - Config RTC External registers
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ProgramResetRtcExt (
  IN  VOID     *FchDataPtr
  )
{
  UINT8                  RtcExtData;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //check if RTCext data lost
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG5E, AccessWidth8, 0, 0x01);
  ReadMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG5F, AccessWidth8, &RtcExtData);
  if ( RtcExtData == 0xFF ) {
    ProgramFchAcpiMmioTbl ((ACPI_REG_WRITE*) (&FchTaishanInitResetRtcextTable[0]), StdHeader);
  }
}
