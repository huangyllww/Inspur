/** @file

  Copyright (c) 2020, Byosoft Corporation.<BR>
  All rights reserved.This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.

  File Name:
    SecureBootRestoreLib.h

  Abstract:
    SecureBoot Policy Restoration Library Private header file

  Revision History:

  TIME:
  $AUTHOR:
  $REVIEWERS:
  $SCOPE:
  $TECHNICAL:


  T.O.D.O

  $END

**/

#ifndef __SECUREBOOT_RESTORE_LIB_H__
#define __SECUREBOOT_RESTORE_LIB_H__

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseCryptLib.h>
#include <Library/DxeServicesLib.h>
#include <Guid/GlobalVariable.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/AuthenticatedVariableFormat.h>
#include <Protocol/FirmwareVolume2.h>
#include <Library/PcdLib.h>
#endif
