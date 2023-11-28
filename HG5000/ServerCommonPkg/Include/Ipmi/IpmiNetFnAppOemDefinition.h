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

//
// Byosoft Net function definition for App command
//
#define SM_BYOSOFT_NETFN_APP      0x3e

// OEM send hardware information to BMC
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
// 0: Cooling Plicy, Others are reserved for future use
// Response:
// Byte1 = Control Mode, 
// 0: Manual Mode, 1: Balance Mode, 2: Silence Mode, 3:Performance Mode, others reserved
#define HYGON_GET_FUN_CONTROL_POLICY 0x0c

// Request:
// Byte1 = Policy Type, 
// 0: Cooling Plicy, Others are reserved for future use
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


#endif
