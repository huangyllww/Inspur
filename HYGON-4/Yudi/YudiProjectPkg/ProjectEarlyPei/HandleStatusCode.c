

#include <PiPei.h>
#include <IndustryStandard/IpmiNetFnStorage.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Ppi/ReportStatusCodeHandler.h>
#include <ByoStatusCode.h>
#include <Ppi/IpmiTransportPpi.h>
#include <Library/HobLib.h>
#include <Library/TimeStampLib.h>


typedef struct {
  UINT8                         Count;
  IPMI_SEL_EVENT_RECORD_DATA    SelRecord[4];
} IPMI_SEL_PC_INFO;

STATIC EFI_GUID gIpmiSelPcInfoHobGuid = { 0x136b544a, 0x1ff7, 0x4a3d, { 0xbc, 0x78, 0x14, 0x91, 0x56, 0xaa, 0x44, 0x5d } };



EFI_STATUS
EfiSmGetTimeStamp (
  OUT UINT32 *NumOfSeconds
  );

EFI_STATUS
EFIAPI
PrjStatusCodeListenerPei (
  IN CONST  EFI_PEI_SERVICES       **PeiServices,
  IN        EFI_STATUS_CODE_TYPE   CodeType,
  IN        EFI_STATUS_CODE_VALUE  Value,
  IN        UINT32                 Instance,
  IN CONST  EFI_GUID               *CallerId,
  IN CONST  EFI_STATUS_CODE_DATA   *Data
  )
{
  UINT16                      ProgCode  = 0xFFFF;
  EFI_PEI_HOB_POINTERS        GuidHob;
  IPMI_SEL_PC_INFO            *SelInfo;
  IPMI_SEL_EVENT_RECORD_DATA	*SelRecord;


  if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE && Value == PEI_CAR_CPU_INIT){
    ProgCode = 0x19;
  }

  if(ProgCode != 0xFFFF){

    GuidHob.Raw = GetFirstGuidHob(&gIpmiSelPcInfoHobGuid);
    ASSERT(GuidHob.Raw != NULL);
    SelInfo = (IPMI_SEL_PC_INFO*)(GuidHob.Guid+1);

    if(SelInfo->Count >= ARRAY_SIZE(SelInfo->SelRecord)){
      return EFI_OUT_OF_RESOURCES;
    }

    SelRecord = &SelInfo->SelRecord[SelInfo->Count];
    GetTimeStampLib(&SelRecord->TimeStamp);
		SelRecord->RecordType   = IPMI_SEL_SYSTEM_RECORD;
		SelRecord->GeneratorId  = PcdGet16(PcdIpmiSelRecordGeneratorId);
		SelRecord->EvMRevision  = IPMI_EVM_REVISION;
		SelRecord->SensorType   = 0x0F;  
		SelRecord->SensorNumber = 0x00;    
		SelRecord->EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
    SelRecord->OEMEvData1   = 0xC2;
    SelRecord->OEMEvData2   = (UINT8)ProgCode;
    SelRecord->OEMEvData3   = 0xFF;
    
    SelInfo->Count++;

    return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;  
}


EFI_STATUS PrjPeiSendBmcProgressCode()
{
  PEI_IPMI_TRANSPORT_PPI      *IpmiTransport;
  UINT8                       ResponseData[2];
  UINT8                       ResponseDataSize;
  EFI_STATUS                  Status;
  EFI_PEI_HOB_POINTERS        GuidHob;
  IPMI_SEL_PC_INFO            *SelInfo;
  UINTN                       Index;

  
  Status = PeiServicesLocatePpi (
              &gPeiIpmiTransportPpiGuid,
              0,
              NULL,
              (VOID **)&IpmiTransport
              );
  if (EFI_ERROR(Status)) {
    return EFI_NOT_AVAILABLE_YET;
  }

  GuidHob.Raw = GetFirstGuidHob(&gIpmiSelPcInfoHobGuid);
  ASSERT(GuidHob.Raw != NULL);
  SelInfo = (IPMI_SEL_PC_INFO*)(GuidHob.Guid+1);

  for(Index=0;Index<SelInfo->Count;Index++){
    ResponseDataSize = sizeof(ResponseData);
    Status = IpmiTransport->SendIpmiCommand (
                               IpmiTransport,
                               EFI_SM_NETFN_STORAGE,
                               0,
                               EFI_STORAGE_ADD_SEL_ENTRY,
                               (UINT8*)&SelInfo->SelRecord[Index],
                               sizeof(IPMI_SEL_EVENT_RECORD_DATA),
                               ResponseData,
                               &ResponseDataSize
                               );
    DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), Post %x :%r.\n", __LINE__, SelInfo->SelRecord[Index].OEMEvData2, Status));
  }

  return Status;
}




EFI_STATUS
EFIAPI
PrjPeiRscHandlerCallBack (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS               Status;
  EFI_PEI_RSC_HANDLER_PPI  *RscHandler;
  IPMI_SEL_PC_INFO         *SelInfo;

  Status = PeiServicesLocatePpi (
             &gEfiPeiRscHandlerPpiGuid,
             0,
             NULL,
             (VOID **)&RscHandler
             );
  ASSERT_EFI_ERROR (Status);

  SelInfo = BuildGuidHob(&gIpmiSelPcInfoHobGuid, sizeof(IPMI_SEL_PC_INFO));
  ASSERT(SelInfo != NULL);
  ZeroMem(SelInfo, sizeof(IPMI_SEL_PC_INFO));

  Status = RscHandler->Register(PrjStatusCodeListenerPei);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

