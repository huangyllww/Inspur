/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM POST API, and related functions.
 *
 * Contains code that initialized the CPM before memory init.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  Kernel
 * @e \$Revision$   @e \$Date$
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
#include <AmdCpmTable.h>
#include <AmdCpmBaseIo.h>
#include <AmdCpmCpu.h>
#include <AmdCpmFch.h>
#include <Ppi/FabricTopologyServicesPpi.h>

VOID
EFIAPI
AmdCpmCoreTopologyInit (
  IN       AMD_CPM_TABLE_PPI           *CpmTablePpiPtr
  );

VOID
EFIAPI
AmdCpmLoadPreInitTable (
  IN       VOID                        *This,
  IN       UINT8                       Stage
  );

VOID
EFIAPI
AmdCpmPlatformIdInit (
  IN       AMD_CPM_TABLE_PPI           *CpmTablePpiPtr
  );

VOID
EFIAPI
AmdCpmPeiRegisterKernel (
  IN       VOID                        *This
  );

EFI_STATUS
EFIAPI
AmdCpmInitBeforeS3LateRestore (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );

EFI_STATUS
EFIAPI
AmdCpmInitPeimNotifyCallback (
  IN      EFI_PEI_SERVICES              **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN      VOID                          *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mCpmInitPeimNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCpmOemTablePpiGuid,
  AmdCpmInitPeimNotifyCallback
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mNotifyListS3Restore =
{
  EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gCpmPeiMemoryDiscoveredPpiGuid,
  AmdCpmInitBeforeS3LateRestore
};

AMD_CPM_CORE_TOPOLOGY_TABLE gCpmCoreTopologyTable = {
  {CPM_SIGNATURE_CORE_TOPOLOGY, sizeof (gCpmCoreTopologyTable) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
};

/*----------------------------------------------------------------------------------------*/
/**
 * CPM Init Function Before S3 Late Restore
 *
 * This function updates CPM Main Table Pointer in AMD_CPM_TABLE_PPI Data Structure
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
AmdCpmInitBeforeS3LateRestore (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{
  EFI_STATUS                  Status;
  AMD_CPM_TABLE_PPI           *CpmTablePtr;
  AMD_CPM_TABLE_HOB_PPI       *CpmTableHobPtr;

  Status = (*PeiServices)->LocatePpi (
                              (CPM_PEI_SERVICES**)PeiServices,
                              &gAmdCpmTablePpiGuid,
                              0,
                              NULL,
                              (VOID**)&CpmTablePtr
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = (*PeiServices)->LocatePpi (
                              (CPM_PEI_SERVICES**)PeiServices,
                              &gAmdCpmTableHobPpiGuid,
                              0,
                              NULL,
                              (VOID**)&CpmTableHobPtr
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CpmTablePtr->CommonFunction.RelocateTable (CpmTableHobPtr);

  CpmTablePtr->MainTablePtr = (AMD_CPM_MAIN_TABLE*) ((AMD_CPM_HOB_HEADER*)CpmTableHobPtr)->MainTablePtr.Pointer;

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM Init PEIM driver
 *
 * This function gets CPM definition tables from AmdCpmOemTablePpi, reorgnizes the tables
 * and installs AmdCpmTablePpi. It also initializes the registers by load Pre Init Table
 * and update Current Platform Id at AmdCpmInitPeim.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
AmdCpmInitPeimEntryPoint (
  IN       CPM_PEI_FILE_HANDLE  FileHandle,
  IN       CPM_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                  Status;

  Status = (**PeiServices).NotifyPpi (PeiServices, &mCpmInitPeimNotify);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback Function of the AMD CPM Init PEIM driver
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
AmdCpmInitPeimNotifyCallback (
  IN      EFI_PEI_SERVICES              **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN      VOID                          *Ppi
  )
{
  EFI_STATUS                  Status;
  AMD_CPM_OEM_TABLE_PPI       *AmdCpmOemTablePpiPtr;
  AMD_CPM_TABLE_PPI           *AmdCpmTablePpiPtr;
  EFI_PEI_PPI_DESCRIPTOR      *PpiListCpmTablePtr;
  EFI_PEI_PPI_DESCRIPTOR      *PpiListCpmTableHobPtr;
  AMD_CPM_MAIN_TABLE          *MainTablePtr;
  AMD_CPM_COMMON_FUNCTION     *CommonFunctionPtr;
  EFI_HOB_GUID_TYPE           *Hob;
  EFI_BOOT_MODE               BootMode;
  AMD_CPM_TABLE_LIST          HobList;
  UINT8                       *HobBuffer;
  UINT8                       CpmBootMode;

  Status = (*PeiServices)->LocatePpi (
                              PeiServices,
                              &gAmdCpmOemTablePpiGuid,
                              0,
                              NULL,
                              (VOID**)&AmdCpmOemTablePpiPtr
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CpmGenerateTableList (AmdCpmOemTablePpiPtr->TableList, AmdCpmOemTablePpiPtr->PlatformId, &HobList);

  Status = (*PeiServices)->GetBootMode (
                             PeiServices,
                             &BootMode
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CpmBootMode = CpmGetBootMode ();

  if (CpmBootMode != CPM_BOOT_MODE_S3) {
    If_CpmBuildGuidHob (gAmdCpmTableHobGuid, Hob, NULL, HobList.Size + AMD_TABLE_ADDITIONAL_BUFFER_SIZE, Status) {

      HobBuffer = (UINT8*) Hob;

    } else {
      return EFI_NOT_FOUND;
    }
  } else {
    Status = (*PeiServices)->AllocatePool (
                                PeiServices,
                                HobList.Size + AMD_TABLE_ADDITIONAL_BUFFER_SIZE,
                                (VOID**)&HobBuffer
                                );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = (*PeiServices)->AllocatePool (
                                PeiServices,
                                sizeof (EFI_PEI_PPI_DESCRIPTOR),
                                (VOID**)&PpiListCpmTableHobPtr
                                );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    PpiListCpmTableHobPtr->Flags  = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
    PpiListCpmTableHobPtr->Guid   = &gAmdCpmTableHobPpiGuid;
    PpiListCpmTableHobPtr->Ppi    = HobBuffer;

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
                              sizeof (AMD_CPM_TABLE_PPI),
                              (VOID**)&AmdCpmTablePpiPtr
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  MainTablePtr->CurrentPlatformId = AmdCpmOemTablePpiPtr->PlatformId;
  MainTablePtr->Service.Pointer   = (VOID*)PeiServices;
  CommonFunctionPtr               = &(AmdCpmTablePpiPtr->CommonFunction);

  AmdCpmTablePpiPtr->Revision     = (AMD_CPM_KERNEL_VERSION << 16) + AmdCpmOemTablePpiPtr->Revision;
  AmdCpmTablePpiPtr->MainTablePtr = MainTablePtr;

  ((AMD_CPM_HOB_HEADER*)HobBuffer)->Revision = (UINT32)AmdCpmTablePpiPtr->Revision;

  AmdCpmPeiRegisterKernel (AmdCpmTablePpiPtr);
  CpmRegisterBaseIo (AmdCpmTablePpiPtr);
  CpmRegisterCpu (AmdCpmTablePpiPtr);
  CpmRegisterFch (AmdCpmTablePpiPtr);

  AmdCpmCoreTopologyInit (AmdCpmTablePpiPtr);
  Status = (*PeiServices)->AllocatePool (
                              PeiServices,
                              sizeof (EFI_PEI_PPI_DESCRIPTOR),
                              (VOID**)&PpiListCpmTablePtr
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PpiListCpmTablePtr->Flags   = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiListCpmTablePtr->Guid    = &gAmdCpmTablePpiGuid;
  PpiListCpmTablePtr->Ppi     = AmdCpmTablePpiPtr;

  AmdCpmLoadPreInitTable (AmdCpmTablePpiPtr, CPM_PRE_INIT_STAGE_0);
  AmdCpmPlatformIdInit (AmdCpmTablePpiPtr);

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
AmdCpmCoreTopologyInit (
  IN       AMD_CPM_TABLE_PPI          *CpmTablePpiPtr
  )
{
  AMD_PEI_FABRIC_TOPOLOGY_SERVICES_PPI          *FabricTopologyServicesPpi;
  AMD_FABRIC_TOPOLOGY_PROCESSOR_NEIGHBOR_INFO   *NeighborInfo;
  AMD_CPM_CORE_TOPOLOGY_TABLE         *AmdCpmCoreTopologyPtr;
  EFI_STATUS                          Status;
  UINTN                               NumberOfProcessors;
  UINTN                               TotalNumberOfDie;
  UINTN                               TotalNumberOfRootBridges;
  UINTN                               NumberOfNeighbors;
  UINTN                               SystemFabricID;
  UINTN                               BusNumberBase;
  UINTN                               BusNumberLimit;
  UINTN                               SocketIndex;
  UINTN                               DieIndex;
  UINTN                               RootBridgeIndex;
  EFI_PEI_SERVICES                    **PeiServices;

  AmdCpmCoreTopologyPtr = NULL;
  switch (CpmTablePpiPtr->ChipId.Sb) {
  case CPM_FCH_REVISION_ID_ZP:
  case CPM_FCH_REVISION_ID_RV:
    AmdCpmCoreTopologyPtr = CpmTablePpiPtr->CommonFunction.AddTable (CpmTablePpiPtr, &gCpmCoreTopologyTable);
    AmdCpmCoreTopologyPtr->CoreList[0].Socket = 0xFF;
    break;
  default:
    break;
  }

  if (AmdCpmCoreTopologyPtr) {
    PeiServices = (EFI_PEI_SERVICES**) CpmTablePpiPtr->MainTablePtr->Service.Pointer;
    // Get number of sockets from CPU topology PPI
    // Locate CPU topology PPI
    Status = (**PeiServices).LocatePpi (
                               PeiServices,
                               &gAmdFabricTopologyServicesPpiGuid,
                               0,
                               NULL,
                               &FabricTopologyServicesPpi
                               );
    FabricTopologyServicesPpi->GetSystemInfo (&NumberOfProcessors, &TotalNumberOfDie, &TotalNumberOfRootBridges);

    for (SocketIndex = 0; SocketIndex < NumberOfProcessors; SocketIndex++) {
      FabricTopologyServicesPpi->GetProcessorInfo (SocketIndex, &TotalNumberOfDie, &TotalNumberOfRootBridges, &NumberOfNeighbors, &NeighborInfo);
      for (DieIndex = 0, RootBridgeIndex = 0; DieIndex < TotalNumberOfDie; DieIndex++) {
        FabricTopologyServicesPpi->GetRootBridgeInfo (SocketIndex, DieIndex, RootBridgeIndex, &SystemFabricID, &BusNumberBase, &BusNumberLimit);
        //AmdCpmCoreTopologyPtr->CoreList[SocketIndex].Socket   = (UINT8) SocketIndex;
        //AmdCpmCoreTopologyPtr->CoreList[SocketIndex].Die      = (UINT8) DieIndex;
        //AmdCpmCoreTopologyPtr->CoreList[SocketIndex].Bus      = (UINT8) BusNumberBase;
        //AmdCpmCoreTopologyPtr->CoreList[SocketIndex+1].Socket = 0xFF;
        AmdCpmCoreTopologyPtr->CoreList[(SocketIndex * TotalNumberOfDie) + DieIndex].Socket   = (UINT8) SocketIndex;
        AmdCpmCoreTopologyPtr->CoreList[(SocketIndex * TotalNumberOfDie) + DieIndex].Die      = (UINT8) DieIndex;
        AmdCpmCoreTopologyPtr->CoreList[(SocketIndex * TotalNumberOfDie) + DieIndex].Bus      = (UINT8) BusNumberBase;
      }
    }
    AmdCpmCoreTopologyPtr->CoreList[SocketIndex * TotalNumberOfDie].Socket = 0xFF;
  }

}

/*----------------------------------------------------------------------------------------*/
/**
 * The function to load Pre Init Table.
 *
 * @param[in]     This           Pointer to AMD CPM TABLE Ppi
 * @param[in]     Stage          Stage number to initialize the registers
 *
 */
VOID
EFIAPI
AmdCpmLoadPreInitTable (
  IN       VOID                   *This,
  IN       UINT8                  Stage
  )
{
  AMD_CPM_PRE_INIT_TABLE          *PreInitTablePtr;
  AMD_CPM_GPIO_INIT_TABLE         *GpioPreInitTablePtr;
  AMD_CPM_PRE_SETTING_ITEM        *TablePtr;
  AMD_CPM_TABLE_PPI               *CpmTablePpiPtr;
  AMD_CPM_GPIO_ITEM               *GpioTablePtr;

  CpmTablePpiPtr  = (AMD_CPM_TABLE_PPI*) This;
  PreInitTablePtr = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_PRE_INIT);
  GpioPreInitTablePtr = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_GPIO_PRE_INIT);

  if (PreInitTablePtr) {
    TablePtr = &PreInitTablePtr->Item[0];
    while (TablePtr->Type != 0xFF ) {
      if (TablePtr->Stage == Stage) {
        switch (TablePtr->Type) {
        case 0:
          CpmTablePpiPtr->CommonFunction.MmioAndThenOr8 ( CpmTablePpiPtr->MainTablePtr->AcpiMemIoBaseAddr + (TablePtr->Select << 0x08) + TablePtr->Offset,
                                                          TablePtr->AndMask,
                                                          TablePtr->OrMask
                                                        );
          break;
        case 1:
        case 2:
          CpmTablePpiPtr->CommonFunction.PciAndThenOr8 ( CpmTablePpiPtr,
                                                         0x00,
                                                         (TablePtr->Select & 0xF8) >> 3,
                                                          TablePtr->Select & 7,
                                                          TablePtr->Offset + (TablePtr->Type == 1? 0 : 0x100),
                                                          TablePtr->AndMask,
                                                          TablePtr->OrMask
                                                        );
          break;
        }
      }
      TablePtr ++;
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
AmdCpmPlatformIdInit (
  IN       AMD_CPM_TABLE_PPI           *CpmTablePpiPtr
  )
{
  AMD_CPM_PLATFORM_ID_TABLE           *PlatformIdTablePtr;
  AMD_CPM_PLATFORM_ID_TABLE2          *PlatformIdTable2Ptr;
  AMD_CPM_PLATFORM_ID_CONVERT_TABLE   *PlatformIdConvertTablePtr;
  UINT16                              PlatformId;
  UINT8                               Index;
  UINT8                               CpuRevisionId;
  EFI_STATUS                          Status;
  UINT8                               Data;
  UINT16                              PlatformIdOverride;
  PlatformId                  = CpmTablePpiPtr->MainTablePtr->CurrentPlatformId;
  PlatformIdTablePtr          = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_GET_PLATFORM_ID);
  PlatformIdTable2Ptr         = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_GET_PLATFORM_ID2);
  PlatformIdConvertTablePtr   = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_GET_PLATFORM_ID_CONVERT);
  CpuRevisionId               = CpmTablePpiPtr->CommonFunction.GetCpuRevisionId (CpmTablePpiPtr);

  PlatformIdOverride          = CFG_CPM_PLATFORM_ID_OVERRIDE;
  if (PlatformIdOverride != 0xFFFF) {
    PlatformId = PlatformIdOverride;
  } else if (PlatformIdTablePtr) {
    PlatformId = 0;
    for (Index = 0; Index < AMD_PLATFORM_ID_TABLE_SIZE; Index++) {
      if (PlatformIdTablePtr->GpioPin[Index] == 0xFF) {
        break;
      }
      PlatformId = PlatformId + (CpmTablePpiPtr->CommonFunction.GetGpio (CpmTablePpiPtr, PlatformIdTablePtr->GpioPin[Index]) << Index);
    }
  } else if (PlatformIdTable2Ptr) {
    PlatformId = 0;
    for (Index = 0; Index < AMD_PLATFORM_ID_TABLE_SIZE; Index++) {
      if (PlatformIdTable2Ptr->GpioPin[Index].SmbusSelect == 0xFF) {
        break;
      }
      Data = 0;
      Status = CpmTablePpiPtr->CommonFunction.ReadSmbus (CpmTablePpiPtr, PlatformIdTable2Ptr->GpioPin[Index].SmbusSelect, (PlatformIdTable2Ptr->GpioPin[Index].SmbusAddress >> 1), PlatformIdTable2Ptr->GpioPin[Index].SmbusOffset, 1, &Data);
      if (!EFI_ERROR (Status)) {
        Data = (Data >> PlatformIdTable2Ptr->GpioPin[Index].SmbusBit) & 0x01;
        PlatformId = PlatformId + (Data << Index);
      }
    }
  }

  CPM_SET_ORIGINAL_PLATFORM_ID (PlatformId);

  if (PlatformIdConvertTablePtr) {
    for (Index = 0; Index < AMD_PLATFORM_ID_CONVERT_TABLE_SIZE; Index++) {
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
AmdCpmPeiSmbusGetByte (
  IN       VOID                    *This,
  IN       UINT8                   Select,
  IN       UINT8                   Address,
  IN       UINT8                   Offset,
  IN       UINTN                   Length,
     OUT   UINT8                   *Value
  )
{
  EFI_PEI_SERVICES                **PeiServices;
  EFI_STATUS                      Status;
  CPM_SMBUS_PPI                   *SmbusPpi;
  EFI_SMBUS_DEVICE_ADDRESS        SmbusSlaveAddress;
  UINTN                           SmbusLength;
  UINTN                           SmbusOffset;

  PeiServices = (EFI_PEI_SERVICES**) ((AMD_CPM_TABLE_PPI*)This)->MainTablePtr->Service.Pointer;

  Status = (**PeiServices).LocatePpi (
                                      (CPM_PEI_SERVICES**)PeiServices,
                                      &gCpmPeiSmbusPpiGuid,
                                      Select,
                                      NULL,
                                      (VOID**)&SmbusPpi
                                      );
  if (!EFI_ERROR (Status)) {
    SmbusLength = Length;
    SmbusOffset = Offset;
    SmbusSlaveAddress.SmbusDeviceAddress = Address;

    Status = SmbusPpi->Execute (    CpmSmbusPpi,
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
AmdCpmPeiSmbusSetByte (
  IN       VOID                    *This,
  IN       UINT8                   Select,
  IN       UINT8                   Address,
  IN       UINT8                   Offset,
  IN       UINTN                   Length,
  IN       UINT8                   *Value
  )
{
  EFI_PEI_SERVICES                **PeiServices;
  EFI_STATUS                      Status;
  CPM_SMBUS_PPI                   *SmbusPpi;
  EFI_SMBUS_DEVICE_ADDRESS        SmbusSlaveAddress;
  UINTN                           SmbusLength;
  UINTN                           SmbusOffset;

  PeiServices = (EFI_PEI_SERVICES**) ((AMD_CPM_TABLE_PPI*)This)->MainTablePtr->Service.Pointer;

  Status = (**PeiServices).LocatePpi (
                                      (CPM_PEI_SERVICES**)PeiServices,
                                      &gCpmPeiSmbusPpiGuid,
                                      Select,
                                      NULL,
                                      (VOID**)&SmbusPpi
                                      );
  if (!EFI_ERROR (Status)) {
    SmbusLength = Length;
    SmbusOffset = Offset;
    SmbusSlaveAddress.SmbusDeviceAddress = Address;

    Status = SmbusPpi->Execute (    CpmSmbusPpi,
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
AmdCpmPeiSmbusGetBlock (
  IN       VOID                    *This,
  IN       UINT8                   Select,
  IN       UINT8                   Address,
  IN       UINT8                   Offset,
  IN       UINTN                   Length,
     OUT   UINT8                   *Value
  )
{
  EFI_PEI_SERVICES                **PeiServices;
  EFI_STATUS                      Status;
  CPM_SMBUS_PPI                   *SmbusPpi;
  EFI_SMBUS_DEVICE_ADDRESS        SmbusSlaveAddress;
  UINTN                           SmbusLength;
  UINTN                           SmbusOffset;

  PeiServices = (EFI_PEI_SERVICES**) ((AMD_CPM_TABLE_PPI*)This)->MainTablePtr->Service.Pointer;

  Status = (**PeiServices).LocatePpi (
                                      (CPM_PEI_SERVICES**)PeiServices,
                                      &gCpmPeiSmbusPpiGuid,
                                      Select,
                                      NULL,
                                      (VOID**)&SmbusPpi
                                      );
  if (!EFI_ERROR (Status)) {
    SmbusLength = Length;
    SmbusOffset = Offset;
    SmbusSlaveAddress.SmbusDeviceAddress = Address;

    Status = SmbusPpi->Execute (    CpmSmbusPpi,
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
AmdCpmPeiSmbusSetBlock (
  IN       VOID                    *This,
  IN       UINT8                   Select,
  IN       UINT8                   Address,
  IN       UINT8                   Offset,
  IN       UINTN                   Length,
  IN       UINT8                   *Value
  )
{
  EFI_PEI_SERVICES                **PeiServices;
  EFI_STATUS                      Status;
  CPM_SMBUS_PPI                   *SmbusPpi;
  EFI_SMBUS_DEVICE_ADDRESS        SmbusSlaveAddress;
  UINTN                           SmbusLength;
  UINTN                           SmbusOffset;

  PeiServices = (EFI_PEI_SERVICES**) ((AMD_CPM_TABLE_PPI*)This)->MainTablePtr->Service.Pointer;

  Status = (**PeiServices).LocatePpi (
                                      (CPM_PEI_SERVICES**)PeiServices,
                                      &gCpmPeiSmbusPpiGuid,
                                      Select,
                                      NULL,
                                      (VOID**)&SmbusPpi
                                      );
  if (!EFI_ERROR (Status)) {
    SmbusLength = Length;
    SmbusOffset = Offset;
    SmbusSlaveAddress.SmbusDeviceAddress = Address;

    Status = SmbusPpi->Execute (    CpmSmbusPpi,
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
 * Register Common Kenerl functions at the AmdCpmInitPeim entry point.
 *
 * This function registers CPM common kernel functions in AmdCpmTablePpi
 * at AmdCpmInitPeim.
 *
 *  @param[in]   This                Pointer to Ppi.
 *
 */
VOID
EFIAPI
AmdCpmPeiRegisterKernel (
  IN       VOID                        *This
  )
{
  AMD_CPM_COMMON_FUNCTION             *CommonFunctionPtr;
  CommonFunctionPtr                   = &((AMD_CPM_TABLE_PPI*)This)->CommonFunction;

  CommonFunctionPtr->GetTablePtr      = CpmGetTablePtr;
  CommonFunctionPtr->GetTablePtr2     = CpmGetTablePtr2;
  CommonFunctionPtr->AddTable         = CpmAddTable;
  CommonFunctionPtr->RemoveTable      = CpmRemoveTable;
  CommonFunctionPtr->ReadSmbus        = AmdCpmPeiSmbusGetByte;
  CommonFunctionPtr->WriteSmbus       = AmdCpmPeiSmbusSetByte;
  CommonFunctionPtr->ReadSmbusBlock   = AmdCpmPeiSmbusGetBlock;
  CommonFunctionPtr->WriteSmbusBlock  = AmdCpmPeiSmbusSetBlock;
  CommonFunctionPtr->RelocateTable    = CpmRelocateTableList;
  CommonFunctionPtr->CopyMem          = CpmCopyMem;
  CommonFunctionPtr->LoadPreInitTable = AmdCpmLoadPreInitTable;

  return;
}
