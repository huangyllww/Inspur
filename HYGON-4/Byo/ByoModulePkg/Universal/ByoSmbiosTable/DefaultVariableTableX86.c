/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Protocol/Smbios.h>
#include <Protocol/PciIo.h>
#include <Protocol/PlatformUsbDeviceSupport.h>
#include <IndustryStandard/SmBios.h>
#include <ByoSmbiosTable.h>
#include <Protocol/ByoPcdAccess.h>
#include <ByoDevicePatch.h>
#include <Library/ByoCommLib.h>
#include <Guid/SmmVariableCommon.h>


EFI_STATUS
AddDefaultVariableTable(
  SMBIOS_DEFAULT_VARIABLE_HOB_TABLE    *DefaultVariableDataTable
)
{
  return EFI_UNSUPPORTED;
}