

#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/BaseMemoryLib.h>
#include <HygonCpu.h>
#include <Library/PeiServicesLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <ByoStatusCode.h>
#include <Library/LocalApicLib.h>


extern EFI_GUID gEfiCpuInfoHobGuid;


EFI_STATUS CpuInitAfterMemInstalled (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi
);

static EFI_PEI_NOTIFY_DESCRIPTOR CpuMemInstalledNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiMemoryDiscoveredPpiGuid,
    CpuInitAfterMemInstalled
  },
};



CPUINFO_HOB_DATA *
CreateCpuHobWithDefaults(
    IN UINT32            MaxNumCpus
  )
{
  CPUINFO_HOB_DATA  *Cpuinfo;
  UINTN             DataSize;


  DEBUG((EFI_D_INFO, "MaxNumCpus:%d\n", MaxNumCpus));  

  DataSize = sizeof(CPUINFO_HOB_DATA) + (MaxNumCpus - 1) * sizeof(CPUINFO);
  Cpuinfo = (CPUINFO_HOB_DATA*)BuildGuidHob(&gEfiCpuInfoHobGuid, DataSize);
  ASSERT(Cpuinfo != NULL);
  ZeroMem(Cpuinfo, DataSize);

  Cpuinfo->CpuCount = MaxNumCpus;
  Cpuinfo->BspNo = 0;
  Cpuinfo->CacheLineSize = 64;
  Cpuinfo->TsegAddress = 0;
  Cpuinfo->TsegSize = TSEG_SIZE;

  return Cpuinfo;
}




EFI_STATUS CpuInitAfterMemInstalled (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{
  REPORT_STATUS_CODE (EFI_PROGRESS_CODE, PEI_CAR_CPU_INIT);

  CreateCpuHobWithDefaults(FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber));
  ProgramVirtualWireMode();
  AsmWriteMsr64 (MSR_SYS_CFG, AsmReadMsr64 (MSR_SYS_CFG) & (~(UINT64)(1 << MTRRFixDRAMModEnBit)) );  
  return EFI_SUCCESS;
}


/**
    This function is called after the permanent memory is installed
    in the system.  This function does the initializes CPU in the memory
    present environment.

    @param FileHandle Handle of the file being invoked
    @param PeiServices Pointer to the PEI services table

    @retval EFI_SUCCESS always success

**/
EFI_STATUS CpuPeiEntry(
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices)
{

  EFI_STATUS      Status;
    Status = (*PeiServices)->NotifyPpi( PeiServices, CpuMemInstalledNotifyList );
    ASSERT_EFI_ERROR ( Status );

  return Status;
}


