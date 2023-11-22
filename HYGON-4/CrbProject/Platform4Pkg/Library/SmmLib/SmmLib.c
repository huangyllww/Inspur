
#include <Base.h>
#include <library/PcdLib.h>
#include <library/IoLib.h>
#include <Fch.h>



VOID
EFIAPI
ClearSmi (
  VOID
  )
{

  UINT32              SmmControlData32;


  //
  // Clear SmiCmdPort Status Bit
  //
  SmmControlData32 = MmioRead32(ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG88);
  SmmControlData32 |= BIT11;
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG88, SmmControlData32);


  //
  // Set the EOS Bit
  //
  SmmControlData32 = MmioRead32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98);
  SmmControlData32 |= BIT28;
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98, SmmControlData32);

}





