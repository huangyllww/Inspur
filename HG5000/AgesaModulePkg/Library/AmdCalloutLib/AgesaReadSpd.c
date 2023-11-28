/* $NoKeywords:$ */
/**
 * @file
 *
 * AgesaReadSpd.c
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project:
 * @e \$Revision: 309090 $ @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 **/
/*****************************************************************************
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
  * ***************************************************************************
  *
 */

#include "Uefi.h"
#include "PiPei.h"
#include "AGESA.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdCalloutLib.h"
#include "AgesaReadSpd.h"
#include "Ppi/AmdMemPpi.h"
#include <Library/PeiServicesTablePointerLib.h>

#define FILECODE LIBRARY_AMDCALLOUTLIB_AGESAREADSPD_FILECODE

/**
 Call the host environment inter0xfa, 0xce, to read an SPD's content.

 @param[in]       FcnData
 @param[in, out]  ReadSpd

 @return          he AGESA Status returned from the callout.
*/
AGESA_STATUS
AgesaReadSpd  (
  IN       UINTN                   FcnData,
  IN OUT   AGESA_READ_SPD_PARAMS   *ReadSpd
  )
{
  EFI_PEI_SERVICES                **PeiServices;
  PEI_AMD_PLATFORM_DIMM_SPD_PPI   *PlatformDimmPpi;
  EFI_STATUS                      Status;
  AGESA_STATUS                    AgesaStatus;

  IDS_HDT_CONSOLE (MAIN_FLOW, "      AgesaReadSpd Entry\n");

  PeiServices = (EFI_PEI_SERVICES**)GetPeiServicesTablePointer();
  //
  // Locate DIMM SPD read PPI.
  //
  Status = (**PeiServices).LocatePpi
                            (
                            PeiServices,
                            &gAmdPlatformDimmSpdPpiGuid,
                            0,
                            NULL,
                            &PlatformDimmPpi
                            );
  if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "       PlatformDimmSpdRead.LocatePpi %r\n", Status);
      return AGESA_ERROR;
  }
  //
  // Invoke SPD Read
  //
  Status = PlatformDimmPpi->PlatformDimmSpdRead (PeiServices, PlatformDimmPpi, ReadSpd);

  IDS_HDT_CONSOLE (MAIN_FLOW, "       PlatformDimmSpdRead Exit %r\n", Status);

  if (!EFI_ERROR (Status)) {
    AgesaStatus = AGESA_SUCCESS;
  } else {
    AgesaStatus = AGESA_ERROR;
  }
  return AgesaStatus;
}
