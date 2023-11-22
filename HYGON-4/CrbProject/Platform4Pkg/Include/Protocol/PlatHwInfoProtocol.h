#ifndef __PLAT_HW_INFO_H__
#define __PLAT_HW_INFO_H__


#define PLAT_HW_INFO_SIGNATURE           SIGNATURE_32('P', 'H', 'W', 'I')


typedef struct {
  CHAR8                      *Manufacturer;
  CHAR8                      *PartNumber;
  CHAR8                      *SerialNumber;
  CHAR8                      *BankLocator;
  CHAR8                      *DeviceLocator;
  CHAR8                      *TypeStr;
  UINTN                      DimmSizeMB;
  UINT8                      Index;
  UINT8                      Socket;
  UINT8                      Channel;
  UINT8                      Dimm;
  UINT8                      DevWidth;
  UINT8                      Ranks;
  UINT8                      NoTrained:1;
  UINT8                      Present:1;
  UINT8                      Ecc:1;
} PLAT_DIMM_INFO;

typedef struct {
  UINT32         Signature;
  UINT16         CpuFreq;
  UINTN          CpuPhySockets;
  UINTN          CpuCount;
  CHAR8          *CpuName;
  UINTN          DimmTotalSizeMB;
  UINTN          MemSpeed;
  UINTN          DimmCount;
  BOOLEAN        IsDimmMixed;
  CHAR8          *FirstDimmManu;
  CHAR8          *FirstDimmPn;
  CHAR8          *FirstDimmTypeStr;
  UINTN          FirstDimmSizeMB;  
  UINTN          DimmInfoCount;
  PLAT_DIMM_INFO *DimmInfo;
} PLAT_HW_INFO;

extern EFI_GUID gPlatformHwInfoReadyProtocolGuid;

#endif
