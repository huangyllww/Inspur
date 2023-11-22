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

#ifndef _HYGON_PLATFORM_RAS_SMM_DRIVER_H_
#define _HYGON_PLATFORM_RAS_SMM_DRIVER_H_

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmCpu.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/debugLib.h>

#include <Protocol/FchSmmSwDispatch2.h>
#include <Protocol/FchSmmApuRasDispatch.h>
#include <Protocol/HygonRasSmmProtocol.h>
#include <Library/SmnAccessLib.h>
#include <Library/RasElogEventLib.h>           // byo231101 +

#include "HygonRas.h"
#include "HygonCpmRas.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define MSR_PSTATE_0  0xC0010064ul

typedef enum _SMI_MODE {
  INTERRUPT_MODE = 1,
  POLLING_MODE
} SMI_MODE;

/// P-state MSR
typedef union {
  struct {
    ///< Bitfields of P-state MSR
    UINT64    CpuFid_7_0 : 8;          ///< CpuFid[7:0]
    UINT64    CpuDfsId   : 6;          ///< CpuDfsId
    UINT64    CpuVid     : 8;          ///< CpuVid
    UINT64    IddValue   : 8;          ///< IddValue
    UINT64    IddDiv     : 2;          ///< IddDiv
    UINT64               : 31;         ///< Reserved
    UINT64    PstateEn   : 1;          ///< Pstate Enable
  } Field;
  UINT64    Value;
} PSTATE_MSR;

/// BSP/AP MSR Access
typedef struct {
  UINT32     RegisterAddress; ///< MSR Address
  UINT64     RegisterValue;   ///< BSC's MSR Value
  BOOLEAN    IsWrite;         ///< Access Type
} PLAT_RAS_MSR_ACCESS;

#define SMI_BASE       0x200             // DWORD
#define FCH_SMI_REG96  0x96
#define FCH_SMI_REGC3  0xC3

#define SMI_TIMER_ENABLE  BIT15

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

extern HYGON_RAS_SMM_PROTOCOL        *mHygonRasSmmProtocol;
extern PLATFORM_APEI_PRIVATE_BUFFER  *mPlatformApeiData;

extern EFI_GUID  gHygonPlatformApeiDataProtocolGuid;

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

// EFI_STATUS
// EFIAPI
// HygonMcetoSmiCallback (
// IN       EFI_HANDLE                        DispatchHandle,
// IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
// IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
// IN OUT   UINTN                             *SizeOfSwContext
// );

EFI_STATUS
  RasSmmRegisterMceSwSmi (
                          VOID
                          );

EFI_STATUS
  RasSmmRegisterMcePeriodicSmi (
                                VOID
                                );

EFI_STATUS
  RasSmmRegisterEinjSwSmi (
                           VOID
                           );

EFI_STATUS
  RasSmmRegisterNbioSmi (
                         VOID
                         );

VOID
RasSmnRead (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT32              *Value
  );

VOID
RasSmnWrite (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT32              *Value
  );

VOID
RasSmnRW (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT32              AndMask,
  IN       UINT32              OrMask
  );

VOID
RasSmnRead8 (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT8               *Value8
  );

VOID
RasSmnWrite8 (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT8               *Value8
  );

VOID
RasSmnRW8 (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT8               AndMask,
  IN       UINT8               OrMask
  );

UINT64
ConvertToMilliseconds (
  IN       UINT64 TscTicks
  );

#endif //_HYGON_PLATFORM_RAS_SMM_DRIVER_H_
