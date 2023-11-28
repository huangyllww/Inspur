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

#ifndef _SOL_STATUS_PROTOCOL_H_
#define _SOL_STATUS_PROTOCOL_H_

typedef enum {
  IPMI_SOL_BAUD_9600 = 6,
  IPMI_SOL_BAUD_19200,
  IPMI_SOL_BAUD_38400,
  IPMI_SOL_BAUD_57600,
  IPMI_SOL_BAUD_115200
} SOL_BAUDRATE;

typedef struct _EFI_SOL_STATUS_PROTOCOL EFI_SOL_STATUS_PROTOCOL;

typedef
BOOLEAN
(EFIAPI *EFI_GET_SOL_STATUS) (
  IN EFI_SOL_STATUS_PROTOCOL                   *This,
  IN OUT UINT8                                 *SolBaudRate
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_SOL_BAUDRATE) (
  IN EFI_SOL_STATUS_PROTOCOL                   *This,
  IN UINT8                                     *SolBaudRate
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_SOL_BAUDRATE) (
  IN EFI_SOL_STATUS_PROTOCOL                   *This,
  IN UINT8                                     SolBaudRate
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_SOL_ENABLE) (
  IN EFI_SOL_STATUS_PROTOCOL                   *This,
  IN BOOLEAN                                   Enabled
  );

struct _EFI_SOL_STATUS_PROTOCOL {
  EFI_SET_SOL_ENABLE             SetSolEnable;
  EFI_GET_SOL_STATUS             GetSolStatus;
  EFI_GET_SOL_BAUDRATE           GetSolBaudRate;
  EFI_SET_SOL_BAUDRATE           SetSolBaudRate;
  UINT8                          SolBaudRate;
  BOOLEAN                        IpmiSolStatus;
};

extern EFI_GUID gEfiIpmiSolStatusProtocolGuid;

#endif
