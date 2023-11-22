/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Fabric ACPI CRAT.
 *
 * This funtion collect CRAT information.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Fabric
 *
 */
/*
 ****************************************************************************
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Porting.h"
#include "HYGON.h"
#include "FabricAcpiTable.h"
#include <Library/BaseLib.h>
#include "Library/HygonBaseLib.h"
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersST.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/HygonIdsHookLib.h>
#include "Library/UefiBootServicesTableLib.h"
#include <Protocol/HygonAcpiCratServicesProtocol.h>
#include <Protocol/FabricNumaServicesProtocol.h>
#include <Protocol/FabricTopologyServices.h>
#include "Filecode.h"

#define FILECODE  FABRIC_HYGX_FABRICDXE_FABRICACPICRAT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
FabricGetCratMemoryInfo (
  IN       HYGON_FABRIC_ACPI_CRAT_SERVICES_PROTOCOL     *This,
  IN       CRAT_HEADER                                *CratHeaderStructPtr,
  IN OUT   UINT8                                     **TableEnd
  );

VOID
CreateCratMemoryEntry (
  IN       CRAT_HEADER  *CratHeaderStructPtr,
  IN OUT   UINT8   **TablePointer,
  IN       UINT32  Domain,
  IN       UINT64  RegionBase,
  IN       UINT64  RegionSize
  );

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
STATIC HYGON_FABRIC_ACPI_CRAT_SERVICES_PROTOCOL  mFabricAcpiCratServicesProtocol = {
  0x1,
  FabricGetCratMemoryInfo
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
FabricStAcpiCratProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return (gBS->InstallProtocolInterface (
                 &ImageHandle,
                 &gHygonFabricAcpiCratServicesProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &mFabricAcpiCratServicesProtocol
                 ));
}

EFI_STATUS
EFIAPI
FabricGetCratMemoryInfo (
  IN       HYGON_FABRIC_ACPI_CRAT_SERVICES_PROTOCOL     *This,
  IN       CRAT_HEADER                                *CratHeaderStructPtr,
  IN OUT   UINT8                                     **TableEnd
  )
{
  UINT32                         Domain;
  UINT32                         SocketId;
  UINT32                         CddId;
  UINT32                         LogicalDieId;
  UINT32                         RbId;
  UINT32                         DramMapIndex;
  UINT64                         MemoryLength;
  UINT64                         MemoryBase;
  UINT64                         SizeBelowHole;
  UINT64                         HoleBase;
  DRAM_BASE_ADDRESS_REGISTER     DramBaseAddr;
  DRAM_LIMIT_ADDRESS_REGISTER    DramLimitAddr;
  DRAM_HOLE_CONTROL_REGISTER     DramHoleCtrl;
  FABRIC_NUMA_SERVICES_PROTOCOL  *FabricNuma;

  if (gBS->LocateProtocol (&gHygonFabricNumaServicesProtocolGuid, NULL, (VOID **)&FabricNuma) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  for (DramMapIndex = 0; DramMapIndex < NUMBER_OF_DRAM_REGIONS; DramMapIndex++) {
    DramBaseAddr.Value =
      FabricRegisterAccRead (
        0,
        0,
        DRAMBASEADDR0_FUNC,
        (DRAMBASEADDR0_REG + (DramMapIndex * DRAM_REGION_REGISTER_OFFSET)),
        IOMS0_INSTANCE_ID
        );
    if (DramBaseAddr.Field.AddrRngVal == 1) {
      DramLimitAddr.Value =
        FabricRegisterAccRead (
          0,
          0,
          DRAMLIMITADDR0_FUNC,
          (DRAMLIMITADDR0_REG + (DramMapIndex * DRAM_REGION_REGISTER_OFFSET)),
          IOMS0_INSTANCE_ID
          );

      FabricTopologyGetLocationFromFabricId (DramLimitAddr.HyGxField.DstFabricID, &SocketId, &LogicalDieId, &RbId);
      if (LogicalDieId >= FABRIC_ID_CDD0_DIE_NUM) {
        //DRAM NUMA node
        CddId = (UINT32)GetCddPhysicalDieId (LogicalDieId) - FABRIC_ID_CDD0_DIE_NUM;
        if (FabricNuma->DomainXlat (FabricNuma, (UINTN)SocketId, (UINTN)CddId, &Domain) != EFI_SUCCESS) {
          return EFI_ABORTED;
        }
      } else {
        //CXL DUMA node
        if (FabricNuma->CxlDomainXlat (FabricNuma, (UINTN) SocketId, (UINTN) LogicalDieId, (UINTN) RbId, &Domain) != EFI_SUCCESS) {
          return EFI_ABORTED;
        }
      }

      MemoryLength = LShiftU64 (((UINT64)(DramLimitAddr.HyGxField.DramLimitAddr + 1 - DramBaseAddr.Field.DramBaseAddr)), 28);
      MemoryBase   = LShiftU64 (((UINT64)(DramBaseAddr.Field.DramBaseAddr)), 28);

      if (DramBaseAddr.Field.DramBaseAddr == 0) {
        CreateCratMemoryEntry (CratHeaderStructPtr, TableEnd, Domain, 0ull, 0xA0000ull);
        MemoryBase    = (1 * 1024 * 1024);
        MemoryLength -= MemoryBase;
      }

      if (DramBaseAddr.Field.LgcyMmioHoleEn == 1) {
        DramHoleCtrl.Value = FabricRegisterAccRead (0, 0, DRAMHOLECTRL_FUNC, DRAMHOLECTRL_REG, IOMS0_INSTANCE_ID);
        ASSERT (DramHoleCtrl.Field.DramHoleValid == 1);

        HoleBase = LShiftU64 (((UINT64)(DramHoleCtrl.Field.DramHoleBase)), 24);
        SizeBelowHole = HoleBase - MemoryBase;

        CreateCratMemoryEntry (CratHeaderStructPtr, TableEnd, Domain, MemoryBase, SizeBelowHole);

        MemoryBase   = 0x100000000;
        MemoryLength = LShiftU64 (((UINT64)(DramLimitAddr.HyGxField.DramLimitAddr + 1 - 0x10)), 28);
      }

      CreateCratMemoryEntry (CratHeaderStructPtr, TableEnd, Domain, MemoryBase, MemoryLength);
    }
  }

  return EFI_SUCCESS;
}

VOID
CreateCratMemoryEntry (
  IN       CRAT_HEADER  *CratHeaderStructPtr,
  IN OUT   UINT8   **TablePointer,
  IN       UINT32  Domain,
  IN       UINT64  RegionBase,
  IN       UINT64  RegionSize
  )
{
  CRAT_MEMORY  *MemoryEntry;

  MemoryEntry = (CRAT_MEMORY *) *TablePointer;
  *TablePointer += sizeof (CRAT_MEMORY);
  CratHeaderStructPtr->TotalEntries++;

  MemoryEntry->Type            = CRAT_MEMORY_TYPE;
  MemoryEntry->Length          = sizeof (CRAT_MEMORY);
  MemoryEntry->ProximityDomain = Domain;
  MemoryEntry->BaseAddressLow  = (UINT32) (RegionBase & 0xFFFFFFFF);
  MemoryEntry->BaseAddressHigh = (UINT32) (RShiftU64 (RegionBase, 32) & 0xFFFFFFFF);
  MemoryEntry->LengthLow       = (UINT32) (RegionSize & 0xFFFFFFFF);
  MemoryEntry->LengthHigh      = (UINT32) (RShiftU64 (RegionSize, 32) & 0xFFFFFFFF);
  MemoryEntry->Flags.Enabled   = 1;
  ///@todo get from memory protocol
  MemoryEntry->Width           = 64;
}
