#ifndef __BYO_CUSTOM_PSTATE_PROTOCOL_H__
#define __BYO_CUSTOM_PSTATE_PROTOCOL_H__

#include <AMD.h>
#include <Library/CcxIdsCustomPstatesLib.h>


#define BYO_CUSTOM_PSTATE_PROTOCOL_GUID = \
  { 0xbd55c5a8, 0x6ffb, 0x45f7, { 0xa2, 0x46, 0x8d, 0x70, 0x8f, 0x54, 0xfd, 0x10 } }

typedef struct _BYO_CUSTOM_PSTATE_PROTOCOL  BYO_CUSTOM_PSTATE_PROTOCOL;


typedef
EFI_STATUS
(EFIAPI *CPU_PSTATE_OEM_UPDATE_TABLE)(
  BYO_CUSTOM_PSTATE_PROTOCOL  *This,
  IDS_CUSTOM_CORE_PSTATE      *CustomPstate
  );


struct _BYO_CUSTOM_PSTATE_PROTOCOL {
  CPU_PSTATE_OEM_UPDATE_TABLE  UpdateTable;
  UINT16                       *CpuPxSpeed;
  UINTN                        CpuPxSpeedCount;
};

extern EFI_GUID gByoCustomPStateProtocolGuid;

#endif
