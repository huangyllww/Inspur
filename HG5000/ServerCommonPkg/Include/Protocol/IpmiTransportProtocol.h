/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  IpmiTransportProtocol.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef _IPMI_TRANSPORT_PROTO_H_
#define _IPMI_TRANSPORT_PROTO_H_

#include "EfiServerManagement.h"

typedef struct _EFI_IPMI_TRANSPORT EFI_IPMI_TRANSPORT;

#define EFI_IPMI_TRANSPORT_PROTOCOL_GUID \
  { \
    0x6bb945e8, 0x3743, 0x433e, 0xb9, 0xe, 0x29, 0xb3, 0xd, 0x5d, 0xc6, 0x30 \
  }

//
// Common Defines
//
typedef UINT32  EFI_BMC_STATUS;

#define EFI_BMC_OK        0
#define EFI_BMC_SOFTFAIL  1
#define EFI_BMC_HARDFAIL  2
#define EFI_BMC_UPDATE_IN_PROGRESS  3

//
//  IPMI Function Prototypes
//
typedef
EFI_STATUS
(EFIAPI *EFI_IPMI_SEND_COMMAND) (
  IN EFI_IPMI_TRANSPORT                * This,
  IN UINT8                             NetFunction,
  IN UINT8                             Lun,
  IN UINT8                             Command,
  IN UINT8                             *CommandData,
  IN UINTN                             CommandDataSize,
  OUT UINT8                            *ResponseData,
  OUT UINT8                            *ResponseDataSize
  );

typedef
EFI_STATUS
(EFIAPI *EFI_IPMI_GET_CHANNEL_STATUS) (
  IN EFI_IPMI_TRANSPORT               *This,
  OUT EFI_BMC_STATUS                  *BmcStatus,
  OUT EFI_SM_COM_ADDRESS              *ComAddress
  );

//
// IPMI TRANSPORT PROTOCOL
//
typedef struct _EFI_IPMI_TRANSPORT {
  UINT64                      Revision;
  EFI_IPMI_SEND_COMMAND       SendIpmiCommand;
  EFI_IPMI_GET_CHANNEL_STATUS GetBmcStatus;
  EFI_HANDLE                  IpmiHandle;
  UINT8                       LastCompleteCode;
};

extern EFI_GUID gEfiIpmiTransportProtocolGuid;
extern EFI_GUID gSmmIpmiTransportProtocolGuid;

#endif
