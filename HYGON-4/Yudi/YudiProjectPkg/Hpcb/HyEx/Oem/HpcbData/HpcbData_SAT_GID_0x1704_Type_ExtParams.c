/* $NoKeywords:$ */

/**
 * @file
 *
 */
/*****************************************************************************
 *
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 *
 *
 ***************************************************************************/

#include "MyPorting.h"
#include <MiscMemDefines.h>
#include <HPCB.h>
#include <HpcbCustomizedDefinitions.h>

CHAR8  mDummyBuf;

HPCB_TYPE_DATA_START_SIGNATURE ();
HPCB_TYPE_HEADER  HpcbTypeHeader = {
  HPCB_GROUP_MEMORY,
  HPCB_MEM_TYPE_CONFIG_PARAMETERS,
  (
   sizeof (HPCB_TYPE_HEADER)
  ),
  0,
  0,
  0
};

HPCB_PARAM_ATTRIBUTE  ParameterListAttribute[] = {
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_BOTTOMIO,                           sizeof (UINT16) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEMHOLEREMAPPING,                   sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_LIMITMEMORYTOBELOW1TB,              sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_USERTIMINGMODE,                     sizeof (UINT32) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEMCLOCKVALUE,                      sizeof (UINT32) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_ENABLECHIPSELECTINTLV,              sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_ENABLECHANNELINTLV,                 sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_ENABLEECCFEATURE,                   sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_ENABLEPOWERDOWN,                    sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_ENABLEPARITY,                       sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_ENABLEBANKSWIZZLE,                  sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_ENABLEMEMCLR,                       sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_UMAMODE,                            sizeof (UINT8) - 1,   0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_UMASIZE,                            sizeof (UINT32) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEMRESTORECTL,                      sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_SAVEMEMCONTEXTCTL,                  sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_ISCAPSULEMODE,                      sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_FORCETRAINMODE,                     sizeof (UINT8) - 1,   0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DIMMTYPEUSEDINMIXEDCONFIG,          sizeof (UINT8) - 1,   0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_AMPENABLE,                          sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DRAMDOUBLEREFRESHRATE,              sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_PMUTRAINMODE,                       sizeof (UINT8) - 1,   0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_ECCREDIRECTION,                     sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_SCRUBDRAMRATE,                      sizeof (UINT16) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_SCRUBL2RATE,                        sizeof (UINT16) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_SCRUBL3RATE,                        sizeof (UINT16) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_SCRUBICRATE,                        sizeof (UINT16) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_SCRUBDCRATE,                        sizeof (UINT16) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_ECCSYNCFLOOD,                       sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_ECCSYMBOLSIZE,                      sizeof (UINT16) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DQSTRAININGCONTROL,                 sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_UMAABOVE4G,                         sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_UMAALIGNMENT,                       sizeof (UINT32) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEMORYALLCLOCKSON,                  sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEMORYBUSFREQUENCYLIMIT,            sizeof (UINT32) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_POWERDOWNMODE,                      sizeof (UINT32) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_IGNORESPDCHECKSUM,                  sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEMORYMODEUNGANGED,                 sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEMORYQUADRANKCAPABLE,              sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEMORYRDIMMCAPABLE,                 sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEMORYLRDIMMCAPABLE,                sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEMORYUDIMMCAPABLE,                 sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEMORYSODIMMCAPABLE,                sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DRAMDOUBLEREFRESHRATEEN,            sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DIMMTYPEDDDR5CAPABLE,               sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_ENABLEZQRESET,                      sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_ENABLEBANKGROUPSWAP,                sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_ODTSCMDTHROTEN,                     sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_SWCMDTHROTEN,                       sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_FORCEPWRDOWNTHROTEN,                sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_ODTSCMDTHROTCYC,                    sizeof (UINT8) - 1,   0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_SWCMDTHROTCYC,                      sizeof (UINT8) - 1,   0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DIMMSENSORCONF,                     sizeof (UINT16) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DIMMSENSORUPPER,                    sizeof (UINT16) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DIMMSENSORLOWER,                    sizeof (UINT16) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DIMMSENSORCRITICAL,                 sizeof (UINT16) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DIMMSENSORRESOLUTION,               sizeof (UINT8) - 1,   0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_AUTOREFFINEGRANMODE,                sizeof (UINT8) - 1,   0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_ENABLEMEMPSTATE,                    sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DDRROUTEBALANCEDTEE,                sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEM_MBIST_TEST_ENABLE,              sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEM_MBIST_SUBTEST_TYPE,             sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEM_MBIST_AGGRESOR_ON,              sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEM_MBIST_HALT_ON_ERROR,            sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEM_CPU_VREF_RANGE,                 sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEM_DRAM_VREF_RANGE,                sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEM_TSME_ENABLE,                    sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEM_NVDIMM_POWER_SOURCE,            sizeof (UINT8) - 1,   0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEM_DATA_POISON,                    sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEM_DATA_SCRAMBLE,                  sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEM_PS_ERROR_HANDLING,              sizeof (UINT8) - 1,   0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEM_TEMP_CONTROLLED_REFRESH_EN,     sizeof (BOOLEAN) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT, sizeof (UINT8) - 1,   0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_MEM_I2C_I3C_VOLTAGE_LEVEL,          sizeof (UINT8) - 1,   0 },
  { 0,                   HPCB_TOKEN_CONFIG_LIMIT,                              0,                    0 },
};

_1BYTE_ALIGN UINT8  ParameterListValue[] = {
  BSU16 (BLDCFG_BOTTOM_IO),                     // < BottomIo - Bottom of 32-bit IO space (8-bits).
  BSBLN (BLDCFG_MEM_HOLE_REMAPPING),            // < MemHoleRemapping - Memory Hole Remapping (1-bit).
  BSBLN (BLDCFG_MEM_LIMIT_1TB),                 // < Limit memory address space to below 1 TB
  BSU32 (BLDCFG_TIMING_MODE_SELECT),            // < UserTimingMode - User Memclock Mode.
  BSU32 (BLDCFG_MEMORY_CLOCK_SELECT),           // < MemClockValue -  Memory Clock Value.
  BSBLN (BLDCFG_MEMORY_ENABLE_CHIPSELECT_INTERLEAVING), // < EnableChipSelectIntlv - Dram chip select Interleaving (1-bit).
  BSBLN (BLDCFG_MEMORY_CHANNEL_INTERLEAVING),   // < EnableChannelIntlv - Channel Interleaving (1-bit).
  BSBLN (BLDCFG_ENABLE_ECC_FEATURE),            // < EnableEccFeature - enable ECC error to go into MCE.
  BSBLN (BLDCFG_MEMORY_POWER_DOWN),             // < EnablePowerDown - CKE based power down mode (1-bit).
  BSBLN (BLDCFG_PARITY_CONTROL),                // < EnableParity - Parity control.
  BSBLN (BLDCFG_BANK_SWIZZLE),                  // < EnableBankSwizzle - BankSwizzle control.
  BSBLN (BLDCFG_ENABLE_MEM_CLR),                // < EnableMemClr - Memory Clear functionality control.
  BSU08 (BLDCFG_UMA_MODE),                      // < UmaMode - Uma Mode
  BSU32 (BLDCFG_UMA_SIZE),                      // < UmaSize - The size of shared graphics dram (16-bits)
  BSBLN (BLDCFG_MEMORY_RESTORE_CONTROL),        // < MemRestoreCtl - Memory context restore control
  BSBLN (BLDCFG_SAVE_MEMORY_CONTEXT_CONTROL),   // < SaveMemContextCtl - Control switch to save memory context at the end of MemAuto
  BSBLN (BLDCFG_IS_CAPSULE_MODE),               // < IsCapsuleMode - Capsule reboot control
  BSU08 (BLDCFG_FORCE_TRAIN_MODE),              // < ForceTrainMode - Training Mode
  BSU08 (BLDCFG_DIMM_TYPE_TECHNOLGY),           // < DimmTypeUsedInMixedConfig - Select the preferred technology type that HGPI will enable
  BSBLN (BLDCFG_AMP_MODE_ENABLE),               // < AmpEnable - Specify the average time between refresh requests to all DRAM devices.
  BSBLN (BLDCFG_DRAM_DOUBLE_REFRESH_RATE),      // < DramDoubleRefreshRate - PMU Training Mode
  BSU08 (BLDCFG_PMU_TRAINING_MODE),             // < PmuTrainMode - PMU Training Mode
  BSBLN (BLDCFG_ECC_REDIRECTION),               // < CfgEccRedirection; ///< ECC Redirection.
  BSU16 (BLDCFG_SCRUB_DRAM_RATE),               // < CfgScrubDramRate -  Scrub Dram Rate.
  BSU16 (BLDCFG_SCRUB_L2_RATE),                 // < CfgScrubL2Rate -  Scrub L2Rate.
  BSU16 (BLDCFG_SCRUB_L3_RATE),                 // < CfgScrubL3Rate -  Scrub L3Rate.
  BSU16 (BLDCFG_SCRUB_IC_RATE),                 // < CfgScrubIcRate -  Scrub Ic Rate.
  BSU16 (BLDCFG_SCRUB_DC_RATE),                 // < CfgScrubDcRate -  Scrub Dc Rate.
  BSBLN (BLDCFG_ECC_SYNC_FLOOD),                // < CfgEccSyncFlood -  ECC Sync Flood.
  BSU16 (BLDCFG_ECC_SYMBOL_SIZE),               // < CfgEccSymbolSize -  ECC Symbol Size.
  BSBLN (BLDCFG_DQS_TRAINING_CONTROL),          // < CfgDqsTrainingControl -  Dqs Training Control.
  BSBLN (BLDCFG_UMA_ABOVE_4GB),                 // < CfgUmaAbove4G -  Uma Above 4G Support
  BSU32 (BLDCFG_UMA_BUFFER_ALIGNMENT),          // < CfgUmaAlignment -  Uma alignment
  BSBLN (BLDCFG_MEMORY_ALL_CLOCKS_ON),          // < CfgMemoryAllClocksOn -  Memory All Clocks On.
  BSU32 (BLDCFG_MEMORY_BUS_FREQUENCY_LIMIT),    // < CfgMemoryBusFrequencyLimit -  Memory Bus Frequency Limit.
  BSU32 (BLDCFG_POWER_DOWN_MODE),               // < CfgPowerDownMode -  Power Down Mode.
  BSBLN (BLDCFG_IGNORE_SPD_CHECKSUM),           // < CfgIgnoreSpdChecksum -  Ignore Spd Checksum.
  BSBLN (BLDCFG_MEMORY_MODE_UNGANGED),          // < CfgMemoryModeUnganged -  Memory Mode Unganged.
  BSBLN (BLDCFG_MEMORY_QUAD_RANK_CAPABLE),      // < CfgMemoryQuadRankCapable -  Memory Quad Rank Capable.
  BSBLN (BLDCFG_MEMORY_RDIMM_CAPABLE),          // < CfgMemoryRDimmCapable -  Memory RDIMM Capable.
  BSBLN (BLDCFG_MEMORY_LRDIMM_CAPABLE),         // < CfgMemoryLRDimmCapable -  Memory LRDIMM Capable.
  BSBLN (BLDCFG_MEMORY_UDIMM_CAPABLE),          // < CfgMemoryUDimmCapable -  Memory UDIMM Capable
  BSBLN (BLDCFG_MEMORY_SODIMM_CAPABLE),         // < CfgMemorySODimmCapable - Memory SODimm Capable.
  BSBLN (BLDCFG_DRAM_DOUBLE_REFRESH_RATE_ENABLE), // < CfgDramDoubleRefreshRateEn - Double DRAM refresh rate
  BSBLN (BLDCFG_MEMORY_DDR5_CAPABLE),           // < DimmTypeDddr5Capable -  Indicates that the system is DDR5 Capable
  BSBLN (BLDCFG_ZQ_RESET_CONTROL_ENABLE),       // < EnableZqReset - Indicates that the system is ZQRESET Capable
  BSBLN (BLDCFG_BANK_SWAP_GROUP_ENABLE),        // < EnableBankGroupSwap - Indicates that Bank Group Swap is enabled
  BSBLN (BLDCFG_ODT_CMD_THROT_ENABLE),          // < UMC::CH::ThrottleCtrl[OdtsCmdThrotEn]
  BSBLN (BLDCFG_SW_CMD_THROT_ENABLE),           // < UMC::CH::ThrottleCtrl[SwCmdThrotEn]
  BSBLN (BLDCFG_FORCE_POWER_DOWN_THROT_ENABLE), // < UMC::CH::ThrottleCtrl[ForcePwrDownThrotEn]
  BSU08 (BLDCFG_ODT_CMD_THROT_CYCLE),           // < UMC::CH::ThrottleCtrl[OdtsCmdThrotCyc]
  BSU08 (BLDCFG_SW_CMD_THROT_CYCLE),            // < UMC::CH::ThrottleCtrl[SwCmdThrotCyc]
  BSU16 (BLDCFG_DIMM_SENSOR_CONFIGURATION),     // < DIMM temperature sensor register index BSU16 (1), Configuration
  BSU16 (BLDCFG_DIMM_SENSOR_UPPER),             // < DIMM temperature sensor register index BSU16 (2), Alarm temperature upper boundary
  BSU16 (BLDCFG_DIMM_SENSOR_LOWER),             // < DIMM temperature sensor register index BSU16 (3), Alarm temperature lower boundary
  BSU16 (BLDCFG_DIMM_SENSOR_CRITICAL),          // < DIMM temperature sensor register index BSU16 (4), Critical temperature
  BSU08 (BLDCFG_DIMM_SENSOR_RESOLUTION),        // < DIMM temperature sensor register index BSU16 (8), Temperature resolution register
  BSU08 (BLDCFG_AUTO_REF_FINE_GRAN_MODE),       // < UMC::CH::SpazCtrl_AutoRefFineGranMode
  BSBLN (BLDCFG_ENABLE_MEMPSTATE),              // < Memory Pstate Enable
  BSBLN (BLDCFG_ENABLE_DDRROUTEBALANCEDTEE),    // < CfgDdrRouteBalancedTee - Motherboard DDR Routing control.
  BSU08 (BLDCFG_MEM_MBIST_TEST_ENABLE),         // < MBIST Test Enable/Disable
  BSU08 (BLDCFG_MEM_MBIST_SUBTEST_TYPE),        // < MBIST SubTest Type
  BSU08 (BLDCFG_MEM_MBIST_AGGRESOR_ON),         // < Aggressor Enable/Disable
  BSU08 (BLDCFG_MEM_MBIST_HALT_ON_ERROR),       // < Halt on Error in case MBIST Test failed
  BSU08 (BLDCFG_MEM_MBIST_CPU_VREF_RANGE),      // < CPU Vref margins in percent (%)
  BSU08 (BLDCFG_MEM_MBIST_DRAM_VREF_RANGE),     // < DRAM Vref margins in percent (%)
  BSU08 (BLDCFG_MEM_TSME_ENABLE),               // < TSME enable
  BSU08 (BLDCFG_MEM_NVDIMM_POWER_SOURCE),       // < NVDIMM power source, 1: device, 2: host
  BSU08 (BLDCFG_MEM_DATA_POISON),               // < Data Poison enabling 1: Enabled, 0: Disabled
  BSU08 (BLDCFG_MEM_DATA_SCRAMBLE),             // < Data Scramble enabling 1: Enabled, 0: Disabled
  BSU08 (BLDCFG_MEM_PS_ERROR_HANDLING),         // < Platform specific error handling
  BSBLN (BLDCFG_MEM_TEMP_CONTROLLED_REFRESH_EN),// < Dram Temperature Controlled Refresh Rate Enable
  BSU08 (BLDCFG_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT), // < MBIST per bit slave die report
  BSU08 (BLDCFG_MEM_I2C_I3C_VOLTAGE_LEVEL),          // < Set I2C Or I3C Controller Voltage for SPD
  BSU08 (0xFF)
};

HPCB_TYPE_DATA_END_SIGNATURE ();

int
main (
  IN int argc, IN char *argv[]
  )
{
  return 0;
}
