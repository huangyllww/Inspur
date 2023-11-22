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
#include <PiSmm.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/HygonPspBaseLib.h>
#include <Protocol/SmmBase2.h>

typedef
EFI_STATUS
  EFIAPI
(*FP_PSP_READ_FLASH) (
  IN       UINTN                                        FlashAddress,
  IN OUT   UINTN                                        *NumBytes,
  OUT   UINT8                                        *Buffer
  );

typedef
EFI_STATUS
  EFIAPI
(*FP_PSP_WRITE_FLASH) (
  IN UINTN                                        FlashAddress,
  IN OUT UINTN                                    *NumBytes,
  IN UINT8                                        *Buffer
  );

typedef
EFI_STATUS
  EFIAPI
(*FP_PSP_ERASE_FLASH) (
  IN UINTN                                        FlashAddress,
  IN OUT UINTN                                    *NumBytes
  );

UINTN               mBlockSize, mTotalBlocks;
FP_PSP_READ_FLASH   mPspReadFlash  = NULL;
FP_PSP_WRITE_FLASH  mPspWriteFlash = NULL;
FP_PSP_ERASE_FLASH  mPspEraseFlash = NULL;

/**
 * Retrieve block size of Flash Device
 *
 * @param[in]       BlockSize       Size of each block
 *
 * @retval EFI_SUCCESS      Initial success
 * @retval Others           Error happens during initialize
 */
EFI_STATUS
EFIAPI
PspGetFlashBlockSize (
  OUT   UINTN                                       *BlockSize
  )
{
  *BlockSize = mBlockSize;
  return EFI_SUCCESS;
}

/**
 * Read data from Flash device
 *
 * @param[in]       FlashAddress Physical flash address
 * @param[in, out]  NumBytes     Number in Byte; return Bytes been read on output
 * @param[in]       Buffer       Buffer contain the read data (Allocated by caller)
 *
 * @retval EFI_SUCCESS      Initial success
 * @retval Others           Error happens during initialize
 */
EFI_STATUS
EFIAPI
PspReadFlash (
  IN       UINTN                                        FlashAddress,
  IN OUT   UINTN                                        *NumBytes,
  OUT   UINT8                                        *Buffer
  )
{
  EFI_STATUS  Status;

  if (mPspReadFlash == NULL) {
    return (EFI_NOT_FOUND);
  }

  DEBUG ((EFI_D_INFO, "PspReadFlash %x %x\n", FlashAddress, *NumBytes));

  Status =  mPspReadFlash (
              FlashAddress,
              NumBytes,
              Buffer
              );

  return Status;
}

/**
 * Write data to Flash device
 *
 * @param[in]       FlashAddress Physical flash address
 * @param[in, out]  NumBytes     Number in Byte; return Bytes been written on output
 * @param[in]       Buffer       Buffer contain the written data (Allocated by caller)
 *
 * @retval EFI_SUCCESS      Initial success
 * @retval Others           Error happens during initialize
 */
EFI_STATUS
EFIAPI
PspWriteFlash (
  IN       UINTN                                    FlashAddress,
  IN OUT   UINTN                                    *NumBytes,
  IN       UINT8                                    *Buffer
  )
{
  EFI_STATUS  Status;

  if (mPspWriteFlash == NULL) {
    return (EFI_NOT_FOUND);
  }

  DEBUG ((EFI_D_INFO, "PspWriteFlash %x %x\n", FlashAddress, *NumBytes));

  Status = mPspWriteFlash (
             FlashAddress,
             NumBytes,
             Buffer
             );

  return Status;
}

/**
 * Erase Flash region according to input in unit of block size
 *
 *
 * @param[in]       FlashAddress Physical flash address
 * @param[in, out]  NumBytes     Number in Byte; return Bytes been erased on output
 *
 * @retval EFI_SUCCESS      Initial success
 * @retval Others           Error happens during initialize
 */
EFI_STATUS
EFIAPI
PspEraseFlash (
  IN       UINTN                                    FlashAddress,
  IN OUT   UINTN                                    *NumBytes
  )
{
  EFI_STATUS  Status;

  if (mPspEraseFlash == NULL) {
    return (EFI_NOT_FOUND);
  }

  DEBUG ((EFI_D_INFO, "PspEraseFlash %x %x\n", FlashAddress, *NumBytes));

  Status = mSmmFlashAccessProtocol->EraseBlocks (
                                      mSmmFlashAccessProtocol,
                                      FlashAddress,
                                      NumBytes
                                      );

  return (Status);
}

/**
 * Initial Psp Storage required information
 *
 *
 * @param[in]  SystemTable Point to EFI system table structure
 *
 * @retval EFI_SUCCESS    Initial success
 * @retval Others         Error happens during initialize
 */
EFI_STATUS
EFIAPI
HygonPspFlashAccLibSmmConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS              Status;
  EFI_SMM_BASE2_PROTOCOL  *SmmBase2;
  BOOLEAN                 InSmm;
  EFI_SMM_SYSTEM_TABLE2   *Smst;

  DEBUG ((EFI_D_INFO, "HygonPspFlashAccLibSmmConstructor Entry\n"));

  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID **)&SmmBase2);
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  Status = SmmBase2->InSmm (SmmBase2, &InSmm);
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  if (!InSmm) {
    return EFI_SUCCESS;
  }

  Status = SmmBase2->GetSmstLocation (SmmBase2, &Smst);
  if (EFI_ERROR (Status)) {
    return RETURN_SUCCESS;
  }

  // Locate Flash access library required information, the implementation can be varies
  mBlockSize     =  < Filled by OEM >;
  mPspReadFlash  = < Filled by OEM >;
  mPspWriteFlash = < Filled by OEM >;
  mPspEraseFlash =  < Filled by OEM >;

  return EFI_SUCCESS;
}
