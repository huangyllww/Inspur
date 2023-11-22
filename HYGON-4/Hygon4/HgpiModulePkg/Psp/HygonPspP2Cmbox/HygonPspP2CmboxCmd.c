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
#include <Filecode.h>

#define FILECODE  PSP_HYGONPSPP2CMBOX_HYGONPSPP2CMBOXCMD_FILECODE

extern UINTN              mBlockSize;
extern PSP_NV_DESCRIPTOR  mPspNvDb[];

EFI_STATUS
GetPspNvDescriptor (
  IN       UINT64 TargetNvId,
  OUT   PSP_NV_DESCRIPTOR **PspNvDescriptor
  )
{
  PSP_NV_DESCRIPTOR  *_PspNvDescriptor;

  IDS_HDT_CONSOLE_PSP_TRACE ("GetPspNvDescriptor %x\n", TargetNvId);

  for (_PspNvDescriptor = &mPspNvDb[0]; _PspNvDescriptor->TargetID != SMI_TARGET_END; _PspNvDescriptor++) {
    if (TargetNvId == _PspNvDescriptor->TargetID) {
      if (_PspNvDescriptor->Detected) {
        *PspNvDescriptor = _PspNvDescriptor;
        return EFI_SUCCESS;
      } else {
        IDS_HDT_CONSOLE_PSP_TRACE ("TargetNvId is not Detected\n");
        return EFI_NOT_FOUND;
      }
    }
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("TargetNvId is not in the DB\n");
  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
P2CmboxSpiGetBlockSize (
  IN OUT   SPI_INFO_REQ *Req
  )
{
  PSP_NV_DESCRIPTOR  *PspNvDescriptor;
  EFI_STATUS         Status;

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.SpiGetBlockSize TargetNvId %x\n", Req->TargetNvId);
  Status = EFI_UNSUPPORTED;
  Status = GetPspNvDescriptor (Req->TargetNvId, &PspNvDescriptor);
  ASSERT (Status == EFI_SUCCESS);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (PspNvDescriptor->NvRegionSize % mBlockSize == 0);
  Req->BlockSize = mBlockSize;
  Req->NumberOfBlocks = (PspNvDescriptor->NvRegionSize / mBlockSize);

  IDS_HDT_CONSOLE_PSP_TRACE (
    "Return Lba:0x%x BlockSize:0x%x NumberOfBlocks:0x%x\n",
    Req->Lba,
    Req->BlockSize,
    Req->NumberOfBlocks
    );

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
P2CmboxSpiReadFv (
  IN OUT   SPI_RW_REQ *Req
  )
{
  EFI_STATUS         Status;
  UINTN              NumBytes;
  PSP_NV_DESCRIPTOR  *PspNvDescriptor;

  IDS_HDT_CONSOLE_PSP_TRACE (
    "Psp.SpiReadFv TargetNvId %x Lba:0x%x Offset:0x%x NumByte:0x%x Buffer:0x%x\n",
    Req->TargetNvId,
    Req->Lba,
    Req->Offset,
    Req->NumByte,
    &Req->Buffer
    );

  Status = EFI_UNSUPPORTED;
  Status = GetPspNvDescriptor (Req->TargetNvId, &PspNvDescriptor);
  ASSERT (Status == EFI_SUCCESS);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  NumBytes = Req->NumByte;
  IDS_HDT_CONSOLE_PSP_TRACE ("Call FlashAccLib PspReadFlash\n");
  Status = PspReadFlash (PspNvDescriptor->NvRegionBase + (Req->Lba * mBlockSize) + Req->Offset, &NumBytes, &Req->Buffer[0]);
  IDS_HDT_CONSOLE_PSP_TRACE ("Return Sts[%x] Bytes[%xs]\n", Status, NumBytes);
  ASSERT (NumBytes == Req->NumByte);
  return (Status);
}

EFI_STATUS
EFIAPI
P2CmboxSpiWriteFv (
  IN OUT   SPI_RW_REQ  *Req
  )
{
  EFI_STATUS         Status;
  UINTN              NumBytes;
  PSP_NV_DESCRIPTOR  *PspNvDescriptor;

  IDS_HDT_CONSOLE_PSP_TRACE (
    "Psp.SpiWriteFv TargetNvId %x Lba:0x%x Offset:0x%x NumByte:0x%x Buffer:0x%x\n",
    Req->TargetNvId,
    Req->Lba,
    Req->Offset,
    Req->NumByte,
    &Req->Buffer
    );

  Status = EFI_UNSUPPORTED;
  Status = GetPspNvDescriptor (Req->TargetNvId, &PspNvDescriptor);
  ASSERT (Status == EFI_SUCCESS);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  NumBytes = Req->NumByte;
  IDS_HDT_CONSOLE_PSP_TRACE ("Call FlashAccLib PspWriteFlash\n");
  Status = PspWriteFlash (PspNvDescriptor->NvRegionBase + (Req->Lba * mBlockSize) + Req->Offset, &NumBytes, &Req->Buffer[0]);
  IDS_HDT_CONSOLE_PSP_TRACE ("Return Sts[%x] Bytes[%xs]\n", Status, NumBytes);
  ASSERT (NumBytes == Req->NumByte);

  return (Status);
}

EFI_STATUS
EFIAPI
P2CmboxSpiEraseFv (
  IN OUT   SPI_ERASE_REQ *Req
  )
{
  EFI_STATUS         Status;
  UINTN              NumBytes;
  PSP_NV_DESCRIPTOR  *PspNvDescriptor;

  IDS_HDT_CONSOLE_PSP_TRACE (
    "Psp.SpiEraseFv TargetNvId 0x%x LBA 0x%x NumBlock 0x%x\n",
    Req->TargetNvId,
    Req->Lba,
    Req->NumberOfBlocks
    );

  Status = EFI_UNSUPPORTED;
  Status = GetPspNvDescriptor (Req->TargetNvId, &PspNvDescriptor);
  ASSERT (Status == EFI_SUCCESS);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  NumBytes = Req->NumberOfBlocks * mBlockSize;
  IDS_HDT_CONSOLE_PSP_TRACE ("Call FlashAccLib PspEraseFlash\n");
  Status = PspEraseFlash (PspNvDescriptor->NvRegionBase + (Req->Lba * mBlockSize), &NumBytes);
  IDS_HDT_CONSOLE_PSP_TRACE ("Return Sts[%x] Bytes[%xs]\n", Status, NumBytes);
  ASSERT (NumBytes == Req->NumberOfBlocks * mBlockSize);
  return (Status);
}

EFI_STATUS
EFIAPI
ProcessPspCommand (
  IN UINT64                 Cmd,
  IN MBOX_DEFAULT_BUFFER   *MboxBuffer
  )
{
  EFI_STATUS  Status = EFI_NOT_FOUND;

  switch (Cmd) {
    case MboxPspCmdSpiGetAttrib:
      HGPI_TESTPOINT (TpPspP2CmboxSpiGetAttribEntry, NULL);
      Status = EFI_SUCCESS;
      break;
    case MboxPspCmdSpiSetAttrib:
      HGPI_TESTPOINT (TpPspP2CmboxSpiSetAttribEntry, NULL);
      Status = EFI_SUCCESS;
      break;

    case MboxPspCmdSpiGetBlockSize:
      HGPI_TESTPOINT (TpPspP2CmboxSpiGetBlockSizeEntry, NULL);
      Status = P2CmboxSpiGetBlockSize (&(((MBOX_SPI_INFO_BUFFER *)MboxBuffer)->Req));
      break;

    case MboxPspCmdSpiReadFV:
      HGPI_TESTPOINT (TpPspP2CmboxSpiReadFVEntry, NULL);
      Status = P2CmboxSpiReadFv (&(((MBOX_SPI_RW_BUFFER *)MboxBuffer)->Req));
      break;

    case MboxPspCmdSpiWriteFV:
      HGPI_TESTPOINT (TpPspP2CmboxSpiWriteFVEntry, NULL);
      Status = P2CmboxSpiWriteFv (&(((MBOX_SPI_RW_BUFFER *)MboxBuffer)->Req));
      break;

    case MboxPspCmdSpiEraseFV:
      HGPI_TESTPOINT (TpPspP2CmboxSpiEraseFVEntry, NULL);
      Status = P2CmboxSpiEraseFv (&(((MBOX_SPI_ERASE_BUFFER *)MboxBuffer)->Req));
      break;

    default:
      IDS_HDT_CONSOLE_PSP_TRACE ("Unsupported P2C Command 0x%x\n", Cmd);
      break;
  }

  MboxBuffer->Header.Status = (UINT32)Status;

  return (Status);
}
