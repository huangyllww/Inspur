/**
 * @file
 *
 * HYGON Integrated Debug Print Routines
 *
 * Contains all functions related to IDS Debug Print
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  IDS
 */
/*****************************************************************************
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

/*----------------------------------------------------------------------------------------
*                             M O D U L E S    U S E D
*----------------------------------------------------------------------------------------
*/
#include <Uefi.h>
#include <Pi/PiMultiPhase.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/HygonBaseLib.h>
#include <HygonIdsDebugPrintLocal.h>
#include <Filecode.h>

#define FILECODE  LIBRARY_HYGONIDSDEBUGPRINTLIB_HYGONIDSDEBUGPRINTLIB_FILECODE

extern HYGON_IDS_DEBUG_PRINT_INSTANCE  HygonIdsDebugPrintRedirectIoInstance;
extern HYGON_IDS_DEBUG_PRINT_INSTANCE  HygonIdsDebugPrintHdtoutInstance;
extern HYGON_IDS_DEBUG_PRINT_INSTANCE  HygonIdsDebugPrintSerialInstance;

#define MAX_LOCAL_BUFFER_SIZE  512

CONST HYGON_IDS_DEBUG_PRINT_INSTANCE  *mHygonIdsDebugPrintInstances[] =
{
  &HygonIdsDebugPrintRedirectIoInstance,
  &HygonIdsDebugPrintHdtoutInstance,
  &HygonIdsDebugPrintSerialInstance,
  NULL,
};

STATIC BOOLEAN gHygonIdsDebugPrintEnable;                                  // byo230914 +

/**
 *
 *  @param[in,out] Value  - Hex value to convert to a string in Buffer.
 *
 *
 */
VOID
GetHygonIdsDebugPrintInstances (
  IN OUT   CONST HYGON_IDS_DEBUG_PRINT_INSTANCE   ***pIdsHygonIdsDebugPrintInstancesPtr
  )
{
  *pIdsHygonIdsDebugPrintInstancesPtr = &mHygonIdsDebugPrintInstances[0];
}

/**
 *  Check If any print service is enabled.
 *
 *  @param[in] HygonIdsDebugPrintInstances    The Pointer to print service list
 *
 *  @retval       TRUE    At least on print service is enabled
 *  @retval       FALSE   All print service is disabled
 *
 **/
BOOLEAN
HygonIdsDebugPrintCheckSupportAny (
  IN      CONST HYGON_IDS_DEBUG_PRINT_INSTANCE **HygonIdsDebugPrintInstances
  )
{
  BOOLEAN  IsSupported;
  UINTN    i;

  IsSupported = FALSE;
  for (i = 0; HygonIdsDebugPrintInstances[i] != NULL; i++) {
    if (HygonIdsDebugPrintInstances[i]->support ()) {
      IsSupported = TRUE;
    }
  }

  return IsSupported;
}

RETURN_STATUS
EFIAPI
HygonIdsDebugPrintLibConstructor (
  VOID
  )
{
  UINT8  i;

  gHygonIdsDebugPrintEnable = PcdGetBool(PcdHygonIdsDebugPrintEnable);     // byo230914 +

  if (HygonIdsDebugPrintCheckSupportAny (mHygonIdsDebugPrintInstances)) {
    // init input
    for (i = 0; mHygonIdsDebugPrintInstances[i] != NULL; i++) {
      if (mHygonIdsDebugPrintInstances[i]->support ()) {
        mHygonIdsDebugPrintInstances[i]->constructor ();
      }
    }
  }

  return RETURN_SUCCESS;
}

/**
 *  Process debug string and print to the final Physical Layer
 *
 *  @param[in] FilterFlag    - Filter Flag
 *  @param[in] *Format - format string
 *  @param[in] Marker  - Variable parameter
 *
**/
VOID
HygonIdsDebugPrintf (
  IN      UINT64      FilterFlag,
  IN      CONST CHAR8 *Format,
  IN      VA_LIST     Marker
  )
{
  CHAR8                           LocalBuffer[MAX_LOCAL_BUFFER_SIZE];
  UINTN                           OutPutStringLen;
  UINT64                          Filter;
  HYGON_IDS_DEBUG_PRINT_INSTANCE  **HygonIdsDebugPrintInstances;
  UINT8                           i;

  if (!gHygonIdsDebugPrintEnable) {                                        // byo230914 -
    return;
  }

  OutPutStringLen = AsciiVSPrint (LocalBuffer, sizeof (LocalBuffer), Format, Marker);

  GetHygonIdsDebugPrintInstances (
    (const HYGON_IDS_DEBUG_PRINT_INSTANCE ***)&HygonIdsDebugPrintInstances
    );
  if (HygonIdsDebugPrintCheckSupportAny (
        (const HYGON_IDS_DEBUG_PRINT_INSTANCE **)HygonIdsDebugPrintInstances
        )) {
    // init input
    for (i = 0; HygonIdsDebugPrintInstances[i] != NULL; i++) {
      if (HygonIdsDebugPrintInstances[i]->support ()) {
        Filter = PcdGet64 (PcdHygonIdsDebugPrintFilter);
        // Get Customize filter if implemented(Option)
        HygonIdsDebugPrintInstances[i]->customfilter (&Filter);
        if (FilterFlag & Filter) {
          // Print Physical Layer
          HygonIdsDebugPrintInstances[i]->print (&LocalBuffer[0], OutPutStringLen);
        }
      }
    }
  }
}
