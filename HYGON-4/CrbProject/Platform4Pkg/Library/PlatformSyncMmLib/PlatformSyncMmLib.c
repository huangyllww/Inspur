

#include <Uefi.h>
#include <Guid/HygonCbsConfig.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/ByoCommLib.h>
#include <SetupVariable.h>
#include <BmcConfig.h>


BOOLEAN gAfterReadyToBoot    = FALSE;
STATIC BOOLEAN gNvVarWrite   = FALSE;
EFI_GUID  gBmcSystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;

EFI_STATUS
EFIAPI
PlatSyncSmmReadyToBootNotify (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  gAfterReadyToBoot = TRUE;
  return EFI_SUCCESS;
}


typedef struct {
  CHAR16    *Name;
  EFI_GUID  *Guid;
} NV_VAR_NAME;

STATIC NV_VAR_NAME gNvVarNameList[] = {
  {CBS_SYSTEM_CONFIGURATION_NAME, &gCbsSystemConfigurationGuid},
  {PLATFORM_SETUP_VARIABLE_NAME,  &gPlatformSetupVariableGuid}, 
  {BMC_SETUP_VARIABLE_NAME,       &gBmcSystemConfigurationGuid},
  {SEL_EVENT_SETUP_VARIABLE_NAME, &gBmcSystemConfigurationGuid},
};

VOID
EFIAPI
PlatformAfterSetVariable (
  IN CHAR16                  *VariableName,
  IN EFI_GUID                *VendorGuid,
  IN UINT32                  Attributes,
  IN UINTN                   DataSize,
  IN VOID                    *Data
  )  
{
  UINT8    Cmos;
  UINTN    Index;

  if(!gNvVarWrite && gAfterReadyToBoot && (Attributes & EFI_VARIABLE_NON_VOLATILE)){
    for(Index=0;Index<ARRAY_SIZE(gNvVarNameList);Index++){
      if(StrCmp(VariableName, gNvVarNameList[Index].Name) == 0 && 
         CompareGuid(VendorGuid, gNvVarNameList[Index].Guid)){
        break;
      }
    }
    if(Index < ARRAY_SIZE(gNvVarNameList)){
      Cmos = PcdGet8(PcdNvVarSwSetCmosOffset);
      if(Cmos != 0xFF){
        WriteCheckedCmosByte(Cmos, 1);
        DEBUG((EFI_D_INFO, "NvVarSwSet\n"));
      }
      gNvVarWrite = TRUE;
    }
  }
}  



EFI_STATUS
EFIAPI
PlatformSyncMmLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  VOID        *Registration;
  
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEdkiiSmmReadyToBootProtocolGuid,
                    PlatSyncSmmReadyToBootNotify,
                    &Registration
                    );
  ASSERT_EFI_ERROR(Status);
  
  return EFI_SUCCESS;
}


