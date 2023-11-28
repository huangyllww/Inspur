
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <PlatformDefinition.h>
#include <Library/BaseMemoryLib.h>
#include <Token.h>
#include <AmdCpu.h>
#include <Library/PeiServicesLib.h>


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





VOID
InitializeCpu()
{
  UINT32		    Value;

  // Read SVR data from Local APIC
  Value = *(UINT32*)(UINTN)(APIC_BASE + APIC_SPURIOUS_VECTOR_REGISTER);
  //Clear SVR, Use Vector 0Fh
  Value &= 0x0F;
  Value |= 1 << 8;	//Enable Apic
  *(UINT32*)(UINTN)(APIC_BASE + APIC_SPURIOUS_VECTOR_REGISTER) = Value;

  //Program LINT0 vector as ExtInt
  Value = *(UINT32*)(UINTN)(APIC_BASE + APIC_LVT_LINT0_REGISTER);
  Value &= 0xFFFE58FF;
  Value |= 0x700;
  *(UINT32*)(UINTN)(APIC_BASE + APIC_LVT_LINT0_REGISTER) = Value;

  //Program the LINT1 vector entry as NMI
  Value = *(UINT32*)(UINTN)(APIC_BASE + APIC_LVT_LINT1_REGISTER);
  Value &= 0xFFFE58FF;
  Value |= 0x400;

  *(UINT32*)(UINTN)(APIC_BASE + APIC_LVT_LINT1_REGISTER) = Value;

  AsmWriteMsr64 (MSR_SYS_CFG, AsmReadMsr64 (MSR_SYS_CFG) & (~(UINT64)(1 << MTRRFixDRAMModEnBit)) );

	return;
}






CPUINFO_HOB_DATA *
CreateCpuHobWithDefaults(
    IN UINT8            MaxNumCpus
  )
{
  CPUINFO_HOB_DATA	*Cpuinfo;
  UINT8		          i;
  UINTN             DataSize;

  DataSize = sizeof(CPUINFO_HOB_DATA) + (MaxNumCpus - 1) * sizeof(CPUINFO);
  Cpuinfo = (CPUINFO_HOB_DATA*)BuildGuidHob(&gEfiCpuInfoHobGuid, DataSize);
  ASSERT(Cpuinfo != NULL);
  ZeroMem(Cpuinfo, DataSize);

  Cpuinfo->CpuCount = MaxNumCpus;
  Cpuinfo->BspNo = 0;
  Cpuinfo->CacheLineSize = 64;
  Cpuinfo->TsegAddress = 0;
  Cpuinfo->TsegSize = TSEG_SIZE;

  for (i = 0; i < MaxNumCpus; i++) {
    Cpuinfo->Cpuinfo[i].Valid	= FALSE;
    Cpuinfo->Cpuinfo[i].ApicId	= i;
  }
  
  return Cpuinfo;
}




EFI_STATUS CpuInitAfterMemInstalled (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{
  CreateCpuHobWithDefaults(NCPU);

  InitializeCpu();

  return  EFI_SUCCESS;
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

	EFI_STATUS			Status;
    Status = (*PeiServices)->NotifyPpi( PeiServices, CpuMemInstalledNotifyList );
    ASSERT_EFI_ERROR ( Status );
    
	return Status;
}


