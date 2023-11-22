/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPM OEM API, and related functions.
 *
 * Contains OEM code that defined CPM tables before CPM init.
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

#ifndef _HYGON_CPM_OEM_INIT_PEIM_H_
#define _HYGON_CPM_OEM_INIT_PEIM_H_

#define HYGON_CPM_OEM_REVISION  0x1080

// byo230824 - >>
#include <HygonGpioBase.h>
/*
#define GPIO_STICKY_DIS   0
#define GPIO_STICKY_EN    1
#define GPIO_FUNCTION_0   0
#define GPIO_FUNCTION_1   1
#define GPIO_FUNCTION_2   2
#define GPIO_FUNCTION_3   3
#define GPIO_FUNCTION_4   4
#define GPIO_FUNCTION_5   5
#define GPIO_FUNCTION_6   6
#define GPIO_FUNCTION_7   7
#define GPIO_PD_EN        0
#define GPIO_PU_EN        1
#define GPIO_PU_PD_DIS    2
#define GPIO_OUTPUT_LOW   0
#define GPIO_OUTPUT_HIGH  1
#define GPIO_INPUT        2
#define GPIO_NA           3
#define OUTPUT_DISABLE    0
#define OUTPUT_ENABLE     1
*/
// byo230824 - <<

#define EVENT_DISABLE       0
#define EVENT_ENABLE        1
#define SCITRIG_LOW         0
#define SCITRIG_HI          1
#define SCITRIG_AUTO        3
#define SCILEVEL_EDGE       0
#define SCILEVEL_LEVEL      1
#define SMISCI_DISABLE      0
#define SMISCI_ENABLE       1
#define SCIS0_DISABLE       0
#define SCIS0_ENABLE        1
#define SCIMAP_00           0
#define SCIMAP_01           1
#define SCIMAP_02           2
#define SCIMAP_03           3
#define SCIMAP_04           4
#define SCIMAP_05           5
#define SCIMAP_06           6
#define SCIMAP_07           7
#define SCIMAP_08           8
#define SCIMAP_09           9
#define SCIMAP_10           10
#define SCIMAP_11           11
#define SCIMAP_12           12
#define SCIMAP_13           13
#define SCIMAP_14           14
#define SCIMAP_15           15
#define SCIMAP_16           16
#define SCIMAP_17           17
#define SCIMAP_18           18
#define SCIMAP_19           19
#define SCIMAP_20           20
#define SCIMAP_21           21
#define SCIMAP_22           22
#define SCIMAP_23           23
#define SMITRIG_LOW         0
#define SMITRIG_HI          1
#define SMICONTROL_DISABLE  0
#define SMICONTROL_SMI      1
#define SMICONTROL_NMI      2
#define SMICONTROL_IRQ13    3

#define MISC_BASE       0xE00
#define FCH_MISC_REG00  0x00                    // ClkCntrl0
#define FCH_MISC_REG04  0x04                    // ClkCntrl1

#define EVAL_PRESENT       40
#define EVAL_DETECT_VALUE  0
#define EVAL_POWER_GOOD    89
// LAN
#define GBE_POWER_CONTROL_REVA  23      // GBE_EN#

#define WLAN_POWER_CONTROL_REVA  6      // WLAN_DISABLE_L

#define BT_POWER_CONTROL_REVA  8        // WLAN_BT_DIS#

#define PCIE_X16_SWITCH  3              // PCIe x8/x16 switch

#define SATAE_M2_SWITCH  131            // SATAe/M.2 switch

#define BoardRevA  0
#define BoardRevB  1

/// Physical or virtual on-board device Id.

typedef enum {
  DEVICE_ID_PCIE_X8_SLOT    =             0x01,                         ///< Device ID for PCIE X8 slot device. GPP_CLK0
  DEVICE_ID_PCIE_X16_SLOT   =             0x02,                         ///< Device ID for PCIE x16 slot device, GFX_CLK
  DEVICE_ID_PCIE_X4_SLOT    =             0x03,                         ///< Device ID for PCIE x4 slot device, GPP_CLK3
  DEVICE_ID_BT              =             0x04,                         ///< Device Id for BlueTooth
  DEVICE_ID_GBE             =             0x05,                         ///< Device Id for GBE
  DEVICE_ID_WLAN            =             0x06,                         ///< Device ID for WLAN
  DEVICE_ID_M2_CONNECT      =             0x07,                         ///< Device ID for M2 connect, GPP_CLK1
  DEVICE_ID_SATA_EXPRESS    =             0x09,                         ///< Device ID for SATA Express
  DEVICE_ID_PCIE_X16_SWITCH =             0x0A,                         ///< Device ID for PCIe x16/x8 switch
  DEVICE_ID_SATAE_M2_SWITCH =             0x0B,                         ///< Device ID for SATAe/M.2 switch
} CPM_GPIO_DEVICE_ID;

/// Configuration values for GppClk in Fch

typedef enum {
  GPP_CLK0 =                          0x00,                       ///< GPP_CLK0
  GPP_CLK1,                                                       ///< GPP_CLK1
  GPP_CLK2,                                                       ///< GPP_CLK2
  GPP_CLK3,                                                       ///< GPP_CLK3
  GPP_CLK4,                                                       ///< GPP_CLK4
  GPP_CLK5,                                                       ///< GPP_CLK5
  GPP_CLK6,                                                       ///< GPP_CLK6
  GPP_CLK7,                                                       ///< GPP_CLK7
  GPP_CLK8,                                                       ///< GPP_CLK8
  GPP_CLK9,                                                       ///< GFX_CLK
  GPP_SKIP =                          0xFE,                       ///< SKIP
} CPM_GPP_CLK;

/// Configuration values for SrcClk in ClkGen

typedef enum {
  SRC_CLK0 =                          0x00,                       ///< SRC0
  SRC_CLK1,                                                       ///< SRC1
  SRC_CLK2,                                                       ///< SRC2
  SRC_CLK3,                                                       ///< SRC3
  SRC_CLK4,                                                       ///< SRC4
  SRC_CLK5,                                                       ///< SRC5
  SRC_CLK6,                                                       ///< SRC6
  SRC_CLK7,                                                       ///< SRC7
  SRC_CLK8,                                                       ///< SRC8
  SRC_CLK9,                                                       ///< SRC9
  SRC_CLK10,                                                      ///< SRC10
  SRC_CLK11,                                                      ///< SRC11
  SRC_SKIP =                          0xFE,                       ///< SKIP
} CPM_SRC_CLK;

/// Configuration values for ClkReq

typedef enum {
  CLK_DISABLE =                       0x00,                       ///< CLK_DISABLE
  CLK_REQ0,                                                       ///< CLK_REQ0
  CLK_REQ1,                                                       ///< CLK_REQ1
  CLK_REQ2,                                                       ///< CLK_REQ2
  CLK_REQ3,                                                       ///< CLK_REQ3
  CLK_REQ4,                                                       ///< CLK_REQ4
  CLK_REQ5,                                                       ///< CLK_REQ5
  CLK_REQ6,                                                       ///< CLK_REQ6
  CLK_REQ7,                                                       ///< CLK_REQ7
  CLK_REQ8,                                                       ///< CLK_REQ8
  CLK_REQGFX =                        0x0C,                       ///< CLK_REQG
  CLK_ENABLE =                        0xFF,                       ///< CLK_ENABLE
} CPM_CLK_REQ;

/// Configuration values for Slot Check

typedef enum {
  NON_SLOT_CHECK =                    0x00,                       ///< Do not check the device on slot
  SLOT_CHECK     =                        0x01,                   ///< Check PCI space of the device on slot
  GPIO_CHECK     =                        0x02,                   ///< Check GPIO pin of the device on slot
  CLKPM_CHECK    =                       0x05,                    ///< Check Clock Power Management Enable of the device on slot
  ACPI_CHANGE    =                       0x80,                    ///< Change PCIe Clock in ACPI method
} CPM_SLOT_CHECK;

/// Structure for Variables to be used for CPM

typedef struct {
  UINT8    LoopbackAdaptor;                                       ///< Loopback Adaptor
                                                                  ///< 0: None
                                                                  ///< 1: Enabled
  UINT8    BlueToothEn;                                           ///< BlurTooth Device
                                                                  ///< 0: Auto
                                                                  ///< 1: Disabled
  UINT8    SystemBootWithPS0;                                     ///< System Boot with PS0
                                                                  ///< 0: Yes
                                                                  ///< 1: No
  UINT8    UnusedGppClkOff;                                       ///< Unused GPP Clocks Off
                                                                  ///< 0: Disabled
                                                                  ///< 1: Enabled
  UINT8    ClockRequest;                                          ///< Clock Request
                                                                  ///< 0: Disable
                                                                  ///< 1: Enable
  UINT8    OemTravisLCtrl;                                        ///< Travis-L Control
  UINT8    SpreadSpectrumSwitch;                                  ///< External Spread Spectrum Control
                                                                  ///< 0: Disabled
                                                                  ///< 1: Enabled
  UINT8    LanPowerControl;                                       ///< GBE power control
                                                                  ///< 1: Disable
                                                                  ///< 0: Enables
  UINT8    WlanPowerControl;                                      ///< WLAN power control
                                                                  ///< 0: Disabled
                                                                  ///< 1: Enabled
  UINT8    PX56Support;                                           ///< PowerXpress 5.6 support
                                                                  ///< 0: Disable
                                                                  ///< 1: Enable
  UINT8    EvalAutoDetection;                                     ///< PowerXpress 5.6 support
                                                                  ///< 0: Disable
                                                                  ///< 1: Enable
  UINT8    HygonPspEnable;                                        ///< Enable PSP
                                                                  ///< 0: Disable
                                                                  ///< 1: Enable
} CPM_OEM_SETUP_OPTION;

EFI_STATUS
EFIAPI
HygonCpmOemSetupOption (
  IN       EFI_PEI_SERVICES       **PeiServices,
  IN       CPM_OEM_SETUP_OPTION   *SetupOption
  );

#endif
