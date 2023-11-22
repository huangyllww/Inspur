/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ByoBootManagerLib.h

Abstract:

Revision History:

**/
#ifndef __BYO_BOOT_MANAGER_LIB_H_
#define __BYO_BOOT_MANAGER_LIB_H_

#include <ByoBootGroup.h>
#include <Protocol/PciIo.h>
#include <Protocol/AtaPassThru.h>
#include <IndustryStandard/Atapi.h>
#include <Library/UefiBootManagerLib.h>

#define LENGTH_OF_DEVICE_NAME  128
#define GET_VENDOR_STR         0x00000001
#define GET_TYPE_STR           0x00000002
#define GET_DEFAULT_STR        0x80000000

#define EfiBootManagerGetVariableAndSize       BootManagerGetVariableAndSize
#define ByoEfiBootManagerFreeLoadOptions       EfiBootManagerFreeLoadOptions
#define ByoEfiBootManagerBoot                  EfiBootManagerBoot
#define ByoEfiBootManagerVariableToLoadOption  EfiBootManagerVariableToLoadOption
#define ByoEfiBootManagerGetLoadOptions        EfiBootManagerGetLoadOptions

EFI_STATUS
EFIAPI
CreateFvBootOption (
  EFI_GUID                     *FileGuid,
  CHAR16                       *Description,
  EFI_BOOT_MANAGER_LOAD_OPTION *BootOption,
  BOOLEAN                      IsBootCategory,
  UINT8                        *OptionalData,    OPTIONAL
  UINT32                       OptionalDataSize       
  );
typedef enum {
  AcpiFloppyBoot,
  MessageAtapiBoot,
  MessageSataBoot,
  MessageUsbBoot,
  MessageScsiBoot,
  MessageNetworkBoot,
  UnsupportedBoot
} BOOT_TYPE;


/**
  Get the NameString of netcard with SlotString and non-zero PCI Function number.
  The caller has responsibility to free it when the reval is not NULL.
  @param DevicePath                DevicePath of netcard, and it can't be NULL.
  @param NeedSlotString            If the NameString returned with SlotString.

  @return  The description string, and it's non-NULL.
 **/
CHAR16 *
EFIAPI
GetNameStringOfNetcard (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN BOOLEAN                   NeedSlotString
  );

/**
  Append non-zero IfNum to the end of the current net NameString in the form of L"-x".
  If the PCI Function Number is zero, must append it in the same form before appending IfNum.
 **/
VOID
EFIAPI
AppendIfNumToNameString (
  IN OUT CHAR16              **String,
  IN UINTN                   FunctionNumber,
  IN UINT16                  IfNum
  );

CHAR16 *
EFIAPI
GetPciVendorString (
  IN EFI_PCI_IO_PROTOCOL  *PciIo,
  IN UINT32                Attribute
  );


BM_MENU_TYPE
EFIAPI
GetEfiBootGroupType(
  EFI_DEVICE_PATH_PROTOCOL *FilePath
  );

UINT8
EFIAPI
GetEfiNetWorkType (
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  OUT CHAR16                     **TypeStr
  );

/**
  For a bootable Device path, return its boot type.

  @param  DevicePath                   The bootable device Path to check

  @retval AcpiFloppyBoot               If given device path contains ACPI_DEVICE_PATH type device path node
                                       which HID is floppy device.
  @retval MessageAtapiBoot             If given device path contains MESSAGING_DEVICE_PATH type device path node
                                       and its last device path node's subtype is MSG_ATAPI_DP.
  @retval MessageSataBoot              If given device path contains MESSAGING_DEVICE_PATH type device path node
                                       and its last device path node's subtype is MSG_SATA_DP.
  @retval MessageScsiBoot              If given device path contains MESSAGING_DEVICE_PATH type device path node
                                       and its last device path node's subtype is MSG_SCSI_DP.
  @retval MessageUsbBoot               If given device path contains MESSAGING_DEVICE_PATH type device path node
                                       and its last device path node's subtype is MSG_USB_DP.
  @retval MessageNetworkBoot           If given device path contains MESSAGING_DEVICE_PATH type device path node
                                       and its last device path node's subtype is MSG_MAC_ADDR_DP, MSG_VLAN_DP,
                                       MSG_IPv4_DP or MSG_IPv6_DP.
  @retval UnsupportedBoot              If given device path doesn't match the above condition, it's not supported.

**/
BOOT_TYPE
EFIAPI
BootTypeFromDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  );

CHAR16 *
EFIAPI
GetDescFromPci (
  EFI_HANDLE Handle,
  EFI_DEVICE_PATH_PROTOCOL *ptDevPath
  );

EFI_STATUS
EFIAPI
AtaReadIdentifyData (
  IN  EFI_ATA_PASS_THRU_PROTOCOL    *ptAtaPassThru,
  IN  UINT16                        Port,
  IN  UINT16                        PortMP,
  OUT ATA_IDENTIFY_DATA             *IdentifyData
  );

/**
  Initializes a BootOption instance.

  @param FileGuid          Pointer to the File Guid.
  @param Description       The description of the boot option.
  @param BootOption        Pointer to the BootOption.
  @param IsBootCategory    It is BootOption? TURE or FALSE.
  @param OptionalData      Optional data of the load option, else NULL.
  @param OptionalDataSize  Size of the optional data of the load option, else 0.

  @retval EFI_SUCCESS           The load option was initialized successfully.
  @retval other                 The load option was initialized unsuccessfully.
**/
EFI_STATUS
EFIAPI
CreateFvFileBootOption (
  IN  CONST EFI_GUID                    *FileGuid,
  IN CHAR16                             *Description,
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION   *BootOption,
  IN BOOLEAN                            IsBootCategory,
  IN UINT8                              *OptionalData,    OPTIONAL
  IN UINT32                             OptionalDataSize
  );

/**
  Registers Firmware volume with passed options.

  @param FileGuid          Pointer to the File Guid.
  @param Description       The description of the boot option.
  @param IsBootCategory    It is BootOption? TURE or FALSE.
  @param OptionalData      Optional data of the load option, else NULL.
  @param OptionalDataSize  Size of the optional data of the load option, else 0.
  @param OptionNumber      Entry in BootXXXX if replacing existing, else LoadOptionNumberUnassigned.

  @retval (INTN) NewOption.OptionNumber   The NewBootOption number.
**/
INTN
EFIAPI
RegisterFvFileBootOptionEx (
  IN  CONST EFI_GUID               *FileGuid,
  IN  CHAR16                       *Description,
  IN  BOOLEAN                      IsBootCategory,
  IN  UINT8                        *OptionalData,   OPTIONAL
  IN  UINT32                       OptionalDataSize,
  IN  UINTN                        OptionNumber
  );


/**
  Registers Firmware volume with passed options.

  @param FileGuid          Pointer to the File Guid.
  @param Description       The description of the boot option.
  @param IsBootCategory    It is BootOption? TURE or FALSE.
  @param OptionalData      Optional data of the load option, else NULL.
  @param OptionalDataSize  Size of the optional data of the load option, else 0.

  @retval (INTN) NewOption.OptionNumber   The NewBootOption number.

**/
INTN
EFIAPI
RegisterFvFileBootOption (
  IN  CONST EFI_GUID               *FileGuid,
  IN  CHAR16                       *Description,
  IN  BOOLEAN                      IsBootCategory,
  IN  UINT8                        *OptionalData,   OPTIONAL
  IN  UINT32                       OptionalDataSize
  );

/**
  Registers Windows Recovery Boot Option

  @param Description       The description of the boot option.

  @retval (INTN) NewOption.OptionNumber   The NewBootOption number.

**/
INTN
EFIAPI
RegisterWindowsRecoveryBootOption (
  IN  CHAR16                       *Description
  );

/**
  Read the EFI variable (VendorGuid/Name) and return a dynamically allocated
  buffer, and the size of the buffer. If failure return NULL.

  @param  Name                  String part of EFI variable name
  @param  VendorGuid            GUID part of EFI variable name
  @param  VariableSize          Returns the size of the EFI variable that was read

  @return                       Dynamically allocated memory that contains a copy of the EFI variable
                                Caller is responsible freeing the buffer.
  @retval NULL                  Variable was not read

**/
VOID *
EFIAPI
BootManagerGetVariableAndSize (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
  );

/**
  Build the BootTemp#### or DriverTemp#### option from the VariableName.

  @param  VariableName          EFI Variable name indicate if it is Boot#### or
                                Driver####

  @retval EFI_SUCCESS     Get the option just been created
  @retval EFI_NOT_FOUND   Failed to get the new option

**/
EFI_STATUS
EFIAPI
TempBootManagerVariableToLoadOption (
  IN  CHAR16                          *VariableName,
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION *Option  
  );

/**
  Sorting UEFI boot options.
  This function can be called after the first calling EfiBootManagerRefreshAllBootOption() to sort UEFI boot options.The
  order of Legacy boot options can not be changed(Legacy boot options are sorted in LegacyBootManagerLib).It will keep
  the order of NV boot options and insert new UEFI boot options according to sorting policy:
  1. Check if the PcdBiosBootModeType is BIOS_BOOT_LEGACY_OS, and return when it's BIOS_BOOT_LEGACY_OS;
  2. Check if boot options exist, and return when no boot options exist;
  3. Check if new boot options exist, and return when no new boot options.All old boot options were sorted at the last
  booting.
  4. Get sorting policy from L"CustomizedSortPolicy" variable defined in ByoCustomizedSortPolicy.h.
  The platform can customize the sorting policy by setting this variable, otherwise the default policy is used.
  5. Assign DevicePriority to all UEFI HDD and net options according to the sorting policy and hardware information,
  such as SATA port number, netcard slot number, PCI bus number and so on.
  6. Assign ItemOrder to all UEFI HDD and net options to make sure that they can be sorted as sorting policy.
  7. Call EfiBootManagerSortLoadOptionVariable() to actually sort all boot options and set to L"BootOrder".This step will
  sort the boot options into groups based on L"UefiBootGroupOrder" variable.

  @param  OldOptions            Boot options saved from the last booting, they are typically get before calling
                                EfiBootManagerRefreshAllBootOption().
  @param  OldOptionsCount       Count of OldOptions.
  @param  SortOptions           Boot options needed to be sorted.If it's NULL this function will sort boot options got from BootOrder in NV.
  @param  SortOptionsCount      Count of SortOptions.
  @param  CompareBootOption     The comparator function pointer.It can point to the function implemented by the platform
                                or NULL(use ByoCompareBootOption()) as CompareFunction parameter of
                                EfiBootManagerSortLoadOptionVariable().
**/
VOID
EFIAPI
ByoSortBootOptions (
  IN CONST EFI_BOOT_MANAGER_LOAD_OPTION  *OldOptions,
  IN UINTN                               OldOptionsCount,
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION    *SortOptions,
  IN CONST UINTN                         SortOptionsCount,
  IN SORT_COMPARE                        CompareBootOption
  );

/**
  Copy load option

  @param  Dst       Target option of copy
  @param  Src       Place to copy from

  @retval None
**/
VOID
EFIAPI
ByoCopyBootOption(
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION   *Dst,
  IN     EFI_BOOT_MANAGER_LOAD_OPTION   *Src
  );

/**
  Unload driver Image

  @param  FileGuid              The file Guid

  @retval EFI_NOT_FOUND         Can not find the file guid
  @retval other                 UnloadImage status
**/
EFI_STATUS 
EFIAPI
UnloadDriverWithFileGuid(
  EFI_GUID      *FileGuid
  );

/**
  Connect network driver

  @param None

  @retval None
**/
VOID
EFIAPI
ConnectNetworkDriverWithPciIo(
  VOID
  );

/**
  Disconnect network driver

  @param None

  @retval None
**/
VOID
EFIAPI
DisconnectNetworkDriverWithPciIo(
  VOID
  );

/**
  Delete boot group order and boot order variable

  @param None

  @retval None
**/
VOID
EFIAPI
ByoRemoveBootOrder(
  VOID
  );

#endif
