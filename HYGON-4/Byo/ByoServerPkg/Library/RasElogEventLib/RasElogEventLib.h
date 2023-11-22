/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef _RAS_ELOG_EVENT_LIB_H_
#define _RAS_ELOG_EVENT_LIB_H_

#include <Library/DebugLib.h>

typedef enum _MCA_STATUS_UMC_EXTERR_TYPE {
  DramEccErr = 0,
  WriteDataPoisonErr,
  SdpParityErr,
  ApbErr,
  AddressCommandParityErr,
  WriteDataCrcErr
} MCA_STATUS_UMC_EXTERR_TYPE;
//
// Common Defines
//
typedef enum {
  EfiElogSmSMBIOS,
  EfiElogSmIPMI,
  EfiElogSmMachineCritical,
  EfiElogSmASF,
  EfiElogSmOEM,
  EfiSmElogMax
} EFI_SM_ELOG_TYPE;

UINT32
PopCount16(
  UINT16    Testword
  );
  
typedef struct _DIMM_INFO {
  UINT8     ChipSelect;                 ///< The chip select on which the targeted address locates.
  UINT8     Bank;                       ///< The Bank for which the error address resides
  UINT32    Row;                        ///< The Row for which the error address resides
  UINT16    Column;                     ///< The Column for which the error address resides
  UINT8     rankmul;                    ///< The Rank Multiplex for which the error address resides
  UINT8     DimmId;                     ///< Reserved
} DIMM_INFO;



EFI_STATUS
McaToIpmi(
  IN  DIMM_INFO 			    *DimmInfo,  
  IN  OUT UINT8 			    *ErrorBuffer,
  OUT UINTN               *DataSize
  );

EFI_STATUS
NbioToIpmi(
	  IN OUT UINT8				    *ErrorBuffer,
	     OUT UINTN            *DataSize
  );

EFI_STATUS
PcieToIpmi(
  IN OUT UINT8		                *ErrorBuffer,
  OUT    UINTN                      *DataSize
  );

EFI_STATUS
McaToIpmiAli (
  IN  DIMM_INFO 			    *DimmInfo,  
  IN  OUT UINT8 			    *ErrorBuffer,
  OUT UINTN               *DataSize
  );

EFI_STATUS
NbioToIpmiAli(
	  IN OUT UINT8				    *ErrorBuffer,
	     OUT UINTN            *DataSize
  );

EFI_STATUS
PcieToIpmiAli(
  IN OUT UINT8		                  *ErrorBuffer,
  OUT    UINTN                      *DataSize
  );




EFI_STATUS
MemTrainingToIpmi(
  IN  UINT8                         Socket,
  IN  UINT8                         Channel,
  IN  UINT8                         Dimm,
  IN  OUT UINT8                     *ErrorBuffer,
  IN  OUT UINTN                     *ErrorBufferSize
  );


// wz201005 + >> #2983807 add standard SEL record
EFI_STATUS
MemTrainingToIpmiAli(
  IN  UINT8                         Socket,
  IN  UINT8                         Channel,
  IN  UINT8                         Dimm,
  IN  OUT UINT8                     *ErrorBuffer,
  IN  OUT UINTN                     *ErrorBufferSize
  );
// wz201005 + << #2983807 add standard SEL record


EFI_STATUS
SetEventLogData (
  IN  UINT8                         *ElogData,
  IN  EFI_SM_ELOG_TYPE              DataType,
  IN  BOOLEAN                       AlertEvent,
  IN  UINTN                         DataSize,
  OUT UINT64                        *RecordId
  );

EFI_STATUS
SendErrorLogDataToBmc (
  IN  UINT8                         *ElogData,
  IN  UINTN                         DataSize
  );


EFI_STATUS
EfiSmGetTimeStamp (
  OUT UINT32 *NumOfSeconds
  );

#endif
