/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPM OEM API, and related functions.
 *
 * Contains OEM code that defined CPM tables before CPM init.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  OEM
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

#include <HygonCpmPei.h>
#include "HygonCpmOemInitPeim.h"
#include <Ppi/NbioPcieComplexPpi.h>
#include <Library/HygonSocBaseLib.h>

//HyEx
extern  HYGON_CPM_PLATFORM_ID_CONVERT_TABLE    gCpmPlatformIdConvertTableHyEx;
extern  HYGON_CPM_PRE_INIT_TABLE               gCpmPreInitTableHyEx;
extern  HYGON_CPM_GPIO_INIT_TABLE              gCpmGpioInitTableNanHaiVtb3HyEx;
extern  HYGON_CPM_GEVENT_INIT_TABLE            gCpmGeventInitTableNanHaiVtb3HyEx;
extern  HYGON_CPM_GPIO_DEVICE_CONFIG_TABLE     gCpmGpioDeviceConfigTableNanHaiVtb3HyEx;
extern  HYGON_CPM_GPIO_DEVICE_DETECTION_TABLE  gCpmGpioDeviceDetectionTableNanHaiVtb3HyEx;
extern  HYGON_CPM_GPIO_DEVICE_RESET_TABLE      gCpmGpioDeviceResetTableNanHaiVtb3HyEx;
extern  HYGON_CPM_GPIO_DEVICE_POWER_TABLE      gCpmGpioDevicePowerTableNanHaiVtb3HyEx;
extern  HYGON_CPM_PCIE_CLOCK_TABLE             gCpmPcieClockTableNanHaiVtb3HyEx;
extern  HYGON_CPM_HSIO_TOPOLOGY_TABLE          gCpmHsioTopologyTableNanHaiVtb3S0HyEx;
extern  HYGON_CPM_PLATFORM_ID_TABLE            gCpmPlatformIdTableHyEx;
extern  HYGON_CPM_DIMM_MAP_TABLE               gCpmDimmMapTableHyEx;
extern  HYGON_CPM_EQ_CONFIG_TABLE              gCpmEqConfigurationTableNanHaiVtb3HyEx;

//HyGx
extern  HYGON_CPM_PLATFORM_ID_CONVERT_TABLE    gCpmPlatformIdConvertTableHyGx;
extern  HYGON_CPM_PRE_INIT_TABLE               gCpmPreInitTableHyGx;
extern  HYGON_CPM_GPIO_INIT_TABLE              gCpmGpioInitTableNanHaiVtb3HyGx;
extern  HYGON_CPM_GEVENT_INIT_TABLE            gCpmGeventInitTableNanHaiVtb3HyGx;
extern  HYGON_CPM_GPIO_DEVICE_CONFIG_TABLE     gCpmGpioDeviceConfigTableNanHaiVtb3HyGx;
extern  HYGON_CPM_GPIO_DEVICE_DETECTION_TABLE  gCpmGpioDeviceDetectionTableNanHaiVtb3HyGx;
extern  HYGON_CPM_GPIO_DEVICE_RESET_TABLE      gCpmGpioDeviceResetTableNanHaiVtb3HyGx;
extern  HYGON_CPM_GPIO_DEVICE_POWER_TABLE      gCpmGpioDevicePowerTableNanHaiVtb3HyGx;
extern  HYGON_CPM_PCIE_CLOCK_TABLE             gCpmPcieClockTableNanHaiVtb3HyGx;
extern  HYGON_CPM_HSIO_TOPOLOGY_TABLE          gCpmHsioTopologyTableNanHaiVtb3S0HyGx;
extern  HYGON_CPM_PLATFORM_ID_TABLE            gCpmPlatformIdTableHyGx;
extern  HYGON_CPM_DIMM_MAP_TABLE               gCpmDimmMapTableHyGx;

#define HsioInputParserGetNextDescriptor(Descriptor)  (Descriptor != \
                                                       NULL ? ((((Descriptor->Flags & DESCRIPTOR_TERMINATE_LIST) != \
                                                                 0) ? NULL : (++Descriptor))) : NULL)

EFI_STATUS
EFIAPI
HygonCpmOemInitPeimNotifyCallback (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mCpmOemInitPeimNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiI2cMasterPpiGuid,
  HygonCpmOemInitPeimNotifyCallback
};

EFI_STATUS
EFIAPI
HygonCpmOverrideTableNotifyCallback (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mCpmOemTableOverrideNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonCpmTablePpiGuid,
  HygonCpmOverrideTableNotifyCallback
};

//
// CPM Main Table
//
HYGON_CPM_MAIN_TABLE  gCpmMainTable = {
  { CPM_SIGNATURE_MAIN_TABLE, sizeof (gCpmMainTable) / sizeof (UINT8), 0, 0, 0, 0x0000000F },
  "NanHaiVtb3", // PlatformName
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

VOID  *gCpmTableListHyEx[] = {
  &gCpmMainTable,
  &gCpmPlatformIdTableHyEx,
  &gCpmPlatformIdConvertTableHyEx,
  &gCpmPreInitTableHyEx,
  &gCpmGpioInitTableNanHaiVtb3HyEx,
  &gCpmGeventInitTableNanHaiVtb3HyEx,
  &gCpmGpioDeviceConfigTableNanHaiVtb3HyEx,
  &gCpmGpioDevicePowerTableNanHaiVtb3HyEx,
  &gCpmGpioDeviceDetectionTableNanHaiVtb3HyEx,
  &gCpmGpioDeviceResetTableNanHaiVtb3HyEx,
  &gCpmPcieClockTableNanHaiVtb3HyEx,
  &gCpmHsioTopologyTableNanHaiVtb3S0HyEx,
  &gCpmDimmMapTableHyEx,
  &gCpmEqConfigurationTableNanHaiVtb3HyEx,
  NULL
};

VOID  *gCpmTableListHyGx[] = {
  &gCpmMainTable,
  &gCpmPlatformIdTableHyGx,
  &gCpmPlatformIdConvertTableHyGx,
  &gCpmPreInitTableHyGx,
  &gCpmGpioInitTableNanHaiVtb3HyGx,
  &gCpmGeventInitTableNanHaiVtb3HyGx,
  &gCpmGpioDeviceConfigTableNanHaiVtb3HyGx,
  &gCpmGpioDevicePowerTableNanHaiVtb3HyGx,
  &gCpmGpioDeviceDetectionTableNanHaiVtb3HyGx,
  &gCpmGpioDeviceResetTableNanHaiVtb3HyGx,
  &gCpmPcieClockTableNanHaiVtb3HyGx,
  &gCpmHsioTopologyTableNanHaiVtb3S0HyGx,
  &gCpmDimmMapTableHyGx,
  NULL
};

// byo230825 + >>
STATIC EFI_PEI_PPI_DESCRIPTOR gByoHygonCpmTableOverridePpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gByoHygonCpmTableOverrideHookPpiGuid,
  NULL
};
// byo230825 + <<

/*----------------------------------------------------------------------------------------*/

/**
 * Entry point of the HYGON CPM OEM Init PEIM driver
 *
 * This function defines CPM OEM definition tables and installs HygonCpmOemTablePpi.
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
HygonCpmOemInitPeimEntryPoint (
  IN       CPM_PEI_FILE_HANDLE      FileHandle,
  IN       CPM_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = (**PeiServices).NotifyPpi (PeiServices, &mCpmOemInitPeimNotify);

  return Status;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Callback Function of HYGON CPM OEM Init PEIM driver
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
HygonCpmOemInitPeimNotifyCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDescriptor,
  IN  VOID                            *Ppi
  )
{
  EFI_STATUS               Status;
  HYGON_CPM_OEM_TABLE_PPI  *HygonCpmOemTablePpi;
  EFI_PEI_PPI_DESCRIPTOR   *PpiListCpmOemTable;
  UINT32                    CpuModel;

  Status = (*PeiServices)->LocatePpi (
                             (CPM_PEI_SERVICES **)PeiServices,
                             &gHygonCpmOemTablePpiGuid,
                             0,
                             NULL,
                             (VOID **)&HygonCpmOemTablePpi
                             );
  if (EFI_ERROR (Status)) {
    Status = (*PeiServices)->AllocatePool (
                               PeiServices,
                               sizeof (HYGON_CPM_OEM_TABLE_PPI),
                               (VOID **)&HygonCpmOemTablePpi
                               );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    HygonCpmOemTablePpi->Revision   = HYGON_CPM_OEM_REVISION;
    HygonCpmOemTablePpi->PlatformId = HYGON_CPM_PLATFORM_ID_DEFAULT;
    CpuModel = GetHygonSocModel();
    if (CpuModel == HYGON_EX_CPU) {
      HygonCpmOemTablePpi->TableList  = &gCpmTableListHyEx[0];
    } else {
      HygonCpmOemTablePpi->TableList  = &gCpmTableListHyGx[0];
    }

    Status = (*PeiServices)->AllocatePool (
                               PeiServices,
                               sizeof (EFI_PEI_PPI_DESCRIPTOR),
                               (VOID **)&PpiListCpmOemTable
                               );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    PpiListCpmOemTable->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
    PpiListCpmOemTable->Guid  = &gHygonCpmOemTablePpiGuid;
    PpiListCpmOemTable->Ppi   = HygonCpmOemTablePpi;
    Status = (*PeiServices)->InstallPpi (
                               PeiServices,
                               PpiListCpmOemTable
                               );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = (**PeiServices).NotifyPpi (PeiServices, &mCpmOemTableOverrideNotify);
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
HygonCpmOverrideTableNotifyCallback (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  )
{
  EFI_STATUS                    Status;
  HYGON_CPM_OVERRIDE_TABLE_PPI  *HygonCpmOverrideTablePpi;
  EFI_PEI_PPI_DESCRIPTOR        *PpiListCpmOverrideTablePtr;

  Status = (*PeiServices)->InstallPpi(PeiServices, &gByoHygonCpmTableOverridePpiList);  // byo230825 +

  Status = (*PeiServices)->AllocatePool (
                             (CPM_PEI_SERVICES **)PeiServices,
                             sizeof (HYGON_CPM_OVERRIDE_TABLE_PPI),
                             (VOID **)&HygonCpmOverrideTablePpi
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = (*PeiServices)->AllocatePool (
                             (CPM_PEI_SERVICES **)PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             (VOID **)&PpiListCpmOverrideTablePtr
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PpiListCpmOverrideTablePtr->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiListCpmOverrideTablePtr->Guid  = &gHygonCpmOverrideTablePpiGuid;
  PpiListCpmOverrideTablePtr->Ppi   = HygonCpmOverrideTablePpi;

  Status = (*PeiServices)->InstallPpi (
                             (CPM_PEI_SERVICES **)PeiServices,
                             PpiListCpmOverrideTablePtr
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}
