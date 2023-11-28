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

#ifndef _SM_SOL_STATUS_DRIVER_H_
#define _SM_SOL_STATUS_DRIVER_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include "EfiServerManagement.h"

#include <Library/IpmiBaseLib.h>
#include <Protocol/IpmiInterfaceProtocol.h>
#include <Protocol/IpmiSolStatusProtocol.h>
#include <BmcConfig.h>

#define SOL_BAUD_9600   6
#define SOL_BAUD_19200  7
#define SOL_BAUD_38400  8
#define SOL_BAUD_57600  9
#define SOL_BAUD_115200 10

#endif
