/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  DhcpGetNetworkId.h

Abstract:
  DhcpGetNetworkId driver.

Revision History:

**/



#ifndef _DHCP_GET_NETWORK_ID_H_
#define _DHCP_GET_NETWORK_ID_H_

#include <Uefi.h>
#include <IndustryStandard/Dhcp.h>
#include <Pi/PiPeiCis.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/NetLib.h>
#include <Library/PrintLib.h>
#include <Library/ByoCommLib.h>
#include <Protocol/Dhcp4.h>
#include <Protocol/SimpleNetwork.h>
#include <Protocol/NetworkInterfaceIdentifier.h>
#include <Protocol/ByoNetworkIdentifierProtocol.h>
#include <Protocol/Ip4Config2.h>
#include <Protocol/SimpleNetwork.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <DhcpGetNetworldCfg.h>
#include <Library/HiiLib.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/ByoPlatformSetupProtocol.h>
#include <Protocol/SetupItemUpdate.h>
#include <Library/DevicePathLib.h>
#include "DhcpGetNetworldCfg.h"
#include <SysMiscCfg.h>
#include <Library/SetupUiLib.h>
#include <Library/UefiHiiServicesLib.h>


#define DHCP_OPTION_MAGIC            0x63538263 // Network byte order
#define DHCP4_OPCODE_REQUEST         1

//
// DHCP message types
//
#define DHCP_MSG_DISCOVER       1
#define DHCP_MSG_OFFER          2
#define DHCP_MSG_REQUEST        3
#define DHCP_MSG_DECLINE        4
#define DHCP_MSG_ACK            5
#define DHCP_MSG_NAK            6
#define DHCP_MSG_RELEASE        7
#define DHCP_MSG_INFORM         8


#define DHCP_SERVER_PORT                  67
#define DHCP_CLIENT_PORT                  68

#define GET_NEXT_DHCP_OPTION(Opt) \
  (EFI_DHCP4_PACKET_OPTION *) ((UINT8 *) (Opt) + \
   sizeof (EFI_DHCP4_PACKET_OPTION) + (Opt)->Length - 1)

#define DHCP_NETWORKID_VENDOR_CLASS_TAG  "$DhcpNetworkID$"

#pragma pack(1)

typedef struct {
  UINT8 Type;
} DHCP4_OPTION_MESG;

typedef struct {
  UINT8 ParaList[32];
} DHCP4_OPTION_PARA;

typedef struct {
  UINT8 ClassIdentifier[16];
} DHCP4_OPTION_CLID;

#pragma pack()

typedef union {
  DHCP4_OPTION_MESG           *Mesg;
  DHCP4_OPTION_PARA           *Para;
  DHCP4_OPTION_CLID           *Clid;
} DHCP4_OPTION_ENTRY;


typedef struct _DHCP_NETWORK_ID_PRIVATE_DATA {
  UINT32                                    Signature;

  EFI_HANDLE                                Controller;
  EFI_HANDLE                                DriverBindingHandle;
  EFI_HANDLE                                Dhcp4Child;

  EFI_IP4_CONFIG2_PROTOCOL                  *Ip4Config2;
  EFI_DHCP4_PROTOCOL                        *Dhcp4;
  EFI_SIMPLE_NETWORK_PROTOCOL               *Snp;

  EFI_DHCP4_MODE_DATA                       Dhcp4Mode;
  EFI_IP4_CONFIG2_POLICY                    Policy;

  EFI_IP_ADDRESS                            StationIp;   // Client IP
  EFI_IP_ADDRESS                            SubnetMask;  // Client IP Subnet Mask
  EFI_IP_ADDRESS                            GatewayIp;
  EFI_IP_ADDRESS                            ServerIp;    // DHCP Server IP

  EFI_DHCP4_PACKET                          SeedPacket;

  UINT8                                     *ServerNetworkId[16];
  UINTN                                     NetworkIdNum;
} DHCP_NETWORK_ID_PRIVATE_DATA;

EFI_STATUS
EFIAPI
DhcpGetNetworkId(
  OUT UINT8     *NetworkId,
  IN OUT UINT8  *NetworkIdLen
  );

EFI_STATUS
Dhcp4Config (
  IN DHCP_NETWORK_ID_PRIVATE_DATA       *Private
  );

EFI_STATUS
Dhcp4NetworkIdSendRecv (
  IN  DHCP_NETWORK_ID_PRIVATE_DATA         *gPrivate
  );
#endif
