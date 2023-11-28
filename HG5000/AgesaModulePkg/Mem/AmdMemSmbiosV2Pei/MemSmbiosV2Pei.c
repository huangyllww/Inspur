/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Memory API for ZP DDR4, and related functions.
 *
 * Contains code that initializes memory
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 12:28:05 -0600 (Tue, 09 Dec 2014) $
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
/*++
Module Name:

  MemSmbiosV2Pei.c

Abstract:
--*/
#include "Uefi.h"
#include "PiPei.h"
#include "AGESA.h"
#include "Library/IdsLib.h"
#include "Library/AmdBaseLib.h"
#include "MemSmbiosV2Pei.h"
#include "Library/AmdPspApobLib.h"
#include "Ppi/AmdMemPpi.h"
#include "Library/MemChanXLat.h"
#include "Library/MemSmbiosV2Lib.h"
#include "Library/FchBaseLib.h"
#include "Filecode.h"
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/AmdMemoryInfoHob.h>

#define FILECODE        MEM_AMDMEMSMBIOSV2PEI_MEMSMBIOSV2PEI_FILECODE

extern EFI_GUID gAmdMemChanXLatPpiGuid;

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
 *  Entry for AmdMemSmbiosV2Pei
 *
 *    @retval          EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
PeiAmdMemSmbiosV2Entry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                     Status;
  AMD_CONFIG_PARAMS              StdHeader;
  EFI_STATUS                     ApobStatus;
  APOB_TYPE_HEADER               *ApobSmbiosInfo;
  AMD_MEMORY_CHANNEL_XLAT_PPI    *MemChanXLatPpi;
  HOST_TO_APCB_CHANNEL_XLAT      *HostToApcbChanXLatTab;
  UINT8                          NumSockets;
  AMD_MEMORY_INFO_HOB            *AmdMemoryInfoHob = NULL;
  EFI_PEI_HOB_POINTERS           Hob;
  UINTN                          SizeofBuffer = 0;

  if (FchReadSleepType () != 0x03) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  AmdMemSmbiosV2Pei Entry\n");
    ApobSmbiosInfo = NULL;
    ApobStatus = AmdPspGetApobEntryInstance (APOB_SMBIOS, APOB_MEM_SMBIOS_TYPE, 0, FALSE, &ApobSmbiosInfo);
    if (ApobStatus != EFI_SUCCESS) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "  No SMBIOS data found in APOB\n");
      return ApobStatus;
    }

    HostToApcbChanXLatTab = NULL;
    Status = (*PeiServices)->LocatePpi (
                PeiServices,
                &gAmdMemChanXLatPpiGuid,
                0,
                NULL,
                (VOID**)&MemChanXLatPpi
                );
    if (EFI_SUCCESS == Status) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Memory Channel Translation Table located\n");
      HostToApcbChanXLatTab = MemChanXLatPpi->XLatTab;
    }
    
    //Set Host to Apcb Memory Channel Translation Table to PcdHostToApcbChanXLatTab
    do {
        HostToApcbChanXLatTab++;
        SizeofBuffer++;
    }while (HostToApcbChanXLatTab->RequestedChannelId != 0xFF);
    
    HostToApcbChanXLatTab = HostToApcbChanXLatTab - SizeofBuffer;
    SizeofBuffer = SizeofBuffer * 2 + 2;
    PcdSetPtr (PcdHostToApcbChanXLatTab, &SizeofBuffer, (VOID*)HostToApcbChanXLatTab);
    
    NumSockets = PcdGet8 (PcdAmdNumberOfPhysicalSocket);

    // Locate AMD_MEMORY_INFO_HOB
    Status = (*PeiServices)->GetHobList (PeiServices, &Hob.Raw);
    if (!EFI_ERROR (Status)) {
      while (!END_OF_HOB_LIST (Hob)) {
        if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION &&
                CompareGuid ( &Hob.Guid->Name, &gAmdMemoryInfoHobGuid)) {
            AmdMemoryInfoHob = (AMD_MEMORY_INFO_HOB *) (Hob.Raw +
                                                    sizeof (EFI_HOB_GENERIC_HEADER) +
                                                    sizeof (EFI_GUID));
            IDS_HDT_CONSOLE (MAIN_FLOW, "  Get MemoryInfo HOB\n");
            break;
        }
        Hob.Raw = GET_NEXT_HOB (Hob);
      }
    }

    if (AGESA_SUCCESS != GetMemSmbiosV2 (DDR4_TECHNOLOGY, NumSockets, ApobSmbiosInfo, HostToApcbChanXLatTab, &StdHeader, AmdMemoryInfoHob)) {
      ASSERT (FALSE);
    }

    IDS_HDT_CONSOLE (MAIN_FLOW, "  AmdMemSmbiosV2Pei Exit\n");
  }

  return EFI_SUCCESS;
}
