/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  WheaElog.c

Abstract:
  Source file for the IPMI driver.

Revision History:

**/



#include <Protocol/SdrSensorDevProtocol.h>
#include <Ipmi/SetupBmcCfg/HwMonitor/Hm.h>

//#pragma warning(disable:4146)

//#define SDR_DEBUG

SENSOR_READING          sr[200];
UINT8                   sr_Index = 0;
//EFI_IPMI_TRANSPORT     *mIpmiTransport = NULL;

#define Max_Sensor_Val_Number 200
OEM_SENSOR_READING_VAL  SDR_M_B_K1_K2[Max_Sensor_Val_Number];

UINT16
SdrGetReservation(){
  EFI_STATUS            Status;
  UINT32                DataSize;
  SDR_RESERVE_REPO_RS   sdr_reserve_repo_rs;


  DataSize = sizeof (SDR_RESERVE_REPO_RS);
  ZeroMem (&sdr_reserve_repo_rs, DataSize);
  Status = EfiSendCommandToBMC(
             EFI_SM_NETFN_STORAGE,
             EFI_STORAGE_RESERVE_SDR_REPOSITORY,
             NULL,
             0,
             (UINT8*) (&sdr_reserve_repo_rs),
             (UINT8*) &DataSize 
             );
  if(EFI_ERROR (Status)){
    return 0;
  } 
  return sdr_reserve_repo_rs.reserve_id;
}

EFI_STATUS
GetSdrRecord(
 IN EFI_IPMI_SDR_RECORD_STRUCT_HEADER SdrHeader,
 OUT EFI_IPMI_SENSOR_RECORD_STRUCT *SdrRecord
)
{
  EFI_STATUS                        Status = EFI_SUCCESS;
  UINT8                             CurrentLen;
  UINT8                             Len;
  UINT8                             SdrMaxReadLen = SDR_MAX_READ_LEN;
  EFI_IPMI_GET_SDR                  GetSdr;
  UINT8                             Buffer[160];
  UINT8                             RecordBuffer[160];
  UINT32                            DataSize;
#ifdef SDR_DEBUG
  UINT8   i;
#endif  
  CurrentLen = 0;
  Len = SdrHeader.RecordLength + 5;   //add header size
  SdrMaxReadLen = SDR_MAX_READ_LEN;
  GetSdr.RecordId = SdrHeader.RecordId;
  GetSdr.ReservationId = SdrGetReservation();
  while(CurrentLen < Len){  
      GetSdr.RecordOffset  = CurrentLen;
      if((Len-CurrentLen)<SdrMaxReadLen){
        GetSdr.BytesToRead  = Len-CurrentLen;
      }else{
        GetSdr.BytesToRead  = SdrMaxReadLen;
      }            
      ZeroMem(Buffer, sizeof(Buffer));
      DataSize = sizeof(Buffer);
      Status = EfiSendCommandToBMC(
                 EFI_SM_NETFN_STORAGE,
                 EFI_STORAGE_GET_SDR,
                 (UINT8*) &GetSdr,
                 sizeof(EFI_IPMI_GET_SDR),
                 (UINT8*) &Buffer,
                 (UINT8*) &DataSize
                 );
      if(Status == EFI_OUT_OF_RESOURCES){
         SdrMaxReadLen = GetSdr.BytesToRead - 1;  //comp code == 0xca ,so too many bytes to read ,reduce one 
         continue;
      }
      CopyMem(RecordBuffer + CurrentLen, Buffer+2, GetSdr.BytesToRead );  // buffer + 2 ,skipp next record id uint16
      CurrentLen+=GetSdr.BytesToRead;
  }


#ifdef SDR_DEBUG
//debug            
DEBUG((EFI_D_ERROR,"Record BufferData:\n"));                                
for(i=0;i<Len;i++){
DEBUG((EFI_D_ERROR," 0x%X",RecordBuffer[i]));
}
DEBUG((EFI_D_ERROR,"\n"));            
//debug
#endif

  CopyMem(SdrRecord, RecordBuffer, Len);
  return Status;
}

EFI_STATUS
EfiSensorReading(
  IN  SDR_SENOSR_DEV_PROTOCOL  *This,
  IN  UINT8                     SenorNum,
  IN  UINT8                     Lun,
  OUT UINTN                    *Value,
  OUT UINT8                    *Div
){
  EFI_STATUS                        Status = EFI_SUCCESS;
  UINT8                             Data8 = 0;
  UINT8                             Buffer[20];
  UINT32                            DataSize;

  UINT8                             index = 0;
  UINTN                             Mul;

  Data8 = SenorNum;
  DataSize = sizeof(Buffer);
  ZeroMem(Buffer, DataSize);

  Status = EfiSendCommandToBMC(
             EFI_SM_NETFN_SENSOR,
             EFI_GET_SENSOR_READING,
             (UINT8*) &Data8,
             sizeof(UINT8),
             (UINT8*) &Buffer,
             (UINT8*) &DataSize
             );
  if(!EFI_ERROR (Status)){
    CopyMem(Value, Buffer, sizeof(UINTN));
//    *Value = Buffer[0];
  }else{
    *Value = 0;
  }

//Byosoft + {
  while(index < Max_Sensor_Val_Number) {
    
    if (SDR_M_B_K1_K2[index].SensorNumber == SenorNum) {
      if(SDR_M_B_K1_K2[index].Type == 1) //temparue sensor
        break;

      if(SDR_M_B_K1_K2[index].Type == 2) { //Voltage sensor
        *Div = SDR_M_B_K1_K2[index].K2;
        Mul = ((UINT8)(*Value)) * SDR_M_B_K1_K2[index].M;
        *Value = Mul;
        break;
      }

      if(SDR_M_B_K1_K2[index].Type == 0x0b) { //watts sensor
        *Div = 0x88;
        Mul = ((UINT8)(*Value)) * SDR_M_B_K1_K2[index].M;
        *Value = Mul;
        break;
      }

      if(SDR_M_B_K1_K2[index].Type == 4) { //fanspeed sensor
        *Div = 0x88;
        Mul = ((UINT8)(*Value)) * SDR_M_B_K1_K2[index].M;
        *Value = Mul;
        break;
      }
    }

    index++;  
  }
//Byosoft + }

  return Status;
}


#if 0
EFI_STATUS
GetSensorReadingFactors(EFI_IPMI_SDR_RECORD_STRUCT_1 *full,UINT8 Reading)
{
    CHAR8      Id[17];
    EFI_STATUS Status;
    UINT8       CommandData[20];
    UINT8       CommandSize;
    UINT8       ResponeData[20];
    UINT8       ResponeSize;

    ZeroMem(CommandData,sizeof(CommandData));
    ZeroMem(ResponeData,sizeof(ResponeData));
    ZeroMem(Id, sizeof(Id));
    CopyMem(Id, full->AsciiIdString, 16);

    CommandData[0] = full->SensorNumber;
    CommandData[1] = Reading;
    CommandSize = sizeof(CommandData);
    
    Status = mIpmiTransport->SendIpmiCommand(
                             mIpmiTransport,
                             EFI_SM_NETFN_SENSOR,
                             full->OwnerLun,
                             EFI_GET_SENSOR_FACTORS_READING,
                             (UINT8*) &CommandData[0],
                             CommandSize,
                             (UINT8*) &ResponeData,
                             (UINT8*) &ResponeSize
                           );
     if(ResponeData == NULL){
        return EFI_DEVICE_ERROR;
     }

     CopyMem(&full->Mtol, &ResponeData[1], sizeof(full->Mtol));
     CopyMem(&full->Bacc, &ResponeData[3], sizeof(full->Bacc));
     return 0;
}



UINT8
HasAnalogReading()
{

    /* Compact sensors can't return analog values so we false */
    if (!sr[sr_Index].full) {
        return 0;
    }

    if ( UNITS_ARE_DISCRETE(sr[sr_Index].full) ) {
        return 0;/* Sensor specified as not having Analog Units */
    }

    /*
     * If sensor has linearization, then we should be able to update the
     * reading factors and if we cannot fail the conversion.
     */
    if (sr[sr_Index].full->Linearization >= SDR_SENSOR_L_NONLINEAR &&
        sr[sr_Index].full->Linearization <= 0x7F) {
        if (GetSensorReadingFactors(sr[sr_Index].full, sr[sr_Index].s_reading) != EFI_SUCCESS){
            sr[sr_Index].s_reading_valid = 0;
            return 0;
        }
    }

    return 1;


}

/* sdr_convert_sensor_reading  -  convert raw sensor reading
 *
 * @sensor:    sensor record
 * @val:    raw sensor reading
 *
 * returns floating-point sensor reading
 */
double
SdrConvertSensorReading(
   EFI_IPMI_SDR_RECORD_STRUCT_1 *sensor, 
   UINT8 val
)
{
    INT32 m, b, k1, k2;
    double result = 0;

    m = __TO_M(sensor->Mtol);
    b = __TO_B(sensor->Bacc);
    k1 = __TO_B_EXP(sensor->Bacc);
    k2 = __TO_R_EXP(sensor->Bacc);

    switch ((sensor->SensorUnits1 &0xC0) >> 5) {
    case 0:
        result = (double) (((m * val) +
                    (b * pow(10, k1))) * pow(10, k2));
        break;
    case 1:
        if (val & 0x80)
            val++;
        /* Deliberately fall through to case 2. */
    case 2:
        result = (double) (((m * (UINT8) val) +
                    (b * pow(10, k1))) * pow(10, k2));
        break;
    default:
        /* Oops! This isn't an analog sensor. */
        return 0.0;
    }

    switch (sensor->Linearization & 0x7f) {
    case SDR_SENSOR_L_LN:
        result = log(result);
        break;
    case SDR_SENSOR_L_LOG10:
        result = log10(result);
        break;
    case SDR_SENSOR_L_LOG2:
        result = (double) (log(result) / log(2.0));
        break;
    case SDR_SENSOR_L_E:
        result = exp(result);
        break;
    case SDR_SENSOR_L_EXP10:
        result = pow(10.0, result);
        break;
    case SDR_SENSOR_L_EXP2:
        result = pow(2.0, result);
        break;
    case SDR_SENSOR_L_1_X:
        result = pow(result, -1.0);    /*1/x w/o exception */
        break;
    case SDR_SENSOR_L_SQR:
        result = pow(result, 2.0);
        break;
    case SDR_SENSOR_L_CUBE:
        result = pow(result, 3.0);
        break;
    case SDR_SENSOR_L_SQRT:
        result = sqrt(result);
        break;
    case SDR_SENSOR_L_CUBERT:
//        result = cbrt(result);
//        break;
    case SDR_SENSOR_L_LINEAR:
    default:
        break;
    }
    return result;
}

#endif

EFI_STATUS
EfiSensorLookup()
{
  EFI_STATUS   Status = EFI_SUCCESS;

  EFI_IPMI_SDR_REPOSITORY_INFO      RepositoryInfo;
  EFI_IPMI_SENSOR_RECORD_STRUCT     SdrRecord;
  EFI_IPMI_SDR_RECORD_STRUCT_HEADER SdrHeader;
  EFI_IPMI_GET_SDR                  GetSdr;

  UINT8                             Buffer[160];
  UINT32                            DataSize;
  UINTN                             Index;
  //UINT8                             IdLen;
  //UINT8                             Data8 = 0;

  DEBUG((EFI_D_ERROR,"%a\n",__FUNCTION__));
  //
  // Issue Get SDR Respository Info command
  //
  DataSize = sizeof (EFI_IPMI_SDR_REPOSITORY_INFO);
  ZeroMem (&RepositoryInfo, DataSize);
  Status = EfiSendCommandToBMC(
             EFI_SM_NETFN_STORAGE,
             EFI_STORAGE_GET_SDR_REPOSITORY_INFO,
             NULL,
             0,
             (UINT8*) (&RepositoryInfo),
             (UINT8*) &DataSize
             ); 
                         
  if(EFI_ERROR (Status)){
    return EFI_DEVICE_ERROR;
  }
  GetSdr.ReservationId = SdrGetReservation();
  GetSdr.RecordId      = 0x0000;
  GetSdr.RecordOffset  = 0x00;
  GetSdr.BytesToRead   = 0x05;  //read header
  ZeroMem(&SdrHeader, sizeof(SdrHeader));
  ZeroMem(&SdrRecord, sizeof(SdrRecord));
  
  for (Index=0; (Index < RepositoryInfo.RecordCount) && (GetSdr.RecordId != 0xFFFF); Index++) {
    DataSize = sizeof (EFI_IPMI_SENSOR_RECORD_STRUCT);   //Size = 0x40    
    ZeroMem(Buffer, sizeof(Buffer));
    Status = EfiSendCommandToBMC(
               EFI_SM_NETFN_STORAGE,
               EFI_STORAGE_GET_SDR,
               (UINT8*) &GetSdr,
               sizeof(EFI_IPMI_GET_SDR),
               (UINT8*) &Buffer,
               (UINT8*) &DataSize
               );                
    if(EFI_ERROR (Status)){
      return EFI_DEVICE_ERROR;
    }
    CopyMem(&SdrHeader, Buffer, DataSize);
    DEBUG ((EFI_D_ERROR, " RecordVersion =%x \n",SdrHeader.Version));
    DEBUG ((EFI_D_ERROR, " EFI_STORAGE_GET_SDR = %r  DataSize = %x\n",Status,DataSize));
    DEBUG ((EFI_D_ERROR, " RecordID = %x, NextRecord ID =%x\n",SdrHeader.RecordId,SdrHeader.NextRecordId));  
    DEBUG ((EFI_D_ERROR, " RecordType = %x RecordLen = %x\n",SdrHeader.RecordType,SdrHeader.RecordLength));
    gBS->Stall(10 * 1000);   //byosoft +
   
    switch(SdrHeader.RecordType){
      case SDR_RECORD_TYPE_FULL_SENSOR:
         GetSdrRecord(SdrHeader ,&SdrRecord);
         if(SdrRecord.SensorType1.SensorType == 0x01 ||  // temp
            SdrRecord.SensorType1.SensorType == 0x02 ||  // volta
            SdrRecord.SensorType1.SensorType == 0x0B ||  // Watts
            SdrRecord.SensorType1.SensorType == 0x04) {  // fan
             DEBUG((EFI_D_ERROR,"Here we found sensor type %x,id name = %-16a \n",SdrRecord.SensorType1.SensorType,SdrRecord.SensorType1.AsciiIdString));
             DEBUG((EFI_D_ERROR," SensorNumber = %x\n",SdrRecord.SensorType1.SensorNumber));
             //CopyMem(sr[sr_Index].full, &SdrRecord, sizeof(SdrRecord));
             //IdLen = sr[sr_Index].full->IdStringLength & 0x1f;
             //IdLen = IdLen < sizeof(sr[sr_Index].s_id) ?
             //              IdLen : sizeof(sr[sr_Index].s_id) - 1;
             //CopyMem(sr[sr_Index].s_id, sr[sr_Index].full->AsciiIdString, IdLen);
             //sr[sr_Index].s_recordtype = SdrHeader.RecordType;
             //Data8 = SdrRecord.SensorType1.SensorNumber;
             /*Status = mIpmiTransport->SendIpmiCommand(
                                       mIpmiTransport,
                                       EFI_SM_NETFN_SENSOR,
                                       SdrRecord.SensorType1.OwnerLun,
                                       EFI_GET_SENSOR_READING,
                                       (UINT8*) &Data8,
                                       sizeof(UINT8),
                                       (UINT8*) &Buffer,
                                       (UINT8*) &DataSize
                                     );
             DEBUG((EFI_D_ERROR,"EFI_GET_SENSOR_READING =%r DataSize =%x\n",Status,DataSize));*/
             //if(DataSize < 2){
             //   break;  //invalid len ,break
              
             //}
             /*
#ifdef SDR_DEBUG
{
UINT8 i;
//debug            
DEBUG((EFI_D_ERROR,"Buffer:\n"));                                
for(i=0;i<DataSize;i++){
DEBUG((EFI_D_ERROR," 0x%X",Buffer[i]));
}
}
DEBUG((EFI_D_ERROR,"\n"));            
//debug
#endif       
             */
            /* if (IS_READING_UNAVAILABLE(Buffer[1]))     
               sr[sr_Index].s_reading_unavailable;
             if (IS_SCANNING_DISABLED(Buffer[1])) {
                sr[sr_Index].s_scanning_disabled = 1;
             }
            if ( !sr[sr_Index].s_reading_unavailable ) {
                sr[sr_Index].s_reading_valid = 1;
                sr[sr_Index].s_reading = Buffer[0];
            }
            if (DataSize > 2)
                sr[sr_Index].s_data2   = Buffer[2];
            if (DataSize > 3)
                sr[sr_Index].s_data3   = Buffer[3];*/

            if(sr_Index < Max_Sensor_Val_Number) {
              SDR_M_B_K1_K2[sr_Index].SensorNumber = SdrRecord.SensorType1.SensorNumber;
              SDR_M_B_K1_K2[sr_Index].M            = SdrRecord.SensorType1.MLo + ((UINT16)(SdrRecord.SensorType1.MHi) << 8);
              SDR_M_B_K1_K2[sr_Index].B            = SdrRecord.SensorType1.BLo + ((UINT16)(SdrRecord.SensorType1.BHi) << 8);             
              SDR_M_B_K1_K2[sr_Index].K1           = SdrRecord.SensorType1.BExp & 0x0F;
              SDR_M_B_K1_K2[sr_Index].K2           = (SdrRecord.SensorType1.RExp & 0xF0 >> 4);
              SDR_M_B_K1_K2[sr_Index].Type         = SdrRecord.SensorType1.SensorType;
            }
            sr_Index++;
         }
        break;
      case SDR_RECORD_TYPE_COMPACT_SENSOR:
        if(SdrRecord.SensorType2.SensorType == 0x01){ // temp

        }
        break;
      case SDR_RECORD_TYPE_EVENTONLY_SENSOR:

        if(SdrRecord.SensorType3.SensorType == 0x01){ // temp

        }
        break;
      default:
        break;
    }
    GetSdr.RecordId = SdrHeader.NextRecordId;
  }

  return Status;
}





EFI_STATUS
SdrSensorDevEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
/*++
  
  Routine Description:
    This is the standard EFI driver point. This function intitializes
    the private data required for creating SOL Status Driver.
    
  Arguments:
    ImageHandle     - Handle for the image of this driver
    SystemTable     - Pointer to the EFI System Table

  Returns:
    EFI_SUCCESS     - Protocol successfully installed
    EFI_UNSUPPORTED - Protocol can't be installed.

--*/
{
  EFI_STATUS              Status = EFI_SUCCESS;
  SDR_SENOSR_DEV_PROTOCOL  *SensorDev = NULL;
  EFI_HANDLE                Handle = NULL;

  sr_Index = 0;
  ZeroMem(sr, sizeof(SENSOR_READING));
  SensorDev = AllocateZeroPool(sizeof(SDR_SENOSR_DEV_PROTOCOL));
  //Status = gBS->LocateProtocol (
  //           &gEfiIpmiTransportProtocolGuid,
  //           NULL,
  //           &mIpmiTransport
  //         );
  //ASSERT_EFI_ERROR(Status);
  EfiInitializeIpmiBase();
  Status = EfiSensorLookup();
  ASSERT_EFI_ERROR(Status);

  SensorDev->SensorReading = EfiSensorReading;
  Status = gBS->InstallProtocolInterface (
                   &Handle,
                   &gSdrSensorDevProtocolGuid,
                   EFI_NATIVE_INTERFACE,
                   SensorDev
                   );              
  ASSERT_EFI_ERROR(Status);
  
  return Status;
}
