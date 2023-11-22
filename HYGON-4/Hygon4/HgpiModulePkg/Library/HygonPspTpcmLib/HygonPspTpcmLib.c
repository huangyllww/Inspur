/* $NoKeywords:$ */

/**
 * @file
 *
 * PSP Mailbox related functions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  PSP
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
 * ***************************************************************************
 *
 */

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include "HGPI.h"
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/HygonPspBaseLib.h>
#include <Library/HygonPspMboxLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HygonPspTpcmLib.h>

#define HTON16(val)  SwapBytes16 (val)
#define NTOH16(val)  SwapBytes16 (val)
#define HTON32(val)  SwapBytes32 (val)
#define NTOH32(val)  SwapBytes32 (val)
#define HTON64(val)  SwapBytes64 (val)
#define NTOH64(val)  SwapBytes64 (val)

#define TAG_COMMAND          0xC1
#define CODE_COMMAND         0x1001
#define TAG_RESPONSE         0xC4
#define CTRL_CODE_XBIT_MASK  0x1
#define MEASURE_CODE_SHIFT   16

#define STAGE_OS_LOADER  1000
#define STAGE_OS         2000

#define MASK_64M  (~(0x3ffffffULL))
#define WITHIN_64M_SEG(Buf, Size)  ((((UINT64)(Buf) & MASK_64M) == (((UINT64)(Buf) + (Size) - 1) & MASK_64M)) \
        ? (Buf) : ((UINT8 *)(Buf) + (Size)))

/**
  BIOS requests PSP to verify the image loaded in memory. This command is normally used for
  secure boot purpose as in application of PSP being TPM.

  @param[in]    MeasureStage   The current measurement stage
  @param[in]    ImageInfo      Image description info in string
  @param[in]    NumAddrRange   Number of address ranges provided in the range array
  @param[in]    Ranges         Array of address ranges
  @param[out]   MeasureResult  Result of the measurement itself, 0 on success, otherwise error code.
  @param[out]   ControlResult  The execution control result based on the measurement result and
                               possible internal measurement policy in PSP. 0 - execution allowed,
                               1 - not allowed.

  @retval EFI_SUCCESS  The function return without any program error.
  @retval Others       Error code indicate program error, and normally the measurement result and
                       control result should be deemed as invalid.
**/
EFI_STATUS
PspMboxImageVerify (
  IN  UINT32    MeasureStage,
  IN  VOID     *ImageInfo,
  IN  UINT32    ImageInfoSize,
  IN  UINT32    NumAddrRange,
  IN  IMAGE_ADDR_RANGE Ranges[],
  OUT UINT32   *MeasureResult,
  OUT UINT32   *ControlResult
  )
{
  MBOX_IMAGE_VERIFY_BUF  *Buffer;
  UINT8                  Buff[2 * sizeof (*Buffer)]; // twice the size to make sure always can find the buffer within one 64MB segment.
  MBOX_IMAGE_VERIFY_CMD  *cmd;
  UINT32                 Buff1Size;
  UINT8                  *Buff1;
  // UINT8   Buff1[2 * sizeof(*cmd)];
  MBOX_IMAGE_VERIFY_RESPONSE  *resp;
  UINT8                       Buff2[2 * sizeof (*resp)];
  INT32                       CmdRet = 0;
  RANGE_NAME                  *Name;
  UINT32                      RangeNameOffset;
  UINT32                      Idx;

  if (!MeasureResult || !ControlResult) {
    return (EFI_INVALID_PARAMETER);
  }

  RangeNameOffset = sizeof (*cmd) + sizeof (ADDR_RANGE) * NumAddrRange;
  Buff1Size = (RangeNameOffset + sizeof (RANGE_NAME)) * 2;

  Buff1 = AllocatePool (Buff1Size);
  if (!Buff1) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Allocate memory %uB for command buffer failed\n", Buff1Size);
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (&Buff[0], sizeof (Buff));
  ZeroMem (&Buff1[0], Buff1Size);
  ZeroMem (&Buff2[0], sizeof (Buff2));

  Buffer = (MBOX_IMAGE_VERIFY_BUF *)WITHIN_64M_SEG (&Buff[0], sizeof (*Buffer));
  cmd    = (MBOX_IMAGE_VERIFY_CMD *)WITHIN_64M_SEG (&Buff1[0], Buff1Size / 2);
  resp   = (MBOX_IMAGE_VERIFY_RESPONSE *)WITHIN_64M_SEG (&Buff2[0], sizeof (*resp));
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.PspMboxImageVerify\n");

  cmd->Tag    = HTON32 (TAG_COMMAND);
  cmd->MsgLen = HTON32 (Buff1Size / 2);
  cmd->Code   = HTON32 (CODE_COMMAND);
  cmd->Stage  = HTON32 (MeasureStage);
  cmd->NumAddrRange = HTON32 (NumAddrRange);

  for (Idx = 0; Idx < NumAddrRange; Idx++) {
    cmd->Ranges[Idx].StartAddr = HTON64 (Ranges[Idx].Start);
    cmd->Ranges[Idx].Length    = HTON32 ((UINT32)Ranges[Idx].Length);
  }

  Name = (RANGE_NAME *)((UINT8 *)cmd + RangeNameOffset);
  Name->Length    = HTON32 (ImageInfoSize);
  Name->StartAddr = HTON64 ((UINT64)ImageInfo);

  Buffer->input_addr       = (UINT64)cmd;
  Buffer->input_length     = Buff1Size / 2;
  Buffer->output_addr      = (UINT64)resp;
  Buffer->output_maxlength = sizeof (*resp);

  CmdRet = SendPspCommandCh2 (Buffer, MboxBiosCh2CmdImageVerify);
  if (CmdRet == (FALSE)) {
    FreePool (Buff1);
    return EFI_DEVICE_ERROR;
  }

  if (NTOH32 (resp->Tag) != TAG_RESPONSE || NTOH32 (resp->MsgLen) != sizeof (*resp) || Buffer->output_maxlength != sizeof (*resp)) {
    FreePool (Buff1);
    return EFI_DEVICE_ERROR;
  }

  if ((NTOH16 (resp->CtrlCode) & CTRL_CODE_XBIT_MASK) != 0) {
    *ControlResult = 1;
  } else {
    *ControlResult = 0;
  }

  *MeasureResult = NTOH16 (resp->RetCode);
  FreePool (Buff1);

  return EFI_SUCCESS;
}

/**
  BIOS requests PSP to verify OS loader loaded in memory. This command is normally used for
  secure boot purpose as in application of PSP being TPM.

  @param[in]  StartAddr           Start address (physical) of the image currently loaded in memory.
  @param[in]  Length              The length of the image in memory
  @param[out] *Result             The verification result returned by PSP. 0: PASS, otherwise FAIL.
  @retval EFI_STATUS              0: Success, NonZero Error
**/
// EFI_STATUS
// PspMboxVerifyOsLoader (
// IN UINT64    StartAddr,
// IN UINT64    Length,
// OUT UINT64   *Result
// )
// {
// return PspMboxImageVerify(STAGE_OS_LOADER, StartAddr, Length, Result);
// }

/**
  BIOS requests PSP to verify OS loaded in memory. This command is normally used for
  secure boot purpose as in application of PSP being TPM.

  @param[in]  StartAddr           Start address (physical) of the image currently loaded in memory.
  @param[in]  Length              The length of the image in memory
  @param[out] *Result             The verification result returned by PSP. 0: PASS, otherwise FAIL.
  @retval EFI_STATUS              0: Success, NonZero Error
**/
// EFI_STATUS
// PspMboxVerifyOs (
// IN UINT64    StartAddr,
// IN UINT64    Length,
// OUT UINT64   *Result
// )
// {
// return PspMboxImageVerify(STAGE_OS, StartAddr, Length, Result);
// }
