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

#ifndef _EFI_BMC_CONFIG_H
#define _EFI_BMC_CONFIG_H

#include <Guid/HiiPlatformSetupFormset.h>

#define SYSTEM_CONFIGURATION_GUID \
  { \
    0xec87d643, 0xeba4, 0x4bb5, 0xa1, 0xe5, 0x3f, 0x3e, 0x36, 0xb2, 0xd, 0xa9 \
  }

#define SERVER_MANAGEMENT_FORM_SET_GUID \
  { \
    0x6d99172d, 0xd29a, 0x4b59, { 0x8c, 0x8f, 0x23, 0x83, 0xbc, 0xd7, 0x7d, 0x3c } \
  }

#define SETUP_BMC_CFG_GUID \
  { \
    0x8236697e, 0xf0f6, 0x405f, 0x99, 0x13, 0xac, 0xbc, 0x50, 0xaa, 0x45, 0xd1 \
  }

#define PASSWORD_16_BYTES  0
#define PASSWORD_20_BYTES  1

#define MAX_USERS   16
#pragma pack(1)

typedef struct _BMC_USER_INFO {
  UINT16 UserName[20];
  UINT16 UserPassword[20];
  UINT8  EnabaleChannelUser;
  UINT8  Privilege;
  UINT16 UserPresent; //aligned on a 16-bit boundary for StrCpy
} BMC_USER_INFO;

typedef struct _BMC_LAN_INFO {
  UINT8   NicIpSource; 
  UINT8   Reserved1;
  UINT16  IpAddress[20];
  UINT16  SubnetMask[20];
  UINT16  GatewayIp[20];
  UINT16  DhcpName[64];
} BMC_LAN_INFO;

//
//IPV6 LAN INFO STRUCT
//
typedef struct _BMC_LAN_IPV6_INFO {
  UINT8   NicIpv6Source; 
//
//Add Ipv6Address,GatewayIpv6 size from 40 t0 46 for reducing there size in NVRAM
//The maxsize of Ipv6Address,GatewayIpv6 is 45 (eg. 2001:2002:2003:2004:2006:2006:192.168.172.110)
// 
  UINT16  Ipv6Address[46];
  UINT16  GatewayIpv6[46];
  UINT8   NicIpv6Prefix;
  UINT8   Reserved1;  
  UINT16  DhcpName[64];
  UINT8   OnBoardNIC_IpSelection;
  UINT8   DedicateNIC_IpSelection;
} BMC_LAN_IPV6_INFO;


typedef struct 
{
  UINT8  OnBoardNIC_IpMode;
  UINT8  Reserved1;
  UINT16 OnBoardNIC_IpAddress[20];
  UINT16 OnBoardNIC_SubnetMask[20];
  UINT16 OnBoardNIC_GatewayIp[20];
  UINT16 Dhcp_Name[64];

  UINT8  DedicateNIC_IpMode;
  UINT8  Reserved2;  
  UINT16 DedicateNIC_IpAddress[20];
  UINT16 DedicateNIC_SubnetMask[20];
  UINT16 DedicateNIC_GatewayIp[20];
//  UINT8  RmmNic_IpMode;
//  UINT8  Reserved2;  
//  UINT16 RmmNic_IpAddress[20];
//  UINT16 RmmNic_SubnetMask[20];
//  UINT16 RmmNic_GatewayIp[20];
//  UINT8  RmmPresent;

//  UINT8  NicPresent;

  BMC_USER_INFO BmcUserInfo[MAX_USERS];

  UINT8   User; 
  UINT32  BmcStatus;
  
  UINT8  OnBoardNIC_Ipv6Mode;
//
//Add size of OnBoardNIC_Ipv6Address,OnBoardNIC_GatewayIpv6 from 40 to 46 for reducing there size in NVRAM.
//  
  UINT16 OnBoardNIC_Ipv6Address[46];
  UINT16 OnBoardNIC_GatewayIpv6[46];
  UINT8  OnBoardNIC_Ipv6PreLen;
  UINT8  Reserved3;  
  UINT16 Dhcp_Name_Ipv6[64];
  UINT8  OnBoardNIC_IpSelection;

  UINT8  DedicateNIC_Ipv6Mode;
//  UINT8  RmmNic_Ipv6Mode;
//
//Add size of RmmNic_Ipv6Address,RmmNic_GatewayIpv6 from 40 to 46 for reducing there size in NVRAM.
//  
  UINT16 DedicateNIC_Ipv6Address[46];
  UINT16 DedicateNIC_GatewayIpv6[46];
  UINT8  DedicateNIC_Ipv6PreLen;
  UINT8  Reserved4;  
//  UINT16 RmmNic_Ipv6Address[46];
//  UINT16 RmmNic_GatewayIpv6[46];
//  UINT8  RmmNic_PrefixLength;
//  UINT8  Ipv6RmmPresent;
  UINT8  DedicateNIC_IpSelection;
  
  //Reserved
    UINT8  iBMCPortMode;
    UINT8  UserPwdChanged;
} BMC_LAN_CONFIG;


//
//ServerMgmt
//
typedef struct {
  UINT8   FanProfile;
  UINT8   FanSpeedControl;
  UINT8   WdtEnable[4];
  UINT8   SerialOverLanEnable;
  UINT8   WdtPolicy[4];
  UINT16  WdtTimeout[4];
  UINT8   PowerRestorePolicy;
  UINT8   NCSI;
 // UINT8   ShareLink;  //+TangYan+
 // UINT8   VLanState;
 // UINT16  VLanID;
  UINT8   BmcVendor;//0:byo 1:ami
} BMC_SETUP_DATA;

#pragma pack()

#define BMC_SETUP_VARIABLE_NAME    L"BmcSetup"
#define BMC_SETUP_VARIABLE_FLAG    (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE)

#define BMC_SETUP_DATA_VARSTORE \
  efivarstore BMC_SETUP_DATA,  varid = 0xf,  attribute = 7,  name  = BmcSetup,  guid  = SYSTEM_CONFIGURATION_GUID;

#define KEY_SERVER_MGMT_CLEAR_SEL         0x8012
#define FORM_ID_BMC_LAN_CONFIG            0x402D
#define FORM_ID_BMC_USER_CONFIG           0x402E
#define FORM_ID_BMC_USER_ADD              0x402F
#define FORM_ID_BMC_USER_DEL              0x4030
#define FORM_ID_BMC_USER_CHANGE           0x4031
#define HM_FORM_ID                        0x4000
#define FRU_FORM_ID                       0x4001

#define SEL_FORM_ID                       0x4002
#define KEY_OPEN_SEL_FORM                 0x8040
#define LABEL_SEL_START                   0x1100
#define LABEL_SEL_END                     0x1102

#define  KEY_BMC_FAN_PROFILE                             3055  
#define  KEY_BMC_FAN_SPEED_CONTROL                       3056

#define  KEY_BMC_ONBOARD_NIC_IPV4_ADDR                   2001
#define  KEY_BMC_ONBOARD_NIC_IPV4_SUBNET_MASK_ADDR       2002
#define  KEY_BMC_ONBOARD_NIC_IPV4_GATEWAY_ADDR           2003

#define  KEY_BMC_DEDICATE_NIC_IPV4_ADDR                   2004
#define  KEY_BMC_DEDICATE_NIC_IPV4_SUBNET_MASK_ADDR       2005
#define  KEY_BMC_DEDICATE_NIC_IPV4_GATEWAY_ADDR           2006

#define  KEY_BMC_ONBOARD_NIC_DHCP_NAME                   2010

#define  KEY_BMC_ONBOARD_NIC_IPV6_ADDR                   2011
#define  KEY_BMC_ONBOARD_NIC_IPV6_GATEWAY_ADDR           2012

#define  KEY_BMC_DEDICATE_NIC_IPV6_ADDR                   2013
#define  KEY_BMC_DEDICATE_NIC_IPV6_GATEWAY_ADDR           2014


#define  KEY_BMC_USER_SELECT                              2025

#define  KEY_BMC_ADD_USER_NAME                            3500
#define  KEY_BMC_ADD_USER_PASSWORD                        3800
#define  KEY_BMC_ADD_USER_PRIVILEGE                       3600
#define  KEY_BMC_ADD_USER_ENABALE_CHANNEL_USER            3700

#define  KEY_BMC_DEL_USER_NAME                            3501
#define  KEY_BMC_DEL_USER_PASSWORD                        3801

#define  KEY_BMC_CHANGE_USER_NAME                         3502
#define  KEY_BMC_CHANGE_USER_PASSWORD                     3802
#define  KEY_BMC_CHANGE_USER_PRIVILEGE                    3602
#define  KEY_BMC_CHANGE_USER_ENABALE_CHANNEL_USER         3702

#define  KEY_BMC_VERIFY_USER_PASSWORD                     3803




#define  KEY_BMC_USER_NAME1                               2901
#define  KEY_BMC_USER_PRIVILEGE1                          2201
#define  KEY_BMC_USER_ENABALE_CHANNEL_USER1               2401
#define  KEY_BMC_USER_PASSWORD1                           3001

#define  KEY_BMC_USER_NAME2                               2902
#define  KEY_BMC_USER_PRIVILEGE2                          2202
#define  KEY_BMC_USER_ENABALE_CHANNEL_USER2               2402
#define  KEY_BMC_USER_PASSWORD2                           3002

#define  KEY_BMC_USER_NAME3                               2903
#define  KEY_BMC_USER_PRIVILEGE3                          2203
#define  KEY_BMC_USER_ENABALE_CHANNEL_USER3               2403
#define  KEY_BMC_USER_PASSWORD3                           3003


#define  KEY_BMC_USER_NAME4                               2904
#define  KEY_BMC_USER_PRIVILEGE4                          2204
#define  KEY_BMC_USER_ENABALE_CHANNEL_USER4               2404
#define  KEY_BMC_USER_PASSWORD4                           3004

#define  KEY_BMC_USER_NAME5                               2905
#define  KEY_BMC_USER_PRIVILEGE5                          2205
#define  KEY_BMC_USER_ENABALE_CHANNEL_USER5               2405
#define  KEY_BMC_USER_PASSWORD5                           3005

#define  KEY_BMC_USER_NAME6                               2906
#define  KEY_BMC_USER_PRIVILEGE6                          2206
#define  KEY_BMC_USER_ENABALE_CHANNEL_USER6               2406
#define  KEY_BMC_USER_PASSWORD6                           3006

#define  KEY_BMC_USER_NAME7                               2907
#define  KEY_BMC_USER_PRIVILEGE7                          2207
#define  KEY_BMC_USER_ENABALE_CHANNEL_USER7               2407
#define  KEY_BMC_USER_PASSWORD7                           3007

#define  KEY_BMC_USER_NAME8                               2908
#define  KEY_BMC_USER_PRIVILEGE8                          2208
#define  KEY_BMC_USER_ENABALE_CHANNEL_USER8               2408
#define  KEY_BMC_USER_PASSWORD8                           3008

#define  KEY_BMC_USER_NAME9                               2909
#define  KEY_BMC_USER_PRIVILEGE9                          2209
#define  KEY_BMC_USER_ENABALE_CHANNEL_USER9               2409
#define  KEY_BMC_USER_PASSWORD9                           3009

#define  KEY_BMC_USER_NAME10                               2910
#define  KEY_BMC_USER_PRIVILEGE10                          2210
#define  KEY_BMC_USER_ENABALE_CHANNEL_USER10               2410
#define  KEY_BMC_USER_PASSWORD10                           3010

#define  KEY_BMC_USER_NAME11                               2911
#define  KEY_BMC_USER_PRIVILEGE11                          2211
#define  KEY_BMC_USER_ENABALE_CHANNEL_USER11               2411
#define  KEY_BMC_USER_PASSWORD11                           3011

#define  KEY_BMC_USER_NAME12                               2912
#define  KEY_BMC_USER_PRIVILEGE12                          2212
#define  KEY_BMC_USER_ENABALE_CHANNEL_USER12               2412
#define  KEY_BMC_USER_PASSWORD12                           3012

#define  KEY_BMC_USER_NAME13                               2913
#define  KEY_BMC_USER_PRIVILEGE13                          2213
#define  KEY_BMC_USER_ENABALE_CHANNEL_USER13               2413
#define  KEY_BMC_USER_PASSWORD13                           3013

#define  KEY_BMC_USER_NAME14                               2914
#define  KEY_BMC_USER_PRIVILEGE14                          2214
#define  KEY_BMC_USER_ENABALE_CHANNEL_USER14               2414
#define  KEY_BMC_USER_PASSWORD14                           3014

#define  KEY_BMC_USER_NAME15                               2915
#define  KEY_BMC_USER_PRIVILEGE15                          2215
#define  KEY_BMC_USER_ENABALE_CHANNEL_USER15               2415
#define  KEY_BMC_USER_PASSWORD15                           3015

#define  KEY_BMC_USER_NAME16                               2916
#define  KEY_BMC_USER_PRIVILEGE16                          2216
#define  KEY_BMC_USER_ENABALE_CHANNEL_USER16               2416
#define  KEY_BMC_USER_PASSWORD16                           3016

#endif
