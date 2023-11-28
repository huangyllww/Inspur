/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  CrbSmmPlatform.c

Abstract:
  Source file for the Smm platform driver.

Revision History:

**/

#include "ProjectSmm.h"
#include <RtcDef.h>


//                              1   2   3
static UINT8 gDayOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

VOID CalcNewDate(UINT16 Year, UINT8 *Month, UINT8 *Day, UINT8 WeekdayAdd)
{
  UINT8  MyDay;
  UINT8  MyMonth;

  if(IsLeapYear(Year)){gDayOfMonth[2-1] = 29;}  
  
  MyDay   = *Day + WeekdayAdd;
  MyMonth = *Month;
  if(MyDay > gDayOfMonth[*Month-1]){
    MyDay -= gDayOfMonth[*Month-1];
    MyMonth++;
  }
  if(MyMonth > 12){
    MyMonth -= 12;
  }

  *Month = MyMonth;
  *Day   = MyDay;
}

UINT8 GetNearestWeekdayIntervalNeeded(UINT8 CurWeekday, BOOLEAN TodayValid)
{
  UINT8  Index;
  UINT8  NewWeekDay;
  UINT8  WeekDayBitMap;

  WeekDayBitMap = (gSetupData.UserDefMon?BIT0:0)|
                  (gSetupData.UserDefTue?BIT1:0)|
                  (gSetupData.UserDefWed?BIT2:0)|
                  (gSetupData.UserDefThu?BIT3:0)|
                  (gSetupData.UserDefFri?BIT4:0)|
                  (gSetupData.UserDefSat?BIT5:0)|
                  (gSetupData.UserDefSun?BIT6:0);  

  DEBUG((EFI_D_INFO, "CurWeekday:%d TodayValid:%d WeekDayBitMap:%X\n", CurWeekday, TodayValid, WeekDayBitMap));
  
  Index = 0;
  if(!TodayValid){Index=1;}
  for(; Index<=7; Index++){
    NewWeekDay = (CurWeekday+Index)%7;
    if((1<<NewWeekDay) & WeekDayBitMap){
      return Index;
    }
  }
  
  return 0xFF;
}




EFI_STATUS EFIAPI GetCurrentTime(EFI_TIME *Time)
{
  EFI_STATUS            Status;
  RTC_REGISTER_B        RegisterB;
  UINT8                 Cmos0B;


  if(Time == NULL){
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }

  ZeroMem(Time, sizeof(EFI_TIME));

  Status = RtcWaitToUpdate();
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "(L%d)%r\n", __LINE__, Status));  
    goto ProcExit;
  }  
 
  RegisterB.Data = CmosRead(RTC_ADDRESS_REGISTER_B);
  Cmos0B = RegisterB.Data;
  RegisterB.Bits.Set = 1;   // Updates inhibited
  RegisterB.Bits.Aie = 0;  
  RegisterB.Bits.Mil = 1;		// 24 hours
  RegisterB.Bits.Dm  = 0;		// BCD Format
  CmosWrite(RTC_ADDRESS_REGISTER_B, RegisterB.Data);

// Now we could safely read/write register 0 ~ 9
  Time->Day    = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_DAY_OF_THE_MONTH));
  Time->Month  = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_MONTH));
  Time->Year   = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_YEAR)) + 2000;		// assume 20 Century.
  Time->Hour   = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_HOURS));
  Time->Minute = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_MINUTES));
  Time->Second = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_SECONDS));

  CmosWrite(RTC_ADDRESS_REGISTER_B, Cmos0B);

ProcExit:
  return Status;
}



EFI_STATUS EFIAPI SetCurrentTime(EFI_TIME *Time, UINTN SetFlag)
{
  EFI_STATUS            Status;
  RTC_REGISTER_B        RegisterB;
  UINT8                 Cmos0B;
  BOOLEAN               Cmos0BChanged;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status        = EFI_SUCCESS;	
  Cmos0BChanged = FALSE;
  Cmos0B        = 2;

  if(SetFlag == SET_TIME_FLAG_ALL || SetFlag == SET_TIME_FLAG_TIME){
    if(Time->Second > 59 || Time->Minute > 59 || Time->Hour > 23){
      Status = EFI_INVALID_PARAMETER;
      DEBUG((EFI_D_ERROR, "(L%d)%r\n", __LINE__, Status));    
      goto ProcExit;
    }
  }

  if(SetFlag == SET_TIME_FLAG_ALL || SetFlag == SET_TIME_FLAG_DATE){
    if(Time->Year < 2019 || Time->Month > 12 || Time->Day > 31){
      Status = EFI_INVALID_PARAMETER;
      DEBUG((EFI_D_ERROR, "(L%d)%r\n", __LINE__, Status, Time->Year, Time->Month, Time->Day));    
      goto ProcExit;
    } 
  }
  
  Status = RtcWaitToUpdate();
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "(L%d)%r\n", __LINE__, Status));  
    goto ProcExit;
  }  
 
  RegisterB.Data = CmosRead(RTC_ADDRESS_REGISTER_B);
  Cmos0B = RegisterB.Data;
  RegisterB.Bits.Set = 1;   // Updates inhibited
  RegisterB.Bits.Aie = 0;  
  RegisterB.Bits.Mil = 1;		// 24 hours
  RegisterB.Bits.Dm  = 0;		// BCD Format
  CmosWrite(RTC_ADDRESS_REGISTER_B, RegisterB.Data);
  Cmos0BChanged = TRUE;

// Now we could safely read/write register 0 ~ 9
  if(SetFlag == SET_TIME_FLAG_ALL || SetFlag == SET_TIME_FLAG_DATE){
    CmosWrite(RTC_ADDRESS_YEAR,             DecimalToBcd8((UINT8)(Time->Year - 2000)));
    CmosWrite(RTC_ADDRESS_MONTH,            DecimalToBcd8(Time->Month));
    CmosWrite(RTC_ADDRESS_DAY_OF_THE_MONTH, DecimalToBcd8(Time->Day)); 
  }
  if(SetFlag == SET_TIME_FLAG_ALL || SetFlag == SET_TIME_FLAG_TIME){
    CmosWrite(RTC_ADDRESS_HOURS,   DecimalToBcd8(Time->Hour));
    CmosWrite(RTC_ADDRESS_MINUTES, DecimalToBcd8(Time->Minute));
    CmosWrite(RTC_ADDRESS_SECONDS, DecimalToBcd8(Time->Second)); 
  }  
  
  CmosWrite(RTC_ADDRESS_REGISTER_B, Cmos0B);
  
ProcExit:
  return Status;
}





STATIC VOID ShowRtcSetting()
{
  RTC_REGISTER_B        RegisterB;
  UINT16                CurYear;
  UINT8                 CurMonth;
  UINT8                 CurDay;
  UINT8                 CurHour;
  UINT8                 CurMinute;
  UINT8                 CurSecond;
//UINT8                 WakeMonth;
  UINT8                 WakeDay;
  UINT8                 WakeHour;
  UINT8                 WakeMinute;
  UINT8                 WakeSecond;

  
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  RegisterB.Data = CmosRead(RTC_ADDRESS_REGISTER_B);

  DEBUG((EFI_D_INFO, "CmosB:%X\n", RegisterB.Data)); 
  DEBUG((EFI_D_INFO, "[800]:%08X\n", IoRead32(mAcpiBaseAddr + PMIO_STS_REG))); 

  CurDay     = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_DAY_OF_THE_MONTH));
  CurMonth   = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_MONTH));
  CurYear    = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_YEAR)) + 2000;		// assume 20 Century.
  CurHour    = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_HOURS));
  CurMinute  = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_MINUTES));
  CurSecond  = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_SECONDS));
  DEBUG((EFI_D_INFO, "Current: %04d-%02d-%02d %02d:%02d:%02d\n",  \
                      CurYear, CurMonth, CurDay,             \
                      CurHour, CurMinute, CurSecond         \
                      ));

  WakeDay     = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_REGISTER_D) & 0x3F);
  WakeHour    = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_HOURS_ALARM));
  WakeMinute  = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_MINUTES_ALARM));
  WakeSecond  = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_SECONDS_ALARM));
  DEBUG((EFI_D_INFO, "week: %02d %02d:%02d:%02d\n",  \
                      WakeDay,             \
                      WakeHour, WakeMinute, WakeSecond  \
                      ));

}



EFI_STATUS ProjectEnableS5RtcWake()
{
  EFI_STATUS            Status;
  RTC_REGISTER_B        RegisterB;
  RTC_REGISTER_D        RegisterD;  
  UINT8                 Second;
  UINT8                 Minute;  
  UINT8                 Hour;
  UINT8                 Month;
  UINT8                 Day;
  UINT16                CurYear;
  UINT8                 CurMonth;
  UINT8                 CurDay;
  UINT8                 CurWeekDay;
  UINT8                 WeekDayAdd;
  UINT8                 Cmos0B;
  BOOLEAN               Cmos0BChanged;
  UINT8                 CurHour;
  UINT8                 CurMinute;
  UINT8                 CurSecond;
  BOOLEAN               TodayValid;
  SETUP_DATA            *SetupData;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  SetupData = &gSetupData;
  if(!(SetupData->UserDefMon || SetupData->UserDefTue || SetupData->UserDefWed || 
     SetupData->UserDefThu || SetupData->UserDefFri || SetupData->UserDefSat || 
     SetupData->UserDefSun)){
    return EFI_ABORTED;
  }

  ShowRtcSetting();

  Status        = EFI_SUCCESS;	
  Cmos0BChanged = FALSE;
  Cmos0B        = 2;
	
  Second   = SetupData->RTCWakeupTime.Second;
  Minute   = SetupData->RTCWakeupTime.Minute;
  Hour     = SetupData->RTCWakeupTime.Hour;
  DEBUG((EFI_D_INFO, "%02d:%02d:%02d\n", Hour, Minute, Second));
	
  if(Second > 59 || Minute > 59 || Hour > 23){
    Status = EFI_INVALID_PARAMETER;
    DEBUG((EFI_D_ERROR, "(L%d)%r\n", __LINE__, Status));    
    goto ProcExit;
  }
  Status = RtcWaitToUpdate();
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "(L%d)%r\n", __LINE__, Status));  
    goto ProcExit;
  }  
 
  RegisterB.Data = CmosRead(RTC_ADDRESS_REGISTER_B);
  Cmos0B = RegisterB.Data;
  RegisterB.Bits.Set = 1;   // Updates inhibited
  RegisterB.Bits.Aie = 0;  
  RegisterB.Bits.Mil = 1;		// 24 hours
  RegisterB.Bits.Dm  = 0;		// BCD Format
  CmosWrite(RTC_ADDRESS_REGISTER_B, RegisterB.Data);
  Cmos0BChanged = TRUE;

// Now we could safely read/write register 0 ~ 9
  CurDay     = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_DAY_OF_THE_MONTH));
  CurMonth   = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_MONTH));
  CurYear    = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_YEAR)) + 2000;		// assume 20 Century.
  CurWeekDay = CaculateWeekDay(CurYear, CurMonth, CurDay);
  CurHour    = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_HOURS));
  CurMinute  = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_MINUTES));
  CurSecond  = CheckAndConvertBcd8ToDecimal8(CmosRead(RTC_ADDRESS_SECONDS));

  DEBUG((EFI_D_INFO, "Current: %04d-%02d-%02d %02d:%02d:%02d %d\n",  \
                      CurYear, CurMonth, CurDay,             \
                      CurHour, CurMinute, CurSecond,         \
                      CurWeekDay                             \
                      ));
  if((CurHour*3600 + CurMinute*60 + CurSecond) >= (Hour*3600 + Minute*60 + Second)){
    TodayValid = FALSE;
  } else {
    TodayValid = TRUE;
  }
  WeekDayAdd = GetNearestWeekdayIntervalNeeded(CurWeekDay, TodayValid);
  if(WeekDayAdd == 0xFF){
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }
  Month = CurMonth;
  Day   = CurDay;
  CalcNewDate(CurYear, &Month, &Day, WeekDayAdd);
  DEBUG((EFI_D_INFO, "[RTC] %02d-%02d %02d:%02d:%02d\n", Month, Day, Hour, Minute, Second));    
  CmosWrite(RTC_ADDRESS_SECONDS_ALARM, DecimalToBcd8(Second));
  CmosWrite(RTC_ADDRESS_MINUTES_ALARM, DecimalToBcd8(Minute));
  CmosWrite(RTC_ADDRESS_HOURS_ALARM,   DecimalToBcd8(Hour)); 
  RegisterD.Data = CmosRead(RTC_ADDRESS_REGISTER_D);
  RegisterD.Bits.DateAlarm = DecimalToBcd8(Day);
  CmosWrite(RTC_ADDRESS_REGISTER_D, RegisterD.Data);

  CmosRead(RTC_ADDRESS_REGISTER_C);  // Read 0xC to clear pending RTC interrupts  

  MmioOr32(0xFED80300 + 0x74, BIT2);   // RtcEnEn
  
  RegisterB.Bits.Aie = 1;
  RegisterB.Bits.Set = 0;
  CmosWrite(RTC_ADDRESS_REGISTER_B, RegisterB.Data);
  IoWrite16(mAcpiBaseAddr + PMIO_STS_REG, PMIO_STS_RTC);
  IoOr16(mAcpiBaseAddr + PMIO_PM_EN, PMIO_PM_EN_RTC);

  Status = EFI_SUCCESS;
  
ProcExit:
  if(EFI_ERROR(Status) && Cmos0BChanged){
    CmosWrite(RTC_ADDRESS_REGISTER_B, Cmos0B);
  }	
  return Status;
}


