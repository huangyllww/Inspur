#ifndef __PROJECT_TOKEN_H__
#define __PROJECT_TOKEN_H__

#define PERF_MODE_HIGH_PERF       1
#define PERF_MODE_HIGH_BALANCE    2
#define PERF_MODE_ENERGY_BALANCE  3
#define PERF_MODE_ENERGY          4
#define PERF_MODE_OEM             0

#define CPU_P0_STATE              0x82
#define CPU_P0P1_STATE            0x81
#define CPU_P0P1P2_STATE          0x80

#define NET_BOOT_IP_NULL          3

//
// Some VendorId DeviceId defination
//

// PMC_SIERRA: aka Microsemi
#define DIDVID_PMC_SIERRA_SWITCH_CARD_BRIDGE             0x406811F8
#define DIDVID_PMC_SIERRA_SWITCH_CARD_MEMORY_CONTROLLER  0xBEEF11F8

#define DIDVID_INSPUR_X722_BRIDGE_1                      0x37C08086
#define DIDVID_INSPUR_X722_MEMORY_CONTROLLER             0x37A08086
#define DIDVID_INSPUR_X722_SIGNAL_PROCESSING_CONTROLLER  0x37B18086

#define DIDVID_CAMBRICON_CABC_370_BRIDGE                 0xC0101000
#define DIDVID_CAMBRICON_CABC_370                        0x0370CABC
#define SUB_DIDVID_CAMBRICON_CABC_370                    0x0054CABC

#define ALIBABA_VENDOR_ID                                0xBABA

// mucse vendor id
#define MUCSE_VENDOR_ID                                  0x8848

//
// DXIO/PCIE configuration MARCOs
//
#define PCIE_CFG_J107_J79_SATA             0x1
#define PCIE_CFG_SANZANG_J38_B_UP_DOWN     0x2
#define PCIE_CFG_SANZANG_J38_B_MIDDLE      0x3
#define PCIE_CFG_SANZANG_8GPU              0x5
#define PCIE_CFG_SANZANG_YZQT_02381_107P1_RETIMER  0x6

#define PCIE_CFG_STORAGE_RISERA_03304      0x7

#define PCIE_CFG_J44_2PCIE                 0x8

#define PCIE_CFG_TIANPENG_J38_B_MIDDLE     0x20
#define PCIE_CFG_TIANPENG_J38_B_UP_DOWN    0x21
#define PCIE_CFG_TIANPENG_J124_J79_D       0x22

#define PCIE_CFG_YangJian_J38_B_UP_DOWN    0x30

#define ONBOARD_NET_CARD_STARTLANE 12


#endif