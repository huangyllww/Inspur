#ifndef _RTC_INTR_DET_LIB_H_
#define _RTC_INTR_DET_LIB_H_


/**
  DumpRtcRam24 is to Dump Ram24 Data
  @param[out]  RtcRamData     pointer to ram24
  @retval      VOID
**/
VOID 
DumpRtcRam24 (
  IN RTCRAM_DATA     *RtcRamData
  );

VOID 
ReadRtcRam24 (
  IN OUT UINT8     *Byte
  );

#endif 
