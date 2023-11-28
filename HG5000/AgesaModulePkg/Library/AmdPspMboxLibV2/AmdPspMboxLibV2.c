/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP Mailbox related functions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 312135 $   @e \$Date: 2015-02-03 03:16:17 +0800 (Tue, 03 Feb 2015) $
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
 *
 ***************************************************************************/

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include "AGESA.h"
#include <Library/AmdBaseLib.h>
#include <Library/BaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Library/AmdPspMboxLibV2.h>
#include <Library/TimerLib.h>
#include <Filecode.h>

#define FILECODE LIBRARY_AMDPSPMBOXLIBV2_AMDPSPMBOXLIBV2_FILECODE

#define C2P_REG_OFFSET     0x10500ul              ///< BIOS to PSP mailbox Register SMN offset
#define CMDRESP_CH2_OFFSET 0x80
#define BUFFER_CH2_OFFSET 0xE0
#define CMDRESP_CH2_ADDR(MmioBase) ((UINT32)(MmioBase) + C2P_REG_OFFSET + CMDRESP_CH2_OFFSET)
#define BUFFER_CH2_ADDR(MmioBase) ((UINT32)(MmioBase) + C2P_REG_OFFSET + BUFFER_CH2_OFFSET)

#define BUF_SIZE_MAX (4096 - sizeof(UINT32))
#pragma pack(push, 1)
// Command Buffer format expected:
typedef struct {
   UINT32 BufSize;
   UINT32 CmdSize;
   UINT8 CmdBuf[BUF_SIZE_MAX];
} CMDRESP_BUF;
#pragma pack(pop)


#define HYGON_PSP_TPM2_CMD 0x100
#define RESP_SIZE_OFFSET 0x2

/**
  BIOS sends command to PSP via mailbox mapping from C2PMSG_32

  @param[in]  MboxBuffer        PSP mbox buffer
  @param[in]  Cmd               Psp Mailbox Command

  @retval BOOLEAN                0: Success, 1 Error

**/
BOOLEAN
SendPspCommandCh2 (
  IN       VOID       *MboxBuffer,
  IN       UINT32   Cmd
  )
{
  BOOLEAN    Status;
  UINT32   PspMmioBase;
  PSP_MBOX_CH2_CMD PspCmd;
  PSP_MBOX_CH2_CMD *PspCmdReg;
  volatile UINT64  *PspBufReg;
  Status = TRUE;

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.SendC2PCMD_Ch2 [0x%x] Buffer:0x%x\n", Cmd, MboxBuffer);
  // Get PspMbox location. fail if not found
  if (GetPspMmioBase (&PspMmioBase) == FALSE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("GetPspMmioBase Error\n");
    return FALSE;
  }
  PspCmdReg = (PSP_MBOX_CH2_CMD *)(UINTN)CMDRESP_CH2_ADDR(PspMmioBase);
  PspBufReg = (volatile UINT64 *)(UINTN)BUFFER_CH2_ADDR(PspMmioBase);

  // Wait till the previous command is processed and thus mbox is ready for new command.
  while ((!PspCmdReg->Field.Response) && (PspCmdReg->Field.CommandId)) {
    ;
  }
  //Set the mbox
  //Do it on local variable 1st to avoid pontencial racing issue
  PspCmd.Value = 0;
  PspCmd.Field.CommandId = Cmd;
  *PspBufReg = (UINT64) (UINTN) MboxBuffer;
  // Now send the command, PSP will receive it as an interrupt;
  PspCmdReg->Value = PspCmd.Value;

  /// Wait for PSP to be done or reflect error @todo add timeout
  while (!PspCmdReg->Field.Response) {
    ;
  }
  
  if (PspCmdReg->Field.Status != 0) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Error: Psp.SendC2PCMD_Ch2 return status 0x%x\n", PspCmdReg->Field.Status);
    Status = FALSE;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("\nPsp.SendC2PCMD_Ch2 Exit\n");
  return Status;
}

EFI_STATUS
fTpmExecuteCommand (
    IN       VOID                 *CommandBuffer,
    IN       UINT32               CommandSize,
    IN OUT   VOID                 *ResponseBuffer,
    IN OUT   UINT32               *ResponseSize
)
{
    EFI_STATUS   Status = EFI_SUCCESS;
    UINT32 RespSize;
    CMDRESP_BUF CmdRespBuff;
    
    if (CommandSize > BUF_SIZE_MAX) {
        IDS_HDT_CONSOLE_PSP_TRACE("Unsupported CommandSize 0x%x\n", CommandSize);
        return EFI_UNSUPPORTED;
    }

    CmdRespBuff.BufSize = SwapBytes32(BUF_SIZE_MAX);
    // Copy the command into the commandresp buffer
    CopyMem(&CmdRespBuff.CmdBuf[0], CommandBuffer, CommandSize);
    CmdRespBuff.CmdSize = SwapBytes32(CommandSize);

    if (SendPspCommandCh2(&CmdRespBuff, HYGON_PSP_TPM2_CMD) != TRUE) {
        IDS_HDT_CONSOLE_PSP_TRACE("Send command failed in iTpmExecuteCommand\n");
        Status = EFI_DEVICE_ERROR;
    }

    // Copy the response to Response buffer
    RespSize = *(UINT32 *)&CmdRespBuff.CmdBuf[RESP_SIZE_OFFSET];
    if (RespSize != CmdRespBuff.CmdSize) {
        IDS_HDT_CONSOLE_PSP_TRACE("Warning: response buffer size mismatch 0x%x, 0x%x\n", CmdRespBuff.CmdSize, RespSize);
    }
    RespSize = SwapBytes32(RespSize);
    if (RespSize > *ResponseSize) {
        IDS_HDT_CONSOLE_PSP_TRACE("ResponseBuffer too small 0x%x, required: 0x%x\n", *ResponseSize, RespSize);
        return EFI_UNSUPPORTED;
    }
    CopyMem(ResponseBuffer, &CmdRespBuff.CmdBuf[0], RespSize);
    *ResponseSize = RespSize;

    return Status;
}

/**
  BIOS sends command to PSP

  @param[in]  MboxBuffer        PSP mbox buffer
  @param[in]  Cmd               Psp Mailbox Command

  @retval BOOLEAN                0: Success, 1 Error

**/
BOOLEAN
SendPspCommand (
  IN       VOID       *MboxBuffer,
  IN       MBOX_COMMAND       Cmd
  )
{
  BOOLEAN    Status;
  PSP_MBOX_V2 *PspMbox;
  PSP_MBOX_V2_CMD PspCmd;
  MBOX_BUFFER_HEADER *BufferHdrPtr;
  UINT32     i;
  Status = TRUE;

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.SendC2PCMD [0x%x] Buffer:0x%x\n", Cmd, MboxBuffer);
  BufferHdrPtr = MboxBuffer;
  for (i = 0; i < BufferHdrPtr->TotalSize; i++) {
    IDS_HDT_CONSOLE_PSP_TRACE ("%02x ", * (((UINT8 *) BufferHdrPtr) + i) );
    if ((i != 0) && ((i + 1) % 16 == 0)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("\n");
    }
  }
  // Get PspMbox location. fail if not found
  if (GetPspMboxLocation (&PspMbox) == FALSE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("GetPspMboxLocation Error\n");
    ASSERT (FALSE); // Assertion in the debug build
    return (FALSE);
  }
  //Skip Sending the C2P mailbox when recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, Skip the CMD\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return (FALSE);
  }
  AGESA_TESTPOINT (TpPspC2PmboxBeforeSendCmdBase + Cmd, NULL);

  // Wait till mailbox is ready and PSP finish processing command
  while ( (!PspMbox->Cmd.Field.Ready) ||  (PspMbox->Cmd.Field.CommandId)) {
    ;
  }
  //Clear the ready bit before send the command
  //Do it on local variable 1st to avoid pontencial racing issue
  PspCmd.Value = 0;
  PspCmd.Field.Ready = 0;
  PspCmd.Field.CommandId = (UINT8) Cmd;
  PspMbox->Buffer = (UINT64) (UINTN) MboxBuffer;
  // Now send the command
  PspMbox->Cmd.Value = PspCmd.Value;

  AGESA_TESTPOINT (TpPspC2PmboxWaitCmdBase + Cmd, NULL);

  /// Wait for PSP to be done or reflect error @todo add timeout
  while (PspMbox->Cmd.Field.CommandId) {
    ;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("\nBuffer dump after CMD finished:\n");
  BufferHdrPtr = MboxBuffer;
  for (i = 0; i < BufferHdrPtr->TotalSize; i++) {
    IDS_HDT_CONSOLE_PSP_TRACE ("%02x ", * (((UINT8 *) BufferHdrPtr) + i) );
    if ((i != 0) && ((i + 1) % 16 == 0)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("\n");
    }
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("\nPsp.SendC2PCMD Exit\n");
  return Status;
}

/**
  BIOS sends the SMM information to PSP. SMM information is used by PSP to pass data back to BIOS

  @param[in]  SmmInfoReq          Point to the structure containing SMMInfo command required data

  @retval EFI_STATUS              0: Success, NonZero Error

**/
EFI_STATUS
PspMboxBiosCmdSmmInfo (
  IN SMM_REQ_BUFFER      *SmmInfoReq
  )
{
  MBOX_SMM_BUFFER    *Buffer;
  UNALIGNED_MBOX_BUFFER   UBuff;    // Unaligned buffer
  BOOLEAN CmdSts;

  CmdSts = FALSE;
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

  Buffer = BALIGN32 (&UBuff);
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.SmmInfo\n");

  Buffer->Header.TotalSize             = sizeof (MBOX_SMM_BUFFER);

  CopyMem (&Buffer->Req, SmmInfoReq, sizeof (SMM_REQ_BUFFER));
  IDS_HDT_CONSOLE_PSP_TRACE ("SMMBase %x SMMMask %x\n", SmmInfoReq->SMMBase, SmmInfoReq->SMMMask);
  IDS_HDT_CONSOLE_PSP_TRACE ("PSPSmmDataRegion %x PspSmmDataLength %x\n", SmmInfoReq->PSPSmmDataRegion, SmmInfoReq->PspSmmDataLength);
  IDS_HDT_CONSOLE_PSP_TRACE ("SmmTrigInfo::Addr:0x%x AddrType:0x%x Width:0x%x AndMask:0x%x OrMask:0x%x\n",
              SmmInfoReq->SmmTrigInfo.Address,
              SmmInfoReq->SmmTrigInfo.AddressType,
              SmmInfoReq->SmmTrigInfo.ValueWidth,
              SmmInfoReq->SmmTrigInfo.ValueAndMask,
              SmmInfoReq->SmmTrigInfo.ValueOrMask);

  IDS_HDT_CONSOLE_PSP_TRACE ("SmiEnb.Address:0x%x AddrType:0x%x Width:0x%x Mask:0x%x ExpectValue:0x%x\n",
  SmmInfoReq->SmmRegInfo.SmiEnb.Address,
  SmmInfoReq->SmmRegInfo.SmiEnb.AddressType,
  SmmInfoReq->SmmRegInfo.SmiEnb.ValueWidth,
  SmmInfoReq->SmmRegInfo.SmiEnb.RegBitMask,
  SmmInfoReq->SmmRegInfo.SmiEnb.ExpectValue);


  IDS_HDT_CONSOLE_PSP_TRACE ("Eos.Address:0x%x AddrType:0x%x Width:0x%x Mask:0x%x ExpectValue:0x%x\n",
  SmmInfoReq->SmmRegInfo.Eos.Address,
  SmmInfoReq->SmmRegInfo.Eos.AddressType,
  SmmInfoReq->SmmRegInfo.Eos.ValueWidth,
  SmmInfoReq->SmmRegInfo.Eos.RegBitMask,
  SmmInfoReq->SmmRegInfo.Eos.ExpectValue);


  IDS_HDT_CONSOLE_PSP_TRACE ("FakeSmiEn.Address:0x%x AddrType:0x%x Width:0x%x Mask:0x%x ExpectValue:0x%x\n",
  SmmInfoReq->SmmRegInfo.FakeSmiEn.Address,
  SmmInfoReq->SmmRegInfo.FakeSmiEn.AddressType,
  SmmInfoReq->SmmRegInfo.FakeSmiEn.ValueWidth,
  SmmInfoReq->SmmRegInfo.FakeSmiEn.RegBitMask,
  SmmInfoReq->SmmRegInfo.FakeSmiEn.ExpectValue);

  IDS_HDT_CONSOLE_PSP_TRACE ("PspMboxSmmBufferAddress:0x%x PspMboxSmmFlagAddress:0x%x\n",
  SmmInfoReq->PspMboxSmmBufferAddress,
  SmmInfoReq->PspMboxSmmFlagAddress);

  CmdSts = SendPspCommand (Buffer, MboxBiosCmdSmmInfo);
  if (CmdSts == FALSE) {
    return EFI_DEVICE_ERROR;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
  return (Buffer->Header.Status);

}


/**
  BIOS sends the Sx information to PSP. This mailbox command is send just prior of entering Sx state

  @param[in]  SleepType           SleepType the system is transition to.
                                    3 : S3, 4:S4, 5:S5

  @retval EFI_STATUS              0: Success, NonZero Error
**/
EFI_STATUS
PspMboxBiosCmdSxInfo (
  IN UINT8 SleepType,
  IN UINT8     *SmmBuffer,
  IN BOOLEAN   *SmmFlag
  )
{
  EFI_STATUS        Status;
  MBOX_SX_BUFFER    *Buffer;

  Status = EFI_INVALID_PARAMETER;
  Buffer = NULL;
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.SxInfo\n");
  if ((SmmBuffer != NULL) && (SmmFlag != NULL)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("SmmBuffer: %x\n",  SmmBuffer);
    IDS_HDT_CONSOLE_PSP_TRACE ("SmmFlag: %x\n",  SmmFlag);
    Buffer = (MBOX_SX_BUFFER*)SmmBuffer;
    ZeroMem (Buffer, sizeof (MBOX_SX_BUFFER));
    Buffer->Header.TotalSize      = sizeof (MBOX_SX_BUFFER);
    Buffer->Req.SleepType        = SleepType;
    *SmmFlag = TRUE;
    SendPspCommand (Buffer, MboxBiosCmdSxInfo);
    *SmmFlag = FALSE;
    IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
    Status = (Buffer->Header.Status) ? EFI_DEVICE_ERROR : EFI_SUCCESS;
  }
  return Status;

}

/**
  BIOS sends the Resume information to PSP. PSP will use this information to measure the resume code.
  During resume path PSP will use this information before handing off to BIOS to ensure the resume
  code is not tampered

  @param[in]  S3ResumeAddress       Location of BIOS reset code that will first fetch on resume

  @param[in]  S3ResumeCodeSize      Size of resume code that PSP need to measure to ensure authenticity

  @retval EFI_STATUS              0: Success, NonZero Error
**/
EFI_STATUS
PspMboxBiosCmdResumeInfo (
  IN UINT64 S3ResumeAddress,
  IN UINT64 S3ResumeCodeSize
  )
{
///@todo Reopen this command
  return EFI_SUCCESS;
//  MBOX_RSM_BUFFER    *Buffer;
//  UNALIGNED_MBOX_BUFFER   UBuff;    // Unaligned buffer
//
//  Buffer = BALIGN32 (&UBuff);
//  PSP_DEBUG ("Psp.C2PMbox.ResumeInfo\n");
//
//  Buffer->Header.TotalSize      = sizeof (MBOX_RSM_BUFFER);
//
//  Buffer->Req.ResumeVecorAddress = S3ResumeAddress;
//  Buffer->Req.ResumeVecorLength  = S3ResumeCodeSize;
//
//  CmdSts = SendPspCommand (Buffer, MboxBiosCmdRsmInfo);
  // if (CmdSts == FALSE) {
  //   return EFI_DEVICE_ERROR;
  // }
//  PSP_DEBUG ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
//  return (Buffer->Header.Status);
}


/**
  BIOS sends this command to inform PSP that BIOS is handing off to OS/OROM. Any mailbox command
  after this message need to come from SMM space (the SMM info is provided earlier via mailbox)
  i.e. PSP should check the parameter address to ensure it falls in SMM region

  @retval EFI_STATUS              0: Success, NonZero Error
**/
EFI_STATUS
PspMboxBiosCmdExitBootServices ()
{
  MBOX_DEFAULT_BUFFER     *Buffer;
  UNALIGNED_MBOX_BUFFER   UBuff;    // Unaligned buffer
  BOOLEAN CmdSts;

  CmdSts = FALSE;
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

  Buffer = BALIGN32 (&UBuff);
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.ExitBootServices\n");

  Buffer->Header.TotalSize      = sizeof (MBOX_DEFAULT_BUFFER);

  CmdSts = SendPspCommand (Buffer, MboxBiosCmdBootDone);
  if (CmdSts == FALSE) {
    return EFI_DEVICE_ERROR;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
  return (Buffer->Header.Status);
}

/**
  BIOS requests the capabilities from the PSP

  @param[in]  Capabilities      PSP Writes capabilities into this field when it returns.

  @retval EFI_STATUS                0: Success, NonZero Error

**/
EFI_STATUS
PspMboxBiosQueryCaps (
  IN OUT   UINT32 *Capabilities
  )
{
  MBOX_CAPS_BUFFER   *Buffer;
  UNALIGNED_MBOX_BUFFER   UBuff;    // Unaligned buffer
  BOOLEAN CmdSts;

  CmdSts = FALSE;
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

  Buffer = BALIGN32 (&UBuff);
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.PspMboxBiosQueryCaps\n");

  if (Capabilities == NULL) {
    return (EFI_NOT_FOUND);
  }

  Buffer->Header.TotalSize      = sizeof (MBOX_CAPS_BUFFER);

  Buffer->Req.Capabilities      = 1; // 1 to identify the command is sending from V9

  CmdSts = SendPspCommand (Buffer, MboxBiosCmdPspQuery);
  if (CmdSts == FALSE) {
    return EFI_DEVICE_ERROR;
  }
  *Capabilities = Buffer->Req.Capabilities;
  IDS_HDT_CONSOLE_PSP_TRACE ("Caps return %x\n", Buffer->Req.Capabilities);
  IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
  return (Buffer->Header.Status);
}


/**
  BIOS sends AP CS BASE

  @param[in]  ApCsBase       Address to update the code segment base on APs

  @retval EFI_STATUS                0: Success, NonZero Error

**/
EFI_STATUS
PspMboxBiosSendApCsBase (
  IN       UINT32 ApCsBase
  )
{
  MBOX_AP_CS_BASE_BUFFER  *Buffer;
  UNALIGNED_MBOX_BUFFER    UBuff;    // Unaligned buffer
  BOOLEAN CmdSts;

  CmdSts = FALSE;
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

  Buffer = BALIGN32 (&UBuff);
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.PspMboxApCsBase\n");

  Buffer->Header.TotalSize      = sizeof (MBOX_AP_CS_BASE_BUFFER);

  Buffer->Req.Value = ApCsBase;

  CmdSts = SendPspCommand (Buffer, MboxBiosCmdSetApCsBase);
  if (CmdSts == FALSE) {
    return EFI_DEVICE_ERROR;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
  return (Buffer->Header.Status);
}


/**
  BIOS get FW versions from the PSP

  @param[in]  FwVersions      PSP Writes multiple FW versions into this buffer when it returns.

  @retval EFI_STATUS                0: Success, NonZero Error

**/
EFI_STATUS
PspMboxBiosGetFwVersions (
  IN OUT   VERSION_BUFFER *FwVersions
  )
{
  MBOX_GET_VERSION_BUFFER *Buffer;
  UNALIGNED_MBOX_BUFFER   UBuff;    // Unaligned buffer
  BOOLEAN CmdSts;

  CmdSts = FALSE;
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

  Buffer = BALIGN32 (&UBuff);
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.PspMboxBiosGetFwVersions\n");

  if (FwVersions == NULL) {
    return (EFI_NOT_FOUND);
  }

  Buffer->Header.TotalSize      = sizeof (MBOX_GET_VERSION_BUFFER);

  CmdSts = SendPspCommand (Buffer, MboxBiosCmdGetVersion);
  if (CmdSts == FALSE) {
    return EFI_DEVICE_ERROR;
  }
  CopyMem (FwVersions, &Buffer->VersionBuffer, sizeof (VERSION_BUFFER));
  IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
  return (Buffer->Header.Status);
}

/**
  Check if Ftpm is ready

  @retval EFI_STATUS  0: Success, NonZero Error

**/
EFI_STATUS
CheckFtpmCapsV2 (
  IN OUT   UINT32 *Caps
  )
{
  UINT32 Capabilities;

  if (PcdGetBool(PcdHygonFtpmEnable) != 0) {
    Capabilities = PSP_CAP_TPM_SUPPORTED;
  } else {
    if (PspMboxBiosQueryCaps (&Capabilities)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("QueryCaps command return status fail\n");
      return (EFI_UNSUPPORTED);
    }
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("Mbox return Caps %x\n", Capabilities);

  *Caps = (Capabilities & (PSP_CAP_TPM_SUPPORTED + PSP_CAP_TPM_REQ_FACTORY_RESET));

  return (EFI_SUCCESS);
}

/**
  BIOS requests the HSTI state from the PSP

  @param[in]  HSTIState             PSP Writes HSTIState into this field when it returns.

  @retval EFI_STATUS                0: Success, NonZero Error

**/
EFI_STATUS
PspMboxBiosQueryHSTIState (
  IN OUT   UINT32 *HSTIState
  )
{
  MBOX_HSTI_STATE   *Buffer;
  UNALIGNED_MBOX_BUFFER   UBuff;    // Unaligned buffer
  BOOLEAN CmdSts;

  CmdSts = FALSE;
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

  Buffer = BALIGN32 (&UBuff);
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.PspMboxBiosQueryHSTIState\n");

  if (HSTIState == NULL) {
    return (EFI_NOT_FOUND);
  }

  Buffer->Header.TotalSize = sizeof (MBOX_HSTI_STATE);

  Buffer->HSTIState = 0; // Result of command will be placed here

  CmdSts = SendPspCommand (Buffer, MboxBiosCmdHSTIQuery);
  if (CmdSts == FALSE) {
    return EFI_DEVICE_ERROR;
  }
  *HSTIState = Buffer->HSTIState;
  IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x HSTIState:0x%x\n", Buffer->Header.Status, Buffer->HSTIState);
  return (Buffer->Header.Status);
}

/**
  BIOS sends the message to PSP for clear SmmLock bit in C6 private region

  @retval EFI_STATUS              0: Success, NonZero Error
**/
EFI_STATUS
PspMboxBiosClearSmmLock (
  IN UINT8     *SmmBuffer,
  IN BOOLEAN   *SmmFlag
  )
{
  EFI_STATUS              Status;
  MBOX_DEFAULT_BUFFER     *Buffer;

  Status = EFI_INVALID_PARAMETER;
  Buffer = NULL;
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.ClearSmmLock\n");
  if ((SmmBuffer != NULL) && (SmmFlag != NULL)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("SmmBuffer: %x\n",  SmmBuffer);
    IDS_HDT_CONSOLE_PSP_TRACE ("SmmFlag: %x\n",  SmmFlag);
    Buffer = (MBOX_DEFAULT_BUFFER *) SmmBuffer;
    ZeroMem (Buffer, sizeof (MBOX_DEFAULT_BUFFER));
    Buffer->Header.TotalSize      = sizeof (MBOX_DEFAULT_BUFFER);
    *SmmFlag = TRUE;
    SendPspCommand (Buffer, MboxBiosCmdClrSmmLock);
    *SmmFlag = FALSE;
    IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
    Status = (Buffer->Header.Status) ? EFI_DEVICE_ERROR : EFI_SUCCESS;
  }
  return Status;
}

/**
  BIOS sends the message to PSP o lock DF registers

  @retval EFI_STATUS              0: Success, NonZero Error
**/
EFI_STATUS
PspMboxBiosLockDFReg (
  )
{
  MBOX_DEFAULT_BUFFER     *Buffer;
  UNALIGNED_MBOX_BUFFER   UBuff;    // Unaligned buffer
  BOOLEAN CmdSts;

  CmdSts = FALSE;
  Buffer = BALIGN32 (&UBuff);
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.LockDFReg\n");

  Buffer->Header.TotalSize      = sizeof (MBOX_DEFAULT_BUFFER);

  CmdSts = SendPspCommand (Buffer, MboxBiosCmdLockDFReg);
  if (CmdSts == FALSE) {
    return EFI_DEVICE_ERROR;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
  return (Buffer->Header.Status);
}

/**
  BIOS sends TA command to PSP

  @retval EFI_STATUS              0: Success, NonZero Error
**/
TEE_STATUS
PspMboxBiosTa (
  UINT64              TaCmdBufferAddress,
  UINT64              TaCmdBufferSize
  )
{
  MBOX_TA     *Buffer;
  UNALIGNED_MBOX_BUFFER   UBuff;    // Unaligned buffer
  BOOLEAN CmdSts;

  CmdSts = FALSE;
  Buffer = BALIGN32 (&UBuff);
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.PspMboxBiosTa Buffer:%x Size:%x\n", TaCmdBufferAddress, TaCmdBufferSize);

  Buffer->Header.TotalSize      = sizeof (MBOX_TA);
  Buffer->Header.Status         = 0;
  Buffer->TaCmdBufferAddress = TaCmdBufferAddress;
  Buffer->TaCmdBufferSize    = TaCmdBufferSize;

  CmdSts = SendPspCommand (Buffer, MboxBiosCmdTa);
  if (CmdSts == FALSE) {
    return TEE_ERR_NWD_INVALID_SESSION;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
  return (Buffer->Header.Status);
}

/**
  BIOS will send this command to lock SPI, X86 need in SMM mode when send this command

  @retval EFI_STATUS              0: Success, NonZero Error
**/
EFI_STATUS
PspMboxBiosUnLockSpi (
  )
{
  MBOX_DEFAULT_BUFFER     *Buffer;
  UNALIGNED_MBOX_BUFFER   UBuff;    // Unaligned buffer
  BOOLEAN CmdSts;

  CmdSts = FALSE;
  Buffer = BALIGN32 (&UBuff);
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.LockSpi\n");

  Buffer->Header.TotalSize      = sizeof (MBOX_DEFAULT_BUFFER);

  CmdSts = SendPspCommand (Buffer, MboxBiosCmdUnLockSpi);
  if (CmdSts == FALSE) {
    return EFI_DEVICE_ERROR;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
  return (Buffer->Header.Status);
}

/**
  BIOS assign memory buffer to PSP, PSP decode ucode patch to here and return status to BIOS
  Input:    UcodePatchBuffer      Pointer to 4KB buffer
  @retval EFI_STATUS              0: Success, NonZero Error
**/
EFI_STATUS
PspMboxBiosGetUcodePatch (
  IN UINT64    *UcodePatchBuffer,
  IN UINT32    BufferSize
  )
{
  MBOX_UCODE_PATCH_BUFFER  *Buffer;
  UNALIGNED_MBOX_BUFFER     UBuff;    // Unaligned buffer
  BOOLEAN CmdSts;

  if ( UcodePatchBuffer == 0) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Ucode Patch Buffer invalid. \n");
      return EFI_INVALID_PARAMETER;
  }
  
  CmdSts = FALSE;
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));
  Buffer = BALIGN32 (&UBuff);
       
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.GetUcodePatch\n");
  IDS_HDT_CONSOLE_PSP_TRACE ("Ucode Patch Buffer Address : 0x%lx\n",  UcodePatchBuffer);

  Buffer->Header.TotalSize      = sizeof (MBOX_UCODE_PATCH_BUFFER);

  Buffer->UcodePatchBuffer.Address = (UINT64)UcodePatchBuffer;
  Buffer->UcodePatchBuffer.Size = BufferSize;

  CmdSts = SendPspCommand (Buffer, MboxBiosCmdGetUcodePatch);
  if (CmdSts == FALSE) {
    return EFI_DEVICE_ERROR;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
  return (Buffer->Header.Status);
}
/**
  get or set current available CCP VQ count
  Input:Cmd  0=get current available CCP VQ count, 1=set current available CCP VQ count
  Input OUT:CmdBuffer if Cmd=0 Cmd don't supported
                      if Cmd=1 set available CCP VQ count to CmdBuffer (CMD unrealized)
  @retval EFI_STATUS              0: Success, NonZero Error
**/
EFI_STATUS
PspMboxOperationVQ (
  IN OUT UINT8* VqCount
  )
{
    MBOX_EXPORTPSPCCPVQ_BUFFER* Buffer;
    UNALIGNED_MBOX_BUFFER   UBuff;    // Unaligned buffer
    BOOLEAN                 CmdSts;
    UINT32                  Retry=0;
    
    IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.PspMboxOperationVQ entry\n");
    CmdSts = FALSE;
    Buffer = BALIGN32 (&UBuff);
    ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));
    
    Buffer->Header.TotalSize = sizeof(MBOX_EXPORTPSPCCPVQ_BUFFER);
    Buffer->Header.Status = VQ_OPERATION_RET_WAITING;
    Buffer->ExportPspccpvqBuffer.ExportVqNr=*VqCount;
    
    IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.PspMboxOperationVQ VqCount=%x\n",Buffer->ExportPspccpvqBuffer.ExportVqNr);
    IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.PspMboxOperationVQ RetStatus=%x\n",Buffer->Header.Status);
    CmdSts = SendPspCommand (Buffer, MboxBiosCmdCcpVqOperation);
    if (CmdSts == FALSE) {
        IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.PspMboxOperationVQ Message fail\n");
       return EFI_DEVICE_ERROR;
     }
    
    do{
        MicroSecondDelay(100);
        IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.PspMboxOperationVQ waiting psp return\n");
        Retry++;
    }while((Buffer->Header.Status==VQ_OPERATION_RET_WAITING)&&(Retry<VQ_OPERATION_MSG_RETRY_TIME));
    IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.PspMboxOperationVQ RetStatus=%x\n",Buffer->Header.Status);
    if(Buffer->Header.Status!=VQ_OPERATION_RET_SUCCESS){
        IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.PspMboxOperationVQ Cmd fail\n");
        return EFI_DEVICE_ERROR;
    }
    IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.PspMboxOperationVQ Exit\n");
    return EFI_SUCCESS;
}
