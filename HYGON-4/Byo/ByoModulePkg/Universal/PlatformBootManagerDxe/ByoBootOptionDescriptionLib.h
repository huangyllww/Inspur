/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ByoBootOptionDescriptionLib.h

Abstract:

Revision History:

**/
#ifndef __BYO_BOOT_OPTION_DESCRIPTION_LIB_H_
#define __BYO_BOOT_OPTION_DESCRIPTION_LIB_H_

#include <Protocol/PciIo.h>

/**
  Return the UEFI boot description for the option.

  @param DevicePath                DevicePath of Option.

  @return  The description string.
**/
CHAR16 *
EFIAPI
ByoGetUEFIBootDescription (
  IN EFI_DEVICE_PATH_PROTOCOL        *DevicePath
  );

#endif
