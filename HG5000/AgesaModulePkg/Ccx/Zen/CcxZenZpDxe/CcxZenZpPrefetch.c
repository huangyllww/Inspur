/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU prefetch functions.
 *
 * This funtion provides for performance tuning to optimize for specific
 * workloads. For general performance use the recommended settings.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Ccx
 * @e \$Revision: 315062 $   @e \$Date: 2015-03-19 07:54:52 +0800 (Thu, 19 Mar 2015) $
 *
 */
/*
 ****************************************************************************
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
#include <Library/BaseLib.h>
#include "Porting.h"
#include "AMD.h"
#include "Filecode.h"
#include "cpuRegisters.h"
#include "CcxZenZpPrefetch.h"
#include "Library/AmdBaseLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Library/CcxRolesLib.h"
#include "Protocol/MpService.h"

#define FILECODE CCX_ZEN_CCXZENZPDXE_CCXZENZPPREFETCH_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
CcxZpPrefetchModeAps (
  IN       CCX_PREFETCH_MODE                *PrefetchMode
  );

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
 *  CcxZpInitializePrefetchMode
 *
 *  Description:
 *    This funtion provides for performance tuning to optimize for specific
 *    workloads. For general performance use the recommended settings.
 *
 *  Parameters:
 *    @param[in]  StdHeader                 Config handle for library and services
 *
 *    @retval     AGESA_STATUS
 *
 *
 */
AGESA_STATUS
CcxZenZpInitializePrefetchMode (
  IN       AMD_CONFIG_PARAMS                *StdHeader
  )
{
  EFI_STATUS                Status;
  CCX_PREFETCH_MODE         PrefetchMode;
  EFI_MP_SERVICES_PROTOCOL *MpServices;

  PrefetchMode.HardwarePrefetchMode = PcdGet8 (PcdAmdHardwarePrefetchMode);
  PrefetchMode.SoftwarePrefetchMode = PcdGet8 (PcdAmdSoftwarePrefetchMode);
  if ((PrefetchMode.HardwarePrefetchMode != HARDWARE_PREFETCHER_AUTO) ||
      (PrefetchMode.SoftwarePrefetchMode != SOFTWARE_PREFETCHES_AUTO)) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  CcxZpInitializePrefetchMode:\n");
    IDS_HDT_CONSOLE (CPU_TRACE, "    HardwarePrefetchMode - %d\n", PrefetchMode.HardwarePrefetchMode);
    IDS_HDT_CONSOLE (CPU_TRACE, "    SoftwarePrefetchMode - %d\n", PrefetchMode.SoftwarePrefetchMode);

    Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &MpServices);
    ASSERT (!EFI_ERROR (Status));
    if (EFI_ERROR (Status)) {
      return AGESA_ERROR;
    }
    MpServices->StartupAllAPs (
        MpServices,
        CcxZpPrefetchModeAps,
        FALSE,
        NULL,
        0,
        (VOID *) &PrefetchMode,
        NULL
    );
    CcxZpPrefetchModeAps (&PrefetchMode);
  }
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CcxZpPrefetchModeAps
 *
 *  Description:
 *    This funtion provides for performance tuning to optimize for specific
 *    workloads. For general performance use the recommended settings.
 *
 *  Parameters:
 *    @param[in]  PrefetchMode              Prefetch mode
 *
 *
 */
VOID
CcxZpPrefetchModeAps (
  IN       CCX_PREFETCH_MODE                *PrefetchMode
  )
{
  ASSERT (PrefetchMode->HardwarePrefetchMode < MAX_HARDWARE_PREFETCH_MODE);

  switch (PrefetchMode->HardwarePrefetchMode) {
  case DISABLE_HARDWARE_PREFETCH:
    // DC_CFG (MSR_C001_1022)
    //  [13] = 1
    //  [15] = 1
    AsmMsrOr64 (MSR_DC_CFG, (BIT13 | BIT15));
    // CU_CFG3 (MSR_C001_102B)
    //  [3]  = 1
    //  [16] = 1
    //  [17] = 1
    //  [18] = 1
    if (CcxIsComputeUnitPrimary (NULL)) {
      AsmMsrOr64 (MSR_CU_CFG3, (BIT3 | BIT16 | BIT17 | BIT18));
    }
    break;
  case DISABLE_L2_STRIDE_PREFETCHER:
    // For DISABLE_L2_STRIDE_PREFETCHER, also implement the following :
    // DISABLE_L1_PREFETCHER
    // DISABLE_HW_PREFETCHER_TRAINING_ON_SOFTWARE_PREFETCHES
    // Hence, no break to allow code to flow through.
    // CU_CFG3 (MSR_C001_102B)
    //  [1] = 0
    if (CcxIsComputeUnitPrimary (NULL)) {
      AsmMsrAnd64 (MSR_CU_CFG3, ~BIT1);
    }
  case DISABLE_L1_PREFETCHER:
    // For this setting, also implement DISABLE_HW_PREFETCHER_TRAINING_ON_SOFTWARE_PREFETCHES.
    // Hence, no break to allow code to flow through.
    // CU_CFG3 (MSR_C001_102B)
    //  [3] = 1
    if (CcxIsComputeUnitPrimary (NULL)) {
      AsmMsrOr64 (MSR_CU_CFG3, BIT3);
    }
  case DISABLE_HW_PREFETCHER_TRAINING_ON_SOFTWARE_PREFETCHES:
    // DC_CFG (MSR_C001_1022)
    //  [15] = 1
    AsmMsrOr64 (MSR_DC_CFG, BIT15);
    break;
  default:
    break;
  }
  // DISABLE_SOFTWARE_PREFETCHES
  if (PrefetchMode->SoftwarePrefetchMode == DISABLE_SOFTWARE_PREFETCHES) {
    // MSR_DE_CFG (MSR_C001_1029)
    //  [7:2] = 0x3F
    if (CcxIsComputeUnitPrimary (NULL)) {
      AsmMsrOr64 (MSR_DE_CFG, 0xFC);
    }
  }
}

