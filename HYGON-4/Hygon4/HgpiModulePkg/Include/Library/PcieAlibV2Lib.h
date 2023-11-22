/* $NoKeywords:$ */

/**
 * @file
 *
 * PCIe ALIB
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: GNB
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

#ifndef _PCIEALIBV2LIB_H_
#define _PCIEALIBV2LIB_H_

#pragma pack (push, 1)

/// _ALIB_PORT_DATA structure
typedef struct {
  UINT8    PciePortMaxSpeed;                                ///< Maximum speed for PCIe Port
  UINT8    PciePortAcSpeed;                                 ///< AC power speed for PCIe Port
  UINT8    PciePortDcSpeed;                                 ///< DC power speed for PCIe Port
  UINT8    PciePortCurSpeed;                                ///< Current speed for PCIe Port
  UINT8    PcieSbPort;                                      ///< Sb Port
  UINT8    PcieLinkSafeMode;                                ///< Link Safe Mode
  UINT8    PcieLocalOverrideSpeed;                          ///< Local Override Speed
  UINT8    StartPhyLane;                                    ///< Start PHY lane
  UINT8    EndPhyLane;                                      ///< End PHY lane
  UINT8    StartCoreLane;                                   ///< Start CORE lane
  UINT8    EndCoreLane;                                     ///< End Core lane
  UINT8    PortId;                                          ///< Logical Port ID
  UINT8    LinkHotplug;                                     ///< Hotplug Flags
  UINT8    PciDev;                                          ///< PCI Device Number
  UINT8    PciFun;                                          ///< PCI Function Number
} _ALIB_PORT_DATA;

/// Set size for Port Data
typedef union {
  _ALIB_PORT_DATA    PortData;                              ///< Port data for ALIB
  UINT8              Padding[20];                           ///< Padding size
} ALIB_PORT_DATA;

/// _ALIB_GLOBAL_DATA structure
typedef  struct {
  UINT8    PsppPolicy;                                    ///< PSPP Policy
  UINT8    PcieVidGen1;                                   ///< VID for Gen1
  UINT8    PcieVidGen2;                                   ///< VID for Gen2
  UINT8    PcieVidGen3;                                   ///< VID for Gen3
  UINT8    DpmMask;                                       ///< DPM Mask
  UINT8    NumBoostStates;                                ///< DPM States
  UINT8    DockedTdpHeadroom;                             ///< Docked TDP Headroom
  UINT8    DcTdpEnable;                                   ///< Dc TDP Enable
} _ALIB_GLOBAL_DATA;

/// Set size for Global Data
typedef union {
  _ALIB_GLOBAL_DATA    Data;                                ///< Global data struct for ALIB
  UINT8                Padding[32];                         ///< Padding size
} ALIB_GLOBAL_DATA;

/// ALIB_GLOBAL_DATA structure
typedef struct {
  ALIB_GLOBAL_DATA    Data;                                 ///< Global data struct for ALIB
  ALIB_PORT_DATA      PortData[64];                         ///< Padding size (16 ports per die, 4 dies)
} ALIB_DATA;

#pragma pack (pop)

HGPI_STATUS
PcieAlibV2Feature (
  IN       HYGON_CONFIG_PARAMS    *StdHeader
  );

#endif
