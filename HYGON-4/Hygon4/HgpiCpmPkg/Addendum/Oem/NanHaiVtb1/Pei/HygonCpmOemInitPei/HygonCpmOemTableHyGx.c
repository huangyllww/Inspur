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
HYGON_CPM_PLATFORM_ID_TABLE2          gCpmPlatformIdTableHyGx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table contect
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYGX_CPM_SIGNATURE_GET_PLATFORM_ID2, sizeof (gCpmPlatformIdTableHyGx) / sizeof (UINT8), 0, 0, 0, 1 },
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

HYGON_CPM_PLATFORM_ID_CONVERT_TABLE   gCpmPlatformIdConvertTableHyGx = {
  { HYGX_CPM_SIGNATURE_GET_PLATFORM_ID_CONVERT, sizeof (gCpmPlatformIdConvertTableHyGx) / sizeof (UINT8), 0, 0, 0, 1 },
  {
    0xFFFF,
  }
};

//
// Pre-Init Table
//
HYGON_CPM_PRE_INIT_TABLE              gCpmPreInitTableHyGx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table contect
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYGX_CPM_SIGNATURE_PRE_INIT, sizeof (gCpmPreInitTableHyGx) / sizeof (UINT8), 0, 0, 0, 0x01 },
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
HYGON_CPM_GPIO_INIT_TABLE             gCpmGpioInitTableNanHaiVtb1HyGx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table content
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYGX_CPM_SIGNATURE_GPIO_INIT, sizeof (gCpmGpioInitTableNanHaiVtb1HyGx) / sizeof (UINT8), 0, 0, 0, 0x0000000F },
  { // Socket PhysicalDie gpio function          output            pullup
    //GPIO_DEF_V3 (0,  0, 0,    GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // CPLD_P0_PWRBTN_N
    //GPIO_DEF_V3 (0,  0, 1,    GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // None
    //GPIO_DEF_V3 (0,  0, 2,    GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // PCIe Wake, L-wake cpu
      GPIO_DEF_V3 (0,  0, 3  ,  GPIO_FUNCTION_0,  GPIO_OUTPUT_HIGH ,  GPIO_PU_EN),       // P0_SPD_SW_CTRL        
      GPIO_DEF_V3 (0,  0, 4  ,  GPIO_FUNCTION_0,  GPIO_OUTPUT_HIGH ,  GPIO_PU_EN),       // FM_SMI_ACTIVE_N 
    //GPIO_DEF_V3 (0,  0, 9,    GPIO_FUNCTION_0,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // NA
      GPIO_DEF_V3 (0,  0, 19,     GPIO_FUNCTION_1,  GPIO_NA ,         GPIO_PU_PD_DIS),   // For clk gen(CK440)
      GPIO_DEF_V3 (0,  0, 20,     GPIO_FUNCTION_1,  GPIO_NA ,         GPIO_PU_PD_DIS),   // For clk gen(CK440)
    //GPIO_DEF_V3 (0,  0, 21,   GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // LPC BUS PD_N
    //GPIO_DEF_V3 (0,  0, 22,   GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // LPC BUS PME_N
    //GPIO_DEF_V3 (0,  0, 23,   GPIO_FUNCTION_0,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // NA
    //GPIO_DEF_V3 (0,  0, 26,   GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // P0_PCIE_RST_N
    //GPIO_DEF_V3 (0,  0, 27,   GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // P0_PCIE_RST3_N    
      GPIO_DEF_V3 (0,  0, 29,   GPIO_FUNCTION_0,  GPIO_OUTPUT_LOW ,   GPIO_PU_PD_DIS),   // AGPI029
    //GPIO_DEF_V3 (0,  0, 30,   GPIO_FUNCTION_0,  GPIO_OUTPUT_HIGH ,  GPIO_PU_EN),       // AGPIO30
      GPIO_DEF_V3 (0,  0, 31,   GPIO_FUNCTION_0,  GPIO_OUTPUT_HIGH ,  GPIO_PU_EN),       // AGPIO31 LL
      GPIO_DEF_V3 (0,  0, 32,   GPIO_FUNCTION_0,  GPIO_OUTPUT_LOW ,   GPIO_PU_EN),       // AGPIO32
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
      GPIO_DEF_V3 (0,  0, 135,  GPIO_FUNCTION_1,  GPIO_NA ,           GPIO_PU_EN),       // UART0 CTS
      GPIO_DEF_V3 (0,  0, 136,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_EN),       // UART0 RXD
      GPIO_DEF_V3 (0,  0, 137,  GPIO_FUNCTION_1,  GPIO_NA ,           GPIO_PU_EN),       // UART0 RTS
      GPIO_DEF_V3 (0,  0, 138,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_EN),       // UART0 TXD
      GPIO_DEF_V3 (0,  0, 139,  GPIO_FUNCTION_1,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // Clear Password header
   // GPIO_DEF_V3 (0,  0, 141,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // UART1 RXD
   // GPIO_DEF_V3 (0,  0, 143,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // UART1 TXD
    //GPIO_DEF_V3 (0,  0, 145,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // None
    //GPIO_DEF_V3 (0,  0, 146,  GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // For hotplug
      GPIO_DEF_V3 (0,  0, 147,  GPIO_FUNCTION_1,  GPIO_NA ,           GPIO_PU_PD_DIS),   // I2C1 SCL
      GPIO_DEF_V3 (0,  0, 148,  GPIO_FUNCTION_1,  GPIO_NA ,           GPIO_PU_PD_DIS),   // I2C1 SDA
    //GPIO_DEF_V3 (0,  0, 256,  GPIO_FUNCTION_0,  GPIO_OUTPUT_HIGH,   GPIO_PU_EN    ),   // Reserved GPIO for CPLD
    //GPIO_DEF_V3 (0,  0, 257,  GPIO_FUNCTION_0,  GPIO_OUTPUT_HIGH,   GPIO_PU_EN    ),   // Reserved GPIO for CPLD
    //GPIO_DEF_V3 (0,  0, 258,  GPIO_FUNCTION_0,  GPIO_OUTPUT_HIGH,   GPIO_PU_EN    ),   // Reserved GPIO for CPLD
    //GPIO_DEF_V3 (0,  0, 259,  GPIO_FUNCTION_0,  GPIO_OUTPUT_HIGH,   GPIO_PU_EN    ),   // Reserved GPIO for CPLD
    //GPIO_DEF_V3 (0,  0, 261,  GPIO_FUNCTION_0,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // Manufacture Mode, H-Manufacture Mode , L-Normal Operation
      GPIO_DEF_V3 (0,  0, 262,  GPIO_FUNCTION_0,  GPIO_INPUT ,          GPIO_PU_EN),     // AGPIO2
    //GPIO_DEF_V3 (0,  0, 263,  GPIO_FUNCTION_0,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // BIOS Recovery,  H-Normal,  L-Recovery
    //GPIO_DEF_V3 (1,  0, 0,    GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // CPLD_P0_PWRBTN_N
    //GPIO_DEF_V3 (1,  0, 1,    GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // None
    //GPIO_DEF_V3 (1,  0, 2,    GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // PCIe Wake, L-wake cpu
    //GPIO_DEF_V3 (1,  0, 3,    GPIO_FUNCTION_0,  GPIO_OUTPUT_HIGH,   GPIO_PU_EN    ),   // SPD Switch Control,  H-cpu as master,  L-bmc as master
    //GPIO_DEF_V3 (1,  0, 9,    GPIO_FUNCTION_0,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // NA
      GPIO_DEF_V3 (1,  0, 10,   GPIO_FUNCTION_3,  GPIO_NA ,           GPIO_PU_PD_DIS),   // MDIO0_SCL
    //GPIO_DEF_V3 (1,  0, 21,   GPIO_FUNCTION_0,  GPIO_NA ,           GPIO_PU_PD_DIS),   // LPC BUS PD_N
    //GPIO_DEF_V3 (1,  0, 23,   GPIO_FUNCTION_0,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // NA
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
    //GPIO_DEF_V3 (1,  0, 260,  GPIO_FUNCTION_0,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // Reserved
    //GPIO_DEF_V3 (1,  0, 261,  GPIO_FUNCTION_0,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // Reserved
    //GPIO_DEF_V3 (1,  0, 262,  GPIO_FUNCTION_0,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // Reserved
    //GPIO_DEF_V3 (1,  0, 263,  GPIO_FUNCTION_0,  GPIO_INPUT,         GPIO_PU_PD_DIS),   // Reserved
    0xFF,
  }
};

//
// GEVENT Init Table
//
HYGON_CPM_GEVENT_INIT_TABLE           gCpmGeventInitTableNanHaiVtb1HyGx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table contect
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYGX_CPM_SIGNATURE_GEVENT_INIT, sizeof (gCpmGeventInitTableNanHaiVtb1HyGx) / sizeof (UINT8), 0, 0, 0, 0x00000001 },
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

HYGON_CPM_GPIO_DEVICE_CONFIG_TABLE    gCpmGpioDeviceConfigTableNanHaiVtb1HyGx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table contect
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYGX_CPM_SIGNATURE_GPIO_DEVICE_CONFIG, sizeof (gCpmGpioDeviceConfigTableNanHaiVtb1HyGx) / sizeof (UINT8), 0, 0, 0,
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
HYGON_CPM_GPIO_DEVICE_DETECTION_TABLE gCpmGpioDeviceDetectionTableNanHaiVtb1HyGx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table contect
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYGX_CPM_SIGNATURE_GPIO_DEVICE_DETECTION, sizeof (gCpmGpioDeviceDetectionTableNanHaiVtb1HyGx) / sizeof (UINT8), 0, 0, 0,
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
HYGON_CPM_GPIO_DEVICE_RESET_TABLE gCpmGpioDeviceResetTableNanHaiVtb1HyGx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table contect
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYGX_CPM_SIGNATURE_GPIO_DEVICE_RESET, sizeof (gCpmGpioDeviceResetTableNanHaiVtb1HyGx) / sizeof (UINT8), 0, 0, 0,
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
HYGON_CPM_GPIO_DEVICE_POWER_TABLE gCpmGpioDevicePowerTableNanHaiVtb1HyGx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table contect
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYGX_CPM_SIGNATURE_GPIO_DEVICE_POWER, sizeof (gCpmGpioDevicePowerTableNanHaiVtb1HyGx) / sizeof (UINT8), 0, 0, 0,
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
HYGON_CPM_PCIE_CLOCK_TABLE    gCpmPcieClockTableNanHaiVtb1HyGx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table content
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYGX_CPM_SIGNATURE_PCIE_CLOCK, sizeof (gCpmPcieClockTableNanHaiVtb1HyGx) / sizeof (UINT8), 0, 0, 0, 0x0F },
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
HYGON_CPM_HSIO_TOPOLOGY_TABLE  gCpmHsioTopologyTableNanHaiVtb1S0HyGx = {
  // {  UINT32 TableSignature;   // Signature of CPM table
  // UINT16 TableSize;        // Table size
  // UINT8  FormatRevision;   // Revision of table format
  // UINT8  ContentRevision;  // Revision of table contect
  // UINT32 PlatformMask;     // The mask of platform table supports
  // UINT32 Attribute;        // Table attribute},
  { HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY, sizeof (gCpmHsioTopologyTableNanHaiVtb1S0HyGx) / sizeof (UINT8), 0, 0, 0,
    0x0000000F },
  0,  // SocketId
  {   // HSIO_PORT_DESCRIPTOR
    { // P8 - 0-7 SATA port
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioSATAEngine, 0, 7, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_SATA (
        HsioPortEnabled                       // Port Present
      )
    },
    { // P8 - x1 slot
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 8, 8, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
        HsioPortEnabled,                     // Port Present
        0,                                    // Requested Device
        0,                                    // Requested Function
        HsioHotplugDisabled,                  // Hotplug
        HsioGenMaxSupported,                  // Max Link Speed
        HsioGenMaxSupported,                  // Max Link Capability
        HsioAspmDisabled,                     // ASPM
        HsioAspmDisabled,                     // ASPM L1.1 disabled
        HsioAspmDisabled,                     // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        0
      )
    },
    { // P7 - x16 slot
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioCxlEngine, 32, 39, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_CXL (
        HsioPortEnabled,                     // Port Present
        CXL_2_0,                              // CXL Mode
        0,                                    // Requested Device
        0,                                    // Requested Function
        HsioHotplugDisabled,                  // Hotplug
        HsioGenMaxSupported,                  // Max Link Speed
        HsioGenMaxSupported,                  // Max Link Capability
        HsioAspmDisabled,                     // ASPM
        HsioAspmDisabled,                     // ASPM L1.1 disabled
        HsioAspmDisabled,                     // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        0
      )
    },
    { // P7 - x16 slot
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 40, 47, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
        HsioPortEnabled,                     // Port Present
        0,                                    // Requested Device
        0,                                    // Requested Function
        HsioHotplugDisabled,                  // Hotplug
        HsioGenMaxSupported,                  // Max Link Speed
        HsioGenMaxSupported,                  // Max Link Capability
        HsioAspmDisabled,                     // ASPM
        HsioAspmDisabled,                     // ASPM L1.1 disabled
        HsioAspmDisabled,                     // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        0
      )
    },
    { // P6 - x8 slot
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 64, 79, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
        HsioPortEnabled,                      // Port Present
        0,                                    // Requested Device
        0,                                    // Requested Function
        HsioHotplugDisabled,                  // Hotplug
        HsioGenMaxSupported,                  // Max Link Speed
        HsioGenMaxSupported,                  // Max Link Capability
        HsioAspmDisabled,                     // ASPM
        HsioAspmDisabled,                     // ASPM L1.1 disabled
        HsioAspmDisabled,                     // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        0
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
        HsioAspmDisabled,                     // ASPM
        HsioAspmDisabled,                     // ASPM L1.1 disabled
        HsioAspmDisabled,                     // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        0
      )
    },
    { // G4 - x2 slot
      DESCRIPTOR_TERMINATE_LIST,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 48, 63, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
        HsioPortEnabled,                      // Port Present
        0,                                    // Requested Device
        0,                                    // Requested Function
        HsioHotplugDisabled,                  // Hotplug
        HsioGenMaxSupported,                  // Max Link Speed
        HsioGenMaxSupported,                  // Max Link Capability
        HsioAspmDisabled,                     // ASPM
        HsioAspmDisabled,                     // ASPM L1.1 disabled
        HsioAspmDisabled,                     // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        0
      )
    }
  } // End of HSIO_PORT_DESCRIPTOR
};

// CPM HSIO Topology Table Socket 1
//
HYGON_CPM_HSIO_TOPOLOGY_TABLE  gCpmHsioTopologyTableNanHaiVtb1S1HyGx = {
  { HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY_S1, sizeof (gCpmHsioTopologyTableNanHaiVtb1S1HyGx) / sizeof (UINT8), 0, 0, 0,
    0x0000000F },
  1,  // SocketId
  {   // HSIO_PORT_DESCRIPTOR
    { // P8 - 0-7 SATA port
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioSATAEngine, 0, 7, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_SATA (
        HsioPortEnabled                       // Port Present
      )
    },
    { // P8 - x1 slot disalbe
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 8, 8, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
        HsioPortDisabled,                     // Port Present
        0,                                    // Requested Device
        0,                                    // Requested Function
        HsioHotplugDisabled,                  // Hotplug
        HsioGenMaxSupported,                  // Max Link Speed
        HsioGenMaxSupported,                  // Max Link Capability
        HsioAspmDisabled,                     // ASPM
        HsioAspmDisabled,                     // ASPM L1.1 disabled
        HsioAspmDisabled,                     // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        0
      )
    },
    { // P7 - x16 slot
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 32, 47, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
        HsioPortEnabled,                     // Port Present
        0,                                    // Requested Device
        0,                                    // Requested Function
        HsioHotplugDisabled,                  // Hotplug
        HsioGenMaxSupported,                  // Max Link Speed
        HsioGenMaxSupported,                  // Max Link Capability
        HsioAspmDisabled,                     // ASPM
        HsioAspmDisabled,                     // ASPM L1.1 disabled
        HsioAspmDisabled,                     // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        0
      )
    },
    { // P6 - x8 slot
      0,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 64, 79, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
        HsioPortEnabled,                      // Port Present
        0,                                    // Requested Device
        0,                                    // Requested Function
        HsioHotplugDisabled,                  // Hotplug
        HsioGenMaxSupported,                  // Max Link Speed
        HsioGenMaxSupported,                  // Max Link Capability
        HsioAspmDisabled,                     // ASPM
        HsioAspmDisabled,                     // ASPM L1.1 disabled
        HsioAspmDisabled,                     // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        0
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
        HsioAspmDisabled,                     // ASPM
        HsioAspmDisabled,                     // ASPM L1.1 disabled
        HsioAspmDisabled,                     // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        0
      )
    },
    { // G4 - x2 slot
      DESCRIPTOR_TERMINATE_LIST,
      HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 48, 63, HsioHotplugDisabled, 1),
      HSIO_PORT_DATA_INITIALIZER_PCIE (
        HsioPortEnabled,                      // Port Present
        0,                                    // Requested Device
        0,                                    // Requested Function
        HsioHotplugDisabled,                  // Hotplug
        HsioGenMaxSupported,                  // Max Link Speed
        HsioGenMaxSupported,                  // Max Link Capability
        HsioAspmDisabled,                     // ASPM
        HsioAspmDisabled,                     // ASPM L1.1 disabled
        HsioAspmDisabled,                     // ASPM L1.2 disabled
        HsioClkPmSupportDisabled,             // Clock PM
        0
      )
    }
  } // End of HSIO_PORT_DESCRIPTOR
};

