/* $NoKeywords:$ */
/**
 * @file
 *
 * Graphics Controller family specific service procedure
 *
 *
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
* ***************************************************************************
*
*/


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "FchPlatform.h"
#include  "Filecode.h"
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */



/*----------------------------------------------------------------------------------------
 * Default FCH interface settings at InitReset phase.
 *----------------------------------------------------------------------------------------
 */
CONST FCH_RESET_INTERFACE ROMDATA FchResetInterfaceDefault = {
  TRUE,                // UmiGen2
  TRUE,                // SataEnable
  TRUE,                // IdeEnable
  TRUE,                // GppEnable
  TRUE,                // Xhci0Enable
  TRUE                 // Xhci1Enable
};


/*----------------------------------------------------------------
 *  InitReset Phase Data Block Default (Failsafe)
 *----------------------------------------------------------------
 */
FCH_RESET_DATA_BLOCK   InitResetCfgDefault = {
  NULL,                  // StdHeader
  { TRUE,
    TRUE,
    FALSE,
    FALSE,
    TRUE,
    TRUE
    },                   // FchReset

  0,                     // FastSpeed
  3,                     // WriteSpeed
  0,                     // Mode
  0,                     // AutoMode
  0,                     // BurstWrite
  FALSE,                 // SataIdeCombMdPriSecOpt
  0,                     // Cg2Pll
  FALSE,                 // EcKbd
  FALSE,                 // LegacyFree
  FALSE,                 // SataSetMaxGen2
  1,                     // SataClkMode
  0,                     // SataModeReg
  FALSE,                 // SataInternal100Spread
  2,                     // SpiSpeed
//  0xFCFCFCFC,                     // 38
//  0x88FC,                     // 3c
//  0,                     // 1d_34
  1,                     // 20_0
  FALSE,                 // EcChannel0

  {                      // FCH_GPP
    {                    // Array of FCH_GPP_PORT_CONFIG       PortCfg[4]
      {
        FALSE,           // PortPresent
        FALSE,           // PortDetected
        FALSE,           // PortIsGen2
        FALSE,           // PortHotPlug
        0,               // PortMisc
      },
      {
        FALSE,           // PortPresent
        FALSE,           // PortDetected
        FALSE,           // PortIsGen2
        FALSE,           // PortHotPlug
        0,               // PortMisc
      },
      {
        FALSE,           // PortPresent
        FALSE,           // PortDetected
        FALSE,           // PortIsGen2
        FALSE,           // PortHotPlug
        0,               // PortMisc
      },
      {
        FALSE,           // PortPresent
        FALSE,           // PortDetected
        FALSE,           // PortIsGen2
        FALSE,           // PortHotPlug
        0,               // PortMisc
      },
    },
    PortA1B1C1D1,        // GppLinkConfig
    FALSE,               // GppFunctionEnable
    FALSE,               // GppToggleReset
    0,                   // GppHotPlugGeventNum
    0,                   // GppFoundGfxDev
    FALSE,               // GppGen2
    0,                   // GppGen2Strap
    FALSE,               // GppMemWrImprove
    FALSE,               // GppUnhidePorts
    0,                   // GppPortAspm
    FALSE,               // GppLaneReversal
    FALSE,               // GppPhyPllPowerDown
    FALSE,               // GppDynamicPowerSaving
    FALSE,               // PcieAer
    FALSE,               // PcieRas
    FALSE,               // PcieCompliance
    FALSE,               // PcieSoftwareDownGrade
    FALSE,               // UmiPhyPllPowerDown
    FALSE,               // SerialDebugBusEnable
    0,                   // GppHardwareDownGrade
    0,                   // GppL1ImmediateAck
    FALSE,               // NewGppAlgorithm
    0,                   // HotPlugPortsStatus
    0,                   // FailPortsStatus
    40,                  // GppPortMinPollingTime
    FALSE,               // IsCapsuleMode
  },
  {                      // FCH_SPI
    FALSE,               // LpcMsiEnable
    0x00000000,          // LpcSsid
    0,                   // RomBaseAddress
    0,                   // Speed
    0,                   // FastSpeed
    0,                   // WriteSpeed
    0,                   // Mode
    0,                   // AutoMode
    0,                   // BurstWrite
    TRUE,                // LpcClk0
    TRUE,                // LpcClk1
    0,                   // SPI100_Enable
    {0}                  // SpiDeviceProfile
  },
  FALSE,                 // QeEnabled
  FALSE,                 // FCH OSCOUT1_CLK Continous
  0,                     // LpcClockDriveStrength
  {                      // FCH_PT
    {1, 1, 0, 0, 1, 1, 1, 0, 0},// FCH_PT_USB
    {1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0},// FCH_PT_SATA
    {1, 1},// FCH_PT_PCIE
    {0xFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0000000000000000},// FCH_PT_ADDR
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},// FCH_PT_USBPort for PROM3/4
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},            // FCH_PT_USBPort for PROM2
    {1, 1, 1, 1, 1, 1, 1, 1, 1},               // FCH_PT_USBPort for PROM1
    {                      // PT USB TX
      {                     // Array of USB31[2] Tx setting
        {0x0F,0x0F,0x01,0x01,0x04,0x03,0x00,0x01,0x04,0x01},         // USB31PCS_B1 Tx setting
        {0x0F,0x0F,0x01,0x01,0x04,0x04,0x00,0x01,0x04,0x01},         // USB31PCS_B2 Tx setting
      },
      {                     // Array of USB30[3] Tx setting
        {0x0F,0x01,0x04},                                            // USB30PCS_P0 Tx setting
        {0x0F,0x01,0x08},                                            // USB30PCS_P1 Tx setting
        {0x0F,0x01,0x04},                                            // USB30PCS_P2 Tx setting
        {0x0F,0x01,0x08},                                            // USB30PCS_P3 Tx setting
        {0x0F,0x01,0x04},                                            // USB30PCS_P4 Tx setting
        {0x0F,0x01,0x08},                                            // USB30PCS_P5 Tx setting
      },
      5,                                               // USB2.0 TX driving current, 7: largest By USB_HSDP/N[0]
      5,                                               // USB2.0 TX driving current, 7: largest By USB_HSDP/N[5]
      5,                                               // USB2.0 TX driving current, 7: largest By USB_HSDP/N[13][11]
      5,                                               // USB2.0 TX driving current, 7: largest By USB_HSDP/N[12][10]
      5,                                               // USB2.0 TX driving current, 7: largest By USB_HSDP/N[2][6]
      5,                                               // USB2.0 TX driving current, 7: largest By USB_HSDP/N[3][7]
      5,                                               // USB2.0 TX driving current, 7: largest By USB_HSDP/N[4][8]
      5,                                               // USB2.0 TX driving current, 7: largest By USB_HSDP/N[1][9]      
    },
    {                      // PT SATA TX
      {0x00,0x04,0x08,0x00,0x01,0x01,0x00,0x02,0x03},             // SATA Port0 TX SETTINGS
      {0x00,0x04,0x08,0x00,0x01,0x01,0x00,0x02,0x03},             // SATA Port1 TX SETTINGS
      {0x00,0x04,0x08,0x00,0x01,0x01,0x00,0x02,0x03},             // SATA Port2 TX SETTINGS
      {0x00,0x04,0x08,0x00,0x01,0x01,0x00,0x02,0x03},             // SATA Port3 TX SETTINGS
      {0x00,0x04,0x08,0x00,0x01,0x01,0x00,0x02,0x03},             // SATA Port4 TX SETTINGS
      {0x00,0x04,0x08,0x00,0x01,0x01,0x00,0x02,0x03},             // SATA Port5 TX SETTINGS
      {0x00,0x04,0x08,0x00,0x01,0x01,0x00,0x02,0x03},             // SATA Port6 TX SETTINGS
      {0x00,0x04,0x08,0x00,0x01,0x01,0x00,0x02,0x03},             // SATA Port7 TX SETTINGS
    },
    {1, 1, 1, 1, 1, 1, 1, 1},// Enable/Disable PCIE Switch Downstream Ports
    {                        // PTSvidSsid
      {0x1B21,0x1142},                                            // XHCI_SVID XHCI_SSID
      {0x1B21,0x1062},                                            // AHCI_SVID AHCI_SSID
      {0x1B21,0x0201},                                            // PCIE_SVID PCIE_SSID
      {0x1B21,0x3306},                                            // PCIE_DSP0_SVID PCIE_DSP0_SSID
      {0x1B21,0x3306},                                            // PCIE_DSP1_SVID PCIE_DSP1_SSID
      {0x1B21,0x3306},                                            // PCIE_DSP2_SVID PCIE_DSP2_SSID
      {0x1B21,0x3306},                                            // PCIE_DSP3_SVID PCIE_DSP3_SSID
      {0x1B21,0x3306},                                            // PCIE_DSP4_SVID PCIE_DSP4_SSID
    },
  },
  NULL,                  // EarlyOemGpioTable
  {0},                   // FchBldCfg
  0,                     // USB3 ECC SMI control
  0xFF,                  // USB3 Controller0 Port Num
  0xFF,                  // USB3 Controller1 Port Num
  0x01B3C953,            // USB3 LANEPARACTL0
  FALSE,                 // EspiEnable
  FALSE,                 // EspiIo80Enable
  FALSE,                 // EspiKbc6064Enable
  FALSE,                 // EspiEc0Enable
  TRUE,                  // WdtEnable
  NULL,                  // OemResetProgrammingTablePtr
  0xFED80000,            //  FchAcpiMmioBase
};


