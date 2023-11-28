/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  WheaElog.c

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef _SMM_BMCELOG_H_
#define _SMM_BMCELOG_H_

//
// Statements that include other files
//
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/SmmLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/ReportStatusCodeLib.h>

#include <EfiServerManagement.h>

#include <Library/IpmiBaseLib.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <Protocol/GenericElog.h>
#include <EfiSmStatusCodes.h>

//#include EFI_GUID_DEFINITION (StatusCode)

#define MAX_TEMP_DATA           160
#define SEL_RESERVE_ID_SUPPORT  0x02

//
// BMC Elog instance data
//
typedef struct {
  UINTN                       Signature;
  EFI_SM_COM_ADDRESS          ControllerAddress;
  EFI_SM_COM_ADDRESS          TargetAddress;
  UINT16                      Instance;
  EFI_SM_ELOG_TYPE            DataType;
  UINT8                       TempData[MAX_TEMP_DATA + 1];
  EFI_SM_ELOG_REDIR_PROTOCOL  BmcElog;
} EFI_BMC_ELOG_INSTANCE_DATA;


//
// BMC Elog Instance signature
//
#define SM_ELOG_REDIR_SIGNATURE                 SIGNATURE_32 ('e', 'l', 'o', 'f')

#define INSTANCE_FROM_EFI_SM_ELOG_REDIR_THIS(a) CR (a, EFI_BMC_ELOG_INSTANCE_DATA, BmcElog, SM_ELOG_REDIR_SIGNATURE)

#endif
