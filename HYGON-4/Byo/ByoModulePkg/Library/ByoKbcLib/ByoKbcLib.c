/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>

//---------------------------------- PS2 ---------------------------------------
#define  CMD_READ_OUTPUT_PORT     0xD0
#define  CMD_WRITE_OUTPUT_PORT    0xD1
#define    OUTPUT_PORT_GATEA20      BIT1
#define  KBC_DISMS_INF           0xA7
#define  KBC_ENMS_INF            0xA8
#define  KBC_KB_INF_ST           0xAB
#define  KBC_DISKEYBD_INF        0xAD
#define  KBC_ENAKEYBD_INF        0xAE
#define  KBC_WRITMSD_BYTE        0xD4
#define  MS_READ_DEV_TYPE        0xF2
#define  MS_DISABLE_REPORT       0xF5
#define  KB_ECHO                 0xEE
#define  KB_ENABLE_SCANING       0xF4
#define  KBC_ACK_STATUS          0xFA

#define KBC_STS_ERR0   0
#define KBC_STS_ERR1   0xFF
#define KBC_CMD_PORT   0x64
#define KBC_STS_PORT   0x64
#define KBC_OUT_BUFFUL BIT0
#define KBC_IN_BUFFUL  BIT1  
#define KBC_DAT_PORT   0x60
#define KBC_TIMEOUT    65536   // 0.07s

EFI_STATUS KbcWaitInputBufferFree()
{
  UINT32  TimeOut;
  
  for (TimeOut = 0; TimeOut < KBC_TIMEOUT; TimeOut += 30) {
    if (!(IoRead8(KBC_STS_PORT) & KBC_IN_BUFFUL)) {
      break;
    }
    MicroSecondDelay(30);
  }
  if(TimeOut == KBC_TIMEOUT){
    return EFI_TIMEOUT;
  }else{
    return EFI_SUCCESS;
  }
}

EFI_STATUS KbcWaitOutputBufferFull()
{
  UINT32  TimeOut;
  
  for (TimeOut = 0; TimeOut < KBC_TIMEOUT; TimeOut += 30) {
    if (IoRead8(KBC_STS_PORT) & KBC_OUT_BUFFUL) {
      break;
    }
    MicroSecondDelay(30);
  }
  if(TimeOut == KBC_TIMEOUT){
    return EFI_TIMEOUT;
  }else{
    return EFI_SUCCESS;
  }
}

EFI_STATUS KbcWaitOutputBufferFree()
{
  UINT32  TimeOut;
  
  for (TimeOut = 0; TimeOut < KBC_TIMEOUT; TimeOut += 30) {
    if (!(IoRead8(KBC_STS_PORT) & KBC_OUT_BUFFUL)) {
      break;
    }
    IoRead8(KBC_DAT_PORT);
  }
  if(TimeOut == KBC_TIMEOUT){
    return EFI_DEVICE_ERROR;
  }else{
    return EFI_SUCCESS;
  }
}

EFI_STATUS KbcSendCmd(UINT8 Cmd)
{
  EFI_STATUS  Status;

  Status = KbcWaitInputBufferFree();
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }
  IoWrite8(KBC_CMD_PORT, Cmd);
  Status = KbcWaitInputBufferFree();
  
ProcExit:
  return Status;  
}


BOOLEAN CheckKbcPresent(VOID)
{
  UINT8   KbcSts;

  KbcSts = IoRead8 (KBC_CMD_PORT);

  if (KbcSts == KBC_STS_ERR0) {
    return FALSE;
  }

  if (KbcSts == KBC_STS_ERR1) {
    return FALSE;
  }

  return TRUE;
}


EFI_STATUS
KbcCmdReadData (
  IN  UINT8  Cmd,
  OUT UINT8  *Data  OPTIONAL
  )
{
  EFI_STATUS  Status;
  BOOLEAN     IntState;
  UINT8       Data8;

  IntState = SaveAndDisableInterrupts();  
  
  if(!CheckKbcPresent()){
    Status = EFI_UNSUPPORTED;
    goto ProcExit;
  }
  
  Status = KbcWaitOutputBufferFree();
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }  
  
  Status = KbcSendCmd(Cmd);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }  
  Status = KbcWaitOutputBufferFull();
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  Data8 = IoRead8(KBC_DAT_PORT);
  if(Data != NULL){
    *Data = Data8;
  }

ProcExit: 
  SetInterruptState(IntState);
  return Status;
}


VOID IsPs2DevicePresent(EFI_BOOT_SERVICES *BS, BOOLEAN *Ps2Kb, BOOLEAN *Ps2Ms)
{
  EFI_STATUS  Status;
  BOOLEAN     KbInfDis = FALSE;
  BOOLEAN     MsInfDis = FALSE;
  UINTN       Loops;
  UINTN       ErrCount;
  EFI_TPL     OldTpl;
  UINT8       Data8;
  
 
  *Ps2Kb = FALSE;
  *Ps2Ms = FALSE;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  OldTpl = BS->RaiseTPL(TPL_HIGH_LEVEL);

  if(!CheckKbcPresent()){
    goto ProcExit;
  }

  Status = KbcSendCmd(KBC_DISKEYBD_INF);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  } 
  KbInfDis = TRUE;
  
  Status = KbcSendCmd(KBC_DISMS_INF);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  } 
  MsInfDis = TRUE;

  Loops = 20;
  ErrCount = 0;
  while((--Loops) && ErrCount < 3){
    Status = KbcWaitOutputBufferFree();
    if(EFI_ERROR(Status)){
      DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));
      continue;
    }
    Status = KbcWaitInputBufferFree();
    if(EFI_ERROR(Status)){
      DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));
      continue;
    }
    
    Status = KbcSendCmd(KBC_WRITMSD_BYTE);
    if(EFI_ERROR(Status)){
      DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));
      continue;
    }
    IoWrite8(KBC_DAT_PORT, MS_READ_DEV_TYPE);
    Status = KbcWaitOutputBufferFull();
    if(EFI_ERROR(Status)){
      DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));
      continue;
    }
    Data8 = IoRead8(KBC_DAT_PORT);
    if(Data8 != 0xFA){
      ErrCount++;
      DEBUG((EFI_D_INFO, "(L%d) %X\n", __LINE__, Data8));
      continue;
    }
    
    Status = KbcWaitOutputBufferFull();
    if(EFI_ERROR(Status)){
      DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));
      continue;
    }
    Data8 = IoRead8(KBC_DAT_PORT);
    if (Data8 == 0x00 || Data8 == 0x03 || Data8 == 0x04 || Data8 == 0xFF) { //IVS-20181217 OS driver can change the Data of PS2MS returned when OS reboot.
      *Ps2Ms = TRUE;
      break;
    } else {
      ErrCount++;
      DEBUG((EFI_D_INFO, "(L%d) %X\n", __LINE__, Data8));
    }
  }

  DEBUG((EFI_D_INFO, "Loops:%d, ErrCount:%d, Ps2Ms:%d\n", Loops, ErrCount, *Ps2Ms));


  Loops = 20;
  ErrCount = 0;  
  while((--Loops) && ErrCount < 3){
    Status = KbcWaitOutputBufferFree();
    if(EFI_ERROR(Status)){
      DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));
      continue;
    }
    Status = KbcWaitInputBufferFree();
    if(EFI_ERROR(Status)){
      DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));
      continue;
    }
    
    IoWrite8(KBC_DAT_PORT, KB_ECHO);
    Status = KbcWaitOutputBufferFull();
    if(EFI_ERROR(Status)){
      DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));
      continue;
    }
    Data8 = IoRead8(KBC_DAT_PORT);
    if(Data8 == KB_ECHO) {
      *Ps2Kb = TRUE;
      break;
    } else {
      ErrCount++;
      DEBUG((EFI_D_INFO, "(L%d) %X\n", __LINE__, Data8));
    }
  }

  DEBUG((EFI_D_INFO, "Loops:%d, ErrCount:%d, Ps2Kb:%d\n", Loops, ErrCount, *Ps2Kb));  


ProcExit:
  if(KbInfDis){
    Status = KbcSendCmd(KBC_ENAKEYBD_INF);
    DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));
  }
  if(MsInfDis){
    Status = KbcSendCmd(KBC_ENMS_INF);
    DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));
  }
  BS->RestoreTPL(OldTpl);
  return;
}


EFI_STATUS
KbcCmdSendData (
  IN  UINT8  Cmd,
  OUT UINT8  Data
  )
{
  EFI_STATUS  Status;
  BOOLEAN     IntState;


  IntState = SaveAndDisableInterrupts();  
  
  if(!CheckKbcPresent()){
    Status = EFI_UNSUPPORTED;
    goto ProcExit;
  }
  
  Status = KbcWaitOutputBufferFree();
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }  
  
  Status = KbcSendCmd(Cmd);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }  
  Status = KbcWaitInputBufferFree();
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  IoWrite8(KBC_DAT_PORT, Data);
  Status = KbcWaitInputBufferFree();
  
ProcExit: 
  SetInterruptState(IntState);
  return Status;
}


