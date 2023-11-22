/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPM structures and definitions
 *
 * Contains HYGON CPM Common Interface
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  Include
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

#ifndef _HYGON_CPM_COMMON_H_
#define _HYGON_CPM_COMMON_H_

#pragma pack(push)

#include <HGPI.h>
#include <HygonPcieComplex.h>
#include <HygonCpmPlatform.h>
#include <HygonCpmDefine.h>
#include <HygonCpmFunction.h>
#include <HygonCpmPostCode.h>

// #define HYGON_CPM_KERNEL_VERSION                 0xnnnm
// CPM Kernel Main Version  = nnn
// CPM Kernel Sub Version   = m
#define HYGON_CPM_KERNEL_VERSION  0x0610

#ifndef HYGON_MAX_CPU_CORE_NUMBERS
  #define HYGON_MAX_CPU_CORE_NUMBERS  0x8
#endif

/// The signatures of CPM table

typedef enum {
  
  CPM_SIGNATURE_MAIN_TABLE              =            'MPC$',         ///< The signature of HYGON_CPM_MAIN_TABLE

  // HyEx
  HYEX_CPM_SIGNATURE_GET_PLATFORM_ID         =            '00A$',        ///< The signature of HYGON_CPM_PLATFORM_ID_TABLE
  HYEX_CPM_SIGNATURE_GET_PLATFORM_ID_CONVERT =            '01A$',        ///< The signature of HYGON_CPM_PLATFORM_ID_CONVERT_TABLE
  HYEX_CPM_SIGNATURE_PRE_INIT                =            '02A$',        ///< The signature of HYGON_CPM_PRE_INIT_TABLE
  HYEX_CPM_SIGNATURE_INIT_FLAG               =            '03A$',        ///< The signature of HYGON_CPM_INIT_FLAG_TABLE
  HYEX_CPM_SIGNATURE_GET_PLATFORM_ID2        =            '04A$',        ///< The signature of HYGON_CPM_PLATFORM_ID_TABLE2
  HYEX_CPM_SIGNATURE_CORE_TOPOLOGY           =            '05A$',        ///< The signature of HYGON_CPM_CORE_TOPOLOGY_TABLE

  HYEX_CPM_SIGNATURE_GPIO_INIT               =            '06A$',        ///< The signature of HYGON_CPM_GPIO_INIT_TABLE
  HYEX_CPM_SIGNATURE_GEVENT_INIT             =            '07A$',        ///< The signature of HYGON_CPM_GEVENT_INIT_TABLE
  HYEX_CPM_SIGNATURE_GPIO_DEVICE_CONFIG      =            '08A$',        ///< The signature of HYGON_CPM_GPIO_DEVICE_CONFIG_TABLE
  HYEX_CPM_SIGNATURE_GPIO_DEVICE_POWER       =            '09A$',        ///< The signature of HYGON_CPM_GPIO_DEVICE_POWER_TABLE
  HYEX_CPM_SIGNATURE_GPIO_DEVICE_DETECTION   =            '10A$',        ///< The signature of HYGON_CPM_GPIO_DEVICE_DETECTION_TABLE
  HYEX_CPM_SIGNATURE_GPIO_DEVICE_RESET       =            '11A$',        ///< The signature of HYGON_CPM_GPIO_DEVICE_RESET_TABLE
  HYEX_CPM_SIGNATURE_SET_MEM_VOLTAGE         =            '12A$',        ///< The signature of HYGON_CPM_GPIO_MEM_VOLTAGE_TABLE
  HYEX_CPM_SIGNATURE_PCIE_CLOCK              =            '13A$',        ///< The signature of HYGON_CPM_PCIE_CLOCK_TABLE
  HYEX_CPM_SIGNATURE_EXT_CLKGEN              =            '14A$',        ///< The signature of HYGON_CPM_EXT_CLKGEN_TABLE
  HYEX_CPM_SIGNATURE_GPIO_PRE_INIT           =            '15A$',        ///< The signature of HYGON_CPM_GPIO_PRE_INIT_TABLE

  HYEX_CPM_SIGNATURE_PCIE_TOPOLOGY           =            '16A$',         ///< The signature of HYGON_CPM_PCIE_TOPOLOGY_TABLE
  HYEX_CPM_SIGNATURE_PCIE_TOPOLOGY_OVERRIDE  =            '17A$',         ///< The signature of HYGON_CPM_PCIE_TOPOLOGY_OVERRIDE_TABLE
  HYEX_CPM_SIGNATURE_PCIE_EXPRESS_CARD       =            '18A$',         ///< The signature of HYGON_CPM_EXPRESS_CARD_TABLE
  HYEX_CPM_SIGNATURE_SET_VDDP_VDDR_VOLTAGE   =            '19A$',         ///< The signature of HYGON_CPM_GPIO_VDDP_VDDR_VOLTAGE_TABLE
  HYEX_CPM_SIGNATURE_PCIE_OTHER_HOTPLUG_CARD =            '20A$',         ///< The signature of HYGON_CPM_OTHER_HOTPLUG_CARD_TABLE
  HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY           =            '21A$',         ///< The signature of Socket 0 HYGON_CPM_HSIO_TOPOLOGY_TABLE
  HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY_S1        =            '22A$',         ///< The signature of Socket 1 HYGON_CPM_HSIO_TOPOLOGY_TABLE
  HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY_S2        =            '23A$',         ///< The signature of Socket 2 HYGON_CPM_HSIO_TOPOLOGY_TABLE
  HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY_S3        =            '24A$',         ///< The signature of Socket 3 HYGON_CPM_HSIO_TOPOLOGY_TABLE
  HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY_S4        =            '25A$',         ///< The signature of Socket 4 HYGON_CPM_HSIO_TOPOLOGY_TABLE
  HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY_S5        =            '26A$',         ///< The signature of Socket 5 HYGON_CPM_HSIO_TOPOLOGY_TABLE
  HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY_S6        =            '27A$',         ///< The signature of Socket 6 HYGON_CPM_HSIO_TOPOLOGY_TABLE
  HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY_S7        =            '28A$',         ///< The signature of Socket 7 HYGON_CPM_HSIO_TOPOLOGY_TABLE
  HYEX_CPM_SIGNATURE_LPC_UART                =            '29A$',         ///< The signature of HYGON_CPM_LPC_UART_TABLE
  HYEX_CPM_SIGNATURE_DIMM_MAP                =            '30A$',         ///< The signature of HYGON_CPM_DIMM_MAP_TABLE
  HYEX_CPM_SIGNATURE_CODEC_VERBTABLE         =            '31A$',         ///< The signature of HYGON_CPM_CODEC_VERBTABLE_TABLE
  HYEX_CPM_SIGNATURE_EQ_CONFIG               =            '32A$',         ///< The signature of HYGON_CPM_EQ_CONFIG_TABLE

  // HyGx
  HYGX_CPM_SIGNATURE_GET_PLATFORM_ID         =            '50A$',        ///< The signature of HYGON_CPM_PLATFORM_ID_TABLE
  HYGX_CPM_SIGNATURE_GET_PLATFORM_ID_CONVERT =            '51A$',        ///< The signature of HYGON_CPM_PLATFORM_ID_CONVERT_TABLE
  HYGX_CPM_SIGNATURE_PRE_INIT                =            '52A$',        ///< The signature of HYGON_CPM_PRE_INIT_TABLE
  HYGX_CPM_SIGNATURE_INIT_FLAG               =            '53A$',        ///< The signature of HYGON_CPM_INIT_FLAG_TABLE
  HYGX_CPM_SIGNATURE_GET_PLATFORM_ID2        =            '54A$',        ///< The signature of HYGON_CPM_PLATFORM_ID_TABLE2
  HYGX_CPM_SIGNATURE_CORE_TOPOLOGY           =            '55A$',        ///< The signature of HYGON_CPM_CORE_TOPOLOGY_TABLE

  HYGX_CPM_SIGNATURE_GPIO_INIT               =            '56A$',        ///< The signature of HYGON_CPM_GPIO_INIT_TABLE
  HYGX_CPM_SIGNATURE_GEVENT_INIT             =            '57A$',        ///< The signature of HYGON_CPM_GEVENT_INIT_TABLE
  HYGX_CPM_SIGNATURE_GPIO_DEVICE_CONFIG      =            '58A$',        ///< The signature of HYGON_CPM_GPIO_DEVICE_CONFIG_TABLE
  HYGX_CPM_SIGNATURE_GPIO_DEVICE_POWER       =            '59A$',        ///< The signature of HYGON_CPM_GPIO_DEVICE_POWER_TABLE
  HYGX_CPM_SIGNATURE_GPIO_DEVICE_DETECTION   =            '60A$',        ///< The signature of HYGON_CPM_GPIO_DEVICE_DETECTION_TABLE
  HYGX_CPM_SIGNATURE_GPIO_DEVICE_RESET       =            '61A$',        ///< The signature of HYGON_CPM_GPIO_DEVICE_RESET_TABLE
  HYGX_CPM_SIGNATURE_SET_MEM_VOLTAGE         =            '62A$',        ///< The signature of HYGON_CPM_GPIO_MEM_VOLTAGE_TABLE
  HYGX_CPM_SIGNATURE_PCIE_CLOCK              =            '63A$',        ///< The signature of HYGON_CPM_PCIE_CLOCK_TABLE
  HYGX_CPM_SIGNATURE_EXT_CLKGEN              =            '64A$',        ///< The signature of HYGON_CPM_EXT_CLKGEN_TABLE
  HYGX_CPM_SIGNATURE_GPIO_PRE_INIT           =            '65A$',        ///< The signature of HYGON_CPM_GPIO_PRE_INIT_TABLE

  HYGX_CPM_SIGNATURE_PCIE_TOPOLOGY           =            '66A$',         ///< The signature of HYGON_CPM_PCIE_TOPOLOGY_TABLE
  HYGX_CPM_SIGNATURE_PCIE_TOPOLOGY_OVERRIDE  =            '67A$',         ///< The signature of HYGON_CPM_PCIE_TOPOLOGY_OVERRIDE_TABLE
  HYGX_CPM_SIGNATURE_PCIE_EXPRESS_CARD       =            '68A$',         ///< The signature of HYGON_CPM_EXPRESS_CARD_TABLE
  HYGX_CPM_SIGNATURE_SET_VDDP_VDDR_VOLTAGE   =            '69A$',         ///< The signature of HYGON_CPM_GPIO_VDDP_VDDR_VOLTAGE_TABLE
  HYGX_CPM_SIGNATURE_PCIE_OTHER_HOTPLUG_CARD =            '70A$',         ///< The signature of HYGON_CPM_OTHER_HOTPLUG_CARD_TABLE
  HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY           =            '71A$',         ///< The signature of Socket 0 HYGON_CPM_HSIO_TOPOLOGY_TABLE
  HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY_S1        =            '72A$',         ///< The signature of Socket 1 HYGON_CPM_HSIO_TOPOLOGY_TABLE
  HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY_S2        =            '73A$',         ///< The signature of Socket 2 HYGON_CPM_HSIO_TOPOLOGY_TABLE
  HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY_S3        =            '74A$',         ///< The signature of Socket 3 HYGON_CPM_HSIO_TOPOLOGY_TABLE
  HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY_S4        =            '75A$',         ///< The signature of Socket 4 HYGON_CPM_HSIO_TOPOLOGY_TABLE
  HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY_S5        =            '76A$',         ///< The signature of Socket 5 HYGON_CPM_HSIO_TOPOLOGY_TABLE
  HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY_S6        =            '77A$',         ///< The signature of Socket 6 HYGON_CPM_HSIO_TOPOLOGY_TABLE
  HYGX_CPM_SIGNATURE_HSIO_TOPOLOGY_S7        =            '78A$',         ///< The signature of Socket 7 HYGON_CPM_HSIO_TOPOLOGY_TABLE
  HYGX_CPM_SIGNATURE_LPC_UART                =            '79A$',         ///< The signature of HYGON_CPM_LPC_UART_TABLE
  HYGX_CPM_SIGNATURE_DIMM_MAP                =            '80A$',         ///< The signature of HYGON_CPM_DIMM_MAP_TABLE
  HYGX_CPM_SIGNATURE_CODEC_VERBTABLE         =            '81A$',         ///< The signature of HYGON_CPM_CODEC_VERBTABLE_TABLE
  HYGX_CPM_SIGNATURE_EQ_CONFIG               =            '82A$',         ///< The signature of HYGON_CPM_EQ_CONFIG_TABLE
} HYGON_CPM_TABLE_SIGNATURE;

/// The value of special clock id

typedef enum {
  CPM_CLKID_APU_CLK           =                         0x80, ///< APU_CLK Power Down Enable
  CPM_CLKID_DISP2_CLK         =                         0x81, ///< DISP2_CLK Power Down Enable
  CPM_CLKID_PCIE_RCLK_OUTPUT  =                         0x82, ///< PCIE_RCLK_Output Power Down Enable
  CPM_CLKID_DISP_CLK          =                         0x83, ///< DISP_CLK Power Down Enable
  CPM_CLKID_PCIE_RCLK         =                         0x84, ///< PCIE_RCLK Power Down Enable
  CPM_CLKID_CLOCK_BUFFER_BIAS =                         0x85, ///< Clock Buffer Bias Power Down Enable
  CPM_CLKID_OSCOUT2_OUTOFF    =                         0x86, ///< OSCOUT2 Power Down Enable
} HYGON_CPM_CLOCK_ID;

/// The value of Boot Mode

typedef enum {
  CPM_BOOT_MODE_S0 =                          0x00,           ///< BOOT ON S0
  CPM_BOOT_MODE_S1 =                          0x01,           ///< BOOT ON S1 RESUME
  CPM_BOOT_MODE_S3 =                          0x03,           ///< BOOT ON S3 RESUME
  CPM_BOOT_MODE_S4 =                          0x04,           ///< BOOT ON S4 RESUME
  CPM_BOOT_MODE_S5 =                          0x05,           ///< BOOT ON S5 RESUME
} HYGON_CPM_BOOT_MODE;

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
  CPM_CPU_REVISION_ID_SAT,                                    ///< CPU Revision ID for SAT
  CPM_CPU_REVISION_ID_HYGON_SAT,                              ///< CPU Revision ID for HYGON SAT PS-473
} HYGON_CPM_CPU_REVISION_ID;

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
  CPM_PCIE_REVISION_ID_SAT,                                   ///< PCIE Revision ID for SAT
  CPM_PCIE_REVISION_ID_HYGON_SAT,                             ///< PCIE Revision ID for HYGON SAT PS-473
} HYGON_CPM_PCIE_REVISION_ID;

/// The value of CPU Revision ID

typedef enum {
  CPM_FCH_REVISION_ID_DEFAULT =                    0x00,      ///< FCH Revision ID for Default
  CPM_FCH_REVISION_ID_KB      =                    0x01,      ///< FCH Revision ID for KB
  CPM_FCH_REVISION_ID_ML      =                    0x02,      ///< FCH Revision ID for ML
  CPM_FCH_REVISION_ID_CZ      =                    0x03,      ///< FCH Revision ID for CZ
  CPM_FCH_REVISION_ID_NL      =                    0x04,      ///< FCH Revision ID for NL
  CPM_FCH_REVISION_ID_AM      =                    0x05,      ///< FCH Revision ID for AM
  CPM_FCH_REVISION_ID_ST      =                    0x06,      ///< FCH Revision ID for ST
  CPM_FCH_REVISION_ID_BR      =                    0x07,      ///< FCH Revision ID for BR
  CPM_FCH_REVISION_ID_SAT     =                    0x08,      ///< FCH Revision ID for SAT
  CPM_FCH_REVISION_ID_RV      =                    0x09,      ///< FCH Revision ID for RV
} HYGON_CPM_FCH_REVISION_ID;

/// The value of the stage to load HYGON CPM Pre Init Table

typedef enum {
  CPM_PRE_INIT_STAGE_0 =                      0,              ///< Stage 0 to load Pre Init Table
  CPM_PRE_INIT_STAGE_1 =                      1,              ///< Stage 1 to load Pre Init Table
} HYGON_CPM_PRE_INIT_STAGE;

/// Configuration values for CPM table attribute

typedef enum {
  CPM_PEI      =                                   BIT0,      ///< Used by CPM PEI driver
  CPM_DXE      =                                   BIT1,      ///< Used by CPM DXE driver
  CPM_SMM      =                                   BIT2,      ///< Used by CPM SMM driver
  CPM_OVERRIDE =                              BIT3,           ///< Modified by CPM driver
} HYGON_CPM_TABLE_ATTRIBUTE;

/// CPM table header

typedef struct {
  UINT32    TableSignature;                                   ///< Signature of CPM table
  UINT16    TableSize;                                        ///< Table size
  UINT8     FormatRevision;                                   ///< Revision of table format
  UINT8     ContentRevision;                                  ///< Revision of table contect
  UINT32    PlatformMask;                                     ///< The mask of platform table supports
  UINT32    Attribute;                                        ///< Table attribute
} HYGON_CPM_TABLE_COMMON_HEADER;

/// Table pointer

typedef union {
  VOID      *Pointer;                                         ///< Table pointer
  UINT64    Raw;                                              ///< Table pointer value
} HYGON_CPM_POINTER;

/// Device and function number of PCI device

typedef struct {
  UINT8    Device   : 5;                                    ///< PCI Device Number
  UINT8    Function : 3;                                    ///< PCI Function Number
} HYGON_CPM_PCI_DEVICE_FUNCTION;

/// Item of CPM table list

typedef struct {
  UINT32               TableId;                               ///< Signature of table
  UINT8                Flag;                                  ///< Location of table. 0: ROM. 1: RAM
  HYGON_CPM_POINTER    SubTable;                              ///< Pointer of table
} HYGON_CPM_TABLE_ITEM;

/// CPM table list

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                           ///< Table header
  UINT32                           Number;                           ///< Table number to be recorded in the list
  UINT32                           Size;                             ///< Table size
  HYGON_CPM_TABLE_ITEM             Item[HYGON_TABLE_LIST_ITEM_SIZE]; ///< The array of CPM table
} HYGON_CPM_TABLE_LIST;

/// GPIO pin list for platform Id

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                                ///< Table header
  UINT16                           GpioPin[HYGON_PLATFORM_ID_TABLE_SIZE]; ///< The array of GPIO pin for platform Id
} HYGON_CPM_PLATFORM_ID_TABLE;

/// SMBUS GPIO Pin for Platform ID

typedef struct _HYGON_CPM_SMBUS_PLATFORM_ID {
  UINT8    SmbusSelect;                                                 ///< SMBUS Number
  UINT8    SmbusAddress;                                                ///< SMBUS Address
  UINT8    SmbusOffset;                                                 ///< SMBUS Offset
  UINT8    SmbusBit;                                                    ///< SMBUS Bit
} HYGON_CPM_SMBUS_PLATFORM_ID;

/// EEPROM for platform Id

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                                ///< Table header
  HYGON_CPM_SMBUS_PLATFORM_ID      GpioPin[HYGON_PLATFORM_ID_TABLE_SIZE]; ///< SMBUS GPIO pin for Platform ID
} HYGON_CPM_PLATFORM_ID_TABLE2;

/// Platform Id converting item

typedef struct {
  UINT8     CpuRevisionId;                                    ///< CPU Revision ID
  UINT16    OriginalIdMask;                                   ///< Platform Id mask from platform Id table
  UINT16    OriginalId;                                       ///< Platform Id from platform Id table
  UINT16    ConvertedId;                                      ///< Platform Id which is used to check whether
                                                              ///< CPM table supports current platform
} HYGON_CPM_PLATFORM_ID_CONVERT_ITEM;

/// Platform Id mapping table

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER         Header;                                   ///< Table Header
  HYGON_CPM_PLATFORM_ID_CONVERT_ITEM    Item[HYGON_PLATFORM_ID_CONVERT_TABLE_SIZE]; ///< Platform Id mapping list
} HYGON_CPM_PLATFORM_ID_CONVERT_TABLE;

/// The table definition for early initialization

typedef struct {
  UINT8    Type;                                              ///< Register type. 0: FCH MMIO. 1: PCI
  UINT8    Select;                                            ///< Register sub-type
  UINT8    Offset;                                            ///< Register offset
  UINT8    AndMask;                                           ///< AND mask
  UINT8    OrMask;                                            ///< OR mask
  UINT8    Stage;                                             ///< Stage number
} HYGON_CPM_PRE_SETTING_ITEM;

/// Register table to be initialized in the earliest stage
typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                      ///< Table header
  HYGON_CPM_PRE_SETTING_ITEM       Item[HYGON_PRE_INIT_SIZE];   ///< Register setting
} HYGON_CPM_PRE_INIT_TABLE;

/// The table definition for each CPU Die information
typedef struct {
  UINT8    Socket;                                            ///< CPU Socket Number.
  UINT8    Die;                                               ///< CPU Die Number.
  UINT8    Bus;                                               ///< CPU Bus Number.
} HYGON_CPM_CORE_TOPOLOGY_LIST;

/// CPU Core Topology Table
typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                                      ///< Table header
  HYGON_CPM_CORE_TOPOLOGY_LIST     CoreList[HYGON_CPM_CORE_TOPOLOGY_SIZE];      ///< CPU Core Topology List
} HYGON_CPM_CORE_TOPOLOGY_TABLE;

//
// The table definition for GPIO
//
// HYGON_CPM_GPIO_INIT_TABLE
// HYGON_CPM_GEVENT_INIT_TABLE
// HYGON_CPM_GPIO_DEVICE_CONFIG_TABLE
// HYGON_CPM_GPIO_DEVICE_POWER_TABLE
// HYGON_CPM_GPIO_DEVICE_DETECTION_TABLE
// HYGON_CPM_GPIO_DEVICE_RESET_TABLE
//

/// GPIO Pin

typedef struct {
  UINT16    Pin;                                              ///< GPIO pin number
  UINT8     Value;                                            ///< GPIO pin value
} HYGON_CPM_GPIO_PIN;

/// Defintion of GPIO setting

typedef union {
  UINT16    Raw;                                              ///< GPIO setting value
  struct {
    ///<
    UINT16    Out            : 1;                             ///< Output state
    UINT16    OutEnB         : 1;                             ///< Output enable
    UINT16    PullUpSel      : 1;                             ///< Pull up select: 0: 4K. 1: 8K
    UINT16    SetEnB         : 1;                             ///< Gate of Out and OutEnB
    UINT16    Sticky         : 1;                             ///< Sticky enable
    UINT16    PullUp         : 1;                             ///< Pull up enable
    UINT16    PullDown       : 1;                             ///< Pull down enable
    UINT16    PresetEn       : 1;                             ///< Gate of Sticky, PullUp & PullDown
    UINT16    IoMux          : 3;                             ///< Multi-function IO pin function select of GPIO
    UINT16    IoMuxEn        : 1;                             ///< Gate of IoMux
    UINT16    DrvStrengthSel : 2;                             ///< Drive Strength Select: 0: 4mA. 1: 8mA. 2: 12mA. 3: 16mA
    UINT16    Reserved2      : 2;                             ///< Reserved
  }                               Gpio;                       ///< Bit mapping for GPIO setting
} HYGON_CPM_GPIO_SETTING;

/// GPIO setting item
typedef struct {
  UINT16                    Pin;                              ///< GPIO pin number
  HYGON_CPM_GPIO_SETTING    Setting;                          ///< GPIO setting
} HYGON_CPM_GPIO_ITEM;

/// GPIO init table

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                         ///< Table header
  HYGON_CPM_GPIO_ITEM              GpioList[HYGON_GPIO_ITEM_SIZE]; ///< GPIO setting list
} HYGON_CPM_GPIO_INIT_TABLE;

/// The definition of GEVENT setting

typedef union {
  UINT16    Raw;                                              ///< GEVENT setting value
  struct {
    ///<
    UINT16    EventEnable : 1;                                ///< EventEnable. 0: Disable, 1: Enable
    UINT16    SciTrig     : 1;                                ///< SciTrig. 0: Falling Edge, 1: Rising Edge
    UINT16    SciLevl     : 1;                                ///< SciLevl.0 trigger mode. 0: Edge trigger, 1: Level Trigger
    UINT16    SmiSciEn    : 1;                                ///< SmiSciEn. 0: Not send SMI, 1: Send SMI
    UINT16    SciS0En     : 1;                                ///< SciS0En. 0: Disable, 1: Enable
    UINT16    SciMap      : 5;                                ///< SciMap. 0000b->1111b
    UINT16    SciTrigAuto : 1;                                ///< SciTrigAuto. 1: Disable. 0: Enable.
    UINT16    SmiTrig     : 1;                                ///< SmiTrig. 0: Active Low, 1: Active High
    UINT16    SmiControl  : 4;                                ///< SmiControl. 0: Disable, 1: SMI 2: NMI 3: IRQ13
  }                               Gevent;                     ///< Bit mapping for GEVENT setting
  struct {
    ///<
    UINT16    DebounceTmrOut     : 4;                         ///< Specifies the debounce timer out number
    UINT16    DebounceTmrOutUnit : 1;                         ///< 0: 30.5us (One RtcClk period), 1: 122us (four RtcClk periods)
    UINT16    DebounceCntrl      : 2;                         ///< 00b: No debounce, 01b: Preserve low glitch
                                                              ///< 10b: Preserve high glitch, 11b: Remove glitch
    UINT16    Reserved           : 1;                         ///< Reserved
    UINT16    LevelTrig          : 1;                         ///< 0: Edge trigger, 1: Level trigger
    UINT16    ActiveLevel        : 2;                         ///< 00b: Active High. 01b: Active Low. 10b: Active on both edges if LevelTrig=0
    UINT16    InterruptEnable    : 2;                         ///< [0]: Enable interrupt status, [1]: Enable interrupt delivery
    UINT16    WakeCntrl          : 3;                         ///< [0]: Enable wake in S0I3 state, [1]: Enable wake in S3 state, [2]: Enable wake in S4/S5 state
  }                               Gpio;                       ///< Bit mapping for GPIO interrupt setting
} HYGON_CPM_GEVENT_SETTING;

/// GEVENT setting item

typedef struct {
  UINT16                      Pin;                            ///< GEVENT pin number
  HYGON_CPM_GEVENT_SETTING    Setting;                        ///< GEVENT setting
} HYGON_CPM_GEVENT_ITEM;

/// GEVENT init table

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                             ///< Table header
  HYGON_CPM_GEVENT_ITEM            GeventList[HYGON_GEVENT_ITEM_SIZE]; ///< GEVENT setting list
} HYGON_CPM_GEVENT_INIT_TABLE;

/// Configuration of Device which is controlled by GPIO pin

typedef struct {
  UINT8    DeviceId;                                  ///< Device Id
  union {
    UINT8    Raw;                                     ///< Device Config Value
    struct {
      UINT8    Enable        : 2;                     ///< Flag of power state. 0: Disable. 1: Enable. 2: Auto Detection
      UINT8    ResetAssert   : 1;                     ///< Flag to assert reset pin
      UINT8    ResetDeassert : 1;                     ///< Flag to de-assert reset pin
      UINT8    Reserved      : 4;                     ///< Reserved
    }                             Setting;            ///< Bit mapping of Device Config
  }                               Config;             ///< Device Config
} HYGON_CPM_GPIO_DEVICE_CONFIG;

/// Device config table

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                             ///< Table header
  HYGON_CPM_GPIO_DEVICE_CONFIG     DeviceList[HYGON_GPIO_DEVICE_SIZE]; ///< Device config list
} HYGON_CPM_GPIO_DEVICE_CONFIG_TABLE;

/// Device detection method

typedef struct {
  UINT8     DeviceId;                                 ///< Device Id
  UINT8     Type;                                     ///< Detection type. 0: One GPIO pin. 1: Two GPIO pins. 2: Special Pin
  UINT16    PinNum1;                                  ///< Pin number of GPIO 1
  UINT8     Value1;                                   ///< Value of GPIO 1
  UINT16    PinNum2;                                  ///< Pin number of GPIO 2
  UINT8     Value2;                                   ///< Value of GPIO 2
  UINT16    PinNum3;                                  ///< Pin number of GPIO 3
  UINT8     Value3;                                   ///< Value of GPIO 3
} HYGON_CPM_GPIO_DEVICE_DETECTION;

/// Device Detection Table

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER      Header;                                          ///< Table header
  HYGON_CPM_GPIO_DEVICE_DETECTION    DeviceDetectionList[HYGON_GPIO_DEVICE_DETECT_SIZE]; ///< Device Detection List
} HYGON_CPM_GPIO_DEVICE_DETECTION_TABLE;

/// Device Reset Table

typedef struct {
  UINT8    DeviceId;                                  ///< Device Id
  UINT8    Mode;                                      ///< Reset mode
                                                      ///< 0: Reset Assert.
                                                      ///< 1: Reset De-assert
                                                      ///< 2: Delay between Assert and Deassert
  UINT8    Type;                                      ///< Register type
                                                      ///< 0: GPIO.
                                                      ///< 1: Special pin.if Mode = 0 or 1
  union {
    UINT32                Stall;                      ///< Delay
    HYGON_CPM_GPIO_PIN    Gpio;                       ///< GPIO pin
  }                               Config;             ///< Setting
  UINT8    InitFlag;                                  ///< Init flag in post
} HYGON_CPM_GPIO_DEVICE_RESET;

/// Configuration values for CPM GPIO Device Init Flag

typedef enum {
  GPIO_DEVICE_INIT_DISABLE     =      0,              ///< GPIO Device does not need to be initialized or is controlled by GPIO Device COnfig
  GPIO_DEVICE_INIT_STAGE_1     =      1,              ///< GPIO Device needs to be initialized on stage 1
  GPIO_DEVICE_INIT_STAGE_2     =      2,              ///< GPIO Device needs to be initialized on stage 2
  GPIO_DEVICE_INIT_STAGE_DUMMY =  3,                  ///< GPIO Device does not need to be initialized in BIOS post
  GPIO_DEVICE_INIT_STAGE_3     =      4,              ///< GPIO Device needs to be initialized from S0 to S3/S4/S5
  GPIO_DEVICE_INIT_STAGE_4     =      5,              ///< GPIO Device needs to be initialized from S0 to S3
  GPIO_DEVICE_INIT_STAGE_5     =      6,              ///< GPIO Device needs to be initialized from S0 to S4
  GPIO_DEVICE_INIT_STAGE_6     =      7,              ///< GPIO Device needs to be initialized from S0 to S5
} HYGON_CPM_GPIO_DEVICE_INIT_FLAG;

/// Device Reset Table

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                                        ///< Table header
  HYGON_CPM_GPIO_DEVICE_RESET      DeviceResetList[HYGON_GPIO_DEVICE_RESET_SIZE]; ///< Device reset list
} HYGON_CPM_GPIO_DEVICE_RESET_TABLE;

/// Item of device power on / off sequence

typedef struct {
  UINT8    DeviceId;                                  ///< Device Id
  UINT8    Mode;                                      ///< Device Power Mode. 1: Power On. 0: Power Off
  UINT8    Type;                                      ///< Device Power Item. 0: Set GPIO. 1: Wait GPIO. 2: Add Delay
  union {
    UINT32                Stall;                      ///< Delay
    HYGON_CPM_GPIO_PIN    SetGpio;                    ///< Set GPIO pin
    HYGON_CPM_GPIO_PIN    WaitGpio;                   ///< Wait for GPIO pin to some value
  }                               Config;             ///< Dvice Power Item Setting
  UINT8    InitFlag;                                  ///< Init flag in post
} HYGON_CPM_GPIO_DEVICE_POWER;

/// GPIO Device Init Flag Table

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                                        ///< Table header
  UINT8                            PowerInitFlag[HYGON_GPIO_DEVICE_POWER_SIZE];   ///< Init Flag for Power Sequence
  UINT8                            ResetInitFlag[HYGON_GPIO_DEVICE_RESET_SIZE];   ///< Init Flag for Reset Sequence
} HYGON_CPM_INIT_FLAG_TABLE;

/// Device Power Sequence Table

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                                        ///< Table header
  HYGON_CPM_GPIO_DEVICE_POWER      DevicePowerList[HYGON_GPIO_DEVICE_POWER_SIZE]; ///< Device power sequence list
} HYGON_CPM_GPIO_DEVICE_POWER_TABLE;

/// GPIO setting for one memory voltage (VDDIO)

typedef struct {
  UINT8     Voltage;                                  ///< DDR3Voltage
  UINT16    GpioPin1;                                 ///< GPIO pin 1
  UINT8     Value1;                                   ///< Value of GPIO pin 1
  UINT16    GpioPin2;                                 ///< GPIO pin 2
  UINT8     Value2;                                   ///< Value of GPIO pin 2
} HYGON_CPM_GPIO_MEM_VOLTAGE_ITEM;

/// Memory voltage table

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER      Header;                            ///< Table header
  HYGON_CPM_GPIO_MEM_VOLTAGE_ITEM    Item[HYGON_GPIO_MEM_VOLTAGE_SIZE]; ///< GPIO setting list for memory voltage
} HYGON_CPM_GPIO_MEM_VOLTAGE_TABLE;

/// GPIO setting for VDDP/VDDR voltage

typedef struct {
  UINT8     Voltage;                                  ///< VDDP/VDDR Voltage
  UINT16    GpioPin1;                                 ///< GPIO pin
  UINT8     Value1;                                   ///< Value of GPIO pin
} HYGON_CPM_GPIO_VDDP_VDDR_VOLTAGE_ITEM;

/// VDDP/VDDR voltage table

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER            Header;                                  ///< Table header
  HYGON_CPM_GPIO_VDDP_VDDR_VOLTAGE_ITEM    Item[HYGON_GPIO_VDDP_VDDR_VOLTAGE_SIZE]; ///< GPIO setting list for memory voltage
} HYGON_CPM_GPIO_VDDP_VDDR_VOLTAGE_TABLE;

/// PCIe Clock Setting

typedef struct _HYGON_CPM_PCIE_CLOCK_ITEM {
  UINT8     ClkId;                                    ///< FCH PCIe Clock
  UINT8     ClkReq;                                   ///< FCH PCIe ClkReq
  UINT8     ClkIdExt;                                 ///< External Clock Source
  UINT8     ClkReqExt;                                ///< External ClkReq
  UINT8     DeviceId;                                 ///< Device Id. No Device Id if 0xFF
  UINT8     Device;                                   ///< Device Number of PCIe bridge
  UINT8     Function;                                 ///< Function Number of PCIe bridge
  UINT8     SlotCheck;                                ///< Slot Check Flag:
                                                      ///< BIT0: Check PCI Space
                                                      ///< BIT1: Check GPIO pin
                                                      ///< BIT2: Check Clock Power Management Enable
                                                      ///< BIT3~6: Reserved
                                                      ///< BIT7: Change PCIe Clock in ACPI method
  UINT32    SpecialFunctionId;                        ///< Id of Special Function
} HYGON_CPM_PCIE_CLOCK_ITEM;

/// PCIe Clock Table

typedef struct _HYGON_CPM_PCIE_CLOCK_TABLE {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                      ///< Table header
  HYGON_CPM_PCIE_CLOCK_ITEM        Item[HYGON_PCIE_CLOCK_SIZE]; ///< PCIe clock setting list
} HYGON_CPM_PCIE_CLOCK_TABLE;

/// External ClkGen Register Setting Item

typedef struct _HYGON_CPM_EXT_CLKGEN_ITEM {
  UINT8    Function;                                  ///< External ClkGen Setting Item Type
                                                      ///< 0x00~0x7F: Initial Sequence Id
                                                      ///< 0x80~0x8F: Clock Disable Sequence
                                                      ///< 0x90~0x9F: ClkReq Enable Sequence
  UINT8    Offset;                                    ///< Register Offset
  UINT8    AndMask;                                   ///< AND Mask
  UINT8    OrMask;                                    ///< Or Mask
} HYGON_CPM_EXT_CLKGEN_ITEM;

/// External ClkGen Table

typedef struct _HYGON_CPM_EXT_CLKGEN_TABLE {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                      ///< Table header
  UINT8                            SmbusSelect;                 ///< SMBUS Number
  UINT8                            SmbusAddress;                ///< SMBUS Address
  HYGON_CPM_EXT_CLKGEN_ITEM        Item[HYGON_EXT_CLKGEN_SIZE]; ///< External ClkGen Register Setting List
} HYGON_CPM_EXT_CLKGEN_TABLE;

/// PCIE Topology Table

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                                ///< Table header
  UINT32                           SocketId;                              ///< Socket Id
  PCIe_PORT_DESCRIPTOR             Port[HYGON_PCIE_PORT_DESCRIPTOR_SIZE]; ///< PCIe Port Descriptor List
  PCIe_DDI_DESCRIPTOR              Ddi[HYGON_PCIE_DDI_DESCRIPTOR_SIZE];   ///< PCIe DDI Descriptor List
} HYGON_CPM_PCIE_TOPOLOGY_TABLE;

/// HSIO Topology Table

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                                ///< Table header
  UINT32                           SocketId;                              ///< Socket Id
  HSIO_PORT_DESCRIPTOR             Port[HYGON_HSIO_PORT_DESCRIPTOR_SIZE]; ///< HSIO Port Descriptor List
} HYGON_CPM_HSIO_TOPOLOGY_TABLE;

/// The override table definition for PCIE Topology

typedef struct {
  union {
    UINT8    Raw;                                             ///< Value of Flag
    struct {
      UINT8    EnableOverride      : 1;                       ///< Override Enable field of descriptor
      UINT8    DdiTypeOverride     : 1;                       ///< Override DDI type
      UINT8    LaneOverride        : 1;                       ///< Override StartLane and EndLane
      UINT8    PortPresentOverride : 1;                       ///< Override PortPresent
      UINT8    IsDdi               : 1;                       ///< This item is used to override Port or DDI descriptor
                                                              ///< 0: Port Descriptor
                                                              ///< 1: DDI Descriptor
      UINT8    Reserved            : 2;                       ///< Reserved
      UINT8    Valid               : 1;                       ///< Valid Flag
    }                             Config;                     ///< Bitmap of Flag
  }                               Flag;                       ///< Flag of PCIe Topology override item
  UINT8    Offset;                                            ///< Offset of Port Descriptor or DDI Dscriptor list
  UINT8    Enable;                                            ///< Descriptor Enable
  UINT8    DdiType;                                           ///< Ddi Type
  UINT8    PortPresent;                                       ///< Port Present
  UINT8    StartLane;                                         ///< Start Lane
  UINT8    EndLane;                                           ///< End Lane
} HYGON_CPM_PCIE_TOPOLOGY_OVERRIDE_ITEM;

/// PCIE Topology Override Table

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER            Header;                                  ///< Table header
  HYGON_CPM_PCIE_TOPOLOGY_OVERRIDE_ITEM    Item[HYGON_PCIE_TOPOLOGY_OVERRIDE_SIZE]; ///< Override Item List
} HYGON_CPM_PCIE_TOPOLOGY_OVERRIDE_TABLE;

/// Express Card Table

typedef struct _HYGON_CPM_EXPRESS_CARD_TABLE {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                        ///< Table header
  UINT8                            Device;                        ///< Device Number of PCIe Bridge
  UINT8                            Function;                      ///< Function Number of PCIe Bridge
  UINT8                            EventPin;                      ///< GEVENT Pin
  UINT8                            DeviceId;                      ///< Device Id
} HYGON_CPM_EXPRESS_CARD_TABLE;

/// Other Hotplug Card Table

typedef struct _HYGON_CPM_OTHER_HOTPLUG_CARD_TABLE {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                        ///< Table header
  UINT8                            Number;                        ///< Card Number: 0 ~ 2
  UINT8                            Device0;                       ///< Device Number of PCIe Bridge for Card 0
  UINT8                            Function0;                     ///< Function Number of PCIe Bridge for Card 0
  UINT8                            EventPin0;                     ///< GEVENT Pin for Card 0
  UINT8                            DeviceId0;                     ///< Device Id for Card 0
  UINT8                            Device1;                       ///< Device Number of PCIe Bridge for Card 1
  UINT8                            Function1;                     ///< Function Number of PCIe Bridge for Card 1
  UINT8                            EventPin1;                     ///< GEVENT Pin  for Card 1
  UINT8                            DeviceId1;                     ///< Device Id  for Card 1
} HYGON_CPM_OTHER_HOTPLUG_CARD_TABLE;

/// Convert from Device Id of SATA controller to SATA mode mask

typedef struct {
  UINT16    DeviceId;                                         ///< Device Id of SATA controller
  UINT8     Mask;                                             ///< SATA Mode Mask
} HYGON_CPM_SATA_MODE_MASK;

/// The table definition for LPC UART

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                    ///< Table header
  UINT8                            ChipId;                    ///< Chip Id. 0: SMSC_1036
  UINT8                            LpcIndex;                  ///< Index of LPC register. 0: 2E/2F. 1: 4E/4F
  UINT16                           Address;                   ///< Uart base register
  UINT8                            Irq;                       ///< IRQ
} HYGON_CPM_LPC_UART_TABLE;

/// Local SMI Status

typedef struct {
  UINT8     ApicId;                                                   ///< Core apic id
  UINT32    LocalSmiStatus;                                           ///< SMMFEC4 Local SMI Status
} HYGON_CPM_LOCAL_SMI_STATUS;

/// SMI Data

typedef struct {
  UINT32                        Signature;                            ///< Signature "$SCK"
  UINT8                         SmiCpuCnt;                            ///< Total number of cores
  HYGON_CPM_LOCAL_SMI_STATUS    SmiSts[HYGON_MAX_CPU_CORE_NUMBERS];   ///< X-Total number of cores. Holds "LocalSMIStatus" structure for each core.
} HYGON_CPM_SMI_DATA;

/// HYGON CPM Main Table

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                        ///< Table header
  UINT8                            PlatformName[32];              ///< Platform name
  UINT8                            BiosType;                      ///< BIOS type
  UINT16                           CurrentPlatformId;             ///< Current Platform Id
  UINT32                           PcieMemIoBaseAddr;             ///< PcieMemIoBaseAddr
  UINT32                           AcpiMemIoBaseAddr;             ///< AcpiMemIoBaseAddr
  HYGON_CPM_POINTER                Service;                       ///< Reserved for internal use
  HYGON_CPM_POINTER                TableInRomList;                ///< Reserved for internal use
  HYGON_CPM_POINTER                TableInRamList;                ///< Reserved for internal use
  HYGON_CPM_POINTER                TableInHobList;                ///< Reserved for internal use
  HYGON_CPM_POINTER                HobTablePtr;                   ///< Reserved for internal use

  UINT8                            ExtClkGen;                     ///< External ClkGen Config. 0x00~0x7F
  UINT8                            UnusedGppClkOffEn;             ///< Config to turn off unused GPP clock
  UINT8                            LpcUartEn;                     ///< LpcUartEn
} HYGON_CPM_MAIN_TABLE;

/// Header of CPM Hob table

typedef struct {
  HYGON_CPM_POINTER    HeaderPtr;                             ///< Table header
  HYGON_CPM_POINTER    MainTablePtr;                          ///< Pointer of CPM main table
  UINT32               Revision;                              ///< CPM Revision
  UINT32               BufferItem;                            ///< Available Table Number
  UINT32               BufferOffset;                          ///< Offset of Available Buffer
  UINT32               BufferSize;                            ///< Size of Available Table
} HYGON_CPM_HOB_HEADER;

/// Structure of PCI PFA
typedef union {
  UINT16    Raw;                                              ///< PCI Pfa value
  struct {
    ///<
    UINT8    Function : 3;                                    ///< PCI Function Number
    UINT8    Device   : 5;                                    ///< PCI Device Number
    UINT8    Bus;                                             ///< PCI Bus Number
  }                               Pfa;                        ///< PCI Pfa
} HYGON_CPM_PCI_PFA;

#pragma pack(push)
#pragma pack(1)

/// CPM NV Data Table
typedef struct {
  UINT32    CpmVersion;                                       ///< CPM Revision
  UINT32    CpmPcieMmioBaseAddr;                              ///< PcieMmioBaseAddress
  UINT32    CpmAcpiMmioBaseAddr;                              ///< AcpiMmioBaseAddress
  UINT8     CpmSbChipId;                                      ///< SbChipId
  UINT8     CpmSbStrap;                                       ///< SbStrap
  UINT8     CpmChipId[2];                                     ///< ChipId
  UINT8     CpmEcRamGpioBaseOffset;                           ///< EcRamGpioBaseOffset
  UINT16    CpmSwSmiPort;                                     ///< SwSmiPort
  UINT8     CpmSwSmiCmdSetAutoUmaMode;                        ///< SwSmiCmd to set Auto UMA Allocation
  UINT32    CpmUmaSize;                                       ///< UMA Size

  UINT32    CpmAtrmRomSize;                                   ///< VBIOS image size
  UINT8     CpmAtrmRomImage[0x10000];                         ///< VBIOS image

  UINT32    CpmMainTable;                                     ///< Offset of HYGON_CPM_MAIN_TABLE
  UINT32    CpmDeviceDetectionTable;                          ///< Offset of HYGON_CPM_GPIO_DEVICE_DETECTION_TABLE
  UINT32    CpmDeviceResetTable;                              ///< Offset of HYGON_CPM_GPIO_DEVICE_RESET_TABLE
  UINT32    CpmDevicePowerTable;                              ///< Offset of HYGON_CPM_GPIO_DEVICE_POWER_TABLE
  UINT32    CpmExpressCardTable;                              ///< Offset of HYGON_CPM_EXPRESS_CARD_TABLE
  UINT32    CpmPcieClockTable;                                ///< offset of HYGON_CPM_PCIE_CLOCK_TABLE
  UINT32    CpmOtherHotplugCardTable;                         ///< offset of HYGON_CPM_OTHER_HOTPLUG_CARD_TABLE
  UINT32    CpmCoreTopologyTable;                             ///< offset of HYGON_CPM_CORE_TOPOLOGY_TABLE
} HYGON_CPM_NV_DATA_STRUCT;

#pragma pack(pop)

/// Structure for FCH Strap Data

typedef struct {
  UINT32    ImcEnable     : 1;                                ///< Imc status. 0: Disable, 1: Enable
  UINT32    InternalClock : 1;                                ///< Internal clock status. 0: 25Mhz crystal clock, 1: 100Mhz PCI-E clock
  UINT32    S5PlusSupport : 1;                                ///< S5+ support. 0: not support. 1: support.
} HYGON_CPM_STRAP_SETTING;

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
  UINT8    Cpu;               ///< CPU/APU Chip Id
  UINT8    Sb;                ///< SB Chip Id
  UINT8    Reserved[6];       ///<
} HYGON_CPM_CHIP_ID;

/// System DIMM map table
typedef struct {
  UINT8    Socket;                     ///< DIMM Socket ID 0-3
  UINT8    Cdd;                        ///< DIMM CDD ID 0-3
  UINT8    UMCchannel;                 ///< DIMM UMCchannel ID 0-2
  UINT8    Dimm;                       ///< DIMM number 0-1
  UINT8    LogicChannel;               ///< DIMM logic channel ID 0-MAX
  CHAR8    ChannelName;                ///< DIMM Channel name base on platform
} HYGON_DIMM_DESCRIPTOR;

typedef struct {
  CHAR8    T17DeviceLocatorInfo[20];             // byo230918 -
  UINT8    SocketLocator;
  UINT8    CddLocator;
  UINT8    UmcChlLocator;
  UINT8    DimmLocator;
  UINT8    ChannelNameLocator;
} T17_DEVICE_LOCATOR;

typedef struct {
  CHAR8    T17BankLocatorInfo[13];
  UINT8    SocketLocator;
  UINT8    CddLocator;
  UINT8    UmcChlLocator;
  UINT8    DimmLocator;
  UINT8    ChannelNameLocator;
} T17_BANK_LOCATOR;

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                                ///< Table header
  T17_DEVICE_LOCATOR               T17DeviceLocator;
  T17_BANK_LOCATOR                 T17BankLocator;
  HYGON_DIMM_DESCRIPTOR            DIMM[HYGON_PLATFORM_MAX_DIMM_SUPPORT]; ///< DIMM MAP Descriptor List
} HYGON_CPM_DIMM_MAP_TABLE;

#define FLAG_FORCE             BIT0
#define FLAG_SPECIFIED         BIT1
#define FLAG_THROW             BIT2
#define FLAG_USED              BIT3
#define FLAG_SEARCHED          BIT4           
#define EQ_TABLE_END           BIT31
#define MAX_EQ_CFG_ITEM        100
#define TX_EQ(pre_cursor, main_cursor, post_cursor)    (UINT32)((pre_cursor << 16) + (main_cursor << 8) + post_cursor)
#define TX_PRESET(preset_value)                        (UINT32)preset_value
#define GET_PRE_CURSOR(eq_value)                       (UINT8)((eq_value >> 16) & 0xFF)
#define GET_MAIN_CURSOR(eq_value)                      (UINT8)((eq_value >> 8) & 0xFF)
#define GET_POST_CURSOR(eq_value)                      (UINT8)(eq_value & 0xFF)
#define PER_PORT                                        TRUE
#define PER_LANE                                        FALSE

#define EQ_TYPE_MASK      0xF00

typedef enum{
  // PCIe
  PCIE_EQ_TYPE = 0,
  //PCIe EQ Configuration After training
  PCIE_GEN3_TX_FORCE_DS_CURSOR,
  PCIE_GEN4_TX_FORCE_DS_CURSOR,
  PCIE_GEN5_TX_FORCE_DS_CURSOR,
  PCIE_GEN3_TX_FORCE_US_CURSOR,
  PCIE_GEN4_TX_FORCE_US_CURSOR,
  PCIE_GEN5_TX_FORCE_US_CURSOR,
  PCIE_GEN3_TX_FORCE_US_PRESET,
  PCIE_GEN4_TX_FORCE_US_PRESET,
  PCIE_GEN5_TX_FORCE_US_PRESET,
  PCIE_RX_SSC_OFF_PHUG1,
  PCIE_RX_SSC_OFF_FRUG1,
  PCIE_GEN5_RX_ICTRL,
  PCIE_GEN5_RX_FPK,
  //PCIe EQ Configuration Before training
  PCIE_EQ_CFG_BEFORE_TRAINING = 0X80,
  PCIE_GEN3_TX_FORCE_DS_CURSOR_BEFORE_TRAINING,
  PCIE_GEN4_TX_FORCE_DS_CURSOR_BEFORE_TRAINING,
  PCIE_GEN5_TX_FORCE_DS_CURSOR_BEFORE_TRAINING,
  PCIE_GEN3_TX_FORCE_US_PRESET_BEFORE_TRAINING,
  PCIE_GEN4_TX_FORCE_US_PRESET_BEFORE_TRAINING,
  PCIE_GEN5_TX_FORCE_US_PRESET_BEFORE_TRAINING,
  PCIE_GEN3_TX_FORCE_DS_INITIAL_PRESET,
  PCIE_GEN4_TX_FORCE_DS_INITIAL_PRESET,
  PCIE_GEN5_TX_FORCE_DS_INITIAL_PRESET,
  PCIE_GEN3_TX_FORCE_US_INITIAL_PRESET,
  PCIE_GEN4_TX_FORCE_US_INITIAL_PRESET,
  PCIE_GEN5_TX_FORCE_US_INITIAL_PRESET,
  PCIE_RX_TREM_CTRL,
  PCIE_RX_TERM_OFFSET,
  PCIE_TX_TERM_CTRL,
  PCIE_TXUP_TERM_OFFSET,
  PCIE_TXDN_TERM_OFFSET,
  PCIE_RX_EQ_AFE_CONFIG,
  PCIE_RX_AFE_OVER,

  // USB
  USB_EQ_TYPE = 0X100,
  USB2_TX_SWING,
  USB2_TX_PREEMPHASIS,
  USB3_TX_VBOOST,
  USB3_GEN1_TX_EQ_LEVEL,
  USB3_GEN2_TX_EQ_LEVEL,

  // SATA
  SATA_EQ_TYPE = 0X200,
  SATA_EQ_GEN1_CURSOR,
  SATA_EQ_GEN2_CURSOR,
  SATA_EQ_GEN3_CURSOR,
  
  //end
  EQ_TYPE_END
}EQ_CFG_TYPE;

typedef struct {
  // Common
  UINT32       Flag;
  EQ_CFG_TYPE  Type;
  UINT8        Socket;
  UINT8        PhysicalIodId;
  UINT8        Nbio;

  // PCIe EQ
  UINT32       VidDid;
  UINT32       SubSysVidDid;
  UINT8        StartLane;
  UINT8        EndLane;
  BOOLEAN      PerPort;
  UINT32       CfgValue[16];

  // USB EQ
  UINT8        UsbPort;
  UINT8        UsbCfgValue[3];

  // SATA EQ
  UINT8        SataPort;
  UINT8        SataCfgValue[3];
  
}HYGON_EQ_CFG;

/// EQ config table
typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER     Header;                           ///< Table header
  HYGON_EQ_CFG                      EqCfgList[MAX_EQ_CFG_ITEM];       ///< Device config list
} HYGON_CPM_EQ_CONFIG_TABLE;

typedef enum {
  DE_EMPHASIS_3_5,
  DE_EMPHASIS_6,
  MAX_SUPPORTED_LEVEL
} USB3_TX_EQ_LEVEL;

typedef struct {
  USB3_TX_EQ_LEVEL  TxEqLevel;
  UINT8             RegisterPreemph;
  UINT8             RegisterMainCursor;
  UINT8             RegisterPreCursor;
  UINT8             RegisterPostCursor;
} USB3_TX_EQ_LEVEL_TRANSLATION;

typedef struct {
  HYGON_CPM_TABLE_COMMON_HEADER    Header;                                ///< Table header
  UINT32                           Verbtable[HYGON_CODEC_VERBTABLE_SIZE]; ///< Verbtable Descriptor List
} HYGON_CPM_CODEC_VERBTABLE_TABLE;

#pragma pack(pop)

#endif
