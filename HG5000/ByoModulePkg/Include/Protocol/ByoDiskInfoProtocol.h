
#ifndef __BYO_DISKINFO_PROTOCOL_H__
#define __BYO_DISKINFO_PROTOCOL_H__

#include <Protocol/DevicePath.h>

typedef struct _BYO_DISKINFO_PROTOCOL  BYO_DISKINFO_PROTOCOL;

#define BYO_DISK_INFO_DEV_SATA       1
#define BYO_DISK_INFO_DEV_NVME       2

typedef
EFI_STATUS
(EFIAPI *BYO_DISKINFO_GET_MODEL_NUMBER)(
  IN     BYO_DISKINFO_PROTOCOL         *This,
  IN OUT CHAR8                         *ModelName,
  IN OUT UINTN                         *ModelNameSize
  );

typedef
EFI_STATUS
(EFIAPI *BYO_DISKINFO_GET_SERIAL_NUMBER)(
  IN      BYO_DISKINFO_PROTOCOL         *This,
  IN  OUT CHAR8                         *SerialNumber,
  IN  OUT UINTN                         *SerialNumberSize  
  );  
   
typedef
EFI_STATUS
(EFIAPI *BYO_DISKINFO_GET_DISK_SIZE)(
  IN      BYO_DISKINFO_PROTOCOL         *This,
      OUT UINT64                        *Size
  );   

typedef
EFI_STATUS
(EFIAPI *BYO_DISKINFO_GET_DEVICE_PATH)(
  IN      BYO_DISKINFO_PROTOCOL         *This,
      OUT   EFI_DEVICE_PATH_PROTOCOL    **DevicePath     // caller should free me
  );  

typedef
EFI_STATUS
(EFIAPI *BYO_DISKINFO_GET_PORT_INDEX)(
  IN      BYO_DISKINFO_PROTOCOL         *This,
      OUT UINTN                         *PortIndex  
  );


struct _BYO_DISKINFO_PROTOCOL {
  UINTN                              DevType;
  BYO_DISKINFO_GET_MODEL_NUMBER      GetMn;
  BYO_DISKINFO_GET_SERIAL_NUMBER     GetSn;
  BYO_DISKINFO_GET_DISK_SIZE         GetDiskSize;
  BYO_DISKINFO_GET_DEVICE_PATH       GetDp;
  BYO_DISKINFO_GET_PORT_INDEX        GetPortIndex;
};

extern EFI_GUID gByoDiskInfoProtocolGuid;

#endif
