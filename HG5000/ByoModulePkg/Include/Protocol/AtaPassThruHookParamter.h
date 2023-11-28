#ifndef __ATA_PASS_THRU_HOOK_PARAMETER_H__
#define __ATA_PASS_THRU_HOOK_PARAMETER_H__

#include <Protocol/AtaPassThru.h>
#include <Protocol/PciIo.h>

extern EFI_GUID gAtaPassThruHookProtocolGuid;


#define ATA_PASSTHRU_HOOK_PARAMETER_SIGN     SIGNATURE_32('A', 'P', 'T', 'H')

#define ATA_PT_SATA_MODE_IDE      0
#define ATA_PT_SATA_MODE_AHCI     1
#define ATA_PT_SATA_MODE_RAID     2
#define ATA_PT_SATA_MODE_UNKNOWN  0xFF

typedef struct {
  UINT32                          Sign;
  EFI_HANDLE                      Controller;
  EFI_ATA_PASS_THRU_PROTOCOL      *AtaPassThru;
  EFI_PCI_IO_PROTOCOL             *PciIo;
  UINTN                           SataMode;
} ATA_PASSTHRU_HOOK_PARAMETER;

extern EFI_GUID gAtaPassThruHookProtocolGuid;

#endif

