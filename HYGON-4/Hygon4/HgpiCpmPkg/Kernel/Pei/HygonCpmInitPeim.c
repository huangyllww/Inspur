/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPM POST API, and related functions.
 *
 * Contains code that initialized the CPM before memory init.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  Kernel
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
#include <HygonCpmTable.h>
#include <HygonCpmBaseIo.h>
#include <HygonCpmCpu.h>
#include <HygonCpmFch.h>
#include <Ppi/FabricTopologyServicesPpi.h>
#include <Library/HygonSocBaseLib.h>

VOID
EFIAPI
HygonCpmCoreTopologyInit (
  IN       HYGON_CPM_TABLE_PPI           *CpmTablePpiPtr
  );

VOID
EFIAPI
HygonCpmLoadPreInitTable (
  IN       VOID                        *This,
  IN       UINT8                       Stage
  );

VOID
EFIAPI
HygonCpmPlatformIdInit (
  IN       HYGON_CPM_TABLE_PPI           *CpmTablePpiPtr
  );

VOID
EFIAPI
HygonCpmPeiRegisterKernel (
  IN       VOID                        *This
  );

EFI_STATUS
EFIAPI
HygonCpmInitBeforeS3LateRestore (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );

EFI_STATUS
EFIAPI
HygonCpmInitPeimNotifyCallback (
  IN      EFI_PEI_SERVICES              **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN      VOID                          *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mCpmInitPeimNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonCpmOemTablePpiGuid,
  HygonCpmInitPeimNotifyCallback
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mNotifyListS3Restore =
{
  EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gCpmPeiMemoryDiscoveredPpiGuid,
  HygonCpmInitBeforeS3LateRestore
};

HYGON_CPM_CORE_TOPOLOGY_TABLE  gCpmCoreTopologyTableHyEx = {
  { HYEX_CPM_SIGNATURE_CORE_TOPOLOGY, sizeof (gCpmCoreTopologyTableHyEx) / sizeof (UINT8), 0, 0, 0, 0x0000000F },
};

HYGON_CPM_CORE_TOPOLOGY_TABLE  gCpmCoreTopologyTableHyGx = {
  { HYGX_CPM_SIGNATURE_CORE_TOPOLOGY, sizeof (gCpmCoreTopologyTableHyGx) / sizeof (UINT8), 0, 0, 0, 0x0000000F },
};

/*----------------------------------------------------------------------------------------*/

/**
 * CPM Init Function Before S3 Late Restore
 *
 * This function updates CPM Main Table Pointer in HYGON_CPM_TABLE_PPI Data Structure
 * after PEI MEMORY DISCOVERED PPI is installed.
 *
 * @param[in]     PeiServices    Pointer to Pei Services
 * @param[in]     NotifyDesc     The descriptor for the notification event
 * @param[in]     InvokePpi      Pointer to the PPI in question.
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 *
 */
EFI_STATUS
EFIAPI
HygonCpmInitBeforeS3LateRestore (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{
  EFI_STATUS               Status;
  HYGON_CPM_TABLE_PPI      *CpmTablePtr;
  HYGON_CPM_TABLE_HOB_PPI  *CpmTableHobPtr;

  Status = (*PeiServices)->LocatePpi (
                             (CPM_PEI_SERVICES **)PeiServices,
                             &gHygonCpmTablePpiGuid,
                             0,
                             NULL,
                             (VOID **)&CpmTablePtr
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = (*PeiServices)->LocatePpi (
                             (CPM_PEI_SERVICES **)PeiServices,
                             &gHygonCpmTableHobPpiGuid,
                             0,
                             NULL,
                             (VOID **)&CpmTableHobPtr
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CpmTablePtr->CommonFunction.RelocateTable (CpmTableHobPtr);

  CpmTablePtr->MainTablePtr = (HYGON_CPM_MAIN_TABLE *)((HYGON_CPM_HOB_HEADER *)CpmTableHobPtr)->MainTablePtr.Pointer;

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Entry point of the HYGON CPM Init PEIM driver
 *
 * This function gets CPM definition tables from HygonCpmOemTablePpi, reorgnizes the tables
 * and installs HygonCpmTablePpi. It also initializes the registers by load Pre Init Table
 * and update Current Platform Id at HygonCpmInitPeim.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
HygonCpmInitPeimEntryPoint (
  IN       CPM_PEI_FILE_HANDLE  FileHandle,
  IN       CPM_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = (**PeiServices).NotifyPpi (PeiServices, &mCpmInitPeimNotify);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Callback Function of the HYGON CPM Init PEIM driver
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
HygonCpmInitPeimNotifyCallback (
  IN      EFI_PEI_SERVICES              **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN      VOID                          *Ppi
  )
{
  EFI_STATUS                 Status;
  HYGON_CPM_OEM_TABLE_PPI    *HygonCpmOemTablePpiPtr;
  HYGON_CPM_TABLE_PPI        *HygonCpmTablePpiPtr;
  EFI_PEI_PPI_DESCRIPTOR     *PpiListCpmTablePtr;
  EFI_PEI_PPI_DESCRIPTOR     *PpiListCpmTableHobPtr;
  HYGON_CPM_MAIN_TABLE       *MainTablePtr;
  EFI_HOB_GUID_TYPE          *Hob;
  EFI_BOOT_MODE              BootMode;
  HYGON_CPM_TABLE_LIST       HobList;
  UINT8                      *HobBuffer;
  UINT8                      CpmBootMode;

  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonCpmOemTablePpiGuid,
                             0,
                             NULL,
                             (VOID **)&HygonCpmOemTablePpiPtr
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CpmGenerateTableList (HygonCpmOemTablePpiPtr->TableList, HygonCpmOemTablePpiPtr->PlatformId, &HobList);

  Status = (*PeiServices)->GetBootMode (
                             PeiServices,
                             &BootMode
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CpmBootMode = CpmGetBootMode ();

  if (CpmBootMode != CPM_BOOT_MODE_S3) {
    If_CpmBuildGuidHob (gHygonCpmTableHobGuid, Hob, NULL, HobList.Size + HYGON_TABLE_ADDITIONAL_BUFFER_SIZE, Status) {
      HobBuffer = (UINT8 *)Hob;
    } else {
      return EFI_NOT_FOUND;
    }
  } else {
    Status = (*PeiServices)->AllocatePool (
                               PeiServices,
                               HobList.Size + HYGON_TABLE_ADDITIONAL_BUFFER_SIZE,
                               (VOID **)&HobBuffer
                               );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = (*PeiServices)->AllocatePool (
                               PeiServices,
                               sizeof (EFI_PEI_PPI_DESCRIPTOR),
                               (VOID **)&PpiListCpmTableHobPtr
                               );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    PpiListCpmTableHobPtr->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
    PpiListCpmTableHobPtr->Guid  = &gHygonCpmTableHobPpiGuid;
    PpiListCpmTableHobPtr->Ppi   = HobBuffer;

    Status = (*PeiServices)->InstallPpi (
                               PeiServices,
                               PpiListCpmTableHobPtr
                               );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = (**PeiServices).NotifyPpi (PeiServices, &mNotifyListS3Restore);
  }

  MainTablePtr = CpmCopyTableListToMemory (&HobList, HobBuffer);

  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (HYGON_CPM_TABLE_PPI),
                             (VOID **)&HygonCpmTablePpiPtr
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  MainTablePtr->CurrentPlatformId = HygonCpmOemTablePpiPtr->PlatformId;
  MainTablePtr->Service.Pointer   = (VOID *)PeiServices;

  HygonCpmTablePpiPtr->Revision     = (HYGON_CPM_KERNEL_VERSION << 16) + HygonCpmOemTablePpiPtr->Revision;
  HygonCpmTablePpiPtr->MainTablePtr = MainTablePtr;

  ((HYGON_CPM_HOB_HEADER *)HobBuffer)->Revision = (UINT32)HygonCpmTablePpiPtr->Revision;

  HygonCpmPeiRegisterKernel (HygonCpmTablePpiPtr);
  CpmRegisterBaseIo (HygonCpmTablePpiPtr);
  CpmRegisterCpu (HygonCpmTablePpiPtr);
  CpmRegisterFch (HygonCpmTablePpiPtr);

  HygonCpmCoreTopologyInit (HygonCpmTablePpiPtr);
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             (VOID **)&PpiListCpmTablePtr
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PpiListCpmTablePtr->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiListCpmTablePtr->Guid  = &gHygonCpmTablePpiGuid;
  PpiListCpmTablePtr->Ppi   = HygonCpmTablePpiPtr;

  HygonCpmLoadPreInitTable (HygonCpmTablePpiPtr, CPM_PRE_INIT_STAGE_0);
  HygonCpmPlatformIdInit (HygonCpmTablePpiPtr);

  Status = (*PeiServices)->InstallPpi (
                             PeiServices,
                             PpiListCpmTablePtr
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Initialize Core Topology Table
 *
 * @param[in]     CpmTablePpiPtr Point to Ppi
 *
 */
VOID
EFIAPI
HygonCpmCoreTopologyInit (
  IN       HYGON_CPM_TABLE_PPI          *CpmTablePpiPtr
  )
{
  HYGON_PEI_FABRIC_TOPOLOGY_SERVICES_PPI         *FabricTopologyServicesPpi;
  HYGON_CPM_CORE_TOPOLOGY_TABLE                  *HygonCpmCoreTopologyPtr;
  EFI_STATUS                                     Status;
  UINTN                                          NumberOfProcessors;
  UINTN                                          TotalNumberOfDie;
  UINTN                                          TotalNumberOfPhysicalDie;
  UINTN                                          TotalNumberOfLogicalDie;
  UINTN                                          TotalNumberOfRootBridges;
  UINTN                                          SystemFabricID;
  UINTN                                          BusNumberBase;
  UINTN                                          BusNumberLimit;
  UINTN                                          SocketIndex;
  UINTN                                          DieIndex;
  UINTN                                          RootBridgeIndex;
  EFI_PEI_SERVICES                               **PeiServices;
  UINT32                                         CpuModel;

  HygonCpmCoreTopologyPtr = NULL;
  switch (CpmTablePpiPtr->ChipId.Sb) {
    case CPM_FCH_REVISION_ID_SAT:
    case CPM_FCH_REVISION_ID_RV:
      CpuModel = GetHygonSocModel();
      
      if (CpuModel == HYGON_EX_CPU) {
        HygonCpmCoreTopologyPtr = CpmTablePpiPtr->CommonFunction.AddTable (CpmTablePpiPtr, &gCpmCoreTopologyTableHyEx);
      } else if (CpuModel == HYGON_GX_CPU) {
        HygonCpmCoreTopologyPtr = CpmTablePpiPtr->CommonFunction.AddTable (CpmTablePpiPtr, &gCpmCoreTopologyTableHyGx);
      }
      
      ASSERT(HygonCpmCoreTopologyPtr != NULL);
      
      HygonCpmCoreTopologyPtr->CoreList[0].Socket = 0xFF;
      break;
    default:
      break;
  }

  if (HygonCpmCoreTopologyPtr) {
    PeiServices = (EFI_PEI_SERVICES **)CpmTablePpiPtr->MainTablePtr->Service.Pointer;
    // Get number of sockets from CPU topology PPI
    // Locate CPU topology PPI
    Status = (**PeiServices).LocatePpi (
                               PeiServices,
                               &gHygonFabricTopologyServicesPpiGuid,
                               0,
                               NULL,
                               &FabricTopologyServicesPpi
                               );
    FabricTopologyServicesPpi->GetSystemInfo (&NumberOfProcessors, &TotalNumberOfDie, &TotalNumberOfRootBridges);

    for (SocketIndex = 0; SocketIndex < NumberOfProcessors; SocketIndex++) {
      FabricTopologyServicesPpi->GetProcessorInfo (SocketIndex, &TotalNumberOfPhysicalDie, &TotalNumberOfLogicalDie, &TotalNumberOfRootBridges);
      for (DieIndex = 0, RootBridgeIndex = 0; DieIndex < TotalNumberOfLogicalDie; DieIndex++) {
        FabricTopologyServicesPpi->GetRootBridgeInfo (SocketIndex, DieIndex, RootBridgeIndex, &SystemFabricID, &BusNumberBase, &BusNumberLimit);
        // HygonCpmCoreTopologyPtr->CoreList[SocketIndex].Socket   = (UINT8) SocketIndex;
        // HygonCpmCoreTopologyPtr->CoreList[SocketIndex].Die      = (UINT8) DieIndex;
        // HygonCpmCoreTopologyPtr->CoreList[SocketIndex].Bus      = (UINT8) BusNumberBase;
        // HygonCpmCoreTopologyPtr->CoreList[SocketIndex+1].Socket = 0xFF;
        HygonCpmCoreTopologyPtr->CoreList[(SocketIndex * TotalNumberOfDie) + DieIndex].Socket = (UINT8)SocketIndex;
        HygonCpmCoreTopologyPtr->CoreList[(SocketIndex * TotalNumberOfDie) + DieIndex].Die    = (UINT8)DieIndex;
        HygonCpmCoreTopologyPtr->CoreList[(SocketIndex * TotalNumberOfDie) + DieIndex].Bus    = (UINT8)BusNumberBase;
      }
    }

    HygonCpmCoreTopologyPtr->CoreList[SocketIndex * TotalNumberOfDie].Socket = 0xFF;
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * The function to load Pre Init Table.
 *
 * @param[in]     This           Pointer to HYGON CPM TABLE Ppi
 * @param[in]     Stage          Stage number to initialize the registers
 *
 */
VOID
EFIAPI
HygonCpmLoadPreInitTable (
  IN       VOID                   *This,
  IN       UINT8                  Stage
  )
{
  HYGON_CPM_PRE_INIT_TABLE    *PreInitTablePtr;
  HYGON_CPM_GPIO_INIT_TABLE   *GpioPreInitTablePtr;
  HYGON_CPM_PRE_SETTING_ITEM  *TablePtr;
  HYGON_CPM_TABLE_PPI         *CpmTablePpiPtr;
  HYGON_CPM_GPIO_ITEM         *GpioTablePtr;
  UINT32                      CpuModel;

  CpmTablePpiPtr      = (HYGON_CPM_TABLE_PPI *)This;
  PreInitTablePtr     = NULL;
  GpioPreInitTablePtr = NULL;
  CpuModel            = GetHygonSocModel();
  
  if (CpuModel == HYGON_EX_CPU) {
    PreInitTablePtr     = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, HYEX_CPM_SIGNATURE_PRE_INIT);
    GpioPreInitTablePtr = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, HYEX_CPM_SIGNATURE_GPIO_PRE_INIT);
  } else if (CpuModel == HYGON_GX_CPU) {
    PreInitTablePtr     = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, HYGX_CPM_SIGNATURE_PRE_INIT);
    GpioPreInitTablePtr = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, HYGX_CPM_SIGNATURE_GPIO_PRE_INIT);
  }

  if (PreInitTablePtr) {
    TablePtr = &PreInitTablePtr->Item[0];
    while (TablePtr->Type != 0xFF ) {
      if (TablePtr->Stage == Stage) {
        switch (TablePtr->Type) {
          case 0:
            CpmTablePpiPtr->CommonFunction.MmioAndThenOr8 (
                                             CpmTablePpiPtr->MainTablePtr->AcpiMemIoBaseAddr + (TablePtr->Select << 0x08) + TablePtr->Offset,
                                             TablePtr->AndMask,
                                             TablePtr->OrMask
                                             );
            break;
          case 1:
          case 2:
            CpmTablePpiPtr->CommonFunction.PciAndThenOr8 (
                                             CpmTablePpiPtr,
                                             0x00,
                                             (TablePtr->Select & 0xF8) >> 3,
                                             TablePtr->Select & 7,
                                             TablePtr->Offset + (TablePtr->Type == 1 ? 0 : 0x100),
                                             TablePtr->AndMask,
                                             TablePtr->OrMask
                                             );
            break;
        }
      }

      TablePtr++;
    }
  }

  if (GpioPreInitTablePtr) {
    GpioTablePtr = &GpioPreInitTablePtr->GpioList[0];
    while (GpioTablePtr->Pin != 0xFF ) {
      CpmTablePpiPtr->CommonFunction.SetGpio (CpmTablePpiPtr, GpioTablePtr->Pin, GpioTablePtr->Setting.Raw);
      GpioTablePtr++;
    }
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Initialize Current Platform Id.
 *
 * @param[in]     CpmTablePpiPtr Point to Ppi
 *
 */
VOID
EFIAPI
HygonCpmPlatformIdInit (
  IN       HYGON_CPM_TABLE_PPI           *CpmTablePpiPtr
  )
{
  HYGON_CPM_PLATFORM_ID_TABLE          *PlatformIdTablePtr;
  HYGON_CPM_PLATFORM_ID_TABLE2         *PlatformIdTable2Ptr;
  HYGON_CPM_PLATFORM_ID_CONVERT_TABLE  *PlatformIdConvertTablePtr;
  UINT16                               PlatformId;
  UINT8                                Index;
  UINT8                                CpuRevisionId;
  EFI_STATUS                           Status;
  UINT8                                Data;
  UINT16                               PlatformIdOverride;
  UINT32                               CpuModel;

  PlatformId                = CpmTablePpiPtr->MainTablePtr->CurrentPlatformId;
  PlatformIdTablePtr        = NULL;
  PlatformIdTable2Ptr       = NULL;
  PlatformIdConvertTablePtr = NULL;
  CpuRevisionId             = CpmTablePpiPtr->CommonFunction.GetCpuRevisionId (CpmTablePpiPtr);

  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_EX_CPU) {
    PlatformIdTablePtr        = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, HYEX_CPM_SIGNATURE_GET_PLATFORM_ID);
    PlatformIdTable2Ptr       = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, HYEX_CPM_SIGNATURE_GET_PLATFORM_ID2);
    PlatformIdConvertTablePtr = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, HYEX_CPM_SIGNATURE_GET_PLATFORM_ID_CONVERT);
  } else if (CpuModel == HYGON_GX_CPU) {
    PlatformIdTablePtr        = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, HYGX_CPM_SIGNATURE_GET_PLATFORM_ID);
    PlatformIdTable2Ptr       = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, HYGX_CPM_SIGNATURE_GET_PLATFORM_ID2);
    PlatformIdConvertTablePtr = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, HYGX_CPM_SIGNATURE_GET_PLATFORM_ID_CONVERT);
  }

  PlatformIdOverride = CFG_CPM_PLATFORM_ID_OVERRIDE;
  if (PlatformIdOverride != 0xFFFF) {
    PlatformId = PlatformIdOverride;
  } else if (PlatformIdTablePtr) {
    PlatformId = 0;
    for (Index = 0; Index < HYGON_PLATFORM_ID_TABLE_SIZE; Index++) {
      if (PlatformIdTablePtr->GpioPin[Index] == 0xFF) {
        break;
      }

      PlatformId = PlatformId + (CpmTablePpiPtr->CommonFunction.GetGpio (CpmTablePpiPtr, PlatformIdTablePtr->GpioPin[Index]) << Index);
    }
  } else if (PlatformIdTable2Ptr) {
    PlatformId = 0;
    for (Index = 0; Index < HYGON_PLATFORM_ID_TABLE_SIZE; Index++) {
      if (PlatformIdTable2Ptr->GpioPin[Index].SmbusSelect == 0xFF) {
        break;
      }

      Data   = 0;
      Status = CpmTablePpiPtr->CommonFunction.ReadSmbus (CpmTablePpiPtr, PlatformIdTable2Ptr->GpioPin[Index].SmbusSelect, (PlatformIdTable2Ptr->GpioPin[Index].SmbusAddress >> 1), PlatformIdTable2Ptr->GpioPin[Index].SmbusOffset, 1, &Data);
      if (!EFI_ERROR (Status)) {
        Data = (Data >> PlatformIdTable2Ptr->GpioPin[Index].SmbusBit) & 0x01;
        PlatformId = PlatformId + (Data << Index);
      }
    }
  }

  CPM_SET_ORIGINAL_PLATFORM_ID (PlatformId);

  if (PlatformIdConvertTablePtr) {
    for (Index = 0; Index < HYGON_PLATFORM_ID_CONVERT_TABLE_SIZE; Index++) {
      if (PlatformIdConvertTablePtr->Item[Index].CpuRevisionId == 0xFF) {
        PlatformId = 0;
        break;
      }

      if ((PlatformId & PlatformIdConvertTablePtr->Item[Index].OriginalIdMask) == PlatformIdConvertTablePtr->Item[Index].OriginalId &&
          CpuRevisionId == PlatformIdConvertTablePtr->Item[Index].CpuRevisionId) {
        PlatformId = PlatformIdConvertTablePtr->Item[Index].ConvertedId;
        break;
      }
    }
  }

  if (PlatformId >= 32) {
    PlatformId = 0;
  }

  CPM_SET_CURRENT_PLATFORM_ID (PlatformId);

  CpmTablePpiPtr->MainTablePtr->CurrentPlatformId = PlatformId;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Kernal Common function to read the register of Smbus Device.
 *
 * @param[in]     This           Point to Ppi
 * @param[in]     Select         The Smbus number.
 * @param[in]     Address        The address of Smbus device
 * @param[in]     Offset         The Offset of Smbus register
 * @param[in]     Length         The register size in BYTE
 * @param[out]    Value          Data Pointer to save register value
 *
 * @retval        EFI_SUCCESS    SSDT table initialized successfully
 * @retval        EFI_ERROR      Initialization failed
 */
EFI_STATUS
HygonCpmPeiSmbusGetByte (
  IN       VOID                    *This,
  IN       UINT8                   Select,
  IN       UINT8                   Address,
  IN       UINT8                   Offset,
  IN       UINTN                   Length,
  OUT   UINT8                   *Value
  )
{
  EFI_PEI_SERVICES          **PeiServices;
  EFI_STATUS                Status;
  CPM_SMBUS_PPI             *SmbusPpi;
  EFI_SMBUS_DEVICE_ADDRESS  SmbusSlaveAddress;
  UINTN                     SmbusLength;
  UINTN                     SmbusOffset;

  PeiServices = (EFI_PEI_SERVICES **)((HYGON_CPM_TABLE_PPI *)This)->MainTablePtr->Service.Pointer;

  Status = (**PeiServices).LocatePpi (
                             (CPM_PEI_SERVICES **)PeiServices,
                             &gCpmPeiSmbusPpiGuid,
                             Select,
                             NULL,
                             (VOID **)&SmbusPpi
                             );
  if (!EFI_ERROR (Status)) {
    SmbusLength = Length;
    SmbusOffset = Offset;
    SmbusSlaveAddress.SmbusDeviceAddress = Address;

    Status = SmbusPpi->Execute (
                         CpmSmbusPpi,
                         SmbusSlaveAddress,
                         SmbusOffset,
                         EfiSmbusReadByte,
                         FALSE,
                         &SmbusLength,
                         Value
                         );
  }

  return Status;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Kernal Common function to set the register of Smbus device.
 *
 * @param[in]     This           Point to Ppi
 * @param[in]     Select         The Smbus number.
 * @param[in]     Address        The address of Smbus device
 * @param[in]     Offset         The Offset of Smbus register
 * @param[in]     Length         The register size in BYTE
 * @param[in]     Value          Data Pointer to register value
 *
 * @retval        EFI_SUCCESS    SSDT table initialized successfully
 * @retval        EFI_ERROR      Initialization failed
 */
EFI_STATUS
HygonCpmPeiSmbusSetByte (
  IN       VOID                    *This,
  IN       UINT8                   Select,
  IN       UINT8                   Address,
  IN       UINT8                   Offset,
  IN       UINTN                   Length,
  IN       UINT8                   *Value
  )
{
  EFI_PEI_SERVICES          **PeiServices;
  EFI_STATUS                Status;
  CPM_SMBUS_PPI             *SmbusPpi;
  EFI_SMBUS_DEVICE_ADDRESS  SmbusSlaveAddress;
  UINTN                     SmbusLength;
  UINTN                     SmbusOffset;

  PeiServices = (EFI_PEI_SERVICES **)((HYGON_CPM_TABLE_PPI *)This)->MainTablePtr->Service.Pointer;

  Status = (**PeiServices).LocatePpi (
                             (CPM_PEI_SERVICES **)PeiServices,
                             &gCpmPeiSmbusPpiGuid,
                             Select,
                             NULL,
                             (VOID **)&SmbusPpi
                             );
  if (!EFI_ERROR (Status)) {
    SmbusLength = Length;
    SmbusOffset = Offset;
    SmbusSlaveAddress.SmbusDeviceAddress = Address;

    Status = SmbusPpi->Execute (
                         CpmSmbusPpi,
                         SmbusSlaveAddress,
                         SmbusOffset,
                         EfiSmbusWriteByte,
                         FALSE,
                         &SmbusLength,
                         Value
                         );
  }

  return Status;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Kernal Common function to read the register of Smbus Device in block mode.
 *
 * @param[in]     This           Point to Ppi
 * @param[in]     Select         The Smbus number.
 * @param[in]     Address        The address of Smbus device
 * @param[in]     Offset         The Offset of Smbus register
 * @param[in]     Length         The register size in BYTE
 * @param[out]    Value          Data Pointer to save register value
 *
 * @retval        EFI_SUCCESS    SSDT table initialized successfully
 * @retval        EFI_ERROR      Initialization failed
 */
EFI_STATUS
HygonCpmPeiSmbusGetBlock (
  IN       VOID                    *This,
  IN       UINT8                   Select,
  IN       UINT8                   Address,
  IN       UINT8                   Offset,
  IN       UINTN                   Length,
  OUT   UINT8                   *Value
  )
{
  EFI_PEI_SERVICES          **PeiServices;
  EFI_STATUS                Status;
  CPM_SMBUS_PPI             *SmbusPpi;
  EFI_SMBUS_DEVICE_ADDRESS  SmbusSlaveAddress;
  UINTN                     SmbusLength;
  UINTN                     SmbusOffset;

  PeiServices = (EFI_PEI_SERVICES **)((HYGON_CPM_TABLE_PPI *)This)->MainTablePtr->Service.Pointer;

  Status = (**PeiServices).LocatePpi (
                             (CPM_PEI_SERVICES **)PeiServices,
                             &gCpmPeiSmbusPpiGuid,
                             Select,
                             NULL,
                             (VOID **)&SmbusPpi
                             );
  if (!EFI_ERROR (Status)) {
    SmbusLength = Length;
    SmbusOffset = Offset;
    SmbusSlaveAddress.SmbusDeviceAddress = Address;

    Status = SmbusPpi->Execute (
                         CpmSmbusPpi,
                         SmbusSlaveAddress,
                         SmbusOffset,
                         EfiSmbusReadBlock,
                         FALSE,
                         &SmbusLength,
                         Value
                         );
  }

  return Status;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Kernal Common function to set the register of Smbus device in block mode.
 *
 * @param[in]     This           Point to Ppi
 * @param[in]     Select         The Smbus number.
 * @param[in]     Address        The address of Smbus device
 * @param[in]     Offset         The Offset of Smbus register
 * @param[in]     Length         The register size in BYTE
 * @param[in]     Value          Data Pointer to register value
 *
 * @retval        EFI_SUCCESS    SSDT table initialized successfully
 * @retval        EFI_ERROR      Initialization failed
 */
EFI_STATUS
HygonCpmPeiSmbusSetBlock (
  IN       VOID                    *This,
  IN       UINT8                   Select,
  IN       UINT8                   Address,
  IN       UINT8                   Offset,
  IN       UINTN                   Length,
  IN       UINT8                   *Value
  )
{
  EFI_PEI_SERVICES          **PeiServices;
  EFI_STATUS                Status;
  CPM_SMBUS_PPI             *SmbusPpi;
  EFI_SMBUS_DEVICE_ADDRESS  SmbusSlaveAddress;
  UINTN                     SmbusLength;
  UINTN                     SmbusOffset;

  PeiServices = (EFI_PEI_SERVICES **)((HYGON_CPM_TABLE_PPI *)This)->MainTablePtr->Service.Pointer;

  Status = (**PeiServices).LocatePpi (
                             (CPM_PEI_SERVICES **)PeiServices,
                             &gCpmPeiSmbusPpiGuid,
                             Select,
                             NULL,
                             (VOID **)&SmbusPpi
                             );
  if (!EFI_ERROR (Status)) {
    SmbusLength = Length;
    SmbusOffset = Offset;
    SmbusSlaveAddress.SmbusDeviceAddress = Address;

    Status = SmbusPpi->Execute (
                         CpmSmbusPpi,
                         SmbusSlaveAddress,
                         SmbusOffset,
                         EfiSmbusWriteBlock,
                         FALSE,
                         &SmbusLength,
                         Value
                         );
  }

  return Status;
}

/*---------------------------------------------------------------------------------------*/

/**
 * Register Common Kenerl functions at the HygonCpmInitPeim entry point.
 *
 * This function registers CPM common kernel functions in HygonCpmTablePpi
 * at HygonCpmInitPeim.
 *
 *  @param[in]   This                Pointer to Ppi.
 *
 */
VOID
EFIAPI
HygonCpmPeiRegisterKernel (
  IN       VOID                        *This
  )
{
  HYGON_CPM_COMMON_FUNCTION  *CommonFunctionPtr;

  CommonFunctionPtr = &((HYGON_CPM_TABLE_PPI *)This)->CommonFunction;

  CommonFunctionPtr->GetTablePtr     = CpmGetTablePtr;
  CommonFunctionPtr->GetTablePtr2    = CpmGetTablePtr2;
  CommonFunctionPtr->AddTable        = CpmAddTable;
  CommonFunctionPtr->RemoveTable     = CpmRemoveTable;
  CommonFunctionPtr->ReadSmbus       = HygonCpmPeiSmbusGetByte;
  CommonFunctionPtr->WriteSmbus      = HygonCpmPeiSmbusSetByte;
  CommonFunctionPtr->ReadSmbusBlock  = HygonCpmPeiSmbusGetBlock;
  CommonFunctionPtr->WriteSmbusBlock = HygonCpmPeiSmbusSetBlock;
  CommonFunctionPtr->RelocateTable   = CpmRelocateTableList;
  CommonFunctionPtr->CopyMem = CpmCopyMem;
  CommonFunctionPtr->LoadPreInitTable = HygonCpmLoadPreInitTable;

  return;
}
