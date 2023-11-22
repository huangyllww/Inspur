/* $NoKeywords:$ */

/**
 * @file
 *
 * HygonNbioIOMMUDxe Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonNbioPcieSATDxe
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
#include <Filecode.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/GnbLib.h>
#include <Library/GnbPciLib.h>
#include <Library/GnbPciAccLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/PcieConfigLib.h>
#include <SocLogicalId.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Library/UefiLib.h>
#include <Protocol/AcpiTable.h>
#include <Library/NbioIommuIvrsLib.h>
#include <Protocol/HygonNbioPcieServicesProtocol.h>
#include <Protocol/SocLogicalIdProtocol.h>
#include <IdsHookId.h>
#include <Library/HygonIdsHookLib.h>
#include <CcxRegistersDm.h>
#include <Library/HygonSocBaseLib.h>

#define FILECODE  NBIO_IOMMU_HYGONNBIOIOMMUSTDXE_HYGONNBIOIOMMUSTDXE_FILECODE

/*----------------------------------------------------------------------------------------*/

/**
 * Enable IOMMU base address. (MMIO space )
 *
 *
 * @param[in]     GnbHandle       GNB handle
 * @param[in]     StdHeader       Standard Configuration Header
 * @retval        HGPI_SUCCESS
 * @retval        HGPI_ERROR
 */
HGPI_STATUS
STATIC
EnableIommuMmio (
  IN       GNB_HANDLE           *GnbHandle,
  IN       HYGON_CONFIG_PARAMS    *StdHeader
  )
{
  HGPI_STATUS  Status;
  UINT16       CapabilityOffset;
  UINT64       BaseAddress;
  UINT32       Value;
  PCI_ADDR     GnbIommuPciAddress;

  Status = HGPI_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEnableIommuMmio Enter\n");
  GnbIommuPciAddress = NbioGetHostPciAddress (GnbHandle);
  GnbIommuPciAddress.Address.Function = 0x2;
  if (GnbLibPciIsDevicePresent (GnbIommuPciAddress.AddressValue, StdHeader)) {
    CapabilityOffset = GnbLibFindPciCapability (GnbIommuPciAddress.AddressValue, IOMMU_CAP_ID, StdHeader);

    GnbLibPciRead (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x8), AccessWidth32, &Value, StdHeader);
    BaseAddress = (UINT64)Value << 32;
    GnbLibPciRead (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x4), AccessWidth32, &Value, StdHeader);
    BaseAddress |= Value;

    if ((BaseAddress & 0xfffffffffffffffe) != 0x0) {
      IDS_HDT_CONSOLE (
        GNB_TRACE,
        "  Enable IOMMU MMIO at address %x for Socket %d Die P%d L%d NBIO %d\n",
        BaseAddress,
        GnbHandle->SocketId,
        GnbHandle->PhysicalDieId,
        GnbHandle->LogicalDieId,
        GnbHandle->RbId
        );
      GnbLibPciRMW (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x8), AccessS3SaveWidth32, 0xFFFFFFFF, 0x0, StdHeader);
      GnbLibPciRMW (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x4), AccessS3SaveWidth32, 0xFFFFFFFE, 0x1, StdHeader);
    } else {
      // ASSERT (FALSE);
      Status = HGPI_ERROR;
      IDS_HDT_CONSOLE (GNB_TRACE, "No base address assigned - IOMMU disabled\n");
    }
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEnableIommuMmio Exit\n");
  return Status;
}

/**
 * Enable vIOMMU base address and VF control base address. (MMIO space )
 *
 *
 * @param[in]     GnbHandle       GNB handle
 * @param[in]     StdHeader       Standard Configuration Header
 * @retval        HGPI_SUCCESS
 * @retval        HGPI_ERROR
 */
HGPI_STATUS
STATIC
EnablevIommuMmio (
  IN       GNB_HANDLE           *GnbHandle,
  IN       HYGON_CONFIG_PARAMS    *StdHeader
  )
{
  HGPI_STATUS  Status;
  UINT16       CapabilityOffset;
  UINT64       BaseAddress;
  UINT32       Value;
  PCI_ADDR     GnbIommuPciAddress;

  Status = HGPI_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEnablevIommuMmio Enter\n");
  GnbIommuPciAddress = NbioGetHostPciAddress (GnbHandle);
  GnbIommuPciAddress.Address.Function = 0x2;
  if (GnbLibPciIsDevicePresent (GnbIommuPciAddress.AddressValue, StdHeader)) {
    CapabilityOffset = GnbLibFindPciCapability (GnbIommuPciAddress.AddressValue, VIOMMU_CAP_ID, StdHeader);
    if (CapabilityOffset == 0) {
      Status = HGPI_ERROR;
      IDS_HDT_CONSOLE (GNB_TRACE, "vIOMMU Cap Not Found\n");
    } else {
      // vIOMMU function base address
      GnbLibPciRead (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0xc), AccessWidth32, &Value, StdHeader);
      BaseAddress = (UINT64)Value << 32;
      GnbLibPciRead (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x8), AccessWidth32, &Value, StdHeader);
      BaseAddress |= Value;

      if ((BaseAddress & 0xfffffffffffffffe) != 0x0) {
        IDS_HDT_CONSOLE (
          GNB_TRACE,
          "  Enable vIOMMU MMIO at address 0x%llx for Socket %d Die P%d L%d NBIO %d\n",
          BaseAddress,
          GnbHandle->SocketId,
          GnbHandle->PhysicalDieId,
          GnbHandle->LogicalDieId,
          GnbHandle->RbId
          );
        GnbLibPciRMW (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0xc), AccessS3SaveWidth32, 0xFFFFFFFF, 0x0, StdHeader);
        GnbLibPciRMW (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x8), AccessS3SaveWidth32, 0xFFFFFFFE, 0x1, StdHeader);
      } else {
        // ASSERT (FALSE);
        Status = HGPI_ERROR;
        IDS_HDT_CONSOLE (GNB_TRACE, "No base address assigned - vIOMMU base disabled\n");
      }

      // vIOMMU function control base address
      GnbLibPciRead (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x14), AccessWidth32, &Value, StdHeader);
      BaseAddress = (UINT64)Value << 32;
      GnbLibPciRead (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x10), AccessWidth32, &Value, StdHeader);
      BaseAddress |= Value;

      if ((BaseAddress & 0xfffffffffffffffe) != 0x0) {
        IDS_HDT_CONSOLE (
          GNB_TRACE,
          "  Enable vIOMMU Control MMIO at address %x for Socket %d Die P%d L%d NBIO %d\n",
          BaseAddress,
          GnbHandle->SocketId,
          GnbHandle->PhysicalDieId,
          GnbHandle->LogicalDieId,
          GnbHandle->RbId
          );
        GnbLibPciRMW (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x14), AccessS3SaveWidth32, 0xFFFFFFFF, 0x0, StdHeader);
        GnbLibPciRMW (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x10), AccessS3SaveWidth32, 0xFFFFFFFE, 0x1, StdHeader);
      } else {
        // ASSERT (FALSE);
        Status = HGPI_ERROR;
        IDS_HDT_CONSOLE (GNB_TRACE, "No control address assigned - vIOMMU control base disabled\n");
      }
    }
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEnablevIommuMmio Exit\n");
  return Status;
}

/**
 *---------------------------------------------------------------------------------------
 *  InstallIvrsAcpiTable
 *
 *  Description:
 *     notification event handler for install Ivrs Acpi Table
 *  Parameters:
 *    @param[in]     Event      Event whose notification function is being invoked.
 *    @param[in]     *Context   Pointer to the notification function's context.
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
EFIAPI
InstallIvrsAcpiTable (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_ACPI_TABLE_PROTOCOL        *AcpiTable;
  EFI_STATUS                     Status;
  HYGON_SOC_LOGICAL_ID_PROTOCOL  *SocLogicalId;
  SOC_LOGICAL_ID                 LogicalId;
  UINT64                         EfrAndMask;
  UINT64                         EfrOrMask;

  IDS_HDT_CONSOLE (MAIN_FLOW, "InstallIvrsAcpiTable Start\n");

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  if (EFI_ERROR (Status)) {
    return;
  }

  // Create ACPI IVRS SSDT table

  EfrAndMask = 0xFFFFFFFFFFFFFFFF;
  EfrOrMask  = 0;

  Status = gBS->LocateProtocol (&gHygonSocLogicalIdProtocolGuid, NULL, (VOID **)&SocLogicalId);
  ASSERT (!EFI_ERROR (Status));
  SocLogicalId->GetLogicalIdOnCurrentCore (SocLogicalId, &LogicalId);

  GnbIommuIvrsTable (EfrAndMask, EfrOrMask);
  //
  // Close event, so it will not be invoked again.
  //
  gBS->CloseEvent (Event);
  IDS_HDT_CONSOLE (MAIN_FLOW, "InstallIvrsAcpiTable End\n");
}

/**
 *---------------------------------------------------------------------------------------
 *  NbioIOMMUDxeInitCallback
 *
 *  Description:
 *
 *  Parameters:
 *    @param[in]     Event      Event whose notification function is being invoked.
 *    @param[in]     *Context   Pointer to the notification function's context.
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
EFIAPI
NbioIOMMUDxeInitCallback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL  *PcieServicesProtocol;
  PCIe_PLATFORM_CONFIG                   *Pcie;
  EFI_STATUS                             Status;
  GNB_HANDLE                             *GnbHandle;
  HYGON_CONFIG_PARAMS                    *StdHeader;
  GNB_PCIE_INFORMATION_DATA_HOB          *PciePlatformConfigHobData;
  VOID                                   *Registration;
  VOID                                   *Interface;
  BOOLEAN                                IsIommuSupport;
  HYGON_SOC_LOGICAL_ID_PROTOCOL          *SocLogicalId;
  UINT32                                 HygonCpuModel;

  Status = gBS->LocateProtocol (
                  &gEfiPciEnumerationCompleteProtocolGuid,
                  NULL,
                  &Interface
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioIOMMUDxeInitCallback Entry\n");

  GnbHandle = NULL;
  StdHeader = NULL;    /// @todo - do we need this?
  IsIommuSupport = FALSE;

  Status = gBS->LocateProtocol (&gHygonSocLogicalIdProtocolGuid, NULL, (VOID **)&SocLogicalId);
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    SocLogicalId = (HYGON_SOC_LOGICAL_ID_PROTOCOL *)NULL;
  }

  // Need topology structure
  Status = gBS->LocateProtocol (
                  &gHygonNbioPcieServicesProtocolGuid,
                  NULL,
                  &PcieServicesProtocol
                  );
  ASSERT (Status == EFI_SUCCESS);
  if (Status == EFI_SUCCESS) {
    PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **)&PciePlatformConfigHobData);
    Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
    GnbHandle = NbioGetHandle (Pcie);
    while (GnbHandle != NULL) {
      if (!GnbHandle->IohubPresent) {
        GnbHandle = GnbGetNextHandle (GnbHandle);
        continue;
      }

      HygonCpuModel = GetHygonSocModel();
      if (HygonCpuModel >= HYGON_GX_CPU) {
        Status = EnablevIommuMmio (GnbHandle, StdHeader);
      }
      Status = EnableIommuMmio (GnbHandle, StdHeader);
      if (Status == EFI_SUCCESS) {
        IsIommuSupport = TRUE;
      }

      GnbHandle = GnbGetNextHandle (GnbHandle);
    }

    IDS_HOOK (IDS_HOOK_NBIO_AFTER_CCX, NULL, (void *)NbioGetHandle (Pcie));
  }

  if (PcdGetBool (PcdCfgIommuSupport) && PcdGetBool (PcdIvrsControl) && IsIommuSupport) {
    EfiCreateProtocolNotifyEvent (&gEfiAcpiTableProtocolGuid, TPL_CALLBACK, InstallIvrsAcpiTable, NULL, &Registration);
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioIOMMUDxeInitCallback Exit\n");
  // Make sure the hook ONLY called one time.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }
}

EFI_STATUS
EFIAPI
HygonNbioIOMMUDxeEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_EVENT   Event;
  VOID        *Registration;

  HGPI_TESTPOINT (TpNbioIommuDxeEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioIOMMUDxeEntry\n");

  Event = EfiCreateProtocolNotifyEvent (
            &gEfiPciEnumerationCompleteProtocolGuid,
            TPL_CALLBACK,
            NbioIOMMUDxeInitCallback,
            NULL,
            &Registration
            );
  if(Event == NULL) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "ERROR: NbioIOMMUDxeInitCallback Create fail!!!!\n");
    Status = RETURN_NOT_READY;
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioIOMMUDxeExit\n");
  HGPI_TESTPOINT (TpNbioIommuDxeExit, NULL);
  return Status;
}
