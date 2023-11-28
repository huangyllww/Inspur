/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM OEM API, and related functions.
 *
 * Contains OEM code that defined CPM tables before CPM init.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  OEM
 * @e \$Revision: 270765 $   @e \$Date: 2013-08-15 16:35:40 -0400 (Thu, 15 Aug 2013) $
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
#include "AmdCpmOemInitPeim.h"
#include <Ppi/M24Lc128Ppi.h>
#include <Ppi/NbioPcieComplexPpi.h>

extern  AMD_CPM_PLATFORM_ID_CONVERT_TABLE         gCpmPlatformIdConvertTable;
extern  AMD_CPM_PRE_INIT_TABLE                    gCpmPreInitTable;
extern  AMD_CPM_GPIO_INIT_TABLE                   gCpmGpioInitTableHygonDM1SLT;
extern  AMD_CPM_GEVENT_INIT_TABLE                 gCpmGeventInitTableHygonDM1SLT;
extern  AMD_CPM_GPIO_DEVICE_CONFIG_TABLE          gCpmGpioDeviceConfigTableHygonDM1SLT;
extern  AMD_CPM_GPIO_DEVICE_DETECTION_TABLE       gCpmGpioDeviceDetectionTableHygonDM1SLT;
extern  AMD_CPM_GPIO_DEVICE_RESET_TABLE           gCpmGpioDeviceResetTableHygonDM1SLT;
extern  AMD_CPM_GPIO_DEVICE_POWER_TABLE           gCpmGpioDevicePowerTableHygonDM1SLT;
extern  AMD_CPM_PCIE_CLOCK_TABLE                  gCpmPcieClockTableHygonDM1SLT;
extern  AMD_CPM_DXIO_TOPOLOGY_TABLE               gCpmDxioTopologyTableHygonDM1SLTS0; 
extern  AMD_CPM_PLATFORM_ID_TABLE                 gCpmPlatformIdTable;

#define DxioInputParserGetNextDescriptor(Descriptor) (Descriptor != NULL ? ((((Descriptor->Flags & DESCRIPTOR_TERMINATE_LIST) != 0) ? NULL : (++Descriptor))) : NULL)

EFI_STATUS
EFIAPI
AmdXgbeMacAddr (
    IN CONST EFI_PEI_SERVICES     **PeiServices
  );

EFI_STATUS
EFIAPI
AmdCpmOemInitPeimNotifyCallback (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

#ifdef AGESA_V9
STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mCpmOemInitPeimNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiI2cMasterPpiGuid,
  AmdCpmOemInitPeimNotifyCallback
};
#endif

EFI_STATUS
EFIAPI
AmdCpmTableOverride (
  IN       EFI_PEI_SERVICES       **PeiServices
  );

EFI_STATUS
EFIAPI
AmdCpmOverrideTableNotifyCallback (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR mCpmOemTableOverrideNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCpmTablePpiGuid,
  AmdCpmOverrideTableNotifyCallback
};

//
// CPM Main Table
//
AMD_CPM_MAIN_TABLE gCpmMainTable = {
  {CPM_SIGNATURE_MAIN_TABLE, sizeof (gCpmMainTable) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  "HygonDM1SLT",   // PlatformName
  0xFF,         // BiosType
  1,            // CurrentPlatformId
  0,            // PcieMemIoBaseAddr
  0,            // AcpiMemIoBaseAddr
  NULL,         // Reserved for Internal Used
  NULL,         // Reserved for Internal Used
  NULL,         // Reserved for Internal Used
  NULL,         // Reserved for Internal Used
  NULL,         // Reserved for Internal Used
  0,            // ExtClkGen
  0,            // UnusedGppClkOffEn
  0             // Reserved
};

VOID *gCpmTableList[] = {
  &gCpmMainTable,
  &gCpmPlatformIdTable,
  &gCpmPlatformIdConvertTable,
  &gCpmPreInitTable,
  &gCpmGpioInitTableHygonDM1SLT,
  &gCpmGeventInitTableHygonDM1SLT,
  &gCpmGpioDeviceConfigTableHygonDM1SLT,
  &gCpmGpioDevicePowerTableHygonDM1SLT,
  &gCpmGpioDeviceDetectionTableHygonDM1SLT,
  &gCpmGpioDeviceResetTableHygonDM1SLT,
  &gCpmPcieClockTableHygonDM1SLT,
  &gCpmDxioTopologyTableHygonDM1SLTS0,

  NULL
};

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM OEM Init PEIM driver
 *
 * This function defines CPM OEM definition tables and installs AmdCpmOemTablePpi.
 * It also defines callback function to update these definition table on run time.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
AmdCpmOemInitPeimEntryPoint (
  IN       CPM_PEI_FILE_HANDLE      FileHandle,
  IN       CPM_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS              Status;
#ifndef AGESA_V9
  EFI_PEI_NOTIFY_DESCRIPTOR       NotifyDescriptorV9;
  VOID                            *PpiV9 = NULL;
#endif

#ifdef AGESA_V9
  Status = (**PeiServices).NotifyPpi (PeiServices, &mCpmOemInitPeimNotify);
#else
  Status = AmdCpmOemInitPeimNotifyCallback ((EFI_PEI_SERVICES**)PeiServices,
                               &NotifyDescriptorV9,
                               PpiV9);
#endif
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback Function of AMD CPM OEM Init PEIM driver
 *
 * @param[in]     PeiServices       The PEI core services table.
 * @param[in]     NotifyDescriptor  The descriptor for the notification event.
 * @param[in]     Ppi               Pointer to the PPI in question
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
AmdCpmOemInitPeimNotifyCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDescriptor,
  IN  VOID                            *Ppi
  )
{
  EFI_STATUS              Status;
  AMD_CPM_OEM_TABLE_PPI   *AmdCpmOemTablePpi;
  EFI_PEI_PPI_DESCRIPTOR  *PpiListCpmOemTable;

  Status = (*PeiServices)->LocatePpi (
                               (CPM_PEI_SERVICES**)PeiServices,
                               &gAmdCpmOemTablePpiGuid,
                               0,
                               NULL,
                               (VOID**)&AmdCpmOemTablePpi
                               );
  if (EFI_ERROR (Status)) {
    Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (AMD_CPM_OEM_TABLE_PPI),
                             (VOID**)&AmdCpmOemTablePpi
                             );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    AmdCpmOemTablePpi->Revision = AMD_CPM_OEM_REVISION;
    AmdCpmOemTablePpi->PlatformId = AMD_CPM_PLATFORM_ID_DEFAULT;
    AmdCpmOemTablePpi->TableList = &gCpmTableList[0];

    Status = (*PeiServices)->AllocatePool (
                               PeiServices,
                               sizeof (EFI_PEI_PPI_DESCRIPTOR),
                               (VOID**)&PpiListCpmOemTable
                               );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    PpiListCpmOemTable->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
    PpiListCpmOemTable->Guid = &gAmdCpmOemTablePpiGuid;
    PpiListCpmOemTable->Ppi = AmdCpmOemTablePpi;
    Status = (*PeiServices)->InstallPpi (
                                PeiServices,
                                PpiListCpmOemTable
                                );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = (**PeiServices).NotifyPpi (PeiServices, &mCpmOemTableOverrideNotify);
    //AmdXgbeMacAddr (PeiServices);
  }

  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback Function to Override CPM OEM Definition Tables
 *
 * @param[in]     PeiServices       The PEI core services table.
 * @param[in]     NotifyDescriptor  The descriptor for the notification event.
 * @param[in]     Ppi               Pointer to the PPI in question
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
AmdCpmOverrideTableNotifyCallback (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  )
{
  EFI_STATUS                            Status;
  AMD_CPM_OVERRIDE_TABLE_PPI            *AmdCpmOverrideTablePpi;
  EFI_PEI_PPI_DESCRIPTOR                *PpiListCpmOverrideTablePtr;

  Status = AmdCpmTableOverride (PeiServices);

  Status = (*PeiServices)->AllocatePool (
                              (CPM_PEI_SERVICES**)PeiServices,
                              sizeof (AMD_CPM_OVERRIDE_TABLE_PPI),
                              (VOID**)&AmdCpmOverrideTablePpi
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = (*PeiServices)->AllocatePool (
                              (CPM_PEI_SERVICES**)PeiServices,
                              sizeof (EFI_PEI_PPI_DESCRIPTOR),
                              (VOID**)&PpiListCpmOverrideTablePtr
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PpiListCpmOverrideTablePtr->Flags   = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiListCpmOverrideTablePtr->Guid    = &gAmdCpmOverrideTablePpiGuid;
  PpiListCpmOverrideTablePtr->Ppi     = AmdCpmOverrideTablePpi;

  Status = (*PeiServices)->InstallPpi (
                              (CPM_PEI_SERVICES**)PeiServices,
                              PpiListCpmOverrideTablePtr
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}

/**
 *
 * This function Programs MAC addresses to Ancillary Data Structure.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
AmdXgbeMacAddr (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_PEI_M24LC128_PPI            *M24Lc128Ppi;
  UINT8                            MacBuffer[24];
  UINT32                           Value;
  UINT8                           *MacPointer; 
  DXIO_PORT_DESCRIPTOR            *EngineDescriptor;
  DXIO_COMPLEX_DESCRIPTOR         *PcieTopologyData;
  PEI_AMD_NBIO_PCIE_COMPLEX_PPI   *NbioPcieComplexPpi;
  EFI_STATUS              Status = EFI_SUCCESS;
  
  DEBUG((EFI_D_ERROR, "Inside AmdXgbeMacAddr\n"));
  
  Status = (*PeiServices)->LocatePpi (PeiServices, &gM24Lc128PpiGuid, 0, NULL, &M24Lc128Ppi);
  Status = M24Lc128Ppi->Read (PeiServices, 5, 0x50, 0x4d, 24, MacBuffer);
  MacPointer = MacBuffer;
  Status = (*PeiServices)->LocatePpi (PeiServices, &gAmdNbioPcieComplexPpiGuid, 0, NULL, &NbioPcieComplexPpi);
  if (!EFI_ERROR (Status)) {
    NbioPcieComplexPpi->PcieGetComplex (NbioPcieComplexPpi, &PcieTopologyData);
  }
//  EngineDescriptor = ComplexDescriptor->PciePortList;
  EngineDescriptor = PcieTopologyData->PciePortList;
  while (EngineDescriptor != NULL) {
    if (EngineDescriptor->EngineData.EngineType == DxioEthernetEngine) {
      switch (EngineDescriptor->EngineData.StartLane % 32) {
      case 4:
        // Initialize this descriptor for Port 0
        Value = *((UINT32*) MacPointer);
        EngineDescriptor->EtherNet.MacAddressLo = Value;  //Read the low value of the mac address here...;
        MacPointer += 4;
        Value = *((UINT32*) MacPointer);
        Value &= 0xFFFF;
        Value |= 0x80000000;  // Set Valid
        EngineDescriptor->EtherNet.MacAddressHi = Value;  //Read the high value of the mac address here...;
        MacPointer += 2;
        break;
      case 5:
        // Initialize this descriptor for Port 1
          Value = *((UINT32*) MacPointer);
          EngineDescriptor->EtherNet.MacAddressLo = Value;
          MacPointer += 4;
          Value = *((UINT32*) MacPointer);
          Value &= 0xFFFF;
          Value |= 0x80000000;  // Set Valid
          EngineDescriptor->EtherNet.MacAddressHi = Value;
          MacPointer += 2;
        break;
      case 6:
        // Initialize this descriptor for Port 2
          Value = *((UINT32*) MacPointer);
          EngineDescriptor->EtherNet.MacAddressLo = Value;
          MacPointer += 4;
          Value = *((UINT32*) MacPointer);
          Value &= 0xFFFF;
          Value |= 0x80000000;  // Set Valid
          EngineDescriptor->EtherNet.MacAddressHi = Value;
          MacPointer += 2;
        break;
      case 7:
        // Initialize this descriptor for Port 3
          Value = *((UINT32*) MacPointer);
          EngineDescriptor->EtherNet.MacAddressLo = Value;
          MacPointer += 4;
          Value = *((UINT32*) MacPointer);
          Value &= 0xFFFF;
          Value |= 0x80000000;  // Set Valid
          EngineDescriptor->EtherNet.MacAddressHi = Value;
          MacPointer += 2;
        break;
      default:
        ASSERT (FALSE);  // Illegal lane address
        break;
      }
    }
    EngineDescriptor = DxioInputParserGetNextDescriptor (EngineDescriptor);
  } 
  return Status;
}
