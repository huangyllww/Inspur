/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SafeMemLib.h

Abstract:
  Safe Memory Operation Library Header File

Revision History:

TIME:
$AUTHOR:
$REVIEWERS:
$SCOPE:
$TECHNICAL:

**/

#ifndef _SAFE_MEM_LIB_H_
#define _SAFE_MEM_LIB_H_

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
  );

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
  );

#endif
