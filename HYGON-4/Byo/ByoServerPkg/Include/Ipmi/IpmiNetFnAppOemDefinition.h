/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  IpmiNetFnAppOemDefinition.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef _IPMI_NETFNAPP_OEM_DEFINITIONS_H_
#define _IPMI_NETFNAPP_OEM_DEFINITIONS_H_


#define SM_BYOSOFT_NETFN_APP      0x3E
#define SM_BYOSOFT_NETFN_SUB_FUN  0x7a

// OEM get/set share link status
// Request:
// Byte1 = parameter selector, 
// 00h = get share link status, no parameter data needed
// 01h = set share link status, one parameter data needed
// Response:
// Byte1 = share link status, 
// 00h = share link disable
// 01h = share link enable
#define SM_BYOSOFT_SHARE_LINK_FUN  0xAE

//
// Hygon Net function definition for App command
//
#define SM_HYGON_NETFN_APP  0x30 //0x3a
// Request:
// Byte1 = Policy Type, 
// 0: Cooling Policy, Others are reserved for future use
// Response:
// Byte1 = Control Mode, 
// 0: Manual Mode, 1: Balance Mode, 2: Silence Mode, 3:Performance Mode, others reserved
#define HYGON_GET_FUN_CONTROL_POLICY 0x0c

// Request:
// Byte1 = Policy Type, 
// 0: Cooling Policy, Others are reserved for future use
// Byte2 = Control Mode
// 0: Manual Mode, 1: Balance Mode, 2: Silence Mode, 3:Performance Mode, others reserved
// Response:
// Byte1 = completion code 
// 0: success
#define HYGON_SET_FUN_CONTROL_POLICY 0xb0 //0x0b

// Request:
// Byte1 = Fan Index(start from 0) 
// 0xFF: All fans, others:reserved
// Byte2 = Fan Duty, 0 - 100
// Response:
// Byte1 = completion code 
// 0: success
#define HYGON_SET_FUN_SPEED 0x0d

// Request:
// none
// Response:
// Byte1 = Cooling Policy
// 0: Manual Mode, 1: Balance Mode, 2: Silence Mode, 3:Performance Mode
// Byte2 = Fan Number
// Byte3~8 = Fan Duty
#define HYGON_GET_FUN_SPEED 0xb0 //0x0e

// Request:
// Byte1 = Operation type
// 0: Open USB Key, 1: Query USB Key, 2: Close USB Key
// Byte2 = Status of Operation for close USB key
// 0: Success, 1: Open USB key device Fail, 2: Open current setting file Fail, 3:Apply setting Fail, 4:Write current setting file Fail
// Response:
// Byte1 = Return Status
// 0: Success, 1: Fail
#define SM_BYOSOFT_NETFN_SUB_REDFISH 0xF0

#define SM_BYOSOFT_NETFN_SUB_REDFISH_OPEN_USB_KEY  0x0
#define SM_BYOSOFT_NETFN_SUB_REDFISH_QUERY_USB_KEY 0x1
#define SM_BYOSOFT_NETFN_SUB_REDFISH_CLOSE_USB_KEY 0x2

#define BYOSOFT_NETFN_SET_BIOS_CRASH             0x1C
#define BYOSOFT_SET_BIOS_CRASHED_CMD             0x1
#define BYOSOFT_CLEAR_BIOS_CRASHED_CMD           0x0
#define BYOSOFT_NETFN_GET_BIOS_CRASH             0x1D
#define BYOSOFT_RESPONSE_BIOS_CRASHED            0x1
#define BYOSOFT_RESPONSE_BIOS_OK                 0x0

#define BYOSOFT_NETFN_SWITCH_BIOS_TYPE           0x1E
#define BYOSOFT_SET_MASTER_BIOS_CMD              0x0
#define BYOSOFT_SET_SLAVE_BIOS_CMD               0x1
#define BYOSOFT_NETFN_GET_CURRENT_BIOS_TYPE      0x1F
#define BYOSOFT_RESPONSE_MASTER_BIOS             0x81
#define BYOSOFT_RESPONSE_SLAVE_BIOS              0x80

#define BYOSOFT_NETFN_SET_WDT_RETRY_TIMES        0xF2
#define BYOSOFT_NETFN_GET_WDT_RETRY_TIMES        0xF3

#define BYOSOFT_NETFN_SET_BIOS_PHYSICAL_DAMAGE   0xF6
#define BYOSOFT_SET_BIOS_PHYSICAL_DAMAGE_CMD     0x1
#define BYOSOFT_CLEAR_BIOS_PHYSICAL_DAMAGE_CMD   0x0
#define BYOSOFT_NETFN_GET_BIOS_PHYSICAL_DAMAGE   0xF7
#define BYOSOFT_RESPONSE_BIOS_PHYSICAL_DAMAGE    0x1
#define BYOSOFT_RESPONSE_BIOS_PHYSICAL_OK        0x0

#define BYOSOFT_NETFN_STOP_PEI_RUNNING_FLAG      0xF8
#define BYOSOFT_NETFN_GET_PEI_RUNNING_FLAG       0xF9

#endif
