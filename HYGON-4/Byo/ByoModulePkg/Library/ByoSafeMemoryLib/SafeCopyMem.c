/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SafeCopyMem.c

Abstract:
  Safe Memory Operation Library CopyMem Instance Implementation File

Revision History:

TIME:
$AUTHOR:
$REVIEWERS:
$SCOPE:
$TECHNICAL:

**/

#include "SafeMemLibInternal.h"

/**
  Copies a source buffer to a destination buffer.

  This function copies MIN(DestMax, Length) bytes from Source to Destination, and returns
  status. The implementation must be reentrant, and it must handle the case where Source overlaps Destination.

  If an error is returned, then the Destination is unmodified.

  @param  Destination              The pointer to the destination buffer of the memory copy.
  @param  DestMax                  The maximum number of destination buffer of the memory copy.
  @param  Source                   The pointer to the source buffer of the memory copy.
  @param  Length                   The number of bytes to copy from Source to Destination.

  @retval RETURN_SUCCESS           memory is copied.
  @retval RETURN_BAD_BUFFER_SIZE   If DestMax is 0.
  @retval RETURN_INVALID_PARAMETER If Destination is NULL.
                                   If Source is NULL.
                                   If MIN (DestMax, Length) is greater than (MAX_ADDRESS - Destination + 1).
                                   If MIN (DestMax, Length) is greater than (MAX_ADDRESS - Source + 1).
  @retval RETURN_ACCESS_DENIED     If Source and Destination overlap.
**/
RETURN_STATUS
EFIAPI
CopyMemBS (
  OUT VOID        *Destination,
  IN  UINTN       DestMax,
  IN  CONST VOID  *Source,
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
  // 2. Neither Destination nor Source shall be a null pointer.
  //
  SAFE_MEMORY_CONSTRAINT_CHECK ((Destination != NULL), RETURN_INVALID_PARAMETER);
  SAFE_MEMORY_CONSTRAINT_CHECK ((Source != NULL), RETURN_INVALID_PARAMETER);

  //
  // 3. DestMax shall not equal zero.
  //
  SAFE_MEMORY_CONSTRAINT_CHECK ((DestMax != 0), RETURN_BAD_BUFFER_SIZE);

  SourceLen = MIN (DestMax, Length);
  //
  // 4. SourceLen shall not be greater than (MAX_ADDRESS - Destination + 1).
  //    SourceLen shall not be greater than (MAX_ADDRESS - Source + 1).
  //
  SAFE_MEMORY_CONSTRAINT_CHECK (((SourceLen - 1) <= (MAX_ADDRESS - (UINTN) Destination)), RETURN_INVALID_PARAMETER);
  SAFE_MEMORY_CONSTRAINT_CHECK (((SourceLen - 1) <= (MAX_ADDRESS - (UINTN) Source)), RETURN_INVALID_PARAMETER);

  //
  // 5. If Destination is equal to Source or neither DestMax nor Length shall be zero, do nothing and return.
  //
  if (Destination == Source) {
    return RETURN_SUCCESS;
  }

  //
  // 6. Copying shall not take place between objects that overlap.
  //
  SAFE_MEMORY_CONSTRAINT_CHECK ((InternalSafeMemoryIsOverlap (Destination, SourceLen, (VOID *) Source, SourceLen) == FALSE), RETURN_ACCESS_DENIED);

  //
  // 7. Copying Source to Destination.
  //
  (VOID) CopyMem (Destination, Source, SourceLen);

  return RETURN_SUCCESS;
}
