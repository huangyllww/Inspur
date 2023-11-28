/* $NoKeywords:$ */
/**
 * @file
 *
 * Base Fabric Pstates Lib implementation for DF1.0
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/FabricPstatesLib.h>
#include <Filecode.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_FABRICPSTATESZPLIB_FABRICPSTATESZPLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

BOOLEAN
FabricPstatesAreAllSocketsInAUnifiedDomain (
  VOID
  )
{
  return TRUE;
}

BOOLEAN
FabricPstatesAreAllDieOnSocketInAUnifiedDomain (
  IN       UINTN  Socket
  )
{
  return TRUE;
}

BOOLEAN
FabricPstatesIsSwitchingEnabledOnDie (
  IN       UINTN  Socket,
  IN       UINTN  Die
  )
{
  // Read F5x174[NbPstateDis]
  return TRUE;
}

UINTN
FabricPstatesGetNumberOfPstatesOnDie (
  IN       UINTN  Socket,
  IN       UINTN  Die
  )
{
  // Read F5
  return 4;
}

VOID
FabricPstatesGetPstateInfo (
  IN       UINTN               Socket,
  IN       UINTN               Die,
  IN       UINTN               Pstate,
     OUT   FABRIC_PSTATE_INFO  *Info
  )
{
  Info->Frequency = Pstate * 800;
  Info->Voltage = 1550 - (Pstate * 200);
  Info->Power = 400 * Pstate;
  Info->AssociatedMstate = Pstate != 3 ? 0 : 1;
}

BOOLEAN
FabricPstatesForcePstateOnDie (
  IN       UINTN  Socket,
  IN       UINTN  Die,
  IN       UINTN  Pstate
  )
{
  // Check for previous force and save pstatectrl if none?

  // Check SmuCfgLock and return FALSE if appropriate

  // Force
  return TRUE;
}

VOID
FabricPstatesReleaseForceOnDie (
  IN       UINTN  Socket,
  IN       UINTN  Die
  )
{
  // Restore pstatectrl
}

UINTN
FabricPstatesGetCurrentPstateOnDie (
  IN       UINTN  Socket,
  IN       UINTN  Die
  )
{
  // Check F5
  return 0;
}
