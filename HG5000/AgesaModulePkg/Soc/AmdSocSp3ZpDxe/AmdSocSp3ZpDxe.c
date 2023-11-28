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
 ******************************************************************************
 */

#include <Filecode.h>
#include "AGESA.h"
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/AmdSocBaseLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/DxeSocLogicalIdServicesLib.h>
#include <Protocol/AmdSocProtocol.h>
#include "AmdSocSp3ZpDxe.h"

#define FILECODE SOC_AMDSOCSP3ZPDXE_AMDSOCSP3ZPDXE_FILECODE

extern  EFI_BOOT_SERVICES       *gBS;

//
// Driver Global Data
//

/*********************************************************************************
 * Name: AmdSocSp3ZpDxeInit
 *
 * Description
 *   Entry point of the AMD SOC Zeppelin SP3 DXE driver
 *   Perform the configuration init, resource reservation, early post init
 *   and install all the supported protocol
 *
 * Input
 *   ImageHandle : EFI Image Handle for the DXE driver
 *   SystemTable : pointer to the EFI system table
 *
 * Output
 *   EFI_SUCCESS : Module initialized successfully
 *   EFI_ERROR   : Initialization failed (see error for more details)
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
AmdSocSp3ZpDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  EFI_HANDLE          Handle;
  SOC_ID_STRUCT       SocIdSp3, SocIdAm4, SocIdDm1, *SocId, SocIdSp4;
  SOC_ID_STRUCT       HygonSp3, HygonDm1, HygonAm4, HygonSL1r2;
  UINT8 i;

  DEBUG ((EFI_D_ERROR, "*****************************DXE SOC SP3 ZP Driver Entry*********************\n"));

  // Check Hardware Identification
  SocId = NULL;
  SocIdSp3.SocFamilyID = F17_ZP_RAW_ID;
  SocIdSp3.PackageType = ZP_SP3;
  SocIdAm4.SocFamilyID = F17_ZP_RAW_ID;
  SocIdAm4.PackageType = ZP_AM4;
  SocIdDm1.SocFamilyID = F17_ZP_RAW_ID;
  SocIdDm1.PackageType = ZP_DM1;
  SocIdSp4.SocFamilyID = F17_ZP_RAW_ID;
  SocIdSp4.PackageType = ZP_SP4;

  HygonSp3.SocFamilyID = F18_DN_RAW_ID;
  HygonSp3.PackageType = ZP_SP3;
  HygonDm1.SocFamilyID = F18_DN_RAW_ID;
  HygonDm1.PackageType = ZP_DM1;
  
  HygonAm4.SocFamilyID = F18_DN_RAW_ID;
  HygonAm4.PackageType = ZP_AM4;

  HygonSL1r2.SocFamilyID = F18_DN_RAW_ID;
  HygonSL1r2.PackageType = TR_SP3r2;
  
  if (SocHardwareIdentificationCheck (&SocIdSp3))
  {
    SocId = &SocIdSp3;
  }
  else if (SocHardwareIdentificationCheck (&SocIdAm4))
  {
    SocId = &SocIdAm4;
  }
  else if (SocHardwareIdentificationCheck (&SocIdDm1))
  {
    SocId = &SocIdDm1;
  }
  else if (SocHardwareIdentificationCheck (&SocIdSp4))
  {
    SocId = &SocIdSp4;
  }
  else if (SocHardwareIdentificationCheck (&HygonSp3))
  {
    SocId = &HygonSp3;
  }
  else if (SocHardwareIdentificationCheck (&HygonDm1))
  {
    SocId = &HygonDm1;
  }
    else if (SocHardwareIdentificationCheck (&HygonAm4))
  {
    SocId = &HygonAm4;
  }
  else if (SocHardwareIdentificationCheck (&HygonSL1r2))
  {
    SocId = &HygonSL1r2;
  }
  
  if (NULL != SocId) {
    //
    // Publish the logical ID protocol
    //
    Status = SocLogicalIdServiceProtocolInstall (ImageHandle, SystemTable);

    IDS_HOOK (IDS_HOOK_BEGINNING_OF_AGESA, NULL, NULL);

    //
    // Publish the Silicon installation protocol
    //

    Handle = ImageHandle;
    for (i = 0 ; i < SiliconDriverProtocolListNumber; i++) {
      Status = gBS->InstallProtocolInterface (
                    &Handle,
                    SiliconDriverProtocolList[i],
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );
    }
  }
  DEBUG ((EFI_D_ERROR, "*****************************DXE SOC SP3 ZP Driver Exit*********************\n"));
  return (Status);
}

