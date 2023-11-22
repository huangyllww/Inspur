/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

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

#define SYS_MISC_CFG_HDD_SMART_CHECK            BIT25

#define NET_BOOT_IP_ALL                         0
#define NET_BOOT_IP_V4                          1
#define NET_BOOT_IP_V6                          2
#define ENABLE_SPEPARATE_CONTROL_NET_TYPE       BIT2
#define ENABLE_PXE_IPV4                         BIT3
#define ENABLE_PXE_IPV6                         BIT4
#define ENABLE_HTTP_IPV4                        BIT5
#define ENABLE_HTTP_IPV6                        BIT6

#define SATA_MODE_IDE                           0
#define SATA_MODE_AHCI                          1
#define SATA_MODE_RAID                          2

#define ROM_POLICY_LEGACY_FIRST      0
#define ROM_POLICY_UEFI_FIRST        1
#define ROM_POLICY_NO_LAUNCH         2
#define ROM_POLICY_BY_CSM            3


#define BIOS_BOOT_ALL_OS             0
#define BIOS_BOOT_UEFI_OS            1
#define BIOS_BOOT_LEGACY_OS          2

#endif

