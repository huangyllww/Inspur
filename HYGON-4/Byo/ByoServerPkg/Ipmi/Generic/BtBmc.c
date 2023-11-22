/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include "IpmiBmc.h"
#include <BaseIpmi.h>
#include <Library/TimerLib.h>


typedef  struct {
    UINT8 ClrWrPtr        : 1;
    UINT8 ClrRdPtr        : 1;
    UINT8 H2bAtn          : 1;
    UINT8 B2hAtn          : 1;
    UINT8 SmsAtn          : 1;
    UINT8 Oem0            : 1;
    UINT8 HBusy           : 1;
    UINT8 BBusy           : 1;
} BT_CTRL_REGISTER;





EFI_STATUS LpcByteRead(UINTN Port, UINT8 *Value)
{
  *Value = IoRead8(Port);
  return EFI_SUCCESS;
}

EFI_STATUS LpcByteWrite(UINTN Port, UINT8 Value)
{
  IoWrite8(Port, Value);
  return EFI_SUCCESS;  
}

BOOLEAN LpcIdle(VOID)
{
  return TRUE;
}

UINT8 BtReadCtrl (
    IN  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance
)
{
  EFI_STATUS  Status;
  UINT8       Temp = 0;

  Status = LpcByteRead (IpmiInstance->IpmiIoBase + BT_REG_CTRL, &Temp);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_ERROR, "BT read byte failed\n"));
    return 0;
  }

  return Temp;
}

VOID BtWriteCtrl (
    IN  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance,
    IN  UINT8                       Data
  )
{
    EFI_STATUS Status;

    Status = LpcByteWrite (IpmiInstance->IpmiIoBase + BT_REG_CTRL, Data);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Write ctrl reg failed\n"));
    }

    return;
}



EFI_STATUS 
BtWaitRead (
  IN  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance
  )
{
  UINT8             Reg;
  UINT32            Counter = IpmiInstance->RetryCounter;
  BT_CTRL_REGISTER  *p     = (BT_CTRL_REGISTER*)&Reg;

  while (TRUE) {
    Reg = BtReadCtrl (IpmiInstance);
    if (p->B2hAtn) {
      break;
    }

    if (Counter-- == 0) {
      DEBUG ((DEBUG_ERROR, "BtWaitRead TIMEOUT\n"));
      return EFI_TIMEOUT;
    }

    MicroSecondDelay (BT_DELAY_PER_RETRY);
  }

  return EFI_SUCCESS;
}



EFI_STATUS 
BtWaitWrite (
    IN  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance
)
{
  UINT8             Reg;
  UINT32            Counter = IpmiInstance->RetryCounter;
  BT_CTRL_REGISTER  *p     = (BT_CTRL_REGISTER*)&Reg;

  while (TRUE) {
    Reg = BtReadCtrl(IpmiInstance);
    if (p->BBusy == 0 && p->H2bAtn == 0) {
      break;
    }

    if (Counter-- == 0) {
      DEBUG((DEBUG_ERROR, "BtWaitWrite TIMEOUT\n"));
      return EFI_TIMEOUT;
    }

    MicroSecondDelay (BT_DELAY_PER_RETRY);
  }

  return EFI_SUCCESS;
}


VOID 
BtWriteData (
    IN  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance,
    IN  UINT8                       Data
  )
{
  EFI_STATUS Status;

  Status = LpcByteWrite (IpmiInstance->IpmiIoBase + BT_REG_HOST2BMC, Data);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Write DATA reg failed\n"));
  }

  return;
}


UINT8 
BtReadData (
    IN  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance
  )
{
  EFI_STATUS  Status;
  UINT8       Temp;

  Status = LpcByteRead (IpmiInstance->IpmiIoBase + BT_REG_BMC2HOST, &Temp);
  if (EFI_ERROR (Status)) {
      return 0;
  }

  return Temp;
}



EFI_STATUS 
BtWriteTransaction (
    IN  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance,
    IN  UINT8                       Sequence,
    IN  UINT8                       CommandDataSize
  )
{
  EFI_STATUS      Status;
  UINT8           RegCtrl;
  UINT8           Index;
  CONST UINT8     *Data = NULL;


  if (!LpcIdle ()) {
    DEBUG ((DEBUG_ERROR, "LPC not ready\n"));
    return EFI_NOT_READY;
  }

  Status = BtWaitWrite (IpmiInstance);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  RegCtrl = BtReadCtrl(IpmiInstance);
  RegCtrl = (RegCtrl & BT_CTRL_H_BUSY) ? BT_CTRL_H_BUSY : 0;
  RegCtrl |= BT_CTRL_CLR_WR_PTR;
  BtWriteCtrl(IpmiInstance, RegCtrl);

  Data = IpmiInstance->TempData;
  BtWriteData(IpmiInstance, CommandDataSize+BT_CMD_MIN_LEN);
  BtWriteData(IpmiInstance, *Data++);
  BtWriteData(IpmiInstance, Sequence);
  BtWriteData(IpmiInstance, *Data++);
  
  for (Index = 0; Index < CommandDataSize; Index++) {
    BtWriteData(IpmiInstance, *Data++);
  }

  RegCtrl = BT_CTRL_H2B_ATN;
  BtWriteCtrl (IpmiInstance, RegCtrl);

  return EFI_SUCCESS;
}




EFI_STATUS 
BtReadTransaction (
    IN  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance,
    OUT UINT8                       *Sequence,
    OUT UINT8                       *CompletionCode,
    OUT UINT8                       *RecvLength
    )
{
  EFI_STATUS  Status;
  UINT8       Length;
  UINT8       Index;
  UINT8       *Data = NULL;
  UINT8       Count = 0;
  

  Status = BtWaitRead(IpmiInstance);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BtWriteCtrl(IpmiInstance, BT_CTRL_H_BUSY);
  BtWriteCtrl(IpmiInstance, BT_CTRL_B2H_ATN);
  BtWriteCtrl(IpmiInstance, BT_CTRL_CLR_RD_PTR);
 
  Length = BtReadData(IpmiInstance);
  Data   = IpmiInstance->OutData;
  for (Index = 0; Index < Length; Index++) {
    switch (Index) {
        case 0:                                   // NetFun/Lun
        case 2:                                   // Cmd
          *Data++ = BtReadData(IpmiInstance);
          break;

        case 1:                                   // Seq
          *Sequence = BtReadData(IpmiInstance);
          break;

        case 3:                                   // Completion Code
          *CompletionCode = BtReadData(IpmiInstance);
          break;

        default:
          *Data++ = BtReadData(IpmiInstance);
          Count++;
          break;
    }
  }

  BtWriteCtrl(IpmiInstance, BT_CTRL_H_BUSY);

  if(Length < 4){
    return EFI_DEVICE_ERROR;
  } else {
    *RecvLength = Count;
    return EFI_SUCCESS;
  }
}



EFI_STATUS
EFIAPI
EfiIpmiBtSendCommand (
  IN      EFI_IPMI_TRANSPORT           *This,
  IN      UINT8                        NetFunction,
  IN      UINT8                        Lun,
  IN      UINT8                        Command,
  IN      UINT8                        *CommandData,
  IN      UINTN                        CommandDataSize,
  IN OUT  UINT8                        *ResponseData,
  IN OUT  UINT8                        *ResponseDataSize
  )
{
  UINT8                       CompletionCode;
  EFI_STATUS                  Status;
  UINT8                       Sequence;
  UINT8                       SequenceResponse;
  IPMI_CMD_HEADER             *Response;
  UINT8                       RetryCount = 0;
  UINT8                       OrgLength;
  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance;
  EFI_IPMI_COMMAND            *IpmiCommand;
  EFI_IPMI_RESPONSE           *IpmiResponse;
  UINT8                       RecvLength;
  

  if (This == NULL || ResponseData == NULL || ResponseDataSize == NULL || (CommandDataSize != 0 && CommandData == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  IpmiInstance = INSTANCE_FROM_EFI_SM_IPMI_BMC_THIS(This);

  IpmiCommand   = (EFI_IPMI_COMMAND *) IpmiInstance->TempData;
  IpmiResponse  = (EFI_IPMI_RESPONSE *) IpmiInstance->TempData;

  if(CommandDataSize > sizeof(IpmiCommand->CommandData)){
    return EFI_INVALID_PARAMETER;    
  }

  IpmiCommand->Lun         = Lun;
  IpmiCommand->NetFunction = NetFunction;
  IpmiCommand->Command     = Command;

  if (CommandDataSize) {
    CopyMem (
      IpmiCommand->CommandData,
      CommandData,
      CommandDataSize
      );
  }

  OrgLength = *ResponseDataSize;
  do {
      RetryCount++;
      if (RetryCount > 3) {
        DEBUG((DEBUG_ERROR, "[IpmiInterface] BtExecuteIpmiCmd TIMEOUT. \n"));
        return EFI_DEVICE_ERROR;
      }

      CompletionCode = IPMI_COMPLETE_CODE_NO_ERROR;
      Sequence = IpmiInstance->Sequence++;

      Status = BtWriteTransaction(IpmiInstance, Sequence, (UINT8)CommandDataSize);
      if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "BtWriteTransaction:%r\n", Status));
        continue;
      }
      Status = BtReadTransaction(IpmiInstance, &SequenceResponse, &CompletionCode, &RecvLength);
      if (EFI_ERROR (Status)) {
        DEBUG((DEBUG_ERROR, "BtReadTransaction:%r\n", Status));
        continue;
      }

      IpmiInstance->IpmiTransport.LastCompleteCode = CompletionCode;
      
      if (OrgLength >= RecvLength) {
         CopyMem(ResponseData, IpmiInstance->OutData+2, RecvLength);
        *ResponseDataSize = RecvLength;
      } else {
        Status =  EFI_BUFFER_TOO_SMALL;
        continue;
      }

      if (CompletionCode != IPMI_COMPLETE_CODE_NO_ERROR) {
          *ResponseData = CompletionCode;
          *ResponseDataSize = 1;
          DEBUG ((DEBUG_ERROR, "CompletionCode %x\n", CompletionCode));
          Status = EFI_LOAD_ERROR;
          continue;
      }

      Response = (IPMI_CMD_HEADER*)IpmiInstance->OutData;
      if (Response->NetFn != (NetFunction | IPMI_RESPONSE_NETFN_BIT)) {
        DEBUG((DEBUG_ERROR, "NetFn ERROR %x\n", Response->NetFn));
        Status = EFI_NO_MAPPING;
        continue;
      }
  } while (EFI_ERROR (Status));

  return Status;
}





