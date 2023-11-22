/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  TcmPlatformDataStruc.h

Abstract:
  Part of TCM Module.

Revision History:

  1. Tcm module init version.
     Support setup function.
$END--------------------------------------------------------------------

**/



#ifndef __TCM_PLATFORM_DATA_STRUC__
#define __TCM_PLATFORM_DATA_STRUC__

#pragma pack(1)
typedef struct {
  UINT8   TcmEnableCmd;
  UINT8   TcmPendingOperationCmd;
} TCM_SETUP_CONFIG;

#pragma pack()

#endif      // __TCM_PLATFORM_DATA_STRUC__

