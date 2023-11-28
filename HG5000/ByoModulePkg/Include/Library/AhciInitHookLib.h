#ifndef __AHCI_INIT_HOOK_LIB_H__
#define __AHCI_INIT_HOOK_LIB_H__

#include <Base.h>
#include <Protocol/PciIo.h>

typedef  
VOID
(EFIAPI *AHCI_INIT_HOOK_POD_AFTER) (
    EFI_PCI_IO_PROTOCOL              *PciIo,
    UINT8                            Port
  );

extern AHCI_INIT_HOOK_POD_AFTER gAhciInitHookPodAfter;  
  
#endif
