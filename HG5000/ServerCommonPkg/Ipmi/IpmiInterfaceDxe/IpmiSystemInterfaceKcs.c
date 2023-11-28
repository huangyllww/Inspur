/** @file
 Byosoft IPMI KCS interface implement code.
*/

#include "IpmiInterfaceCommon.h"
#include "Token.h"

#ifndef IPMI_DELAY_TIMER
#define IPMI_DELAY_TIMER       10000
#endif
/**
 Read KCS Status Register.

 @param[in]         IpmiData            A pointer to IPMI_PRIVATE_DATA.
 @param[out]        KcsStatus           A pointer to buffer storing KCS status.
*/
VOID
KcsReadStatus (
  IN  IPMI_PRIVATE_DATA                 *IpmiData,
  OUT VOID                              *KcsStatus
  )
{
  UINT8                                 *KcsStatusPtr;

  KcsStatusPtr = (UINT8*)KcsStatus;
  *KcsStatusPtr = IoRead8 (IpmiData->IpmiBaseAddress + IpmiData->IpmiBaseAddressOffset);
}

/**
 Read KCS Data Register

 @param[in]         IpmiData            A pointer to IPMI_PRIVATE_DATA.
 @param[out]        KcsData             A pointer to buffer storing KCS data.
*/
VOID
KcsReadData (
  IN  IPMI_PRIVATE_DATA                 *IpmiData,
  OUT VOID                              *KcsData
  )
{
  UINT8                                 *KcsDataPtr;

  KcsDataPtr = (UINT8*)KcsData;
  *KcsDataPtr = IoRead8 (IpmiData->IpmiBaseAddress);
}

/**
 Write command to KCS Command Register

 @param[in]         IpmiData            A pointer to IPMI_PRIVATE_DATA.
 @param[in]         KcsCommand          Command to write to KCS.
*/
VOID
KcsWriteCommand (
  IN  IPMI_PRIVATE_DATA                 *IpmiData,
  IN  UINT8                             KcsCommand
  )
{
  IoWrite8 (IpmiData->IpmiBaseAddress + IpmiData->IpmiBaseAddressOffset, KcsCommand);
}

/**
 Write data to KCS Data Register

 @param[in]         IpmiData            A pointer to IPMI_PRIVATE_DATA.
 @param[in]         KcsData             Data to write to KCS.
*/
VOID
KcsWriteData (
  IN  IPMI_PRIVATE_DATA                 *IpmiData,
  IN  UINT8                             KcsData
  )
{
  IoWrite8 (IpmiData->IpmiBaseAddress, KcsData);
}

/**
 Wait KCS system interface output buffer full.

 @param[in]         IpmiData            A pointer to IPMI_PRIVATE_DATA.
 
 @retval EFI_SUCCESS                    Output buffer is full in a given time.
 @retval EFI_TIMEOUT                    Output buffer is not full in a given time.
*/
EFI_STATUS
KcsWaitObFull (
  IN  IPMI_PRIVATE_DATA             *IpmiData
  )
{
  UINT32                           k = 0;
  KCS_STATUS_REGISTER              Reg;

  for (k = 0; k < IpmiData->TotalTimeTicks; k++) {
    KcsReadStatus (IpmiData, &Reg);
    if (Reg.Obf == 1) {
      return EFI_SUCCESS;
    }

    MicroSecondDelay(IPMI_DELAY_TIMER);
  }

  return EFI_TIMEOUT;
}

/**
 Wait KCS system interface input buffer empty.

 @param[in]         IpmiData            A pointer to IPMI_PRIVATE_DATA.
 
 @retval EFI_SUCCESS                    Iutput buffer is empty in a given time.
 @retval EFI_TIMEOUT                    Iutput buffer is not empty in a given time.
*/
EFI_STATUS
KcsWaitIbEmpty(
  IN  IPMI_PRIVATE_DATA             *IpmiData
  )
{
  UINT32                              k = 0;
  KCS_STATUS_REGISTER                 Reg;

  for (k = 0; k < IpmiData->TotalTimeTicks; k++) {
    KcsReadStatus (IpmiData, &Reg);
    if (Reg.Ibf == 0) {
      return EFI_SUCCESS;
    }

    MicroSecondDelay(IPMI_DELAY_TIMER);
  }

  return EFI_TIMEOUT;
}

/**
 Clear BMC's output buffer.

 @param[in]         IpmiData            A pointer to IPMI_PRIVATE_DATA.
*/
VOID
KcsClearObf (
  IN  IPMI_PRIVATE_DATA             *IpmiData
  )
{
  KCS_STATUS_REGISTER                   Reg;
  UINT8                                 Data;

  KcsReadStatus (IpmiData, &Reg);

  if (Reg.Obf == 1) {
    KcsReadData (IpmiData, &Data);
  }

}

/**
 Get KCS system interface state.

 @param[in]         IpmiData            A pointer to IPMI_PRIVATE_DATA.
 
 @retval KCS Interface's State Bits.
*/
UINT8
KcsGetState (
  IN  IPMI_PRIVATE_DATA             *IpmiData
  )
{
  UINT8                                 State;

  KcsReadStatus (IpmiData, &State);

  return (State >> 6);
}

/**
 KCS System Interface Error Processing.

 @param[in]         IpmiData            A pointer to IPMI_PRIVATE_DATA.
 @param[out]        StatusCodes         KCS Interface Status Codes.
 
 @retval EFI_ABORTED                    KCS Interface is idle.
 @retval EFI_TIMEOUT                    Output buffer is not full or iutput buffer is not empty
                                        in a given time.
 @retval EFI_DEVICE_ERROR               KCS Interface cannot enter idle state.
*/
EFI_STATUS
KcsErrorExit (
  IN  IPMI_PRIVATE_DATA                 *IpmiData,
  OUT UINT16                            *StatusCodes
  )
{
  UINT8                                 Command;
  UINT8                                 Data;
  UINT8                                 KcsStatusCodes;
  UINTN                                 Retry;
  EFI_STATUS                            Status;

  Retry = 0;
  while (Retry < KCS_RETRY_LIMIT) {

    Status = KcsWaitIbEmpty (IpmiData);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Write GET_STATUS/ABORT command to enter "error1" phase
    //
    Command = KCS_CC_GET_STATUS_ABORT;
    KcsWriteCommand (IpmiData, Command);

    Status = KcsWaitIbEmpty (IpmiData);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    KcsClearObf (IpmiData);

    //
    // Write 00h dummy byte to DATA_IN to enter "error2" phase
    //
    Data = 0x00;
    KcsWriteData (IpmiData, Data);

    Status = KcsWaitIbEmpty (IpmiData);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (KcsGetState (IpmiData) != KCS_READ_STATE) {
      ++Retry;
      continue;
    }

    Status = KcsWaitObFull (IpmiData);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    KcsReadData (IpmiData, &KcsStatusCodes);
    if (StatusCodes != NULL) {
      *StatusCodes = (UINT16)KcsStatusCodes | STATUS_CODE_ENABLE;
    }

    //
    // Write READ to DATA_IN to enter "error3" phase
    //
    Data = KCS_CC_READ;
    KcsWriteData (IpmiData, Data);

    Status = KcsWaitIbEmpty (IpmiData);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (KcsGetState (IpmiData) != KCS_IDLE_STATE) {
      ++Retry;
      continue;
    } else {
      break;
    }

  }

  if (Retry == KCS_RETRY_LIMIT) {
    return EFI_DEVICE_ERROR;
  }

  Status = KcsWaitObFull (IpmiData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  KcsClearObf (IpmiData);

  //
  // Enter "idle" phase
  //
  return EFI_ABORTED;
}

/**
 KCS System Interface Write Processing.

 @param[in]         IpmiData            A pointer to IPMI_PRIVATE_DATA.
 @param[in]         Request             IPMI_CMD_HEADER sturcture, incoude NetFnLun and Cmd
 @param[in]         SendData            Command data buffer to be written to BMC
 @param[in]         SendLength          Command data length of command data buffer
 @param[out]        StatusCodes         The bit 15 of StatusCodes means this argument is valid or not:
                                        1. If bit 15 set to 1, this is a valid Status Code,
                                        and the Status Code is in low byte.
                                        2. If bit 15 set to 0, there is no Status Code
                                        StatusCodes is valid when return value is EFI_ABORTED.
                                        If the return value is EFI_DEVICE_ERROR, it does not 
                                        guarantee StatusCodes is valid, the caller must check bit 15.
                                        
 @retval EFI_SUCCESS                    Write Transaction finish successfully.
 @retval EFI_ABORTED                    KCS Interface is idle.
 @retval EFI_TIMEOUT                    Output buffer is not full or iutput buffer is not empty
                                        in a given time.
 @retval EFI_DEVICE_ERROR               KCS Interface cannot enter idle state.
*/
EFI_STATUS
KcsWriteTransaction (
  IN  IPMI_PRIVATE_DATA                 *IpmiData,
  IN  IPMI_CMD_HEADER                   *Request,
  IN  VOID                              *SendData OPTIONAL,
  IN  UINTN                             SendLength,
  OUT UINT16                            *StatusCodes OPTIONAL
  )
{
  EFI_STATUS                            Status;
  UINT8                                 Command;
  UINTN                                 Index;
  UINTN                                 TotalLength;
  UINT8                                 *Buffer;
  BOOLEAN                               Done;

  //
  // Ready to write command
  //
  Status = KcsWaitIbEmpty (IpmiData);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  KcsClearObf (IpmiData);

  //
  // wr_data phase
  //
  Index = 0;
  TotalLength = sizeof (IPMI_CMD_HEADER) + SendLength;
  Buffer = (UINT8*)Request;
  Done = FALSE;
  while (!Done) {
    switch (Index) {
    case 0:
      //
      // case 0 always write command to enter "wr_start" phase
      //
      Command = KCS_CC_WRITE_START;
      KcsWriteCommand (IpmiData, Command);
      break;

    case 1:
      //
      // case 1 always write NetFnLun
      //
      KcsWriteData (IpmiData, *Buffer);
      Buffer++;
      break;

    default:
      //
      // default have 2 possible.
      //   1. More then one byte left.
      //   2. Only one byte left.
      //
      if (Index == TotalLength) {
        Command = KCS_CC_WRITE_END;
        KcsWriteCommand (IpmiData, Command);
        
        Done = TRUE;

      } else {
        KcsWriteData (IpmiData, *Buffer);

        if (Index == 2) {
          Buffer = (UINT8*)SendData;
        } else {
          ++Buffer;
        }
      }
      break;
    }

    Status = KcsWaitIbEmpty (IpmiData);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (KcsGetState (IpmiData) != KCS_WRITE_STATE ) {
      goto ErrorExit;
    }

    KcsClearObf (IpmiData);
    ++Index;
  }

  KcsWriteData (IpmiData, *Buffer);

  return EFI_SUCCESS;

ErrorExit:
  return KcsErrorExit(IpmiData, StatusCodes);
}

/**
 KCS System Interface Read Processing.

 @param[in]         IpmiData            A pointer to IPMI_PRIVATE_DATA.
 @param[out]        Response            IPMI_CMD_HEADER sturcture, incoude NetFnLun and Cmd
 @param[out]        CompletionCode      Completion code of this transaction
 @param[out]        RecvData            Data buffer to put the data read from BMC (from completion code)
 @param[out]        RecvLength          Length of Data readed from BMC
 @param[out]        StatusCodes         The bit 15 of StatusCodes means this argument is valid or not:
                                        1. If bit 15 set to 1, this is a valid Status Code,
                                        and the Status Code is in low byte.
                                        2. If bit 15 set to 0, there is no Status Code
                                        StatusCodes is valid when return value is EFI_ABORTED.
                                        If the return value is EFI_DEVICE_ERROR, it does not 
                                        guarantee StatusCodes is valid, the caller must check bit 15.
 
 @retval EFI_SUCCESS                    Read Transaction finish successfully.
 @retval EFI_ABORTED                    KCS Interface is idle.
 @retval EFI_TIMEOUT                    Output buffer is not full or iutput buffer is not empty
                                        in a given time.
 @retval EFI_DEVICE_ERROR               KCS Interface cannot enter idle state. 
*/
EFI_STATUS
KcsReadTransaction (
  IN  IPMI_PRIVATE_DATA                 *IpmiData,
  OUT IPMI_CMD_HEADER                   *Response,
  OUT UINT8                             *CompletionCode,
  OUT VOID                              *RecvData,
  OUT UINT8                             *RecvLength,
  OUT UINT16                            *StatusCodes OPTIONAL
  )
{
  UINT8                                 *Buffer;
  UINT8                                 Index;
  UINT8                                 DataByte;
  UINT8                                 DummyByte;
  EFI_STATUS                            Status;

  //
  // Continue to read data from data_out
  // till IDLE_STATE
  //
  *RecvLength = 0;
  Buffer = (UINT8*)Response;
  Index = 0;
  DataByte = KCS_CC_READ;
  while (TRUE) {
    Status = KcsWaitIbEmpty (IpmiData);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (KcsGetState (IpmiData) != KCS_READ_STATE) {
      break;
    }

    Status = KcsWaitObFull (IpmiData);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    KcsReadData (IpmiData, Buffer);

    switch (Index) {
    case 0:
      //
      // case 0: receive NetFunLun Response byte
      // next will be command byte
      //
      ++Buffer;
      break;

    case 1:
      //
      // case 1: receive command byte
      // next will be all data bytes,
      // but we separate the completion code from all data bytes
      //
      Buffer = CompletionCode;
      break;

    case 2:
      //
      // case 2: receive completion code
      // next will be all other data bytes
      //
      Buffer = (UINT8*)RecvData;
      break;

    default:
      //
      // default: receive all data (from completion code)
      //
      ++Buffer;
      ++(*RecvLength);
      break;
    }
    ++Index;
    KcsWriteData (IpmiData, DataByte);
  }

  //
  // The state should be IDLE_STATE
  // If the state is not IDLE_STATE, go to Error_Exit
  //
  if (KcsGetState (IpmiData) != KCS_IDLE_STATE) {
    return KcsErrorExit(IpmiData, StatusCodes);
  }

  Status = KcsWaitObFull (IpmiData);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  KcsReadData (IpmiData, &DummyByte);
  
  return EFI_SUCCESS;
}

/**
 Send request, include Network Function, LUN, and command of IPMI, with/without
 additional data to BMC.

 @param[in]        This                 A pointer to IPMI_INTERFACE_PROTOCOL structure.
 @param[in]        Request              IPMI_CMD_HEADER structure, storing Network Function, LUN
                                        and various IPMI command, to send to BMC.
 @param[in]        SendData             Optional arguments, if an IPMI command is required to
                                        send with data, this argument is a pointer to the data buffer.
                                        If no data is required, set this argument as NULL.
 @param[in]        SendLength           When sending command with data, this argument is the length of the data,
                                        otherwise set this argument as 0.
 @param[out]       RecvData             Data buffer to put the data read from BMC.
 @param[out]       RecvLength           Length of Data readed from BMC.
 @param[out]       StatusCodes          The bit 15 of StatusCodes means this argument is valid or not:
                                        1. If bit 15 set to 1, this is a valid Status Code,
                                        and the Status Code is in low byte.
                                        2. If bit 15 set to 0, there is no Status Code
                                        StatusCodes is valid when return value is EFI_ABORTED.
                                        If the return value is EFI_DEVICE_ERROR, it does not 
                                        guarantee StatusCodes is valid, the caller must check bit 15.

 @retval EFI_SUCCESS                    Execute command successfully.
 @retval EFI_ABORTED                    1. When writing to BMC, BMC cannot enter "Write State",
                                        the error processing make BMC to enter "Idle State" successfully.
                                        2. When finishing reading from BMC, BMC cannot enter "Idle State",
                                        the error processing make BMC to enter "Idle State" successfully.
 @retval EFI_TIMEOUT                    Output buffer is not full or iutput buffer is not empty
                                        in a given time.
 @retval EFI_DEVICE_ERROR               1. When writing to BMC, BMC cannot enter "Write State",
                                        the error processing cannot make BMC to enter "Idle State".
                                        2. When finishing reading from BMC, BMC cannot enter "Idle State",
                                        the error processing cannot make BMC to enter "Idle State".
 @retval EFI_NO_MAPPING                 The request Network Function and the response Network Function
                                        does not match.
 @retval EFI_LOAD_ERROR                 Execute command successfully, but the completion code return
                                        from BMC is not 00h.
 @retval EFI_INVALID_PARAMETER          This or RecvData or RecvLength is NULL.
*/
EFI_STATUS
EFIAPI
KcsExecuteIpmiCmd (
  IN  IPMI_INTERFACE_PROTOCOL           *This,
  IN  IPMI_CMD_HEADER                   Request,
  IN  VOID                              *SendData OPTIONAL,
  IN  UINTN                             SendLength,
  OUT VOID                              *RecvData,
  OUT UINT8                             *RecvLength,
  OUT UINT16                            *StatusCodes OPTIONAL
  )
{ 
  UINT8                                 CompletionCode;
  EFI_STATUS                            Status;
  IPMI_CMD_HEADER                       Response;
  IPMI_PRIVATE_DATA                     *IpmiData;
  UINT8                                 Retry;

  DEBUG ((EFI_D_INFO, "\n[IpmiInterfaceDxeSmm] KcsExecuteIpmiCmd Start:\n"));

  {
    UINTN Index;
    UINT8* Buffer;
    DEBUG ((EFI_D_INFO, "Request NetFun: %x\n",Request.NetFn));
    DEBUG ((EFI_D_INFO, "       Command: %x",Request.Cmd));

    if(SendData!=NULL){
      Buffer = (UINT8*)SendData;
      DEBUG ((EFI_D_INFO, "\nSend Data:"));
      for(Index=0;Index<SendLength;Index++){
        DEBUG ((EFI_D_INFO, "%02x ",Buffer[Index]));
      }
      Buffer = NULL;
      DEBUG ((EFI_D_INFO, "\n"));
    }
  }
  if (This == NULL || RecvData == NULL || RecvLength == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  if (StatusCodes != NULL) {
    *StatusCodes = 0;
  }

  //
  // If we get abort status, it means there is error during transaction and BMC
  // enter idle state. Try again to cover it.
  //
  CompletionCode = IPMI_COMPLETE_CODE_NO_ERROR;
  Status = EFI_TIMEOUT;
  Response.NetFn = 0;
  IpmiData = IPMI_PRIVATE_DATA_FROM_THIS (This);

  for (Retry = 0; Retry < 3; ++Retry) {
    Status = KcsWriteTransaction (IpmiData, &Request, SendData, SendLength, StatusCodes);
    if (EFI_ERROR (Status)) {
      if (Status == EFI_ABORTED) {
        continue;
      } else {
        goto Exit;
      }
    }

    Status = KcsReadTransaction (
               IpmiData,
               &Response,
               &CompletionCode,
               RecvData,
               RecvLength,
               StatusCodes
               );
    if (!EFI_ERROR (Status)) {
      break;
    } else {
      if (Status == EFI_ABORTED) {
        continue;
      } else {
        goto Exit;
      }
    }
    
  }
  IpmiData->LastCompleteCode = CompletionCode;
  //
  // After retry, it is still not success, return directly
  //
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  if (CompletionCode != IPMI_COMPLETE_CODE_NO_ERROR) {
    *((UINT8*)RecvData) = CompletionCode;
    *RecvLength = 1;
    Status = EFI_LOAD_ERROR;
    goto Exit;
  }
#if 0
  {
    UINTN Index;
    UINT8* Buffer;
    if(RecvLength!=0){
      Buffer = (UINT8*)RecvData;
      DEBUG ((EFI_D_INFO, "\nRecvice Data:"));
      for(Index=0;Index<*RecvLength;Index++){
        DEBUG ((EFI_D_INFO, "%02x ",Buffer[Index]));
      }
      Buffer = NULL;
      DEBUG ((EFI_D_INFO, "\n"));
    }
  }
#endif  
  if (Response.NetFn != (Request.NetFn | IPMI_RESPONSE_NETFN_BIT)) {
    Status = EFI_NO_MAPPING;
    goto Exit;
  }

  Status = EFI_SUCCESS;

Exit:
  DEBUG ((EFI_D_INFO, "[IpmiInterfaceDxeSmm] KcsExecuteIpmiCmd End. Status: %r\n", Status));
  
  return Status;
}

/**
 KCS return SMS_ATN Flag set or not.

 @param[in]         IpmiData            A pointer to IPMI_PRIVATE_DATA.

 @retval TRUE                           SMS_ATN bit is set.
 @retval FALSE                          SMS_ATN bit is not set.
*/
BOOLEAN
KcsReadSmsAtnFlag (
  IN  IPMI_PRIVATE_DATA             *IpmiData
  )
{
  KCS_STATUS_REGISTER                   Reg;

  KcsReadStatus (IpmiData, &Reg);

  if (Reg.SmsAtn == 1) {
    return TRUE;
  } else {
    return FALSE;
  }

}

/**
 Return system interface type that BMC currently use.

 @param[in]         This                A pointer to IPMI_INTERFACE_PROTOCOL structure.

 @retval     IPMI_KCS                   Current system interface type is KCS.
*/
IPMI_INTERFACE_TYPE
EFIAPI
KcsGetInterfaceType (
  IN  IPMI_INTERFACE_PROTOCOL       *This
  )
{
  return IPMI_KCS;
}

/**
 Return system interface name that BMC currently use.

 @param[in]         This                A pointer to IPMI_INTERFACE_PROTOCOL structure.
 @param[out]        InterfaceName       The buffer storing the name string. It is the caller's
                                        responsibility to provide enough space to store the name string.
                                        The longest name is five CHAR16, that is, four for interface
                                        name string and one for NULL character.

 @retval EFI_INVALID_PARAMETER          InterfaceName is NULL.
 @retval EFI_SUCCESS                    Return interface name string successfully.
*/
EFI_STATUS
EFIAPI
KcsGetInterfaceName (
  IN  IPMI_INTERFACE_PROTOCOL           *This,
  OUT CHAR16                            *InterfaceName
  )
{

  if (InterfaceName == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  InterfaceName[0] = L'K';
  InterfaceName[1] = L'C';
  InterfaceName[2] = L'S';
  InterfaceName[3] = 0;

  return EFI_SUCCESS;
}

/**
 Initial function pointer of KCS system interface to corrsponding function.

 @param[in]         IpmiData            A pointer to IPMI_PRIVATE_DATA
*/
VOID
InitialKcsSystemInterface (
  IN  IPMI_PRIVATE_DATA             *IpmiData
  )
{
  IpmiData->IpmiProtocol.GetIpmiInterfaceType = KcsGetInterfaceType;
  IpmiData->IpmiProtocol.GetIpmiInterfaceName = KcsGetInterfaceName;
  IpmiData->IpmiProtocol.ExecuteIpmiCmd = KcsExecuteIpmiCmd;
  IpmiData->ReadSmsAtnFlag = KcsReadSmsAtnFlag;
}
