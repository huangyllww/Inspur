/** @file

Copyright (c) 2006 - 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  UefiVariableDefaultHob.h

Revision History:

**/

#ifndef __UEFI_VARIABLE_DEFAULT_H__
#define __UEFI_VARIABLE_DEFAULT_H__

#include <Uefi.h>

typedef struct {
  UINT16 DefaultId;
  UINT16 BoardId;
} DEFAULT_INFO;

typedef struct {
  UINT16 Offset;
  UINT8  Value;
} DATA_DELTA;

typedef struct {
  //
  // HeaderSize includes HeaderSize fields and DefaultInfo arrays
  //
  UINT16 HeaderSize;
  //
  // DefaultInfo arrays those have the same default setting.
  //
  DEFAULT_INFO DefaultInfo[1];
  //
  // Default data is stored as variable storage or the array of DATA_DELTA.
  //
} DEFAULT_DATA;

#endif
