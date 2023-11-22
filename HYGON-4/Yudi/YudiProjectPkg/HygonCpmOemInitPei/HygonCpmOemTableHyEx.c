/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPM OEM tables, and callback function.
 *
 * Contains code that defines OEM tables and callback function to override
 * OEM table on run time.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  OEM
 *
 */
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
#include <HygonCpmBaseIo.h>
#include "HygonCpmOemInitPeim.h"

//
// OEM CPM Table Definition
//

//
// Platform Id Table: Get Board Id from SMBUS
//

// Read SMBUS to detect board ID
// BOARD_ID_BIT2  BOARD_ID_BIT1  BOARD_ID_BIT0    PCBA
// 0                      0                          0             Normal  REVA
// 0                      1                          0             SLT REVA
// 1                      0                          0             DAP REVA
HYGON_CPM_PLATFORM_ID_TABLE2          gCpmPlatformIdTableHyEx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table contect
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYEX_CPM_SIGNATURE_GET_PLATFORM_ID2, sizeof (gCpmPlatformIdTableHyEx) / sizeof (UINT8), 0, 0, 0, 1 },
  {
    // UINT8  SmbusSelect;      ///< SMBUS Number
    // UINT8  SmbusAddress;     ///< SMBUS Address
    // UINT8  SmbusOffset;      ///< SMBUS Offset
    // UINT8  SmbusBit;         ///< SMBUS Bit
    0xFF,
  }
};

//
// Convert Table from Board Id to Platform Id
//

HYGON_CPM_PLATFORM_ID_CONVERT_TABLE   gCpmPlatformIdConvertTableHyEx = {
  { HYEX_CPM_SIGNATURE_GET_PLATFORM_ID_CONVERT, sizeof (gCpmPlatformIdConvertTableHyEx) / sizeof (UINT8), 0, 0, 0, 1 },
  {
    0xFFFF,
  }
};

//
// Pre-Init Table
//
HYGON_CPM_PRE_INIT_TABLE              gCpmPreInitTableHyEx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table contect
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYEX_CPM_SIGNATURE_PRE_INIT, sizeof (gCpmPreInitTableHyEx) / sizeof (UINT8), 0, 0, 0, 0x01 },
  {
    // {UINT8  Type;     // Register type. 0: FCH MMIO. 1: PCI
    // UINT8  Select;   // Register sub-type
    // UINT8  Offset;   // Register offset
    // UINT8  AndMask;  // AND mask
    // UINT8  OrMask;   // OR mask
    // UINT8  Stage;    // Stage number},
    0xFF,
  }
};

//
// GPIO Init Table
//
HYGON_CPM_GPIO_INIT_TABLE             gCpmGpioInitTableNanHaiVtb1HyEx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table content
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYEX_CPM_SIGNATURE_GPIO_INIT, sizeof (gCpmGpioInitTableNanHaiVtb1HyEx) / sizeof (UINT8), 0, 0, 0, 0x0000000F },
  { // Socket PhysicalDie gpio function          output            pullup
    //GPIO_DEF_V3 (0,  0, 0,    GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // CPLD_P0_PWRBTN_N
    //GPIO_DEF_V3 (0,  0, 1,    GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // None
    //GPIO_DEF_V3 (0,  0, 2,    GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // PCIe Wake, L-wake cpu
      GPIO_DEF_V3 (0,  0, 3  ,  GPIO_FUNCTION_0,  GPIO_OUTPUT_HIGH ,  GPIO_PU_EN),       // P0_SPD_SW_CTRL        

      GPIO_DEF_V3 (0,  0, 4  ,  GPIO_FUNCTION_0,  GPIO_INPUT ,  GPIO_PU_EN),       //FM_CPU0_BMC_SMI_ACTIVE_N ->AGPIO4/WOL1 
      GPIO_DEF_V3 (0,  0, 139,  GPIO_FUNCTION_1,  GPIO_INPUT,         GPIO_PU_EN),   //FM_ISBIOS_BMC_SMI_N ->UART0_INTR_AGPIO139

      GPIO_DEF_V3 (0,  0, 9,    GPIO_FUNCTION_1,  GPIO_NA,         GPIO_PU_PD_DIS),   //SGPIO_SATA0_CPU0_R_DOUT ->AGPIO9_SGPIO0_DATAOUT_MDIO1_SCL
      GPIO_DEF_V3 (0,  0, 10,    GPIO_FUNCTION_2,  GPIO_NA,         GPIO_PU_PD_DIS),   //SGPIO_SATA0_CPU0_R_CLOCK ->S0A3_GPIO_0_AGPIO10_0_SGPIO0_CLK_MDIO0_SCL
      GPIO_DEF_V3 (0,  0, 23,    GPIO_FUNCTION_1,  GPIO_NA,         GPIO_PU_PD_DIS),   //SGPIO_SATA0_CPU0_R_LOAD ->AGPIO23_SGPIO0_LOAD_MDIO1_SDA

      
      GPIO_DEF_V3 (0,  0, 19,     GPIO_FUNCTION_1,  GPIO_NA ,         GPIO_PU_PD_DIS),   // For clk gen(CK440)
      GPIO_DEF_V3 (0,  0, 20,     GPIO_FUNCTION_1,  GPIO_NA ,         GPIO_PU_PD_DIS),   // For clk gen(CK440)
    //GPIO_DEF_V3 (0,  0, 21,   GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // LPC BUS PD_N
    //GPIO_DEF_V3 (0,  0, 22,   GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // LPC BUS PME_N
    //GPIO_DEF_V3 (0,  0, 23,   GPIO_FUNCTION_0,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // NA
    //GPIO_DEF_V3 (0,  0, 26,   GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // P0_PCIE_RST_N
    //GPIO_DEF_V3 (0,  0, 27,   GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // P0_PCIE_RST3_N    
      GPIO_DEF_V3 (0,  0, 29,   GPIO_FUNCTION_0,  GPIO_OUTPUT_LOW ,   GPIO_PU_PD_DIS),   // AGPI029
      
      GPIO_DEF_V3 (0,  0, 30,   GPIO_FUNCTION_0,  GPIO_INPUT ,  GPIO_PU_EN),       //j_t PU_BIOS_WP_R_N ->AGPIO30_SGPIO3_LOAD_AGPIO30
      GPIO_DEF_V3 (0,  0, 31,   GPIO_FUNCTION_0,  GPIO_INPUT ,  GPIO_PU_EN),       //j_t PU_MFG_MODE_N ->AGPIO31_SGPIO3_CLK_AGPIO31

      //GPIO_DEF_V3 (0,  0, 32,   GPIO_FUNCTION_0,  GPIO_OUTPUT_LOW ,   GPIO_PU_EN),       // AGPIO32
      GPIO_DEF_V3 (0,  0, 10,   GPIO_FUNCTION_3,  GPIO_NA ,           GPIO_PU_PD_DIS),   // MDIO0_SCL
      GPIO_DEF_V3 (0,  0, 40,   GPIO_FUNCTION_2,  GPIO_NA ,           GPIO_PU_PD_DIS),   // MDIO0_SDA
    //GPIO_DEF_V3 (0,  0, 67,   GPIO_FUNCTION_1,  GPIO_NA ,           GPIO_PU_PD_DIS),   // SVI3 Reset_N
    //GPIO_DEF_V3 (0,  0, 70,   GPIO_FUNCTION_0,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // HG6 RSVD strap pin,BOOT BIOS image select
    //GPIO_DEF_V3 (0,  0, 74,   GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // LPC bus 33MHz CLK or ESPI CLK
    //GPIO_DEF_V3 (0,  0, 75,   GPIO_FUNCTION_1,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // Reserved strap pin for next generation
    //GPIO_DEF_V3 (0,  0, 76,   GPIO_FUNCTION_1,  GPIO_NA ,           GPIO_PU_PD_DIS),   // SPI_TPM_CS_N
    //GPIO_DEF_V3 (0,  0, 86,   GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // LPC System Management Interrupt
    //GPIO_DEF_V3 (0,  0, 87 ,  GPIO_FUNCTION_2,  GPIO_INPUT ,        GPIO_PU_EN),       // P0_LPC_SERIRQ
    //GPIO_DEF_V3 (0,  0, 88,   GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // LPC BUS CLKRUN
      GPIO_DEF_V3 (0,  0, 89 ,  GPIO_FUNCTION_2,  GPIO_NA ,           GPIO_PU_PD_DIS),   // P0_HP_ALERT_N 
    //GPIO_DEF_V3 (0,  0, 104,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // LPC BUS DATA0
    //GPIO_DEF_V3 (0,  0, 105,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // LPC BUS DATA1
    //GPIO_DEF_V3 (0,  0, 106,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // LPC BUS DATA2
    //GPIO_DEF_V3 (0,  0, 107,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // LPC BUS DATA3
      GPIO_DEF_V3 (0,  0, 108,  GPIO_FUNCTION_1,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // ESPI Alert 
    //GPIO_DEF_V3 (0,  0, 109,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // LPC BUS FRAME
      GPIO_DEF_V3 (0,  0, 113,  GPIO_FUNCTION_1,  GPIO_NA ,           GPIO_PU_PD_DIS),   // I2C2 SCL connect with VDDIO_S3 VR controller
      GPIO_DEF_V3 (0,  0, 114,  GPIO_FUNCTION_1,  GPIO_NA ,           GPIO_PU_PD_DIS),   // I2C2 SDA connect with VDDIO_S3 VR controller
    //GPIO_DEF_V3 (0,  0, 117,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // SPI BUS CLK
    //GPIO_DEF_V3 (0,  0, 118,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // SPI BUS CS
    //GPIO_DEF_V3 (0,  0, 119,  GPIO_FUNCTION_2,  GPIO_NA ,           GPIO_PU_PD_DIS),   // SPI BUS IRQ
    //GPIO_DEF_V3 (0,  0, 120,  GPIO_FUNCTION_3,  GPIO_NA ,           GPIO_PU_PD_DIS),   // SPI BUS
    //GPIO_DEF_V3 (0,  0, 121,  GPIO_FUNCTION_4,  GPIO_NA ,           GPIO_PU_PD_DIS),   // SPI BUS
    //GPIO_DEF_V3 (0,  0, 122,  GPIO_FUNCTION_6,  GPIO_NA ,           GPIO_PU_PD_DIS),   // SPI ROM WP
    //GPIO_DEF_V3 (0,  0, 129,  GPIO_FUNCTION_0,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // ESPI and KBD reset
    //GPIO_DEF_V3 (0,  0, 133,  GPIO_FUNCTION_5,  GPIO_NA ,           GPIO_PU_PD_DIS),   // SPI ROM HOLD
      GPIO_DEF_V3 (0,  0, 135,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_EN),       //j_t UART_CPU0_UART0_CTS ->UART0_CTS_L_UART2_RXD_EGPIO135
      GPIO_DEF_V3 (0,  0, 137,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_EN),       //UART_CPU0_UART0_RTS ->UART0_RTS_L_UART2_TXD_EGPIO137
      GPIO_DEF_V3 (0,  0, 136,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_EN),       //UART_CPU0_UART0_RX ->UART0_RXD_EGPIO136
      GPIO_DEF_V3 (0,  0, 138,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_EN),       //UART_CPU0_UART0_TX ->UART0_TXD_EGPIO138

   // GPIO_DEF_V3 (0,  0, 141,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // UART1 RXD
   // GPIO_DEF_V3 (0,  0, 143,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // UART1 TXD
    //GPIO_DEF_V3 (0,  0, 145,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // None
    //GPIO_DEF_V3 (0,  0, 146,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // For hotplug
      GPIO_DEF_V3 (0,  0, 147,  GPIO_FUNCTION_2,  GPIO_NA ,           GPIO_PU_PD_DIS),   //j_t CPU0_PASSWORD_CLEAR_N ->I2C1_SCL_EGPIO147_SFP_SCL
      GPIO_DEF_V3 (0,  0, 148,  GPIO_FUNCTION_2,  GPIO_NA ,           GPIO_PU_PD_DIS),   //j_t CPU0_BIOS_RCVR_MODE_N ->I2C1_SDA_EGPIO148_SFP_SDA

      GPIO_DEF_V3 (0,  0, 256,  GPIO_FUNCTION_0,  GPIO_INPUT,   GPIO_PU_EN    ),   //CPU1_G4_SATA_BP_PRSNT0_N ->AGPIO256_SGPIO2_DATAOUT_AGPIO256
      GPIO_DEF_V3 (0,  0, 257,  GPIO_FUNCTION_0,  GPIO_INPUT,   GPIO_PU_EN    ),   //CPU1_G5_SATA_BP_PRSNT0_N ->AGPIO257_SGPIO2_LOAD_AGPIO257
      GPIO_DEF_V3 (0,  0, 259,  GPIO_FUNCTION_0,  GPIO_INPUT,   GPIO_PU_EN    ),   //CPU0_G5_SATA_BP_PRSNT0_N ->AGPIO259_SGPIO2_DATAIN_AGPIO259
      GPIO_DEF_V3 (0,  0, 258,  GPIO_FUNCTION_0,  GPIO_INPUT,   GPIO_PU_EN    ),   //CPU0_G4_SATA_BP_PRSNT0_N ->AGPIO258_SGPIO2_CLK_AGPIO258

      GPIO_DEF_V3 (0,  0, 260,  GPIO_FUNCTION_1,  GPIO_NA,         GPIO_PU_PD_DIS),   //SGPIO_SATA1_CPU0_R_DOUT ->AGPIO260_SGPIO1_DATAOUT_AGPIO260
      GPIO_DEF_V3 (0,  0, 261,  GPIO_FUNCTION_1,  GPIO_NA,         GPIO_PU_PD_DIS),   //SGPIO_SATA1_CPU0_R_LOAD ->AGPIO261_SGPIO1_LOAD_AGPIO261
      GPIO_DEF_V3 (0,  0, 262,  GPIO_FUNCTION_1,  GPIO_NA ,          GPIO_PU_EN),     //SGPIO_SATA1_CPU0_R_CLOCK->AGPIO262_SGPIO1_CLK_AGPIO262

    //GPIO_DEF_V3 (0,  0, 263,  GPIO_FUNCTION_0,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // BIOS Recovery,  H-Normal,  L-Recovery
    //GPIO_DEF_V3 (1,  0, 0,    GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // CPLD_P0_PWRBTN_N
    //GPIO_DEF_V3 (1,  0, 1,    GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // None
    //GPIO_DEF_V3 (1,  0, 2,    GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // PCIe Wake, L-wake cpu
    //GPIO_DEF_V3 (1,  0, 3,    GPIO_FUNCTION_0,  GPIO_OUTPUT_HIGH,   GPIO_PU_EN    ),   // SPD Switch Control,  H-cpu as master,  L-bmc as master
      GPIO_DEF_V3 (1,  0, 9,    GPIO_FUNCTION_1,  GPIO_NA,         GPIO_PU_PD_DIS),   //SGPIO_SATA0_CPU0_R_DOUT ->AGPIO9_SGPIO0_DATAOUT_MDIO1_SCL
      GPIO_DEF_V3 (1,  0, 10,    GPIO_FUNCTION_2,  GPIO_NA,         GPIO_PU_PD_DIS),   //SGPIO_SATA0_CPU0_R_CLOCK ->S0A3_GPIO_0_AGPIO10_0_SGPIO0_CLK_MDIO0_SCL
      GPIO_DEF_V3 (1,  0, 23,    GPIO_FUNCTION_1,  GPIO_NA,         GPIO_PU_PD_DIS),   //SGPIO_SATA0_CPU0_R_LOAD ->AGPIO23_SGPIO0_LOAD_MDIO1_SDA

    //GPIO_DEF_V3 (1,  0, 21,   GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // LPC BUS PD_N
    //GPIO_DEF_V3 (1,  0, 26,   GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // P0_PCIE_RST_N
    //GPIO_DEF_V3 (1,  0, 27,   GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // P0_PCIE_RST3_N
    //GPIO_DEF_V3 (1,  0, 29,   GPIO_FUNCTION_0,  GPIO_OUTPUT_HIGH,   GPIO_PU_EN    ),   // BIOS POST_COMPLETE#
    //GPIO_DEF_V3 (1,  0, 30,   GPIO_FUNCTION_1,  GPIO_OUTPUT_HIGH,   GPIO_PU_EN    ),   // H-disable, L-enable   156.25M clock source
      GPIO_DEF_V3 (1,  0, 40,   GPIO_FUNCTION_2,  GPIO_NA ,           GPIO_PU_PD_DIS),   // MDIO0_SDA
      GPIO_DEF_V3 (1,  0, 67,   GPIO_FUNCTION_1,  GPIO_NA ,           GPIO_PU_PD_DIS),   // SVI3 Reset_N
    //GPIO_DEF_V3 (1,  0, 70,   GPIO_FUNCTION_0,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // HG6 RSVD strap pin,BOOT BIOS image select
    //GPIO_DEF_V3 (1,  0, 74,   GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // LPC bus 33MHz CLK or ESPI CLK
    //GPIO_DEF_V3 (1,  0, 75,   GPIO_FUNCTION_1,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // Reserved strap pin for next generation
      GPIO_DEF_V3 (1,  0, 108,  GPIO_FUNCTION_1,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // ESPI Alert 
    //GPIO_DEF_V3 (1,  0, 109,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // LPC BUS FRAME
      GPIO_DEF_V3 (1,  0, 113,  GPIO_FUNCTION_1,  GPIO_NA ,           GPIO_PU_PD_DIS),   // I2C2 SCL connect with VDDIO_S3 VR controller
      GPIO_DEF_V3 (1,  0, 114,  GPIO_FUNCTION_1,  GPIO_NA ,           GPIO_PU_PD_DIS),   // I2C2 SDA connect with VDDIO_S3 VR controller
    //GPIO_DEF_V3 (1,  0, 117,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // SPI BUS CLK
    //GPIO_DEF_V3 (1,  0, 118,  GPIO_FUNCTION_1,  GPIO_NA ,           GPIO_PU_PD_DIS),   // SPI BUS CS
    //GPIO_DEF_V3 (1,  0, 120,  GPIO_FUNCTION_3,  GPIO_NA ,           GPIO_PU_PD_DIS),   // SPI BUS
    //GPIO_DEF_V3 (1,  0, 121,  GPIO_FUNCTION_4,  GPIO_NA ,           GPIO_PU_PD_DIS),   // SPI BUS
    //GPIO_DEF_V3 (1,  0, 122,  GPIO_FUNCTION_6,  GPIO_NA ,           GPIO_PU_PD_DIS),   // SPI ROM WP
    //GPIO_DEF_V3 (1,  0, 129,  GPIO_FUNCTION_0,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // ESPI and KBD reset
    //GPIO_DEF_V3 (1,  0, 133,  GPIO_FUNCTION_5,  GPIO_NA ,           GPIO_PU_PD_DIS),   // SPI ROM HOLD
    //GPIO_DEF_V3 (1,  0, 136,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // UART0 RXD
    //GPIO_DEF_V3 (1,  0, 138,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // UART0 TXD
    //GPIO_DEF_V3 (1,  0, 145,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // None
    //GPIO_DEF_V3 (1,  0, 146,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // For hotplug
      GPIO_DEF_V3 (1,  0, 147,  GPIO_FUNCTION_1,  GPIO_NA ,           GPIO_PU_PD_DIS),   // I2C1 SCL
      GPIO_DEF_V3 (1,  0, 148,  GPIO_FUNCTION_1,  GPIO_NA ,           GPIO_PU_PD_DIS),   // I2C1 SDA
    //GPIO_DEF_V3 (1,  0, 256,  GPIO_FUNCTION_0,  GPIO_OUTPUT_HIGH,   GPIO_PU_EN    ),   // Reserved GPIO for CPLD
    //GPIO_DEF_V3 (1,  0, 257,  GPIO_FUNCTION_0,  GPIO_OUTPUT_HIGH,   GPIO_PU_EN    ),   // Reserved GPIO for CPLD
    //GPIO_DEF_V3 (1,  0, 258,  GPIO_FUNCTION_0,  GPIO_OUTPUT_HIGH,   GPIO_PU_EN    ),   // Reserved GPIO for CPLD
    //GPIO_DEF_V3 (1,  0, 259,  GPIO_FUNCTION_0,  GPIO_OUTPUT_HIGH,   GPIO_PU_EN    ),   // Reserved GPIO for CPLD
      GPIO_DEF_V3 (0,  0, 260,  GPIO_FUNCTION_1,  GPIO_NA,         GPIO_PU_PD_DIS),   //SGPIO_SATA1_CPU0_R_DOUT ->AGPIO260_SGPIO1_DATAOUT_AGPIO260
      GPIO_DEF_V3 (0,  0, 261,  GPIO_FUNCTION_1,  GPIO_NA,         GPIO_PU_PD_DIS),   //SGPIO_SATA1_CPU0_R_LOAD ->AGPIO261_SGPIO1_LOAD_AGPIO261
      GPIO_DEF_V3 (0,  0, 262,  GPIO_FUNCTION_1,  GPIO_NA ,          GPIO_PU_EN),     //SGPIO_SATA1_CPU0_R_CLOCK->AGPIO262_SGPIO1_CLK_AGPIO262
    //GPIO_DEF_V3 (1,  0, 263,  GPIO_FUNCTION_0,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // Reserved
    0xFF,
  }
};

//
// GEVENT Init Table
//
HYGON_CPM_GEVENT_INIT_TABLE           gCpmGeventInitTableNanHaiVtb1HyEx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table contect
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYEX_CPM_SIGNATURE_GEVENT_INIT, sizeof (gCpmGeventInitTableNanHaiVtb1HyEx) / sizeof (UINT8), 0, 0, 0, 0x00000001 },
  {// GEvent EventEnable   SciTrigE      SciLevl         SmiSciEn        SciS0En         SciMap      SmiTrig       SmiControl
    // GEVENT_DEFINITION (0x03,  EVENT_ENABLE, SCITRIG_HI,   SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_03,  SMITRIG_HI,   SMICONTROL_DISABLE), // GEVENT03: PM_INT_IN
    // GEVENT_DEFINITION (0x05,  EVENT_ENABLE, SCITRIG_HI,   SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_05,  SMITRIG_HI,   SMICONTROL_DISABLE), // GEVENT05: LAN_MEDIA_SENSE
    // GEVENT_DEFINITION (0x08,  EVENT_ENABLE, SCITRIG_LOW,  SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_08,  SMITRIG_LOW,  SMICONTROL_DISABLE), // GEVENT08: PCIE_WAKE_UP#
    // GEVENT_DEFINITION (0x0C,  EVENT_ENABLE, SCITRIG_LOW,  SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_12,  SMITRIG_LOW,  SMICONTROL_DISABLE), // GEVENT12: USB_OC#
    // GEVENT_DEFINITION (0x0D,  EVENT_ENABLE, SCITRIG_HI,   SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_13,  SMITRIG_HI,   SMICONTROL_DISABLE), // GEVENT13: LAN_LOW_POWER
    // GEVENT_DEFINITION (0x0E,  EVENT_ENABLE, SCITRIG_LOW,  SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_14,  SMITRIG_LOW,  SMICONTROL_DISABLE), // GEVENT14: LAN_SMART#
    // GEVENT_DEFINITION (0x0F,  EVENT_ENABLE, SCITRIG_LOW,  SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_15,  SMITRIG_LOW,  SMICONTROL_DISABLE), // GEVENT15: EVALCARD_ALERT#
    0xFF,
  }
};

//
// CPM GPIO Module
//

HYGON_CPM_GPIO_DEVICE_CONFIG_TABLE    gCpmGpioDeviceConfigTableNanHaiVtb1HyEx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table contect
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYEX_CPM_SIGNATURE_GPIO_DEVICE_CONFIG, sizeof (gCpmGpioDeviceConfigTableNanHaiVtb1HyEx) / sizeof (UINT8), 0, 0, 0,
    0x0000000F },
  {// DeviceId          Enable            Assert  Deassert  Hotplugs
    // GPIO_DEVICE_DEFINITION (DEVICE_ID_GBE,    CPM_DEVICE_ON,    0,      0,        0), // GBE
    // GPIO_DEVICE_DEFINITION (DEVICE_ID_BT,     CPM_DEVICE_ON,    0,      0,        0), // BT
    // GPIO_DEVICE_DEFINITION (DEVICE_ID_WLAN,   CPM_DEVICE_ON,    0,      0,        0), // WLAN
    // GPIO_DEVICE_DEFINITION (DEVICE_ID_PCIE_X16_SWITCH,   CPM_DEVICE_ON,    0,      0,        0), // PCIe 1x16/2x8 Switch
    // GPIO_DEVICE_DEFINITION (DEVICE_ID_SATAE_M2_SWITCH,   CPM_DEVICE_ON,    0,      0,        0), // SataExpress/M.2 Switch
    0xFF,
  }
};

//
// CPM Device Detection Table for Speedway
//
HYGON_CPM_GPIO_DEVICE_DETECTION_TABLE gCpmGpioDeviceDetectionTableNanHaiVtb1HyEx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table contect
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYEX_CPM_SIGNATURE_GPIO_DEVICE_DETECTION, sizeof (gCpmGpioDeviceDetectionTableNanHaiVtb1HyEx) / sizeof (UINT8), 0, 0, 0,
    0x0000000F },
  {
    // {UINT8  DeviceId; // Device Id
    // UINT8  Type;     // Detection type. 0: One GPIO pin. 1: Two GPIO pins. 2: Special Pin
    // UINT16 PinNum1;  // Pin number of GPIO 1
    // UINT8  Value1;   // Value of GPIO 1
    // UINT16 PinNum2;  // Pin number of GPIO 2
    // UINT8  Value2;   // Value of GPIO 2
    // UINT16 PinNum3;  // Pin number of GPIO 3
    // UINT8  Value3;   // Value of GPIO 3},
    0xFF,
  }
};

//
// CPM Device Reset Table
//
HYGON_CPM_GPIO_DEVICE_RESET_TABLE gCpmGpioDeviceResetTableNanHaiVtb1HyEx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table contect
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYEX_CPM_SIGNATURE_GPIO_DEVICE_RESET, sizeof (gCpmGpioDeviceResetTableNanHaiVtb1HyEx) / sizeof (UINT8), 0, 0, 0,
    0x0000000F },
  {
    // {UINT8  DeviceId;       // Device Id
    // UINT8  Mode;           // Reset mode     // 0: Reset Assert. // 1: Reset De-assert             // 2: Delay between Assert and Deassert
    // UINT8  Type;           // Register type  // 0: GPIO.         // 1: Special pin.if Mode = 0 or 1
    // UINT32 ((UINT16)Pin + ((UINT8)Value << 16));                 // GPIO pin value
    // UINT8  InitFlag;       // Init flag in post},
    0xFF,
  }
};

//
// CPM GPIO Device Init Table (Power On/Off)
//
HYGON_CPM_GPIO_DEVICE_POWER_TABLE gCpmGpioDevicePowerTableNanHaiVtb1HyEx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table contect
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYEX_CPM_SIGNATURE_GPIO_DEVICE_POWER, sizeof (gCpmGpioDevicePowerTableNanHaiVtb1HyEx) / sizeof (UINT8), 0, 0, 0,
    0x0000000F },
  {
    // {UINT8  DeviceId;       // Device Id
    // UINT8  Mode;           // Device Power Mode. 1: Power On. 0: Power Off
    // UINT8  Type;           // Device Power Item. 0: Set GPIO. 1: Wait GPIO. 2: Add Delay
    // UINT32 ((UINT16)Pin + ((UINT8)Value << 16));                 // GPIO pin value or delay timer
    // UINT8  InitFlag;       // Init flag in post},
    // {DEVICE_ID_GBE,    CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (GBE_POWER_CONTROL_REVA, 1),  0},
    // {DEVICE_ID_BT,     CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (BT_POWER_CONTROL_REVA, 0),   0},
    // {DEVICE_ID_WLAN,   CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (WLAN_POWER_CONTROL_REVA, 0), 0},
    // {DEVICE_ID_PCIE_X16_SWITCH,   CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (PCIE_X16_SWITCH, 0), 0},
    // {DEVICE_ID_SATAE_M2_SWITCH,   CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (SATAE_M2_SWITCH, 0), 0},

    // {DEVICE_ID_GBE,    CPM_POWER_ON,  CPM_POWER_SET,   CPM_GPIO_PIN (GBE_POWER_CONTROL_REVA, 0),  0},
    // {DEVICE_ID_BT,     CPM_POWER_ON,  CPM_POWER_SET,   CPM_GPIO_PIN (BT_POWER_CONTROL_REVA, 1),   0},
    // {DEVICE_ID_WLAN,   CPM_POWER_ON,  CPM_POWER_SET,   CPM_GPIO_PIN (WLAN_POWER_CONTROL_REVA, 1), 0},
    // {DEVICE_ID_PCIE_X16_SWITCH,   CPM_POWER_ON, CPM_POWER_SET,   CPM_GPIO_PIN (PCIE_X16_SWITCH, 1), 0},
    // {DEVICE_ID_SATAE_M2_SWITCH,   CPM_POWER_ON, CPM_POWER_SET,   CPM_GPIO_PIN (SATAE_M2_SWITCH, 1), 0},
    0xFF,
  }
};

//
// PCIE Clock Table
//
HYGON_CPM_PCIE_CLOCK_TABLE    gCpmPcieClockTableNanHaiVtb1HyEx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table content
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYEX_CPM_SIGNATURE_PCIE_CLOCK, sizeof (gCpmPcieClockTableNanHaiVtb1HyEx) / sizeof (UINT8), 0, 0, 0, 0x0F },
  {
    // {UINT8  ClkId;              // FCH PCIe Clock
    // UINT8  ClkReq;             // FCH PCIe ClkReq
    // UINT8  ClkIdExt;           // External Clock Source
    // UINT8  ClkReqExt;          // External ClkReq
    // UINT8  DeviceId;           // Device Id. No Device Id if 0xFF
    // UINT8  Device;             // Device Number of PCIe bridge
    // UINT8  Function;           // Function Number of PCIe bridge
    // UINT8  SlotCheck;          // Slot Check Flag: // BIT0: Check PCI Space // BIT1: Check GPIO pin // BIT2: Check Clock Power Management Enable // BIT3~6: Reserved // BIT7: Change PCIe Clock in ACPI method
    // UINT32 SpecialFunctionId;  // Id of Special Function}
    // GPP_CLK0 clock for PCIE X8 slot is controlled by CLK_REQ0_L. For Bristol, this slot is not used
    // {GPP_CLK0,            CLK_ENABLE,  SRC_CLK4,  CLK_ENABLE,  DEVICE_ID_PCIE_X8_SLOT,  0, 0, NON_SLOT_CHECK,            0},
    0xFF,
  }
};

//
// CPM HSIO Topology Table Socket 0
//
HYGON_CPM_HSIO_TOPOLOGY_TABLE  gCpmHsioTopologyTableNanHaiVtb1S0HyEx = {//ByoAddHsioLane() will overwrite this table.
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table contect
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY, sizeof (gCpmHsioTopologyTableNanHaiVtb1S0HyEx) / sizeof (UINT8), 0, 0, 0,
    0x0000000F },
  0,  // SocketId
  {   // HSIO_PORT_DESCRIPTOR
    { // G0 - x16 slot
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioSATAEngine, 112, 115, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_SATA (
        HsioPortEnabled                       // Port Present
        )
    },
    { // P7 - 116-119 PCIE disable
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 116, 119, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
      HsioPortDisabled,                     // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      HsioHotplugDisabled,                  // Hotplug
      HsioGenMaxSupported,                  // Max Link Speed
      HsioGenMaxSupported,                  // Max Link Capability
      HsioAspmL1,                           // ASPM
      HsioAspmL1,                           // ASPM L1.1 disabled
      HsioAspmL1,                           // ASPM L1.2 disabled
      HsioClkPmSupportDisabled,             // Clock PM
      5
      )
    },
    { // P7 - x8 slot
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 120, 127, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
      HsioPortEnabled,                      // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      HsioHotplugDisabled,                  // Hotplug
      HsioGenMaxSupported,                  // Max Link Speed
      HsioGenMaxSupported,                  // Max Link Capability
      HsioAspmL1,                           // ASPM
      HsioAspmL1,                           // ASPM L1.1 disabled
      HsioAspmL1,                           // ASPM L1.2 disabled
      HsioClkPmSupportDisabled,             // Clock PM
      4
      )
    },
    { // P8 - x1 PCIE for BMC
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 128, 128, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
      HsioPortEnabled,                      // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      HsioHotplugDisabled,                  // Hotplug
      HsioGenMaxSupported,                  // Max Link Speed
      HsioGenMaxSupported,                  // Max Link Capability
      HsioAspmL1,                           // ASPM
      HsioAspmL1,                           // ASPM L1.1 disabled
      HsioAspmL1,                           // ASPM L1.2 disabled
      HsioClkPmSupportDisabled,             // Clock PM
      0
      )
    },
    { //P8 - 129 PCIE disable
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 129, 129, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
      HsioPortEnabled,                      // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      HsioHotplugDisabled,                  // Hotplug
      HsioGenMaxSupported,                  // Max Link Speed
      HsioGenMaxSupported,                  // Max Link Capability
      HsioAspmL1,                           // ASPM
      HsioAspmL1,                           // ASPM L1.1 disabled
      HsioAspmL1,                            // ASPM L1.2 disabled
      HsioClkPmSupportDisabled,             // Clock PM
      0
      )
    },
    { // G4 - x16 slot
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 64, 79, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
      HsioPortEnabled,                      // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      HsioHotplugDisabled,                  // Hotplug
      HsioGenMaxSupported,                  // Max Link Speed
      HsioGenMaxSupported,                  // Max Link Capability
      HsioAspmL1,                           // ASPM
      HsioAspmL1,                           // ASPM L1.1 disabled
      HsioAspmL1,                           // ASPM L1.2 disabled
      HsioClkPmSupportDisabled,             // Clock PM
      1
      )
    },
    { // G5 - x16 slot
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 80, 95, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
      HsioPortEnabled,                      // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      HsioHotplugDisabled,                  // Hotplug
      HsioGenMaxSupported,                  // Max Link Speed
      HsioGenMaxSupported,                  // Max Link Capability
      HsioAspmL1,                           // ASPM
      HsioAspmL1,                           // ASPM L1.1 disabled
      HsioAspmL1,                           // ASPM L1.2 disabled
      HsioClkPmSupportDisabled,             // Clock PM
      2
      )
    },
    { // P6 - x16 slot
      DESCRIPTOR_TERMINATE_LIST,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 96, 111, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
      HsioPortEnabled,                      // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      HsioHotplugDisabled,                  // Hotplug
      HsioGenMaxSupported,                  // Max Link Speed
      HsioGenMaxSupported,                  // Max Link Capability
      HsioAspmL1,                           // ASPM
      HsioAspmL1,                           // ASPM L1.1 disabled
      HsioAspmL1,                           // ASPM L1.2 disabled
      HsioClkPmSupportDisabled,             // Clock PM
      3
      )
    }
  } // End of HSIO_PORT_DESCRIPTOR
};

// CPM HSIO Topology Table Socket 1
//
HYGON_CPM_HSIO_TOPOLOGY_TABLE  gCpmHsioTopologyTableNanHaiVtb1S1HyEx = {  //ByoAddHsioLane() will overwrite this table.
  { HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY_S1, sizeof (gCpmHsioTopologyTableNanHaiVtb1S1HyEx) / sizeof (UINT8), 0, 0, 0,
    0x0000000F },
  1,  // SocketId
  {   // HSIO_PORT_DESCRIPTOR
    { // G4 - x8 slot
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 64, 67, HsioHotplugServerExpress, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
        HsioPortEnabled,                      // Port Present
        1,                                    // Requested Device
        1,                                    // Requested Function
        HsioHotplugServerExpress,             // Hotplug
        HsioGenMaxSupported,                  // Max Link Speed
        HsioGenMaxSupported,                  // Max Link Capability
        HsioAspmL1,                           // ASPM
        HsioAspmL1,                           // ASPM L1.1 disabled
        HsioAspmL1,                           // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,              // Clock PM
        8                                      // Slot Number
        )
    },
    { // G4 - x8 slot
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 68, 71, HsioHotplugServerExpress, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
        HsioPortEnabled,                      // Port Present
        1,                                    // Requested Device
        2,                                    // Requested Function
        HsioHotplugServerExpress,             // Hotplug
        HsioGenMaxSupported,                  // Max Link Speed
        HsioGenMaxSupported,                  // Max Link Capability
        HsioAspmL1,                           // ASPM
        HsioAspmL1,                           // ASPM L1.1 disabled
        HsioAspmL1,                           // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        9                                     // Slot Number
        )
    },
    { // G4 - x8 slot
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 72, 75, HsioHotplugServerExpress, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
        HsioPortEnabled,                      // Port Present
        1,                                    // Requested Device
        3,                                    // Requested Function
        HsioHotplugServerExpress,             // Hotplug
        HsioGenMaxSupported,                  // Max Link Speed
        HsioGenMaxSupported,                  // Max Link Capability
        HsioAspmL1,                           // ASPM
        HsioAspmL1,                           // ASPM L1.1 disabled
        HsioAspmL1,                           // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        10                                     // Slot Number
        )
    },
    { // G4 - x8 slot
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 76, 79, HsioHotplugServerExpress, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
        HsioPortEnabled,                      // Port Present
        1,                                    // Requested Device
        4,                                    // Requested Function
        HsioHotplugServerExpress,             // Hotplug
        HsioGenMaxSupported,                  // Max Link Speed
        HsioGenMaxSupported,                  // Max Link Capability
        HsioAspmL1,                           // ASPM
        HsioAspmL1,                           // ASPM L1.1 disabled
        HsioAspmL1,                           // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        11                                    // Slot Number
        )
    },
    { // G5 - x16 slot
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 80, 95, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
        HsioPortEnabled,                      // Port Present
        0,                                    // Requested Device
        0,                                    // Requested Function
        HsioHotplugDisabled,                  // Hotplug
        HsioGenMaxSupported,                  // Max Link Speed
        HsioGenMaxSupported,                  // Max Link Capability
        HsioAspmL1,                           // ASPM
        HsioAspmL1,                           // ASPM L1.1 disabled
        HsioAspmL1,                           // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        6
        )
    },
    { // P6 - x16 slot
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 96, 111, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
        HsioPortEnabled,                      // Port Present
        0,                                    // Requested Device
        0,                                    // Requested Function
        HsioHotplugDisabled,                  // Hotplug
        HsioGenMaxSupported,                  // Max Link Speed
        HsioGenMaxSupported,                  // Max Link Capability
        HsioAspmL1,                           // ASPM
        HsioAspmL1,                           // ASPM L1.1 disabled
        HsioAspmL1,                           // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        7
        )
    },
    { // P7 - x16 slot
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 112, 127, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
        HsioPortEnabled,                      // Port Present
        0,                                    // Requested Device
        0,                                    // Requested Function
        HsioHotplugDisabled,                  // Hotplug
        HsioGenMaxSupported,                  // Max Link Speed
        HsioGenMaxSupported,                  // Max Link Capability
        HsioAspmL1,                           // ASPM
        HsioAspmL1,                           // ASPM L1.1 disabled
        HsioAspmL1,                           // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        8
        )
    },
    { // P8 -X1 Unused
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 128, 128, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
        HsioPortEnabled,                      // Port Present
        0,                                    // Requested Device
        0,                                    // Requested Function
        HsioHotplugDisabled,                  // Hotplug
        HsioGenMaxSupported,                  // Max Link Speed
        HsioGenMaxSupported,                  // Max Link Capability
        HsioAspmL1,                           // ASPM
        HsioAspmL1,                           // ASPM L1.1 disabled
        HsioAspmL1,                           // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        0
      )
    }, 
    { // P8 -X1 I350
      DESCRIPTOR_TERMINATE_LIST,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 129, 129, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
        HsioPortEnabled,                      // Port Present
        0,                                    // Requested Device
        0,                                    // Requested Function
        HsioHotplugDisabled,                  // Hotplug
        HsioGenMaxSupported,                  // Max Link Speed
        HsioGenMaxSupported,                  // Max Link Capability
        HsioAspmL1,                           // ASPM
        HsioAspmL1,                           // ASPM L1.1 disabled
        HsioAspmL1,                           // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        0
      )
    }
  } // End of HSIO_PORT_DESCRIPTOR
};

HYGON_CPM_EQ_CONFIG_TABLE   gCpmEqConfigurationTableNanHaiVtb1HyEx = {
  {HYEX_CPM_SIGNATURE_EQ_CONFIG, sizeof (gCpmEqConfigurationTableNanHaiVtb1HyEx) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {

    //USB sample
    //     Flag, Type, Socket, PhysicalIodId, Nbio, UsbPort, EQDate...
    //USB_EQ(FLAG_FORCE, USB3_GEN1_TX_EQ_LEVEL, 0, 0, 0, 0, 0x02, 0x34, 0x07),
    //USB_EQ(FLAG_FORCE, USB3_GEN2_TX_EQ_LEVEL, 0, 0, 1, 2, 0x02, 0x34, 0x07),
    //USB_EQ(FLAG_FORCE, USB2_TX_SWING, 1, 0, 1, 2, 3),
    //USB_EQ(FLAG_FORCE, USB2_TX_PREEMPHASIS, 1, 0, 0, 1, 4),
    //USB_EQ(FLAG_FORCE, USB3_TX_VBOOST, 0, 0, 1, 3, 5),

    //SATA sample
    //      Flag, Type, Socket, PhysicalIodId, Nbio, SataPort, EQDate...
    //SATA_EQ(FLAG_FORCE, SATA_EQ_GEN1_CURSOR, 0, 0, 3, 0, 0x0, 0x16, 0x0A),
    //SATA_EQ(FLAG_FORCE, SATA_EQ_GEN2_CURSOR, 1, 0, 2, 0, 0x0, 0x16, 0x0A),
    //SATA_EQ(FLAG_FORCE, SATA_EQ_GEN3_CURSOR, 0, 0, 1, 0, 0x0, 0x16, 0x0A),

    {EQ_TABLE_END, EQ_TYPE_END, 0xFF, 0xFF, 0xFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, TRUE, {0} }
  }
};

/*----------------------------------------------------------------------------------------*/

HYGON_CPM_DIMM_MAP_TABLE  gCpmDimmMapTableHyEx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table contect
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},

  //VTB3 DIMM map
  // Channel A = 0, Channel B = 1, Channel C = 2, Channel D = 3
  // Channel E = 4, Channel F = 5, Channel G = 6, Channel H = 7
  // Channel I = 8, Channel J = 9, Channel K = 0xA, Channel L = 0xB
  
  { HYEX_CPM_SIGNATURE_DIMM_MAP, sizeof (gCpmDimmMapTableHyEx) / sizeof (UINT8), 0, 0, 0, 0x01 },
  {"PxCxUxDx_X",  1, 3, 5, 7, 9 },
  {"PX CHANNEL X",1, 0, 0, 0, 11},
  {
    DIMM_MAP (CpmSocket0, CpmCdd0, CpmUmcChannel0, CpmUmcDimm0, 0x0 , 'K'),
    DIMM_MAP (CpmSocket0, CpmCdd0, CpmUmcChannel0, CpmUmcDimm1, 0x0 , 'K'),	
    DIMM_MAP (CpmSocket0, CpmCdd0, CpmUmcChannel1, CpmUmcDimm0, 0x1 , 'L'),
    DIMM_MAP (CpmSocket0, CpmCdd0, CpmUmcChannel1, CpmUmcDimm1, 0x1 , 'L'),
    DIMM_MAP (CpmSocket0, CpmCdd0, CpmUmcChannel2, CpmUmcDimm0, 0x2 , 'J'),
    DIMM_MAP (CpmSocket0, CpmCdd0, CpmUmcChannel2, CpmUmcDimm1, 0x2 , 'J'),	
    
    DIMM_MAP (CpmSocket0, CpmCdd1, CpmUmcChannel0, CpmUmcDimm0, 0x3 , 'E'),
    DIMM_MAP (CpmSocket0, CpmCdd1, CpmUmcChannel0, CpmUmcDimm1, 0x3 , 'E'),    
    DIMM_MAP (CpmSocket0, CpmCdd1, CpmUmcChannel1, CpmUmcDimm0, 0x4 , 'F'),
    DIMM_MAP (CpmSocket0, CpmCdd1, CpmUmcChannel1, CpmUmcDimm1, 0x4 , 'F'),	
    DIMM_MAP (CpmSocket0, CpmCdd1, CpmUmcChannel2, CpmUmcDimm0, 0x5 , 'D'),
    DIMM_MAP (CpmSocket0, CpmCdd1, CpmUmcChannel2, CpmUmcDimm1, 0x5 , 'D'),
    
    DIMM_MAP (CpmSocket0, CpmCdd2, CpmUmcChannel0, CpmUmcDimm0, 0x6 , 'A'),
    DIMM_MAP (CpmSocket0, CpmCdd2, CpmUmcChannel0, CpmUmcDimm1, 0x6 , 'A'),	
    DIMM_MAP (CpmSocket0, CpmCdd2, CpmUmcChannel1, CpmUmcDimm0, 0x7 , 'B'),
    DIMM_MAP (CpmSocket0, CpmCdd2, CpmUmcChannel1, CpmUmcDimm1, 0x7 , 'B'),
    DIMM_MAP (CpmSocket0, CpmCdd2, CpmUmcChannel2, CpmUmcDimm0, 0x8 , 'C'),
    DIMM_MAP (CpmSocket0, CpmCdd2, CpmUmcChannel2, CpmUmcDimm1, 0x8 , 'C'),
    
    DIMM_MAP (CpmSocket0, CpmCdd3, CpmUmcChannel0, CpmUmcDimm0, 0x9 , 'G'),
    DIMM_MAP (CpmSocket0, CpmCdd3, CpmUmcChannel0, CpmUmcDimm1, 0x9 , 'G'),    
    DIMM_MAP (CpmSocket0, CpmCdd3, CpmUmcChannel1, CpmUmcDimm0, 0xa , 'H'),
    DIMM_MAP (CpmSocket0, CpmCdd3, CpmUmcChannel1, CpmUmcDimm1, 0xa , 'H'),	
    DIMM_MAP (CpmSocket0, CpmCdd3, CpmUmcChannel2, CpmUmcDimm0, 0xb , 'I'),
    DIMM_MAP (CpmSocket0, CpmCdd3, CpmUmcChannel2, CpmUmcDimm1, 0xb , 'I'),

    DIMM_MAP (CpmSocket1, CpmCdd0, CpmUmcChannel0, CpmUmcDimm0, 0x0 , 'K'),
    DIMM_MAP (CpmSocket1, CpmCdd0, CpmUmcChannel0, CpmUmcDimm1, 0x0 , 'K'),	
    DIMM_MAP (CpmSocket1, CpmCdd0, CpmUmcChannel1, CpmUmcDimm0, 0x1 , 'L'),
    DIMM_MAP (CpmSocket1, CpmCdd0, CpmUmcChannel1, CpmUmcDimm1, 0x1 , 'L'),
    DIMM_MAP (CpmSocket1, CpmCdd0, CpmUmcChannel2, CpmUmcDimm0, 0x2 , 'J'),
    DIMM_MAP (CpmSocket1, CpmCdd0, CpmUmcChannel2, CpmUmcDimm1, 0x2 , 'J'),	
    
    DIMM_MAP (CpmSocket1, CpmCdd1, CpmUmcChannel0, CpmUmcDimm0, 0x3 , 'E'),
    DIMM_MAP (CpmSocket1, CpmCdd1, CpmUmcChannel0, CpmUmcDimm1, 0x3 , 'E'),    
    DIMM_MAP (CpmSocket1, CpmCdd1, CpmUmcChannel1, CpmUmcDimm0, 0x4 , 'F'),
    DIMM_MAP (CpmSocket1, CpmCdd1, CpmUmcChannel1, CpmUmcDimm1, 0x4 , 'F'),	
    DIMM_MAP (CpmSocket1, CpmCdd1, CpmUmcChannel2, CpmUmcDimm0, 0x5 , 'D'),
    DIMM_MAP (CpmSocket1, CpmCdd1, CpmUmcChannel2, CpmUmcDimm1, 0x5 , 'D'),
    
    DIMM_MAP (CpmSocket1, CpmCdd2, CpmUmcChannel0, CpmUmcDimm0, 0x6 , 'A'),
    DIMM_MAP (CpmSocket1, CpmCdd2, CpmUmcChannel0, CpmUmcDimm1, 0x6 , 'A'),	
    DIMM_MAP (CpmSocket1, CpmCdd2, CpmUmcChannel1, CpmUmcDimm0, 0x7 , 'B'),
    DIMM_MAP (CpmSocket1, CpmCdd2, CpmUmcChannel1, CpmUmcDimm1, 0x7 , 'B'),
    DIMM_MAP (CpmSocket1, CpmCdd2, CpmUmcChannel2, CpmUmcDimm0, 0x8 , 'C'),
    DIMM_MAP (CpmSocket1, CpmCdd2, CpmUmcChannel2, CpmUmcDimm1, 0x8 , 'C'),
    
    DIMM_MAP (CpmSocket1, CpmCdd3, CpmUmcChannel0, CpmUmcDimm0, 0x9 , 'G'),
    DIMM_MAP (CpmSocket1, CpmCdd3, CpmUmcChannel0, CpmUmcDimm1, 0x9 , 'G'),    
    DIMM_MAP (CpmSocket1, CpmCdd3, CpmUmcChannel1, CpmUmcDimm0, 0xa , 'H'),
    DIMM_MAP (CpmSocket1, CpmCdd3, CpmUmcChannel1, CpmUmcDimm1, 0xa , 'H'),	
    DIMM_MAP (CpmSocket1, CpmCdd3, CpmUmcChannel2, CpmUmcDimm0, 0xb , 'I'),
    DIMM_MAP (CpmSocket1, CpmCdd3, CpmUmcChannel2, CpmUmcDimm1, 0xb , 'I'),
  }
};
