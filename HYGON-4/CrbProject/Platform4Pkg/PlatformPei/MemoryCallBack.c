
#include "PlatformPei.h"
#include <Library/PerformanceLib.h>
#include <SetupVariable.h>
#include <HygonCpu.h>

STATIC EFI_MEMORY_TYPE_INFORMATION gDefaultMemoryTypeInformation[] = {
  { EfiACPIReclaimMemory,   0x40   },     // ASL
  { EfiACPIMemoryNVS,       0x100  },     // ACPI NVS (including S3 related)
  { EfiReservedMemoryType,  0x100  },     // BIOS Reserved (including S3 related)
  { EfiRuntimeServicesData, 0x50   },     // Runtime Service Data
  { EfiRuntimeServicesCode, 0x50   },     // Runtime Service Code
  { EfiMaxMemoryType,       0      }
};


#define SYS_MEM_ATTRIB \
      EFI_RESOURCE_ATTRIBUTE_PRESENT | \
      EFI_RESOURCE_ATTRIBUTE_INITIALIZED | \
      EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE | \
      EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE | \
      EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE | \
      EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE | \
      EFI_RESOURCE_ATTRIBUTE_TESTED

#define SYS_MMIO_ATTRIB \
          EFI_RESOURCE_ATTRIBUTE_PRESENT | \
          EFI_RESOURCE_ATTRIBUTE_INITIALIZED | \
          EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE | \
          EFI_RESOURCE_ATTRIBUTE_TESTED

/**
   Validate variable data for the MemoryTypeInformation.

   @param MemoryData       Variable data.
   @param MemoryDataSize   Variable data length.

   @return TRUE            The variable data is valid.
   @return FALSE           The variable data is invalid.

**/
STATIC
BOOLEAN
ValidateMemoryTypeInfoVariable (
  IN EFI_MEMORY_TYPE_INFORMATION      *MemoryData,
  IN UINTN                            MemoryDataSize
  )
{
  UINTN                       Count;
  UINTN                       Index;

  // Check the input parameter.
  if (MemoryData == NULL) {
    return FALSE;
  }

  // Get Count
  Count = MemoryDataSize / sizeof (*MemoryData);

  // Check Size
  if (Count * sizeof(*MemoryData) != MemoryDataSize) {
    return FALSE;
  }

  // Check last entry type filed.
  if (MemoryData[Count - 1].Type != EfiMaxMemoryType) {
    return FALSE;
  }

  // Check the type filed.
  for (Index = 0; Index < Count - 1; Index++) {
    if (MemoryData[Index].Type >= EfiMaxMemoryType) {
      return FALSE;
    }
    if(MemoryData[Index].Type == EfiBootServicesData || 
       MemoryData[Index].Type == EfiBootServicesCode){
       DEBUG((EFI_D_ERROR, "ValidateMemoryTypeInfoVariable meet BS type.\n"));
      return FALSE;
    }
  }

  return TRUE;
}


typedef struct {
  CHAR8    FvName[8];
  UINT32   FvBase;
  BOOLEAN  NeedHob;
} FV_INFO_LIST;

// core will only auto adds Fv hob for FV in FV, so normal FV should report hob manually.
// there are no modules in top "dir" of FvMain2, so here it does not need to report Fv Hob.
FV_INFO_LIST gFvList[] = {
  {{"FvMain"},  _PCD_VALUE_PcdFlashFvMainBase,    TRUE},
  {{"FvMain2"}, _PCD_VALUE_PcdFlashFvMain2Base,   FALSE},
  {{"SetupDb"}, _PCD_VALUE_PcdSetupItemsFvBase,   TRUE},
};


VOID
ReportResourceForDxe (
  IN  EFI_PEI_SERVICES      **PeiServices,
  IN  EFI_BOOT_MODE         BootMode,
  IN  SETUP_DATA            *SetupData
  )
{
  UINTN                            DataSize;
  UINT32                           FvAddr;
  UINT32                           FvSize;
  VOID                             *FvData;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *Var2Ppi;
  EFI_MEMORY_TYPE_INFORMATION      MemoryData[EfiMaxMemoryType + 1];
  EFI_STATUS                       Status;
  UINTN                            Index;
  UINTN                            Count;
  EFI_FIRMWARE_VOLUME_HEADER       *FvHdr;


  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&Var2Ppi
             );
  ASSERT_EFI_ERROR(Status);



// Report FV
  if(BootMode != BOOT_IN_RECOVERY_MODE){

    PERF_START (NULL, "report", "FV", 0);

    Count = sizeof(gFvList)/sizeof(gFvList[0]);
    for(Index=0;Index<Count;Index++){
      FvAddr = gFvList[Index].FvBase;
      if(FvAddr == 0){
        continue;
      }
      FvHdr = (EFI_FIRMWARE_VOLUME_HEADER*)(UINTN)FvAddr;
      if(FvHdr->Signature != EFI_FVH_SIGNATURE){
        continue;
      }
      FvSize = (UINT32)FvHdr->FvLength;

// TPM/TCM will Measure FV Data at the callback of FvInfoPpi
// So here we should copy it to memory before, then DxeCore will
// not read it again to save boot time.
      FvData = AllocatePages(EFI_SIZE_TO_PAGES(FvSize));
      ASSERT(FvData!=NULL);

      PERF_START (NULL, gFvList[Index].FvName, "FV", 0);
      CopyMem(FvData, (VOID*)FvHdr, FvSize);
      PERF_END(NULL, gFvList[Index].FvName, "FV", 0);

      DEBUG((EFI_D_INFO, "FvMem(%X,%X)\n", FvData, FvSize));

      if(gFvList[Index].NeedHob){
        BuildFvHob((UINTN)FvData, FvSize);
      }

      PeiServicesInstallFvInfoPpi(
        NULL,
        FvData,
        FvSize,
        NULL,
        NULL
        );
    }

    PERF_END(NULL, "report", "FV", 0);
  }

  FvSize = PcdGet32(PcdFlashNvLogoSize);
  if(FvSize){
    FvData = AllocatePages(EFI_SIZE_TO_PAGES(FvSize));
    ASSERT(FvData!=NULL);
    FvHdr = (EFI_FIRMWARE_VOLUME_HEADER*)(UINTN)PcdGet32(PcdFlashNvLogoBase);
    CopyMem(FvData, (VOID*)FvHdr, FvSize);
    PcdSet32S(PcdLogoDataAddress, (UINT32)(UINTN)FvData);
    PcdSet32S(PcdLogoDataSize, FvSize);
  }

  if(PcdGet8(PcdIsPlatformCmosBad)){
    Status = EFI_NOT_FOUND;
  } else {
    DataSize = sizeof (MemoryData);
    Status = Var2Ppi->GetVariable (
                        Var2Ppi,
                        EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
                        &gEfiMemoryTypeInformationGuid,
                        NULL,
                        &DataSize,
                        &MemoryData
                        );
    DEBUG((EFI_D_INFO, "GMTIV:%r\n", Status));
  }
  if (EFI_ERROR (Status) || !ValidateMemoryTypeInfoVariable(MemoryData, DataSize)) {
    BuildGuidDataHob (
      &gEfiMemoryTypeInformationGuid,
      gDefaultMemoryTypeInformation,
      sizeof(gDefaultMemoryTypeInformation)
      );
  }
}







EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                       Status;
  EFI_BOOT_MODE                    BootMode;
  SETUP_DATA                       *SetupData;
  VOID                             *AcpiVar;
  EFI_SMRAM_DESCRIPTOR             *SmramDesc;
  UINT64                           RegionState;
  PLATFORM_COMM_INFO               *PlatCommInfo;
  UINT64                           Data64;
    

  DEBUG((EFI_D_INFO, "MemCallBack\n"));

  Status = PeiServicesGetBootMode(&BootMode);
  ASSERT_EFI_ERROR(Status);

  SetupData = (SETUP_DATA*)GetSetupDataHobData();

  PlatCommInfo = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  DEBUG((EFI_D_INFO, "SMM_ADDR_HL(%lX) SMM_MASK_HL(%lX) HYGON_MSR_HWCR(%lX)\n", \
    AsmReadMsr64(HYGON_MSR_SMM_ADDR_HL), AsmReadMsr64(HYGON_MSR_SMM_MASK_HL), AsmReadMsr64(HYGON_MSR_HWCR)));

  Data64 = AsmReadMsr64 (HYGON_MSR_SMM_MASK_HL);
  Data64 &= ~((UINT64)3);
  AsmWriteMsr64 (HYGON_MSR_SMM_MASK_HL, Data64);

  RegionState = EFI_SMRAM_OPEN;
  if(AsmReadMsr64(HYGON_MSR_SMM_MASK_HL) & 3){
    RegionState = EFI_SMRAM_CLOSED;
  }
  if(AsmReadMsr64(HYGON_MSR_HWCR) & BIT0){
    RegionState |= EFI_SMRAM_LOCKED;
  }

  DEBUG((EFI_D_INFO, "RegionState:%X\n", RegionState));

  SmramDesc = (EFI_SMRAM_DESCRIPTOR*)BuildGuidHob (
                                       &gSmramDescTableGuid,
                                       sizeof(EFI_SMRAM_DESCRIPTOR) * 2
                                       );
  ASSERT(SmramDesc != NULL);
  SmramDesc[0].CpuStart = PlatCommInfo->TsegBase;
  SmramDesc[0].PhysicalStart = SmramDesc[0].CpuStart;
  SmramDesc[0].PhysicalSize = EFI_PAGE_SIZE;
  SmramDesc[0].RegionState = RegionState | EFI_ALLOCATED;

  SmramDesc[1].CpuStart = PlatCommInfo->TsegBase + EFI_PAGE_SIZE;
  SmramDesc[1].PhysicalStart = SmramDesc[1].CpuStart;
  SmramDesc[1].PhysicalSize = PlatCommInfo->TsegSize - EFI_PAGE_SIZE;
  SmramDesc[1].RegionState = RegionState;

  AcpiVar = BuildGuidHob (
              &gEfiAcpiVariableGuid,
              sizeof(EFI_SMRAM_DESCRIPTOR)
              );
  CopyMem(AcpiVar, SmramDesc, sizeof(EFI_SMRAM_DESCRIPTOR));

  if(BootMode == BOOT_ON_S3_RESUME){
    Status = SmmAccessPpiInstall(PeiServices);
    ASSERT_EFI_ERROR(Status);
    Status = SmmControlPpiInstall(PeiServices);
    ASSERT_EFI_ERROR(Status);

  } else {
    ReportResourceForDxe(PeiServices, BootMode, SetupData);
    if(BootMode == BOOT_IN_RECOVERY_MODE){
      PeimInitializeRecovery(PeiServices);
    } else if(BootMode == BOOT_ON_FLASH_UPDATE){
    }
  }
  return Status;
}


