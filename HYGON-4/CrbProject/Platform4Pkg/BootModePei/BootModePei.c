

#include <PiPei.h>
#include <Pi/PiBootMode.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/Capsule.h>
#include <Library/ByoHygonCommLib.h>
#include <Guid/CapsuleVendor.h>
#include <Guid/MemoryTypeInformation.h>
#include <Ppi/Capsule.h>
#include <Library/PlatformCommLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/IoLib.h>
#include <ByoStatusCode.h>
#include <Library/BaseCryptLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>


STATIC EFI_PEI_PPI_DESCRIPTOR  gBootModePpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMasterBootModePpiGuid,
  NULL
};


const UINT8 gHashDataTag[16] = {"_FVHASH_\0\0\0\0\0\0\0"};

typedef struct {
  UINT32  Offset;
  UINT32  Length;
} FV_INFO;

typedef struct {
  UINT8    Tag[16];
  UINT8    Hash[32];
  UINT8    FvInfoCount;
  UINT8    FvInfoCountR;
  UINT8    Reserved[2];
  FV_INFO  FvInfo[1];
} FV_HASH_INFO;

BOOLEAN
FvMainHashCheck (
  VOID
  )
{
  FV_HASH_INFO *HashInfo = (FV_HASH_INFO*)(UINTN)PcdGet32(PcdFvMainHashDataBase);
  UINTN        CtxSize;
  VOID         *HashCtx;
  UINTN        Index;
  UINT8        Hash[32];


  DEBUG((EFI_D_INFO, "FvMainHashCheck\n"));

  if(HashInfo == NULL){
    DEBUG((EFI_D_INFO, "FvMain Hash not supported\n"));
    return TRUE;
  }
  
  if(CompareMem(HashInfo->Tag, gHashDataTag, sizeof(HashInfo->Tag)) != 0){
    DEBUG((EFI_D_ERROR, "bad Tag\n"));
    return FALSE;
  }
  if(HashInfo->FvInfoCount == 0){
    DEBUG((EFI_D_ERROR, "No FvMain\n"));
    return FALSE;     
  }
  if(HashInfo->FvInfoCountR & HashInfo->FvInfoCount){
    DEBUG((EFI_D_ERROR, "bad FvInfo count\n"));
    return FALSE;    
  }

  CtxSize = Sha256GetContextSize();
  HashCtx = AllocatePages(EFI_SIZE_TO_PAGES(CtxSize));
  ASSERT (HashCtx != NULL);
  Sha256Init (HashCtx);
  for(Index=0; Index<HashInfo->FvInfoCount; Index++){
    DEBUG((EFI_D_INFO, "+%x L:%x\n", HashInfo->FvInfo[Index].Offset, HashInfo->FvInfo[Index].Length));
    Sha256Update(HashCtx, (UINT8*)(UINTN)(PcdGet32(PcdFlashAreaBaseAddress) + HashInfo->FvInfo[Index].Offset), HashInfo->FvInfo[Index].Length);
  }
  Sha256Final(HashCtx, Hash);
  FreePages(HashCtx, EFI_SIZE_TO_PAGES(CtxSize));

  if(CompareMem(Hash, HashInfo->Hash, sizeof(HashInfo->Hash)) != 0){
    DEBUG((EFI_D_ERROR, "FvMain Hash Not Match\n"));
    return FALSE;    
  }  

  DEBUG((EFI_D_INFO, "FvMain Hash Match\n"));
  return TRUE;
}


EFI_STATUS
BootModeInit (
  IN CONST EFI_PEI_SERVICES                 **PeiServices,
  IN       EFI_PEI_READ_ONLY_VARIABLE2_PPI  *Var2Ppi,
  OUT      EFI_BOOT_MODE                    *pBootMode OPTIONAL
  )
{
  EFI_STATUS                       Status;
  EFI_BOOT_MODE                    BootMode;
  UINT16                           SleepType;
  UINTN                            Size;
  UINTN                            p;
  UINT32                           FvBB, FvBBEnd;
  UINT16                           PmBase = PcdGet16(AcpiIoPortBaseAddress);


  BootMode = BOOT_WITH_FULL_CONFIGURATION;

// 1. Check Recovery
  p = (UINTN)&gBootModePpi;
  if(p < FixedPcdGet32(PcdPspLoadBiosBaseAddress) + FixedPcdGet32(PcdFlashAreaSize)){
    p = p - FixedPcdGet32(PcdPspLoadBiosBaseAddress) + FixedPcdGet32(PcdFlashAreaBaseAddress);
  }
  FvBB = PcdGet32(PcdFlashFvRecoveryBase);
  FvBBEnd = FvBB + PcdGet32(PcdFlashFvRecoverySize) - 1;

  if(!(p > FvBB && p < FvBBEnd) || !FvMainHashCheck()){
    DEBUG((EFI_D_INFO, "p:%X FvBB(%X,%X)\n", p, FvBB, FvBBEnd));
    BootMode = BOOT_IN_RECOVERY_MODE;
    REPORT_STATUS_CODE(EFI_PROGRESS_CODE, PEI_RECOVERY_AUTO);
    goto UpdateBootMode;
  }

// 2. default setting.
  Size = 0;
  Status = Var2Ppi->GetVariable (
                      Var2Ppi,
                      EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
                      &gEfiMemoryTypeInformationGuid,
                      NULL,
                      &Size,
                      NULL
                      );

  if (Status == EFI_NOT_FOUND) {
    DEBUG((EFI_D_INFO, "NoMTI\n"));
    BootMode = BOOT_WITH_DEFAULT_SETTINGS;
    goto UpdateBootMode;
  }

  SleepType = IoRead16(PmBase+4) & ACPI_BM_MASK;
  switch (SleepType) {
    case ACPI_S3:
      BootMode = BOOT_ON_S3_RESUME;
      break;

    case ACPI_S4:
      BootMode = BOOT_ON_S4_RESUME;
      break;

    case ACPI_S5:
      BootMode = BOOT_ON_S5_RESUME;
      break;

    default:
      DEBUG((EFI_D_INFO, "SleepType:0x%X\n", SleepType));
      break;
  }


UpdateBootMode:
  DEBUG((DEBUG_INFO, "BootMode:0x%x\n", BootMode));
  Status = (**PeiServices).SetBootMode(PeiServices, BootMode);
  ASSERT_EFI_ERROR(Status);
  Status = (**PeiServices).InstallPpi(PeiServices, &gBootModePpi);
  ASSERT_EFI_ERROR (Status);
  if(pBootMode!=NULL){
    *pBootMode = BootMode;
  }

  return Status;
}



EFI_STATUS
EFIAPI
BootModePeiEntry (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS                       Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *Var2Ppi;

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID**)&Var2Ppi
             );
  ASSERT_EFI_ERROR(Status);

  Status = BootModeInit(PeiServices, Var2Ppi, NULL);
  ASSERT_EFI_ERROR(Status);

  return Status;
}


