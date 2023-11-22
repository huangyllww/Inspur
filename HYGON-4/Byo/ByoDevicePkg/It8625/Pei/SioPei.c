/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

By: 
  winddy_zhang

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/


#include "SioPei.h"
#include <Library/BaseLib.h>
#include <Uefi/UefiInternalFormRepresentation.h>
#include <Ite8625Reg.h>


EFI_SIO_TABLE mSioPeiTable[] = {

  {0x07, 0xFF, 0x04},
  {0x60, 0xFF, (UINT8)(_PCD_VALUE_PcdHwmIoBaseAddress >> 8)},
  {0x61, 0xFF, (UINT8)(_PCD_VALUE_PcdHwmIoBaseAddress & 0xFF)},
  {0x62, 0xFF, 0x02},
  {0x63, 0xFF, 0x30},
  {0x70, 0xFF, 0x09},
  {0xF0, 0xFF, 0x00},
  {0xF1, 0xFF, 0x00},
  {0xF2, 0xFF, 0x00},
  {0xF3, 0xFF, 0x00},
  {0xF4, 0xFF, 0x00},
  {0xF5, 0xFF, 0x00},
  {0xFA, 0xFF, 0x00},
  {0xFB, 0xF3, 0x0C},
  {0x30, 0xFF, 0x01},

  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_KBC_DEV_NUM },
  { R_SIO_BASE_ADDRESS_HIGH,        0xff, 0x00 },
  { R_SIO_BASE_ADDRESS_LOW,         0xff, 0x60 },
  { R_SIO_BASE_ADDRESS_HIGH2,       0xff, 0x00 },
  { R_SIO_BASE_ADDRESS_LOW2,        0xff, 0x64 },
  { R_SIO_PRIMARY_INTERRUPT_SELECT, 0xff, 0x01 },
  { R_KBC_KB_SPECIAL_CFG,           0xff, 0x48 },
  { R_SIO_ACTIVATE,                 0xff, DEVICE_ENABLE },


  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_MS_DEV_NUM },
  { R_SIO_BASE_ADDRESS_HIGH,        0xff, 0x00 },
  { R_SIO_BASE_ADDRESS_LOW,         0xff, 0x60 },
  { R_SIO_BASE_ADDRESS_HIGH2,       0xff, 0x00 },
  { R_SIO_BASE_ADDRESS_LOW2,        0xff, 0x64 },
  { R_SIO_PRIMARY_INTERRUPT_SELECT, 0xff, 0x0C },
  { 0x71,                           0xFF, 0x02 },
  { R_KBC_MS_SPECIAL_CFG,           0xff, 0x00 },
  { R_SIO_ACTIVATE,                 0xff, DEVICE_ENABLE },



  { R_SIO_LOGICAL_DEVICE,    0xff, SIO_FDC_DEV_NUM },
  { R_SIO_BASE_ADDRESS_HIGH, 0xff, 0x00 },
  { R_SIO_BASE_ADDRESS_LOW,  0xff, 0x00 },
  { R_SIO_ACTIVATE,          0xff, DEVICE_DISABLE },

  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_UART_A_DEV_NUM },
  { R_SIO_BASE_ADDRESS_HIGH,        0xff, 0x03 },
  { R_SIO_BASE_ADDRESS_LOW,         0xff, 0xf8 },
  { R_SIO_PRIMARY_INTERRUPT_SELECT, 0xff, 0x04 },
  { R_SIO_ACTIVATE,                 0xff, DEVICE_ENABLE },

  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_UART_B_DEV_NUM },
  { R_SIO_BASE_ADDRESS_HIGH,        0xff, 0x00 },
  { R_SIO_BASE_ADDRESS_LOW,         0xff, 0x00 },
  { R_SIO_PRIMARY_INTERRUPT_SELECT, 0xff, 0x00 },
  { R_SIO_ACTIVATE,                 0xff, DEVICE_DISABLE },

  { R_SIO_LOGICAL_DEVICE,           0xff, SIO_CIR_DEV_NUM },
  { R_SIO_ACTIVATE,                 0xff, DEVICE_DISABLE },
};



EFI_SIO_TABLE mSioGpioTable[] = {
  {0x07, 0xFF, 0x07},
};



// TMPIN Source Selection 1 (TSS1)
// TMPIN Source Selection 2 (TSS2)
EFI_SIO_TABLE mHwmInitTable[] = {
  {R_SMI_MASK3,          0xFF, 0x00},  // bank 0
  {R_ADC_VCER,           0xFF, 0x11},  // TMPIN_1 Diode mode; TMPIN_2 Resistor Mode
  
  {R_SMI_MASK3,          0xFF, 0x40},  // bank 2
  {0x10,                 0xFF, 0x49},  // Fan 1 A smart limit
  {0x11,                 0xFF, 0x96},  // Fan 1 A slope value
  {0x14,                 0xFF, 0x53},  // Fan 1 B smart limit
  {0x15,                 0xFF, 0x07},  // Fan 1 B slope value 
  {0x20,                 0xFF, 0x49},  // Fan 2 A smart limit
  {0x21,                 0xFF, 0x96},  // Fan 2 A slope value  
  {0x1D,                 0xFF, 0x10},  // TMPIN1(TD1), TMPIN2(TR2)
  {0x1E,                 0xFF, 0x02},  // TMPIN3(reserved)
  
  { R_SMI_MASK3,                    0xFF,0x00 },  // bank 0
  { R_CFG,                          0xFF,0x19 },  // start
  { R_IF_SELECTION,                 0xFF,0x44 },
  { R_FAN_PWM_SSF_SELECTION,        0xFF,0x00 },
  { R_FAN_TAC_CONTROL,              0xFF,0xC8 },  // TMPIN[3:1] Enhanced Interrupt Mode Enable
  { R_FAN_CTL_MAIN_CONTROL,         0xFF,0x70 },  // FAN_TAC3-1 Enable 
  { R_FAN_CTL_CONTROL,              0xFF,0xC0 },  // FAN_CTL Polarity active high
  { R_FAN_CTL1_PWM_CONTROL,         0xFF,0x80 },  // fan 1 Automatic, TMPIN1, Tachometer Closed-loop Mode Enable
  { R_FAN_CTL2_PWM_CONTROL,         0xFF,0x80 },  // fan 2 TMPIN1 (29h)
  { R_FAN_CTL3_PWM_CONTROL,         0xFF,0x94 },  // fan 3
  { R_ADC_EVS,                      0xFF,0xFF },  // ADC VIN7-0 Scan Enable
  { R_ADC_TECER,                    0xFF,0x40 },  // default

  
//Fan 1
  { R_FAN1_AM_TMP_LIMIT_OFF,        0xFF,0x0A },  // Temperature Limit Value of Fan OFF
  { R_FAN1_AM_TMP_LIMIT_START,      0xFF,0x0F },  // SmartGuardian Automatic Mode Temperature Limit of Fan Start
  { R_FAN1_AM_TMP_LIMIT_FULL,       0xFF,0x32 },  // SmartGuardian Automatic Mode Temperature Limit of Full Speed
  { R_FAN1_AM_START_PWM,            0xFF,0x33 },  // SmartGuardian Automatic Mode Start PWM Registers
  { R_FAN1_AM_CONTROL,              0xFF,0x2F },  // FAN Smoothing, Slope PWM 
  { R_FAN1_AM_DELTA_TMP,            0xFF,0x40 },  // SmartGuardian Automatic Mode delta-Temperature Registers

//Fan 2
  { R_FAN2_AM_TMP_LIMIT_OFF,        0xFF,0x0A },
  { R_FAN2_AM_TMP_LIMIT_START,      0xFF,0x0F },
  { R_FAN2_AM_TMP_LIMIT_FULL,       0xFF,0x32 },
  { R_FAN2_AM_START_PWM,            0xFF,0x33 },
  { R_FAN2_AM_CONTROL,              0xFF,0x2F },
  { R_FAN2_AM_DELTA_TMP,            0xFF,0x40 },

//Fan 3
  { R_FAN3_AM_TMP_LIMIT_OFF,        0xFF,0x00 },
  { R_FAN3_AM_TMP_LIMIT_START,      0xFF,0x37 },
  { R_FAN3_AM_TMP_LIMIT_FULL,       0xFF,0x5F },
  { R_FAN3_AM_START_PWM,            0xFF,0x4C },
  { R_FAN3_AM_CONTROL,              0xFF,0xD0 },
  { R_FAN3_AM_DELTA_TMP,            0xFF,0x02 }, 

  {R_TMPIN1_HIGH_LIMIT,             0xFF, 0x55},
  {R_TMPIN1_LOW_LIMIT,              0xFF, 0x0A},
  {R_TMPIN2_HIGH_LIMIT,             0xFF, 0x55},
  {R_TMPIN2_LOW_LIMIT,              0xFF, 0x0A},
  {R_TMPIN3_HIGH_LIMIT,             0xFF, 0xDE},
  {R_TMPIN3_LOW_LIMIT,              0xFF, 0x0C},
  {R_TMPIN4_HIGH_LIMIT,             0xFF, 0x59},
  {R_TMPIN4_LOW_LIMIT,              0xFF, 0x0B},
  {R_TMPIN5_HIGH_LIMIT,             0xFF, 0x9C},
  {R_TMPIN5_LOW_LIMIT,              0xFF, 0xE0},
  {R_TMPIN6_HIGH_LIMIT,             0xFF, 0x58},
  {R_TMPIN6_LOW_LIMIT,              0xFF, 0x0C},

  {R_VIN0_HIGH_LIMIT,               0xFF, 0x6F},
  {R_VIN0_LOW_LIMIT,                0xFF, 0xDB}, 
  {R_VIN1_HIGH_LIMIT,               0xFF, 0x50},
  {R_VIN1_LOW_LIMIT,                0xFF, 0xBF}, 
  {R_VIN2_HIGH_LIMIT,               0xFF, 0xFB},
  {R_VIN2_LOW_LIMIT,                0xFF, 0xBC}, 
  {R_VIN3_HIGH_LIMIT,               0xFF, 0x8D},
  {R_VIN3_LOW_LIMIT,                0xFF, 0xC0}, 
  {R_VIN4_HIGH_LIMIT,               0xFF, 0x2F},
  {R_VIN4_LOW_LIMIT,                0xFF, 0x24}, 
  {R_VIN5_HIGH_LIMIT,               0xFF, 0x79},
  {R_VIN5_LOW_LIMIT,                0xFF, 0x2F}, 
  {R_VIN6_HIGH_LIMIT,               0xFF, 0xC5},
  {R_VIN6_LOW_LIMIT,                0xFF, 0x1B}, 
  {R_I3VSB_HIGH_LIMIT,              0xFF, 0x46},
  {R_I3VSB_LOW_LIMIT,               0xFF, 0x53}, 


// Offset Adjustment For THERMD[+|-],SIO need to decrease 10 degree
  {BEEP_EVT_ENABLE,                 0x00, BIT7},        // 0x56 writable
  {R_TMPIN1_ZERO_DEGREE_ADJUST,     0xFF, 0xF6}, 

//{0xB0,                            0xFF, 0x91},
//{0xB1,                            0xFF, 0x4B},
//{0x9D,                            0xFF, 0x04},
};





EFI_STATUS SioEarlyInitialize(VOID)
{
  UINT8    Index;  
    
  EnterSioCfgMode();
  for (Index = 0; Index < ARRAY_SIZE(mSioPeiTable); Index++) {
    SioRegAndThenOr(
      mSioPeiTable[Index].Register, 
      (UINT8)~mSioPeiTable[Index].DataMask, 
      mSioPeiTable[Index].Value
      );
  }
  ExitSioCfgMode();  
  
  return EFI_SUCCESS;
}


void HwmInit()
{
  UINT8    Index;  

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  for (Index = 0; Index < ARRAY_SIZE(mHwmInitTable); Index++) {
    HwmRegAndThenOr(
      mHwmInitTable[Index].Register, 
      (UINT8)~mHwmInitTable[Index].DataMask, 
      mHwmInitTable[Index].Value
      );
  }
}





VOID SetGpioDefault()
{
  UINT8    Index;  
    
  EnterSioCfgMode();
  for (Index = 0; Index < ARRAY_SIZE(mSioGpioTable); Index++) {
    SioRegAndThenOr(
      mSioGpioTable[Index].Register, 
      (UINT8)~mSioGpioTable[Index].DataMask, 
      mSioGpioTable[Index].Value
      );
  }
  ExitSioCfgMode();

//LibSetPowerLed(LED_STATUS_ON);
}



EFI_STATUS
SioPeiEntry (
  IN EFI_PEI_FILE_HANDLE           FileHandle,
  IN CONST EFI_PEI_SERVICES        **PeiServices
  )
{
  DEBUG((DEBUG_INFO, "%a\n", __FUNCTION__));
//DEBUG((DEBUG_INFO, "chipid:%X\n", GetSioChipId()));
  
  SetGpioDefault();
  SioEarlyInitialize();
  HwmInit();  

  return EFI_SUCCESS;
}

