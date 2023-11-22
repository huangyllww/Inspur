

#include <PlatformSetupDxe.h>


VOID UpdateTpcmInfo(EFI_HII_HANDLE HiiHandle);


EFI_STATUS
EFIAPI
MainFormCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
  )
{
  SETUP_FORMSET_INFO    *SetupFormSet;

  if (Action == EFI_BROWSER_ACTION_FORM_OPEN) {
    if (KeyValue == KEY_TPCM_INFO_INIT) {
      SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
//-   UpdateTpcmInfo(SetupFormSet->HiiHandle);
    }
    return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;
}


