/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ByoNetworkIdentifierProtocol.h

Abstract:
  header file for DhcpGetNetworkId driver.

Revision History:

**/



#ifndef _BYO_NETWORK_IDENTIFIER_PROTOCOL_H_
#define _BYO_NETWORK_IDENTIFIER_PROTOCOL_H_

#define NETWORK_ID_MAX_SIZE                   0x80

#define NETWORK_IDENTIFIER_PROTOCOL_GUID \
  { 0x1145c186, 0xe61b, 0x490a, { 0xb1, 0x8f, 0xe3, 0xf3, 0x9f, 0xa7, 0x53, 0xf }}


/**
 * @brief   This function reports a Network Identifier for the node calling the function.
 *          This NetworkId is usually part of the cryptographic hash generation.
 * 
 * @param   NetworkId[in]         A pointer to the destination NetworkId.
 * @param   NetworkIdLen[in out]  A pointer to the destination NetworkId Length.
 * @retval  EFI_SUCCESS          The NetworkId is successfully obtained. 
 * 
**/
typedef
EFI_STATUS
(EFIAPI *GET_NETWORK_IDENTIFIER)(
  OUT UINT8     *NetworkId,
  IN OUT UINT8  *NetworkIdLen
  );

/**
 * @brief    Define a structure for the Network Identifier report protocol
 * 
 * @memberof GET_NETWORK_IDENTIFIER   This member points to the Get NetworkIdentifier reporting instance.
 * 
**/
typedef struct {
  GET_NETWORK_IDENTIFIER   GetNetworkId;
} NETWORK_IDENTIFIER_PROTOCOL;

#endif
