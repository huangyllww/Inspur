

#include <PiPei.h>
#include <Pi/PiBootMode.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/Capsule.h>
#include <Library/PlatformCommLib.h>
#include <Guid/CapsuleVendor.h>
#include <Guid/MemoryTypeInformation.h>
#include <Ppi/Capsule.h>
#include <Framework/StatusCode.h>
#include <PlatformDefinition.h>
#include <Token.h>
#include <Library/ReportStatusCodeLib.h>
#include <ByoStatusCode.h>


STATIC EFI_PEI_PPI_DESCRIPTOR  gBootModePpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMasterBootModePpiGuid,
  NULL
};



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
  BOOLEAN                          BootState;
  BOOLEAN                          IsRecovery = FALSE;
  UINTN                            p;
  UINT32                           FvBB, FvBBEnd;


  BootMode = BOOT_WITH_FULL_CONFIGURATION;

// 1. Check Recovery
  p = (UINTN)&gBootModePpi;
  if(p < SEC_HIGH_MEMORY_REGION_BASE + PcdGet32(PcdFlashAreaSize)){
    p = p - SEC_HIGH_MEMORY_REGION_BASE + PcdGet32(PcdFlashAreaBaseAddress);
  }
  FvBB = PcdGet32(PcdFlashFvRecoveryBase);
  FvBBEnd = FvBB + PcdGet32(PcdFlashFvRecoverySize) - 1;

  if(!(p > FvBB && p < FvBBEnd)){
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
  
#if 0
  if(GetSleepTypeAfterWakeup(PeiServices, &SleepType)){
    switch (SleepType) {
      case PMIO_PM1_CNT_S3:
        BootMode = BOOT_ON_S3_RESUME;
        goto CheckNext;
        break;

      case PMIO_PM1_CNT_S4:
        BootMode = BOOT_ON_S4_RESUME;
        break;

      case PMIO_PM1_CNT_S5:
        BootMode = BOOT_ON_S5_RESUME;
        goto CheckNext;
        break;

      default:
        DEBUG((EFI_D_ERROR, "SleepType:0x%X\n", SleepType));
        goto CheckNext;
        break;
    }
    goto UpdateBootMode;
  }
#endif
//CheckNext:  
#define AcpiPm1Ctl    0x804
#define ACPI_S0       (0 << 10)
#define ACPI_S3       (3 << 10)
#define ACPI_S4       (4 << 10)
#define ACPI_S5       (5 << 10)
#define ACPI_BM_MASK  (7 << 10)
  SleepType = IoRead16(AcpiPm1Ctl) & ACPI_BM_MASK;
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
      DEBUG((EFI_D_ERROR, "SleepType:0x%X\n", SleepType));
      break;
  }
	
  BootState = PcdGetBool(PcdBootState);
  DEBUG((EFI_D_INFO, "BootState:%d\n", BootState));
//  if(!BootState){
//    BootMode = BOOT_ASSUMING_NO_CONFIGURATION_CHANGES;
//    goto UpdateBootMode;
//  }

UpdateBootMode:
  DEBUG((DEBUG_ERROR, "BootMode:0x%x\n", BootMode));
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


