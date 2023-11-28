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
/** @file
  MbistApp - Collects MBIST data from AGESA and presents the information for
  use by Server QA

  Detailed Description of UEFI Application MbistApp

  Copyright for UEFI Application MbistApp

  License for UEFI Application MbistApp

**/

#include <Uefi.h>
#include <PiDxe.h>

//
// Libraries
//
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/HobLib.h>
#include <Guid/AmdMbistHob.h>
#include "MbistApp.h"
#include <AGESA.h>

//
// String token ID of help message text.
// Shell supports to find help message in the resource section of an application image if
// .MAN file is not found. This global variable is added to make build tool recognizes
// that the help string is consumed by user and then build tool will add the string into
// the resource section. Thus the application can use '-?' option to show help message in
// Shell.
//
//GLOBAL_REMOVE_IF_UNREFERENCED EFI_STRING_ID mStringHelpTokenId = STRING_TOKEN (STR_MBIST_HELP_INFORMATION);

/**
  This is the declaration of an EFI image entry point. This entry point is
  the same for UEFI Applications, UEFI OS Loaders, and UEFI Drivers including
  both device drivers and bus drivers.

  @param  ImageHandle           The firmware allocated handle for the UEFI image.
  @param  SystemTable           A pointer to the EFI System Table.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval Others                An unexpected error occurred.
**/
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  UINTN Socket;
  UINTN Die;
  UINTN SubTest;
  UINTN Channel;
  UINTN ChipSelect;
  UINTN IsValid;
  UINTN ArrayIndex;
  EFI_HOB_GUID_TYPE   *MbistHob;
  AMD_MEMORY_MBIST_HOB *MbistHobData;

  Status = EFI_UNSUPPORTED;

  Print (L"MBIST Status Application\n\n");

  MbistHob = GetFirstGuidHob (&gAmdMemoryMbistHobGuid);
  if (MbistHob == NULL) {
    Print (L"MBIST HOB Not located, Exiting\n");
    return Status;
  }
  MbistHobData = GET_GUID_HOB_DATA (MbistHob);
  Print (L"Data presented in MBIST table:\n");
  Print (L"Number of sockets = %d\n", MbistHobData->NumSockets);
  Print (L"Number of Dies  = %d\n", MbistHobData->NumDiePerSocket * MbistHobData->NumSockets);
  Print (L"Number of SubTests = %d\n", MbistHobData->NumSubTests);
  Print (L"Number of channels per socket = %d\n",
      MbistHobData->NumDiePerSocket * MbistHobData->NumChannelPerDie);

  Print (L"\n");
  Print (L"MBIST Test Results:\n");
  Print (L"Socket #,  Die #,  SubTest #,   Channel #,  ChipSel#,  Valid,  Status\n");
  for (Socket = 0; Socket < MbistHobData->NumSockets; Socket++) {
    for (Die = 0; Die < MbistHobData->NumDiePerSocket ; Die++) {   //Die Loops
      for (SubTest = 0; SubTest < MbistHobData->NumSubTests; SubTest++) { //Subtest Loop
        for (Channel = 0; Channel < MbistHobData->NumChannelPerDie; Channel++) {
          ArrayIndex = (Socket * MbistHobData->NumChannelPerDie) + Channel;
          //
          // The upper Nibbles are in dont care mode and not applicable hence
          // Chipselect loop should be for 4 chipselect only
          //
          for (ChipSelect = 0; ChipSelect < 4; ChipSelect++) {
            IsValid = (MbistHobData->MbistTestStatus[Socket][Die][SubTest][Channel].ResultValid >> ChipSelect) & 1;
            Print (L"%d          ", Socket);
            Print (L"%d          ", Die);
            Print (L"%d          ", SubTest);
            Print (L"%d          ", Channel);
            Print (L"%d          ", ChipSelect);
            // Valid
            Print (L"%s", (IsValid == 1 ? L"True    ": L"False   "));
            // MBIST Results
            if (IsValid == 1) {
              Print (L"%s\n",
              (((MbistHobData->MbistTestStatus[Socket][Die][SubTest][Channel].ErrorStatus >> ChipSelect) & 1) ^ 1) ? L"Pass": L"Fail");
            } else {
              Print (L"NA\n");
            }
          }
        }
      }
    }
  }
  Status = EFI_SUCCESS;
  return Status;
}
