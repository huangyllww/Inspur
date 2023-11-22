/* $NoKeywords:$ */

/**
 * @file
 *
 * Platform PCIe Complex Definitions
 *
 * Contains HYGON HGPI core interface
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Include
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
 *
 ***************************************************************************/

#ifndef _HYGON_SERVER_HOTPLUG_H_
#define _HYGON_SERVER_HOTPLUG_H_
#include <HygonSmuBiosIf.h>

#ifndef DESCRIPTOR_TERMINATE_LIST
  #define DESCRIPTOR_TERMINATE_LIST  0x80000000ull
#endif

/// PCIe Hot Plug Format
typedef enum {
  HotplugPresenceDetect,                                      ///< Simple Presence Detect
  HotplugExpressModule,                                       ///< PCIe Express Module
  HotplugEnterpriseSsd,                                       ///< Enterprise SSD
  HotplugExpressModuleB,                                      ///< PCIe Express Module B
  MaxHotplugFormat                                            ///< Not valid value, used to verify input
} HSIO_HOTPLUG_TYPE;

/// Engine Configuration
typedef struct {
  IN       UINT8    StartLane;                            /**< Start Lane ID (in reversed configuration StartLane > EndLane)
                                                           * This value should correspond exactly to the definition the PCIe topology
                                                           */
  IN       UINT8    EndLane;                              /**< End lane ID (in reversed configuration StartLane > EndLane)
                                                           * This value should correspond exactly to the definition the PCIe topology
                                                           */
  IN       UINT8    SocketNumber;                         ///< Socket Number of this port
  IN       UINT8    SlotNumber;                           ///< Unique identifier for the physical slot number of this port
} HOTPLUG_ENGINE_DATA;

typedef struct {
  UINT32                   Flags;
  HOTPLUG_ENGINE_DATA      Engine;
  PCIE_HOTPLUG_MAPPING     Mapping;
  PCIE_HOTPLUG_FUNCTION    Function;
  PCIE_HOTPLUG_RESET       Reset;
  PCIE_HOTPLUG_GPIO        Gpio;
} HOTPLUG_DESCRIPTOR;

typedef enum {
  Pca9539 = 0,
  Pca9555,
  Pca9506,
} I2C_CHIPSELECT;

typedef enum {
  Low8BitGpio = 0,
  High8BitGpio,
} I2C_GPIO_BYTEMAPPING;

typedef enum {
  Pca9545BusSegment0 = 0,
  Pca9545BusSegment1,
  Pca9545BusSegment2,
  Pca9545BusSegment3,
  Pca9545BusSegmentNone = 7,
} PCA_9545_BUS_SEG;

typedef struct {
  // Lane setting
  UINT8                   StartLane;
  UINT8                   EndLane;
  UINT8                   SocketNumber;

  // Hotplug type
  HSIO_HOTPLUG_TYPE       HotplugType;

  // I2C Device
  PCA_9545_BUS_SEG        Pca9545BusSegment;
  I2C_CHIPSELECT          PcaChipSelect;
  UINT8                   I2CSlaveAddress;
  I2C_GPIO_BYTEMAPPING    I2CGpioByteMapping;
  UINT8                   FunctionMask;
} HOTPLUG_USERCONFIG_DESCRIPTOR;

// Macro for statically initializing various structures
#define  HOTPLUG_ENGINE_DATA_INITIALIZER(mStartLane, mEndLane, mSocketNumber, mSlotNumber)  { mStartLane, mEndLane, \
                                                                                              mSocketNumber, \
                                                                                              mSlotNumber },

#define  PCIE_HOTPLUG_INITIALIZER_MAPPING(mHotPlugFormat, \
                                          mGpioDescriptorValid, \
                                          mResetDescriptorValid, \
                                          mPortActive, \
                                          mMasterSlaveAPU, \
                                          mDieNumber, \
                                          mAlternateSlotNumber, \
                                          mPrimarySecondaryLink) \
  { mHotPlugFormat, 1, mGpioDescriptorValid, mResetDescriptorValid, mPortActive, mMasterSlaveAPU, mDieNumber, \
    0, mAlternateSlotNumber, mPrimarySecondaryLink, 0 },

#define  PCIE_HOTPLUG_INITIALIZER_FUNCTION(mI2CGpioBitSelector, \
                                           mI2CGpioByteMapping, \
                                           mAddrExt, \
                                           mI2CGpioDeviceMapping, \
                                           mI2CDeviceType, \
                                           mI2CBusSegment, \
                                           mFunctionMask) \
  { mI2CGpioBitSelector, 0, mI2CGpioByteMapping, mAddrExt, mI2CGpioDeviceMapping, mI2CDeviceType, mI2CBusSegment & 7, \
    mFunctionMask, 0, 0, mI2CBusSegment >> 3 },

#define  PCIE_HOTPLUG_INITIALIZER_RESET(mI2CGpioByteMapping, \
                                        mI2CGpioDeviceMapping, \
                                        mI2CDeviceType, \
                                        mI2CBusSegment, \
                                        mResetSelect) \
  { 0, mI2CGpioByteMapping, 0, mI2CGpioDeviceMapping, mI2CDeviceType, mI2CBusSegment, mResetSelect, 0, 0 },

#define  PCIE_HOTPLUG_INITIALIZER_NO_RESET()  { 0, 0, 0, 0, 0, 0, 0, 0, 0 },

#define  PCIE_HOTPLUG_INITIALIZER_GPIO(mI2CGpioByteMapping, \
                                       mI2CGpioDeviceMapping, \
                                       mI2CDeviceType, \
                                       mI2CBusSegment, \
                                       mGpioSelect, \
                                       mGpioInterruptEnable) \
  { 0, mI2CGpioByteMapping, 0, mI2CGpioDeviceMapping, mI2CDeviceType, mI2CBusSegment, mGpioSelect, \
    mGpioInterruptEnable }

#define  PCIE_HOTPLUG_INITIALIZER_NO_GPIO()  { 0, 0, 0, 0, 0, 0, 0, 0 }

// bitmasks for GPIO pins based on hotplug format

#define PCIE_HOTPLUG_EXPRESSMODULE_PRESENT_  0x01
#define PCIE_HOTPLUG_EXPRESSMODULE_PWRFLT_   0x02
#define PCIE_HOTPLUG_EXPRESSMODULE_ATNSW_    0x04
#define PCIE_HOTPLUG_EXPRESSMODULE_EMILS     0x08
#define PCIE_HOTPLUG_EXPRESSMODULE_PWREN_    0x10
#define PCIE_HOTPLUG_EXPRESSMODULE_ATNLED    0x20
#define PCIE_HOTPLUG_EXPRESSMODULE_PWRLED    0x40
#define PCIE_HOTPLUG_EXPRESSMODULE_EMIL      0x80

#define PCIE_HOTPLUG_EXPRESSMODULE_B_ATNLED    0x01
#define PCIE_HOTPLUG_EXPRESSMODULE_B_PWRLED    0x02
#define PCIE_HOTPLUG_EXPRESSMODULE_B_PWREN_    0x04
#define PCIE_HOTPLUG_EXPRESSMODULE_B_ATNSW_    0x08
#define PCIE_HOTPLUG_EXPRESSMODULE_B_PRESENT_  0x10
#define PCIE_HOTPLUG_EXPRESSMODULE_B_PWRFLT_   0x20
#define PCIE_HOTPLUG_EXPRESSMODULE_B_EMILS     0x40
#define PCIE_HOTPLUG_EXPRESSMODULE_B_EMIL      0x80

#define PCIE_HOTPLUG_EXPRESS_SSD_PRESENT_  0x10
#define PCIE_HOTPLUG_EXPRESS_SSD_IFDET_    0x20
#define PCIE_HOTPLUG_EXPRESS_SSD_DUALEN_   0x40

#endif // _HGPI_H_
