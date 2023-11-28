/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  RasElogEventLib.c

Abstract:
  Source file for the RasElogEventLib.

Revision History:

**/
#include <PiDxe.h>
#include <AmdRas.h>
#include "AmdCpmRas.h"
#include "RasElogEventLib.h"
#include <IndustryStandard\IpmiNetFnStorage.h>
#include <Ipmi\IpmiNetFnStorageDefinitions.h>

UINT8                       ErrorLogData[0x80];

EFI_STATUS
SendElogEventMca (
  IN  UINT8        *ErrorRecord,
  IN  UINT8        BankIndex,
  IN  DIMM_INFO    *DimmInfo
)
{
  EFI_STATUS            Status;
  RAS_MCA_ERROR_INFO    *RasMcaErrorInfo;
  UINTN                 EventDataSize;
  EventDataSize = sizeof(IPMI_SEL_EVENT_RECORD_DATA);

  DEBUG((EFI_D_ERROR, "SendElogEventMca: == ENTER ==\n"));

  RasMcaErrorInfo = (RAS_MCA_ERROR_INFO*) ErrorRecord;
  Status = McaToIpmi(RasMcaErrorInfo, ErrorLogData, DimmInfo);
//  Status = McaToIpmi(RasMcaErrorInfo, BankIndex, DimmInfo, ErrorLogData, &EventDataSize);
  if(Status == EFI_SUCCESS){
    Status = SendErrorLogDataToBmc(ErrorLogData, EventDataSize);
    DEBUG((EFI_D_ERROR, "SendElogEventMca: SetEventLogData for Ipmi-%r\n", Status));
  }
  return  Status;
}

EFI_STATUS
SendElogEventNbio (
  IN  UINT8        *ErrorRecord
  )
{
  EFI_STATUS             Status;
  RAS_NBIO_ERROR_INFO    *RasNbioErrorInfo;
  UINTN                  EventDataSize;

  DEBUG((EFI_D_ERROR, "SendElogEventNbio: == ENTER ==\n"));
  RasNbioErrorInfo = (RAS_NBIO_ERROR_INFO*)ErrorRecord;
  EventDataSize = sizeof(IPMI_SEL_EVENT_RECORD_DATA);

  //Status = NbioToIpmi(RasNbioErrorInfo, ErrorLogData, &EventDataSize);  
  Status = NbioToIpmi(RasNbioErrorInfo, ErrorLogData);
  if(Status == EFI_SUCCESS){
    Status = SendErrorLogDataToBmc(ErrorLogData, EventDataSize);
    DEBUG((EFI_D_ERROR, "SendElogEventNbio: SetEventLogData for Ipmi-%r\n", Status));
  }

  return EFI_SUCCESS;
}

EFI_STATUS
SendElogEventPcie (
  IN  UINT8        *ErrorRecord,
  IN  UINT32       PcieUncorrStatus,
  IN  UINT32       PcieCorrStatus
  )
{
  EFI_STATUS                    Status;
  GENERIC_PCIE_AER_ERR_ENTRY    *GenPcieAerErrEntry;
  UINTN                         EventDataSize;
  EventDataSize = sizeof(IPMI_SEL_EVENT_RECORD_DATA);

  DEBUG((EFI_D_ERROR, "SendElogEventPcie: == ENTER ==\n"));
  GenPcieAerErrEntry = (GENERIC_PCIE_AER_ERR_ENTRY*)ErrorRecord;
  //Status = PcieToIpmi(GenPcieAerErrEntry, PcieUncorrStatus, PcieCorrStatus, ErrorLogData, &EventDataSize);
  Status = PcieToIpmi(GenPcieAerErrEntry, ErrorLogData);
  if(Status == EFI_SUCCESS){
    Status = SendErrorLogDataToBmc(ErrorLogData, EventDataSize);
    DEBUG((EFI_D_ERROR, "SendElogEventPcie: SetEventLogData for Ipmi-%r\n", Status));
  }

    return EFI_SUCCESS;
}

EFI_STATUS
SendElogEventMemTest (
  IN  UINT8        Socket,
  IN  UINT8        Channel,
  IN  UINT8        Dimm
  )
{
  EFI_STATUS                    Status;
  UINTN                         EventDataSize;

  DEBUG((EFI_D_ERROR, "SendElogEventMemTest: == ENTER ==\n"));
  DEBUG((EFI_D_ERROR, "Socket:%x Channel:%x Dimm:%x\n",Socket,Channel,Dimm));
  Status = MemTestToIpmi(Socket, Channel, Dimm, ErrorLogData, &EventDataSize);
  if(Status == EFI_SUCCESS){
    Status = SendErrorLogDataToBmc(ErrorLogData, EventDataSize);
    DEBUG((EFI_D_ERROR, "SendElogEventMemTest: SetEventLogData for Ipmi-%r\n", Status));
  }

    return EFI_SUCCESS;
}


EFI_STATUS
SendElogEventMemTraining (
  IN  UINT8        Socket,
  IN  UINT8        Channel,
  IN  UINT8        Dimm
  )
{
  EFI_STATUS                    Status;
  UINTN                         EventDataSize;

  DEBUG((EFI_D_ERROR, "SendElogEventMemTraining: == ENTER ==\n"));
  DEBUG((EFI_D_ERROR, "Memory Disabled @Socket:%x Channel:%x Dimm:%x\n",Socket,Channel,Dimm));  
  Status = MemTrainingToIpmi(Socket, Channel, Dimm, ErrorLogData, &EventDataSize);
  if(Status == EFI_SUCCESS){
    Status = SendErrorLogDataToBmc(ErrorLogData, EventDataSize);
    DEBUG((EFI_D_ERROR, "SendElogEventMemTraining: SetEventLogData for Ipmi-%r\n", Status));
  }

    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
RasElogEventLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return EFI_SUCCESS;
}
