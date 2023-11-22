/* $NoKeywords:$ */

/**
 * @file
 *
 * NBIO ALIB
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: NBIO
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/HygonBaseLib.h>
#include <GnbHsio.h>
#include <Filecode.h>
#include <Library/PcieAlibV2Lib.h>
#include <Library/GnbLib.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/HygonNbioPcieServicesProtocol.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Protocol/HygonNbioAlibServicesProtocol.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/GnbHeapLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <cpuRegisters.h>
#include <GnbFuseTable.h>
#define FILECODE  NBIO_NBIOALIB_HYGONNBIOALIBSTDXE_HYGONNBIOALIBCALLBACK_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
HGPI_STATUS
PcieAlibUpdateGnbData (
  IN OUT   VOID                   *SsdtBuffer,
  IN       HYGON_CONFIG_PARAMS      *StdHeader
  );

VOID
STATIC
PcieAlibUpdatePciePortDataCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  );

/*----------------------------------------------------------------------------------------*/

/**
 * Checksum Acpi Table
 *
 *
 *
 * @param[in] Table               Acpi Table
 * @param[in] StdHeader           Standard configuration header
 * @retval    HGPI_STATUS
 */
VOID
PcieAlibChecksumAcpiTable (
  IN OUT   ACPI_TABLE_HEADER *Table,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT8   *BuffTempPtr;
  UINT8   Checksum;
  UINT32  BufferOffset;

  Table->Checksum = 0;
  Checksum    = 0;
  BuffTempPtr = (UINT8 *)Table;
  for (BufferOffset = 0; BufferOffset < Table->TableLength; BufferOffset++) {
    Checksum = Checksum - *(BuffTempPtr + BufferOffset);
  }

  Table->Checksum = Checksum;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Update PCIe info
 *
 *
 *
 *
 * @param[in] DataBuffer          Ponter to data buffer
 */
HGPI_STATUS
PcieAlibUpdatePcieData (
  IN OUT   VOID                  *DataBuffer
  )
{
  EFI_STATUS                             Status;
  HGPI_STATUS                            HgpiStatus;
  PCIe_PLATFORM_CONFIG                   *Pcie;
  DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL  *PcieServicesProtocol;
  GNB_PCIE_INFORMATION_DATA_HOB          *PciePlatformConfigHobData;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibUpdatePcieData Enter\n");
  ASSERT (DataBuffer != NULL);
  HgpiStatus = HGPI_SUCCESS;

  //
  // Locate PCIe platform config
  //
  Status = gBS->LocateProtocol (
                  &gHygonNbioPcieServicesProtocolGuid,
                  NULL,
                  &PcieServicesProtocol
                  );

  if (Status == EFI_SUCCESS) {
    PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **)&PciePlatformConfigHobData);
    Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
    //
    // Update policy data
    //
    ((ALIB_DATA *)DataBuffer)->Data.Data.PsppPolicy = Pcie->PsppPolicy;
    IDS_HDT_CONSOLE (GNB_TRACE, "  PsppPolicy = %x\n", Pcie->PsppPolicy);
    //
    // Update data for each port
    //
    PcieConfigRunProcForAllEngines (
      DESCRIPTOR_PCIE_ENGINE,
      PcieAlibUpdatePciePortDataCallback,
      DataBuffer,
      Pcie
      );
  } else {
    ASSERT (FALSE);
    HgpiStatus = HGPI_FATAL;
  }

  return HgpiStatus;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Build ALIB ACPI table
 *
 *
 *
 * @param[in,out] AlibSsdtPtr     Pointer to pointer to ALIB SSDT table
 * @retval        HGPI_SUCCESS
 * @retval        HGPI_ERROR
 */
HGPI_STATUS
PcieAlibBuildAcpiTable (
  OUT   VOID                 **AlibSsdtPtr
  )
{
  HGPI_STATUS                            Status;
  HGPI_STATUS                            HgpiStatus;
  VOID                                   *AlibSsdtBuffer;
  VOID                                   *AlibSsdtTable;
  UINTN                                  AlibSsdtlength;
  UINT32                                 AmlObjName;
  VOID                                   *AmlObjPtr;
  EFI_ACPI_TABLE_PROTOCOL                *AcpiTable;
  UINTN                                  TableHandle;
  DXE_HYGON_NBIO_ALIB_SERVICES_PROTOCOL  *NbioAlibServiceProtocol;

  TableHandle = 0;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibBuildAcpiTable Enter\n");
  HgpiStatus = HGPI_SUCCESS;

  Status = gBS->LocateProtocol (&gHygonNbioALibServicesProtocolGuid, NULL, (VOID **)&NbioAlibServiceProtocol);
  NbioAlibServiceProtocol->AlibGetConfig (NbioAlibServiceProtocol, &AlibSsdtTable);

  AlibSsdtlength = ((ACPI_TABLE_HEADER *)AlibSsdtTable)->TableLength;
  if (*AlibSsdtPtr == NULL) {
    AlibSsdtBuffer = GnbAllocateHeapBuffer (
                       HYGON_ACPI_ALIB_BUFFER_HANDLE,
                       AlibSsdtlength,
                       (HYGON_CONFIG_PARAMS *)NULL
                       );
    ASSERT (AlibSsdtBuffer != NULL);
    if (AlibSsdtBuffer == NULL) {
      return HGPI_ERROR;
    }

    *AlibSsdtPtr = AlibSsdtBuffer;
  } else {
    AlibSsdtBuffer = *AlibSsdtPtr;
  }

  // Check length of port data
  ASSERT (sizeof (_ALIB_PORT_DATA) <= 20);
  // Check length of global data
  ASSERT (sizeof (_ALIB_GLOBAL_DATA) <= 32);
  // Copy template to buffer
  LibHygonMemCopy (AlibSsdtBuffer, AlibSsdtTable, AlibSsdtlength, (HYGON_CONFIG_PARAMS *)NULL);
  // Update table OEM fields.
  LibHygonMemCopy (
    (VOID *)&((ACPI_TABLE_HEADER *)AlibSsdtBuffer)->OemId,
    (VOID *)PcdGetPtr (PcdHygonAcpiTableHeaderOemId),
    AsciiStrnLenS (PcdGetPtr (PcdHygonAcpiTableHeaderOemId), 6),
    (HYGON_CONFIG_PARAMS *)NULL
    );
  LibHygonMemCopy (
    (VOID *)&((ACPI_TABLE_HEADER *)AlibSsdtBuffer)->OemTableId,
    (VOID *)PcdGetPtr (PcdHygonAcpiAlibSsdtTableHeaderOemTableId),
    AsciiStrnLenS (PcdGetPtr (PcdHygonAcpiAlibSsdtTableHeaderOemTableId), 8),
    (HYGON_CONFIG_PARAMS *)NULL
    );
  //
  // Update register base base
  //
  Status = PcieAlibUpdateGnbData (AlibSsdtBuffer, (HYGON_CONFIG_PARAMS *)NULL);
  // Returns HGPI_ERROR if STUB is installed.  This condition is ok.
  if (Status == HGPI_SUCCESS) {
    //
    // Update transfer block
    //
    AmlObjName = STRING_TO_UINT32 ('A', 'D', 'A', 'T');
    AmlObjPtr  = GnbLibFind (AlibSsdtBuffer, AlibSsdtlength, (UINT8 *)&AmlObjName, sizeof (AmlObjName));
    if (AmlObjPtr != NULL) {
      AmlObjPtr = (UINT8 *)AmlObjPtr + 10;
    }

    // Dispatch function from table
    Status = PcieAlibUpdatePcieData (AmlObjPtr);

    HGPI_STATUS_UPDATE (Status, HgpiStatus);
    if (HgpiStatus != HGPI_SUCCESS) {
      // Shrink table length to size of the header
      ((ACPI_TABLE_HEADER *)AlibSsdtBuffer)->TableLength = sizeof (ACPI_TABLE_HEADER);
    }
  }

  PcieAlibChecksumAcpiTable ((ACPI_TABLE_HEADER *)AlibSsdtBuffer, (HYGON_CONFIG_PARAMS *)NULL);

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = AcpiTable->InstallAcpiTable (AcpiTable, AlibSsdtBuffer, ((ACPI_TABLE_HEADER *)AlibSsdtBuffer)->TableLength, &TableHandle);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibBuildAcpiTable Exit [0x%x]\n", HgpiStatus);

  return HgpiStatus;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Update MMIO info
 *
 *
 *
 *
 * @param[in] SsdtBuffer          Pointer to SSDT table
 * @param[in] StdHeader           Standard configuration header
 */
HGPI_STATUS
PcieAlibUpdateGnbData (
  IN OUT   VOID                  *SsdtBuffer,
  IN       HYGON_CONFIG_PARAMS     *StdHeader
  )
{
  UINT64  LocalMsrRegister;
  UINT32  AmlObjName;
  VOID    *AmlObjPtr;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibUpdateGnbData Enter\n");
  //
  // Locate Base address variable
  //
  AmlObjName = STRING_TO_UINT32 ('A', 'G', 'R', 'B');
  AmlObjPtr  = GnbLibFind (
                 SsdtBuffer,
                 ((ACPI_TABLE_HEADER *)SsdtBuffer)->TableLength,
                 (UINT8 *)&AmlObjName,
                 sizeof (AmlObjName)
                 );
  // ASSERT (AmlObjPtr != NULL);
  if (AmlObjPtr == NULL) {
    return HGPI_ERROR;
  }

  //
  // Update PCIe MMIO base
  //
  LocalMsrRegister = AsmReadMsr64 (MSR_MMIO_Cfg_Base);
  if ((LocalMsrRegister & BIT0) != 0 && (LocalMsrRegister & 0xFFFFFFFF00000000) == 0) {
    *(UINT32 *)((UINT8 *)AmlObjPtr + 5) = (UINT32)(LocalMsrRegister & 0xFFFFF00000);
  } else {
    ASSERT (FALSE);
    return HGPI_ERROR;
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibUpdateGnbData Exit\n");
  return HGPI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Callback to update PCIe port data
 *
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          Not used
 * @param[in]       Pcie            Pointer to global PCIe configuration
 *
 */
VOID
STATIC
PcieAlibUpdatePciePortDataCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  _ALIB_PORT_DATA  *PortData;
  UINT32           BridgeNumber;
  GNB_HANDLE       *GnbHandle;

  if ((Engine == NULL) || (Pcie == NULL)) {
    IDS_HDT_CONSOLE (GNB_TRACE, "%a() - The Engine/Pcie parameter are invalid (NULL pointer)\n", __FUNCTION__);
    return;
  }

  GnbHandle    = (GNB_HANDLE *)PcieConfigGetParentSilicon (Engine);
  BridgeNumber = (GnbHandle->InstanceId * 16) + Engine->Type.Port.PortId;
  PortData     = &((ALIB_DATA *)Buffer)->PortData[BridgeNumber].PortData;

  if (PcieConfigIsEngineAllocated (Engine) && (Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled || PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS))) {
    //
    // Various speed capability
    //
    IDS_HDT_CONSOLE (GNB_TRACE, "  Engine->Type.Port.PcieBridgeId = %x\n", BridgeNumber);
    /// @todo for PSPP    PortData->PciePortMaxSpeed = (UINT8) PcieFmGetLinkSpeedCap (PCIE_PORT_GEN_CAP_MAX, Engine);
    /// @todo for PSPP    PortData->PciePortCurSpeed = (UINT8) PcieFmGetLinkSpeedCap (PCIE_PORT_GEN_CAP_BOOT, Engine);
    PortData->PciePortDcSpeed = PcieGen1;
    PortData->PciePortAcSpeed = PortData->PciePortMaxSpeed;
    if (Pcie->PsppPolicy == PsppBalanceLow) {
      PortData->PciePortAcSpeed = PcieGen1;
    }

    if (PcieConfigIsSbPcieEngine (Engine)) {
      PortData->PcieSbPort = 0x1;
      PortData->PciePortAcSpeed = PortData->PciePortMaxSpeed;
    }

    if (Engine->Type.Port.PortData.MiscControls.LinkSafeMode != 0) {
      PortData->PcieLinkSafeMode = 0x1;
      PortData->PcieLocalOverrideSpeed = Engine->Type.Port.PortData.MiscControls.LinkSafeMode;
    }

    IDS_HDT_CONSOLE (GNB_TRACE, "  PortData->PciePortMaxSpeed = %x\n", PortData->PciePortMaxSpeed);
    IDS_HDT_CONSOLE (GNB_TRACE, "  PortData->PciePortCurSpeed = %x\n", PortData->PciePortCurSpeed);
    IDS_HDT_CONSOLE (GNB_TRACE, "  PortData->PciePortDcSpeed = %x\n", PortData->PciePortDcSpeed);
    IDS_HDT_CONSOLE (GNB_TRACE, "  PortData->PciePortAcSpeed = %x\n", PortData->PciePortAcSpeed);

    //
    // various port capability
    //
    PortData->StartPhyLane  = (UINT8)Engine->EngineData.StartLane;
    PortData->EndPhyLane    = (UINT8)Engine->EngineData.EndLane;
    PortData->StartCoreLane = (UINT8)Engine->Type.Port.StartCoreLane;
    PortData->EndCoreLane   = (UINT8)Engine->Type.Port.EndCoreLane;
    PortData->PortId = Engine->Type.Port.PortId;
    PortData->LinkHotplug = Engine->Type.Port.PortData.LinkHotplug;
    PortData->PciDev = (UINT8)Engine->Type.Port.Address.Address.Device;
    PortData->PciFun = (UINT8)Engine->Type.Port.Address.Address.Function;
    IDS_HDT_CONSOLE (GNB_TRACE, "  PortData->StartPhyLane = %x\n", PortData->StartPhyLane);
    IDS_HDT_CONSOLE (GNB_TRACE, "  PortData->EndPhyLane = %x\n", PortData->EndPhyLane);
    IDS_HDT_CONSOLE (GNB_TRACE, "  PortData->StartCoreLane = %x\n", PortData->StartCoreLane);
    IDS_HDT_CONSOLE (GNB_TRACE, "  PortData->EndCoreLane = %x\n", PortData->EndCoreLane);
    IDS_HDT_CONSOLE (GNB_TRACE, "  PortData->PortId = %x\n", PortData->PortId);
    IDS_HDT_CONSOLE (GNB_TRACE, "  PortData->LinkHotplug = %x\n", PortData->LinkHotplug);
    IDS_HDT_CONSOLE (GNB_TRACE, "  PortData->PciDev = %x\n", PortData->PciDev);
    IDS_HDT_CONSOLE (GNB_TRACE, "  PortData->PciFun = %x\n", PortData->PciFun);
  } else {
    PortData->PciePortMaxSpeed = PcieGen1;
    PortData->PciePortCurSpeed = PcieGen1;
    PortData->PciePortDcSpeed  = PcieGen1;
    PortData->PciePortAcSpeed  = PcieGen1;
    PortData->PcieLocalOverrideSpeed = PcieGen1;
    IDS_HDT_CONSOLE (GNB_TRACE, "  Engine->Type.Port.PcieBridgeId = %x not allocated or non-hotplug\n", Engine->Type.Port.PcieBridgeId);
  }
}
