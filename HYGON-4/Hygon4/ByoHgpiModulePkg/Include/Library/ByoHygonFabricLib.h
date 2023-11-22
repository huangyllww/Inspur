#ifndef __BYO_HYGON_FABRIC_LIB_H__
#define __BYO_HYGON_FABRIC_LIB_H__


EFI_STATUS LibGetPxm(EFI_BOOT_SERVICES *BS, UINT32 *Pxm, UINTN *PxmCount);

UINTN
LibGetCpuTdpValue (
    EFI_BOOT_SERVICES *BS
  );

typedef struct {
  UINT32  PciId;
  UINT8   PortBus;
  UINT8   PortDev;
  UINT8   PortFun;
  UINT8   ClassCode[3];
} BYO_PEI_PCIE_INFO;

VOID
LibGetPcieSubDeviceInfo (
  IN     VOID               *pGnbHandle,
  IN     VOID               *pEngine,
  OUT    BYO_PEI_PCIE_INFO  *PcieInfo
  );

VOID LibGetSysPci64Range(UINT64 *Base, UINT64 *Size);

#endif
