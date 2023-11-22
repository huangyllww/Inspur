
#ifndef __PLAT_COMM_LIB_H__
#define __PLAT_COMM_LIB_H__

#include <Uefi.h>


VOID *GetSetupDataHobData(VOID);
VOID *GetCarTopData();

VOID
ConvertIdentifyDataFormat (
  IN  UINT8   *Data,
  IN  UINTN   DataSize
  );


typedef struct {
  UINT8    Socket;                     ///< DIMM Socket ID 0-3
  UINT8    Cdd;                        ///< DIMM CDD ID 0-3
  UINT8    UMCchannel;                 ///< DIMM UMCchannel ID 0-2
  UINT8    Dimm;                       ///< DIMM number 0-1
  UINT8    LogicChannel;               ///< DIMM logic channel ID 0-MAX
  UINT8    Channel;                    ///< DIMM Channel
} BYO_HYGON_DIMM_MAP_DATA;

typedef struct {
  UINT8                   MapDataCount;
  BYO_HYGON_DIMM_MAP_DATA MapData[1];
} BYO_HYGON_DIMM_MAP_INFO;

extern EFI_GUID gHygonDimmMapInfoHobGuid;


#define PCI_HOST_MAX_COUNT         16

#define PLAT_COMM_INFO_SIGN        SIGNATURE_32('P', 'C', 'I', 'F')


#pragma pack(1)

typedef struct {
  UINT64  Signature;
  
  UINT64  Tom2;
  UINT64  UmaBase;
  UINT64  UmaSize;
  UINT64  Pci64Base;
  UINT64  Pci64Size;
  UINT64  Mmio64Base[PCI_HOST_MAX_COUNT];
  UINT64  Mmio64Size[PCI_HOST_MAX_COUNT];
  UINT64  vIOMMUBase[PCI_HOST_MAX_COUNT];
  UINT64  vIOMMUCtrlBase[PCI_HOST_MAX_COUNT];
  UINT64  RasPolicyAddr;
  
  UINT32  ThisSize;
  UINT32  Tolum;
  UINT32  TsegBase;
  UINT32  TsegSize;
  UINT32  S3DataRecord;
  UINT32  S3DataRecordSize;
  UINT32  S3MemoryAddr;
  UINT32  S3MemorySize;
  UINT32  LowMemSize;
  UINT32  PciHostCount;
  UINT32  Sockets;
  UINT32  NbApicId[PCI_HOST_MAX_COUNT];
  UINT32  SbApicId;
  UINT32  NbApicBase[PCI_HOST_MAX_COUNT];
  UINT32  SbApicBase;
  UINT32  GSIBase[PCI_HOST_MAX_COUNT];
  UINT32  NbioMmuBase[PCI_HOST_MAX_COUNT];
  UINT32  MmioBase[PCI_HOST_MAX_COUNT];
  UINT32  MmioSize[PCI_HOST_MAX_COUNT];
  UINT32  AddOnGfxPciId;
  
  UINT16  IoBase[PCI_HOST_MAX_COUNT];
  UINT16  IoSize[PCI_HOST_MAX_COUNT];
  
  UINT8   BusBase[PCI_HOST_MAX_COUNT];
  UINT8   BusLimit[PCI_HOST_MAX_COUNT];
  UINT8   VgaUid;
  UINT8   Csm;                                                // 0xFF: default, unused
  UINT8   IgdSocket;
  UINT8   IgdStartLane;
  UINT8   CpuPhySockets;
  UINT8   LegacyPxeEn;
  UINT8   CpuSmtMode;
  UINT8   CpuModel;
  UINT8   PeiNeedMinMem;

  UINT8   AcpiWakeupSrc;
  UINT8   BmcVersion[3];
  UINT8   BmcIpType;
  EFI_IP_ADDRESS BmcIp;
  
} PLATFORM_COMM_INFO;

#pragma pack()


extern EFI_GUID gEfiPlatformCommInfoGuid;

VOID *GetPlatformCommInfo(VOID);
VOID *GetHygonCbsHobData(VOID);
VOID *GetS3RecordTable();
VOID SetS3RecordTable(UINT32 Address);

BOOLEAN
IsGfxClassCode (
  UINT8  ClassCode[3],
  UINT32 DIDVID
  );

UINTN LibUpdateMBSizeUnit(UINTN SizeMB, CHAR8 *UnitChar);
CHAR8 *LibGetPcieLinkSpeedStr(UINT8 LinkSpeed);



extern EFI_GUID gCarTopDataHobGuid;

#pragma pack(1)

typedef struct {
  UINT32  Bist;
  UINT64  ResetTsc;
  UINT64  JmpSecCoreTsc;
} CAR_TOP_DATA;

#pragma pack()



#define __STR(a)        #a
#define TKN2STR(a)      __STR(a)

#define __TKN2VERSTR(a,b)  #a#b
#define TKN2VERSTR(a,b)    __TKN2VERSTR(a,b)






#define REALTEK_VENDOR_ID            0x10EC
#define HG_VENDOR_ID                 0x1D94
#define ASMEDIA_VENDOR_ID            0x1B21
#define HG_AHCI_PCIID                0x79011D94

#define PLAT_PCI_IO_LIMIT               0xFFFF
#define PLAT_PCI_IO_BASE                0x1000

#define S3_PEI_MEMORY_SIZE             0x100000

#define S3_DATA_RECORD_SIZE            0x1000



/// Standard PCI Config Space registers definitions.
#define PCI_VID_REG                          0x00
#define PCI_DID_REG                          0x02
#define PCI_CMD_REG                          0x04
#define   PCI_CMD_IO_EN                        BIT0
#define   PCI_CMD_MEM_EN                       BIT1
#define   PCI_CMD_BM_EN                        BIT2
#define   PCI_CMD_INT_DIS                      BIT10
#define PCI_PRI_STS_REG                      0x06
#define   PCI_STS_CAP_LIST                     BIT4
#define PCI_REV_ID_REG                       0x08
#define PCI_CC_PI_REG                        0x09
#define PCI_SCC_REG                          0x0A
#define   PCI_SCC_IDE                          0x01
#define   PCI_SCC_AHCI                         0x06
#define   PCI_SCC_RAID                         0x04
#define PCI_BCC_REG                          0x0B
#define   PCI_BCC_STORAGE                      0x01
#define   PCI_BCC_NETWORK                      0x02
#define   PCI_BCC_DISPLAY                      0x03
#define PCI_HDR_REG                          0x0E
#define PCI_BAR0_REG                         0x10
#define PCI_BAR1_REG                         0x14
#define PCI_BAR2_REG                         0x18
#define PCI_BAR3_REG                         0x1C
#define PCI_BAR4_REG                         0x20
#define PCI_BAR5_REG                         0x24
#define PCI_SSID_REG                         0x2C

#define PCI_PBN_REG                          0x18
#define PCI_SCBN_REG                         0x19
#define PCI_SBBN_REG                         0x1A
#define PCI_PBU32_REG                        0x28
#define PCI_PLU32_REG                        0x2C

#define PCI_CAP_POINT_REG                    0x34
#define PCI_EXROM_ADDR_REG                   0x38
#define PCI_INT_LINE_REG                     0x3C

#define PCI_CAP_ID_PM                        0x01



#define CMOS_BAD_REASON_UNKNOWN         0
#define CMOS_BAD_REASON_JUMPER          1
#define CMOS_BAD_REASON_SW_SET          2
#define CMOS_BAD_REASON_SW_UPDATE       3
#define CMOS_BAD_REASON_NO_VARIABLE     4


#define TKN_CMOS_BAD_POLICY_IGNORE                      BIT0            // ignore cmos bad



#define ASPEED_VIDEO_ROM_FILE_GUID \
    {0xf27751d8, 0x67af, 0x48af, { 0xb6, 0xf4, 0x6b, 0x80, 0x8b, 0x76, 0xb5, 0xda}}
    
    
#define ASMEDIA_SATA_ROM_FILE_GUID \
    {0xdee94736, 0xcc97, 0x46d0, { 0xbf, 0x2f, 0xa0, 0x59, 0x00, 0x25, 0xf9, 0x09}}

// {5AD6B7ED-03CD-451B-877F-503108F4596F}
#define VGA_07501BD4_ROM_FILE_GUID \
    {0x5ad6b7ed, 0x03cd, 0x451b, { 0x87, 0x7f, 0x50, 0x31, 0x08, 0xf4, 0x59, 0x6f}}




#endif


