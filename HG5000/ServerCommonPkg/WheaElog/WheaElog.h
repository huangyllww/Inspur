/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  WheaElog.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/


#ifndef _WHEA_ELOG_H_
#define _WHEA_ELOG_H_

#include <Uefi.h>
#include <IndustryStandard\WheaDefs.h>
#include <IndustryStandard\WheaAcpi.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/NvMediaAccess.h>
#include <Protocol/AcpiTable.h>



//
// Statements that include other files
//

#define ErstRegisterExecute { \
  EFI_ACPI_3_0_SYSTEM_IO,     \
  8,                          \
  0,                          \
  EFI_ACPI_3_0_BYTE,          \
  0xB2}

#define ErstRegisterFiller  { \
  EFI_ACPI_3_0_SYSTEM_MEMORY, \
  64,                         \
  0,                          \
  EFI_ACPI_3_0_QWORD,         \
  0}

#pragma pack (1)

#define   M_ATTRIBUTE_MASK    0x03  // Only BIT0 & BIT1 to be checked
#define   B_RECORD_FREE       0x03
#define   B_RECORD_INUSE      0x03  // Multiplex FREE & INUSE
#define   B_RECORD_PROCESSED  0x02
#define   B_RECORD_CLEARED    0x00


typedef struct {
  UINT16      Signature;
  UINT32      Resv1;
  UINT8       Resv2;
  // BIT[1:0]=11(free),01(In Use),00(cleared) BIT[7:2] is Reserved
  // THIS IS THE ONLY POSSIBLE STATES DUE TO FLASH CHIP DESIGN
  UINT8       Attributes; 
} EFI_ERROR_RECORD_SERIALIZATION_INFO;

typedef struct {
	UINT32			Signature;
	UINT16			Revision;	
	UINT32			PlatformReserved;
	UINT16			SectionCount;
	UINT32			ErrorSeverity;
	UINT32			ValidationBits;
	UINT32			RecordLength;
	UINT64			TimeStamp;
	EFI_GUID		PlatformID;
	EFI_GUID		PartitionID;
	EFI_GUID		CreatorID;
	EFI_GUID		NotificationType;
	UINT64			RecordID;
	UINT32			Flag;
	UINT64			OSReserved; // EFI_ERROR_RECORD_SERIALIZATION_INFO
	UINT64			Reserved0;
	UINT32			Reserved1;
} ERROR_RECORD_HEADER;


typedef struct {
  NV_MEDIA_ACCESS_PROTOCOL  *NvAcc;
  EFI_PHYSICAL_ADDRESS      BaseAddress;          // Physical Address where ELOG Records start
  UINT32                    FvBase;
  UINT32                    FvSize;
  UINT32                    Length;               // ELOG Space for Records in bytes (excluding FV HDR etc)
  UINT32                    TotalRecordBytes;     // Bytes occupied by records in ELOG including cleared ones
  UINT32                    FreeRecordBytes;      // Bytes occupied by records marked as cleared
  EFI_PHYSICAL_ADDRESS      NextRecordAddress;    // Address where a new record can be appended
  UINT16                    HeaderLength;
} EFI_WHEA_ELOG_STATISTICS;


typedef struct {
  UINT64      Command;
  UINT64      RecordID;
  UINT64      NextRecordID;
  UINT32      LogOffset;
  UINT32      Padding0;
  UINT64      Status;
  UINT64      BusyFlag;
  UINT64      RecordCount;
  UINT64      ErrorLogAddressRange;
  UINT64      ErrorLogAddressLength;
  UINT64      ErrorLogAddressAttributes;
  UINT64      Dummy;
  UINT64	    CurRecordID;
} WHEA_PARAMETER_REGION;

#pragma pack()


EFI_STATUS
PrstInit (
  VOID
  );

EFI_STATUS
PrstDefragment (
  VOID
  );

VOID
PrstRead (
  EFI_PHYSICAL_ADDRESS  RecordBuffer,
  UINT64                RecordID,
  UINT64                *Status
  );

VOID
PrstWrite(
  EFI_PHYSICAL_ADDRESS  Record,
  UINT64                *Status
  );

VOID
PrstClear (
  UINT64                RecordID,
  UINT64                *Status
  );

extern EFI_GUID gEfiWheaElogFvGuid;

#endif


