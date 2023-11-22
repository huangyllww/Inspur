/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Memory API, and related functions.
 *
 * Contains code that initializes channel translation table
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

  MemChanXLatSatPei.c
  Init Memory channel translation table

Abstract:
--*/

#include "PiPei.h"
#include <Ppi/HygonMemPpi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/MemChanXLat.h>
#include <Library/IdsLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include "Filecode.h"

#define FILECODE  MEM_HYGONMEMCHANXLATHyGxPEI_MEMCHANXLATHyGxPEI_FILECODE

extern EFI_GUID  gHygonMemChanXLatPpiGuid;

//
// Create a Channel Mapping Transaltion Table per CPU Package
//
HOST_TO_HPCB_CHANNEL_XLAT  Dm1ChannelXlatTable[] = {
  // Channel A = 0, Channel B = 1
  // Requested   Translated
  { 0,    0    },
  { 1,    1    },
  { 0xFF, 0xFF },
};

HOST_TO_HPCB_CHANNEL_XLAT  Sp5ChannelXlatTable[] = {
  // Channel A = 0, Channel B = 1, Channel C = 2, Channel D = 3
  // Channel E = 4, Channel F = 5, Channel G = 6, Channel H = 7
  // Channel I = 8, Channel J = 9, Channel K = 0xA, Channel L = 0xB
  // Requested   Translated
  { 0,    3    },
  { 1,    2    },
  { 2,    0    },
  { 3,    1    },
  { 4,    7    },
  { 5,    6    },
  { 6,    4    },
  { 7,    5    },
  { 0xFF, 0xFF },
};

HOST_TO_HPCB_CHANNEL_XLAT  Dm2ChannelXlatTable[] = {
  // Channel A = 0, Channel B = 1, Channel C = 2, Channel D = 3
  // Channel E = 4, Channel F = 5, Channel G = 6, Channel H = 7
  // Requested   Translated
  { 0,    3    },
  { 1,    2    },
  { 2,    7    },
  { 3,    6    },
  { 0xFF, 0xFF },
};

/*++

Routine Description:

  Initialize the memory channel translation table.

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
HygonMemChanXLatHyGxPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                     Status;
  HYGON_MEMORY_CHANNEL_XLAT_PPI  *mMemChanXLatPpi;
  EFI_PEI_PPI_DESCRIPTOR         *mMemChanXLatPpiList;
  UINTN                          CddNumberPerSocket;

  // ----------------------------------------------------------------
  //
  // Allocate space for PPI data structure
  //
  // ----------------------------------------------------------------
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (HYGON_MEMORY_CHANNEL_XLAT_PPI),
                             &mMemChanXLatPpi
                             );
  if (EFI_SUCCESS != Status) {
    return Status;
  }

  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             &mMemChanXLatPpiList
                             );
  if (EFI_SUCCESS != Status) {
    return Status;
  }

  mMemChanXLatPpi->XLatTab = NULL;

  // Check Hardware Identification
  CddNumberPerSocket = FabricTopologyGetNumberOfCddsOnSocket (0);
  if (CddNumberPerSocket ==  4) {
    mMemChanXLatPpi->XLatTab = (VOID *)&Sp5ChannelXlatTable[0];
  }

  if (CddNumberPerSocket ==  1) {
    mMemChanXLatPpi->XLatTab = (VOID *)&Dm1ChannelXlatTable[0];
  }

  if (CddNumberPerSocket ==  2) {
    mMemChanXLatPpi->XLatTab = (VOID *)&Dm2ChannelXlatTable[0];
  }

  mMemChanXLatPpi->Revision  = HYGON_MEMORY_CHANNEL_XLAT_REVISION;
  mMemChanXLatPpiList->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  mMemChanXLatPpiList->Guid  = &gHygonMemChanXLatPpiGuid;
  mMemChanXLatPpiList->Ppi   = mMemChanXLatPpi;

  Status = (**PeiServices).InstallPpi (PeiServices, mMemChanXLatPpiList);

  return (Status);
}
