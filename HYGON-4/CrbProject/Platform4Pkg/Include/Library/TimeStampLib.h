
#ifndef __TIME_STAMP_LIB_H__
#define __TIME_STAMP_LIB_H__

#include <Uefi.h>


typedef struct {
  UINT16 year;			  /* Year */
  UINT8  mon;			    /* Month.	[1-12] */
  UINT8  day;			    /* Day.		[1-31] */
  UINT8  hour;			  /* Hours.	[0-23] */
  UINT8  min;			    /* Minutes.	[0-59] */
  UINT8  sec;			    /* Seconds.	[0-59] */
} DATE_TIME_INFO;


EFI_STATUS
GetTimeStampLib (
  OUT UINT32   *NumOfSeconds
  );

void TimeStampToDateTime (UINT64 t, DATE_TIME_INFO *Info);

#endif

