/* $NoKeywords:$ */

/**
 * @file
 *
 * FCH PEIM
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project   FCH PEIM
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

#ifndef _FCH_PEI_H_
#define _FCH_PEI_H_

#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/FchInitHookLib.h>
#include "FchPlatform.h"
#include "../FchTaishanCore/FchCommonCfg.h"
#include <Ppi/HygonFchInitPpi.h>

#include  <IdsHookId.h>
#include  <Library/HygonIdsHookLib.h>

/**
 *  @page FchPei  HYGON FCH PEI Driver
 *  @section ACPI_States  To define values for each ACPI Sx
 *           state.
 */
#define ACPI_S0       (0 << 10)
#define ACPI_S3       (3 << 10)
#define ACPI_S4       (4 << 10)
#define ACPI_S5       (5 << 10)
#define ACPI_BM_MASK  (7 << 10)

/// Module data structure
typedef struct _FCH_PEI_PRIVATE {
  UINTN                  Signature;          ///< Signature
  HYGON_FCH_INIT_PPI     FchInitPpi;         ///< FCH INIT PPI
  HYGON_CONFIG_PARAMS    StdHdr;             ///< HYGON_CONFIG_PARAMS
} FCH_PEI_PRIVATE;

#define FCH_PEI_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('$', 'F', 'C', 'H')

#define FCH_PEI_PRIVATE_FROM_THIS(a) \
  (FCH_PEI_PRIVATE *)(CR ((a), FCH_PEI_PRIVATE, FchInitPpi, FCH_PEI_PRIVATE_DATA_SIGNATURE))

/// Functions Prototypes
EFI_STATUS
EFIAPI
FchPeiHyGxInit (
  IN       EFI_PEI_FILE_HANDLE    FileHandle,
  IN       CONST EFI_PEI_SERVICES **PeiServices
  );

UINT8
PmIoRead8 (
  IN       EFI_PEI_SERVICES  **PeiServices,
  IN       UINT8             RegIndex
  );

UINT16
PmIoRead16 (
  IN       EFI_PEI_SERVICES  **PeiServices,
  IN       UINT8             RegIndex
  );

EFI_STATUS
FchUpdateBootMode (
  IN       EFI_PEI_SERVICES    **PeiServices
  );

EFI_STATUS
EFIAPI
FchInitXhciOnRecovery (
  IN       HYGON_FCH_INIT_PPI   *This,
  IN       UINT32             XhciRomAddress
  );

EFI_STATUS
EFIAPI
FchInitEhciOnRecovery (
  IN       HYGON_FCH_INIT_PPI   *This,
  IN       UINT32             EhciTemporaryBarAddress
  );

EFI_STATUS
EFIAPI
FchInitSataOnRecovery (
  IN       HYGON_FCH_INIT_PPI   *This,
  IN       UINT32             SataBar0,
  IN       UINT32             SataBar5
  );

EFI_STATUS
EFIAPI
FchInitGppOnRecovery (
  IN      HYGON_FCH_INIT_PPI     *This,
  IN      FCH_GPP_R            *FchGpp
  );

extern EFI_STATUS
FchXhciOnRecovery (
  IN      FCH_PEI_PRIVATE      FchPrivate,
  IN      UINT32               XhciRomAddress
  );

extern EFI_STATUS
FchEhciOnRecovery (
  IN      FCH_PEI_PRIVATE      FchPrivate,
  IN      UINT32               EhciTemporaryBarAddress
  );

extern EFI_STATUS
FchSataOnRecovery (
  IN      FCH_PEI_PRIVATE      FchPrivate,
  IN      UINT32               SataBar0,
  IN      UINT32               SataBar5
  );

extern EFI_STATUS
FchGppOnRecovery (
  IN      FCH_PEI_PRIVATE      FchPrivate,
  IN      FCH_GPP_R            *FchGpp
  );

#ifndef GEC_ROM_SHADOW_ADDRESS
  #define GEC_ROM_SHADOW_ADDRESS  0xFED61000ul            // GecShadowRomBase
#endif
#endif // _FCH_PEI_H_
