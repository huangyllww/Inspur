/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPU Power Management functions.
 *
 * Contains code for doing early power management
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Ccx
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
#include <Library/BaseLib.h>
#include "Porting.h"
#include "HYGON.h"
#include "Filecode.h"
#include "CcxRegistersDm.h"
#include "Library/HygonBaseLib.h"
#include "Library/BaseCoreLogicalIdLib.h"
#include "Library/CcxRolesLib.h"
#include "Library/HygonS3SaveLib.h"
#include "Library/IdsLib.h"
#include "Protocol/MpService.h"
#include "PiDxe.h"

#define FILECODE  CCX_DHARMA_CCXDHARMADXE_CCXDHARMAC6_FILECODE

extern  EFI_BOOT_SERVICES  *gBS;

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
CcxDharmaInitializeC6Aps (
  IN       UINT16  *CStateBaseAddr
  );

VOID
CcxDharmaInitializeC1Aps (
  IN       UINT16  *CStateBaseAddr
  );

VOID
CcxDharmaDisableC1C6Aps (
  IN       UINT16  *CStateBaseAddr
  );

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/

/**
 * Enabling Core Performance Boost.
 *
 * Set up D18F4x15C[BoostSrc] and start the PDMs according to the BKDG.
 *
 * @param[in]  StdHeader               Config handle for library and services.
 *
 * @retval     HGPI_SUCCESS           Always succeeds.
 *
 */
HGPI_STATUS
CcxDharmaInitializeC6 (
  IN       HYGON_CONFIG_PARAMS         *StdHeader
  )
{
  UINT8                     CStateMode;
  UINT16                    CStateBaseAddr;
  EFI_STATUS                Status;
  EFI_MP_SERVICES_PROTOCOL  *MpServices;

  CStateMode = PcdGet8 (PcdHygonCStateMode);
  ASSERT (CStateMode <= 2);

  CStateBaseAddr = PcdGet16 (PcdHygonCStateIoBaseAddress);

  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &MpServices);
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    return HGPI_ERROR;
  }

  if (CStateMode == 0) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  CcxDharmaDisableC1C6Aps - Disable CC1 and CC6 \n");

    MpServices->StartupAllAPs (
                  MpServices,
                  CcxDharmaDisableC1C6Aps,
                  FALSE,
                  NULL,
                  0,
                  (VOID *)&CStateBaseAddr,
                  NULL
                  );
    CcxDharmaDisableC1C6Aps (&CStateBaseAddr);
  }

  if (CStateMode == 1) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  CcxDharmaInitializeC1 - Enable CC1\n");

    MpServices->StartupAllAPs (
                  MpServices,
                  CcxDharmaInitializeC1Aps,
                  FALSE,
                  NULL,
                  0,
                  (VOID *)&CStateBaseAddr,
                  NULL
                  );
    CcxDharmaInitializeC1Aps (&CStateBaseAddr);
  }

  if (CStateMode == 2) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  CcxDharmaInitializeC6 - Enable CC6\n");

    MpServices->StartupAllAPs (
                  MpServices,
                  CcxDharmaInitializeC6Aps,
                  FALSE,
                  NULL,
                  0,
                  (VOID *)&CStateBaseAddr,
                  NULL
                  );
    CcxDharmaInitializeC6Aps (&CStateBaseAddr);
  }

  return HGPI_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/

/**
 *
 *  CcxDharmaInitializeC6Aps
 *
 *  @param[in]  CStateBaseAddr   Pointer to CState base address
 *
 *  Description:
 *    This funtion enables C6 on all threads.
 *
 */
VOID
CcxDharmaInitializeC6Aps (
  IN       UINT16  *CStateBaseAddr
  )
{
  CSTATE_CFG_MSR       CstateCfg;
  HYGON_CONFIG_PARAMS  StdHeader;
  CSTATE_ADDRESS_MSR   CstateAddr;

  if (CcxIsComputeUnitPrimary (&StdHeader)) {
    CstateCfg.Value = 0;
    CstateCfg.Field.CCR0_CC1DFSID = 8;
    CstateCfg.Field.CCR0_CC6EN    = 1;
    CstateCfg.Field.CCR0_CC1EN    = 1;
    CstateCfg.Field.CCR1_CC1DFSID = 8;
    CstateCfg.Field.CCR1_CC6EN    = 1;
    CstateCfg.Field.CCR1_CC1EN    = 1;
    CstateCfg.Field.CCR2_CC1DFSID = 8;
    CstateCfg.Field.CCR2_CC6EN    = 1;
    CstateCfg.Field.CCR2_CC1EN    = 1;
    AsmWriteMsr64 (MSR_CSTATE_CFG, CstateCfg.Value);
  }

  CstateAddr.Value = 0;
  CstateAddr.Field.CstateAddr = (UINT32)*CStateBaseAddr;
  AsmWriteMsr64 (MSR_CSTATE_ADDRESS, CstateAddr.Value);
}

/* -----------------------------------------------------------------------------*/

/**
 *
 *  CcxDharmaInitializeC1Aps
 *
 *  @param[in]  CStateBaseAddr   Pointer to CState base address
 *
 *  Description:
 *    This funtion enables C1 on all threads.
 *
 */
VOID
CcxDharmaInitializeC1Aps (
  IN       UINT16  *CStateBaseAddr
  )
{
  CSTATE_CFG_MSR       CstateCfg;
  HYGON_CONFIG_PARAMS  StdHeader;
  CSTATE_ADDRESS_MSR   CstateAddr;

  if (CcxIsComputeUnitPrimary (&StdHeader)) {
    CstateCfg.Value = 0;
    CstateCfg.Field.CCR0_CC1DFSID = 8;
    CstateCfg.Field.CCR0_CC6EN    = 0;
    CstateCfg.Field.CCR0_CC1EN    = 1;
    CstateCfg.Field.CCR1_CC1DFSID = 8;
    CstateCfg.Field.CCR1_CC6EN    = 0;
    CstateCfg.Field.CCR1_CC1EN    = 1;
    CstateCfg.Field.CCR2_CC1DFSID = 8;
    CstateCfg.Field.CCR2_CC6EN    = 0;
    CstateCfg.Field.CCR2_CC1EN    = 1;
    AsmWriteMsr64 (MSR_CSTATE_CFG, CstateCfg.Value);
  }

  CstateAddr.Value = 0;
  CstateAddr.Field.CstateAddr = (UINT32)*CStateBaseAddr;
  AsmWriteMsr64 (MSR_CSTATE_ADDRESS, CstateAddr.Value);
}

/* -----------------------------------------------------------------------------*/

/**
 *
 *  CcxDharmaDisableC1C6Aps
 *
 *  @param[in]  CStateBaseAddr   Pointer to CState base address
 *
 *  Description:
 *    This funtion disable C1 and C6 on all threads.
 *
 */
VOID
CcxDharmaDisableC1C6Aps (
  IN       UINT16  *CStateBaseAddr
  )
{
  CSTATE_CFG_MSR       CstateCfg;
  HYGON_CONFIG_PARAMS  StdHeader;
  CSTATE_ADDRESS_MSR   CstateAddr;

  if (CcxIsComputeUnitPrimary (&StdHeader)) {
    CstateCfg.Value = 0;
    CstateCfg.Field.CCR0_CC1DFSID = 8;
    CstateCfg.Field.CCR0_CC6EN    = 0;
    CstateCfg.Field.CCR0_CC1EN    = 0;
    CstateCfg.Field.CCR1_CC1DFSID = 8;
    CstateCfg.Field.CCR1_CC6EN    = 0;
    CstateCfg.Field.CCR1_CC1EN    = 0;
    CstateCfg.Field.CCR2_CC1DFSID = 8;
    CstateCfg.Field.CCR2_CC6EN    = 0;
    CstateCfg.Field.CCR2_CC1EN    = 0;
    AsmWriteMsr64 (MSR_CSTATE_CFG, CstateCfg.Value);
  }

  CstateAddr.Value = 0;
  AsmWriteMsr64 (MSR_CSTATE_ADDRESS, CstateAddr.Value);
}
