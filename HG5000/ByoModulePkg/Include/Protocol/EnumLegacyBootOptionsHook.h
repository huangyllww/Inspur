#ifndef __ENUM_LEGACY_BOOT_OPTIONS_HOOK_H__
#define __ENUM_LEGACY_BOOT_OPTIONS_HOOK_H__

#include <UefiBootManagerLoadOption.h>
#include <Protocol/LegacyBios.h>

#define ENUM_LEGACY_BOOT_OPTIONS_HOOK_SIGN     SIGNATURE_32('L', 'B', 'O', 'H')

typedef struct {
  UINT32                          Sign;
  BBS_TABLE                       *BbsTable;
  UINT16                          BbsCount;
  UINT16                          *BbsIndexList;
  UINT16                          *BbsIndexCount;
} ENUM_LEGACY_BOOT_OPTIONS_HOOK_PARAMETER;

extern EFI_GUID gEnumLegacyBootOptionsHookGuid;

#endif
