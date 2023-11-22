/** @file

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ByoGuidValueToName.c

Abstract:

Revision History:

**/
#include <Uefi.h>
#include <Library/PrintLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>

extern PRINT_GUID_VALUE_TO_NAME gPrintGuidNameFunc;
extern CHAR8 *gNameArray[];
extern CHAR8 *gGuidArray[];
extern UINTN gTableNumber;

CHAR8 *ByoGuidValueToName (
  CHAR8 *Value
  )
{
  UINTN Index = 0;

  for (Index = 0; Index < gTableNumber; Index++) {
    if (CompareMem(Value, gGuidArray[Index], AsciiStrSize(Value)) == 0) {
      return gNameArray[Index];
    }
  }
  return NULL;
}

EFI_STATUS
EFIAPI
ByoGuidValueToNameLibConstructor (
  VOID
  )
{
  gPrintGuidNameFunc = ByoGuidValueToName;
  return EFI_SUCCESS;
}
