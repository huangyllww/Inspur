
/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
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

#include <PiDxe.h>
#include <Ipmi\IpmiNetFnStorageDefinitions.h>
#include <IndustryStandard\IpmiNetFnStorage.h>
#include <Library\BaseMemoryLib.h>
#include <Library\IpmiBaseLib.h>


#include <AmdRas.h>
#include "AmdCpmRas.h"
#include "RasElogEventLib.h"

#define MANUFACTURER_ID0                0xA2
#define MANUFACTURER_ID1                0x63
#define MANUFACTURER_ID2                0x00
#define SUBCMD_MCA                      0x29
#define SUBCMD_AER                      0x2A
#define SUBCMD_SMN                      0x2B
#define SUBCMD_MEM_TEST_TRAINING        0x2C
#define MCA_DATA_SIZE                   37
#define AER_DATA_SIZE                   24
#define SMN_DATA_SIZE                   8
#define MEM_TEST_TRAINING_DATA_SIZE     7

/**
    Translate/convert, if possible, error entry from MCA to Ipmi format

    @param RAS_MCA_ERROR_INFO   *McaErrorRecord - pointer to MCA error structure
    @param UINT8                *SmbiosErrorBuffer - buffer with Ipmi error entry, only data
    @param DIMM_INFO            *DimmInfo - pointer to DIMM info structure, can be NULL for 
                                            for non DRAM ECC errors

    @retval EFI_SUCCESS - Ipmi error log entry created
    @retval EFI_INVALID_PARAMETER - input param contains null pointer
    @retval EFI_NOT_FOUND - translation cannot be done

**/
/*
EFI_STATUS
McaToIpmi(
  IN  RAS_MCA_ERROR_INFO            *McaErrorRecord,
  IN  UINT8                         BankIndex,
  IN  DIMM_INFO                     *DimmInfo,
  IN  OUT UINT8                     *ErrorBuffer,
  IN  OUT UINTN                     *ErrorBufferSize
  )
{
  EFI_STATUS                  Status = EFI_SUCCESS;

  if((McaErrorRecord == NULL)||(ErrorBuffer == NULL)){
    return  EFI_INVALID_PARAMETER;
  }

  SetMem(ErrorBuffer, MCA_DATA_SIZE, 0);

  if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Val) {

    ErrorBuffer[0] = MANUFACTURER_ID0;
    ErrorBuffer[1] = MANUFACTURER_ID1;
    ErrorBuffer[2] = MANUFACTURER_ID2;
    ErrorBuffer[3] = SUBCMD_MCA;

    ErrorBuffer[4] = McaErrorRecord->CpuInfo.SocketId;
    ErrorBuffer[5] = McaErrorRecord->CpuInfo.CcxId;
    ErrorBuffer[6] = McaErrorRecord->CpuInfo.DieId;
    ErrorBuffer[7] = McaErrorRecord->CpuInfo.CoreId;
    ErrorBuffer[8] = McaErrorRecord->CpuInfo.ThreadID;
    ErrorBuffer[9] = McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber - MCA_UMC0_BANK;
    if(DimmInfo != NULL) {
      ErrorBuffer[10] = DimmInfo->DimmId;
    }
    CopyMem(&ErrorBuffer[13], &McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr, sizeof(UINT64));
    CopyMem(&ErrorBuffer[21], &McaErrorRecord->McaBankErrorInfo[BankIndex].McaIpidMsr, sizeof(UINT64));
    CopyMem(&ErrorBuffer[29], &McaErrorRecord->McaBankErrorInfo[BankIndex].McaSyndMsr, sizeof(UINT64));

    *ErrorBufferSize = MCA_DATA_SIZE;
  }
  else {
    Status = EFI_NOT_FOUND;
  }

  return  Status;
}
*/

/**
    Translate/convert, if possible, error entry from NBIO to Ipmi format

    @param RAS_NBIO_ERROR_INFO *NbioErrorRecord - pointer to NBIO error structure
    @param UINT8              *ErrorBuffer - buffer with Ipmi error entry, only data

    @retval EFI_SUCCESS - Ipmi error log entry created
    @retval EFI_INVALID_PARAMETER - input param contains null pointer
    @retval EFI_NOT_FOUND - translation cannot be done

**/
/*
EFI_STATUS
NbioToIpmi(
  IN  RAS_NBIO_ERROR_INFO           *NbioErrorRecord,
  IN  OUT UINT8                     *ErrorBuffer,
  IN  OUT UINTN                     *ErrorBufferSize
  )
{
  if((NbioErrorRecord == NULL)||(ErrorBuffer == NULL)){
    return  EFI_INVALID_PARAMETER;
  }

  SetMem(ErrorBuffer, AER_DATA_SIZE, 0);

  ErrorBuffer[0] = MANUFACTURER_ID0;
  ErrorBuffer[1] = MANUFACTURER_ID1;
  ErrorBuffer[2] = MANUFACTURER_ID2;
  ErrorBuffer[3] = SUBCMD_AER;

  ErrorBuffer[4] = NbioErrorRecord->DieBusNumber;
  ErrorBuffer[5] = 0;
  ErrorBuffer[6] = 0;
  ErrorBuffer[7] = 0;
  CopyMem(&ErrorBuffer[8], &NbioErrorRecord->NbioGlobalStatusLo, sizeof(UINT32));
  CopyMem(&ErrorBuffer[16], &NbioErrorRecord->NbioGlobalStatusHi, sizeof(UINT32));

  *ErrorBufferSize = AER_DATA_SIZE;

  return EFI_SUCCESS;
}
*/
/**
    Translate/convert, if possible, error entry from PCIe to Ipmi format

    @param GENERIC_PCIE_AER_ERR_ENTRY *GenPcieAerErrEntry - pointer to GENERIC_PCIE_AER_ERR_ENTRY structure
    @param UINT8              *ErrorBuffer - buffer with Ipmi error entry, only data

    @retval EFI_SUCCESS - Ipmi error log entry created
    @retval EFI_INVALID_PARAMETER - input param contains null pointer
    @retval EFI_NOT_FOUND - translation cannot be done

**/
/*
EFI_STATUS
PcieToIpmi(
  IN  GENERIC_PCIE_AER_ERR_ENTRY    *GenPcieAerErrEntry,
  IN  UINT32                        PcieUncorrStatus,
  IN  UINT32                        PcieCorrStatus,
  IN  OUT UINT8                     *ErrorBuffer,
  IN  OUT UINTN                     *ErrorBufferSize
  )
{
  if((GenPcieAerErrEntry == NULL)||(ErrorBuffer == NULL)){
    return  EFI_INVALID_PARAMETER;
  }

  SetMem(ErrorBuffer, AER_DATA_SIZE, 0);

  ErrorBuffer[0] = MANUFACTURER_ID0;
  ErrorBuffer[1] = MANUFACTURER_ID1;
  ErrorBuffer[2] = MANUFACTURER_ID2;
  ErrorBuffer[3] = SUBCMD_AER;

  ErrorBuffer[4] = GenPcieAerErrEntry->PcieAerErrorSection.DeviceId.PrimaryBus;
  ErrorBuffer[5] = GenPcieAerErrEntry->PcieAerErrorSection.DeviceId.Device;
  ErrorBuffer[6] = GenPcieAerErrEntry->PcieAerErrorSection.DeviceId.Function;
  ErrorBuffer[7] = (UINT8)GenPcieAerErrEntry->PcieAerErrorSection.DeviceId.Slot;
  CopyMem(&ErrorBuffer[8], &PcieUncorrStatus, sizeof(UINT32));
  CopyMem(&ErrorBuffer[16], &PcieCorrStatus, sizeof(UINT32));

  *ErrorBufferSize = AER_DATA_SIZE;

  return EFI_SUCCESS;
}
*/

	
	
	/**
		Translate/convert, if possible, error entry from MCA to Ipmi format
	
		@param RAS_MCA_ERROR_INFO	*McaErrorRecord - pointer to MCA error structure
		@param UINT8				*SmbiosErrorBuffer - buffer with Ipmi error entry, only data
		@param DIMM_INFO			*DimmInfo - pointer to DIMM info structure, can be NULL for 
												for non DRAM ECC errors
	
		@retval EFI_SUCCESS - Ipmi error log entry created
		@retval EFI_INVALID_PARAMETER - input param contains null pointer
		@retval EFI_NOT_FOUND - translation cannot be done
	
	**/
	
	EFI_STATUS
	McaToIpmi(
	  IN  RAS_MCA_ERROR_INFO	*McaErrorRecord,
	  IN  OUT UINT8 			*ErrorBuffer,
	  IN  DIMM_INFO 			*DimmInfo
	  )
	{
		EFI_STATUS					                  Status = EFI_SUCCESS;
		UINTN						                      BankIndex;
		IPMI_SEL_EVENT_RECORD_DATA	          *SelRecord;
		UINT32	                              TimeStamp = 0;
		UMC_EXT_ERROR_CODE		                UmcExtErrorCode;
		UINT16						                    McaErrorCode;
		UINT8						                      McaExtErrorCode;
    UINT16                                MyGeneratorId;
    UINT8                                 MySensorNumber;
    UINT8                             ResponseBuff[16];
    UINT8                             ResponseSize;
		
		EfiSmGetTimeStamp (&TimeStamp);
		
		if((McaErrorRecord == NULL)||(ErrorBuffer == NULL)){
			return	EFI_INVALID_PARAMETER;
		}
    EfiInitializeIpmiBase ();

		ResponseSize = sizeof (ResponseBuff);
		SetMem (ResponseBuff, ResponseSize, 0);
    Status = EfiSendCommandToBMC (
					  0x6,
					  0x1,
					  (UINT8 *)NULL,
					  0,
					  (UINT8 *)&ResponseBuff,
					  (UINT8 *)&ResponseSize
					  );
		for(BankIndex = 0; BankIndex < 23; BankIndex++){
			if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Value == 0){
				continue;
			}
			switch (McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber){
				case MCA_UMC0_BANK:
				case MCA_UMC1_BANK:
          if(ResponseBuff[2]>3)//AMI BMC
          {
            MyGeneratorId = 0x21;
            MySensorNumber= 0xF9;
          }else{
            MyGeneratorId = 1;
            MySensorNumber = 0xA2;
          }
					SelRecord = (IPMI_SEL_EVENT_RECORD_DATA*)ErrorBuffer;
					
					SelRecord->TimeStamp = TimeStamp;
					SelRecord->RecordType = IPMI_SEL_SYSTEM_RECORD;
					SelRecord->GeneratorId = MyGeneratorId;//(UINT16)EFI_GENERATOR_ID(SMI_HANDLER_SOFTWARE_ID);
					SelRecord->EvMRevision = IPMI_EVM_REVISION;
					SelRecord->EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
					
					SelRecord->SensorType = 0x0C; // Memory
					SelRecord->SensorNumber = MySensorNumber;//ByoBMC define;
					//common data
					SelRecord->OEMEvData1 = ((EFI_OEM_SPECIFIC_DATA << 6) | (EFI_OEM_SPECIFIC_DATA << 4));
					SelRecord->OEMEvData2 = (McaErrorRecord->CpuInfo.DieId << 3)//Die Number
											|((McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber - MCA_UMC0_BANK)<<1);//Memory Controller Number
					
					SelRecord->OEMEvData3 = (McaErrorRecord->CpuInfo.SocketId << 5)//CPU Socket Number
											|((McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber - MCA_UMC0_BANK)<<2)//Channel Number
											|((DimmInfo->ChipSelect)>1?1:0);//DIMM Number
					
					UmcExtErrorCode.Value = (UINT8)McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.ErrorCodeExt;
					if(UmcExtErrorCode.Field.DramEccErr){
						SelRecord->OEMEvData2 |=(DramEccErr<<5);
					}else if(UmcExtErrorCode.Field.WriteDataPoisonErr){
						SelRecord->OEMEvData2 |=(WriteDataPoisonErr<<5);
					}else if(UmcExtErrorCode.Field.SdpParityErr) {
						SelRecord->OEMEvData2 |=(SdpParityErr<<5);
					}else if(UmcExtErrorCode.Field.ApbErr) {
						SelRecord->OEMEvData2 |=(ApbErr<<5);
					}else if(UmcExtErrorCode.Field.AddrCmdParityErr) {
						SelRecord->OEMEvData2 |=(AddressCommandParityErr<<5);
						SelRecord->OEMEvData1 |= 0x02;
					}else if(UmcExtErrorCode.Field.WriteDataCrcErr) {
						SelRecord->OEMEvData2 |=(WriteDataCrcErr<<5);
					}
					
					if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UECC){
						SelRecord->OEMEvData1 |= 0x01;
					}else if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.CECC) {
						SelRecord->OEMEvData1 |= 0x00;
					}
					DEBUG((EFI_D_INFO,"Send Sel Recorder is:%x %x %x %x %x %x %x %x %x %x %x\n",SelRecord->RecordId,SelRecord->RecordType,SelRecord->TimeStamp,SelRecord->GeneratorId,SelRecord->EvMRevision,SelRecord->SensorType,SelRecord->SensorNumber,SelRecord->EventDirType,SelRecord->OEMEvData1,SelRecord->OEMEvData2,SelRecord->OEMEvData3));
					break;	// End of UMC processing
				case MCA_LS_BANK:
				case MCA_IF_BANK:
				case MCA_L2_BANK:
				case MCA_DE_BANK:
				case MCA_EMPTY0_BANK://reserved
				case MCA_EX_BANK:
				case MCA_FP_BANK:
				case MCA_L3_C0_S0_BANK:
				case MCA_L3_C0_S1_BANK:
				case MCA_L3_C0_S2_BANK:
				case MCA_L3_C0_S3_BANK:
				case MCA_L3_C1_S0_BANK:
				case MCA_L3_C1_S1_BANK:
				case MCA_L3_C1_S2_BANK:
				case MCA_L3_C1_S3_BANK:
				case MCA_SMU_BANK:
				case MCA_PSP_BANK:
				case MCA_FUSE_BANK:
				case MCA_CS0_BANK:
				case MCA_CS1_BANK:
				case  MCA_PIE_BANK:
					if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Val){
					  if(ResponseBuff[2]>3)//AMI BMC
            {
              MyGeneratorId = 0x21;
              MySensorNumber= 0xFA;
            }else{
              MyGeneratorId = 1;
              MySensorNumber = 0xA5;
            }
						SelRecord = (IPMI_SEL_EVENT_RECORD_DATA*)ErrorBuffer;
	
						SelRecord->TimeStamp = TimeStamp;
						SelRecord->RecordType = IPMI_SEL_SYSTEM_RECORD;
						SelRecord->GeneratorId = MyGeneratorId;//(UINT16)EFI_GENERATOR_ID(SMI_HANDLER_SOFTWARE_ID);
						SelRecord->EvMRevision = IPMI_EVM_REVISION;
						SelRecord->EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
						
						// OEM PORTING NOTE - below values are just dummy, need to map with existing OEM error list
						// ---------------------------- start --------------------------------------------------
						SelRecord->SensorType = 0x07; // OEM Sensor Type 0xC0..0xFF
						SelRecord->SensorNumber = MySensorNumber;
						//SelRecord->OEMEvData1 = 0xA0 | 0x00;	  //BMC FW;
						SelRecord->OEMEvData1 = ((EFI_OEM_SPECIFIC_DATA << 6) | (EFI_OEM_SPECIFIC_DATA << 4));
						if (McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber == MCA_PIE_BANK) {
							// Correctable / Uncorrectable error
							if ((McaErrorRecord->McaBankErrorInfo[BankIndex].McaSyndMsr.Field.ErrorInformation & 0x3) == 0x01) {
								// Correctable
								SelRecord->OEMEvData1 |= 0xC;
							} else {
								// Uncorrectable
								SelRecord->OEMEvData1 |= 0xB;
							}
						}else{
							if (McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC) {
								SelRecord->OEMEvData1 |= 0x0B;
							}else {
								SelRecord->OEMEvData1 |= 0x0C;
							}
						}
						
						SelRecord->OEMEvData2 = (McaErrorRecord->CpuInfo.SocketId << 7) 
												| (McaErrorRecord->CpuInfo.DieId<<4);
						if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber >MCA_UMC1_BANK){
							SelRecord->OEMEvData2 |= (McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber - MCA_UMC1_BANK + MCA_L3_C0_S0_BANK);
						}else if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber >MCA_FP_BANK){
							SelRecord->OEMEvData2 |= MCA_L3_C0_S0_BANK;
						}else{
							SelRecord->OEMEvData2 |= (McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber);
						}
						
						//Check Error code types
						McaErrorCode = (UINT16)McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.ErrorCode;
						McaExtErrorCode = (UINT8)McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.ErrorCodeExt;
						SelRecord->OEMEvData3 =  McaExtErrorCode<<4;
						if (((McaErrorCode & TLB_ERROR_MASK) >> TLB_ERROR_CHK_SHIFT) == 1) {
							//TLB error
							SelRecord->OEMEvData3 |= 1 ;
						} else if (((McaErrorCode & MEM_ERROR_MASK) >> MEM_ERROR_CHK_SHIFT) == 1) {
							//Memory error
							SelRecord->OEMEvData3 |= 2 ;
						} else if (((McaErrorCode & INT_ERROR_MASK) >> INT_ERROR_CHK_SHIFT) == 1) {
						  //Bus Error
							SelRecord->OEMEvData3 |= 3 ;
						} else if (((McaErrorCode & PP_ERROR_MASK) >> PP_ERROR_CHK_SHIFT) == 1) {
						  //Internal unclassified error
							SelRecord->OEMEvData3 |= 4 ;
						}
	          DEBUG((EFI_D_INFO,"Send Sel Recorder is:%x %x %x %x %x %x %x %x %x %x %x\n",SelRecord->RecordId,SelRecord->RecordType,SelRecord->TimeStamp,SelRecord->GeneratorId,SelRecord->EvMRevision,SelRecord->SensorType,SelRecord->SensorNumber,SelRecord->EventDirType,SelRecord->OEMEvData1,SelRecord->OEMEvData2,SelRecord->OEMEvData3));
						// ---------------------------- end ----------------------------------------------------
					} else {
						Status = EFI_NOT_FOUND;
					}
					break;	// End of PIE processing
				default:
					Status = EFI_NOT_FOUND;
					break;
			}
			if(Status == EFI_SUCCESS){
				break;
			}
		}
		
		return	Status;
	}

	
	
	/**
		Translate/convert, if possible, error entry from NBIO to Ipmi format
	
		@param RAS_NBIO_ERROR_INFO *NbioErrorRecord - pointer to NBIO error structure
		@param UINT8			  *ErrorBuffer - buffer with Ipmi error entry, only data
	
		@retval EFI_SUCCESS - Ipmi error log entry created
		@retval EFI_INVALID_PARAMETER - input param contains null pointer
		@retval EFI_NOT_FOUND - translation cannot be done
	
	**/
	
	EFI_STATUS
	NbioToIpmi(
	  IN  RAS_NBIO_ERROR_INFO	*NbioErrorRecord,
	  IN OUT UINT8				*ErrorBuffer
	  )
	{
		EFI_STATUS					Status = EFI_SUCCESS;
		IPMI_SEL_EVENT_RECORD_DATA	*SelRecord;
		UINT32	  TimeStamp = 0;
		//DEBUG((EFI_D_ERROR, "zzzzzzzzz %a L%d\n", __FUNCTION__, __LINE__));
		EfiSmGetTimeStamp (&TimeStamp);
		//DEBUG((EFI_D_ERROR, "zzzzzzzzz %a L%d\n", __FUNCTION__, __LINE__));
		if((NbioErrorRecord == NULL)||(ErrorBuffer == NULL)){
			return	EFI_INVALID_PARAMETER;
		}
		//DEBUG((EFI_D_ERROR, "zzzzzzzzz %a L%d\n", __FUNCTION__, __LINE__));
		SelRecord = (IPMI_SEL_EVENT_RECORD_DATA*)ErrorBuffer;
						
		SelRecord->TimeStamp = TimeStamp;
		SelRecord->RecordType = IPMI_SEL_SYSTEM_RECORD;
		SelRecord->GeneratorId = EFI_SOFTWARE_ID;//(UINT16)EFI_GENERATOR_ID(SMI_HANDLER_SOFTWARE_ID);
		SelRecord->EvMRevision = IPMI_EVM_REVISION;
		SelRecord->EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
		//DEBUG((EFI_D_ERROR, "zzzzzzzzz %a L%d\n", __FUNCTION__, __LINE__));
	// PORTING PORTING	  
		SelRecord->SensorType = 0x07;
		SelRecord->SensorNumber = 0xA6; // Correctable
		SelRecord->OEMEvData1 = ((EFI_OEM_SPECIFIC_DATA << 6) | (EFI_OEM_SPECIFIC_DATA << 4) | NbioErrorRecord->TypeId);
		//SelRecord->OEMEvData1 = 0xA0;
		SelRecord->OEMEvData2 = (NbioErrorRecord->GroupId << 3) | (NbioErrorRecord->Die);
		SelRecord->OEMEvData3 = NbioErrorRecord->TypeId;//todo
		//DEBUG((EFI_D_ERROR, "zzzzzzzzz %a L%d\n", __FUNCTION__, __LINE__));
	// -----------------------		
	DEBUG((EFI_D_INFO,"Send Sel Recorder is:%x %x %x %x %x %x %x %x %x %x %x\n",SelRecord->RecordId,SelRecord->RecordType,SelRecord->TimeStamp,SelRecord->GeneratorId,SelRecord->EvMRevision,SelRecord->SensorType,SelRecord->SensorNumber,SelRecord->EventDirType,SelRecord->OEMEvData1,SelRecord->OEMEvData2,SelRecord->OEMEvData3));
		return Status;
	}


	
	/**
		Translate/convert, if possible, error entry from PCIe to Ipmi format
	
		@param GENERIC_PCIE_AER_ERR_ENTRY *GenPcieAerErrEntry - pointer to GENERIC_PCIE_AER_ERR_ENTRY structure
		@param UINT8			  *ErrorBuffer - buffer with Ipmi error entry, only data
	
		@retval EFI_SUCCESS - Ipmi error log entry created
		@retval EFI_INVALID_PARAMETER - input param contains null pointer
		@retval EFI_NOT_FOUND - translation cannot be done
	
	**/
	
	EFI_STATUS
	PcieToIpmi(
	  IN  GENERIC_PCIE_AER_ERR_ENTRY *GenPcieAerErrEntry,
	  IN OUT UINT8			*ErrorBuffer
	  )
	{
		EFI_STATUS					Status = EFI_SUCCESS;
		IPMI_SEL_EVENT_RECORD_DATA	*SelRecord;
		UINT32	  TimeStamp = 0;
    UINT16                                MyGeneratorId;
    UINT8                                 MySensorNumber;
    UINT8                             ResponseBuff[16];
    UINT8                             ResponseSize;
		
		EfiSmGetTimeStamp (&TimeStamp);
		if((GenPcieAerErrEntry == NULL)||(ErrorBuffer == NULL)){
			return	EFI_INVALID_PARAMETER;
		}

    EfiInitializeIpmiBase ();

		ResponseSize = sizeof (ResponseBuff);
		SetMem (ResponseBuff, ResponseSize, 0);
    Status = EfiSendCommandToBMC (
					  0x6,
					  0x1,
					  (UINT8 *)NULL,
					  0,
					  (UINT8 *)&ResponseBuff,
					  (UINT8 *)&ResponseSize
					  );

    if(ResponseBuff[2]>3)//AMI BMC
    {
       MyGeneratorId = 0x21;
       MySensorNumber= 0xEF;
    }else{
       MyGeneratorId = 1;
       MySensorNumber = 0xA3;
    }
		
		SelRecord = (IPMI_SEL_EVENT_RECORD_DATA*)ErrorBuffer;
						
		SelRecord->TimeStamp = TimeStamp;
		SelRecord->RecordType = IPMI_SEL_SYSTEM_RECORD;
		SelRecord->GeneratorId = MyGeneratorId;//(UINT16)EFI_GENERATOR_ID(SMI_HANDLER_SOFTWARE_ID);
		SelRecord->EvMRevision = IPMI_EVM_REVISION;
		SelRecord->EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
	
	// PORTING PORTING	  
		SelRecord->SensorType = 0x13; //SEL_SENS_TYPE_CRIT_ERR	  
		SelRecord->SensorNumber = MySensorNumber; // Correctable
		SelRecord->OEMEvData1 = ((EFI_OEM_SPECIFIC_DATA << 6) | (EFI_OEM_SPECIFIC_DATA << 4));
		
		if(GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity == ERROR_RECOVERABLE || GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity == ERROR_SEVERITY_FATAL)
		{
			SelRecord->OEMEvData1 = 0xA0 | 0x05 ;//CRITICAL_INTERRUPT_PCI_SERR
		} else if (GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity == ERROR_SEVERITY_CORRECTED)
		{
			SelRecord->OEMEvData1 = 0xA0 | 0x04 ;//CRITICAL_INTERRUPT_PCI_PERR		
		} else {
			SelRecord->OEMEvData1 = 0xA0 | 0x04 ;//CRITICAL_INTERRUPT_PCI_PERR		
		}
		SelRecord->OEMEvData2 = (UINT8)GenPcieAerErrEntry->PcieAerErrorSection.DeviceId.PrimaryBus;
		SelRecord->OEMEvData3 = (UINT8)( GenPcieAerErrEntry->PcieAerErrorSection.DeviceId.Device << 3 | 
											GenPcieAerErrEntry->PcieAerErrorSection.DeviceId.Function);
	DEBUG((EFI_D_INFO,"Send Sel Recorder is:%x %x %x %x %x %x %x %x %x %x %x\n",SelRecord->RecordId,SelRecord->RecordType,SelRecord->TimeStamp,SelRecord->GeneratorId,SelRecord->EvMRevision,SelRecord->SensorType,SelRecord->SensorNumber,SelRecord->EventDirType,SelRecord->OEMEvData1,SelRecord->OEMEvData2,SelRecord->OEMEvData3));
	// -------------------------------
		return Status;
	}


EFI_STATUS
MemTestToIpmi(
  IN  UINT8                         Socket,
  IN  UINT8                         Channel,
  IN  UINT8                         Dimm,
  IN  OUT UINT8                     *ErrorBuffer,
  IN  OUT UINTN                     *ErrorBufferSize
  )
{
  if(ErrorBuffer == NULL){
    return  EFI_INVALID_PARAMETER;
  }

  SetMem(ErrorBuffer, MEM_TEST_TRAINING_DATA_SIZE, 0);

  ErrorBuffer[0] = MANUFACTURER_ID0;
  ErrorBuffer[1] = MANUFACTURER_ID1;
  ErrorBuffer[2] = MANUFACTURER_ID2;
  ErrorBuffer[3] = SUBCMD_MEM_TEST_TRAINING;

  ErrorBuffer[4] = Socket;
  ErrorBuffer[5] = Channel;
  ErrorBuffer[6] = Dimm;

  *ErrorBufferSize = MEM_TEST_TRAINING_DATA_SIZE;

  return EFI_SUCCESS;
}

EFI_STATUS
MemTrainingToIpmi(
  IN  UINT8                         Socket,
  IN  UINT8                         Channel,
  IN  UINT8                         Dimm,
  IN  OUT UINT8                     *ErrorBuffer,
  IN  OUT UINTN                     *ErrorBufferSize
  )
{
  if(ErrorBuffer == NULL){
    return  EFI_INVALID_PARAMETER;
  }

  SetMem(ErrorBuffer, MEM_TEST_TRAINING_DATA_SIZE, 0);

  ErrorBuffer[0] = MANUFACTURER_ID0;
  ErrorBuffer[1] = MANUFACTURER_ID1;
  ErrorBuffer[2] = MANUFACTURER_ID2;
  ErrorBuffer[3] = SUBCMD_MEM_TEST_TRAINING;

  ErrorBuffer[4] = Socket;
  ErrorBuffer[5] = Channel;
  ErrorBuffer[6] = Dimm;

  *ErrorBufferSize = MEM_TEST_TRAINING_DATA_SIZE;

  return EFI_SUCCESS;
}
