/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP Flash Access DXE Library
 * It will route the Flash access to SMM libary through SMM communication protocol
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AMD PSP UEFI Driver
 * @e sub-project   PspStorage Library
 * @e \$Revision$   @e \$Date$
 *
 *//*****************************************************************************
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
#include <Library/AmdPspFlashAccLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include "AGESA.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Filecode.h>

#define FILECODE LIBRARY_AMDPSPFLASHUPDATELIB_AMDPSPFLASHUPDATELIB_FILECODE


/**
 *  Update Flash region with Buffer
 *
 *  @param[in]  Address   Address on flash
 *  @param[in]  Size      Size of the buffer
 *  @param[in]  Buffer    Pointing to the start of the Buffer
 *
 *  @retval EFI_SUCCESS       The Data save to FLASH successfully
 *  @retval Other             Some error occurs when executing this function.
 *
 **/
EFI_STATUS
PspUpdateFlash (
  IN       UINT32 Address,
  IN       UINT32 Size,
  IN       VOID  *Buffer
  )
{
  EFI_STATUS                      Status;
  UINTN                           FlashStartAddr;
  UINTN                           FlashBytes;
  UINT8                           *TempBuffer;
  UINTN                           NumBytes;
  UINTN                           BlockSize;

  IDS_HDT_CONSOLE_PSP_TRACE ("Updating SPI %x %x %x\n", Address, Size, Buffer);
  //Get the block size
  PspGetFlashBlockSize (&BlockSize);
  //Make sure the address on the boundary of SPI block size
  FlashStartAddr = Address & (~ (BlockSize - 1));
  if (((Address + Size) % BlockSize) == 0) {
    FlashBytes = ((UINTN) Address + Size) - FlashStartAddr;
  } else {
    FlashBytes = (((UINTN) Address + Size + BlockSize) & (~ (BlockSize - 1))) - FlashStartAddr;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("Calc FlashAddr %x %x\n", FlashStartAddr, FlashBytes);
  //Read the block
  TempBuffer = NULL;
//###  TempBuffer = AllocateZeroPool (FlashBytes);
  TempBuffer = AllocateRuntimeZeroPool (FlashBytes);
  ASSERT (TempBuffer != NULL);
  if (TempBuffer == NULL) {
    return (EFI_OUT_OF_RESOURCES);
  }
  CopyMem (TempBuffer, (VOID *) FlashStartAddr, FlashBytes);
  //Update the block
  CopyMem (TempBuffer + (Address - FlashStartAddr), Buffer, Size);
  //Erase the blocks
  NumBytes = FlashBytes;
  Status = EFI_UNSUPPORTED;
  Status = PspEraseFlash (FlashStartAddr, &NumBytes);
  ASSERT (NumBytes == FlashBytes);
  ASSERT (Status == EFI_SUCCESS);
  if (EFI_ERROR (Status)) {
    FreePool (TempBuffer);
    return Status;
  }
  //Write the data
  NumBytes = FlashBytes;
  Status = EFI_UNSUPPORTED;
  Status = PspWriteFlash (FlashStartAddr, &NumBytes, TempBuffer);
  ASSERT (NumBytes == FlashBytes);
  ASSERT (Status == EFI_SUCCESS);
  if (EFI_ERROR (Status)) {
    FreePool (TempBuffer);
    return Status;
  }

  FreePool (TempBuffer);
  IDS_HDT_CONSOLE_PSP_TRACE ("Updated\n");

  return Status;
}
