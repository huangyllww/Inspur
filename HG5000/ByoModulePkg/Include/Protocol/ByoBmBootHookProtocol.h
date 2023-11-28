
#ifndef __BYO_BM_BOOT_HOOK_PARAMETER_H__
#define __BYO_BM_BOOT_HOOK_PARAMETER_H__

#include <UefiBootManagerLoadOption.h>



#define BYO_BM_BOOT_HOOK_PARAMETER_SIGN     SIGNATURE_32('B', 'T', 'H', 'K')

typedef struct {
  UINT32                                          Sign;
  EFI_BOOT_MANAGER_LOAD_OPTION                    *BootOption;
  EFI_STATUS                                      Status;
} BYO_BM_BOOT_HOOK_PARAMETER;


#endif 

