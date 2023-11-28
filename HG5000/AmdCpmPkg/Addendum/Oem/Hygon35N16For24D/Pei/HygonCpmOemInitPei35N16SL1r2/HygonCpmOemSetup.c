#include <AmdCpmPei.h>
#include <AmdCpmOem.h>
#include "HygonCpmOemInitPeim.h"

#include <SetupConfig.h>
#include EFI_PPI_DEFINITION (CpmVariable)

EFI_STATUS
EFIAPI
GetSystemConfiguration (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN OUT   SYSTEM_CONFIGURATION         *SystemConfiguration
  )
{
  UINTN                               DataSize;
  EFI_STATUS                          Status;
  UINT32                              Attributes;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI     *Variable;
  EFI_GUID                            SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;

  Status = (**PeiServices).LocatePpi (
                             PeiServices,
                             &gEfiPeiReadOnlyVariable2PpiGuid,
                             0,
                             NULL,
                             (VOID**)&Variable
                             );
//  ASSERT_EFI_ERROR (Status);

  Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
  DataSize = sizeof (SYSTEM_CONFIGURATION);

  Status = Variable->GetVariable (
                       Variable,
                       L"Setup",
                       &SystemConfigurationGuid,
                       &Attributes,
                       &DataSize,
                       SystemConfiguration
                       );
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Update Setup Options
 *
 * This function reads setup options from ReadOnlyVariable and fills in the data
 * structure of CPM OEM Setup Option.
 *
 * @param[in]     PeiServices    Pointer to Pei Services
 * @param[in]     SetupOption    Pointer to CPM Setup Option Data
 *
 * @retval        EFI_SUCCESS    Function initialized successfully
 * @retval        EFI_ERROR      Function failed (see error for more details)
 *
 */
EFI_STATUS
EFIAPI
AmdCpmOemSetupOption (
  IN       EFI_PEI_SERVICES       **PeiServices,
  IN       CPM_OEM_SETUP_OPTION   *SetupOption
  )
{
  SYSTEM_CONFIGURATION            SystemConfiguration;
  EFI_STATUS                      Status;

  Status = GetSystemConfiguration (PeiServices, &SystemConfiguration);
  if (EFI_ERROR (Status)) {
    SetupOption->BlueToothEn              = 0;
    SetupOption->SystemBootWithPS0        = 0;
    SetupOption->OemTravisLCtrl           = 0;
    SetupOption->WlanPowerControl         = 0;
    SetupOption->UnusedGppClkOff          = 0;
    SetupOption->SpreadSpectrumSwitch     = 0;
    SetupOption->ClockRequest             = 0;
    SetupOption->LanPowerControl          = 0;
    SetupOption->AmdPspEnable             = 0;
    return EFI_SUCCESS;
  }

  SetupOption->BlueToothEn              = SystemConfiguration.AMD_CPM_SETUP_OPTION_BLUE_TOOTH_EN;
  SetupOption->SystemBootWithPS0        = SystemConfiguration.AMD_CPM_SETUP_OPTION_SYSTEM_BOOT_WITH_PS0;
  SetupOption->OemTravisLCtrl           = SystemConfiguration.AMD_CPM_SETUP_OPTION_TRAVISL_CONTROL;
  SetupOption->UnusedGppClkOff          = SystemConfiguration.AMD_CPM_SETUP_OPTION_UNUSED_GPP_CLOCK_OFF;
  SetupOption->SpreadSpectrumSwitch     = SystemConfiguration.AMD_CPM_SETUP_OPTION_SPREAD_SPECTRUM_SWITCH;
  SetupOption->ClockRequest             = SystemConfiguration.AMD_CPM_SETUP_OPTION_CLKREQ;
  SetupOption->WlanPowerControl         = SystemConfiguration.AMD_CPM_SETUP_OPTION_WLAN_EN;
  SetupOption->LanPowerControl          = SystemConfiguration.AMD_CPM_SETUP_OPTION_LAN_POWER_CONTROL;
  SetupOption->PX56Support              = SystemConfiguration.AMD_CPM_SETUP_OPTION_POWERXPRESS_5_6;
  SetupOption->EvalAutoDetection        = SystemConfiguration.AMD_CPM_SETUP_OPTION_EVAL_AUTO_DETECTION;
  SetupOption->AmdPspEnable             = SystemConfiguration.AMD_CPM_SETUP_OPTION_PSP_ENABLE;
  return EFI_SUCCESS;
}
