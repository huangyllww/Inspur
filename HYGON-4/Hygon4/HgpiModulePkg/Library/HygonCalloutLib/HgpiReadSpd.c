/* $NoKeywords:$ */

/**
 * @file
 *
 * HgpiReadSpd.c
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: HGPI
 * @e sub-project:
 *
 **/
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
  * ***************************************************************************
  *
 */

#include "Uefi.h"
#include "PiPei.h"
#include "HGPI.h"
#include "Library/HygonBaseLib.h"
#include "Library/HygonCalloutLib.h"
#include "HgpiReadSpd.h"
#include "Ppi/HygonMemPpi.h"
#include <Library/PeiServicesTablePointerLib.h>

#define FILECODE  LIBRARY_HYGONCALLOUTLIB_HGPIREADSPD_FILECODE

/**
 Call the host environment inter0xfa, 0xce, to read an SPD's content.

 @param[in]       FcnData
 @param[in, out]  ReadSpd

 @return          he HGPI Status returned from the callout.
*/
HGPI_STATUS
HgpiReadSpd  (
  IN       UINTN                   FcnData,
  IN OUT   HGPI_READ_SPD_PARAMS   *ReadSpd
  )
{
  EFI_PEI_SERVICES                 **PeiServices;
  PEI_HYGON_PLATFORM_DIMM_SPD_PPI  *PlatformDimmPpi;
  EFI_STATUS                       Status;
  HGPI_STATUS                      HgpiStatus;
  HYGON_MEMORY_INIT_COMPLETE_PPI   *MemoryInitCompletePpiPtr;
  UINT8                            Socket;
  UINT8                            Channel;
  UINT8                            Dimm;
  UINT16                           SpdSize;

  PeiServices = (EFI_PEI_SERVICES **)GetPeiServicesTablePointer ();

  Socket  = ReadSpd->SocketId;
  Channel = ReadSpd->MemChannelId;
  Dimm    = ReadSpd->DimmId;

  IDS_HDT_CONSOLE (MAIN_FLOW, "      HgpiReadSpd Entry\n");

  HgpiStatus = HGPI_SUCCESS;

  SpdSize = PcdGetBool (PcdDDR5Enable) ? DDR5_SPD_SIZE : DDR4_SPD_SIZE;

  if (PcdGetBool (PcdHgpiInternalSpdInfo)) {
    // get spd data from fixed buffer
    // MemInitReadSpdBuffer (ReadSpd);
  } else if (PcdGetBool (PcdHgpiSpdUseHpobData)) {
    Status = (*PeiServices)->LocatePpi (
                               PeiServices,
                               &gHygonMemoryInitCompletePpiGuid,
                               0,
                               NULL,
                               &MemoryInitCompletePpiPtr
                               );
    if (EFI_SUCCESS != Status) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "gHygonMemoryInitCompletePpiGuid Not Found.\n");
      return HGPI_ERROR;
    }

    if (MemoryInitCompletePpiPtr->HygonDimmSpInfo.DimmSpdInfo[Socket][Channel][Dimm].DimmPresent) {
      if (MemoryInitCompletePpiPtr->HygonDimmSpInfo.DimmSpdInfo[Socket][Channel][Dimm].SpdDataPtr != NULL) {
        LibHygonMemCopy (
          ReadSpd->Buffer,
          MemoryInitCompletePpiPtr->HygonDimmSpInfo.DimmSpdInfo[Socket][Channel][Dimm].SpdDataPtr,
          SpdSize,
          &ReadSpd->StdHeader
          );
        IDS_HDT_CONSOLE (MAIN_FLOW, "Get Spd Data success.\n");
        return HGPI_SUCCESS;
      } else {
        IDS_HDT_CONSOLE (MAIN_FLOW, "Dimm present, but no SPD data.\n");
        return HGPI_ERROR;
      }
    } else {
      IDS_HDT_CONSOLE (MAIN_FLOW, "Dimm not present.\n");
      return HGPI_ERROR;
    }
  } else {
    //
    // Locate DIMM SPD read PPI.
    //
    Status = (**PeiServices).LocatePpi (
                               PeiServices,
                               &gHygonPlatformDimmSpdPpiGuid,
                               0,
                               NULL,
                               &PlatformDimmPpi
                               );
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "       PlatformDimmSpdRead.LocatePpi %r\n", Status);
      return HGPI_ERROR;
    }

    //
    // Invoke SPD Read
    //
    Status = PlatformDimmPpi->PlatformDimmSpdRead (PeiServices, PlatformDimmPpi, ReadSpd);

    IDS_HDT_CONSOLE (MAIN_FLOW, "       PlatformDimmSpdRead Exit %r\n", Status);

    if (!EFI_ERROR (Status)) {
      HgpiStatus = HGPI_SUCCESS;
    } else {
      HgpiStatus = HGPI_ERROR;
    }
  }

  return HgpiStatus;
}
