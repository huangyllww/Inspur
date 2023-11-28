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


// *** This is a platform specific reference only driver, the implemenatation depends on OEM design**

#include "PiDxe.h"
#include <Protocol/PspPlatformProtocol.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/AmdPspBaseLibV2.h>
EFI_HANDLE              mPspPlatformHandle = NULL;

#define PSP_NOTIFY_SMM        0xDD          //< Filled by OEM >
#define RESUME_BSP_STACK_SIZE (1024 * 128)  //< Filled by OEM >
#define RESUME_AP_STACK_SIZE  (1024 * 16)   //< Filled by OEM >


PSP_PLATFORM_PROTOCOL mPspPlatformProtocol = {
  TRUE,                   //Support CPU Context Resume
  PSP_NOTIFY_SMM,         //SW SMI Command for Build Context
  RESUME_BSP_STACK_SIZE,  //BSP Stack Size
  RESUME_AP_STACK_SIZE,   //AP Stack Size
  NULL                    //Address of PEI Info Structure
};

EFI_STATUS
PspPlatformEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  UINT64       RsmInfoLocation;
  EFI_STATUS   Status;
  TYPE_ATTRIB       TypeAttrib;
  UINT64            EntryAddress;
  UINT32            EntrySize;
  UINT64            EntryDest;
  UINT64       ResetVectorIp;
  UINT64       ResetVectorSeg;
  UINT32                          RegEax;

// Search for Resume HandOff Infomation structure
// This should based on Resume SEC phase design.
  AsmCpuid (0x80000001, &RegEax, NULL, NULL, NULL);
  if (((RegEax >> 20) & 0xFF) == 0x6) {
    //IF BR
    ResetVectorIp = 0xFFFFFFF0;
    ResetVectorSeg = 0xFFFF0000;
  } else if (((RegEax >> 20) & 0xFF) == 0x8) {
    // IF ZP
    BIOSEntryInfo (BIOS_FIRMWARE, INSTANCE_IGNORED, &TypeAttrib, &EntryAddress, &EntrySize, &EntryDest);
    ResetVectorSeg = EntryDest + EntrySize - 0x10000;
    ResetVectorIp = ResetVectorSeg + 0xFFF0;
  } else {
    return EFI_UNSUPPORTED;
  }

  RsmInfoLocation = *((UINT16 *) (UINTN) (ResetVectorIp + 0x3));
  RsmInfoLocation += 0xFFF5;
  RsmInfoLocation = (RsmInfoLocation & 0xFFFF) | ResetVectorSeg;
  RsmInfoLocation -= sizeof (RSM_HANDOFF_INFO);
  mPspPlatformProtocol.RsmHandOffInfo = (RSM_HANDOFF_INFO *)RsmInfoLocation;
// ===========================================================================================

  //Install Protocol
  Status = gBS->InstallProtocolInterface (
                              &mPspPlatformHandle,
                              &gPspPlatformProtocolGuid,
                              EFI_NATIVE_INTERFACE,
                              &mPspPlatformProtocol
                                );
  return EFI_SUCCESS;
}

