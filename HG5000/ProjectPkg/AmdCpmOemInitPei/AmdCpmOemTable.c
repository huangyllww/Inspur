/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM OEM tables, and callback function.
 *
 * Contains code that defines OEM tables and callback function to override
 * OEM table on run time.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  OEM
 * @e \$Revision: 270847 $   @e \$Date: 2013-08-16 10:52:00 -0400 (Fri, 16 Aug 2013) $
 *
 */
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
#include <AmdCpmBaseIo.h>
#include "AmdCpmOemInitPeim.h"
#include <Token.h>

//
// OEM CPM Table Definition
//
//
// Platform Id Table: Get Board Id from SMBUS
//

// Read SMBUS to detect board ID
//BOARD_ID_BIT2  BOARD_ID_BIT1  BOARD_ID_BIT0    PCBA
//    0                      0                          0             Normal  REVA
//    0                      1                          0             SLT REVA
//    1                      0                          0             DAP REVA
AMD_CPM_PLATFORM_ID_TABLE2          gCpmPlatformIdTable = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GET_PLATFORM_ID2, sizeof (gCpmPlatformIdTable) / sizeof (UINT8), 0, 0, 0, 1},
  {
//   UINT8  SmbusSelect;      ///< SMBUS Number
//   UINT8  SmbusAddress;     ///< SMBUS Address
//   UINT8  SmbusOffset;      ///< SMBUS Offset
//   UINT8  SmbusBit;         ///< SMBUS Bit
    0xFF,
  }
};

//
// Convert Table from Board Id to Platform Id
//

AMD_CPM_PLATFORM_ID_CONVERT_TABLE   gCpmPlatformIdConvertTable = {
  {CPM_SIGNATURE_GET_PLATFORM_ID_CONVERT, sizeof (gCpmPlatformIdConvertTable) / sizeof (UINT8), 0, 0, 0, 1},
  {
    0xFFFF,
  }
};


//
// Pre-Init Table
//
AMD_CPM_PRE_INIT_TABLE              gCpmPreInitTable = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_PRE_INIT, sizeof (gCpmPreInitTable) / sizeof (UINT8), 0, 0, 0, 0x01},
  {
//  {UINT8  Type;     // Register type. 0: FCH MMIO. 1: PCI
//   UINT8  Select;   // Register sub-type
//   UINT8  Offset;   // Register offset
//   UINT8  AndMask;  // AND mask
//   UINT8  OrMask;   // OR mask
//   UINT8  Stage;    // Stage number},
    0xFF,
  }
};

//
// GPIO Init Table for Hygon52D16
//
AMD_CPM_GPIO_INIT_TABLE             gCpmGpioInitTableHygon52D16 = {

  {CPM_SIGNATURE_GPIO_INIT, sizeof (gCpmGpioInitTableHygon52D16) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {  
  // GPIO                   Function     Output  PullUp
  // PWR_BTN_L/AGPIO0       PWR_BTN_L    I       PU
	//GPIO_DEF_V2(0, 0, 0, GPIO_FUNCTION_0, GPIO_INPUT,GPIO_PU_PD_DIS),
  // SYS_RESET_L/AGPIO1     SYS_RESET_L  I       PU
	//GPIO_DEF_V2(0, 0, 1, GPIO_FUNCTION_0, GPIO_INPUT,GPIO_PU_PD_DIS),
  // WAKE_L/AGPIO2          WAKE_L       I       PU
  //GPIO_DEF_V2(0, 0, 2, GPIO_FUNCTION_0, GPIO_INPUT,GPIO_PU_PD_DIS),
  // WOL0/AGPIO3        Reserved     I/O     PU
 //PIO_DEF_V2 (0, 0, 3, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
  
  // WOL1/AGPIO4        Reserved     I/O     PD
  
  // AGPIO5/DEVSLP0/WOL2        Slimeline    I       PD
  GPIO_DEF_V2 (0, 0, 5, GPIO_FUNCTION_0, GPIO_INPUT, GPIO_PU_PD_DIS),
  
  // AGPIO6/DEVSLP1/WOL3        Slimeline    I       PD
  GPIO_DEF_V2 (0, 0, 6, GPIO_FUNCTION_0, GPIO_INPUT, GPIO_PU_PD_DIS),
  
  // AGPIO9_0/SGPIO0_DATAOUT/MDIO1_SCL     SGPIO_DATAOUT       O       PD
 //PIO_DEF_V2 (0, 0, 9, GPIO_FUNCTION_1, GPIO_NA, GPIO_PU_PD_DIS),  

  // EGPIO9_2/SGPIO2_DATAO UT/MDIO5_SCL     PCIE SLOT2 present Input        I       PD 
  GPIO_DEF_V2 (0, 1, 9, GPIO_FUNCTION_0, GPIO_INPUT, GPIO_PU_EN), 

  // S0A3_GPIO_0/AGPIO10_0/SGPIO0_CLK/MDIO0_SCL     SGPIO_CLK    O       N/A
//GPIO_DEF_V2 (0, 0, 10, GPIO_FUNCTION_2, GPIO_NA, GPIO_PU_PD_DIS), 
  
  // S0A3_GPIO_2/EGPIO10_2/SGPIO2_CLK/MDIO4_SCL     PCIE SLOT0 present Input        I       N/A
//GPIO_DEF_V2 (0, 1, 10, GPIO_FUNCTION_1, GPIO_NA, GPIO_PU_PD_DIS),
  
  // USB_0_OC0_L/AGPIO16_0      USB_OC0_L       I       PU
//GPIO_DEF_V2 (0, 0, 16, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_EN),
  
  // USB_0_OC1_L/AGPIO17_0      USB_OC1_L       I       PU
//GPIO_DEF_V2 (0, 0, 17, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_EN),
  
  // USB_0_OC2_L/AGPIO18_0      USB_0_OC2_L       I       PU
//GPIO_DEF_V2 (0, 0, 18, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_EN),
  
  // USB_0_OC3_L/AGPIO24_0      NC       NC       PU 
  // USB_1_OC0_L/EGPIO16_1      USB_1_OC0_L       I       PU
//GPIO_DEF_V2 (0, 1, 16, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_EN),
  
  // USB_1_OC1_L/EGPIO17_1      NC       NC       PU 
  // USB_1_OC2_L/EGPIO18_1      NC       NC       PU  
  // USB_1_OC3_L/EGPIO24_1      NC       NC       PU
  // SCL1/I2C3_SCL/AGPIO19/BMC_SCL      I2C3_SCL       OD       N/A
//GPIO_DEF_V2 (0, 0, 19, GPIO_FUNCTION_3, GPIO_NA, GPIO_PU_PD_DIS),
  
  // SDA1/I2C3_SDA/AGPIO20/BMC_SDA      I2C3_SDA       OD       N/A
//GPIO_DEF_V2 (0, 0, 20, GPIO_FUNCTION_3, GPIO_NA, GPIO_PU_PD_DIS),
  
  // LPC_PD_L/EMMC_CMD/AGPIO21      LPC_PD_L       NC       N/A
//GPIO_DEF_V2 (0, 0, 21, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
  GPIO_DEF_V2 (0, 0, 21 , GPIO_FUNCTION_2, GPIO_INPUT, GPIO_PU_EN),  // LPC_PD_L/AGPIO21 for NMI.

  
  // LPC_PME_L/EMMC_PWR_CTRL/AGPIO22      LPC_PME_L       I       PU
//GPIO_DEF_V2 (0, 0, 22, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
  
  // AGPIO23_0/SGPIO0_LOAD/MDIO1_SDA      SGPIO_LOAD       I       PU
//GPIO_DEF_V2 (0, 0, 23, GPIO_FUNCTION_1, GPIO_NA, GPIO_PU_PD_DIS),
  
  // EGPIO23_2/SGPIO2_LOAD/MDIO5_SDA      PCIE SLOT3 present In       I       PU
  GPIO_DEF_V2 (0, 1, 23, GPIO_FUNCTION_0, GPIO_INPUT, GPIO_PU_EN),
  
  // EGPIO3_2      Lan1       O       PU
	GPIO_DEF_V2 (0, 1, 3, GPIO_FUNCTION_0, GPIO_OUTPUT_HIGH, GPIO_PU_EN),
  // EGPIO4_2      Lan0       O       PU
	GPIO_DEF_V2 (0, 1, 4, GPIO_FUNCTION_0, GPIO_OUTPUT_HIGH, GPIO_PU_EN),
  // AGPIO40_0/SGPIO0_DATAIN/MDIO0_SDA      SGPIO_DATAIN       NC       N/A
//GPIO_DEF_V2 (0, 0, 40, GPIO_FUNCTION_1, GPIO_NA, GPIO_PU_PD_DIS),
  
  // EGPIO40_2/SGPIO2_DATAIN/MDIO4_SDA      PCIE SLOT1 present In       I       N/A
  GPIO_DEF_V2 (0, 1, 40, GPIO_FUNCTION_0, GPIO_INPUT, GPIO_PU_EN),
  
  // SPI_TPM_CS_L/AGPIO76/PSP_ROM_CS_L      SPI_TPM_CS_L       O       PU
//GPIO_DEF_V2 (0, 0, 76, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_EN),
  
  // LPC_SMI_L/NMI_SYNC_FLOOD_L/AGPIO86      CPU0_NMI_SYNC_FLOOD_R_N       I       PU
//GPIO_DEF_V2 (0, 0, 86, GPIO_FUNCTION_1, GPIO_NA, GPIO_PU_EN),
  
  // SERIRQ/EMMC_DAT7/AGPIO87      LPC IRQ       I/O       PU
//GPIO_DEF_V2 (0, 0, 87, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
  
  // LPC_CLKRUN_L/EMMC_DAT5/AGPIO88      LPC_CLKRUN_L       I       PU
//GPIO_DEF_V2 (0, 0, 88, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_EN),
  
  // PM_INTR_L/AGPIO89      VPP INTR       I       PU
//GPIO_DEF_V2 (0, 0, 89, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_EN),
  
  // ESPI_RESET_L/KBRST_L/AGPIO129      KBRST_L       I       PU
//GPIO_DEF_V2 (0, 0, 129, GPIO_FUNCTION_1, GPIO_NA, GPIO_PU_EN),
  
  // UART0_INTR/AGPIO139      NC       NC       PD
  // UART1_INTR/AGPIO144      NC       NC       PD  
  // PCIE_RST0_L/EGPIO26_0      PCIE RESET       O       N/A        CPU0&1
//GPIO_DEF_V2 (0, 0, 26, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_EN),
//GPIO_DEF_V2 (1, 0, 26, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_EN),
  
  // PCIE_RST2_L/EGPIO26_2      PCIE RESET       O       N/A        CPU0&1
//GPIO_DEF_V2 (0, 1, 26, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_EN),
//GPIO_DEF_V2 (1, 1, 26, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_EN),
  
  // EGPIO42      SAFE_CARD_PRSNT       I       N/A
  GPIO_DEF_V2 (0, 0, 42, GPIO_FUNCTION_0, GPIO_INPUT, GPIO_PU_EN),
  
  // EGPIO5_2/DEVSLP0_2      CPU0_BIOS_POST_COMPLT_N       O       N/A
  // EGPIO6_2/DEVSLP1_2      PCIE_CPU0_RAID_CARD_PRSNT_N       I       N/A
  GPIO_DEF_V2 (0, 1, 6, GPIO_FUNCTION_0, GPIO_INPUT, GPIO_PU_EN),
	
  // EGPIO70/EMMC_CLK      NC       NC       PD 
  // LPCCLK0/EMMC_DAT4/EGPIO74      LPCCLK0       I       N/A
//GPIO_DEF_V2 (0, 0, 74, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
  
  // LPCCLK1/EMMC_DAT6/EGPIO75      BMC_TO_CPU0_INIT_N       I       N/A
  GPIO_DEF_V2 (0, 0, 75, GPIO_FUNCTION_2, GPIO_INPUT, GPIO_PU_EN),
  
  // LAD0/EMMC_DAT0/EGPIO104      LAD0       I/O       PU
//GPIO_DEF_V2 (0, 0, 104, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
  
  // LAD1/EMMC_DAT1/EGPIO105      LAD1       I/O       PU
//GPIO_DEF_V2 (0, 0, 105, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
  
  // LAD2/EMMC_DAT2/EGPIO106      LAD2       I/O       PU
//GPIO_DEF_V2 (0, 0, 106, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
  
  // LAD3/EMMC_DAT3/EGPIO107      LAD3       I/O       PU
//GPIO_DEF_V2 (0, 0, 107, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
  
  // ESPI_ALERT_L/LDRQ0_L/EGPIO108      Slimeline       O       PD
  GPIO_DEF_V2 (0, 0, 108, GPIO_FUNCTION_2, GPIO_OUTPUT_HIGH, GPIO_PU_EN),
  
  // LFRAME_L/EMMC_DS/EGPIO109      LFRAME_L       O       N/A
//GPIO_DEF_V2 (0, 0, 109, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
  
  // SCL0/I2C2_SCL/EGPIO113/SPD_SCL      I2C2_SCL       OD       N/A   Spd May cannot Read
 // GPIO_DEF_V2 (0, 0, 113, GPIO_FUNCTION_3, GPIO_NA, GPIO_PU_PD_DIS),
  
  // SDA0/I2C2_SDA/EGPIO114/SPD_SDA      I2C2_SDA       OD       N/A
 // GPIO_DEF_V2 (0, 0, 114, GPIO_FUNCTION_3, GPIO_NA, GPIO_PU_PD_DIS),
  
  // SPI_CLK/ESPI_CLK      SPI Clock       O       PD
//PIO_DEF_V2 (0, 0, 117, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
 
 // SPI_CS1_L/EGPIO118      SPI_CS1_L       O       PU
//GPIO_DEF_V2 (0, 0, 118, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_EN),
  
  // SPI_CS2_L/ESPI_CS_L/EGPIO119      TPM IRQ In       I       PU
  GPIO_DEF_V2 (0, 0, 119, GPIO_FUNCTION_2, GPIO_INPUT, GPIO_PU_EN),
  
  // SPI_DI/ESPI_DAT1      SPI DI       I       PD
//PIO_DEF_V2 (0, 0, 120, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
  // SPI_DO/ESPI_DAT0      SPI DO       O       PD
//PIO_DEF_V2 (0, 0, 121, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
  // SPI_WP_L/ESPI_DAT2/EGPIO122      SPI WP       O       PU
//GPIO_DEF_V2 (0, 0, 122, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_EN),
  
  // SPI_HOLD_L/ESPI_DAT3/EGPIO133      SPI HOLD       O       PU
//GPIO_DEF_V2 (0, 0, 133, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_EN),
  
  // UART0_CTS_L/UART2_RXD/EGPIO135      NC       NC       PD  
  // UART0_RXD/EGPIO136      UART0_RXD       I       PD
//GPIO_DEF_V2 (0, 0, 136, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
  
  // UART0_RTS_L/UART2_TXD/EGPIO137      NC       NC       PU  
  // UART0_TXD/EGPIO138      UART0_TXD       O       PU
//GPIO_DEF_V2 (0, 0, 138, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
  
  // UART1_CTS_L/UART3_TXD/EGPIO140      UART1_CTS_L       I       PD
//GPIO_DEF_V2 (0, 0, 140, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
  
  // UART1_RXD/EGPIO141      UART1_RXD       I       PD
 //PIO_DEF_V2 (0, 0, 141, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
  
  // UART1_RTS_L/UART3_RXD/EGPIO142      UART1_RTS_L       O       PU
//GPIO_DEF_V2 (0, 0, 142, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
  
  // UART1_TXD/EGPIO143      UART1_TXD       O       PU
//GPIO_DEF_V2 (0, 0, 143, GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_PD_DIS),
  
  // I2C0_SCL/EGPIO145/HP_SCL      VPP IC CLOCK       OD       N/A
//GPIO_DEF_V2 (0, 0, 145, GPIO_FUNCTION_2, GPIO_NA, GPIO_PU_PD_DIS),
  
  // I2C0_SDA/EGPIO146/HP_SDA      VPP IC DATA       OD       N/A
//GPIO_DEF_V2 (0, 0, 146, GPIO_FUNCTION_2, GPIO_NA, GPIO_PU_PD_DIS),
  
  // I2C1_SCL/EGPIO147/SFP_SCL      I2C_HP_RST_N       O       N/A
  GPIO_DEF_V2 (0, 0, 147, GPIO_FUNCTION_1, GPIO_OUTPUT_HIGH, GPIO_PU_EN),
  
  // I2C1_SDA/EGPIO148/SFP_SDA      NC       NC       N/A
  // I2C4_SCL/EGPIO149      NC       NC       N/A
  // I2C4_SDA/EGPIO150      NC       NC       N/A
  // I2C5_SCL/EGPIO151      FM_BIOS_RCVR_N       I       N/A
  GPIO_DEF_V2 (0, 0, 151, GPIO_FUNCTION_1, GPIO_INPUT, GPIO_PU_EN),

  //UART0_RTS_L/UART2_TXD/EGPIO137	O	EGPIO137	////PU	GPIO£¬BIOS_POST_COMPLT_N
  GPIO_DEF_V2(0, 0, 137 , GPIO_FUNCTION_2, GPIO_OUTPUT_HIGH, GPIO_PU_EN),
  
  // I2C5_SDA/EGPIO152      NC       NC       N/A

    0xFF, //End
  }
};

//
// GEVENT Init Table
//
AMD_CPM_GEVENT_INIT_TABLE           gCpmGeventInitTableHygon52D16 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GEVENT_INIT, sizeof (gCpmGeventInitTableHygon52D16) / sizeof (UINT8), 0, 0, 0, 0x00000001},
  {//                  GEvent EventEnable   SciTrigE      SciLevl         SmiSciEn        SciS0En         SciMap      SmiTrig       SmiControl
//    GEVENT_DEFINITION (0x03,  EVENT_ENABLE, SCITRIG_HI,   SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_03,  SMITRIG_HI,   SMICONTROL_DISABLE), // GEVENT03: PM_INT_IN
//    GEVENT_DEFINITION (0x05,  EVENT_ENABLE, SCITRIG_HI,   SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_05,  SMITRIG_HI,   SMICONTROL_DISABLE), // GEVENT05: LAN_MEDIA_SENSE
//    GEVENT_DEFINITION (0x08,  EVENT_ENABLE, SCITRIG_LOW,  SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_08,  SMITRIG_LOW,  SMICONTROL_DISABLE), // GEVENT08: PCIE_WAKE_UP#
//    GEVENT_DEFINITION (0x0C,  EVENT_ENABLE, SCITRIG_LOW,  SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_12,  SMITRIG_LOW,  SMICONTROL_DISABLE), // GEVENT12: USB_OC#
//    GEVENT_DEFINITION (0x0D,  EVENT_ENABLE, SCITRIG_HI,   SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_13,  SMITRIG_HI,   SMICONTROL_DISABLE), // GEVENT13: LAN_LOW_POWER
//    GEVENT_DEFINITION (0x0E,  EVENT_ENABLE, SCITRIG_LOW,  SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_14,  SMITRIG_LOW,  SMICONTROL_DISABLE), // GEVENT14: LAN_SMART#
//    GEVENT_DEFINITION (0x0F,  EVENT_ENABLE, SCITRIG_LOW,  SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_15,  SMITRIG_LOW,  SMICONTROL_DISABLE), // GEVENT15: EVALCARD_ALERT#
	GEVENT_DEFINITION (0x05,  EVENT_ENABLE, SCITRIG_HI,   SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_05,  SMITRIG_LOW,	 SMICONTROL_NMI), // GEVENT05: LAN_MEDIA_SENSE

    0xFF,
  }
};

//
// CPM GPIO Module
//

AMD_CPM_GPIO_DEVICE_CONFIG_TABLE    gCpmGpioDeviceConfigTableHygon52D16 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_CONFIG, sizeof (gCpmGpioDeviceConfigTableHygon52D16) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {//                       DeviceId          Enable            Assert  Deassert  Hotplugs
//    GPIO_DEVICE_DEFINITION (DEVICE_ID_GBE,    CPM_DEVICE_ON,    0,      0,        0), // GBE
//    GPIO_DEVICE_DEFINITION (DEVICE_ID_BT,     CPM_DEVICE_ON,    0,      0,        0), // BT
//    GPIO_DEVICE_DEFINITION (DEVICE_ID_WLAN,   CPM_DEVICE_ON,    0,      0,        0), // WLAN
//    GPIO_DEVICE_DEFINITION (DEVICE_ID_PCIE_X16_SWITCH,   CPM_DEVICE_ON,    0,      0,        0), // PCIe 1x16/2x8 Switch
//    GPIO_DEVICE_DEFINITION (DEVICE_ID_SATAE_M2_SWITCH,   CPM_DEVICE_ON,    0,      0,        0), // SataExpress/M.2 Switch
    0xFF,
  }
};

//
// CPM Device Detection Table for Hygon52D16
//
AMD_CPM_GPIO_DEVICE_DETECTION_TABLE gCpmGpioDeviceDetectionTableHygon52D16 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_DETECTION, sizeof (gCpmGpioDeviceDetectionTableHygon52D16) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
//  {UINT8  DeviceId; // Device Id
//   UINT8  Type;     // Detection type. 0: One GPIO pin. 1: Two GPIO pins. 2: Special Pin
//   UINT16 PinNum1;  // Pin number of GPIO 1
//   UINT8  Value1;   // Value of GPIO 1
//   UINT16 PinNum2;  // Pin number of GPIO 2
//   UINT8  Value2;   // Value of GPIO 2
//   UINT16 PinNum3;  // Pin number of GPIO 3
//   UINT8  Value3;   // Value of GPIO 3},
    0xFF,
  }
};

//
// CPM Device Reset Table
//
AMD_CPM_GPIO_DEVICE_RESET_TABLE gCpmGpioDeviceResetTableHygon52D16 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_RESET, sizeof (gCpmGpioDeviceResetTableHygon52D16) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
//  {UINT8  DeviceId;       // Device Id
//   UINT8  Mode;           // Reset mode     // 0: Reset Assert. // 1: Reset De-assert             // 2: Delay between Assert and Deassert
//   UINT8  Type;           // Register type  // 0: GPIO.         // 1: Special pin.if Mode = 0 or 1
//   UINT32 ((UINT16)Pin + ((UINT8)Value << 16));                 // GPIO pin value
//   UINT8  InitFlag;       // Init flag in post},
    0xFF,
  }
};

//
// CPM GPIO Device Init Table (Power On/Off)
//
AMD_CPM_GPIO_DEVICE_POWER_TABLE gCpmGpioDevicePowerTableHygon52D16 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_POWER, sizeof (gCpmGpioDevicePowerTableHygon52D16) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
//  {UINT8  DeviceId;       // Device Id
//   UINT8  Mode;           // Device Power Mode. 1: Power On. 0: Power Off
//   UINT8  Type;           // Device Power Item. 0: Set GPIO. 1: Wait GPIO. 2: Add Delay
//   UINT32 ((UINT16)Pin + ((UINT8)Value << 16));                 // GPIO pin value or delay timer
//   UINT8  InitFlag;       // Init flag in post},
//    {DEVICE_ID_GBE,    CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (GBE_POWER_CONTROL_REVA, 1),  0},
//    {DEVICE_ID_BT,     CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (BT_POWER_CONTROL_REVA, 0),   0},
//    {DEVICE_ID_WLAN,   CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (WLAN_POWER_CONTROL_REVA, 0), 0},
//    {DEVICE_ID_PCIE_X16_SWITCH,   CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (PCIE_X16_SWITCH, 0), 0},
//    {DEVICE_ID_SATAE_M2_SWITCH,   CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (SATAE_M2_SWITCH, 0), 0},

//    {DEVICE_ID_GBE,    CPM_POWER_ON,  CPM_POWER_SET,   CPM_GPIO_PIN (GBE_POWER_CONTROL_REVA, 0),  0},
//    {DEVICE_ID_BT,     CPM_POWER_ON,  CPM_POWER_SET,   CPM_GPIO_PIN (BT_POWER_CONTROL_REVA, 1),   0},
//    {DEVICE_ID_WLAN,   CPM_POWER_ON,  CPM_POWER_SET,   CPM_GPIO_PIN (WLAN_POWER_CONTROL_REVA, 1), 0},
//    {DEVICE_ID_PCIE_X16_SWITCH,   CPM_POWER_ON, CPM_POWER_SET,   CPM_GPIO_PIN (PCIE_X16_SWITCH, 1), 0},
//    {DEVICE_ID_SATAE_M2_SWITCH,   CPM_POWER_ON, CPM_POWER_SET,   CPM_GPIO_PIN (SATAE_M2_SWITCH, 1), 0},
    0xFF,
  }
};

//
// PCIE Clock Table
//
AMD_CPM_PCIE_CLOCK_TABLE    gCpmPcieClockTableHygon52D16 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table content
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_PCIE_CLOCK, sizeof (gCpmPcieClockTableHygon52D16) / sizeof (UINT8), 0, 0, 0, 0x0F},
  {
//  {UINT8  ClkId;              // FCH PCIe Clock
//   UINT8  ClkReq;             // FCH PCIe ClkReq
//   UINT8  ClkIdExt;           // External Clock Source
//   UINT8  ClkReqExt;          // External ClkReq
//   UINT8  DeviceId;           // Device Id. No Device Id if 0xFF
//   UINT8  Device;             // Device Number of PCIe bridge
//   UINT8  Function;           // Function Number of PCIe bridge
//   UINT8  SlotCheck;          // Slot Check Flag: // BIT0: Check PCI Space // BIT1: Check GPIO pin // BIT2: Check Clock Power Management Enable // BIT3~6: Reserved // BIT7: Change PCIe Clock in ACPI method
//   UINT32 SpecialFunctionId;  // Id of Special Function}
    // GPP_CLK0 clock for PCIE X8 slot is controlled by CLK_REQ0_L. For Bristol, this slot is not used
//    {GPP_CLK0,            CLK_ENABLE,  SRC_CLK4,  CLK_ENABLE,  DEVICE_ID_PCIE_X8_SLOT,  0, 0, NON_SLOT_CHECK,            0},
    // GPP_CLK1 clock for SSD is controlled by CLK_REQ1_L
//    {GPP_CLK1,            CLK_REQ1,    SRC_CLK8,  CLK_REQ8,    DEVICE_ID_M2_CONNECT,    0, 0, NON_SLOT_CHECK,            0},
    // GPP_CLK2 clock for Promontory chip is always on
//    {GPP_CLK2,            CLK_ENABLE,  SRC_CLK6,  CLK_ENABLE,  DEVICE_ID_PROMONTORY,    0, 0, NON_SLOT_CHECK,            0},
    // GFX_CLK clock for PCIE X16 slot is controlled by CLK_REQG_L. For Bristol, this slot is used as x8
//    {GPP_CLK9,            CLK_REQGFX,  SRC_CLK2,  CLK_REQ2,    DEVICE_ID_PCIE_X16_SLOT, 0, 0, NON_SLOT_CHECK,            0},
    // GPP_CLK3 clock for PCIE X4 slot is always on
//    {GPP_CLK3,            CLK_ENABLE,  SRC_CLK10, CLK_ENABLE,  DEVICE_ID_PCIE_X4_SLOT,  0, 0, NON_SLOT_CHECK,            0},
//    {CPM_CLKID_PCIE_RCLK, CLK_DISABLE, SRC_SKIP,  0,           0,                       0, 0, NON_SLOT_CHECK,            0},
    0xFF,
  }
};


/* //

[7000]
00 - 15   PCIE   <---
16 - 31   xGMI
32 - 47   PCIE   <---
48 - 63   xGMI

64 - 79   xGMI
80 - 95   PCIE   <---
96 - 111  xGMI
112- 127  PCIE   <---
*///

//
// CPM DXIO Topology Table Socket 0 with field card in slot1
//
AMD_CPM_DXIO_TOPOLOGY_TABLE gCpmDxioTopologyTableHygon52D16S0 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_DXIO_TOPOLOGY, sizeof (gCpmDxioTopologyTableHygon52D16S0) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  0,  // SocketId
  {   // DXIO_PORT_DESCRIPTOR
//Lane 0-7, 8x, OCP3.0 
 /* {
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 0, 7, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
         DxioPortEnabled,                      // Port Present
         0,                                    // Requested Device
         0,                                    // Requested Function
         DxioHotplugDisabled,                  // Hotplug
         DxioGenMaxSupported,                  // Max Link Speed
         DxioGenMaxSupported,                  // Max Link Capability
         DxioAspmDisabled,                        // ASPM
         DxioAspmDisabled,                     // ASPM L1.1 disabled
         DxioAspmDisabled,                     // ASPM L1.2 disabled
         DxioClkPmSupportDisabled              // Clock PM
       )
  },
  */
	


//Lane 8, 1x, BMC
  {
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 8, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                     // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      DxioHotplugDisabled,                  // Hotplug
      DxioGenMaxSupported,                  // Max Link Speed  
      DxioGenMaxSupported,                  // Max Link Capability  
      DxioAspmDisabled,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  },
	
	//Lane 9, 1x, 1061R_A
  {
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 9, 9, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                     // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      DxioHotplugDisabled,                  // Hotplug
      DxioGen2,                  // Max Link Speed  
      DxioGen2,                  // Max Link Capability  
      DxioAspmDisabled,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  },
	
		//Lane 10, 1x, 1061R_B
  {
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 10, 10, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                     // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      DxioHotplugDisabled,                  // Hotplug
      DxioGen2,                  // Max Link Speed  
      DxioGen2,                  // Max Link Capability  
      DxioAspmDisabled,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  },

		//Lane 11, 1x, SOC security Card
  {
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 11, 11, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                     // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      DxioHotplugDisabled,                  // Hotplug
      DxioGenMaxSupported,                  // Max Link Speed  
      DxioGenMaxSupported,                  // Max Link Capability  
      DxioAspmDisabled,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  },
	
	//Lane 12-15, 4x, GbE Controller WX1860A2
  {
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 12, 15, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                     // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      DxioHotplugDisabled,                  // Hotplug
      DxioGenMaxSupported,                  // Max Link Speed  
      DxioGenMaxSupported,                  // Max Link Capability  
      DxioAspmDisabled,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  },
	
	//Lane 48-55, 8x, PCIE
  {
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 48, 55, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                     // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      DxioHotplugDisabled,                  // Hotplug
      DxioGenMaxSupported,                  // Max Link Speed  
      DxioGenMaxSupported,                  // Max Link Capability  
      DxioAspmDisabled,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  },
	
	//Lane 56-63, 8x, PCIE
  {
    DESCRIPTOR_TERMINATE_LIST,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 56, 63, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                     // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      DxioHotplugDisabled,                  // Hotplug
      DxioGenMaxSupported,                  // Max Link Speed  
      DxioGenMaxSupported,                  // Max Link Capability  
      DxioAspmDisabled,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  },
	
 }   // End of DXIO_PORT_DESCRIPTOR
};



//
// CPM DXIO Topology Table
//
AMD_CPM_DXIO_TOPOLOGY_TABLE gCpmDxioTopologyTableHygon52D16S1 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_DXIO_TOPOLOGY_S1, sizeof (gCpmDxioTopologyTableHygon52D16S1) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  1,  // SocketId
  {   // DXIO_PORT_DESCRIPTOR

	//Lane 0-15, 16x, PCIE
  {
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 0 ,15, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                     // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      DxioHotplugDisabled,                  // Hotplug
      DxioGenMaxSupported,                  // Max Link Speed  
      DxioGenMaxSupported,                  // Max Link Capability  
      DxioAspmDisabled,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  },

	//Lane 48-55, 8x, PCIE
  {
    DESCRIPTOR_TERMINATE_LIST,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 48 ,55, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                     // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      DxioHotplugDisabled,                  // Hotplug
      DxioGenMaxSupported,                  // Max Link Speed  
      DxioGenMaxSupported,                  // Max Link Capability  
      DxioAspmDisabled,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  },
	
	//Lane 56-63, 8x, PCIE
 /* {
    DESCRIPTOR_TERMINATE_LIST,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 56 ,63, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                     // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      DxioHotplugDisabled,                  // Hotplug
      DxioGenMaxSupported,                  // Max Link Speed  
      DxioGenMaxSupported,                  // Max Link Capability  
      DxioAspmDisabled,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  },*/

  }   // End of DXIO_PORT_DESCRIPTOR
};





/*----------------------------------------------------------------------------------------*/
/**
 * Detect PCIe Devices Automatically
 *
 * This function patches PCIe Topology Override Table by detecting PCIe devices
 * automatically.
 *
 * @param[in]     AmdCpmTablePpi                Pointer to AMD CPM Table PPI
 * @param[in]     PcieTopologyTablePtr          Pointer to PCIe Topology Table
 * @param[in]     BoardId                       Current Board ID
 *
 */
VOID
DetectPcieDevices (
  IN       AMD_CPM_TABLE_PPI            *AmdCpmTablePpi,
  IN       AMD_CPM_PCIE_TOPOLOGY_TABLE  *PcieTopologyTablePtr,
  IN       UINT16                       BoardId
  )
{

}

/*----------------------------------------------------------------------------------------*/
/**
 * CPM Override Function After AMD CPM Table PPI
 *
 * This function updates CPM OEM Tables according to setup options or the value to be detected
 * on run time after AMD CPM Table PPI is installed.
 *
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Function initialized successfully
 * @retval        EFI_ERROR      Function failed (see error for more details)
 *
 */
EFI_STATUS
EFIAPI
AmdCpmTableOverride (
  IN       EFI_PEI_SERVICES       **PeiServices
  )
{
  EFI_STATUS                        Status = EFI_SUCCESS;

  return Status;
}

