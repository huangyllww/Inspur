
#ifndef __SYS_MISC_CFG__
#define __SYS_MISC_CFG__


#define SYS_MISC_CFG_DUAL_VGA                   BIT0
#define SYS_MISC_CFG_PRI_VGA_IGD                BIT1
#define SYS_MISC_CFG_PCI64                      BIT2
#define SYS_MISC_CFG_SHELL_EN                   BIT3
#define SYS_MISC_NVME_ADDON_OPROM               BIT4
#define SYS_MISC_SATA_MODE                      BIT5
#define SYS_MISC_SATA_MODE_BIT_SHIFT            5
#define SYS_MISC_SATA_MODE_                     BIT6

#define SYS_MISC_CFG_CONNECT_ALL                BIT16
#define SYS_MISC_CFG_UPD_BOOTORDER              BIT17
#define SYS_MISC_CFG_IGD_PRESENT                BIT18
#define SYS_MISC_CFG_DIS_SHOW_LOGO              BIT19

#define SYS_MISC_CFG_STS_PS2KB                  BIT20
#define SYS_MISC_CFG_STS_PS2MS                  BIT21
#define SYS_MISC_CFG_STS_NO_KB                  BIT22

#define SYS_MISC_CFG_NO_USB_BOOT                BIT23

#define SYS_MISC_CFG_STS_ONLY_VIRTUAL_USB_KB    BIT24


#define NET_BOOT_IP_ALL                         0
#define NET_BOOT_IP_V4                          1
#define NET_BOOT_IP_V6                          2

#define SATA_MODE_IDE                           0
#define SATA_MODE_AHCI                          1
#define SATA_MODE_RAID                          2

#endif

