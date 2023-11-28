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


#include "SetupBmcCfg.h"
#include <Guid/MdeModuleHii.h>
#include <Library/SetupUiLib.h>
#include <Library/ByoCommLib.h>

//
// Global Varibels
//
BMC_LAN_CONFIG           mOldSetupBmcCfgPtr;
BMC_LAN_CONFIG           mTempSetupBmcCfg;

UINT8 IsBMCLanConfigChanged = 0;



STATIC BMC_USER_RECORD mUserRecord;

typedef struct _USER_INPUT_LINE_CTX {
  UINTN  InputX;
  UINTN  InputY;
  CHAR16 *String;
} USER_INPUT_LINE_CTX;

#define MAX_STRING_SIZE  20



EFI_STATUS
GetSetIpPram (
  IN     UINT8          Channel,
  IN     UINT8          LanParameter,
  IN OUT UINT16         *IpAddress,
  IN     BOOLEAN        CommandGet
  )
/*++

Routine Description:

  Get/Set IP address,subnet mask, and Gateway IP of BMC channels

Arguments:

  Channel - Channel to set
  LanParameter - BMC Command Parameter to set/get IP,Subnet mask or Gateway IP
  IpAddress - IPv4 format address to Set/Get
  CommandGet - TRUE - Read Lan parameter, False = Set Lan parameter.

Returns:

  Status : Status returned by BMC. 

--*/
{
  UINT8       commanddata[20];
  UINT8       commanddatasize;
  UINT8       Response[10];
  UINT8       Responsesize;
  UINT8       Ipadd[4];
  EFI_STATUS  Status;

//
//Count number of times command has been sent to BMC.
//
  UINT8 RetryCount=0;

  if (CommandGet == TRUE) {
    commanddata[0]  = Channel;
    commanddata[1]  = LanParameter;
    commanddata[2]  = 0;
    commanddata[3]  = 0;
    commanddatasize = 4;
    Responsesize    = 10;

    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_LAN,
              EFI_GET_LAN_CONFIG_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );
    if (Status == EFI_SUCCESS) {
      IpAddtoStr (IpAddress, &Response[1]);
    }
  } else {
    commanddata[0]  = Channel;
    commanddata[1]  = LanParameter;
    StrStringToIp4Adr ((EFI_IPv4_ADDRESS *) &Ipadd[0], IpAddress);
    CopyMem (&commanddata[2], &Ipadd[0], 4);
    commanddatasize = 6;
    Responsesize    = 10;

    //
    //Check if command is to set RMM4 ip address.
    //
      
      if(Channel==DEDICATE_NIC_CHANNEL_SEL && IsBMCLanConfigChanged==1)// && LanParameter==IP_ADDRESS_PARM)
      {
    //
    //Send command to BMC untill EFI_SUCCESS is returned.
    //
        do
        {
                Status = EfiSendCommandToBMC (
                          EFI_SM_NETFN_LAN,
                          EFI_SET_LAN_CONFIG_CMD,
                          (UINT8 *) &commanddata[0],
                          commanddatasize,
                          (UINT8 *) &Response,
                          (UINT8 *) &Responsesize
                          );
            RetryCount++;
    //
    //Check if Status returned is EFI_SUCCESS or retry count reached 100
    //
            if(RetryCount>=100 || Status==EFI_SUCCESS)
                break;
    //
    //Delay 100ms before issueing command once again.
    //
            gBS->Stall(100000);
        }while(Status!=EFI_SUCCESS);

      }
      else
      {
    //
    //If command is not to set RMM4 ip address or BMC conf is not changed then send command only once.
    //
        
                Status = EfiSendCommandToBMC (
                          EFI_SM_NETFN_LAN,
                          EFI_SET_LAN_CONFIG_CMD,
                          (UINT8 *) &commanddata[0],
                          commanddatasize,
                          (UINT8 *) &Response,
                          (UINT8 *) &Responsesize
                          );
      }
  }
  return Status;
}

UINTN Wcslen(CHAR16 *string) {
    UINTN length=0;
    while(*string++) length++;
    return length;
}

CHAR16* Wcscpy(CHAR16 *string1, CHAR16* string2){
    CHAR16 *dest = string1;
    while(*string2){
        *(string1++) = *(string2++);
    }
    *string1 = 0;        //Fill 0 as end of string and
    return dest;
}

UINT8 FindNumOfColonsToAdd(UINT8 ColCount,UINT16 *IPaddress,UINTN IPaddressLength)
/*++

Routine Description:
  Return the number of :0 to be added in IP address to expand abbreviated IP address.

Arguments:
  ColCount - Number of colons present in IP address.
  IPaddress - IP address entered by user.
  IPaddressLength - String length od IP address entered by user.

Returns:
  Number of :0 to be added in IP address.

--*/

{
//
//When :: is in middle of the IP address then maximum number of :0 to be added is 6, so m=5.
//
    UINT8 m=5,n,c=0;
//
//If number of colons in IP address is 8 then number of :0 to be added is 1.
//
    if(ColCount==8)
        return 0;

//
//If :: is in begining or end of IP address then maximum number of :0 to add is 7, so m=7.
//
    if(IPaddress[0]==0x3a && IPaddress[1]==0x3a || IPaddress[IPaddressLength]==0x3a && IPaddress[IPaddressLength-1]==0x3a)
        m=6;

//
//If two adjucent colons are in middle of IP address then the minimum number of colons 2 IPaddress is 2 and maximum number of colons can be 7.
//This for loop returns the number of :0 to be added in IP address.
//

    for(n=2;n<=7;n++)
    {
        if(ColCount==n)
            return (m-c);
        c++;
    }

    return 0;
}

/*
    Rewrite the function FindNumOfColonsToAdd to FindNumOfColonsToAdd1,add a parameter BOOLEAN DotFlag.
    DotFlag is the flag of '.',for we will meet this Ipv6 format (0::10.11.12.13)
*/
UINT8 FindNumOfColonsToAdd1(UINT8 ColCount,UINT16 *IPaddress,UINTN IPaddressLength,BOOLEAN DotFlag)
{

    UINT8 m=5,n,c=0;

    if(ColCount==8)
        return 0;

    if(IPaddress[0]==0x3a && IPaddress[1]==0x3a || IPaddress[IPaddressLength]==0x3a && IPaddress[IPaddressLength-1]==0x3a)
        m=6;

    for(n=2;n<=7;n++)
    {
        if(ColCount==n){
            if(DotFlag)    
                return (m-c-1);
            else
                return (m-c);
        }
            
        c++;
    }

    return 0;
}

EFI_STATUS CheckifAbbreviatedAddress(UINT16* IPaddress)
/*++

Routine Description:

  Checks if abbreviated IP address is entered by user, then updates IP address accordngly.

Arguments:

  Tmp : IPV6 Ip address
Returns:

  EFI_SUCCESS. 

--*/

{
  UINTN Index;
  UINTN Colons;
  UINTN DigitsSinceColon;
  BOOLEAN FoundDoubleColon;

  Colons = 0;
  DigitsSinceColon = 0;
  FoundDoubleColon = FALSE;

  //
  // If the first character is a colon, must be a double colon.
  //
  if (IPaddress [0] == 0x3A && IPaddress [1] != 0x3A) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Iterate through the string.
  //
  for (Index = 0; Index < 40; Index++) {
    if (IPaddress [Index] == '\0') {
      //
      // Found a string terminator character, stop iterating.
      //
      break;
    } else if (IPaddress [Index] == ':') {
      //
      // Found a period. Total number must not exceed seven.
      //
      if (++Colons > 7) {
        return EFI_INVALID_PARAMETER;
      }

      //
      // Check for a double colon. It is only allowed to occur once.
      //
      if (Index != 0 && DigitsSinceColon == 0) {
        if (FoundDoubleColon) {
          return EFI_INVALID_PARAMETER;
        }

        FoundDoubleColon = TRUE;
      }

      DigitsSinceColon = 0;
    } else if ((IPaddress [Index] >= '0' && IPaddress [Index] <= '9') ||
               (IPaddress [Index] >= 'a' && IPaddress [Index] <= 'f') ||
               (IPaddress [Index] >= 'A' && IPaddress [Index] <= 'F')) {
      //
      // Found a digit. Number of digits between colons must not exceed four.
      //
      if (++DigitsSinceColon > 4) {
        return EFI_INVALID_PARAMETER;
      }
    } else {
      //
      // Any other character is invalid.
      //
      return EFI_INVALID_PARAMETER;
    }
  }

  //
  // If the last character is a colon, must be a double colon.
  //
  if (DigitsSinceColon == 0) {
    ASSERT (Index > 2);
    if (IPaddress [Index - 2] != ':') {
      return EFI_INVALID_PARAMETER;
    }
    ASSERT (FoundDoubleColon);
  }

    return EFI_SUCCESS;
}

EFI_STATUS
GetSetIpv6Status (
  IN     UINT8          Channel,
  IN     UINT16          LanParameter,
  IN OUT UINT8         *Ipv6Enable,
  IN     BOOLEAN        CommandGet
  )
/*++

Routine Description:

  Get/Set IPV6 Prefix Length of BMC channels

Arguments:

  Channel - Channel to set
  LanParameter - BMC Command Parameter to set/get IP,Subnet mask or Gateway IP
  Ipv6 Prefix Length - IPv6 format address to Set/Get
  CommandGet - TRUE - Read Lan parameter, False = Set Lan parameter.

Returns:

  Status : Status returned by BMC. 

--*/
{
  UINT8       commanddata[20];
  UINT8       commanddatasize;
  UINT8       Response[10];
  UINT8       Responsesize;
  EFI_STATUS  Status;

  if (CommandGet == TRUE) {
    commanddata[0]  = (UINT8)Channel;
    commanddata[1]  = (UINT8)LanParameter;
    commanddata[2]  = 0;
    commanddata[3]  = 0;
    commanddatasize = 4;
    Responsesize    = 4;
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_LAN,
              EFI_GET_LAN_CONFIG_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );
    if (Status == EFI_SUCCESS) {
      CopyMem (Ipv6Enable, &Response[1],1);
    }
  } else {
    commanddata[0]  = (UINT8)Channel;
    commanddata[1]  = (UINT8)LanParameter;
    CopyMem (&commanddata[2], Ipv6Enable,1);
    commanddatasize = 3;
    Responsesize    = 1;
    
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_LAN,
              EFI_SET_LAN_CONFIG_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );
  }
  return Status;
}
//
//END IPV6
//


EFI_STATUS
BmcSetMyLanConfig (IN UINT8          Channel)
/*++

Routine Description:

  Send LAN configuration to BMC.

Arguments:

  Laninfo - Ponter to LAN configuration.
  Channel - Lan channel

Returns:

  EFI_SUCCESS

--*/
{
  UINT8       commanddata[20];
  UINT8       commanddatasize;
  UINT8       Response[20];
  UINT8       Responsesize;
//  CHAR8       TempDhcpName[64];
//  UINTN       Index;
  EFI_STATUS  Status = EFI_SUCCESS;
  
//  UINT8 RetryCount=0;
  ZeroMem (commanddata, 20);
  ZeroMem (Response, 20); 
  //
  // Set IP Source
  //
  commanddata[0] = Channel;               // Channel number
  commanddata[1] = IP_SOURCE_PARM;        // Parameter selector
  commanddata[2] = 1;  // IP Source (STATIC/ DYANAMIC)
  commanddatasize = 3;
  Responsesize    = 10;

//
//If BMC LAN configuration IP source is not changed, then issue command once.
//
  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_LAN,           // NetFunction
            EFI_SET_LAN_CONFIG_CMD,     // Command
            (UINT8 *) &commanddata[0],  // *CommandData
            commanddatasize,            // CommandDataSize
            (UINT8 *) &Response,        // *ResponseData
            (UINT8 *) &Responsesize     // *ResponseDataSize
            );
//  DEBUG((EFI_D_INFO," Setup Laninfo->NicIpSource = STATIC - %r\n",Status));

  return EFI_SUCCESS;              
}  

EFI_STATUS
BmcSetLanConfig (
  IN BMC_LAN_INFO   *Laninfo,
  IN UINT8          Channel
  )
/*++

Routine Description:

  Send LAN configuration to BMC.

Arguments:

  Laninfo - Ponter to LAN configuration.
  Channel - Lan channel

Returns:

  EFI_SUCCESS

--*/
{
  UINT8       commanddata[20];
  UINT8       commanddatasize;
  UINT8       Response[20];
  UINT8       Responsesize;
  CHAR8       TempDhcpName[64];
  UINTN       Index;
  EFI_STATUS  Status;
  
//  UINT8 RetryCount=0;
  ZeroMem (commanddata, 20);
  ZeroMem (Response, 20); 
  //
  // Set IP Source
  //
  commanddata[0] = Channel;               // Channel number
  commanddata[1] = IP_SOURCE_PARM;        // Parameter selector
  commanddata[2] = Laninfo->NicIpSource;  // IP Source (STATIC/ DYANAMIC)
  commanddatasize = 3;
  Responsesize    = 10;
//
//If BMC LAN configuration IP source is not changed, then issue command once.
//
      Status = EfiSendCommandToBMC (
                EFI_SM_NETFN_LAN,           // NetFunction
                EFI_SET_LAN_CONFIG_CMD,     // Command
                (UINT8 *) &commanddata[0],  // *CommandData
                commanddatasize,            // CommandDataSize
                (UINT8 *) &Response,        // *ResponseData
                (UINT8 *) &Responsesize     // *ResponseDataSize
                );
      DEBUG((EFI_D_ERROR," Setup Laninfo->NicIpSource = %x - %r\n",Laninfo->NicIpSource,Status));
                
 // }
  
  if (Laninfo->NicIpSource != 2) {
    //
    // IP Source is Static.Set Ip Address,Subnet Mask and Gateway Ip
    //
    GetSetIpPram (Channel, IP_ADDRESS_PARM, &Laninfo->IpAddress[0], FALSE);
    GetSetIpPram (Channel, SUBNET_MASK_PARM, &Laninfo->SubnetMask[0], FALSE);
    GetSetIpPram (Channel, GATEWAY_IP_PARM, &Laninfo->GatewayIp[0], FALSE);
  }

  ZeroMem (TempDhcpName, 64);
  Unicode2Ascii (TempDhcpName, &Laninfo->DhcpName[0]);

  for (Index = 0; Index < 4; Index++) {
    commanddata[0]  = Channel;
    commanddata[1]  = DHCP_NAME_PARM;
    commanddata[2]  = (UINT8) Index + 1;
    if (commanddata[2] == 4) {
      commanddata[3] = 1;   //Mark Update complete for Last Block
    } else {
      commanddata[3] = 0;
    }

    commanddatasize = 20;
    Responsesize    = 20;
    gBS->CopyMem (&commanddata[4], &TempDhcpName[Index * 16], 16);

    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_LAN,
              EFI_SET_LAN_CONFIG_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );

  }
 
  return EFI_SUCCESS;
}




EFI_STATUS
BmcGetLanConfig (
  IN BMC_LAN_INFO   *Laninfo,
  IN UINT8          Channel
  )
/*++
Routine Description:
  Fill LAN configuartion for given Channel.

Arguments:
   Laninfo - Ponter to SetupBmCfg Lan Configuration
   Channel - Lan Channel

Returns:
  Standard variable return values
--*/
{
  UINT8       commanddata[20];
  UINT8       commanddatasize;
  UINT8       Response[20];
  UINT8       Responsesize;
  UINTN       Index;
  EFI_STATUS  Status;

  ZeroMem (commanddata, 20);
  ZeroMem (Response, 20); 

  //
  // Get IP Source (DHCP/ STATIC)
  //
  commanddata[0] = Channel;         // Channel number
  commanddata[1] = IP_SOURCE_PARM;  // Parameter selector
  commanddata[2] = 0;               // Set Selector.
  commanddata[3] = 0;               // Block Selector 
  commanddatasize = 4;
  Responsesize    = 20;

  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_LAN,           // NetFunction
            EFI_GET_LAN_CONFIG_CMD,     // Command
            (UINT8 *) &commanddata[0],  // *CommandData
            commanddatasize,            // CommandDataSize
            (UINT8 *) &Response,        // *ResponseData
            (UINT8 *) &Responsesize     // *ResponseDataSize
            );

#if 1
  DEBUG ((EFI_D_ERROR, "BmcGetLanConfig  -%r response:\n",Status));
  for(Index = 0 ;Index < 20;Index++){
    DEBUG ((EFI_D_ERROR, "%02x ",*(Response+Index)));
  }
  DEBUG((EFI_D_ERROR,"\n"));
#endif


  if (Status == EFI_SUCCESS) {
    Laninfo->NicIpSource = Response[1]; // Response1 = IP Source
  } else {
    Laninfo->NicIpSource = 2;
    StrCpy(&Laninfo->IpAddress[0] , L"0.0.0.0");
    StrCpy(&Laninfo->SubnetMask[0], L"0.0.0.0");
    StrCpy(&Laninfo->GatewayIp[0] , L"0.0.0.0");
  }
  //
  // Get Ip Address,Subnet Mask and Gateway Ip
  //
  GetSetIpPram (Channel, IP_ADDRESS_PARM, &Laninfo->IpAddress[0], TRUE);
  GetSetIpPram (Channel, SUBNET_MASK_PARM, &Laninfo->SubnetMask[0], TRUE);
  GetSetIpPram (Channel, GATEWAY_IP_PARM, &Laninfo->GatewayIp[0], TRUE);
  
  if(StrCmp(&Laninfo->IpAddress[0] , L"0.0.0.0") ==0){
    StrCpy(&Laninfo->SubnetMask[0], L"0.0.0.0");
    StrCpy(&Laninfo->GatewayIp[0] , L"0.0.0.0");
  }
  
  //
  // Get DHCP Name
  //
  if(Laninfo->NicIpSource == 2 && Status == EFI_SUCCESS){
    for (Index = 0; Index < 4; Index++) {
      commanddata[0]  = Channel;
      commanddata[1]  = DHCP_NAME_PARM;
      commanddata[2]  = 0;
      commanddata[3]  = (UINT8) Index + 1;
      commanddatasize = 4;
      Responsesize    = 20;

      Status = EfiSendCommandToBMC (
                EFI_SM_NETFN_LAN,
                EFI_GET_LAN_CONFIG_CMD,
                (UINT8 *) &commanddata[0],
                commanddatasize,
                (UINT8 *) &Response,
                (UINT8 *) &Responsesize
                );
      if (Status == EFI_SUCCESS) {
        AsciiToUnicode (&Response[1], &Laninfo->DhcpName[16 * Index]);
      }
    }
  }
  
  return Status;
}
//IPV6 Get LAN configuration function
//
EFI_STATUS
BmcGetLanIPv6Config (
  IN BMC_LAN_IPV6_INFO   *Laninfo,
  IN UINT8          Channel
  )
/*++
Routine Description:
  Fill LAN configuartion for given Channel.

Arguments:
   Laninfo - Ponter to SetupBmCfg Lan Configuration
   Channel - Lan Channel

Returns:
  Standard variable return values
--*/
{
  UINT8       commanddata[22];
  UINT8       commanddatasize;
  UINT8       Response[22];
  UINT8       Responsesize;
  UINT8       SelIPv6Addr;
  EFI_STATUS  Status;

  ZeroMem (commanddata, 22);
  ZeroMem (Response, 22); 
  
  //
  // Get IP Source (DHCP/ STATIC)
  //
  commanddata[0] = (UINT8) Channel;                   // Channel number
  commanddata[1] = (UINT8) 0xc4;  // Parameter selector
  commanddata[2] = 0;               // Set Selector.
  commanddata[3] = 0;               // Block Selector 
  commanddatasize = 4;
  Responsesize    = 3;
  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_LAN,           // NetFunction
            EFI_GET_LAN_CONFIG_CMD,     // Command
            (UINT8 *) &commanddata[0],  // *CommandData
            commanddatasize,            // CommandDataSize
            (UINT8 *) &Response,        // *ResponseData
            (UINT8 *) &Responsesize     // *ResponseDataSize
            );
   DEBUG ((EFI_D_ERROR, "%a GATEWAY_IPV6_CONFIG_PARM Status - %r\n",__FUNCTION__,Status));
  if(Status != EFI_SUCCESS){  
  	commanddata[0] = (UINT8) Channel;                   // Channel number
    commanddata[1] = (UINT8) GATEWAY_IPV6_CONFIG_PARM;  // Parameter selector
    commanddata[2] = 0;               // Set Selector.
    commanddata[3] = 0;               // Block Selector 
    commanddatasize = 4;
    Responsesize    = 3;
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_LAN,           // NetFunction
              EFI_GET_LAN_CONFIG_CMD,     // Command
              (UINT8 *) &commanddata[0],  // *CommandData
              commanddatasize,            // CommandDataSize
              (UINT8 *) &Response,        // *ResponseData
              (UINT8 *) &Responsesize     // *ResponseDataSize
              );
   	}

  SelIPv6Addr = IPV6_ADDRESS_PARM;
  if (Status == EFI_SUCCESS) {
    if (Response[1] == 2) {
      Laninfo->NicIpv6Source = 2;       // IP source is dynamic
      SelIPv6Addr = IPV6_ADDRESS_DHCP;
    } else if (Response[1] == 1) {
      Laninfo->NicIpv6Source = 1;       // IP source is static
    } else {
      Laninfo->NicIpv6Source = 1;
    }
  } else {
    Laninfo->NicIpv6Source = 1;
  }

  ZeroMem (commanddata, 22);
  ZeroMem (Response, 22); 
  
  //
  // Get IP Source (DHCP/ STATIC)
  //
  commanddata[0] = (UINT8) Channel; // Channel number
  commanddata[1] = SelIPv6Addr;     // Parameter selector
  commanddata[2] = 0;               // Set Selector.
  commanddata[3] = 0;               // Block Selector 
  commanddatasize = 4;
  Responsesize    = 22;
  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_LAN,           // NetFunction
            EFI_GET_LAN_CONFIG_CMD,     // Command
            (UINT8 *) &commanddata[0],  // *CommandData
            commanddatasize,            // CommandDataSize
            (UINT8 *) &Response,        // *ResponseData
            (UINT8 *) &Responsesize     // *ResponseDataSize
            );
   DEBUG ((EFI_D_ERROR, "%a IPV6_ADDRESS_PARM Status - %r\n",__FUNCTION__,Status));
              
  //
  // Get Ipv6 Address,PRefix Length
  // 
  if (Status == EFI_SUCCESS) {  
    Ipv6AddtoStr (&Laninfo->Ipv6Address[0], &Response[3]);
    Laninfo->NicIpv6Prefix = Response[19];
    
    if (Response[2] != 0x80) {
      Laninfo->NicIpv6Source = 2;     // IP source is dynamic
    }
  }else{
    StrCpy(&Laninfo->Ipv6Address[0], L"0000:0000:0000:0000:0000:0000:0000:0000");
  }
  
  //
  // Get Ipv6 Gateway Ip
  //
  ZeroMem (commanddata, 22);
  ZeroMem (Response, 22); 
  
  commanddata[0]  = (UINT8)Channel;
  commanddata[1]  = (UINT8)GATEWAY_IPV6_PARM;
  commanddata[2]  = 0;
  commanddata[3]  = 0;
  commanddatasize = 4;//command data is 4 bytes
  Responsesize    = 18;//response is 20 bytes
  
  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_LAN,
             EFI_GET_LAN_CONFIG_CMD,
             (UINT8 *) &commanddata[0],
             commanddatasize,
             (UINT8 *) &Response,
             (UINT8 *) &Responsesize
             );
   DEBUG ((EFI_D_ERROR, "%a GATEWAY_IPV6_PARM Status - %r\n",__FUNCTION__,Status));
             
  if (Status == EFI_SUCCESS) {
    Ipv6AddtoStr (&Laninfo->GatewayIpv6[0], &Response[1]);
  }else{
    StrCpy(&Laninfo->GatewayIpv6[0], L"0000:0000:0000:0000:0000:0000:0000:0000");
  }
  
  return Status;
}

EFI_STATUS
BmcSetLanIpv6Config (
  IN BMC_LAN_IPV6_INFO   *Laninfo,
  IN UINT8          Channel
  )
/*++

Routine Description:

  Send LAN configuration to BMC.

Arguments:

  Laninfo - Ponter to LAN configuration.
  Channel - Lan channel


Returns:

  EFI_SUCCESS

--*/
{
   UINT8       commanddata[22];
   UINT8       commanddatasize;
   UINT8       Response[22];
   UINT8       Responsesize;
   CHAR8       TempDhcpName[64];
   UINTN       Index;
   EFI_STATUS  Status;
   UINT8       Ipadd[16];   

  ZeroMem (commanddata, 22);
  ZeroMem (Response, 22);
  ZeroMem (Ipadd, 16);   

  if (Laninfo->NicIpv6Source == 1) {

//Set static Ipv6
    
      commanddata[0]  = (UINT8)Channel;
      commanddata[1]  = (UINT8)GATEWAY_IPV6_CONFIG_PARM;
      commanddata[2]  = 1;   // Set to Static Gateway
      commanddatasize = 3;
      Responsesize    = 1;

    Status = EfiSendCommandToBMC (
               EFI_SM_NETFN_LAN,           // NetFunction
               EFI_SET_LAN_CONFIG_CMD,     // Command
               (UINT8 *) &commanddata[0],  // *CommandData
               commanddatasize,            // CommandDataSize
               (UINT8 *) &Response,        // *ResponseData
               (UINT8 *) &Responsesize     // *ResponseDataSize
               );


  
    //
    // Set IPv6 Source to Static
    //
    ZeroMem (commanddata, 22);
    ZeroMem (Response, 22);
    commanddata[0] = Channel;                 // Channel number
    commanddata[1] = IPV6_ADDRESS_PARM;       // Parameter selector
    commanddata[2] = 0;                       // Set Selector
    commanddata[3] = 0x80;                    // Address source/type

    //
    //To check if abbreviated IPV6 address is entered by user.
    //
    CheckifAbbreviatedAddress(&Laninfo->Ipv6Address[0]);
  
    StrStringToIp6Adr ((EFI_IPv6_ADDRESS *) &Ipadd[0], &Laninfo->Ipv6Address[0]);
    CopyMem (&commanddata[4], Ipadd, 16);
  
    commanddata[20] = Laninfo->NicIpv6Prefix; // Address Prefix Length 
    
    commanddatasize = 22;
    Responsesize    = 1;
    Status = EfiSendCommandToBMC (
               EFI_SM_NETFN_LAN,           // NetFunction
               EFI_SET_LAN_CONFIG_CMD,     // Command
               (UINT8 *) &commanddata[0],  // *CommandData
               commanddatasize,            // CommandDataSize
               (UINT8 *) &Response,        // *ResponseData
               (UINT8 *) &Responsesize     // *ResponseDataSize
               );

    //
    // IP Source is Static. Set Ipv6 Gateway Ip
    //   
    ZeroMem (commanddata, 22);
    ZeroMem (Response, 22);
    
    commanddata[0]  = (UINT8)Channel;
    commanddata[1]  = (UINT8)0xc4;
    commanddata[2]  = 1;   // Set to Static Gateway
    commanddatasize = 3;
    Responsesize    = 1;

    Status = EfiSendCommandToBMC (
               EFI_SM_NETFN_LAN,           // NetFunction
               EFI_SET_LAN_CONFIG_CMD,     // Command
               (UINT8 *) &commanddata[0],  // *CommandData
               commanddatasize,            // CommandDataSize
               (UINT8 *) &Response,        // *ResponseData
               (UINT8 *) &Responsesize     // *ResponseDataSize
               );
	if(Status != EFI_SUCCESS){    
/*	  ZeroMem (commanddata, 22);
      ZeroMem (Response, 22);
    
      commanddata[0]  = (UINT8)Channel;
      commanddata[1]  = (UINT8)GATEWAY_IPV6_CONFIG_PARM;
      commanddata[2]  = 1;   // Set to Static Gateway
      commanddatasize = 3;
      Responsesize    = 1;

    Status = EfiSendCommandToBMC (
               EFI_SM_NETFN_LAN,           // NetFunction
               EFI_SET_LAN_CONFIG_CMD,     // Command
               (UINT8 *) &commanddata[0],  // *CommandData
               commanddatasize,            // CommandDataSize
               (UINT8 *) &Response,        // *ResponseData
               (UINT8 *) &Responsesize     // *ResponseDataSize
               );*/
		}

    ZeroMem (commanddata, 22);
    ZeroMem (Response, 22); 
    ZeroMem (Ipadd, 16);    

    commanddata[0]  = (UINT8)Channel;
    commanddata[1]  = (UINT8)GATEWAY_IPV6_PARM;
    //
    //To check if abbreviated IPV6 address is entered by user.
    //
    CheckifAbbreviatedAddress(&Laninfo->GatewayIpv6[0]);

    StrStringToIp6Adr ((EFI_IPv6_ADDRESS *) &Ipadd[0], &Laninfo->GatewayIpv6[0]);
    CopyMem (&commanddata[2], Ipadd, 16);    
    
    commanddatasize = 18;
    Responsesize    = 1;                  

    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_LAN,
              EFI_SET_LAN_CONFIG_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );  
  } else {


   

      commanddata[0]  = (UINT8)Channel;
      commanddata[1]  = (UINT8)GATEWAY_IPV6_CONFIG_PARM;
      commanddata[2]  = 2;   // Set to Dynamic Gateway
      commanddatasize = 3;
      Responsesize    = 1;

    Status = EfiSendCommandToBMC (
               EFI_SM_NETFN_LAN,           // NetFunction
               EFI_SET_LAN_CONFIG_CMD,     // Command
               (UINT8 *) &commanddata[0],  // *CommandData
               commanddatasize,            // CommandDataSize
               (UINT8 *) &Response,        // *ResponseData
               (UINT8 *) &Responsesize     // *ResponseDataSize
               );  
    //
    // Set IPv6 Source to Dynamic
    //
  	  ZeroMem (commanddata, 22);
      ZeroMem (Response, 22);
    
	
    commanddata[0] = Channel;                 // Channel number
    commanddata[1] = IPV6_ADDRESS_DHCP;       // Parameter selector
    commanddata[2] = 0;                       // Set Selector
    commanddata[3] = 0;                       // Address source/type
    commanddatasize = 22;
    Responsesize    = 1;
    Status = EfiSendCommandToBMC (
               EFI_SM_NETFN_LAN,           // NetFunction
               EFI_SET_LAN_CONFIG_CMD,     // Command
               (UINT8 *) &commanddata[0],  // *CommandData
               commanddatasize,            // CommandDataSize
               (UINT8 *) &Response,        // *ResponseData
               (UINT8 *) &Responsesize     // *ResponseDataSize
               );         
  
    //
    // IP Source is dynamic.
    //   
    ZeroMem (commanddata, 22);
    ZeroMem (Response, 22);
    
    commanddata[0]  = (UINT8)Channel;
    commanddata[1]  = (UINT8)0xc4;
    commanddata[2]  = 2;   // Set to Dynamic Gateway
    commanddatasize = 3;
    Responsesize    = 1;

    Status = EfiSendCommandToBMC (
               EFI_SM_NETFN_LAN,           // NetFunction
               EFI_SET_LAN_CONFIG_CMD,     // Command
               (UINT8 *) &commanddata[0],  // *CommandData
               commanddatasize,            // CommandDataSize
               (UINT8 *) &Response,        // *ResponseData
               (UINT8 *) &Responsesize     // *ResponseDataSize
               );  
	if(Status != EFI_SUCCESS){    
/*	  ZeroMem (commanddata, 22);
      ZeroMem (Response, 22);
    
      commanddata[0]  = (UINT8)Channel;
      commanddata[1]  = (UINT8)GATEWAY_IPV6_CONFIG_PARM;
      commanddata[2]  = 2;   // Set to Dynamic Gateway
      commanddatasize = 3;
      Responsesize    = 1;

    Status = EfiSendCommandToBMC (
               EFI_SM_NETFN_LAN,           // NetFunction
               EFI_SET_LAN_CONFIG_CMD,     // Command
               (UINT8 *) &commanddata[0],  // *CommandData
               commanddatasize,            // CommandDataSize
               (UINT8 *) &Response,        // *ResponseData
               (UINT8 *) &Responsesize     // *ResponseDataSize
               );  */
	}

    ZeroMem (commanddata, 22);
    ZeroMem (Response, 22);   

    commanddata[0]  = (UINT8)Channel;
    commanddata[1]  = (UINT8)GATEWAY_IPV6_PARM;  
    
    commanddatasize = 18;
    Responsesize    = 1;                  

    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_LAN,
              EFI_SET_LAN_CONFIG_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              ); 
  }

  ZeroMem (TempDhcpName, 64);
  Unicode2Ascii (TempDhcpName, &Laninfo->DhcpName[0]);

  for (Index = 0; Index < 4; Index++) {

    commanddata[0]  = 0x1;
    commanddata[1]  = DHCP_NAME_PARM;
    commanddata[2]  = (UINT8) Index + 1;
    if (commanddata[2] == 4) {
      commanddata[3] = 1;   //Mark Update complete for Last Block
    } else {
      commanddata[3] = 0;
    }

    commanddatasize = 20;
    Responsesize    = 20;
    gBS->CopyMem (&commanddata[4], &TempDhcpName[Index * 16], 16);

    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_LAN,
              EFI_SET_LAN_CONFIG_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );

  }
  return EFI_SUCCESS;
}


EFI_STATUS
BmcGetUserName (
  IN UINT16         *UserName,
  IN UINT8          UserId
  )
/*++

Routine Description:

    Read BMC user Name From BMC

Arguments:

  UserName  - BMC user Name
  UserId    - User ID 

Returns:
  UserName 
  EFI_STATUS: BMC response.

--*/
{
  EFI_STATUS  Status;
  UINT8       commanddata[20];
  UINT8       commanddatasize;
  UINT8       Response[20];
  UINT8       Responsesize;
  UINT8       Index;
  //CHAR8       *FakeName;
  //
  // Get User Name
  //
  commanddata[0]  = UserId;
  commanddatasize = 1;
  Responsesize    = 20;
 
  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_APP,
            GET_USER_NAME_CMD,
            (UINT8 *) &commanddata[0],
            commanddatasize,
            (UINT8 *) &Response,
            (UINT8 *) &Responsesize
            );
   DEBUG ((EFI_D_ERROR, "%a GET_USER_NAME_CMD Status - %r\n",__FUNCTION__,Status));
            
  if (Status == EFI_SUCCESS) {
  	
   	for (Index = 0; Response[Index] != 0; Index++) {
      UserName[Index] = (CHAR16) Response[Index];
    }
	
	UserName[Responsesize] = 0;
	DEBUG((EFI_D_INFO,"Get user name :%s.",UserName));
    if(!StrnCmp(UserName,L" ",1)){
		Status = EFI_NOT_FOUND;
  	}
  }else{
    //FakeName = AllocateZeroPool (20);
    //AsciiSPrint(FakeName, 20, "%a",(UINT8*)PcdGetPtr(PcdSkipSetupString));
    //AsciiToUnicode (FakeName, UserNam);
    //FreePool (FakeName);
    StrCpy(UserName,PcdGetPtr(PcdSkipInputString));
  }

  return Status;
}

EFI_STATUS
BmcGetUserInfo (
  IN BMC_USER_INFO  *Userinfo,
  IN UINT8          UserId
  )
/*++

Routine Description:

  Fill User configuration for given user.

Arguments:
  Userinfo  - Poniter to SetupBmcCfg User Configuration 
  UserId    - User ID

Returns:

  EFI_STATUS: BMC response.

--*/
{
  EFI_STATUS  Status;
  UINT8       commanddata[20];
  UINT8       commanddatasize;
  UINT8       Response[20];
  UINT8       Responsesize;
//  UINT8       i;
  
  ZeroMem (commanddata, 20);
  ZeroMem (Response, 20); 
  //
  // Initialize User Password to NULL String.
  //
  StrCpy (Userinfo->UserPassword, L"\0");
  Status = BmcGetUserName (Userinfo->UserName, UserId);
  if(!EFI_ERROR(Status)){
    Userinfo->UserPresent = 1;
  }
  //
  //  Get User Privilege & User enable.
  //
  //for (i=8; i>0; i-=7) {  
    commanddata[0]  = DEDICATE_NIC_CHANNEL_SEL;//i;   //channel  1 & 8
    commanddata[1]  = UserId; 
    commanddatasize = 2;
    Responsesize    = 20;

    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_APP,
              GET_USER_ACCESS_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );
#if 1
     DEBUG ((EFI_D_ERROR, "GET_USER_ACCESS_CMD Status - %r\n",Status));
    //for(Index = 0 ;Index < 20;Index++){
    //  DEBUG ((EFI_D_ERROR, "%02x ",*(Response+Index)));
    //}
    //DEBUG((EFI_D_ERROR,"\n"));
#endif          
    if(EFI_ERROR(Status)){
      commanddata[0]  = BASE_BOARD_NIC_CHANNEL_SEL;//i;   //channel  1 & 8
      commanddata[1]  = UserId; 
      commanddatasize = 2;
      Responsesize    = 20;

      Status = EfiSendCommandToBMC (
                EFI_SM_NETFN_APP,
                GET_USER_ACCESS_CMD,
                (UINT8 *) &commanddata[0],
                commanddatasize,
                (UINT8 *) &Response,
                (UINT8 *) &Responsesize
                );
    }

    if (Status == EFI_SUCCESS) {
      Userinfo->Privilege = (UINT8) (Response[3] & 0xf) > 4 ? 0xf : (UINT8) (Response[3] & 07);
      // Bits 7&6 = user enable(01)/disable(10).IPMI Spec 2.0 June 12,2009 Markup
      if ((Response[1] & 0xC0) == 0x40) { 
        Userinfo->EnabaleChannelUser = 1;
      }else{
        Userinfo->EnabaleChannelUser = 0;
      }
      //
      //Assume all channel has the same user list.
      //
      //break;
    }else{
       Userinfo->Privilege          = 1;
       Userinfo->EnabaleChannelUser = 0;
    }
  //} //for 2 loop 
  return Status;
}

EFI_STATUS
BmcSetUserInfo (
  IN BMC_USER_INFO  *Userinfo,
  IN UINT8          UserId
  )
/*++

Routine Description:

  Send User Configuration to BMC

Arguments:

  Userinfo  - Ponter to User Configuration
  UserId    - UserID of the UserInfo buffer

Returns:

  EFI_SUCCESS - GC_TODO: Add description for return value

--*/
{

  UINT8       commanddata[24];  //Predefine User Name/Privilege/Password command data buffer with maximal buffer length and necessary redundancy.
  UINT8       commanddatasize;  //Send to BMC data size.
  UINT8       Response[24];
  UINT8       Responsesize;
  EFI_STATUS  Status;
  UINT8       Index;

  //
  // Set User Name
  //
  if ((UserId != 0) && (UserId != 1)) {
    commanddata[0] = UserId;
    ZeroMem (&commanddata[1], 16);
    Unicode2Ascii (&commanddata[1], Userinfo->UserName);
    commanddatasize = 17;
    Responsesize    = 10;

    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_APP,
              SET_USER_NAME_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );
    if(!EFI_ERROR(Status)){
      Userinfo->UserPresent = 1;
    }
  }
  //
  // Set User Privilege and enable user channel
  //
  commanddata[0] = UserId;      // userid
  if((Userinfo->EnabaleChannelUser == 0)) {
    commanddata[1] = 00;          // Disable User
    } else {
    commanddata[1] = 01;          // Enable User
  }

  ZeroMem (&commanddata[2], 16);
  commanddatasize = 18;
  Responsesize    = 20;

  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_APP,
            SET_USER_PASSWORD_CMD,
            (UINT8 *) &commanddata[0],
            commanddatasize,
            (UINT8 *) &Response,
            (UINT8 *) &Responsesize
            );

  for (Index = 0; Index < 2; Index++) {
    if (Index == 1) {
      //Byosoft +
      commanddata[0] = 0xB0|BASE_BOARD_NIC_CHANNEL_SEL;                // Channel 1
      //commanddata[0] = 0xB8;                // Channel 8
      //Byosoft +
    } else {
      commanddata[0] = 0xB0|DEDICATE_NIC_CHANNEL_SEL;                // channel 8
    }
    commanddata[1] = UserId;                // userid
    commanddata[2] = Userinfo->Privilege;   // Privilege
    commanddata[3] = 00;                    // Session Limit
    commanddatasize = 4;
    Responsesize    = 20;

    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_APP,
              SET_USER_ACCESS_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );
  }
  //
  // Set User Password
  //
  if (StrLen (Userinfo->UserPassword) != 0) {
    commanddata[0] = UserId | BIT7;      // userid: Enable BIT7 to support 20 bytes user passwords.
    commanddata[1] = 02;                 // operation: Set Password
    ZeroMem (&commanddata[2], 20);       // Zero Password data
    Unicode2Ascii (&commanddata[2], Userinfo->UserPassword);
    commanddatasize = 22;                // commanddatasize: Byte[1]:UserId,Byte[2]:operation,Byte[3-22]:Password data.Please refer IPMI2.0 Spec.
    Responsesize    = 24;

    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_APP,
              SET_USER_PASSWORD_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );
  }
  return EFI_SUCCESS;
}


BmcSetUserDefault(
   IN BMC_LAN_CONFIG *BmcCfgPtr
  )
{
  BMC_USER_INFO       *UserInfoPtr;

  DEBUG ((EFI_D_ERROR, "BmcSetUserDefault()\n"));
  UserInfoPtr = (BMC_USER_INFO *) &(BmcCfgPtr->BmcUserInfo[0]);
  BmcCfgPtr->User           = 0;
  BmcCfgPtr->UserPwdChanged = 0;
  StrCpy (UserInfoPtr->UserPassword, L"\0");
  StrCpy(UserInfoPtr->UserName,PcdGetPtr(PcdSkipInputString));
  UserInfoPtr->EnabaleChannelUser = 0;
  UserInfoPtr->Privilege          = 0xf;
  UserInfoPtr->UserPresent        = 0;
  ZeroMem(&mUserRecord, sizeof(mUserRecord));
  return EFI_SUCCESS;
}

BmcSetIpModeDefault(
   IN BMC_LAN_CONFIG *BmcCfgPtr
  )
{

  DEBUG ((EFI_D_ERROR, "BmcSetIpModeDefault()\n"));
  BmcCfgPtr->OnBoardNIC_IpMode = 2;         //ipv4 DHCP
  BmcCfgPtr->OnBoardNIC_IpSelection = 1;    //ipv6 enable
  BmcCfgPtr->OnBoardNIC_Ipv6Mode = 2;       //ipv6 DHCP
  BmcCfgPtr->DedicateNIC_IpMode = 2;
  BmcCfgPtr->DedicateNIC_IpSelection = 1;
  BmcCfgPtr->DedicateNIC_Ipv6Mode = 2;
  
  return EFI_SUCCESS;
}


EFI_STATUS
BmcCfgSetVariable (
  IN CHAR16        *VariableName,
  IN EFI_GUID      *VendorGuid,
  IN UINT32        Attributes,
  IN UINTN         DataSize,
  IN VOID          *Data
  )
/*++
Routine Description:
  This routine will intercept the L"SetupBmcCfg" Set variable function

Arguments:
  VariableName                    Name of Variable to be written
  VendorGuid                      Variable vendor GUID
  Attributes                      Attribute value of the variable found
  DataSize                        Size of Data found. If size is less than the
                                  data, this value contains the required size.
  Data                            Data pointer

Returns:
  Standard variable return values
--*/
{

  //UINT8         Index;
  UINT8         Ipv6Status=0;
  BMC_LAN_CONFIG *BmcCfgPtr;
  //BMC_USER_INFO *UserInfoPtr;
  //BMC_USER_INFO *OldUserInfoPtr;
  UINTN         Status;
  EFI_GUID      SetupBmcCfgGuid=SYSTEM_CONFIGURATION_GUID;

  if ((StrCmp (VariableName, L"BmcLanConfig" ) != 0) || (CompareGuid(VendorGuid,&SetupBmcCfgGuid)== FALSE)) {
    return EFI_NOT_FOUND;
  }
  BmcCfgPtr = (BMC_LAN_CONFIG *) Data;

  
#if 0
  {
   UINT8         *Buffer = NULL;
   UINT16         i;
   Buffer = AllocateZeroPool(sizeof(BMC_LAN_CONFIG));
   CopyMem(Buffer,BmcCfgPtr,sizeof(BMC_LAN_CONFIG));
  DEBUG ((EFI_D_ERROR, "3.BMC Cfg Save 2 Var BmcCfgPtr \n"));
  for(i = 0 ;i < sizeof (BMC_LAN_CONFIG);i++){
    DEBUG ((EFI_D_ERROR, "%02x ",*(Buffer+i)));
  }
  DEBUG((EFI_D_ERROR,"\n"));
  FreePool(Buffer);
  }
#endif


  DEBUG ((EFI_D_ERROR, "BMC Cfg Set Var :%s\n",VariableName));
  //
  // Set Lan Configuration
  //
  Status = MemCmp (
            (UINT8 *) &BmcCfgPtr->OnBoardNIC_IpMode,
            (UINT8 *) &mOldSetupBmcCfgPtr.OnBoardNIC_IpMode,
            sizeof (BMC_LAN_INFO)
            );
  if (Status == FALSE) {
    DEBUG ((EFI_D_ERROR, "beofore BMC Cfg Set BmcSetLanConfig     OnBoardNIC_IpMode:%d\n",BmcCfgPtr->OnBoardNIC_IpMode));
  
    BmcSetLanConfig ((BMC_LAN_INFO *) &BmcCfgPtr->OnBoardNIC_IpMode, BASE_BOARD_NIC_CHANNEL_SEL);
//
//BMC LAN configuration IP source value is changed.
//
    IsBMCLanConfigChanged = 1;
  }

  Status = MemCmp (
            (UINT8 *) &BmcCfgPtr->DedicateNIC_IpMode,
            (UINT8 *) &mOldSetupBmcCfgPtr.DedicateNIC_IpMode,
            sizeof (BMC_LAN_INFO)
            );
  if (Status == FALSE) {
    DEBUG ((EFI_D_ERROR, "before BMC Cfg Set BmcSetLanConfig     DedicateNIC_IpMode:%d\n",BmcCfgPtr->DedicateNIC_IpMode));
    BmcSetLanConfig ((BMC_LAN_INFO *) &BmcCfgPtr->DedicateNIC_IpMode, DEDICATE_NIC_CHANNEL_SEL);
//
//BMC LAN configuration IP source value is changed.
//
    IsBMCLanConfigChanged = 1;
  }
#if 0
  {
     UINT8         *Buffer = NULL;
     UINT16         i;
     DEBUG ((EFI_D_ERROR, "After  BMC Cfg Get BmcSetLanConfig confirm\n"));

     
     Buffer = AllocateZeroPool(sizeof(BMC_LAN_INFO));
     BmcGetLanConfig ( (BMC_LAN_INFO *)Buffer, BASE_BOARD_NIC_CHANNEL_SEL);  
     DEBUG ((EFI_D_ERROR, "4.BMC Cfg Save 2 Var BmcCfgPtr \n"));
    for(i = 0 ;i < sizeof (BMC_LAN_INFO);i++){
      DEBUG ((EFI_D_ERROR, "%02x ",*(Buffer+i)));
    }
    DEBUG((EFI_D_ERROR,"\n"));
    FreePool(Buffer);
  }
#endif
//
//Moved the code out of above if condition since the if condition satisfies only when IPV4 values are changed. When only IPV6 values are changed
//control does not get into if loop and IPV6 values are not updated into BMC.
//
//
//Check if IPV6 is Enable or Disable
//
   if(BmcCfgPtr->OnBoardNIC_IpSelection ==1){
       //
       // Set Ipv6 status to Enable since user has requested it to be enabled in BIOS setup
       //
       Ipv6Status = 2;
       GetSetIpv6Status (BASE_BOARD_NIC_CHANNEL_SEL, IPV6_STATUS_PARM, &Ipv6Status, FALSE);
        
       Status = MemCmp (
            (UINT8 *) &BmcCfgPtr->OnBoardNIC_Ipv6Mode,
            (UINT8 *) &mOldSetupBmcCfgPtr.OnBoardNIC_Ipv6Mode,
            sizeof (BMC_LAN_IPV6_INFO)
            );
        if (Status == FALSE){
          BmcSetLanIpv6Config ((BMC_LAN_IPV6_INFO *) &BmcCfgPtr->OnBoardNIC_Ipv6Mode, BASE_BOARD_NIC_CHANNEL_SEL);
      }
   }else if(BmcCfgPtr->OnBoardNIC_IpSelection == 0){
       //
       // Set Ipv6 status to disable since IPV6 option is disabled in  BIOS setup
       //
       Ipv6Status = 0;
       GetSetIpv6Status (BASE_BOARD_NIC_CHANNEL_SEL, IPV6_STATUS_PARM, &Ipv6Status, FALSE);
  }

   if(BmcCfgPtr->DedicateNIC_IpSelection ==1){
       //
       // Set Ipv6 status to Enable since user has requested it to be enabled in BIOS setup
       //
       Ipv6Status = 2;     
       GetSetIpv6Status (DEDICATE_NIC_CHANNEL_SEL, IPV6_STATUS_PARM, &Ipv6Status, FALSE);       
       Status = MemCmp (
            (UINT8 *) &BmcCfgPtr->DedicateNIC_Ipv6Mode,
            (UINT8 *) &mOldSetupBmcCfgPtr.DedicateNIC_Ipv6Mode,
            sizeof (BMC_LAN_IPV6_INFO)
            );
        if (Status == FALSE){
          BmcSetLanIpv6Config ((BMC_LAN_IPV6_INFO *) &BmcCfgPtr->DedicateNIC_Ipv6Mode, DEDICATE_NIC_CHANNEL_SEL);
      }
   }else if(BmcCfgPtr->DedicateNIC_IpSelection == 0){
       //
       // Set Ipv6 status to disable since IPV6 option is disabled in  BIOS setup
       //
       Ipv6Status = 0;      
       GetSetIpv6Status (DEDICATE_NIC_CHANNEL_SEL, IPV6_STATUS_PARM, &Ipv6Status, FALSE);
  }
  
  //
  //Moved user information setting before RMM4 configuration setting. Other than channel number rest all parameters for BMC configuration 
  //and RMM4 configuration are same. So sfter sending BMC configuartion commands a time delay is needed for BMC before sending RMM4 commands.
  //So before sending RMM4 commands we can send user configuration commands.
  //

  //
  // Set User Configuration
  //
   //patch UserPassword1~16   
   
  /* H3C Bmc User info saved by callback.
  for(Index = 0;Index<MAX_USERS;Index++){
    CopyMem (BmcCfgPtr->BmcUserInfo[Index].UserPassword, mTempSetupBmcCfg.BmcUserInfo[Index].UserPassword, 20*2);
  }

  
  UserInfoPtr     = (BMC_USER_INFO *) &(BmcCfgPtr->BmcUserInfo[0]);
  OldUserInfoPtr  = (BMC_USER_INFO *) &(mOldSetupBmcCfgPtr.BmcUserInfo[0]);
  for (Index = 1; Index <= MAX_USERS; Index++) {
    Status = MemCmp ((UINT8 *) UserInfoPtr, (UINT8 *) OldUserInfoPtr, sizeof (BMC_USER_INFO));
    if (Status == FALSE) {
      BmcSetUserInfo (UserInfoPtr, Index);
    }
    UserInfoPtr++;
    OldUserInfoPtr++;
  }
  */
  return EFI_SUCCESS;
}

EFI_STATUS
BmcCfgGetVariable (
  IN CHAR16        *VariableName,
  IN EFI_GUID      * VendorGuid,
  OUT UINT32       *Attributes OPTIONAL,
  IN OUT UINTN     *DataSize,
  OUT VOID         *Data
  )
/*++
Routine Description:
  This routine will intercept the L"SetupBmcCfg" variable reads 
Arguments:
  VariableName                    Name of Variable to be written
  VendorGuid                      Variable vendor GUID
  Attributes                      Attribute value of the variable found
  DataSize                        Size of Data found. If size is less than the
                                  data, this value contains the required size.
  Data                            Data pointer

Returns:
  Standard variable return values
--*/
{
//  UINTN               Index;
  UINT8              temp=0;
  UINT8              *Ipv6Status = &temp;
  BMC_LAN_CONFIG       *BmcCfgPtr;
  //BMC_USER_INFO       *UserInfoPtr;
  EFI_GUID            SetupBmcCfgGuid=SYSTEM_CONFIGURATION_GUID;
#if 1
  UINT8         *Buffer = NULL;
#endif  
  if (( StrCmp (VariableName, L"BmcLanConfig" ) != 0) || (CompareGuid(VendorGuid,&SetupBmcCfgGuid)== FALSE)) {
    return EFI_NOT_FOUND;
  }

#if 0
{
  UINT8       commanddata[20];
  UINT8       commanddatasize;
  UINT8       Response[20];
  UINT8       Responsesize;
  UINTN       i,j;
  EFI_STATUS  Status;

  ZeroMem (commanddata, 20);
  ZeroMem (Response, 20); 

  for(i=1;i<0xb;i++){
  //
  // Get Channel info
  //
  commanddata[0] = (UINT8)i;         // Channel number 
  commanddatasize = 1;
  Responsesize    = 20;

    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_APP,
              EFI_APP_GET_CHANNEL_INFO,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );
    DEBUG((EFI_D_ERROR," Get Channel Info[%d] = %r  Responsesize=%x  j=",i,Status,Responsesize));
    for(j=0;j<Responsesize;j++){
     DEBUG((EFI_D_ERROR," 0x%02x ",Response[j]));
    
    }
    DEBUG((EFI_D_ERROR,"\n"));
    
  }
}
#endif

  //
  // Check return Buffer size
  //
  if (*DataSize < sizeof (BMC_LAN_CONFIG)) {
    *DataSize = sizeof (BMC_LAN_CONFIG);
    return EFI_BUFFER_TOO_SMALL;
  }
  *DataSize = sizeof (BMC_LAN_CONFIG);
  BmcCfgPtr = (BMC_LAN_CONFIG *) Data;
  ZeroMem (BmcCfgPtr, *DataSize);

  //BmcCfgPtr->User       = 1;

  DEBUG ((EFI_D_ERROR, "BMC Cfg Get Var :%s size = %x\n",VariableName,*DataSize));

  //
  // Get Onboard Lan Configuration
  //
  GetSetIpv6Status(BASE_BOARD_NIC_CHANNEL_SEL, IPV6_STATUS_PARM, Ipv6Status, TRUE);
  BmcGetLanConfig ((BMC_LAN_INFO *) &BmcCfgPtr->OnBoardNIC_IpMode, BASE_BOARD_NIC_CHANNEL_SEL);


 //
 // Configure IPV6 if user has requested
 // 
  if(*Ipv6Status == 2){
    BmcCfgPtr->OnBoardNIC_IpSelection = 1;
  }else{ 
    BmcCfgPtr->OnBoardNIC_IpSelection = 0;
  }
  BmcGetLanIPv6Config ((BMC_LAN_IPV6_INFO *) &BmcCfgPtr->OnBoardNIC_Ipv6Mode, BASE_BOARD_NIC_CHANNEL_SEL);
  
  //
  // Get Dedicate Lan Configuration
  //  
  GetSetIpv6Status(DEDICATE_NIC_CHANNEL_SEL, IPV6_STATUS_PARM, Ipv6Status, TRUE);
  BmcGetLanConfig ((BMC_LAN_INFO *) &BmcCfgPtr->DedicateNIC_IpMode, DEDICATE_NIC_CHANNEL_SEL);
 //
 // Configure IPV6 if user has requested
 // 
  if(*Ipv6Status == 2){
    BmcCfgPtr->DedicateNIC_IpSelection = 1;
  }else{
    BmcCfgPtr->DedicateNIC_IpSelection = 0;
  }  
  BmcGetLanIPv6Config ((BMC_LAN_IPV6_INFO *) &BmcCfgPtr->DedicateNIC_Ipv6Mode, DEDICATE_NIC_CHANNEL_SEL);
  
  //
  // Get User Configuration
  //  
  //UserInfoPtr = (BMC_USER_INFO *) &(BmcCfgPtr->BmcUserInfo[0]);
  //for (Index = 1; Index <= MAX_USERS; Index++) { 
  //  BmcGetUserInfo (UserInfoPtr++, (UINT8)Index);
  //}
  BmcSetUserDefault(BmcCfgPtr);  

#if 0
  Buffer = AllocateZeroPool(sizeof(BMC_LAN_CONFIG));
  CopyMem(Buffer,BmcCfgPtr,sizeof(BMC_LAN_CONFIG));

  DEBUG ((EFI_D_ERROR, "1.BMC Cfg Get Var BmcCfgPtr \n"));
  for(Index = 0 ;Index < sizeof (BMC_LAN_CONFIG);Index++){
    DEBUG ((EFI_D_ERROR, "%02x ",*(Buffer+Index)));
  }
  DEBUG((EFI_D_ERROR,"\n"));
  FreePool(Buffer);

#endif
  
  //
  // Save a copy of BMC_LAN_CONFIG
  //
  CopyMem (&mOldSetupBmcCfgPtr, BmcCfgPtr, sizeof (BMC_LAN_CONFIG));
  CopyMem (&mTempSetupBmcCfg, BmcCfgPtr, sizeof (BMC_LAN_CONFIG));
  
  return EFI_SUCCESS;
}

EFI_STATUS 
isValidDHCPName ( 
  CHAR16 *String
  )
/*++

Routine Description:

  Check whether user entered DHCP name string is valid

Arguments:

  String to check.

Returns:

  EFI_SUCCESS - Valid DHCP name string.
  EFI_UNSUPPORTED - Not a valid DHCP name string.

--*/
{
  UINTN  Strlen,Index;
  UINT16  Character;

  Strlen = StrLen(String);
  for(Index= 0 ; Index < Strlen; Index++) {
     Character= String[ Index];
     if(((Character >= L'A')&&(Character <= L'Z')) ||((Character >= L'a')&& (Character <= L'z'))) {
       continue;
     }
     if((Character >= L'0')&& (Character <= L'9') && Index != 0 ) {
       continue;
     }

     if((Character == L'-' && Index != 0)){
       continue;
     }
     break;
  }
  if( Index == Strlen) {
    return EFI_SUCCESS;
    } else {
    return EFI_UNSUPPORTED;
  }
}

EFI_STATUS
BmcGetUserPrivilege (
  IN BMC_USER_INFO  *Userinfo,
  IN UINT8          UserId
  )
{
    EFI_STATUS  Status;
    UINT8       commanddata[20];
    UINT8       commanddatasize;
    UINT8       Response[20];
    UINT8       Responsesize;
  

    commanddata[0]  = 8;//i;   //channel  1 & 8
    commanddata[1]  = UserId; 
    commanddatasize = 2;
    Responsesize    = 20;

    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_APP,
              GET_USER_ACCESS_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );
       

    if (Status == EFI_SUCCESS) {
      Userinfo->Privilege = (UINT8) (Response[3] & 0xf) > 4 ? 0xf : (UINT8) (Response[3] & 07);
      // Bits 7&6 = user enable(01)/disable(10).IPMI Spec 2.0 June 12,2009 Markup
      if ((Response[1] & 0xC0) == 0x40) { 
        Userinfo->EnabaleChannelUser = 1;
      }else{
        Userinfo->EnabaleChannelUser = 0;
      }
      //
      //Assume all channel has the same user list.
      //
      //break;
    }else{
       Userinfo->Privilege          = 1;
       Userinfo->EnabaleChannelUser = 0;
    }
  return  Status;   
}


EFI_STATUS
BmcSetUserName (
  IN CHAR16       *UserName,
  IN UINT8         UserId
  )
{


  UINT8       commanddata[24];  //Predefine User Name/Privilege/Password command data buffer with maximal buffer length and necessary redundancy.
  UINT8       commanddatasize;  //Send to BMC data size.
  UINT8       Response[24];
  UINT8       Responsesize;
  EFI_STATUS  Status;
  
  Status = EFI_UNSUPPORTED;
  //
  // Set User Name
  //
  if ((UserId != 0) && (UserId != 1)) {
    commanddata[0] = UserId;
    ZeroMem (&commanddata[1], 16);
    Unicode2Ascii (&commanddata[1], UserName);
    commanddatasize = 17;
    Responsesize    = 10;

    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_APP,
              SET_USER_NAME_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );
  }
  return  Status;   
}


EFI_STATUS
BmcSetBmcPassword (
  IN UINT8       PassordSize,
  IN CHAR16       *Password,
  IN UINT8         UserId
  )
{
  UINT8       commanddata[24];  //Predefine User Name/Privilege/Password command data buffer with maximal buffer length and necessary redundancy.
  UINT8       commanddatasize;  //Send to BMC data size.
  UINT8       Response[24];
  UINT8       Responsesize;
  EFI_STATUS  Status;

  //
  // Set User Password
  //
  if (StrLen (Password) != 0) {
    commanddata[0] = UserId | (PassordSize << 7);      // userid: Enable BIT7 to support 20 bytes user passwords.
    commanddata[1] = 02;                 // operation: Set Password
    ZeroMem (&commanddata[2], 20);       // Zero Password data
    Unicode2Ascii (&commanddata[2], Password);
    commanddatasize = 22;                // commanddatasize: Byte[1]:UserId,Byte[2]:operation,Byte[3-22]:Password data.Please refer IPMI2.0 Spec.
    Responsesize    = 24;

    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_APP,
              SET_USER_PASSWORD_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );
  }
  
  return  Status;   
}

EFI_STATUS
BmcSetUserAccess (
  IN BMC_USER_INFO  *Userinfo,
  IN UINT8          UserId
  )
{

  UINT8       commanddata[24];  //Predefine User Name/Privilege/Password command data buffer with maximal buffer length and necessary redundancy.
  UINT8       commanddatasize;  //Send to BMC data size.
  UINT8       Response[24];
  UINT8       Responsesize;
  EFI_STATUS  Status;
  UINT8       Index;

  //
  // Set User Privilege and enable user channel
  //
  commanddata[0] = UserId;      // userid
  if((Userinfo->EnabaleChannelUser == 0)) {
    commanddata[1] = 00;          // Disable User
    } else {
    commanddata[1] = 01;          // Enable User
  }

  ZeroMem (&commanddata[2], 16);
  commanddatasize = 18;
  Responsesize    = 20;

  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_APP,
            SET_USER_PASSWORD_CMD,
            (UINT8 *) &commanddata[0],
            commanddatasize,
            (UINT8 *) &Response,
            (UINT8 *) &Responsesize
            );

  for (Index = 0; Index < 2; Index++) {
    if (Index == 1) {
      //Byosoft +
      commanddata[0] = 0xB0|BASE_BOARD_NIC_CHANNEL_SEL;                // Channel 1
      //commanddata[0] = 0xB8;                // Channel 8
      //Byosoft +
    } else {
      commanddata[0] = 0xB0|DEDICATE_NIC_CHANNEL_SEL;                // channel 8
    }
    commanddata[1] = UserId;                // userid
    commanddata[2] = Userinfo->Privilege;   // Privilege
    commanddata[3] = 00;                    // Session Limit
    commanddatasize = 4;
    Responsesize    = 20;

    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_APP,
              SET_USER_ACCESS_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );
  }
  return Status;
}



EFI_STATUS
BmcDelUser (
  IN UINT8         UserId
  )
{


  UINT8       commanddata[24] = {0};  //Predefine User Name/Privilege/Password command data buffer with maximal buffer length and necessary redundancy.
  UINT8       commanddatasize;  //Send to BMC data size.
  UINT8       Response[24] = {0};
  UINT8       Responsesize;
  EFI_STATUS  Status;
  UINT8       Index;
  UINTN           VarSize;
  BMC_SETUP_DATA  SetupData;
  EFI_GUID        SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;

  VarSize = sizeof (BMC_SETUP_DATA);
  Status = gRT->GetVariable (
                  BMC_SETUP_VARIABLE_NAME,
                  &SystemConfigurationGuid,
                  NULL,
                  &VarSize,
                  &SetupData
                  );
  
  Status = EFI_UNSUPPORTED;


  DEBUG((EFI_D_INFO,"UserId is %d\n",UserId));
  if(SetupData.BmcVendor){
    if ( UserId != 0) {
      commanddata[0] = UserId;
      SetMem( &commanddata[1], 16, 0xff);
      commanddatasize = 17;
      Responsesize    = 10;
      Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_APP,
              SET_USER_NAME_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );
     }
  }else{//byo bmc
  //
  // Set User Name
  //
  if ((UserId != 0) && (UserId != 1)) {
    commanddata[0] = UserId;
    SetMem( &commanddata[1], 16, 0x20);
    commanddatasize = 17;
    Responsesize    = 10;
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_APP,
              SET_USER_NAME_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );
  }
  for (Index = 0; Index < 2; Index++) {
    if (Index == 1) {
      commanddata[0] = 0x80|BASE_BOARD_NIC_CHANNEL_SEL;              // Channel 1
    } else {
      commanddata[0] = 0x80|DEDICATE_NIC_CHANNEL_SEL;                // channel 8
    }
    commanddata[1] = UserId;                // userid
    commanddata[2] = 0x0f;                   // Privilege
    commanddata[3] = 00;                    // Session Limit
    commanddatasize = 4;
    Responsesize    = 20;

    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_APP,
              SET_USER_ACCESS_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );
    }
  }
  return  Status;   
}






UINT8
TestUserPassword(
 IN UINT8          UserId,
 IN UINT16         *Password
)
{
  UINT8       commanddata[24];  //Predefine User Name/Privilege/Password command data buffer with maximal buffer length and necessary redundancy.
  UINT8       commanddatasize;  //Send to BMC data size.
  UINT8       Response[24];
  UINT8       Responsesize;
  EFI_STATUS  Status;
  //
  // Set User Password
  //
  DEBUG ((EFI_D_ERROR, "StrLen (Password) = %x\n",StrLen (Password)));
  if(StrLen (Password) == 0){
    return 0x81;
  }
  commanddata[0] = UserId | BIT7;      // userid: Enable BIT7 to support 20 bytes user passwords.
  commanddata[1] = 03;                 // operation: Test Password
  ZeroMem (&commanddata[2], 20);       // Zero Password data
  Unicode2Ascii (&commanddata[2], Password);
  commanddatasize = 22;                // commanddatasize: Byte[1]:UserId,Byte[2]:operation,Byte[3-22]:Password data.Please refer IPMI2.0 Spec.
  Responsesize    = 24;

  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_APP,
            SET_USER_PASSWORD_CMD,
            (UINT8 *) &commanddata[0],
            commanddatasize,
            (UINT8 *) &Response,
            (UINT8 *) &Responsesize
            );
  DEBUG ((EFI_D_ERROR, "EfiIGetLastIpmiCmdCompleteCode  %X :\n",EfiIGetLastIpmiCmdCompleteCode()));
  if(EfiIGetLastIpmiCmdCompleteCode()==0x81)
  {
	commanddatasize = 18;
	Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_APP,
            SET_USER_PASSWORD_CMD,
            (UINT8 *) &commanddata[0],
            commanddatasize,
            (UINT8 *) &Response,
            (UINT8 *) &Responsesize
            );
  }
  return EfiIGetLastIpmiCmdCompleteCode();
}


BOOLEAN 
CheckUserNameMatch(CHAR16 *UserName)
{
  UINTN   i;
  UINTN   Length;
  Length = StrLen(UserName);
  if(Length<5){
	return FALSE;
  }
  if((UserName[0]<'A') || (UserName[0]>'Z' && UserName[0]<'a') || (UserName[0]>'z')){
	return FALSE;  //first must be letter
  }
  for(i=0;i<Length;i++){
    if((UserName[i] >= '0') && (UserName[i] <= '9')) {
      continue;
    } 
    if((UserName[i] >= 'A') && (UserName[i] <= 'Z')) {
      continue;
    }  
    if((UserName[i] >= 'a') && (UserName[i] <= 'z')) {
      continue;
    }
    if(UserName[i] == '_') {
      continue;
    }
    return FALSE;
  }
  return TRUE;
}


VOID 
ShowSingleChar16 (
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *ConOut,
  CHAR16                           c
  )
{
  CHAR16  Buffer[2];
  ZeroMem(Buffer, sizeof(Buffer));
  Buffer[0] = c;
  ConOut->OutputString(ConOut, Buffer);  
}


EFI_STATUS
DrawInputDialog (
  EFI_HII_HANDLE      HiiHandle,
  IN  CHAR16         *Title,
  OUT CHAR16          *Password
  )
{
  EFI_STATUS                        Status = EFI_SUCCESS;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *ConOut;
  EFI_SIMPLE_TEXT_OUTPUT_MODE       SavedConsoleMode;
  UINTN                             Columns;
  UINTN                             Rows;
  UINTN                             Column;
  UINTN                             Row;
  UINTN                             NumberOfLines;
  UINTN                             MaxLength;
  CHAR16                           *Line = NULL;
  UINTN                             Index = 0;
  UINTN                             LineIndex;
  USER_INPUT_LINE_CTX               LineCtx[1];
  UINTN                             PasswordSize = MAX_STRING_SIZE;
  UINTN                             AttribTable, AttribInput;
  //CHAR16                           *EnterNewPassword;
  EFI_INPUT_KEY                     Key;
  USER_INPUT_TYPE                   InputType;
  MOUSE_ACTION_INFO                 Mouse;
  
  //EnterNewPassword = HiiGetString(HiiHandle, STRING_TOKEN(STR_DELETE_USER_ACCOUNT), NULL);

  AttribTable = EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE;
  AttribInput = EFI_WHITE | EFI_BACKGROUND_BLUE;

  BltSaveAndRetore (gBS, TRUE);

  ConOut = gST->ConOut;
  CopyMem(&SavedConsoleMode, ConOut->Mode, sizeof(SavedConsoleMode));
  ConOut->QueryMode(ConOut, SavedConsoleMode.Mode, &Columns, &Rows); //128 * 40
  ConOut->EnableCursor(ConOut, TRUE);
  ConOut->SetAttribute(ConOut, AttribTable);

  NumberOfLines = 5;
  MaxLength = MAX_STRING_SIZE + 2;
  Row    = (Rows - NumberOfLines) / 2;
  Column = (Columns - (MaxLength + 2)) / 2;
  Line = AllocateZeroPool((MaxLength + 2 + 1) * sizeof(CHAR16));      // '|', xxxx, '|', '\0'
  ASSERT(Line != NULL);

  SetMem16(Line, (MaxLength + 2) * 2, BOXDRAW_HORIZONTAL);
  Line[0]             = BOXDRAW_DOWN_RIGHT;
  Line[MaxLength + 1] = BOXDRAW_DOWN_LEFT;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row++);
  ConOut->OutputString(ConOut, Line);

  SetMem16(Line, (MaxLength + 2) * 2, L' ');
  Line[0]             = BOXDRAW_VERTICAL;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row);
  ConOut->OutputString(ConOut, Line);
  ConOut->SetCursorPosition(ConOut, Column + 1 , Row++);
  ConOut->OutputString(ConOut, Title);

  SetMem16(Line, (MaxLength + 2) * 2, BOXDRAW_HORIZONTAL);
  Line[0]             = BOXDRAW_VERTICAL_RIGHT;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL_LEFT;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row++);
  ConOut->OutputString(ConOut, Line);

  //SetMem16(Line, (MaxLength + 2) * 2, L' ');
  //Line[0]             = BOXDRAW_VERTICAL;
  //Line[MaxLength + 1] = BOXDRAW_VERTICAL;
  //Line[MaxLength + 2] = L'\0';
  //ConOut->SetCursorPosition(ConOut, Column, Row);
  //ConOut->OutputString(ConOut, Line);
  //ConOut->SetCursorPosition(ConOut, Column+1, Row++);
  //ConOut->OutputString(ConOut, EnterNewPassword);

  SetMem16(Line, (MaxLength + 2) * 2, L' ');
  Line[0]             = BOXDRAW_VERTICAL;
  Line[1]             = L'[';
  Line[MaxLength]     = L']';
  Line[MaxLength + 1] = BOXDRAW_VERTICAL;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row);
  ConOut->OutputString(ConOut, Line);
  LineCtx[0].InputX = Column+2;
  LineCtx[0].InputY = Row;
  LineCtx[0].String = Password;
  Row ++;

  SetMem16(Line, (MaxLength + 2) * 2, BOXDRAW_HORIZONTAL);
  Line[0]             = BOXDRAW_UP_RIGHT;
  Line[MaxLength + 1] = BOXDRAW_UP_LEFT;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row++);
  ConOut->OutputString(ConOut, Line);

  LineIndex = 0;
  ConOut->SetCursorPosition(ConOut, LineCtx[LineIndex].InputX, LineCtx[LineIndex].InputY);
  ConOut->SetAttribute(ConOut, AttribInput);

  while (TRUE) {
    InputType = WaitForUserInput(&Key, &Mouse);
    if (USER_INPUT_MOUSE == InputType) {
      if (MOUSE_RIGHT_CLICK == Mouse.Action) {
        Key.UnicodeChar = CHAR_NULL;
        Key.ScanCode = SCAN_ESC;
      }
    }

    if(Key.ScanCode == SCAN_ESC){
      Status = EFI_ABORTED;
      break;
    }
    if(Key.ScanCode == SCAN_NULL){
      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        LineCtx[LineIndex].String[Index] = 0;         // give a end flag.
        if (LineIndex == 0) {
          if (StrLen(Password) == 0) {
            continue;
          }
          DEBUG((DEBUG_INFO, "First Password:%s\n",Password));
          break; //input done
        }
      } else if(Key.UnicodeChar == CHAR_BACKSPACE){
        if(Index){
          Index --;
          LineCtx[LineIndex].String[Index] = 0;
          ShowSingleChar16(ConOut, CHAR_BACKSPACE);
        }
      } else {
        if(Index < PasswordSize){
          LineCtx[LineIndex].String[Index] = Key.UnicodeChar;
          ShowSingleChar16(ConOut, L'*');
          Index ++;
        }
      }
    }
  }// while (TRUE)

  //FreePool(EnterNewPassword);
  FreePool(Line);
  ConOut->EnableCursor(ConOut, FALSE);
  ConOut->SetCursorPosition(ConOut, SavedConsoleMode.CursorColumn, SavedConsoleMode.CursorRow);
  ConOut->SetAttribute(ConOut, SavedConsoleMode.Attribute);

  BltSaveAndRetore (gBS, FALSE);

  return Status;
}






EFI_STATUS
ValidateSetupInput (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
/*++

Routine Description:

  Validate user strings entered.

Arguments:

  @param[in]  This               Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]  Action             Specifies the type of action taken by the browser.
  @param[in]  QuestionId         A unique value which is sent to the original
                                 exporting driver so that it can identify the type
                                 of data to expect. The format of the data tends to
                                 vary based on the opcode that enerated the callback.
  @param[in]  Type               The type of value for the question.
  @param[in]  Value              A pointer to the data being sent to the original
                                 exporting driver.
  @param[out]  ActionRequest     On return, points to the action requested by the
                                 callback function.

  @retval EFI_SUCCESS            The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES   Not enough storage is available to hold the
                                 variable and its data.
  @retval EFI_DEVICE_ERROR       The variable could not be saved.
  @retval EFI_UNSUPPORTED        The specified Action is not supported by the
                                 callback.Currently not implemented.
  @retval EFI_INVALID_PARAMETERS Passing in wrong parameter.
  @retval Others                 Other errors as indicated.

Returns:

  GC_TODO: add return values

--*/
{
  EFI_STATUS        Status;
  EFI_IPv4_ADDRESS  Ip_Add;
  EFI_IPv6_ADDRESS  Ipv6_Add;
  BMC_LAN_CONFIG    *TempBmcLanConfig = NULL;
  EFI_GUID          SetupBmcCfgGuid = SYSTEM_CONFIGURATION_GUID;
  EFI_INPUT_KEY     Key;  
  UINT16            TempUserName[20];
  //UINT16            FakeUserName[20];
  UINT8             Index;
  BMC_USER_INFO     *UserInfoPtr;
  BMC_USER_INFO     *OldUserInfoPtr;  
  CHAR16            *StringBuffer1;
  CHAR16            *StringBuffer11;
  CHAR16            *StringBuffer2;
  CHAR16            *StringBuffer3;
  CHAR16            *StringBuffer4;  
  CHAR16            *StringBuffer5;  
  CHAR16            *Password;   
  static CHAR16     CurrentUserLastPwd[20];   
  static UINTN      PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
  static UINTN      UserState = 0xFF;

  Status         = EFI_SUCCESS;  
  Index          = 0;
  UserInfoPtr    = NULL;
  OldUserInfoPtr = NULL;
  StringBuffer1  = NULL;
  StringBuffer2  = NULL;
  StringBuffer3  = NULL;
  StringBuffer4  = NULL;
  StringBuffer5  = NULL;
  Password       = NULL;
  ZeroMem(TempUserName, sizeof(TempUserName));
  ZeroMem(&Ipv6_Add, sizeof(Ipv6_Add));

  DEBUG ((EFI_D_ERROR, "[ValidateSetupInput] QuestionID:%d, Action:%d\n",QuestionId,Action));

  //if (Type != EFI_IFR_TYPE_STRING) {
  //  return EFI_SUCCESS;
  //}

  if (Action == EFI_BROWSER_ACTION_FORM_OPEN ||
     Action == EFI_BROWSER_ACTION_RETRIEVE ) {
    return EFI_SUCCESS;
  }

  //
  // Retrive uncommitted data from Browser
  //
  TempBmcLanConfig = AllocateZeroPool (sizeof (BMC_LAN_CONFIG));
  if (TempBmcLanConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }  
  if (!HiiGetBrowserData (&SetupBmcCfgGuid, L"BmcLanConfig", sizeof (BMC_LAN_CONFIG), (UINT8 *) TempBmcLanConfig)) {
    FreePool (TempBmcLanConfig);
    return EFI_NOT_FOUND;
  }  

  if( Action == EFI_BROWSER_ACTION_FORM_CLOSE){
     BmcSetUserDefault(TempBmcLanConfig);
     HiiSetBrowserData (
              &SetupBmcCfgGuid,
              L"BmcLanConfig",
              sizeof (BMC_LAN_CONFIG),
              (UINT8 *) TempBmcLanConfig,
              NULL
              );  
              
     FreePool(TempBmcLanConfig); 
     return EFI_SUCCESS;
  }   

  if (Action == EFI_BROWSER_ACTION_CHANGED) {
    //
    // Check Lan parameters IP address , subnet Mask ,GateWay IP format
    //
    switch (QuestionId) {
      case KEY_BMC_ONBOARD_NIC_IPV4_ADDR:
          Status = StrStringToIp4Adr (&Ip_Add, TempBmcLanConfig->OnBoardNIC_IpAddress);
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpy (TempBmcLanConfig->OnBoardNIC_IpAddress, mTempSetupBmcCfg.OnBoardNIC_IpAddress);
          StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_INVALID_IP), NULL);  
          StringBuffer2 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_GATEWAY), NULL);  
          StringBuffer3 = HiiGetString(HiiHandle, STRING_TOKEN(STR_IPV4_FORMAT), NULL);  
          StringBuffer4 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);          
          do {
            CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3,StringBuffer4, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);
        } else {
          StrCpy (mTempSetupBmcCfg.OnBoardNIC_IpAddress,TempBmcLanConfig->OnBoardNIC_IpAddress);    
          //Status = BmcSetMyLanConfig (BASE_BOARD_NIC_CHANNEL_SEL);
          //GetSetIpPram (BASE_BOARD_NIC_CHANNEL_SEL, IP_ADDRESS_PARM, TempBmcLanConfig->OnBoardNIC_IpAddress, FALSE);
        }
        break;

      case KEY_BMC_ONBOARD_NIC_IPV4_SUBNET_MASK_ADDR:
          Status = StrStringToIp4Adr (&Ip_Add, TempBmcLanConfig->OnBoardNIC_SubnetMask);
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpy (TempBmcLanConfig->OnBoardNIC_SubnetMask, mTempSetupBmcCfg.OnBoardNIC_SubnetMask);
          StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_INVALID_SUBNET), NULL);  
          StringBuffer2 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_GATEWAY), NULL);  
          StringBuffer3 = HiiGetString(HiiHandle, STRING_TOKEN(STR_IPV4_FORMAT), NULL);  
          StringBuffer4 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);        
          do {
            CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3,StringBuffer4, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);
        } else {
          StrCpy (mTempSetupBmcCfg.OnBoardNIC_SubnetMask,TempBmcLanConfig->OnBoardNIC_SubnetMask);      
          //Status = BmcSetMyLanConfig (BASE_BOARD_NIC_CHANNEL_SEL);
          //GetSetIpPram (BASE_BOARD_NIC_CHANNEL_SEL, SUBNET_MASK_PARM, TempBmcLanConfig->OnBoardNIC_SubnetMask, FALSE);  
        }
        break;

      case KEY_BMC_ONBOARD_NIC_IPV4_GATEWAY_ADDR:
          Status = StrStringToIp4Adr (&Ip_Add, TempBmcLanConfig->OnBoardNIC_GatewayIp);
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpy (TempBmcLanConfig->OnBoardNIC_GatewayIp, mTempSetupBmcCfg.OnBoardNIC_GatewayIp);
           StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_INVALID_GATEWAY_IP), NULL);  
           StringBuffer2 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_GATEWAY), NULL);  
           StringBuffer3 = HiiGetString(HiiHandle, STRING_TOKEN(STR_IPV4_FORMAT), NULL);  
           StringBuffer4 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);
           do {
             CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3, StringBuffer4 ,NULL);
           } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);
        } else {
          StrCpy (mTempSetupBmcCfg.OnBoardNIC_GatewayIp,TempBmcLanConfig->OnBoardNIC_GatewayIp);      
          //Status = BmcSetMyLanConfig (BASE_BOARD_NIC_CHANNEL_SEL);  
          //GetSetIpPram (BASE_BOARD_NIC_CHANNEL_SEL, GATEWAY_IP_PARM, TempBmcLanConfig->OnBoardNIC_GatewayIp, FALSE);   
        }
        break;
      case KEY_BMC_DEDICATE_NIC_IPV4_ADDR:
          Status = StrStringToIp4Adr (&Ip_Add, TempBmcLanConfig->DedicateNIC_IpAddress);
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpy (TempBmcLanConfig->DedicateNIC_IpAddress, mTempSetupBmcCfg.DedicateNIC_IpAddress);
          StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_INVALID_IP), NULL);  
          StringBuffer2 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_GATEWAY), NULL);  
          StringBuffer3 = HiiGetString(HiiHandle, STRING_TOKEN(STR_IPV4_FORMAT), NULL);  
          StringBuffer4 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);
          do {
            CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3, StringBuffer4 ,NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);
        } else {
          StrCpy (mTempSetupBmcCfg.DedicateNIC_IpAddress,TempBmcLanConfig->DedicateNIC_IpAddress);    
          //Status = BmcSetMyLanConfig (DEDICATE_NIC_CHANNEL_SEL);
          //GetSetIpPram (DEDICATE_NIC_CHANNEL_SEL, IP_ADDRESS_PARM, TempBmcLanConfig->DedicateNIC_IpAddress, FALSE); 
        }
        break;

      case KEY_BMC_DEDICATE_NIC_IPV4_SUBNET_MASK_ADDR:
          Status = StrStringToIp4Adr (&Ip_Add, TempBmcLanConfig->DedicateNIC_SubnetMask);
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpy (TempBmcLanConfig->DedicateNIC_SubnetMask, mTempSetupBmcCfg.DedicateNIC_SubnetMask);
          StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_INVALID_SUBNET), NULL);  
          StringBuffer2 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_GATEWAY), NULL);  
          StringBuffer3 = HiiGetString(HiiHandle, STRING_TOKEN(STR_IPV4_FORMAT), NULL);  
          StringBuffer4 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);
          do {
            CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3,StringBuffer4, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);
        } else {
          StrCpy (mTempSetupBmcCfg.DedicateNIC_SubnetMask,TempBmcLanConfig->DedicateNIC_SubnetMask);      
          //Status = BmcSetMyLanConfig (DEDICATE_NIC_CHANNEL_SEL);
          //GetSetIpPram (DEDICATE_NIC_CHANNEL_SEL, SUBNET_MASK_PARM, TempBmcLanConfig->DedicateNIC_SubnetMask, FALSE);  
        }
        break;

      case KEY_BMC_DEDICATE_NIC_IPV4_GATEWAY_ADDR:
          Status = StrStringToIp4Adr (&Ip_Add, TempBmcLanConfig->DedicateNIC_GatewayIp);
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpy (TempBmcLanConfig->DedicateNIC_GatewayIp, mTempSetupBmcCfg.DedicateNIC_GatewayIp);
          StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_INVALID_GATEWAY_IP), NULL);  
          StringBuffer2 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_GATEWAY), NULL);  
          StringBuffer3 = HiiGetString(HiiHandle, STRING_TOKEN(STR_IPV4_FORMAT), NULL);  
          StringBuffer4 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);
          do {
            CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3,StringBuffer4, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);
        } else {
          StrCpy (mTempSetupBmcCfg.DedicateNIC_GatewayIp,TempBmcLanConfig->DedicateNIC_GatewayIp);      
          //Status = BmcSetMyLanConfig (DEDICATE_NIC_CHANNEL_SEL);
          //GetSetIpPram (DEDICATE_NIC_CHANNEL_SEL, GATEWAY_IP_PARM, TempBmcLanConfig->DedicateNIC_GatewayIp, FALSE);  
        }
        break;
        
        case KEY_BMC_ONBOARD_NIC_IPV6_ADDR:
          Status = StrStringToIp6Adr (&Ipv6_Add, TempBmcLanConfig->OnBoardNIC_Ipv6Address);
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpy (TempBmcLanConfig->OnBoardNIC_Ipv6Address, mTempSetupBmcCfg.OnBoardNIC_Ipv6Address);
          StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_INVALID_IP), NULL);  
          StringBuffer2 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_IP), NULL);  
          StringBuffer3 = HiiGetString(HiiHandle, STRING_TOKEN(STR_IPV6_FORMAT), NULL);  
          StringBuffer4 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);
          do {
             CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3, StringBuffer4, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);
        } else {
          StrCpy (mTempSetupBmcCfg.OnBoardNIC_Ipv6Address,TempBmcLanConfig->OnBoardNIC_Ipv6Address);      
          //Status = BmcSetMyLanConfig (BASE_BOARD_NIC_CHANNEL_SEL);
          //GetSetIpPram (BASE_BOARD_NIC_CHANNEL_SEL, IPV6_ADDRESS_PARM, TempBmcLanConfig->OnBoardNIC_Ipv6Address, FALSE);
        }
        break;

      case KEY_BMC_ONBOARD_NIC_IPV6_GATEWAY_ADDR:
          Status = StrStringToIp6Adr (&Ipv6_Add, TempBmcLanConfig->OnBoardNIC_GatewayIpv6);
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpy (TempBmcLanConfig->OnBoardNIC_GatewayIpv6, mTempSetupBmcCfg.OnBoardNIC_GatewayIpv6);
          StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_INVALID_GATEWAY_IP), NULL);  
          StringBuffer2 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_GATEWAY), NULL);  
          StringBuffer3 = HiiGetString(HiiHandle, STRING_TOKEN(STR_IPV6_FORMAT), NULL);  
          StringBuffer4 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL); 
          do {
           CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3, StringBuffer4, NULL);
           } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);
        } else {
          StrCpy (mTempSetupBmcCfg.OnBoardNIC_GatewayIpv6,TempBmcLanConfig->OnBoardNIC_GatewayIpv6);      
          //Status = BmcSetMyLanConfig (BASE_BOARD_NIC_CHANNEL_SEL);
          //GetSetIpPram (BASE_BOARD_NIC_CHANNEL_SEL, GATEWAY_IPV6_PARM, TempBmcLanConfig->OnBoardNIC_GatewayIpv6, FALSE);
        }
        break;

      case KEY_BMC_DEDICATE_NIC_IPV6_ADDR:
          Status = StrStringToIp6Adr (&Ipv6_Add, TempBmcLanConfig->DedicateNIC_Ipv6Address);
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpy (TempBmcLanConfig->DedicateNIC_Ipv6Address, mTempSetupBmcCfg.DedicateNIC_Ipv6Address);
          StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_INVALID_IP), NULL);  
          StringBuffer2 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_IP), NULL);  
          StringBuffer3 = HiiGetString(HiiHandle, STRING_TOKEN(STR_IPV6_FORMAT), NULL);  
          StringBuffer4 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);  
          do {
            CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3, StringBuffer4, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);
        } else {
          StrCpy (mTempSetupBmcCfg.DedicateNIC_Ipv6Address,TempBmcLanConfig->DedicateNIC_Ipv6Address);      
         //Status = BmcSetMyLanConfig (DEDICATE_NIC_CHANNEL_SEL);
         // GetSetIpPram (DEDICATE_NIC_CHANNEL_SEL, IPV6_ADDRESS_PARM, TempBmcLanConfig->DedicateNIC_Ipv6Address, FALSE);
        }
        break;

      case KEY_BMC_DEDICATE_NIC_IPV6_GATEWAY_ADDR:
          Status = StrStringToIp6Adr (&Ipv6_Add, TempBmcLanConfig->DedicateNIC_GatewayIpv6);
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpy (TempBmcLanConfig->DedicateNIC_GatewayIpv6, mTempSetupBmcCfg.DedicateNIC_GatewayIpv6);
          StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_INVALID_GATEWAY_IP), NULL);  
          StringBuffer2 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_GATEWAY), NULL);  
          StringBuffer3 = HiiGetString(HiiHandle, STRING_TOKEN(STR_IPV6_FORMAT), NULL);  
          StringBuffer4 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);           
          do {
            CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3, StringBuffer4, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);
        } else {
          StrCpy (mTempSetupBmcCfg.DedicateNIC_GatewayIpv6,TempBmcLanConfig->DedicateNIC_GatewayIpv6);      
         // Status = BmcSetMyLanConfig (DEDICATE_NIC_CHANNEL_SEL);
          //GetSetIpPram (DEDICATE_NIC_CHANNEL_SEL, GATEWAY_IPV6_PARM, TempBmcLanConfig->DedicateNIC_GatewayIpv6, FALSE);
        }
        break;

      case KEY_BMC_ONBOARD_NIC_DHCP_NAME:
        Status = isValidDHCPName(TempBmcLanConfig->Dhcp_Name);
        if( Status ==  EFI_UNSUPPORTED) { 
          StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_DHCP_INVALID), NULL);  
          StringBuffer2 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);  
          do {
             CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);
          StrCpy (TempBmcLanConfig->Dhcp_Name,mTempSetupBmcCfg.Dhcp_Name);
          Status = EFI_INVALID_PARAMETER;
        }else {
          StrCpy (mTempSetupBmcCfg.Dhcp_Name,TempBmcLanConfig->Dhcp_Name);
        }
        break;

      case KEY_BMC_ADD_USER_NAME:
      case KEY_BMC_DEL_USER_NAME:
      case KEY_BMC_CHANGE_USER_NAME:
        UserInfoPtr = (BMC_USER_INFO *) &(TempBmcLanConfig->BmcUserInfo[0]);
        DEBUG((EFI_D_ERROR,"UserInfoPtr UserName :%s\n",UserInfoPtr->UserName));
        OldUserInfoPtr  = (BMC_USER_INFO *) &(mOldSetupBmcCfgPtr.BmcUserInfo[0]);
        
        if(!CheckUserNameMatch(UserInfoPtr->UserName)){
           StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_USERNAME), NULL); 
		   StringBuffer11 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_USERNAME1), NULL); 
           StringBuffer2 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);  
           CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1,StringBuffer11, StringBuffer2, NULL);
           Status = EFI_INVALID_PARAMETER;
		   ZeroMem(UserInfoPtr->UserName,sizeof(UserInfoPtr->UserName));
           break;
        };
        TempBmcLanConfig->User = 0;
        for (Index = 1; Index <= MAX_USERS; Index++) {
          ZeroMem(TempUserName, sizeof(TempUserName));
          Status = BmcGetUserName (TempUserName, Index);
          //DEBUG((EFI_D_ERROR,"Current User ID: %d\n",Index ));        
          //DEBUG((EFI_D_ERROR,"User ID: %d Username :%s\n",Index,TempUserName));
          if(EFI_ERROR (Status)){
             if((TempBmcLanConfig->User == 0) && (QuestionId == KEY_BMC_ADD_USER_NAME)){
               TempBmcLanConfig->User = Index;
               mUserRecord.UserId = Index;
               mUserRecord.Operation = BMC_USER_ADD;
             }
             continue;
          }
          //
          // If user name already Exist.
          //          
          if(0 == StrCmp (TempUserName, UserInfoPtr->UserName)){
            if(QuestionId == KEY_BMC_ADD_USER_NAME){
              StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_NAME_EXIST), NULL);  
              UiConfirmDialog(DIALOG_WAIT_ENTER_KEY, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
              StrCpy (UserInfoPtr->UserName, OldUserInfoPtr->UserName); //resotore current user name
              TempBmcLanConfig->User = Index;
              Status = EFI_INVALID_PARAMETER;        
              break;
            }else{              
              TempBmcLanConfig->User = Index;
              if(mUserRecord.UserId != 0  && mUserRecord.UserId!=Index){
                  TempBmcLanConfig->UserPwdChanged = 0;
                  UserInfoPtr->UserPresent         = 0;
              }else{
                  mUserRecord.UserId = Index;
              }
              break; //found user ,break
            }
          }
        }
        if(!TempBmcLanConfig->User && (QuestionId == KEY_BMC_ADD_USER_NAME)){
          StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_NEED_SET_PASSWORD), NULL);
          UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
        }else if(!TempBmcLanConfig->User){
          StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_INVALID_USER), NULL);
          UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
          StrCpy (UserInfoPtr->UserName, OldUserInfoPtr->UserName); //resotore current user name
        }
        break;   

      case KEY_BMC_ADD_USER_PRIVILEGE: 
      case KEY_BMC_ADD_USER_ENABALE_CHANNEL_USER:
      case KEY_BMC_CHANGE_USER_PRIVILEGE:
      case KEY_BMC_CHANGE_USER_ENABALE_CHANNEL_USER:
        UserInfoPtr = (BMC_USER_INFO *) &(TempBmcLanConfig->BmcUserInfo[0]);
        //
        //Changing 
        //
        Status = BmcSetUserAccess(UserInfoPtr,TempBmcLanConfig->User);
        if(EFI_ERROR (Status)){
            StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_SET_FAILED), NULL);
            UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
            Status = EFI_NOT_AVAILABLE_YET;  
        }
        break;
      //default:
      //  CopyMem(TempBmcLanConfig, &mTempSetupBmcCfg, sizeof (BMC_LAN_CONFIG));
      //  break;
    }
    //
    // Update uncommitted data of Browser
    //

   
    HiiSetBrowserData (
            &SetupBmcCfgGuid,
            L"BmcLanConfig",
            sizeof (BMC_LAN_CONFIG),
            (UINT8 *) TempBmcLanConfig,
            NULL
            );  
            
    goto Exit;
  }
  
  /*if( Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD){
     BmcSetIpModeDefault(TempBmcLanConfig);
     HiiSetBrowserData (
              &SetupBmcCfgGuid,
              L"BmcLanConfig",
              sizeof (BMC_LAN_CONFIG),
              (UINT8 *) TempBmcLanConfig,
              NULL
              );  
              
     FreePool(TempBmcLanConfig); 
     return EFI_SUCCESS;
  } */

  if (Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
    //
    // restore bmc lan configuration
    //
    HiiSetBrowserData (
            &SetupBmcCfgGuid,
            L"BmcLanConfig",
            sizeof (BMC_LAN_CONFIG),
            (UINT8 *) &mOldSetupBmcCfgPtr,
            NULL
            );  
  
    FreePool(TempBmcLanConfig);
    return  Status;
  }
  
  //password callback
  if (Action == EFI_BROWSER_ACTION_CHANGING /* && Value->string != 0 */) {    

      //DEBUG((EFI_D_INFO, "Value->string =%x\n" ,Value->string));
      //
      // Get Password form hii databas handle.
      //
      if (Type == EFI_IFR_TYPE_STRING && Value->string != 0) {
        Password = HiiGetString(HiiHandle, Value->string, NULL);  
        if (NULL == Password && PasswordState == BROWSER_STATE_SET_PASSWORD) {
          PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
          goto Exit;
        }
      }
      //DEBUG((EFI_D_INFO, "%a() L%d Password:%s\n", __FUNCTION__, __LINE__, Password));

      
      if(Value->string == 0 && PasswordState == BROWSER_STATE_SET_PASSWORD){
        PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
        Status = EFI_NOT_AVAILABLE_YET;
        goto Exit;
      }

      if(UserState != TempBmcLanConfig->User && UserState!=0xFF){
         PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
      }

      PcdSet8(PcdConfirmDeletePwd,0);

      UserInfoPtr = (BMC_USER_INFO *) &(TempBmcLanConfig->BmcUserInfo[0]);
      OldUserInfoPtr  = (BMC_USER_INFO *) &(mOldSetupBmcCfgPtr.BmcUserInfo[0]);  

      switch (QuestionId) {
        case KEY_BMC_CHANGE_USER_PASSWORD:
          //password cant be NULL
          if (NULL ==Password || !StrLen(Password)) {
            Status = EFI_SUCCESS;
            break;
          }
		  if(!StrCmp(Password,CurrentUserLastPwd)){
            StringBuffer5 = HiiGetString(HiiHandle, STRING_TOKEN(STR_PASSWORD_FAILED), NULL);
            UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer5, NULL); 
            break;
		  }
          Status = BmcSetBmcPassword(PASSWORD_20_BYTES, Password, TempBmcLanConfig->User);
          if(!EFI_ERROR(Status)){
            StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_SET_SUCCESS), NULL);
            UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
			StrCpy(CurrentUserLastPwd,Password);   
          }else{
            StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_SET_FAILED), NULL);
            UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);  
          }
          break;
        case KEY_BMC_DEL_USER_PASSWORD:
        case KEY_BMC_VERIFY_USER_PASSWORD:
          //DEBUG((EFI_D_INFO, "UserID = %x\n", TempBmcLanConfig->User));
          //DEBUG((EFI_D_INFO, "Password = %s\n", Password));
          if(TempBmcLanConfig->User == 0){
            StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_SET_USER_FIRST), NULL);
            UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
            Status = EFI_NOT_AVAILABLE_YET;
            break;
          }         
          //password cant be NULL
          if (NULL ==Password || !StrLen(Password)) {
            Status = EFI_NOT_READY;
            break;
          }
          //verify passsword correct
          if (TestUserPassword(TempBmcLanConfig->User,Password)) {
            //UserInfoPtr->UserPresent = 1;
            StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_WRONG_PASSWORD), NULL);
            UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
          }else{
            if(QuestionId == KEY_BMC_DEL_USER_PASSWORD){
              //
              //delete user
              //
              Status = BmcDelUser(TempBmcLanConfig->User);
              if(!EFI_ERROR(Status)){
                StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_DELETE_USER_SUCCESS), NULL);
                UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);   
                //CopyMem(UserInfoPtr, OldUserInfoPtr, sizeof(OldUserInfoPtr));
                BmcSetUserDefault(TempBmcLanConfig);
              }else{
                StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_DELETE_USER_FAIL), NULL);
                UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);  
              }
            }else{
			  StrCpy(CurrentUserLastPwd,Password);
              mUserRecord.UserId               = TempBmcLanConfig->User;
              UserInfoPtr->UserPresent         = 1;
              TempBmcLanConfig->UserPwdChanged = 1;
              BmcGetUserPrivilege(UserInfoPtr,TempBmcLanConfig->User);
            }
          }
          Status = EFI_NOT_AVAILABLE_YET;
          break;
          
        case KEY_BMC_ADD_USER_PASSWORD:
          UserState = TempBmcLanConfig->User;
          //DEBUG((EFI_D_INFO, "UserID = %x\n", TempBmcLanConfig->User));
          //DEBUG((EFI_D_INFO, "PasswordState = %x\n", PasswordState));

          switch (PasswordState) {
            case BROWSER_STATE_VALIDATE_PASSWORD:
              if(TempBmcLanConfig->User == 0){
                  StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_SET_USER_FIRST), NULL);
                  UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
                  Status = EFI_NOT_AVAILABLE_YET;
                  break;
              }
              //check password present,Standard Ipmi spec don't support the cmd to check user password present! so we just check the user name.
              ZeroMem(TempUserName, sizeof(TempUserName));
              Status = BmcGetUserName (TempUserName, TempBmcLanConfig->User);
              if (EFI_ERROR (Status)) {
                PasswordState = BROWSER_STATE_SET_PASSWORD;
                Status = EFI_SUCCESS;
                break;
              }
              //password cant be NULL
              if (NULL ==Password) {
                PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
                Status = EFI_NOT_READY;
                break;
              }
              //verify passsword correct
              if (!TestUserPassword(TempBmcLanConfig->User,Password)) {
                PasswordState = BROWSER_STATE_SET_PASSWORD;
                Status = EFI_SUCCESS;
              } else {
                Status = EFI_NOT_READY;
              }
              break;
            case BROWSER_STATE_SET_PASSWORD:
              if (*Password != CHAR_NULL) {
                //DEBUG((EFI_D_INFO, "UserId:%x\n", TempBmcLanConfig->User));
                //DEBUG((EFI_D_INFO, "UserName:%s\n",UserInfoPtr->UserName));
                if(TempBmcLanConfig->User !=0 ){
                  BmcSetUserName(UserInfoPtr->UserName, TempBmcLanConfig->User);
                }
                //DEBUG((EFI_D_INFO, "Password:%s\n", Password));
                Status = BmcSetBmcPassword(PASSWORD_20_BYTES, Password, TempBmcLanConfig->User);
                //DEBUG((EFI_D_INFO, "BmcSetBmcPassword :%r  --Complete Code = %x\n", Status,EfiIGetLastIpmiCmdCompleteCode()));
                          
                if(EFI_ERROR (Status) || EfiIGetLastIpmiCmdCompleteCode() != 0){
                       if(TempBmcLanConfig->User !=0){
                         BmcDelUser(TempBmcLanConfig->User);
                       }
                       StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_PD), NULL);
                       UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
                       PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
                       Status = EFI_NOT_AVAILABLE_YET;
                       break;
                }
                if(TempBmcLanConfig->User != 0 ){
                  UserInfoPtr->UserPresent = 1;  //patch load default
                }
                StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_SET_PASSWORD), NULL);
                UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
              } else {     
                  StringBuffer1 = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_PD), NULL);
                  UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
                  PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
                  Status = EFI_NOT_AVAILABLE_YET;
              }
              PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
              break;

            default:
              PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
              break;
          }
          break;
      }
      // set UserPwdChanged flag for save user password in SetupBmcCfgRouteConfig
      HiiSetBrowserData (
        &SetupBmcCfgGuid,
        L"BmcLanConfig",
        sizeof (BMC_LAN_CONFIG),
        (UINT8 *) TempBmcLanConfig,
        NULL
        );
  }

Exit:
  if (TempBmcLanConfig != NULL) {
      FreePool(TempBmcLanConfig);
      TempBmcLanConfig =NULL;
  }
  if (StringBuffer1 != NULL) {
      FreePool(StringBuffer1);
      StringBuffer1 = NULL;
  }
  if (StringBuffer2 != NULL) {
      FreePool(StringBuffer2);
      StringBuffer2 = NULL;
  }     
  if (StringBuffer3 != NULL) {
      FreePool(StringBuffer3);
      StringBuffer3 = NULL;      
  }
  if (StringBuffer4 != NULL) {
      FreePool(StringBuffer4);
      StringBuffer4 = NULL;
  }
  //DEBUG((EFI_D_INFO, "%a() L%d PasswordState:%x\n", __FUNCTION__, __LINE__, PasswordState));
  //DEBUG((EFI_D_INFO, "%a()End  L%d Status:%r\n", __FUNCTION__, __LINE__, Status));
  return Status;
}
