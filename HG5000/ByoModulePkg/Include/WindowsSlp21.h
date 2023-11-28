/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SlicTable.h

Abstract:


Revision History:

**/
#ifndef _EFI_ACPI_SLIC_TABLE_H_
#define _EFI_ACPI_SLIC_TABLE_H_

#include <IndustryStandard/Acpi.h>


#define EFI_ACPI_SLIC_TABLE_SIGNATURE  SIGNATURE_32('S', 'L', 'I', 'C')


#pragma pack(1)

typedef struct {
  UINT32                           dwType;
  UINT32                           Length;
  UINT8                            Type;
  UINT8                            Version;
  UINT16                           Reserved;
  UINT32                           Key;
  UINT32                           Magic;
  UINT32                           Bitlen;
  UINT32                           Pubexp;
  UINT8                            Data[128];
} EFI_ACPI_OEM_PUBLIC_KEY_STRUCTURE;

typedef struct {
  UINT32                           Type;
  UINT32                           Length;
  UINT32                           OemRevision;
  UINT8                            Oemid[6];
  UINT64                           OemTableId;
  UINT64                           WindowsFlag;
  UINT32                           Reserved1;
  UINT64                           Reserved2;
  UINT64                           Reserved3;
  UINT8                            Signature[128];
} EFI_ACPI_SLP_MARKER_STRUCTURE;

//
// SLIC Table structure
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER        Header;
  EFI_ACPI_OEM_PUBLIC_KEY_STRUCTURE  PublicKey;
  EFI_ACPI_SLP_MARKER_STRUCTURE      Marker;
} EFI_ACPI_SLIC_TABLE;

#pragma pack()

#endif
