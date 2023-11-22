/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PeiIpmiBmc.h

Abstract:
  Source file for the IPMI PEIM.

Revision History:

**/

#ifndef _PEI_IPMI_BMC_H_
#define _PEI_IPMI_BMC_H_

#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/IpmiBaseLib.h>

#include <Ppi/IpmiTransportPpi.h>

#include "PeiIpmiBmcDef.h"
#include "PeiKcsBmc.h"


#define BMC_TIMEOUT_PEI      50  // [s] How long shall BIOS wait for BMC
#define BMC_KCS_TIMEOUT      5   // [s] Single KSC request timeout


//
// IPMI Instance signature
//
#define SM_IPMI_BMC_SIGNATURE             SIGNATURE_32 ('i', 'p', 'm', 'i')
#define PEI_IPMI_SEND_COMMAND_MAX_RETRY   2   // Number of retries
#define COMP_INSUFFICIENT_PRIVILEGE       0xD4

#define INSTANCE_FROM_PEI_SM_IPMI_BMC_THIS(a) \
  CR ( \
  a, \
  PEI_IPMI_BMC_INSTANCE_DATA, \
  IpmiTransportPpi, \
  SM_IPMI_BMC_SIGNATURE \
  )

#endif // _PEI_IPMI_BMC_H_
