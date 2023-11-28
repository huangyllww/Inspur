/* $NoKeywords:$ */
/**
 * @file
 *
 * Contains the definition of the SMU9
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
#ifndef SMU9_BIOS_IF_H_
#define SMU9_BIOS_IF_H_

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
#define SMU9_BIOS_IF_VERSION 0x7

/// @todo Comments: Need to document
typedef struct {
  //BLDCFG
  UINT32 MaxSusPwr; //Power [W]
  UINT32 TDC; //Current [A]
  UINT32 EDC; //Current [A]
  UINT8  DeterminismPercentageEn; //[1 means use the value in DeterminismSlider; 0 means use the default value]
  UINT8  spare3[3];               ///<
  UINT32 DeterminismPercentage; //[0 means full non-determinism; 100 means full determinism; range is between 0-100]

  //BLDCFG/CBS fan table
  UINT8 FanTable_Override;     //[1 means use the full set of data specified below; 0 means use default fan table]
  UINT8 FanTable_Hysteresis;   ///<
  UINT8 FanTable_TempLow;      ///<
  UINT8 FanTable_TempMed;      ///<
  UINT8 FanTable_TempHigh;     ///<
  UINT8 FanTable_TempCritical; ///<
  UINT8 FanTable_PwmLow;       ///<
  UINT8 FanTable_PwmMed;       ///<
  UINT8 FanTable_PwmHigh;      ///<
  UINT8 FanTable_PwmFreq;      //[0 = 25kHz; 1 = 100Hz]
  UINT8 FanTable_Polarity;     //[0 = negative; 1 = positive]
  UINT8 FanTable_spare;        ///<

  //CBS debug options
  UINT8  DldoPsmMargin;                    //[PSM count 1 ~ 1.8mV]
  UINT8  ForceFanPwmEn;                    //[1 means use the ForceFanPwm value below]
  UINT8  ForceFanPwm;                      //[% 0-100]
  UINT8  CoreStretcherDis;                 //[1 means disable]
  UINT8  L3StretcherDis;                   //[1 means disable]
  UINT8  CoreStretchThreshEn;              //[1 means use the CoreStretchThresh value below]
  UINT8  CoreStretchThresh;                //[1 = 2.5%]
  UINT8  L3StretchThreshEn;                //[1 means use the L3StretchThresh value below]
  UINT8  L3StretchThresh;                  //[1 = 2.5%]
  UINT8  DldoBypass;                       //[1 means bypass]
  UINT8  XiSeparationEn;                   //[1 means use the XiSeparation value below]
  UINT8  XiSeparationHigh;                 //[0 = async mode; 3 = 2 cycle; 4 = 2.5 cycle; 5 = 3 cycle; 6 = 3.5 cycle; 7 = 4 cycle]
  UINT8  XiSeparationLow;                  //[0 = async mode; 3 = 2 cycle; 4 = 2.5 cycle; 5 = 3 cycle; 6 = 3.5 cycle; 7 = 4 cycle]
  UINT8  AvfsCoeffTable_Override;          //[1 = override; 0 = use defaults]
  UINT8  ForceVddcrCpuVidEn;               //[1 = use the value in ForceVddcrCpuVid; 0 = use default algorithm]
  UINT8  ForceVddcrCpuVid;                 //[VID code]
  UINT32 VddcrCpuVoltageMargin;            //[mV]
  UINT16 FcwSlewFrac_L3FidTotalSteps;     ///<
  UINT8  FcwSlewFrac_L3FidTotalStepsEn;   ///<
  UINT8  ForceCclkFrequencyEn;            //[1 = force CCLK frequency to ForceCclkFrequency; 0 = use default CCLK controller algorithm]
  UINT32 ForceCclkFrequency;              //[MHz]
  UINT8  UseTelemetryData;                //[1 = use telemetry instead of CAC power estimates for algorithms; 0 = use default]
  UINT8  WaitVidCompDis;                  //[0 = use VOTF feedback from VR; 1 = use internal timer]
  UINT8  xGMIForceLinkWidthEn;            //[1 = force to the value in xGMIForceLinkWidth; 0 = do not force]
  UINT8  xGMIForceLinkWidth;              //[0 = x2; 1 = x8; 2 = x16]
  UINT8  xGMIMaxLinkWidthEn;              //[1 = use the max width defined in xGMIMaxLinkWidth; 0 = use the default max width of x16]
  UINT8  xGMIMaxLinkWidth;                //[0 = x2; 1 = x8; 2 = x16]
  UINT8  spare[2];

  //CBS debug options [AVFS]
  INT32 CorePsfFreq;                      ///<
  INT32 CorePsfTemp;                      ///<
  INT32 CorePsfSidd;                      ///<
  INT32 CorePsfCac;                       ///<
  INT32 CorePsfNumActiveCores;            ///<
  INT32 CorePsfOtherDie;                  ///<
  INT32 CorePsfSigma;                     ///<
  INT32 L3PsfFreq;                        ///<
  INT32 L3PsfTemp;                        ///<
  INT32 L3PsfSidd;                        ///<
  INT32 L3PsfCac;                         ///<
  INT32 L3PsfNumActiveCores;              ///<
  INT32 L3PsfOtherDie;                    ///<
  INT32 L3PsfSigma;                       ///<
} PPTable_t;

/// @todo Comments: Need to document
typedef struct {
  UINT32 HotPlugFormat              :   3;        ///<
  UINT32 Reserved                   :   1;        ///<
  UINT32 GpioDescriptorValid        :   1;        ///<
  UINT32 ResetDescriptorValid       :   1;        ///<
  UINT32 PortActive                 :   1;        ///<
  UINT32 MasterSlaveAPU             :   1;        ///<
  UINT32 DieID                      :   4;        ///<
  UINT32 ApertureID                 :  12;        ///<
  UINT32 AltSlotNum                 :   6;        ///<
  UINT32 SecondaryLink              :   1;        ///<
  UINT32 Reserved1                  :   1;        ///<
} PcieHotPlugMappingDescriptor_t;

/// @todo Comments: Need to document
typedef struct {
  UINT32 i2c_bit_select             :   2;        ///<
  UINT32 i2c_nibble_select          :   1;        ///<
  UINT32 i2c_byte_select            :   3;        ///<
  UINT32 reserved                   :   2;        ///<
  UINT32 i2c_device_address         :   3;        ///<
  UINT32 i2c_device_type            :   2;        ///<
  UINT32 i2c_bus_segment            :   3;        ///<
  UINT32 function_mask              :   8;        ///<
  UINT32 port_mapping               :   5;        ///<
  UINT32 reserved1                  :   3;        ///<
} PcieHotPlugFunctionDescriptor_t;

/// @todo Comments: Need to document
typedef struct {
  UINT32 reserved                   :   3;        ///<
  UINT32 i2c_byte_select            :   3;        ///<
  UINT32 reserved1                  :   2;        ///<
  UINT32 i2c_device_address         :   3;        ///<
  UINT32 i2c_device_type            :   2;        ///<
  UINT32 i2c_bus_segment            :   3;        ///<
  UINT32 reset_select               :   8;        ///<
  UINT32 reserved2                  :   8;        ///<
} PcieHotPlugResetDescriptor_t;

/// @todo Comments: Need to document
typedef struct {
  UINT32 reserved                   :   3;        ///<
  UINT32 i2c_byte_select            :   3;        ///<
  UINT32 reserved1                  :   2;        ///<
  UINT32 i2c_device_address         :   3;        ///<
  UINT32 i2c_device_type            :   2;        ///<
  UINT32 i2c_bus_segment            :   3;        ///<
  UINT32 gpio_select                :   8;        ///<
  UINT32 gpio_interrupt_en          :   8;        ///<
} SmuGenericGpioDescriptor_t;
typedef struct {
  UINT32  PhyPortId                 :   5;
  UINT32  reserved                  :   27;
}PcieHotPlugPatchDescriptor_t;

/// @todo Comments: Need to document
typedef struct {
  PcieHotPlugMappingDescriptor_t  MappingDescriptor   [64];     ///<
  PcieHotPlugFunctionDescriptor_t FunctionDescriptor  [64];     ///<
  PcieHotPlugResetDescriptor_t    ResetDescriptor     [64];     ///<
  SmuGenericGpioDescriptor_t      GpioDescriptor      [64];     ///<
  PcieHotPlugPatchDescriptor_t    HotPlugPatch        [64];
} PcieHotPlugConfigTable_t;

#endif
