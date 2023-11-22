/* $NoKeywords:$ */

/**
 * @file
 *
 * Service procedure to initialize Integrated Info Table
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

#ifndef _GFXENUMCONNECTORSLIB_H_
#define _GFXENUMCONNECTORSLIB_H_

/// External connector info
typedef struct {
  PCIE_CONNECTOR_TYPE    ConnectorType;       ///<
  UINT8                  DisplayDeviceEnum;   ///<
  UINT16                 ConnectorEnum;       ///<
  UINT16                 EncoderEnum;         ///<
  UINT8                  ConnectorIndex;      ///<
} EXT_CONNECTOR_INFO;

/// External display device info
typedef struct {
  UINT8     DisplayDeviceEnum;                ///<
  UINT8     DeviceIndex;                      ///<
  UINT16    DeviceTag;                        ///<
  UINT16    DeviceAcpiEnum;                   ///<
} EXT_DISPLAY_DEVICE_INFO;

/// Connector enumeration info
typedef struct {
  HGPI_STATUS           Status;              ///<
  UINT8                 DisplayDeviceEnum;   ///<
  UINT8                 RequestedPriorityIndex; ///<
  UINT8                 CurrentPriorityIndex; ///<
  PCIe_ENGINE_CONFIG    *Engine;              ///<
} CONNECTOR_ENUM_INFO;

VOID
GfxIntegratedCopyDisplayInfo (
  IN       PCIe_ENGINE_CONFIG             *Engine,
  OUT      EXT_DISPLAY_PATH               *DisplayPath,
  OUT      EXT_DISPLAY_PATH               *SecondaryDisplayPath,
  IN       GFX_PLATFORM_CONFIG            *Gfx
  );

HGPI_STATUS
GfxIntegratedEnumerateAllConnectors (
  OUT      EXT_DISPLAY_PATH                 *DisplayPathList,
  IN OUT   PCIe_PLATFORM_CONFIG             *Pcie,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  );

EXT_CONNECTOR_INFO *
GfxIntegratedExtConnectorInfo (
  IN       UINT8                         ConnectorType
  );

EXT_DISPLAY_DEVICE_INFO *
GfxIntegratedExtDisplayDeviceInfo (
  IN       UINT8                         DisplayDeviceEnum,
  IN       UINT8                         DisplayDeviceIndex
  );

#endif
