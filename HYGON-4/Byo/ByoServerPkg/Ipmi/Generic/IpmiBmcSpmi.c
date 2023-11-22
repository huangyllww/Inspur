/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include "IpmiBmc.h"
#include <Library/BaseMemoryLib.h>
#include <Protocol/AcpiTable.h>
#include <IndustryStandard/Acpi.h>


#ifndef EFI_ACPI_HEADER

#define EFI_ACPI_OEM_ID           'X','8','6','_','H','G'   // OEMID 6
#define EFI_ACPI_OEM_TABLE_ID     SIGNATURE_64('E', 'D', 'K', '2', ' ', ' ', ' ', ' ')
#define EFI_ACPI_OEM_REVISION     0x00000002
#define EFI_ACPI_CREATOR_ID       0x20202020
#define EFI_ACPI_CREATOR_REVISION 0x01000013

// A macro to initialise the common header part of EFI ACPI tables as defined by
// EFI_ACPI_DESCRIPTION_HEADER structure.
#define EFI_ACPI_HEADER(Signature, Type, Revision) {              \
    Signature,                      /* UINT32  Signature */       \
    sizeof (Type),                  /* UINT32  Length */          \
    Revision,                       /* UINT8   Revision */        \
    0,                              /* UINT8   Checksum */        \
    {EFI_ACPI_OEM_ID },         /* UINT8   OemId[6] */        \
    EFI_ACPI_OEM_TABLE_ID,      /* UINT64  OemTableId */      \
    EFI_ACPI_OEM_REVISION,      /* UINT32  OemRevision */     \
    EFI_ACPI_CREATOR_ID,        /* UINT32  CreatorId */       \
    EFI_ACPI_CREATOR_REVISION   /* UINT32  CreatorRevision */ \
  } 
#endif


#define SPMI_DESCRIPTION_TABLE_REVISION 0x5

#pragma pack(1)
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER Header;
  UINT8                       InterfaceType;
  UINT8                       Reserved1;
  UINT16                      Reversion;
  UINT8                       InterruptType;
  UINT8                       Gpe;
  UINT8                       Reserved2;
  UINT8                       PciDeviceFlag;
  UINT32                      GlobalInterrupt;
  UINT8                       BaseAddress[12];
  union {
    struct {
      UINT8 Segment;
      UINT8 Bus;
      UINT8 Device;
      UINT8 Function;
    } Pci;
    UINT8 Uid[4];
  }                           PciOrUid;
  UINT8                       Reserved3;
} SPMI_DESCRIPTION_TABLE;
#pragma pack ()

SPMI_DESCRIPTION_TABLE  mSpmi = {
  EFI_ACPI_HEADER (
    EFI_ACPI_6_0_SERVER_PLATFORM_MANAGEMENT_INTERFACE_TABLE_SIGNATURE,
    SPMI_DESCRIPTION_TABLE,
    SPMI_DESCRIPTION_TABLE_REVISION
  ),
  FixedPcdGet8(PcdIpmiInterfaceType),  // InterfaceType
  1,  // This field must always be 1
  0x0200,  //ipmi 2.0
  0,  // InterruptType
  0,  // Gpe
  0,
  0,  // PciDeviceFlag
  0,  // GlobalInterrupt
  {
    0x01, 0x08, 0x00, 0x01,
    0xA2, 0x0C, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
  },  // BaseAddress[12]
  {
    {
      0
    }
  },  // PciOrUid
  0
};

/**
  Report SPMI if IPMI version >= 1.5.

  @param[in] Event    Event for which this notification function is being
                      called.
  @param[in] Context  Pointer to the ATA_ATAPI_PASS_THRU_INSTANCE that
                      represents the HBA.
**/
VOID
InstallSmpiTable (
  IN EFI_EVENT    Event,
  IN VOID*        Context
  )
{
  UINTN                         TableKey;
  EFI_STATUS                    Status;
  EFI_ACPI_TABLE_PROTOCOL*      AcpiTableProtocol = NULL;


  Status = gBS->LocateProtocol(&gEfiAcpiTableProtocolGuid, NULL, (VOID**)&AcpiTableProtocol);
  if (EFI_ERROR(Status)) {
    return;
  }

  gBS->CloseEvent (Event);

  mSpmi.Header.Checksum = CalculateCheckSum8((UINT8*)&mSpmi, mSpmi.Header.Length);
  Status = AcpiTableProtocol->InstallAcpiTable (AcpiTableProtocol, &mSpmi, mSpmi.Header.Length, &TableKey);
  DEBUG((EFI_D_INFO, "InstallSmpiTable %r\n", Status));
}


