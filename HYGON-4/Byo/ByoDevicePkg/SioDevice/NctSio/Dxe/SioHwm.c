
/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/


#include "SioDriver.h"
#include <Library/S3BootScriptLib.h>

typedef  struct _INIT_DATA{
  UINT16      Addr16;
  UINT8       AndValue;
  UINT8       Data;
} INIT_DATA;

INIT_DATA gSmartFanConfigTable[]={
  //{R_FAN_MODE_SEL, FAN_MODE_SEL_MASK,  FAN_MODE_MANUL},
  //{R_FAN_PWM_OUT, 0x00,  0x80},

  // config fan
  {R_FAN_MODE_SEL, FAN_MODE_SEL_MASK,  FAN_MODE_SMART},
  {R_FAN_TEMP_SOURCE_SEL, FAN_TEMP_SOURC_SEL_MASK, 0x1},
  {R_SMARTFAN_CRITICAL_T, 0x00, CRITICAL_TEMP},
  {R_FANOUT_STEP, 0x00, 0x1},
  {R_SMARTFAN_T1, 0x00, T1},
  {R_SMARTFAN_T2, 0x00, T2},
  {R_SMARTFAN_T3, 0x00, T3},
  {R_SMARTFAN_T4, 0x00, T4},
  {R_SMARTFAN_PWM1, 0x00, PWM1},
  {R_SMARTFAN_PWM2, 0x00, PWM2},
  {R_SMARTFAN_PWM3, 0x00, PWM3},
  {R_SMARTFAN_PWM4, 0x00, PWM4},
  //{R_SMARTFAN_CRITICAL_TOLERANCE, FAN_TOLERANCE_MASK, CRITICALTMP_TOLERANCE},
};


EFI_STATUS
SioSmartFanInit(
){

  UINT8                      Index = 0;
  //UINT8                      Data8 = 0;
  EFI_STATUS                 Status;
  UINTN                      VarSize;
  SIO_SETUP_CONFIG           SioConfig;

  VarSize = sizeof(SIO_SETUP_CONFIG);
  ZeroMem(&SioConfig, VarSize);
  Status = gRT->GetVariable (
                  SIO_SETUP_VARIABLE_NAME,
                  &gSioSetupConfigGuid,
                  NULL,
                  &VarSize,
                  &SioConfig
                  );
  WriteHwmRegEx(0,0x4,0); // SYSFANOUT pin produces a PWM duty cycle output
  WriteHwmRegEx(0, 0x5E, ReadHwmRegEx(0, 0x5E) & (~BIT2)); // Enable CPUTIN Current Mode
  WriteHwmRegEx(0, 0x5D, ReadHwmRegEx(0, 0x5D) & (~BIT2)); // Sensor type selection for CPUTIN
  WriteHwmRegEx(0, 0x5D, ReadHwmRegEx(0, 0x5D) | BIT0); // EN_VBAT_MNT
  if (SioConfig.SysFanmode == 0x4) {
    for (Index=0;Index<sizeof(gSmartFanConfigTable)/sizeof(INIT_DATA);Index++) {
      switch (gSmartFanConfigTable[Index].Addr16) {
        case R_FAN_MODE_SEL:
          gSmartFanConfigTable[Index].Data = SioConfig.SysFanmode << 4;
          break;
        case R_FAN_TEMP_SOURCE_SEL:
          gSmartFanConfigTable[Index].Data = SioConfig.SysFanTempSource;
          break;
        case R_SMARTFAN_CRITICAL_T:
          gSmartFanConfigTable[Index].Data = SioConfig.SysFanCriticalTemp;
          break;
        case R_SMARTFAN_T1:
          gSmartFanConfigTable[Index].Data = SioConfig.SysFanTemp1;
          break;
        case R_SMARTFAN_T2:
          gSmartFanConfigTable[Index].Data = SioConfig.SysFanTemp2;
          break;
        case R_SMARTFAN_T3:
          gSmartFanConfigTable[Index].Data = SioConfig.SysFanTemp3;
          break;
        case R_SMARTFAN_T4:
          gSmartFanConfigTable[Index].Data = SioConfig.SysFanTemp4;
          break;
        case R_SMARTFAN_PWM1:
          gSmartFanConfigTable[Index].Data = SioConfig.SysFanDcPwm1 * 255 / 100;
          break;
        case R_SMARTFAN_PWM2:
          gSmartFanConfigTable[Index].Data = SioConfig.SysFanDcPwm2 * 255 / 100;
          break;
        case R_SMARTFAN_PWM3:
          gSmartFanConfigTable[Index].Data = SioConfig.SysFanDcPwm3 * 255 / 100;
          break;
        case R_SMARTFAN_PWM4:
          gSmartFanConfigTable[Index].Data = SioConfig.SysFanDcPwm4 * 255 / 100;
          break;
        default:
          break;
      }
      HwmRegAndThenOrEx(BANK_CFG_SYS_FAN, (UINT8)gSmartFanConfigTable[Index].Addr16, gSmartFanConfigTable[Index].AndValue, gSmartFanConfigTable[Index].Data);
    }
  }else{
    WriteHwmRegEx(0,0x4,0); // SYSFANOUT pin produces a PWM duty cycle output
    HwmRegAndThenOrEx(BANK_CFG_SYS_FAN, R_FAN_MODE_SEL, FAN_MODE_SEL_MASK, SioConfig.SysFanmode << 4);
    HwmRegAndThenOrEx(BANK_CFG_SYS_FAN, R_FAN_TEMP_SOURCE_SEL, FAN_TEMP_SOURC_SEL_MASK, SioConfig.SysFanTempSource);
    HwmRegAndThenOrEx(BANK_CFG_SYS_FAN, R_FAN_PWM_OUT, 0x00, 0xFF);
  }
  if (SioConfig.CpuFanmode == 0x4) {
    for (Index=0;Index<sizeof(gSmartFanConfigTable)/sizeof(INIT_DATA);Index++) {
      switch (gSmartFanConfigTable[Index].Addr16) {
        case R_FAN_MODE_SEL:
          gSmartFanConfigTable[Index].Data = SioConfig.CpuFanmode << 4;
          break;
        case R_FAN_TEMP_SOURCE_SEL:
          gSmartFanConfigTable[Index].Data = SioConfig.CpuFanTempSource;
          break;
        case R_SMARTFAN_CRITICAL_T:
          gSmartFanConfigTable[Index].Data = SioConfig.CpuFanCriticalTemp;
          break;
        case R_SMARTFAN_T1:
          gSmartFanConfigTable[Index].Data = SioConfig.CpuFanTemp1;
          break;
        case R_SMARTFAN_T2:
          gSmartFanConfigTable[Index].Data = SioConfig.CpuFanTemp2;
          break;
        case R_SMARTFAN_T3:
          gSmartFanConfigTable[Index].Data = SioConfig.CpuFanTemp3;
          break;
        case R_SMARTFAN_T4:
          gSmartFanConfigTable[Index].Data = SioConfig.CpuFanTemp4;
          break;
        case R_SMARTFAN_PWM1:
          gSmartFanConfigTable[Index].Data = SioConfig.CpuFanDcPwm1 * 255 / 100;
          break;
        case R_SMARTFAN_PWM2:
          gSmartFanConfigTable[Index].Data = SioConfig.CpuFanDcPwm2 * 255 / 100;
          break;
        case R_SMARTFAN_PWM3:
          gSmartFanConfigTable[Index].Data = SioConfig.CpuFanDcPwm3 * 255 / 100;
          break;
        case R_SMARTFAN_PWM4:
          gSmartFanConfigTable[Index].Data = SioConfig.CpuFanDcPwm4 * 255 / 100;
          break;
        default:
          break;
      }
      HwmRegAndThenOrEx(BANK_CFG_CPU_FAN, (UINT8)gSmartFanConfigTable[Index].Addr16, gSmartFanConfigTable[Index].AndValue, gSmartFanConfigTable[Index].Data);
    }
  }else {
    HwmRegAndThenOrEx(BANK_CFG_CPU_FAN, R_FAN_MODE_SEL, FAN_MODE_SEL_MASK, SioConfig.CpuFanmode << 4);
    HwmRegAndThenOrEx(BANK_CFG_CPU_FAN, R_FAN_TEMP_SOURCE_SEL, FAN_TEMP_SOURC_SEL_MASK, SioConfig.CpuFanTempSource);
    HwmRegAndThenOrEx(BANK_CFG_CPU_FAN, R_FAN_PWM_OUT, 0x00, 0xFF);
  }
  return EFI_SUCCESS;
}


EFI_STATUS
SioHwmDxeInit (
    VOID
  )
{
  EnterSioCfgMode();
#ifdef NCT6776
  WriteSioReg(R_SIO_MULTI_FUNC_SELECT, BIT0|BIT1); // select AUXFANIN1 & AUXFANIN2
#endif
#ifdef NCT6126
  WriteSioReg(R_SIO_MULTI_FUNC_SEL3, BIT0|BIT1);
#endif
  WriteSioReg(R_SIO_LOGICAL_DEVICE, SIO_HW_MONITOR_DEV_NUM);
  WriteSioReg(R_SIO_BASE_ADDRESS_HIGH, 0x02);
  WriteSioReg(R_SIO_BASE_ADDRESS_LOW, 0x90);
  WriteSioReg(R_SIO_ACTIVATE, DEVICE_ENABLE);
  SioSmartFanInit();
  DEBUG ((DEBUG_ERROR, "SMIOVT1(%X) SELECT %X \n", 0x21, ReadHwmRegEx(0x6, 0x21)));
  DEBUG ((DEBUG_ERROR, "SMIOVT2(%X) SELECT %X \n", 0x22, ReadHwmRegEx(0x6, 0x22)));
  DEBUG ((DEBUG_ERROR, "SMIOVT3(%X) SELECT %X \n", 0x23, ReadHwmRegEx(0x6, 0x23)));

  ExitSioCfgMode();
  
  DEBUG ((DEBUG_ERROR, "0x902:%d\n", IoRead8(0x902)));
  IoWrite8(0x902, IoRead8(0x902) | BIT7); // stop cpld to control fan
  DEBUG ((DEBUG_ERROR, "0x902:%d\n", IoRead8(0x902)));
  return EFI_SUCCESS;
}

