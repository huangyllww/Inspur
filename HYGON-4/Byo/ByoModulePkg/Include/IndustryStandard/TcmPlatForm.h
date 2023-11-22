/** @file

Copyright (c) 2006 - 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  TcmPlatForm.h

Abstract:
  Tcm platform define.

Revision History:

Bug 3269 - Add TCM int1A function support.
TIME: 2011-12-30
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  Use Smi to handle legacy int 1A(0xBB) interrupt.
$END------------------------------------------------------------


Bug 3144 - Add Tcm Measurement Architecture.
TIME: 2011-11-24
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  1. PEI: Measure CRTM Version.
          Measure Main Bios.
  2. DXE: add 'TCPA' acpi table.
          add event log.
          Measure Handoff Tables.
          Measure All Boot Variables.
          Measure Action.
  Note: As software of SM3's hash has not been implemented, so hash
        function is invalid.
$END------------------------------------------------------------

Bug 3075 - Add TCM support.
TIME: 2011-11-14
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  1. Tcm module init version.
     Only support setup function.
$END------------------------------------------------------------

**/



#ifndef __TCM_PLATFORM_H__
#define __TCM_PLATFORM_H__

#include <Uefi.h>
#include <IndustryStandard/Acpi.h>
#include "Tcm12.h"




//
// Standard event types
//
#define TCM_EV_POST_CODE                ((TCM_EVENTTYPE) 0x00000001)
#define TCM_EV_SEPARATOR                ((TCM_EVENTTYPE) 0x00000004)
#define TCM_EV_S_CRTM_CONTENTS          ((TCM_EVENTTYPE) 0x00000007)
#define TCM_EV_S_CRTM_VERSION           ((TCM_EVENTTYPE) 0x00000008)

//
// EFI specific event types
//
#define TCM_EV_EFI_EVENT_BASE                   ((TCM_EVENTTYPE)0x80000000)
#define TCM_EV_EFI_VARIABLE_DRIVER_CONFIG       (TCM_EV_EFI_EVENT_BASE + 1)
#define TCM_EV_EFI_VARIABLE_BOOT                (TCM_EV_EFI_EVENT_BASE + 2)
#define TCM_EV_EFI_BOOT_SERVICES_APPLICATION    (TCM_EV_EFI_EVENT_BASE + 3)
#define TCM_EV_EFI_BOOT_SERVICES_DRIVER         (TCM_EV_EFI_EVENT_BASE + 4)
#define TCM_EV_EFI_RUNTIME_SERVICES_DRIVER      (TCM_EV_EFI_EVENT_BASE + 5)
#define TCM_EV_EFI_GPT_EVENT                    (TCM_EV_EFI_EVENT_BASE + 6)
#define TCM_EV_EFI_ACTION                       (TCM_EV_EFI_EVENT_BASE + 7)
#define TCM_EV_EFI_PLATFORM_FIRMWARE_BLOB       (TCM_EV_EFI_EVENT_BASE + 8)
#define TCM_EV_EFI_HANDOFF_TABLES               (TCM_EV_EFI_EVENT_BASE + 9)


#define EFI_CALLING_EFI_APPLICATION         \
  "Calling EFI Application from Boot Option"
#define EFI_RETURNING_FROM_EFI_APPLICATOIN  \
  "Returning from EFI Application from Boot Option"
#define EFI_EXIT_BOOT_SERVICES_INVOCATION   \
  "Exit Boot Services Invocation"
#define EFI_EXIT_BOOT_SERVICES_FAILED       \
  "Exit Boot Services Returned with Failure"
#define EFI_EXIT_BOOT_SERVICES_SUCCEEDED    \
  "Exit Boot Services Returned with Success"

//
// Set structure alignment to 1-byte
//
#pragma pack (1)

typedef UINT32                     TCM_EVENTTYPE;


typedef struct {
  TCM_PCRINDEX                      PCRIndex;  ///< PCRIndex event extended to
  TCM_EVENTTYPE                     EventType; ///< TCG EFI event type
  TCM_DIGEST                        Digest;    ///< Value extended into PCRIndex
  UINT32                            EventSize; ///< Size of the event data
  UINT8                             Event[1];  ///< The event data
} TCM_PCR_EVENT;

#define TSS_EVENT_DATA_MAX_SIZE   256


typedef struct {
  TCM_PCRINDEX                      PCRIndex;
  TCM_EVENTTYPE                     EventType;
  TCM_DIGEST                        Digest;
  UINT32                            EventSize;
} TCM_PCR_EVENT_HDR;


typedef struct {
  EFI_PHYSICAL_ADDRESS              BlobBase;
  UINT64                            BlobLength;
} TCM_EFI_PLATFORM_FIRMWARE_BLOB;


typedef struct {
  UINTN                             NumberOfTables;
  EFI_CONFIGURATION_TABLE           TableEntry[1];
} TCM_EFI_HANDOFF_TABLE_POINTERS;


typedef struct {
  EFI_GUID                          VariableName;
  UINTN                             UnicodeNameLength;
  UINTN                             VariableDataLength;
  CHAR16                            UnicodeName[1];
  INT8                              VariableData[1];
} TCM_EFI_VARIABLE_DATA;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER       Header;
  UINT16                            PlatformClass;
  UINT32                            Laml;             // Log Area Minimum Length (LAML), 64KB
  EFI_PHYSICAL_ADDRESS              Lasa;             // Log Area Start Address (LASA)
} EFI_TCM_CLIENT_ACPI_TABLE;

#pragma pack ()

#endif

