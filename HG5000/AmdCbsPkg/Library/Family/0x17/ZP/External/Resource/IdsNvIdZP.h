/*****************************************************************************
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
 *
 ***************************************************************************/


//This file is auto generated, don't edit it manually

#ifndef _IDS_NV_ID_ZP_H_
#define _IDS_NV_ID_ZP_H_

//Zen Common Options
#define IDSNVID_CMN_CPU_GEN_W_A05   (0x0000) //RedirectForReturnDis
#define IDSNVID_CMN_CPU_L2_TLB_WAY_ALLOC   (0x0001) //L2 TLB Associativity
#define IDSNVID_CMN_CPU_PFEH   (0x0002) //Platform First Error Handling
#define IDSNVID_CMN_CPU_CPB   (0x0003) //Core Performance Boost
#define IDSNVID_CMN_CPU_EN_IBS   (0x0004) //Enable IBS
#define IDSNVID_CMN_CPU_GLOBAL_CSTATE_CTRL   (0x0005) //Global C-state Control
#define IDSNVID_CMN_CPU_OPCACHE_CTRL   (0x0006) //Opcache Control
#define IDSNVID_CMN_CPU_OC_MODE   (0x0007) //OC Mode
#define IDSNVID_CMN_CPU_SEV_ASID_SPACE_LIMIT   (0x0008) //SEV-ES ASID Space Limit
#define IDSNVID_CMN_CPU_STREAMING_STORES_CTRL   (0x0009) //Streaming Stores Control
//Custom Core Pstates
#define IDSNVID_CPU_LEGAL_DISCLAIMER   (0x000A) //Custom Core Pstates Legal Disclaimer
#define IDSNVID_CPU_LEGAL_DISCLAIMER1   (0x000B) //Custom Core Pstates Legal Disclaimer 1
//Decline
//Accept
#define IDSNVID_CPU_PST_CUSTOM_P0   (0x000C) //Custom Pstate0
#define IDSNVID_CPU_COF_P0   (0x000D) //  Frequency (MHz)
#define IDSNVID_CPU_VOLTAGE_P0   (0x000E) //  Voltage (uV)
#define IDSNVID_CPU_PST0_FID   (0x000F) //  Pstate0 FID
#define IDSNVID_CPU_PST0_DID   (0x0010) //  Pstate0 DID
#define IDSNVID_CPU_PST0_VID   (0x0011) //  Pstate0 VID
#define IDSNVID_CPU_PST_CUSTOM_P1   (0x0012) //Custom Pstate1
#define IDSNVID_CPU_COF_P1   (0x0013) //  Frequency (MHz)
#define IDSNVID_CPU_VOLTAGE_P1   (0x0014) //  Voltage (uV)
#define IDSNVID_CPU_PST1_FID   (0x0015) //  Pstate1 FID
#define IDSNVID_CPU_PST1_DID   (0x0016) //  Pstate1 DID
#define IDSNVID_CPU_PST1_VID   (0x0017) //  Pstate1 VID
#define IDSNVID_CPU_PST_CUSTOM_P2   (0x0018) //Custom Pstate2
#define IDSNVID_CPU_COF_P2   (0x0019) //  Frequency (MHz)
#define IDSNVID_CPU_VOLTAGE_P2   (0x001A) //  Voltage (uV)
#define IDSNVID_CPU_PST2_FID   (0x001B) //  Pstate2 FID
#define IDSNVID_CPU_PST2_DID   (0x001C) //  Pstate2 DID
#define IDSNVID_CPU_PST2_VID   (0x001D) //  Pstate2 VID
#define IDSNVID_CPU_PST_CUSTOM_P3   (0x001E) //Custom Pstate3
#define IDSNVID_CPU_COF_P3   (0x001F) //  Frequency (MHz)
#define IDSNVID_CPU_VOLTAGE_P3   (0x0020) //  Voltage (uV)
#define IDSNVID_CPU_PST3_FID   (0x0021) //  Pstate3 FID
#define IDSNVID_CPU_PST3_DID   (0x0022) //  Pstate3 DID
#define IDSNVID_CPU_PST3_VID   (0x0023) //  Pstate3 VID
#define IDSNVID_CPU_PST_CUSTOM_P4   (0x0024) //Custom Pstate4
#define IDSNVID_CPU_COF_P4   (0x0025) //  Frequency (MHz)
#define IDSNVID_CPU_VOLTAGE_P4   (0x0026) //  Voltage (uV)
#define IDSNVID_CPU_PST4_FID   (0x0027) //  Pstate4 FID
#define IDSNVID_CPU_PST4_DID   (0x0028) //  Pstate4 DID
#define IDSNVID_CPU_PST4_VID   (0x0029) //  Pstate4 VID
#define IDSNVID_CPU_PST_CUSTOM_P5   (0x002A) //Custom Pstate5
#define IDSNVID_CPU_COF_P5   (0x002B) //  Frequency (MHz)
#define IDSNVID_CPU_VOLTAGE_P5   (0x002C) //  Voltage (uV)
#define IDSNVID_CPU_PST5_FID   (0x002D) //  Pstate5 FID
#define IDSNVID_CPU_PST5_DID   (0x002E) //  Pstate5 DID
#define IDSNVID_CPU_PST5_VID   (0x002F) //  Pstate5 VID
#define IDSNVID_CPU_PST_CUSTOM_P6   (0x0030) //Custom Pstate6
#define IDSNVID_CPU_COF_P6   (0x0031) //  Frequency (MHz)
#define IDSNVID_CPU_VOLTAGE_P6   (0x0032) //  Voltage (uV)
#define IDSNVID_CPU_PST6_FID   (0x0033) //  Pstate6 FID
#define IDSNVID_CPU_PST6_DID   (0x0034) //  Pstate6 DID
#define IDSNVID_CPU_PST6_VID   (0x0035) //  Pstate6 VID
#define IDSNVID_CPU_PST_CUSTOM_P7   (0x0036) //Custom Pstate7
#define IDSNVID_CPU_COF_P7   (0x0037) //  Frequency (MHz)
#define IDSNVID_CPU_VOLTAGE_P7   (0x0038) //  Voltage (uV)
#define IDSNVID_CPU_PST7_FID   (0x0039) //  Pstate7 FID
#define IDSNVID_CPU_PST7_DID   (0x003A) //  Pstate7 DID
#define IDSNVID_CPU_PST7_VID   (0x003B) //  Pstate7 VID
//Core/Thread Enablement
#define IDSNVID_CPU_CORE_THREAD_LEGAL_DISCLAIMER   (0x003C) //Core/Thread Enablement Legal Disclaimer
//Disagree
//Agree
#define IDSNVID_CMN_CPU_GEN_DOWNCORE_CTRL   (0x003D) //Downcore control
//Prefetcher settings
#define IDSNVID_CMN_CPU_L1_STREAM_HW_PREFETCHER   (0x003E) //L1 Stream HW Prefetcher
#define IDSNVID_CMN_CPU_L2_STREAM_HW_PREFETCHER   (0x003F) //L2 Stream HW Prefetcher
//DF Common Options
#define IDSNVID_DF_CMN_DRAM_SCRUB_TIME   (0x0040) //DRAM scrub time
#define IDSNVID_DF_CMN_REDIR_SCRUB_CTRL   (0x0041) //Redirect scrubber control
#define IDSNVID_DF_CMN_SYNC_FLOOD_PROP   (0x0042) //Disable DF sync flood propagation
#define IDSNVID_DF_CMN_FREEZE_QUEUE_ERROR   (0x0043) //Freeze DF module queues on error
#define IDSNVID_DF_CMN_GMI_ENCRYPTION   (0x0044) //GMI encryption control
#define IDSNVID_DF_CMN_X_GMI_ENCRYPTION   (0x0045) //xGMI encryption control
#define IDSNVID_DF_CMN_CC6_MEM_ENCRYPTION   (0x0046) //CC6 memory region encryption
#define IDSNVID_DF_CMN_CC6_ALLOCATION_SCHEME   (0x0047) //Location of private memory regions
#define IDSNVID_DF_CMN_SYS_PROBE_FILTER   (0x0048) //System probe filter
#define IDSNVID_DF_CMN_MEM_INTLV   (0x0049) //Memory interleaving
#define IDSNVID_DF_CMN_MEM_INTLV_SIZE   (0x004A) //Memory interleaving size
#define IDSNVID_DF_CMN_CHNL_INTLV_HASH   (0x004B) //Channel interleaving hash
#define IDSNVID_DF_CMN_MEM_CLEAR   (0x004C) //Memory Clear
//UMC Common Options
//DDR4 Common Options
//DRAM Timing Configuration
#define IDSNVID_CMN_MEM_TIMING_LEGAL_DISCLAIMER   (0x004D) //DRAM Timing Configuration Legal Disclaimer
#define IDSNVID_CMN_MEM_TIMING_LEGAL_DISCLAIMER1   (0x004E) //DRAM Timing Configuration Legal Disclaimer 1
//I Decline
//I Accept
#define IDSNVID_CMN_MEM_OVERCLOCK_DDR4   (0x004F) //Overclock
#define IDSNVID_CMN_MEM_SPEED_DDR4   (0x0050) //Memory Clock Speed
#define IDSNVID_CMN_MEM_TIMING_TCL_DDR4   (0x0051) //Tcl
#define IDSNVID_CMN_MEM_TIMING_TRCDRD_DDR4   (0x0052) //Trcdrd
#define IDSNVID_CMN_MEM_TIMING_TRCDWR_DDR4   (0x0053) //Trcdwr
#define IDSNVID_CMN_MEM_TIMING_TRP_DDR4   (0x0054) //Trp
#define IDSNVID_CMN_MEM_TIMING_TRAS_DDR4   (0x0055) //Tras
#define IDSNVID_CMN_MEM_TIMING_TRC_CTRL_DDR4   (0x0056) //Trc Ctrl
#define IDSNVID_CMN_MEM_TIMING_TRC_DDR4   (0x0057) //Trc
#define IDSNVID_CMN_MEM_TIMING_TRRD_S_DDR4   (0x0058) //TrrdS
#define IDSNVID_CMN_MEM_TIMING_TRRD_L_DDR4   (0x0059) //TrrdL
#define IDSNVID_CMN_MEM_TIMING_TFAW_CTRL_DDR4   (0x005A) //Tfaw Ctrl
#define IDSNVID_CMN_MEM_TIMING_TFAW_DDR4   (0x005B) //Tfaw
#define IDSNVID_CMN_MEM_TIMING_TWTR_S_DDR4   (0x005C) //TwtrS
#define IDSNVID_CMN_MEM_TIMING_TWTR_L_DDR4   (0x005D) //TwtrL
#define IDSNVID_CMN_MEM_TIMING_TWR_CTRL_DDR4   (0x005E) //Twr Ctrl
#define IDSNVID_CMN_MEM_TIMING_TWR_DDR4   (0x005F) //Twr
#define IDSNVID_CMN_MEM_TIMING_TRCPAGE_CTRL_DDR4   (0x0060) //Trcpage Ctrl
#define IDSNVID_CMN_MEM_TIMING_TRCPAGE_DDR4   (0x0061) //Trcpage
#define IDSNVID_CMN_MEM_TIMING_TRDRD_SC_L_CTRL_DDR4   (0x0062) //TrdrdScL Ctrl
#define IDSNVID_CMN_MEM_TIMING_TRDRD_SC_L_DDR4   (0x0063) //TrdrdScL
#define IDSNVID_CMN_MEM_TIMING_TWRWR_SC_L_CTRL_DDR4   (0x0064) //TwrwrScL Ctrl
#define IDSNVID_CMN_MEM_TIMING_TWRWR_SC_L_DDR4   (0x0065) //TwrwrScL
#define IDSNVID_CMN_MEM_TIMING_TRFC_CTRL_DDR4   (0x0066) //Trfc Ctrl
#define IDSNVID_CMN_MEM_TIMING_TRFC_DDR4   (0x0067) //Trfc
#define IDSNVID_CMN_MEM_TIMING_TRFC2_CTRL_DDR4   (0x0068) //Trfc2 Ctrl
#define IDSNVID_CMN_MEM_TIMING_TRFC2_DDR4   (0x0069) //Trfc2
#define IDSNVID_CMN_MEM_TIMING_TRFC4_CTRL_DDR4   (0x006A) //Trfc4 Ctrl
#define IDSNVID_CMN_MEM_TIMING_TRFC4_DDR4   (0x006B) //Trfc4
#define IDSNVID_CMN_MEM_CTRLLER_PROC_ODT_DDR4   (0x006C) //ProcODT
#define IDSNVID_CMN_MEM_TIMING_TCWL_DDR4   (0x006D) //Tcwl
#define IDSNVID_CMN_MEM_TIMING_TRTP_DDR4   (0x006E) //Trtp
#define IDSNVID_CMN_MEM_TIMING_TRDWR_DDR4   (0x006F) //Trdwr
#define IDSNVID_CMN_MEM_TIMING_TWRRD_DDR4   (0x0070) //Twrrd
#define IDSNVID_CMN_MEM_TIMING_TWRWR_SC_DDR4   (0x0071) //TwrwrSc
#define IDSNVID_CMN_MEM_TIMING_TWRWR_SD_DDR4   (0x0072) //TwrwrSd
#define IDSNVID_CMN_MEM_TIMING_TWRWR_DD_DDR4   (0x0073) //TwrwrDd
#define IDSNVID_CMN_MEM_TIMING_TRDRD_SC_DDR4   (0x0074) //TrdrdSc
#define IDSNVID_CMN_MEM_TIMING_TRDRD_SD_DDR4   (0x0075) //TrdrdSd
#define IDSNVID_CMN_MEM_TIMING_TRDRD_DD_DDR4   (0x0076) //TrdrdDd
#define IDSNVID_CMN_MEM_TIMING_TCKE_DDR4   (0x0077) //Tcke
//DRAM Controller Configuration
#define IDSNVID_CMN_MEM_CTRLLER2_T_MODE_DDR4   (0x0078) //Cmd2T
#define IDSNVID_CMN_MEM_GEAR_DOWN_MODE_DDR4   (0x0079) //Gear Down Mode
//DRAM Power Options
#define IDSNVID_CMN_MEM_CTRLLER_PWR_DN_EN_DDR4   (0x007A) //Power Down Enable
//Data Bus Configuration
#define IDSNVID_CMN_MEM_DATA_BUS_CONFIG_CTL_DDR4   (0x007B) //Data Bus Configuration User Controls
#define IDSNVID_CMN_MEM_CTRLLER_RTT_NOM_DDR4   (0x007C) //RttNom
#define IDSNVID_CMN_MEM_CTRLLER_RTT_WR_DDR4   (0x007D) //RttWr
#define IDSNVID_CMN_MEM_CTRLLER_RTT_PARK_DDR4   (0x007E) //RttPark
//Common RAS
#define IDSNVID_CMN_MEM_DATA_POISONING_DDR4   (0x007F) //Data Poisoning
//ECC Configuration
#define IDSNVID_CMN_MEM_CTRLLER_DRAM_ECC_SYMBOL_SIZE_DDR4   (0x0080) //DRAM ECC Symbol Size
#define IDSNVID_CMN_MEM_CTRLLER_DRAM_ECC_EN_DDR4   (0x0081) //DRAM ECC Enable
//Security
#define IDSNVID_CMN_MEM_TSME_DDR4   (0x0082) //TSME
#define IDSNVID_CMN_MEM_CTRLLER_DATA_SCRAMBLE_DDR4   (0x0083) //Data Scramble
//DRAM Memory Mapping
#define IDSNVID_CMN_MEM_MAPPING_BANK_INTERLEAVE_DDR4   (0x0084) //Chipselect Interleaving
#define IDSNVID_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_DDR4   (0x0085) //BankGroupSwap
#define IDSNVID_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_ALT_DDR4   (0x0086) //BankGroupSwapAlt
#define IDSNVID_CMN_MEM_ADDRESS_HASH_BANK_DDR4   (0x0087) //Address Hash Bank
#define IDSNVID_CMN_MEM_ADDRESS_HASH_CS_DDR4   (0x0088) //Address Hash CS
//Memory MBIST
#define IDSNVID_CMN_MEM_MBIST_EN   (0x0089) //MBIST Enable
#define IDSNVID_CMN_MEM_MBIST_TESTMODE   (0x008A) //MBIST Test Mode
#define IDSNVID_CMN_MEM_MBIST_AGGRESSORS   (0x008B) //MBIST Aggressors
#define IDSNVID_CMN_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT   (0x008C) //MBIST Per Bit Slave Die Reporting
//NBIO Common Options
#define IDSNVID_CMN_GNB_SMU_SYSTEM_CONFIG   (0x008D) //System Configuration
#define IDSNVID_DBG_POISON_CONSUMPTION   (0x008E) //NBIO Internal Poison Consumption
#define IDSNVID_DBG_RAS_CONTROL   (0x008F) //NBIO RAS Control
#define IDSNVID_CMN_DETERMINISM_SLIDER   (0x0090) //Determinism Slider
#define IDSNVID_CMNC_TDP_CTL   (0x0091) //cTDP Control
#define IDSNVID_CMNC_TDP_LIMIT   (0x0092) //cTDP
#define IDSNVID_CMN_NBIO_EFFICIENCY_OPTIMIZED_MODE   (0x0093) //Efficiency Optimized Mode
#define IDSNVID_CMN_NBIO_PSI_DISABLE   (0x0094) //PSI
#define IDSNVID_DBG_GNB_DBG_ACS_ENABLE   (0x0095) //ACS Enable
#define IDSNVID_GNB_DBG_PCIE_ARI_SUPPORT   (0x0096) //PCIe ARI Support
#define IDSNVID_CMN_CLDO_VDDP_CTL   (0x0097) //CLDO_VDDP Control
#define IDSNVID_CMN_CLDOVDD_PVOLTAGE   (0x0098) //CLDO_VDDP voltage
#define IDSNVID_CMN_GNB_HD_AUDIO_EN   (0x0099) //HD Audio Enable
#define IDSNVID_CFG_PCIE_LOOPBACK_MODE   (0x009A) //Block PCIe Loopback
#define IDSNVID_CFG_PCIE_CRS_DELAY   (0x009B) //CRS Delay
#define IDSNVID_CFG_PCIE_CRS_LIMIT   (0x009C) //CRS Limit
//NB Configuration
#define IDSNVID_CMN_GNB_NB_IOMMU   (0x009D) //IOMMU
#define IDSNVID_PCIE_SYNC_RESET   (0x009E) //Concurrent Training
//Fan Control
#define IDSNVID_DBG_FAN_CTL   (0x009F) //Fan Control
#define IDSNVID_DBG_FORCE_PWM_CTL   (0x00A0) //Force PWM Control
#define IDSNVID_DBG_FORCE_PWM   (0x00A1) //Force PWM
#define IDSNVID_DBG_FAN_TABLE_CTL   (0x00A2) //Fan Table Control
#define IDSNVID_DBG_FAN_TABLE_TEMP_LOW   (0x00A3) //Low Temperature
#define IDSNVID_DBG_FAN_TABLE_TEMP_MED   (0x00A4) //Medium Temperature
#define IDSNVID_DBG_FAN_TABLE_TEMP_HIGH   (0x00A5) //High Temperature
#define IDSNVID_DBG_FAN_TABLE_TEMP_CRITICAL   (0x00A6) //Critical Temperature
#define IDSNVID_DBG_FAN_TABLE_T_PWM_LOW   (0x00A7) //Low Pwm
#define IDSNVID_DBG_FAN_TABLE_PWM_MED   (0x00A8) //Medium Pwm
#define IDSNVID_DBG_FAN_TABLE_PWM_HIGH   (0x00A9) //High Pwm
#define IDSNVID_DBG_FAN_TABLE_HYST   (0x00AA) //Temperature Hysteresis
#define IDSNVID_DBG_FAN_TABLE_PWM_FREQ   (0x00AB) //Pwm Frequency
#define IDSNVID_DBG_FAN_POLARITY   (0x00AC) //Fan Polarity
//Hot Plug flags
#define IDSNVID_CMN_DISABLE_SIDEBAND   (0x00AD) //Ignore sideband
#define IDSNVID_CMN_DISABLE_L1WA   (0x00AE) //Disable L1 w/a
#define IDSNVID_CMN_DISABLE_BRIDGE_DIS   (0x00AF) //Disable BridgeDis
#define IDSNVID_CMN_DISABLE_IRQ_POLL   (0x00B0) //Disable irq polling
#define IDSNVID_CMN_IRQ_SETS_BRIDGE_DIS   (0x00B1) //IRQ sets BridgeDis
//FCH Common Options
//SATA Configuration Options
#define IDSNVID_CMN_FCH_SATA_ENABLE   (0x00B2) //SATA Controller
#define IDSNVID_CMN_FCH_SATA_CLASS   (0x00B3) //SATA Mode
#define IDSNVID_CMN_FCH_SATA_RAS_SUPPORT   (0x00B4) //Sata RAS Support
#define IDSNVID_CMN_FCH_SATA_AHCI_DIS_PREFETCH_FUNCTION   (0x00B5) //Sata Disabled AHCI Prefetch Function
#define IDSNVID_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P0   (0x00B6) //Aggresive SATA Device Sleep Port 0
#define IDSNVID_DBG_FCH_SATA_DEV_SLP_PORT0_NUM   (0x00B7) //DevSleep0 Port Number
#define IDSNVID_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P1   (0x00B8) //Aggresive SATA Device Sleep Port 1
#define IDSNVID_DBG_FCH_SATA_DEV_SLP_PORT1_NUM   (0x00B9) //DevSleep1 Port Number
//USB Configuration Options
#define IDSNVID_CMN_FCH_USB_XHC_I0_ENABLE   (0x00BA) //XHCI controller enable
//MCM USB enable
#define IDSNVID_CMN_FCH_USB_XHC_I1_ENABLE   (0x00BB) //XHCI Controller1 enable (Die1)
#define IDSNVID_CMN_FCH_USB_XHC_I2_ENABLE   (0x00BC) //XHCI2 enable (MCM1/Die0)
#define IDSNVID_CMN_FCH_USB_XHC_I3_ENABLE   (0x00BD) //XHCI3 enable (MCM1/Die1)
//SD (Secure Digital) Options
#define IDSNVID_CMN_FCH_SD_CONFIG   (0x00BE) //SD Configuration Mode
//I2C Configuration Options
#define IDSNVID_CMN_FCH_I2_C0_CONFIG   (0x00C0) //I2C 0 Enable
#define IDSNVID_CMN_FCH_I2_C1_CONFIG   (0x00C1) //I2C 1 Enable
#define IDSNVID_CMN_FCH_I2_C2_CONFIG   (0x00C2) //I2C 2 Enable
#define IDSNVID_CMN_FCH_I2_C3_CONFIG   (0x00C3) //I2C 3 Enable
#define IDSNVID_CMN_FCH_I2_C4_CONFIG   (0x00C4) //I2C 4 Enable
#define IDSNVID_CMN_FCH_I2_C5_CONFIG   (0x00C5) //I2C 5 Enable
//Uart Configuration Options
#define IDSNVID_CMN_FCH_UART0_CONFIG   (0x00C6) //Uart 0 Enable
#define IDSNVID_CMN_FCH_UART0_LEGACY_CONFIG   (0x00C7) //Uart 0 Legacy Options
#define IDSNVID_CMN_FCH_UART1_CONFIG   (0x00C8) //Uart 1 Enable
#define IDSNVID_CMN_FCH_UART1_LEGACY_CONFIG   (0x00C9) //Uart 1 Legacy Options
#define IDSNVID_CMN_FCH_UART2_CONFIG   (0x00CA) //Uart 2 Enable (no HW FC)
#define IDSNVID_CMN_FCH_UART2_LEGACY_CONFIG   (0x00CB) //Uart 2 Legacy Options
#define IDSNVID_CMN_FCH_UART3_CONFIG   (0x00CC) //Uart 3 Enable (no HW FC)
#define IDSNVID_CMN_FCH_UART3_LEGACY_CONFIG   (0x00CD) //Uart 3 Legacy Options
//ESPI Configuration Options
#define IDSNVID_CMN_FCH_ESPI_CONFIG   (0x00CE) //ESPI Enable
//Promontory Common Options
//PT SATA Configuration Options
#define IDSNVID_CMN_PT_SATA_PORT_ENABLE   (0x00CF) //PT SATA Port Enable
#define IDSNVID_CMN_PT_SATA_CLASS   (0x00D0) //PT SATA Mode
#define IDSNVID_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P0   (0x00D1) //PT Aggresive SATA Device Sleep Port 0
#define IDSNVID_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P1   (0x00D2) //PT Aggresive SATA Device Sleep Port 1
//PT USB Configuration Options
#define IDSNVID_DBG_PT_XHCI_GEN1   (0x00D3) //PT XHCI GEN1
#define IDSNVID_DBG_PT_XHCI_GEN2   (0x00D4) //PT XHCI GEN2
#define IDSNVID_DBG_PT_USB_EQUALIZATION4   (0x00D5) //PT USB Equalization4
#define IDSNVID_DBG_PT_USB_REDRIVER   (0x00D6) //PT USB Redriver
//PROM4 USB Port Configuration Options
#define IDSNVID_DBG_PT_USB31_P0   (0x00D7) //PT USB31 PORT0
#define IDSNVID_DBG_PT_USB31_P1   (0x00D8) //PT USB31 PORT1
#define IDSNVID_DBG_PT_USB30_P0   (0x00D9) //PT USB30 PORT0
#define IDSNVID_DBG_PT_USB30_P1   (0x00DA) //PT USB30 PORT1
#define IDSNVID_DBG_PT_USB30_P2   (0x00DB) //PT USB30 PORT2
#define IDSNVID_DBG_PT_USB30_P3   (0x00DC) //PT USB30 PORT3
#define IDSNVID_DBG_PT_USB30_P4   (0x00DD) //PT USB30 PORT4
#define IDSNVID_DBG_PT_USB30_P5   (0x00DE) //PT USB30 PORT5
#define IDSNVID_DBG_PT_USB20_P0   (0x00DF) //PT USB20 PORT0
#define IDSNVID_DBG_PT_USB20_P1   (0x00E0) //PT USB20 PORT1
#define IDSNVID_DBG_PT_USB20_P2   (0x00E1) //PT USB20 PORT2
#define IDSNVID_DBG_PT_USB20_P3   (0x00E2) //PT USB20 PORT3
#define IDSNVID_DBG_PT_USB20_P4   (0x00E3) //PT USB20 PORT4
#define IDSNVID_DBG_PT_USB20_P5   (0x00E4) //PT USB20 PORT5
//PROM2 USB Port Configuration Options
#define IDSNVID_DBG_PT_PROM2_USB31_P0   (0x00E5) //PT USB31 PORT0
#define IDSNVID_DBG_PT_PROM2_USB31_P1   (0x00E6) //PT USB31 PORT1
#define IDSNVID_DBG_PT_PROM2_USB30_P0   (0x00E7) //PT USB30 PORT0
#define IDSNVID_DBG_PT_PROM2_USB30_P1   (0x00E8) //PT USB30 PORT1
#define IDSNVID_DBG_PT_PROM2_USB20_P0   (0x00E9) //PT USB20 PORT0
#define IDSNVID_DBG_PT_PROM2_USB20_P1   (0x00EA) //PT USB20 PORT1
#define IDSNVID_DBG_PT_PROM2_USB20_P2   (0x00EB) //PT USB20 PORT2
#define IDSNVID_DBG_PT_PROM2_USB20_P3   (0x00EC) //PT USB20 PORT3
#define IDSNVID_DBG_PT_PROM2_USB20_P4   (0x00ED) //PT USB20 PORT4
#define IDSNVID_DBG_PT_PROM2_USB20_P5   (0x00EE) //PT USB20 PORT5
//PROM1 USB Port Configuration Options
#define IDSNVID_DBG_PT_PROM1_USB31_P0   (0x00EF) //PT USB31 PORT0
#define IDSNVID_DBG_PT_PROM1_USB31_P1   (0x00F0) //PT USB31 PORT1
#define IDSNVID_DBG_PT_PROM1_USB30_P0   (0x00F1) //PT USB30 PORT0
#define IDSNVID_DBG_PT_PROM1_USB20_P0   (0x00F2) //PT USB20 PORT0
#define IDSNVID_DBG_PT_PROM1_USB20_P1   (0x00F3) //PT USB20 PORT1
#define IDSNVID_DBG_PT_PROM1_USB20_P2   (0x00F4) //PT USB20 PORT2
#define IDSNVID_DBG_PT_PROM1_USB20_P3   (0x00F5) //PT USB20 PORT3
#define IDSNVID_DBG_PT_PROM1_USB20_P4   (0x00F6) //PT USB20 PORT4
#define IDSNVID_DBG_PT_PROM1_USB20_P5   (0x00F7) //PT USB20 PORT5
//NTB Common Options
#define IDSNVID_DBG_S_P3_NTB_ENABLE   (0x00F8) //NTB Enable
#define IDSNVID_DBG_S_P3_NTB_LOCATION   (0x00F9) //NTB Location
#define IDSNVID_DBG_S_P3_NTB_PC_IE_CORE   (0x00FA) //NTB active on PCIeCore
#define IDSNVID_DBG_S_P3_NTB_MODE   (0x00FB) //NTB Mode
#define IDSNVID_DBG_S_P3_NTB_LINK_SPEED   (0x00FC) //  Link Speed
#define IDSNVID_DBG_S_P3_NTB_BAR1_SIZE    (0x00FD) //  BAR1 SIZE
#define IDSNVID_DBG_S_P3_NTB_BAR23_SIZE   (0x00FE) //  BAR23 SIEZE
#define IDSNVID_DBG_S_P3_NTB_BAR45_SIZE   (0x00FF) //  BAR45 SIEZE
#define IDSNVID_CMN_CPU_RDSEED_RDRAND_CTRL   (0x0A00) //hzp20170921+ //Hygon definition starts from 0x0A00 to avoid from conflicting  with amd
#define IDSNVID_CMN_CPU_LOAD_UCODE_CONTROL   (0x0A01) //<PS-315+> //Hygon definition starts from 0x0A00 to avoid from conflicting  with amd
#define IDSNVID_CMN_CPU_DISPLAY_RDRAND_RDSEED   (0x0A02) //<PS-625> //Hygon definition starts from 0x0A00 to avoid from conflicting  with amd
#define IDSNVID_CMN_CPU_SMEE_CONTROL   (0x0A03)  //Hygon definition starts from 0x0A00 to avoid from conflicting  with amd
#define IDSNVID_CMN_CPU_SMEE_CAP_CONTROL   (0x0A04)  //Hygon definition starts from 0x0A00 to avoid from conflicting  with amd
#define IDSNVID_CMN_CPU_SVM_CONTROL   (0x0A05)  //Hygon definition starts from 0x0A00 to avoid from conflicting  with amd
#define IDSNVID_CBS_PCIE_HOTPLUG_SUPPORT   (0x0A0D) //PCIE Hotplug
#define IDSNVID_CMN_AVAILABLE_VQ_COUNT (0x0A0E)
//<--- add new available ccp vq count item 2019/12/05
#define IDSNVID_CMN_MEM_CTRLLER_AUTO_REFRESH_RATE_DDR4   (0x0A0F)
#define IDS_NVID_END   (0xfffful) //End of IDS NV ID

#endif //_IDS_NV_ID_ZP_H_
