/* $NoKeywords:$ */

/**
 * @file
 *
 * Graphics Controller family specific service procedure
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: FCH
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "FchCommonCfg.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 * Default FCH interface settings at InitReset phase.
 *----------------------------------------------------------------------------------------
 */
CONST FCH_RESET_INTERFACE ROMDATA  FchResetInterfaceDefault = {
  TRUE,                // UmiGen2
  0xFFFFFFFF,          // SataEnable
  TRUE,                // IdeEnable
  TRUE,                // GppEnable
  0xFFFFFFFFFFFFFFFF,  // XhciEnable
};

/*----------------------------------------------------------------
 *  InitReset Phase Data Block Default (Failsafe)
 *----------------------------------------------------------------
 */
FCH_RESET_DATA_BLOCK  InitResetCfgDefault = {
  NULL,                  // StdHeader
  { 
    TRUE,                // UmiGen2
    0xFFFFFFFF,          // SATA Controller
    FALSE,               // IdeEnable
    FALSE,               // GppEnable
    0xFFFFFFFFFFFFFFFF,  // XHCI Controller
  },                     // FchReset
  {                      // SATA_ST
    0,                   // SataModeReg
    0xFFFFFFFF,          // SataEnable
    0,                   // Sata6AhciCap
    FALSE,               // SataSetMaxGen2
    TRUE,                // IdeEnable
    1,                   // SataClkMode
  },                     
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
    2,                   // Speed
    0,                   // FastSpeed
    3,                   // WriteSpeed
    0,                   // Mode
    0,                   // AutoMode
    0,                   // BurstWrite
    TRUE,                // LpcClk0
    TRUE,                // LpcClk1
    1,                   // SPI100_Enable
    { 0 }                // SpiDeviceProfile
  },
  {                      //FCH_ESPI
    FALSE,               // EspiIo80Enable
    FALSE,               // EspiKbc6064Enable
    FALSE,               // EspiEc0Enable
  },
  { 0 },                 // FchBldCfg
  {
    0,                   // USB3 ECC SMI control
    0xFF,                // USB3 Controller0 Port Num
    0xFF,                // USB3 Controller1 Port Num
    0x01B3C953,          // USB3 LANEPARACTL0
  },
  {                      // FCH_MISC2
    0,                   // Cg2Pll
    FALSE,               // EcKbd
    FALSE,               // LegacyFree
    FALSE,               // SataInternal100Spread
    FALSE,               // EcChannel0
    FALSE,               // QeEnabled
    FALSE,               // FCH OSCOUT1_CLK Continous
    0,                   // LpcClockDriveStrength
    NULL,                // EarlyOemGpioTable
    TRUE,                // WdtEnable
    NULL,                // OemResetProgrammingTablePtr
    0xFED80000,          // FchAcpiMmioBase
    FALSE,               // FchSerialIrqSupport
  }
};
