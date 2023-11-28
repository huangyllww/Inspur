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

#ifndef _EFI_SETUP_BMC_CFG_H
#define _EFI_SETUP_BMC_CFG_H
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <EfiServerManagement.h>
#include <Library/IpmiBaseLib.h>
#include <Library/EfiServerMgmtRtLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/PrintLib.h>

#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/SetupSaveNotify.h>
#include <Protocol/IpmiInterfaceProtocol.h>
#include <BmcConfig.h>
#include "Token.h"
#include <SetupVariable.h>

//#include <Ipmi/IpmiNetFnIntelPlatformSpecificDefinitions.h>
extern EFI_GUID gEfiSetupEnterGuid;

extern EFI_HII_HANDLE HiiHandle;
extern UINT8    BmcConfigBin[];
extern UINT8    SetupBmcCfgStrings[];

#pragma pack(1)
///
/// HII specific Vendor Device Path definition.
///
typedef struct {
  VENDOR_DEVICE_PATH             VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL       End;
} HII_VENDOR_DEVICE_PATH;
#pragma pack()


#define IS_DIGIT(Ch)  (((Ch) >= L"0"[0]) && ((Ch) <= L"9"[0]))

// Channel Definitions
#ifndef BASE_BOARD_NIC_CHANNEL_SEL
#define BASE_BOARD_NIC_CHANNEL_SEL     1
#endif

#ifndef DEDICATE_NIC_CHANNEL_SEL
#define DEDICATE_NIC_CHANNEL_SEL       8
#endif

#define INVALID_HANDLE ((EFI_HII_HANDLE)-1)

// Lan Net function
#define EFI_SM_NETFN_LAN        0x0C

// Set Lan configuration command 
#define EFI_SET_LAN_CONFIG_CMD  0x01

// Get Lan configuration command 
#define EFI_GET_LAN_CONFIG_CMD  0x02

// Definitions for Lan configuration Parameters 
#define IP_ADDRESS_PARM         0x03
#define IP_SOURCE_PARM          0x04
#define SUBNET_MASK_PARM        0x06
#define GATEWAY_IP_PARM         0x0c
#define VLAN_PARM               0x14
#define DHCP_NAME_PARM          0xC7

// Definitions for Lan configuration Parameters  for IPV6
//
#define IPV6_STATUS_PARM          0x33
#define IPV6_ADDRESS_PARM         0x38
#define IPV6_ADDRESS_DHCP         0x3b
#define GATEWAY_IPV6_CONFIG_PARM  0x40
#define GATEWAY_IPV6_PARM         0x41
#define DHCP_IPV6_NAME_PARM       0xC7
//User configuration Commands
#define GET_USER_NAME_CMD       0x46
#define GET_USER_ACCESS_CMD     0x44
#define SET_USER_NAME_CMD       0x45
#define SET_USER_ACCESS_CMD     0x43
#define SET_USER_PASSWORD_CMD   0x47


#define BMC_SETUP_CONFIG_PRIVATE_SIGNATURE SIGNATURE_32 ('B', 'M', 'C', 'C')


typedef enum {
    BMC_USER_RSV = 0,
    BMC_USER_ADD,
    BMC_USER_DEL,
    BMC_USER_CHANGED
} BMC_USER_OPERATION;

typedef struct _BMC_USER_RECORD {
  UINT8  UserId;
  UINT8  Operation;
  UINT16 UserName[20];
  UINT16 UserPassword[20];
} BMC_USER_RECORD;

typedef struct {
  UINTN                            Signature;
  EFI_HANDLE                       DriverHandle;
  EFI_HII_HANDLE                   HiiHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL   ConfigAccess;
  SETUP_SAVE_NOTIFY_PROTOCOL       SetupSaveNotify;
  BMC_LAN_CONFIG                   BmcLanConfig;
} BMC_SETUP_CONFIG_PRIVATE_DATA;

#define BMC_SETUP_CONFIG_PRIVATE_FROM_THIS_HII(a)  \
  CR(a, BMC_SETUP_CONFIG_PRIVATE_DATA, ConfigAccess, BMC_SETUP_CONFIG_PRIVATE_SIGNATURE)




UINT8
IsNicPresent (
  VOID
  );

/*++

Routine Description:

  Check for NIC presence 

Arguments:

  None

Returns:

  TRUE - If NIC present

--*/

UINTN
MemCmp (
  IN UINT8 *dest,
  IN UINT8 *src,
  IN UINTN size
  );
/*++
Routine Description:
  Compare Memory contents

Arguments:
  dest - Destination memory Address.
  src  - Source Memory Address.
  size - Length of Memory location to compare.
  
Returns:
  TRUE- If Buffers are same
  FALSE-If Buffers are not same
 
--*/

void
MemCpy (
  IN void*dst,
  IN void*src,
  IN int sz
  );
/*++

Routine Description:

  Copy memory buffer

Arguments:

  dst - Destination memory location
  src - Source
  sz  - Size

Returns:
  
  None

--*/
VOID
AsciiToUnicode (
  IN      CHAR8     *AsciiString,
  IN OUT  CHAR16    *UnicodeString
  );
/*++

Routine Description:

  Converts an ascii string to unicode string.

Arguments:

  AsciiString - ASCII String to Convert
  UnicodeString - Converted Unicode String 

Returns:
  UnicodeString - Converted Unicode String 
--*/
CHAR8 *
Unicode2Ascii (
  OUT CHAR8          *AsciiStr,
  IN  CHAR16         *UnicodeStr
  );
/*++
  
  Routine Description:

    This function converts Unicode string to ASCII string.
  
  Arguments:

    AsciieStr      - Converted ACII string
    UnicodeStr     - Unicode String to convert
 
  Returns: 

    Start of the ASCII ouput string.
    
--*/
STATIC
EFI_STATUS
EfiStringToValue (
  OUT UINT64        *Val,
  IN  CHAR16        *String,
  OUT UINT8         *EndIdx OPTIONAL
  );
/*++

Routine Description:
  Parses and converts Unicode string to decimal value.
  The returned value is 64-bit.
  The string is expected in decimal format,
  the string is parsed and format verified.
  This function is missing from the libs. One day it maybe
  replaced with a lib function when it'll become available.

Arguments:
  Val    - pointer to the variable to store the value to
  String - string that contains the value to parse and convert
  EndIdx - index on which the parsing stopped. It points to the
           first character that was not part of the returned Val.
           It's valid only if the function returns success.
           It's optional and it could be NULL.

Returns:
  EFI_SUCCESS           - if successful
  EFI_INVALID_PARAMETER - if String is in unexpected format

--*/

EFI_STATUS
StrStringToIp4Adr (
  OUT EFI_IPv4_ADDRESS   *Ip4Addr,
  IN  CHAR16             *String
  );
/*++

Routine Description:
  Parses and converts Unicode string to IP v4 address.
  The value will 64-bit.
  The string must be four decimal values 0-255 separated by dots.
  The string is parsed and format verified.

Arguments:
  Ip4Addr - pointer to the variable to store the value to
  String  - string that contains the value to parse and convert

Returns:
  EFI_SUCCESS           - if successful
  EFI_INVALID_PARAMETER - if String contains invalid IP v4 format

--*/

EFI_STATUS
StrStringToIp6Adr (
  OUT EFI_IPv6_ADDRESS      *Ip6Addr,
  IN  CHAR16             *String
  );
EFI_STATUS
Ipv6AddtoStr (
  IN OUT CHAR16        *Buffer,
  IN     UINT8         *Ipadd
  );
EFI_STATUS
IpAddtoStr (
  IN OUT CHAR16        *Buffer,
  IN     UINT8         *Ipadd
  );
/*++

Routine Description:

  Converts IP Address Buffer to String

Arguments:

  Buffer  - GC_TODO: add argument description
  Ipadd   - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/

VOID
EfiLibSafeFreePool (
  IN  VOID             *Buffer
  );

/*++

Routine Description:

  Validate user strings entered.

Arguments:

  This      - FROM CALLBACK PROTOCOL
  KeyValue  - Setup option
  Data      - VFR Control data
  Packet    - Return Data if any.

Returns:

  GC_TODO: add return values

--*/

//
//IPV6 Prefix Length
//
EFI_STATUS
GetSetIpv6Prefix (
  IN     UINT8          Channel,
  IN     UINT8          LanParameter,
  IN OUT UINT8         *PrefixLen,
  IN     BOOLEAN        CommandGet
  );
//
//IPV6 Prefix Length
//
EFI_STATUS
GetSetIpv6Status (
  IN     UINT8          Channel,
  IN     UINT16          LanParameter,
  IN OUT UINT8          *Ipv6Enable,
  IN     BOOLEAN        CommandGet
  );

EFI_STATUS
GetSetIpv6Pram (
  IN     UINT8          Channel,
  IN     UINT8          LanParameter,
  IN OUT UINT16         *IpAddress,
  IN     BOOLEAN        CommandGet
  );
/*++

Routine Description:

  Get/Set IP address,Prefix Length and Gateway IP of BMC channels

Arguments:

  Channel - Channel to set
  LanParameter - BMC Command Parameter to set/get IP or Gateway IP
  IpAddress - IPv6 format address to Set/Get
  CommandGet - TRUE - Read Lan parameter, False = Set Lan parameter.

Returns:

  Status : Status returned by BMC. 

--*/
EFI_STATUS
BmcGetLanIPv6Config (
  IN BMC_LAN_IPV6_INFO   *Laninfo,
  IN UINT8          Channel
  );

EFI_STATUS
BmcSetLanIpv6Config (
  IN BMC_LAN_IPV6_INFO   *Laninfo,
  IN UINT8          Channel
  );
/*++

Routine Description:

  Send LAN configuration to BMC.

Arguments:

  Laninfo - Ponter to LAN configuration.
  Channel - Lan channel

Returns:

  EFI_SUCCESS

--*/
EFI_STATUS
BmcCfgGetVariable (
  IN CHAR16        *VariableName,
  IN EFI_GUID      *VendorGuid,
  OUT UINT32       *Attributes OPTIONAL,
  IN OUT UINTN     *DataSize,
  OUT VOID         *Data
  );

EFI_STATUS
BmcCfgSetVariable (
  IN CHAR16        *VariableName,
  IN EFI_GUID      *VendorGuid,
  IN UINT32        Attributes,
  IN UINTN         DataSize,
  IN VOID          *Data
  );
  
EFI_STATUS
ValidateSetupInput (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );
  
UINTN
EfiValueToHexStr (
  IN  OUT CHAR16  *Buffer, 
  IN  UINT64      Value, 
  IN  UINTN       Flags, 
  IN  UINTN       Width
  );
  
EFI_STATUS
HexStringToBuf (
  IN OUT UINT8                     *Buf,   
  IN OUT UINTN                    *Len,
  IN     CHAR16                    *Str,
  OUT    UINTN                     *ConvertedStrLen  OPTIONAL
  );

BOOLEAN
IsHexDigit (
  OUT UINT8      *Digit,
  IN  CHAR16      Char
  );

VOID
EFIAPI
UpdateBmcLanConfig (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

EFI_STATUS
EFIAPI
ServerMgmtFormCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
  );

EFI_STATUS
EFIAPI
HhmCallBack (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      Key,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
  );

VOID
InitFruStrings (
  IN EFI_HII_HANDLE     HiiHandle
  );

#endif
