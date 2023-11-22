
#ifndef _BYO_CREATE_POP_UP_H_
#define _BYO_CREATE_POP_UP_H_

#include <Uefi.h>
#include <Protocol/SimpleTextIn.h>

#define BYO_CREATE_POP_UP_PROTOCOL_GUID \
  { \
    0x5e65c586, 0xec74, 0x406f, { 0x9c, 0xae, 0x7e, 0xda, 0xce, 0x6f, 0x9c, 0x6f } \
  }

typedef struct _BYO_CREATE_POP_UP_PROTOCOL  BYO_CREATE_POP_UP_PROTOCOL;


typedef
VOID
(EFIAPI *BYO_CREATE_POP_UP)(
  IN  UINTN          Attribute,
  OUT EFI_INPUT_KEY  *Key,
  IN  VA_LIST        Marker
  );

struct _BYO_CREATE_POP_UP_PROTOCOL {
  BYO_CREATE_POP_UP        CreatePopUp;
};

extern EFI_GUID gByoCreatePopUpProtocolGuid;

#endif
