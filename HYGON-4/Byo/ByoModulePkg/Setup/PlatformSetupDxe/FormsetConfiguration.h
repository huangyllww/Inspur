/*++

Copyright (c) 2010 - 2015, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:

Abstract:
  Platform configuration setup.

Revision History:


--*/

#ifndef _FORMSET_CONFIGURATION_H
#define _FORMSET_CONFIGURATION_H

#include <ByoPlatformSetupConfig.h>
#include <SystemPasswordVariable.h>
#include <Guid/ByoSetupFormsetGuid.h>
#include <ByoPlatformSetupVariable.h>

//
// Form ID, Must be Unique.
//
#define EXIT_KEY_SAVE_EXIT              0x2000
#define EXIT_KEY_SAVE                   0x2001
#define EXIT_KEY_DISCARD                0x2002
#define EXIT_KEY_DEFAULT                0x2003
#define KEY_BIOS_UPDATE                 0x2004
#define KEY_BIOS_UPDATE_ALL             0x2005
#define KEY_BIOS_UPDATE_NETWORK         0x2006
#define KEY_BIOS_UPDATE_NETWORK_ALL     0x2007
#define KEY_SAVE_USER_DEFAULTS_VALUE    0x2008
#define KEY_RESTORE_USER_DEFAULTS_VALUE 0x2009

#define EXIT_KEY_DISCARD_MODIFY         0x2010
#define EXIT_KEY_SAVE_EXIT_AND_RESET    0x2011
#define EXIT_KEY_DISCARD_EXIT_AND_RESET 0x2012

#define KEY_VALUE_CSM_CONFIGURATION     0x2200

#define  SEC_KEY_ADMIN_PD               0x3101
#define  SEC_KEY_POWER_ON_PD            0x3102
#define  SEC_KEY_CLEAR_USER_PD          0x3103
#define  KEY_ADMIN_PASSWORD             0x3104
#define  KEY_POWERON_PASSWORD           0x3105

#define  KEY_USB_DEV_LIST               0x4201
#define  CHIPSET_SB_USB_FORM_ID         0x1001

#define FORM_OPROM_PRIORITY             0x1002
#define EFI_BYO_SETUP_VARIABLE_NAME                  L"ByoSetup"
#define BYO_SETUP_USER_DEFAULT_VARIABLE_NAME         L"ByoSetupUserDefault"
#define BYO_USER_DEFAULT_VARIABLE_NAME               L"ByoUserDefault"

typedef struct {
  // User waiting time
  UINT16  Timeout;
  // language 0x00(unused), 0x80(En), 0x81(Chs)
  UINT8   Language;
} BYO_SETUP_USER_SAVE_DEFEAT;

typedef struct {
  UINT16  Timeout;
} BOOT_TIMEOUT_DATA;

#endif // End,#ifndef _FORMSET_CONFIGURATION_H