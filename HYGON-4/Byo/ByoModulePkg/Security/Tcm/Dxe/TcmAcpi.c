/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  TcmAcpi.c

Abstract:
  Acpi part of TCM Module.

Revision History:

**/

#include "TcmAcpi.h"
#include <Library/SafeMemLib.h>

/**
  Initialize and publish TPM items in ACPI table.

  @retval   EFI_SUCCESS     The TCG ACPI table is published successfully.
  @retval   Others          The TCG ACPI table is not published.

**/

EFI_STATUS
PublishAcpiTable (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_ACPI_TABLE_PROTOCOL        *AcpiTable;
  UINTN                          TableKey;
  EFI_ACPI_DESCRIPTION_HEADER    *Table;
  UINTN                          TableSize;

  AcpiTable = NULL;
  Table     = NULL;
  Status = GetSectionFromFv (
             &gEfiCallerIdGuid,
             EFI_SECTION_RAW,
             0,
             (VOID **) &Table,
             &TableSize
             );
  ASSERT_EFI_ERROR (Status);
  if (Table == NULL){
    DEBUG ((EFI_D_INFO, "%a line %d Error: Table == NULL.\n", __FUNCTION__, __LINE__));
    return EFI_NOT_FOUND;
  }
  DEBUG((EFI_D_ERROR,"GetSectionFromFv Status %x\n",TableSize));
  DEBUG((EFI_D_ERROR,"GetSectionFromFv Length %x\n",Table->Length));
  //
  // Measure to PCR[0] with event EV_POST_CODE ACPI DATA
  // TBD

/**
  TcmEvent->PCRIndex  = 0;
  TcmEvent->EventType = TCM_EV_POST_CODE;
  TcmEvent->EventSize = ACPI_DATA_LEN;

  Status = TcmDxeHashLogExtendEvent (
             &gTcmDxeData.TcmProtocol,
             Table,
             TableSize,
             TCM_ALG_SM3,
             TcmEvent,
             &EventNumber,
             NULL
             );
**/
  if (Table->OemTableId != SIGNATURE_64 ('T', 'c', 'm', 'T', 'a', 'b', 'l', 'e')) {
    return EFI_INVALID_PARAMETER;
  }

  Status = CopyMemBS (Table->OemId, sizeof (Table->OemId), PcdGetPtr (PcdAcpiDefaultOemId), sizeof (Table->OemId));
  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_INFO, "%a line %d CopyMemBS failed.\n", __FUNCTION__, __LINE__));
  }

  //
  // Publish the TPM ACPI table
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);
  ASSERT_EFI_ERROR (Status);
  DEBUG((EFI_D_ERROR,"LocateProtocol Status %x\n",Status));

  TableKey = 0;
  Status = AcpiTable->InstallAcpiTable (
                        AcpiTable,
                        Table,
                        TableSize,
                        &TableKey
                        );
  ASSERT_EFI_ERROR (Status);
  DEBUG((EFI_D_ERROR,"InstallAcpiTable Status %x\n",Status));
  return Status;
}


/**
  The driver's entry point.

  It publishes TCM ACPI Table.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval other           Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
TcmAcpiEntry (
  IN    EFI_HANDLE       ImageHandle,
  IN    EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = PublishAcpiTable ();
  ASSERT_EFI_ERROR (Status);

  return Status;
}





