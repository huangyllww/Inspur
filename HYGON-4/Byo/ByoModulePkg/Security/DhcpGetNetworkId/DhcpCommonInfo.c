/** @file
  Functions implementation related with DHCPv4 for NetworkID Driver.

  Copyright (c) 2022, Byosoft Corporation.<BR>
  All rights reserved.This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.

**/

#include "DhcpGetNetworkId.h"

#define DHCP_TAG_SERVER_NETWORK_ID   0xAA


//
// There are 3 times retries with the value of 4, 8, 16
//
UINT32 mDhcpNetworkIdTimeout[4] = {4, 8, 16};


/**
  EFI_DHCP4_CALLBACK is provided by the consumer of the EFI DHCPv4 Protocol driver
  to intercept events that occurred in the configuration process.

  @param[in]  This              Pointer to the EFI DHCPv4 Protocol.
  @param[in]  Context           Pointer to the context set by EFI_DHCP4_PROTOCOL.Configure().
  @param[in]  CurrentState      The current operational state of the EFI DHCPv4 Protocol driver.
  @param[in]  Dhcp4Event        The event that occurs in the current state, which usually means a
                                state transition.
  @param[in]  Packet            The DHCPv4 packet that is going to be sent or already received.
  @param[out] NewPacket         The packet that is used to replace the above Packet.

  @retval EFI_SUCCESS           Tells the EFI DHCPv4 Protocol driver to continue the DHCP process.
  @retval EFI_NOT_READY         Only used in the Dhcp4Selecting state. The EFI DHCPv4 Protocol
                                driver will continue to wait for more DHCPOFFER packets until the
                                retry timeout expires.
  @retval EFI_ABORTED           Tells the EFI DHCPv4 Protocol driver to abort the current process
                                and return to the Dhcp4Init or Dhcp4InitReboot state.

**/
EFI_STATUS
EFIAPI
Dhcp4NetworkIdCallBack (
  IN  EFI_DHCP4_PROTOCOL               *This,
  IN  VOID                             *Context,
  IN  EFI_DHCP4_STATE                  CurrentState,
  IN  EFI_DHCP4_EVENT                  Dhcp4Event,
  IN  EFI_DHCP4_PACKET                 *Packet            OPTIONAL,
  OUT EFI_DHCP4_PACKET                 **NewPacket        OPTIONAL
  )
{
  DHCP_NETWORK_ID_PRIVATE_DATA    *Private;
  EFI_STATUS                      Status = EFI_SUCCESS;
  UINT8                           *Option;
  UINTN                           OptionLen;

  //
  // Only need to parse ACK in D.O.R.A phase
  //
  if (Dhcp4Event != Dhcp4RcvdAck) {
    return EFI_SUCCESS;
  }

  ASSERT (Packet != NULL);

  Private = (DHCP_NETWORK_ID_PRIVATE_DATA *) Context;
  
  CopyMem (&Private->StationIp, &Packet->Dhcp4.Header.YourAddr, sizeof (EFI_IPv4_ADDRESS));

  Option = Packet->Dhcp4.Option;

  while (Option[0] != DHCP4_TAG_EOP) {
    switch (Option[0]) {
      case DHCP4_TAG_SERVER_ID:
        CopyMem (&Private->ServerIp, &Option[2], sizeof (EFI_IPv4_ADDRESS));
        break;
      case DHCP4_TAG_NETMASK:
        CopyMem (&Private->SubnetMask, &Option[2], sizeof (EFI_IPv4_ADDRESS));
        break;
      case DHCP4_TAG_ROUTER:
        CopyMem (&Private->GatewayIp, &Option[2], sizeof (EFI_IPv4_ADDRESS));
        break;
      default:
        break;
    }

    OptionLen = Option[1];
    Option += OptionLen + 2;
  }

  return Status;
}



/**
  Start the D.O.R.A DHCPv4 process to acquire the IPv4 address. D.O.R.A process also implies selection of DHCP server to query Network Identifier.

  @param[in]  Private           Pointer to PxeBc private data.
  @param[in]  Dhcp4             Pointer to the EFI_DHCP4_PROTOCOL

  @retval EFI_SUCCESS           The D.O.R.A process successfully finished.
  @retval Others                Failed to finish the D.O.R.A process.

**/
EFI_STATUS
Dhcp4Config (
  IN DHCP_NETWORK_ID_PRIVATE_DATA       *Private
  )
{
  EFI_STATUS                   Status;
  EFI_DHCP4_PACKET_OPTION      *OptList[1];
  UINT8                        Buffer[32];
  EFI_DHCP4_CONFIG_DATA        Config;
  EFI_DHCP4_MODE_DATA          Dhcp4Mode;
  EFI_DHCP4_PROTOCOL           *Dhcp4;

  Dhcp4 = Private->Dhcp4;
  ASSERT (Dhcp4 != NULL);

  Status   = EFI_SUCCESS;

  //
  // Build standard option list for the DHCP D.O.R.A.
  //
  OptList[0] = (EFI_DHCP4_PACKET_OPTION *)Buffer;
  OptList[0]->OpCode  = DHCP4_TAG_PARA_LIST;
  OptList[0]->Length  = 3;
  OptList[0]->Data[0] = DHCP4_TAG_NETMASK;
  OptList[0]->Data[1] = DHCP4_TAG_ROUTER;
  OptList[0]->Data[2] = DHCP4_TAG_DNS_SERVER;

  ZeroMem (&Config, sizeof (EFI_DHCP4_CONFIG_DATA));
  Config.OptionCount      = 1;
  Config.OptionList       = OptList;
  Config.CallbackContext  = Private;
  Config.Dhcp4Callback    = NULL;
  Config.DiscoverTryCount = 3;
  Config.DiscoverTimeout  = mDhcpNetworkIdTimeout;

  //
  // Configure the DHCPv4 instance for D.O.R.A
  //
  Status = Dhcp4->Configure (Dhcp4, &Config);

  //
  // Start the DHCP process
  //
  Status = Dhcp4->Start (Dhcp4, NULL);


  Status = Dhcp4->GetModeData (Dhcp4, &Dhcp4Mode);
  if (EFI_ERROR (Status) || Dhcp4Mode.State != Dhcp4Bound) {
    goto ON_EXIT;
  }

  DEBUG((DEBUG_INFO, "DHCP D.O.R.A is in Bound state\n"));
  //
  // Get DHCP Server, Client IP information after D.O.R.A is done
  //
  CopyMem (&Private->StationIp, &Dhcp4Mode.ClientAddress, sizeof (EFI_IPv4_ADDRESS));
  CopyMem (&Private->SubnetMask, &Dhcp4Mode.SubnetMask, sizeof (EFI_IPv4_ADDRESS));
  CopyMem (&Private->GatewayIp, &Dhcp4Mode.RouterAddress, sizeof (EFI_IPv4_ADDRESS));
  CopyMem (&Private->ServerIp, &Dhcp4Mode.ServerAddress, sizeof (EFI_IPv4_ADDRESS));


  DEBUG_CODE(
    CHAR8 IpStr[32];
    AsciiSPrint (IpStr, 32, "%d.%d.%d.%d", Private->StationIp.v4.Addr[0], Private->StationIp.v4.Addr[1],
                                             Private->StationIp.v4.Addr[2], Private->StationIp.v4.Addr[3]);
    DEBUG((DEBUG_INFO, "   Station IP : %a\n", IpStr));

    AsciiSPrint (IpStr, 32, "%d.%d.%d.%d", Private->SubnetMask.v4.Addr[0], Private->SubnetMask.v4.Addr[1],
                                             Private->SubnetMask.v4.Addr[2], Private->SubnetMask.v4.Addr[3]);
    DEBUG((DEBUG_INFO, "   SubnetMask : %a\n", IpStr));

    AsciiSPrint (IpStr, 32, "%d.%d.%d.%d", Private->GatewayIp.v4.Addr[0], Private->GatewayIp.v4.Addr[1],
                                             Private->GatewayIp.v4.Addr[2], Private->GatewayIp.v4.Addr[3]);
    DEBUG((DEBUG_INFO, "   Gateway : %a\n", IpStr));

    AsciiSPrint (IpStr, 32, "%d.%d.%d.%d", Private->ServerIp.v4.Addr[0], Private->ServerIp.v4.Addr[1],
                                             Private->ServerIp.v4.Addr[2], Private->ServerIp.v4.Addr[3]);
    DEBUG((DEBUG_INFO, "   Server IP : %a\n", IpStr));
  );

ON_EXIT:

  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "DHCP D.O.R.A fails\n"));
    Dhcp4->Stop (Dhcp4);
    Dhcp4->Configure (Dhcp4, NULL);
  }

  return Status;
}


/**
  Build the options buffer for the DHCPv4 request packet which is used for Network Identifier.

  @param[in]  Private             Pointer to PxeBc private data.
  @param[out] OptList             Pointer to the option pointer array.
  @param[in]  Buffer              Pointer to the buffer to contain the option list.
  @param[in]  NeedMsgType         If TRUE, it is necessary to include the Msg type option.
                                  Otherwise, it is not necessary.

  @return     Index               The count of the built-in options.

**/
UINT32
BuildDhcp4OptionsForNetworkId (
  OUT EFI_DHCP4_PACKET_OPTION  **OptList,
  IN  UINT8                    *Buffer
  )
{
  UINT32                   Index;
  DHCP4_OPTION_ENTRY       OptEnt;

  Index      = 0;
  OptList[0] = (EFI_DHCP4_PACKET_OPTION *) Buffer;

  //
  // Append message type.
  //
  OptList[Index]->OpCode  = DHCP4_TAG_MSG_TYPE;
  OptList[Index]->Length  = 1;
  OptEnt.Mesg             = (DHCP4_OPTION_MESG *) OptList[Index]->Data;
  OptEnt.Mesg->Type       = DHCP_MSG_REQUEST;
  Index++;
  OptList[Index]          = GET_NEXT_DHCP_OPTION (OptList[Index - 1]);

  //
  // Append parameter request list option.
  // NetworkID is carried by DHCP ACK FileName Override.
  //
  OptList[Index]->OpCode    = DHCP4_TAG_PARA_LIST;
  OptList[Index]->Length    = 4;
  OptEnt.Para               = (DHCP4_OPTION_PARA *) OptList[Index]->Data;
  OptEnt.Para->ParaList[0]  = DHCP4_TAG_NETMASK;
  OptEnt.Para->ParaList[1]  = DHCP4_TAG_ROUTER;
  OptEnt.Para->ParaList[2]  = DHCP4_TAG_DNS_SERVER;
  OptEnt.Para->ParaList[3]  = DHCP4_TAG_OVERLOAD;

  Index++;
  OptList[Index]            = GET_NEXT_DHCP_OPTION (OptList[Index - 1]);

  //
  // Append vendor class identify option with OEM specific request for NetworkID
  //
  OptList[Index]->OpCode  = DHCP4_TAG_VENDOR_CLASS_ID;
  OptList[Index]->Length  = (UINT8) sizeof (DHCP4_OPTION_CLID);
  OptEnt.Clid             = (DHCP4_OPTION_CLID *) OptList[Index]->Data;
  CopyMem (
    OptEnt.Clid,
    DHCP_NETWORKID_VENDOR_CLASS_TAG,
    sizeof (DHCP_NETWORKID_VENDOR_CLASS_TAG)
    );
  Index++;

  return Index;
}


/**
  Build and send out the a special DHCPREQUEST packet for NetworkID. Parse responding DHCPACK packet.

  @param[in]  Private               Pointer to DHCP NetworkID private.

  @retval     EFI_SUCCESS           Successfully discovered boot file.
  @retval     EFI_OUT_OF_RESOURCES  Failed to allocate resource.
  @retval     EFI_NOT_FOUND         Can't get the PXE reply packet.
  @retval     Others                Failed to discover boot file.

**/
EFI_STATUS
Dhcp4NetworkIdSendRecv (
  IN  DHCP_NETWORK_ID_PRIVATE_DATA         *Private
  )
{
  EFI_STATUS                          Status;
  EFI_DHCP4_PROTOCOL                  *Dhcp4;
  EFI_DHCP4_TRANSMIT_RECEIVE_TOKEN    Token;
  BOOLEAN                             IsBCast;
  UINT8                               Buffer[312];
  EFI_DHCP4_PACKET_OPTION             *OptList[16];
  UINT32                              OptCount;
  UINT32                              Xid;
  UINTN                               Index;
  UINTN                               TryIndex;
  EFI_DHCP4_PACKET                    *DhcpAck;
  UINT8                               *Option;
  UINT8                               *OptionEnd;
  UINTN                               OptionLen;

  Dhcp4     = Private->Dhcp4;
  Status    = EFI_SUCCESS;

  ZeroMem (&Token, sizeof (EFI_DHCP4_TRANSMIT_RECEIVE_TOKEN));

  //
  // Use unicast by default
  //
  IsBCast = FALSE;

  //
  // Build all the options for the request packet.
  //
  OptCount = BuildDhcp4OptionsForNetworkId (OptList, Buffer);

  //
  // Build the request packet with seed packet and option list.
  //
  Status = Dhcp4->Build (
                    Dhcp4,
                    &Private->SeedPacket,
                    0,
                    NULL,
                    OptCount,
                    OptList,
                    &Token.Packet
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Set fields of the token for the request packet.
  // Assign a special Xid
  //
  Xid                                 = NET_RANDOM (NetRandomInitSeed ());
  Token.Packet->Dhcp4.Header.Xid      = HTONL (Xid);
  Token.Packet->Dhcp4.Header.Reserved = HTONS ((UINT16) ((IsBCast) ? 0x8000 : 0x0));

  Token.RemotePort = DHCP_SERVER_PORT;

  //
  // Unicast NetworkIdentifier Request Packet to DHCP server so that information of which client is doing User Password Authentication 
  // can be protected
  //
  CopyMem (&Token.Packet->Dhcp4.Header.ClientAddr, &Private->StationIp, sizeof (EFI_IPv4_ADDRESS));
  CopyMem (&Token.GatewayAddress, &Private->GatewayIp, sizeof (EFI_IPv4_ADDRESS));
  CopyMem (&Token.RemoteAddress, &Private->ServerIp, sizeof (EFI_IPv4_ADDRESS));

  //
  // Send out the request packet to discover the bootfile. 4 time in retry is a value required by RFC 2131
  //
  for (TryIndex = 1; TryIndex <= 4; TryIndex++) {

    Token.TimeoutValue                  = (UINT16) (1 * TryIndex);
    Token.Packet->Dhcp4.Header.Seconds  = HTONS((UINT16) (1 * (TryIndex - 1)));

    Status = Dhcp4->TransmitReceive (Dhcp4, &Token);
    if (Token.Status != EFI_TIMEOUT) {
      break;
    }
  }

  //
  // Immediately stop DHCP after Receiving DHCPACK
  //
  Dhcp4->Stop (Dhcp4);
  Dhcp4->Configure (Dhcp4, NULL);

  //
  // If Receiving DHCPACK Packet timeout
  //
  if (TryIndex > 3) {
    //
    // No server response our PXE request
    //
    Status = EFI_TIMEOUT;
    goto FUNC_EXIT;
  }


  //***************************************************************************************************************
  //                                            Parse DHCPACK Packet
  //***************************************************************************************************************
  DhcpAck  = Token.ResponseList;

  //
  // NetworkID DHCPACK xid must exactly matches DHCP NetworkID REQUEST
  //
  for (Index = 0; Index < Token.ResponseCount; Index++) {
    //
    // Check ACK length > DHCP4_HEADER + magik + first Option
    //
	  if (DhcpAck->Length < (sizeof(EFI_DHCP4_HEADER) + sizeof(UINT32) + 3 * sizeof(UINT8)) ||
        DhcpAck->Length > 512 ){
      //
      // No server response our PXE request
      //
      continue;
    }

    if (DhcpAck->Dhcp4.Header.Xid == HTONL (Xid) &&
	     	DhcpAck->Dhcp4.Option[0] == DHCP4_TAG_MSG_TYPE &&
		    DhcpAck->Dhcp4.Option[1] == 1 &&
		    DhcpAck->Dhcp4.Option[2] == DHCP_MSG_ACK) {
      break;
    }
    DhcpAck = (EFI_DHCP4_PACKET *)((UINT8 *)DhcpAck + DhcpAck->Size);
  }
  
  if (Index >= Token.ResponseCount) {
    Status = EFI_INVALID_PARAMETER;
    goto FUNC_EXIT;  
  }

  //
  // NetworkID DHCPACK must contain OVERLOAD Option
  //
  Option = DhcpAck->Dhcp4.Option;
  OptionEnd = Option + DhcpAck->Length - sizeof(EFI_DHCP4_HEADER) - sizeof(UINT32) - 2;

  while (Option < OptionEnd && Option[0] != DHCP4_TAG_EOP && Option[0] != DHCP4_TAG_OVERLOAD) {
    OptionLen = Option[1];
    Option += OptionLen + 2;
  }

  if (Option >= OptionEnd || Option[0] != DHCP4_TAG_OVERLOAD) {
    Status = EFI_INVALID_PARAMETER;
    goto FUNC_EXIT;
  }

  //
  // NetworkID DHCPACK must override OVERLOAD Option BootFileName 
  //
  if (Option[2] != 1) {
    Status = EFI_INVALID_PARAMETER;
    goto FUNC_EXIT;
  }

  //
  // Continue to parse NetworkID OPTION (0xAA) inside BootFileName
  //
  Option = (UINT8 *)DhcpAck->Dhcp4.Header.BootFileName;
  OptionEnd = Option + 128 - 2;
  while (Option < OptionEnd && Option[0] != DHCP4_TAG_EOP && Option[0] != DHCP_TAG_SERVER_NETWORK_ID) {
    OptionLen = Option[1];
    Option += OptionLen + 2;
  }

  //
  // Now we get to NetworkId Option, copy Length + NetworkId info
  //
  if (Option < OptionEnd && Option[0] == DHCP_TAG_SERVER_NETWORK_ID) {
    if (Private->NetworkIdNum < 16) {
      Private->ServerNetworkId[Private->NetworkIdNum] = AllocateZeroPool(Option[1] + 1);
      if (Private->ServerNetworkId[Private->NetworkIdNum] != NULL) {
        CopyMem(Private->ServerNetworkId[Private->NetworkIdNum], &Option[1], Option[1] + 1);
        Private->NetworkIdNum ++;
      }
      DEBUG((DEBUG_INFO, "DHCP NetworkId Len %d\n", Option[1]));
      DumpMem8(&Option[2], Option[1]);
    } else {
      Status = EFI_OUT_OF_RESOURCES;
    }
  } else {
    Status = EFI_INVALID_PARAMETER;
  }


FUNC_EXIT:

  if (Token.ResponseList != NULL) {
    FreePool (Token.ResponseList);
  }
  if (Token.Packet != NULL) {
    FreePool (Token.Packet);
  }

  return Status;
}