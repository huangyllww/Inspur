#ifndef _SMU_FEATURE_H_
#define _SMU_FEATURE_H_

// *** IMPORTANT ***
// SMU TEAM: Always increment the interface version if
// any structure is changed in this file
#define SMU_FEATURE_VERSION  0x48000001

#define ENABLE_DEBUG_FEATURES

// Feature Control Defines
#define FEATURE_CCLK_CONTROLLER_BIT     0
#define FEATURE_FAN_CONTROLLER_BIT      1
#define FEATURE_DATA_CALCULATION_BIT    2
#define FEATURE_PPT_BIT                 3
#define FEATURE_TDC_BIT                 4
#define FEATURE_THERMAL_BIT             5
#define FEATURE_FIT_BIT                 6
#define FEATURE_QOS_BIT                 7
#define FEATURE_DF_CSTATES_BIT          8
#define FEATURE_PROCHOT_BIT             9
#define FEATURE_MCM_DATA_TRANSFER_BIT   10
#define FEATURE_DLWM_BIT                11
#define FEATURE_PC6_BIT                 12
#define FEATURE_CSTATE_BOOST_BIT        13
#define FEATURE_VOLTAGE_CONTROLLER_BIT  14
#define FEATURE_HOT_PLUG_BIT            15
#define FEATURE_CC1_BIT                 16
#define FEATURE_CC6_BIT                 17
#define FEATURE_SPARE_18_BIT            18
#define FEATURE_SPARE_19_BIT            19
#define FEATURE_SPARE_20_BIT            20
#define FEATURE_SPARE_21_BIT            21
#define FEATURE_SPARE_22_BIT            22
#define FEATURE_SPARE_23_BIT            23
#define FEATURE_SPARE_24_BIT            24
#define FEATURE_SPARE_25_BIT            25
#define FEATURE_SPARE_26_BIT            26
#define FEATURE_SPARE_27_BIT            27
#define FEATURE_SPARE_28_BIT            28
#define FEATURE_SPARE_29_BIT            29
#define FEATURE_SPARE_30_BIT            30
#define FEATURE_SPARE_31_BIT            31

#define NUM_FEATURES  32

#define FEATURE_CCLK_CONTROLLER_MASK     (1 << FEATURE_CCLK_CONTROLLER_BIT)
#define FEATURE_FAN_CONTROLLER_MASK      (1 << FEATURE_FAN_CONTROLLER_BIT)
#define FEATURE_DATA_CALCULATION_MASK    (1 << FEATURE_DATA_CALCULATION_BIT)
#define FEATURE_PPT_MASK                 (1 << FEATURE_PPT_BIT)
#define FEATURE_TDC_MASK                 (1 << FEATURE_TDC_BIT)
#define FEATURE_THERMAL_MASK             (1 << FEATURE_THERMAL_BIT)
#define FEATURE_FIT_MASK                 (1 << FEATURE_FIT_BIT)
#define FEATURE_QOS_MASK                 (1 << FEATURE_QOS_BIT)
#define FEATURE_DF_CSTATES_MASK          (1 << FEATURE_DF_CSTATES_BIT)
#define FEATURE_PROCHOT_MASK             (1 << FEATURE_PROCHOT_BIT)
#define FEATURE_MCM_DATA_TRANSFER_MASK   (1 << FEATURE_MCM_DATA_TRANSFER_BIT)
#define FEATURE_DLWM_MASK                (1 << FEATURE_DLWM_BIT)
#define FEATURE_PC6_MASK                 (1 << FEATURE_PC6_BIT)
#define FEATURE_CSTATE_BOOST_MASK        (1 << FEATURE_CSTATE_BOOST_BIT)
#define FEATURE_VOLTAGE_CONTROLLER_MASK  (1 << FEATURE_VOLTAGE_CONTROLLER_BIT)
#define FEATURE_HOT_PLUG_MASK            (1 << FEATURE_HOT_PLUG_BIT)
#define FEATURE_CC1_MASK                 (1 << FEATURE_CC1_BIT)
#define FEATURE_CC6_MASK                 (1 << FEATURE_CC6_BIT)

// These defines are used with the following messages:
// SMC_MSG_TransferTableDram2Smu
// SMC_MSG_TransferTableSmu2Dram
#define TABLE_PPTABLE         0
#define TABLE_PCIE_HP_CONFIG  1
#define TABLE_PMSTATUSLOG     2
#define TABLE_COUNT           3
#endif
