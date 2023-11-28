/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM structures and definitions
 *
 * Contains AMD CPM Common Interface
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  Include
 * @e \$Revision$   @e \$Date$
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

#ifndef _AMD_CPM_COMMON_H_
#define _AMD_CPM_COMMON_H_

#pragma pack(push)

#include <AGESA.h>
#include <AmdPcieComplex.h>
#include <AmdCpmPlatform.h>
#include <AmdCpmDefine.h>
#include <AmdCpmFunction.h>
#include <AmdCpmPostCode.h>
#include <Token.h>
// #define AMD_CPM_KERNEL_VERSION                 0xnnnm
// CPM Kernel Main Version  = nnn
// CPM Kernel Sub Version   = m
#define AMD_CPM_KERNEL_VERSION                    0x0610

#ifndef AMD_MAX_CPU_CORE_NUMBERS
  #define AMD_MAX_CPU_CORE_NUMBERS                0x8
#endif

/// The signatures of CPM table

typedef enum {
  CPM_SIGNATURE_MAIN_TABLE =                  'MPC$',         ///< The signature of AMD_CPM_MAIN_TABLE

  CPM_SIGNATURE_GET_PLATFORM_ID =             '00A$',         ///< The signature of AMD_CPM_PLATFORM_ID_TABLE
  CPM_SIGNATURE_GET_PLATFORM_ID_CONVERT =     '10A$',         ///< The signature of AMD_CPM_PLATFORM_ID_CONVERT_TABLE
  CPM_SIGNATURE_PRE_INIT =                    '20A$',         ///< The signature of AMD_CPM_PRE_INIT_TABLE
  CPM_SIGNATURE_INIT_FLAG =                   '40A$',         ///< The signature of AMD_CPM_INIT_FLAG_TABLE
  CPM_SIGNATURE_GET_PLATFORM_ID2 =            '50A$',         ///< The signature of AMD_CPM_PLATFORM_ID_TABLE2
  CPM_SIGNATURE_CORE_TOPOLOGY =               '70A$',         ///< The signature of AMD_CPM_CORE_TOPOLOGY_TABLE

  CPM_SIGNATURE_GPIO_INIT =                   '01A$',         ///< The signature of AMD_CPM_GPIO_INIT_TABLE
  CPM_SIGNATURE_GEVENT_INIT =                 '11A$',         ///< The signature of AMD_CPM_GEVENT_INIT_TABLE
  CPM_SIGNATURE_GPIO_DEVICE_CONFIG =          '21A$',         ///< The signature of AMD_CPM_GPIO_DEVICE_CONFIG_TABLE
  CPM_SIGNATURE_GPIO_DEVICE_POWER =           '31A$',         ///< The signature of AMD_CPM_GPIO_DEVICE_POWER_TABLE
  CPM_SIGNATURE_GPIO_DEVICE_DETECTION =       '41A$',         ///< The signature of AMD_CPM_GPIO_DEVICE_DETECTION_TABLE
  CPM_SIGNATURE_GPIO_DEVICE_RESET =           '51A$',         ///< The signature of AMD_CPM_GPIO_DEVICE_RESET_TABLE
  CPM_SIGNATURE_SET_MEM_VOLTAGE =             '61A$',         ///< The signature of AMD_CPM_GPIO_MEM_VOLTAGE_TABLE
  CPM_SIGNATURE_PCIE_CLOCK =                  '71A$',         ///< The signature of AMD_CPM_PCIE_CLOCK_TABLE
  CPM_SIGNATURE_EXT_CLKGEN =                  '81A$',         ///< The signature of AMD_CPM_EXT_CLKGEN_TABLE
  CPM_SIGNATURE_GPIO_PRE_INIT =               '91A$',         ///< The signature of AMD_CPM_GPIO_PRE_INIT_TABLE
  CPM_SIGNATURE_PT_GPIO_INIT =                '92A$',         ///< The signature of AMD_CPM_PT_GPIO_PRE_INIT_TABLE

  CPM_SIGNATURE_PCIE_TOPOLOGY =               '02A$',         ///< The signature of AMD_CPM_PCIE_TOPOLOGY_TABLE
  CPM_SIGNATURE_PCIE_TOPOLOGY_OVERRIDE =      '12A$',         ///< The signature of AMD_CPM_PCIE_TOPOLOGY_OVERRIDE_TABLE
  CPM_SIGNATURE_PCIE_EXPRESS_CARD =           '22A$',         ///< The signature of AMD_CPM_EXPRESS_CARD_TABLE
  CPM_SIGNATURE_SET_VDDP_VDDR_VOLTAGE =       '42A$',         ///< The signature of AMD_CPM_GPIO_VDDP_VDDR_VOLTAGE_TABLE
  CPM_SIGNATURE_PCIE_OTHER_HOTPLUG_CARD =     '52A$',         ///< The signature of AMD_CPM_OTHER_HOTPLUG_CARD_TABLE
  CPM_SIGNATURE_DXIO_TOPOLOGY =               '62A$',         ///< The signature of Socket 0 AMD_CPM_DXIO_TOPOLOGY_TABLE
  EPRO_CPM_SIGNATURE_DXIO_TOPOLOGY =          '63A$',         ///< The signature of Socket 0 HYGON_CPM_DXIO_TOPOLOGY_TABLE 
  CPM_SIGNATURE_DXIO_TOPOLOGY_S1 =            '72A$',         ///< The signature of Socket 1 AMD_CPM_DXIO_TOPOLOGY_TABLE

  CPM_SIGNATURE_LPC_UART =                    '06A$',         ///< The signature of AMD_CPM_LPC_UART_TABLE

} AMD_CPM_TABLE_SIGNATURE;


/// The value of special clock id

typedef enum {
  CPM_CLKID_APU_CLK =                         0x80,           ///< APU_CLK Power Down Enable
  CPM_CLKID_DISP2_CLK =                       0x81,           ///< DISP2_CLK Power Down Enable
  CPM_CLKID_PCIE_RCLK_OUTPUT =                0x82,           ///< PCIE_RCLK_Output Power Down Enable
  CPM_CLKID_DISP_CLK =                        0x83,           ///< DISP_CLK Power Down Enable
  CPM_CLKID_PCIE_RCLK =                       0x84,           ///< PCIE_RCLK Power Down Enable
  CPM_CLKID_CLOCK_BUFFER_BIAS =               0x85,           ///< Clock Buffer Bias Power Down Enable
  CPM_CLKID_OSCOUT2_OUTOFF =                  0x86,           ///< OSCOUT2 Power Down Enable
} AMD_CPM_CLOCK_ID;


/// The value of Boot Mode

typedef enum {
  CPM_BOOT_MODE_S0 =                          0x00,           ///< BOOT ON S0
  CPM_BOOT_MODE_S1 =                          0x01,           ///< BOOT ON S1 RESUME
  CPM_BOOT_MODE_S3 =                          0x03,           ///< BOOT ON S3 RESUME
  CPM_BOOT_MODE_S4 =                          0x04,           ///< BOOT ON S4 RESUME
  CPM_BOOT_MODE_S5 =                          0x05,           ///< BOOT ON S5 RESUME
} AMD_CPM_BOOT_MODE;

/// The value of CPU Revision ID

typedef enum {
  CPM_CPU_REVISION_ID_TN =                    0x00,           ///< CPU Revision ID for TN
  CPM_CPU_REVISION_ID_ON,                                     ///< CPU Revision ID for ON
  CPM_CPU_REVISION_ID_KV,                                     ///< CPU Revision ID for KV
  CPM_CPU_REVISION_ID_KB,                                     ///< CPU Revision ID for KB
  CPM_CPU_REVISION_ID_ML,                                     ///< CPU Revision ID for ML
  CPM_CPU_REVISION_ID_CZ,                                     ///< CPU Revision ID for CZ
  CPM_CPU_REVISION_ID_NL,                                     ///< CPU Revision ID for NL
  CPM_CPU_REVISION_ID_AM,                                     ///< CPU Revision ID for AM
  CPM_CPU_REVISION_ID_ST,                                     ///< CPU Revision ID for ST
  CPM_CPU_REVISION_ID_BR,                                     ///< CPU Revision ID for BR
  CPM_CPU_REVISION_ID_ZP,                                     ///< CPU Revision ID for ZP
  CPM_CPU_REVISION_ID_HYGON_ZP,                               ///< CPU Revision ID for HYGON ZP PS-473
} AMD_CPM_CPU_REVISION_ID;

/// The value of PCIE Revision ID

typedef enum {
  CPM_PCIE_REVISION_ID_TN =                   0x00,           ///< PCIE Revision ID for TN
  CPM_PCIE_REVISION_ID_ON,                                    ///< PCIE Revision ID for ON
  CPM_PCIE_REVISION_ID_KV,                                    ///< PCIE Revision ID for KV
  CPM_PCIE_REVISION_ID_KB,                                    ///< PCIE Revision ID for KB
  CPM_PCIE_REVISION_ID_ML,                                    ///< PCIE Revision ID for ML
  CPM_PCIE_REVISION_ID_CZ,                                    ///< PCIE Revision ID for CZ
  CPM_PCIE_REVISION_ID_NL,                                    ///< PCIE Revision ID for NL
  CPM_PCIE_REVISION_ID_AM,                                    ///< PCIE Revision ID for AM
  CPM_PCIE_REVISION_ID_ST,                                    ///< PCIE Revision ID for ST
  CPM_PCIE_REVISION_ID_BR,                                    ///< PCIE Revision ID for BR
  CPM_PCIE_REVISION_ID_ZP,                                    ///< PCIE Revision ID for ZP
  CPM_PCIE_REVISION_ID_HYGON_ZP,                              ///< PCIE Revision ID for HYGON ZP PS-473
} AMD_CPM_PCIE_REVISION_ID;

/// The value of CPU Revision ID

typedef enum {
  CPM_FCH_REVISION_ID_DEFAULT =               0x00,           ///< FCH Revision ID for Default
  CPM_FCH_REVISION_ID_KB =                    0x01,           ///< FCH Revision ID for KB
  CPM_FCH_REVISION_ID_ML =                    0x02,           ///< FCH Revision ID for ML
  CPM_FCH_REVISION_ID_CZ =                    0x03,           ///< FCH Revision ID for CZ
  CPM_FCH_REVISION_ID_NL =                    0x04,           ///< FCH Revision ID for NL
  CPM_FCH_REVISION_ID_AM =                    0x05,           ///< FCH Revision ID for AM
  CPM_FCH_REVISION_ID_ST =                    0x06,           ///< FCH Revision ID for ST
  CPM_FCH_REVISION_ID_BR =                    0x07,           ///< FCH Revision ID for BR
  CPM_FCH_REVISION_ID_ZP =                    0x08,           ///< FCH Revision ID for ZP
  CPM_FCH_REVISION_ID_RV =                    0x09,           ///< FCH Revision ID for RV
} AMD_CPM_FCH_REVISION_ID;

/// The value of the stage to load AMD CPM Pre Init Table

typedef enum {
  CPM_PRE_INIT_STAGE_0 =                      0,              ///< Stage 0 to load Pre Init Table
  CPM_PRE_INIT_STAGE_1 =                      1,              ///< Stage 1 to load Pre Init Table
} AMD_CPM_PRE_INIT_STAGE;

/// Configuration values for CPM table attribute

typedef enum {
  CPM_PEI =                                   BIT0,           ///< Used by CPM PEI driver
  CPM_DXE =                                   BIT1,           ///< Used by CPM DXE driver
  CPM_SMM =                                   BIT2,           ///< Used by CPM SMM driver
  CPM_OVERRIDE =                              BIT3,           ///< Modified by CPM driver
} AMD_CPM_TABLE_ATTRIBUTE;


/// CPM table header

typedef struct {
  UINT32                          TableSignature;             ///< Signature of CPM table
  UINT16                          TableSize;                  ///< Table size
  UINT8                           FormatRevision;             ///< Revision of table format
  UINT8                           ContentRevision;            ///< Revision of table contect
  UINT32                          PlatformMask;               ///< The mask of platform table supports
  UINT32                          Attribute;                  ///< Table attribute
} AMD_CPM_TABLE_COMMON_HEADER;


/// Table pointer

typedef union {
  VOID                            *Pointer;                   ///< Table pointer
  UINT64                          Raw;                        ///< Table pointer value
} AMD_CPM_POINTER;


/// Device and function number of PCI device

typedef struct {
  UINT8                           Device:5;                 ///< PCI Device Number
  UINT8                           Function:3;               ///< PCI Function Number
} AMD_CPM_PCI_DEVICE_FUNCTION;


/// Item of CPM table list

typedef struct {
  UINT32                          TableId;                    ///< Signature of table
  UINT8                           Flag;                       ///< Location of table. 0: ROM. 1: RAM
  AMD_CPM_POINTER                 SubTable;                   ///< Pointer of table
} AMD_CPM_TABLE_ITEM;


/// CPM table list

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                         ///< Table header
  UINT32                          Number;                         ///< Table number to be recorded in the list
  UINT32                          Size;                           ///< Table size
  AMD_CPM_TABLE_ITEM              Item[AMD_TABLE_LIST_ITEM_SIZE]; ///< The array of CPM table
} AMD_CPM_TABLE_LIST;


/// GPIO pin list for platform Id

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                               ///< Table header
  UINT16                          GpioPin[AMD_PLATFORM_ID_TABLE_SIZE];  ///< The array of GPIO pin for platform Id
} AMD_CPM_PLATFORM_ID_TABLE;


/// SMBUS GPIO Pin for Platform ID

typedef struct _AMD_CPM_SMBUS_PLATFORM_ID {
  UINT8                           SmbusSelect;                          ///< SMBUS Number
  UINT8                           SmbusAddress;                         ///< SMBUS Address
  UINT8                           SmbusOffset;                          ///< SMBUS Offset
  UINT8                           SmbusBit;                             ///< SMBUS Bit
} AMD_CPM_SMBUS_PLATFORM_ID;


/// EEPROM for platform Id

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                               ///< Table header
  AMD_CPM_SMBUS_PLATFORM_ID       GpioPin[AMD_PLATFORM_ID_TABLE_SIZE];  ///< SMBUS GPIO pin for Platform ID
} AMD_CPM_PLATFORM_ID_TABLE2;


/// Platform Id converting item

typedef struct {
  UINT8                           CpuRevisionId;              ///< CPU Revision ID
  UINT16                          OriginalIdMask;             ///< Platform Id mask from platform Id table
  UINT16                          OriginalId;                 ///< Platform Id from platform Id table
  UINT16                          ConvertedId;                ///< Platform Id which is used to check whether
                                                              ///<   CPM table supports current platform
} AMD_CPM_PLATFORM_ID_CONVERT_ITEM;


/// Platform Id mapping table

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER         Header;                                   ///< Table Header
  AMD_CPM_PLATFORM_ID_CONVERT_ITEM    Item[AMD_PLATFORM_ID_CONVERT_TABLE_SIZE]; ///< Platform Id mapping list
} AMD_CPM_PLATFORM_ID_CONVERT_TABLE;


/// The table definition for early initialization

typedef struct {
  UINT8                           Type;                       ///< Register type. 0: FCH MMIO. 1: PCI
  UINT8                           Select;                     ///< Register sub-type
  UINT8                           Offset;                     ///< Register offset
  UINT8                           AndMask;                    ///< AND mask
  UINT8                           OrMask;                     ///< OR mask
  UINT8                           Stage;                      ///< Stage number
} AMD_CPM_PRE_SETTING_ITEM;


/// Register table to be initialized in the earliest stage
typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                     ///< Table header
  AMD_CPM_PRE_SETTING_ITEM        Item[AMD_PRE_INIT_SIZE];    ///< Register setting
} AMD_CPM_PRE_INIT_TABLE;

/// The table definition for each CPU Die information
typedef struct {
  UINT8                           Socket;                     ///< CPU Socket Number.
  UINT8                           Die;                        ///< CPU Die Number.
  UINT8                           Bus;                        ///< CPU Bus Number.
} AMD_CPM_CORE_TOPOLOGY_LIST;

/// CPU Core Topology Table
typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                                     ///< Table header
  AMD_CPM_CORE_TOPOLOGY_LIST      CoreList[AMD_CPM_CORE_TOPOLOGY_SIZE];       ///< CPU Core Topology List
} AMD_CPM_CORE_TOPOLOGY_TABLE;

//
// The table definition for GPIO
//
//  AMD_CPM_GPIO_INIT_TABLE
//  AMD_CPM_GEVENT_INIT_TABLE
//  AMD_CPM_GPIO_DEVICE_CONFIG_TABLE
//  AMD_CPM_GPIO_DEVICE_POWER_TABLE
//  AMD_CPM_GPIO_DEVICE_DETECTION_TABLE
//  AMD_CPM_GPIO_DEVICE_RESET_TABLE
//


/// GPIO Pin

typedef struct {
  UINT16                          Pin;                        ///< GPIO pin number
  UINT8                           Value;                      ///< GPIO pin value
} AMD_CPM_GPIO_PIN;


/// Defintion of GPIO setting

typedef union {
  UINT16                          Raw;                        ///< GPIO setting value
  struct {                                                    ///<
    UINT16                        Out:1;                      ///< Output state
    UINT16                        OutEnB:1;                   ///< Output enable
    UINT16                        PullUpSel:1;                ///< Pull up select: 0: 4K. 1: 8K
    UINT16                        SetEnB:1;                   ///< Gate of Out and OutEnB
    UINT16                        Sticky:1;                   ///< Sticky enable
    UINT16                        PullUp:1;                   ///< Pull up enable
    UINT16                        PullDown:1;                 ///< Pull down enable
    UINT16                        PresetEn:1;                 ///< Gate of Sticky, PullUp & PullDown
    UINT16                        IoMux:3;                    ///< Multi-function IO pin function select of GPIO
    UINT16                        IoMuxEn:1;                  ///< Gate of IoMux
    UINT16                        DrvStrengthSel:2;           ///< Drive Strength Select: 0: 4mA. 1: 8mA. 2: 12mA. 3: 16mA
    UINT16                        Reserved2:2;                ///< Reserved
  }                               Gpio;                       ///< Bit mapping for GPIO setting
} AMD_CPM_GPIO_SETTING;


/// GPIO setting item
typedef struct {
  UINT16                          Pin;                        ///< GPIO pin number
  AMD_CPM_GPIO_SETTING            Setting;                    ///< GPIO setting
} AMD_CPM_GPIO_ITEM;


/// GPIO init table

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                       ///< Table header
  AMD_CPM_GPIO_ITEM               GpioList[AMD_GPIO_ITEM_SIZE]; ///< GPIO setting list
} AMD_CPM_GPIO_INIT_TABLE;

/// PT GPIO function

typedef
EFI_STATUS
(EFIAPI *PTInitialGpioPei) (
  IN       EFI_PEI_SERVICES       **PeiServices,
  IN       VOID                   *PTGpioPtr
  );

/// Write GPIO function

typedef
EFI_STATUS
(EFIAPI *PTWriteGpioPei) (
  IN       EFI_PEI_SERVICES       **PeiServices,
  IN       UINT8                  Pin,
  IN       UINT8                  Value
  );

/// Read GPIO function

typedef
EFI_STATUS
(EFIAPI *PTReadGpioPei) (
  IN       EFI_PEI_SERVICES       **PeiServices,
  IN       UINT8                  Pin,
  OUT      UINT8                  Value
  );

/// Module data structure

typedef struct _EFI_PT_GPIO_PPI  {
  PTInitialGpioPei                PTInitiialGpio;              ///< PTInitiialGpio
  PTWriteGpioPei                  PTWriteGpio;                 ///< PTWriteGpio;
  PTReadGpioPei                   PTReadGpio;                  ///< PTReadGpio;
} EFI_PT_GPIO_PPI;

/// Promontory GPIO pin 0 to 7

#define Promontory_GPIO_ITEM_SIZE 0x9

/// Defintion of Promontory GPIO common setting

typedef union {
  UINT16                          Raw;                        ///< GPIO setting value
  struct {                                                    ///<
    UINT16                        GpioDeBounceTimer:3;        ///< GPIO sampling rate of the de-bounce timer
    UINT16                        GpioDeBounceTimeoutTh:3;    ///< GPIO De-bounce timer timeout threshold
    UINT16                        GpioInterruptOutputEn:1;    ///< GPIO interrupt signal output enable
    UINT16                        GpioInterruptActLevel:1;    ///< GPIO interrupt signal active level
    UINT16                        GPIOInterruptMode:1;        ///< GPIO interrupt mode
    UINT16                        Reserved2:7;                ///< Reserved
  }                               Common;                     ///< Bit mapping for Common setting
} Promontory_GPIO_COMMON_SETTING;

/// Defintion of Promontory GPIO pin 0 to 7 setting

typedef union {
  UINT16                          Raw;                        ///< GPIO setting value
  struct {                                                    ///<
    UINT16                        OutEnB:1;                   ///< Output enable
    UINT16                        Out:1;                      ///< Output control if Output enable
    UINT16                        interruptEnable:1;          ///< GPIO pin interrupt enable
    UINT16                        interruptLevelTrigType:1;   ///< GPIO level trigger type
    UINT16                        interruptType:2;            ///< GPIO pin interrupt type
    UINT16                        interruptMask:1;            ///< GPIO pin interrupt mask
    UINT16                        Reserved2:9;                ///< Reserved
  }                               Gpio;                       ///< Bit mapping for GPIO setting
} Promontory_GPIO_SETTING;

/// Promontory GPIO setting item

typedef struct {
  UINT16                          Pin;                        ///< GPIO pin number
  Promontory_GPIO_SETTING         Setting;                    ///< GPIO setting
} Promontory_GPIO_ITEM;

/// Promontory GPIO init table

typedef struct {
  Promontory_GPIO_COMMON_SETTING  GpioCommon;                          ///< GPIO common setting
  Promontory_GPIO_ITEM            GpioList[Promontory_GPIO_ITEM_SIZE]; ///< GPIO setting list
} Promontory_GPIO_INIT_TABLE;

/// PT GPIO init table

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                              ///< Table header
  Promontory_GPIO_COMMON_SETTING  GpioCommon;                          ///< GPIO common setting
  Promontory_GPIO_ITEM            GpioList[Promontory_GPIO_ITEM_SIZE]; ///< GPIO setting list
} AMD_CPM_PT_GPIO_INIT_TABLE;

/// The definition of GEVENT setting

typedef union {
  UINT16                          Raw;                        ///< GEVENT setting value
  struct {                                                    ///<
    UINT16                        EventEnable:1;              ///< EventEnable. 0: Disable, 1: Enable
    UINT16                        SciTrig:1;                  ///< SciTrig. 0: Falling Edge, 1: Rising Edge
    UINT16                        SciLevl:1;                  ///< SciLevl.0 trigger mode. 0: Edge trigger, 1: Level Trigger
    UINT16                        SmiSciEn:1;                 ///< SmiSciEn. 0: Not send SMI, 1: Send SMI
    UINT16                        SciS0En:1;                  ///< SciS0En. 0: Disable, 1: Enable
    UINT16                        SciMap:5;                   ///< SciMap. 0000b->1111b
    UINT16                        SciTrigAuto:1;              ///< SciTrigAuto. 1: Disable. 0: Enable.
    UINT16                        SmiTrig:1;                  ///< SmiTrig. 0: Active Low, 1: Active High
    UINT16                        SmiControl:4;               ///< SmiControl. 0: Disable, 1: SMI 2: NMI 3: IRQ13
  }                               Gevent;                     ///< Bit mapping for GEVENT setting
  struct {                                                    ///<
    UINT16                        DebounceTmrOut:4;           ///< Specifies the debounce timer out number
    UINT16                        DebounceTmrOutUnit:1;       ///< 0: 30.5us (One RtcClk period), 1: 122us (four RtcClk periods)
    UINT16                        DebounceCntrl:2;            ///< 00b: No debounce, 01b: Preserve low glitch
                                                              ///< 10b: Preserve high glitch, 11b: Remove glitch
    UINT16                        Reserved:1;                 ///< Reserved
    UINT16                        LevelTrig:1;                ///< 0: Edge trigger, 1: Level trigger
    UINT16                        ActiveLevel:2;              ///< 00b: Active High. 01b: Active Low. 10b: Active on both edges if LevelTrig=0
    UINT16                        InterruptEnable:2;          ///< [0]: Enable interrupt status, [1]: Enable interrupt delivery
    UINT16                        WakeCntrl:3;                ///< [0]: Enable wake in S0I3 state, [1]: Enable wake in S3 state, [2]: Enable wake in S4/S5 state
  }                               Gpio;                       ///< Bit mapping for GPIO interrupt setting
} AMD_CPM_GEVENT_SETTING;


/// GEVENT setting item

typedef struct {
  UINT16                          Pin;                        ///< GEVENT pin number
  AMD_CPM_GEVENT_SETTING          Setting;                    ///< GEVENT setting
} AMD_CPM_GEVENT_ITEM;


/// GEVENT init table

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                           ///< Table header
  AMD_CPM_GEVENT_ITEM             GeventList[AMD_GEVENT_ITEM_SIZE]; ///< GEVENT setting list
} AMD_CPM_GEVENT_INIT_TABLE;


/// Configuration of Device which is controlled by GPIO pin

typedef struct {
  UINT8                           DeviceId;           ///< Device Id
  union {
    UINT8                         Raw;                ///< Device Config Value
    struct {
      UINT8                       Enable:2;           ///< Flag of power state. 0: Disable. 1: Enable. 2: Auto Detection
      UINT8                       ResetAssert:1;      ///< Flag to assert reset pin
      UINT8                       ResetDeassert:1;    ///< Flag to de-assert reset pin
      UINT8                       Reserved:4;         ///< Reserved
    }                             Setting;            ///< Bit mapping of Device Config
  }                               Config;             ///< Device Config
} AMD_CPM_GPIO_DEVICE_CONFIG;


/// Device config table

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                           ///< Table header
  AMD_CPM_GPIO_DEVICE_CONFIG      DeviceList[AMD_GPIO_DEVICE_SIZE]; ///< Device config list
} AMD_CPM_GPIO_DEVICE_CONFIG_TABLE;


/// Device detection method

typedef struct {
  UINT8                           DeviceId;           ///< Device Id
  UINT8                           Type;               ///< Detection type. 0: One GPIO pin. 1: Two GPIO pins. 2: Special Pin
  UINT16                          PinNum1;            ///< Pin number of GPIO 1
  UINT8                           Value1;             ///< Value of GPIO 1
  UINT16                          PinNum2;            ///< Pin number of GPIO 2
  UINT8                           Value2;             ///< Value of GPIO 2
  UINT16                          PinNum3;            ///< Pin number of GPIO 3
  UINT8                           Value3;             ///< Value of GPIO 3
} AMD_CPM_GPIO_DEVICE_DETECTION;


/// Device Detection Table

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                                           ///< Table header
  AMD_CPM_GPIO_DEVICE_DETECTION   DeviceDetectionList[AMD_GPIO_DEVICE_DETECT_SIZE]; ///< Device Detection List
} AMD_CPM_GPIO_DEVICE_DETECTION_TABLE;


/// Device Reset Table

typedef struct {
  UINT8                           DeviceId;           ///< Device Id
  UINT8                           Mode;               ///< Reset mode
                                                      ///<   0: Reset Assert.
                                                      ///<   1: Reset De-assert
                                                      ///<   2: Delay between Assert and Deassert
  UINT8                           Type;               ///< Register type
                                                      ///<   0: GPIO.
                                                      ///<   1: Special pin.if Mode = 0 or 1
  union {
    UINT32                        Stall;              ///< Delay
    AMD_CPM_GPIO_PIN              Gpio;               ///< GPIO pin
  }                               Config;             ///< Setting
  UINT8                           InitFlag;           ///< Init flag in post
} AMD_CPM_GPIO_DEVICE_RESET;


/// Configuration values for CPM GPIO Device Init Flag

typedef enum {
  GPIO_DEVICE_INIT_DISABLE =      0,                  ///< GPIO Device does not need to be initialized or is controlled by GPIO Device COnfig
  GPIO_DEVICE_INIT_STAGE_1 =      1,                  ///< GPIO Device needs to be initialized on stage 1
  GPIO_DEVICE_INIT_STAGE_2 =      2,                  ///< GPIO Device needs to be initialized on stage 2
  GPIO_DEVICE_INIT_STAGE_DUMMY =  3,                  ///< GPIO Device does not need to be initialized in BIOS post
  GPIO_DEVICE_INIT_STAGE_3 =      4,                  ///< GPIO Device needs to be initialized from S0 to S3/S4/S5
  GPIO_DEVICE_INIT_STAGE_4 =      5,                  ///< GPIO Device needs to be initialized from S0 to S3
  GPIO_DEVICE_INIT_STAGE_5 =      6,                  ///< GPIO Device needs to be initialized from S0 to S4
  GPIO_DEVICE_INIT_STAGE_6 =      7,                  ///< GPIO Device needs to be initialized from S0 to S5
} AMD_CPM_GPIO_DEVICE_INIT_FLAG;


/// Device Reset Table

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                                       ///< Table header
  AMD_CPM_GPIO_DEVICE_RESET       DeviceResetList[AMD_GPIO_DEVICE_RESET_SIZE];  ///< Device reset list
} AMD_CPM_GPIO_DEVICE_RESET_TABLE;

/// Item of device power on / off sequence

typedef struct {
  UINT8                           DeviceId;           ///< Device Id
  UINT8                           Mode;               ///< Device Power Mode. 1: Power On. 0: Power Off
  UINT8                           Type;               ///< Device Power Item. 0: Set GPIO. 1: Wait GPIO. 2: Add Delay
  union {
    UINT32                        Stall;              ///< Delay
    AMD_CPM_GPIO_PIN              SetGpio;            ///< Set GPIO pin
    AMD_CPM_GPIO_PIN              WaitGpio;           ///< Wait for GPIO pin to some value
  }                               Config;             ///< Dvice Power Item Setting
  UINT8                           InitFlag;           ///< Init flag in post
} AMD_CPM_GPIO_DEVICE_POWER;

/// GPIO Device Init Flag Table

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                                       ///< Table header
  UINT8                           PowerInitFlag[AMD_GPIO_DEVICE_POWER_SIZE];    ///< Init Flag for Power Sequence
  UINT8                           ResetInitFlag[AMD_GPIO_DEVICE_RESET_SIZE];    ///< Init Flag for Reset Sequence
} AMD_CPM_INIT_FLAG_TABLE;

/// Device Power Sequence Table

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                                       ///< Table header
  AMD_CPM_GPIO_DEVICE_POWER       DevicePowerList[AMD_GPIO_DEVICE_POWER_SIZE];  ///< Device power sequence list
} AMD_CPM_GPIO_DEVICE_POWER_TABLE;


/// GPIO setting for one memory voltage (VDDIO)

typedef struct {
  UINT8                           Voltage;            ///< DDR3Voltage
  UINT16                          GpioPin1;           ///< GPIO pin 1
  UINT8                           Value1;             ///< Value of GPIO pin 1
  UINT16                          GpioPin2;           ///< GPIO pin 2
  UINT8                           Value2;             ///< Value of GPIO pin 2
} AMD_CPM_GPIO_MEM_VOLTAGE_ITEM;


/// Memory voltage table

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                           ///< Table header
  AMD_CPM_GPIO_MEM_VOLTAGE_ITEM   Item[AMD_GPIO_MEM_VOLTAGE_SIZE];  ///< GPIO setting list for memory voltage
} AMD_CPM_GPIO_MEM_VOLTAGE_TABLE;


/// GPIO setting for VDDP/VDDR voltage

typedef struct {
  UINT8                           Voltage;            ///< VDDP/VDDR Voltage
  UINT16                          GpioPin1;           ///< GPIO pin
  UINT8                           Value1;             ///< Value of GPIO pin
} AMD_CPM_GPIO_VDDP_VDDR_VOLTAGE_ITEM;

/// VDDP/VDDR voltage table

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                           ///< Table header
  AMD_CPM_GPIO_VDDP_VDDR_VOLTAGE_ITEM Item[AMD_GPIO_VDDP_VDDR_VOLTAGE_SIZE];  ///< GPIO setting list for memory voltage
} AMD_CPM_GPIO_VDDP_VDDR_VOLTAGE_TABLE;


/// PCIe Clock Setting

typedef struct _AMD_CPM_PCIE_CLOCK_ITEM {
  UINT8                           ClkId;              ///< FCH PCIe Clock
  UINT8                           ClkReq;             ///< FCH PCIe ClkReq
  UINT8                           ClkIdExt;           ///< External Clock Source
  UINT8                           ClkReqExt;          ///< External ClkReq
  UINT8                           DeviceId;           ///< Device Id. No Device Id if 0xFF
  UINT8                           Device;             ///< Device Number of PCIe bridge
  UINT8                           Function;           ///< Function Number of PCIe bridge
  UINT8                           SlotCheck;          ///< Slot Check Flag:
                                                      ///<   BIT0: Check PCI Space
                                                      ///<   BIT1: Check GPIO pin
                                                      ///<   BIT2: Check Clock Power Management Enable
                                                      ///<   BIT3~6: Reserved
                                                      ///<   BIT7: Change PCIe Clock in ACPI method
  UINT32                          SpecialFunctionId;  ///< Id of Special Function
} AMD_CPM_PCIE_CLOCK_ITEM;

/// PCIe Clock Table

typedef struct _AMD_CPM_PCIE_CLOCK_TABLE {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                     ///< Table header
  AMD_CPM_PCIE_CLOCK_ITEM         Item[AMD_PCIE_CLOCK_SIZE];  ///< PCIe clock setting list
} AMD_CPM_PCIE_CLOCK_TABLE;


/// External ClkGen Register Setting Item

typedef struct _AMD_CPM_EXT_CLKGEN_ITEM {
  UINT8                           Function;           ///< External ClkGen Setting Item Type
                                                      ///<   0x00~0x7F: Initial Sequence Id
                                                      ///<   0x80~0x8F: Clock Disable Sequence
                                                      ///<   0x90~0x9F: ClkReq Enable Sequence
  UINT8                           Offset;             ///< Register Offset
  UINT8                           AndMask;            ///< AND Mask
  UINT8                           OrMask;             ///< Or Mask
} AMD_CPM_EXT_CLKGEN_ITEM;


/// External ClkGen Table

typedef struct _AMD_CPM_EXT_CLKGEN_TABLE {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                     ///< Table header
  UINT8                           SmbusSelect;                ///< SMBUS Number
  UINT8                           SmbusAddress;               ///< SMBUS Address
  AMD_CPM_EXT_CLKGEN_ITEM         Item[AMD_EXT_CLKGEN_SIZE];  ///< External ClkGen Register Setting List
} AMD_CPM_EXT_CLKGEN_TABLE;


/// PCIE Topology Table

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                               ///< Table header
  UINT32                          SocketId;                             ///< Socket Id
  PCIe_PORT_DESCRIPTOR            Port[AMD_PCIE_PORT_DESCRIPTOR_SIZE];  ///< PCIe Port Descriptor List
  PCIe_DDI_DESCRIPTOR             Ddi[AMD_PCIE_DDI_DESCRIPTOR_SIZE];    ///< PCIe DDI Descriptor List
} AMD_CPM_PCIE_TOPOLOGY_TABLE;


/// DXIO Topology Table

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                               ///< Table header
  UINT32                          SocketId;                             ///< Socket Id
  DXIO_PORT_DESCRIPTOR            Port[AMD_DXIO_PORT_DESCRIPTOR_SIZE];  ///< DXIO Port Descriptor List
} AMD_CPM_DXIO_TOPOLOGY_TABLE;


/// The override table definition for PCIE Topology

typedef struct {
  union {
    UINT8                         Raw;                        ///< Value of Flag
    struct {
      UINT8                       EnableOverride:1;           ///< Override Enable field of descriptor
      UINT8                       DdiTypeOverride:1;          ///< Override DDI type
      UINT8                       LaneOverride:1;             ///< Override StartLane and EndLane
      UINT8                       PortPresentOverride:1;      ///< Override PortPresent
      UINT8                       IsDdi:1;                    ///< This item is used to override Port or DDI descriptor
                                                              ///<   0: Port Descriptor
                                                              ///<   1: DDI Descriptor
      UINT8                       Reserved:2;                 ///< Reserved
      UINT8                       Valid:1;                    ///< Valid Flag
    }                             Config;                     ///< Bitmap of Flag
  }                               Flag;                       ///< Flag of PCIe Topology override item
  UINT8                           Offset;                     ///< Offset of Port Descriptor or DDI Dscriptor list
  UINT8                           Enable;                     ///< Descriptor Enable
  UINT8                           DdiType;                    ///< Ddi Type
  UINT8                           PortPresent;                ///< Port Present
  UINT8                           StartLane;                  ///< Start Lane
  UINT8                           EndLane;                    ///< End Lane
} AMD_CPM_PCIE_TOPOLOGY_OVERRIDE_ITEM;


/// PCIE Topology Override Table

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER         Header;                                 ///< Table header
  AMD_CPM_PCIE_TOPOLOGY_OVERRIDE_ITEM Item[AMD_PCIE_TOPOLOGY_OVERRIDE_SIZE];  ///< Override Item List
} AMD_CPM_PCIE_TOPOLOGY_OVERRIDE_TABLE;


/// Express Card Table

typedef struct _AMD_CPM_EXPRESS_CARD_TABLE {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                         ///< Table header
  UINT8                           Device;                         ///< Device Number of PCIe Bridge
  UINT8                           Function;                       ///< Function Number of PCIe Bridge
  UINT8                           EventPin;                       ///< GEVENT Pin
  UINT8                           DeviceId;                       ///< Device Id
} AMD_CPM_EXPRESS_CARD_TABLE;

/// Other Hotplug Card Table

typedef struct _AMD_CPM_OTHER_HOTPLUG_CARD_TABLE {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                         ///< Table header
  UINT8                           Number;                         ///< Card Number: 0 ~ 2
  UINT8                           Device0;                        ///< Device Number of PCIe Bridge for Card 0
  UINT8                           Function0;                      ///< Function Number of PCIe Bridge for Card 0
  UINT8                           EventPin0;                      ///< GEVENT Pin for Card 0
  UINT8                           DeviceId0;                      ///< Device Id for Card 0
  UINT8                           Device1;                        ///< Device Number of PCIe Bridge for Card 1
  UINT8                           Function1;                      ///< Function Number of PCIe Bridge for Card 1
  UINT8                           EventPin1;                      ///< GEVENT Pin  for Card 1
  UINT8                           DeviceId1;                      ///< Device Id  for Card 1
} AMD_CPM_OTHER_HOTPLUG_CARD_TABLE;

/// Convert from Device Id of SATA controller to SATA mode mask

typedef struct {
  UINT16                          DeviceId;                   ///< Device Id of SATA controller
  UINT8                           Mask;                       ///< SATA Mode Mask
} AMD_CPM_SATA_MODE_MASK;


/// The table definition for LPC UART

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                     ///< Table header
  UINT8                           ChipId;                     ///< Chip Id. 0: SMSC_1036
  UINT8                           LpcIndex;                   ///< Index of LPC register. 0: 2E/2F. 1: 4E/4F
  UINT16                          Address;                    ///< Uart base register
  UINT8                           Irq;                        ///< IRQ
} AMD_CPM_LPC_UART_TABLE;


/// Local SMI Status

typedef struct {
  UINT8                           ApicId;                             ///< Core apic id
  UINT32                          LocalSmiStatus;                     ///< SMMFEC4 Local SMI Status
} AMD_CPM_LOCAL_SMI_STATUS;


/// SMI Data

typedef struct {
  UINT32                          Signature;                          ///< Signature "$SCK"
  UINT8                           SmiCpuCnt;                          ///< Total number of cores
  AMD_CPM_LOCAL_SMI_STATUS        SmiSts[AMD_MAX_CPU_CORE_NUMBERS];   ///< X-Total number of cores. Holds "LocalSMIStatus" structure for each core.
} AMD_CPM_SMI_DATA;


/// AMD CPM Main Table

typedef struct {
  AMD_CPM_TABLE_COMMON_HEADER     Header;                         ///< Table header
  UINT8                           PlatformName[32];               ///< Platform name
  UINT8                           BiosType;                       ///< BIOS type
  UINT16                          CurrentPlatformId;              ///< Current Platform Id
  UINT32                          PcieMemIoBaseAddr;              ///< PcieMemIoBaseAddr
  UINT32                          AcpiMemIoBaseAddr;              ///< AcpiMemIoBaseAddr
  AMD_CPM_POINTER                 Service;                        ///< Reserved for internal use
  AMD_CPM_POINTER                 TableInRomList;                 ///< Reserved for internal use
  AMD_CPM_POINTER                 TableInRamList;                 ///< Reserved for internal use
  AMD_CPM_POINTER                 TableInHobList;                 ///< Reserved for internal use
  AMD_CPM_POINTER                 HobTablePtr;                    ///< Reserved for internal use

  UINT8                           ExtClkGen;                      ///< External ClkGen Config. 0x00~0x7F
  UINT8                           UnusedGppClkOffEn;              ///< Config to turn off unused GPP clock
  UINT8                           LpcUartEn;                      ///< LpcUartEn
} AMD_CPM_MAIN_TABLE;


/// Header of CPM Hob table

typedef struct {
  AMD_CPM_POINTER                 HeaderPtr;                  ///< Table header
  AMD_CPM_POINTER                 MainTablePtr;               ///< Pointer of CPM main table
  UINT32                          Revision;                   ///< CPM Revision
  UINT32                          BufferItem;                 ///< Available Table Number
  UINT32                          BufferOffset;               ///< Offset of Available Buffer
  UINT32                          BufferSize;                 ///< Size of Available Table
} AMD_CPM_HOB_HEADER;


/// Structure of PCI PFA
typedef union {
  UINT16                          Raw;                        ///< PCI Pfa value
  struct {                                                    ///<
    UINT8                         Function:3;                 ///< PCI Function Number
    UINT8                         Device:5;                   ///< PCI Device Number
    UINT8                         Bus;                        ///< PCI Bus Number
  }                               Pfa;                        ///< PCI Pfa
} AMD_CPM_PCI_PFA;

#pragma pack(push)
#pragma pack(1)

/// CPM NV Data Table
typedef struct {
  UINT32                          CpmVersion;                 ///< CPM Revision
  UINT32                          CpmPcieMmioBaseAddr;        ///< PcieMmioBaseAddress
  UINT32                          CpmAcpiMmioBaseAddr;        ///< AcpiMmioBaseAddress
  UINT8                           CpmSbChipId;                ///< SbChipId
  UINT8                           CpmSbStrap;                 ///< SbStrap
  UINT8                           CpmChipId[2];               ///< ChipId
  UINT8                           CpmEcRamGpioBaseOffset;     ///< EcRamGpioBaseOffset
  UINT16                          CpmSwSmiPort;               ///< SwSmiPort
  UINT8                           CpmSwSmiCmdSetAutoUmaMode;  ///< SwSmiCmd to set Auto UMA Allocation
  UINT32                          CpmUmaSize;                 ///< UMA Size

  UINT32                          CpmAtrmRomSize;             ///< VBIOS image size
  UINT8                           CpmAtrmRomImage[0x10000];   ///< VBIOS image

  UINT32                          CpmMainTable;               ///< Offset of AMD_CPM_MAIN_TABLE
  UINT32                          CpmDeviceDetectionTable;    ///< Offset of AMD_CPM_GPIO_DEVICE_DETECTION_TABLE
  UINT32                          CpmDeviceResetTable;        ///< Offset of AMD_CPM_GPIO_DEVICE_RESET_TABLE
  UINT32                          CpmDevicePowerTable;        ///< Offset of AMD_CPM_GPIO_DEVICE_POWER_TABLE
  UINT32                          CpmExpressCardTable;        ///< Offset of AMD_CPM_EXPRESS_CARD_TABLE
  UINT32                          CpmPcieClockTable;          ///< offset of AMD_CPM_PCIE_CLOCK_TABLE
  UINT32                          CpmOtherHotplugCardTable;   ///< offset of AMD_CPM_OTHER_HOTPLUG_CARD_TABLE
  UINT32                          CpmCoreTopologyTable;       ///< offset of AMD_CPM_CORE_TOPOLOGY_TABLE
} AMD_CPM_NV_DATA_STRUCT;

#pragma pack(pop)

/// Structure for FCH Strap Data

typedef struct {
  UINT32      ImcEnable:1;                                    ///< Imc status. 0: Disable, 1: Enable
  UINT32      InternalClock:1;                                ///< Internal clock status. 0: 25Mhz crystal clock, 1: 100Mhz PCI-E clock
  UINT32      S5PlusSupport:1;                                ///< S5+ support. 0: not support. 1: support.
} AMD_CPM_STRAP_SETTING;

/// Convert from PCIe device and function number to ASL name

typedef struct {
  UINT8     PcieRevision;     ///< PCIe bridge revision
  UINT8     Device;           ///< Device number of PCIe bridge
  UINT8     Function;         ///< Function number of PCIe bridge
  UINT8     NameId;           ///< ASL name id of PCIe bridge
  UINT32    Name;             ///< ASL name of PCIe bridge
} PCIE_BRIDGE_NAME;

/// Structure for CPU revision

typedef struct {
  UINT32    Mask;             ///< CPU Id Mask
  UINT32    Value;            ///< Value
  UINT8     CpuRevision;      ///< Cpu Revision
  UINT8     PcieRevision;     ///< PCIe bridge revision
} CPU_REVISION_ITEM;

/// Structure for Chip Id

typedef struct {
  UINT8     Cpu;              ///< CPU/APU Chip Id
  UINT8     Sb;               ///< SB Chip Id
  UINT8     Reserved[6];      ///<
} AMD_CPM_CHIP_ID;

#pragma pack(pop)

#endif
