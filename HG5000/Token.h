
#ifndef __TOKEN_H__
#define __TOKEN_H__

//
// Define BIOS Customer ID
//
#define BCID_NORMAL    0
#define BCID_TENCENT    1
#define BCID_INSPUR_ST    2


#include "BCID.h"

#define BC_NORMAL (BCID == BCID_NORMAL)
#define BC_TENCENT (BCID == BCID_TENCENT)	
#define BC_INSPUR_ST (BCID == BCID_INSPUR_ST)	

#define RECOVERY_STEP_CMOS_OFFSET       0x74
#define AFTER_FLASH_SPI                 0x79
#define RECOVERY_STEP_CMOS_LENGTH       1

#define SETUP_DATA_CHANGE_CMOS_OFFSET   0x75
#define SETUP_DATA_CHANGE_CMOS_LENGTH   1





#define TPM_DECODE_NONE          0
#define TPM_DECODE_LPC           1
#define TPM_DECODE_SPI           2




#define NCPU	                   128

#define USE_AP_HLT               1

//#define DIMM_SLOTS_PER_SCKT      16

#define SimNow_SUPPORT           0



#define PSP_NOTIFY_SMM_SW_SMI    0x81

#define MCT_BOTTOM_IO_VALUE	     0x80
#define PCIEX_LENGTH	           0x8000000

#define HYGON_RELEASE_BIOS                            0
#define TKN_BLDCFG_FCH_CONSOLE_OUT_ENABLE             0
#define TKN_BLDCFG_FCH_CONSOLE_OUT_SERIAL_PORT        2   // 1 - SOC UART 0, 2 - SOC UART 1


#define SETUP_CURRENT_VERSION                         5

#define TKN_ONBOARD_LAN_MAX_COUNT                     2
#define CSM_DEFAULT_VALUE                             0
#define INTERNAL_SHELL_DEFAULT_ENABLE                 0
#define SYNC_NVRAM_AT_SMIFLASH_VERIFY                 1

#define SEC_HIGH_MEMORY_REGION_BASE	                	0x1000000

#define FCE_DEFAULT_VALUE_OVERRIDE_DATA

#define TKN_IGD_NOT_PHYSICAL_PRESENT                  0


#define __STR16(a)      L#a
#define TKN2STR16(a)    __STR16(a)


#define HDP_SALT              "90d)Wfdj-"
#define HDP_FIRST_HASH_SIZE   12


// DO NOT change below 3 macro's value!
// Please change UART_SELECTION in OemSet.cmd instead.
#define TKN_UART_SELECTION_CPU_UART0                  0
#define TKN_UART_SELECTION_BMC_UART1                  1
#define TKN_UART_SELECTION_CPU_UART1                  2




             
#define TKN_KEEP_PASSWORD_WHEN_LOAD_FCE               0

#define TKN_IPMI_IO_BASE                              0xCA2

#define TKN_BYO_CBS    1
#define TKN_LOAD_DEFAULT_RTC_IF_CMOS_BAD              1




#pragma pack(1)

#define TKN_LANG_CHS_NAME              "zh-Hans"

#define SETUP_USER_SAVE_VAR_NAME       L"UserDef"
#define USER_BOOT_ORDER_VAR_NAME       L"UserBootOrder"

#define SETUP_USER_SAVE_VAR_FLAG      (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE)

typedef struct {
  UINT8    Language;                   // 0x00(unused), 0x80(En), 0x81(Chs)
  UINT8    ChangePopByUser;            // 1:enable 0:disable
  UINT8    Reserved[15];
} SETUP_USER_SAVE_NV_DATA;





#define BYO_SYS_CFG_STS_SIGNATURE        SIGNATURE_32('B', 'Y', 'C', 'F')

typedef struct {
  UINT32   Signature;
  UINT8    Riser1SlotCount;
  UINT8    Riser2SlotCount;
  UINT8    Riser3SlotCount;
  UINT8    Reserved2[64];
} BYO_SYS_CFG_STS_DATA;



#pragma pack()






#define SPD_MUX_GROUP0_CTRL_ADDR    0xFF      
#define SPD_MUX_GROUP0_CHANNEL      1

#define SPD_MUX_GROUP1_CTRL_ADDR    0xFF      
#define SPD_MUX_GROUP1_CHANNEL      2

#define SPD_MUX_GROUP2_CTRL_ADDR    0xFF      
#define SPD_MUX_GROUP2_CHANNEL      4

#define SPD_MUX_GROUP3_CTRL_ADDR    0xFF      
#define SPD_MUX_GROUP3_CHANNEL      8

#define BLDCFG_I2C_MUX_ADRRESS      0xE0


#define ADD_ON_SETUP_DATA \
  UINT8  Language; \
  UINT8  USBRearCtrl;\
  UINT8  USBFronCtrl;\
  UINT8  AMDMaxCpuFrequency; \
  UINT8  Pci64Limit;  \
  UINT8  PcieMaxPayloadSize; \
  UINT8  PcieAspm; \
  UINT8  PerfMode; \
  UINT8  RetimerCard;\
  UINT8  AfterFlash; \
  UINT8  DealErrorDimm;\
  UINT8  SelectPxeNic;\
  UINT8  PXEBootPriority;\
  UINT8  BIOSLock;\
  UINT8  BiosUpdateParm;\
  UINT16	PxeNicVid;\
  UINT16	PxeNicDid;\
  UINT16	PxeNicMacCrc;\
  UINT8  FtpmSupport;\
  UINT8  DtpmSupport;\
  UINT8  HideBrandLogo;\
  UINT8  Reserved[8]; /*Reserved data default value should be *ZERO* to keep compatible */ \
   


//
// should care about alignment. WORD !!!
//
#define ADD_ON_SETUP_VOLATILE_DATA \
  UINT8  Riser1SlotCount;\
  UINT8  Riser2SlotCount;\
  UINT8  Riser3SlotCount;


#define PCIE_MAX_PAYLOAD_AUTO         0
#define PCIE_MAX_PAYLOAD_128          1
#define PCIE_MAX_PAYLOAD_256          2
#define PCIE_MAX_PAYLOAD_512          3

#define PCIE_ASPM_DISABLE        0
#define PCIE_ASPM_L0S            1
#define PCIE_ASPM_L1             2
#define PCIE_ASPM_L0SL1          3

#endif


