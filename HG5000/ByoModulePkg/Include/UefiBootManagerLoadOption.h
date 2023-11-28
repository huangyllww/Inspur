
#ifndef __UEFI_BOOT_MANAGER_LOAD_OPTION_H__
#define __UEFI_BOOT_MANAGER_LOAD_OPTION_H__

#include <Uefi.h>
#include <Protocol/DevicePath.h>


typedef enum {
  LoadOptionTypeBoot,
  LoadOptionTypeDriver,
  LoadOptionTypeMax
} EFI_BOOT_MANAGER_LOAD_OPTION_TYPE;

typedef struct {
  //
  // Data read from UEFI NV variables
  //
  UINTN                             OptionNumber;       // #### numerical value, could be LoadOptionNumberUnassigned
  EFI_BOOT_MANAGER_LOAD_OPTION_TYPE OptionType;         // LoadOptionTypeBoot or LoadOptionTypeDriver
  UINT32                            Attributes;         // Load Option Attributes
  CHAR16                            *Description;       // Load Option Description
  EFI_DEVICE_PATH_PROTOCOL          *FilePath;          // Load Option Device Path
  UINT8                             *OptionalData;      // Load Option optional data to pass into image
  UINT32                            OptionalDataSize;   // Load Option size of OptionalData
  BOOLEAN                           BootNext;           // TRUE if this was a L"BootNext" Variable
  EFI_STATUS                        Status;             // Status returned from boot attempt gBS->StartImage ()
  CHAR16                            *ExitData;          // Exit data returned from gBS->StartImage () 
  UINTN                             ExitDataSize;       // Size of ExitData
  BOOLEAN                           Ignore;
  BOOLEAN                           Legacy;
  BOOLEAN                           Deleted;
  BOOLEAN                           New;
  UINT8                             GroupType;
  EFI_DEVICE_PATH_PROTOCOL          *DupDp;
  UINT32                            ItemOrder;
} EFI_BOOT_MANAGER_LOAD_OPTION;


#endif
