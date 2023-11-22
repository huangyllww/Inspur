/** @file

  Copyright (c) 2022, Byosoft Corporation.<BR>
  All rights reserved.This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.

  File Name:
    ByoPcdAccess.c

  Abstract:
    Provide services to access BYO PCD
**/

#include <PiDxe.h>
#include <Protocol/ByoPcdAccess.h>

BOOLEAN
EFIAPI
PcdGetOpalUpdateDataStoreTable (
  )
{
  return PcdGetBool (PcdOpalUpdateDataStoreTable);
}

BOOLEAN
EFIAPI
PcdGetAhciOrPortNameIndexBase0 (
  )
{
  return PcdGetBool(PcdAhciOrPortNameIndexBase0);
}

UINT8
EFIAPI
PcdGetHddPasswordMaxLength (
  )
{
  return PcdGet8 (PcdHddPasswordMaxLength);
}

UINT8
EFIAPI
PcdGetGraphicsConsoleDeltaXPersent (
  )
{
  return PcdGet8 (PcdGraphicsConsoleDeltaXPersent);
}

UINT8
EFIAPI
PcdGetIsPlatformCmosBad (
  )
{
  return PcdGet8 (PcdIsPlatformCmosBad);
}

BYO_PCD_ACCESS_PROTOCOL mPcdAccessProtocol = {
  PcdGetOpalUpdateDataStoreTable,
  PcdGetAhciOrPortNameIndexBase0,
  PcdGetHddPasswordMaxLength,
  PcdGetGraphicsConsoleDeltaXPersent,
  BYO_PCD_ACCESS_PROTOCOL_SIGNATURE,
  5,
  PcdGetIsPlatformCmosBad
};
