
/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __SIO_SETUP_DATA_STRUCT__
#define __SIO_SETUP_DATA_STRUCT__

#include <Guid/HiiPlatformSetupFormset.h>


#define SYSTEM_CONFIGURATION_GUID \
  { \
    0xec87d643, 0xeba4, 0x4bb5, {0xa1, 0xe5, 0x3f, 0x3e, 0x36, 0xb2, 0xd, 0xa9} \
  }

#define SETUP_SIO_CFG_GUID \
  { \
    0x140fcbc6, 0xe8d1, 0x4b0a, {0xaa, 0x5f, 0xf1, 0x17, 0xe0, 0xa, 0x18, 0x85} \
  }

#define AUTO_ID(x) x

#define SIO_CONFIG_VARSTORE_ID      0x0001
#define SIO_CONFIG_FORM_ID          0x0001

#define HEALTH_MONITOR_FORM_ID       0x1000
#define FAN_CONTROL_FORM_ID          0x1015

#define CPUTIN_TEMP_KEY               0x1001
#define SYSTIN_TEMP_KEY               0x1002
#define AUXTIN_TEMP_KEY               0x1003
#define CPUFANIN_KEY                  0x1004
#define SYSFANIN_KEY                  0x1005
#define AUXFANIN0_KEY                 0x1006
#define AUXFANIN1_KEY                 0x1007
#define AUXFANIN2_KEY                 0x1008
#define VOLTAGE_3VCC_KEY              0x1009
#define VOLTAGE_AVCC_KEY              0x1010
#define VOLTAGE_CPUVCORE_KEY          0x1011
#define VOLTAGE_VIN0_KEY              0x1012
#define VOLTAGE_VIN1_KEY              0x1013
#define VOLTAGE_VIN2_KEY              0x1014
#define VOLTAGE_VIN3_KEY              0x1015
#define VOLTAGE_VBAT_KEY              0x1016
#define VOLTAGE_3VSB_KEY              0x1017
#define TSI_TEMP_KEY                  0x1018


#pragma pack(1)
typedef struct {
  UINT8   InteractiveText;
  UINT8   UartAEn;
  UINT8   UartBEn;
  UINT8   WtdEn;
  UINT8   WatchDogTime;
  UINT8   SysFanmode;
  UINT8   SysFanTempSource;
  UINT8   SysFanCriticalTemp;
  UINT8   SysFanTemp1;
  UINT8   SysFanDcPwm1;
  UINT8   SysFanTemp2;
  UINT8   SysFanDcPwm2;
  UINT8   SysFanTemp3;
  UINT8   SysFanDcPwm3;
  UINT8   SysFanTemp4;
  UINT8   SysFanDcPwm4;
  UINT8   CpuFanmode;
  UINT8   CpuFanTempSource;
  UINT8   CpuFanCriticalTemp;
  UINT8   CpuFanTemp1;
  UINT8   CpuFanDcPwm1;
  UINT8   CpuFanTemp2;
  UINT8   CpuFanDcPwm2;
  UINT8   CpuFanTemp3;
  UINT8   CpuFanDcPwm3;
  UINT8   CpuFanTemp4;
  UINT8   CpuFanDcPwm4;
  UINT8   Reserved;
} SIO_SETUP_CONFIG;
#pragma pack()

/*
#define INTERACTIVE_TEXT_REFRESH(HelpToken, CaptionToken, ValueToken,Key_Id) \
  grayoutif TRUE;\
      text\
      help   = STRING_TOKEN(STR_EMPTY),\
      text   = CaptionToken,\
      flags  = INTERACTIVE,\  
      key    = Key_Id;\
      refresh interval = 3 \
  endif;
*/


#define INTERACTIVE_ONEOF_REFRESH(HelpToken, CaptionToken, ValueToken, Key) \
  grayoutif TRUE;\
    oneof varid        = SIO_SETUP_CONFIG.InteractiveText, \
      questionid       = Key,                        \
      prompt           = CaptionToken,               \
      help             = HelpToken,                  \
      option text      = ValueToken, value = 0, flags = INTERACTIVE | DEFAULT;\
      refresh interval = 3 \
    endoneof;              \
  endif;


/*
#define INTERACTIVE_TEXT(CaptionToken, ValueToken) \
  grayoutif TRUE;\
      text\
      help   = STRING_TOKEN(STR_EMPTY),\
      text   = CaptionToken,\
      text   = ValueToken;\
  endif;
*/


#define SIO_SETUP_VARIABLE_NAME    L"SIO_SETUP_CONFIG"
#define SIO_SETUP_VARIABLE_FLAG    (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE)



extern EFI_GUID gSioSetupConfigGuid;

#endif      // __SIO_SETUP_DATA_STRUCT__
