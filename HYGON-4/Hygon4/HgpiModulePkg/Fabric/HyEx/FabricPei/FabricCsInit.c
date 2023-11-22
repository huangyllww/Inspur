/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Fabric Coherent Slave initialization.
 *
 * This funtion initializes the CS devices in the fabric.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Fabric
 *
 */
/*
 ****************************************************************************
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
#include "Porting.h"
#include "HYGON.h"
#include <Library/IdsLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersST.h>
#include <Library/FabricRegisterAccLib.h>
#include "Filecode.h"
#include "FabricCsInit.h"

#define FILECODE  FABRIC_HYEX_FABRICPEI_FABRICCSINIT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/

/**
 *
 *  FabricCsInit
 *
 *  Description:
 *    This funtion initializes the CS devices in the fabric.
 *
 */
VOID
FabricCsInit (
  IN       CONST EFI_PEI_SERVICES       **PeiServices,
  IN       HYGON_PEI_SOC_LOGICAL_ID_PPI   *SocLogicalIdPpi
  )
{
  UINTN                      i;
  UINTN                      j;
  UINTN                      CddsPresent;
  DRAM_CONT_MON_REGISTER     DramContMon;
  SPEC_DRAM_RD_MON_REGISTER  SpecDramRdMon;

  for (i = 0; i < FabricTopologyGetNumberOfSocketPresent (); i++) {
    CddsPresent = FabricTopologyGetCddsPresentOnSocket (i);
    for (j = 0; j < MAX_CDDS_PER_SOCKET; j++) {
      if (!IS_CDD_PRESENT (j, CddsPresent)) {
        continue;
      }

      IDS_HDT_CONSOLE (CPU_TRACE, "    Socket %d cdd %d  set DRAM_CONT_MON_REG  \n", i, j);
      DramContMon.Value = 0;
      DramContMon.Field.UmcRdThresh = 6;
      DramContMon.Field.SpecDramRdSampleWin = 5;
      DramContMon.Field.UmcRdSampleWin = 0;
      DramContMon.Field.DramContentionThresh = 0xC;
      DramContMon.Field.DramContentionMonEn  = 1;
      CddFabricRegisterAccWrite (
        i,
        j,
        DRAM_CONT_MON_FUNC,
        DRAM_CONT_MON_REG,
        FABRIC_REG_ACC_BC,
        DramContMon.Value,
        FALSE
        );

      IDS_HDT_CONSOLE (CPU_TRACE, "    Socket %d cdd %d  set SPEC_DRAM_RD_MON_REG  \n", i, j);
      SpecDramRdMon.Value = 0;
      SpecDramRdMon.Field.UmcRdThreshHi  = 0x10;
      SpecDramRdMon.Field.UmcRdThreshLow = 4;
      SpecDramRdMon.Field.SpecDramRdMissThresh = 6;
      SpecDramRdMon.Field.SpecDramRdContentionThresh = 0x8;
      SpecDramRdMon.Field.SpecDramRdMonEn = 1;
      CddFabricRegisterAccWrite (
        i,
        j,
        SPEC_DRAM_RD_MON_FUNC,
        SPEC_DRAM_RD_MON_REG,
        FABRIC_REG_ACC_BC,
        SpecDramRdMon.Value,
        FALSE
        );
    }
  }
}
