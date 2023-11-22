/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PlatformBootOption.c

Abstract:
  Provides the platform specific actions to refresh boot description.

Revision History:

**/

#include "PlatformBootManagerDxe.h"
#include "ByoBootOptionDescriptionLib.h"

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/ByoCommLib.h>
#include <Library/PcdLib.h>

#include <Guid/FileInfo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/UsbIo.h>
#include <Protocol/PciIo.h>
#include <Protocol/SecurityManagementProtocol.h>
#include <Protocol/NetworkInterfaceIdentifier.h>

#include <SysMiscCfg.h>
#include <IndustryStandard/Pci22.h>

//
// Support multiple OS on the same HDD
//
#define UOS_OS_STRING L"UOS"
#define UBUNTU_OS_STRING L"Ubuntu Linux"
#define DISCARD_OPTION_FLAG (DELETE_FLAG + 1) // it can not be DELETE_FLAG

#define INCREMENT_OF_COUNT 10
#define MAX_VALUE_OF_COUNT 20

#define BMC_USB_KEY_VENDOR  0xcbcc
#define BMC_USB_KEY_PRODUCT 0xcbcc

#define LENGTH_OF_DEVICE_NAME  128
UINTN mNetcardCount = MAX_VALUE_OF_COUNT;
typedef struct {
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  CHAR16                    *NetNameString;
  BOOLEAN                   Active;
} NETCARD_INFO;

NETCARD_INFO *mNetcardInfo = NULL;
UINTN mNetCount = 0;

SECURITY_MANAGEMENT_PROTOCOL *mSecurityManagement = NULL;

UEFI_OS_BOOT_FILE  mUefiX64OsBootFiles[] = {
  {L"\\EFI\\ctyunos\\shimx64.efi",               L"CTyunOS"},
  {L"\\EFI\\ctyunos\\grubx64.efi",               L"CTyunOS"},
  {L"\\EFI\\kos\\shimx64-kos.efi",               L"kos"},
  {L"\\EFI\\kos\\grubx64.efi",                   L"kos"},
  {L"\\EFI\\euleros\\shimx64.efi",               L"EulerOS"},
  {L"\\EFI\\openEuler\\shimx64.efi",             L"EulerOS"},
  {L"\\EFI\\onie\\grubx64.efi",                  L"Onie"},
  {L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi",      L"Windows Boot Manager"},
  {L"\\EFI\\centos\\shimx64-centos.efi",         L"CentOS Linux"},
  {L"\\EFI\\centos\\shimx64.efi",                L"CentOS Linux"},
  {L"\\EFI\\centos\\shim.efi",                   L"CentOS Linux"},
  {L"\\EFI\\centos\\grubx64.efi",                L"CentOS Linux"},
  {L"\\EFI\\uos\\shimx64.efi",                   UOS_OS_STRING},
  {L"\\EFI\\UnionTech\\shimx64.efi",             UOS_OS_STRING},
  {L"\\EFI\\ubuntu\\shimx64.efi",                UBUNTU_OS_STRING},
  {L"\\EFI\\ubuntu\\grubx64.efi",                UBUNTU_OS_STRING},
  {L"\\EFI\\redhat\\shimx64-redhat.efi",         L"Red Hat Linux"},
  {L"\\EFI\\redhat\\shimx64.efi",                L"Red Hat Linux"},
  {L"\\EFI\\redhat\\shim.efi",                   L"Red Hat Linux"},
  {L"\\EFI\\redhat\\grubx64.efi",                L"Red Hat Linux"},
  {L"\\EFI\\redhat\\grub.efi",                   L"Red Hat Linux"},
  {L"\\EFI\\neokylin\\shimx64.efi",              L"Kylin Linux"},
  {L"\\EFI\\kylin\\shimx64.efi",                 L"Kylin Linux"},
  {L"\\EFI\\kylin\\grubx64.efi",                 L"Kylin Linux"},
  {L"\\EFI\\tencent\\grubx64.efi",               L"Tencent OS tlinux"},
  {L"\\EFI\\tencent\\grub.efi",                  L"Tencent OS tlinux 1.2"},
  {L"\\EFI\\alios\\shimx64-alios.efi",           L"AliOS"},
  {L"\\EFI\\alios\\grubx64.efi",                 L"AliOS"},
  {L"\\EFI\\centos\\grubx64.efi",                L"CentOS Linux"},
  {L"\\EFI\\sles12\\shim.efi",                   L"SLES 12"},
  {L"\\EFI\\sles12\\grubx64.efi",                L"SLES 12"},
  {L"\\EFI\\sles\\shim.efi",                     L"SLES 11sp3"},
  {L"\\EFI\\opensuse\\shim.efi",                 L"OpenSUSE"},
  {L"\\EFI\\opensuse\\grubx64.efi",              L"OpenSUSE"},
  {L"\\EFI\\debian\\shimx64.efi",                L"Debian"},
  {L"\\EFI\\debian\\grubx64.efi",                L"Debian"},
  {L"\\EFI\\VMware\\mboot32.efi",                L"Vmware"},
  {L"\\EFI\\fedora\\shim.efi",                   L"Fedora"},
  {L"\\EFI\\linx\\grubx64.efi" ,                 L"linx"},
  {L"\\boot\\efi\\EFI\\BOOT\\BOOTX64.EFI",       L"linx"},
  {L"\\EFI\\anolis\\shimx64-anolios.efi",        L"Anolis"},
  {L"\\EFI\\anolis\\grubx64.efi",                L"Anolis"},
  {EFI_REMOVABLE_MEDIA_FILE_NAME,                NULL},
  {NULL,                                         NULL}
  };

UEFI_OS_BOOT_FILE  mUefiAarch64OsBootFiles[] = {
    {L"\\EFI\\deepin\\grubaa64.efi",             L"deepin"},
    {L"\\EFI\\euleros\\shimaa64.efi",            L"EulerOS"},
    {L"\\EFI\\centos\\grubaa64.efi",             L"CentOS"},
    {L"\\EFI\\debian\\grubaa64.efi",             L"Debian"},
    {L"\\EFI\\GRUB2\\GRUBAA64.EFI",              L"Hisilicon Linux"},
    {L"\\EFI\\redhat\\grubaa64.efi",             L"Redhat"},
    {L"\\EFI\\sles\\grubaa64.efi",               L"SuSE"},
    {L"\\EFI\\uos\\shimaa64.efi",                UOS_OS_STRING},
    {L"\\EFI\\uos\\grubaa64.efi",                UOS_OS_STRING},
    {L"\\EFI\\uniontech\\grubaa64.efi",          UOS_OS_STRING},
    {L"\\EFI\\ubuntu\\grubaa64.efi",             UBUNTU_OS_STRING},
    {L"\\EFI\\neokylin\\shimaa64.efi",           L"Neokylin"},
    {L"\\EFI\\kylin\\shimaa64.efi",              L"Kylin"},
    {L"\\EFI\\kylin\\grubaa64.efi",              L"Kylin"},
    {L"\\EFI\\redflag\\shimaa64.efi",            L"Asianux"},
    {L"\\EFI\\ems\\shimaa64.efi",                L"EmindServer"},
    {L"\\EFI\\isoft\\shimaa64.efi",              L"iSoft"},
    {L"\\EFI\\alios\\shimaa64.efi",              L"Alibaba Linux"},
    {L"\\EFI\\alios\\grubaa64.efi",              L"Alibaba Linux"},
    {L"\\EFI\\bclinux\\shimaa64.efi",            L"BigCloud Linux"},
    {L"\\EFI\\tencent\\grubaa64.efi",            L"tencent"},
    {NULL,                                       NULL}
};

UEFI_OS_BOOT_FILE  mUefiRiscv64OsBootFiles[] = {
  {EFI_REMOVABLE_MEDIA_FILE_NAME,                NULL},
  {NULL,                                         NULL}
};

#ifdef MDE_CPU_X64
  UEFI_OS_BOOT_FILE *mUefiOsBootFiles = mUefiX64OsBootFiles;
#elif defined (MDE_CPU_AARCH64)
  UEFI_OS_BOOT_FILE *mUefiOsBootFiles = mUefiAarch64OsBootFiles;
#elif defined (MDE_CPU_RISCV64)
  UEFI_OS_BOOT_FILE *mUefiOsBootFiles = mUefiRiscv64OsBootFiles;
#endif

typedef struct {
  UINT16        DeviceId;
  UINT16        VendorId;
  CHAR16        *NetcardTypeString;
} NETCARD_TYPE_STRING;

NETCARD_TYPE_STRING mNetcardStringTable[] = {
  { 0x0103, 0x8088, L"WangXun Gigabit Server Adapter WX1860A4"},
  { 0xFFFF, 0xFFFF, L"Customized Netcard"}
};

//
// Support multiple OS on the same HDD
//
typedef struct {
  EFI_DEVICE_PATH_PROTOCOL       *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL       *EspDevicePath;
  CHAR16                         *Description;
  CHAR16                         *OsString;
} FOUND_OS_LOADER;

  FOUND_OS_LOADER           *gOsLoaders = NULL;
  UINTN                     gOsLoaderMaxCount = MAX_VALUE_OF_COUNT;
  UINTN                     gOsLoadersCount;

typedef struct {
  UINTN    Bus;
  UINTN    Device;
  UINTN    Segment;
  UINTN    *NetIndex;
  UINT16   *IfNums;
  UINTN    Count;
  UINT16   MinIfNum;
} NETCARD_PORT_INFO;

/**
  Get the headers (dos, image, optional header) from an image

  @param  Device                SimpleFileSystem device handle
  @param  FileName              File name for the image
  @param  DosHeader             Pointer to dos header
  @param  Hdr                   The buffer in which to return the PE32, PE32+, or TE header.

  @retval EFI_SUCCESS           Successfully get the machine type.
  @retval EFI_NOT_FOUND         The file is not found.
  @retval EFI_LOAD_ERROR        File is not a valid image file.

**/
EFI_STATUS
EFIAPI
GetImageHeader (
  IN EFI_HANDLE                             Device,
  IN CHAR16                                 *FileName,
  OUT VOID                                  **FileBuffer,
  OUT UINTN                                 *FileSize,
  OUT EFI_IMAGE_DOS_HEADER                  *DosHeader,
  OUT EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr
  )
{
  EFI_STATUS                       Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *Volume;
  EFI_FILE_HANDLE                  Root;
  EFI_FILE_HANDLE                  ThisFile;
  UINTN                            BufferSize;
  UINT64                           Size;
  EFI_FILE_INFO                    *Info;

  Root     = NULL;
  ThisFile = NULL;
  //
  // Handle the file system interface to the device
  //
  Status = gBS->HandleProtocol (
                  Device,
                  &gEfiSimpleFileSystemProtocolGuid,
                  (VOID *) &Volume
                  );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = Volume->OpenVolume (
                     Volume,
                     &Root
                     );
  if (EFI_ERROR (Status)) {
    Root = NULL;
    goto Done;
  }
  ASSERT (Root != NULL);
  Status = Root->Open (Root, &ThisFile, FileName, EFI_FILE_MODE_READ, 0);
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  ASSERT (ThisFile != NULL);

  //
  // Get file size
  //
  BufferSize  = SIZE_OF_EFI_FILE_INFO + 200;
  do {
    Info   = NULL;
    Status = gBS->AllocatePool (EfiBootServicesData, BufferSize, (VOID **) &Info);
    if (EFI_ERROR (Status)) {
      goto Done;
    }
    Status = ThisFile->GetInfo (
                         ThisFile,
                         &gEfiFileInfoGuid,
                         &BufferSize,
                         Info
                         );
    if (!EFI_ERROR (Status)) {
      break;
    }
    if (Status != EFI_BUFFER_TOO_SMALL) {
      FreePool (Info);
      goto Done;
    }
    FreePool (Info);
  } while (TRUE);

  Size = Info->FileSize;
  if (FileSize != NULL) {
    *FileSize = Size;
  }
  FreePool (Info);

  //
  // Read dos header
  //
  BufferSize = sizeof (EFI_IMAGE_DOS_HEADER);
  Status = ThisFile->Read (ThisFile, &BufferSize, DosHeader);
  if (EFI_ERROR (Status) ||
      BufferSize < sizeof (EFI_IMAGE_DOS_HEADER) ||
      Size <= DosHeader->e_lfanew ||
      DosHeader->e_magic != EFI_IMAGE_DOS_SIGNATURE) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

  //
  // Move to PE signature
  //
  Status = ThisFile->SetPosition (ThisFile, DosHeader->e_lfanew);
  if (EFI_ERROR (Status)) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

  //
  // Read and check PE signature
  //
  BufferSize = sizeof (EFI_IMAGE_OPTIONAL_HEADER_UNION);
  Status = ThisFile->Read (ThisFile, &BufferSize, Hdr.Pe32);
  if (EFI_ERROR (Status) ||
      BufferSize < sizeof (EFI_IMAGE_OPTIONAL_HEADER_UNION) ||
      Hdr.Pe32->Signature != EFI_IMAGE_NT_SIGNATURE) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

  //
  // Check PE32 or PE32+ magic
  //
  if ((Hdr.Pe32->OptionalHeader.Magic != EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    && (Hdr.Pe32->OptionalHeader.Magic != EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC)) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

  //
  // Read all file
  //
  if (FileSize != NULL) {
    Status = ThisFile->SetPosition (ThisFile, 0);
    if (!EFI_ERROR (Status)) {
      *FileBuffer = AllocatePool(*FileSize);
      if (EFI_ERROR(Status)) {
        Status = EFI_OUT_OF_RESOURCES;
        *FileSize = 0;
        goto Done;
      }
      Status = ThisFile->Read (ThisFile, FileSize, *FileBuffer);
      if (EFI_ERROR(Status)) {
        FreePool(*FileBuffer);
        *FileBuffer = NULL;
        *FileSize = 0;
      }
    }
  }

 Done:
  if (ThisFile != NULL) {
    ThisFile->Close (ThisFile);
  }
  if (Root != NULL) {
    Root->Close (Root);
  }
  return Status;
}

CHAR16 *
AppendOsDescription (
  IN EFI_HANDLE                Handle
  )
{
  EFI_STATUS                          Status;
  EFI_HANDLE                          *SimpleFileSystemHandles;
  UINTN                               NumberSimpleFileSystemHandles;
  EFI_DEVICE_PATH_PROTOCOL            *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
  UINTN                               Size,TempSize,Index;
  UINTN                               LoaderIndex;
  EFI_IMAGE_DOS_HEADER                DosHeader;
  EFI_IMAGE_OPTIONAL_HEADER_UNION     HdrData;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION Hdr;
  CHAR16                              *Description;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Description = NULL;
  DevicePath  = DevicePathFromHandle(Handle);
  ShowDevicePathDxe(gBS, DevicePath);
  //
  // Detect the the default boot file from removable Media
  //
  Size = GetDevicePathSize(DevicePath) - END_DEVICE_PATH_LENGTH;
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiSimpleFileSystemProtocolGuid,
         NULL,
         &NumberSimpleFileSystemHandles,
         &SimpleFileSystemHandles
         );
  for (Index = 0; Index < NumberSimpleFileSystemHandles; Index++) {
    //
    // Get the device path size of SimpleFileSystem handle
    //
    TempDevicePath = DevicePathFromHandle (SimpleFileSystemHandles[Index]);
    // ShowDevicePathDxe(gBS, TempDevicePath);
    TempSize = GetDevicePathSize (TempDevicePath)- END_DEVICE_PATH_LENGTH;
    //
    // Check whether the device path of boot option is part of the SimpleFileSystem handle's device path
    //
    if ((Size <= TempSize) && (CompareMem (TempDevicePath, DevicePath, Size) == 0)) {
      //
      // Load the default boot file \EFI\BOOT\boot{machinename}.EFI from removable Media
      //  machinename is ia32, ia64, x64, ...
      //
      Hdr.Union = &HdrData;
      Status = EFI_NOT_FOUND;
      LoaderIndex = 0;
      while (mUefiOsBootFiles[LoaderIndex].FilePathString != NULL) {
        Status = GetImageHeader (
                   SimpleFileSystemHandles[Index],
                   mUefiOsBootFiles[LoaderIndex].FilePathString,
                   NULL,
                   NULL,
                   &DosHeader,
                   Hdr
                   );
        if(!EFI_ERROR(Status)){
          break;
        }
        LoaderIndex ++;
      }
      
      if (!EFI_ERROR (Status) && EFI_IMAGE_MACHINE_TYPE_SUPPORTED (Hdr.Pe32->FileHeader.Machine)
      && Hdr.Pe32->OptionalHeader.Subsystem == EFI_IMAGE_SUBSYSTEM_EFI_APPLICATION) {
        if (mUefiOsBootFiles[LoaderIndex].Description != NULL) {
          Size = StrSize(mUefiOsBootFiles[LoaderIndex].Description);
          Description = AllocateZeroPool (Size);
          StrCpyS(Description, Size / sizeof(CHAR16), mUefiOsBootFiles[LoaderIndex].Description);
        }
        break;
      }
    }
  }

  if (SimpleFileSystemHandles != NULL) {
    FreePool(SimpleFileSystemHandles);
  }
  DEBUG((EFI_D_INFO, "OS string : %s\n", Description));
  return Description;
}

VOID
AppenOsDescriptionOnFixedDevice(
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION    *BootOptions,
  IN CONST UINTN                         BootOptionsCount
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  EFI_HANDLE                            Handle;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  EFI_BLOCK_IO_PROTOCOL                 *BlkIo;
  CHAR16                                *OsDescriptionPtr;
  UINTN                                 Size;
  CHAR16                                *Temp;

  Status = EFI_NOT_READY;

  if ((BootOptions == NULL) || (BootOptionsCount == 0)) {
    return;
  }
  for (Index = 0; Index < BootOptionsCount; Index++) {
    if ((BootOptions[Index].OptionNumber == DELETE_FLAG)
      || (BootOptions[Index].OptionNumber == DISCARD_OPTION_FLAG)) {
      continue;
    }
    // boot option:
    DevicePath = BootOptions[Index].FilePath;
    Status = gBS->LocateDevicePath(&gEfiBlockIoProtocolGuid, &DevicePath, &Handle);
    if (EFI_ERROR (Status)) {
      continue;
    }
    Status = gBS->HandleProtocol(
                      Handle,
                      &gEfiBlockIoProtocolGuid,
                      (VOID **)&BlkIo
                      );
    if (EFI_ERROR(Status)) {
      continue;
    }
    //
    // skip the removable block io
    //
    if (BlkIo->Media->RemovableMedia) {
      continue;
    }
    //
    // Append OS string to description 
    //
    OsDescriptionPtr = AppendOsDescription(Handle);
    if (OsDescriptionPtr != NULL) {
      Size = StrSize (BootOptions[Index].Description) + StrLen(OsDescriptionPtr) * sizeof(CHAR16) + 4;
      Temp = (CHAR16 *)AllocatePool(Size);
      UnicodeSPrint (Temp, Size, L"%s(%s)", OsDescriptionPtr, BootOptions[Index].Description);
      FreePool (BootOptions[Index].Description);
      BootOptions[Index].Description = Temp;
      FreePool (OsDescriptionPtr);
    }
  }
}

VOID
MarkInvalidOptions (
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION    *BootOptions,
  IN  CONST UINTN                        BootOptionsCount
  )
{
  EFI_STATUS                            Status;
  UINT8                                 NetBootIp4Ip6;
  UINTN                                 Index;
  UINT8                                 IpType;
  EFI_HANDLE                            Handle;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  CHAR16                                *BootStr;
  UINT8                                 GroupType;
  EFI_USB_IO_PROTOCOL                   *UsbIo;
  EFI_USB_DEVICE_DESCRIPTOR             DeviceDescriptor;

  if ((BootOptions == NULL) || (BootOptionsCount == 0)) {
    return;
  }

  NetBootIp4Ip6 = PcdGet8(PcdNetBootIp4Ip6);
  for (Index = 0; Index < BootOptionsCount; Index++) {
    if (BootOptions[Index].OptionNumber == DELETE_FLAG) {
      continue;
    }

    DevicePath = BootOptions[Index].FilePath;
    GroupType = GetEfiBootGroupType(DevicePath);
    //
    // Mark invalid NetworkType boot options
    //
    if (GroupType == BM_MENU_TYPE_UEFI_PXE) {
      IpType = GetEfiNetWorkType(BootOptions[Index].FilePath, &BootStr);
      if ((NetBootIp4Ip6 & ENABLE_SPEPARATE_CONTROL_NET_TYPE) == 0) {
        if (((NetBootIp4Ip6 != NET_BOOT_IP_ALL) && (NetBootIp4Ip6 != NET_BOOT_IP_V4) && (NetBootIp4Ip6 != NET_BOOT_IP_V6))
          || (NetBootIp4Ip6 == NET_BOOT_IP_V4 && IpType == MSG_IPv6_DP)
          || (NetBootIp4Ip6 == NET_BOOT_IP_V6 && IpType == MSG_IPv4_DP)
          || (IpType == 0xFF)) {
          //
          // Invalid boot options:needed to be deleted
          //
          BootOptions[Index].OptionNumber = DELETE_FLAG;
        }
      } else {
        if ((((NetBootIp4Ip6 & ENABLE_PXE_IPV4) == 0) && (IpType == MSG_IPv4_DP) && (StrStr(BootStr, L"PXE") != NULL))
          || (((NetBootIp4Ip6 & ENABLE_PXE_IPV6) == 0) && (IpType == MSG_IPv6_DP) && (StrStr(BootStr, L"PXE") != NULL))
          || (((NetBootIp4Ip6 & ENABLE_HTTP_IPV4) == 0) && (IpType == MSG_IPv4_DP) && (StrStr(BootStr, L"HTTP") != NULL))
          || (((NetBootIp4Ip6 & ENABLE_HTTP_IPV6) == 0) && (IpType == MSG_IPv6_DP) && (StrStr(BootStr, L"HTTP") != NULL))
          || (IpType == 0xFF)) {
          //
          // Invalid boot options:needed to be deleted
          //
          BootOptions[Index].OptionNumber = DELETE_FLAG;
        }
      }
    } else { // Mark BMC emulated USB device boot option
      DevicePath = BootOptions[Index].FilePath;
      Status = gBS->LocateDevicePath(&gEfiUsbIoProtocolGuid, &DevicePath, &Handle);
      if (EFI_ERROR (Status)) {
        continue;
      }
      Status = gBS->HandleProtocol(
                        Handle,
                        &gEfiUsbIoProtocolGuid,
                        (VOID **)&UsbIo
                        );
      if (EFI_ERROR(Status)) {
        continue;
      }

      UsbIo->UsbGetDeviceDescriptor (UsbIo, &DeviceDescriptor);
      if (DeviceDescriptor.IdVendor == BMC_USB_KEY_VENDOR && DeviceDescriptor.IdProduct == BMC_USB_KEY_PRODUCT) {
        //
        // Invalid boot options:needed to be deleted
        //
        BootOptions[Index].OptionNumber = DELETE_FLAG;
      }
    }
  }
}

VOID
ProcessBootDescriptions(
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION    *BootOptions,
  IN  CONST UINTN                        BootOptionsCount
  )
{
  UINTN                                 Index;
  CHAR16                                *String = NULL;
  if ((BootOptions == NULL) || (BootOptionsCount == 0)) {
    return;
  }

  for (Index = 0; Index < BootOptionsCount; Index++) {
    if (BootOptions[Index].OptionNumber == DELETE_FLAG) {
      continue;
    }
    String = ByoGetUEFIBootDescription(BootOptions[Index].FilePath);
    if (String != NULL) {
      FreePool(BootOptions[Index].Description);
      BootOptions[Index].Description = String;
    }
  }
}

BOOLEAN
IsOsLoaderValid (
  IN EFI_HANDLE                   Handle,
  IN CONST CHAR16                 *Description
  )
{
  UINTN                                 Index;
  UINTN                                 Size;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  BOOLEAN                               IsEspBrandNew = TRUE;
  BOOLEAN                               IsDescriptionNew = TRUE;
  BOOLEAN                               SameEspHasUos = FALSE;
  BOOLEAN                               SameEspHasUbuntu = FALSE;

  if ((Description == NULL)) {
    return FALSE;
  }

  DevicePath = DevicePathFromHandle(Handle);
  Size = GetDevicePathSize (DevicePath)- END_DEVICE_PATH_LENGTH;
  for (Index = 0; Index < gOsLoadersCount; Index++) {
    if (CompareMem(gOsLoaders[Index].EspDevicePath, DevicePath, Size) == 0) {
      IsEspBrandNew = FALSE;
      if (StrnCmp(gOsLoaders[Index].OsString, Description, StrLen(Description)) == 0) {
        IsDescriptionNew = FALSE;
      }

      if (StrnCmp(UOS_OS_STRING, gOsLoaders[Index].OsString, StrLen(UOS_OS_STRING)) == 0) {
        SameEspHasUos = TRUE;
      }

      if (StrnCmp(UBUNTU_OS_STRING, gOsLoaders[Index].OsString, StrLen(UBUNTU_OS_STRING)) == 0) {
        SameEspHasUbuntu = TRUE;
      }
    }
  }

  if (IsEspBrandNew) { // Current Os Loader is on the brand new ESP
    return TRUE;
  } else { // found same ESP
    //
    // Limit: 1) UOS and Ubuntu can not be installed on the same ESP
    //        2) UOS only boot \\EFI\\uos\\shimx64.efi
    //
    if (IsDescriptionNew) {
      if (SameEspHasUos && (StrnCmp(Description, UBUNTU_OS_STRING,
        StrLen(UBUNTU_OS_STRING)) == 0)) { // Same ESP has UOS, not boot ubuntu OS Loader
        return FALSE;
      }

      if (SameEspHasUbuntu && (StrnCmp(Description, UOS_OS_STRING,
        StrLen(UOS_OS_STRING)) == 0)) { // Same ESP has Ubuntu, not boot UOS OS Loader
        return FALSE;
      }
      return TRUE;
    }
  }

  return FALSE;
}

BOOLEAN
CheckAndAddOsLoaders (
  IN EFI_HANDLE                     Handle,
  IN CONST CHAR16                   *String
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  EFI_IMAGE_DOS_HEADER                  DosHeader;
  EFI_IMAGE_OPTIONAL_HEADER_UNION       HdrData;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr;
  UINTN                                 Size;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  BOOLEAN                               FoundOneOsLoader = FALSE;
  VOID                                  *FileBuffer;
  UINTN                                 FileSize;

  if (String == NULL) {
    return FALSE;
  }

  for (Index = 0; mUefiOsBootFiles[Index].FilePathString != NULL; Index++) {
    Hdr.Union = &HdrData;
    Status = GetImageHeader (
                      Handle,
                      mUefiOsBootFiles[Index].FilePathString,
                      &FileBuffer,
                      &FileSize,
                      &DosHeader,
                      Hdr
                      );
    if (!EFI_ERROR(Status) && EFI_IMAGE_MACHINE_TYPE_SUPPORTED (Hdr.Pe32->FileHeader.Machine)
      && (Hdr.Pe32->OptionalHeader.Subsystem == EFI_IMAGE_SUBSYSTEM_EFI_APPLICATION)
      && (mUefiOsBootFiles[Index].Description != NULL)) {
      DevicePath = FileDevicePath(Handle, mUefiOsBootFiles[Index].FilePathString);
      if (DevicePath != NULL) {
        //
        // Check if OS loader is secure
        //
        if ((FileBuffer != NULL) && (mSecurityManagement != NULL)
          &&(mSecurityManagement->ExecuteSecurity2Handlers != NULL)) {
          Status = mSecurityManagement->ExecuteSecurity2Handlers(
                                          EFI_AUTH_OPERATION_VERIFY_IMAGE,
                                          0,
                                          DevicePath,
                                          FileBuffer,
                                          FileSize,
                                          FALSE
                                          );
          if (EFI_ERROR(Status)) {
            FreePool(FileBuffer);
            FoundOneOsLoader = TRUE;
            continue;
          }
        }

        if (gOsLoadersCount != 0) {
          if (!IsOsLoaderValid(Handle, mUefiOsBootFiles[Index].Description)) {
            continue;
          }
        }

        //
        // Fill in information of OS Loaders
        //
        if ((gOsLoaders != NULL) && (gOsLoadersCount == gOsLoaderMaxCount)) {
          gOsLoaders = (FOUND_OS_LOADER *)ReallocatePool (
                          sizeof(FOUND_OS_LOADER) * gOsLoadersCount,
                          sizeof(FOUND_OS_LOADER) * (gOsLoadersCount + INCREMENT_OF_COUNT),
                          gOsLoaders
                          );
          ASSERT(gOsLoaders != NULL);
          gOsLoaderMaxCount = gOsLoaderMaxCount + INCREMENT_OF_COUNT;
        }

        gOsLoaders[gOsLoadersCount].EspDevicePath = DuplicateDevicePath(DevicePathFromHandle(Handle));
        gOsLoaders[gOsLoadersCount].DevicePath = DevicePath;
        gOsLoaders[gOsLoadersCount].OsString = (CHAR16 *)AllocateCopyPool (
                                                  StrSize(mUefiOsBootFiles[Index].Description),
                                                  mUefiOsBootFiles[Index].Description
                                                  );
        ASSERT(gOsLoaders[gOsLoadersCount].OsString != NULL);
        Size = StrSize (String) + StrSize(mUefiOsBootFiles[Index].Description) + 2;
        gOsLoaders[gOsLoadersCount].Description = (CHAR16 *)AllocatePool(Size);
        ASSERT(gOsLoaders[gOsLoadersCount].Description != NULL);

        UnicodeSPrint (gOsLoaders[gOsLoadersCount].Description, Size, L"%s(%s)", mUefiOsBootFiles[Index].Description, String);
        gOsLoadersCount++;
        FoundOneOsLoader = TRUE;
        if (FileBuffer != NULL) {
          FreePool(FileBuffer);
        }
      }
    }
  }

  return FoundOneOsLoader;
}

//
// 1) Find all OS Loaders on the current HDD option;
// 2) return informations of OS Loaders;
// 3) As soon as an OS Loader is found, the current
// boot option will be deleted(Set OptionNumber to DELETE_FLAG).
//
VOID
FindMoreOsLoaders (
  IN OUT  EFI_BOOT_MANAGER_LOAD_OPTION  *Options,
  IN      UINTN                         OptionsCount
  )
{
  EFI_STATUS                            Status;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  UINTN                                 Count;
  UINTN                                 Index;
  UINTN                                 Size;
  EFI_HANDLE                            *SimpleFileSystemHandles;
  UINTN                                 NumberSimpleFileSystemHandles;
  BOOLEAN                               FoundOneOsLoader = FALSE;
  BOOLEAN                               IsEmmcDevice;

  if ((Options == NULL) || (OptionsCount == 0)) {
    return;
  }

  Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiSimpleFileSystemProtocolGuid,
                    NULL,
                    &NumberSimpleFileSystemHandles,
                    &SimpleFileSystemHandles
                    );
  if (EFI_ERROR(Status) || (NumberSimpleFileSystemHandles == 0)) {
    return;
  }

  gBS->LocateProtocol(&gSecurityManagementProtocolGuid, NULL, (VOID **)&mSecurityManagement);

  if (gOsLoaders == NULL) {
    gOsLoaders = (FOUND_OS_LOADER *)AllocateZeroPool(sizeof(FOUND_OS_LOADER) * gOsLoaderMaxCount);
    ASSERT(gOsLoaders != NULL);
    gOsLoadersCount = 0;
  }

  for (Count = 0; Count < OptionsCount; Count++) {
    IsEmmcDevice = FALSE;
    DevicePath = Options[Count].FilePath;
    while (!IsDevicePathEnd (DevicePath) && (DevicePathType (DevicePath) != MESSAGING_DEVICE_PATH)) {
      DevicePath = NextDevicePathNode (DevicePath);
    }
    if (!IsDevicePathEnd(DevicePath) && (DevicePathSubType(DevicePath) == MSG_EMMC_DP)) {
      IsEmmcDevice = TRUE;
    }
    if (!IsEmmcDevice && (GetEfiBootGroupType(Options[Count].FilePath) != BM_MENU_TYPE_UEFI_HDD)
      && (GetEfiBootGroupType(Options[Count].FilePath) != BM_MENU_TYPE_UEFI_PXE)) {
      continue;
    }

    Size = GetDevicePathSize(Options[Count].FilePath) - END_DEVICE_PATH_LENGTH;
    for (Index = 0; Index < NumberSimpleFileSystemHandles; Index++) {
      DevicePath = DevicePathFromHandle(SimpleFileSystemHandles[Index]);
      if ((Size <= (GetDevicePathSize (DevicePath)- END_DEVICE_PATH_LENGTH))
        && (CompareMem(DevicePath, Options[Count].FilePath, Size) == 0)) {
          FoundOneOsLoader = CheckAndAddOsLoaders (
                                SimpleFileSystemHandles[Index],
                                Options[Count].Description
                                );
          if (FoundOneOsLoader) {
            Options[Count].OptionNumber = DISCARD_OPTION_FLAG;
          }
      }
    }
  }
}

VOID
AdjustBootOptions (
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION **UpdatedBootOptions,
  IN OUT UINTN                        *UpdatedBootOptionsCount
  )
{
  EFI_STATUS                Status;
  UINTN                     Index = 0;

  //
  // Delete discarded options enumerated before
  //
  while (Index != *UpdatedBootOptionsCount) {
    if ((*UpdatedBootOptions)[Index].OptionNumber == DISCARD_OPTION_FLAG) {
      EfiBootManagerFreeLoadOption(*UpdatedBootOptions + Index);
      CopyMem (*UpdatedBootOptions + Index,  *UpdatedBootOptions + Index + 1,
                sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * (*UpdatedBootOptionsCount - Index - 1));
      *UpdatedBootOptionsCount = *UpdatedBootOptionsCount - 1;
      continue;
    }

    Index++;
  }

  //
  // Build boot options for OS loaders
  //
  *UpdatedBootOptions = (EFI_BOOT_MANAGER_LOAD_OPTION *)ReallocatePool (
                                  sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * (*UpdatedBootOptionsCount),
                                  sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * (*UpdatedBootOptionsCount + gOsLoadersCount),
                                  *UpdatedBootOptions
                                  );
  ASSERT (*UpdatedBootOptions != NULL);

  for (Index = 0; Index < gOsLoadersCount; Index++) {
    Status = EfiBootManagerInitializeLoadOption (
                                  (*UpdatedBootOptions) + (*UpdatedBootOptionsCount)++,
                                  LoadOptionNumberUnassigned,
                                  LoadOptionTypeBoot,
                                  LOAD_OPTION_ACTIVE,
                                  gOsLoaders[Index].Description,
                                  gOsLoaders[Index].DevicePath,
                                  NULL,
                                  0
                                  );
    ASSERT_EFI_ERROR (Status);
  }

  if (gOsLoaders != NULL) {
    for (Index = 0; Index < gOsLoadersCount; Index++) {
      if (gOsLoaders[Index].OsString != NULL) { FreePool(gOsLoaders[Index].OsString); }
      if (gOsLoaders[Index].Description != NULL) { FreePool(gOsLoaders[Index].Description); }
      if (gOsLoaders[Index].EspDevicePath != NULL) { FreePool(gOsLoaders[Index].EspDevicePath); }
      if (gOsLoaders[Index].DevicePath != NULL) { FreePool(gOsLoaders[Index].DevicePath); }
    }
    FreePool(gOsLoaders);
    gOsLoaders = NULL;
  }
  gOsLoadersCount = 0;
  gOsLoaderMaxCount = MAX_VALUE_OF_COUNT;
}

VOID
CreateNetDevicePath (
  VOID
  )
{
  EFI_STATUS                Status;
  UINTN                     Index;
  UINTN                     Count;
  UINTN                     HandleCount;
  EFI_HANDLE                *Handles;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *Net31DevicePath = NULL;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  EFI_HANDLE                PciHandle;
  EFI_PCI_IO_PROTOCOL       *Net31PciIo;
  UINT8                     ClassCode[3];
  UINTN                     Net31HandleCount = 0;
  EFI_HANDLE                *Net31HandleBuffer;
  UINTN                     AddedNetCount;
  BOOLEAN                   NetcardDpAdded;
  EFI_HANDLE                SnpHandle;
  BOOLEAN                   FoundPciNii;
  CHAR16                    *NameString;
  UINTN                     Seg, Bus, Dev, Func;
  EFI_NETWORK_INTERFACE_IDENTIFIER_PROTOCOL  *Nii;
  UINT16                    IfNum;

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  Status = gBS->LocateHandleBuffer (
    ByProtocol,
    &gEfiPciIoProtocolGuid,
    NULL,
    &HandleCount,
    &Handles
  );
  if (EFI_ERROR(Status)) {
    return;
  }

  gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiNetworkInterfaceIdentifierProtocolGuid_31,
                  NULL,
                  &Net31HandleCount,
                  &Net31HandleBuffer
                  );
  mNetcardInfo = (NETCARD_INFO *)AllocateZeroPool(sizeof(NETCARD_INFO) * mNetcardCount);
  ASSERT(mNetcardInfo != NULL);

  //
  // Add DevicePath of Pci netcard:
  // 1) If it support gEfiNetworkInterfaceIdentifierProtocolGuid_31(call it Nii later), save the
  // DevicePath of Nii which has MAC DevicePath node;
  // 2) If it doesn't support Nii, save the DevicePath of Pci handle without MAC DevicePath node.
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (Handles[Index], &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
    if (EFI_ERROR(Status)) {
      continue;
    }

    Status = PciIo->Pci.Read (
                  PciIo,
                  EfiPciIoWidthUint8,
                  0x9,
                  3,
                  ClassCode
                  );
    if (EFI_ERROR(Status) || (ClassCode[2] != PCI_CLASS_NETWORK)) {
      continue;
    }

    DevicePath = DevicePathFromHandle(Handles[Index]);
    if (DevicePath == NULL) {
      continue;
    }

    //
    // Check if current netcard if connected,then save the DevicePath with MAC node.
    // NOTE:one PciIo may be connected to two or more Nii.
    //
    FoundPciNii = FALSE;
    for (Count = 0; Count < Net31HandleCount; Count++) {
      Status = gBS->HandleProtocol (
                      Net31HandleBuffer[Count],
                      &gEfiDevicePathProtocolGuid,
                      (VOID**)&TempDevicePath
                      );
      if (EFI_ERROR(Status)) {
        continue;
      }
      Net31DevicePath = TempDevicePath;
      Status = gBS->LocateDevicePath (
                    &gEfiPciIoProtocolGuid,
                    &TempDevicePath,
                    &PciHandle
                    );
      if (EFI_ERROR(Status)) {
        continue;
      }

      Status = gBS->HandleProtocol (
                    PciHandle,
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &Net31PciIo
                    );
      if (EFI_ERROR(Status)) {
        continue;
      }

      if (Net31PciIo == PciIo) { // found PCI netcard which supports Nii
        //
        // Add DevicePath of PCI netcard which supports Nii
        //
        if (mNetCount == mNetcardCount) {
          mNetcardInfo = (NETCARD_INFO *)ReallocatePool(
                                                          sizeof(NETCARD_INFO) * mNetcardCount,
                                                          sizeof(NETCARD_INFO) * (mNetcardCount + INCREMENT_OF_COUNT),
                                                          mNetcardInfo);
          ASSERT(mNetcardInfo != NULL);
          mNetcardCount = mNetcardCount + INCREMENT_OF_COUNT;
        }
        mNetcardInfo[mNetCount].DevicePath = DuplicateDevicePath(Net31DevicePath); // DevicePath of Nii with MAC node
        ASSERT(mNetcardInfo[mNetCount].DevicePath != NULL);
        mNetcardInfo[mNetCount].PciIo = PciIo;
        mNetcardInfo[mNetCount].Active = FALSE;
        NameString = GetNameStringOfNetcard(mNetcardInfo[mNetCount].DevicePath, TRUE);
        PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Func);
        IfNum = 0;
        Status = gBS->HandleProtocol(Net31HandleBuffer[Count], &gEfiNetworkInterfaceIdentifierProtocolGuid_31, (VOID**)&Nii);
        if (!EFI_ERROR(Status)) {
          IfNum = Nii->IfNum;
        }
        if (IfNum != 0) {
          AppendIfNumToNameString(&NameString, Func, IfNum);
        }
        mNetcardInfo[mNetCount].NetNameString = NameString;
        mNetCount++;
        FoundPciNii = TRUE;
      }
    }

    if (!FoundPciNii) { // current PCI netcard doesn't support Nii
      TempDevicePath = DevicePath;
      Status = gBS->LocateDevicePath (
                    &gEfiSimpleNetworkProtocolGuid,
                    &TempDevicePath,
                    &SnpHandle
                    );
      if (EFI_ERROR(Status) && !PcdGetBool(PcdCreatePciNetOptionsWithNoOpRom)) {
        continue;
      }

      //
      // Add DevicePath of PCI netcard which doesn't support Nii and SNP
      //
      if (mNetCount == mNetcardCount) {
        mNetcardInfo = (NETCARD_INFO *)ReallocatePool(
                                                        sizeof(NETCARD_INFO) * mNetcardCount,
                                                        sizeof(NETCARD_INFO) * (mNetcardCount + INCREMENT_OF_COUNT),
                                                        mNetcardInfo);
        ASSERT(mNetcardInfo != NULL);
        mNetcardCount = mNetcardCount + INCREMENT_OF_COUNT;
      }
      mNetcardInfo[mNetCount].DevicePath = DuplicateDevicePath(DevicePath); // DevicePath of PciIo, no MAC node
      ASSERT(mNetcardInfo[mNetCount].DevicePath != NULL);
      mNetcardInfo[mNetCount].PciIo = PciIo;
      mNetcardInfo[mNetCount].Active = FALSE;
      mNetcardInfo[mNetCount].NetNameString = NULL;
      mNetCount++;
    }
  }

  //
  // Add DevicePath of netcard which doesn't support Pci, but do support Nii.
  //
  AddedNetCount = mNetCount;
  for (Index = 0; Index < Net31HandleCount; Index++) {
    DevicePath = DevicePathFromHandle(Net31HandleBuffer[Index]);
    if (DevicePath == NULL) {
      continue;
    }

    //
    // Skip added DevicePath which support Pci and Nii
    //
    NetcardDpAdded = FALSE;
    for (Count = 0; Count < AddedNetCount; Count++) {
      if (UefiCompareDevicePath(DevicePath, mNetcardInfo[Count].DevicePath)) {
        NetcardDpAdded = TRUE;
        break;
      }
    }
    if (NetcardDpAdded) {
      continue;
    }

    if (mNetCount == mNetcardCount) {
      mNetcardInfo = (NETCARD_INFO *)ReallocatePool(
                                                      sizeof(NETCARD_INFO) * mNetcardCount,
                                                      sizeof(NETCARD_INFO) * (mNetcardCount + INCREMENT_OF_COUNT),
                                                      mNetcardInfo);
      ASSERT(mNetcardInfo != NULL);
      mNetcardCount = mNetcardCount + INCREMENT_OF_COUNT;
    }

    //
    // Add DevicePath of Nii netcard which doesn't support PCI
    //
    mNetcardInfo[mNetCount].DevicePath = DuplicateDevicePath(DevicePath);
    ASSERT(mNetcardInfo[mNetCount].DevicePath != NULL);
    mNetcardInfo[mNetCount].Active = FALSE;
    mNetcardInfo[mNetCount].PciIo = NULL;
    NameString = GetNameStringOfNetcard(mNetcardInfo[mNetCount].DevicePath, TRUE);
    IfNum = 0;
    Status = gBS->HandleProtocol(Net31HandleBuffer[Count], &gEfiNetworkInterfaceIdentifierProtocolGuid_31, (VOID**)&Nii);
    if (!EFI_ERROR(Status)) {
      IfNum = Nii->IfNum;
    }
    if (IfNum != 0) {
      AppendIfNumToNameString(&NameString, Func, IfNum);
    }
    mNetcardInfo[mNetCount].NetNameString = NameString;
    mNetCount++;
  }

  if (Net31HandleBuffer != NULL) {
    FreePool(Net31HandleBuffer);
  }

  if (Handles != NULL) {
    FreePool(Handles);
  }

  //
  // Add more DevicePath of netcard which not support Pci or Nii, only gEfiSimpleNetworkProtocolGuid
  //
  Status  = gBS->LocateHandleBuffer (
                   ByProtocol,
                   &gEfiSimpleNetworkProtocolGuid,
                   NULL,
                   &HandleCount,
                   &Handles
                   );
  if (EFI_ERROR(Status)) {
    return;
  }

  AddedNetCount = mNetCount;
  for (Index = 0; Index < HandleCount; Index++) {
    DevicePath = DevicePathFromHandle(Handles[Index]);
    if (DevicePath == NULL) {
      continue;
    }

    //
    // Skip added DevicePath which support Pci, Nii and gEfiSimpleNetworkProtocolGuid.
    //
    NetcardDpAdded = FALSE;
    for (Count = 0; Count < AddedNetCount; Count++) {
      if (UefiCompareDevicePath(DevicePath, mNetcardInfo[Count].DevicePath)) {
        NetcardDpAdded = TRUE;
        break;
      }
    }
    if (NetcardDpAdded) {
      continue;
    }

    //
    // Add DevicePath of remaining  SNP netcardss
    //
    if (mNetCount == mNetcardCount) {
      mNetcardInfo = (NETCARD_INFO *)ReallocatePool(
                                                      sizeof(NETCARD_INFO) * mNetcardCount,
                                                      sizeof(NETCARD_INFO) * (mNetcardCount + INCREMENT_OF_COUNT),
                                                      mNetcardInfo);
      ASSERT(mNetcardInfo != NULL);
      mNetcardCount = mNetcardCount + INCREMENT_OF_COUNT;
    }
    mNetcardInfo[mNetCount].DevicePath = DuplicateDevicePath(DevicePath);
    ASSERT(mNetcardInfo[mNetCount].DevicePath != NULL);
    mNetcardInfo[mNetCount].Active = FALSE;
    mNetcardInfo[mNetCount].PciIo = NULL;
    mNetcardInfo[mNetCount].NetNameString = NULL;
    mNetCount++;
  }

  if (Handles != NULL) {
    FreePool(Handles);
  }
}

VOID
CreateNetOptions (
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION **UpdatedBootOptions,
  IN OUT UINTN                        *UpdatedBootOptionsCount
  )
{
  EFI_STATUS                Status;
  UINTN                     Index;
  UINTN                     Count;
  UINTN                     NetIndex;
  CHAR16                    *IpTypeStr = L"";
  CHAR16                    *BootTypeStr;
  CHAR16                    *DevStr;
  CHAR16                    *Description;
  UINTN                     Size;
  UINT8                     NetBootIp4Ip6;
  IPv4_DEVICE_PATH          *Ipv4Dp;
  IPv6_DEVICE_PATH          *Ipv6Dp;
  URI_DEVICE_PATH           *UriDp = NULL;
  UINT8                     Mac[6];
  MAC_ADDR_DEVICE_PATH      *MacDp;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathBackup;
  EFI_DEVICE_PATH_PROTOCOL  *PxeDevicePathBackup;
  BOOLEAN                   NeedAddMacNode;
  UINTN                     SameMacOptionCount = 1;
  UINTN                     SameIpOptionCount = 1;
  BOOLEAN                   IpControlUseUsage2 = FALSE;

  NetBootIp4Ip6 = PcdGet8(PcdNetBootIp4Ip6);

  if ((NetBootIp4Ip6 & ENABLE_SPEPARATE_CONTROL_NET_TYPE) == 0) {
    if ((NetBootIp4Ip6 != NET_BOOT_IP_ALL) && (NetBootIp4Ip6 != NET_BOOT_IP_V4) && (NetBootIp4Ip6 != NET_BOOT_IP_V6)) {
      DEBUG((EFI_D_INFO, "All IP closed, no need to create net options %a\n", __FUNCTION__));
      return;
    }
  } else {
    IpControlUseUsage2 = TRUE;
    if (((NetBootIp4Ip6 & ENABLE_PXE_IPV4) == 0) && ((NetBootIp4Ip6 & ENABLE_PXE_IPV6) == 0)
      && ((NetBootIp4Ip6 & ENABLE_HTTP_IPV4) == 0) && ((NetBootIp4Ip6 & ENABLE_HTTP_IPV6) == 0)) {
      DEBUG((EFI_D_INFO, "All IP closed, no need to create net options %a\n", __FUNCTION__));
      return;
    }
  }

  if ((!IpControlUseUsage2 && (NetBootIp4Ip6 == NET_BOOT_IP_ALL))
    || (IpControlUseUsage2 && (((NetBootIp4Ip6 & ENABLE_PXE_IPV4) != 0) || ((NetBootIp4Ip6 & ENABLE_HTTP_IPV4) != 0))
    && (((NetBootIp4Ip6 & ENABLE_PXE_IPV6) != 0) || ((NetBootIp4Ip6 & ENABLE_HTTP_IPV6) != 0)))) {
    SameMacOptionCount = 2; // one port of one netcard has two PXE options:PXE IPv4 and PXE IPv6
  }

  if ((!IpControlUseUsage2 && PcdGetBool(PcdNetBootHttpEnable))
    || (IpControlUseUsage2 && (((NetBootIp4Ip6 & ENABLE_HTTP_IPV4) != 0) || ((NetBootIp4Ip6 & ENABLE_HTTP_IPV6) != 0)))) { // support http
    SameIpOptionCount = 2; // Two more net options of one port of one netcard need to be added:HTTP IPv4 and HTTP IPv6

    //
    // Creat Uri DevicePath for HTTP net option,and attention:
    // UEFI Spec doesn't require the URI to be a NULL-terminated string
    // So we allocate a new buffer and always append a '\0' to it.
    //
    UriDp = (URI_DEVICE_PATH *) CreateDeviceNode (
                                 MESSAGING_DEVICE_PATH,
                                 MSG_URI_DP,
                                 (UINT16) (sizeof (URI_DEVICE_PATH))
                                 );
    ASSERT(UriDp != NULL);
  }
  //
  // Init MAC node, IPv4 node and IPv6 node
  //
  MacDp = (MAC_ADDR_DEVICE_PATH *) CreateDeviceNode (MESSAGING_DEVICE_PATH, MSG_MAC_ADDR_DP, (UINT16) sizeof (MAC_ADDR_DEVICE_PATH));
  ASSERT(MacDp != NULL);

  //
  // Now only onboard Wangxun netcard of KH40000Crb need to create MAC node,and its IfType is 0x01.
  // Later attention:different devices has different values,like IfType of Intel netcards is 0.
  //
  MacDp->IfType = 0x01;

  Ipv4Dp = (IPv4_DEVICE_PATH *)CreateDeviceNode(MESSAGING_DEVICE_PATH, MSG_IPv4_DP, (UINT16) sizeof (IPv4_DEVICE_PATH));
  ASSERT(Ipv4Dp != NULL);
  Ipv6Dp = (IPv6_DEVICE_PATH *)CreateDeviceNode(MESSAGING_DEVICE_PATH, MSG_IPv6_DP, (UINT16) sizeof (IPv6_DEVICE_PATH));
  ASSERT(Ipv6Dp != NULL);
  Ipv6Dp->PrefixLength = 0x40;

  for (Index = 0; Index < mNetCount; Index++) {
    if (mNetcardInfo[Index].Active) {
      continue;
    }

    NeedAddMacNode = FALSE;
    ZeroMem(MacDp->MacAddress.Addr, sizeof(MacDp->MacAddress.Addr));

    //
    // Create FilePath of net option
    //
    DevicePath = mNetcardInfo[Index].DevicePath;
    while (!IsDevicePathEnd (DevicePath) &&
         ((DevicePathType (DevicePath) != MESSAGING_DEVICE_PATH) ||
          (DevicePathSubType (DevicePath) != MSG_MAC_ADDR_DP))
         ) {
      DevicePath = NextDevicePathNode (DevicePath);
    }
    if (IsDevicePathEnd(DevicePath)) {
      NeedAddMacNode = TRUE;
    }

    DevicePath = DuplicateDevicePath(mNetcardInfo[Index].DevicePath);
    if (NeedAddMacNode) {
      Status = GetOnboardLanMacAddress(gBS, mNetcardInfo[Index].DevicePath, Mac);
      if (!EFI_ERROR(Status)) {
        CopyMem(&(MacDp->MacAddress.Addr[0]), Mac, 6);
      }
      DevicePath = AppendDevicePathNode(mNetcardInfo[Index].DevicePath, (EFI_DEVICE_PATH_PROTOCOL *)MacDp);
      if (DevicePath == NULL) {
        continue;
      }
    }

    DevicePathBackup = DevicePath; // backup the DevicePath with MAC
    if (mNetcardInfo[Index].NetNameString != NULL) {
      DevStr = (CHAR16 *)AllocateCopyPool(StrSize(mNetcardInfo[Index].NetNameString), mNetcardInfo[Index].NetNameString);
      ASSERT(DevStr != NULL);
    } else {
      DevStr = GetNameStringOfNetcard(mNetcardInfo[Index].DevicePath, TRUE);
    }

    for (Count = 0; Count < SameMacOptionCount; Count++) {
      if ((Count == 1) && (SameMacOptionCount == 2)) { // IPAll
        DevicePath = AppendDevicePathNode(DevicePathBackup, (EFI_DEVICE_PATH_PROTOCOL *)Ipv6Dp);
        IpTypeStr = L"IPv6";
      } else {
        if ((!IpControlUseUsage2 && (NetBootIp4Ip6 == NET_BOOT_IP_V4)) || (SameMacOptionCount == 2)
          || (IpControlUseUsage2 && (((NetBootIp4Ip6 & ENABLE_PXE_IPV4) != 0) || ((NetBootIp4Ip6 & ENABLE_HTTP_IPV4) != 0)))) {
          DevicePath = AppendDevicePathNode(DevicePathBackup, (EFI_DEVICE_PATH_PROTOCOL *)Ipv4Dp);
          IpTypeStr = L"IPv4";
        } else if ((!IpControlUseUsage2 && (NetBootIp4Ip6 == NET_BOOT_IP_V6))
          || (IpControlUseUsage2 && (((NetBootIp4Ip6 & ENABLE_PXE_IPV6) != 0) || ((NetBootIp4Ip6 & ENABLE_HTTP_IPV6) != 0)))) {
          DevicePath = AppendDevicePathNode(DevicePathBackup, (EFI_DEVICE_PATH_PROTOCOL *)Ipv6Dp);
          IpTypeStr = L"IPv6";
        } else {
          continue;
        }
      }
      if (DevicePath == NULL) {
        break;
      }
      PxeDevicePathBackup = DevicePath;
      for (NetIndex = 0; NetIndex < SameIpOptionCount; NetIndex++) {
        if ((!IpControlUseUsage2 && (NetIndex == 0))
          || (IpControlUseUsage2 && (NetIndex == 0) && (((StrStr(IpTypeStr, L"IPv4") != NULL) && ((NetBootIp4Ip6 & ENABLE_PXE_IPV4) != 0))
          || ((StrStr(IpTypeStr, L"IPv6") != NULL) && ((NetBootIp4Ip6 & ENABLE_PXE_IPV6) != 0))))) { // add PXE net options
          BootTypeStr = L"PXE";
          DevicePath = DuplicateDevicePath(PxeDevicePathBackup);
        } else if ((!IpControlUseUsage2 && (NetIndex == 1))
          || (IpControlUseUsage2 && (NetIndex == 1) && (((StrStr(IpTypeStr, L"IPv4") != NULL) && ((NetBootIp4Ip6 & ENABLE_HTTP_IPV4) != 0))
          || ((StrStr(IpTypeStr, L"IPv6") != NULL) && ((NetBootIp4Ip6 & ENABLE_HTTP_IPV6) != 0))))) { // add http net options
          BootTypeStr = L"HTTP";

          //
          // Append Uri() DevicePath
          //
          DevicePath = AppendDevicePathNode(PxeDevicePathBackup, (EFI_DEVICE_PATH_PROTOCOL *)UriDp);
          if (DevicePath == NULL) {
            break;
          }
        } else {
          continue;
        }

        //
        // Init Description of Net option
        //
        Size = StrSize(L"UEFI") + (StrLen(DevStr) + 1) * sizeof(CHAR16) + StrSize(BootTypeStr) + StrSize(IpTypeStr);
        Description = (CHAR16 *)AllocateZeroPool(Size);
        ASSERT(Description != NULL);
        UnicodeSPrint (
          Description, 
          Size, 
          L"UEFI %s %s %s", 
          DevStr, 
          BootTypeStr,
          IpTypeStr
          );
        //
        // Add Net options
        //
        *UpdatedBootOptions = (EFI_BOOT_MANAGER_LOAD_OPTION *)ReallocatePool(sizeof(EFI_BOOT_MANAGER_LOAD_OPTION) * (*UpdatedBootOptionsCount),
                                                               sizeof(EFI_BOOT_MANAGER_LOAD_OPTION) * (*UpdatedBootOptionsCount + 1),
                                                               *UpdatedBootOptions
                                                               );
        ASSERT(*UpdatedBootOptions != NULL);
        Status = EfiBootManagerInitializeLoadOption (
                     &(*UpdatedBootOptions)[*UpdatedBootOptionsCount],
                     LoadOptionNumberUnassigned,
                     LoadOptionTypeBoot,
                     LOAD_OPTION_ACTIVE,
                     Description,
                     DevicePath,
                     NULL,
                     0
                     );
        ASSERT_EFI_ERROR (Status);
        DEBUG((EFI_D_INFO, "Add Net Option:%s %a line=%d\n", (*UpdatedBootOptions)[*UpdatedBootOptionsCount].Description, __FUNCTION__, __LINE__));
        ShowDevicePathDxe(gBS, (*UpdatedBootOptions)[*UpdatedBootOptionsCount].FilePath);
        (*UpdatedBootOptionsCount)++;
        if (Description != NULL) {
          FreePool(Description);
        }
        if (DevicePath != NULL) {
          FreePool(DevicePath);
        }
      }
      if (PxeDevicePathBackup != NULL) {
        FreePool(PxeDevicePathBackup);
      }
    }
    if (DevStr != NULL) {
      FreePool(DevStr);
    }
    if (DevicePathBackup != NULL) {
      FreePool(DevicePathBackup);
    }
  }

  if (MacDp != NULL) {
    FreePool(MacDp);
  }

  if (Ipv4Dp != NULL) {
    FreePool(Ipv4Dp);
  }

  if (Ipv6Dp != NULL) {
    FreePool(Ipv6Dp);
  }

  if (UriDp != NULL) {
    FreePool(UriDp);
  }
}

VOID
AddUnconnectedNetOptions (
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION **UpdatedBootOptions,
  IN OUT UINTN                        *UpdatedBootOptionsCount
  )
{
  UINTN                         Index;
  UINTN                         Count;
  UINTN                         OldOptionsCount;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;

  //
  // Create all DevicePath of netcard with OptionRom
  //
  CreateNetDevicePath();

  //
  // Check if UEFI onboard net options exist, and build UEFI onboard net options.
  //
  OldOptionsCount = *UpdatedBootOptionsCount;
  for (Count = 0; Count < mNetCount; Count++) {
    DevicePath = mNetcardInfo[Count].DevicePath;
    for (Index = 0; Index < OldOptionsCount; Index++) {
      if (GetEfiBootGroupType((*UpdatedBootOptions)[Index].FilePath) != BM_MENU_TYPE_UEFI_PXE) {
        continue;
      }
      if (CompareMem(DevicePath, (*UpdatedBootOptions)[Index].FilePath, GetDevicePathSize(DevicePath) - END_DEVICE_PATH_LENGTH) == 0) {
        break;
      }
    }
    if (Index < OldOptionsCount) { // exist
      mNetcardInfo[Count].Active = TRUE;
      continue;
    }
  }

  CreateNetOptions(UpdatedBootOptions, UpdatedBootOptionsCount);
  for (Index = 0; Index < mNetCount; Index++) {
    if (mNetcardInfo[Index].DevicePath != NULL) {
      FreePool(mNetcardInfo[Index].DevicePath);
    }
    if (mNetcardInfo[Index].NetNameString != NULL) {
      FreePool(mNetcardInfo[Index].NetNameString);
    }
  }
  if (mNetcardInfo != NULL) {
    FreePool(mNetcardInfo);
    mNetcardInfo = NULL;
    mNetcardCount = MAX_VALUE_OF_COUNT;
    mNetCount = 0;
  }
}

VOID
CorrectIfNumOfPciNetOptions (
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION    *BootOptions,
  IN  CONST UINTN                        BootOptionsCount
  )
{
  EFI_STATUS                Status;
  UINTN                     Index, Count;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_HANDLE                Handle;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  UINTN                     Seg, Bus, Device, Function;
  NETCARD_PORT_INFO         *NetPortsInfo = NULL;
  UINTN                     NetcardCount = 0;
  EFI_NETWORK_INTERFACE_IDENTIFIER_PROTOCOL  *Nii = NULL;
  UINT16                    IfNum = 0;
  UINTN                     OptionIndex, Size;
  CHAR16                    *DevStr;
  UINT8                     IpType;
  CHAR16                    *IpTypeStr;
  CHAR16                    *BootTypeStr;
  CHAR16                    *String;
  CHAR16                    UefiPrefix[] = L"UEFI";

  if ((BootOptions == NULL) || (BootOptionsCount == 0)) {
    return;
  }

  NetPortsInfo = (NETCARD_PORT_INFO *)AllocateZeroPool(sizeof(NETCARD_PORT_INFO) * BootOptionsCount);
  ASSERT(NetPortsInfo != NULL);

  //
  // Go through all boot options to find out all UEFI PCI net options and get their information to fill NetPortsInfo:
  // 1) The UEFI PCI net options which are from the same netcard have the same PCI Bus Number and Device Number;
  // 2) Record the minimum IfNum, numbers of UEFI net options from the same PCI netcard, IfNum value and Index;
  //
  for (Index = 0; Index < BootOptionsCount; Index++) {
    if ((GetEfiBootGroupType(BootOptions[Index].FilePath) != BM_MENU_TYPE_UEFI_PXE) || (BootOptions[Index].OptionNumber == DELETE_FLAG)) {
      continue;
    }

    DevicePath = BootOptions[Index].FilePath;
    Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &DevicePath, &Handle);
    if (EFI_ERROR(Status)) {
      continue;
    }
    Status = gBS->HandleProtocol (Handle, &gEfiPciIoProtocolGuid, (VOID **) &PciIo);
    if (EFI_ERROR(Status)) {
      continue;
    }

    Bus = 0xFFFF;
    Device = 0xFFFF;
    PciIo->GetLocation(PciIo, &Seg, &Bus, &Device, &Function);
    IfNum = 0;
    DevicePath = BootOptions[Index].FilePath;
    Status = gBS->LocateDevicePath(&gEfiNetworkInterfaceIdentifierProtocolGuid_31, &DevicePath, &Handle);
    if (!EFI_ERROR(Status)) {
      gBS->HandleProtocol(Handle, &gEfiNetworkInterfaceIdentifierProtocolGuid_31, (VOID**)&Nii);
      if (Nii != NULL) {
        IfNum = Nii->IfNum;
      }
    }

    //
    // Check if the netcard of the current net option has been recorded in NetPortsInfo.
    // If yes, add IfNum to IfNums, add Index to NetIndex, and Update MinIfNum and Count;
    // If no, add a new record to NetPortsInfo to record the information of new netcard
    // and current net option.
    //
    for (Count = 0; Count < NetcardCount; Count++) {
      if ((Bus == NetPortsInfo[Count].Bus) && (Device == NetPortsInfo[Count].Device) && (Seg == NetPortsInfo[Count].Segment)) {
        if (IfNum < NetPortsInfo[Count].MinIfNum) {
          NetPortsInfo[Count].MinIfNum = IfNum;
        }
        NetPortsInfo[Count].NetIndex[NetPortsInfo[Count].Count] = Index;
        NetPortsInfo[Count].IfNums[NetPortsInfo[Count].Count] = IfNum;
        NetPortsInfo[Count].Count++;
        break;
      }
    }
    if (Count == NetcardCount) { // not found, new net option from new netcard
      NetPortsInfo[NetcardCount].Bus = Bus;
      NetPortsInfo[NetcardCount].Device = Device;
      NetPortsInfo[NetcardCount].Segment = Seg;
      NetPortsInfo[NetcardCount].MinIfNum = IfNum;
      NetPortsInfo[NetcardCount].NetIndex = (UINTN *)AllocateZeroPool(sizeof(UINTN) * BootOptionsCount);
      ASSERT(NetPortsInfo[NetcardCount].NetIndex != NULL);
      NetPortsInfo[NetcardCount].IfNums = (UINT16 *)AllocateZeroPool(sizeof(UINT16) * BootOptionsCount);
      ASSERT(NetPortsInfo[NetcardCount].IfNums != NULL);
      NetPortsInfo[NetcardCount].NetIndex[0] = Index;
      NetPortsInfo[NetcardCount].IfNums[0] = IfNum;
      NetPortsInfo[NetcardCount].Count = 1;
      NetcardCount++;
    }
  }

  //
  // If MinIfNum is not 0 for a netcard, the IfNum of all UEFI net options from this netcard should
  // be adjusted to count from 0.And regenerate the Description for these UEFI net options based on
  // the adjusted new IfNum value.
  //
  for (Count = 0; Count < NetcardCount; Count++) {
    if (NetPortsInfo[Count].MinIfNum == 0) {
      continue;
    }
    for (Index = 0; Index < NetPortsInfo[Count].Count; Index++) {
      IfNum = NetPortsInfo[Count].IfNums[Index] - NetPortsInfo[Count].MinIfNum;
      //
      // Adjust the IfNum in Description
      //
      OptionIndex = NetPortsInfo[Count].NetIndex[Index];
      IpType = GetEfiNetWorkType(BootOptions[OptionIndex].FilePath, &BootTypeStr);
      DevStr = GetNameStringOfNetcard(BootOptions[OptionIndex].FilePath, TRUE);
      if (IfNum != 0) {
        AppendIfNumToNameString(&DevStr, Function, IfNum);
      }

      switch (IpType) {
        case MSG_IPv4_DP:
          IpTypeStr = L"IPv4";
          break;
        case MSG_IPv6_DP:
          IpTypeStr = L"IPv6";
          break;
        default:
          IpTypeStr = L"";
          break;
      }
      Size = StrSize(UefiPrefix) + StrSize(DevStr) + StrSize(BootTypeStr) + (StrLen(IpTypeStr) + 1) * sizeof(CHAR16);
      String = (CHAR16 *)AllocatePool(Size);
      ASSERT(String != NULL);
      UnicodeSPrint (
        String, 
        Size,
        L"%s %s %s %s",
        UefiPrefix,
        DevStr, 
        BootTypeStr,
        IpTypeStr
        );
      FreePool(DevStr);
      FreePool(BootOptions[OptionIndex].Description);
      BootOptions[OptionIndex].Description = String;
    }
  }

  if (NetPortsInfo != NULL) {
    for (Index = 0; Index < NetcardCount; Index++) {
      if (NetPortsInfo[Index].IfNums != NULL) {
        FreePool(NetPortsInfo[Index].IfNums);
      }
      if (NetPortsInfo[Index].NetIndex != NULL) {
        FreePool(NetPortsInfo[Index].NetIndex);
      }
    }
    FreePool(NetPortsInfo);
  }
}

EFI_STATUS
EFIAPI
PlatformRefreshAllBootOptions (
  IN  CONST EFI_BOOT_MANAGER_LOAD_OPTION *BootOptions,
  IN  CONST UINTN                        BootOptionsCount,
  OUT       EFI_BOOT_MANAGER_LOAD_OPTION **UpdatedBootOptions,
  OUT       UINTN                        *UpdatedBootOptionsCount
  )
{
  EFI_STATUS                Status;
  UINTN                     Index;
  PLAT_HOST_INFO_PROTOCOL   *HostInfo;
  UINTN                     *BootOptionOrder = NULL;
  UINTN                     BufferSize;

  Status = EFI_NOT_READY;

  *UpdatedBootOptions = (EFI_BOOT_MANAGER_LOAD_OPTION *)AllocateZeroPool(sizeof(EFI_BOOT_MANAGER_LOAD_OPTION) * BootOptionsCount);
  ASSERT(UpdatedBootOptions != NULL);

  *UpdatedBootOptionsCount = BootOptionsCount;

  for (Index = 0; Index < BootOptionsCount; Index++) {
    Status = DuplicateBootOption(*UpdatedBootOptions + Index, BootOptions + Index);
    if (EFI_ERROR(Status)) {
      DEBUG ((EFI_D_INFO, "[Bds] Failed to refresh boot options!\n"));
      EfiBootManagerFreeLoadOptions (*UpdatedBootOptions, BootOptionsCount);
      return Status;
    }
  }

  //
  // Mark invalid options
  //
  MarkInvalidOptions(*UpdatedBootOptions, BootOptionsCount);

  //
  // Processing the boot description according to the platform specifications
  //
  ProcessBootDescriptions(*UpdatedBootOptions, BootOptionsCount);

  //
  // Find more os loader on the same EFI partition of current HDD option
  //
  FindMoreOsLoaders(*UpdatedBootOptions, BootOptionsCount);

  //
  // Append OS string on the fixed BlockIo
  //
  AppenOsDescriptionOnFixedDevice(*UpdatedBootOptions, BootOptionsCount);

  //
  // Add more harddisk options with multiple Os Loaders
  // and delete the old one enumerated before.
  //
  AdjustBootOptions(UpdatedBootOptions, UpdatedBootOptionsCount);

  if (PcdGetBool(PcdAlwaysCreatNetOption)) {
    if (PcdGet8(PcdBiosBootModeType) != BIOS_BOOT_LEGACY_OS) {
      AddUnconnectedNetOptions(UpdatedBootOptions, UpdatedBootOptionsCount);
    }
  }

  //
  // Update boot device for the platform
  //
  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, (VOID **)&HostInfo);
  if (!EFI_ERROR(Status) && HostInfo->UpdateBootOption != NULL) {
    HostInfo->UpdateBootOption((VOID **)UpdatedBootOptions, UpdatedBootOptionsCount);
  }

  //
  // Check and correct the IfNum in Description of UEFI PCI net options
  //
  CorrectIfNumOfPciNetOptions(*UpdatedBootOptions, *UpdatedBootOptionsCount);

  //
  // Update the Attribute of options according to L"BootOptionDisable"
  //
  InactiveDisabledOptions(*UpdatedBootOptions, *UpdatedBootOptionsCount);

  //
  // sorting new options as default bootorder
  //
  BufferSize = sizeof(UINTN) * *UpdatedBootOptionsCount;
  BootOptionOrder = (UINTN *)AllocateZeroPool(BufferSize);
  ASSERT(BootOptionOrder != NULL);

  ByoSortBootOptions(NULL, 0, (*UpdatedBootOptions), *UpdatedBootOptionsCount, NULL);
  for (Index =0; Index < *UpdatedBootOptionsCount; Index++) {
    BootOptionOrder[Index] = (*UpdatedBootOptions)[Index].OptionNumber;
  }
  gRT->SetVariable (
                    L"BootOptionOrder",
                    &gEdkiiBdsVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    BufferSize,
                    BootOptionOrder
                    );

  //
  // The boot option has been enumerated and refresh.
  //
  PcdSet32S(PcdSystemMiscConfig, PcdGet32(PcdSystemMiscConfig) | SYS_MISC_CFG_UPD_BOOTORDER);

  return EFI_SUCCESS;
}