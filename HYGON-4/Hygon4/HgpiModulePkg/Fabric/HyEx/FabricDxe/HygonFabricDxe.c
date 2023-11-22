/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Satori API, and related functions.
 *
 * Contains code that initializes the core complex
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  CCX
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

#include <PiDxe.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Guid/HobList.h>
#include <Guid/EventGroup.h>
#include <Filecode.h>
#include <Library/DxeFabricTopologyServicesLib.h>
#include <Library/FabricResourceManagerLib.h>
#include <Library/FabricResourceSizeForEachRbLib.h>
#include <Protocol/SocLogicalIdProtocol.h>
#include <Protocol/FabricNumaServicesProtocol.h>
#include "FabricPiePwrMgmt.h"
#include "FabricAcpiTable.h"
#include "FabricReadyToBoot.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IdsLib.h>

#define FILECODE  FABRIC_HYEX_FABRICDXE_HYGONFABRICSTDXE_FILECODE

/*++
Module Name:

  HygonFabricStDxe.c
  Init Data Fabric interface

Abstract:
--*/

EFI_STATUS
EFIAPI
FabricStResourceManagerProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  );

EFI_STATUS
EFIAPI
FabricStAllocateMmio (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL *This,
  IN OUT   UINT64                     *BaseAddress,
  IN OUT   UINT64                     *Length,
  IN       UINT64                      Alignment,
  IN       FABRIC_TARGET               Target,
  IN OUT   FABRIC_MMIO_ATTRIBUTE      *Attributes
  );

EFI_STATUS
EFIAPI
FabricStAllocateIo (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL *This,
  IN OUT   UINT32                     *BaseAddress,
  IN OUT   UINT32                     *Length,
  IN       FABRIC_TARGET               Target
  );

EFI_STATUS
EFIAPI
FabricStGetAvailableResource (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL   *This,
  IN       FABRIC_RESOURCE_FOR_EACH_RB       *ResourceSizeForEachRb
  );

EFI_STATUS
EFIAPI
FabricStReallocateResourceForEachRb (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL  *This,
  IN       FABRIC_RESOURCE_FOR_EACH_RB      *ResourceSizeForEachRb,
  IN       FABRIC_ADDR_SPACE_SIZE            *SpaceStatus
  );

EFI_STATUS
EFIAPI
FabricStResourceRestoreDefault (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL     *This
  );

EFI_STATUS
EFIAPI
FabricStEnableVgaMmio (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL  *This,
  IN       FABRIC_TARGET          Target
  );

STATIC FABRIC_RESOURCE_MANAGER_PROTOCOL  mHygonFabricStResourceManager = {
  HYGON_FABRIC_RESOURCE_PROTOCOL_REV,
  FabricStAllocateMmio,
  FabricStAllocateIo,
  FabricStGetAvailableResource,
  FabricStReallocateResourceForEachRb,
  FabricStResourceRestoreDefault,
  FabricStEnableVgaMmio
};

/*++

Routine Description:

  Dharma - Satori Driver Entry.  Initialize the core complex.

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
HygonFabricHyExDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  UINT32                         NumaDomainCount;
  EFI_STATUS                     Status;
  EFI_EVENT                      ReadyToBootEvent;
  HYGON_SOC_LOGICAL_ID_PROTOCOL  *SocLogicalIdProtocol;
  FABRIC_NUMA_SERVICES_PROTOCOL  *FabricNuma;

  HGPI_TESTPOINT (TpDfDxeEntry, NULL);

  Status = EFI_SUCCESS;

  IDS_HDT_CONSOLE (CPU_TRACE, "  HygonFabricHyExDxeInit Entry\n");

  Status = gBS->LocateProtocol (
                  &gHygonSocLogicalIdProtocolGuid,
                  NULL,
                  &SocLogicalIdProtocol
                  );

  ASSERT (Status == EFI_SUCCESS);

  // Publish DXE topology services
  Status = FabricTopologyServiceProtocolInstall (ImageHandle, SystemTable);
  ASSERT (Status == EFI_SUCCESS);

  // Publish DF Resource Manager services
  Status = FabricStResourceManagerProtocolInstall (ImageHandle, SystemTable);
  ASSERT (Status == EFI_SUCCESS);

  // Publish NUMA services protocol
  Status = FabricStNumaServicesProtocolInstall (ImageHandle, SystemTable);
  ASSERT (Status == EFI_SUCCESS);

  if (gBS->LocateProtocol (&gHygonFabricNumaServicesProtocolGuid, NULL, (VOID **)&FabricNuma) == EFI_SUCCESS) {
    if (FabricNuma->GetDomainInfo (FabricNuma, &NumaDomainCount, NULL, NULL, NULL) == EFI_SUCCESS) {
      if (NumaDomainCount > 1) {
        IDS_HDT_CONSOLE (CPU_TRACE, "  Create NUMA ACPI table\n");
        // Publish ACPI Fabric SRAT services protocol
        Status = FabricStAcpiSratProtocolInstall (ImageHandle, SystemTable);
        ASSERT (Status == EFI_SUCCESS);

        // Publish ACPI Fabric MSCT services protocol
        Status = FabricStAcpiMsctProtocolInstall (ImageHandle, SystemTable);
        ASSERT (Status == EFI_SUCCESS);

        // Publish ACPI SLIT services protocol
        Status = FabricStAcpiSlitProtocolInstall (ImageHandle, SystemTable);
        ASSERT (Status == EFI_SUCCESS);
      }
    }

    // Publish ACPI Fabric CRAT services protocol
    Status = FabricStAcpiCratProtocolInstall (ImageHandle, SystemTable);
    ASSERT (Status == EFI_SUCCESS);

    // Publish ACPI CDIT services
    Status = FabricStAcpiCditProtocolInstall (ImageHandle, SystemTable);
    ASSERT (Status == EFI_SUCCESS);
  }

  // IDS_HDT_CONSOLE (CPU_TRACE, "    PIE power management\n");
  // FabricPiePwrMgmtInit (SocLogicalIdProtocol);

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  FabricReadyToBoot,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &ReadyToBootEvent
                  );
  ASSERT (Status == EFI_SUCCESS);

  IDS_HDT_CONSOLE (CPU_TRACE, "  HygonFabricHyExDxeInit End\n");

  HGPI_TESTPOINT (TpDfDxeExit, NULL);

  return (Status);
}

EFI_STATUS
EFIAPI
FabricStResourceManagerProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_HANDLE  Handle;
  EFI_STATUS  Status;

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gHygonFabricResourceManagerServicesProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mHygonFabricStResourceManager
                  );
  return Status;
}

/*---------------------------------------------------------------------------------------*/

/**
 * BSC entry point for allocate MMIO
 *
 * program MMIO base/limit/control registers
 *
 * @param[in]         This               Function pointer to FABRIC_RESOURCE_MANAGER_PROTOCOL.
 * @param[in, out]    BaseAddress        Starting address of the requested MMIO range.
 * @param[in, out]    Length             Length of the requested MMIO range.
 * @param[in]         Alignment          Alignment bit map.
 * @param[in]         Target             PCI bus number/Die number of the requestor.
 * @param[in, out]    Attributes         Attributes of the requested MMIO range indicating whether
 *                                       it is readable/writable/non-posted
 *
 * @retval            EFI_STATUS         EFI_OUT_OF_RESOURCES - The requested range could not be added because there are not
 *                                                              enough mapping resources.
 *                                       EFI_ABORTED          - One or more input parameters are invalid. For example, the
 *                                                              PciBusNumber does not correspond to any device in the system.
 *                                       EFI_SUCCESS          - Success to get an MMIO region
 */
EFI_STATUS
EFIAPI
FabricStAllocateMmio (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL *This,
  IN OUT   UINT64                           *BaseAddress,
  IN OUT   UINT64                           *Length,
  IN       UINT64                            Alignment,
  IN       FABRIC_TARGET                     Target,
  IN OUT   FABRIC_MMIO_ATTRIBUTE            *Attributes
  )
{
  EFI_STATUS  Status;

  Status = FabricAllocateMmio (BaseAddress, Length, Alignment, Target, Attributes);

  return Status;
}

/*---------------------------------------------------------------------------------------*/

/**
 * BSC entry point for for allocate IO
 *
 * program IO base/limit registers
 *
 * @param[in]         This               Function pointer to FABRIC_RESOURCE_MANAGER_PROTOCOL.
 * @param[in, out]    BaseAddress        Starting address of the requested MMIO range.
 * @param[in, out]    Length             Length of the requested MMIO range.
 * @param[in]         Target             PCI bus number/Die number of the requestor.
 *
 * @retval            EFI_STATUS         EFI_OUT_OF_RESOURCES - The requested range could not be added because there are not
 *                                                              enough mapping resources.
 *                                       EFI_ABORTED          - One or more input parameters are invalid. For example, the
 *                                                              PciBusNumber does not correspond to any device in the system.
 *                                       EFI_SUCCESS          - Success to get an IO region
 */
EFI_STATUS
EFIAPI
FabricStAllocateIo (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL *This,
  IN OUT   UINT32                           *BaseAddress,
  IN OUT   UINT32                           *Length,
  IN       FABRIC_TARGET                     Target
  )
{
  EFI_STATUS  Status;

  Status = FabricAllocateIo (BaseAddress, Length, Target);

  return Status;
}

/*---------------------------------------------------------------------------------------*/

/**
 * FabricStGetAvailableResource
 *
 * Get available DF resource (MMIO/IO) for each RB
 *
 * @param[in]         This                  Function pointer to FABRIC_RESOURCE_MANAGER_PROTOCOL.
 * @param[in, out]    ResourceSizeForEachRb Avaiable DF resource (MMIO/IO) for each RB
 *
 * @retval            EFI_SUCCESS           Success to get available resource
 *                    EFI_ABORTED           Can't get information of MMIO or IO
 */
EFI_STATUS
EFIAPI
FabricStGetAvailableResource (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL  *This,
  IN       FABRIC_RESOURCE_FOR_EACH_RB       *ResourceSizeForEachRb
  )
{
  EFI_STATUS  Status;

  Status = FabricGetAvailableResource (ResourceSizeForEachRb);

  return Status;
}

/*---------------------------------------------------------------------------------------*/

/**
 * FabricStReallocateResourceForEachRb
 *
 * ResourceSize = UsedSize + UserRequestSize (input)
 * Save ResourceSize to NV variable
 *
 * @param[in]         This                      Function pointer to FABRIC_RESOURCE_MANAGER_PROTOCOL.
 * @param[in, out]    ResourceSizeForEachRb     Avaiable DF resource (MMIO/IO) size for each RB
 * @param[in, out]    SpaceStatus               Current status
 *
 * @retval            EFI_SUCCESS               Save user's request to NV variable successfully
 *                    EFI_OUT_OF_RESOURCES      No enough resource
 */
EFI_STATUS
EFIAPI
FabricStReallocateResourceForEachRb (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL  *This,
  IN       FABRIC_RESOURCE_FOR_EACH_RB       *ResourceSizeForEachRb,
  IN       FABRIC_ADDR_SPACE_SIZE            *SpaceStatus
  )
{
  EFI_STATUS  Status;

  Status = FabricReallocateResourceForEachRb (ResourceSizeForEachRb, SpaceStatus);

  return Status;
}

/*---------------------------------------------------------------------------------------*/

/**
 * FabricStResourceRestoreDefault
 *
 * Restore default MMIO/IO distribution strategy by clearing NV variable
 *
 * @retval            EFI_SUCCESS               Success to clear NV variable
 */
EFI_STATUS
EFIAPI
FabricStResourceRestoreDefault (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL     *This
  )
{
  FabricResourceRestoreDefault ();

  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/

/**
 * FabricStEnableVgaMmio
 *
 * Set VGA Enable register
 *
 * @param[in]         Target                    PCI bus number/Die number of the requestor.
 *
 * @retval            EFI_SUCCESS               Success to set VGA enable registers
 *                    EFI_ABORTED               Can't find destination
 */
EFI_STATUS
EFIAPI
FabricStEnableVgaMmio (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL  *This,
  IN       FABRIC_TARGET                      Target
  )
{
  EFI_STATUS  Status;

  Status = FabricEnableVgaMmio (Target);

  return Status;
}
