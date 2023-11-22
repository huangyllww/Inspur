/** @file
    TPM Platform Hierarchy configuration library.

    This library provides functions for customizing the TPM's Platform Hierarchy
    Authorization Value (platformAuth) and Platform Hierarchy Authorization
    Policy (platformPolicy) can be defined through this function.

    Copyright (c) 2019 - 2022, Byosoft Corporation. All rights reserved.<BR>

    This software and associated documentation (if any)
    is furnished under a license and may only be used or copied in
    accordance with the terms of the license. Except as permitted by such
    license, no part of this software or documentation may be reproduced,
    stored in a retrieval system, or transmitted in any form or by any
    means without the express written consent of Byosoft Corporation.


**/

#ifndef _TPM_PLATFORM_HIERARCHY_LIB_H_
#define _TPM_PLATFORM_HIERARCHY_LIB_H_

#include <PiDxe.h>
#include <Uefi.h>

/**
   This service will perform the TPM Platform Hierarchy configuration at the SmmReadyToLock event.

**/
VOID
EFIAPI
ConfigureTpmPlatformHierarchy (
  VOID
  );

#endif
