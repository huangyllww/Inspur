

#include <Library/TimeStampLib.h>
#include <Library/BaseLib.h>

#define	SECS_PER_HOUR	(60 * 60)
#define	SECS_PER_DAY	(SECS_PER_HOUR * 24)
#define LEAPS_THRU_END_OF(y) ((y)/4 - (y)/100 + (y)/400)
#define __isleap(year)	((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))

static const UINT16 __mon_yday[2][13] = {
  /* Normal years.  */
  { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
  /* Leap years.  */
  { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

void TimeStampToDateTime (UINT64 t, DATE_TIME_INFO *Info)
{
  UINT32       y2, y, remain;
  INT32        days;
  CONST UINT16 *MonthDays;
  

  days = (INT32)DivU64x32Remainder(t, SECS_PER_DAY, &remain);
  Info->hour = (UINT8)(remain / SECS_PER_HOUR);
  remain = remain % SECS_PER_HOUR;
  Info->min = (UINT8)(remain / 60);
  Info->sec = (UINT8)(remain % 60);

  y = 1970;
  while(days < 0 || days >= (__isleap(y) ? 366 : 365)) {
    y2 = y + days / 365 - (days % 365 < 0);
    days -= (y2 - y) * 365 + LEAPS_THRU_END_OF(y2 - 1) - LEAPS_THRU_END_OF(y - 1);
    y = y2;
  }
  Info->year = (UINT16)y;

  MonthDays = __mon_yday[__isleap(Info->year)];
  for (y = 11; days < MonthDays[y]; y--){
    continue;
  }
  Info->mon = (UINT8)y+1;
  days -= MonthDays[y];
  Info->day = (UINT8)(days + 1);  
}