/* $NoKeywords:$ */

/**
 * @file
 *
 * PspP2Cmbox Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HYGON PSP UEFI Driver
 * @e sub-project   PspP2Cmbox Driver
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
#include "HygonPspP2Cmbox.h"
#include <HygonPspSmmCommunication.h>
#include <Library/HygonPspFlashAccLib.h>
#include <Filecode.h>

#define FILECODE  PSP_HYGONPSPP2CMBOX_HYGONPSPFLASHACCSMMHDLR_FILECODE

extern UINTN              mBlockSize;
extern PSP_NV_DESCRIPTOR  mPspNvDb[];
extern EFI_HANDLE         mPspSmmCommHandle;

EFI_STATUS
PspFlashAccSmmCommunicateHandler (
  IN       EFI_HANDLE  DispatchHandle,
  IN       CONST VOID  *Context,
  IN OUT   VOID    *CommBuffer,
  IN OUT   UINTN   *CommBufferSize
  )
{
  PSP_SMM_COMMUNICATION_CMN  *Buffer;
  PSP_SMM_COMM_RW_FLASH      *RwBuffer;
  PSP_SMM_COMM_ERASEFLASH    *EraseBuffer;

  Buffer = (PSP_SMM_COMMUNICATION_CMN *)CommBuffer;
  IDS_HDT_CONSOLE_PSP_TRACE ("PspFlashAccSmmCommunicateHandler ID %x\n", Buffer->id);

  switch (Buffer->id) {
    case PSP_SMM_COMM_ID_GET_BLOCK_SIZE:
      PspGetFlashBlockSize (&((PSP_SMM_COMM_GETBLKSIZE *)CommBuffer)->BlockSize);
      break;
    case PSP_SMM_COMM_ID_READ_FLASH:
      RwBuffer = (PSP_SMM_COMM_RW_FLASH *)CommBuffer;
      PspReadFlash (RwBuffer->FlashAddress, &RwBuffer->NumBytes, RwBuffer->Buffer);
      break;
    case PSP_SMM_COMM_ID_WRITE_FALSH:
      RwBuffer = (PSP_SMM_COMM_RW_FLASH *)CommBuffer;
      PspWriteFlash (RwBuffer->FlashAddress, &RwBuffer->NumBytes, RwBuffer->Buffer);
      break;
    case PSP_SMM_COMM_ID_ERASE_FALSH:
      EraseBuffer = (PSP_SMM_COMM_ERASEFLASH *)CommBuffer;
      PspEraseFlash (EraseBuffer->FlashAddress, &EraseBuffer->NumBytes);
      break;
    case PSP_SMM_COMM_ID_CLOSE_INTERFACE:
      if (mPspSmmCommHandle != NULL) {
        gSmst->SmiHandlerUnRegister (mPspSmmCommHandle);
      } else {
        IDS_HDT_CONSOLE_PSP_TRACE ("mPspSmmCommHandle NULL\n");
      }

      break;
    default:
      ASSERT (FALSE);
  }

  return EFI_SUCCESS;
}
