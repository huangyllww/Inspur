/* $NoKeywords:$ */
/**
 * @file
 *
 * Gnb Cxl Data Hob GUID definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  CxlLib
 * @e \$Revision: 309090 $   @e \$Date: 2022-11-07 19:57:05 +0800 (Mon, 7 Nov 2022) $
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
#ifndef _GNB_CXL_HOB_INFO_H_
#define _GNB_CXL_HOB_INFO_H_

#define _GNB_CXL_HOB_INFO_GUID \
{ \
  0x876ceb3e, 0x3109, 0x4897, 0x30, 0x92, 0xa1, 0x42, 0xa1, 0x7a, 0x54, 0x87 \
}

extern EFI_GUID gHygonCxlInfoHobGuid;

#define  MAX_CXL_COUNT    16
#define  MAX_RCEC_COUNT   16

#pragma pack (push, 1)

typedef struct {
  UINT8               SocketId;
  UINT8               LogicalDieId;
  UINT8               PhysicalDieId;
  UINT8               RbIndex;
  UINT8               RbBusNumber;
  UINT8               StartRciepBus;
  UINT8               EndRciepBus;
} RCEC_INFO;

typedef struct {
  UINT8               Cxl11Index;   ///System CXL index
  UINT8               SocketId;
  UINT8               LogicalDieId;
  UINT8               PhysicalDieId;
  UINT8               RbIndex;  
  UINT32              DspRcrb;     ///< Downstream Port RCRB address
  UINT32              UspRcrb;     ///< Upstream Port RCRB address
  UINT32              DspMemBar0;  ///< Downstream port MEMBAR0
  UINT32              UspMemBar0;  ///< Upstream port MEMBAR0
  PCI_ADDR            PciAddr;     ///< PCI address of the CXL RCiEP
  PCI_ADDR            ParentPciAddr;
  UINT8               PortWidth;   ///< Lane width of the port
  UINT8               InitState;
} CXL11_INFO;

typedef enum {
  CXL_NOT_CONFIGURED = 0,
  CXL_CONFIGURED = 1,
} CXL_INIT_STATE;

typedef struct {
    UINT32            Cxl11Count;
    CXL11_INFO        Cxl11Info[MAX_CXL_COUNT];
    UINT32            RcecCount;
    RCEC_INFO         Rcec[MAX_RCEC_COUNT];
} CXL_INFO_HOB_DATA;

#pragma pack (pop)

#endif
