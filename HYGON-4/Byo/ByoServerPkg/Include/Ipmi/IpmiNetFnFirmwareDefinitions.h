/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  IpmiNetFnFirmwareDefinitions.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef _IPMINETFNFIRMWAREDEFINITIONS_H_
#define _IPMINETFNFIRMWAREDEFINITIONS_H_

//
// Net function definition for Firmware command
//
#define EFI_SM_NETFN_FIRMWARE 0x08

//
// All Firmware commands and their structure definitions to follow here
//

/*----------------------------------------------------------------------------------------
    Definitions for Get BMC Execution Context
----------------------------------------------------------------------------------------*/
#define EFI_FIRMWARE_GET_BMC_EXECUTION_CONTEXT  0x23

//
//  Constants and Structure definitions for "Get Device ID" command to follow here
//
typedef struct {
  UINT8   CurrentExecutionContext;
  UINT8   PartitionPointer;
} EFI_IPMI_MSG_GET_BMC_EXEC_RSP;

//
// Current Execution Context responses
//
#define EFI_FIRMWARE_BMC_IN_FULL_RUNTIME        0x10
#define EFI_FIRMWARE_BMC_IN_FORCED_UPDATE_MODE  0x11


#endif

