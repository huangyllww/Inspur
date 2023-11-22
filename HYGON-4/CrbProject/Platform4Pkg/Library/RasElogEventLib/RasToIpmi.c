
/** @file

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  RasToIpmi.c

Abstract:
  Source file for the RasElogEventLib.

Revision History:

**/

#include "RasElogEventLib.h"


#define BYO_MEMORY_ERROR_SENSOR_NUMBER          0xF9
#define BYO_CPU_ERROR_SENSOR_NUMBER             0xFA
#define BYO_PCIE_ERROR_SENSOR_NUMBER            0xEF

typedef enum  {
  DramEccErr = 0,
  WriteDataPoisonErr,
  SdpParityErr,
  ApbErr,
  AddressCommandParityErr,
  WriteDataCrcErr
} MCA_STATUS_UMC_EXTERR_TYPE;

#pragma pack(1)

typedef struct {
  UINT8  ErrorType:4;
  UINT8  Data2Type:2;
  UINT8  Data3Type:2;

  UINT8  Reserved:1;
  UINT8  ControllerNumber:2;
  UINT8  DieNumber:2;
  UINT8  ExtErrorCode:3;

  UINT8  DimmNumber:2;
  UINT8  ChannelNumber:4;
  UINT8  SocketNumber:2;
} BYO_MEM_ERROR_EVENT_DATA;


typedef struct {
  UINT8  ErrorType:4;
  UINT8  Data2Type:2;
  UINT8  Data3Type:2;

  UINT8  BankType:4;
  UINT8  DieNumber:3;
  UINT8  SocketNumber:1;

  UINT8  ErrorCode:4;
  UINT8  ExtErrorCode:4;
}BYO_CPU_ERROR_EVENT_DATA;


typedef struct {
  UINT8  ErrorType:4;
  UINT8  Data2Type:2;
  UINT8  Data3Type:2;

  UINT8  BusNumber;
  
  UINT8  FuncNumber:3;  
  UINT8  DevNumber:5;
}BYO_PCIE_ERROR_EVENT_DATA;

#pragma pack()



UINT8 GetDimmChannel(UINT8 Socket, UINT8 Cdd, UINT8 UmcChannel);


EFI_STATUS
MemTrainingToIpmi (
  IN  UINT8                         Socket,
  IN  UINT8                         Channel,
  IN  UINT8                         Dimm,
  IN  OUT UINT8                     *ErrorBuffer,
  IN  OUT UINTN                     *DataSize
  )
{
  IPMI_SEL_EVENT_RECORD_DATA	*SelRecord;
  UINT32	                     TimeStamp;
  BYO_MEM_ERROR_EVENT_DATA     *MemEventData;
  

  if(ErrorBuffer == NULL || DataSize == NULL){
    return EFI_INVALID_PARAMETER;
  }
  
  ZeroMem(ErrorBuffer, sizeof(IPMI_SEL_EVENT_RECORD_DATA));
  GetTimeStampLib(&TimeStamp);
  
  SelRecord = (IPMI_SEL_EVENT_RECORD_DATA*)ErrorBuffer;
  MemEventData = (BYO_MEM_ERROR_EVENT_DATA*)&SelRecord->OEMEvData1;
  
  SelRecord->TimeStamp    = TimeStamp;
  SelRecord->RecordType   = IPMI_SEL_SYSTEM_RECORD;
	SelRecord->GeneratorId  = PcdGet16(PcdIpmiSelRecordGeneratorId);
	SelRecord->EvMRevision  = IPMI_EVM_REVISION;
	SelRecord->EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
	SelRecord->SensorType   = 0x0C;
	SelRecord->SensorNumber = BYO_MEMORY_ERROR_SENSOR_NUMBER;

  MemEventData->Data2Type = EFI_OEM_SPECIFIC_DATA;
  MemEventData->Data3Type = EFI_OEM_SPECIFIC_DATA;  
  MemEventData->ErrorType = 0xB;

  MemEventData->SocketNumber  = Socket;
  MemEventData->DimmNumber    = Dimm;  
  MemEventData->ChannelNumber = Channel;
  MemEventData->DieNumber        = 0;
  MemEventData->ControllerNumber = 0;

  *DataSize = sizeof(IPMI_SEL_EVENT_RECORD_DATA);

  return EFI_SUCCESS;
}



EFI_STATUS
McaToIpmi (
    IN  RAS_MCA_ERROR_INFO  *McaErrorRecord,
    IN  UINT8               BankIndex,    
    IN  DIMM_INFO           *DimmInfo,  
    IN  OUT UINT8           *ErrorBuffer,
    OUT UINTN               *DataSize
  )
{
	EFI_STATUS					                  Status = EFI_SUCCESS;
	IPMI_SEL_EVENT_RECORD_DATA	          *SelRecord;
	UINT32	                              TimeStamp = 0;
	UMC_EXT_ERROR_CODE		                UmcExtErrorCode;
	UINT16						                    McaErrorCode;
	UINT8						                      McaExtErrorCode;
  BYO_MEM_ERROR_EVENT_DATA              *MemEventData;
  BYO_CPU_ERROR_EVENT_DATA              *CpuEventData;
  BOOLEAN                               IsMemCe = FALSE;
  UINT8                                 UmcChannel;
  UINT8                                 Socket;
  UINT8                                 Cdd;
  UINT8                                 DimmIndex;

	if(McaErrorRecord == NULL || ErrorBuffer == NULL || BankIndex >= ARRAY_SIZE(McaErrorRecord->McaBankErrorInfo)){
		return EFI_INVALID_PARAMETER;
	}
	if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Value == 0){
		return EFI_INVALID_PARAMETER;
	}  

  ZeroMem(ErrorBuffer, sizeof(IPMI_SEL_EVENT_RECORD_DATA));
  *DataSize = sizeof(IPMI_SEL_EVENT_RECORD_DATA);
	GetTimeStampLib(&TimeStamp);
  
	switch (McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber){
    
    case MCA_UMC0_SubChnl0_BANK:
    case MCA_UMC0_SubChnl1_BANK:
    case MCA_UMC1_SubChnl0_BANK:
    case MCA_UMC1_SubChnl1_BANK:
    case MCA_UMC2_SubChnl0_BANK:
    case MCA_UMC2_SubChnl1_BANK:
      
			SelRecord = (IPMI_SEL_EVENT_RECORD_DATA*)ErrorBuffer;
      MemEventData = (BYO_MEM_ERROR_EVENT_DATA*)&SelRecord->OEMEvData1;
			SelRecord->TimeStamp    = TimeStamp;
			SelRecord->RecordType   = IPMI_SEL_SYSTEM_RECORD;
			SelRecord->GeneratorId  = PcdGet16(PcdIpmiSelRecordGeneratorId);
			SelRecord->EvMRevision  = IPMI_EVM_REVISION;
			SelRecord->EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
			SelRecord->SensorType   = 0x0C;
			SelRecord->SensorNumber = BYO_MEMORY_ERROR_SENSOR_NUMBER;

			if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UECC){
				MemEventData->ErrorType = 0x01;
			}else if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.CECC) {
				MemEventData->ErrorType = 0x00;
        IsMemCe = TRUE;
			}
      MemEventData->Data2Type = EFI_OEM_SPECIFIC_DATA;
      MemEventData->Data3Type = EFI_OEM_SPECIFIC_DATA;

      Socket     = McaErrorRecord->CpuInfo.SocketId;
      Cdd        = McaErrorRecord->CpuInfo.CddId;
      UmcChannel = (UINT8)(McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber - MCA_UMC0_SubChnl0_BANK)/2;

      MemEventData->SocketNumber     = Socket;
      MemEventData->DieNumber        = Cdd;
      MemEventData->ControllerNumber = UmcChannel;

			UmcExtErrorCode.Value = (UINT8)McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.ErrorCodeExt;
			if(UmcExtErrorCode.Field.DramEccErr){
				MemEventData->ExtErrorCode = DramEccErr;
			}else if(UmcExtErrorCode.Field.WriteDataPoisonErr){
				MemEventData->ExtErrorCode = WriteDataPoisonErr;
			}else if(UmcExtErrorCode.Field.SdpParityErr) {
				MemEventData->ExtErrorCode = SdpParityErr;
			}else if(UmcExtErrorCode.Field.ApbErr) {
				MemEventData->ExtErrorCode = ApbErr;
			}else if(UmcExtErrorCode.Field.AddrCmdParityErr) {
				MemEventData->ExtErrorCode = AddressCommandParityErr;
				MemEventData->ErrorType = 0x02;
			}else if(UmcExtErrorCode.Field.WriteDataCrcErr) {
				MemEventData->ExtErrorCode = WriteDataCrcErr;
			}

      MemEventData->DimmNumber = (DimmInfo->ChipSelect > 1) ? 1 : 0;
      MemEventData->ChannelNumber = GetDimmChannel(Socket, Cdd, UmcChannel);
      DimmIndex = MemEventData->SocketNumber * 24 + MemEventData->ChannelNumber * 12 + MemEventData->DimmNumber;
      McaErrorRecord->DimmIndex = DimmIndex;
      if(IsMemCe && IsIgnoreThisMemCe(TimeStamp, DimmIndex)){
        McaErrorRecord->CeIgnoreBank |= 1 << (McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber - MCA_UMC0_SubChnl0_BANK);
        Status = EFI_ABORTED;
      }
 			break;
			
		default:
			if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Val){
				SelRecord = (IPMI_SEL_EVENT_RECORD_DATA*)ErrorBuffer;
        CpuEventData = (BYO_CPU_ERROR_EVENT_DATA*)&SelRecord->OEMEvData1;

				SelRecord->TimeStamp    = TimeStamp;
				SelRecord->RecordType   = IPMI_SEL_SYSTEM_RECORD;
				SelRecord->GeneratorId  = PcdGet16(PcdIpmiSelRecordGeneratorId);
				SelRecord->EvMRevision  = IPMI_EVM_REVISION;
				SelRecord->EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
				SelRecord->SensorType   = 0x07;
				SelRecord->SensorNumber = BYO_CPU_ERROR_SENSOR_NUMBER;

        CpuEventData->Data2Type = EFI_OEM_SPECIFIC_DATA;
        CpuEventData->Data3Type = EFI_OEM_SPECIFIC_DATA;

				if (McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC) {
					CpuEventData->ErrorType = 0x0B;
				}else {
					CpuEventData->ErrorType = 0x0C;
				}

        CpuEventData->SocketNumber = McaErrorRecord->CpuInfo.SocketId;
        CpuEventData->DieNumber    = McaErrorRecord->CpuInfo.CddId;

				if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber > MCA_UMC2_SubChnl1_BANK){
					CpuEventData->BankType = (UINT8)(McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber - MCA_UMC2_SubChnl1_BANK + MCA_L3_C0_S0_BANK);
				}else if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber > MCA_FP_BANK){
					CpuEventData->BankType = MCA_L3_C0_S0_BANK;
				}else{
					CpuEventData->BankType = (UINT8)McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber;
				}
				
				//Check Error code types
				McaErrorCode    = (UINT16)McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.ErrorCode;
				McaExtErrorCode = (UINT8)McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.ErrorCodeExt;
				CpuEventData->ExtErrorCode = McaExtErrorCode;
				if (((McaErrorCode & TLB_ERROR_MASK) >> TLB_ERROR_CHK_SHIFT) == 1) {           //TLB error
					CpuEventData->ErrorCode = 1;
				} else if (((McaErrorCode & MEM_ERROR_MASK) >> MEM_ERROR_CHK_SHIFT) == 1) {    //Memory error
					CpuEventData->ErrorCode = 2;
				} else if (((McaErrorCode & INT_ERROR_MASK) >> INT_ERROR_CHK_SHIFT) == 1) {    //Bus Error
					CpuEventData->ErrorCode = 3;
				} else if (((McaErrorCode & PP_ERROR_MASK) >> PP_ERROR_CHK_SHIFT) == 1) {      //Internal unclassified error
					CpuEventData->ErrorCode = 4;
				}
			} else {
				Status = EFI_NOT_FOUND;
			}
			break;	// End of PIE processing
			
	}
	
	return Status;
}




/**
    Translate/convert, if possible, error entry from NBIO to Ipmi format

    @param RAS_NBIO_ERROR_INFO *NbioErrorRecord - pointer to NBIO error structure
    @param UINT8              *ErrorBuffer - buffer with Ipmi error entry, only data

    @retval EFI_SUCCESS - Ipmi error log entry created
    @retval EFI_INVALID_PARAMETER - input param contains null pointer
    @retval EFI_NOT_FOUND - translation cannot be done

**/
EFI_STATUS
NbioToIpmi(
	  IN  RAS_NBIO_ERROR_INFO	*NbioErrorRecord,
	  IN OUT UINT8				    *ErrorBuffer,
    OUT    UINTN            *DataSize	 
  )
{
  EFI_STATUS		              Status = EFI_SUCCESS;
  IPMI_SEL_EVENT_RECORD_DATA	*SelRecord;
  UINT32	                    TimeStamp = 0;


  if(NbioErrorRecord == NULL || ErrorBuffer == NULL || DataSize == NULL){
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem(ErrorBuffer, sizeof(IPMI_SEL_EVENT_RECORD_DATA));
  GetTimeStampLib(&TimeStamp);

  SelRecord = (IPMI_SEL_EVENT_RECORD_DATA*)ErrorBuffer;
  SelRecord->TimeStamp = TimeStamp;
  SelRecord->RecordType = IPMI_SEL_SYSTEM_RECORD;
  SelRecord->GeneratorId = EFI_SOFTWARE_ID;
  SelRecord->EvMRevision = IPMI_EVM_REVISION;
  SelRecord->EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
  SelRecord->SensorType = 0x07;
  SelRecord->SensorNumber = 0xA6; // Correctable
  SelRecord->OEMEvData1 = (UINT8) ((EFI_OEM_SPECIFIC_DATA << 6) | (EFI_OEM_SPECIFIC_DATA << 4) | NbioErrorRecord->TypeId);
  SelRecord->OEMEvData2 = (NbioErrorRecord->GroupId << 3) | (NbioErrorRecord->LogicalDie);
  SelRecord->OEMEvData3 = NbioErrorRecord->TypeId;//todo

  *DataSize = sizeof(IPMI_SEL_EVENT_RECORD_DATA);

  return Status;
}





/**
    Translate/convert, if possible, error entry from PCIe to Ipmi format

    @param GENERIC_PCIE_AER_ERR_ENTRY *GenPcieAerErrEntry - pointer to GENERIC_PCIE_AER_ERR_ENTRY structure
    @param UINT8              *ErrorBuffer - buffer with Ipmi error entry, only data

    @retval EFI_SUCCESS - Ipmi error log entry created
    @retval EFI_INVALID_PARAMETER - input param contains null pointer
    @retval EFI_NOT_FOUND - translation cannot be done

**/
EFI_STATUS
PcieToIpmi (
  IN     GENERIC_PCIE_AER_ERR_ENTRY *GenPcieAerErrEntry,
  IN OUT UINT8			                *ErrorBuffer,
  OUT    UINTN                      *DataSize
  )
{
	EFI_STATUS					        Status = EFI_SUCCESS;
	IPMI_SEL_EVENT_RECORD_DATA	*SelRecord;
	UINT32	                    TimeStamp = 0;
  BYO_PCIE_ERROR_EVENT_DATA   *PcieEventData;


	if(GenPcieAerErrEntry == NULL || ErrorBuffer == NULL){
		return EFI_INVALID_PARAMETER;
	}

  DEBUG((EFI_D_INFO, "%08X %08X %08X %08X\n", \
    GenPcieAerErrEntry->PcieAerErrorSection.AerInfo.AerInfoData[0], \
    GenPcieAerErrEntry->PcieAerErrorSection.AerInfo.AerInfoData[1], \
    GenPcieAerErrEntry->PcieAerErrorSection.AerInfo.AerInfoData[2], \
    GenPcieAerErrEntry->PcieAerErrorSection.AerInfo.AerInfoData[3]));
  DEBUG((EFI_D_INFO, "%08X %08X %08X %08X\n", \
    GenPcieAerErrEntry->PcieAerErrorSection.AerInfo.AerInfoData[4], \
    GenPcieAerErrEntry->PcieAerErrorSection.AerInfo.AerInfoData[5], \
    GenPcieAerErrEntry->PcieAerErrorSection.AerInfo.AerInfoData[6], \
    GenPcieAerErrEntry->PcieAerErrorSection.AerInfo.AerInfoData[7]));

  ZeroMem(ErrorBuffer, sizeof(IPMI_SEL_EVENT_RECORD_DATA));
	GetTimeStampLib(&TimeStamp);
	
	SelRecord = (IPMI_SEL_EVENT_RECORD_DATA*)ErrorBuffer;
  PcieEventData = (BYO_PCIE_ERROR_EVENT_DATA*)&SelRecord->OEMEvData1;
					
	SelRecord->TimeStamp = TimeStamp;
	SelRecord->RecordType = IPMI_SEL_SYSTEM_RECORD;
	SelRecord->GeneratorId = PcdGet16(PcdIpmiSelRecordGeneratorId);
	SelRecord->EvMRevision = IPMI_EVM_REVISION;
	SelRecord->EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;

	SelRecord->SensorType = 0x13;
	SelRecord->SensorNumber = BYO_PCIE_ERROR_SENSOR_NUMBER;

  PcieEventData->Data2Type = EFI_OEM_SPECIFIC_DATA;
  PcieEventData->Data3Type = EFI_OEM_SPECIFIC_DATA;
  	
	if(GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity == ERROR_RECOVERABLE || 
     GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity == ERROR_SEVERITY_FATAL)  {
		PcieEventData->ErrorType = 0x05;     //CRITICAL_INTERRUPT_PCI_SERR
	} else if (GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity == ERROR_SEVERITY_CORRECTED)	{
		PcieEventData->ErrorType = 0x04 ;    //CRITICAL_INTERRUPT_PCI_PERR		
	} else {
		PcieEventData->ErrorType = 0x04 ;    //CRITICAL_INTERRUPT_PCI_PERR		
	}

  PcieEventData->BusNumber  = GenPcieAerErrEntry->PcieAerErrorSection.DeviceId.PrimaryBus;
  PcieEventData->DevNumber  = GenPcieAerErrEntry->PcieAerErrorSection.DeviceId.Device;
  PcieEventData->FuncNumber = GenPcieAerErrEntry->PcieAerErrorSection.DeviceId.Function;

  *DataSize = sizeof(IPMI_SEL_EVENT_RECORD_DATA);
	return Status;
}





