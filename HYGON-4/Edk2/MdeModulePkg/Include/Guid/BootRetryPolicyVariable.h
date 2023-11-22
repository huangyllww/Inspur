#ifndef __BOOT_RETRY_POLICY_VARIABLE_H__
#define __BOOT_RETRY_POLICY_VARIABLE_H__

#define BOOT_RETRY_POLICY_VAR_NAME     L"BootRetryPolicy"

#define BOOT_RETRY_POLICY_GUID \
  { \
    0x4ea97626, 0x6f61, 0x4042, { 0x94, 0xa1, 0x71, 0x30, 0x38, 0xd1, 0xa6, 0x4d } \
  }

#define BOOT_RETRY_POLICY_VAR_ATTR   (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)

//
// Every memory marks disabled GroupType(Hdd, ODD, USB_DISK, USB_ODD, PXE)
//
typedef struct {
  UINT8  GroupRetryTime;
  UINT8  AllRetryTime;
} BOOT_RETRY_POLICY_VARIABLE;

#endif /* __BOOT_RETRY_POLICY_VARIABLE_H__ */