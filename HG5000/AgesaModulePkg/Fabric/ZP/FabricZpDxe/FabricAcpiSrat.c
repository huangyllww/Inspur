/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric ACPI SRAT.
 *
 * This funtion collect SRAT information.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ****************************************************************************
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Porting.h"
#include "AMD.h"
#include "FabricAcpiTable.h"
#include <Library/BaseLib.h>
#include "Library/AmdBaseLib.h"
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersZP.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/AmdIdsHookLib.h>
#include "Library/UefiBootServicesTableLib.h"
#include <Protocol/AmdAcpiSratServicesProtocol.h>
#include <Protocol/FabricNumaServicesProtocol.h>
#include <Protocol/FabricTopologyServices.h>
#include "Filecode.h"


#define FILECODE FABRIC_ZP_FABRICZPDXE_FABRICACPISRAT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define BELOW_4GB_ENTRY_COUNT  2  // One for below 1MB and another for the MMIO hole below 4GB

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
FabricCreateSratMemoryInfo (
  IN       AMD_FABRIC_ACPI_SRAT_SERVICES_PROTOCOL     *This,
  IN       SRAT_HEADER                                *SratHeaderStructPtr,
  IN OUT   UINT8                                     **TableEnd
  );

EFI_STATUS
EFIAPI
FabricGetSratMemoryInfo (
  IN       AMD_FABRIC_ACPI_SRAT_SERVICES_PROTOCOL     *This,
     OUT   UINT32                                     *NumberOfDomains,
     OUT   MEMORY_INFO                                **MemoryInfo
  );

VOID
CreateSratMemoryEntry (
  IN OUT   UINT8   **TablePointer,
  IN       UINT32  Domain,
  IN       UINT64  RegionBase,
  IN       UINT64  RegionSize
  );

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
MEMORY_INFO mMemoryInfo[NUMBER_OF_DRAM_REGIONS + BELOW_4GB_ENTRY_COUNT];
UINT32      mMemoryInfoCtr = 0;

STATIC AMD_FABRIC_ACPI_SRAT_SERVICES_PROTOCOL  mFabricAcpiSratServicesProtocol = {
  0x1,
  FabricCreateSratMemoryInfo,
  FabricGetSratMemoryInfo
};

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

EFI_STATUS
EFIAPI
FabricZpAcpiSratProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return (gBS->InstallProtocolInterface (
               &ImageHandle,
               &gAmdFabricAcpiSratServicesProtocolGuid,
               EFI_NATIVE_INTERFACE,
               &mFabricAcpiSratServicesProtocol
               ));
}

EFI_STATUS
EFIAPI
FabricCreateSratMemoryInfo (
  IN       AMD_FABRIC_ACPI_SRAT_SERVICES_PROTOCOL     *This,
  IN       SRAT_HEADER                                *SratHeaderStructPtr,
  IN OUT   UINT8                                     **TableEnd
  )
{
  UINT32                         i;
  UINT32                         Domain;
  UINT32                         SocketId;
  UINT32                         DieId;
  UINT32                         DramMapIndex;
  UINT64                         MemoryLength;
  UINT64                         MemoryBase;
  UINT64                         SizeBelowHole;
  UINT64                         HoleBase;
  DRAM_BASE_ADDRESS_REGISTER     DramBaseAddr;
  DRAM_LIMIT_ADDRESS_REGISTER    DramLimitAddr;
  DRAM_HOLE_CONTROL_REGISTER     DramHoleCtrl;
  FABRIC_NUMA_SERVICES_PROTOCOL  *FabricNuma;

  if (gBS->LocateProtocol (&gAmdFabricNumaServicesProtocolGuid, NULL, (VOID **) &FabricNuma) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  // Initialize memory info structure
  mMemoryInfoCtr = 0;
  for (i = 0; i < NUMBER_OF_DRAM_REGIONS; i++) {
    mMemoryInfo[i].Domain = 0;
    mMemoryInfo[i].RegionSize = 0;
  }

  for (DramMapIndex = 0; DramMapIndex < NUMBER_OF_DRAM_REGIONS; DramMapIndex++) {
    DramBaseAddr.Value = FabricRegisterAccRead (0, 0, DRAMBASEADDR0_FUNC, (DRAMBASEADDR0_REG + (DramMapIndex * DRAM_REGION_REGISTER_OFFSET)), ZP_IOMS0_INSTANCE_ID);
    if (DramBaseAddr.Field.AddrRngVal == 1) {
      DramLimitAddr.Value = FabricRegisterAccRead (0, 0, DRAMLIMITADDR0_FUNC, (DRAMLIMITADDR0_REG + (DramMapIndex * DRAM_REGION_REGISTER_OFFSET)), ZP_IOMS0_INSTANCE_ID);

      SocketId = ((DramLimitAddr.Field.DstFabricID >> FABRIC_ID_SOCKET_SHIFT) & FABRIC_ID_SOCKET_SIZE_MASK);
      DieId = ((DramLimitAddr.Field.DstFabricID >> FABRIC_ID_DIE_SHIFT) & FABRIC_ID_DIE_SIZE_MASK);

      if (FabricNuma->DomainXlat (FabricNuma, (UINTN) SocketId, (UINTN) DieId, &Domain) != EFI_SUCCESS) {
        return EFI_ABORTED;
      }

      MemoryLength = LShiftU64 (((UINT64) (DramLimitAddr.Field.DramLimitAddr + 1 - DramBaseAddr.Field.DramBaseAddr)), 28);
      MemoryBase = LShiftU64 (((UINT64) (DramBaseAddr.Field.DramBaseAddr)), 28);

      if (DramBaseAddr.Field.DramBaseAddr == 0) {
        CreateSratMemoryEntry (TableEnd, Domain, 0ull, 0xA0000ull);
        MemoryBase = (1 * 1024 * 1024);
        MemoryLength -= MemoryBase;

        mMemoryInfo[mMemoryInfoCtr].Domain = Domain;
        mMemoryInfo[mMemoryInfoCtr].RegionSize = MemoryBase;
        mMemoryInfoCtr++;
      }

      if (DramBaseAddr.Field.LgcyMmioHoleEn == 1) {
        DramHoleCtrl.Value = FabricRegisterAccRead (0, 0, DRAMHOLECTRL_FUNC, DRAMHOLECTRL_REG, ZP_IOMS0_INSTANCE_ID);
        ASSERT (DramHoleCtrl.Field.DramHoleValid == 1);

        HoleBase = LShiftU64 (((UINT64) (DramHoleCtrl.Field.DramHoleBase)), 24);
        SizeBelowHole = HoleBase - MemoryBase;

        CreateSratMemoryEntry (TableEnd, Domain, MemoryBase, SizeBelowHole);
        mMemoryInfo[mMemoryInfoCtr].Domain = Domain;
        mMemoryInfo[mMemoryInfoCtr].RegionSize = SizeBelowHole;
        mMemoryInfoCtr++;

        MemoryBase = 0x100000000;
        MemoryLength = LShiftU64 (((UINT64) (DramLimitAddr.Field.DramLimitAddr + 1 - 0x10)), 28);
      }

      CreateSratMemoryEntry (TableEnd, Domain, MemoryBase, MemoryLength);
      mMemoryInfo[mMemoryInfoCtr].Domain = Domain;
      mMemoryInfo[mMemoryInfoCtr].RegionSize = MemoryLength;
      mMemoryInfoCtr++;
    }
  }

  ASSERT (mMemoryInfoCtr <= (NUMBER_OF_DRAM_REGIONS + BELOW_4GB_ENTRY_COUNT));

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FabricGetSratMemoryInfo (
  IN       AMD_FABRIC_ACPI_SRAT_SERVICES_PROTOCOL     *This,
     OUT   UINT32                                     *NumberOfDomains,
     OUT   MEMORY_INFO                                **MemoryInfo
  )
{
  if ((NumberOfDomains == NULL) && (MemoryInfo == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (mMemoryInfoCtr == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (NumberOfDomains != NULL) {
    *NumberOfDomains = mMemoryInfoCtr;
  }

  if (MemoryInfo != NULL) {
    *MemoryInfo = &mMemoryInfo[0];
  }

  return EFI_SUCCESS;
}

VOID
CreateSratMemoryEntry (
  IN OUT   UINT8   **TablePointer,
  IN       UINT32  Domain,
  IN       UINT64  RegionBase,
  IN       UINT64  RegionSize
  )
{
  SRAT_MEMORY  *MemoryEntry;

  MemoryEntry = (SRAT_MEMORY *) *TablePointer;
  *TablePointer += sizeof (SRAT_MEMORY);

  MemoryEntry->Type = SRAT_MEMORY_TYPE;
  MemoryEntry->Length = sizeof (SRAT_MEMORY);
  MemoryEntry->ProximityDomain = Domain;
  MemoryEntry->BaseAddressLow = (UINT32) (RegionBase & 0xFFFFFFFF);
  MemoryEntry->BaseAddressHigh = (UINT32) (RShiftU64 (RegionBase, 32) & 0xFFFFFFFF);
  MemoryEntry->LengthLow = (UINT32) (RegionSize & 0xFFFFFFFF);
  MemoryEntry->LengthHigh = (UINT32) (RShiftU64 (RegionSize, 32) & 0xFFFFFFFF);
  MemoryEntry->Flags.Enabled = 1;
}

