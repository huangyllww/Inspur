/*++
	Copyright (c) 2006-2019 Byosoft Co.,Ltd.
	This program contains proprietary and confidential information.
	All rights reserved except as may be permitted by prior written
	consent.

Module Name:

    HardwareMonitor.h
    
Abstract:

    This is Ite SIO chip Temperature protocol for platform.

Revision History
;	----------------------------------------------------------------------------
;	Rev		Date	 			Description
;	----------------------------------------------------------------------------
;	
--*/

#ifndef _SIO_HWM_PROTOCOL_H_
#define _SIO_HWM_PROTOCOL_H_

#define EFI_SIO_HWM_PROTOCOL_GUID \
       {0x26A593DD, 0x5C7C, 0x4905, 0x9A, 0xD4, 0x52, 0x6D, 0x4A, 0xC9, 0x10, 0x7D}


typedef struct _EFI_SIO_HWM_PROTOCOL EFI_SIO_HWM_PROTOCOL;

typedef enum {
  SENSOR_CPU_FAN,
  SENSOR_SYS_FAN,
  SENSOR_TEMPIN1,
  SENSOR_TEMPIN2,
  SENSOR_TEMPIN3, 
  SENSOR_VIN0,          // unit in 0.1 mv
  SENSOR_VIN1,          // unit in 0.1 mv
  SENSOR_VIN2,          // unit in 0.1 mv
  SENSOR_VIN3,          // unit in 0.1 mv
  SENSOR_VIN4,          // unit in 0.1 mv
  SENSOR_VIN5,          // unit in 0.1 mv
  SENSOR_VIN6,          // unit in 0.1 mv  
  SENSOR_VBAT,          // unit in 0.1 mv
} SENSOR_SELECT;



typedef
EFI_STATUS
(EFIAPI *GET_AND_PARSE_SENSOR_DATA) (
  IN  SENSOR_SELECT      Type,
  OUT UINTN              *Value
  );

struct _EFI_SIO_HWM_PROTOCOL {
	GET_AND_PARSE_SENSOR_DATA GetSensorData;
};


extern EFI_GUID gEfiSioHwmProtocolGuid;

#endif

