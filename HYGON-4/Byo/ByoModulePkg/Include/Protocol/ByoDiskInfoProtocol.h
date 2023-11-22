/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_DISKINFO_PROTOCOL_H__
#define __BYO_DISKINFO_PROTOCOL_H__


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
(EFIAPI *BYO_DISKINFO_GET_FIRMWARE_REVISION)(
  IN     BYO_DISKINFO_PROTOCOL         *This,
  IN OUT CHAR8                         *FirmwareRevision,
  IN OUT UINTN                         *FirmwareRevisionSize
  );

struct _BYO_DISKINFO_PROTOCOL {
  UINTN                              DevType;
  BYO_DISKINFO_GET_MODEL_NUMBER      GetMn;
  BYO_DISKINFO_GET_SERIAL_NUMBER     GetSn;
  BYO_DISKINFO_GET_DISK_SIZE         GetDiskSize;
  BYO_DISKINFO_GET_FIRMWARE_REVISION GetFr;
};

extern EFI_GUID gByoDiskInfoProtocolGuid;

#endif
