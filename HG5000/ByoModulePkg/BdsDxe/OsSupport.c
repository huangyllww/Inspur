

#include "Bds.h"
#include <Protocol/VariableLock.h>


extern EFI_GUID gSetupFileGuid;
EFI_STATUS ByoEfiBootManagerBootApp(IN EFI_GUID *AppGuid);

STATIC BOOLEAN gSetupFilePresent = FALSE;
STATIC BOOLEAN gBootFwUi         = FALSE;
STATIC BOOLEAN gBootRecovery     = FALSE;

/**
  Formalize OsIndication related variables.

  For OsIndicationsSupported, Create a BS/RT/UINT64 variable to report caps
  Delete OsIndications variable if it is not NV/BS/RT UINT64.

  Item 3 is used to solve case when OS corrupts OsIndications. Here simply delete this NV variable.

  Create a boot option for BootManagerMenu if it hasn't been created yet

**/
VOID
BdsFormalizeOSIndicationVariable (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINT64                          OsIndicationSupport;
  UINT64                          OsIndication;
  UINTN                           DataSize;
  UINT32                          Attributes;


  if(IsFilePresentInFv(gBS, &gSetupFileGuid)){
    gSetupFilePresent = TRUE;
    OsIndicationSupport = EFI_OS_INDICATIONS_BOOT_TO_FW_UI;
  } else {
    OsIndicationSupport = 0;
  }

  Status = gRT->SetVariable (
                  EFI_OS_INDICATIONS_SUPPORT_VARIABLE_NAME,
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof(UINT64),
                  &OsIndicationSupport
                  );
  DEBUG((EFI_D_INFO, "%a (L%d) %r %lX\n", __FUNCTION__, __LINE__, Status, OsIndicationSupport));


  //
  // If OsIndications is invalid, remove it.
  // Invalid case
  //   1. Data size != UINT64
  //   2. OsIndication value inconsistence
  //   3. OsIndication attribute inconsistence
  //
  OsIndication = 0;
  Attributes = 0;
  DataSize = sizeof(UINT64);
  Status = gRT->GetVariable (
                  EFI_OS_INDICATIONS_VARIABLE_NAME,
                  &gEfiGlobalVariableGuid,
                  &Attributes,
                  &DataSize,
                  &OsIndication
                  );
  if (Status == EFI_NOT_FOUND) {
    return;
  }

  if ((DataSize != sizeof (OsIndication)) ||
      ((OsIndication & ~OsIndicationSupport) != 0) ||
      (Attributes != (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE))
     ){

    DEBUG ((EFI_D_ERROR, "[Bds] Unformalized OsIndications variable exists. Delete it\n"));
    Status = gRT->SetVariable (
                    EFI_OS_INDICATIONS_VARIABLE_NAME,
                    &gEfiGlobalVariableGuid,
                    0,
                    0,
                    NULL
                    );

    ASSERT_EFI_ERROR(Status);
  }
}




CHAR16  *mReadOnlyVariables[] = {
  EFI_PLATFORM_LANG_CODES_VARIABLE_NAME,
  EFI_LANG_CODES_VARIABLE_NAME,
  EFI_BOOT_OPTION_SUPPORT_VARIABLE_NAME,
  EFI_HW_ERR_REC_SUPPORT_VARIABLE_NAME,
  EFI_OS_INDICATIONS_SUPPORT_VARIABLE_NAME
  };

// Mark the read-only variables if the Variable Lock protocol exists
EFI_STATUS LockReadOnlyVariable()
{
  EDKII_VARIABLE_LOCK_PROTOCOL    *VariableLock;
  EFI_STATUS                      Status;
  UINTN                           Index;

  Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &VariableLock);
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < ARRAY_SIZE (mReadOnlyVariables); Index++) {
      Status = VariableLock->RequestToLock (VariableLock, mReadOnlyVariables[Index], &gEfiGlobalVariableGuid);
      ASSERT_EFI_ERROR (Status);
    }
  }

  return Status;
}



BOOLEAN CheckOsIndication()
{
  UINT64               DataSize;
  EFI_STATUS           Status;
  UINT64               OsIndication;
  BOOLEAN              BootFwUi;
  BOOLEAN              PlatformRecovery;


  DataSize = sizeof (UINT64);
  Status = gRT->GetVariable (
                  EFI_OS_INDICATIONS_VARIABLE_NAME,
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &DataSize,
                  &OsIndication
                  );
  if (EFI_ERROR (Status)) {
    OsIndication = 0;
  }

  BootFwUi         = (BOOLEAN) ((OsIndication & EFI_OS_INDICATIONS_BOOT_TO_FW_UI) != 0);
  PlatformRecovery = FALSE;   // (BOOLEAN) ((OsIndication & EFI_OS_INDICATIONS_START_PLATFORM_RECOVERY) != 0);

// Clear EFI_OS_INDICATIONS_BOOT_TO_FW_UI to acknowledge OS
  if (BootFwUi || PlatformRecovery) {
    OsIndication &= ~((UINT64)(EFI_OS_INDICATIONS_BOOT_TO_FW_UI));
    Status = gRT->SetVariable (
                    EFI_OS_INDICATIONS_VARIABLE_NAME,
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    sizeof(UINT64),
                    &OsIndication
                    );
    ASSERT_EFI_ERROR (Status);
  }

  gBootFwUi = BootFwUi;
  gBootRecovery = PlatformRecovery;

  return (BootFwUi || PlatformRecovery);
  
}



VOID BootOsIndication()
{
  if (gBootFwUi && gSetupFilePresent) {
    ByoEfiBootManagerBootApp(&gSetupFileGuid);
  }
}



