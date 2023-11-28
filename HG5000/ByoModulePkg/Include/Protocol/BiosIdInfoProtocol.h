#ifndef __BIOS_ID_INFO_H__
#define __BIOS_ID_INFO_H__

#include <Base.h>
#include <Library/BiosIdLib.h>

#define BIOS_ID_INFO_SIGN     SIGNATURE_32('B', 'I', 'I', 'F')

typedef struct {
  UINT32 Signature;
  CHAR8  BiosVer[32];
  CHAR8  BiosDate[11];
  CHAR8  BoardId[16];     // setup may be required for a special name, so give a enough buffer.
  UINT8  VerMajor;
  UINT8  VerMinor;
  UINT32 Feature;
  UINT32 Ssid;
  CHAR16 RecoveryFilePath[32];
  CHAR16 RecoveryFileName[12+1];
  CHAR16 RecoverySignFileName[12+1];
  BIOS_ID_IMAGE  BiosId;
} BIOS_ID_INFO;

extern EFI_GUID gBiosIdInfoProtocolGuid;
extern EFI_GUID gBiosIdInfoPpiGuid;
extern EFI_GUID gBiosIdHobGuid;

#endif
