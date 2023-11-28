#ifndef __SETUP__ITEM_ID_H__
#define __SETUP__ITEM_ID_H__

#define PRINTABLE_LANGUAGE_NAME_STRING_ID     0x0001
#define MAIN_MAIN                      4096 
#define ADVANCED_MAIN                  4097 
#define BOOT_MAIN                      4098 
#define CPU_FORM_ID                    4109
#define TEST_FORM_ID                   4110
#define ConsoleRedirection_FORM_ID     4111
#define NVME_FORM_ID                   4112
#define ERRMGMT_FORM_ID                4113
#define PCIE_FORM_ID                   4114
#define PCIE_SLOT0_FORM_ID             4115
#define PCIE_SLOT1_FORM_ID             4116
#define PCIE_SLOT2_FORM_ID             4117
#define PCIE_SLOT3_FORM_ID             4118
#define PCIE_SLOT4_FORM_ID             4119
#define PCIE_SLOT5_FORM_ID             4120
#define PCIE_SLOT6_FORM_ID             4121
#define PCIE_SLOT7_FORM_ID             4122
#define USB_PORT_FORM_ID               4124
#define USB_FORM_ID                    4126
#define USB_0_FORM_ID                  4127
#define USB_1_FORM_ID                  4128
#define USB_2_FORM_ID                  4129
#define USB_3_FORM_ID                  4130
#define SATA_FORM_ID                   4131
#define SATA_CONTROL0_FORM_ID          4132
#define SATA_CONTROL1_FORM_ID          4133
#define SATA_CONTROL2_FORM_ID          4134
#define SATA_CONTROL3_FORM_ID          4135
#define VIDEO_FORM_ID                  4136
#define VIRTUALIZATION_CTRL_FORM_ID    4137

#define  KEY_VALUE_ADVANCED_USBPORT0_VAL  4200
#define  KEY_VALUE_ADVANCED_USBPORT1_VAL  4201
#define  KEY_VALUE_ADVANCED_USBPORT2_VAL  4202
#define  KEY_VALUE_ADVANCED_USBPORT3_VAL  4203
#define  KEY_VALUE_ADVANCED_USBPORT4_VAL  4204
#define  KEY_VALUE_ADVANCED_USBPORT5_VAL  4205
#define  KEY_VALUE_ADVANCED_USBPORT6_VAL  4206
#define  KEY_VALUE_ADVANCED_USBPORT7_VAL  4207
//
#define  KEY_VALUE_ADVANCED_USBCtrl0_VAL  4208
#define  KEY_VALUE_ADVANCED_USBCtrl1_VAL  4209
//

  #define  KEY_USER_ACCESS_LEVEL       4106
  #define  KEY_SAVE_AND_EXIT_VALUE     4353
  #define  KEY_DISCARD_AND_EXIT_VALUE  4354
  #define  KEY_RESTORE_DEFAULTS_VALUE  4355
  #define  KEY_SAVE_VALUE     4456

  #define KEY_SAVE_USER_DEFAULTS_VALUE      8000
  #define KEY_RESTORE_USER_DEFAULTS_VALUE   8001
  #define KEY_BIOS_UPDATE                   8002
  #define KEY_SYS_RESET                     8003
  #define KEY_SYS_SHUTDOWN                  8004
  
  
  #define  SEC_KEY_ADMIN_PD         4356
  #define  SEC_KEY_POWER_ON_PD      4357
  #define  SEC_KEY_CLEAR_USER_PD    4358
  
  #define  KEY_RESET_TO_SETUP_MODE  4360
  #define  KEY_RESTORE_FACTORY_KEYS 4361
  #define  CX_STATE_CTRL_FORM_ID    4362
  #define  KEY_VALUE_PCIERP         4363  

#define BIOS_POWERON_FORM_ID           0x5000
#define ADVANCE_MISC_FORM_ID           0x5100

#define  CHIPSET_MEMORY_FORM_ID        4609
#define  CHIPSET_SATA_FORM_ID          4622
#define  NETWORK_SETUP_FORM_ID         4631

#define MAIN_PAGE_KEY_LANGUAGE         0x2341

#define MAIN_CPU_FORM_ID                0x0002
#define MAIN_MEMORY_FORM_ID             0x0003 
#define MAIN_SYSTEM_SUMMARY_FORM_ID     0x0004
#define MAIN_DATE_TIME_FORM_ID          0x0005
#define MAIN_FIRMWARE_FORM_ID          0x0006
#define PSP_FWVERSION_FORM_ID           0x1006


#define AUTO_ID(x) x

#define LABEL_NVME_DEVICE_LIST         0x1000

#endif
