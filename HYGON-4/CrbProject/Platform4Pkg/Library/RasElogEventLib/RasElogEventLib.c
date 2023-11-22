/** @file

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
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

#include "RasElogEventLib.h"
#include <Library/HobLib.h>


#define MAX_DIMMS_SUPPORT         48

typedef struct {
  BYO_HYGON_DIMM_MAP_INFO         *MapInfo;
  BOOLEAN                         Pfeh;
  UINT8                           ErrorLogData[0x80];
  
  UINT16                          McaErrThreshSwCount;
  UINT16                          McaErrThreshCount;
  UINT16                          McaErrThresh2Count;  
  UINT16                          MemLeakybucketMinites;
  UINT16                          MemLeakybucketOnceCount;
  BOOLEAN                         ClearMemCeThresholdEvery24H;
  BOOLEAN                         Is2phase;
  UINT8                           MemLeakybucketMiniutesOrSeconds;
  UINT8                           MemTinyStormThresholdCount;
  UINT16                          MemTinyStormTimeIntervalSec;

  UINT32                          TimeStamp[MAX_DIMMS_SUPPORT];
  UINT8                           CeCountphase[MAX_DIMMS_SUPPORT];
  UINT8                           TinyStormCount[MAX_DIMMS_SUPPORT];
  BOOLEAN                         InTinyStorm[MAX_DIMMS_SUPPORT];  
} BYO_RAS_LOG_CTX;

STATIC BYO_RAS_LOG_CTX gByoRasLogCtx;



UINT16 GetMemCeThru(UINT8 DimmIndex)
{
  BYO_RAS_LOG_CTX  *Ctx = &gByoRasLogCtx;
  UINT16           Thru = 0xFFFF;

  if(Ctx->Is2phase && DimmIndex < ARRAY_SIZE(Ctx->CeCountphase)){
    if(Ctx->CeCountphase[DimmIndex] == 1){
      Thru = Ctx->McaErrThresh2Count;
    } else if(Ctx->CeCountphase[DimmIndex] == 0){
      Thru = Ctx->McaErrThreshCount;
    }
  }

  DEBUG((EFI_D_INFO, "GetMemCeThru(%d):%d\n", DimmIndex, Thru));
  
  return Thru;
}


UINT8 GetDimmChannel(UINT8 Socket, UINT8 Cdd, UINT8 UmcChannel)
{
  UINTN                    Index;
  BYO_HYGON_DIMM_MAP_DATA  *MapData = gByoRasLogCtx.MapInfo->MapData;

  for(Index=0;Index<gByoRasLogCtx.MapInfo->MapDataCount;Index++){
    if(MapData[Index].Socket == Socket && MapData[Index].Cdd == Cdd && MapData[Index].UMCchannel == UmcChannel){
      return MapData[Index].Channel;
    }
  }

  return 0xFF;
}


BOOLEAN
IsIgnoreThisMemCe (
  UINT32	   TimeStamp,
  UINT8      DimmIndex
  )
{
  UINTN            Reduce;
  UINT32           Seconds;
  BYO_RAS_LOG_CTX  *Ctx = &gByoRasLogCtx;
  

  DEBUG((EFI_D_INFO, "CeChk(0x%x,%d)\n", TimeStamp, DimmIndex));

  if(DimmIndex >= ARRAY_SIZE(Ctx->TimeStamp) || TimeStamp <= Ctx->TimeStamp[DimmIndex]){
    return FALSE;
  }

  Seconds = TimeStamp - Ctx->TimeStamp[DimmIndex];
  Reduce  = Seconds/(Ctx->MemLeakybucketMinites * Ctx->MemLeakybucketMiniutesOrSeconds) * Ctx->MemLeakybucketOnceCount;

  if(Ctx->Is2phase){
    Ctx->CeCountphase[DimmIndex]++;
    if(Ctx->CeCountphase[DimmIndex] == 2){
      Ctx->CeCountphase[DimmIndex] = 0;
    } else {
      DEBUG((EFI_D_INFO, "ignore phase 0\n"));
      return TRUE;
    }
  }

  Seconds = TimeStamp - Ctx->TimeStamp[DimmIndex];
  Ctx->TimeStamp[DimmIndex] = TimeStamp;

  DEBUG((EFI_D_INFO, "Seconds:%d\n", Seconds));

// meet Threshold in T0, enter tiny storm.
// meet Threshold out of T0, exit tiny storm. report depends on Leakybucket.
// meet 3 tiny storm, report a CE.
  if(Ctx->MemTinyStormTimeIntervalSec){
    if(Seconds < Ctx->MemTinyStormTimeIntervalSec){
      Ctx->TinyStormCount[DimmIndex]++;
      Ctx->InTinyStorm[DimmIndex] = TRUE;
      DEBUG((EFI_D_INFO, "TinyStormCount:%d\n", Ctx->TinyStormCount[DimmIndex]));
      if(Ctx->TinyStormCount[DimmIndex] >= Ctx->MemTinyStormThresholdCount){
        Ctx->TinyStormCount[DimmIndex] = 0;
        DEBUG((EFI_D_INFO, "TinyStorm report\n"));
        return FALSE;
      }
      return TRUE;
      
    } else {
      if(Ctx->InTinyStorm[DimmIndex]){
        Ctx->InTinyStorm[DimmIndex] = FALSE;
        DEBUG((EFI_D_INFO, "TinyStorm exit\n"));
      }
      Ctx->TinyStormCount[DimmIndex] = 0;
    }
  }

  if(Ctx->MemLeakybucketMinites == 0 || Ctx->MemLeakybucketOnceCount == 0){
    Reduce = 0;
  } else {
    Reduce  = Seconds/(Ctx->MemLeakybucketMinites * Ctx->MemLeakybucketMiniutesOrSeconds) * Ctx->MemLeakybucketOnceCount;
    DEBUG((EFI_D_INFO, "Seconds:%d, Reduce:%d, Th:%d\n", Seconds, Reduce, Ctx->McaErrThreshSwCount));
  }
  if(Ctx->McaErrThreshSwCount <= Reduce || (Ctx->ClearMemCeThresholdEvery24H && Seconds >= 60*60*24)){
    DEBUG((EFI_D_INFO, "ignore\n"));
    return TRUE;
  }
  
  return FALSE;
}


EFI_STATUS
SendErrorLogDataToBmc (
  IN  UINT8                         *ElogData,
  IN  UINTN                         DataSize
  )
{
  EFI_STATUS                  Status;
  UINT8                       ResponseData[2];
  UINT8                       ResponseDataSize = 2;

  if(gByoRasLogCtx.Pfeh){
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_STORAGE,
              EFI_STORAGE_ADD_SEL_ENTRY,
              (UINT8*)ElogData,
              (UINT8)DataSize,
              ResponseData,
              &ResponseDataSize
              );
  } else {
    Status = EFI_UNSUPPORTED;
  }

  return Status;
}



EFI_STATUS
SendElogEventMca (
  IN  UINT8        *ErrorRecord,
  IN  UINT8        BankIndex,
  IN  DIMM_INFO    *DimmInfo
)
{
  EFI_STATUS            Status;
  UINTN                 DataSize;

  DEBUG((EFI_D_INFO, "SendElogEventMca(bank:%d)\n", BankIndex));
  
  Status = McaToIpmi((RAS_MCA_ERROR_INFO*)ErrorRecord, BankIndex, DimmInfo, gByoRasLogCtx.ErrorLogData, &DataSize);
  if(!EFI_ERROR(Status)){
    Status = SendErrorLogDataToBmc(gByoRasLogCtx.ErrorLogData, DataSize);
  }

  return Status;
}



EFI_STATUS
SendElogEventNbio (
  IN  UINT8        *ErrorRecord
  )
{
  EFI_STATUS             Status;
  UINTN                  DataSize;
  

  DEBUG((EFI_D_INFO, "SendElogEventNbio\n"));
  
  Status = NbioToIpmi((RAS_NBIO_ERROR_INFO*)ErrorRecord, gByoRasLogCtx.ErrorLogData, &DataSize);
  if(Status == EFI_SUCCESS){
    Status = SendErrorLogDataToBmc(gByoRasLogCtx.ErrorLogData, DataSize);
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
  UINTN                         DataSize;
  

  DEBUG((EFI_D_ERROR, "SendElogEventPcie\n"));

  Status = PcieToIpmi((GENERIC_PCIE_AER_ERR_ENTRY*)ErrorRecord, gByoRasLogCtx.ErrorLogData, &DataSize);
  if(Status == EFI_SUCCESS){
    Status = SendErrorLogDataToBmc(gByoRasLogCtx.ErrorLogData, DataSize);
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
  UINTN                         DataSize;

  DEBUG((EFI_D_INFO, "SendElogEventMemTraining(%x,%x,%x)\n", Socket, Channel, Dimm));

  Status = MemTrainingToIpmi(Socket, Channel, Dimm, gByoRasLogCtx.ErrorLogData, &DataSize);
  if(Status == EFI_SUCCESS){
    Status = SendErrorLogDataToBmc(gByoRasLogCtx.ErrorLogData, DataSize);
  }

  return EFI_SUCCESS;
}



RETURN_STATUS
EFIAPI
RasElogEventLibConstructor (
  VOID
  )
{
  EFI_PEI_HOB_POINTERS     GuidHob;
  BYO_HYGON_DIMM_MAP_INFO  *DimmMapInfo = NULL;
  PLATFORM_COMM_INFO       *PlatCommInfo;
  UINTN                    Size;
  BYO_RAS_POLICY_DATA      *RasPolicy;
  UINT32                   TimeStamp;
  UINTN                    Index;
  BYO_RAS_LOG_CTX          *Ctx = &gByoRasLogCtx;  


  DEBUG((EFI_D_INFO, "RasElogEventLibConstructor\n"));
  
  GuidHob.Raw = GetFirstGuidHob(&gHygonDimmMapInfoHobGuid);
  if(GuidHob.Raw != NULL){
    DimmMapInfo = (BYO_HYGON_DIMM_MAP_INFO*)(GuidHob.Guid+1);
  }

  PlatCommInfo = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  ASSERT(DimmMapInfo->MapDataCount <= MAX_DIMMS_SUPPORT);

  Size = sizeof(BYO_HYGON_DIMM_MAP_INFO) + sizeof(BYO_HYGON_DIMM_MAP_DATA) * (DimmMapInfo->MapDataCount - 1);
  Ctx->MapInfo = AllocatePool(Size);
  ASSERT(Ctx->MapInfo != NULL);
  CopyMem(Ctx->MapInfo, DimmMapInfo, Size);

  RasPolicy = (BYO_RAS_POLICY_DATA*)(UINTN)PlatCommInfo->RasPolicyAddr;
  if(RasPolicy != NULL && RasPolicy->Tag == BYO_RAS_POLICY_DATA_TAG && RasPolicy->Version == BYO_RAS_POLICY_DATA_VER){
    Ctx->MemTinyStormTimeIntervalSec = RasPolicy->MemTinyStormTimeIntervalSec;
    Ctx->MemTinyStormThresholdCount  = RasPolicy->MemTinyStormThresholdCount;
    Ctx->McaErrThreshSwCount         = RasPolicy->McaErrThreshSwCount;
    Ctx->McaErrThreshCount           = RasPolicy->McaErrThreshCount;
    Ctx->MemLeakybucketMinites       = RasPolicy->LeakybucketMinites;
    Ctx->MemLeakybucketOnceCount     = RasPolicy->LeakybucketOnceCount;
    Ctx->ClearMemCeThresholdEvery24H = RasPolicy->Leakybucket24HClear;    
  }

  gByoRasLogCtx.Pfeh = PcdGetBool(PcdHygonCcxCfgPFEHEnable);
  gByoRasLogCtx.MemLeakybucketMiniutesOrSeconds = FixedPcdGet8(PcdMemLeakybucketMiniutesOrSeconds);

  GetTimeStampLib(&TimeStamp);
  for(Index=0; Index<ARRAY_SIZE(Ctx->TimeStamp); Index++){
    Ctx->TimeStamp[Index] = TimeStamp;
  }

  if(Ctx->McaErrThreshSwCount > 0xFFF){
    Ctx->McaErrThresh2Count = 0xFFF - (Ctx->McaErrThreshSwCount - Ctx->McaErrThreshCount);
    Ctx->Is2phase = TRUE;
  }
  Ctx->McaErrThreshCount = 0xFFF - Ctx->McaErrThreshCount;

  DEBUG((EFI_D_INFO, "Th2:%d, 2phase:%d, Th:%d\n", Ctx->McaErrThresh2Count, Ctx->Is2phase, Ctx->McaErrThreshCount));
  DEBUG((EFI_D_INFO, "RasElogEventLibConstructor Pfeh:%d\n", Ctx->Pfeh));
 
  return EFI_SUCCESS;
}


