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
#ifndef SMU10_H
#define SMU10_H

//#include <stdint.h>
//#include <features/smu_features.h>

#pragma pack(push, 1)

#define ENABLE_DEBUG_FEATURES

//#define SMU9_MAX_LEVELS_GRAPHICS        5   // SCLK
//#define SMU9_MAX_LEVELS_GIO             5   // LCLK Levels
//#define SMU9_MAX_LEVELS_UVD             5   // VCLK/DCLK levels for UVD.
//#define SMU9_MAX_LEVELS_VCE             5   // ECLK levels for VCE.
//#define SMU9_MAX_LEVELS_ACP             5   // ACLK levels for ACP.

//#define GPIO_CLAMP_MODE_VRHOT      1
//#define GPIO_CLAMP_MODE_THERM      2
//#define GPIO_CLAMP_MODE_DC         4

// Feature Control Defines
#define FEATURE_CCLK_CONTROLLER_BIT   0
#define FEATURE_FAN_CONTROLLER_BIT    1
#define FEATURE_DATA_CALCULATION_BIT  2
#define FEATURE_PPT_BIT               3
#define FEATURE_TDC_BIT               4
#define FEATURE_THERMAL_BIT           5
#define FEATURE_FIT_BIT               6
#define FEATURE_GFXOFF_BIT            7
#define FEATURE_PLL_POWER_DOWN_BIT    8
#define FEATURE_ULV_BIT               9
#define FEATURE_VDDOFF_BIT            10
#define FEATURE_VCN_DPM_BIT           11
#define FEATURE_ACP_DPM_BIT           12
#define FEATURE_ISP_DPM_BIT           13
#define FEATURE_FCLK_DPM_BIT          14
#define FEATURE_SOCCLK_DPM_BIT        15
#define FEATURE_MP0CLK_DPM_BIT        16
#define FEATURE_LCLK_DPM_BIT          17
#define FEATURE_SHUBCLK_DPM_BIT       18
#define FEATURE_DCEFCLK_DPM_BIT       19
#define FEATURE_GFX_DPM_BIT           20
#define FEATURE_DS_GFXCLK_BIT         21
#define FEATURE_DS_SOCCLK_BIT         22
#define FEATURE_DS_LCLK_BIT           23
#define FEATURE_DS_DCEFCLK_BIT        24
#define FEATURE_SPARE_25_BIT          25
#define FEATURE_SPARE_26_BIT          26
#define FEATURE_SPARE_27_BIT          27
#define FEATURE_SPARE_28_BIT          28
#define FEATURE_SPARE_29_BIT          29
#define FEATURE_SPARE_30_BIT          30
#define FEATURE_SPARE_31_BIT          31

#define NUM_FEATURES                  32

#define FEATURE_CCLK_CONTROLLER_MASK  (1 << FEATURE_CCLK_CONTROLLER_BIT   )
#define FEATURE_FAN_CONTROLLER_MASK   (1 << FEATURE_FAN_CONTROLLER_BIT    )
#define FEATURE_DATA_CALCULATION_MASK (1 << FEATURE_DATA_CALCULATION_BIT  )
#define FEATURE_PPT_MASK              (1 << FEATURE_PPT_BIT               )
#define FEATURE_TDC_MASK              (1 << FEATURE_TDC_BIT               )
#define FEATURE_THERMAL_MASK          (1 << FEATURE_THERMAL_BIT           )
#define FEATURE_FIT_MASK              (1 << FEATURE_FIT_BIT               )
#define FEATURE_GFXOFF_MASK           (1 << FEATURE_GFXOFF_BIT            )
#define FEATURE_PLL_POWER_DOWN_MASK   (1 << FEATURE_PLL_POWER_DOWN_BIT    )
#define FEATURE_ULV_MASK              (1 << FEATURE_ULV_BIT               )
#define FEATURE_VDDOFF_MASK           (1 << FEATURE_VDDOFF_BIT            )
#define FEATURE_VCN_DPM_MASK          (1 << FEATURE_VCN_DPM_BIT           )
#define FEATURE_ACP_DPM_MASK          (1 << FEATURE_ACP_DPM_BIT           )
#define FEATURE_ISP_DPM_MASK          (1 << FEATURE_ISP_DPM_BIT           )
#define FEATURE_FCLK_DPM_MASK         (1 << FEATURE_FCLK_DPM_BIT          )
#define FEATURE_SOCCLK_DPM_MASK       (1 << FEATURE_SOCCLK_DPM_BIT        )
#define FEATURE_MP0CLK_DPM_MASK       (1 << FEATURE_MP0CLK_DPM_BIT        )
#define FEATURE_LCLK_DPM_MASK         (1 << FEATURE_LCLK_DPM_BIT          )
#define FEATURE_SHUBCLK_DPM_MASK      (1 << FEATURE_SHUBCLK_DPM_BIT       )
#define FEATURE_DCEFCLK_DPM_MASK      (1 << FEATURE_DCEFCLK_DPM_BIT       )
#define FEATURE_GFX_DPM_MASK          (1 << FEATURE_GFX_DPM_BIT           )
#define FEATURE_DS_GFXCLK_MASK        (1 << FEATURE_DS_GFXCLK_BIT         )
#define FEATURE_DS_SOCCLK_MASK        (1 << FEATURE_DS_SOCCLK_BIT         )
#define FEATURE_DS_LCLK_MASK          (1 << FEATURE_DS_LCLK_BIT           )
#define FEATURE_DS_DCEFCLK_MASK       (1 << FEATURE_DS_DCEFCLK_BIT        )


struct SMU10_Firmware_Footer
{
  uint32_t Signature;
};
typedef struct SMU10_Firmware_Footer SMU10_Firmware_Footer;

struct SMU10_Firmware_Header
{
    uint32_t Version;
    uint32_t ImageSize;
    uint32_t CodeSize;
    uint32_t HeaderSize;
    uint32_t EntryPoint;

    uint32_t Rtos;
    uint32_t UcodeLoadStatus;
    uint32_t DpmTable;
    uint32_t FanTable;
    uint32_t PmFuseTable;
    uint32_t Globals;
    uint32_t SoftRegisters;

    uint32_t Reserved[7];
    uint32_t Signature;
};

typedef struct SMU10_Firmware_Header SMU_Firmware_Header;

struct SMU_EnabledFeaturesScoreboard {
  uint32_t BapmOn              : 1;
  uint32_t C7On                : 1;
  uint32_t ConnStdbyOn         : 1;
  uint32_t GpuCacOn            : 1;
  uint32_t BbbOn               : 1;
  uint32_t IbsOn               : 1;
  uint32_t LclkDpmForced       : 1;
  uint32_t LclkDpmOn           : 1;
  uint32_t PspDpmOn            : 1;
  uint32_t PkgPwrLimitOn       : 1;
  uint32_t PstateArbiterOn     : 1;
  uint32_t TdcLimitOn          : 1;
  uint32_t SocVoltageOn        : 1;
  uint32_t StapmOn             : 1;
  uint32_t ThermalOn           : 1;
  uint32_t FpsOn               : 1;
  uint32_t LeapmOn             : 1;
  uint32_t LnkWdthMgtOn        : 1;
  uint32_t Reserved            : 14;
  // NOTE: Make sure that resetting to 0 works in InitDpm() when this
  // structure becomes larger than 32 bits.
};
typedef struct SMU_EnabledFeaturesScoreboard SMU_EnabledFeaturesScoreboard;

#define SMU10_MAX_CUS 2
#define SMU10_PSMS_PER_CU 10
#define SMU10_Num_tests 3

struct SMU10_BtcDebugParams
{
  uint32_t BC_completed;
  int16_t dV[SMU10_Num_tests]; // 6 bytes
  int16_t dV_LLtravel[SMU10_Num_tests]; // 6 bytes
  int16_t MinPsmCount[SMU10_MAX_CUS][SMU10_Num_tests];
  int16_t ObservedPsmCount[SMU10_MAX_CUS][SMU10_Num_tests][SMU10_PSMS_PER_CU];
  int16_t ObservedTemperature[SMU10_MAX_CUS][SMU10_Num_tests];
  int16_t dV_minPSM[SMU10_MAX_CUS][SMU10_Num_tests];
  int16_t SYS_Quiescent[SMU10_MAX_CUS][SMU10_Num_tests];
  int16_t AvgPsmCount[SMU10_MAX_CUS][SMU10_Num_tests];
  int32_t temp_monitor[SMU10_MAX_CUS][6]; // here 3 is dependent on how may remote sensors we have for the tracked Tcen
};

typedef struct SMU10_BtcDebugParams SMU10_BtcDebugParams;

struct SMU_SoftRegisters {
  uint32_t LoggerSharedSpace[0x100]; // 1K of shared space
};

typedef struct SMU_SoftRegisters SMU_SoftRegisters;

#define SMU10_FIRMWARE_HEADER_LOCATION 0x1FF80
#define SMU10_UNBCSR_START_ADDR 0xC0100000

enum DISPLAY_PHY_TYPE {
  DISPLAY_PHY_TYPE_POWER_DOWN = 1,
  DISPLAY_PHY_TYPE_DP54x4,
  DISPLAY_PHY_TYPE_DP54x2,
  DISPLAY_PHY_TYPE_DP54x1,
  DISPLAY_PHY_TYPE_DP432x4,
  DISPLAY_PHY_TYPE_DP432x2,
  DISPLAY_PHY_TYPE_DP432x1,
  DISPLAY_PHY_TYPE_DP324x4,
  DISPLAY_PHY_TYPE_DP324x2,
  DISPLAY_PHY_TYPE_DP324x1,
  DISPLAY_PHY_TYPE_DP27x4,
  DISPLAY_PHY_TYPE_DP27x2,
  DISPLAY_PHY_TYPE_DP27x1,
  DISPLAY_PHY_TYPE_DP243x4,
  DISPLAY_PHY_TYPE_DP243x2,
  DISPLAY_PHY_TYPE_DP243x1,
  DISPLAY_PHY_TYPE_DP216x4,
  DISPLAY_PHY_TYPE_DP216x2,
  DISPLAY_PHY_TYPE_DP216x1,
  DISPLAY_PHY_TYPE_DP162x4,
  DISPLAY_PHY_TYPE_DP162x2,
  DISPLAY_PHY_TYPE_DP162x1,
  DISPLAY_PHY_TYPE_HDMI597,
  DISPLAY_PHY_TYPE_HDMI297,
  DISPLAY_PHY_TYPE_HDMI162,
  DISPLAY_PHY_TYPE_LVDS,
  DISPLAY_PHY_TYPE_DVI,
  DISPLAY_PHY_TYPE_VGA,
  DISPLAY_PHY_TYPE_COUNT
};

struct SMU10_MultimediaPowerLogData {
  uint32_t avgTotalPower;
  uint32_t avgGpuPower;
  uint32_t avgUvdPower;
  uint32_t avgVcePower;

  uint32_t avgSclk;
  uint32_t avgDclk;
  uint32_t avgVclk;
  uint32_t avgEclk;

  uint32_t startTimeHi;
  uint32_t startTimeLo;

  uint32_t endTimeHi;
  uint32_t endTimeLo;
};

typedef struct SMU10_MultimediaPowerLogData SMU10_MultimediaPowerLogData;

typedef struct {
  // MP1_EXT_SCRATCH0
  uint8_t CurrLevel_GFXCLK;
  uint8_t CurrLevel_UVD;
  uint8_t CurrLevel_VCE;
  uint8_t CurrLevel_LCLK;
  // MP1_EXT_SCRATCH1
  uint8_t CurrLevel_MP0CLK;
  uint8_t CurrLevel_UCLK;
  uint8_t CurrLevel_SOCCLK;
  uint8_t CurrLevel_DCEFCLK;
  // MP1_EXT_SCRATCH2
  uint8_t TargLevel_GFXCLK;
  uint8_t TargLevel_UVD;
  uint8_t TargLevel_VCE;
  uint8_t TargLevel_LCLK;
  // MP1_EXT_SCRATCH3
  uint8_t TargLevel_MP0CLK;
  uint8_t TargLevel_UCLK;
  uint8_t TargLevel_SOCCLK;
  uint8_t TargLevel_DCEFCLK;
  // MP1_EXT_SCRATCH4-7
  uint32_t Reserved[4];
} FwStatus_t;
#pragma pack(pop)


#endif

