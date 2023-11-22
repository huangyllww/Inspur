/*****************************************************************************
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

#include <Filecode.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>              // byo230831 +
#include <Library/UefiDriverEntryPoint.h>
#include <Protocol/HygonVerProtocol.h>
#include "HygonVersionDxe.h"

#define FILECODE  UNIVERSAL_VERISION_HYGONVERSIONDXE_HYGONVERSIONDXE_FILECODE

extern  EFI_BOOT_SERVICES  *gBS;

//
// Driver Global Data
//

// byo230831 + >>
CONST CHAR8 *HygonPatchVersionCheck(CONST CHAR8 *VersionStr, CONST CHAR8 *PatchVerStr)
{
  CHAR8            c;                                  
  UINT32           Ver  = 0;                           
  UINT32           Ver2 = 0;                           
  UINT8            *pVer;               
  UINTN            i;                       
  CONST CHAR8      *p;                                        
 

// HygonPI 4.0.0.0
  pVer = (UINT8*)&Ver; 
  p = VersionStr;
  i = 0;
  while(1){
    c = *p;
    if(c == 0){
      break;
    }
    
    if(c >= '0' && c <= '9'){
      pVer[i]  = pVer[i] * 10;
      pVer[i] |= c - '0';
    } else if(c == '.'){
      i++;
    }
    if(i >= 4){
      break;
    }
    p++;
  }
  Ver = SwapBytes32(Ver);
  DEBUG((EFI_D_INFO, "Pi Ver(%a):%08X\n", VersionStr, Ver));

// 2.1.0.2_200228_001
  pVer = (UINT8*)&Ver2;
  p    = PatchVerStr;
  i    = 0;
  while(1){
    c = *p;
    if(c == 0){
      break;
    }
    
    if(c >= '0' && c <= '9'){
      pVer[i]  = pVer[i] * 10;
      pVer[i] |= c - '0';
    } else if(c == '.'){
      i++;
    } else if(c == '_'){
      break;
    }
    if(i >= 4){
      break;
    }    
    p++;
  }
  Ver2 = SwapBytes32(Ver2);
  DEBUG((EFI_D_INFO, "Patch Ver(%a):%08X\n", PatchVerStr, Ver2));

  if(Ver2 >= Ver){
    return PatchVerStr;
  } else {
    return VersionStr;
  }
}
// byo230831 + <<


/*********************************************************************************
 * Name: HygonVersionDxeInit
 *
 * Description
 *   Entry point of the HYGON Version DXE driver
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
HygonVersionDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  EFI_HANDLE          Handle;
  UINTN               SigStringSize;
//UINTN               PcdFullSize;              // byo230831 -
  CONST CHAR8         *HygonVersionStringSig;
  UINT8               *HygonVersionString;
  HYGON_VER_PROTOCOL  *HygonVerProtocol;
  CONST CHAR8         *Version;                 // byo230831 +
  UINTN               VersionSize;              // byo230831 +

  //
  // Publish HGPI version string
  //
 
  // Get Signature and version string size.
  HygonVersionStringSig = PcdGetPtr (PcdHygonPackageString);
  SigStringSize = AsciiStrSize (HygonVersionStringSig);

// byo230831 - >>  
  Version = HygonVersionStringSig + SigStringSize;
  Version = HygonPatchVersionCheck(Version, PcdGetPtr(PcdHygonPatchVersionString));   // "HGPI!\0HygonPI 4.0.0.0"
  VersionSize = AsciiStrSize(Version);
// byo230831 - <<  

  // Allocate memory for version string
  Status = gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  SigStringSize + VersionSize,              // byo230831 -
                  &HygonVersionString
                  );
  ASSERT (!EFI_ERROR (Status));

// byo230831 - >>
  CopyMem (HygonVersionString, HygonVersionStringSig, SigStringSize);
  CopyMem (HygonVersionString+SigStringSize, Version, VersionSize);
// byo230831 - <<

  // Allocate memory for HGPI Version String Protocol
  Status = gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  sizeof (HYGON_VER_PROTOCOL),
                  &HygonVerProtocol
                  );
  ASSERT (!EFI_ERROR (Status));

  HygonVerProtocol->Signature     = HygonVersionString;
  HygonVerProtocol->VersionString = HygonVersionString + SigStringSize;
  //
  // Publish HGPI version string protocol
  //
  Handle = ImageHandle;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gHygonVersionStringProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  HygonVerProtocol
                  );

  return (Status);
}
