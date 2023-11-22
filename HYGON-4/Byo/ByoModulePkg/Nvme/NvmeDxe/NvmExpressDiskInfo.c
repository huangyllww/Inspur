/** @file
  This file is used to implement the EFI_DISK_INFO_PROTOCOL interface..

  Copyright (c) 2013 - 2022, Byosoft Corporation. All rights reserved.<BR>
  This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.


**/

#include "NvmExpress.h"

EFI_DISK_INFO_PROTOCOL gNvmExpressDiskInfoProtocolTemplate = {
  EFI_DISK_INFO_NVME_INTERFACE_GUID,
  NvmExpressDiskInfoInquiry,
  NvmExpressDiskInfoIdentify,
  NvmExpressDiskInfoSenseData,
  NvmExpressDiskInfoWhichIde
};




EFI_STATUS
EFIAPI
NvmeByoDiskInfoGetMn (
  IN     BYO_DISKINFO_PROTOCOL         *This,
  IN OUT CHAR8                         *ModelName,
  IN OUT UINTN                         *ModelNameSize
  )
{
  NVME_DEVICE_PRIVATE_DATA        *Device;
  UINTN                           Size;

  if ((ModelNameSize == NULL) || (*ModelNameSize > 0 && ModelName == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Device = NVME_DEVICE_PRIVATE_DATA_FROM_BYO_DISK_INFO(This);
  Size = AsciiStrSize(Device->Controller->Mn);
  if(*ModelNameSize < Size){
    *ModelNameSize = Size;
    return EFI_BUFFER_TOO_SMALL;
  }

  *ModelNameSize = Size;
  AsciiStrCpyS(ModelName, *ModelNameSize/sizeof(CHAR8), Device->Controller->Mn);
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
NvmeByoDiskInfoGetSn (
  IN      BYO_DISKINFO_PROTOCOL         *This,
  IN  OUT CHAR8                         *SerialNumber,
  IN  OUT UINTN                         *SerialNumberSize  
  )
{
  NVME_DEVICE_PRIVATE_DATA        *Device;
  UINTN                           Size;

  if ((SerialNumberSize == NULL) || (*SerialNumberSize > 0 && SerialNumber == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Device = NVME_DEVICE_PRIVATE_DATA_FROM_BYO_DISK_INFO(This);
  Size = AsciiStrSize(Device->Controller->Sn);
  if(*SerialNumberSize < Size){
    *SerialNumberSize = Size;
    return EFI_BUFFER_TOO_SMALL;
  }

  *SerialNumberSize = Size;
  AsciiStrCpyS(SerialNumber, *SerialNumberSize/sizeof(CHAR8), Device->Controller->Sn);  
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
NvmeByoDiskInfoGetFr (
  IN     BYO_DISKINFO_PROTOCOL         *This,
  IN OUT CHAR8                         *FirmwareRevision,
  IN OUT UINTN                         *FirmwareRevisionSize
  )
{
  NVME_DEVICE_PRIVATE_DATA        *Device;
  UINTN                           Size;

  if ((FirmwareRevisionSize == NULL) || (*FirmwareRevisionSize > 0 && FirmwareRevision == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Device = NVME_DEVICE_PRIVATE_DATA_FROM_BYO_DISK_INFO(This);
  Size = AsciiStrSize(Device->Controller->Fr);
  if(*FirmwareRevisionSize < Size){
    *FirmwareRevisionSize = Size;
    return EFI_BUFFER_TOO_SMALL;
  }

  *FirmwareRevisionSize = Size;
  AsciiStrCpyS(FirmwareRevision, *FirmwareRevisionSize/sizeof(CHAR8), Device->Controller->Fr);
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
NvmeByoDiskInfoGetDiskSize (
  IN      BYO_DISKINFO_PROTOCOL         *This,
      OUT UINT64                        *Size
  )
{
  NVME_DEVICE_PRIVATE_DATA        *Device;

  if(Size == NULL || This == NULL){
    return EFI_INVALID_PARAMETER;
  }
  
  Device = NVME_DEVICE_PRIVATE_DATA_FROM_BYO_DISK_INFO(This);
  *Size = MultU64x32(Device->Media.LastBlock+1, Device->Media.BlockSize);
  return EFI_SUCCESS;
}


BYO_DISKINFO_PROTOCOL gNvmeByoDiskInfoProtocolTemplate = {
  BYO_DISK_INFO_DEV_NVME,
  NvmeByoDiskInfoGetMn,
  NvmeByoDiskInfoGetSn,
  NvmeByoDiskInfoGetDiskSize,
  NvmeByoDiskInfoGetFr
};




/**
  Initialize the installation of DiskInfo protocol.

  This function prepares for the installation of DiskInfo protocol on the child handle.
  By default, it installs DiskInfo protocol with NVME interface GUID.

  @param[in]  Device  The pointer of NVME_DEVICE_PRIVATE_DATA.

**/
VOID
InitializeDiskInfo (
  IN  NVME_DEVICE_PRIVATE_DATA    *Device
  )
{
  CopyMem(&Device->DiskInfo,    &gNvmExpressDiskInfoProtocolTemplate, sizeof(EFI_DISK_INFO_PROTOCOL));
  CopyMem(&Device->ByoDiskInfo, &gNvmeByoDiskInfoProtocolTemplate,    sizeof(BYO_DISKINFO_PROTOCOL));  
}


/**
  Provides inquiry information for the controller type.

  This function is used to get inquiry data.  Data format
  of Identify data is defined by the Interface GUID.

  @param[in]      This              Pointer to the EFI_DISK_INFO_PROTOCOL instance.
  @param[in, out] InquiryData       Pointer to a buffer for the inquiry data.
  @param[in, out] InquiryDataSize   Pointer to the value for the inquiry data size.

  @retval EFI_SUCCESS            The command was accepted without any errors.
  @retval EFI_NOT_FOUND          Device does not support this data class
  @retval EFI_DEVICE_ERROR       Error reading InquiryData from device
  @retval EFI_BUFFER_TOO_SMALL   InquiryDataSize not big enough

**/
EFI_STATUS
EFIAPI
NvmExpressDiskInfoInquiry (
  IN     EFI_DISK_INFO_PROTOCOL   *This,
  IN OUT VOID                     *InquiryData,
  IN OUT UINT32                   *InquiryDataSize
  )
{
  return EFI_NOT_FOUND;
}


/**
  Provides identify information for the controller type.

  This function is used to get identify data.  Data format
  of Identify data is defined by the Interface GUID.

  @param[in]      This              Pointer to the EFI_DISK_INFO_PROTOCOL
                                    instance.
  @param[in, out] IdentifyData      Pointer to a buffer for the identify data.
  @param[in, out] IdentifyDataSize  Pointer to the value for the identify data
                                    size.

  @retval EFI_SUCCESS            The command was accepted without any errors.
  @retval EFI_NOT_FOUND          Device does not support this data class
  @retval EFI_DEVICE_ERROR       Error reading IdentifyData from device
  @retval EFI_BUFFER_TOO_SMALL   IdentifyDataSize not big enough

**/
EFI_STATUS
EFIAPI
NvmExpressDiskInfoIdentify (
  IN     EFI_DISK_INFO_PROTOCOL   *This,
  IN OUT VOID                     *IdentifyData,
  IN OUT UINT32                   *IdentifyDataSize
  )
{
  EFI_STATUS                      Status;
  NVME_DEVICE_PRIVATE_DATA        *Device;

  Device = NVME_DEVICE_PRIVATE_DATA_FROM_DISK_INFO (This);

  Status = EFI_BUFFER_TOO_SMALL;
  if (*IdentifyDataSize >= sizeof (Device->NamespaceData)) {
    Status = EFI_SUCCESS;
    CopyMem (IdentifyData, &Device->NamespaceData, sizeof (Device->NamespaceData));
  }
  *IdentifyDataSize = sizeof (Device->NamespaceData);
  return Status;
}

/**
  Provides sense data information for the controller type.

  This function is used to get sense data.
  Data format of Sense data is defined by the Interface GUID.

  @param[in]      This              Pointer to the EFI_DISK_INFO_PROTOCOL instance.
  @param[in, out] SenseData         Pointer to the SenseData.
  @param[in, out] SenseDataSize     Size of SenseData in bytes.
  @param[out]     SenseDataNumber   Pointer to the value for the sense data size.

  @retval EFI_SUCCESS            The command was accepted without any errors.
  @retval EFI_NOT_FOUND          Device does not support this data class.
  @retval EFI_DEVICE_ERROR       Error reading SenseData from device.
  @retval EFI_BUFFER_TOO_SMALL   SenseDataSize not big enough.

**/
EFI_STATUS
EFIAPI
NvmExpressDiskInfoSenseData (
  IN     EFI_DISK_INFO_PROTOCOL   *This,
  IN OUT VOID                     *SenseData,
  IN OUT UINT32                   *SenseDataSize,
  OUT    UINT8                    *SenseDataNumber
  )
{
  return EFI_NOT_FOUND;
}


/**
  This function is used to get controller information.

  @param[in]  This         Pointer to the EFI_DISK_INFO_PROTOCOL instance.
  @param[out] IdeChannel   Pointer to the Ide Channel number.  Primary or secondary.
  @param[out] IdeDevice    Pointer to the Ide Device number.  Master or slave.

  @retval EFI_SUCCESS       IdeChannel and IdeDevice are valid.
  @retval EFI_UNSUPPORTED   This is not an IDE device.

**/
EFI_STATUS
EFIAPI
NvmExpressDiskInfoWhichIde (
  IN  EFI_DISK_INFO_PROTOCOL   *This,
  OUT UINT32                   *IdeChannel,
  OUT UINT32                   *IdeDevice
  )
{
  return EFI_UNSUPPORTED;
}

