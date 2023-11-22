/* $NoKeywords:$ */

/**
 * @file
 *
 * FCH PEIM
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project   FCH PEIM
 *
 */
/*****************************************************************************
 *
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 *
 ******************************************************************************
 */
#include "FchPei.h"
#include "FchReset.h"
#include "FchStall.h"
#include <Library/FchPeiLib.h>
#define FILECODE  UEFI_PEI_FCHPEI_FCHPEI_FILECODE

#include <Ppi/CpuIo.h>
#include <Ppi/Reset.h>
#include <Ppi/Stall.h>
#include <Ppi/SmmControl.h>

#include <Library/BaseMemoryLib.h>

extern EFI_GUID gFchResetDataHobGuid;
extern FCH_RESET_DATA_BLOCK  InitResetCfgDefault;

//
// Module globals
//
STATIC EFI_PEI_RESET_PPI  mResetPpi = {
  FchPeiReset
};

STATIC EFI_PEI_STALL_PPI  mStallPpi = {
  FCH_STALL_RESOLUTION_USEC,
  FchPeiStall
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mPpiListReset = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiResetPpiGuid,
  &mResetPpi
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mPpiListStall = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiStallPpiGuid,
  &mStallPpi
};

// Data init routine to setup default value
EFI_STATUS
EFIAPI
FchInitResetDataDefault (
  IN       FCH_RESET_DATA_BLOCK  *FchParams
  );

EFI_STATUS
EFIAPI
FchInitPcdResetData (
  IN       FCH_RESET_DATA_BLOCK  *FchParams
  );

// main routine for Fch PEI init work
EFI_STATUS
EFIAPI
FchInitPei (
  IN       FCH_RESET_DATA_BLOCK  *FchParams
  );

EFI_STATUS
EFIAPI
FchSmmS3RestoreSmiCallbackPpi (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mNotifyFchSmmTriggerPpi = {
  EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gEfiEndOfPeiSignalPpiGuid,
  FchSmmS3RestoreSmiCallbackPpi
};

/*********************************************************************************
 * Name: FchPeiHyExInit
 *
 * Description:
 *               > Update boot mode
 *               > Install Reset PPI
 *               > Install SMBUS PPI
 *               > Install Stall PPI
 *
 * Input
 *   FfsHeader   : pointer to the firmware file system header
 *   PeiServices : pointer to the PEI service table
 *
 * Output
 *   EFI_SUCCESS : Module initialized successfully
 *   EFI_ERROR   : Initialization failed (see error for more details)
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
FchPeiHyExInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS         Status;
  EFI_PEI_RESET_PPI  *PeiReset;
  EFI_PEI_STALL_PPI  *PeiStall;
  EFI_HOB_GUID_TYPE  *FchHob;

  FCH_PEI_PRIVATE         *FchPrivate;
  EFI_PEI_PPI_DESCRIPTOR  *PpiListFchInit;
  FCH_RESET_DATA_BLOCK    *FchParams;
  EFI_BOOT_MODE           BootMode;

  HGPI_TESTPOINT (TpFchPeiEntry, NULL);
  //
  // Check SOC ID
  //

  //
  // Check Fch HW ID
  //

  // Create Fch GUID HOB to save FCH_RESET_DATA_BLOCK
  Status = (*PeiServices)->CreateHob (
                             PeiServices,
                             EFI_HOB_TYPE_GUID_EXTENSION,
                             sizeof (EFI_HOB_GUID_TYPE) + sizeof (FCH_RESET_DATA_BLOCK),
                             &FchHob
                             );

  ASSERT (!EFI_ERROR (Status));
  CopyMem (&FchHob->Name, &gFchResetDataHobGuid, sizeof (EFI_GUID));
  FchHob++;
  FchParams = (FCH_RESET_DATA_BLOCK *)FchHob;
  // load default to Fch data structure
  Status = FchInitResetDataDefault (FchParams);

  // Init FCH_PEI_PRIVATE
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (FCH_PEI_PRIVATE),
                             &FchPrivate
                             );
  ASSERT (!EFI_ERROR (Status));

  FchPrivate->Signature = FCH_PEI_PRIVATE_DATA_SIGNATURE;
  FchPrivate->StdHdr.AltImageBasePtr  = (UINT32)PeiServices;
  FchPrivate->FchInitPpi.Revision     = HYGON_FCH_INIT_PPI_REV;
  FchPrivate->FchInitPpi.FchResetData = (VOID *)FchParams;
  FchPrivate->FchInitPpi.FpFchXhciRecovery = FchInitXhciOnRecovery;
  FchPrivate->FchInitPpi.FpFchEhciRecovery = FchInitEhciOnRecovery;
  FchPrivate->FchInitPpi.FpFchSataRecovery = FchInitSataOnRecovery;
  FchPrivate->FchInitPpi.FpFchGppRecovery  = FchInitGppOnRecovery;

  // platform/OEM update 6.5
  // HgpiFchOemCallout (FchParams);
  // Do the real init tasks
  DEBUG ((DEBUG_INFO, "[FchInitPei] Fch Pei Init ...Start.\n"));
  Status = FchInitPei (FchParams);
  DEBUG ((DEBUG_INFO, "[FchInitPei] Fch Pei Init ...Complete.\n"));
  //
  // Update the boot mode
  //
//Status = FchUpdateBootMode ((EFI_PEI_SERVICES **)PeiServices);      // byo230831 -
//ASSERT (!EFI_ERROR (Status));                                       // byo230831 -

  //
  // publish other PPIs
  //
  // Reset PPI
  // check to see if an instance is already installed
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gEfiPeiResetPpiGuid,
                             0,
                             NULL,
                             &PeiReset
                             );

  if (EFI_NOT_FOUND == Status) {
    // No instance currently installed, install our own
    Status = (*PeiServices)->InstallPpi (
                               PeiServices,
                               &mPpiListReset
                               );

    ASSERT (!EFI_ERROR (Status));
  }

  // Stall PPI
  // check to see if an instance is already installed
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gEfiPeiStallPpiGuid,
                             0,
                             NULL,
                             &PeiStall
                             );

  if (EFI_NOT_FOUND == Status) {
    // There is no instance currently installed, install our own
    Status = (*PeiServices)->InstallPpi (
                               PeiServices,
                               &mPpiListStall
                               );

    ASSERT (!EFI_ERROR (Status));
  }

  // Allocate memory for the PPI descriptor
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             &PpiListFchInit
                             );
  ASSERT (!EFI_ERROR (Status));

  PpiListFchInit->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiListFchInit->Guid  = &gHygonFchInitPpiGuid;
  PpiListFchInit->Ppi   = &FchPrivate->FchInitPpi;

  Status = (*PeiServices)->InstallPpi (
                             PeiServices,
                             PpiListFchInit
                             );

  (*PeiServices)->GetBootMode (PeiServices, &BootMode);
  if(BootMode == BOOT_ON_S3_RESUME) {
    Status = (*PeiServices)->NotifyPpi (PeiServices, &mNotifyFchSmmTriggerPpi);
  }

  HGPI_TESTPOINT (TpFchPeiExit, NULL);
  return Status;
}

/*********************************************************************************
 * Name: FchInitResetDataDefault
 *
 * Description:
 *               load Default value of FCH_RESET_DATA_BLOCK
 *
 * Input
 *   FchParams   : pointer to FCH_RESET_DATA_BLOCK
 *
 * Output
 *   EFI_SUCCESS : Module initialized successfully
 *   EFI_ERROR   : Initialization failed (see error for more details)
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
FchInitResetDataDefault (
  IN       FCH_RESET_DATA_BLOCK  *FchParams
  )
{
  EFI_STATUS  Status;

  *FchParams = InitResetCfgDefault;

  Status = FchPlatformOemPeiInit ((VOID *)FchParams);

  ASSERT (!EFI_ERROR (Status));

  IDS_HOOK (IDS_HOOK_FCH_INIT_RESET, NULL, (VOID *)FchParams);

  FchInitPcdResetData (FchParams);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FchInitPcdResetData (
  IN FCH_RESET_DATA_BLOCK  *FchParams
  )
{
  FchParams->FchBldCfg.CfgSmbus0BaseAddress = PcdGet16 (PcdHygonFchCfgSmbus0BaseAddress);
  FchParams->FchBldCfg.CfgSioPmeBaseAddress = PcdGet16 (PcdHygonFchCfgSioPmeBaseAddress);
  FchParams->FchBldCfg.CfgAcpiPm1EvtBlkAddr = PcdGet16 (PcdHygonFchCfgAcpiPm1EvtBlkAddr);
  FchParams->FchBldCfg.CfgAcpiPm1CntBlkAddr = PcdGet16 (PcdHygonFchCfgAcpiPm1CntBlkAddr);
  FchParams->FchBldCfg.CfgAcpiPmTmrBlkAddr  = PcdGet16 (PcdHygonFchCfgAcpiPmTmrBlkAddr);
  FchParams->FchBldCfg.CfgCpuControlBlkAddr = PcdGet16 (PcdHygonFchCfgCpuControlBlkAddr);
  FchParams->FchBldCfg.CfgAcpiGpe0BlkAddr   = PcdGet16 (PcdHygonFchCfgAcpiGpe0BlkAddr);
  FchParams->FchBldCfg.CfgSmiCmdPortAddr    = PcdGet16 (PcdHygonFchCfgSmiCmdPortAddr);
  // FchParams->FchBldCfg.CfgCpuControlBlkAddr = PcdGet16 (PcdHygonFchCfgCpuControlBlkAddr);

  // Dynamic PCDs
  FchParams->Misc2.LegacyFree              = PcdGetBool (PcdLegacyFree);
  // FchParams.Misc2.EcKbd                 = PcdGetBool (PcdEcKbd);
  FchParams->Misc2.FchOscout1ClkContinous  = PcdGetBool (PcdFchOscout1ClkContinous);
  FchParams->FchReset.SataEnable           = PcdGet32 (PcdSataEnableHyEx);
  FchParams->SataMode.SataSetMaxGen2       = PcdGetBool (PcdSataSetMaxGen2);
  FchParams->SataMode.SataClkMode          = PcdGet8 (PcdSataClkMode);
  FchParams->FchReset.Xhci0Enable          = PcdGetBool (PcdXhci0EnableHyEx);
  FchParams->FchReset.Xhci1Enable          = PcdGetBool (PcdXhci1EnableHyEx);
  FchParams->Misc2.LpcClockDriveStrength   = PcdGet8 (PcdLpcClockDriveStrength);
  FchParams->Xhci.XhciECCDedErrRptEn       = PcdGetBool (PcdXhciECCDedErrRptEn);
  FchParams->Spi.SpiMode                   = PcdGet8 (PcdResetMode);
  FchParams->Spi.SpiSpeed                  = PcdGet8 (PcdResetSpiSpeed);
  FchParams->Spi.SpiFastSpeed                 = PcdGet8 (PcdResetFastSpeed);
  FchParams->Spi.LpcClk0                   = PcdGetBool (PcdLpcClk0);
  FchParams->Spi.LpcClk1                   = PcdGetBool (PcdLpcClk1);
  FchParams->Espi.EspiEc0Enable            = PcdGetBool (PcdEspiEc0Enable);
  FchParams->Espi.EspiIo80Enable           = PcdGetBool (PcdEspiIo80Enable);
  FchParams->Espi.EspiKbc6064Enable        = PcdGetBool (PcdEspiKbc6064Enable);
  FchParams->Gpp.SerialDebugBusEnable      = PcdGetBool (PcdSerialDebugBusEnable);
  FchParams->Misc2.WdtEnable               = PcdGetBool (PcdFchWdtEnable);
  FchParams->Xhci.Xhci0PortNum             = PcdGet8 (PcdXhci0PortNum);
  FchParams->Misc2.FchSerialIrqSupport     = PcdGetBool(PcdFchSerialIrqSupport);

  return EFI_SUCCESS;
}

/*********************************************************************************
 * Name: FchInitPei
 *
 * Description:
 *               Initialization for FCH controller
 *
 * Input
 *   FchParams   : pointer to FCH_RESET_DATA_BLOCK
 *
 * Output
 *   EFI_SUCCESS : Module initialized successfully
 *   EFI_ERROR   : Initialization failed (see error for more details)
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
FchInitPei (
  IN       FCH_RESET_DATA_BLOCK  *FchParams
  )
{
  FchInitReset (FchParams);

  return EFI_SUCCESS;
}

/*********************************************************************************
 * Name: FchGetBootMode
 *
 * Description:
 *   This function determines whether the platform is resuming from an S state
 *   using the FCH ACPI registers
 *
 * Input:
 *   PeiServices : a pointer to the PEI service table pointer
 *   pFchPrivate  : pointer to the FCH PEI private data structure
 *
 * Output:
 *   EFI_BOOT_MODE : Boot mode from SB
 *
 *********************************************************************************/
EFI_BOOT_MODE
FchGetBootMode (
  IN       EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_PEI_CPU_IO_PPI  *CpuIo;
  UINT16              FchBootMode;
  UINTN               AcpiPm1Ctl;

  //
  // find the CpuIo protocol
  //
  CpuIo = (*PeiServices)->CpuIo;

  //
  // Check the FCH WAK_STS bit in the ACPI_PM1_CTL register
  //
  // get the address PM1_CTL register address
  AcpiPm1Ctl = LibFchPmIoRead16 (PeiServices, FCH_PMIOA_REG62);

  // get the boot mode as seen by the south bridge
  FchBootMode = (CpuIo->IoRead16 (PeiServices, CpuIo, AcpiPm1Ctl) & ACPI_BM_MASK);

  // convert the boot mode to the EFI version
  if (ACPI_S3 == FchBootMode) {
    return (BOOT_ON_S3_RESUME);
  }

  if (ACPI_S4 == FchBootMode) {
    return (BOOT_ON_S4_RESUME);
  }

  if (ACPI_S5 == FchBootMode) {
    return (BOOT_ON_S5_RESUME);
  }

  // S0 or unsupported Sx mode
  return (BOOT_WITH_FULL_CONFIGURATION);
}

/*********************************************************************************
 * Name: FchUpdateBootMode
 *
 * Description:
 *   This function update the platform boot mode based on the information
 *   gathered from the south bridge.
 *   Note that we do not publish the BOOT_MODE PPI since the platform
 *   is responsible for deciding what the actual boot mode is.
 *
 * Input:
 *   PeiServices : a pointer to the PEI service table pointer
 *   pFchPrivate  : pointer to the FCH PEI private data structure
 *
 * Output:
 *
 *********************************************************************************/
EFI_STATUS
FchUpdateBootMode (
  IN       EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_BOOT_MODE  BootMode;
  EFI_BOOT_MODE  FchBootMode;

  // Get FCH Boot mode
  FchBootMode = FchGetBootMode (PeiServices);

  // Get the platform boot mode
  (*PeiServices)->GetBootMode (
                    PeiServices,
                    &BootMode
                    );

  // Update boot mode if we are more important than the platform
  if ((BOOT_IN_RECOVERY_MODE != BootMode) && (BOOT_ON_FLASH_UPDATE != BootMode) && 
      (BOOT_WITH_FULL_CONFIGURATION != FchBootMode)) {
    // Set Sx boot mode
    (*PeiServices)->SetBootMode (
                      PeiServices,
                      FchBootMode
                      );
  }

  return (EFI_SUCCESS);
}

EFI_STATUS
EFIAPI
FchInitXhciOnRecovery (
  IN       HYGON_FCH_INIT_PPI   *This,
  IN       UINT32             XhciRomAddress
  )
{
  FCH_PEI_PRIVATE  *FchPrivate;

  FchPrivate = FCH_PEI_PRIVATE_FROM_THIS (This);
  // FchXhciOnRecovery (*FchPrivate, XhciRomAddress);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FchInitEhciOnRecovery (
  IN       HYGON_FCH_INIT_PPI   *This,
  IN       UINT32             EhciTemporaryBarAddress
  )
{
  FCH_PEI_PRIVATE  *FchPrivate;

  FchPrivate = FCH_PEI_PRIVATE_FROM_THIS (This);
  // FchEhciOnRecovery (*FchPrivate, EhciTemporaryBarAddress);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FchInitSataOnRecovery (
  IN       HYGON_FCH_INIT_PPI   *This,
  IN       UINT32             SataBar0,
  IN       UINT32             SataBar5
  )
{
  FCH_PEI_PRIVATE  *FchPrivate;

  FchPrivate = FCH_PEI_PRIVATE_FROM_THIS (This);
  // FchSataOnRecovery (*FchPrivate, SataBar0, SataBar5 );
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FchInitGppOnRecovery (
  IN       HYGON_FCH_INIT_PPI   *This,
  IN       FCH_GPP_R          *FchGpp
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FchSmmS3RestoreSmiCallbackPpi (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS           Status;
  PEI_SMM_CONTROL_PPI  *SmmControl;
  UINT8                SmiCommand;
  UINTN                Size;

  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gPeiSmmControlPpiGuid,
                             0,
                             NULL,
                             &SmmControl
                             );
  ASSERT (!EFI_ERROR (Status));
  if(EFI_ERROR (Status)) {
    return Status;
  }

  SmiCommand = PcdGet8 (PcdFchOemBeforePciRestoreSwSmi);
  Size   = sizeof (SmiCommand);
  Status = SmmControl->Trigger (
                         (EFI_PEI_SERVICES **)PeiServices,
                         SmmControl,
                         (INT8 *)&SmiCommand,
                         &Size,
                         FALSE,
                         0
                         );
  ASSERT (!EFI_ERROR (Status));

  SmiCommand = PcdGet8 (PcdFchOemAfterPciRestoreSwSmi);
  Size   = sizeof (SmiCommand);
  Status = SmmControl->Trigger (
                         (EFI_PEI_SERVICES **)PeiServices,
                         SmmControl,
                         (INT8 *)&SmiCommand,
                         &Size,
                         FALSE,
                         0
                         );
  ASSERT (!EFI_ERROR (Status));

  return Status;
}
