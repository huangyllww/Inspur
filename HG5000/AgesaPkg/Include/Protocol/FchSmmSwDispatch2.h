/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD FCH SMM SW Dispacther Protocol
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AMD FCH UEFI Drivers
 * @e sub-project:  Protocols
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/******************************************************************************
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
****************************************************************************/
#ifndef _FCH_SMM_SW_DISPATCH2_PROTOCOL_H_
#define _FCH_SMM_SW_DISPATCH2_PROTOCOL_H_

#include <Pi/PiSmmCis.h>

extern EFI_GUID gFchSmmSwDispatch2ProtocolGuid;

typedef struct _FCH_SMM_SW_DISPATCH2_PROTOCOL FCH_SMM_SW_DISPATCH2_PROTOCOL;

/// FCH SW SMM Register Context structure
typedef struct {
  UINTN AmdSwValue;                         ///< Soft SMI value
  UINT8 Order;                              ///< Priority 0-Highest (reserved), 0xFF-Lowest (reserved)
} FCH_SMM_SW_REGISTER_CONTEXT;

/// SMM Software SMI Context
typedef struct {
  UINTN    SwSmiCpuIndex;                   ///< 0-based CPU ID
  UINT8    CommandPort;                     ///< SMI Command port
  UINT8    DataPort;                        ///< SMI Data Port
} FCH_SMM_SW_CONTEXT;


typedef
EFI_STATUS
(EFIAPI *FCH_SMM_SW_HANDLER_ENTRY_POINT2) (
  IN       EFI_HANDLE                          DispatchHandle,
  IN CONST FCH_SMM_SW_REGISTER_CONTEXT         *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                  *SwContext,
  IN OUT   UINTN                               *SizeOfSwContext
);

typedef
EFI_STATUS
(EFIAPI *FCH_SMM_SW_DISPATCH2_REGISTER) (
  IN CONST FCH_SMM_SW_DISPATCH2_PROTOCOL       *This,
  IN       FCH_SMM_SW_HANDLER_ENTRY_POINT2     DispatchFunction,
  IN CONST FCH_SMM_SW_REGISTER_CONTEXT         *SwRegisterContext,
     OUT   EFI_HANDLE                          *DispatchHandle
);

typedef
EFI_STATUS
(EFIAPI *FCH_SMM_SW_DISPATCH2_UNREGISTER) (
  IN CONST FCH_SMM_SW_DISPATCH2_PROTOCOL       *This,
  IN       EFI_HANDLE                          DispatchHandle
);

struct  _FCH_SMM_SW_DISPATCH2_PROTOCOL {
  FCH_SMM_SW_DISPATCH2_REGISTER    Register;
  FCH_SMM_SW_DISPATCH2_UNREGISTER  UnRegister;
  UINTN                            MaximumSwiValue;
};


#endif
