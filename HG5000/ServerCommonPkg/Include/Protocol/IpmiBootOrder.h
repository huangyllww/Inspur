/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  KcsBmc.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef __EFI_IPMIBOOT_PROTOCOL_H__
#define __EFI_IPMIBOOT_PROTOCOL_H__

#define EFI_IPMI_BOOT_GUID \
  { \
    0x5c9b75ec, 0x8ec7, 0x45f2, 0x8f, 0x8f, 0xc1, 0xd8, 0x8f, 0x3b, 0x93, 0x45 } \
  }

typedef EFI_STATUS (*PROCESS_BOOT_FLAGS) ( IN VOID );
typedef EFI_STATUS (*PROCESS_BOOT_ORDER_TABLE) ( IN VOID );
typedef EFI_STATUS (*PROCESS_F2_BOOT_ORDER_TABLE) ( IN VOID );

typedef struct _IPMI_BOOT_PROTOCOL { 
  PROCESS_BOOT_FLAGS          ProcessBootFlags;
  PROCESS_BOOT_ORDER_TABLE    ProcessBootOrderTable;
  PROCESS_F2_BOOT_ORDER_TABLE    ProcessF2BOT;
} IPMI_BOOT_PROTOCOL;

extern EFI_GUID gEfiIpmiBootGuid;

#endif
