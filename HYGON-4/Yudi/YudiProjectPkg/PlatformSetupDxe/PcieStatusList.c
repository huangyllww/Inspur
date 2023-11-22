
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/HiiLib.h>
#include "SetupItemId.h"
#include <Guid/MdeModuleHii.h>
#include <Library/BaseLib.h>
#include <Library/PlatformCommLib.h>
#include <HygonHsioInfo.h>
#include <Library/UefiBootServicesTableLib.h>
#include <ByoPlatformSetupConfig.h>


CHAR16 *GetClassCodeName(UINT8 ClassCode[3]);


VOID UpdatePcieStatusList(EFI_HII_HANDLE HiiHandle)
{
  VOID                           *StartOpCodeHandle = NULL;
  VOID                           *EndOpCodeHandle   = NULL;
  EFI_IFR_GUID_LABEL             *StartLabel;
  EFI_IFR_GUID_LABEL             *EndLabel;
  EFI_STATUS                     Status;
  EFI_STRING_ID                  StrId;
  SETUP_SLOT_INFO_DATA           *SetupSlot;
  SETUP_SLOT_INFO                *Slot;
  UINTN                          Index;
  CHAR8                          *LinkSpeedStr;
  CHAR8                          *MaxLinkSpeedStr;
  STATIC CHAR16                  StrBuffer[256];


  DEBUG((EFI_D_INFO, "UpdatePcieStatusList\n"));

  Status = gBS->LocateProtocol (
                  &gHygonSetupSlotDevInfoProtoclGuid,
                  NULL,
                  (VOID**)&SetupSlot
                  );
  if(EFI_ERROR(Status) || SetupSlot->DevCount == 0){
    return;
  }
  
  StartOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (StartOpCodeHandle != NULL);
  EndOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (EndOpCodeHandle != NULL);

  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = PCIE_DYNAMIC_LABEL;

  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = PCIE_DYNAMIC_LABEL_END;


  for(Index=0; Index<SetupSlot->DevCount; Index++){
    Slot = &SetupSlot->Info[Index];

    LinkSpeedStr    = LibGetPcieLinkSpeedStr(Slot->CurLinkSpeed);
    MaxLinkSpeedStr = LibGetPcieLinkSpeedStr(Slot->MaxLinkSpeed);
    
    UnicodeSPrint(
      StrBuffer,
      sizeof(StrBuffer), 
      L"%a %a/%a %dX/%dX %s(ID:%08X)",
      Slot->SlotName,
      LinkSpeedStr,
      MaxLinkSpeedStr,
      Slot->CurLinkWidth,
      Slot->MaxLinkWidth,
      GetClassCodeName(Slot->DevClassCode),
      Slot->DevPciId
      );
    StrId = HiiSetString (
              HiiHandle,
              0,
              StrBuffer,
              NULL
              );
    HiiCreateTextOpCode (
      StartOpCodeHandle,
      StrId,
      STRING_TOKEN(STR_EMPTY),
      STRING_TOKEN(STR_EMPTY)
      );
  }

  Status = HiiUpdateForm (
             HiiHandle,
             &gEfiFormsetGuidDevices,        // Formset GUID
             PCIE_FORM_ID,                   // Form ID
             StartOpCodeHandle,              // Label for where to insert opcodes
             EndOpCodeHandle                 // Replace data
             );
  DEBUG((EFI_D_INFO, "HiiUpdateForm:%r\n", Status));

  if(StartOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(StartOpCodeHandle);
  }
  if(EndOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(EndOpCodeHandle);
  }  
}


