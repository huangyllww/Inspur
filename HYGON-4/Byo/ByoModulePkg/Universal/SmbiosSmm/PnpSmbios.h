/*++
====================================================================
      NOTICE: Copyright (c) 2006 - 2012 Byosoft Corporation. All rights reserved.
              This program and associated documentation (if any) is furnished
              under a license. Except as permitted by such license,no part of this
              program or documentation may be reproduced, stored divulged or used
              in a public system, or transmitted in any form or by any means
              without the express written consent of Byosoft Corporation.
====================================================================
Module Name:

  PnpSmbios.h

Abstract:

Revision History:

--*/

#ifndef _PNP_SMBIOS_H_
#define _PNP_SMBIOS_H_

#include <SmbiosSmm.h>
#include <Guid/SmBios.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SetMemAttributeSmmLib.h>
#include <SmbiosPnp52.h>
#include <Library/ByoCommLib.h>


#define SMBIOS_BUFFER_SIZE        SIZE_32KB
#define END_HANDLE                0xFFFF


#pragma pack(1)


#define EFI_PNP_52_SIGNATURE  SIGNATURE_32('_','p','n','p')
typedef struct {
  UINTN               Signature;
  LIST_ENTRY          Link;
  SMBIOS_REC_HEADER   header;
  PNP_52_DATA_BUFFER  *pRecord;
} PNP_52_RECORD;

typedef struct {
  EFI_SMBIOS_TABLE_HEADER  Header;
  UINT8                    Tailing[2];
} EFI_SMBIOS_TABLE_END_STRUCTURE;

#pragma pack()

UINT16
PnpTestSmiPort(
	IN VOID *ParameterBuffer
);

UINT16
PnpGetSmbiosInformation (
  IN VOID *ParameterBuffer
);

UINT16
PnpGetSmbiosStructure(
  IN VOID *ParameterBuffer
);

UINT16
PnpGetSmbiosStructureByType (
  IN VOID *ParameterBuffer
);

UINT16
PnpSetSmbiosStructure (
  IN VOID *ParameterBuffer
);

UINT16
PnpSetSmbiosStructure32 (
  IN PNP_52_DATA_BUFFER        *DataBufferPtr,
  IN UINT8                     Control,
  IN BOOLEAN                   FixHandle
);

EFI_STATUS 
Mem2RecordList (
  IN UINT8 *pBase, 
  IN UINTN size
);

VOID 
GetFromFlash ();

#endif

