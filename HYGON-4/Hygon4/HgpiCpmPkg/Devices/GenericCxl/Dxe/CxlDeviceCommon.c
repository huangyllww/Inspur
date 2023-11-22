/** @file

Cxl 1.1 and 2.0 Device common Handler.

**/
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

#include "CxlDeviceDriver.h"
#include <Library/BaseLib.h>
#include <Library/DxeServicesTableLib.h>

GLOBAL_REMOVE_IF_UNREFERENCED CONST CHAR8 *mEfiMemoryTypeName[] = {
  "EfiReservedMemoryType",
  "EfiLoaderCode",
  "EfiLoaderData",
  "EfiBootServicesCode",
  "EfiBootServicesData",
  "EfiRuntimeServicesCode",
  "EfiRuntimeServicesData",
  "EfiConventionalMemory",
  "EfiUnusableMemory",
  "EfiACPIReclaimMemory",
  "EfiACPIMemoryNVS",
  "EfiMemoryMappedIO",
  "EfiMemoryMappedIOPortSpace",
  "EfiPalCode",
  "EfiPersistentMemory",
  "EfiMaxMemoryType"
};

GLOBAL_REMOVE_IF_UNREFERENCED CONST CHAR8 *mGcdMemoryTypeName[] = {
  "EfiGcdMemoryTypeNonExistent",
  "EfiGcdMemoryTypeReserved",
  "EfiGcdMemoryTypeSystemMemory",
  "EfiGcdMemoryTypeMemoryMappedIo",
  "EfiGcdMemoryTypePersistent",
  "EfiGcdMemoryTypeMoreReliable",
  "EfiGcdMemoryTypeMaximum"
};

CONST CHAR8 *
CxlMgrGcdMemoryTypeString (
  EFI_GCD_MEMORY_TYPE  GcdMemoryType
  )
{
  if (GcdMemoryType > EfiGcdMemoryTypeMaximum) {
    GcdMemoryType = EfiGcdMemoryTypeMaximum;
  }
  return mGcdMemoryTypeName[(UINTN)GcdMemoryType];
}

CONST CHAR8 *
CxlMgrEfiMemoryTypeString (
  EFI_MEMORY_TYPE  EfiMemoryType
  )
{
  if (EfiMemoryType > EfiMaxMemoryType) {
    EfiMemoryType = EfiMaxMemoryType;
  }
  return mEfiMemoryTypeName[(UINTN)EfiMemoryType];
}

/**
 *---------------------------------------------------------------------------------------
 *  CxlEnableMmioAccess
 *
 *  Description:
 *     Enable CXL MMIO space access
 *  Parameters:
 *    @param[in]     *PciIo      EFI_PCI_IO_PROTOCOL pointer
 *
 *---------------------------------------------------------------------------------------
 **/
void CxlEnableMmioAccess (
  IN EFI_PCI_IO_PROTOCOL  *PciIo
  )
{
  EFI_STATUS           Status;
  UINT32               Value32;
  
  Status = PciIo->Pci.Read (
    PciIo,
    EfiPciIoWidthUint32,
    PCI_COMMAND_OFFSET,
    1,
    (VOID *)&Value32
    );
  if (!EFI_ERROR (Status)) {
    if ((Value32 & EFI_PCI_COMMAND_MEMORY_SPACE) == 0) {
      DEBUG ((DEBUG_INFO, "  Cxl Enable Mmio Access \n"));
      
      Value32 |= EFI_PCI_COMMAND_MEMORY_SPACE;
      Status = PciIo->Pci.Write (
        PciIo,
        EfiPciIoWidthUint32,
        PCI_COMMAND_OFFSET,
        1,
        (VOID *)&Value32
        );
    }
  }
  
  return;
}

/**
 *---------------------------------------------------------------------------------------
 *  DumpCxlAvailResource
 *
 *  Description:
 *     Print CXL DF available CXL memory resource
 *  Parameters:
 *    @param[in]     *CxlResourceSize      FABRIC_CXL_AVAIL_RESOURCE pointer
 *
 *---------------------------------------------------------------------------------------
 **/
void DumpCxlAvailResource (
  IN   FABRIC_CXL_AVAIL_RESOURCE  *CxlResourceSize
  )
{
  UINT8                     RegionIndex;
  FABRIC_CXL_MEM_RESOURCE  *MemRegion;
  
  for (RegionIndex = 0; RegionIndex < CxlResourceSize->MemRegionCount; RegionIndex++) {    
    DEBUG ((DEBUG_INFO, "  Region Index %d \n", RegionIndex));
    MemRegion = &CxlResourceSize->MemRegion[RegionIndex];
    DEBUG ((DEBUG_INFO, "    SocketMap       : 0x%X \n", MemRegion->SocketMap));
    DEBUG ((DEBUG_INFO, "    DieMap          : 0x%X \n", MemRegion->DieMap));
    DEBUG ((DEBUG_INFO, "    NbioMap         : 0x%X \n", MemRegion->NbioMap));
    DEBUG ((DEBUG_INFO, "    IntlvType       : 0x%X \n", MemRegion->IntlvType));
    DEBUG ((DEBUG_INFO, "    IntlvSize       : 0x%X \n", MemRegion->IntlvSize));
    DEBUG ((DEBUG_INFO, "    Alignment       : 0x%X \n", MemRegion->Alignment));
    DEBUG ((DEBUG_INFO, "    Base            : 0x%lX \n", MemRegion->Base));
    DEBUG ((DEBUG_INFO, "    Size            : 0x%lX \n", MemRegion->Size));
  }
}

/**
 *---------------------------------------------------------------------------------------
 *  CxlGetMemPoolSize
 *
 *  Description:
 *     Read CXL memory range registers to get CXL memory size
 *  Parameters:
 *    @param[in]     *CxlDevice      CXL_DEVICE  pointer
 *
 *---------------------------------------------------------------------------------------
 **/
void CxlGetMemPoolSize (
  IN CXL_DEVICE                *CxlDevice
  )
{
  EFI_STATUS                                     Status;
  UINT16                                         HdmIndex;
  CXL_DVSEC_FLEX_BUS_DEVICE_CAPABILITY           FlexBusDevCap;
  CXL_DVSEC_FLEX_BUS_DEVICE_RANGE1_SIZE_LOW      RangeSizeLow;
  CXL_DVSEC_FLEX_BUS_DEVICE_RANGE1_SIZE_HIGH     RangeSizeHigh;
  EFI_PCI_IO_PROTOCOL                           *PciIo;
  
  DEBUG ((DEBUG_INFO, "  CxlGetMemPoolSize Entry \n"));
  
  PciIo = CxlDevice->StartPciIo;
  
  CxlDevice->MemPool[0].Size = 0;
  CxlDevice->MemPool[1].Size = 0;
  CxlDevice->HostMemPool.Size = 0;
  
  if (CxlDevice->FlexBusDeviceCapPtr == 0) {
    DEBUG ((DEBUG_INFO, "    Not found Flex Bus Device capability \n"));
    return;
  }
 
  Status = PciIo->Pci.Read (
    PciIo,
    EfiPciIoWidthUint16,
    CxlDevice->FlexBusDeviceCapPtr + CXL_FLEX_BUS_CAP_OFFSET,
    1,
    (VOID *)&FlexBusDevCap.Uint16
    );
  
  if (FlexBusDevCap.Bits.MemCapable == 0) {
    DEBUG ((DEBUG_INFO, "    CXL memory link failure \n"));
    return;
  }
  
  //1: CXL memory is initialized by hardware and firmware, then set MemoryActive within 1 second of reset deassertion
  //0: CXL memory is initialized by UEFI device driver or OS device driver, then set MemoryActive
  CxlDevice->MemHwInitMode = (UINT8) FlexBusDevCap.Bits.MemHwInitMode;
  DEBUG ((DEBUG_INFO, "    MemHwInitMode is %d \n", FlexBusDevCap.Bits.MemHwInitMode));  
  
  //computes the size of the HDM range
  CxlDevice->MemPoolCount = FlexBusDevCap.Bits.HdmCount;
  DEBUG ((DEBUG_INFO, "    HDM count %d \n", CxlDevice->MemPoolCount));
  
  for (HdmIndex = 0; HdmIndex < CxlDevice->MemPoolCount; HdmIndex++) {
    Status = PciIo->Pci.Read (
      PciIo,
      EfiPciIoWidthUint32,
      (CxlDevice->FlexBusDeviceCapPtr + CXL_FLEX_BUS_RANGE1_SIZE_LOW_OFFSET + HdmIndex * 0x10),
      1,
      (VOID *)&RangeSizeLow.Uint32
      );

    DEBUG ((DEBUG_INFO, "    Range %d SizeLow = 0x%X \n", HdmIndex, RangeSizeLow.Uint32));
    
    Status = PciIo->Pci.Read (
      PciIo,
      EfiPciIoWidthUint32,
      (CxlDevice->FlexBusDeviceCapPtr + CXL_FLEX_BUS_RANGE1_SIZE_HIGH_OFFSET + HdmIndex * 0x10),
      1,
      (VOID *)&RangeSizeHigh.Uint32
      );
    DEBUG ((DEBUG_INFO, "    Range %d SizeHigh = 0x%X \n", HdmIndex, RangeSizeHigh.Uint32));
    
    if (RangeSizeLow.Bits.MemoryInfoValid == 1) {
      CxlDevice->MemPool[HdmIndex].Size = LShiftU64 ((UINT64)(RangeSizeHigh.Uint32), 32) + (UINT64)(RangeSizeLow.Uint32 & 0xF0000000);
      CxlDevice->MemPool[HdmIndex].EfiType = EfiConventionalMemory;
      CxlDevice->MemPool[HdmIndex].MediaType = (UINT8) RangeSizeLow.Bits.MediaType;
      CxlDevice->MemPool[HdmIndex].MemoryActive = (BOOLEAN) RangeSizeLow.Bits.MemoryActive;
      CxlDevice->MemPool[HdmIndex].MemoryClass = (UINT8) RangeSizeLow.Bits.MemoryClass;
    } else {
      CxlDevice->MemPool[HdmIndex].Size = 0;
      CxlDevice->MemPool[HdmIndex].EfiType = 2;
      CxlDevice->MemPool[HdmIndex].MediaType = 0;
      CxlDevice->MemPool[HdmIndex].MemoryActive = 0;
      CxlDevice->MemPool[HdmIndex].MemoryClass = 0;
    }

    DEBUG ((DEBUG_INFO, "    HDM %d Memory Range Size 0x%lX, Active %d \n", HdmIndex, CxlDevice->MemPool[HdmIndex].Size, CxlDevice->MemPool[HdmIndex].MemoryActive)); 
    CxlDevice->HostMemPool.Size += CxlDevice->MemPool[HdmIndex].Size;
  }
 
  DEBUG ((DEBUG_INFO, "  CxlGetMemPoolSize Exit \n"));
  return;
}

/**
 *---------------------------------------------------------------------------------------
 *  CxlEnableMemPool
 *
 *  Description:
 *     Allocate memory space for CXL memory pool and enable them
 *  Parameters:
 *    @param[in]     *CxlDevice      CXL_DEVICE  pointer
 *
 *---------------------------------------------------------------------------------------
 **/
EFI_STATUS 
CxlEnableMemPool (
  IN CXL_DEVICE                *CxlDevice
  )
{
  EFI_STATUS                                 Status;
  UINT64                                     CxlHostMemSize;
  UINT64                                     CxlHostMemBase;
  UINT64                                     CxlHdmMemBase;
  FABRIC_CXL_TARGET                          Target;
  CXL_DVSEC_FLEX_BUS_DEVICE_RANGE1_BASE_LOW  RangeBaseLow;
  CXL_DVSEC_FLEX_BUS_DEVICE_RANGE1_BASE_HIGH RangeBaseHigh;
  EFI_PCI_IO_PROTOCOL                       *PciIo;
  UINT16                                     HdmIndex;
  CXL_DVSEC_FLEX_BUS_DEVICE_CONTROL          FlexBusDevControl;
  UINT8                                      ReportCxlMemAttr;
  
  DEBUG ((DEBUG_INFO, "  CxlEnableMemPool Entry \n"));
  
  Status = EFI_SUCCESS;
  CxlHostMemBase = 0;
  CxlHdmMemBase = 0;
  PciIo = CxlDevice->StartPciIo;
  
  CxlHostMemSize = CxlDevice->HostMemPool.Size;
  DEBUG ((DEBUG_INFO, "    Cxl Device HostMemPool size = 0x%lX \n", CxlHostMemSize)); 
  
  if (CxlHostMemSize == 0) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }
  
  if (gCxlMgrProtocol == NULL) {
    DEBUG ((DEBUG_INFO, "    gCxlMgrProtocol is NULL \n"));
    Status = EFI_ABORTED;
    goto ON_EXIT;
  }
  
  //Allocate Memory space for CXL device
  Target.TgtType = TARGET_PCI_BUS;
  Target.PciBusNum = (UINT16) CxlDevice->PciAddr.Address.Bus;
  Status = gCxlMgrProtocol->FabricCxlAllocateMem (gCxlMgrProtocol, &CxlHostMemBase, &CxlHdmMemBase, &CxlHostMemSize, ALIGN_256M, Target);
  if ((CxlDevice->FlexBusDeviceCapPtr != 0) && (Status == EFI_SUCCESS)) {
    //Set Memory range base
    CxlDevice->HostMemPool.Base = CxlHostMemBase;
    
    for (HdmIndex = 0; HdmIndex < CxlDevice->MemPoolCount; HdmIndex++) {
      CxlDevice->MemPool[HdmIndex].Base = CxlHdmMemBase;
      DEBUG ((DEBUG_INFO, "    Set CXL HDM %d memory base = 0x%lX \n", HdmIndex, CxlHdmMemBase));
      
      RangeBaseHigh.Uint32 = (UINT32) RShiftU64 (CxlHdmMemBase, 32);
      Status = PciIo->Pci.Write (
        PciIo,
        EfiPciIoWidthUint32,
        (CxlDevice->FlexBusDeviceCapPtr + CXL_FLEX_BUS_RANGE1_BASE_HIGH_OFFSET + HdmIndex * 0x10),
        1,
        (VOID *)&RangeBaseHigh.Uint32
        );

      RangeBaseLow.Uint32 = ((UINT32) CxlHdmMemBase) & 0xF0000000;
      Status = PciIo->Pci.Write (
              PciIo,
              EfiPciIoWidthUint32,
              (CxlDevice->FlexBusDeviceCapPtr + CXL_FLEX_BUS_RANGE1_BASE_LOW_OFFSET + HdmIndex * 0x10),
              1,
              (VOID *)&RangeBaseLow.Uint32
              );
      
      CxlHdmMemBase += CxlDevice->MemPool[HdmIndex].Size;
    }
    
    //Set CXL.mem Enable. 
    //Any subsequent accesses to HDM are decoded and routed to the local memory by the device.
    //CXL memory can be read or write when MemoryActive = 1
    //CXL memory reads shall return all 1's and the writes will be dropped when MemoryActive = 0
    DEBUG ((DEBUG_INFO, "  Enable CXL memory \n")); 
    Status = PciIo->Pci.Read (
      PciIo,
      EfiPciIoWidthUint16,
      (CxlDevice->FlexBusDeviceCapPtr + CXL_FLEX_BUS_CONTROL_OFFSET),
      1,
      (VOID *)&FlexBusDevControl.Uint16
      );
    
    FlexBusDevControl.Bits.MemEnable = 1;
    
    Status = PciIo->Pci.Write (
      PciIo,
      EfiPciIoWidthUint16,
      (CxlDevice->FlexBusDeviceCapPtr + CXL_FLEX_BUS_CONTROL_OFFSET),
      1,
      (VOID *)&FlexBusDevControl.Uint16
      );
    
    //Convert memory pool
    ReportCxlMemAttr = PcdGet8 (PcdCxlMemAttr);
    if (ReportCxlMemAttr == 1) {
      DEBUG ((DEBUG_INFO, "  Change CXL memory attribute to WB|WT|WC|UC \n"));
      Status = CxlConvertMemoryPool (
                CxlHostMemBase,
                CxlHostMemSize,
                EfiGcdMemoryTypeSystemMemory,
                (EFI_MEMORY_WB | EFI_MEMORY_WT | EFI_MEMORY_WC | EFI_MEMORY_UC)
                );
    } else if (ReportCxlMemAttr == 2) {
      DEBUG ((DEBUG_INFO, "  Change CXL memory attribute to WB|WT|WC|UC|SP \n"));
      Status = CxlConvertMemoryPool (
                CxlHostMemBase,
                CxlHostMemSize,
                EfiGcdMemoryTypeSystemMemory,
                (EFI_MEMORY_WB | EFI_MEMORY_WT | EFI_MEMORY_WC | EFI_MEMORY_UC | EFI_MEMORY_SP)
                );      
    }
  }
  
ON_EXIT:
  DEBUG ((DEBUG_INFO, "  CxlEnableMemPool Exit \n"));
  return Status;
}

/**
 *---------------------------------------------------------------------------------------
 *  CxlLockDvsec
 *
 *  Description:
 *    Lock CXL DVSEC registers, all register fields in the PCIe DVSEC for CXL Devices
 *    Capability with RWL attribute become read only
 *  Parameters:
 *    @param[in]     *CxlDevice      CXL_DEVICE  pointer
 *
 *---------------------------------------------------------------------------------------
 **/
EFI_STATUS
CxlLockDvsec (
  IN CXL_DEVICE                *CxlDevice
  )
{
  EFI_STATUS                      Status;
  CXL_DVSEC_FLEX_BUS_DEVICE_LOCK  DeviceLock;
  EFI_PCI_IO_PROTOCOL            *PciIo;
  
  PciIo = CxlDevice->StartPciIo;
  
  if (CxlDevice->FlexBusDeviceCapPtr != 0) {
    DEBUG ((DEBUG_INFO, "  Lock CXL DVSEC registers \n")); 
    Status = PciIo->Pci.Read (
      PciIo,
      EfiPciIoWidthUint16,
      (CxlDevice->FlexBusDeviceCapPtr + CXL_FLEX_BUS_LOCK_OFFSET),
      1,
      (VOID *)&DeviceLock.Uint16
      );
    
    DeviceLock.Bits.ConfigLock = 1;
    
    Status = PciIo->Pci.Write (
      PciIo,
      EfiPciIoWidthUint16,
      (CxlDevice->FlexBusDeviceCapPtr + CXL_FLEX_BUS_LOCK_OFFSET),
      1,
      (VOID *)&DeviceLock.Uint16
      );
  }
  
  return Status;
}

/**
 *---------------------------------------------------------------------------------------
 *  CxlConvertMemoryPool
 *
 *  Description:
 *     Convert CXL memory space to UEFI memory type and attribute
 *  Parameters:
 *    @param[in]     *CxlDevice      CXL_DEVICE  pointer
 *
 *---------------------------------------------------------------------------------------
 **/
EFI_STATUS 
CxlConvertMemoryPool (
  IN UINT64                     Base,
  IN UINT64                     Size,
  IN EFI_GCD_MEMORY_TYPE        GcdMemoryType,
  IN UINT64                     Attributes
  )
{
  EFI_STATUS                       Status;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR  GcdDescriptor;
  
  DEBUG ((DEBUG_INFO, "  %a - ENTRY\n", __FUNCTION__));
  DEBUG ((DEBUG_INFO, "    Memory Pool: 0x%lX - 0x%lX\n", Base, Base + Size - 1));
  DEBUG ((DEBUG_INFO, "    Requested: GcdMemoryType = %d (%a), Attributes = 0x%lX\n",
    GcdMemoryType,
    CxlMgrGcdMemoryTypeString (GcdMemoryType),
    Attributes));

  // Get current GCD parameters
  Status = gDS->GetMemorySpaceDescriptor (Base, &GcdDescriptor);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "    Error: gDS->GetMemorySpaceDescriptor () failed! \n"));
    goto ON_EXIT;
  }
  DEBUG ((DEBUG_INFO, "    Current: GcdMemoryType = %d (%a), Attributes = 0x%lX\n",
    GcdDescriptor.GcdMemoryType,
    CxlMgrGcdMemoryTypeString (GcdDescriptor.GcdMemoryType),
    GcdDescriptor.Attributes));

  // Need to convert Type?
  if (GcdMemoryType != GcdDescriptor.GcdMemoryType) {
    // GCD Type convertion is a two-step process:
    //  Step1: Change to EfiGcdMemoryTypeNonExistent via gDS->RemoveMemorySpace()
    //  Step2: Change to desired GcdMemoryType and Attributes via gDS->AddMemorySpace()
    if (GcdDescriptor.GcdMemoryType != EfiGcdMemoryTypeNonExistent) {
      Status = gDS->RemoveMemorySpace (Base, Size);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "    Error: gDS->RemoveMemorySpace () failed! \n"));
        goto ON_EXIT;
      }
    }
    if (GcdMemoryType != EfiGcdMemoryTypeNonExistent) {
      Status = gDS->AddMemorySpace (GcdMemoryType, Base, Size, Attributes);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "    Error: gDS->AddMemorySpace () failed! \n"));
        goto ON_EXIT;
      }
    }

    // Get converted GCD parameters
    Status = gDS->GetMemorySpaceDescriptor (Base, &GcdDescriptor);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "    Error: gDS->GetMemorySpaceDescriptor () failed! \n"));
      goto ON_EXIT;
    }
    DEBUG ((DEBUG_INFO, "    After GcdMemoryType convertion: GcdMemoryType = %d (%a), Attributes = 0x%lX\n",
      GcdDescriptor.GcdMemoryType,
      CxlMgrGcdMemoryTypeString (GcdDescriptor.GcdMemoryType),
      GcdDescriptor.Attributes));
  }

  // Need to change Attributes?
  if (Attributes != GcdDescriptor.Attributes && GcdMemoryType != EfiGcdMemoryTypeNonExistent) {
    Status = gDS->SetMemorySpaceAttributes (Base, Size, Attributes);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "    Error: gDS->SetMemorySpaceAttributes () failed! \n"));
      goto ON_EXIT;
    }

    // Get converted GCD parameters
    Status = gDS->GetMemorySpaceDescriptor (Base, &GcdDescriptor);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "    Error: gDS->GetMemorySpaceDescriptor () failed! \n"));
      goto ON_EXIT;
    }
    DEBUG ((DEBUG_INFO, "    After Attributes convertion: GcdMemoryType = %d (%a), Attributes = 0x%lX\n",
      GcdDescriptor.GcdMemoryType,
      CxlMgrGcdMemoryTypeString (GcdDescriptor.GcdMemoryType),
      GcdDescriptor.Attributes));
  }

ON_EXIT:
  DEBUG ((DEBUG_INFO, "  %a - EXIT (Status = %r)\n", __FUNCTION__, Status));
  return Status;
}

