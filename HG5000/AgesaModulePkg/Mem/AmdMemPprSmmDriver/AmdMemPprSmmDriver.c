/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Memory API, and related functions.
 *
 * Contains code that initializes memory
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Mem
 * @e \$Revision:  $   @e \$Date:  $
 *
 */
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

#include <PiDxe.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Guid/HobList.h>
#include <Library/AmdPspApobLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Protocol/SmmBase2.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/AmdMemPprProtocol.h>

#define FILECODE MEM_AMDMEMPPRSMMDRIVER_MEM_AMDMEMPPRSMMDRIVER_FILECODE

/*++

Routine Description:

  Smm Driver to extrat the Post Package Repair Info

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
GetPprInfo (
  IN       AMD_POST_PACKAGE_REPAIR_INFO_PROTOCOL    *This,
  IN       AMD_DIMM_INFO                            *AmdDimmInfo,
     OUT   AMD_PPR_INFO                             *PprInfo
  )
{
  UINT8 Dimm;
  EFI_STATUS Status;

  Dimm = 0;

  switch (AmdDimmInfo->Chipselect & 0x0F) {
    case 0:
    case 2:
      Dimm = 0;
      break;
    case 4:
    case 8:
      Dimm = 1;
      break;
  }

  if (This->AmdPprArray.DimmSpdInfo [AmdDimmInfo->SocketId*2 + AmdDimmInfo->DieId*4 + Dimm + AmdDimmInfo->ChannelId*2].IsValidRecord) {
    PprInfo->DpprSupported = This->AmdPprArray.DimmSpdInfo [AmdDimmInfo->SocketId*2 + AmdDimmInfo->DieId*4 + Dimm + AmdDimmInfo->ChannelId*2].DpprSupported;
    PprInfo->SpprSupported = This->AmdPprArray.DimmSpdInfo [AmdDimmInfo->SocketId*2 + AmdDimmInfo->DieId*4 + Dimm + AmdDimmInfo->ChannelId*2].SpprSupported;
    PprInfo->DeviceWidth = This->AmdPprArray.DimmSpdInfo [AmdDimmInfo->SocketId*2 + AmdDimmInfo->DieId*4 + Dimm + AmdDimmInfo->ChannelId*2].DeviceWidth;
    PprInfo->SerialNumber = This->AmdPprArray.DimmSpdInfo [AmdDimmInfo->SocketId*2 + AmdDimmInfo->DieId*4 + Dimm + AmdDimmInfo->ChannelId*2].SerialNumber;
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_NOT_FOUND;
  }

  return Status;
}

/*++

Routine Description:

  Driver Entry for Post Package Reapir

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
AmdMemDramPprDriverEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                      Status;
  APOB_TYPE_HEADER                *ApobEntry;
  APOB_MEM_DIMM_SPD_DATA_STRUCT   ApobSpdHeader;
  AMD_POST_PACKAGE_REPAIR_INFO_PROTOCOL  *AmdPprInfoPtr;
  UINT8                           Socket;
  UINT8                           Channel;
  UINT8                           Dimm;
  UINT16                          Instance;
  UINT16                          DieLoop;
  EFI_HANDLE                      SmmHandle;

  ApobEntry = NULL;
  AmdPprInfoPtr = NULL;
  SmmHandle = NULL;

  IDS_HDT_CONSOLE (MAIN_FLOW, "AGESA Post Package Repair Driver\n");

  //
  // Allocate the SMM Runtime Space for Post Package Repair data
  //

  Status = gSmst->SmmAllocatePool (
                         EfiRuntimeServicesData,
                         sizeof (AMD_POST_PACKAGE_REPAIR_INFO_PROTOCOL),
                         &AmdPprInfoPtr
                         );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (AmdPprInfoPtr, sizeof (AMD_POST_PACKAGE_REPAIR_INFO_PROTOCOL));

  //
  // Initialize the fields and mark them FALSE
  //
  for (Socket = 0; Socket < AMD_MEM_MAX_SOCKETS_SUPPORTED; Socket++) {
    for (Channel = 0; Channel < AMD_MEM_MAX_CHANNELS_PER_SOCKET; Channel++) {
      for (Dimm = 0; Dimm < AMD_MEM_MAX_DIMMS_PER_CHANNEL; Dimm++) {
        AmdPprInfoPtr->AmdPprArray.DimmSpdInfo[Socket*2 + Dimm + Channel*2].IsValidRecord = FALSE;
        AmdPprInfoPtr->AmdPprArray.DimmSpdInfo[Socket*2 + Dimm + Channel*2].SerialNumber = 0;
        AmdPprInfoPtr->AmdPprArray.DimmSpdInfo[Socket*2 + Dimm + Channel*2].DeviceWidth = 0;
        AmdPprInfoPtr->AmdPprArray.DimmSpdInfo[Socket*2 + Dimm + Channel*2].DpprSupported = 0;
        AmdPprInfoPtr->AmdPprArray.DimmSpdInfo[Socket*2 + Dimm + Channel*2].SpprSupported = 0;

      }
    }
  }
  DieLoop = 0;
  //
  // Update SPD data
  //
  for (Socket = 0; Socket < AMD_MEM_MAX_SOCKETS_SUPPORTED; Socket++) {
    Instance = DieLoop;
    Instance |= ((Socket & 0x000000FF) << 8);
    IDS_HDT_CONSOLE (MAIN_FLOW, "Get Spd Data from APOB for Socket %d\n", Socket);
    for (DieLoop = 0; DieLoop < AMD_MEM_MAX_DIES_PER_SOCKET; DieLoop++) {
      Instance = DieLoop;
      Instance |= ((Socket & 0x000000FF) << 8);
      Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_DIMM_SPD_DATA_TYPE, Instance, FALSE, &ApobEntry);
      if (Status == EFI_SUCCESS) {
        //Instance |= (Channel >> 1);
        CopyMem (&ApobSpdHeader, ApobEntry, sizeof (APOB_MEM_DIMM_SPD_DATA_STRUCT));
        for (Channel = 0; Channel < ApobSpdHeader.MaxChannelsPerSocket; Channel++) {
          IDS_HDT_CONSOLE (MAIN_FLOW, "Get Spd Data from APOB for Channel %d\n", Channel);
          for (Dimm = 0; Dimm < ApobSpdHeader.MaxDimmsPerChannel; Dimm++) {
            IDS_HDT_CONSOLE (MAIN_FLOW, "Get Spd Data from APOB for Dimm %d\n", Dimm);
            if (ApobSpdHeader.DimmSmbusInfo[Socket*2 + DieLoop*4 + Dimm + Channel*2].DimmPresent == TRUE) {
              AmdPprInfoPtr->AmdPprArray.DimmSpdInfo[Socket*2 + Dimm + Channel*2].IsValidRecord = TRUE;
              AmdPprInfoPtr->AmdPprArray.DimmSpdInfo[Socket*2 + DieLoop*4 + Dimm + Channel*2].SerialNumber = ApobSpdHeader.DimmSmbusInfo[Dimm + Channel*2].SerialNumber;
              AmdPprInfoPtr->AmdPprArray.DimmSpdInfo[Socket*2 + DieLoop*4 + Dimm + Channel*2].DeviceWidth = (UINT8) ApobSpdHeader.DimmSmbusInfo[Dimm + Channel*2].DeviceWidth;
              AmdPprInfoPtr->AmdPprArray.DimmSpdInfo[Socket*2 + DieLoop*4 + Dimm + Channel*2].DpprSupported = (BOOLEAN) ApobSpdHeader.DimmSmbusInfo[Dimm + Channel*2].DpprSupported;
              AmdPprInfoPtr->AmdPprArray.DimmSpdInfo[Socket*2 + DieLoop*4 + Dimm + Channel*2].SpprSupported = (BOOLEAN) ApobSpdHeader.DimmSmbusInfo[Dimm + Channel*2].SpprSupported;
            }
          }
        }
      }
    }
  }

  //
  // Install Protocol Interface under SMM
  //
  AmdPprInfoPtr->AmdGetPprInfo = GetPprInfo;
  Status = gSmst->SmmInstallProtocolInterface (
               &SmmHandle,
               &gAmdPostPackageRepairInfoProtocolGuid,
               EFI_NATIVE_INTERFACE,
               AmdPprInfoPtr
               );

  return (Status);
}
