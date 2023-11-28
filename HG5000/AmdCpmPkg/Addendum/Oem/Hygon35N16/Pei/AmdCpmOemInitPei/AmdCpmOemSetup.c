/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM OEM Function to Get Setup Options.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  OEM
 * @e \$Revision: 270275 $   @e \$Date: 2013-08-08 14:54:44 -0500 (Thu, 08 Aug 2013) $
 *
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 ******************************************************************************
 */

#include <AmdCpmPei.h>
#include <AmdCpmOem.h>
#include "AmdCpmOemInitPeim.h"

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
