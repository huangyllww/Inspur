/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  DhcpGetNetworldCfg.h

Abstract:
  header file for DhcpGetNetworkId driver.

Revision History:

**/

#ifndef _DHCP_GET_NETWORK_ID_CFG_H
#define _DHCP_GET_NETWORK_ID_CFG_H

#define NETWORK_SECURITY_AUTH_GUID \
{ 0xa4ab7812, 0xf4c6, 0x4652, { 0xad, 0x67, 0x73, 0x14, 0x10, 0x87, 0x5c, 0x7e } }

#define PASSWORD_NETWORK_AUTH_VAR_NAME           L"PwdNetworkAuth"
#define NETWORK_SECURITY_AUTH_FORM_ID            1

#define PWD_NETWORK_AUTH_ENABLE                  0x1001

extern UINT8                                     DhcpGetNetworkIdCfgBin[];

typedef struct {
  UINT8    PwdNetworkAuthEnable;
} PASSWORD_NETWORK_AUTH_CONFIG;

#endif