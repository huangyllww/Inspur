#ifndef __HYGON_HSIO_PCIE_INFO_H__
#define __HYGON_HSIO_PCIE_INFO_H__

extern EFI_GUID gHygonHsioInfoHobGuid;


#pragma pack(1)

typedef struct {
  UINT8                     Present;
  UINT8                     Socket;
  UINT8                     StartLane;
  UINT8                     EndLane;
  UINT8                     EngineType;
  UINT8                     BuildIn;
  UINT8                     SlotLength;
  UINT16                    SlotId;
  CHAR8                     Name[32];
} HSIO_PCIE_CTX;

typedef struct {
  UINT8            GpioType;
  UINT8            CtxCount;
  HSIO_PCIE_CTX    Ctx[1];
} HSIO_PCIE_CTX_INFO;

#pragma pack()


#define PCIE_BUILD_IN_NONE            0
#define PCIE_BUILD_IN_IGD             1
#define PCIE_BUILD_IN_LAN             2
#define PCIE_BUILD_IN_SATA            3
#define PCIE_BUILD_IN_NVME            4

#define PCIE_SLOT_LENGTH_OTHER        1
#define PCIE_SLOT_LENGTH_UNKNOWN      2
#define PCIE_SLOT_LENGTH_SHORT        3
#define PCIE_SLOT_LENGTH_LONG         4
#define PCIE_SLOT_LENGTH_NONE         0xFF

#define DXIO_DEV_TYPE_PCIE            0
#define DXIO_DEV_TYPE_ILAN            1
#define DXIO_DEV_TYPE_SATA            2



extern EFI_GUID gHygonSetupSlotDevInfoProtoclGuid;

typedef struct {
  CHAR8         *SlotName;
  UINT32        DevPciId;
  UINT8         DevClassCode[3];
  UINT8         FuncCount;
  UINT32        LinkStatus;
  UINT32        LinkCap;
  UINT32        LinkCap2;
  UINT32        DevCtrl;
  UINT32        LinkCtrl;
  UINT8         CurLinkSpeed;    // 1, 2, 4, 8, 16
  UINT8         CurLinkWidth;
  UINT8         MaxLinkSpeed;    // 1, 2, 4, 8, 16
  UINT8         MaxLinkWidth;  
} SETUP_SLOT_INFO;

typedef struct {
  UINTN            DevCount;
  SETUP_SLOT_INFO  Info[1];
} SETUP_SLOT_INFO_DATA;

#endif

