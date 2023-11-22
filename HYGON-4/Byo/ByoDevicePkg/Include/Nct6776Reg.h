
/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef _NCT6776REG_H
#define _NCT6776REG_H



#define NCT6796_CHIP_ID                0xD423
#define NCT6776_CHIP_ID                0xC333
#define NCT6686_CHIP_ID                0xD441
#define NCT6126_CHIP_ID                0xD283

#define RESOURCE_IO                    BIT0
#define RESOURCE_IRQ                   BIT1
#define RESOURCE_DMA                   BIT2
#define RESOURCE_MEM                   BIT3

#define SIO_LOGICAL_DEVICE_DISABLE     0x0
#define SIO_LOGICAL_DEVICE_ENABLE      0x1


//
// Define some fixed platform device location information
//
#define SIO_CFG_PORT_2E                0x2E
#define SIO_DATA_PORT_2F               0x2F

#define SIO_CFG_PORT_4E                0x4E
#define SIO_CFG_BANK_SEL                0x4E

                                       
#define DEVICE_ENABLE                  0x01
#define DEVICE_DISABLE                 0x00
                                       
//                                     
// Port address for LPC                
//                                     
//#define SIO_INDEX_PORT                  LPC_SIO_INDEX_DEFAULT_PORT
//#define SIO_DATA_PORT                   LPC_SIO_DATA_DEFAULT_PORT
                                        
#define SIO_HW_INDEX_PORT               0x295
#define SIO_HW_DATA_PORT                0x296
#define IA32_TEMPERATURE_TARGET         0x1A2
                                        
//                                      
// Logical Device          
//                                      
#define SIO_FDC_DEV_NUM                 0x00
#define SIO_UART_A_DEV_NUM              0x02
#define SIO_UART_B_DEV_NUM              0x03
#define SIO_PARALLEL_DEV_NUM            0x01
#define SIO_HW_MONITOR_DEV_NUM          0x0B
#define SIO_KBC_DEV_NUM                 0x05
//#define SIO_MS_DEV_NUM                  0x06
#define SIO_GPIO_DEV_NUM                0x07
#define SIO_CIR_DEV_NUM                 0x06
#define SIO_ACPI_DEV_NUM                0x0A
#define SIO_HM_SBTSI_DEV_NUM            0x0B
#define SIO_EC_SPACE                    0x0B

                                        
//                                      
// Global Registers        
//                                      
#define R_SIO_LOGICAL_DEVICE            0x07
#define R_SIO_IRQ_TYPE_SEL              0x10
#define R_SIO_IRQ_TYPE_SEL2             0x11
#define R_SIO_MULTI_FUNC_SELECT         0x1C
#define R_SIO_IRQ_EDGE                  0x00
#define R_SIO_IRQ_LEVEL                 0x01

#define R_SIO_IRQ_POLARITY_SEL          0x13
#define R_SIO_IRQ_POLARITY_SEL2         0x14
  #define R_SIO_IRQ_POLARITY_HIGH       0x00
  #define R_SIO_IRQ_POLARITY_LOW        0x01  
#define R_SIO_IRQ_CONTROL_BIT           0x1D  
#define R_DEVICE_ID0                    0x20
#define R_DEVICE_ID1                    0x21
#define R_REVISION_ID                   0x22
#define R_SIO_CLK_CFG                   0x23
#define R_SIO_GLOBAL_OPTION             0x24
#define R_SIO_GPIO_SET_1                0x25
#define R_SIO_GPIO_SET_2                0x26
#define R_SIO_GPIO_SET_3                0x27
#define R_SIO_GPIO_SET_4                0x28
#define R_SIO_GPIO_SET_5                0x29
#define R_EXT_MULTI_FUNC_SEL            0x2A

//
//Below Global registers can be read from any LDN,but can only be written when LDN=7
//
#define R_SIO_SPECIAL_SET1              0x2A
#define R_SIO_SPECIAL_SET2              0x2B
#define R_SIO_SPECIAL_SET3              0x2C

#define R_SIO_ACTIVATE                  0x30
#define R_SIO_BASE_ADDRESS_HIGH         0x60
#define R_SIO_BASE_ADDRESS_LOW          0x61
#define R_SIO_BASE_ADDRESS_HIGH2        0x62
#define R_SIO_BASE_ADDRESS_LOW2         0x63
#define R_SIO_PRIMARY_INTERRUPT_SELECT  0x70
#define R_SIO_SECOND_INTERRUPT_SELECT   0x72
#define R_SIO_DMA_CHANNEL_SELECT        0x74

//
//LDN=3 Parallel Port
//
#define R_SIO_PARALLEL_PORT_SPECIAL     0xF0

//
//LDN=4
//
#define R_SIO_PER                       0xF0
  #define R_SIO_PER_PS2K_EN             0x8
  #define R_SIO_PER_PS2M_EN             0x10
#define R_SPECIAL_CFG_1                 0xFA
  #define RING_2_WAKE_EN                  BIT3
  #define RING_1_WAKE_EN                  BIT2
  #define SB5V_CTRL_EN                    BIT1
#define R_SPECIAL_CFG_2                 0xFB
  #define RING_2_WAKE_STATUS            BIT3
  #define RING_1_WAKE_STATUS            BIT2
//
//LDN=8
//
#define R_SIO_GPIO0_IO_REG              0xE0
#define R_SIO_GPIO0_MULTI_FUNC          0xE4
#define R_SIO_WDT1_COUNTER_REG          0xF6
#define R_SIO_WDT1_CTRL_STATUS_REG      0xF7

//
//LDN=B
//
#define R_SIO_TSI_BASE_ADDR_MSB        0x62
#define R_SIO_TSI_BASE_ADDR_LSB        0x63
//
//Environment Controller Registers LDN=B
//

#define R_CFG                           0x0
#define R_INT1_STATUS                   0x1
#define R_INT2_STATUS                   0x2
#define R_INT3_STATUS                   0x3

#define R_SMI_MASK1                     0x4
#define R_SMI_MASK2                     0x5
#define R_SMI_MASK3                     0x6

#define R_INT_MASK1                     0x7
#define R_INT_MASK2                     0x8
#define R_INT_MASK3                     0x9

#define R_IF_SELECTION                  0xA
  #define EXT_THERMAL_SENSOR            0x40
  #define SST_PECI_SEL                  0x20
  #define SST_PECI_CLK_SEL              0x0
  #define SST_PECI_t_BIT                0x2
  #define PECI_EN                       0x0
#define R_FAN_PWM_SSF_SELECTION         0xB
#define R_FAN_TAC_CONTROL               0xC

#define R_FAN_TAC1_READING              0xD
#define R_FAN_TAC2_READING              0xE
#define R_FAN_TAC3_READING              0xF
#define R_FAN_TAC1_LIMIT                0x10
#define R_FAN_TAC2_LIMIT                0x11
#define R_FAN_TAC3_LIMIT                0x12

#define R_FAN_CTL_MAIN_CONTROL          0x13
#define R_FAN_CTL_CONTROL               0x14

#define R_FAN_CTL1_PWM_CONTROL          0x15
#define R_FAN_CTL2_PWM_CONTROL          0x16
#define R_FAN_CTL3_PWM_CONTROL          0x17

#define R_FAN_TAC1_EXT_READING          0x18
#define R_FAN_TAC2_EXT_READING          0x19
#define R_FAN_TAC3_EXT_READING          0x1A

#define R_FAN_TAC1_EXT_LIMIT            0x1B
#define R_FAN_TAC2_EXT_LIMIT            0x1C
#define R_FAN_TAC3_EXT_LIMIT            0x1D

#define R_VIN0_READING                  0x20
#define R_VIN1_READING                  0x21
#define R_VIN2_READING                  0x22
#define R_VIN3_READING                  0x23
#define R_VIN4_READING                  0x24
#define R_VIN5_READING                  0x25
#define R_VIN6_READING                  0x26
#define R_VBAT_READING                  0x28

#define R_TMPIN1_READING                0x29
#define R_TMPIN2_READING                0x2A
#define R_TMPIN3_READING                0x2B

#define R_ADC_EVS                       0x50
#define R_ADC_VCER                      0x51
#define R_ADC_TCER                      0x52
#define R_ADC_TECER                     0x55
#define ADJUST_REGISTER_3               0x59
#define BEEP_EVT_ENABLE                 0x5C

#define BANK_CFG_CPU_FAN                0x02
#define BANK_CFG_SYS_FAN                0x01


#define FAN_MODE_SEL_MASK               0x0F
#define FAN_TOLERANCE_MASK              0xF8
#define FAN_TEMP_SOURC_SEL_MASK         0xE0

#define R_FAN_MODE_SEL                  0x02
#define R_FAN_TEMP_SOURCE_SEL           0x00
#define R_FAN_PWM_OUT                   0x09
#define FAN_MODE_MANUL                  0x00
#define FAN_MODE_SMART                  0x40


#define R_SMARTFAN_CRITICAL_T           0x35
#define R_SMARTFAN_CRITICAL_TOLERANCE   0x38

#define R_FANOUT_STEP                0x20
#define R_SMARTFAN_T1                0x21
#define R_SMARTFAN_T2                0x22
#define R_SMARTFAN_T3                0x23
#define R_SMARTFAN_T4                0x24

#define R_SMARTFAN_PWM1              0x27
#define R_SMARTFAN_PWM2              0x28
#define R_SMARTFAN_PWM3              0x29
#define R_SMARTFAN_PWM4              0x2A


#define R_FAN1_AM_TMP_LIMIT_OFF         0x60
#define R_FAN1_AM_TMP_LIMIT_START       0x61
#define R_FAN1_AM_TMP_LIMIT_FULL        0x62
#define R_FAN1_AM_START_PWM             0x63
#define R_FAN1_AM_CONTROL               0x64
#define R_FAN1_AM_DELTA_TMP             0x65
#define R_FAN1_TARGET_ZONE              0x66

#define R_FAN2_AM_TMP_LIMIT_OFF         0x68
#define R_FAN2_AM_TMP_LIMIT_START       0x69
#define R_FAN2_AM_TMP_LIMIT_FULL        0x6A
#define R_FAN2_AM_START_PWM             0x6B
#define R_FAN2_AM_CONTROL               0x6C
#define R_FAN2_AM_DELTA_TMP             0x6D
#define R_FAN2_TARGET_ZONE              0x6E

#define R_FAN3_AM_TMP_LIMIT_OFF         0x70
#define R_FAN3_AM_TMP_LIMIT_START       0x71
#define R_FAN3_AM_TMP_LIMIT_FULL        0x72
#define R_FAN3_AM_START_PWM             0x73
#define R_FAN3_AM_CONTROL               0x74
#define R_FAN3_AM_DELTA_TMP             0x75
#define R_FAN3_TARGET_ZONE              0x76

#define R_FAN_TAC4_LBS_READING          0x80
#define R_FAN_TAC4_MSB_READING          0x81

#define R_FAN5_READING_LSB              0x82
#define R_FAN5_READING_MSB              0x83

typedef enum {
  SioGpioPin10 = 0x0,
  SioGpioPin11,
  SioGpioPin12,
  SioGpioPin13,
  SioGpioPin14,
  SioGpioPin15,
  SioGpioPin16,
  SioGpioPin17,
  SioGpioPin20 = 0x10,
  SioGpioPin21,
  SioGpioPin22,
  SioGpioPin23,
  SioGpioPin24,
  SioGpioPin25,
  SioGpioPin26,
  SioGpioPin27,
  //
  // other sio pin may be used can be add below
  //
  SioGpioPinMax = 0x80
}SIO_GPIO_PIN;

typedef enum{
  GpioStateLow,
  GpioStateHigh
}SIO_GPIO_STATE;

//
//LDN=5 Keyboard
//
#define R_KBC_KB_SPECIAL_CFG              0xF0

//
//LDN=6 Mouse
//
#define R_KBC_MS_SPECIAL_CFG              0xF0

//
//LDN=7 GPIO
//
#define R_WDT_TIMER_CONTROL               0x71
  #define B_WDT_RST_UPON_CIR_INT          BIT7
  #define B_WDT_RST_UPON_KBC_MS_INT       BIT6
  #define B_WDT_RST_UPON_KBC_KB_INT       BIT5
#define R_WDT_TIMER_CONFIGURATION         0x72
  #define B_WDT_TIME_OUT_UINT_SEL         BIT7
  #define B_WDT_OUTPUT_THRU_KBRST         BIT6
  #define B_WDT_TIME_OUT_UINT_EX_SEL      BIT5
  #define INTERRUPT_LEVEL_MAPPING_IRQ1    0x1
  #define INTERRUPT_LEVEL_MAPPING_IRQ12   0xC

#define R_WDT_TIMER_TIME_OUT_LSB          0x73
#define R_WDT_TIMER_TIME_OUT_MSB          0x74
#define R_SIMPLE_IO1x_EN                  0xC1
#define R_SIMPLE_IO3x_EN                  0xC3
#define R_SIMPLE_IO1x_OUTPUT_EN           0xC9
#define R_SIMPLE_IO3x_OUTPUT_EN           0xCB

//LDN=A ACPI
#define R_POWER_LOSS_CONTROL              0xE4

//LDN=0x19 SMBus configuration register
//
#define R_SMB_SPECIAL_CFG                 0x76




#endif
