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

#ifndef _BMC_VAR_VARIABLE_H_
#define _BMC_VAR_VARIABLE_H_

#define BMC_CONFIG_PROTOCOL_GUID \
  { \
    0x5b525f29, 0x827d, 0x4ca3, 0xbe, 0xb5, 0x89, 0x54, 0xb5, 0x6c, 0x5, 0x73 \
  }

typedef struct _BMC_CONFIG_PROTOCOL BMC_CONFIG_PROTOCOL;


//
// Server Management Variable types
//
typedef enum {
  BMC_WDT_CFG,
  BMC_SEL_CFG
} BMC_CFG_CLASS;

//
// Server Management Variable
//
typedef union {
  EFI_BMC_WDT_CFG       WdtCfg[4];
} BMC_CONFIG_DATA;

//
// Server Management Variable Protocol
//
typedef
EFI_STATUS
(EFIAPI *BMC_SET_CFG) (
  IN  BMC_CONFIG_PROTOCOL         *This,
  IN  BMC_CFG_CLASS               CfgClass
  );

typedef
EFI_STATUS
(EFIAPI *BMC_GET_CFG) (
  IN  BMC_CONFIG_PROTOCOL         *This,
  IN  BMC_CFG_CLASS               CfgClass
  );

//
// PROTOCOL
//
struct _BMC_CONFIG_PROTOCOL {
  BMC_SET_CFG       GetCfg;
  BMC_SET_CFG       SetCfg;
};

extern EFI_GUID gEfiSmBmcConfigGuid;

#endif
