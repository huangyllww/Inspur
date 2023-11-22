/** @file

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SmmPlatform.h

Abstract:
  Header file for the Smm platform driver.

Revision History:

**/


#ifndef __SMM_PLATFORM_H__
#define __SMM_PLATFORM_H__


#include <PiSmm.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/TimerLib.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmVariable.h>
#include <Protocol/LegacyUsbInf.h>
#include <Library/PlatformCommLib.h>
#include <Library/PlatformCommLib.h>
#include <ByoStatusCode.h>
#include <Protocol/ByoCommSmiSvcProtocol.h>



EFI_STATUS PlatSmiFlashExInit();

#endif

