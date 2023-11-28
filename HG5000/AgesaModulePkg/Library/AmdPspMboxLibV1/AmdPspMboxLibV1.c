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
 * @e \$Revision: 319679 $   @e \$Date: 2015-06-01 14:52:21 +0800 (Mon, 01 Jun 2015) $
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
#include <Library/BaseMemoryLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdPspMboxLibV1.h>
#include <Filecode.h>

#define FILECODE LIBRARY_AMDPSPMBOXLIBV1_AMDPSPMBOXLIBV1_FILECODE

/**
  Return the PspMbox MMIO location


  @retval EFI_STATUS  0: Success, NonZero Error

**/
EFI_STATUS
GetPspMboxLocation (
  IN OUT   PSP_MBOX **PspMbox
  )
{
  UINT32 PspMmio;

  if (GetPspBar3Addr (&PspMmio) == FALSE) {
    return (EFI_UNSUPPORTED);
  }

  *PspMbox = (PSP_MBOX *)( (UINTN)PspMmio + PSP_MAILBOX_BASE);   // PSPMbox base is at offset CP2MSG_28 ie. offset 28*4 = 0x70

  return (EFI_SUCCESS);
}


/**
  BIOS sends command to PSP

  @param[in]  MboxBuffer        PSP mbox buffer
  @param[in]  Cmd               Psp Mailbox Command

  @retval BOOLEAN                0: Success, 1 Error

**/
/// @todo Change function to some status type and appropriate status
/// Also add code in caller to handle this change
BOOLEAN
SendPspCommand (
  IN       VOID       *MboxBuffer,
  IN       MBOX_COMMAND       Cmd
  )
{
  UINT32     Command;
  BOOLEAN    Status;
  PSP_MBOX   *PspMbox;
  Status = TRUE;

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.SendC2PCMD [0x%x]\n", Cmd);

  // Get PspMbox location. fail if not found
  if (EFI_ERROR (GetPspMboxLocation (&PspMbox))) {
    IDS_HDT_CONSOLE_PSP_TRACE ("GetPspMboxLocation Error\n");
    ASSERT (FALSE); // Assertion in the debug build
    return (FALSE);
  }

  // The Command register may be disable by PSP driver when entering D3.
  // Save Command register
  Command = PspLibPciReadPspConfig (PSP_PCI_CMD_REG);
   //Enable BusMaster & MemAccess
  PspLibPciWritePspConfig (PSP_PCI_CMD_REG, Command | 0x6);

  if (PspMbox->MboxSts.Halt) {
    IDS_HDT_CONSOLE_PSP_TRACE ("MboxSts Halt\n");
    ASSERT (FALSE); // Assertion in the debug build
    //Restore Command register
    PspLibPciWritePspConfig (PSP_PCI_CMD_REG, Command);
    return (FALSE);
  }

  if (PspMbox->MboxSts.Recovery) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery Flag detected, ignore the command\n");
    //Restore Command register
    PspLibPciWritePspConfig (PSP_PCI_CMD_REG, Command);
    return (FALSE);
  }

  AGESA_TESTPOINT (TpPspC2PmboxBeforeSendCmdBase + Cmd, NULL);

  // Wait till mailbox is initialized or done processing command
  /// Wait for PSP to be ready. @todo add timeout
  while ( (!PspMbox->MboxSts.MboxInitialized) ||  (PspMbox->MboxCmd)) {
    ;
  }

  // Now send the command
  PspMbox->Buffer = (MBOX_BUFFER *)MboxBuffer;
  PspMbox->MboxCmd = Cmd;

  AGESA_TESTPOINT (TpPspC2PmboxWaitCmdBase + Cmd, NULL);

  /// Wait for PSP to be done or reflect error @todo add timeout
  while (PspMbox->MboxCmd) {
    ;
  }

  // error vs. terminated
  if (PspMbox->MboxSts.Error || PspMbox->MboxSts.Terminated) {
    IDS_HDT_CONSOLE_PSP_TRACE ("MboxSts Error\n");
    Status = FALSE;
  }
  //Restore Command register
  PspLibPciWritePspConfig (PSP_PCI_CMD_REG, Command);
  return Status;

}


/**
  BIOS sends the DRAM ready message to PSP.

  @retval EFI_STATUS              0: Success, NonZero Error
**/
EFI_STATUS
PspMboxBiosCmdDramInfo (
  )
{
  UNALIGNED_MBOX_BUFFER   UBuff;    // Unaligned buffer
  MBOX_DEFAULT_BUFFER     *Buffer;  // Pointer to aligned buffer in stack
  BOOLEAN CmdSts;

  CmdSts = FALSE;
  Buffer = BALIGN32 (&UBuff);
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.DramInfo\n");

  Buffer->Header.TotalSize      = sizeof (MBOX_DEFAULT_BUFFER);

  CmdSts = SendPspCommand (Buffer, MboxBiosCmdDramInfo);
  if (CmdSts == FALSE) {
    return EFI_DEVICE_ERROR;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
  return (Buffer->Header.Status);
}

/**
 *
 *
 * Bios send this command to inform PSP to save the data needed to restore SRAM during resume from S3
 *
 * @param[in]  S3RestoreBufferBase           PSP reserve memory base near TOM
 * @param[in]  S3RestoreBufferSize           Size of PSP memory
 * @param[in,out]  Hmac                      Hmac value for S3RestoreBufferBase with S3RestoreBufferSize
 *
 * @retval EFI_STATUS              0: Success, NonZero Error
 *
 */
EFI_STATUS
PspMboxBiosCmdS3Info (
  IN       UINT64 S3RestoreBufferBase,
  IN       UINT64 S3RestoreBufferSize,
  IN OUT   UINT8 *Hmac
   )
{
  UNALIGNED_MBOX_BUFFER   UBuff;    // Unaligned buffer
  MBOX_S3DATA_BUFFER      *Buffer;
  BOOLEAN CmdSts;

  CmdSts = FALSE;
  Buffer = BALIGN32 (&UBuff);
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.S3Info\n");

  Buffer->Header.TotalSize      = sizeof (MBOX_S3DATA_BUFFER);

  Buffer->Req.S3RestoreBufferBase  = S3RestoreBufferBase;
  Buffer->Req.S3RestoreBufferSize  = S3RestoreBufferSize;
  ZeroMem (&Buffer->Req.Hmac, S3_RESTORE_BUFFER_HMAC_SIZE);
  CmdSts = SendPspCommand (Buffer, MboxBiosS3DataInfo);
  if (CmdSts == FALSE) {
    return EFI_DEVICE_ERROR;
  }
  CopyMem (Hmac, &Buffer->Req.Hmac, S3_RESTORE_BUFFER_HMAC_SIZE);
  IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
  return (Buffer->Header.Status);
}

/**
  BIOS sends the SMM information to PSP. SMM information is used by PSP to pass data back to BIOS

  @param[in]  SMMBase             Location of SMM base (TSEG)

  @param[in]  SMMLength           Length/Size of SMM space (TSEG)

  @param[in]  PSPSmmDataRegion    Location of reserved PSPSmmDataRegion within the SMM space.
                                  PSP will use this region to communicate with BIOS (i.e pass
                                  data/command at this region for BIOS SMM to process)

  @param[in]  PspSmmDataLength    Length of reserve PSPSmmDataRegion in SMM space for PSP communication

  @param[in]  SmmTrigInfo  IO port address to generate SoftSmi

  @retval EFI_STATUS              0: Success, NonZero Error

**/
EFI_STATUS
PspMboxBiosCmdSmmInfo (
  IN UINT64            SMMBase,
  IN UINT64            SMMLength,
  IN UINT64            PSPSmmDataRegion,
  IN UINT64            PspSmmDataLength,
  IN SMM_TRIGGER_INFO  *SmmTrigInfo,
  IN UINT64            PspMboxSmmBufferAddress,
  IN UINT64            PspMboxSmmFlagAddress
  )
{
  MBOX_SMM_BUFFER    *Buffer;
  UNALIGNED_MBOX_BUFFER   UBuff;    // Unaligned buffer
  BOOLEAN CmdSts;

  CmdSts = FALSE;
  Buffer = BALIGN32 (&UBuff);
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.SmmInfo\n");

  Buffer->Header.TotalSize             = sizeof (MBOX_SMM_BUFFER);

  Buffer->Req.SMMBase                  = SMMBase;
  Buffer->Req.SMMLength                = SMMLength;
  Buffer->Req.PSPSmmDataRegion         = PSPSmmDataRegion;
  Buffer->Req.PspSmmDataLength         = PspSmmDataLength;

  Buffer->Req.SmmTrigInfo.AddressType  = SmmTrigInfo->AddressType;
  Buffer->Req.SmmTrigInfo.Address      = SmmTrigInfo->Address;
  Buffer->Req.SmmTrigInfo.ValueWidth   = SmmTrigInfo->ValueWidth;
  Buffer->Req.SmmTrigInfo.ValueAndMask = SmmTrigInfo->ValueAndMask;
  Buffer->Req.SmmTrigInfo.ValueOrMask  = SmmTrigInfo->ValueOrMask;
  Buffer->Req.PspMboxSmmBufferAddress  = PspMboxSmmBufferAddress;
  Buffer->Req.PspMboxSmmFlagAddress    = PspMboxSmmFlagAddress;

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
  BOOLEAN CmdSts;

  CmdSts = FALSE;
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
    CmdSts = SendPspCommand (Buffer, MboxBiosCmdSxInfo);
    *SmmFlag = FALSE;
    if (CmdSts == FALSE) {
      return EFI_DEVICE_ERROR;
    }
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
  MBOX_RSM_BUFFER    *Buffer;
  UNALIGNED_MBOX_BUFFER   UBuff;    // Unaligned buffer
  BOOLEAN CmdSts;

  CmdSts = FALSE;
  Buffer = BALIGN32 (&UBuff);
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.ResumeInfo\n");

  Buffer->Header.TotalSize      = sizeof (MBOX_RSM_BUFFER);

  Buffer->Req.ResumeVecorAddress = S3ResumeAddress;
  Buffer->Req.ResumeVecorLength  = S3ResumeCodeSize;

  CmdSts = SendPspCommand (Buffer, MboxBiosCmdRsmInfo);
  if (CmdSts == FALSE) {
    return EFI_DEVICE_ERROR;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
  return (Buffer->Header.Status);
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
  Buffer = BALIGN32 (&UBuff);
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

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
  Buffer = BALIGN32 (&UBuff);
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

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
  IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
  return (Buffer->Header.Status);
}


/**
  BIOS sends this dummy command to inform PSP that its done processing the SMM request
  The purpose of this command is to generate interrupt to PSP

  @retval EFI_STATUS              0: Success, NonZero Error
**/
EFI_STATUS
PspMboxBiosCmdNop ()
{
  MBOX_DEFAULT_BUFFER     *Buffer;
  UNALIGNED_MBOX_BUFFER   UBuff;    // Unaligned buffer
  BOOLEAN CmdSts;

  CmdSts = FALSE;
  Buffer = BALIGN32 (&UBuff);
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.Nop\n");

  Buffer->Header.TotalSize      = sizeof (MBOX_DEFAULT_BUFFER);

  CmdSts = SendPspCommand (Buffer, MboxBiosCmdNop);
  if (CmdSts == FALSE) {
    return EFI_DEVICE_ERROR;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
  return (Buffer->Header.Status);

}
/**
  BIOS sends the clear S3 Status message to PSP.

  @retval EFI_STATUS              0: Success, NonZero Error
**/
EFI_STATUS
PspMboxBiosClearS3Status (
  IN UINT8     *SmmBuffer,
  IN BOOLEAN   *SmmFlag
  )
{
  EFI_STATUS              Status;
  MBOX_DEFAULT_BUFFER     *Buffer;
  BOOLEAN CmdSts;

  CmdSts = FALSE;
  Status = EFI_INVALID_PARAMETER;
  Buffer = NULL;
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.ClearS3Sts\n");
  if ((SmmBuffer != NULL) && (SmmFlag != NULL)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("SmmBuffer: %x\n",  SmmBuffer);
    IDS_HDT_CONSOLE_PSP_TRACE ("SmmFlag: %x\n",  SmmFlag);
    Buffer = (MBOX_DEFAULT_BUFFER *) SmmBuffer;
    ZeroMem (Buffer, sizeof (MBOX_DEFAULT_BUFFER));
    Buffer->Header.TotalSize      = sizeof (MBOX_DEFAULT_BUFFER);
    *SmmFlag = TRUE;
    CmdSts = SendPspCommand (Buffer, MboxBiosCmdClearS3Sts);
    *SmmFlag = FALSE;
    if (CmdSts == FALSE) {
      return EFI_DEVICE_ERROR;
    }
    IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
    Status = (Buffer->Header.Status) ? EFI_DEVICE_ERROR : EFI_SUCCESS;
  }
  return Status;
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
  Buffer = BALIGN32 (&UBuff);
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

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
  )
{
  MBOX_DEFAULT_BUFFER     *Buffer;
  UNALIGNED_MBOX_BUFFER   UBuff;    // Unaligned buffer
  BOOLEAN CmdSts;

  CmdSts = FALSE;
  Buffer = BALIGN32 (&UBuff);
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.ClearSmmLock\n");

  Buffer->Header.TotalSize      = sizeof (MBOX_DEFAULT_BUFFER);

  CmdSts = SendPspCommand (Buffer, MboxBiosCmdClrSmmLock);
  if (CmdSts == FALSE) {
    return EFI_DEVICE_ERROR;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("Mbox.Hdr.Sts:0x%x\n", Buffer->Header.Status);
  return (Buffer->Header.Status);
}

/**
  BIOS sends the message to PSP for PCIE device information

  @retval EFI_STATUS              0: Success, NonZero Error
**/
EFI_STATUS
PspMboxBiosPcieInfo (
  UINT32              DevId,
  PCI_ADDR            Address
  )
{
  MBOX_PCIE_INFO_BUFFER     *Buffer;
  UNALIGNED_MBOX_BUFFER   UBuff;    // Unaligned buffer
  BOOLEAN CmdSts;

  CmdSts = FALSE;
  Buffer = BALIGN32 (&UBuff);
  ZeroMem (&UBuff, sizeof (UNALIGNED_MBOX_BUFFER));

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.C2PMbox.PspMboxBiosPcieInfo DevId:%x Address:%x\n", DevId, Address.AddressValue);

  Buffer->Header.TotalSize      = sizeof (MBOX_PCIE_INFO_BUFFER);
  Buffer->Header.Status         = 0;
  Buffer->Info.DevId            = DevId;
  Buffer->Info.Address          = Address;

  CmdSts = SendPspCommand (Buffer, MboxBiosCmdPcieInfo);
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
  Check if Ftpm is ready

  @retval EFI_STATUS  0: Success, NonZero Error

**/
EFI_STATUS
CheckFtpmCapsV1 (
  IN OUT   UINT32 *Caps
  )
{
  UINT32 Capabilities;

  if (PspMboxBiosQueryCaps (&Capabilities)) {
    return (EFI_UNSUPPORTED);
  }

  *Caps = (Capabilities & (PSP_CAP_TPM_SUPPORTED + PSP_CAP_TPM_REQ_FACTORY_RESET));

  return EFI_SUCCESS;
}
