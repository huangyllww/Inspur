

#include "SmmPlatform.h"
#include <Library/HygonPspBaseLib.h>
#include <Guid/HygonCbsConfig.h>
#include <Protocol/SmmVariable.h>
#include <Protocol/HygonHpcbProtocol.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/ByoSmiFlashExtProtocol.h>
#include <Library/HpcbLib.h>
#include <Protocol/HygonHpcbProtocol.h>


typedef struct {
  UINT32   Offset;
  UINT32   Size;
} AREA_INFO;

typedef struct {
  UINT32  AllUpdate:1;
} SMI_FLASH_EX_FLAG;

STATIC AREA_INFO         gProtectAreaInfo;
STATIC SMI_FLASH_EX_FLAG gSmiFlashExFlag;
STATIC UINT8             gDownCoreMode;

BOOLEAN IsProtectArea(UINT32 Offset, UINT32 Size)
{
  if(gProtectAreaInfo.Size == 0){
    return FALSE;
  }
  if(Offset >= gProtectAreaInfo.Offset && 
     Size   <= gProtectAreaInfo.Size &&
    Offset + Size <= gProtectAreaInfo.Offset + gProtectAreaInfo.Size){
    DEBUG((EFI_D_INFO, "ignore protect area(%x,%x)\n", Offset, Size));
    return TRUE;
  }

  return FALSE;
}



EFI_STATUS ReWriteHygonCbsForHpcb()
{
  EFI_STATUS                 Status;
  EFI_SMM_VARIABLE_PROTOCOL  *SmmVar = NULL;  
  UINT32                     Attributes;
  UINTN                      DataSize;
  VOID                       *Data = NULL;
  HYGON_HPCB_SERVICE_PROTOCOL *HpcbSvr;

  
  DEBUG((EFI_D_INFO, "ReWriteHygonCbsForApcb\n"));

  Status = gSmst->SmmLocateProtocol(&gEfiSmmVariableProtocolGuid, NULL, (VOID**)&SmmVar);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  DataSize = 0;
  Data     = NULL;
  Status = SmmVar->SmmGetVariable(
                     CBS_SYSTEM_CONFIGURATION_NAME, 
                     &gCbsSystemConfigurationGuid,
                     &Attributes,
                     &DataSize,
                     Data
                     );
  if(Status != EFI_BUFFER_TOO_SMALL){
    DEBUG((EFI_D_INFO, "SmmGetVariable:%r\n", Status));
    if(gDownCoreMode != 0){                               // not auto.
      IoWrite8(0xCD6, 0x10);
      IoWrite8(0xCD7, IoRead8(0xCD7) | BIT1);      
    }
    Status = EFI_SUCCESS;
    goto ProcExit;
  }

  Data = AllocatePool(DataSize);
  if(Data == NULL){
    DEBUG((EFI_D_INFO, "Data:%p\n", Data)); 
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;    
  }

  Status = SmmVar->SmmGetVariable(
                     CBS_SYSTEM_CONFIGURATION_NAME, 
                     &gCbsSystemConfigurationGuid,
                     &Attributes,
                     &DataSize,
                     Data
                     );
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_INFO, "SmmGetVariable:%r\n", Status)); 
    goto ProcExit;
  }

  Status = gSmst->SmmLocateProtocol(&gHygonHpcbSmmServiceProtocolGuid, NULL, (VOID**)&HpcbSvr);
  if(!EFI_ERROR(Status)){
    Status = HpcbSvr->HpcbReload(HpcbSvr);
    DEBUG((EFI_D_INFO, "HpcbReload:%r\n", Status)); 
  }

  Status = SmmVar->SmmSetVariable(
                     CBS_SYSTEM_CONFIGURATION_NAME, 
                     &gCbsSystemConfigurationGuid,
                     Attributes,
                     DataSize,
                     Data
                     );
  DEBUG((EFI_D_INFO, "SmmSetVariable:%r\n", Status));   

ProcExit:
  if(Data != NULL){
    FreePool(Data);
  }
  return Status;
}


BOOLEAN
EFIAPI
PlatformSmiFlashEx (
  IN  SMI_INFO                     *SmiFlashInfo,
  IN  NV_MEDIA_ACCESS_PROTOCOL     *MediaAccess,
  OUT EFI_STATUS                   *RetStatus
  )
{
  SMI_FLASH_EX_FLAG  *p = &gSmiFlashExFlag;

//DEBUG((EFI_D_INFO, "SmiFlashExt(F:%x, O:%x, L:%x) all:%d PVC:%d\n", SmiFlashInfo->SubFunction, SmiFlashInfo->Offset, SmiFlashInfo->Size, p->AllUpdate, p->PspVerChanged));

  switch(SmiFlashInfo->SubFunction){                
    case SUBFUNCTION_WRITECMOS:                     // not -all
      if(p->AllUpdate){
        p->AllUpdate = FALSE;
      }
      break;
    
    case SUBFUNCTION_PROGRAM_FLASH:
      if(!p->AllUpdate && IsProtectArea(SmiFlashInfo->Offset, SmiFlashInfo->Size)){
        return TRUE;
      }
      break;
    
    case SUBFUNCTION_IF_SIGN_UPDATE:
      break;

    case SUBFUNCTION_ENABLE_USB_POWERBUTTON:
      ReWriteHygonCbsForHpcb();
      break;

    case SUBFUNCTION_IF_SIGN_PREPARE:
      ZeroMem(p, sizeof(SMI_FLASH_EX_FLAG));
      p->AllUpdate = TRUE;
      break;
  }

  return FALSE;                                          // smiflash continue to handle
}




STATIC BYO_SMIFLASH_EXT_PROTOCOL gPlatformSmiflashExProtocol = {
  PlatformSmiFlashEx
};

EFI_STATUS PlatSmiFlashExInit()
{
  EFI_HANDLE     SmmHandle = NULL;  
  EFI_STATUS     Status;
  UINT64         Address;
  UINT32         Size;


  gDownCoreMode = PcdGet8(PcdHygonDownCoreMode);

  if(!PSPEntryInfoV2(PSP_VM_GUARD_DATA, &Address, &Size)){
    gProtectAreaInfo.Offset = (UINT32)(Address - FixedPcdGet32(PcdFlashAreaBaseAddress));
    gProtectAreaInfo.Size   = Size;
    DEBUG((EFI_D_INFO, "PspSecureFwData (+%x, %x)\n", gProtectAreaInfo.Offset, gProtectAreaInfo.Size));
  }

  Status = gSmst->SmmInstallProtocolInterface (
                    &SmmHandle,
                    &gPlatformSmiFlashExProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gPlatformSmiflashExProtocol
                    );
  ASSERT_EFI_ERROR (Status);
  
  return Status;
}


