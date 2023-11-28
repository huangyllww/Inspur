
#include "PlatformPei.h"
#include <Library/PerformanceLib.h>
#include <SetupVariable.h>



STATIC EFI_MEMORY_TYPE_INFORMATION gDefaultMemoryTypeInformation[] = {
  { EfiACPIReclaimMemory,   0x40   },     // ASL
  { EfiACPIMemoryNVS,       0x100  },     // ACPI NVS (including S3 related)
  { EfiReservedMemoryType,  0x100  },     // BIOS Reserved (including S3 related)
  { EfiRuntimeServicesData, 0x50   },     // Runtime Service Data
  { EfiRuntimeServicesCode, 0x50   },     // Runtime Service Code
  { EfiBootServicesData,    0x100  },     // Boot Service Data
  { EfiBootServicesCode,    0x100  },     // Boot Service Code
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
  }

  return TRUE;
}


typedef struct {
  CHAR8    FvName[8];
  UINT32   FvBase;
  BOOLEAN  NeedPeiHandle;
} FV_INFO_LIST;

FV_INFO_LIST gFvList[] = {
  {{"FvMain"},  _PCD_VALUE_PcdFlashFvMainBase,    TRUE},
  {{"FvMain2"}, _PCD_VALUE_PcdFlashFvMain2Base,   FALSE},
  {{"FvNet"},   _PCD_VALUE_PcdFlashFvNetworkBase, FALSE},
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
  UINT32                           Crc32 = 0;


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

      if(FvAddr == _PCD_VALUE_PcdFlashFvNetworkBase){
        if(SetupData->UefiNetworkStack || SetupData->LanBoot){
        } else {
          continue;
        }
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

//-      LibCalcCrc32(FvData, FvSize, &Crc32);
      DEBUG((EFI_D_INFO, "FvMem(%X,%X) Crc32:%X\n", FvData, FvSize, Crc32));
    
      BuildFvHob((UINTN)FvData, FvSize);

      if(gFvList[Index].NeedPeiHandle){
        PeiServicesInstallFvInfoPpi(
          NULL,
          FvData,
          FvSize,
          NULL,
          NULL
          ); 
      }
    }
    
    PERF_END(NULL, "report", "FV", 0);  
  }    

  FvSize = PcdGet32(PcdFlashNvLogoSize);
  if(FvSize){
    FvData = AllocatePages(EFI_SIZE_TO_PAGES(FvSize));
    ASSERT(FvData!=NULL);
    FvHdr = (EFI_FIRMWARE_VOLUME_HEADER*)(UINTN)PcdGet32(PcdFlashNvLogoBase);
    CopyMem(FvData, (VOID*)FvHdr, FvSize);
    PcdSet32(PcdLogoDataAddress, (UINT32)(UINTN)FvData);
    PcdSet32(PcdLogoDataSize, FvSize);
  }

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
  //EFI_HOB_GUID_TYPE                *GuidHob;
  SETUP_DATA                       *SetupData;
  //VOID                             *AcpiVar;
  //EFI_SMRAM_DESCRIPTOR             *SmramDesc;
  //BOOLEAN                          SmramCloseLock;
  //UINT64                           RegionState;
  

  DEBUG((EFI_D_INFO, "MemCallBack\n"));

  Status = PeiServicesGetBootMode(&BootMode);
  ASSERT_EFI_ERROR(Status);  

  SetupData = (SETUP_DATA*)GetSetupDataHobData();

//  GuidHob = GetFirstGuidHob(&gEfiPlatformMemInfoGuid);
//  ASSERT(GuidHob!=NULL);  
//  MemInfo = (PLATFORM_MEMORY_INFO*)GET_GUID_HOB_DATA(GuidHob); 
//
//  
//
//  Status = CpuCachePpiInit();
//  ASSERT_EFI_ERROR(Status); 
//
//  PERF_START(NULL, "CACHE", NULL, 0);    
//  SetCacheMtrr(PeiServices, MemInfo);
//  PERF_END  (NULL, "CACHE", NULL, 0);    

#if 0
  SmramCloseLock = 0;
  if(SmramCloseLock){
    RegionState = EFI_SMRAM_CLOSED | EFI_SMRAM_LOCKED;
  } else {
    RegionState = EFI_SMRAM_OPEN;
  }

  SmramDesc = (EFI_SMRAM_DESCRIPTOR*)BuildGuidHob (
                                       &gSmramDescTableGuid,
                                       sizeof(EFI_SMRAM_DESCRIPTOR) * 2
                                       );
  ASSERT(SmramDesc != NULL);  
  SmramDesc[0].CpuStart = MemInfo->TSegAddr;
  SmramDesc[0].PhysicalStart = SmramDesc[0].CpuStart;
  SmramDesc[0].PhysicalSize = EFI_PAGE_SIZE;
  SmramDesc[0].RegionState = RegionState | EFI_ALLOCATED;
  
  SmramDesc[1].CpuStart = MemInfo->TSegAddr + EFI_PAGE_SIZE;
  SmramDesc[1].PhysicalStart = SmramDesc[1].CpuStart;
  SmramDesc[1].PhysicalSize = MemInfo->TSegSize - EFI_PAGE_SIZE;
  SmramDesc[1].RegionState = RegionState;

  AcpiVar = BuildGuidHob (
              &gEfiAcpiVariableGuid,
              sizeof(EFI_SMRAM_DESCRIPTOR)
              );
  CopyMem(AcpiVar, SmramDesc, sizeof(EFI_SMRAM_DESCRIPTOR));
#endif
  
  if(BootMode == BOOT_ON_S3_RESUME){
    Status = SmmAccessPpiInstall(PeiServices);
    ASSERT_EFI_ERROR(Status);      
    Status = SmmControlPpiInstall(PeiServices);
    ASSERT_EFI_ERROR(Status);

  } else {
    ReportResourceForDxe(PeiServices, BootMode, SetupData);
    if(BootMode == BOOT_IN_RECOVERY_MODE){
      PeimInitializeRecovery((EFI_PEI_SERVICES**)PeiServices);
    } else if(BootMode == BOOT_ON_FLASH_UPDATE){  
    }
  }
  
  return Status;
}


