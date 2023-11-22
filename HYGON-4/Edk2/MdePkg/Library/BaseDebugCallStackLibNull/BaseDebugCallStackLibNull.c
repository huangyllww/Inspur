/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/
#include <Base.h>

/**
  Dump the current call stack trace information by specified depth.
 
  @param[in]    Depth   Call stack depth to be dumped.
                        0 : Non-limited depth value
                        others: The specified depth value
**/
VOID
EFIAPI
ShowCallStack (
  IN UINTN           Depth
  )
{
}

/**
  Dump the call stack trace information by specified PC/FP and depth
 
  @param[in]    Pc      PC pointer.
  @param[in]    Fp      Frame pointer.
  @param[in]    Depth   Call stack depth to be dumped.
                        0 : Non-limited depth value
                        others: The specified depth value
**/
VOID
EFIAPI
ShowCallStackByPcFp (
  IN UINTN           Pc,
  IN UINTN           Fp,
  IN UINTN           Depth
  )
{
}

/**
  Get the function return address by specified PC/FP.
 
  @param[in]    Pc      PC pointer.
  @param[in]    Fp      Frame pointer.

  @return    Function return address
**/
UINTN
EFIAPI
GetReturnAddressByPcFp (
  IN UINTN           Pc,
  IN UINTN           Fp
  )
{
  return 0;
}
