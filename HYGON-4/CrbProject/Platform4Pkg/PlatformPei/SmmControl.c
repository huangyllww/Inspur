

#include "PlatformPei.h"
#include <Ppi/SmmControl.h>
#include <Fch.h>




STATIC
EFI_STATUS
PeiActivate (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN PEI_SMM_CONTROL_PPI  *This,
    IN OUT INT8             *ArgumentBuffer OPTIONAL,
    IN OUT UINTN            *ArgumentBufferSize OPTIONAL,
    IN BOOLEAN              Periodic OPTIONAL,
    IN UINTN                ActivationInterval OPTIONAL
)
{
  UINT8       SmiCmd8;
  UINT16      SmiCmd16, Value16;

  if (Periodic || ((NULL != ArgumentBuffer) && (NULL == ArgumentBufferSize))) {
    return EFI_INVALID_PARAMETER;
  }
  if (NULL == ArgumentBuffer) {
      SmiCmd8 = 0xff;
      SmiCmd16 = 0xffff;
  } else {
      SmiCmd8 = ArgumentBuffer[0];
      SmiCmd16 = (ArgumentBuffer[1] << 8) + ArgumentBuffer[0];
  }

  // Enable ACPI MMIO space
  MmioOr32(ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG04, BIT1);
  // Making sure SW SMI port is SW_SMI_IO_ADDRESS
  Value16 = MmioRead16(ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6A);
  while(Value16 != _PCD_VALUE_PcdSwSmiCmdPort) {
      MmioWrite16(ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6A, _PCD_VALUE_PcdSwSmiCmdPort);
      Value16 = MmioRead16(ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6A);
  }
  // Enable CmdPort SMI
  MmioAndThenOr32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0, (UINT32)~(BIT23 + BIT22), BIT22);
  //
  // Clear SmiEnB to enable SMI function
  //
  MmioAnd32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98, ~BIT31);
  //
  // Set the EOS Bit
  //
  MmioOr32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98, BIT28);

  // Trigger command port SMI
  if (*ArgumentBufferSize == 2) {
      IoWrite16 (_PCD_VALUE_PcdSwSmiCmdPort, SmiCmd16);
  } else {
      IoWrite8 (_PCD_VALUE_PcdSwSmiCmdPort, SmiCmd8);
  }

  return EFI_SUCCESS;
}


STATIC
EFI_STATUS
PeiDeactivate (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN PEI_SMM_CONTROL_PPI  *This,
    IN BOOLEAN              Periodic OPTIONAL
)
{
  if (Periodic) {
    return EFI_INVALID_PARAMETER;
  }

  // Clear SmiCmdPort Status Bit
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG88, BIT11);
  // Set the EOS Bit
  MmioOr32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98, BIT28);

  return EFI_SUCCESS;
}




STATIC PEI_SMM_CONTROL_PPI gSmmControlPpi = {
  PeiActivate,
  PeiDeactivate
};

STATIC EFI_PEI_PPI_DESCRIPTOR  gPpiInstallList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gPeiSmmControlPpiGuid,
    &gSmmControlPpi
  }
};


EFI_STATUS
SmmControlPpiInstall (
  IN CONST EFI_PEI_SERVICES  **PeiServices
)
{
  EFI_STATUS  Status;

  Status = PeiServicesInstallPpi(&gPpiInstallList[0]);
  ASSERT_EFI_ERROR (Status);
  return Status;
}

