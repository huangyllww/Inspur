/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SafeSetMem.c

Abstract:
  Safe Memory Operation Library SetMem Instance Implementation File

Revision History:

TIME:
$AUTHOR:
$REVIEWERS:
$SCOPE:
$TECHNICAL:

**/

#include "SafeMemLibInternal.h"

/**
  Set a destination buffer with value.

  This function set MIN(DestMax, Length) bytes to Destination, and returns
  status.

  If an error is returned, then the Destination is unmodified.

  @param  Destination              The pointer to the destination buffer of the memory set.
  @param  DestMax                  The maximum number of destination buffer of the memory set.
  @param  Value                    The value with which to fill Length bytes of Destination.
  @param  Length                   The number of bytes to set to Destination.

  @retval RETURN_SUCCESS           memory is set.
  @retval RETURN_BAD_BUFFER_SIZE   If DestMax is 0.
  @retval RETURN_INVALID_PARAMETER If Destination is NULL.
                                   If MIN (DestMax, Length) is greater than (MAX_ADDRESS - Destination + 1).
**/
RETURN_STATUS
EFIAPI
SetMemBS (
  OUT VOID        *Destination,
  IN  UINTN       DestMax,
  IN  UINT8       Value,
  IN  UINTN       Length
  )
{
  UINTN           SourceLen;

  //
  // 1. If length equals zero, return successfully.
  //
  if (Length == 0) {
    return RETURN_SUCCESS;
  }

  //
  // 2. Destination shall not be a null pointer.
  //
  SAFE_MEMORY_CONSTRAINT_CHECK ((Destination != NULL), RETURN_INVALID_PARAMETER);

  //
  // 3. DestMax shall not equal zero.
  //
  SAFE_MEMORY_CONSTRAINT_CHECK ((DestMax != 0), RETURN_BAD_BUFFER_SIZE);

  SourceLen = MIN (DestMax, Length);
  //
  // 4. SourceLen shall not be greater than (MAX_ADDRESS - Destination + 1).
  //
  SAFE_MEMORY_CONSTRAINT_CHECK (((SourceLen - 1) <= (MAX_ADDRESS - (UINTN) Destination)), RETURN_INVALID_PARAMETER);

  //
  // 5. Set destination with value.
  //
  (VOID) SetMem (Destination, SourceLen, Value);

  return RETURN_SUCCESS;
}
