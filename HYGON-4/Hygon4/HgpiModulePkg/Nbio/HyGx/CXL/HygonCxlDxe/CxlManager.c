/* $NoKeywords:$ */
/**
 * @file
 *
 * HygonCxlDxe Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonCxlDxe
 * @e \$Revision: 312065 $   @e \$Date: 2022-11-08 16:17:05 -0600 (Nov, 8 Tue 2022) $
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
#include <HygonCxlDxe.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/HygonNbioPcieServicesProtocol.h>
#include <GnbHsio.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Guid/GnbCxlInfoHob.h>
#include <GnbRegisters.h>
#include <FabricRegistersST.h>
#include <Library/NbioHandleLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/UefiLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/GnbLib.h>
#include <Library/HygonPspHpobLib.h>
#include <Library/HobLib.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/HygonHeapLib.h>
#include <Protocol/FabricTopologyServices.h>
#include <Protocol/HygonCxlManagerProtocol.h>
/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define FILECODE        NBIO_CXL_HYGONCXLDXE_CXLMANAGER_FILECODE

#define CXLMEM_TEST_PATTERN    0x55AA55AAAA55AA55
#define TEST_CXL_MEM_SIZE      0x1000000             //16MB
#define TEST_PROGRESS_UNIT     0xFFFFF               //1MB
#define MAX_MISMATCH_COUNT     100
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
FabricCxlAllocateMem (
  IN       HYGON_CXL_MANAGER_PROTOCOL *This,
  IN OUT   UINT64                     *HostBaseAddress,
  IN OUT   UINT64                     *HdmBaseAddress,
  IN OUT   UINT64                     *Length,
  IN       UINT64                      Alignment,
  IN       FABRIC_CXL_TARGET           Target
  );

EFI_STATUS
EFIAPI
FabricCxlGetAvailableResource (
  IN       HYGON_CXL_MANAGER_PROTOCOL           *This,
  IN       FABRIC_CXL_AVAIL_RESOURCE            *CxlResourceSize
  );

EFI_STATUS
EFIAPI
FabricCxlGetUsedResource (
  IN       HYGON_CXL_MANAGER_PROTOCOL           *This,
  IN       FABRIC_CXL_USED_RESOURCE             *CxlResourceSize
  );
/*----------------------------------------------------------------------------------------
 *           P R O T O C O L   I N S T A N T I A T I O N
 *----------------------------------------------------------------------------------------
 */

HYGON_CXL_MANAGER_PROTOCOL mHygonCxlManagerProtocol = {
  HYGON_CXL_MANAGER_REVISION,
  FabricCxlAllocateMem,
  FabricCxlGetAvailableResource,
  FabricCxlGetUsedResource
};

/**
 *---------------------------------------------------------------------------------------
 *  DumpCxlHobInfo
 *
 *  Description:
 *     Print CXL Hob information
 *  Parameters:
 *    @param[in]     *CxlInfo      CXL HOB data pointer
 *
 *---------------------------------------------------------------------------------------
 **/
void
DumpFabricCxlManager (
  IN   FABRIC_CXL_MANAGER     *FabricCxlManager
  )
{
  UINT8  Index;
    
  IDS_HDT_CONSOLE (MAIN_FLOW, "----------------DumpFabricCxlManager Start----------------\n");
  
  for (Index = 0; Index < FabricCxlManager->MemRegionCount; Index++) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Region Index %d \n", Index);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  SocketMap       : 0x%X \n", FabricCxlManager->MemRegion[Index].SocketMap);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  DieMap          : 0x%X \n", FabricCxlManager->MemRegion[Index].DieMap);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  NbioMap         : 0x%X \n", FabricCxlManager->MemRegion[Index].NbioMap);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  IntlvType       : 0x%X \n", FabricCxlManager->MemRegion[Index].IntlvType);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  IntlvSize       : 0x%X \n", FabricCxlManager->MemRegion[Index].IntlvSize);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Base            : 0x%lX \n", FabricCxlManager->MemRegion[Index].Base);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Size            : 0x%lX \n", FabricCxlManager->MemRegion[Index].Size);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  UsedSize        : 0x%lX \n", FabricCxlManager->MemRegion[Index].UsedSize);
  }
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "----------------DumpFabricCxlManager End----------------\n");
}

EFI_STATUS
EFIAPI
FabricCxlManagerProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_HANDLE                             Handle;
  EFI_STATUS                             Status;
  ALLOCATE_HEAP_PARAMS                   AllocateHeapParams;
  FABRIC_CXL_MANAGER                     *FabricCxlManager;
  DRAM_BASE_ADDRESS_REGISTER             DramBaseAddr;
  DRAM_LIMIT_ADDRESS_REGISTER            DramLimitAddr;
  UINT32                                 SocketId;
  UINT32                                 LogicalDieId;
  UINT32                                 NbioId;
  //UINT32                               IntLvNumSockets;
  //UINT32                               IntLvNumDies;
  //UINT32                               IntLvNumChan;
  UINT32                                 DramPairIndex;
  UINT32                                 CxlMemRegionIndex;
  FABRIC_DRAM_ADDR_CONFIG0_REGISTER_HYGX DramAddrConfig0;
  FABRIC_DRAM_ADDR_CONFIG1_REGISTER_HYGX DramAddrConfig1;

  Status = EFI_SUCCESS;
  
  AllocateHeapParams.RequestedBufferSize = sizeof (FABRIC_CXL_MANAGER);
  AllocateHeapParams.BufferHandle        = HYGON_CXL_MANAGER;
  AllocateHeapParams.Persist             = HEAP_SYSTEM_MEM;
  FabricCxlManager = NULL;
  if (HeapAllocateBuffer (&AllocateHeapParams, NULL) == HGPI_SUCCESS) {
    FabricCxlManager = (FABRIC_CXL_MANAGER *) AllocateHeapParams.BufferPtr;
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "FabricCxlManager heap address = %p \n", FabricCxlManager);
  if (FabricCxlManager == NULL) {
    return EFI_ABORTED;
  }
  gBS->SetMem(FabricCxlManager, sizeof (FABRIC_CXL_MANAGER), 0);
  
  //Get CXL memory info from DF DRAM base/limit pair registers
  DramAddrConfig0.Value = FabricRegisterAccRead (0, 0, DRAMADDRCONFIG0_FUNC, DRAMADDRCONFIG0_REG_HYGX, IOMS0_INSTANCE_ID);
  DramAddrConfig1.Value = FabricRegisterAccRead (0, 0, DRAMADDRCONFIG1_FUNC, DRAMADDRCONFIG1_REG_HYGX, IOMS0_INSTANCE_ID);
  IDS_HDT_CONSOLE (MAIN_FLOW, "DramAddrConfig0 = 0x%X \n", DramAddrConfig0.Value);
  IDS_HDT_CONSOLE (MAIN_FLOW, "DramAddrConfig1 = 0x%X \n", DramAddrConfig1.Value);
  
  CxlMemRegionIndex = 0;
  FabricCxlManager->MemRegionCount = 0;
  for (DramPairIndex = 0; DramPairIndex < NUMBER_OF_DRAM_REGIONS; DramPairIndex++) {
    DramBaseAddr.Value = FabricRegisterAccRead (0, 0, DRAMBASEADDR0_FUNC, 
                                                (DRAMBASEADDR0_REG + (DramPairIndex * DRAM_REGION_REGISTER_OFFSET)), 
                                                IOMS0_INSTANCE_ID);
    
    if (DramBaseAddr.Field.AddrRngVal == 1) {
      DramLimitAddr.Value = FabricRegisterAccRead (0, 0, DRAMLIMITADDR0_FUNC, 
                                                (DRAMLIMITADDR0_REG + (DramPairIndex * DRAM_REGION_REGISTER_OFFSET)), 
                                                IOMS0_INSTANCE_ID);
      IDS_HDT_CONSOLE (MAIN_FLOW, "Dram pair %d Base Reg = 0x%X,  Limit Reg = 0x%X\n", DramPairIndex, DramBaseAddr.Value, DramLimitAddr.Value);
      FabricTopologyGetLocationFromFabricId (DramLimitAddr.HyGxField.DstFabricID, &SocketId, &LogicalDieId, &NbioId);
      if (LogicalDieId >= FABRIC_ID_CDD0_DIE_NUM) {
        continue;
      }
      /*
      IntLvNumSockets = 1 << DramBaseAddr.Field.IntLvNumSockets;
      if (DramPairIndex < 16) {
        IntLvNumDies = 1 << ((DramAddrConfig0.Value >> (DramPairIndex * 2)) & 0x3);
      } else {
        IntLvNumDies = 1 << ((DramAddrConfig1.Value >> ((DramPairIndex - 16) * 2)) & 0x3);
      }
      */
      FabricCxlManager->MemRegion[CxlMemRegionIndex].SocketMap = (UINT8)(1 << SocketId);
      FabricCxlManager->MemRegion[CxlMemRegionIndex].DieMap =(UINT8)(1 << LogicalDieId);
      FabricCxlManager->MemRegion[CxlMemRegionIndex].NbioMap = (UINT8)(1 << NbioId);
      FabricCxlManager->MemRegion[CxlMemRegionIndex].IntlvType = NoIntLv;
      FabricCxlManager->MemRegion[CxlMemRegionIndex].Base = LShiftU64 (((UINT64) (DramBaseAddr.Field.DramBaseAddr)), 28);
      FabricCxlManager->MemRegion[CxlMemRegionIndex].Size = LShiftU64 (((UINT64) (DramLimitAddr.HyGxField.DramLimitAddr + 1 - DramBaseAddr.Field.DramBaseAddr)), 28);
      FabricCxlManager->MemRegion[CxlMemRegionIndex].UsedSize = 0;
      FabricCxlManager->MemRegion[CxlMemRegionIndex].Alignment = 0;
      
      FabricCxlManager->MemRegionCount++;
      CxlMemRegionIndex++;
    }
  }
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "Cxl Memory Region Count %d \n", FabricCxlManager->MemRegionCount); 
  DumpFabricCxlManager (FabricCxlManager);
  
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gHygonCxlManagerProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mHygonCxlManagerProtocol
                    );
  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * BSC entry point for allocate CXL memory
 *
 *
 * @param[in]         This               Function pointer to HYGON_CXL_MANAGER_PROTOCOL.
 * @param[in, out]    HostBaseAddress    Starting address of the requested CXL memory range in system.
 * @param[in, out]    HdmBaseAddress     Starting address of the requested CXL memory range in one HDM region.
 * @param[in, out]    Length             Length of the requested CXL memory range.
 * @param[in]         Alignment          Alignment bit map.
 * @param[in]         Target             PCI bus number/Die number of the requestor.
 *
 * @retval            EFI_STATUS         EFI_OUT_OF_RESOURCES - The requested range could not be added because there are not
 *                                                              enough mapping resources.
 *                                       EFI_ABORTED          - One or more input parameters are invalid. For example, the
 *                                                              PciBusNumber does not correspond to any device in the system.
 *                                       EFI_SUCCESS          - Success to get an MMIO region
 */
EFI_STATUS
EFIAPI
FabricCxlAllocateMem (
  IN       HYGON_CXL_MANAGER_PROTOCOL *This,
  IN OUT   UINT64                     *HostBaseAddress,
  IN OUT   UINT64                     *HdmBaseAddress,
  IN OUT   UINT64                     *Length,
  IN       UINT64                      Alignment,
  IN       FABRIC_CXL_TARGET           Target
  )
{
  LOCATE_HEAP_PTR           LocateHeapParams;
  FABRIC_CXL_MANAGER       *FabricCxlManager;
  FABRIC_CXL_MEM_REGION    *MemRegion;
  UINT8                     RegionIndex;
  UINT16                    SocketId;
  UINT16                    DieId;
  UINT16                    RbId;
  UINT16                    DstFabricID;
  UINTN                     Register;
  CFG_ADDRESS_MAP_REGISTER  BusMap;
  EFI_STATUS                ReturnStatus;
  UINT64                    AlignPadLength;
  UINT64                    AvailBaseAddress;
  
  ReturnStatus = EFI_SUCCESS;
  
  FabricCxlManager = NULL;
  LocateHeapParams.BufferHandle = HYGON_CXL_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == HGPI_SUCCESS) {
    FabricCxlManager = (FABRIC_CXL_MANAGER *) LocateHeapParams.BufferPtr;
  }
  if (FabricCxlManager == NULL) {
    return EFI_ABORTED;
  }
    
  if (Target.TgtType == TARGET_PCI_BUS) {
    for (Register = CFGADDRESSMAP0_REG; Register <= CFGADDRESSMAP15_REG; Register += 4) {
      BusMap.Value = FabricRegisterAccRead (0, 0, CFGADDRESSMAP_FUNC, Register, FABRIC_REG_ACC_BC);
      if ((BusMap.Field.RE == 1) && (BusMap.Field.WE == 1) && 
          (BusMap.Field.BusNumLimit >= Target.PciBusNum) && 
          (BusMap.Field.BusNumBase <= Target.PciBusNum)) { 
        DstFabricID = (UINT16) BusMap.Field.DstFabricID;
        break;
      }
    }

    if (Register > CFGADDRESSMAP15_REG) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  Error : Bus not found ! \n");  
      return EFI_ABORTED;
    }
    ASSERT (DstFabricID != 0xFFFF);
    FabricTopologyGetLocationFromFabricId (DstFabricID, (UINT32*)&SocketId, (UINT32*)&DieId, (UINT32*)&RbId);
  } else {
    SocketId = (UINT16) (Target.SocketId);
    DieId = (UINT16) (Target.DieId);
    RbId = (UINT16) (Target.RbId);
  }
  
  IDS_HDT_CONSOLE (CPU_TRACE, "  Allocate CXL memory space for socket %d IOD %d Rb %d \n", SocketId, DieId, RbId);
  
  for (RegionIndex = 0; RegionIndex < FabricCxlManager->MemRegionCount; RegionIndex++) {
    MemRegion = &FabricCxlManager->MemRegion[RegionIndex];
    if ((MemRegion->SocketMap & (1 << SocketId)) &&
        (MemRegion->DieMap & (1 << DieId)) &&
        (MemRegion->NbioMap & (1 << RbId))) {
      //Found CXL memory region
      if (MemRegion->IntlvType == NoIntLv) {
        //Calc align pad length
        AvailBaseAddress = MemRegion->Base + MemRegion->UsedSize;
        if ((AvailBaseAddress & Alignment) != 0) {
          AlignPadLength = Alignment - (AvailBaseAddress & Alignment);
        } else {
          AlignPadLength = 0;
        }
        IDS_HDT_CONSOLE (CPU_TRACE, "  AlignPadLength is 0x%X\n", AlignPadLength);
        
        if ((*Length + AlignPadLength + MemRegion->UsedSize) <= MemRegion->Size) {
          *HostBaseAddress = AvailBaseAddress + AlignPadLength;
          *HdmBaseAddress = *HostBaseAddress - MemRegion->Base;
          MemRegion->UsedSize += *Length + AlignPadLength;
          IDS_HDT_CONSOLE (CPU_TRACE, "  Allocate Host CXL memory from 0x%lX ~ 0x%lX \n", *HostBaseAddress, (*HostBaseAddress + *Length - 1));
          IDS_HDT_CONSOLE (CPU_TRACE, "  Allocate HDM CXL memory from 0x%lX ~ 0x%lX \n", *HdmBaseAddress, (*HdmBaseAddress + *Length - 1));
        } else {
          *Length = MemRegion->Size - MemRegion->UsedSize - AlignPadLength;
          ReturnStatus = EFI_OUT_OF_RESOURCES;
          IDS_HDT_CONSOLE (CPU_TRACE, "  No enough CXL memory space, the biggest size is 0x%X\n", *Length);
        }
      }
      break;
    }
  }
  
  if (RegionIndex >= FabricCxlManager->MemRegionCount) {
    ReturnStatus = EFI_NOT_FOUND;
    IDS_HDT_CONSOLE (CPU_TRACE, "  Not found CXL memory region \n");
  }
  return ReturnStatus;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricCxlGetAvailableResource
 *
 * Get available DF resource (CXL memory) for each RB
 *
 * @param[in]         This                  Function pointer to HYGON_CXL_MANAGER_PROTOCOL.
 * @param[in, out]    ResourceSizeForEachRb Avaiable DF resource (MMIO/IO) for each RB
 *
 * @retval            EFI_SUCCESS           Success to get available resource
 *                    EFI_ABORTED           Can't get information of MMIO or IO
 */
EFI_STATUS
EFIAPI
FabricCxlGetAvailableResource (
  IN       HYGON_CXL_MANAGER_PROTOCOL           *This,
  IN       FABRIC_CXL_AVAIL_RESOURCE            *CxlResourceSize
  )
{
  LOCATE_HEAP_PTR           LocateHeapParams;
  FABRIC_CXL_MANAGER       *FabricCxlManager;
  UINT8                     RegionIndex;
  FABRIC_CXL_MEM_RESOURCE  *MemRegion;
   
  FabricCxlManager = NULL;
  LocateHeapParams.BufferHandle = HYGON_CXL_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == HGPI_SUCCESS) {
    FabricCxlManager = (FABRIC_CXL_MANAGER *) LocateHeapParams.BufferPtr;
  }
  if (FabricCxlManager == NULL) {
    return EFI_ABORTED;
  }
  
  CxlResourceSize->MemRegionCount = FabricCxlManager->MemRegionCount;
  
  for (RegionIndex = 0; RegionIndex < FabricCxlManager->MemRegionCount; RegionIndex++) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Available Region Index %d \n", RegionIndex);
    MemRegion = &CxlResourceSize->MemRegion[RegionIndex];
    
    MemRegion->SocketMap = FabricCxlManager->MemRegion[RegionIndex].SocketMap;
    MemRegion->DieMap = FabricCxlManager->MemRegion[RegionIndex].DieMap;
    MemRegion->NbioMap = FabricCxlManager->MemRegion[RegionIndex].NbioMap;
    MemRegion->IntlvType = FabricCxlManager->MemRegion[RegionIndex].IntlvType;
    MemRegion->IntlvSize = FabricCxlManager->MemRegion[RegionIndex].IntlvSize;
    MemRegion->Alignment = FabricCxlManager->MemRegion[RegionIndex].Alignment;
    MemRegion->Base = FabricCxlManager->MemRegion[RegionIndex].Base + FabricCxlManager->MemRegion[RegionIndex].UsedSize;
    MemRegion->Size = FabricCxlManager->MemRegion[RegionIndex].Size - FabricCxlManager->MemRegion[RegionIndex].UsedSize;
    
    IDS_HDT_CONSOLE (MAIN_FLOW, "  SocketMap       : 0x%X \n", MemRegion->SocketMap);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  DieMap          : 0x%X \n", MemRegion->DieMap);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  NbioMap         : 0x%X \n", MemRegion->NbioMap);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  IntlvType       : 0x%X \n", MemRegion->IntlvType);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  IntlvSize       : 0x%X \n", MemRegion->IntlvSize);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Alignment       : 0x%X \n", MemRegion->Alignment);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Avail Base      : 0x%lX \n", MemRegion->Base);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Avail Size      : 0x%lX \n", MemRegion->Size);
  }
  
  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricCxlGetUsedResource
 *
 * Get used DF resource (CXL memory) for each RB
 *
 * @param[in]         This                  Function pointer to HYGON_CXL_MANAGER_PROTOCOL.
 * @param[in, out]    ResourceSizeForEachRb Avaiable DF resource (MMIO/IO) for each RB
 *
 * @retval            EFI_SUCCESS           Success to get available resource
 *                    EFI_ABORTED           Can't get information of MMIO or IO
 */
EFI_STATUS
EFIAPI
FabricCxlGetUsedResource (
  IN       HYGON_CXL_MANAGER_PROTOCOL           *This,
  IN       FABRIC_CXL_USED_RESOURCE             *CxlResourceSize
  )
{
  LOCATE_HEAP_PTR           LocateHeapParams;
  FABRIC_CXL_MANAGER       *FabricCxlManager;
  UINT8                     RegionIndex;
  FABRIC_CXL_MEM_RESOURCE  *MemRegion;
   
  FabricCxlManager = NULL;
  LocateHeapParams.BufferHandle = HYGON_CXL_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == HGPI_SUCCESS) {
    FabricCxlManager = (FABRIC_CXL_MANAGER *) LocateHeapParams.BufferPtr;
  }
  if (FabricCxlManager == NULL) {
    return EFI_ABORTED;
  }
  
  CxlResourceSize->MemRegionCount = FabricCxlManager->MemRegionCount;
  
  for (RegionIndex = 0; RegionIndex < FabricCxlManager->MemRegionCount; RegionIndex++) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Used Region Index %d \n", RegionIndex);
    MemRegion = &CxlResourceSize->MemRegion[RegionIndex];
    
    MemRegion->SocketMap = FabricCxlManager->MemRegion[RegionIndex].SocketMap;
    MemRegion->DieMap = FabricCxlManager->MemRegion[RegionIndex].DieMap;
    MemRegion->NbioMap = FabricCxlManager->MemRegion[RegionIndex].NbioMap;
    MemRegion->IntlvType = FabricCxlManager->MemRegion[RegionIndex].IntlvType;
    MemRegion->IntlvSize = FabricCxlManager->MemRegion[RegionIndex].IntlvSize;
    MemRegion->Alignment = FabricCxlManager->MemRegion[RegionIndex].Alignment;
    MemRegion->Base = FabricCxlManager->MemRegion[RegionIndex].Base;
    MemRegion->Size = FabricCxlManager->MemRegion[RegionIndex].UsedSize;
    
    //IDS_HDT_CONSOLE (MAIN_FLOW, "  SocketMap       : 0x%X \n", MemRegion->SocketMap);
    //IDS_HDT_CONSOLE (MAIN_FLOW, "  DieMap          : 0x%X \n", MemRegion->DieMap);
    //IDS_HDT_CONSOLE (MAIN_FLOW, "  NbioMap         : 0x%X \n", MemRegion->NbioMap);
    //IDS_HDT_CONSOLE (MAIN_FLOW, "  IntlvType       : 0x%X \n", MemRegion->IntlvType);
    //IDS_HDT_CONSOLE (MAIN_FLOW, "  IntlvSize       : 0x%X \n", MemRegion->IntlvSize);
    //IDS_HDT_CONSOLE (MAIN_FLOW, "  Alignment       : 0x%X \n", MemRegion->Alignment);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Used Base       : 0x%lX \n", MemRegion->Base);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Used Size       : 0x%lX \n", MemRegion->Size);
  }
  
  return EFI_SUCCESS;
}

VOID
CxlMemoryTest (
  VOID
)
{
  LOCATE_HEAP_PTR           LocateHeapParams;
  FABRIC_CXL_MANAGER       *FabricCxlManager;
  UINT8                     RegionIndex;
  UINT64                    CxlMemStart;
  UINT64                    *MemPointer;
  UINT64                    MemDataValue;
  UINT64                    EndPointer;
  UINT64                    MismatchCount;
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "CxlMemoryTest Entry \n");
   
  FabricCxlManager = NULL;
  LocateHeapParams.BufferHandle = HYGON_CXL_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == HGPI_SUCCESS) {
    FabricCxlManager = (FABRIC_CXL_MANAGER *) LocateHeapParams.BufferPtr;
  }
  if (FabricCxlManager == NULL) {
    return;
  }
  
  for (RegionIndex = 0; RegionIndex < FabricCxlManager->MemRegionCount; RegionIndex++) {
    if (FabricCxlManager->MemRegion[RegionIndex].UsedSize != 0) {
      CxlMemStart = FabricCxlManager->MemRegion[RegionIndex].Base;
      
      //Fill CXL memory
      MemPointer = (UINT64 *)CxlMemStart;
      EndPointer = TEST_CXL_MEM_SIZE + CxlMemStart;
      do {
        MemDataValue = CXLMEM_TEST_PATTERN | (UINT64)MemPointer;
        *MemPointer = MemDataValue;
        MemPointer++;
        if (0 == ((UINT64) MemPointer & TEST_PROGRESS_UNIT)) {
          IDS_HDT_CONSOLE (GNB_TRACE, "* ");
        }
      } while ((UINT64) MemPointer < EndPointer);
      
      IDS_HDT_CONSOLE (MAIN_FLOW, "\n");
      
      //Check CXL memory
      MemPointer = (UINT64 *)CxlMemStart;
      MismatchCount = 0;
      do {
        MemDataValue = CXLMEM_TEST_PATTERN | (UINT64)MemPointer;
        if (*MemPointer != MemDataValue) {
          IDS_HDT_CONSOLE (MAIN_FLOW, "CXL Memory test failed at 0x%lX\n", (UINT64)MemPointer);
          IDS_HDT_CONSOLE (MAIN_FLOW, "  Expected 0x%lX  Found 0x%lX\n", MemDataValue, *MemPointer);
          if (MismatchCount >=  MAX_MISMATCH_COUNT) {
            IDS_HDT_CONSOLE (MAIN_FLOW, "Stop test. The data error has reached the upper limit!!\n");
            break;
          }
          MismatchCount++;
        }
        MemPointer++;
      } while ((UINT64) MemPointer < EndPointer);
    }
  }
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "CxlMemoryTest Exit \n");
}
