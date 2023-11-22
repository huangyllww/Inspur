
#ifndef __EFI_POST_LOGIN_PROTOCOL_H__
#define __EFI_POST_LOGIN_PROTOCOL_H__

#define EFI_POST_LOGIN_PROTOCOL_GUID \
  { 0xa8825e56, 0x1552, 0x4a67, { 0x96, 0x79, 0xbf, 0xf5, 0xd9, 0x32, 0x12, 0x20 } }


typedef 
EFI_STATUS
(EFIAPI *EFI_CHECK_SYS_PASSWORD) (
  IN BOOLEAN   BootSetup
  );

typedef struct {
  EFI_CHECK_SYS_PASSWORD  CheckSysPassword;
} EFI_POST_LOGIN_PROTOCOL;
  
  
  
extern EFI_GUID gEfiPostLoginProtocolGuid;

#endif