

#include <Uefi.h>
#include <IndustryStandard/IpmiNetFnStorage.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ByoCommLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <Protocol/ReportStatusCodeHandler.h>
#include <ByoStatusCode.h>
#include <Library/IpmiBaseLib.h>
#include <SetupVariable.h>
#include <Library/TimeStampLib.h>


EFI_STATUS
EfiSmGetTimeStamp (
  OUT UINT32 *NumOfSeconds
  );

EFI_RSC_HANDLER_PROTOCOL  *gRscHandlerProtocol = NULL;
BOOLEAN                   gIpmiSelProgCodeEn;

EFI_STATUS
EFIAPI
PrjStatusCodeListenerDxe (
  IN EFI_STATUS_CODE_TYPE   CodeType,
  IN EFI_STATUS_CODE_VALUE  Value,
  IN UINT32                 Instance,
  IN EFI_GUID               *CallerId,
  IN EFI_STATUS_CODE_DATA   *Data
  )
{
	IPMI_SEL_EVENT_RECORD_DATA	SelRecord;
  UINT8                       ResponseData[2];
  UINT8                       ResponseDataSize;
  UINT32                      TimeStamp;
  EFI_STATUS                  Status;
  UINT16                      ProgCode  = 0xFFFF;
  STATIC UINT8                ReportMap = 0;

  if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE) {
    switch(Value){
      case DXE_CPU_CACHE_INIT:
        if(!(ReportMap & BIT0)){
          ProgCode = 0xA;
          ReportMap |= BIT0;
        }
        break;

      case DXE_USB_BEGIN:
        if(!(ReportMap & BIT1)){
          ProgCode = 0x6;
          ReportMap |= BIT1;
        }
        break;

      case DXE_CON_OUT_CONNECT:
        if(!(ReportMap & BIT2)){
          ProgCode = 0x9;
          ReportMap |= BIT2;
        }
        break;

      case DXE_ATA_BEGIN:
        if(!(ReportMap & BIT3)){
          ProgCode = 0x2;
          ReportMap |= BIT3;
        }
        break;

      case DXE_READY_TO_BOOT:
        if(!(ReportMap & BIT4)){
          ProgCode = 0x13;
          ReportMap |= BIT4;
        }
        break;
    }
  }

  if(ProgCode != 0xFFFF && gIpmiSelProgCodeEn){
    
    ZeroMem(&SelRecord, sizeof(IPMI_SEL_EVENT_RECORD_DATA));
    GetTimeStampLib(&TimeStamp);
		SelRecord.RecordType   = IPMI_SEL_SYSTEM_RECORD;
		SelRecord.TimeStamp    = TimeStamp;
		SelRecord.GeneratorId  = PcdGet16(PcdIpmiSelRecordGeneratorId);
		SelRecord.EvMRevision  = IPMI_EVM_REVISION;
		SelRecord.SensorType   = 0x0F;  
		SelRecord.SensorNumber = 0x00;    
		SelRecord.EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
    SelRecord.OEMEvData1   = 0xC2;
    SelRecord.OEMEvData2   = (UINT8)ProgCode;
    SelRecord.OEMEvData3   = 0xFF;

    ResponseDataSize = sizeof(ResponseData);
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_STORAGE,
              EFI_STORAGE_ADD_SEL_ENTRY,
              (UINT8*)&SelRecord,
              sizeof(SelRecord),
              ResponseData,
              &ResponseDataSize
              );
//- DEBUG((EFI_D_INFO, "SendIpmiCommand(Post %x):%r\n", ProgCode, Status)); 
    return Status;
  }

  return EFI_UNSUPPORTED;
}


VOID
EFIAPI
ProjectRscHandlerCallBack (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  EFI_STATUS                 Status;

  Status = gBS->LocateProtocol(&gEfiRscHandlerProtocolGuid, NULL, (VOID**)&gRscHandlerProtocol);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  Status = gRscHandlerProtocol->Register(PrjStatusCodeListenerDxe, TPL_HIGH_LEVEL);
  ASSERT_EFI_ERROR (Status);  
}


VOID
EFIAPI
PrjExitBootServicesEventNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  if(gRscHandlerProtocol != NULL){
    gRscHandlerProtocol->Unregister(PrjStatusCodeListenerDxe);
  }
}


EFI_STATUS PrjStatusCodeHandleInit(CONST SETUP_DATA *SetupHob)  //j_t IpmiSelProgCodeEn
{
  EFI_EVENT   Event;
  VOID        *Registration;
  EFI_STATUS  Status;
  
  gIpmiSelProgCodeEn = SetupHob->IpmiSelProgCodeEn;
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), IpmiSelProgCodeEn :%d.\n", __LINE__, gIpmiSelProgCodeEn));

  EfiCreateProtocolNotifyEvent (
    &gEfiRscHandlerProtocolGuid,
    TPL_CALLBACK,
    ProjectRscHandlerCallBack,
    NULL,
    &Registration
    );

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  PrjExitBootServicesEventNotify,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);  

  return Status;
}


