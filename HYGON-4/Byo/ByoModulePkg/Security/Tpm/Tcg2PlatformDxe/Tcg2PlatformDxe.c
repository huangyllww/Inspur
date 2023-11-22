/** @file
  Platform specific TPM2 component for configuring the Platform Hierarchy.

  Copyright (c) 2017 - 2022, Byosoft Corporation. All rights reserved.<BR>
  This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.

**/

#include "Tcg2PlatformDxe.h"

///
/// TPM Device (Type 43) data
///
GLOBAL_REMOVE_IF_UNREFERENCED SMBIOS_TABLE_TYPE43 SmbiosTableType43Data = {
  { SMBIOS_TYPE_TPM_DEVICE, sizeof (SMBIOS_TABLE_TYPE43), 0 },
  {
    TO_BE_FILLED, ///< VendorID0
    TO_BE_FILLED, ///< VendorID1
    TO_BE_FILLED, ///< VendorID2
    TO_BE_FILLED  ///< VendorID3
  },
  TO_BE_FILLED, ///< MajorSpecVersion
  TO_BE_FILLED, ///< MinorSpecVersion
  TO_BE_FILLED, ///< FirmwareVersion1
  TO_BE_FILLED, ///< FirmwareVersion2
  TO_BE_FILLED, ///< Description
  TO_BE_FILLED, ///< Characteristics
  TO_BE_FILLED, ///< OEM Defined
};

GLOBAL_REMOVE_IF_UNREFERENCED SMBIOS_TYPE43_STRING_ARRAY SmbiosTableType43Strings = {
  TO_BE_FILLED_STRING   ///< Description
};

/**
   Installs the SMBIOS TPM Device Information type 43

   @retval EFI_UNSUPPORTED      - Could not locate SMBIOS protocol
   @retval EFI_OUT_OF_RESOURCES - Failed to allocate memory for SMBIOS TPM Device Information type.
   @retval EFI_SUCCESS          - Successfully installed SMBIOS TPM Device Information type.
**/
EFI_STATUS
EFIAPI
AddSmbiosTpmDeviceTable (
  VOID
  )
{
  EFI_STATUS              Status;
  CHAR8                   *StringBuffer;
  UINTN                   StringBufferLength;
  UINTN                   Size;
  UINTN                   i;
  UINT32                  ManufacturerID;
  UINT32                  TempManufacturerIDValue;
  UINT32                  FirmwareVersion1;
  UINT32                  FirmwareVersion2;
  EFI_SMBIOS_PROTOCOL     *SmbiosProtocol;
  EFI_SMBIOS_HANDLE       SmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER *Record;
  CHAR8                   *StringPtr;
  CHAR8                   **TableStrings;
  EFI_SMBIOS_TABLE_HEADER *Entry;

  DEBUG ((DEBUG_INFO, "Byo [SMBIOS] AddSmbiosTpmDeviceTable ()\n"));

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &SmbiosProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Error locating gEfiSmbiosProtocolGuid. Status = %r\n", Status));
    return Status;
  }

  //
  // TPM Description string buffer allocation
  //
  StringBufferLength = SMBIOS_STRING_MAX_LENGTH;
  StringBuffer = AllocateZeroPool (StringBufferLength * sizeof (CHAR8));

  if (StringBuffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    return Status;
  }

  //
  // TPM Type specific fields
  // ManufacturerID defined in TCG Vendor ID Registry.
  //
  Status = Tpm2GetCapabilityManufactureID (&ManufacturerID);
  SmbiosTableType43Data.VendorID[0] = (ManufacturerID >> 0)  & 0xFF;
  SmbiosTableType43Data.VendorID[1] = (ManufacturerID >> 8)  & 0xFF;
  SmbiosTableType43Data.VendorID[2] = (ManufacturerID >> 16) & 0xFF;

  //
  //   May be tailed with 0x00 (ASCII_NULL_CHAR) or 0x20 (ASCII_SPACE_CHAR)
  //
  TempManufacturerIDValue = (ManufacturerID >> 24);
  if ((TempManufacturerIDValue) == ASCII_NULL_CHAR || ((TempManufacturerIDValue) == ASCII_SPACE_CHAR)) {
    //  HID containing PNP ID "NNN####"
    SmbiosTableType43Data.VendorID[3] = ASCII_NULL_CHAR;
  } else {
    //  HID containing ACP ID "NNNN####"
    SmbiosTableType43Data.VendorID[3] = (TempManufacturerIDValue) & 0xFF;
  }

  //
  // Spec version for TPM 2.0 devices.
  //
  SmbiosTableType43Data.MajorSpecVersion = 2;
  SmbiosTableType43Data.MinorSpecVersion = 0;

  //
  // Firmware Version
  //
  Status = Tpm2GetCapabilityFirmwareVersion (&FirmwareVersion1, &FirmwareVersion2);
  SmbiosTableType43Data.FirmwareVersion1 = FirmwareVersion1;
  SmbiosTableType43Data.FirmwareVersion2 = FirmwareVersion2;

  // 
  // Format TPM details into Description string
  // 
  AsciiSPrint (
    StringBuffer,
    StringBufferLength,
    "TPM %d.%d, ManufacturerID: %c%c%c%c, Firmware Version: 0x%08x.0x%08x",
    SmbiosTableType43Data.MajorSpecVersion,
    SmbiosTableType43Data.MinorSpecVersion,
    SmbiosTableType43Data.VendorID[0],
    SmbiosTableType43Data.VendorID[1],
    SmbiosTableType43Data.VendorID[2],
    SmbiosTableType43Data.VendorID[3],
    SmbiosTableType43Data.FirmwareVersion1,
    SmbiosTableType43Data.FirmwareVersion2
    );

  SmbiosTableType43Data.Description = STRING_1;
  SmbiosTableType43Strings.Description = StringBuffer;

  //
  // TPM Characteristics
  //   Set bit 2 (TPM Characteristics are not supported)
  //
  SmbiosTableType43Data.Characteristics = BIT2;

  if (Status != EFI_SUCCESS) {
    FreePool (StringBuffer);
    return Status;
  }

  //
  // Generate SMBIOS TPM Device Record (Type 43)
  //
  Entry = (EFI_SMBIOS_TABLE_HEADER *) &SmbiosTableType43Data;
  TableStrings = (CHAR8 **) &SmbiosTableType43Strings;

  if (Entry == NULL) {
    FreePool (StringBuffer);
    return Status;
  }

  //
  // Calculate the total size of the full record
  //
  Size = Entry->Length;

  //
  // Add the size of each non-null string
  //
  if (TableStrings != NULL) {
    for (i = 0; i < SMBIOS_TYPE43_NUMBER_OF_STRINGS; i++) {
      if (TableStrings[i] != NULL) {
        Size += AsciiStrSize (TableStrings[i]);
      }
    }
  }

  //
  // Add the size of the terminating double null
  // If there were any strings added, just add the second null
  //
  if (Size == Entry->Length) {
    Size += 2;
  } else {
    Size += 1;
  }

  //
  // Initialize the full record
  //
  Record = (EFI_SMBIOS_TABLE_HEADER *) AllocateZeroPool (Size);
  if (Record == NULL) {
    FreePool (StringBuffer);
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (Record, Entry, Entry->Length);

  //
  // Copy the strings to the end of the record
  //
  StringPtr = ((CHAR8 *) Record) + Entry->Length;
  if (TableStrings != NULL) {
    for (i = 0; i < SMBIOS_TYPE43_NUMBER_OF_STRINGS; i++) {
      if (TableStrings[i] != NULL) {
        AsciiStrCpyS (StringPtr, Size - Entry->Length, TableStrings[i]);
        StringPtr += AsciiStrSize (TableStrings[i]);
        Size -= AsciiStrSize (TableStrings[i]);
      }
    }
  }

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = SmbiosProtocol->Add (SmbiosProtocol, NULL, &SmbiosHandle, Record);

  FreePool (Record);
  FreePool (StringBuffer);
  return Status;
}

/**
   This callback function will run at the SmmReadyToLock event.

   Configuration of the TPM's Platform Hierarchy Authorization Value (platformAuth)
   and Platform Hierarchy Authorization Policy (platformPolicy) can be defined through this function.
   And add smbios Type 43.

  @param  Event   Pointer to this event
  @param  Context Event handler private data
 **/
VOID
EFIAPI
SmmReadyToLockEventCallBack (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS   Status;
  VOID         *Interface;

  //
  // Try to locate it because EfiCreateProtocolNotifyEvent will trigger it once when registration.
  // Just return if it is not found.
  //
  Status = gBS->LocateProtocol (
                  &gEfiDxeSmmReadyToLockProtocolGuid,
                  NULL,
                  &Interface
                  );
  if (EFI_ERROR (Status)) {
    return ;
  }

  ConfigureTpmPlatformHierarchy ();

  //
  // Add Smbios type 43 table.
  //
  Status = AddSmbiosTpmDeviceTable ();
  DEBUG ((DEBUG_INFO, "AddSmbiosTpmDeviceTable(): Exit - Status = %r\n", Status));

  gBS->CloseEvent (Event);
}

/**
   The driver's entry point. Will register a function for callback during SmmReadyToLock event to
   configure the TPM's platform authorization.

   @param[in] ImageHandle  The firmware allocated handle for the EFI image.
   @param[in] SystemTable  A pointer to the EFI System Table.

   @retval EFI_SUCCESS     The entry point is executed successfully.
   @retval other           Some error occurs when executing this entry point.
**/
EFI_STATUS
EFIAPI
Tcg2PlatformDxeEntryPoint (
  IN    EFI_HANDLE                  ImageHandle,
  IN    EFI_SYSTEM_TABLE            *SystemTable
  )
{
  VOID       *Registration;
  EFI_EVENT  Event;

  Event = EfiCreateProtocolNotifyEvent (
            &gEfiDxeSmmReadyToLockProtocolGuid,
            TPL_CALLBACK,
            SmmReadyToLockEventCallBack,
            NULL,
            &Registration
            );

  ASSERT (Event != NULL);

  return EFI_SUCCESS;
}
