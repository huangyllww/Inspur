
#ifndef __PXE_CONTROL_VARIABLE_H__
#define __PXE_CONTROL_VARIABLE_H__

#define PXE_CONTROL_VAR_NAME     L"PxeControl"

#define PXE_CONTRO_GUID \
  { \
    0x5ee2c920, 0xd987, 0x4544, { 0xa3, 0xc0, 0xec, 0x84, 0xc0, 0xb, 0xdf, 0x85 } \
  }

#define PXE_CONTROL_GUID PXE_CONTRO_GUID // later use it instead of PXE_CONTRO_GUID

#define PXE_CONTRO_VAR_ATTR   (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)
#define PXE_CONTROL_VAR_ATTR PXE_CONTRO_VAR_ATTR // later use it instead of PXE_CONTRO_VAR_ATTR

//
// Every memory marks disabled GroupType(Hdd, ODD, USB_DISK, USB_ODD, PXE)
//
typedef struct {
  UINT8  WaitSeconds;
  UINT8  DetectMediaTimes;
  UINT8  NetworkEnable;
  UINT8  UefiNetworkStack;
  UINT8  Ipv4Pxe;
  UINT8  Ipv4Https;
  UINT8  Ipv6Pxe;
  UINT8  Ipv6Https;
  UINT8  Reserved[8];
} PXE_CONTROL_VARIABLE;

#endif /* __PXE_CONTROL_VARIABLE_H__ */