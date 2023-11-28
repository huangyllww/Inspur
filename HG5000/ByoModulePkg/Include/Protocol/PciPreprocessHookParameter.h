
#ifndef __PROTOCOL_HOOK_PARAMETER_H__
#define __PROTOCOL_HOOK_PARAMETER_H__

#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciHostBridgeResourceAllocation.h>

extern EFI_GUID gPciPreprocessControllerHookProtocolGuid;


#define PCI_PREPROCESS_HOOK_PARAMETER_SIGN     SIGNATURE_32('P', 'P', 'H', 'P')

typedef struct {
  UINT32                                          Sign;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                 *Io;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS     PciAddress;
  EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE    Phase;
} PCI_PREPROCESS_HOOK_PARAMETER;


#endif 

