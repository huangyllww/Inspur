/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdNbioPcieZPDxe Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioPcieZPDxe
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
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
#include <PiDxe.h>
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
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
#include <Protocol/AmdNbioPcieServicesProtocol.h>
#include <Protocol/SocLogicalIdProtocol.h>
#include <IdsHookId.h>
#include <Library/AmdIdsHookLib.h>
#include <CcxRegistersZp.h>

#define FILECODE        NBIO_IOMMU_AMDNBIOIOMMUZPDXE_AMDNBIOIOMMUZPDXE_FILECODE

VOID
IommuMsixWorkaround (
  IN       GNB_HANDLE  *GnbHandle
  );


/*----------------------------------------------------------------------------------------*/
/**
 * Enable IOMMU base address. (MMIO space )
 *
 *
 * @param[in]     GnbHandle       GNB handle
 * @param[in]     StdHeader       Standard Configuration Header
 * @retval        AGESA_SUCCESS
 * @retval        AGESA_ERROR
 */
AGESA_STATUS
STATIC
EnableIommuMmio (
  IN       GNB_HANDLE           *GnbHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  AGESA_STATUS            Status;
  UINT16                  CapabilityOffset;
  UINT64                  BaseAddress;
  UINT32                  Value;
  PCI_ADDR                GnbIommuPciAddress;

  Status = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEnableIommuMmio Enter\n");
  GnbIommuPciAddress = NbioGetHostPciAddress (GnbHandle);
  GnbIommuPciAddress.Address.Function = 0x2;
  if (GnbLibPciIsDevicePresent (GnbIommuPciAddress.AddressValue, StdHeader)) {
    CapabilityOffset = GnbLibFindPciCapability (GnbIommuPciAddress.AddressValue, IOMMU_CAP_ID, StdHeader);

    GnbLibPciRead (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x8), AccessWidth32, &Value, StdHeader);
    BaseAddress = (UINT64) Value << 32;
    GnbLibPciRead (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x4), AccessWidth32, &Value, StdHeader);
    BaseAddress |= Value;

    if ((BaseAddress & 0xfffffffffffffffe) != 0x0) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  Enable IOMMU MMIO at address %x for Socket %d Silicon %d\n", BaseAddress, GnbHandle->SocketId, GnbHandle->DieNumber);
      GnbLibPciRMW (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x8), AccessS3SaveWidth32, 0xFFFFFFFF, 0x0, StdHeader);
      GnbLibPciRMW (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x4), AccessS3SaveWidth32, 0xFFFFFFFE, 0x1, StdHeader);
    } else {
//      ASSERT (FALSE);
      Status = AGESA_ERROR;
      IDS_HDT_CONSOLE (GNB_TRACE, "No base address assigned - IOMMU disabled\n");
    }
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEnableIommuMmio Exit\n");
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
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  EFI_STATUS                    Status;
  AMD_SOC_LOGICAL_ID_PROTOCOL   *SocLogicalId;
  SOC_LOGICAL_ID                LogicalId;
  UINT64                        EfrAndMask;
  UINT64                        EfrOrMask;

  IDS_HDT_CONSOLE (MAIN_FLOW, "InstallIvrsAcpiTable Start\n");

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  if (EFI_ERROR (Status)) {
    return;
  }
  //
  // Create ACPI IVRS SSDT table
  //

//  Status = GnbIommuScratchMemoryRangeInterface ();


  EfrAndMask = 0xFFFFFFFFFFFFFFFF;
  EfrOrMask = 0;

  Status = gBS->LocateProtocol (&gAmdSocLogicalIdProtocolGuid, NULL, (VOID **) &SocLogicalId);
  ASSERT (!EFI_ERROR (Status));
  SocLogicalId->GetLogicalIdOnCurrentCore (SocLogicalId, &LogicalId);
  //For Dhyana family&model&stepping checking
  if (((LogicalId.Revision & (AMD_REV_F17_ZP_Ax | AMD_REV_F17_ZP_B0 | AMD_REV_F17_ZP_B1)) != 0) && (LogicalId.Family == AMD_FAMILY_17_ZP)) {
    EfrOrMask |= (1ull << 62);
  }

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
    DXE_AMD_NBIO_PCIE_SERVICES_PROTOCOL *PcieServicesProtocol;
    PCIe_PLATFORM_CONFIG                *Pcie;
    EFI_STATUS                          Status;
    GNB_HANDLE                          *GnbHandle;
    AMD_CONFIG_PARAMS                   *StdHeader;
    GNB_PCIE_INFORMATION_DATA_HOB       *PciePlatformConfigHobData;
    VOID                                *Registration;
    VOID                                *Interface;
    BOOLEAN                             IsIommuSupport;
    AMD_SOC_LOGICAL_ID_PROTOCOL         *SocLogicalId;
    SOC_LOGICAL_ID                      LogicalId;
    UINT32                               PackageType;

    Status = gBS->LocateProtocol (
            &gEfiPciEnumerationCompleteProtocolGuid,
            NULL,
            &Interface
    );
    if (EFI_ERROR (Status)) {
        return ;
    }
    IDS_HDT_CONSOLE (MAIN_FLOW, "NbioIOMMUDxeInitCallback Entry\n");

    GnbHandle = NULL;
    StdHeader = NULL;  ///@todo - do we need this?
    IsIommuSupport = FALSE;

    PackageType = LibAmdGetPackageType ((AMD_CONFIG_PARAMS *) NULL);
    Status = gBS->LocateProtocol (&gAmdSocLogicalIdProtocolGuid, NULL, (VOID **) &SocLogicalId);
    ASSERT (!EFI_ERROR (Status));
    if (EFI_ERROR (Status)) {
      SocLogicalId = (AMD_SOC_LOGICAL_ID_PROTOCOL *)NULL;
    }

    // Need topology structure
    Status = gBS->LocateProtocol (
                    &gAmdNbioPcieServicesProtocolGuid,
                    NULL,
                    &PcieServicesProtocol
                    );
    ASSERT (Status == EFI_SUCCESS);
    if (Status == EFI_SUCCESS) {
      PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **) &PciePlatformConfigHobData);
      Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
      GnbHandle = NbioGetHandle (Pcie);
      while (GnbHandle != NULL) {
        if (((1 << ZP_SOCKET_AM4) == PackageType) || (((1 << ZP_SOCKET_SP3r2) == PackageType) && (GnbHandle->InstanceId == 0))) {
          if (SocLogicalId != (AMD_SOC_LOGICAL_ID_PROTOCOL *)NULL) {
          SocLogicalId->GetLogicalIdOnCurrentCore(SocLogicalId, &LogicalId);
          //For Dhyana family&model&stepping checking
            if (((LogicalId.Revision & (AMD_REV_F17_ZP_Ax | AMD_REV_F17_ZP_B0 | AMD_REV_F17_ZP_B1)) != 0) && (LogicalId.Family == AMD_FAMILY_17_ZP)) {
              IommuMsixWorkaround (GnbHandle);
            }
          }
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
    //Make sure the hook ONLY called one time.
    if (Event != NULL) {
      gBS->CloseEvent (Event);
    }
}

EFI_STATUS
EFIAPI
AmdNbioIOMMUZPDxeEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                          Status = EFI_SUCCESS;
  EFI_EVENT                           Event;
  VOID                                *Registration;

  AGESA_TESTPOINT (TpNbioIommuDxeEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioIOMMUZPDxeEntry\n");

  Event = EfiCreateProtocolNotifyEvent (
                      &gEfiPciEnumerationCompleteProtocolGuid,
                      TPL_CALLBACK,
                      NbioIOMMUDxeInitCallback,
                      NULL,
                      &Registration
                      );
  if(Event == NULL)
  {
      IDS_HDT_CONSOLE (MAIN_FLOW, "ERROR: NbioIOMMUDxeInitCallback Create fail!!!!\n");
      Status = RETURN_NOT_READY;
  }
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioIOMMUZPDxeExit\n");
  AGESA_TESTPOINT (TpNbioIommuDxeExit, NULL);
  return Status;
}
