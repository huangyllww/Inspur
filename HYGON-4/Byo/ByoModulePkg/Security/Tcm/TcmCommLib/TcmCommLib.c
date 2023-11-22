/** @file

Copyright (c) 2006 - 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  TcmDxeLib.c

Abstract:
  Dxe part of TCM Module.

Revision History:

Bug 3341 - Add retry mechanism for more tcm command to improve stability.
TIME: 2012-02-03
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  1. Max retry count is 3.
  2. remove all ASSERT() in TcmDxe.c
$END------------------------------------------------------------

Bug 3282 - improve TCM action stability.
TIME: 2012-01-06
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  1. update command flow as porting guide request.
  2. use retry mechanism.
$END------------------------------------------------------------

Bug 3269 - Add TCM int1A function support.
TIME: 2011-12-30
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  Use Smi to handle legacy int 1A(0xBB) interrupt.
$END------------------------------------------------------------

Bug 3144 - Add Tcm Measurement Architecture.
TIME: 2011-11-24
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  1. PEI: Measure CRTM Version.
          Measure Main Bios.
  2. DXE: add 'TCPA' acpi table.
          add event log.
          Measure Handoff Tables.
          Measure All Boot Variables.
          Measure Action.
  Note: As software of SM3's hash has not been implemented, so hash
        function is invalid.
$END------------------------------------------------------------

Bug 3075 - Add TCM support.
TIME: 2011-11-14
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  1. Tcm module init version.
     Only support setup function.
$END------------------------------------------------------------

**/

#include <Uefi.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TcmCommLib.h>
#include <Library/SafeMemLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TMIoLib.h>

STATIC CONST UINTN gTcmIssueCmdMaxRetryCount = 3;
STATIC UINT8 TcmCommandBuf[TCM_CMDBUF_LENGTH];



/**
  Check whether the value of TCM chip register satisfies the input BIT setting.

  @param  Register     Address port of register to be checked.
  @param  BitSet       Check these data bits are set.
  @param  BitClear     Check these data bits are clear.
  @param  TimeOut      The max wait time (unit 30MicroSecond) when checking register.

  @retval EFI_SUCCESS  The register satisfies the check bit.
  @retval EFI_TIMEOUT  The register can't run into the expected status in time.
**/
EFI_STATUS
EFIAPI
TcmPcWaitRegisterBits (
  IN      volatile UINT8  *Register,
  IN               UINT8    BitSet,
  IN               UINT8    BitClear,
  IN               UINT32   TimeOut
  )
{
  UINT8   RegRead;
  UINT32  WaitTime;

  for (WaitTime = 0; WaitTime < TimeOut; WaitTime += 30){
    RegRead = TrustedModuleIoRead8((UINTN)Register);
    if ((RegRead & BitSet) == BitSet && (RegRead & BitClear) == 0){
      return EFI_SUCCESS;
    }
    MicroSecondDelay(30);
  }
  return EFI_TIMEOUT;
}





/**
  Get the control of TCM chip by sending requestUse command TCM_PC_ACC_RQUUSE
  to ACCESS Register in the time of default TCM_TIMEOUT_D.

  @param[in] TcmHandle          TCM Handle.

  @retval EFI_SUCCESS           Get the control of TCM chip.
  @retval EFI_INVALID_PARAMETER TcmRegs is NULL.
  @retval EFI_NOT_FOUND         TCM chip doesn't exit.
  @retval EFI_TIMEOUT           Can't get the TCM control in time.
**/
EFI_STATUS
TcmPcRequestUseTcm (
  EFI_TCM_HANDLE TcmHandle
  )
{
           EFI_STATUS        Status;
  volatile TCM_PC_REGISTERS  *TcmRegs;
           UINT8             Data8;

  Status = EFI_SUCCESS;

  TcmRegs = (volatile TCM_PC_REGISTERS*)TcmHandle;
  if (TcmRegs == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Data8 = TrustedModuleIoRead8 ((UINTN)&TcmRegs->Access);
  if(Data8==0xFF){
    return EFI_NOT_FOUND;
  }

  if((Data8 & TCM_PC_ACC_ACTIVE) == TCM_PC_ACC_ACTIVE){
    return EFI_SUCCESS;
  }

  TrustedModuleIoWrite8((UINTN)&TcmRegs->Access, TCM_PC_ACC_RQUUSE);
  Status = TcmPcWaitRegisterBits (
             &TcmRegs->Access,
             TCM_PC_ACC_ACTIVE,
             0,
             TCM_TIMEOUT_D
             );
  return Status;
}





/**
  Set TCM chip to ready state by sending ready command TCM_PC_STS_READY
  to Status Register in time.

  @param[in] TcmRegs            TCM MMIO Base Address Pointer.

  @retval EFI_SUCCESS           TCM chip enters into ready state.
  @retval EFI_INVALID_PARAMETER TcmRegs is NULL.
  @retval EFI_TIMEOUT           TCM chip can't be set to ready state in time.
**/
EFI_STATUS
TcmPcPrepareCommand (
  IN volatile TCM_PC_REGISTERS *TcmRegs
  )
{
  EFI_STATUS Status;

  if (TcmRegs == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TrustedModuleIoWrite8 ((UINTN)&TcmRegs->Status, TCM_PC_STS_READY);
  Status = TcmPcWaitRegisterBits (
             &TcmRegs->Status,
             TCM_PC_STS_READY,
             0,
             TCM_TIMEOUT_B
             );
  return Status;
}






/**
  Get BurstCount by reading burstCount field of TCM register
  in the time of default TCM_TIMEOUT_D.

  @param[in]  TcmRegs           TCM MMIO Base Address Pointer.
  @param[OUT] BurstCount        Pointer to a buffer to store the got BurstCount.

  @retval EFI_SUCCESS           Get BurstCount.
  @retval EFI_INVALID_PARAMETER TcmRegs is NULL or BurstCount is NULL.
  @retval EFI_TIMEOUT           BurstCount can't be got in time.
**/
EFI_STATUS
EFIAPI
TcmPcReadBurstCount(
  IN   volatile TCM_PC_REGISTERS *TcmRegs,
  OUT  UINT16                    *BurstCount
  )
{
  UINT32  WaitTime;
  UINT8   DataByte0;
  UINT8   DataByte1;

  if (BurstCount == NULL || TcmRegs == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  WaitTime = 0;
  do {
    DataByte0   = TrustedModuleIoRead8((UINTN)&TcmRegs->BurstCount);
    DataByte1   = TrustedModuleIoRead8((UINTN)&TcmRegs->BurstCount + 1);
    *BurstCount = (UINT16)((DataByte1 << 8) + DataByte0);
    if(*BurstCount != 0){
      return EFI_SUCCESS;
    }
    MicroSecondDelay(30);
    WaitTime += 30;
  } while (WaitTime < TCM_TIMEOUT_D);

  return EFI_TIMEOUT;
}






/**
  Send command to TCM for execution.

  @param[in] TcmRegs    TCM register space base address.
  @param[in] TcmBuffer  Buffer for TCM command data.
  @param[in] DataLength TCM command data length.

  @retval EFI_SUCCESS  Operation completed successfully.
  @retval EFI_TIMEOUT  The register can't run into the expected status in time.

**/
EFI_STATUS
TcmPcSend (
  IN     volatile TCM_PC_REGISTERS  *TcmRegs,
  IN     UINT8                      *TcmBuffer,
  IN     UINT32                     DataLength
  )
{
  UINT16                            BurstCount;
  UINT32                            Index;
  EFI_STATUS                        Status;

  Status = TcmPcPrepareCommand(TcmRegs);
  if(EFI_ERROR(Status)){
    return Status;
  }
  Index = 0;
  while (Index < DataLength) {
    Status = TcmPcReadBurstCount(TcmRegs, &BurstCount);
    if (EFI_ERROR (Status)) {
      return EFI_TIMEOUT;
    }
    for(; BurstCount > 0 && Index < DataLength; BurstCount--) {
      TrustedModuleIoWrite8((UINTN)&TcmRegs->DataFifo, TcmBuffer[Index]);
      TrustedModuleIoRead16((UINTN)&TcmRegs->Vid);
      Index++;
    }
    if(Index < DataLength){
      Status = TcmPcWaitRegisterBits(     // wait expect = 1
                 &TcmRegs->Status,
                 TCM_PC_STS_EXPECT,
                 0,
                 TCM_TIMEOUT_C
                 );
      if(EFI_ERROR(Status)){
        return EFI_TIMEOUT;
      }
    }
  }

  Status = TcmPcWaitRegisterBits(         // wait expect = 0
             &TcmRegs->Status,
             0,
             TCM_PC_STS_EXPECT,
             TCM_TIMEOUT_C
             );
  return Status;
}








/**
  Receive response data of last command from TCM.

  @param[in]  TcmRegs    TCM register space base address.
  @param[out] TcmBuffer  Buffer for response data.
  @param[out] RespSize   Response data length.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_TIMEOUT           The register can't run into the expected status in time.
  @retval EFI_DEVICE_ERROR      Unexpected device status.
  @retval EFI_BUFFER_TOO_SMALL  Response data is too long.

**/
EFI_STATUS
TcmPcReceive (
     IN   volatile TCM_PC_REGISTERS  *TcmRegs,
     OUT           UINT8             *TcmBuffer,
     OUT           UINT32            *RespSize
  )
{
  EFI_STATUS  Status;
  UINT16      BurstCount;
  UINT32      Index;
  UINT32      ResponseSize;
  UINT32      Data32;

// Wait for the command completion
  Status = TcmPcWaitRegisterBits (
             &TcmRegs->Status,
             TCM_PC_STS_DATA,
             0,
             TCM_TIMEOUT_B
             );
  if (EFI_ERROR (Status)) {
    return EFI_TIMEOUT;
  }
  //
  // Read the response data header and check it
  //
  Index = 0;
  BurstCount = 0;
  while (Index < sizeof (TCM_RSP_COMMAND_HDR)) {
    Status = TcmPcReadBurstCount(TcmRegs, &BurstCount);
    if (EFI_ERROR (Status)) {
      return EFI_TIMEOUT;
    }
    for(; BurstCount > 0 ; BurstCount--){
      Status = TcmPcWaitRegisterBits (
                 &TcmRegs->Status,
                 TCM_PC_STS_DATA,
                 0,
                 TCM_TIMEOUT_B
                 );
      if(EFI_ERROR (Status)){
        return EFI_TIMEOUT;
      }
      TcmBuffer[Index] = TrustedModuleIoRead8((UINTN)&TcmRegs->DataFifo);
      Index++;
      if(Index == sizeof(TCM_RSP_COMMAND_HDR)){
        break;
      }
    }
  }
  //
  // Check the response data header (tag,parasize and returncode )
  //
  CopyMem(&Data32, (TcmBuffer + 2), sizeof(UINT32));
  ResponseSize = SwapBytes32(Data32);
  *RespSize = ResponseSize;
  if(ResponseSize == sizeof(TCM_RSP_COMMAND_HDR)){
    return EFI_SUCCESS;
  }
  if(ResponseSize < sizeof(TCM_RSP_COMMAND_HDR)){
    return EFI_DEVICE_ERROR;
  }
  if(ResponseSize > TCM_CMDBUF_LENGTH){
    return EFI_BUFFER_TOO_SMALL;
  }
  //
  // Continue reading the remaining data
  //
  while (Index < ResponseSize) {
    for (; BurstCount > 0 ; BurstCount--) {
      Status = TcmPcWaitRegisterBits (
                 &TcmRegs->Status,
                 TCM_PC_STS_DATA,
                 0,
                 TCM_TIMEOUT_B
                 );
      if (EFI_ERROR (Status)) {
        return EFI_TIMEOUT;
      }
      TcmBuffer[Index] = TrustedModuleIoRead8((UINTN)&TcmRegs->DataFifo);
      Index++;
      if (Index == ResponseSize) {
        return EFI_SUCCESS;
      }
    }
    Status = TcmPcReadBurstCount(TcmRegs, &BurstCount);
    if (EFI_ERROR (Status) && (Index < ResponseSize)) {
      return EFI_DEVICE_ERROR;
    }
  }
  return EFI_SUCCESS;
}








/**
  Format TCM command data according to the format control character.

  @param[in]      FmtChar     Format control character.
  @param[in, out] ap          List of arguments.
  @param[in]      TcmBuffer   Buffer for TCM command data.
  @param[out]     DataLength  TCM command data length.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_INVALID_PARAMETER Invalid format control character.
  @retval EFI_BUFFER_TOO_SMALL  Buffer too small for command data.

**/
EFI_STATUS
TcmPcSendV (
  IN      UINT8                     FmtChar,
  IN OUT  VA_LIST                   *ap,
  UINT8                             *TcmBuffer,
  UINT32                            *DataLength
  )
{
  UINT8                             DataByte;
  UINT16                            DataWord;
  UINT32                            DataDword;
  TCM_RQU_COMMAND_HDR               TcmCmdHdr;
  TCM_RQU_COMMAND_HDR               *TcmCmdPtr;
  UINTN                             Size;
  UINT8                             *Raw;

  TcmCmdPtr = NULL;
  Raw       = NULL;
  switch (FmtChar) {

    case 'b':
      DataByte  = VA_ARG(*ap, UINT8);
      Raw = &DataByte;
      Size = sizeof(DataByte);
      break;

    case 'w':
      DataWord  = VA_ARG(*ap, UINT16);
      DataWord  = SwapBytes16(DataWord);
      Raw = (UINT8*)&DataWord;
      Size = sizeof (DataWord);
      break;

    case 'd':
      DataDword  = VA_ARG(*ap, UINT32);
      DataDword  = SwapBytes32(DataDword);
      Raw = (UINT8*)&DataDword;
      Size = sizeof(DataDword);
      break;

    case 'h':
      TcmCmdPtr           = VA_ARG (*ap, TCM_RQU_COMMAND_HDR*);
      TcmCmdHdr.tag       = SwapBytes16(TcmCmdPtr->tag);
      TcmCmdHdr.paramSize = SwapBytes32(TcmCmdPtr->paramSize);
      TcmCmdHdr.ordinal   = SwapBytes32(TcmCmdPtr->ordinal);
      Raw                 = (UINT8*)&TcmCmdHdr;
      Size                = sizeof(TcmCmdHdr);
      break;

    case 'r':
      Raw  = VA_ARG(*ap, UINT8*);
      Size = VA_ARG(*ap, UINTN);
      break;

    case '\0':
      return EFI_INVALID_PARAMETER;

    default:
      return EFI_INVALID_PARAMETER;
  }

  if(*DataLength + (UINT32) Size > TCM_CMDBUF_LENGTH) {
    return EFI_BUFFER_TOO_SMALL;
  }
  CopyMem (TcmBuffer + *DataLength, Raw, Size);
  *DataLength += (UINT32) Size;
  return EFI_SUCCESS;
}






/**
  Format response data according to the format control character.

  @param[in]      FmtChar       Format control character.
  @param[in, out] ap            List of arguments.
  @param[out]     TcmBuffer     Buffer for response data.
  @param[in, out] DataIndex     Data offset in response data buffer.
  @param[in]      RespSize      Response data length.
  @param[out]     DataFinished  Reach the end of Response data.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_INVALID_PARAMETER Invalid format control character.
  @retval EFI_BUFFER_TOO_SMALL  Buffer too small for command data.

**/
EFI_STATUS
TcmPcReceiveV (
  IN      UINT8                     FmtChar,
  IN OUT  VA_LIST                   *ap,
     OUT  UINT8                     *TcmBuffer,
  IN OUT  UINT32                    *DataIndex,
  IN      UINT32                    RespSize,
     OUT  BOOLEAN                   *DataFinished
  )
{
  UINT8                *Raw;
  TCM_RSP_COMMAND_HDR  *TcmRspPtr;
  UINTN                Size;
  EFI_STATUS           Status;

  TcmRspPtr = NULL;
  Raw = VA_ARG(*ap, UINT8*);
  switch(FmtChar){

    case 'b':
      Size = sizeof(UINT8);
      break;

    case 'w':
      Size = sizeof(UINT16);
      break;

    case 'd':
      Size = sizeof(UINT32);
      break;

    case 'h':
      Size = sizeof(*TcmRspPtr);
      break;

    case 'r':
      Size = VA_ARG (*ap, UINTN);
      if(*DataIndex + (UINT32) Size <= RespSize) {
        break;
      }
      *DataFinished = TRUE;
      if (*DataIndex >= RespSize) {
        return EFI_SUCCESS;
      }
      CopyMem (Raw, TcmBuffer + *DataIndex, RespSize - *DataIndex);
      *DataIndex += RespSize - *DataIndex;
      return EFI_SUCCESS;

    case '\0':
      return EFI_INVALID_PARAMETER;

    default:
      return EFI_WARN_UNKNOWN_GLYPH;
  }

  if(*DataIndex + (UINT32) Size > RespSize) {
    *DataFinished = TRUE;
    return EFI_SUCCESS;
  }

  if( *DataIndex + (UINT32) Size > TCM_CMDBUF_LENGTH )
    return EFI_BUFFER_TOO_SMALL;

  Status = CopyMemBS (Raw, Size, TcmBuffer + *DataIndex, Size);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "%a line %d CopyMemBS failed.\n", __FUNCTION__, __LINE__));
  }
  *DataIndex += (UINT32) Size;

  switch (FmtChar) {

    case 'w':
      *(UINT16*)Raw = SwapBytes16 (*(UINT16*)Raw);
      break;

    case 'd':
      *(UINT32*)Raw = SwapBytes32 (*(UINT32*)Raw);
      break;

    case 'h':
      TcmRspPtr = (TCM_RSP_COMMAND_HDR*)Raw;
      TcmRspPtr->tag = SwapBytes16(TcmRspPtr->tag);
      TcmRspPtr->paramSize = SwapBytes32(TcmRspPtr->paramSize);
      TcmRspPtr->returnCode = SwapBytes32(TcmRspPtr->returnCode);
      break;
  }
  return EFI_SUCCESS;
}







/**
  Send formatted command to TCM for execution and return formatted data from response.

  @param[in] TcmHandle TCM Handle.
  @param[in] Fmt       Format control string.
  @param[in] ...       The variable argument list.

  @retval EFI_SUCCESS  Operation completed successfully.
  @retval EFI_TIMEOUT  The register can't run into the expected status in time.

**/
EFI_STATUS
EFIAPI
TcmPcExecute (
  IN      EFI_TCM_HANDLE TcmHandle,
  IN      CONST CHAR8    *Fmt,
  ...
  )
{
  EFI_STATUS                 Status;
  VA_LIST                    Ap;
  UINT32                     BufSize;
  UINT32                     ResponseSize;
  BOOLEAN                    DataFinished;
  volatile TCM_PC_REGISTERS  *TcmRegs;

  TcmRegs = (volatile TCM_PC_REGISTERS*)TcmHandle;
  VA_START(Ap, Fmt);

  BufSize = 0;
  while (*Fmt != '\0') {
    if (*Fmt == '%') Fmt++;
    if (*Fmt == '/') break;
    Status = TcmPcSendV(*Fmt, &Ap, TcmCommandBuf, &BufSize);
    if(EFI_ERROR(Status)){
      return Status;
    }
    Fmt++;
  }

  Status = TcmPcSend(TcmRegs, TcmCommandBuf, BufSize);
  if(EFI_ERROR(Status)){
    TrustedModuleIoWrite8 ((UINTN)&TcmRegs->Status, TCM_PC_STS_READY);
    return Status;
  }

  TrustedModuleIoWrite8((UINTN)&TcmRegs->Status, TCM_PC_STS_GO);
  MicroSecondDelay(200);
  Fmt++;

  ZeroMem (TcmCommandBuf, TCM_CMDBUF_LENGTH);
  Status = TcmPcReceive(TcmRegs, TcmCommandBuf, &ResponseSize);
  TrustedModuleIoWrite8((UINTN)&TcmRegs->Status, TCM_PC_STS_READY);
  if(EFI_ERROR (Status)){
    return Status;
  }

  BufSize =0;
  DataFinished = FALSE;
  while (*Fmt != '\0') {
    if (*Fmt == '%') {
      Fmt++;
    }
    Status = TcmPcReceiveV(*Fmt, &Ap, TcmCommandBuf, &BufSize, ResponseSize, &DataFinished);
    if(EFI_ERROR(Status)){
      return Status;
    }
    if(DataFinished){
      return EFI_SUCCESS;
    }
    Fmt++;
  }

  VA_END(Ap);
  return Status;
}

/**
  This command returns the information of TCM  Firmware Version.

  This function parse the value got from TCM_GetCapability and return the TCM Firmware Version.

  @param[out] Firmware Version      The Firmware Version of TCM.

  @retval EFI_SUCCESS            Operation completed successfully.
  @retval EFI_DEVICE_ERROR       The command was unsuccessful.
**/
EFI_STATUS
EFIAPI
TcmGetCapabilityFirmwareVersion (
  IN  EFI_TCM_HANDLE            TcmHandle,
  OUT TCM_VERSION               *FirmwareVersion
  )

{
  EFI_STATUS                       Status;
  TCM_RSP_COMMAND_HDR              *TcmRsp;
  UINT8                            CmdBuf[64];
  UINTN                            Index;
  UINT32                           TcmSendSize;
  TCM_CAP_VERSION_INFO             *TcmCapVersionInfo;

  Status            = EFI_SUCCESS;
  TcmCapVersionInfo = NULL;
  TcmRsp            = NULL;

  for(Index=0;Index<gTcmIssueCmdMaxRetryCount;Index++){
    TcmSendSize           = sizeof(TCM_RQU_COMMAND_HDR) + sizeof (UINT32) * 3;
    *(UINT16*)&CmdBuf[0]  = SwapBytes16(TCM_TAG_RQU_COMMAND);
    *(UINT32*)&CmdBuf[2]  = SwapBytes32(TcmSendSize);
    *(UINT32*)&CmdBuf[6]  = SwapBytes32(TCM_ORD_GetCapability);

    *(UINT32*)&CmdBuf[10] = SwapBytes32(TCM_CAP_VERSION);
    *(UINT32*)&CmdBuf[14] = SwapBytes32(sizeof(TCM_CAP_VERSION_VAL));
    *(UINT32*)&CmdBuf[18] = SwapBytes32(TCM_CAP_VERSION_VAL);

    Status = TcmPcExecute(
                 TcmHandle,
                 "%r%/%r",
                 CmdBuf,
                 TcmSendSize,
                 CmdBuf,
                 sizeof(CmdBuf)
                 );

     TcmRsp = (TCM_RSP_COMMAND_HDR*)&CmdBuf[0];

    if( EFI_ERROR (Status) ||
          (SwapBytes16(TcmRsp->tag) != TCM_TAG_RSP_COMMAND) ||
          (SwapBytes32(TcmRsp->returnCode) != TCM_SUCCESS)
          ){
        Status = EFI_DEVICE_ERROR;
    }else {

      break;
    }
  }
  TcmCapVersionInfo = (TCM_CAP_VERSION_INFO*)&CmdBuf[sizeof(TCM_RSP_COMMAND_HDR) + sizeof (UINT32) - sizeof(TCM_STRUCTURE_TAG)];
  *FirmwareVersion = TcmCapVersionInfo->version;
  DEBUG((EFI_D_INFO,"TCM_ORD_GetCapability FirmwareVersion/Status - %x/%x\n", *FirmwareVersion, Status));

  return Status;
}

/**
  This command returns the information of TCM manufacture ID.

  This function parse the value got from TCM_GetCapability and return the TCM manufacture ID.

  @param[out] ManufactureId      The manufacture ID of TCM.

  @retval EFI_SUCCESS            Operation completed successfully.
  @retval EFI_DEVICE_ERROR       The command was unsuccessful.
**/
EFI_STATUS
EFIAPI
TcmGetCapabilityManufactureID (
  IN  EFI_TCM_HANDLE            TcmHandle,
  OUT     UINT32                *ManufactureId
  )

{
  EFI_STATUS                       Status;
  TCM_RSP_COMMAND_HDR              *TcmRsp;
  UINT8                            CmdBuf[64];
  UINTN                            Index;
  UINT32                           TcmSendSize;
  TCM_CAP_MANUFACTURER_INFO        *TcmManufacturerInfo;

  Status              = EFI_SUCCESS;
  TcmManufacturerInfo = NULL;
  TcmRsp              = NULL;

  for(Index=0;Index<gTcmIssueCmdMaxRetryCount;Index++){
    TcmSendSize           = sizeof(TCM_RQU_COMMAND_HDR) + sizeof (UINT32) * 3;
    *(UINT16*)&CmdBuf[0]  = SwapBytes16(TCM_TAG_RQU_COMMAND);
    *(UINT32*)&CmdBuf[2]  = SwapBytes32(TcmSendSize);
    *(UINT32*)&CmdBuf[6]  = SwapBytes32(TCM_ORD_GetCapability);

    *(UINT32*)&CmdBuf[10] = SwapBytes32(TCM_CAP_PROPERTY);
    *(UINT32*)&CmdBuf[14] = SwapBytes32(sizeof(TCM_CAP_PROP_MANUFACTURER));
    *(UINT32*)&CmdBuf[18] = SwapBytes32(TCM_CAP_PROP_MANUFACTURER);

    Status = TcmPcExecute(
                 TcmHandle,
                 "%r%/%r",
                 CmdBuf,
                 TcmSendSize,
                 CmdBuf,
                 sizeof(CmdBuf)
                 );

     TcmRsp = (TCM_RSP_COMMAND_HDR*)&CmdBuf[0];

    if( EFI_ERROR (Status) ||
          (SwapBytes16(TcmRsp->tag) != TCM_TAG_RSP_COMMAND) ||
          (SwapBytes32(TcmRsp->returnCode) != TCM_SUCCESS)
          ){
        Status = EFI_DEVICE_ERROR;
    }else {
      break;
    }
  }

  TcmManufacturerInfo = (TCM_CAP_MANUFACTURER_INFO *)&CmdBuf[sizeof(TCM_RSP_COMMAND_HDR) + sizeof (UINT32) - sizeof(TCM_STRUCTURE_TAG)];
  *ManufactureId = TcmManufacturerInfo->TcmManufacturer;

  DEBUG((EFI_D_INFO,"TCM_ORD_GetCapability ManufactureId/Status - %8x/%x\n", ManufactureId, Status));

  return Status;
}


/**
  Get Tcm current state by reading its flags.

  @param  TcmEnable                 Indicates current status is enabled or not.
  @param  TcmActivated              Indicates current status is active or not.
  @param  PhysicalPresenceLock      Indicates current status is physical present lock or not.
  @param  LifetimeLock              Indicates current status is life time lock or not.
  @param  CmdEnable                 Indicates current status is command enabled or not.

  @retval EFI_SUCCESS            Operation completed successfully.
  @Others                        Operation completed unsuccessfully.
**/
EFI_STATUS
GetTcmState (
     IN  EFI_TCM_HANDLE            TcmHandle,
     OUT BOOLEAN                   *TcmEnableStatus, OPTIONAL
     OUT BOOLEAN                   *TcmActivated, OPTIONAL
     OUT BOOLEAN                   *PhysicalPresenceLock, OPTIONAL
     OUT BOOLEAN                   *LifetimeLock, OPTIONAL
     OUT BOOLEAN                   *CmdEnable OPTIONAL
  )
{
  EFI_STATUS                       Status;
  TCM_RSP_COMMAND_HDR              *TcmRsp;
  UINT32                           TcmSendSize;
  TCM_PERMANENT_FLAGS              *TcmPermanentFlags;
  TCM_STCLEAR_FLAGS                *VFlags;
  UINT8                            CmdBuf[64];
  UINTN                            Index;

  Status            = EFI_SUCCESS;
  VFlags            = NULL;
  TcmRsp            = NULL;
  TcmPermanentFlags = NULL;

  if((TcmEnableStatus != NULL) || (TcmActivated != NULL) || (LifetimeLock != NULL) || (CmdEnable != NULL)){
    for(Index=0;Index<gTcmIssueCmdMaxRetryCount;Index++){
      TcmSendSize           = sizeof(TCM_RQU_COMMAND_HDR) + sizeof (UINT32) * 3;
      *(UINT16*)&CmdBuf[0]  = SwapBytes16(TCM_TAG_RQU_COMMAND);
      *(UINT32*)&CmdBuf[2]  = SwapBytes32(TcmSendSize);
      *(UINT32*)&CmdBuf[6]  = SwapBytes32(TCM_ORD_GetCapability);

      *(UINT32*)&CmdBuf[10] = SwapBytes32(TCM_CAP_FLAG);
      *(UINT32*)&CmdBuf[14] = SwapBytes32(sizeof(TCM_CAP_FLAG_PERMANENT));
      *(UINT32*)&CmdBuf[18] = SwapBytes32(TCM_CAP_FLAG_PERMANENT);

      Status = TcmPcExecute(
                 TcmHandle,
                 "%r%/%r",
                 CmdBuf,
                 TcmSendSize,
                 CmdBuf,
                 sizeof(CmdBuf)
                 );

      TcmRsp = (TCM_RSP_COMMAND_HDR*)&CmdBuf[0];
      if( EFI_ERROR (Status) ||
          (SwapBytes16(TcmRsp->tag) != TCM_TAG_RSP_COMMAND) ||
          (SwapBytes32(TcmRsp->returnCode) != TCM_SUCCESS)
          ){
        Status = EFI_DEVICE_ERROR;
      }else {
        break;
      }
    }
    if(EFI_ERROR(Status)){
      goto ProcExit;
    }

    TcmPermanentFlags = (TCM_PERMANENT_FLAGS*)&CmdBuf[sizeof(TCM_RSP_COMMAND_HDR) + sizeof (UINT32)];
    if (TcmEnableStatus != NULL) {
      *TcmEnableStatus = !TcmPermanentFlags->disable;
    }
    if (TcmActivated != NULL) {
      *TcmActivated = !TcmPermanentFlags->deactivated;
    }
    if (LifetimeLock != NULL) {
      *LifetimeLock = TcmPermanentFlags->physicalPresenceLifetimeLock;
    }
    if (CmdEnable != NULL) {
      *CmdEnable = TcmPermanentFlags->physicalPresenceCMDEnable;
    }
  }

  //
  // Get TCM Volatile flags (PhysicalPresenceLock)
  //
  if (PhysicalPresenceLock!=NULL) {
    for(Index=0;Index<gTcmIssueCmdMaxRetryCount;Index++){
      TcmSendSize           = sizeof(TCM_RQU_COMMAND_HDR) + sizeof (UINT32) * 3;
      *(UINT16*)&CmdBuf[0]  = SwapBytes16(TCM_TAG_RQU_COMMAND);
      *(UINT32*)&CmdBuf[2]  = SwapBytes32(TcmSendSize);
      *(UINT32*)&CmdBuf[6]  = SwapBytes32(TCM_ORD_GetCapability);

      *(UINT32*)&CmdBuf[10] = SwapBytes32(TCM_CAP_FLAG);
      *(UINT32*)&CmdBuf[14] = SwapBytes32(sizeof(TCM_CAP_FLAG_VOLATILE));
      *(UINT32*)&CmdBuf[18] = SwapBytes32(TCM_CAP_FLAG_VOLATILE);

      Status = TcmPcExecute(
                 TcmHandle,
                 "%r%/%r",
                 CmdBuf,
                 TcmSendSize,
                 CmdBuf,
                 sizeof(CmdBuf)
                 );

      TcmRsp = (TCM_RSP_COMMAND_HDR*)&CmdBuf[0];
      if( EFI_ERROR (Status) ||
          (SwapBytes16(TcmRsp->tag) != TCM_TAG_RSP_COMMAND) ||
          (SwapBytes32(TcmRsp->returnCode) != TCM_SUCCESS)
          ){
        Status = EFI_DEVICE_ERROR;
      }else {
        break;
      }
    }
    if(EFI_ERROR(Status)){
      goto ProcExit;
    }

    VFlags = (TCM_STCLEAR_FLAGS *)&CmdBuf[sizeof(TCM_RSP_COMMAND_HDR) + sizeof (UINT32)];

    if (PhysicalPresenceLock != NULL) {
      *PhysicalPresenceLock = VFlags->physicalPresenceLock;
    }
  }

ProcExit:
  return Status;
}


/**
  Extend a TCM PCR.

  @param[in]  TcmHandle       TCM handle.
  @param[in]  DigestToExtend  The 256 bit value representing the event to be recorded.
  @param[in]  PcrIndex        The PCR to be updated.
  @param[out] NewPcrValue     New PCR value after extend.

  @retval EFI_SUCCESS         Operation completed successfully.
  @retval EFI_DEVICE_ERROR    The command was unsuccessful.

**/
EFI_STATUS
EFIAPI
TcmCommExtend (
  IN      EFI_TCM_HANDLE            TcmHandle,
  IN      TCM_DIGEST                *DigestToExtend,
  IN      TCM_PCRINDEX              PcrIndex,
     OUT  TCM_DIGEST                *NewPcrValue
  )
{
  EFI_STATUS                        Status;
  TCM_DIGEST                        NewValue;
  TCM_RQU_COMMAND_HDR               CmdHdr;
  TCM_RSP_COMMAND_HDR               RspHdr;
  UINTN                             Index;

  if(NewPcrValue == NULL){
    NewPcrValue = &NewValue;
  }

  Status = EFI_SUCCESS;
  for(Index=0;Index<gTcmIssueCmdMaxRetryCount;Index++){
    CmdHdr.tag       = TCM_TAG_RQU_COMMAND;
    CmdHdr.paramSize = sizeof(CmdHdr) + sizeof(PcrIndex) + sizeof(*DigestToExtend);
    CmdHdr.ordinal   = TCM_ORD_Extend;
    Status = TcmPcExecute(
               TcmHandle,
               "%h%d%r%/%h%r",
               &CmdHdr,
               PcrIndex,
               DigestToExtend,
               (UINTN)sizeof(*DigestToExtend),
               &RspHdr,
               NewPcrValue,
               (UINTN)sizeof(*NewPcrValue)
               );
// TcmPcReceiveV() will auto do Swap for FMT "%w", "%d", "%h"
    if( EFI_ERROR(Status) ||
        (RspHdr.tag != TCM_TAG_RSP_COMMAND) ||
        (RspHdr.returnCode != TCM_SUCCESS)
        ){
      Status = EFI_DEVICE_ERROR;
    } else {
      Status = EFI_SUCCESS;
      break;
    }
  }
  return Status;
}





/**
  Send tcm command to Tcm, and get response from tcm.
  If error occurred, it will re-send several times.

  @param[in]     TcmHandle       TCM handle.
  @param[in]     InputBlock      Command block to send.
  @param[in]     InputBlockSize  Send command block size.
  @param[IN,OUT] OutputBlock     Response data from tcm.
  @param[IN]     OutputBlockSize Response data size.

  @retval EFI_SUCCESS         Operation completed successfully.
  @retval EFI_DEVICE_ERROR    The command was unsuccessful.

**/

EFI_STATUS
EFIAPI
IssueTcmCommandWithRetry (
  IN      EFI_TCM_HANDLE            TcmHandle,
  IN      UINT8                     *InputBlock,
  IN      UINT32                    InputBlockSize,
  IN      UINT8                     *OutputBlock,
  IN      UINT32                    OutputBlockSize
  )
{
  UINTN                            Index;
  EFI_STATUS                       Status;
  TCM_RSP_COMMAND_HDR              *TcmRsp;

  Status = EFI_SUCCESS;
  TcmRsp = NULL; 

  for(Index = 0; Index < gTcmIssueCmdMaxRetryCount; Index++) {

    Status = TcmPcExecute(
               TcmHandle,
               "%r%/%r",
               InputBlock,
               InputBlockSize,
               OutputBlock,
               OutputBlockSize
               );
    TcmRsp = (TCM_RSP_COMMAND_HDR*)&OutputBlock[0];

    if( EFI_ERROR(Status) ||
        (SwapBytes16(TcmRsp->tag) != TCM_TAG_RSP_COMMAND) ||
        (SwapBytes32(TcmRsp->returnCode) != TCM_SUCCESS)
        ){
      Status = EFI_DEVICE_ERROR;
    }else {
      Status = EFI_SUCCESS;
      break;
    }
  }
  return Status;
}

/**
  Set TCM Physical Presence attribute.

  @param  PhysicalPresence       Indicates physical presence attribute.

  @retval EFI_SUCCESS            Operation completed successfully.
  @Others                        Operation completed unsuccessfully.
**/
EFI_STATUS
EFIAPI
TcmPhysicalPresence (
  IN TCM_PHYSICAL_PRESENCE  PhysicalPresence
  )
{
  EFI_STATUS              Status;
  TCM_RQU_COMMAND_HDR     *TcmRqu;
  TCM_PHYSICAL_PRESENCE   *TcmPp;
  TCM_RSP_COMMAND_HDR     TcmRsp;
  UINT8                   Buffer[sizeof (*TcmRqu) + sizeof (*TcmPp)];
  EFI_TCM_HANDLE          TcmHandle;

  TcmHandle = (EFI_TCM_HANDLE)(UINTN)PcdGet64 (PcdTpmBaseAddress);

  TcmRqu = (TCM_RQU_COMMAND_HDR*)Buffer;
  TcmPp = (TCM_PHYSICAL_PRESENCE*)(TcmRqu + 1);

  TcmRqu->tag       = SwapBytes16(TCM_TAG_RQU_COMMAND);
  TcmRqu->paramSize = SwapBytes32(sizeof(Buffer));
  TcmRqu->ordinal   = SwapBytes32(TCM_ORD_PhysicalPresence);
  *TcmPp            = SwapBytes16(PhysicalPresence);

  Status = IssueTcmCommandWithRetry(TcmHandle,
                                    (UINT8*)TcmRqu,
                                    sizeof(Buffer),
                                    (UINT8*)&TcmRsp,
                                    (UINT32)sizeof(TcmRsp)
                                    );
  return Status;
}

/**
  Set TCM physical presence command enable and presence.

  @retval EFI_SUCCESS            Operation completed successfully.
  @Others                        Operation completed unsuccessfully.
**/
EFI_STATUS
EFIAPI
TcmSetOppPresent (
  VOID
  )
{
  EFI_STATUS  Status;
  BOOLEAN     LifetimeLock;
  BOOLEAN     CmdEnable;
  EFI_TCM_HANDLE TcmHandle;
  TcmHandle = (EFI_TCM_HANDLE)(UINTN)PcdGet64(PcdTpmBaseAddress);
  
  Status = GetTcmState(TcmHandle,
                       NULL,
                       NULL,
                       NULL,
                       &LifetimeLock,
                       &CmdEnable
                       );
  DEBUG((EFI_D_INFO,"TCM_DEBUG :GetTcmState Status = %x\n",Status));

  if(EFI_ERROR(Status)){
    return Status;
  }

  if (!CmdEnable) {
    if (LifetimeLock) {
      return EFI_ABORTED;
    }

    Status = TcmPhysicalPresence(TCM_PHYSICAL_PRESENCE_CMD_ENABLE);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  Status = TcmPhysicalPresence(TCM_PHYSICAL_PRESENCE_PRESENT);
  return Status;
}

/**
  Send tcm command with optional added data.

  @param  Ordinal                   Indicates command code.
  @param  AdditionalParameterSize   Added data size.
  @param  AdditionalParameters      Added data.

  @retval EFI_SUCCESS            Operation completed successfully.
  @Others                        Operation completed unsuccessfully.
**/
EFI_STATUS
EFIAPI
SimpleTcmCommand(
  IN     TCM_COMMAND_CODE  Ordinal,
  IN     UINTN             AdditionalParameterSize,
  IN     VOID              *AdditionalParameters
  )
{
  EFI_STATUS                       Status;
  TCM_RQU_COMMAND_HDR              *TcmRqu;
  TCM_RSP_COMMAND_HDR              TcmRsp;
  UINT32                           Size;
  EFI_TCM_HANDLE                   TcmHandle;

  TcmHandle = (EFI_TCM_HANDLE)(UINTN)PcdGet64 (PcdTpmBaseAddress);

  TcmRqu = (TCM_RQU_COMMAND_HDR*)AllocatePool(sizeof(*TcmRqu) + AdditionalParameterSize);

  TcmRqu->tag       = SwapBytes16(TCM_TAG_RQU_COMMAND);
  Size              = (UINT32)(sizeof(*TcmRqu) + AdditionalParameterSize);
  TcmRqu->paramSize = SwapBytes32(Size);
  TcmRqu->ordinal   = SwapBytes32(Ordinal);
  CopyMem(TcmRqu+1, AdditionalParameters, AdditionalParameterSize);

  Status = IssueTcmCommandWithRetry(TcmHandle,
                                    (UINT8*)TcmRqu,
                                    Size,
                                    (UINT8*)&TcmRsp,
                                    (UINT32)sizeof(TcmRsp)
                                    );
  FreePool(TcmRqu);

  return Status;
}

/**
  Enable or Disable TPM.

  @param  TcmEnable              Indicates to enable or disable TCM.

  @retval EFI_SUCCESS            Operation completed successfully.
  @Others                        Operation completed unsuccessfully.
**/
EFI_STATUS
EFIAPI
EnableTCMDevice (
  IN BOOLEAN TcmEnable
  )
{
  EFI_STATUS  Status;

  Status = TcmSetOppPresent();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (TcmEnable) {
    Status = SimpleTcmCommand(TCM_ORD_PhysicalEnable, 0, NULL);
  } else {
    Status = SimpleTcmCommand(TCM_ORD_PhysicalDisable, 0, NULL);
  }
  return EFI_SUCCESS;
}

/**
  Active or Deactive TPM.

  @param  TcmActive              Indicates to active or deactive TCM.

  @retval EFI_SUCCESS            Operation completed successfully.
  @Others                        Operation completed unsuccessfully.
**/
EFI_STATUS
EFIAPI
ActivateTCMDevice (
  IN BOOLEAN  TcmActive
  )
{
  EFI_STATUS   Status;
  BOOLEAN      TcmDeactive;

  Status = TcmSetOppPresent();
  if(EFI_ERROR(Status)){
    return Status;
  }

  TcmDeactive = !TcmActive;
  Status = SimpleTcmCommand(TCM_ORD_PhysicalSetDeactivated, sizeof(TcmDeactive), &TcmDeactive);
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR,"TCM Setup ActivateTCMDevice TcmDeactive/Status = %x/%x \n",TcmDeactive, Status));
  }
  return EFI_SUCCESS;
}

/**
  Force clear TPM.

  @retval EFI_SUCCESS            Operation completed successfully.
  @Others                        Operation completed unsuccessfully.
**/
EFI_STATUS
EFIAPI
TcmForceClear (
  VOID
  )
{
  EFI_STATUS  Status;

  Status = TcmSetOppPresent();
  if(EFI_ERROR (Status)){
    return Status;
  }

  Status = SimpleTcmCommand(TCM_ORD_ForceClear, 0, NULL);
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR,"TCM Setup TcmForceClear Status = %x\n",Status));
  }
  return EFI_SUCCESS;
}

/**
  Send TCM_ContinueSelfTest command to TCM.

  @param[in] TcmRegs      TCM MMIO Base Address Pointer.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_TIMEOUT           The register can't run into the expected status in time.
  @retval EFI_BUFFER_TOO_SMALL  Response data buffer is too small.
  @retval EFI_DEVICE_ERROR      Unexpected device behavior.

**/
EFI_STATUS
TcmCommContinueSelfTest (
  IN  EFI_TCM_HANDLE            TcmHandle
  )

{
  EFI_STATUS                       Status;
  TCM_RSP_COMMAND_HDR              *TcmRsp;
  UINT8                            CmdBuf[20];
  UINTN                            Index;
  UINT32                           TcmSendSize;

  TcmRsp = NULL;
  Status = EFI_SUCCESS;

  for(Index=0;Index<gTcmIssueCmdMaxRetryCount;Index++){
    TcmSendSize           = sizeof(TCM_CMD_SELF_TEST);
    *(UINT16*)&CmdBuf[0]  = SwapBytes16(TCM_TAG_RQU_COMMAND);
    *(UINT32*)&CmdBuf[2]  = SwapBytes32(TcmSendSize);
    *(UINT32*)&CmdBuf[6]  = SwapBytes32(TCM_ORD_ContinueSelfTest);

    Status = TcmPcExecute(
                 TcmHandle,
                 "%r%/%r",
                 CmdBuf,
                 TcmSendSize,
                 CmdBuf,
                 sizeof(CmdBuf)
                 );

     TcmRsp = (TCM_RSP_COMMAND_HDR*)&CmdBuf[0];

    if( EFI_ERROR (Status) ||
          (SwapBytes16(TcmRsp->tag) != TCM_TAG_RSP_COMMAND) ||
          (SwapBytes32(TcmRsp->returnCode) != TCM_SUCCESS)
          ){
        Status = EFI_DEVICE_ERROR;
    }else {
      break;
    }
  }
  return Status;
}

//-----------------------------------------------------------------------

