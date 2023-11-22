

#include <Uefi.h>
#include <Guid/MtcVendor.h>
#include <Library/IoLib.h>
#include <Library/SerialPortLib.h>
#include <Protocol/VariableWrite.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/PlatformCommLib.h>
#include <Protocol/ReportRasUceProtocol.h>


EFI_STATUS
EFIAPI
ReportMemUceAddress (
  IN HYGON_REPORT_UCE_PROTOCOL  *This,
  IN UINT64                     MemAddress
  );

EFI_STATUS
EFIAPI
ReportCoreUceApicId (
  IN HYGON_REPORT_UCE_PROTOCOL  *This,
  IN UINTN                      ApicId
  );

BOOLEAN
EFIAPI
RasIsThisCoreUce (
  IN HYGON_REPORT_UCE_PROTOCOL  *This,
  IN UINTN                      ApicId
  );

EFI_STATUS
EFIAPI
RasGetUceMemList (
  IN HYGON_REPORT_UCE_PROTOCOL  *This,
  UINT32                        *MemPageCount,
  UINT32                        **MemPage
  );

EFI_STATUS
EFIAPI
ReportUceEnd (
  IN HYGON_REPORT_UCE_PROTOCOL  *This
  );

EFI_STATUS
EFIAPI
GetNewError (
  IN HYGON_REPORT_UCE_PROTOCOL      *This,
  OUT UINT32                        **MemPage,
  OUT UINT32                        *MemPageCount,
  OUT UINT64                        **ApicId,
  OUT UINTN                         *ApicIdSize
  );

typedef struct {
  UINT64   CpuCoreMap[4];
  UINT32   MemPageCount;
  UINT32   *MemPage;
} RAS_ISO_INFO;

typedef struct {
  RAS_ISO_INFO               Info;
  UINT32                     MaxPageCount;
  HYGON_REPORT_UCE_PROTOCOL  ReportUce;
  BOOLEAN                    VarWriteReady;
  BOOLEAN                    UceReportEnd;
  BOOLEAN                    SyncToVar;
  UINT8                      Csm;
  UINT32                     OldPageCount;
  UINT64                     AddCpuCoreMap[4];  
} REPORT_UCE_CTX;

REPORT_UCE_CTX gReportUceCtx = {
  {0,},
  0,
  {
    ReportMemUceAddress,
    ReportCoreUceApicId,
    RasIsThisCoreUce,
    RasGetUceMemList,
    ReportUceEnd,
    GetNewError
  },
  FALSE,
  FALSE,
  FALSE,
  0xFF,
  0,
  {0, 0}
};



EFI_STATUS ReportMemUceAddress(HYGON_REPORT_UCE_PROTOCOL *This, UINT64 Address)
{
  REPORT_UCE_CTX  *p = &gReportUceCtx;
  UINTN           Index;

  DEBUG((EFI_D_INFO, "ReportMemUceAddress(%lx)\n", Address));

  if(p->Csm && Address < SIZE_1MB){
    DEBUG((EFI_D_INFO, "ignore mem %lx at CsmOn\n", Address));
    return EFI_UNSUPPORTED;
  }

  for(Index=0; Index<p->Info.MemPageCount; Index++){
    if(LShiftU64(p->Info.MemPage[Index], 12) == Address){
      DEBUG((EFI_D_INFO, "already present\n"));
      return EFI_ALREADY_STARTED;
    }
  }

  if(p->Info.MemPageCount >= p->MaxPageCount){
    p->Info.MemPage = ReallocatePool(
                        p->MaxPageCount * sizeof(UINT32), 
                        (p->MaxPageCount+1024) * sizeof(UINT32), 
                        p->Info.MemPage
                        );
    p->MaxPageCount += 1024;
  }

  p->Info.MemPage[p->Info.MemPageCount++] = (UINT32)RShiftU64(Address, 12);

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
ReportCoreUceApicId (
  IN HYGON_REPORT_UCE_PROTOCOL  *This,
  IN UINTN                      ApicId
  )
{
  REPORT_UCE_CTX  *p = &gReportUceCtx;
  UINT64          Data64;

  DEBUG((EFI_D_INFO, "ReportCoreUceApicId(%d)\n", ApicId));

  if(ApicId >= 256){
    return EFI_INVALID_PARAMETER;
  }
  if(ApicId == 0){
    DEBUG((EFI_D_INFO, "BSP cannot reserved.\n"));
    return EFI_UNSUPPORTED;
  }

  if(ApicId >= 64*3){
    Data64 = LShiftU64(1, ApicId - 64*3);
    p->Info.CpuCoreMap[3] |= Data64;
    p->AddCpuCoreMap[3]   |= Data64;
  } else if(ApicId >= 64*2){
    Data64 = LShiftU64(1, ApicId - 64*2);
    p->Info.CpuCoreMap[2] |= Data64;
    p->AddCpuCoreMap[2]   |= Data64;
  } else if(ApicId >= 64){
    Data64 = LShiftU64(1, ApicId - 64);
    p->Info.CpuCoreMap[1] |= Data64;
    p->AddCpuCoreMap[1]   |= Data64;
  } else {
    Data64 = LShiftU64(1, ApicId);
    p->Info.CpuCoreMap[0] |= Data64;
    p->AddCpuCoreMap[0]   |= Data64;
  }

  return EFI_SUCCESS;  
}


BOOLEAN
EFIAPI
RasIsThisCoreUce (
  IN HYGON_REPORT_UCE_PROTOCOL  *This,
  IN UINTN                      ApicId
  )
{
  REPORT_UCE_CTX  *p = &gReportUceCtx;

  DEBUG((EFI_D_INFO, "RasIsThisCoreUce(%d)\n", ApicId));
  
  if(ApicId >= 256){
    return FALSE;
  }

  if(ApicId >= 64*3){
    return !!(p->Info.CpuCoreMap[3] & LShiftU64(1, ApicId - 64*3));
  } else if(ApicId >= 64*2){
    return !!(p->Info.CpuCoreMap[2] & LShiftU64(1, ApicId - 64*2));
  } else if(ApicId >= 64){
    return !!(p->Info.CpuCoreMap[1] & LShiftU64(1, ApicId - 64));
  } else {
    return !!(p->Info.CpuCoreMap[0] & LShiftU64(1, ApicId));
  }
}

EFI_STATUS
EFIAPI
RasGetUceMemList (
  IN HYGON_REPORT_UCE_PROTOCOL  *This,
  UINT32                        *MemPageCount,
  UINT32                        **MemPage
  )
{
  REPORT_UCE_CTX  *p = &gReportUceCtx;

  DEBUG((EFI_D_INFO, "RasGetUceMemList\n"));

  *MemPageCount = p->Info.MemPageCount;
  *MemPage      = p->Info.MemPage;

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
GetNewError (
  IN HYGON_REPORT_UCE_PROTOCOL      *This,
  OUT UINT32                        **MemPage,
  OUT UINT32                        *MemPageCount,
  OUT UINT64                        **ApicId,
  OUT UINTN                         *ApicIdSize
  )
{
  REPORT_UCE_CTX  *p = &gReportUceCtx;

  DEBUG((EFI_D_INFO, "GetNewError\n"));

  *ApicId     = p->AddCpuCoreMap;
  *ApicIdSize = sizeof(p->AddCpuCoreMap);

  if(p->Info.MemPageCount > p->OldPageCount){
    *MemPageCount = p->Info.MemPageCount - p->OldPageCount;
    *MemPage      = &p->Info.MemPage[p->OldPageCount];
  } else {
    *MemPageCount = 0;
  }

  return EFI_SUCCESS;
}


VOID SyncUceInfoToVariable()
{
  REPORT_UCE_CTX             *p = &gReportUceCtx;
  UINTN                      Size;
  HYGON_REPORT_RAS_UCE_INFO  *VarData;
  EFI_STATUS                 Status;


  DEBUG((EFI_D_INFO, "SyncUceInfoToVariable\n"));

  if(p->SyncToVar || !p->UceReportEnd || !p->VarWriteReady){
    return;
  }

  p->SyncToVar = TRUE;

  if(IsZeroBuffer(p->Info.CpuCoreMap, sizeof(p->Info.CpuCoreMap)) && p->Info.MemPageCount == 0){
    DEBUG((EFI_D_INFO, "uceinfo no data.\n"));
    Status = gRT->SetVariable (
                    HYGON_REPORT_RAS_UCE_VARIABLE_NAME, 
                    &gHygonReportRasUceVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    0,
                    NULL
                    );    
    DEBUG((EFI_D_INFO, "ClearUceIsoVar:%r\n", Status));
    return;
  }

  Size = sizeof(HYGON_REPORT_RAS_UCE_INFO) - sizeof(UINT32) + p->Info.MemPageCount * sizeof(UINT32);
  VarData = (HYGON_REPORT_RAS_UCE_INFO*)AllocatePool(Size);
  ASSERT(VarData != NULL);

  CopyMem(VarData->CpuCoreMap, p->Info.CpuCoreMap, sizeof(p->Info.CpuCoreMap));
  VarData->PageCount = p->Info.MemPageCount;
  CopyMem(VarData->MemPage, p->Info.MemPage, VarData->PageCount * sizeof(UINT32));

  Status = gRT->SetVariable (
                  HYGON_REPORT_RAS_UCE_VARIABLE_NAME, 
                  &gHygonReportRasUceVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  Size,
                  VarData
                  );
  FreePool(VarData);
}




EFI_STATUS
EFIAPI
ReportUceEnd (
  IN HYGON_REPORT_UCE_PROTOCOL  *This
  )
{
  DEBUG((EFI_D_INFO, "ReportUceEnd\n"));
  gReportUceCtx.UceReportEnd = TRUE;
  SyncUceInfoToVariable();
  return EFI_SUCCESS;
}




VOID
EFIAPI
PlatEarlyVariableWriteArchCallBack (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  VOID        *Interface;
  EFI_STATUS  Status;


  Status = gBS->LocateProtocol(&gEfiVariableWriteArchProtocolGuid, NULL, (VOID**)&Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);
  gReportUceCtx.VarWriteReady = TRUE;  
  SyncUceInfoToVariable();
}


VOID ReportRasUceMemoryAbove4GBReportedHook()
{
  EFI_STATUS                       Status;
  HYGON_REPORT_UCE_PROTOCOL        *ReportUce;
  UINT32                           MemPageCount;
  UINT32                           *MemPage;
  EFI_PHYSICAL_ADDRESS             Address;
  UINT64                           *ApicId;
  UINTN                            ApicIdSize;
  UINTN                            Index;
  

  DEBUG((EFI_D_INFO, "ReportRasUceAbove4GB\n"));

  Status = gBS->LocateProtocol (
                       &gHygonReportRasUceProtocolGuid,
                       NULL,
                       (VOID**)&ReportUce
                       );
  if (!EFI_ERROR (Status)) {
    Status = ReportUce->GetUceMemList(ReportUce, &MemPageCount, &MemPage);
    if(!EFI_ERROR(Status)){
      for(Index=0;Index<MemPageCount;Index++){
        Address = LShiftU64(MemPage[Index], 12);
        if(Address >= SIZE_4GB){
          Status = gBS->AllocatePages(AllocateAddress, EfiReservedMemoryType, 1, &Address);
          DEBUG((EFI_D_INFO, "AllocatePages(%lx):%r\n", Address, Status));
        }
      }
    }

    Status = ReportUce->GetNewErr(ReportUce, &MemPage, &MemPageCount, &ApicId, &ApicIdSize);
    if(!EFI_ERROR(Status)){
      for(Index=0;Index<MemPageCount;Index++){
        Address = LShiftU64(MemPage[Index], 12);
        if(PcdGet8(PcdLegacyBiosSupport) && Address < SIZE_1MB){
		      continue;
		    }
        if(Address >= SIZE_4GB){
          continue;
        }	
        Status = gBS->AllocatePages(AllocateAddress, EfiReservedMemoryType, 1, &Address);
        DEBUG((EFI_D_INFO, "AllocatePages(%lx):%r\n", Address, Status));
      }
    }    
  }
}



EFI_STATUS
EFIAPI
ReportRasUceDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_PEI_HOB_POINTERS            GuidHob;
  HYGON_REPORT_RAS_UCE_INFO       *HobInfo;
  EFI_STATUS                      Status;
  REPORT_UCE_CTX                  *p = &gReportUceCtx;  
  VOID                            *Registration;
  PLATFORM_COMM_INFO              *PlatformInfo;
  

  DEBUG((EFI_D_INFO, "ReportRasUceDxeEntry\n"));

  PlatformInfo = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  GuidHob.Raw = GetFirstGuidHob(&gHygonReportRasUceVariableGuid);
  if(GuidHob.Raw == NULL){
    return EFI_SUCCESS;
  }
  
  HobInfo = (HYGON_REPORT_RAS_UCE_INFO*)(GuidHob.Guid+1);
  if(HobInfo->PageCount){
    p->Info.MemPage = AllocatePool(sizeof(UINT32) * (HobInfo->PageCount+1024));
    ASSERT(p->Info.MemPage != NULL);
    CopyMem(p->Info.MemPage, HobInfo->MemPage, sizeof(UINT32) * HobInfo->PageCount);
    p->Info.MemPageCount = HobInfo->PageCount;
    p->MaxPageCount      = HobInfo->PageCount + 1024;
    p->OldPageCount      = p->Info.MemPageCount;
  }
  CopyMem(p->Info.CpuCoreMap, HobInfo->CpuCoreMap, sizeof(HobInfo->CpuCoreMap));

  p->Csm = PlatformInfo->Csm;

  Status = gBS->InstallProtocolInterface (
                  &gImageHandle,
                  &gHygonReportRasUceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gReportUceCtx.ReportUce
                  );
  ASSERT(!EFI_ERROR(Status));

  EfiCreateProtocolNotifyEvent (
    &gEfiVariableWriteArchProtocolGuid,
    TPL_CALLBACK,
    PlatEarlyVariableWriteArchCallBack,
    NULL,
    &Registration
    );

  Status = gBS->InstallMultipleProtocolInterfaces(
                  &ImageHandle,
                  &gByoMemoryAbove4GBReportedHookGuid, ReportRasUceMemoryAbove4GBReportedHook,
                  NULL
                  );

  return Status;
}

