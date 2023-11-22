/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include <Library/SpiDeviceLib.h>

EFI_STATUS
SpiDeviceWrite (
  IN     UINTN            Address,
  IN     UINT32           Bytes,
  IN     VOID             *Value
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
SpiDeviceRead (
  IN     UINTN            Address,
  IN     UINT32           Bytes,
  OUT    VOID            *Value
  )
{
  return EFI_UNSUPPORTED;
}