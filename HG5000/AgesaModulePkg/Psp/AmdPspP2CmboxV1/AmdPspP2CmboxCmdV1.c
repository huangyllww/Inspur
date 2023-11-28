
/* $NoKeywords:$ */
/**
 * @file
 *
 * PspP2Cmbox Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AMD PSP UEFI Driver
 * @e sub-project   PspP2Cmbox Driver
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
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
#include "AmdPspP2CmboxV1.h"
#include <Filecode.h>

#define FILECODE PSP_AMDPSPP2CMBOXV1_AMDPSPP2CMBOXCMDV1_FILECODE

extern UINT64                                mPspNvRamRegionBase;
extern UINT32                                mPspNvRamRegionSize;
extern UINTN                                 mBlockSize;

EFI_STATUS
EFIAPI
P2CmboxSpiGetBlockSize (
  IN OUT   SPI_INFO_REQ *Req
  )
{
  ASSERT (mPspNvRamRegionSize != 0);
  ASSERT (mPspNvRamRegionSize % mBlockSize == 0);

  Req->NumberOfBlocks = (mPspNvRamRegionSize / mBlockSize);
  Req->BlockSize = mBlockSize;

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.SpiGetBlockSize Lba:0x%x BlockSize:0x%x NumberOfBlocks:0x%x\n",
         Req->Lba,
         Req->BlockSize,
         Req->NumberOfBlocks);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
P2CmboxSpiReadFv (
  IN OUT   SPI_RW_REQ *Req
  )
{
  EFI_STATUS Status;
  UINTN NumBytes;

  NumBytes = Req->NumByte;
  Status = PspReadFlash ( mPspNvRamRegionBase + (Req->Lba * mBlockSize) + Req->Offset, &NumBytes, &Req->Buffer[0]);
  ASSERT (NumBytes == Req->NumByte);

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.SpiReadFv Lba:0x%x Offset:0x%x NumByte:0x%x Buffer:0x%x\n",
          Req->Lba,
          Req->Offset,
          Req->NumByte,
          &Req->Buffer);
  return (Status);
}

EFI_STATUS
EFIAPI
P2CmboxSpiWriteFv (
  IN OUT   SPI_RW_REQ  *Req
  )
{
  EFI_STATUS Status;
  UINTN NumBytes;

  NumBytes = Req->NumByte;
  Status = PspWriteFlash ( mPspNvRamRegionBase + (Req->Lba * mBlockSize) + Req->Offset, &NumBytes, &Req->Buffer[0]);
  ASSERT (NumBytes == Req->NumByte);

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.SpiWriteFv Lba:0x%x Offset:0x%x NumByte:0x%x Buffer:0x%x\n",
          Req->Lba,
          Req->Offset,
          Req->NumByte,
          &Req->Buffer);

  return (Status);

}

EFI_STATUS
EFIAPI
P2CmboxSpiEraseFv (
  IN OUT   SPI_ERASE_REQ *Req
  )
{
  EFI_STATUS Status;
  UINTN NumBytes;

  NumBytes = Req->NumberOfBlocks * mBlockSize;
  Status = PspEraseFlash (  mPspNvRamRegionBase + (Req->Lba * mBlockSize), &NumBytes);
  ASSERT (NumBytes == Req->NumberOfBlocks * mBlockSize);

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.SpiEraseFv Lba:0x%x NumberOfBlocks:0x%x\n",
          Req->Lba,
          Req->NumberOfBlocks);
  return (Status);
}

EFI_STATUS
EFIAPI
ProcessPspCommand (
  IN UINT64                 Cmd,
  IN MBOX_DEFAULT_BUFFER   *MboxBuffer
  )
{

  EFI_STATUS Status = EFI_NOT_FOUND;

  switch (Cmd) {
  case MboxPspCmdSpiGetAttrib:
    AGESA_TESTPOINT (TpPspP2CmboxSpiGetAttribEntry, NULL);
    Status = EFI_SUCCESS;
    break;
  case MboxPspCmdSpiSetAttrib:
    AGESA_TESTPOINT (TpPspP2CmboxSpiSetAttribEntry, NULL);
    Status = EFI_SUCCESS;
    break;

  case MboxPspCmdSpiGetBlockSize :
    AGESA_TESTPOINT (TpPspP2CmboxSpiGetBlockSizeEntry, NULL);
    Status = P2CmboxSpiGetBlockSize ( &(((MBOX_SPI_INFO_BUFFER *)MboxBuffer)->Req) );
    break;

  case MboxPspCmdSpiReadFV:
    AGESA_TESTPOINT (TpPspP2CmboxSpiReadFVEntry, NULL);
    Status = P2CmboxSpiReadFv (&(((MBOX_SPI_RW_BUFFER *)MboxBuffer)->Req));
    break;

  case MboxPspCmdSpiWriteFV:
    AGESA_TESTPOINT (TpPspP2CmboxSpiWriteFVEntry, NULL);
    Status = P2CmboxSpiWriteFv (&(((MBOX_SPI_RW_BUFFER *)MboxBuffer)->Req) );
    break;

  case MboxPspCmdSpiEraseFV:
    AGESA_TESTPOINT (TpPspP2CmboxSpiEraseFVEntry, NULL);
    Status = P2CmboxSpiEraseFv (&(((MBOX_SPI_ERASE_BUFFER *)MboxBuffer)->Req));
    break;

  default:
    IDS_HDT_CONSOLE_PSP_TRACE ("Unsupported P2C Command 0x%x\n", Cmd);
    break;
  }

  MboxBuffer->Header.Status  = (UINT32)Status;

  return (Status);
}

