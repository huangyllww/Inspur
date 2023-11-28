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

#include <Library/UefiBootServicesTableLib.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Library/AmdPspFlashAccLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/SmmCommunication.h>

#include "AGESA.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <AmdPspSmmCommunication.h>
#include <Filecode.h>

#define FILECODE LIBRARY_AMDPSPFLASHACCLIBDXE_AMDPSPFLASHACCLIBDXE_FILECODE

EFI_SMM_COMMUNICATION_PROTOCOL *mSmmCommunication;
BOOLEAN mPspFlashAccSmmCommReady = FALSE;

extern EFI_GUID gPspFlashAccSmmCommReadyProtocolGuid;

EFI_STATUS
InitSmmComProtocol (
  VOID
  )
{
  EFI_STATUS  Status;
  VOID        *Interface;

  //Check if gPspFlashAccSmmCommReadyProtocolGuid is already installed
  if (mPspFlashAccSmmCommReady == FALSE) {
    Status = gBS->LocateProtocol (&gPspFlashAccSmmCommReadyProtocolGuid, NULL, &Interface);
    ASSERT (Status == EFI_SUCCESS);
    if (EFI_ERROR (Status)) {
      //If you see this error, please notify this Protocol in your drivers
      IDS_HDT_CONSOLE_PSP_TRACE ("AmdPspFlashAccLibDxe locate gPspFlashAccSmmCommReadyProtocol fail \n");
      return Status;
    }
    mPspFlashAccSmmCommReady = TRUE;
  }

  if (mSmmCommunication == NULL) {
    Status = gBS->LocateProtocol (&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **) &mSmmCommunication);
    ASSERT (Status == EFI_SUCCESS);
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("AmdPspFlashAccLibDxe locate SmmCommunicationProtocol fail \n");
      return Status;
    }
  }
  return EFI_SUCCESS;
}
/**
 * Retrieve block size of Flash Device
 *
 * @param[in]       Lba             Start LBA of PSP NVRAM region
 * @param[in]       BlockSize       Size of each block
 * @param[in, out]  NumberOfBlocks  Number of block
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
  EFI_STATUS  Status;
  UINT8       *Buffer;
  EFI_SMM_COMMUNICATE_HEADER *SmmCommBuff;
  PSP_SMM_COMM_GETBLKSIZE  *PspSmmGetBlkBuff;
  UINTN       SmmCommBufferSize;

  Status = InitSmmComProtocol ();
  if (EFI_ERROR (Status)) {
    return (EFI_NOT_FOUND);
  }
  //Init SMM communication buffer header
  Buffer = (UINT8 *) (UINTN) PcdGet64 (PcdAmdSmmCommunicationAddress);
  ASSERT (Buffer != NULL);
  if (Buffer != NULL) {
    ZeroMem (Buffer, PSP_SMM_COMM_GETBLKSIZE_BUFFER_SIZE);
    SmmCommBuff = (EFI_SMM_COMMUNICATE_HEADER *) Buffer;
    CopyGuid (&SmmCommBuff->HeaderGuid, &gPspSmmCommHandleGuid);
    SmmCommBuff->MessageLength = sizeof (PSP_SMM_COMM_GETBLKSIZE);
    //Init PSP SMM communicate private data
    PspSmmGetBlkBuff = (PSP_SMM_COMM_GETBLKSIZE *) &SmmCommBuff->Data;
    PspSmmGetBlkBuff->id = PSP_SMM_COMM_ID_GET_BLOCK_SIZE;


    //Communicate AMD SMM communication handler to save boot script inside SMM
    SmmCommBufferSize = PSP_SMM_COMM_GETBLKSIZE_BUFFER_SIZE;
    mSmmCommunication->Communicate (mSmmCommunication, &Buffer[0], &SmmCommBufferSize);

    *BlockSize = PspSmmGetBlkBuff->BlockSize;
    IDS_HDT_CONSOLE_PSP_TRACE ("PspGetFlashBlockSizeDxe %x \n", PspSmmGetBlkBuff->BlockSize);
  }
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
  UINT8       *RwBuffer;
  EFI_SMM_COMMUNICATE_HEADER *SmmCommBuff;
  PSP_SMM_COMM_RW_FLASH  *PspSmmRwFlashBuff;
  UINTN       SmmCommBufferSize;

  Status = InitSmmComProtocol ();
  if (EFI_ERROR (Status)) {
    return (EFI_NOT_FOUND);
  }
  //Check input Parameters
  if ((*NumBytes == 0) || (Buffer == NULL)) {
    return (EFI_INVALID_PARAMETER);
  }
  //Init SMM communication buffer header
  RwBuffer = (UINT8 *) (UINTN) PcdGet64 (PcdAmdSmmCommunicationAddress);

  ASSERT (RwBuffer != NULL);
  if (RwBuffer != NULL) {
    ZeroMem (RwBuffer, PSP_SMM_COMM_RWFLASH_BUFFER_SIZE);
    SmmCommBuff = (EFI_SMM_COMMUNICATE_HEADER *) RwBuffer;
    CopyGuid (&SmmCommBuff->HeaderGuid, &gPspSmmCommHandleGuid);
    SmmCommBuff->MessageLength = sizeof (PSP_SMM_COMM_RW_FLASH);
    //Init PSP SMM communicate private data
    PspSmmRwFlashBuff = (PSP_SMM_COMM_RW_FLASH *) &((EFI_SMM_COMMUNICATE_HEADER *) RwBuffer)->Data;
    PspSmmRwFlashBuff->id = PSP_SMM_COMM_ID_READ_FLASH;
    PspSmmRwFlashBuff->FlashAddress = FlashAddress;
    PspSmmRwFlashBuff->NumBytes = *NumBytes;
    PspSmmRwFlashBuff->Buffer = Buffer;

    //Communicate AMD SMM communication handler to save boot script inside SMM
    SmmCommBufferSize = PSP_SMM_COMM_RWFLASH_BUFFER_SIZE;
    mSmmCommunication->Communicate (mSmmCommunication, RwBuffer, &SmmCommBufferSize);

    *NumBytes = PspSmmRwFlashBuff->NumBytes;
    IDS_HDT_CONSOLE_PSP_TRACE ("PspReadFlashDxe %x %x %x \n", PspSmmRwFlashBuff->FlashAddress, PspSmmRwFlashBuff->NumBytes, PspSmmRwFlashBuff->Buffer);
  }

  return EFI_SUCCESS;
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
  UINT8       *RwBuffer;
  EFI_SMM_COMMUNICATE_HEADER *SmmCommBuff;
  PSP_SMM_COMM_RW_FLASH  *PspSmmRwFlashBuff;
  UINTN       SmmCommBufferSize;

  Status = InitSmmComProtocol ();
  if (EFI_ERROR (Status)) {
    return (EFI_NOT_FOUND);
  }
  //Check input Parameters
  if ((*NumBytes == 0) || (Buffer == NULL)) {
    return (EFI_INVALID_PARAMETER);
  }
  //Init SMM communication buffer header
  RwBuffer = (UINT8 *) (UINTN) PcdGet64 (PcdAmdSmmCommunicationAddress);

  ASSERT (RwBuffer != NULL);
  if (RwBuffer != NULL) {
    ZeroMem (RwBuffer, PSP_SMM_COMM_RWFLASH_BUFFER_SIZE);
    SmmCommBuff = (EFI_SMM_COMMUNICATE_HEADER *) RwBuffer;
    CopyGuid (&SmmCommBuff->HeaderGuid, &gPspSmmCommHandleGuid);
    SmmCommBuff->MessageLength = sizeof (PSP_SMM_COMM_RW_FLASH);
    //Init PSP SMM communicate private data
    PspSmmRwFlashBuff = (PSP_SMM_COMM_RW_FLASH *) &((EFI_SMM_COMMUNICATE_HEADER *) RwBuffer)->Data;

    PspSmmRwFlashBuff->id = PSP_SMM_COMM_ID_WRITE_FALSH;
    PspSmmRwFlashBuff->FlashAddress = FlashAddress;
    PspSmmRwFlashBuff->NumBytes = *NumBytes;
    PspSmmRwFlashBuff->Buffer = Buffer;

    //Communicate AMD SMM communication handler to save boot script inside SMM
    SmmCommBufferSize = PSP_SMM_COMM_RWFLASH_BUFFER_SIZE;
    mSmmCommunication->Communicate (mSmmCommunication, RwBuffer, &SmmCommBufferSize);

    *NumBytes = PspSmmRwFlashBuff->NumBytes;
    IDS_HDT_CONSOLE_PSP_TRACE ("PspWriteFlash %x %x %x \n", PspSmmRwFlashBuff->FlashAddress, PspSmmRwFlashBuff->NumBytes, PspSmmRwFlashBuff->Buffer);
  }

  return EFI_SUCCESS;
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
  UINT8       *Buffer;
  EFI_SMM_COMMUNICATE_HEADER *SmmCommBuff;
  PSP_SMM_COMM_ERASEFLASH  *PspSmmEraseFlashBuff;
  UINTN       SmmCommBufferSize;

  Status = InitSmmComProtocol ();
  if (EFI_ERROR (Status)) {
    return (EFI_NOT_FOUND);
  }
  //Check input Parameters
  if (*NumBytes == 0) {
    return (EFI_INVALID_PARAMETER);
  }
  //Init SMM communication buffer header
  Buffer = (UINT8 *) (UINTN) PcdGet64 (PcdAmdSmmCommunicationAddress);
  ASSERT (Buffer != NULL);
  if (Buffer != NULL) {
    ZeroMem (Buffer, PSP_SMM_COMM_ERASEFLASH_BUFFER_SIZE);
    SmmCommBuff = (EFI_SMM_COMMUNICATE_HEADER *) Buffer;
    CopyGuid (&SmmCommBuff->HeaderGuid, &gPspSmmCommHandleGuid);
    SmmCommBuff->MessageLength = sizeof (PSP_SMM_COMM_ERASEFLASH);
    //Init PSP SMM communicate private data
    PspSmmEraseFlashBuff = (PSP_SMM_COMM_ERASEFLASH *) &(((EFI_SMM_COMMUNICATE_HEADER *) Buffer)->Data);
    PspSmmEraseFlashBuff->id = PSP_SMM_COMM_ID_ERASE_FALSH;
    PspSmmEraseFlashBuff->FlashAddress = FlashAddress;
    PspSmmEraseFlashBuff->NumBytes = *NumBytes;

    //Communicate AMD SMM communication handler to save boot script inside SMM
    SmmCommBufferSize = PSP_SMM_COMM_ERASEFLASH_BUFFER_SIZE;
    mSmmCommunication->Communicate (mSmmCommunication, Buffer, &SmmCommBufferSize);

    *NumBytes = PspSmmEraseFlashBuff->NumBytes;
    IDS_HDT_CONSOLE_PSP_TRACE ("PspEraseFlash %x %x \n", PspSmmEraseFlashBuff->FlashAddress, PspSmmEraseFlashBuff->NumBytes);
  }

  return EFI_SUCCESS;
}


