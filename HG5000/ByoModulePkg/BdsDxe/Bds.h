
#ifndef _BDS_MODULE_H_
#define _BDS_MODULE_H_

#include <Uefi.h>
#include <Guid/GlobalVariable.h>
#include <Protocol/Bds.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/LoadedImage.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/PerformanceLib.h>
#include <Library/ByoUefiBootManagerLib.h>
#include "PlatformBootManagerLib.h"
#include <Library/ByoCommLib.h>
#include <Protocol/BdsBootManagerProtocol.h>


//
// Prototypes
//

/**

  Service routine for BdsInstance->Entry(). Devices are connected, the
  consoles are initialized, and the boot options are tried.

  @param This            Protocol Instance structure.

**/
VOID
EFIAPI
BdsEntry (
  IN  EFI_BDS_ARCH_PROTOCOL *This
  );

typedef
VOID
(* BDS_PROCESS_VARIABLE) (
  VOID  **Variable,
  UINTN *VariableSize
  );

extern EFI_HII_HANDLE gHiiHandle;


EFI_STATUS
EFIAPI
ByoDrawPasswordDialog (
  IN OUT CHAR16                      *NewPassword,
  IN     UINTN                       PasswordSize,
  IN     BYO_DRAW_PASSWORD_CALLBACK  Callback,
  IN     CHAR16                      *Title,
  OUT    UINTN                       *Reason
  );

EFI_STATUS
ByoDrawPostPasswordDialog (
  IN  CHAR16                     *TitleStr,
  IN  CHAR16                     *Prompt,
  OUT CHAR16                     *Password,
  IN  UINTN                      PasswordSize
  );

VOID
ByoPrintPostString (
  IN UINTN     Row,
  IN UINTN     Column,
  IN CHAR16    *String,
  IN UINTN     Attribute
  );



#endif
