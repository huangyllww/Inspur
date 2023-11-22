/** @file
  Common header file shared by all source files.

  This file includes package header files, dependent library classes.

  Copyright (c) 2022, Byosoft Corporation.<BR>
  All rights reserved.This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.

**/

#ifndef __SPI_DEVICE_LIB_H__
#define __SPI_DEVICE_LIB_H__

#include <Uefi.h>

/**
 * @brief Support SPI device access to write.
 * 
 * @param Address   Write address;
 * @param Bytes     Number of bytes;
 * @param Value     The Value to write.
 * @return EFI_STATUS 
 */
EFI_STATUS
SpiDeviceWrite (
  IN     UINTN            Address,
  IN     UINT32           Bytes,
  IN     VOID             *Value
  );

/**
 * @brief Support SPI device access to read.
 * 
 * @param Address   Read address;
 * @param Bytes     Number of bytes;
 * @param Value     The value that is read.
 * @return EFI_STATUS 
 */
EFI_STATUS
SpiDeviceRead (
  IN     UINTN            Address,
  IN     UINT32           Bytes,
  OUT    VOID             *Value
  );

#endif