/* $NoKeywords:$ */

/**
 * @file
 *
 * FCH DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project   FCH DXE Driver
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

#ifndef _FCH_DXE_H_
#define _FCH_DXE_H_

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>

#include "FchPlatform.h"
#include "../FchTaishanCore/FchCommonCfg.h"
#include <Library/FchDxeLib.h>

#include <Protocol/FchInitProtocol.h>
#include <Protocol/FchMultiFchInitProtocol.h>
#include <Protocol/FabricTopologyServices.h>
//
// Module data structure
//
/// Private data and access defines
typedef struct _FCH_MULTI_FCH_DXE_PRIVATE {
  UINTN                          Signature;             ///< Signature
  FCH_MULTI_FCH_INIT_PROTOCOL    FchMultiFchInit;       ///< Protocol data
  EFI_EVENT                      EventReadyToBoot;      ///< Event related data
} FCH_MULTI_FCH_DXE_PRIVATE;

#define FCH_MULTI_FCH_DXE_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('F', 'M', 'F', 'D')

#define FCH_MULTI_FCH_DXE_PRIVATE_INSTANCE_FROM_THIS(a) \
  CR (a, FCH_MULTI_FCH_DXE_PRIVATE, FchMultiFchInit, FCH_MULTI_FCH_DXE_PRIVATE_DATA_SIGNATURE)

//
// Functions Prototypes
//
EFI_STATUS
EFIAPI
MultiFchDxeHyExInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  );

VOID
FchTSSecondaryFchInitSataDxe (
  IN  UINT8       PhysicalDieId,
  IN  UINT8       SataIndex,
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  );

VOID
FchTSSecondaryFchInitXgbeDxe (
  IN  UINT8       PhysicalDieId,
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  );

VOID
MultiFchInitRtb (
  IN       EFI_EVENT        Event,
  IN       VOID             *Context
  );

VOID
FchTSSecondaryFchInitSataRtb (
  IN  UINT8       PhysicalDieId,
  IN  UINT8       SataIndex,
  IN  UINT8       DieBusNum,
  IN  VOID        *FchDataPtr
  );

#endif // _FCH_DXE_H_
