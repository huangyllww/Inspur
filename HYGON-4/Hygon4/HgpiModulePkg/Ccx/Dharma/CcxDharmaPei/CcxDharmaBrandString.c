/* $NoKeywords:$ */

/**
 * @file
 *
 * Hygon CCX brand string
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Ccx
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
 **/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "HGPI.h"
#include "Filecode.h"
#include "cpuRegisters.h"
#include "Library/PeiServicesTablePointerLib.h"
#include "Ppi/NbioSmuServicesPpi.h"
#include <Library/CcxBaseX86Lib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/IdsLib.h>
#include "PiPei.h"

#define FILECODE  CCX_DHARMA_CCXDHARMAPEI_CCXDHARMABRANDSTRING_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

// This is an example, need to be updated once Processor Revision Guide define brand string exception
// Brand string is always 48 bytes
CONST CHAR8 ROMDATA HyGx_str_Unprogrammed_Sample_7000[48] = "Hygon C86-4G 748x Processor";
CONST CHAR8 ROMDATA HyGx_str_Unprogrammed_Sample_5000[48] = "Hygon C86-4G 548x Processor";
CONST CHAR8 ROMDATA HyGx_str_Unprogrammed_Sample_3000[48] = "Hygon C86-4G 348x Processor";
CONST CHAR8 ROMDATA HyEx_str_Unprogrammed_Sample_7000[48] = "Hygon C86-4G 749x Processor";

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

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
CcxDharmaSetBrandString (
  IN       HYGON_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32                           MsrIndex;
  UINT64                           *MsrNameStringPtrPtr;
  EFI_STATUS                       Status;
  EFI_PEI_SERVICES                 **PeiServices;
  PEI_HYGON_NBIO_SMU_SERVICES_PPI  *GnbSmuService;
  UINT64                           BrandString[6];
  UINT32                           CpuModel;
  UINT8                            PkgType;
  
  BrandString[5] = 0;
  CpuModel = GetHygonSocModel();
  PkgType  = GetSocPkgType();
  IDS_HDT_CONSOLE (CPU_TRACE, "    CpuModel=%d, PkgType=%d\n", CpuModel, PkgType);

  if (CpuModel == HYGON_EX_CPU) {
    MsrNameStringPtrPtr = (UINT64 *)HyEx_str_Unprogrammed_Sample_7000;
  } if (CpuModel == HYGON_GX_CPU) {
    switch (PkgType) {
      case SP6_421:
        MsrNameStringPtrPtr = (UINT64 *)HyGx_str_Unprogrammed_Sample_7000;
        break;
      case SP6_211:
        MsrNameStringPtrPtr = (UINT64 *)HyGx_str_Unprogrammed_Sample_5000;
        break;
      case DM1_102:
        MsrNameStringPtrPtr = (UINT64 *)HyGx_str_Unprogrammed_Sample_3000;
        break;
      default:
        MsrNameStringPtrPtr = (UINT64 *)HyGx_str_Unprogrammed_Sample_7000;
        break;
    }
  }


  // Normally, we get Name String from the SMU
  IDS_HDT_CONSOLE (CPU_TRACE, "    Set brand string \n");
  PeiServices = (EFI_PEI_SERVICES **)GetPeiServicesTablePointer ();
  Status = (**PeiServices).LocatePpi (
                             PeiServices,
                             &gHygonNbioSmuServicesPpiGuid,
                             0,
                             NULL,
                             &GnbSmuService
                             );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (CPU_TRACE, "    Can not locate SMU PPI \n");
  } else if (GnbSmuService->SmuReadBrandString (
                              GnbSmuService,
                              0,
                              ((sizeof (BrandString)) - 1),
                              (UINT8 *)&BrandString[0]
                              ) != EFI_SUCCESS) {
    IDS_HDT_CONSOLE (CPU_TRACE, "    SMU PPI returned an error \n");
  } else {
    IDS_HDT_CONSOLE (CPU_TRACE, "    BrandString[0] = 0x%lx \n", BrandString[0]);
    IDS_HDT_CONSOLE (CPU_TRACE, "    BrandString[1] = 0x%lx \n", BrandString[1]);
    IDS_HDT_CONSOLE (CPU_TRACE, "    BrandString[2] = 0x%lx \n", BrandString[2]);
    IDS_HDT_CONSOLE (CPU_TRACE, "    BrandString[3] = 0x%lx \n", BrandString[3]);
    IDS_HDT_CONSOLE (CPU_TRACE, "    BrandString[4] = 0x%lx \n", BrandString[4]);
    IDS_HDT_CONSOLE (CPU_TRACE, "    BrandString[5] = 0x%lx \n", BrandString[5]);

    if ((BrandString[0] & 0xFFFFFFFF) != 0) {
      // Brand string not null
      MsrNameStringPtrPtr = &BrandString[0];
    }
  }

  // Put values into name MSRs,  Always write the full 48 bytes
  for (MsrIndex = MSR_CPUID_NAME_STRING0; MsrIndex <= MSR_CPUID_NAME_STRING5; MsrIndex++) {
    AsmWriteMsr64 (MsrIndex, *MsrNameStringPtrPtr);
    MsrNameStringPtrPtr++;
  }
}
