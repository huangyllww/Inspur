/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Ccx Down Core Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Lib
 *
 */
/*
 ******************************************************************************
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "HGPI.h"
#include "cpuRegisters.h"
#include "Filecode.h"
#include "PiPei.h"
#include "Library/HygonBaseLib.h"
#include "Library/CcxDownCoreLib.h"
#include "Library/PeiServicesTablePointerLib.h"
#include <Library/PciLib.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  LIBRARY_CCXDOWNCOREXVLIB_CCXDOWNCOREXVLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
#define CCX_XV_DOWN_CORE_REG_MASK  0xF

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/

/**
 *  Set down core register
 *
 *
 *  @param[in] CcxDownCoreMode - Down core mode
 *  @param[in] StdHeader      - Config handle for library and services.
 *
 */
VOID
CcxDownCore (
  IN       CCX_DOWN_CORE_MODE_HYGX CcxDownCoreMode,
  IN OUT   HYGON_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32            DownCoreReg;
  UINT32            LocalPciRegister;
  UINTN             PciAddress;
  EFI_PEI_SERVICES  **PeiServices;

  IDS_HDT_CONSOLE (CPU_TRACE, "  Down Core Mode %d\n", CcxDownCoreMode);
  DownCoreReg = 0;
  switch (CcxDownCoreMode) {
    case CCX_DOWN_CORE_AUTO_HYGX:
      return;
      break;
    case CCX_DOWN_CORE_1_0:
      DownCoreReg = 0xE;
      break;
    case CCX_DOWN_CORE_1_1:
      DownCoreReg = 0xA;
      break;
    case CCX_DOWN_CORE_2_0:
      DownCoreReg = 0xC;
      break;
    default:
      return;
      break;
  }

  PciAddress = MAKE_SBDFO (0, 0, 0x18, FUNC_3, DOWNCORE_CTRL);
  LocalPciRegister = PciRead32 (PciAddress);

  if ((LocalPciRegister & CCX_XV_DOWN_CORE_REG_MASK) == DownCoreReg) {
    return;
  } else {
    PciWrite32 (PciAddress, DownCoreReg);
    IDS_HDT_CONSOLE (CPU_TRACE, "  Set down core register %x, and issue warm reset\n", DownCoreReg);
    PeiServices = (EFI_PEI_SERVICES **)GetPeiServicesTablePointer ();
    ASSERT (PeiServices != NULL);
    (**PeiServices).ResetSystem (PeiServices);
  }
}

/*---------------------------------------------------------------------------------------
 *                           L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */
