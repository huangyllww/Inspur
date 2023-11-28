/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM post code definitions
 *
 * Contains AMD CPM Post Code Definitions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  Include
 * @e \$Revision: 300740 $   @e \$Date: 2014-08-11 17:36:12 -0400 (Mon, 11 Aug 2014) $
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

#ifndef _AMD_CPM_POST_CODE_H_
#define _AMD_CPM_POST_CODE_H_

#ifdef CPM_TP_BASE_VALUE
#define CpmTpBaseValue                        CPM_TP_BASE_VALUE
#else
#define CpmTpBaseValue                        0xC00
#endif

/// The CPM post code

typedef enum {
  CpmTpErrorNoEnoughSizeInHobBuffer =         0x01,           ///< No free table item or no enough size in Hob Buffer
  CpmTpErrorTableNumberOverflow =             0x02,           ///< Table number is greater than AMD_TABLE_LIST_ITEM_SIZE
  CpmTpErrorMainTablePointerInvalid =         0x03,           ///< Main Table pointer is invalid

  CpmTpGpioInitPeimDriverBegin =              0x30,           ///< Begin of GPIO Init PEIM driver
  CpmTpGpioInitPeimDriverEnd =                0x31,           ///< End of GPIO Init PEIM driver
  CpmTpGpioInitPeimResetDeviceBegin =         0x32,           ///< Begin to Reset Device in GPIO Init PEIM driver
  CpmTpGpioInitPeimResetDeviceEnd =           0x33,           ///< End to Reset Device in GPIO Init PEIM driver
  CpmTpGpioInitPeimSetMemVoltageBegin =       0x34,           ///< Begin to Set Mem Voltage in GPIO Init PEIM driver
  CpmTpGpioInitPeimSetMemVoltageEnd =         0x35,           ///< End to Set Mem Voltage in GPIO Init PEIM driver
  CpmTpGpioInitPeimSetVddpVddrVoltageBegin =  0x36,           ///< Begin to Set Vddp/Vddr Voltage in GPIO Init PEIM driver
  CpmTpGpioInitPeimSetVddpVddrVoltageEnd =    0x37,           ///< End to Set Vddp/Vddr Voltage in GPIO Init PEIM driver
  CpmTpGpioInitDxeDriverBegin =               0x38,           ///< Begin of GPIO Init DXE driver
  CpmTpGpioInitDxeDriverEnd =                 0x39,           ///< Begin of GPIO Init DXE driver
  CpmTpGpioInitDxePcieClockInitBegin =        0x3A,           ///< Begin to init PCIe Clock in GPIO Init DXE driver
  CpmTpGpioInitDxePcieClockInitEnd =          0x3B,           ///< Begin to init PCIe Clock in GPIO Init DXE driver
  CpmTpGpioInitSmmDriverBegin =               0x3C,           ///< Begin of GPIO Init SMM driver
  CpmTpGpioInitSmmDriverEnd =                 0x3D,           ///< Begin of GPIO Init SMM driver
  CpmTpPcieInitPeimDriverBegin =              0x3E,           ///< Begin of PCIE Init PEIM driver
  CpmTpPcieInitPeimDriverEnd =                0x3F,           ///< End of PCIE Init PEIM driver
  CpmTpPcieInitDxeDriverBegin =               0x40,           ///< Begin of PCIE Init DXE driver
  CpmTpPcieInitDxeDriverEnd =                 0x41,           ///< End of PCIE Init DXE driver


} AMD_CPM_POST_CODE;

#endif
