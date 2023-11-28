/* $NoKeywords:$ */
/**
 * @file
 *
 * Contains the definition of the SMU10
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: Nbio/SMU
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*
*****************************************************************************
*
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
* ***************************************************************************
*
*/
#ifndef SMU10_BIOS_IF_H
#define SMU10_BIOS_IF_H

//
// Assume standard IA-32 alignment.
// BugBug: Need to check portability of long long
//
typedef unsigned long long  uint64_t;
typedef long long           int64_t;
typedef unsigned int        uint32_t;
typedef int                 int32_t;
typedef unsigned short      uint16_t;
typedef short               int16_t;
typedef unsigned char       uint8_t;
typedef signed char         int8_t;

// *** IMPORTANT ***
// SMU TEAM: Always increment the interface version if
// any structure is changed in this file
#define SMU10_BIOS_IF_VERSION 0x5

typedef struct {
  //PCD infrastructure limits
  uint32_t MaxSusPower;       //Power [mW]
  uint32_t TSP;               //Power [mW]
  uint32_t TskinTimeConstant; //Time constant [seconds]
  uint32_t TjMax;             //Degrees [C]
  uint32_t TDC_VDDCR_CPU;     //Current [mA]
  uint32_t TDC_VDDCR_SOC;     //Current [mA]
  uint32_t EDC_VDDCR_CPU;     //Current [mA]
  uint32_t EDC_VDDCR_SOC;     //Current [mA]

  //PCD/CBS fan table
  uint8_t FanTable_Override;     //[1 means use the full set of data specified below; 0 means use default fan table]
  uint8_t FanTable_Hysteresis;
  uint8_t FanTable_TempLow;
  uint8_t FanTable_TempMed;
  uint8_t FanTable_TempHigh;
  uint8_t FanTable_TempCritical;
  uint8_t FanTable_PwmLow;
  uint8_t FanTable_PwmMed;
  uint8_t FanTable_PwmHigh;
  uint8_t FanTable_PwmFreq;      //[0 = 25kHz; 1 = 100Hz]
  uint8_t FanTable_spare[2];

  //CBS debug options
  uint8_t  CoreDldoPsmMargin;       //[PSM count 1 ~ 1.8mV]
  uint8_t  GfxDldoPsmMargin;        //[PSM count 1 ~ 1.8mV]
  uint8_t  ForceFanPwmEn;           //[1 means use the ForceFanPwm value below]
  uint8_t  ForceFanPwm;             //[% 0-100]
  uint8_t  CoreStretcherDis;        //[1 means disable]
  uint8_t  L3StretcherDis;          //[1 means disable]
  uint8_t  CoreStretchThreshEn;     //[1 means use the CoreStretchThresh value below]
  uint8_t  CoreStretchThresh;       //[1 = 2.5%]
  uint8_t  L3StretchThreshEn;       //[1 means use the L3StretchThresh value below]
  uint8_t  L3StretchThresh;         //[1 = 2.5%]
  uint8_t  CoreDldoBypass;          //[1 means bypass]
  uint8_t  GfxDldoBypass;           //[1 means bypass]
  uint8_t  XiSeparationEn;          //[1 means use the XiSeparation value below]
  uint8_t  XiSeparationHigh;        //[0 = async mode; 3 = 2 cycle; 4 = 2.5 cycle; 5 = 3 cycle; 6 = 3.5 cycle; 7 = 4 cycle]
  uint8_t  XiSeparationLow;         //[0 = async mode; 3 = 2 cycle; 4 = 2.5 cycle; 5 = 3 cycle; 6 = 3.5 cycle; 7 = 4 cycle]
  uint8_t  AvfsCoeffTable_Override; //[1 = override; 0 = use defaults]
  uint32_t VddcrCpuVoltageMargin;   //[mV]
  uint32_t VddcrSocVoltageMargin;   //[mV]
  uint16_t FcwSlewFrac_L3FidTotalSteps;
  uint8_t  FcwSlewFrac_L3FidTotalStepsEn;
  uint8_t  WaitVidCompDis;          //[0 = use VOTF feedback from VR; 1 = use internal timer]

  //CBS debug options [AVFS]
  uint32_t CorePsfFreq;
  uint32_t CorePsfTemp;
  uint32_t CorePsfSidd;
  uint32_t CorePsfCac;
  uint32_t CorePsfNumActiveCores;
  uint32_t CorePsfSigma;
  uint32_t L3PsfFreq;
  uint32_t L3PsfTemp;
  uint32_t L3PsfSidd;
  uint32_t L3PsfCac;
  uint32_t L3PsfNumActiveCores;
  uint32_t L3PsfSigma;

  //CBS debug options [CLDO]
  uint8_t CLDO_VDDP_Bypass;      //[0 = Normal Regulation; 1 = bypass to VDDCR_SOC; 2 = bypass to VDDIO_MEM]
  uint8_t CLDO_VDDM_Bypass;      //[0 = Normal Regulation; 1 = bypass to VDDCR_SOC; 2 = bypass to VDDIO_MEM]
  uint8_t spare2[2];

  //PCD/CBS Telemetry setup
  uint32_t Telemetry_VddcrCpuSlope;
  uint32_t Telemetry_VddcrCpuOffset;
  uint32_t Telemetry_VddcrSocSlope;
  uint32_t Telemetry_VddcrSocOffset;

} PPTable_t;

typedef struct {
  uint32_t HotPlugFormat              :   3;
  uint32_t Reserved                   :   1;
  uint32_t GpioDescriptorValid        :   1;
  uint32_t ResetDescriptorValid       :   1;
  uint32_t PortActive                 :   1;
  uint32_t MasterSlaveAPU             :   1;
  uint32_t DieID                      :   4;
  uint32_t ApertureID                 :  12;
  uint32_t AltSlotNum                 :   6;
  uint32_t SecondaryLink              :   1;
  uint32_t Reserved1                  :   1;
} PcieHotPlugMappingDescriptor_t;

typedef struct
{
  uint32_t i2c_bit_select             :   2;
  uint32_t i2c_nibble_select          :   1;
  uint32_t i2c_byte_select            :   3;
  uint32_t reserved                   :   2;
  uint32_t i2c_device_address         :   3;
  uint32_t i2c_device_type            :   2;
  uint32_t i2c_bus_segment            :   3;
  uint32_t function_mask              :   8;
  uint32_t reserved1                  :   8;
} PcieHotPlugFunctionDescriptor_t;

typedef struct
{
  uint32_t reserved                   :   3;
  uint32_t i2c_byte_select            :   3;
  uint32_t reserved1                  :   2;
  uint32_t i2c_device_address         :   3;
  uint32_t i2c_device_type            :   2;
  uint32_t i2c_bus_segment            :   3;
  uint32_t reset_select               :   8;
  uint32_t reserved2                  :   8;
} PcieHotPlugResetDescriptor_t;

typedef struct
{
  uint32_t reserved                   :   3;
  uint32_t i2c_byte_select            :   3;
  uint32_t reserved1                  :   2;
  uint32_t i2c_device_address         :   3;
  uint32_t i2c_device_type            :   2;
  uint32_t i2c_bus_segment            :   3;
  uint32_t gpio_select                :   8;
  uint32_t gpio_interrupt_en          :   8;
} SmuGenericGpioDescriptor_t;

typedef struct {
  PcieHotPlugMappingDescriptor_t  MappingDescriptor   [64];
  PcieHotPlugFunctionDescriptor_t FunctionDescriptor  [64];
  PcieHotPlugResetDescriptor_t    ResetDescriptor     [64];
  SmuGenericGpioDescriptor_t      GpioDescriptor      [64];
} PcieHotPlugConfigTable_t;

typedef struct {
  uint32_t version;

  PPTable_t                PPTable;
  PcieHotPlugConfigTable_t PcieHotPlugConfigTable;

} SMU10BiosInterfaceTable_t;


#endif
