/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SafePrintLibInternal.h

Abstract:
  Safe Print Operation Library Internal Header File

Revision History:

TIME:
$AUTHOR:
$REVIEWERS:
$SCOPE:
$TECHNICAL:

**/

#ifndef _SAFE_PRINT_LIB_INTERNAL_H_
#define _SAFE_PRINT_LIB_INTERNAL_H_

#include <Base.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>

#ifdef SAFE_PRINT_CONSTRAINT_CHECK
#undef SAFE_PRINT_CONSTRAINT_CHECK
#endif

#define SAFE_PRINT_CONSTRAINT_CHECK(Expression, RetVal)  \
  do { \
    if (!(Expression)) { \
      DEBUG ((DEBUG_VERBOSE, \
        "%a(%d) %a: SAFE_PRINT_CONSTRAINT_CHECK(%a) failed.\n", \
        __FILE__, __LINE__, __FUNCTION__, #Expression)); \
      return RetVal; \
    } \
  } while (FALSE)

#endif
