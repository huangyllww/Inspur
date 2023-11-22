/** @file
  A shell application that triggers capsule update process.

  Copyright (c) 2016 - 2022, Byosoft Corporation. All rights reserved.<BR>
  This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/FileHandleLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/UsbIo.h>

#define BMC_USB_KEY_VENDOR  0xcbcc
#define BMC_USB_KEY_PRODUCT 0xcbcc

EFI_SIMPLE_FILE_SYSTEM_PROTOCOL     *mFs = NULL;

EFI_STATUS
GetRequiredFileSystem (
  )
{
  EFI_HANDLE                 *SimpleFileSystemHandles;
  UINTN                      NumberSimpleFileSystemHandles;
  UINTN                      Index;
  EFI_STATUS                 Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *Fs;
  EFI_USB_IO_PROTOCOL              *UsbIo;
  EFI_USB_DEVICE_DESCRIPTOR        DeviceDescriptor;
  EFI_DEVICE_PATH_PROTOCOL         *DevicePath;
  EFI_HANDLE                       UsbHandle;

  if (mFs != NULL) {
    return EFI_SUCCESS;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumberSimpleFileSystemHandles,
                  &SimpleFileSystemHandles
                  );

  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  for (Index = 0; Index < NumberSimpleFileSystemHandles; Index++) {
    Status = gBS->HandleProtocol (SimpleFileSystemHandles[Index], &gEfiSimpleFileSystemProtocolGuid, (VOID **)&Fs);
    if (!EFI_ERROR (Status)) {
      Status = gBS->HandleProtocol (SimpleFileSystemHandles[Index], &gEfiDevicePathProtocolGuid, (VOID **)&DevicePath);
      if (!EFI_ERROR (Status)) {
        Status = gBS->LocateDevicePath (&gEfiUsbIoProtocolGuid, &DevicePath, &UsbHandle);
        if (!EFI_ERROR (Status)) {
          Status = gBS->HandleProtocol (UsbHandle, &gEfiUsbIoProtocolGuid, (VOID **)&UsbIo);
          if (!EFI_ERROR (Status)) {
            UsbIo->UsbGetDeviceDescriptor (UsbIo, &DeviceDescriptor);
            if (DeviceDescriptor.IdVendor == BMC_USB_KEY_VENDOR && DeviceDescriptor.IdProduct == BMC_USB_KEY_PRODUCT) {
              mFs = Fs;
              return EFI_SUCCESS;
            }
          }
        }
      }
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Read a file.

  @param[in]  FileName        The file to be read.
  @param[out] BufferSize      The file buffer size
  @param[out] Buffer          The file buffer

  @retval EFI_SUCCESS    Read file successfully
  @retval EFI_NOT_FOUND  Shell protocol or file not found
  @retval others         Read file failed
**/
EFI_STATUS
ReadFileToBuffer (
  IN  CHAR16                               *FileName,
  OUT UINTN                                *BufferSize,
  OUT VOID                                 **Buffer
  )
{
  EFI_STATUS                          Status;
  EFI_FILE                            *Root;
  EFI_FILE                            *FileHandle;
  UINT64                              FileInfo;
  UINT8                               *FileBuffer;

  Status = GetRequiredFileSystem ();
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  //
  // Open Root from SFS
  //
  Status = mFs->OpenVolume (mFs, &Root);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Cannot open volume. Status = %r\n", Status));
    return EFI_NOT_FOUND;
  }

  //
  // Open input file
  //
  FileHandle  = NULL;
  Status = Root->Open (Root, &FileHandle, FileName, EFI_FILE_MODE_READ, 0);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Unable to open %s file\n", FileName));
    return EFI_NOT_FOUND;
  }

  //
  // Get the file size
  //
  Status = FileHandleGetSize (FileHandle, &FileInfo);
  if (EFI_ERROR (Status)) {
    FileHandleClose (FileHandle);
    DEBUG ((DEBUG_INFO, "Error Reading %s\n", FileName));
    return EFI_DEVICE_ERROR;
  }

  //
  // Get the file contents
  //
  FileBuffer = AllocatePool (FileInfo + 1);
  Status = FileHandleRead (FileHandle, &FileInfo, FileBuffer);
  if (EFI_ERROR (Status)) {
    FileHandleClose (FileHandle);
    DEBUG ((DEBUG_INFO, "Error Reading %s\n", FileName));
    return EFI_DEVICE_ERROR;
  }

  FileBuffer[FileInfo] = 0;
  *BufferSize = FileInfo;
  *Buffer     = FileBuffer;

  DEBUG ((DEBUG_INFO, "Succeed to read %s\n", FileName));
  FileHandleClose (FileHandle);

  return EFI_SUCCESS;
}

/**
  Write a file.

  @param[in] FileName        The file to be written.
  @param[in] BufferSize      The file buffer size
  @param[in] Buffer          The file buffer

  @retval EFI_SUCCESS    Write file successfully
  @retval EFI_NOT_FOUND  Shell protocol not found
  @retval others         Write file failed
**/
EFI_STATUS
WriteFileFromBuffer (
  IN  CHAR16                               *FileName,
  IN  UINTN                                BufferSize,
  IN  VOID                                 *Buffer
  )
{
  EFI_STATUS                          Status;
  EFI_FILE                            *Root;
  EFI_FILE                            *FileHandle;
  UINT64                              FileInfo;

  Status = GetRequiredFileSystem ();
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  //
  // Open Root from SFS
  //
  Status = mFs->OpenVolume (mFs, &Root);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Cannot open volume. Status = %r\n", Status));
    return EFI_NOT_FOUND;
  }

  //
  // Open input file
  //
  FileHandle  = NULL;
  Status = Root->Open (Root, &FileHandle, FileName, EFI_FILE_MODE_CREATE | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_READ, 0);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Unable to create %s file\n", FileName));
    return EFI_NOT_FOUND;
  }

  //
  // Empty the file contents
  //
  Status = FileHandleGetSize (FileHandle, &FileInfo);
  if (EFI_ERROR (Status)) {
    FileHandleClose (FileHandle);
    DEBUG ((DEBUG_INFO, "Error Reading %s\n", FileName));
    return EFI_DEVICE_ERROR;
  }

  //
  // If the file size is already 0, then it has been empty.
  //
  if (FileInfo != 0) {
    //
    // Set the file size to 0.
    //
    FileInfo = 0;
    Status = FileHandleSetSize (FileHandle, FileInfo);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "Error Deleting %s\n", FileName));
      FileHandleClose (FileHandle);
      return Status;
    }
  }

  //
  // Write Filebuffer to file
  //
  Status = FileHandleWrite (FileHandle, &BufferSize, Buffer);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Unable to write Capsule Update to %s, Status = %r\n", FileName, Status));
    return EFI_NOT_FOUND;
  }

  DEBUG ((DEBUG_INFO, "Succeed to write %s\n", FileName));
  FileHandleClose (FileHandle);

  return EFI_SUCCESS;
}

