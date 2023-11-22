/** @file
  FAT recovery PEIM entry point, Ppi Functions and FAT Api functions.

Copyright (c) 2006 - 2022, Byosoft Corporation.<BR>

All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include "FatLitePeim.h"
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/BiosIdLib.h>
#include <Library/ByoCommLib.h>
#include <Ppi/ByoBiosSignVerifyPpi.h>

EFI_STATUS
UpdateBlocksAndVolumes (
  IN OUT PEI_FAT_PRIVATE_DATA            *PrivateData,
  IN     BOOLEAN                         BlockIo2
  );

BOOLEAN
EFIAPI 
VerifyBiosSign(
  VOID      *ImageData, 
  UINTN     ImageSize
  );


void CapsuleInit(PEI_FAT_PRIVATE_DATA *PrivateData)
{
  UINTN                 Index;
  EFI_STATUS            Status;
  PEI_FAT_FILE          *FileBuffer;
  UINT8                 NoFile;  
  UINTN                 i;
  

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  if(PrivateData->CapsuleInited){
    return;
  }
  
  UpdateBlocksAndVolumes(PrivateData, TRUE);
  for (Index = 0; Index < PrivateData->VolumeCount; Index++) {
    Status = FindRecoveryFile (PrivateData, Index, &NoFile, &FileBuffer);
    if (!EFI_ERROR(Status)) {
      for (i = 0; i < NoFile; i++) {
        if(PrivateData->CapsuleCount < PEI_FAT_MAX_CAPSULE_NUMBER){
          CopyMem (
            (UINT8*)&PrivateData->CapsuleData[PrivateData->CapsuleCount],
            (UINT8*)&FileBuffer[i],
            sizeof(PEI_FAT_FILE)
            );
          PrivateData->CapsuleCount++;
        }
      }
      break;
    }
  }

  if(PrivateData->CapsuleCount == 0){
    PrivateData->VolumeCount = 0;
    UpdateBlocksAndVolumes(PrivateData, FALSE);
    for (Index = 0; Index < PrivateData->VolumeCount; Index++) {
      Status = FindRecoveryFile (PrivateData, Index, &NoFile, &FileBuffer);
      if (!EFI_ERROR(Status)) {
        for (i = 0; i < NoFile; i++) {
          if(PrivateData->CapsuleCount < PEI_FAT_MAX_CAPSULE_NUMBER){
            CopyMem (
              (UINT8*)&PrivateData->CapsuleData[PrivateData->CapsuleCount],
              (UINT8*)&FileBuffer[i],
              sizeof(PEI_FAT_FILE)
              );
            PrivateData->CapsuleCount++;
          }
        }
      }
    }
  }
  
  PrivateData->CapsuleInited = TRUE;
}



/**
  BlockIo installation notification function. Find out all the current BlockIO
  PPIs in the system and add them into private data. Assume there is

  @param  PeiServices             General purpose services available to every
                                  PEIM.
  @param  NotifyDescriptor        The typedef structure of the notification
                                  descriptor. Not used in this function.
  @param  Ppi                     The typedef structure of the PPI descriptor.
                                  Not used in this function.

  @retval EFI_SUCCESS             The function completed successfully.

**/
EFI_STATUS
EFIAPI
BlockIoNotifyEntry (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );


/**
  Discover all the block I/O devices to find the FAT volume.

  @param  PrivateData             Global memory map for accessing global
                                  variables.
  @param  BlockIo2                Boolean to show whether using BlockIo2 or BlockIo

  @retval EFI_SUCCESS             The function completed successfully.

**/
EFI_STATUS
UpdateBlocksAndVolumes (
  IN OUT PEI_FAT_PRIVATE_DATA            *PrivateData,
  IN     BOOLEAN                         BlockIo2
  )
{
  EFI_STATUS                     Status;
  EFI_PEI_PPI_DESCRIPTOR         *TempPpiDescriptor;
  UINTN                          BlockIoPpiInstance;
  EFI_PEI_RECOVERY_BLOCK_IO_PPI  *BlockIoPpi;
  EFI_PEI_RECOVERY_BLOCK_IO2_PPI *BlockIo2Ppi;
  UINTN                          NumberBlockDevices;
  UINTN                          Index;
  EFI_PEI_BLOCK_IO_MEDIA         Media;
  EFI_PEI_BLOCK_IO2_MEDIA        Media2;
  PEI_FAT_VOLUME                 Volume;
  EFI_PEI_SERVICES               **PeiServices;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();
  BlockIo2Ppi = NULL;
  BlockIoPpi  = NULL;
  //
  // Clean up caches
  //
  PrivateData->BlockDeviceCount = 0;
  for (Index = 0; Index < PEI_FAT_CACHE_SIZE; Index++) {
    PrivateData->CacheBuffer[Index].Valid = FALSE;
  }

  //
  // Find out all Block Io Ppi instances within the system
  // Assuming all device Block Io Peims are dispatched already
  //
  for (BlockIoPpiInstance = 0; BlockIoPpiInstance < PEI_FAT_MAX_BLOCK_IO_PPI; BlockIoPpiInstance++) {
    if (BlockIo2) {
      Status = PeiServicesLocatePpi (
                &gEfiPeiVirtualBlockIo2PpiGuid,
                BlockIoPpiInstance,
                &TempPpiDescriptor,
                (VOID **) &BlockIo2Ppi
                );
    } else {
      Status = PeiServicesLocatePpi (
                &gEfiPeiVirtualBlockIoPpiGuid,
                BlockIoPpiInstance,
                &TempPpiDescriptor,
                (VOID **) &BlockIoPpi
                );
    }
    if (EFI_ERROR (Status)) {
      //
      // Done with all Block Io Ppis
      //
      break;
    }

    if (BlockIo2) {
      Status = BlockIo2Ppi->GetNumberOfBlockDevices (
                              PeiServices,
                              BlockIo2Ppi,
                              &NumberBlockDevices
                              );
    } else {
      Status = BlockIoPpi->GetNumberOfBlockDevices (
                             PeiServices,
                             BlockIoPpi,
                             &NumberBlockDevices
                             );
    }
    if (EFI_ERROR (Status)) {
      continue;
    }

    for (Index = 1; Index <= NumberBlockDevices && PrivateData->BlockDeviceCount < PEI_FAT_MAX_BLOCK_DEVICE; Index++) {

      if (BlockIo2) {
        Status = BlockIo2Ppi->GetBlockDeviceMediaInfo (
                                PeiServices,
                                BlockIo2Ppi,
                                Index,
                                &Media2
                                );
        if (EFI_ERROR (Status) || !Media2.MediaPresent) {
          continue;
        }
        PrivateData->BlockDevice[PrivateData->BlockDeviceCount].BlockIo2        = BlockIo2Ppi;
        PrivateData->BlockDevice[PrivateData->BlockDeviceCount].InterfaceType   = Media2.InterfaceType;
        PrivateData->BlockDevice[PrivateData->BlockDeviceCount].LastBlock       = Media2.LastBlock;
        PrivateData->BlockDevice[PrivateData->BlockDeviceCount].BlockSize       = Media2.BlockSize;
      } else {
        Status = BlockIoPpi->GetBlockDeviceMediaInfo (
                               PeiServices,
                               BlockIoPpi,
                               Index,
                               &Media
                               );
        if (EFI_ERROR (Status) || !Media.MediaPresent) {
          continue;
        }
        PrivateData->BlockDevice[PrivateData->BlockDeviceCount].BlockIo    = BlockIoPpi;
        PrivateData->BlockDevice[PrivateData->BlockDeviceCount].DevType    = Media.DeviceType;
        PrivateData->BlockDevice[PrivateData->BlockDeviceCount].LastBlock  = Media.LastBlock;
        PrivateData->BlockDevice[PrivateData->BlockDeviceCount].BlockSize  = (UINT32) Media.BlockSize;
      }

      PrivateData->BlockDevice[PrivateData->BlockDeviceCount].IoAlign = 0;
      //
      // Not used here
      //
      PrivateData->BlockDevice[PrivateData->BlockDeviceCount].Logical           = FALSE;
      PrivateData->BlockDevice[PrivateData->BlockDeviceCount].PartitionChecked  = FALSE;

      PrivateData->BlockDevice[PrivateData->BlockDeviceCount].PhysicalDevNo     = (UINT8) Index;
      PrivateData->BlockDeviceCount++;
    }
  }
  //
  // Find out all logical devices
  //
  FatFindPartitions (PrivateData);
  DEBUG((EFI_D_INFO, "BlockDeviceCount:%d\n", PrivateData->BlockDeviceCount));

  //
  // Build up file system volume array
  //
  PrivateData->VolumeCount = 0;
  for (Index = 0; Index < PrivateData->BlockDeviceCount; Index++) {
    Volume.BlockDeviceNo  = Index;
    Status                = FatGetBpbInfo (PrivateData, &Volume);
    if (Status == EFI_SUCCESS) {
      //
      // Add the detected volume to the volume array
      //
      CopyMem (
        (UINT8 *) &(PrivateData->Volume[PrivateData->VolumeCount]),
        (UINT8 *) &Volume,
        sizeof (PEI_FAT_VOLUME)
        );
      PrivateData->VolumeCount += 1;
      if (PrivateData->VolumeCount >= PEI_FAT_MAX_VOLUME) {
        break;
      }
    }
  }

  return EFI_SUCCESS;
}


/**
  BlockIo installation notification function. Find out all the current BlockIO
  PPIs in the system and add them into private data. Assume there is

  @param  PeiServices             General purpose services available to every
                                  PEIM.
  @param  NotifyDescriptor        The typedef structure of the notification
                                  descriptor. Not used in this function.
  @param  Ppi                     The typedef structure of the PPI descriptor.
                                  Not used in this function.

  @retval EFI_SUCCESS             The function completed successfully.

**/
/*
EFI_STATUS
EFIAPI
BlockIoNotifyEntry (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  if (CompareGuid (NotifyDescriptor->Guid, &gEfiPeiVirtualBlockIo2PpiGuid)) {
    UpdateBlocksAndVolumes (mPrivateData, TRUE);
  } else {
    UpdateBlocksAndVolumes (mPrivateData, FALSE);
  }
  return EFI_SUCCESS;
}
*/

BYO_BIOS_SIGN_VERIFY_PPI  gByoBiosSignVerifyPpi = {
  VerifyBiosSign
};

CONST EFI_PEI_PPI_DESCRIPTOR gByoBiosSignVerifyPpiList = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gByoBiosSignVerifyPpiGuid,
  &gByoBiosSignVerifyPpi
};


/**
  Installs the Device Recovery Module PPI, Initialize BlockIo Ppi
  installation notification

  @param  FileHandle              Handle of the file being invoked. Type
                                  EFI_PEI_FILE_HANDLE is defined in
                                  FfsFindNextFile().
  @param  PeiServices             Describes the list of possible PEI Services.

  @retval EFI_SUCCESS             The entry point was executed successfully.
  @retval EFI_OUT_OF_RESOURCES    There is no enough memory to complete the
                                  operations.

**/
EFI_STATUS
EFIAPI
FatPeimEntry (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS            Status;
  EFI_PHYSICAL_ADDRESS  Address;
  PEI_FAT_PRIVATE_DATA  *PrivateData;

  Status = PeiServicesRegisterForShadow (FileHandle);
  if (!EFI_ERROR (Status)) {
    return Status;
  }

  Status = PeiServicesAllocatePages (
            EfiBootServicesCode,
            (sizeof (PEI_FAT_PRIVATE_DATA) - 1) / PEI_FAT_MEMORY_PAGE_SIZE + 1,
            &Address
            );
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  PrivateData = (PEI_FAT_PRIVATE_DATA *) (UINTN) Address;

  //
  // Initialize Private Data (to zero, as is required by subsequent operations)
  //
  ZeroMem ((UINT8 *) PrivateData, sizeof (PEI_FAT_PRIVATE_DATA));

  PrivateData->Signature = PEI_FAT_PRIVATE_DATA_SIGNATURE;

  //
  // Installs Ppi
  //
  PrivateData->DeviceRecoveryPpi.GetNumberRecoveryCapsules  = GetNumberRecoveryCapsules;
  PrivateData->DeviceRecoveryPpi.GetRecoveryCapsuleInfo     = GetRecoveryCapsuleInfo;
  PrivateData->DeviceRecoveryPpi.LoadRecoveryCapsule        = LoadRecoveryCapsule;

  PrivateData->PpiDescriptor.Flags                          = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PrivateData->PpiDescriptor.Guid = &gEfiPeiDeviceRecoveryModulePpiGuid;
  PrivateData->PpiDescriptor.Ppi  = &PrivateData->DeviceRecoveryPpi;

  Status = PeiServicesInstallPpi (&PrivateData->PpiDescriptor);
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = PeiServicesInstallPpi(&gByoBiosSignVerifyPpiList);
  ASSERT(!EFI_ERROR(Status));
  
  //
  // Other initializations
  //
//PrivateData->BlockDeviceCount = 0;
//
//UpdateBlocksAndVolumes (PrivateData, TRUE);
//UpdateBlocksAndVolumes (PrivateData, FALSE);

  //
  // PrivateData is allocated now, set it to the module variable
  //
//mPrivateData = PrivateData;

  //
  // Installs Block Io Ppi notification function
  //
/*  
  PrivateData->NotifyDescriptor[0].Flags =
    (
      EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK
    );
  PrivateData->NotifyDescriptor[0].Guid    = &gEfiPeiVirtualBlockIoPpiGuid;
  PrivateData->NotifyDescriptor[0].Notify  = BlockIoNotifyEntry;
  PrivateData->NotifyDescriptor[1].Flags  =
    (
      EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK |
      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST
    );
  PrivateData->NotifyDescriptor[1].Guid    = &gEfiPeiVirtualBlockIo2PpiGuid;
  PrivateData->NotifyDescriptor[1].Notify  = BlockIoNotifyEntry;
  return PeiServicesNotifyPpi (&PrivateData->NotifyDescriptor[0]);
*/

  return Status;
}


/**
  Returns the number of DXE capsules residing on the device.

  This function searches for DXE capsules from the associated device and returns
  the number and maximum size in bytes of the capsules discovered. Entry 1 is
  assumed to be the highest load priority and entry N is assumed to be the lowest
  priority.

  @param[in]  PeiServices              General-purpose services that are available
                                       to every PEIM
  @param[in]  This                     Indicates the EFI_PEI_DEVICE_RECOVERY_MODULE_PPI
                                       instance.
  @param[out] NumberRecoveryCapsules   Pointer to a caller-allocated UINTN. On
                                       output, *NumberRecoveryCapsules contains
                                       the number of recovery capsule images
                                       available for retrieval from this PEIM
                                       instance.

  @retval EFI_SUCCESS        One or more capsules were discovered.
  @retval EFI_DEVICE_ERROR   A device error occurred.
  @retval EFI_NOT_FOUND      A recovery DXE capsule cannot be found.

**/
EFI_STATUS
EFIAPI
GetNumberRecoveryCapsules (
  IN EFI_PEI_SERVICES                               **PeiServices,
  IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI             *This,
  OUT UINTN                                         *NumberRecoveryCapsules
  )
{
  PEI_FAT_PRIVATE_DATA  *PrivateData;

  PrivateData = PEI_FAT_PRIVATE_DATA_FROM_THIS (This);

  CapsuleInit(PrivateData);
  *NumberRecoveryCapsules = PrivateData->CapsuleCount;

  if (*NumberRecoveryCapsules == 0) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}


/**
  Returns the size and type of the requested recovery capsule.

  This function gets the size and type of the capsule specified by CapsuleInstance.

  @param[in]  PeiServices       General-purpose services that are available to every PEIM
  @param[in]  This              Indicates the EFI_PEI_DEVICE_RECOVERY_MODULE_PPI
                                instance.
  @param[in]  CapsuleInstance   Specifies for which capsule instance to retrieve
                                the information.  This parameter must be between
                                one and the value returned by GetNumberRecoveryCapsules()
                                in NumberRecoveryCapsules.
  @param[out] Size              A pointer to a caller-allocated UINTN in which
                                the size of the requested recovery module is
                                returned.
  @param[out] CapsuleType       A pointer to a caller-allocated EFI_GUID in which
                                the type of the requested recovery capsule is
                                returned.  The semantic meaning of the value
                                returned is defined by the implementation.

  @retval EFI_SUCCESS        One or more capsules were discovered.
  @retval EFI_DEVICE_ERROR   A device error occurred.
  @retval EFI_NOT_FOUND      A recovery DXE capsule cannot be found.

**/
EFI_STATUS
EFIAPI
GetRecoveryCapsuleInfo (
  IN  EFI_PEI_SERVICES                              **PeiServices,
  IN  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI            *This,
  IN  UINTN                                         CapsuleInstance,
  OUT UINTN                                         *Size,
  OUT EFI_GUID                                      *CapsuleType
  )
{
  EFI_STATUS            Status;
  PEI_FAT_PRIVATE_DATA  *PrivateData;
  UINTN                 BlockDeviceNo;
  UINTN                 NumberRecoveryCapsules;
  PEI_FAT_FILE          *File;
  

  Status = GetNumberRecoveryCapsules (PeiServices, This, &NumberRecoveryCapsules);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((CapsuleInstance == 0) || (CapsuleInstance > NumberRecoveryCapsules)) {
    return EFI_NOT_FOUND;
  }

  PrivateData = PEI_FAT_PRIVATE_DATA_FROM_THIS (This);
  File = &PrivateData->CapsuleData[CapsuleInstance - 1];
  *Size = (UINTN)File->FileSize;

  BlockDeviceNo = PrivateData->CapsuleData[CapsuleInstance - 1].Volume->BlockDeviceNo;
  while (BlockDeviceNo < PrivateData->BlockDeviceCount && PrivateData->BlockDevice[BlockDeviceNo].Logical) {
    BlockDeviceNo = PrivateData->BlockDevice[BlockDeviceNo].ParentDevNo;
  }

  if (BlockDeviceNo < PrivateData->BlockDeviceCount) {
    if (PrivateData->BlockDevice[BlockDeviceNo].BlockIo2 != NULL) {
      switch (PrivateData->BlockDevice[BlockDeviceNo].InterfaceType) {
      case MSG_ATAPI_DP:
        CopyGuid (CapsuleType, &gRecoveryOnFatIdeDiskGuid);
        break;

      case MSG_USB_DP:
        CopyGuid (CapsuleType, &gRecoveryOnFatUsbDiskGuid);
        break;

      case MSG_NVME_NAMESPACE_DP:
        CopyGuid (CapsuleType, &gRecoveryOnFatNvmeDiskGuid);
        break;

      default:
        break;
      }
    }
    if (PrivateData->BlockDevice[BlockDeviceNo].BlockIo != NULL) {
      switch (PrivateData->BlockDevice[BlockDeviceNo].DevType) {
      case LegacyFloppy:
        CopyGuid (CapsuleType, &gRecoveryOnFatFloppyDiskGuid);
        break;

      case IdeCDROM:
      case IdeLS120:
        CopyGuid (CapsuleType, &gRecoveryOnFatIdeDiskGuid);
        break;

      case UsbMassStorage:
        CopyGuid (CapsuleType, &gRecoveryOnFatUsbDiskGuid);
        break;

      default:
        break;
      }
    }

    return EFI_SUCCESS;    
  }else {
    return EFI_NOT_FOUND;
  }
}


/**
  Loads a DXE capsule from some media into memory.

  This function, by whatever mechanism, retrieves a DXE capsule from some device
  and loads it into memory. Note that the published interface is device neutral.

  @param[in]     PeiServices       General-purpose services that are available
                                   to every PEIM
  @param[in]     This              Indicates the EFI_PEI_DEVICE_RECOVERY_MODULE_PPI
                                   instance.
  @param[in]     CapsuleInstance   Specifies which capsule instance to retrieve.
  @param[out]    Buffer            Specifies a caller-allocated buffer in which
                                   the requested recovery capsule will be returned.

  @retval EFI_SUCCESS        The capsule was loaded correctly.
  @retval EFI_DEVICE_ERROR   A device error occurred.
  @retval EFI_NOT_FOUND      A requested recovery DXE capsule cannot be found.

**/
EFI_STATUS
EFIAPI
LoadRecoveryCapsule (
  IN EFI_PEI_SERVICES                             **PeiServices,
  IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI           *This,
  IN UINTN                                        CapsuleInstance,
  OUT VOID                                        *Buffer
  )
{
  EFI_STATUS            Status;
  PEI_FAT_PRIVATE_DATA  *PrivateData;
  UINTN                 NumberRecoveryCapsules;
  PEI_FAT_FILE          *File;


  Status = GetNumberRecoveryCapsules (PeiServices, This, &NumberRecoveryCapsules);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((CapsuleInstance == 0) || (CapsuleInstance > NumberRecoveryCapsules)) {
    return EFI_NOT_FOUND;
  }

  PrivateData = PEI_FAT_PRIVATE_DATA_FROM_THIS (This);
  File = &PrivateData->CapsuleData[CapsuleInstance - 1];

  if(PrivateData->BiosDataBufferSize && CompareMem(&PrivateData->FatBuffer, File, sizeof(PEI_FAT_FILE)) == 0){
    CopyMem(Buffer, PrivateData->BiosDataBuffer, PrivateData->BiosDataBufferSize);
    DEBUG((EFI_D_INFO, "FAT FILE copy from buffer.\n"));
    Status = EFI_SUCCESS;
  } else {
    Status = FatReadFile (
               PrivateData,
               File,
               File->FileSize,
               Buffer
               );
  }
  return Status;

}

INTN
StrCmpNoCase (
  IN      CONST CHAR16  *FirstString,
  IN      CONST CHAR16  *SecondString
  )
{
  CHAR16  a = 0, b = 0;

  while(1){
    a = CharToUpper(*FirstString);
    b = CharToUpper(*SecondString);

    if(a == 0 || a != b){
      break;
    }
    
    FirstString++;
    SecondString++;

  }  

  return a - b;

}




BOOLEAN
IsFdFile (
  IN PEI_FAT_FILE          *File,
  IN BIOS_ID_INFO          *BiosIdInfo,
  IN PEI_FAT_PRIVATE_DATA  *PrivateData
  )
{
  EFI_STATUS  Status;
  CHAR16      *BiosFileExt;
  CHAR16      FileNameExt[FAT_MAX_FILE_EXTENSION_LENGTH];
  CHAR16      *TargetFileName;
  EFI_PHYSICAL_ADDRESS  MemAddress;
  BIOS_ID_IMAGE         FileBiosId;
  UINTN                 Size;
  UINTN                 Index;
  UINTN                 NameLen;


  DEBUG((EFI_D_INFO, "IsFdFile [%s]\n", File->FileName));

  if(PcdGetBool(PcdByoSecureFlashSupport)){
    if(File->FileSize < PcdGet32(PcdFlashAreaSize)){
      DEBUG((EFI_D_INFO, "FileSize %x not match\n", File->FileSize));
      return FALSE;
    }
  } else {
    if(File->FileSize != PcdGet32(PcdFlashAreaSize)){
      DEBUG((EFI_D_INFO, "FileSize %x not match\n", File->FileSize));
      return FALSE;
    }
  }

  TargetFileName = (CHAR16*)PcdGetPtr(PcdBiosRecoveryFileName);
  if(TargetFileName[0] != 0){     // project has a defined FILE_NAME
    if(StrCmpNoCase(TargetFileName, File->FileName) == 0){
      if(PrivateData->BiosDataBufferSize == 0){
        Status = PeiServicesAllocatePages(
                   EfiBootServicesData, 
                   EFI_SIZE_TO_PAGES(PcdGet32(PcdFlashAreaSize)),
                   &MemAddress
                   );
        if(EFI_ERROR(Status)){
          DEBUG((EFI_D_ERROR, "malloc error\n"));
          return FALSE;
        }
        PrivateData->BiosDataBufferSize = PcdGet32(PcdFlashAreaSize);
        PrivateData->BiosDataBuffer     = (VOID*)(UINTN)MemAddress;
      }

      Status = FatReadFile (
                 PrivateData,
                 File,
                 File->FileSize,
                 PrivateData->BiosDataBuffer
                 );
      if(EFI_ERROR(Status)){
        DEBUG((EFI_D_ERROR, "FatReadFile:%r\n", Status));
        ZeroMem(&PrivateData->FatBuffer, sizeof(PEI_FAT_FILE));
        return FALSE;
      }
      return TRUE;
    } else {
      return FALSE;
    }
  } else if(BiosIdInfo->RecoveryFileName[0]){          // follow my bios file name.
    Size = StrLen(File->FileName);
    if(Size == 0){
      return FALSE;
    }
    for(Index=Size-1;Index;Index--){
      if(File->FileName[Index] == '~'){
        break;
      }
    }
    if(Index == 0){
      Index = Size;
    }

    NameLen = StrLen(BiosIdInfo->RecoveryFileName);
    Index   = MIN(NameLen, Index);
    DEBUG((EFI_D_INFO, "CmpSize:%d\n", Index));
    if(LibStrinCmp(File->FileName, BiosIdInfo->RecoveryFileName, Index)){
      DEBUG((EFI_D_INFO, "FileName not match\n"));
      return FALSE;
      
    } else {
      if(PrivateData->BiosDataBufferSize == 0){
        Status = PeiServicesAllocatePages(
                   EfiBootServicesData, 
                   EFI_SIZE_TO_PAGES(File->FileSize),
                   &MemAddress
                   );
        if(EFI_ERROR(Status)){
          DEBUG((EFI_D_ERROR, "malloc error\n"));
          return FALSE;
        }
        PrivateData->BiosDataBufferSize = File->FileSize;
        PrivateData->BiosDataBuffer     = (VOID*)(UINTN)MemAddress;
      }

      Status = FatReadFile (
                 PrivateData,
                 File,
                 File->FileSize,
                 PrivateData->BiosDataBuffer
                 );
      if(EFI_ERROR(Status)){
        DEBUG((EFI_D_ERROR, "FatReadFile:%r\n", Status));
        ZeroMem(&PrivateData->FatBuffer, sizeof(PEI_FAT_FILE));
        return FALSE;
      }

      if (PcdGet32(PcdFlashFvRecoveryBackUpBase) > 0) {
        Status = GetBiosIdInFv(&FileBiosId, (VOID *)((UINTN)PrivateData->BiosDataBuffer + PcdGet32(PcdFlashFvRecoveryBackUpBase) - PcdGet32(PcdFlashAreaBaseAddress)));
      } else {
        Status = GetBiosIdInAnyFv(&FileBiosId, (VOID*)(UINTN)PrivateData->BiosDataBuffer, File->FileSize);
      }
      if (EFI_ERROR(Status)) {
        DEBUG((EFI_D_ERROR, "GetFileBiosId:%r\n", Status));
        return FALSE;        
      }

      if (StrnCmp (BiosIdInfo->BiosId.BiosIdString.BoardId, 
                   FileBiosId.BiosIdString.BoardId, 
                   OFFSET_OF(BIOS_ID_STRING, VersionMajor)/sizeof(CHAR16))) {
        DEBUG((EFI_D_ERROR, "BiosId not match.Status=%r\n", Status));
        return FALSE;  
      }

      if(PcdGetBool(PcdByoSecureFlashSupport)){
        if(!VerifyBiosSign(PrivateData->BiosDataBuffer, File->FileSize)){
          return FALSE;
        }
      }

    }
  } else {
    ZeroMem (FileNameExt, sizeof(FileNameExt));
    Status = GetFileExt (File->FileName, FileNameExt);
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
    BiosFileExt = (CHAR16 *)PcdGetPtr (PcdBiosFileExt);
    if (!EngStriColl(BiosFileExt, FileNameExt)) {
      return FALSE;
    }
  }

  return TRUE;
}





EFI_STATUS
FindRecoveryFileOld (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  UINTN                 VolumeIndex,
  IN  BIOS_ID_INFO          *BiosIdInfo,
  IN OUT UINT8              *NoFile,
  OUT PEI_FAT_FILE          **FileBuffer
  )
{
  EFI_STATUS               Status;
  PEI_FAT_FILE             Parent;
  PEI_FAT_FILE             *File;
  PEI_FAT_FILE             FatTemp;  
  STATIC PEI_FAT_FILE      Buffer[PEI_FAT_MAX_FILES_IN_VOLUME];


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  File = &PrivateData->File;

  //
  // VolumeIndex must be less than PEI_FAT_MAX_VOLUME because PrivateData->VolumeCount
  // cannot be larger than PEI_FAT_MAX_VOLUME when detecting recovery volume.
  //
  ASSERT (VolumeIndex < PEI_FAT_MAX_VOLUME);

  //
  // Construct root directory file
  //
  ZeroMem (&Parent, sizeof (PEI_FAT_FILE));
  Parent.IsFixedRootDir   = (BOOLEAN) ((PrivateData->Volume[VolumeIndex].FatType == Fat32) ? FALSE : TRUE);
  Parent.Attributes       = FAT_ATTR_DIRECTORY;
  Parent.CurrentPos       = 0;
  Parent.CurrentCluster   = Parent.IsFixedRootDir ? 0 : PrivateData->Volume[VolumeIndex].RootDirCluster;
  Parent.StartingCluster  = Parent.CurrentCluster;
  Parent.Volume           = &PrivateData->Volume[VolumeIndex];

  Status                  = FatSetFilePos (PrivateData, &Parent, 0);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }
  //
  // Search for recovery capsule in root directory
  //
  Status = FatReadNextDirectoryEntry (PrivateData, &Parent, TRUE, File);
  while (Status == EFI_SUCCESS) {

    CopyMem(&FatTemp, File, sizeof(PEI_FAT_FILE));
    if (IsFdFile(File, BiosIdInfo, PrivateData)) {
      CopyMem((UINT8 *)&Buffer[(*NoFile)], (UINT8*)&FatTemp, sizeof(PEI_FAT_FILE));
      CopyMem(&PrivateData->FatBuffer, (UINT8*)&FatTemp, sizeof(PEI_FAT_FILE));
      (*NoFile)++;
    }

    if(*NoFile >= PEI_FAT_MAX_FILES_IN_VOLUME){
      break;
    }

    Status = FatReadNextDirectoryEntry (PrivateData, &Parent, TRUE, File);
  }

  if (EFI_ERROR (Status) && *NoFile == 0) {
    return EFI_NOT_FOUND;
  }

  *FileBuffer = Buffer;

  return EFI_SUCCESS;

}





CHAR16 *
FatGetNextNameComponent (
  IN  CHAR16      *Path,
  OUT CHAR16      *Name
  )
{
  UINTN  Count = 0;
  

  while (*Path != 0 && *Path != L'\\') {
    if(Count < 12){
      *Name++ = *Path++;
    } else {
      Path++;
    }
    Count++;
  }
  *Name = 0;
  //
  // Get off of trailing path name separator
  //
  while (*Path == L'\\') {
    Path++;
  }

  return Path;
}


CHAR16 *BuildTargetPathStr(CHAR16 *Folder, CHAR16 *FileName)
{
  UINTN                 PathSize;
  CHAR16                *p;
  
  PathSize = (StrLen(Folder) + 1 + StrLen(FileName) + 1) * sizeof(CHAR16);
  p = (CHAR16*)AllocatePool(PathSize+2);
  if(p == NULL){
    return NULL;
  }
  UnicodeSPrint(p, PathSize, L"%s\\%s", Folder, FileName);

  return p;
}


EFI_STATUS
FindRecoveryFileNew (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  UINTN                 VolumeIndex,
  IN  BIOS_ID_INFO          *BiosIdInfo,
  IN OUT UINT8              *NoFile,
  OUT PEI_FAT_FILE          **FileBuffer
  )
{
  EFI_STATUS             Status;
  PEI_FAT_FILE           Parent;
  PEI_FAT_FILE           *File;
  STATIC PEI_FAT_FILE    Buffer[2];
  CHAR16                 Name[12+1];
  CHAR16                 *Next;
  BOOLEAN                IsFile;
  CHAR16                 *TargetPathFile = NULL;
  

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  TargetPathFile = BuildTargetPathStr(BiosIdInfo->RecoveryFilePath, BiosIdInfo->RecoveryFileName);
  DEBUG((EFI_D_INFO, "TargetPathFile:%s\n", TargetPathFile));  

  File = &PrivateData->File;

  //
  // VolumeIndex must be less than PEI_FAT_MAX_VOLUME because PrivateData->VolumeCount
  // cannot be larger than PEI_FAT_MAX_VOLUME when detecting recovery volume.
  //
  ASSERT (VolumeIndex < PEI_FAT_MAX_VOLUME);

  //
  // Construct root directory file
  //
  Parent.IsFixedRootDir   = (BOOLEAN) ((PrivateData->Volume[VolumeIndex].FatType == Fat32) ? FALSE : TRUE);
  Parent.Attributes       = FAT_ATTR_DIRECTORY;
  Parent.CurrentPos       = 0;
  Parent.CurrentCluster   = Parent.IsFixedRootDir ? 0 : PrivateData->Volume[VolumeIndex].RootDirCluster;
  Parent.StartingCluster  = Parent.CurrentCluster;
  Parent.Volume           = &PrivateData->Volume[VolumeIndex];
  Status                  = FatSetFilePos (PrivateData, &Parent, 0);
  if (EFI_ERROR (Status)) {
    Status = EFI_DEVICE_ERROR;
    goto ProcExit;
  }

  Next = TargetPathFile;
  if(Next[0] == L'\\'){
    Next++;
  }
  while(1){
    Next = FatGetNextNameComponent(Next, Name);
    if(Name[0] == 0){
      break;
    }

    IsFile = (Next[0] == 0);
    DEBUG((EFI_D_INFO, "Name:[%s], IsFile:%d\n", Name, IsFile));    
    
    while (1) {
      Status = FatReadNextDirectoryEntry(PrivateData, &Parent, IsFile, File);
      if(EFI_ERROR(Status)){
        break;
      }
      
      DEBUG((EFI_D_INFO, "File:[%s]\n", File->FileName));

      if(IsFile){
        if(EngStriColl(Name, File->FileName) || EngStriColl(BiosIdInfo->RecoverySignFileName, File->FileName)){
          if (*NoFile < 2) {
            CopyMem((UINT8 *)&Buffer[(*NoFile)], (UINT8 *)File, sizeof(PEI_FAT_FILE));
            (*NoFile)++;
          } else {
            break;
          }
        }
      } else if(EngStriColl(Name, File->FileName)){
        CopyMem(&Parent, File, sizeof(PEI_FAT_FILE));
        break;
      }
    }
  }


ProcExit:
  if(TargetPathFile != NULL){FreePool(TargetPathFile);}
  if (EFI_ERROR (Status) && *NoFile == 0) {
    return EFI_NOT_FOUND;
  } else {
    *FileBuffer = Buffer;
    DEBUG((EFI_D_INFO, "Recovery Files:%d\n", *NoFile));
    return EFI_SUCCESS;
  }
}




/**
  Finds the recovery file on a FAT volume.
  This function finds the the recovery file named FileName on a specified FAT volume and returns
  its FileHandle pointer.

  @param  PrivateData             Global memory map for accessing global
                                  variables.
  @param  VolumeIndex             The index of the volume.
  @param  Handle                  The output file handle.

  @retval EFI_DEVICE_ERROR        Some error occurred when operating the FAT
                                  volume.
  @retval EFI_NOT_FOUND           The recovery file was not found.
  @retval EFI_SUCCESS             The recovery file was successfully found on the
                                  FAT volume.

**/
EFI_STATUS
FindRecoveryFile (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  UINTN                 VolumeIndex,
  IN OUT UINT8              *NoFile,
  OUT PEI_FAT_FILE          **FileBuffer
  )
{
  EFI_STATUS    Status;
  BIOS_ID_INFO  BiosIdInfo;
  BOOLEAN       IsOemFilePath;
  

  *NoFile = 0;
  
  Status = GetBiosIdInfoFromHob (&BiosIdInfo);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  if(BiosIdInfo.RecoveryFilePath[0] == 0){
    IsOemFilePath = FALSE;
  } else {
    IsOemFilePath = TRUE;
  }

  if(!IsOemFilePath){
    return FindRecoveryFileOld(PrivateData, VolumeIndex, &BiosIdInfo, NoFile, FileBuffer);
  } else {
    return FindRecoveryFileNew(PrivateData, VolumeIndex, &BiosIdInfo, NoFile, FileBuffer);
  }
}

