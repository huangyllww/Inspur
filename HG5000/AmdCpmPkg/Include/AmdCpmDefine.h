/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM structures and definitions
 *
 * Contains AMD CPM Default Definitions
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

#ifndef _AMD_CPM_DEFINE_H_
#define _AMD_CPM_DEFINE_H_

#define AMD_CPM_PLATFORM_ID_AUTO                  0xFFFF
#ifndef AMD_CPM_PLATFORM_ID_DEFAULT
  #define AMD_CPM_PLATFORM_ID_DEFAULT             AMD_CPM_PLATFORM_ID_AUTO
#endif

#ifndef CPM_PLATFORM_ID_OVERRIDE
#define CPM_PLATFORM_ID_OVERRIDE                  0xFFFF
#endif

#ifndef AMD_PLATFORM_ID_TABLE_SIZE
  #define AMD_PLATFORM_ID_TABLE_SIZE              0x08
#endif

#ifndef AMD_PLATFORM_ID_CONVERT_TABLE_SIZE
  #define AMD_PLATFORM_ID_CONVERT_TABLE_SIZE      0x10
#endif

#ifndef AMD_PRE_INIT_SIZE
  #define AMD_PRE_INIT_SIZE                       0x10
#endif

#ifndef AMD_CPM_CORE_TOPOLOGY_SIZE
  #define AMD_CPM_CORE_TOPOLOGY_SIZE              0x10
#endif

#ifndef AMD_GPIO_ITEM_SIZE
  #define AMD_GPIO_ITEM_SIZE                      0x100
#endif

#ifndef AMD_GEVENT_ITEM_SIZE
  #define AMD_GEVENT_ITEM_SIZE                    0x20
#endif

#ifndef AMD_GPIO_MEM_VOLTAGE_SIZE
  #define AMD_GPIO_MEM_VOLTAGE_SIZE               0x05
#endif

#ifndef AMD_GPIO_VDDP_VDDR_VOLTAGE_SIZE
  #define AMD_GPIO_VDDP_VDDR_VOLTAGE_SIZE         0x03
#endif

#ifndef AMD_GPIO_DEVICE_SIZE
  #define AMD_GPIO_DEVICE_SIZE                    0x64
#endif

#ifndef AMD_GPIO_DEVICE_DETECT_SIZE
  #define AMD_GPIO_DEVICE_DETECT_SIZE             0x20
#endif

#ifndef AMD_GPIO_DEVICE_RESET_SIZE
  #define AMD_GPIO_DEVICE_RESET_SIZE              0x20
#endif

#ifndef AMD_GPIO_DEVICE_POWER_SIZE
  #define AMD_GPIO_DEVICE_POWER_SIZE              0x30
#endif

#ifndef AMD_PCIE_CLOCK_SIZE
  #define AMD_PCIE_CLOCK_SIZE                     0x10
#endif

#ifndef AMD_EXT_CLKGEN_SIZE
  #define AMD_EXT_CLKGEN_SIZE                     0x20
#endif

#ifndef AMD_PCIE_PORT_DESCRIPTOR_SIZE
  #define AMD_PCIE_PORT_DESCRIPTOR_SIZE           0x08
#endif

#ifndef AMD_PCIE_DDI_DESCRIPTOR_SIZE
  #define AMD_PCIE_DDI_DESCRIPTOR_SIZE            0x08
#endif

#ifndef AMD_DXIO_PORT_DESCRIPTOR_SIZE
  #define AMD_DXIO_PORT_DESCRIPTOR_SIZE           0x40
#endif

#ifndef AMD_PCIE_TOPOLOGY_OVERRIDE_SIZE
  #define AMD_PCIE_TOPOLOGY_OVERRIDE_SIZE         0x10
#endif

#ifndef AMD_TABLE_LIST_ITEM_SIZE
  #define AMD_TABLE_LIST_ITEM_SIZE                0x40
#endif

#ifndef AMD_TABLE_LIST_ADDITIONAL_ITEM_SIZE
  #define AMD_TABLE_LIST_ADDITIONAL_ITEM_SIZE     0x10
#endif

#ifndef AMD_TABLE_ADDITIONAL_BUFFER_SIZE
  #define AMD_TABLE_ADDITIONAL_BUFFER_SIZE        0x400
#endif

#ifndef AMD_CPM_ASL_CPU0
  #define AMD_CPM_ASL_CPU0                        0               // If 0, AMD_CPM_CPM_ASL_CPU0 = C000
#endif

#ifndef AMD_CPM_ASL_EC_NAME
  #define AMD_CPM_ASL_EC_NAME                     0               // If 0, AMD_CPM_ASL_EC_NAME = EC0_
#endif

#ifndef AMD_CPM_ASL_EC_MUTEX
  #define AMD_CPM_ASL_EC_MUTEX                    0               // If 0, AMD_CPM_ASL_EC_MUTEX = M042
#endif

#ifndef AMD_CPM_ASL_LPC_NAME
  #define AMD_CPM_ASL_LPC_NAME                    0               // If 0, AMD_CPM_ASL_LPC_NAME = LPC0
#endif

#ifndef AMD_CPM_ASL_OKEC_NAME
  #define AMD_CPM_ASL_OKEC_NAME                   0               // If 0, AMD_CPM_ASL_OKEC_NAME = OKEC
#endif

#ifndef AMD_CPM_ASL_OEM_CALLBACK
  #define AMD_CPM_ASL_OEM_CALLBACK                0               // If 0, AMD_CPM_ASL_OEM_CALLBACK = MOEM
#endif

#ifndef AMD_CPM_ASL_OEM_MLIB
  #define AMD_CPM_ASL_OEM_MLIB                    0               // If 0, AMD_CPM_ASL_OEM_MLIB = MLIB
#endif

#ifndef AMD_CPM_ACPI_VERSION_SUPPORT
  #define AMD_CPM_ACPI_VERSION_SUPPORT            0x0000003Eul    // BIT0: EFI_ACPI_TABLE_VERSION_NONE
                                                                  // BIT1: EFI_ACPI_TABLE_VERSION_1_0B
                                                                  // BIT2: EFI_ACPI_TABLE_VERSION_2_0
                                                                  // BIT3: EFI_ACPI_TABLE_VERSION_3_0
                                                                  // BIT4: EFI_ACPI_TABLE_VERSION_4_0
                                                                  // BIT5: EFI_ACPI_TABLE_VERSION_5_0
#endif

#ifndef AMD_CPM_SSDT_SUPPORT
  #define AMD_CPM_SSDT_SUPPORT                    1               // If 0, SSDT table will not be loaded
#endif

#ifndef AMD_CPM_PCIE_GFX0_NAME_1
  #define AMD_CPM_PCIE_GFX0_NAME_1                '_2BP'          // PB2_
#endif
#ifndef AMD_CPM_PCIE_GFX1_NAME_1
  #define AMD_CPM_PCIE_GFX1_NAME_1                '_3BP'          // PB3_
#endif
#ifndef AMD_CPM_PCIE_GPP0_NAME_1
  #define AMD_CPM_PCIE_GPP0_NAME_1                '_4BP'          // PB4_
#endif
#ifndef AMD_CPM_PCIE_GPP1_NAME_1
  #define AMD_CPM_PCIE_GPP1_NAME_1                '_5BP'          // PB5_
#endif
#ifndef AMD_CPM_PCIE_GPP2_NAME_1
  #define AMD_CPM_PCIE_GPP2_NAME_1                '_6BP'          // PB6_
#endif
#ifndef AMD_CPM_PCIE_GPP3_NAME_1
  #define AMD_CPM_PCIE_GPP3_NAME_1                '_7BP'          // PB7_
#endif

#ifndef AMD_CPM_PCIE_GFX0_NAME_2
  #define AMD_CPM_PCIE_GFX0_NAME_2                '12BP'          // PB21
#endif
#ifndef AMD_CPM_PCIE_GFX1_NAME_2
  #define AMD_CPM_PCIE_GFX1_NAME_2                '22BP'          // PB22
#endif
#ifndef AMD_CPM_PCIE_GPP0_NAME_2
  #define AMD_CPM_PCIE_GPP0_NAME_2                '13BP'          // PB31
#endif
#ifndef AMD_CPM_PCIE_GPP1_NAME_2
  #define AMD_CPM_PCIE_GPP1_NAME_2                '23BP'          // PB32
#endif
#ifndef AMD_CPM_PCIE_GPP2_NAME_2
  #define AMD_CPM_PCIE_GPP2_NAME_2                '33BP'          // PB33
#endif
#ifndef AMD_CPM_PCIE_GPP3_NAME_2
  #define AMD_CPM_PCIE_GPP3_NAME_2                '43BP'          // PB34
#endif
#ifndef AMD_CPM_PCIE_GPP4_NAME_2
  #define AMD_CPM_PCIE_GPP4_NAME_2                '53BP'          // PB35
#endif

#ifndef AMD_CPM_PCIE_GPP0_NAME_3
  #define AMD_CPM_PCIE_GPP0_NAME_3                '0PPG'          // GPP0
#endif
#ifndef AMD_CPM_PCIE_GPP1_NAME_3
  #define AMD_CPM_PCIE_GPP1_NAME_3                '1PPG'          // GPP1
#endif
#ifndef AMD_CPM_PCIE_GPP2_NAME_3
  #define AMD_CPM_PCIE_GPP2_NAME_3                '2PPG'          // GPP2
#endif
#ifndef AMD_CPM_PCIE_GPP3_NAME_3
  #define AMD_CPM_PCIE_GPP3_NAME_3                '3PPG'          // GPP3
#endif
#ifndef AMD_CPM_PCIE_GFX_NAME_3
  #define AMD_CPM_PCIE_GFX_NAME_3                 '_XFG'          // GFX_
#endif

#ifndef AMD_CPM_PCIE_GPP0_NAME_4
  #define AMD_CPM_PCIE_GPP0_NAME_4                '0PPG'          // GPP0
#endif
#ifndef AMD_CPM_PCIE_GPP1_NAME_4
  #define AMD_CPM_PCIE_GPP1_NAME_4                '1PPG'          // GPP1
#endif
#ifndef AMD_CPM_PCIE_GPP2_NAME_4
  #define AMD_CPM_PCIE_GPP2_NAME_4                '2PPG'          // GPP2
#endif
#ifndef AMD_CPM_PCIE_GPP3_NAME_4
  #define AMD_CPM_PCIE_GPP3_NAME_4                '3PPG'          // GPP3
#endif
#ifndef AMD_CPM_PCIE_GPP4_NAME_4
  #define AMD_CPM_PCIE_GPP4_NAME_4                '4PPG'          // GPP4
#endif
#ifndef AMD_CPM_PCIE_GPP5_NAME_4
  #define AMD_CPM_PCIE_GPP5_NAME_4                '5PPG'          // GPP5
#endif
#ifndef AMD_CPM_PCIE_GPP6_NAME_4
  #define AMD_CPM_PCIE_GPP6_NAME_4                '6PPG'          // GPP6
#endif
#ifndef AMD_CPM_PCIE_GPP7_NAME_4
  #define AMD_CPM_PCIE_GPP7_NAME_4                '7PPG'          // GPP7
#endif

#ifndef AMD_CPM_PCIE_GFX0_NAME_4
  #define AMD_CPM_PCIE_GFX0_NAME_4                '0XFG'          // GFX0
#endif
#ifndef AMD_CPM_PCIE_GFX1_NAME_4
  #define AMD_CPM_PCIE_GFX1_NAME_4                '1XFG'          // GFX1
#endif
#ifndef AMD_CPM_PCIE_GFX2_NAME_4
  #define AMD_CPM_PCIE_GFX2_NAME_4                '2XFG'          // GFX2
#endif
#ifndef AMD_CPM_PCIE_GFX3_NAME_4
  #define AMD_CPM_PCIE_GFX3_NAME_4                '3XFG'          // GFX3
#endif
#ifndef AMD_CPM_PCIE_GFX4_NAME_4
  #define AMD_CPM_PCIE_GFX4_NAME_4                '4XFG'          // GFX4
#endif
#ifndef AMD_CPM_PCIE_GFX5_NAME_4
  #define AMD_CPM_PCIE_GFX5_NAME_4                '5XFG'          // GFX5
#endif
#ifndef AMD_CPM_PCIE_GFX6_NAME_4
  #define AMD_CPM_PCIE_GFX6_NAME_4                '6XFG'          // GFX6
#endif
#ifndef AMD_CPM_PCIE_GFX7_NAME_4
  #define AMD_CPM_PCIE_GFX7_NAME_4                '7XFG'          // GFX7
#endif

#ifndef AMD_CPM_FCH_PCIE_GPP0_NAME
  #define AMD_CPM_FCH_PCIE_GPP0_NAME              '0BPS'          // SPB0
#endif
#ifndef AMD_CPM_FCH_PCIE_GPP1_NAME
  #define AMD_CPM_FCH_PCIE_GPP1_NAME              '1BPS'          // SPB1
#endif
#ifndef AMD_CPM_FCH_PCIE_GPP2_NAME
  #define AMD_CPM_FCH_PCIE_GPP2_NAME              '2BPS'          // SPB2
#endif
#ifndef AMD_CPM_FCH_PCIE_GPP3_NAME
  #define AMD_CPM_FCH_PCIE_GPP3_NAME              '3BPS'          // SPB3
#endif

//
// OFFSET_OF DEFINITION
//
#ifndef OFFSET_OF
  #define OFFSET_OF(type, member) ((UINT8) &(((type*)0)->member))
#endif

//
// GPIO DEFINITION
//
#define GPIO_DEFINITION(gpio, function, output, pullup, sticky) \
{gpio, (function <= 3 ? 0x800 : 0) + (function << 8) + (output < 3 ? (output < 2 ? output : 0x02) : 0x08) + \
((sticky == 1) || (pullup < 3) ? ((sticky == 1 ? 0x10 : 0) + (pullup == 0 ? 0x40 : 0) + \
(pullup == 1 ? 0x20 : 0) + 0x80) : 0)}

//
// Multi Socket/Die GPIO Definition
//
#define GPIO_DEF_V2(socket, die, gpio, function, output, pullup) \
{ (socket << 14) + (die << 10) + gpio, (function <= 3 ? 0x800 : 0) + (function << 8) + \
(output < 3 ? (output < 2 ? output : 0x02) : 0x08) + ((pullup == 0 ? 0x40 : 0) + (pullup == 1 ? 0x20 : 0) + 0x80)}

#define EC_GPIO_DEFINITION(gpio, output) \
{gpio, output}

//
// PROMONTORY GPIO DEFINITION
//
#define PT_GPIO_DEFINITION(gpio, OutEnB, Out, intEnb, intLevelTrigType, intType, intMask ) \
{gpio, OutEnB + (Out << 1) + (intEnb << 2) + (intLevelTrigType << 3) + (intType << 4) + (intMask << 6)}

//
// GEVENT DEFINITION
//
#define GEVENT_DEFINITION(gevent, enable, scitrig, scilevel, smisci, scis0, scimap, smitrig, smicontrol) \
{gevent, enable + (scitrig <= 1 ? scitrig << 1 : 1 << 10) + (scilevel << 2) + (smisci << 3) + (scis0 << 4) + (scimap << 5) + \
(smitrig << 11) + (smicontrol << 12)}

//
// GPIO INTERRUPT DEFINITION
//
#define GPIO_INTERRUPT_DEFINITION(gpio, enable, leveltrig, activelevel, wakecontrol, debouncecontrol, debouncetimerOutunit, debouncetimerOut) \
{gpio | 0x100, debouncetimerOut + (debouncetimerOutunit << 4) + (debouncecontrol << 5) + (leveltrig << 8) + (activelevel << 9) + (enable << 11) + \
(wakecontrol << 13)}

//
// GPIO DEVICE CONFIG DEFINITION
//
#define GPIO_DEVICE_DEFINITION(deviceid, enable, assert, deassert, hotplug) \
{deviceid, enable + (assert << 2) + (deassert << 3) + (hotplug << 4)}

#define CPM_DEVICE_OFF                          0
#define CPM_DEVICE_ON                           1
#define CPM_DEVICE_AUTO                         2

#define CPM_TYPE_GPIO_1                         0
#define CPM_TYPE_GPIO_2                         1
#define CPM_TYPE_GPIO_3                         2
#define CPM_TYPE_GPIO_2_OR                      3
#define CPM_TYPE_GPIO_3_OR                      4

#define CPM_GPIO_PIN(pin, value)    (pin + (value << 16))
#define CPM_EC_GPIO_PIN(pin, value) ((pin | 0x100) + (value << 16))
#define CPM_ECRAM_GPIO_PIN(pin, value) ((pin | 0x200) + (value << 16))

#define CPM_RESET_ASSERT                        0
#define CPM_RESET_DEASSERT                      1
#define CPM_RESET_DELAY                         2

#define CPM_RESET_GPIO                          0
#define CPM_RESET_NA                            0

#define CPM_POWER_OFF                           0
#define CPM_POWER_ON                            1
#define CPM_POWER_SET                           0
#define CPM_POWER_WAIT                          1
#define CPM_POWER_DELAY                         2

// AML Define

#ifndef AML_OPREGION_OP
  #define AML_OPREGION_OP                       0x805B
#endif

//
// BIOS & ACPI SW SMI function number
//
#ifndef CPM_ACPI_RESTORE_SW_SMI
  #define CPM_ACPI_RESTORE_SW_SMI               0xE3
#endif
#ifndef CPM_ACPI_ENABLE_SW_SMI
  #define CPM_ACPI_ENABLE_SW_SMI                0xA0
#endif

#ifndef CPM_ECRAM_BASE_ADDR
  #define CPM_ECRAM_BASE_ADDR                   0x62
#endif

#ifndef CPM_ECRAM_GPIO_OFFSET
  #define CPM_ECRAM_GPIO_OFFSET                 0xAB
#endif

#ifndef CPM_SWSMICMD_SETAUTOUMAMODE
  #define CPM_SWSMICMD_SETAUTOUMAMODE           0xDF
#endif

#ifndef CPM_X86_IO_REMAP_BASE
  #define CPM_X86_IO_REMAP_BASE                 0x000000FDFC000000ull
#endif

#ifndef CPM_PCI_MMIO_BASE
  #define CPM_PCI_MMIO_BASE                     0x0000000080000000ull
#endif


#endif
