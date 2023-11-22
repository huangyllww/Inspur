/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Memory API for SAT DDR4, and related functions.
 *
 * Contains code that initializes memory
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  PSP
 *
 */
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
/*++
Module Name:

  MemSmbiosPei.c

Abstract:
--*/
#include "Uefi.h"
#include "PiPei.h"
#include "HGPI.h"
#include "Library/IdsLib.h"
#include "Library/HygonBaseLib.h"
#include "MemSmbiosPei.h"
#include "Library/HygonPspHpobLib.h"
#include "Ppi/HygonMemPpi.h"
#include "Library/MemChanXLat.h"
#include "Library/MemSmbiosLib.h"
#include "Library/FchBaseLib.h"
#include "Filecode.h"
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/HygonMemoryInfoHob.h>

#define FILECODE  MEM_HYGONMEMSMBIOSPEI_MEMSMBIOSPEI_FILECODE

extern EFI_GUID  gHygonMemChanXLatPpiGuid;

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

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
/**
 *---------------------------------------------------------------------------------------
 *
 *  Entry for HygonMemSmbiosPei
 *
 *    @retval          EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
PeiHygonMemSmbiosEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                     Status;
  HGPI_STATUS                    HgpiStatus;
  HYGON_CONFIG_PARAMS            StdHeader;
  EFI_STATUS                     HpobStatus;
  HPOB_TYPE_HEADER               *HpobSmbiosInfo;
  HYGON_MEMORY_CHANNEL_XLAT_PPI  *MemChanXLatPpi;
  HOST_TO_HPCB_CHANNEL_XLAT      *HostToHpcbChanXLatTab;
  UINT8                          NumSockets;
  HYGON_MEMORY_INFO_HOB          *HygonMemoryInfoHob = NULL;
  EFI_PEI_HOB_POINTERS           Hob;
  UINTN                          SizeofBuffer = 0;
  UINT32                         HpobInstanceId;

  if (FchReadSleepType () != 0x03) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  HygonMemSmbiosPei Entry\n");
    HpobSmbiosInfo = NULL;
    HpobInstanceId = HygonPspGetHpobCddInstanceId (0, 0);
    HpobStatus     = HygonPspGetHpobEntryInstance (HPOB_GROUP_SMBIOS, HPOB_MEM_SMBIOS_TYPE, HpobInstanceId, FALSE, &HpobSmbiosInfo);
    if (HpobStatus != EFI_SUCCESS) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "  No SMBIOS data found in HPOB\n");
      return HpobStatus;
    }

    HostToHpcbChanXLatTab = NULL;
    Status = (*PeiServices)->LocatePpi (
                               PeiServices,
                               &gHygonMemChanXLatPpiGuid,
                               0,
                               NULL,
                               (VOID **)&MemChanXLatPpi
                               );
    if (EFI_ERROR (Status) || (MemChanXLatPpi->XLatTab == NULL)) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Fail To Memory Channel Translation Table located\n");
      return Status;
    }

    IDS_HDT_CONSOLE (MAIN_FLOW, "  Memory Channel Translation Table located\n");
    HostToHpcbChanXLatTab = MemChanXLatPpi->XLatTab;

    // Set Host to Hpcb Memory Channel Translation Table to PcdHostToHpcbChanXLatTab
    do {
      HostToHpcbChanXLatTab++;
      SizeofBuffer++;
    } while (HostToHpcbChanXLatTab->RequestedChannelId != 0xFF);

    HostToHpcbChanXLatTab = HostToHpcbChanXLatTab - SizeofBuffer;
    SizeofBuffer = SizeofBuffer * 2 + 2;
    PcdSetPtrS (PcdHostToHpcbChanXLatTab, &SizeofBuffer, (VOID *)HostToHpcbChanXLatTab);

    NumSockets = PcdGet8 (PcdHygonNumberOfPhysicalSocket);

    // Locate HYGON_MEMORY_INFO_HOB
    Status = (*PeiServices)->GetHobList (PeiServices, &Hob.Raw);
    if (!EFI_ERROR (Status)) {
      while (!END_OF_HOB_LIST (Hob)) {
        if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION &&
            CompareGuid (&Hob.Guid->Name, &gHygonMemoryInfoHobGuid)) {
          HygonMemoryInfoHob = (HYGON_MEMORY_INFO_HOB *)(Hob.Raw +
                                                         sizeof (EFI_HOB_GENERIC_HEADER) +
                                                         sizeof (EFI_GUID));
          IDS_HDT_CONSOLE (MAIN_FLOW, "  Get MemoryInfo HOB\n");
          break;
        }

        Hob.Raw = GET_NEXT_HOB (Hob);
      }
    }

    if (PcdGetBool (PcdDDR5Enable)) {
      HgpiStatus = GetMemSmbios (DDR5_TECHNOLOGY, NumSockets, HpobSmbiosInfo, HostToHpcbChanXLatTab, &StdHeader, HygonMemoryInfoHob);
    } else {
      HgpiStatus = GetMemSmbios (DDR4_TECHNOLOGY, NumSockets, HpobSmbiosInfo, HostToHpcbChanXLatTab, &StdHeader, HygonMemoryInfoHob);
    }

    IDS_HDT_CONSOLE (MAIN_FLOW, "  GetMemSmbios Status = %d \n", HgpiStatus);

    IDS_HDT_CONSOLE (MAIN_FLOW, "  HygonMemSmbiosPei Exit\n");
  }

  return EFI_SUCCESS;
}
