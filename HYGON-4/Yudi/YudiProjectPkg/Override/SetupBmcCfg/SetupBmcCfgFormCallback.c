/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SetupBmcCfgFormCallback.c

Revision History:

**/


#include "SetupBmcCfg.h"
#include <Guid/MdeModuleHii.h>
#include <Library/SetupUiLib.h>
#include <Library/ByoCommLib.h>

//
// Global Variables
//
BMC_LAN_CONFIG           mOldSetupBmcCfgPtr;
BMC_LAN_CONFIG           mTempSetupBmcCfg;

UINT8 IsBMCLanConfigChanged = 0;
UINT8 lastuser = 0;



STATIC BMC_USER_RECORD mUserRecord;

typedef struct _USER_INPUT_LINE_CTX {
  UINTN  InputX;
  UINTN  InputY;
  CHAR16 *String;
} USER_INPUT_LINE_CTX;

#define MAX_STRING_SIZE  20
#define NULL_STRING L""

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
EFI_STATUS
GetSetIpPram (
  IN     UINT8          Channel,
  IN     UINT8          LanParameter,
  IN OUT UINT16         *IpAddress,
  IN     BOOLEAN        CommandGet
  )
{
  UINT8       commanddata[20];
  UINT8       commanddatasize;
  UINT8       Response[10];
  UINT8       Responsesize;
  UINT8       Ipadd[4];
  EFI_STATUS  Status;
  UINT8       RetryCount=0;


  if (CommandGet) {
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
      
      if(Channel==SHARED_NIC_CHANNEL_SEL && IsBMCLanConfigChanged==1)// && LanParameter==IP_ADDRESS_PARM)
      {
    //
    //Send command to BMC until EFI_SUCCESS is returned.
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
    //Delay 100ms before issuing command once again.
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
//If :: is in beginning or end of IP address then maximum number of :0 to add is 7, so m=7.
//
    if(((IPaddress[0]==0x3a) && (IPaddress[1]==0x3a)) || ((IPaddress[IPaddressLength]==0x3a) && (IPaddress[IPaddressLength-1]==0x3a)))
        m=6;

//
//If two adjacent colons are in middle of IP address then the minimum number of colons 2 IPaddress is 2 and maximum number of colons can be 7.
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

    if(((IPaddress[0]==0x3a) && (IPaddress[1]==0x3a)) || ((IPaddress[IPaddressLength]==0x3a) && (IPaddress[IPaddressLength-1]==0x3a)))
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

  Checks if abbreviated IP address is entered by user, then updates IP address accordingly.

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
  // Skip default address.
  //

  if(IPaddress[0] == L':' && IPaddress[1] == L':' && IPaddress[2] == L'\0'){  
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

  Laninfo - Pointer to LAN configuration.
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
  
//  UINT8 RetryCount=0;
  ZeroMem (commanddata, 20);
  ZeroMem (Response, 20); 
  //
  // Set IP Source
  //
  commanddata[0] = Channel;               // Channel number
  commanddata[1] = IP_SOURCE_PARM;        // Parameter selector
  commanddata[2] = 1;  // IP Source (STATIC/ DYNAMIC)
  commanddatasize = 3;
  Responsesize    = 10;

//
//If BMC LAN configuration IP source is not changed, then issue command once.
//
   EfiSendCommandToBMC (
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




/*++

Routine Description:

  Send LAN configuration to BMC.

Arguments:

  Laninfo - Pointer to LAN configuration.
  Channel - Lan channel

Returns:

  EFI_SUCCESS

--*/
EFI_STATUS
BmcSetLanConfig (
  IN BMC_LAN_INFO   *Laninfo,
  IN UINT8          Channel
  )
{
  UINT8       commanddata[20];
  UINT8       commanddatasize;
  UINT8       Response[20];
  UINT8       Responsesize;
  EFI_STATUS  Status;
  
  ZeroMem (commanddata, 20);
  ZeroMem (Response, 20); 
  //
  // Set IP Source
  //
  commanddata[0] = Channel;               // Channel number
  commanddata[1] = IP_SOURCE_PARM;        // Parameter selector
  commanddata[2] = Laninfo->NicIpSource;  // IP Source (STATIC/ DYNAMIC)
  commanddatasize = 3;
  Responsesize    = 10;
  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_LAN,           // NetFunction
            EFI_SET_LAN_CONFIG_CMD,     // Command
            (UINT8 *) &commanddata[0],  // *CommandData
            commanddatasize,            // CommandDataSize
            (UINT8 *) &Response,        // *ResponseData
            (UINT8 *) &Responsesize     // *ResponseDataSize
            );
  DEBUG((EFI_D_INFO," Setup Laninfo->NicIpSource = %x - %r\n",Laninfo->NicIpSource,Status));
                
  if (Laninfo->NicIpSource != 2) {
    //
    // IP Source is Static.Set Ip Address,Subnet Mask and Gateway Ip
    //
    GetSetIpPram (Channel, IP_ADDRESS_PARM, &Laninfo->IpAddress[0], FALSE);
    GetSetIpPram (Channel, SUBNET_MASK_PARM, &Laninfo->SubnetMask[0], FALSE);
    GetSetIpPram (Channel, GATEWAY_IP_PARM, &Laninfo->GatewayIp[0], FALSE);
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
  Fill LAN configuration for given Channel.

Arguments:
   Laninfo - Pointer to SetupBmCfg Lan Configuration
   Channel - Lan Channel

Returns:
  Standard variable return values
--*/
{
  UINT8       commanddata[20];
  UINT8       commanddatasize;
  UINT8       Response[20];
  UINT8       Responsesize;
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

  if (Status == EFI_SUCCESS) {
    Laninfo->NicIpSource = Response[1]; // Response1 = IP Source
    GetSetIpPram (Channel, IP_ADDRESS_PARM, &Laninfo->IpAddress[0], TRUE);
    GetSetIpPram (Channel, SUBNET_MASK_PARM, &Laninfo->SubnetMask[0], TRUE);
    GetSetIpPram (Channel, GATEWAY_IP_PARM, &Laninfo->GatewayIp[0], TRUE);
    
  } else {
    Laninfo->NicIpSource = 2;
    StrCpyS(&Laninfo->IpAddress[0] , ARRAY_SIZE(Laninfo->IpAddress), L"0.0.0.0");
    StrCpyS(&Laninfo->SubnetMask[0], ARRAY_SIZE(Laninfo->SubnetMask), L"0.0.0.0");
    StrCpyS(&Laninfo->GatewayIp[0] , ARRAY_SIZE(Laninfo->GatewayIp), L"0.0.0.0");
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
  Fill LAN configuration for given Channel.

Arguments:
   Laninfo - Pointer to SetupBmCfg Lan Configuration
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
  UINT8       Index;
  EFI_STATUS  Status;
  EFI_IPv6_ADDRESS  Ipv6Address;

  ZeroMem (commanddata, 22);
  ZeroMem (Response, 22); 
  
  //
  // Get IP Source (DHCP/ STATIC)
  //
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
   DEBUG ((EFI_D_INFO, "%a GATEWAY_IPV6_CONFIG_PARM Status - %r\n",__FUNCTION__,Status));

  SelIPv6Addr = IPV6_ADDRESS_PARM;
  if (Status == EFI_SUCCESS) {
    if (Response[1] == 2) {
      Laninfo->NicIpv6Source = 2;       // IP source is dynamic
      SelIPv6Addr = IPV6_ADDRESS_DHCP;
    } else if (Response[1] == 1) {
      Laninfo->NicIpv6Source = 1;       // IP source is static
    } else {
      Laninfo->NicIpv6Source = 2;
    }
  } else {
    Laninfo->NicIpv6Source = 2;
  }

  ZeroMem (commanddata, 22);
  ZeroMem (Response, 22); 
  
  //
  // Get IP Source (DHCP/ STATIC)
  //
  for(Index=0; Index<3; Index++){  // BMC recommended times of acquisition
    commanddata[0] = (UINT8) Channel; // Channel number
    commanddata[1] = SelIPv6Addr;     // Parameter selector
    commanddata[2] = Index;           // Set Selector.
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
    DEBUG ((EFI_D_INFO, "%a IPV6_ADDRESS_PARM Status - %r\n",__FUNCTION__,Status));
    if(Response[3] == 0xFE && Response[4] == 0x80) {
      Status = EFI_DEVICE_ERROR;
    } else {
      break;
    }
  }           
  //
  // Get Ipv6 Address,PRefix Length
  // 
  if (Status == EFI_SUCCESS) {  
    //Ipv6AddtoStr (&Laninfo->Ipv6Address[0], &Response[3],sizeof(Laninfo->Ipv6Address));
    CopyMem(&Ipv6Address, &Response[3], sizeof(EFI_IPv6_ADDRESS));
    Status = NetLibIp6ToStr (&Ipv6Address, &Laninfo->Ipv6Address[0], sizeof(Laninfo->Ipv6Address));
    Laninfo->NicIpv6Prefix = Response[19];
    
  } else {
    StrCpyS(Laninfo->Ipv6Address, ARRAY_SIZE(Laninfo->Ipv6Address), L"::");
    Laninfo->NicIpv6Source = 2;
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
   DEBUG ((EFI_D_INFO, "%a GATEWAY_IPV6_PARM Status - %r\n",__FUNCTION__,Status));
             
  if (Status == EFI_SUCCESS) {
    //Ipv6AddtoStr (&Laninfo->GatewayIpv6[0], &Response[1],sizeof(Laninfo->GatewayIpv6));
    CopyMem(&Ipv6Address, &Response[1], sizeof(EFI_IPv6_ADDRESS));
    Status = NetLibIp6ToStr (&Ipv6Address, &Laninfo->GatewayIpv6[0], sizeof(Laninfo->GatewayIpv6));
  } else {
    StrCpyS(Laninfo->GatewayIpv6, ARRAY_SIZE(Laninfo->GatewayIpv6), L"::");
  }
  
  return Status;
}



/*++

Routine Description:

  Send LAN configuration to BMC.

Arguments:

  Laninfo - Pointer to LAN configuration.
  Channel - Lan channel


Returns:

  EFI_SUCCESS

--*/
EFI_STATUS
BmcSetLanIpv6Config (
  IN BMC_LAN_IPV6_INFO   *Laninfo,
  IN UINT8               Channel
  )
{
   UINT8       commanddata[22];
   UINT8       commanddatasize;
   UINT8       Response[22];
   UINT8       Responsesize;
   UINT8       Ipadd[16];   

  ZeroMem (commanddata, 22);
  ZeroMem (Response, 22);
  ZeroMem (Ipadd, 16);   

  if (Laninfo->NicIpv6Source == 1) {   
    //
    // IP Source is Static. Set to Static Ipv6
    //   

    commanddata[0]  = (UINT8)Channel;
    commanddata[1]  = (UINT8)GATEWAY_IPV6_CONFIG_PARM;
    commanddata[2]  = 1;
    commanddatasize = 3;
    Responsesize    = 1;

     EfiSendCommandToBMC (
               EFI_SM_NETFN_LAN,           // NetFunction
               EFI_SET_LAN_CONFIG_CMD,     // Command
               (UINT8 *) &commanddata[0],  // *CommandData
               commanddatasize,            // CommandDataSize
               (UINT8 *) &Response,        // *ResponseData
               (UINT8 *) &Responsesize     // *ResponseDataSize
               );

    //
    // Set IPv6 address
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
    EfiSendCommandToBMC (
               EFI_SM_NETFN_LAN,           // NetFunction
               EFI_SET_LAN_CONFIG_CMD,     // Command
               (UINT8 *) &commanddata[0],  // *CommandData
               commanddatasize,            // CommandDataSize
               (UINT8 *) &Response,        // *ResponseData
               (UINT8 *) &Responsesize     // *ResponseDataSize
               );

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

    EfiSendCommandToBMC (
              EFI_SM_NETFN_LAN,
              EFI_SET_LAN_CONFIG_CMD,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );  
  } else {
    //
    // Set IPv6 Source to Dynamic
    //
    commanddata[0] = Channel;                 // Channel number
    commanddata[1] = IPV6_ADDRESS_DHCP;       // Parameter selector
    commanddata[2] = 0;                       // Set Selector
    commanddata[3] = 0;                       // Address source/type
    commanddatasize = 22;
    Responsesize    = 1;
     EfiSendCommandToBMC (
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
    commanddata[1]  = (UINT8)GATEWAY_IPV6_CONFIG_PARM;
    commanddata[2]  = 2;   // Set to Dynamic Gateway
    commanddatasize = 3;
    Responsesize    = 1;

   EfiSendCommandToBMC (
               EFI_SM_NETFN_LAN,           // NetFunction
               EFI_SET_LAN_CONFIG_CMD,     // Command
               (UINT8 *) &commanddata[0],  // *CommandData
               commanddatasize,            // CommandDataSize
               (UINT8 *) &Response,        // *ResponseData
               (UINT8 *) &Responsesize     // *ResponseDataSize
               );  

    ZeroMem (commanddata, 22);
    ZeroMem (Response, 22);   

    commanddata[0]  = (UINT8)Channel;
    commanddata[1]  = (UINT8)GATEWAY_IPV6_PARM;  
    
    commanddatasize = 18;
    Responsesize    = 1;                  

    EfiSendCommandToBMC (
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
  UINT8       Response[20] = {0};
  UINT8       Responsesize;
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
   DEBUG ((EFI_D_INFO, "%a GET_USER_NAME_CMD Status - %r,response = %d\n",__FUNCTION__,Status,Responsesize));
            
  if (Status == EFI_SUCCESS) {
    AsciiToUnicode ((CHAR8 *)Response, UserName);
    DEBUG((EFI_D_INFO,"Get user name :%s.\n",UserName));
    if(!StrnCmp(UserName,L" ",1)){
      Status = EFI_NOT_FOUND;
    }    
  }else{
    //FakeName = AllocateZeroPool (20);
    //AsciiSPrint(FakeName, 20, "%a",(UINT8*)PcdGetPtr(PcdSkipSetupString));
    //AsciiToUnicode (FakeName, UserNam);
    //FreePool (FakeName);
    StrCpyS(UserName,Responsesize,NULL_STRING);
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
  Userinfo  - Pointer to SetupBmcCfg User Configuration 
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
  UINT8       Index;
  UINT8       BmcChannel[2] = {BASE_BOARD_NIC_CHANNEL_SEL,SHARED_NIC_CHANNEL_SEL};
  UINT8       BmcChannelCount = 1;

  if (PcdGet8(PcdBmcOnboardLanChannelEnable) == FALSE) {
    BmcChannel[0] = SHARED_NIC_CHANNEL_SEL;
  }else{
     if (PcdGet8(PcdBmcShareLanChannelEnable)){
       BmcChannelCount = 2;
     }
  }
  
  ZeroMem (commanddata, 20);
  ZeroMem (Response, 20); 
  //
  // Initialize User Password to NULL String.
  //
  StrCpyS (Userinfo->UserPassword, ARRAY_SIZE(Userinfo->UserPassword), L"\0");
  Status = BmcGetUserName (Userinfo->UserName, UserId);
  if(!EFI_ERROR(Status)){
    Userinfo->UserPresent = 1;
  }
  //
  //  Get User Privilege & User enable.
  //
  for (Index =0 ;Index < BmcChannelCount; Index++) {  
    commanddata[0]  = BmcChannel[Index];
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

     DEBUG ((EFI_D_INFO, "GET_USER_ACCESS_CMD Status - %r\n",Status));
          
    if(Status == EFI_SUCCESS){
      break;
    }

  }

    if (Status == EFI_SUCCESS) {
      switch((UINT8) (Response[3] & 0xF)){
        case 2:
        case 3:
        case 4:
          Userinfo->Privilege = (UINT8) (Response[3] & 0xF);
          break;
        case 0xF:
        default:
          Userinfo->Privilege = 0xF; 
          break;
      }
      // Bits 7&6 = user enable(01)/disable(10).IPMI Spec 2.0 June 12,2009 Markup
      if ((Response[1] & 0xC0) == 0x40) { 
        Userinfo->EnabaleChannelUser = 1;
      }else{
        Userinfo->EnabaleChannelUser = 0;
      }
    }else{
       Userinfo->Privilege          = 0xF;
       Userinfo->EnabaleChannelUser = 0;
    }
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

  Userinfo  - Pointer to User Configuration
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
  UINT8       BmcChannel[2] = {BASE_BOARD_NIC_CHANNEL_SEL,SHARED_NIC_CHANNEL_SEL};
  UINT8       BmcChannelCount = 1;

  if (PcdGet8(PcdBmcOnboardLanChannelEnable) == FALSE) {
    BmcChannel[0] = SHARED_NIC_CHANNEL_SEL;
  }else{
     if (PcdGet8(PcdBmcShareLanChannelEnable)){
       BmcChannelCount = 2;
     }
  }

  //
  // Set User Name
  //
  if ((UserId != 0) && (UserId != 1)) {
    commanddata[0] = UserId;
    ZeroMem (&commanddata[1], 16);
    Unicode2Ascii ((CHAR8 *)&commanddata[1], Userinfo->UserName);
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

  for (Index = 0; Index < BmcChannelCount; Index++) {
    commanddata[0] = 0xB0 | BmcChannel[Index];
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
    Unicode2Ascii ((CHAR8 *)&commanddata[2], Userinfo->UserPassword);
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

EFI_STATUS
BmcSetUserDefault(
   IN BMC_LAN_CONFIG *BmcCfgPtr
  )
{
  BMC_USER_INFO       *UserInfoPtr;

  DEBUG ((EFI_D_INFO, "BmcSetUserDefault()\n"));
  UserInfoPtr = (BMC_USER_INFO *) &(BmcCfgPtr->BmcUserInfo[0]);
  BmcCfgPtr->User           = 0;
  BmcCfgPtr->UserPwdChanged = 0;
  StrCpyS (UserInfoPtr->UserPassword,ARRAY_SIZE(UserInfoPtr->UserPassword), L"\0");
  StrCpyS(UserInfoPtr->UserName,ARRAY_SIZE(UserInfoPtr->UserName), NULL_STRING);
  UserInfoPtr->EnabaleChannelUser = 0;
  UserInfoPtr->Privilege          = 0xF;
  UserInfoPtr->UserPresent        = 0;
  ZeroMem(&mUserRecord, sizeof(mUserRecord));
  return EFI_SUCCESS;
}


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
EFI_STATUS
BmcCfgSetVariable (
  IN CHAR16        *VariableName,
  IN EFI_GUID      *VendorGuid,
  IN UINT32        Attributes,
  IN UINTN         DataSize,
  IN VOID          *Data
  )

{
  UINT8            Ipv6Status=0;
  BMC_LAN_CONFIG   *BmcCfgPtr;
  EFI_GUID         SetupBmcCfgGuid = SYSTEM_CONFIGURATION_GUID;
  

  if ((StrCmp (VariableName, L"BmcLanConfig" ) != 0) || (CompareGuid(VendorGuid,&SetupBmcCfgGuid)== FALSE)) {
    return EFI_NOT_FOUND;
  }
  BmcCfgPtr = (BMC_LAN_CONFIG*)Data;
  DEBUG ((EFI_D_INFO, "SetVar(%s)\n",VariableName));

  if (PcdGet8(PcdBmcOnboardLanChannelEnable)) {
    if(CompareMem(&BmcCfgPtr->OnBoardNIC_IpMode, &mOldSetupBmcCfgPtr.OnBoardNIC_IpMode, sizeof(BMC_LAN_INFO))) {
      DEBUG((EFI_D_INFO, "OnBoardNIC IpMode changed\n"));
      BmcSetLanConfig ((BMC_LAN_INFO*)&BmcCfgPtr->OnBoardNIC_IpMode, BASE_BOARD_NIC_CHANNEL_SEL);
      IsBMCLanConfigChanged = 1;
    }
  }

  if (PcdGet8(PcdBmcShareLanChannelEnable)) {
    if(CompareMem(&BmcCfgPtr->ShareNIC_IpMode, &mOldSetupBmcCfgPtr.ShareNIC_IpMode, sizeof(BMC_LAN_INFO))) {
      DEBUG((EFI_D_INFO, "ShareNIC changed\n"));
      BmcSetLanConfig ((BMC_LAN_INFO*)&BmcCfgPtr->ShareNIC_IpMode, SHARED_NIC_CHANNEL_SEL);
      IsBMCLanConfigChanged = 1;
    }
  }

// Moved the code out of above if condition since the if condition satisfies only when IPV4 values 
// are changed. When only IPV6 values are changed, control does not get into if loop and IPV6 values 
// are not updated into BMC.
//
//
//Check if IPV6 is Enable or Disable

  if (PcdGet8(PcdBmcOnboardLanChannelEnable) && 
      ((BmcCfgPtr->OnBoardNIC_IpSelection != mOldSetupBmcCfgPtr.OnBoardNIC_IpSelection) ||
      CompareMem(&BmcCfgPtr->OnBoardNIC_Ipv6Mode, &mOldSetupBmcCfgPtr.OnBoardNIC_Ipv6Mode, sizeof (BMC_LAN_IPV6_INFO)))) {
    if(BmcCfgPtr->OnBoardNIC_IpSelection == 1){    
      Ipv6Status = 2;     
      GetSetIpv6Status (BASE_BOARD_NIC_CHANNEL_SEL, IPV6_STATUS_PARM, &Ipv6Status, FALSE);   
      BmcSetLanIpv6Config((BMC_LAN_IPV6_INFO*)&BmcCfgPtr->OnBoardNIC_Ipv6Mode, BASE_BOARD_NIC_CHANNEL_SEL);
    }else if(BmcCfgPtr->OnBoardNIC_IpSelection == 0){
      Ipv6Status = 0;      
      GetSetIpv6Status(BASE_BOARD_NIC_CHANNEL_SEL, IPV6_STATUS_PARM, &Ipv6Status, FALSE);
    }  
  }

  if (PcdGet8(PcdBmcShareLanChannelEnable) && 
      ((BmcCfgPtr->ShareNIC_IpSelection != mOldSetupBmcCfgPtr.ShareNIC_IpSelection) ||
      CompareMem(&BmcCfgPtr->ShareNIC_Ipv6Mode, &mOldSetupBmcCfgPtr.ShareNIC_Ipv6Mode, sizeof (BMC_LAN_IPV6_INFO)))) {
    if(BmcCfgPtr->ShareNIC_IpSelection == 1){   
      Ipv6Status = 2;     
      GetSetIpv6Status (SHARED_NIC_CHANNEL_SEL, IPV6_STATUS_PARM, &Ipv6Status, FALSE);    
      BmcSetLanIpv6Config((BMC_LAN_IPV6_INFO*)&BmcCfgPtr->ShareNIC_Ipv6Mode, SHARED_NIC_CHANNEL_SEL);
    }else if(BmcCfgPtr->ShareNIC_IpSelection == 0){
      Ipv6Status = 0;      
      GetSetIpv6Status(SHARED_NIC_CHANNEL_SEL, IPV6_STATUS_PARM, &Ipv6Status, FALSE);
    }
  }

  return EFI_SUCCESS;
}








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
EFI_STATUS
BmcCfgGetVariable (
  IN CHAR16        *VariableName,
  IN EFI_GUID      *VendorGuid,
  OUT UINT32       *Attributes OPTIONAL,
  IN OUT UINTN     *DataSize,
  OUT VOID         *Data
  )
{
  UINT8               temp=0;
  UINT8               *Ipv6Status = &temp;
  BMC_LAN_CONFIG      *BmcCfgPtr;
  EFI_GUID            SetupBmcCfgGuid=SYSTEM_CONFIGURATION_GUID;
  
 
  if (( StrCmp (VariableName, L"BmcLanConfig" ) != 0) || (CompareGuid(VendorGuid,&SetupBmcCfgGuid)== FALSE)) {
    return EFI_NOT_FOUND;
  }

  //
  // Check return Buffer size
  //
  if (*DataSize < sizeof (BMC_LAN_CONFIG)) {
    *DataSize = sizeof (BMC_LAN_CONFIG);
    return EFI_BUFFER_TOO_SMALL;
  }
  *DataSize = sizeof (BMC_LAN_CONFIG);
  BmcCfgPtr = (BMC_LAN_CONFIG*)Data;
  ZeroMem (BmcCfgPtr, *DataSize);

  DEBUG ((EFI_D_INFO, "Var(%s) L:%d\n",VariableName,*DataSize));

  if (PcdGet8(PcdBmcOnboardLanChannelEnable)){
    GetSetIpv6Status(BASE_BOARD_NIC_CHANNEL_SEL, IPV6_STATUS_PARM, Ipv6Status, TRUE);
    BmcGetLanConfig ((BMC_LAN_INFO *) &BmcCfgPtr->OnBoardNIC_IpMode, BASE_BOARD_NIC_CHANNEL_SEL);
    if(*Ipv6Status == 2){
      BmcCfgPtr->OnBoardNIC_IpSelection = 1;
    }else{ 
      BmcCfgPtr->OnBoardNIC_IpSelection = 0;
    }
    BmcGetLanIPv6Config ((BMC_LAN_IPV6_INFO *) &BmcCfgPtr->OnBoardNIC_Ipv6Mode, BASE_BOARD_NIC_CHANNEL_SEL);
  }

  if (PcdGet8(PcdBmcShareLanChannelEnable)){
    GetSetIpv6Status(SHARED_NIC_CHANNEL_SEL, IPV6_STATUS_PARM, Ipv6Status, TRUE);
    BmcGetLanConfig ((BMC_LAN_INFO *) &BmcCfgPtr->ShareNIC_IpMode, SHARED_NIC_CHANNEL_SEL);
    if(*Ipv6Status == 2){
      BmcCfgPtr->ShareNIC_IpSelection = 1;
    } else {
      BmcCfgPtr->ShareNIC_IpSelection = 0;
    }
    BmcGetLanIPv6Config ((BMC_LAN_IPV6_INFO *) &BmcCfgPtr->ShareNIC_Ipv6Mode, SHARED_NIC_CHANNEL_SEL);
  }

  BmcSetUserDefault(BmcCfgPtr);  



// Save a copy of BMC_LAN_CONFIG
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
      switch((UINT8) (Response[3] & 0xF)){
        case 1:
        case 2:
        case 3:
        case 4:
          Userinfo->Privilege = (UINT8) (Response[3] & 0xF);
          break;
        case 0xF:
        default:
          Userinfo->Privilege = 0xF; 
          break;
      }
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
       Userinfo->Privilege          = 0xF;
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
    Unicode2Ascii ((CHAR8 *)&commanddata[1], UserName);
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
  IN CHAR16       *Password,
  IN UINT8         UserId
  )
{
  UINT8       commanddata[24];  //Predefine User Name/Privilege/Password command data buffer with maximal buffer length and necessary redundancy.
  UINT8       commanddatasize;  //Send to BMC data size.
  UINT8       Response[24];
  UINT8       Responsesize;
  UINTN       PasswordLen;
  EFI_STATUS  Status = EFI_UNSUPPORTED;

  PasswordLen = StrLen(Password);

  //
  // Set User Password
  //
  if (PasswordLen != 0) {
      if (PasswordLen > 16) {
      commanddata[0] = UserId | BIT7;      // userid: Enable BIT7 to support 20 bytes user passwords.
      commanddatasize = 22;                // commanddatasize: Byte[1]:UserId,Byte[2]:operation,Byte[3-22]:Password data.Please refer IPMI2.0 Spec.
    } else {
      commanddata[0] = UserId;
      commanddatasize = 18;
    }
    commanddata[1] = 02;                 // operation: Set Password
    ZeroMem (&commanddata[2], 20);       // Zero Password data
    Unicode2Ascii ((CHAR8 *)&commanddata[2], Password);
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
  UINT8       BmcChannel[2] = {BASE_BOARD_NIC_CHANNEL_SEL,SHARED_NIC_CHANNEL_SEL};
  UINT8       BmcChannelCount = 1;

  if (PcdGet8(PcdBmcOnboardLanChannelEnable) == FALSE) {
    BmcChannel[0] = SHARED_NIC_CHANNEL_SEL;
  }else{
     if (PcdGet8(PcdBmcShareLanChannelEnable)){
       BmcChannelCount = 2;
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

  for (Index = 0; Index < BmcChannelCount; Index++) {
    commanddata[0] = 0xB0 | BmcChannel[Index];
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
BmcEnableUserSolPermissions(
  IN UINT8          UserId
  )
{
  UINT8       commanddata[6];
  UINT8       commanddatasize;
  UINT8       Response[6];
  UINT8       Responsesize;
  EFI_STATUS  Status;
  UINT8       Index;
  UINT8       BmcChannel[2] = {BASE_BOARD_NIC_CHANNEL_SEL,SHARED_NIC_CHANNEL_SEL};
  UINT8       BmcChannelCount = 1;

  if (PcdGet8(PcdBmcOnboardLanChannelEnable) == FALSE) {
    BmcChannel[0] = SHARED_NIC_CHANNEL_SEL;
  }else{
     if (PcdGet8(PcdBmcShareLanChannelEnable)){
       BmcChannelCount = 2;
     }
  }

  for (Index = 0; Index < BmcChannelCount; Index++) {
    commanddata[0] = BmcChannel[Index];
    commanddata[1] = UserId;
    commanddata[2] = 2;

    ZeroMem (&commanddata[3], 3);
    commanddatasize = 6;
    Responsesize    = 6;

    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_APP,
              SET_USER_SOL_PERMISSION,
              (UINT8 *) &commanddata[0],
              commanddatasize,
              (UINT8 *) &Response,
              (UINT8 *) &Responsesize
              );
  }
  return Status;
}


typedef struct {
  UINT8 ManufactureID[3];
  UINT8 Selector;
  UINT8 UserName[16];
  UINT8 Disable;
  UINT8 PwdValidDays[3];
} BMC_USER_PWD_VALID_DAYS;

EFI_STATUS
EFIAPI
BmcSetUserPwdValidDays (
  IN CHAR16       *UserName
  )
{
  BMC_USER_PWD_VALID_DAYS  CommandData;
  UINT8                    CommandDataSize;
  UINT8                    Response[24];
  UINT8                    ResponseSize;
  EFI_STATUS               Status;

  DEBUG ((DEBUG_INFO, "BmcSetUserPwdValidDays()\n"));

  //
  // Byosoft BMC manufacture ID
  //
  CommandData.ManufactureID[0] = 0xcc;
  CommandData.ManufactureID[1] = 0xcb;
  CommandData.ManufactureID[2] = 0x00;
  //
  // Set password valid days
  //
  CommandData.Selector = 0x01;
  //
  // BMC user name
  //
  ZeroMem (&CommandData.UserName[0], 16);
  Unicode2Ascii ((CHAR8 *)&CommandData.UserName[0], UserName);
  //
  // If enable password valid days, 1->disable, 0->enable
  //
  CommandData.Disable = 0;
  //
  // Default valid days(90) in minutes,
  // 90 * 24 * 60 = 129600 = 0x1fa40
  //
  CommandData.PwdValidDays[0] = 0x40;
  CommandData.PwdValidDays[1] = 0xfa;
  CommandData.PwdValidDays[2] = 0x01;

  CommandDataSize = sizeof(BMC_USER_PWD_VALID_DAYS);
  ResponseSize    = sizeof(BMC_USER_PWD_VALID_DAYS);

  Status = EfiSendCommandToBMC (
             0x2e,
             0x2d,
             (UINT8 *) &CommandData,
             CommandDataSize,
             (UINT8 *) &Response,
             (UINT8 *) &ResponseSize
             );
  DEBUG ((DEBUG_INFO, "Set pwd valid day status is %r\n", Status));
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
  UINT8       BmcChannel[2] = {BASE_BOARD_NIC_CHANNEL_SEL,SHARED_NIC_CHANNEL_SEL};
  UINT8       BmcChannelCount = 1;

  if (PcdGet8(PcdBmcOnboardLanChannelEnable) == FALSE) {
    BmcChannel[0] = SHARED_NIC_CHANNEL_SEL;
  }else{
     if (PcdGet8(PcdBmcShareLanChannelEnable)){
       BmcChannelCount = 2;
     }
  }

  Status = EFI_UNSUPPORTED;
  //
  // Set User Name
  //
  if ((UserId != 0) && (UserId != 1)) {
    commanddata[0] = UserId;
    SetMem(&commanddata[1], 16, 0x20); //Set the user name to 16 Spaces
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
  for (Index = 0; Index < BmcChannelCount; Index++) {
    commanddata[0] = 0x80 | BmcChannel[Index];
    commanddata[1] = UserId;                // userid
    commanddata[2] = 0x0f;                  // Privilege
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
  UINTN       PasswordLen;
  //
  // Set User Password
  //
  PasswordLen = StrLen(Password);
  DEBUG ((EFI_D_INFO, "StrLen (Password) = %x\n",PasswordLen));
  if(PasswordLen == 0){
    return 0x81;
  } else if (PasswordLen > 16) {
    commanddata[0] = UserId | BIT7;      // userid: Enable BIT7 to support 20 bytes user passwords.
  } else {
    commanddata[0] = UserId;
  }

  commanddata[1] = 03;                 // operation: Test Password
  ZeroMem (&commanddata[2], 20);       // Zero Password data
  Unicode2Ascii ((CHAR8 *)&commanddata[2], Password);
  commanddatasize = 22;                // commanddatasize: Byte[1]:UserId,Byte[2]:operation,Byte[3-22]:Password data.Please refer IPMI2.0 Spec.
  Responsesize    = 24;

   EfiSendCommandToBMC (
            EFI_SM_NETFN_APP,
            SET_USER_PASSWORD_CMD,
            (UINT8 *) &commanddata[0],
            commanddatasize,
            (UINT8 *) &Response,
            (UINT8 *) &Responsesize
            );  
  if(EfiIGetLastIpmiCmdCompleteCode()==0x81){  //only support 16 bytes user passwords.
    commanddatasize = 18;
    EfiSendCommandToBMC (
            EFI_SM_NETFN_APP,
            SET_USER_PASSWORD_CMD,
            (UINT8 *) &commanddata[0],
            commanddatasize,
            (UINT8 *) &Response,
            (UINT8 *) &Responsesize
            );
  }
  DEBUG ((EFI_D_INFO, "EfiIGetLastIpmiCmdCompleteCode  %X :\n",EfiIGetLastIpmiCmdCompleteCode()));
  return EfiIGetLastIpmiCmdCompleteCode();
}


BOOLEAN 
CheckUserNameMatch(CHAR16 *UserName)
{
  UINTN   i;
  UINTN   Length;
  Length = StrLen(UserName);

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
  
  //EnterNewPassword = HiiGetString(mHiiHandle, STRING_TOKEN(STR_DELETE_USER_ACCOUNT), NULL);

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
                                 vary based on the opcode that generated the callback.
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

  DEBUG ((EFI_D_INFO, "[ValidateSetupInput] QuestionID:%d, Action:%d\n",QuestionId,Action));

  //if (Type != EFI_IFR_TYPE_STRING) {
  //  return EFI_SUCCESS;
  //}

  if (Action == EFI_BROWSER_ACTION_FORM_OPEN ||
     Action == EFI_BROWSER_ACTION_RETRIEVE ) {
    return EFI_SUCCESS;
  }

  //
  // Retrieve uncommitted data from Browser
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
          if (Status == EFI_SUCCESS) {
            Status = Ipv4AddrCheck (&Ip_Add,TYPE_IP4_ADDR);
          }
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpyS (TempBmcLanConfig->OnBoardNIC_IpAddress, ARRAY_SIZE(TempBmcLanConfig->OnBoardNIC_IpAddress), mTempSetupBmcCfg.OnBoardNIC_IpAddress);
          StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_INVALID_IP), NULL);  
          StringBuffer2 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_GATEWAY), NULL);  
          StringBuffer3 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_IPV4_FORMAT), NULL);  
          StringBuffer4 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);          
          do {
            CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3,StringBuffer4, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN && Key.ScanCode != SCAN_ESC);
        } else {
          StrCpyS (mTempSetupBmcCfg.OnBoardNIC_IpAddress, ARRAY_SIZE(mTempSetupBmcCfg.OnBoardNIC_IpAddress),TempBmcLanConfig->OnBoardNIC_IpAddress);    
          //Status = BmcSetMyLanConfig (BASE_BOARD_NIC_CHANNEL_SEL);
          //GetSetIpPram (BASE_BOARD_NIC_CHANNEL_SEL, IP_ADDRESS_PARM, TempBmcLanConfig->OnBoardNIC_IpAddress, FALSE);
        }
        break;

      case KEY_BMC_ONBOARD_NIC_IPV4_SUBNET_MASK_ADDR:
          Status = StrStringToIp4Adr (&Ip_Add, TempBmcLanConfig->OnBoardNIC_SubnetMask);
          if (Status == EFI_SUCCESS) {
            Status = Ipv4AddrCheck (&Ip_Add,TYPE_IP4_MASK);
          }
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpyS (TempBmcLanConfig->OnBoardNIC_SubnetMask, ARRAY_SIZE(TempBmcLanConfig->OnBoardNIC_SubnetMask), mTempSetupBmcCfg.OnBoardNIC_SubnetMask);
          StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_INVALID_SUBNET), NULL);  
          StringBuffer2 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_GATEWAY), NULL);  
          StringBuffer3 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_IPV4_FORMAT), NULL);  
          StringBuffer4 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);        
          do {
            CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3,StringBuffer4, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN && Key.ScanCode != SCAN_ESC);
        } else {
          StrCpyS (mTempSetupBmcCfg.OnBoardNIC_SubnetMask, ARRAY_SIZE(mTempSetupBmcCfg.OnBoardNIC_SubnetMask), TempBmcLanConfig->OnBoardNIC_SubnetMask);      
          //Status = BmcSetMyLanConfig (BASE_BOARD_NIC_CHANNEL_SEL);
          //GetSetIpPram (BASE_BOARD_NIC_CHANNEL_SEL, SUBNET_MASK_PARM, TempBmcLanConfig->OnBoardNIC_SubnetMask, FALSE);  
        }
        break;

      case KEY_BMC_ONBOARD_NIC_IPV4_GATEWAY_ADDR:
          Status = StrStringToIp4Adr (&Ip_Add, TempBmcLanConfig->OnBoardNIC_GatewayIp);
          if (Status == EFI_SUCCESS) {
            Status = Ipv4AddrCheck (&Ip_Add,TYPE_IP4_GATEWAY);
          }
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpyS (TempBmcLanConfig->OnBoardNIC_GatewayIp, ARRAY_SIZE(TempBmcLanConfig->OnBoardNIC_GatewayIp), mTempSetupBmcCfg.OnBoardNIC_GatewayIp);
           StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_INVALID_GATEWAY_IP), NULL);  
           StringBuffer2 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_GATEWAY), NULL);  
           StringBuffer3 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_IPV4_FORMAT), NULL);  
           StringBuffer4 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);
           do {
             CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3, StringBuffer4 ,NULL);
           } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN && Key.ScanCode != SCAN_ESC);
        } else {
          StrCpyS (mTempSetupBmcCfg.OnBoardNIC_GatewayIp, ARRAY_SIZE(mTempSetupBmcCfg.OnBoardNIC_GatewayIp), TempBmcLanConfig->OnBoardNIC_GatewayIp);      
          //Status = BmcSetMyLanConfig (BASE_BOARD_NIC_CHANNEL_SEL);  
          //GetSetIpPram (BASE_BOARD_NIC_CHANNEL_SEL, GATEWAY_IP_PARM, TempBmcLanConfig->OnBoardNIC_GatewayIp, FALSE);   
        }
        break;
      case KEY_BMC_SHARED_NIC_IPV4_ADDR:
          Status = StrStringToIp4Adr (&Ip_Add, TempBmcLanConfig->ShareNIC_IpAddress);
          if (Status == EFI_SUCCESS) {
            Status = Ipv4AddrCheck (&Ip_Add,TYPE_IP4_ADDR);
          }
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpyS (TempBmcLanConfig->ShareNIC_IpAddress, ARRAY_SIZE(TempBmcLanConfig->ShareNIC_IpAddress), mTempSetupBmcCfg.ShareNIC_IpAddress);
          StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_INVALID_IP), NULL);  
          StringBuffer2 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_GATEWAY), NULL);  
          StringBuffer3 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_IPV4_FORMAT), NULL);  
          StringBuffer4 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);
          do {
            CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3, StringBuffer4 ,NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN && Key.ScanCode != SCAN_ESC);
        } else {
          StrCpyS (mTempSetupBmcCfg.ShareNIC_IpAddress, ARRAY_SIZE(mTempSetupBmcCfg.ShareNIC_IpAddress), TempBmcLanConfig->ShareNIC_IpAddress);    
          //Status = BmcSetMyLanConfig (SHARED_NIC_CHANNEL_SEL);
          //GetSetIpPram (SHARED_NIC_CHANNEL_SEL, IP_ADDRESS_PARM, TempBmcLanConfig->ShareNIC_IpAddress, FALSE); 
        }
        break;

      case KEY_BMC_SHARED_NIC_IPV4_SUBNET_MASK_ADDR:
          Status = StrStringToIp4Adr (&Ip_Add, TempBmcLanConfig->ShareNIC_SubnetMask);
          if (Status == EFI_SUCCESS) {
            Status = Ipv4AddrCheck (&Ip_Add,TYPE_IP4_MASK);
          }
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpyS (TempBmcLanConfig->ShareNIC_SubnetMask, ARRAY_SIZE(TempBmcLanConfig->ShareNIC_SubnetMask), mTempSetupBmcCfg.ShareNIC_SubnetMask);
          StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_INVALID_SUBNET), NULL);  
          StringBuffer2 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_GATEWAY), NULL);  
          StringBuffer3 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_IPV4_FORMAT), NULL);  
          StringBuffer4 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);
          do {
            CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3,StringBuffer4, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN && Key.ScanCode != SCAN_ESC);
        } else {
          StrCpyS (mTempSetupBmcCfg.ShareNIC_SubnetMask, ARRAY_SIZE(mTempSetupBmcCfg.ShareNIC_SubnetMask), TempBmcLanConfig->ShareNIC_SubnetMask);      
          //Status = BmcSetMyLanConfig (SHARED_NIC_CHANNEL_SEL);
          //GetSetIpPram (SHARED_NIC_CHANNEL_SEL, SUBNET_MASK_PARM, TempBmcLanConfig->ShareNIC_SubnetMask, FALSE);  
        }
        break;

      case KEY_BMC_SHARED_NIC_IPV4_GATEWAY_ADDR:
          Status = StrStringToIp4Adr (&Ip_Add, TempBmcLanConfig->ShareNIC_GatewayIp);
          if (Status == EFI_SUCCESS) {
            Status = Ipv4AddrCheck (&Ip_Add,TYPE_IP4_GATEWAY);
          }
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpyS (TempBmcLanConfig->ShareNIC_GatewayIp, ARRAY_SIZE(TempBmcLanConfig->ShareNIC_GatewayIp), mTempSetupBmcCfg.ShareNIC_GatewayIp);
          StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_INVALID_GATEWAY_IP), NULL);  
          StringBuffer2 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_GATEWAY), NULL);  
          StringBuffer3 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_IPV4_FORMAT), NULL);  
          StringBuffer4 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);
          do {
            CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3,StringBuffer4, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN && Key.ScanCode != SCAN_ESC);
        } else {
          StrCpyS (mTempSetupBmcCfg.ShareNIC_GatewayIp, ARRAY_SIZE(mTempSetupBmcCfg.ShareNIC_GatewayIp), TempBmcLanConfig->ShareNIC_GatewayIp);      
          //Status = BmcSetMyLanConfig (SHARED_NIC_CHANNEL_SEL);
          //GetSetIpPram (SHARED_NIC_CHANNEL_SEL, GATEWAY_IP_PARM, TempBmcLanConfig->ShareNIC_GatewayIp, FALSE);  
        }
        break;
        
        case KEY_BMC_ONBOARD_NIC_IPV6_ADDR:
          Status = Ipv6AddrCheck (TempBmcLanConfig->OnBoardNIC_Ipv6Address);
          if (Status == EFI_SUCCESS) {
            Status = StrStringToIp6Adr (&Ipv6_Add, TempBmcLanConfig->OnBoardNIC_Ipv6Address);
          }
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpyS (TempBmcLanConfig->OnBoardNIC_Ipv6Address, ARRAY_SIZE(TempBmcLanConfig->OnBoardNIC_Ipv6Address), mTempSetupBmcCfg.OnBoardNIC_Ipv6Address);
          StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_INVALID_IP), NULL);  
          StringBuffer2 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_IP), NULL);  
          StringBuffer3 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_IPV6_FORMAT), NULL);  
          StringBuffer4 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);
          do {
             CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3, StringBuffer4, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN && Key.ScanCode != SCAN_ESC);
        } else {
          StrCpyS (mTempSetupBmcCfg.OnBoardNIC_Ipv6Address, ARRAY_SIZE(mTempSetupBmcCfg.OnBoardNIC_Ipv6Address), TempBmcLanConfig->OnBoardNIC_Ipv6Address);      
          //Status = BmcSetMyLanConfig (BASE_BOARD_NIC_CHANNEL_SEL);
          //GetSetIpPram (BASE_BOARD_NIC_CHANNEL_SEL, IPV6_ADDRESS_PARM, TempBmcLanConfig->OnBoardNIC_Ipv6Address, FALSE);
        }
        break;

      case KEY_BMC_ONBOARD_NIC_IPV6_GATEWAY_ADDR:
          Status = Ipv6AddrCheck (TempBmcLanConfig->OnBoardNIC_GatewayIpv6);
          if (Status == EFI_SUCCESS) {
            Status = StrStringToIp6Adr (&Ipv6_Add, TempBmcLanConfig->OnBoardNIC_GatewayIpv6);
          }
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpyS (TempBmcLanConfig->OnBoardNIC_GatewayIpv6, ARRAY_SIZE(TempBmcLanConfig->OnBoardNIC_GatewayIpv6), mTempSetupBmcCfg.OnBoardNIC_GatewayIpv6);
          StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_INVALID_GATEWAY_IP), NULL);  
          StringBuffer2 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_GATEWAY), NULL);  
          StringBuffer3 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_IPV6_FORMAT), NULL);  
          StringBuffer4 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL); 
          do {
           CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3, StringBuffer4, NULL);
           } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN && Key.ScanCode != SCAN_ESC);
        } else {
          StrCpyS (mTempSetupBmcCfg.OnBoardNIC_GatewayIpv6, ARRAY_SIZE(mTempSetupBmcCfg.OnBoardNIC_GatewayIpv6), TempBmcLanConfig->OnBoardNIC_GatewayIpv6);      
          //Status = BmcSetMyLanConfig (BASE_BOARD_NIC_CHANNEL_SEL);
          //GetSetIpPram (BASE_BOARD_NIC_CHANNEL_SEL, GATEWAY_IPV6_PARM, TempBmcLanConfig->OnBoardNIC_GatewayIpv6, FALSE);
        }
        break;

      case KEY_BMC_SHARED_NIC_IPV6_ADDR:
          Status = Ipv6AddrCheck (TempBmcLanConfig->ShareNIC_Ipv6Address);
          if (Status == EFI_SUCCESS) {
            Status = StrStringToIp6Adr (&Ipv6_Add, TempBmcLanConfig->ShareNIC_Ipv6Address);
          }
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpyS (TempBmcLanConfig->ShareNIC_Ipv6Address, ARRAY_SIZE(TempBmcLanConfig->ShareNIC_Ipv6Address), mTempSetupBmcCfg.ShareNIC_Ipv6Address);
          StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_INVALID_IP), NULL);  
          StringBuffer2 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_IP), NULL);  
          StringBuffer3 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_IPV6_FORMAT), NULL);  
          StringBuffer4 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);  
          do {
            CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3, StringBuffer4, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN && Key.ScanCode != SCAN_ESC);
        } else {
          StrCpyS (mTempSetupBmcCfg.ShareNIC_Ipv6Address, ARRAY_SIZE(mTempSetupBmcCfg.ShareNIC_Ipv6Address), TempBmcLanConfig->ShareNIC_Ipv6Address);      
         //Status = BmcSetMyLanConfig (SHARED_NIC_CHANNEL_SEL);
         // GetSetIpPram (SHARED_NIC_CHANNEL_SEL, IPV6_ADDRESS_PARM, TempBmcLanConfig->ShareNIC_Ipv6Address, FALSE);
        }
        break;

      case KEY_BMC_SHARED_NIC_IPV6_GATEWAY_ADDR:
          Status = Ipv6AddrCheck (TempBmcLanConfig->ShareNIC_GatewayIpv6);
          if (Status == EFI_SUCCESS) {
            Status = StrStringToIp6Adr (&Ipv6_Add, TempBmcLanConfig->ShareNIC_GatewayIpv6);
          }
          if (Status == EFI_INVALID_PARAMETER) {
          StrCpyS (TempBmcLanConfig->ShareNIC_GatewayIpv6, ARRAY_SIZE(TempBmcLanConfig->ShareNIC_GatewayIpv6), mTempSetupBmcCfg.ShareNIC_GatewayIpv6);
          StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_INVALID_GATEWAY_IP), NULL);  
          StringBuffer2 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_GATEWAY), NULL);  
          StringBuffer3 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_IPV6_FORMAT), NULL);  
          StringBuffer4 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);           
          do {
            CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, StringBuffer3, StringBuffer4, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN && Key.ScanCode != SCAN_ESC);
        } else {
          StrCpyS (mTempSetupBmcCfg.ShareNIC_GatewayIpv6, ARRAY_SIZE(mTempSetupBmcCfg.ShareNIC_GatewayIpv6), TempBmcLanConfig->ShareNIC_GatewayIpv6);      
         // Status = BmcSetMyLanConfig (SHARED_NIC_CHANNEL_SEL);
          //GetSetIpPram (SHARED_NIC_CHANNEL_SEL, GATEWAY_IPV6_PARM, TempBmcLanConfig->ShareNIC_GatewayIpv6, FALSE);
        }
        break;

      case KEY_BMC_ONBOARD_NIC_DHCP_NAME:
        Status = isValidDHCPName(TempBmcLanConfig->Dhcp_Name);
        if( Status ==  EFI_UNSUPPORTED) { 
          StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_DHCP_INVALID), NULL);  
          StringBuffer2 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);  
          do {
             CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN && Key.ScanCode != SCAN_ESC);
          StrCpyS (TempBmcLanConfig->Dhcp_Name, ARRAY_SIZE(TempBmcLanConfig->Dhcp_Name), mTempSetupBmcCfg.Dhcp_Name);
          Status = EFI_INVALID_PARAMETER;
        }else {
          StrCpyS (mTempSetupBmcCfg.Dhcp_Name, ARRAY_SIZE(mTempSetupBmcCfg.Dhcp_Name), TempBmcLanConfig->Dhcp_Name);
        }
        break;

      case KEY_BMC_ADD_USER_NAME:
      case KEY_BMC_DEL_USER_NAME:
      case KEY_BMC_CHANGE_USER_NAME:
        UserInfoPtr = (BMC_USER_INFO *) &(TempBmcLanConfig->BmcUserInfo[0]);
        DEBUG((EFI_D_INFO,"UserInfoPtr UserName :%s\n",UserInfoPtr->UserName));
        OldUserInfoPtr  = (BMC_USER_INFO *) &(mOldSetupBmcCfgPtr.BmcUserInfo[0]);
        if ((0 == StrCmp ((CHAR16*)PcdGetPtr(PcdBmcAdminName), UserInfoPtr->UserName)) && (QuestionId == KEY_BMC_DEL_USER_NAME)) {
          StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_DELETE_USER_2), NULL);
          UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
          StrCpyS(UserInfoPtr->UserName,ARRAY_SIZE(UserInfoPtr->UserName), NULL_STRING);
          break;
        }
        if(!CheckUserNameMatch(UserInfoPtr->UserName)){
           ZeroMem(UserInfoPtr->UserName,sizeof(UserInfoPtr->UserName));
           StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_USERNAME), NULL);  
           StringBuffer2 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);  
           do {
             CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, NULL);
           } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN && Key.ScanCode != SCAN_ESC);
           Status = EFI_INVALID_PARAMETER;  
           break;
        };
        TempBmcLanConfig->User = 0;
        for (Index = 1; Index <= MAX_USERS; Index++) {
          ZeroMem(TempUserName, sizeof(TempUserName));
          Status = BmcGetUserName (TempUserName, Index);
          //DEBUG((EFI_D_INFO,"Current User ID: %d\n",Index ));        
          //DEBUG((EFI_D_INFO,"User ID: %d Username :%s\n",Index,TempUserName));
          if(EFI_ERROR (Status) || (0 == StrLen(TempUserName))){
             if(((TempBmcLanConfig->User == 0) || (TempBmcLanConfig->User == 1)) && (QuestionId == KEY_BMC_ADD_USER_NAME)){
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
              StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_NAME_EXIST), NULL);  
              StringBuffer2 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CONTINUE), NULL);  
              do {
               CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, NULL);
              } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN && Key.ScanCode != SCAN_ESC);                  
              StrCpyS (UserInfoPtr->UserName, ARRAY_SIZE(UserInfoPtr->UserName), OldUserInfoPtr->UserName); //restore current user name
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
        DEBUG((EFI_D_INFO,"Current User ID: %d\n",TempBmcLanConfig->User ));             
        if(!TempBmcLanConfig->User && (QuestionId == KEY_BMC_ADD_USER_NAME)){
          StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_NEED_SET_PASSWORD), NULL);
          UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
        }else if(!TempBmcLanConfig->User){
          StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_INVALID_USER), NULL);
          UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
          StrCpyS (UserInfoPtr->UserName, ARRAY_SIZE(UserInfoPtr->UserName), OldUserInfoPtr->UserName); //restore current user name
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
            StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_SET_FAILED), NULL);
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
      // Get Password form hii database handle.
      //
      if (Type == EFI_IFR_TYPE_STRING && Value->string != 0) {
        Password = HiiGetString(mHiiHandle, Value->string, NULL);  
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
            StringBuffer5 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_SAME_WITH_LAST_PASSWORD), NULL);
            UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer5, NULL); 
            Status = EFI_ALREADY_STARTED;
            break;
          }
          Status = BmcSetBmcPassword(Password, TempBmcLanConfig->User);
          if (!EFI_ERROR (Status)) {
            StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_SET_SUCCESS), NULL);
            UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
            StrCpyS(CurrentUserLastPwd, ARRAY_SIZE(CurrentUserLastPwd), Password);
            Status = EFI_ALREADY_STARTED;
          } else {
            StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_SET_FAILED), NULL);
            UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
          }
          break;
        case KEY_BMC_DEL_USER_PASSWORD:
        case KEY_BMC_VERIFY_USER_PASSWORD:
          DEBUG((EFI_D_INFO, "UserID = %x\n", TempBmcLanConfig->User));
          DEBUG((EFI_D_INFO, "Password = %s\n", Password));
          if(TempBmcLanConfig->User == 0){
            StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_SET_USER_FIRST), NULL);
            UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
            Status = EFI_NOT_AVAILABLE_YET;
            break;
          }         
          //password cant be NULL
          if (NULL ==Password || !StrLen(Password)) {
            Status = EFI_NOT_READY;
            break;
          }
          //verify password correct
          if (TestUserPassword(TempBmcLanConfig->User,Password)) {
            //UserInfoPtr->UserPresent = 1;
            StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_WRONG_PASSWORD), NULL);
            UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
          }else{
            if(QuestionId == KEY_BMC_DEL_USER_PASSWORD){
              //
              //delete user
              //
              Status = BmcDelUser(TempBmcLanConfig->User);
              if(!EFI_ERROR(Status)){
                StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_DELETE_USER_SUCCESS), NULL);
                UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);   
                //CopyMem(UserInfoPtr, OldUserInfoPtr, sizeof(OldUserInfoPtr));
                BmcSetUserDefault(TempBmcLanConfig);
              }else{
                StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_DELETE_USER_FAIL), NULL);
                UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);  
              }
            }else{
              StrCpyS(CurrentUserLastPwd, ARRAY_SIZE(CurrentUserLastPwd), Password);
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
                  StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_SET_USER_FIRST), NULL);
                  UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
                  Status = EFI_NOT_AVAILABLE_YET;
                  break;
              }
              //check password present,Standard Ipmi spec don't support the cmd to check user password present! so we just check the user name.
              ZeroMem(TempUserName, sizeof(TempUserName));
              Status = BmcGetUserName (TempUserName, TempBmcLanConfig->User);
              if (EFI_ERROR (Status) || (0 == StrLen(TempUserName))) {
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
              //verify password correct
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
                Status = BmcSetBmcPassword(Password, TempBmcLanConfig->User);
                //DEBUG((EFI_D_INFO, "BmcSetBmcPassword :%r  --Complete Code = %x\n", Status,EfiIGetLastIpmiCmdCompleteCode()));
                          
                if(EFI_ERROR (Status) || EfiIGetLastIpmiCmdCompleteCode() != 0){
                       if(TempBmcLanConfig->User !=0){
                         if(TempBmcLanConfig->User != lastuser){
                           BmcDelUser(TempBmcLanConfig->User);
                           lastuser = 0;
                         }
                       }
                       StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_PD), NULL);
                       UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, StringBuffer1, NULL);
                       PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
                       Status = EFI_NOT_AVAILABLE_YET;
                       break;
                }
                if(TempBmcLanConfig->User != 0 ){
                  UserInfoPtr->UserPresent = 1;  //patch load default
                }
                BmcEnableUserSolPermissions(TempBmcLanConfig->User);
                BmcSetUserPwdValidDays (UserInfoPtr->UserName);
                lastuser = TempBmcLanConfig->User;   
              } else {     
                  StringBuffer1 = HiiGetString(mHiiHandle, STRING_TOKEN(STR_WRONG_PASSWORD), NULL);
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
