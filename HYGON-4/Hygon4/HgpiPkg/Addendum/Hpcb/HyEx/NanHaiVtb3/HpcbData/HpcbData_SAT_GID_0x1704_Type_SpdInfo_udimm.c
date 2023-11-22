/* $NoKeywords:$ */

/**
 * @file
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
 *
 ***************************************************************************/
// This file includes code originally published under the following license.

/** @file
  This module produces the EFI_PEI_S3_RESUME_PPI.
  This module works with StandAloneBootScriptExecutor to S3 resume to OS.
  This module will excute the boot script saved during last boot and after that,
  control is passed to OS waking up handler.

  Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available under the terms and conditions
  of the BSD License which accompanies this distribution.  The
  full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "MyPorting.h"
#include <MiscMemDefines.h>
#include <HPCB.h>
#include <HpcbCustomizedDefinitions.h>

CHAR8  mDummyBuf;

HPCB_TYPE_DATA_START_SIGNATURE ();
HPCB_TYPE_HEADER  HpcbTypeHeader = {
  HPCB_GROUP_MEMORY,
  HPCB_MEM_TYPE_SPD_INFO,
  (sizeof (HPCB_TYPE_HEADER) + sizeof (PSP_SPD_STRUCT)),
  0,
  0,
  0
};

/// PSP_SPD_STRUCT is too big, redefine myPSP_SPD_STRUCT
typedef struct _myPSP_SPD_STRUCT {
  UINT32            TotalDimms;
  SPD_DEF_STRUCT    SpdData[4];
} myPSP_SPD_STRUCT;

myPSP_SPD_STRUCT  memPspSpd = {
  4,                                 // < TotalDimms - Total number of DIMMs in the system
  {
    TRUE,                               // < SpdValid - Indicates that the SPD is valid
    TRUE,                               // < DimmPresent - Indicates that the DIMM is present and Data is valid
    0,                                  // < PageAddress - Indicates that the DIMM is present and Data is valid
    FALSE,                              // < Indicates this DIMM is NVDIMM
    0,                                  // < SPDManufacturerIDCode;    
    0,                                  // < SPDDeviceType;
    0,                                  // < SPDRevisionNumber;
    0,                                  // < PMIC0ManufacturerIDCode;  /// PMIC 0 Manufacturer ID
    0,                                  // < PMIC0DeviceType;
    0,                                  // < PMIC0RevisionNumber;
    0,                                  // < PMIC1ManufacturerIDCode;  /// PMIC 1 Manufacturer ID
    0,                                  // < PMIC1DeviceType;
    0,                                  // < PMIC1RevisionNumber;
    0,                                  // < PMIC2ManufacturerIDCode;  /// PMIC 2 Manufacturer ID
    0,                                  // < PMIC2DeviceType;
    0,                                  // < PMIC2RevisionNumber;
    0,                                  // < ThermalSensorManufacturerIDCode;   /// Thermal Sensor Manufacturer ID
    0,                                  // < ThermalSensorDeviceType;
    0,                                  // < ThermalSensorRevisionNumber;
    0,                                  // < RCDManufacturerIDCode;             /// Registering CLock Driver Manufacturer ID
    0,                                  // < RCDDeviceType;
    0,                                  // < RCDRevisionNumber;
    0,                                  // < DataBufferManufacturerIDCode;      /// Data Buffer Manufacturer ID
    0,                                  // < DBDeviceType;
    0,                                  // < DBRevisionNumber;
    0,                                  // < ModuleManufacturersIDCode;         /// Module Manufacturer's ID
    0,                                  // < ModuleRevisionCode;
    0,                                  // < Reserved1;
    0,                                  // < DramManufacturersIDCode;           /// DRAM Manufacturer's ID
    0,                                  // < DramStepping;
    0,                                  // < Reserved0;
    0xE0,                               // < Address - Indicates the SMBUS address of the DIMM
    { 0, 0, 0, 0 },                     // < SMBUS Mux Info
    DDR5_TECHNOLOGY,                    // < Technology - Indicates the type of technology supported
    UDIMM_PACKAGE,                      // < Package - Package type
    0,                                  // < SocketNumber - Socket Number
    // 1 for DHM1, 3 for SL2
    2,                                   // < ChannelNumber - Channel Number
    1,                                   // < DimmNumber - DIMM Number
    0,                                   // < Reserved
    /// DDR5-1333-2G
    {
      // 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
      0x23, 0x11, 0x0C, 0x02, 0x85, 0x21, 0x00, 0x08, 0x00, 0x60, 0x00, 0x03, 0x01, 0x0B, 0x80, 0x00,                    // 0
      0x00, 0x00, 0x07, 0x0D, 0xF8, 0x0F, 0x00, 0x00, 0x6E, 0x6E, 0x6E, 0x11, 0x00, 0x6E, 0xF0, 0x0A,                    // 1
      0x20, 0x08, 0x00, 0x05, 0x00, 0xA8, 0x1B, 0x28, 0x28, 0x00, 0x78, 0x00, 0x14, 0x3C, 0x00, 0x00,                    // 2
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x36, 0x16, 0x36,                    // 3
      0x16, 0x36, 0x16, 0x36, 0x16, 0x36, 0x16, 0x36, 0x16, 0x36, 0x16, 0x36, 0x16, 0x36, 0x00, 0x00,                    // 4
      0x35, 0xB3, 0x00, 0x85, 0xD9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                    // 5
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                    // 6
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9C, 0xB5, 0x00, 0x00, 0x00, 0x00, 0xE7, 0xD6, 0x36, 0x48,                    // 7
      0x11, 0x11, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                    // 8
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                    // 9
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                    // a
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                    // b
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                    // c
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                    // d
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                    // e
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4D, 0x57,                    // f
    },
  },
};

HPCB_TYPE_DATA_END_SIGNATURE ();

int
main (
  IN int argc, IN char *argv[]
  )
{
  return 0;
}
