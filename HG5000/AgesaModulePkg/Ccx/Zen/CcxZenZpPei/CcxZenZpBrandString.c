/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX brand string
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Ccx
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
 **/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "Filecode.h"
#include "cpuRegisters.h"
#include "Library/PeiServicesTablePointerLib.h"
#include "Ppi/NbioSmuServicesPpi.h"
#include <Library/CcxBaseX86Lib.h>
#include <Library/IdsLib.h>
#include "PiPei.h"

#define FILECODE CCX_ZEN_CCXZENZPPEI_CCXZENZPBRANDSTRING_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
/// A structure containing brand string
typedef struct {
  CONST CHAR8 *Stringstart;  ///< The literal string
} CPU_F17_ZP_EXCEPTION_BRAND;

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
/// A structure containing brand string
typedef struct {
  CONST CHAR8 *Stringstart;  ///< The literal string
} CPU_F17_EXCEPTION_BRAND;

// This is an example, need to be updated once Processor Revision Guide define brand string exception
// Brand string is always 48 bytes
//CONST CHAR8 ROMDATA str_Exception_0[48] = "AMD PRO A12-8800B R7, 12 Compute Cores 4C+8G";
CONST CHAR8 ROMDATA str_Unprogrammed_Sample[48] = "AMD Unprogrammed Engineering Sample";

CONST CPU_F17_EXCEPTION_BRAND ROMDATA CpuF17ExceptionBrandIdString[] =
{
//  str_Exception_0
  NULL
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
BOOLEAN
STATIC
IsException (
     OUT   UINT32                 *ExceptionId,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
* Set the Processor Name String register based on fuses.
*
*  @param[in]   StdHeader           Config handle for library and services.
*
*/
VOID
CcxZenZpSetBrandString (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32                         ExceptionId;
  UINT32                         MsrIndex;
  UINT64                         *MsrNameStringPtrPtr;
  EFI_STATUS                     Status;
  EFI_PEI_SERVICES               **PeiServices;
  PEI_AMD_NBIO_SMU_SERVICES_PPI  *GnbSmuService;
  UINT64                         BrandString[6];

  BrandString[5] = 0;
  MsrNameStringPtrPtr = (UINT64 *) str_Unprogrammed_Sample;
  if (IsException (&ExceptionId, StdHeader)) {
    ASSERT (ExceptionId < (sizeof (CpuF17ExceptionBrandIdString) / sizeof (CpuF17ExceptionBrandIdString[0])));
    MsrNameStringPtrPtr = (UINT64 *) CpuF17ExceptionBrandIdString[ExceptionId].Stringstart;
  } else {
    // Normally, we get Name String from the SMU
    IDS_HDT_CONSOLE (CPU_TRACE, "    Set brand string");
    PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();
    Status = (**PeiServices).LocatePpi (
              PeiServices,
              &gAmdNbioSmuServicesPpiGuid,
              0,
              NULL,
              &GnbSmuService
              );
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE (CPU_TRACE, "    Can not locate SMU PPI");
    } else if (GnbSmuService->SmuReadBrandString (GnbSmuService, 0, ((sizeof (BrandString)) - 1), (UINT8 *) &BrandString[0]) != EFI_SUCCESS) {
      IDS_HDT_CONSOLE (CPU_TRACE, "    SMU PPI returned an error");
    } else {
      MsrNameStringPtrPtr = &BrandString[0];
    }
  }
  // Put values into name MSRs,  Always write the full 48 bytes
  for (MsrIndex = MSR_CPUID_NAME_STRING0; MsrIndex <= MSR_CPUID_NAME_STRING5; MsrIndex++) {
    AsmWriteMsr64 (MsrIndex, *MsrNameStringPtrPtr);
    MsrNameStringPtrPtr++;
  }
}

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
* Check if it's an exception
*
* For family 17h, brand string is obtained from D0F0xBC_xD021_108C, but there may be exceptions.
* This function checks if it's an exception.
*
*  @param[out]  ExceptionId         Id of exception
*  @param[in]   StdHeader           Config handle for library and services.
*
*  @retval      TRUE                It's an exception
*  @retval      FALSE               It's NOT an exception
*/
BOOLEAN
STATIC
IsException (
     OUT   UINT32             *ExceptionId,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  *ExceptionId = 0xFFFF;
  return FALSE;
}
