

#include <PiPei.h>
#include <Pi/PiBootMode.h>      // HobLib.h +
#include <Pi/PiHob.h>           // HobLib.h +
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PlatformCommLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <SetupVariable.h>
#include <Protocol/ReportRasUceProtocol.h>



EFI_STATUS
EFIAPI
ReportRasUceSetupDataReadyCallBack (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  CONST SETUP_DATA          *SetupHob;
  EFI_STATUS                 Status;
  UINTN                      Size;
  HYGON_REPORT_RAS_UCE_INFO  *Info;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *Var2Ppi;
  PLATFORM_COMM_INFO               *PlatCommInfo;
  

  DEBUG((EFI_D_INFO, "ReportRasUceSetupDataReadyCallBack\n"));

  SetupHob = (CONST SETUP_DATA*)GetSetupDataHobData();

  if(SetupHob->UceResetIso == 0){
    DEBUG((EFI_D_INFO, "UceResetIso = 0\n"));
    return EFI_SUCCESS;
  }

  PlatCommInfo = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID**)&Var2Ppi
             );
  ASSERT_EFI_ERROR(Status);

  Size   = 0;
  Info   = NULL;
  Status = Var2Ppi->GetVariable (
                      Var2Ppi,
                      HYGON_REPORT_RAS_UCE_VARIABLE_NAME,
                      &gHygonReportRasUceVariableGuid,
                      NULL,
                      &Size,
                      Info
                      );
  DEBUG((EFI_D_INFO, "GetVar:%r, Size:%d\n", Status, Size));
  if(Status == EFI_BUFFER_TOO_SMALL){
    Info = (HYGON_REPORT_RAS_UCE_INFO*)BuildGuidHob(&gHygonReportRasUceVariableGuid, Size);
    ASSERT(Info != NULL);
    if(SetupHob->ClearUceIsoData){
      ZeroMem(Info, Size);
    } else {
      Status = Var2Ppi->GetVariable (
                          Var2Ppi,
                          HYGON_REPORT_RAS_UCE_VARIABLE_NAME,
                          &gHygonReportRasUceVariableGuid,
                          NULL,
                          &Size,
                          Info
                          );
      DEBUG((EFI_D_INFO, "GetVar:%r, Size:%d\n", Status, Size));
      PlatCommInfo->PeiNeedMinMem = 1;
    }
  } else {
    Size = sizeof(HYGON_REPORT_RAS_UCE_INFO);
    Info = (HYGON_REPORT_RAS_UCE_INFO*)BuildGuidHob(&gHygonReportRasUceVariableGuid, Size);
    ASSERT(Info != NULL);
    ZeroMem(Info, Size);
  }

  return Status;  
}



EFI_STATUS
EFIAPI
ReportRasUceInstallPeiMemoryCallBack (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_PEI_HOB_POINTERS            GuidHob;
  HYGON_REPORT_RAS_UCE_INFO       *HobInfo;
  UINTN                           Index;
  EFI_PHYSICAL_ADDRESS            Address;
  PLATFORM_COMM_INFO              *Info;


  Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  GuidHob.Raw = GetFirstGuidHob(&gHygonReportRasUceVariableGuid);
  if(GuidHob.Raw != NULL){
    HobInfo = (HYGON_REPORT_RAS_UCE_INFO*)(GuidHob.Guid+1);
    for(Index=0;Index<HobInfo->PageCount;Index++){
      Address = LShiftU64(HobInfo->MemPage[Index], 12);
      if(Info->Csm && Address < SIZE_1MB){
        continue;
      }
      if(Address >= SIZE_4GB){
        continue;
      }
      DEBUG((EFI_D_INFO, "ReservePage(%lx)\n", Address));
      BuildMemoryAllocationHob(Address, SIZE_4KB, EfiReservedMemoryType);
    }
  }

  return EFI_SUCCESS;
}




STATIC EFI_PEI_NOTIFY_DESCRIPTOR  gReportRasUcePeiPpiNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gPeiInstallPeiMemoryPpiGuid,
    ReportRasUceInstallPeiMemoryCallBack
  },   
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gByoSetupDataReadyPpiGuid,
    ReportRasUceSetupDataReadyCallBack
  },
};



EFI_STATUS
EFIAPI
ReportRasUcePeiEntry (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = PeiServicesNotifyPpi(&gReportRasUcePeiPpiNotifyList[0]);
  ASSERT_EFI_ERROR(Status);

  return Status;
}


  
