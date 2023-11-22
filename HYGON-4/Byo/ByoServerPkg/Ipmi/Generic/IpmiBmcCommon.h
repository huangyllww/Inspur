/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  IpmiBmcCommon.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef _EFI_IPMI_COMMON_BMC_H_
#define _EFI_IPMI_COMMON_BMC_H_

#include <Protocol/IpmiTransportProtocol.h>

#define MAX_TEMP_DATA              255

#define MAX_SOFT_COUNT             10
#define COMP_CODE_NORMAL           0x00
#define COMP_CODE_TOO_MANY_BYTES   0xca
#define BMC_SLAVE_ADDRESS          0x20

//
// IPMI command completion codes to check for in the UpdateErrorStatus routine.
// These completion codes indicate a soft error and a running total of the occurrences
// of these errors is maintained.
//
#define COMP_CODE_NODE_BUSY               0xC0
#define COMP_CODE_TIMEOUT                 0xC3
#define COMP_CODE_OUT_OF_SPACE            0xC4
#define COMP_CODE_OUT_OF_RANGE            0xC9
#define COMP_CODE_CMD_RESP_NOT_PROVIDED   0xCE
#define COMP_CODE_FAIL_DUP_REQUEST        0xCF
#define COMP_CODE_SDR_REP_IN_UPDATE_MODE  0xD0
#define COMP_CODE_DEV_IN_FW_UPDATE_MODE   0xD1
#define COMP_CODE_BMC_INIT_IN_PROGRESS    0xD2
#define COMP_CODE_UNSPECIFIED             0xFF

#define COMPLETION_CODES \
  { \
    COMP_CODE_NODE_BUSY, COMP_CODE_TIMEOUT, COMP_CODE_OUT_OF_SPACE, COMP_CODE_OUT_OF_RANGE, \
    COMP_CODE_CMD_RESP_NOT_PROVIDED, COMP_CODE_FAIL_DUP_REQUEST, COMP_CODE_SDR_REP_IN_UPDATE_MODE, \
    COMP_CODE_DEV_IN_FW_UPDATE_MODE, COMP_CODE_BMC_INIT_IN_PROGRESS, COMP_CODE_UNSPECIFIED \
  }

//
// Dxe Ipmi instance data
//
typedef struct {
  UINTN               Signature;
  UINT64              KcsTimeoutPeriod;
  UINT8               SlaveAddress;
  UINT8               TempData[MAX_TEMP_DATA];
  UINT8               OutData[MAX_TEMP_DATA];
  EFI_BMC_STATUS      BmcStatus;
  UINT64              ErrorStatus;
  UINT8               SoftErrorCount;
  UINT16              IpmiIoBase;
  EFI_IPMI_TRANSPORT  IpmiTransport;
  EFI_HANDLE          IpmiSmmHandle;
  
  UINT8               Sequence;
  UINT32              RetryCounter;
  
} EFI_IPMI_BMC_INSTANCE_DATA;

//
// Structure of IPMI Command buffer
//
#define EFI_IPMI_COMMAND_HEADER_SIZE  2

typedef struct {
  UINT8 Lun : 2;
  UINT8 NetFunction : 6;
  UINT8 Command;
  UINT8 CommandData[MAX_TEMP_DATA - EFI_IPMI_COMMAND_HEADER_SIZE];
} EFI_IPMI_COMMAND;

//
// Structure of IPMI Command response buffer
//
#define EFI_IPMI_RESPONSE_HEADER_SIZE 3

typedef struct {
  UINT8 Lun : 2;
  UINT8 NetFunction : 6;
  UINT8 Command;
  UINT8 CompletionCode;
  UINT8 ResponseData[MAX_TEMP_DATA - EFI_IPMI_RESPONSE_HEADER_SIZE];
} EFI_IPMI_RESPONSE;

EFI_STATUS
SendDataToBmcPort (
  EFI_IPMI_BMC_INSTANCE_DATA                              *IpmiInstance,
  VOID                                                    *Context,
  UINT8                                                   *Data,
  UINTN                                                   DataSize
  )
/*++

Routine Description:

  Send data to BMC

Arguments:

  IpmiInstance  - The pointer of EFI_IPMI_BMC_INSTANCE_DATA
  Context       - The context of this operation
  Data          - The data pointer to be sent
  DataSize      - The data size

Returns:

  EFI_SUCCESS   - Send out the data successfully

--*/
;

EFI_STATUS
ReceiveBmcDataFromPort (
  EFI_IPMI_BMC_INSTANCE_DATA                              *IpmiInstance,
  VOID                                                    *Context,
  UINT8                                                   *Data,
  UINT8                                                   *DataSize
  )
/*++

Routine Description:

  Routine Description:

  Receive data from BMC

Arguments:

  IpmiInstance  - The pointer of EFI_IPMI_BMC_INSTANCE_DATA
  Context       - The context of this operation
  Data          - The buffer pointer
  DataSize      - The buffer size

Returns:

  EFI_SUCCESS   - Received data successfully

--*/
;

EFI_STATUS
EfiIpmiBmcStatus (
  IN EFI_IPMI_TRANSPORT                                    *This,
  OUT EFI_BMC_STATUS                                       *BmcStatus,
  OUT EFI_SM_COM_ADDRESS                                   *ComAddress
  )
/*++

Routine Description:

  Updates the BMC status and returns the Com Address

Arguments:

  This        - Pointer to IPMI protocol instance
  BmcStatus   - BMC status
  ComAddress  - Com Address

Returns:

  EFI_SUCCESS - Success

--*/
;

EFI_STATUS
IpmiBmcStatus (
  IN  EFI_IPMI_TRANSPORT              *This,
  OUT EFI_BMC_STATUS                  *BmcStatus,
  OUT EFI_SM_COM_ADDRESS              *ComAddress,
  IN  VOID                            *Context
  )
/*++

Routine Description:

  Updates the BMC status and returns the Com Address

Arguments:

  This        - Pointer to IPMI protocol instance
  BmcStatus   - BMC status
  ComAddress  - Com Address
  Context     - Context

Returns:

  EFI_SUCCESS - Success

--*/
;

VOID
GetDeviceSpecificTestResults (
  IN      EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance
  )
/*++

Routine Description:

  This is a BMC specific routine to check the device specific self test results as defined
  in the Bensley BMC core specification.

Arguments:

  IpmiInstance  - Data structure describing BMC variables and used for sending commands

Returns: 
  
  VOID

--*/
;

#endif
