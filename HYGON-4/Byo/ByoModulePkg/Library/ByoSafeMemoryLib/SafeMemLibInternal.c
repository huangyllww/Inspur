/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SafeMemLibInternal.c

Abstract:
  Safe Memory Operation Library Internal Implementation File

Revision History:

TIME:
$AUTHOR:
$REVIEWERS:
$SCOPE:
$TECHNICAL:

**/
#include "SafeMemLibInternal.h"

/**
  Returns if 2 memory blocks are overlapped.

  @param  Base1  Base address of 1st memory block.
  @param  Size1  Size of 1st memory block.
  @param  Base2  Base address of 2nd memory block.
  @param  Size2  Size of 2nd memory block.

  @retval TRUE  2 memory blocks are overlapped.
  @retval FALSE 2 memory blocks are not overlapped.
**/
BOOLEAN
EFIAPI
InternalSafeMemoryIsOverlap (
  IN VOID    *Base1,
  IN UINTN   Size1,
  IN VOID    *Base2,
  IN UINTN   Size2
  )
{
  if ((((UINTN)Base1 >= (UINTN)Base2) && ((UINTN)Base1 < (UINTN)Base2 + Size2)) ||
      (((UINTN)Base2 >= (UINTN)Base1) && ((UINTN)Base2 < (UINTN)Base1 + Size1))) {
    return TRUE;
  }

  return FALSE;
}
