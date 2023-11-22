//# @file/
//#
//# Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
//# All rights reserved.This software and associated documentation (if any)
//# is furnished under a license and may only be used or copied in
//# accordance with the terms of the license. Except as permitted by such
//# license, no part of this software or documentation may be reproduced,
//# stored in a retrieval system, or transmitted in any form or by any
//# means without the express written consent of Byosoft Corporation.
//#
//# File Name:
//#   MemDetect.inf
//#
//# Abstract:
//#
//#
//# Revision History:
//#
//##

#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Guid/HygonMemoryInfoHob.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/MasterBootMode.h>
#include <Library/PlatformCommLib.h>
#include <Library/PeiServicesLib.h>
#include <PlatS3Record.h>
#include <HygonCpu.h>
#include <Library/ByoHygonFabricLib.h>
#include <Ppi/FabricTopologyServicesPpi.h>
#include <Library/HygonSocBaseLib.h>



#define SIZE_16M_ALIGN                  0xFFFFFFFFFF000000l


typedef struct {
  UINT64      Base;
  UINT64      Size;
} EFI_PEI_SAVE_DTOM2_PPI;


#define  MEM_DET_COMMON_MEM_ATTR     (EFI_RESOURCE_ATTRIBUTE_PRESENT | \
                                    EFI_RESOURCE_ATTRIBUTE_INITIALIZED | \
                                    EFI_RESOURCE_ATTRIBUTE_TESTED | \
                                    EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |\
                                    EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE | \
                                    EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE | \
                                    EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE)

#define  MEM_DET_COMMON_MEM_ATTR_ABOVE4G     (EFI_RESOURCE_ATTRIBUTE_PRESENT | \
                                    EFI_RESOURCE_ATTRIBUTE_INITIALIZED | \
                                    EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |\
                                    EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE | \
                                    EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE | \
                                    EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE)


#define  SYS_MMIO_ATTRIB            (EFI_RESOURCE_ATTRIBUTE_PRESENT | \
                                    EFI_RESOURCE_ATTRIBUTE_INITIALIZED | \
                                    EFI_RESOURCE_ATTRIBUTE_TESTED | \
                                    EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE)

#define  SYS_IO_ATTRIB              (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED)


typedef struct {
  UINT32    Base;
  UINT32    Size;
} NVDIMM_INFO;




STATIC EFI_PEI_PPI_DESCRIPTOR  gMemDetectPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gPeiInstallPeiMemoryPpiGuid,
    NULL
  }
};





VOID 
GetSysPciHostBusInfo (
  CONST EFI_PEI_SERVICES   **PeiServices,
        PLATFORM_COMM_INFO *Info
  )
{
  HYGON_PEI_FABRIC_TOPOLOGY_SERVICES_PPI         *FabricTopologyServicesPpi;
  UINTN                                          NumberOfProcessors;
  UINTN                                          TotalNumberOfDie;
  UINTN                                          TotalNumberOfPhysicalDie;
  UINTN                                          TotalNumberOfLogicalDie;
  UINTN                                          TotalNumberOfRootBridges;
  UINTN                                          SocketIndex;
  UINTN                                          DieIndex;
  UINTN                                          RootBridgeIndex;
  EFI_STATUS                                     Status;
  UINTN                                          SystemFabricID;
  UINTN                                          BusNumberBase;
  UINTN                                          BusNumberLimit;
  UINTN                                          Index;
  UINTN                                          HostCount;
  UINTN                                          SystemIdOffset;  
  HYGON_FABRIC_TOPOLOGY_DIE_DEVICE_MAP           *FabricIdMap;


  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonFabricTopologyServicesPpiGuid,
                             0,
                             NULL,
                             &FabricTopologyServicesPpi
                             );
  ASSERT(!EFI_ERROR(Status));
  
  FabricTopologyServicesPpi->GetSystemInfo (
                               &NumberOfProcessors, 
                               &TotalNumberOfDie, 
                               &TotalNumberOfRootBridges
                               );
  Index = 0;
  HostCount = 0;
  for (SocketIndex = 0; SocketIndex < NumberOfProcessors; SocketIndex++) {
    FabricTopologyServicesPpi->GetProcessorInfo (
                                 SocketIndex, 
                                 &TotalNumberOfPhysicalDie, 
                                 &TotalNumberOfLogicalDie, 
                                 &TotalNumberOfRootBridges
                                 );
    DEBUG((EFI_D_INFO, "S[%d] PD:%d, LD:%d, RB:%d\n", SocketIndex, \
      TotalNumberOfPhysicalDie, TotalNumberOfLogicalDie, TotalNumberOfRootBridges));
    
    for (DieIndex = 0; DieIndex < TotalNumberOfLogicalDie; DieIndex++) {
      FabricTopologyServicesPpi->GetDieInfo (
                                   SocketIndex,
                                   DieIndex,
                                   &TotalNumberOfRootBridges,
                                   &SystemIdOffset,
                                   &FabricIdMap
                                   );
      DEBUG((EFI_D_INFO, "  D[%d] PD:%d, LD:%d, RB:%d\n", DieIndex, TotalNumberOfRootBridges));
      for (RootBridgeIndex = 0; RootBridgeIndex < TotalNumberOfRootBridges; RootBridgeIndex++) {
        FabricTopologyServicesPpi->GetRootBridgeInfo (
                                     SocketIndex,
                                     DieIndex,
                                     RootBridgeIndex,
                                     &SystemFabricID,
                                     &BusNumberBase,
                                     &BusNumberLimit
                                     );
        DEBUG((EFI_D_INFO, "    RB[%d] (%x,%x) ID:%x\n", \
          RootBridgeIndex, BusNumberBase, BusNumberLimit, SystemFabricID));

        if(Index < PCI_HOST_MAX_COUNT){
          Info->BusBase[Index]  = (UINT8)BusNumberBase;
          Info->BusLimit[Index] = (UINT8)BusNumberLimit;  
          DEBUG((EFI_D_INFO, "-> RB[%d] Bus(%X,%X)\n", Index, Info->BusBase[Index], Info->BusLimit[Index]));
          Index++;
        }
        HostCount++;
      }
    }
  }

  ASSERT(HostCount <= PCI_HOST_MAX_COUNT);

  Info->PciHostCount = (UINT32)HostCount;
  Info->Sockets      = (UINT32)NumberOfProcessors;
  DEBUG((EFI_D_INFO, "PciHostCount:%d Sockets:%d\n", Info->PciHostCount, Info->Sockets));
}







EFI_STATUS
EFIAPI
MemDetectInit (
    IN  EFI_PEI_FILE_HANDLE      FileHandle,
    IN  CONST EFI_PEI_SERVICES   **PeiServices
)
{
  UINT32                          dTOM = 0;
  UINT64                          TOM2 = 0;
  EFI_STATUS                      Status;
  UINT64                          UMASize = 0;
  UINT64                          UMABase = 0;
  EFI_BOOT_MODE                   BootMode;
  HYGON_MEMORY_INFO_HOB             *MemInfoHob;
  HYGON_MEMORY_RANGE_DESCRIPTOR     *MemRangDesc;
  UINT32                          MemRangeIndex;
  EFI_PEI_SAVE_DTOM2_PPI          SaveDTOM2Ppi = {0};
  VOID                            *HobAddr;
  NVDIMM_INFO                     *NvdimmInfo;
  UINT64                          NvdimmBase;
  UINT64                          NvdimmSize;
  UINT64                          TempSize;
  EFI_HOB_GUID_TYPE               *GuidHob;
  PLATFORM_COMM_INFO              *PlatCommInfo;
  UINT32                          Size;
  UINT64                          Temp1Size = 0, RegionTop64 = 0;
  PLATFORM_S3_RECORD              *S3Record;
  UINTN                           Index;
  UINT64                          MmioSize16MAligned;
  UINT64                          BaseAddr;
  UINT32                          FvBBBase;
  UINT32                          FvBBSize;


  DEBUG((EFI_D_INFO, "MemDetectInit\n"));

  Status = (*PeiServices)->GetBootMode(PeiServices, &BootMode);
  ASSERT_EFI_ERROR(Status);

  GuidHob = GetFirstGuidHob(&gHygonMemoryInfoHobGuid);
  ASSERT_EFI_ERROR(GuidHob != NULL);
  MemInfoHob = (HYGON_MEMORY_INFO_HOB*)GET_GUID_HOB_DATA(GuidHob);

  DEBUG((EFI_D_INFO, "Descriptors: %d\n", MemInfoHob->NumberOfDescriptor));

  MemRangDesc = &MemInfoHob->Ranges[0];
  for(MemRangeIndex = 0; MemRangeIndex < MemInfoHob->NumberOfDescriptor; MemRangeIndex++){

      DEBUG((EFI_D_INFO, "MEM(%lX,%lX) A:%X\n", MemRangDesc[MemRangeIndex].Base, \
                                                MemRangDesc[MemRangeIndex].Size, \
                                                MemRangDesc[MemRangeIndex].Attribute));

      if((MemRangDesc[MemRangeIndex].Attribute == HYGON_MEMORY_ATTRIBUTE_AVAILABLE) &&
         (MemRangDesc[MemRangeIndex].Base < SIZE_4GB)){
        dTOM += (UINT32)MemRangDesc[MemRangeIndex].Size;
      }

      if((MemRangDesc[MemRangeIndex].Base >= SIZE_4GB &&
          (MemRangDesc[MemRangeIndex].Attribute == HYGON_MEMORY_ATTRIBUTE_AVAILABLE))){

          TOM2 += MemRangDesc[MemRangeIndex].Size;
          SaveDTOM2Ppi.Base = MemRangDesc[MemRangeIndex].Base;
          SaveDTOM2Ppi.Size = MemRangDesc[MemRangeIndex].Size;
          RegionTop64 = SaveDTOM2Ppi.Base + SaveDTOM2Ppi.Size;
          HobAddr = GetFirstGuidHob (&gHygonNvdimmInfoHobGuid);
          if (HobAddr != NULL) {
            NvdimmInfo = (NVDIMM_INFO *) GET_GUID_HOB_DATA (HobAddr);
            while (NvdimmInfo->Size != 0) {
              DEBUG((EFI_D_INFO,  "NVDIMM range from APOB: NvdimmBase = %08x, NvdimmSize = %08x\n", NvdimmInfo->Base, NvdimmInfo->Size));
              NvdimmBase = (UINT64) NvdimmInfo->Base;
              NvdimmSize = (UINT64) NvdimmInfo->Size;
              NvdimmBase = LShiftU64 (NvdimmBase, 8);
              NvdimmSize = LShiftU64 (NvdimmSize, 8);
              DEBUG((EFI_D_INFO,  "NvdimmBase = %08x-%08x, NvdimmSize = %08x-%08x\n", \
                         (UINT32) (LShiftU64(NvdimmBase, 32)), (UINT32) (NvdimmBase & 0xffffffff), (UINT32) (LShiftU64(NvdimmSize, 32)), (UINT32) (NvdimmSize & 0xffffffff)));

              if (NvdimmBase == SaveDTOM2Ppi.Base) {
                  Temp1Size = NvdimmSize;
                  if ((NvdimmBase + NvdimmSize) > RegionTop64) {
                      Temp1Size =  RegionTop64 - NvdimmBase;
                  }
                SaveDTOM2Ppi.Base += Temp1Size;
                SaveDTOM2Ppi.Size -= Temp1Size;
                BuildResourceDescriptorHob (
                  EFI_RESOURCE_SYSTEM_MEMORY,
                  EFI_RESOURCE_ATTRIBUTE_PERSISTENT | EFI_RESOURCE_ATTRIBUTE_PERSISTABLE,
                  NvdimmBase,
                  NvdimmSize
                  );
                DEBUG((EFI_D_INFO,  "Descriptor base = %08x-%08x, size = %08x-%08x, *** reserved\n", \
                           (UINT32) (LShiftU64(NvdimmBase, 32)), (UINT32) (NvdimmBase & 0xffffffff),  (UINT32) (LShiftU64(NvdimmSize, 32)), (UINT32) (NvdimmSize & 0xffffffff)));

                 DEBUG((EFI_D_INFO,  "Descriptor base = %lx, size = %lx, *** persistent111\n",NvdimmBase,NvdimmSize));
              } else if ((NvdimmBase > SaveDTOM2Ppi.Base) && (NvdimmBase < RegionTop64)) {
                TempSize = NvdimmBase - SaveDTOM2Ppi.Base;
                //Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_SYSTEM_MEMORY,
                //        MEM_DET_COMMON_MEM_ATTR_ABOVE4G,  SaveDTOM2Ppi.Base, TempSize);
                BuildResourceDescriptorHob (EFI_RESOURCE_SYSTEM_MEMORY,
                        MEM_DET_COMMON_MEM_ATTR_ABOVE4G,  SaveDTOM2Ppi.Base, TempSize);

                if (Status != EFI_SUCCESS) {
                    DEBUG((EFI_D_ERROR, "creation failed @\n"));
                }
                DEBUG((EFI_D_INFO, "Descriptor base = %08x-%08x, size = %08x-%08x, *** memory\n",
                           (UINT32) (LShiftU64(SaveDTOM2Ppi.Base, 32)), (UINT32) (SaveDTOM2Ppi.Base & 0xffffffff), (UINT32) (LShiftU64(TempSize, 32)), (UINT32) (TempSize & 0xffffffff)));

                //Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_SYSTEM_MEMORY,
                //        (EFI_RESOURCE_ATTRIBUTE_PERSISTENT | EFI_RESOURCE_ATTRIBUTE_PERSISTABLE),  NvdimmBase, NvdimmSize);
                BuildResourceDescriptorHob (EFI_RESOURCE_SYSTEM_MEMORY,
                        (EFI_RESOURCE_ATTRIBUTE_PERSISTENT | EFI_RESOURCE_ATTRIBUTE_PERSISTABLE),  NvdimmBase, NvdimmSize);

		         DEBUG((EFI_D_INFO,  "Descriptor base = %lx, size = %lx, *** persistent\n",NvdimmBase,NvdimmSize));
                if (Status != EFI_SUCCESS) {
                    DEBUG((EFI_D_ERROR,  "creation failed @\n"));
                }
                DEBUG((EFI_D_INFO,  "Descriptor base = %08x-%08x, size = %08x-%08x, *** reserved\n",
                           (UINT32) (LShiftU64(NvdimmBase, 32)), (UINT32) (NvdimmBase & 0xffffffff), (UINT32) (LShiftU64(NvdimmSize, 32)), (UINT32) (NvdimmSize & 0xffffffff)));
                SaveDTOM2Ppi.Base += TempSize + NvdimmSize;
                SaveDTOM2Ppi.Size -= (TempSize + NvdimmSize);

              }
              NvdimmInfo++;
            }   // while (NvdimmInfo->Size != 0)

            if (SaveDTOM2Ppi.Size != 0) {   // The last system memory range
              BuildResourceDescriptorHob (
                EFI_RESOURCE_SYSTEM_MEMORY,
                MEM_DET_COMMON_MEM_ATTR_ABOVE4G,
                SaveDTOM2Ppi.Base,
                SaveDTOM2Ppi.Size
                );
//-           DEBUG((EFI_D_INFO, "(L%d) Mem(%lX,%lX)\n", __LINE__, SaveDTOM2Ppi.Base, SaveDTOM2Ppi.Size));
            }
          } else {
              BuildResourceDescriptorHob (
                EFI_RESOURCE_SYSTEM_MEMORY,
                MEM_DET_COMMON_MEM_ATTR_ABOVE4G,
                SaveDTOM2Ppi.Base,
                SaveDTOM2Ppi.Size
                );
//-           DEBUG((EFI_D_INFO, "(L%d) Mem(%lX,%lX)\n", __LINE__, SaveDTOM2Ppi.Base, SaveDTOM2Ppi.Size));
          }

      }

      if((MemRangDesc[MemRangeIndex].Base >= SIZE_4GB) &&
         (MemRangDesc[MemRangeIndex].Attribute == HYGON_MEMORY_ATTRIBUTE_RESERVED)){
        BuildResourceDescriptorHob (
          EFI_RESOURCE_MEMORY_RESERVED,
          MEM_DET_COMMON_MEM_ATTR,
          MemRangDesc[MemRangeIndex].Base,
          MemRangDesc[MemRangeIndex].Size
          );
      }

      if(MemRangDesc[MemRangeIndex].Attribute == HYGON_MEMORY_ATTRIBUTE_UMA){
        UMABase = MemRangDesc[MemRangeIndex].Base;
        UMASize = MemRangDesc[MemRangeIndex].Size;
      }
  }

  DEBUG((EFI_D_INFO, "dTOM = %x\n", dTOM));
  dTOM &= ~(TSEG_SIZE - 1);
  DEBUG((EFI_D_INFO, "dTOM = %x\n", dTOM));
  DEBUG((EFI_D_INFO, "Mem above 4G = %lx\n", TOM2));
  DEBUG((EFI_D_INFO, "Uma(%lx,%lx)\n", UMABase, UMASize));

  PlatCommInfo = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();
  PlatCommInfo->Tolum = dTOM;
  PlatCommInfo->Tom2 = TOM2;
  PlatCommInfo->UmaBase = UMABase;
  PlatCommInfo->UmaSize = UMASize;
  PlatCommInfo->TsegSize = TSEG_SIZE;
  PlatCommInfo->TsegBase = dTOM - PlatCommInfo->TsegSize;

  if(PcdGetBool(PcdAcpiS3Enable)){
    
    PlatCommInfo->S3DataRecordSize = S3_DATA_RECORD_SIZE;
    PlatCommInfo->S3MemorySize     = S3_PEI_MEMORY_SIZE;
    PlatCommInfo->S3DataRecord     = PlatCommInfo->TsegBase - PlatCommInfo->S3DataRecordSize;
    PlatCommInfo->S3MemoryAddr     = PlatCommInfo->S3DataRecord - PlatCommInfo->S3MemorySize;
    PlatCommInfo->LowMemSize       = PlatCommInfo->S3MemoryAddr;

    ASSERT(sizeof(PLATFORM_S3_RECORD) <= S3_DATA_RECORD_SIZE);
    SetS3RecordTable(PlatCommInfo->S3DataRecord);
    S3Record = (PLATFORM_S3_RECORD*)(UINTN)PlatCommInfo->S3DataRecord;

    if(BootMode == BOOT_ON_S3_RESUME){
      DEBUG((EFI_D_INFO, "S3Record(%X,%X)\n", PlatCommInfo->S3DataRecord, PlatCommInfo->S3DataRecordSize));
      DEBUG((EFI_D_INFO, "S3Memory(%X,%X)\n", PlatCommInfo->S3MemoryAddr, PlatCommInfo->S3MemorySize));
      Status = PeiServicesInstallPeiMemory(PlatCommInfo->S3MemoryAddr, PlatCommInfo->S3MemorySize);
      ASSERT(!EFI_ERROR(Status));
      return Status;

    } else {
      ZeroMem(S3Record, sizeof(PLATFORM_S3_RECORD));
      S3Record->Signature = PLAT_S3_RECORD_SIGNATURE;
    }
  } else {
    PlatCommInfo->LowMemSize = PlatCommInfo->TsegBase;
  }

  if(PlatCommInfo->PeiNeedMinMem && BootMode != BOOT_IN_RECOVERY_MODE){
    Size = SIZE_128MB;
  } else {
    Size = SIZE_512MB;
  }
  ASSERT(Size < PlatCommInfo->LowMemSize);
  Status = PeiServicesInstallPeiMemory(PlatCommInfo->LowMemSize - Size, Size);
  ASSERT(!EFI_ERROR(Status));

  FvBBBase = PcdGet32(PcdFlashFvRecoveryBackUpBase);
  if(FvBBBase == 0){
    FvBBBase = PcdGet32(PcdFlashFvRecoveryBase);
  }
  ASSERT(FvBBBase != 0);
  FvBBSize = 0xFFFFFFFF - FvBBBase + 1;
  FvBBBase = PcdGet32(PcdPspLoadBiosBaseAddress)+SIZE_16MB-FvBBSize;
  DEBUG((EFI_D_INFO, "FvBBMem(%x,%x)\n", FvBBBase, FvBBSize));
  BuildMemoryAllocationHob(FvBBBase, FvBBSize, EfiBootServicesData);

  Status = PeiServicesInstallPpi(gMemDetectPpiList);
  ASSERT_EFI_ERROR(Status);

  BuildResourceDescriptorHob (            // 10
    EFI_RESOURCE_SYSTEM_MEMORY,
    MEM_DET_COMMON_MEM_ATTR,
    0,
    (640 * 1024)
    );
  BuildResourceDescriptorHob (            // 6
    EFI_RESOURCE_MEMORY_RESERVED,
    MEM_DET_COMMON_MEM_ATTR,
    0xA0000,
    (384 * 1024)
    );
  BuildResourceDescriptorHob (
    EFI_RESOURCE_SYSTEM_MEMORY,
    MEM_DET_COMMON_MEM_ATTR,
    SIZE_1MB,
    PlatCommInfo->LowMemSize - SIZE_1MB
    );

  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_RESERVED,
    MEM_DET_COMMON_MEM_ATTR,
    PlatCommInfo->LowMemSize,
    PlatCommInfo->S3MemorySize + PlatCommInfo->S3DataRecordSize + PlatCommInfo->TsegSize
    );

  if (UMASize){
    BuildResourceDescriptorHob (
      EFI_RESOURCE_MEMORY_RESERVED,
      MEM_DET_COMMON_MEM_ATTR,
      UMABase,
      UMASize
      );
  }

  Size = ALIGN_VALUE(dTOM, SIZE_256MB);
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    SYS_MMIO_ATTRIB,
    Size,
    PcdGet64(PcdPciExpressBaseAddress) - Size
    );

  BuildResourceDescriptorHob (
    EFI_RESOURCE_IO,
    SYS_IO_ATTRIB,
    PLAT_PCI_IO_BASE,
    PLAT_PCI_IO_LIMIT - PLAT_PCI_IO_BASE + 1
    );

  GetSysPciHostBusInfo(PeiServices, PlatCommInfo);
  LibGetSysPci64Range(&PlatCommInfo->Pci64Base, &PlatCommInfo->Pci64Size);
  DEBUG((EFI_D_INFO, "PCI64(%lx,%lx)\n", PlatCommInfo->Pci64Base, PlatCommInfo->Pci64Size));

  MmioSize16MAligned = DivU64x32(PlatCommInfo->Pci64Size, PlatCommInfo->PciHostCount) & SIZE_16M_ALIGN;
  BaseAddr           = PlatCommInfo->Pci64Base;
  for(Index=0; Index<PlatCommInfo->PciHostCount; Index++){
    PlatCommInfo->Mmio64Base[Index] = BaseAddr;
    PlatCommInfo->Mmio64Size[Index] = MmioSize16MAligned;
    BaseAddr += MmioSize16MAligned;
  }
  PlatCommInfo->Mmio64Size[Index-1] = PlatCommInfo->Pci64Base + PlatCommInfo->Pci64Size - PlatCommInfo->Mmio64Base[Index-1];

  for(Index=0; Index<PlatCommInfo->PciHostCount; Index++){
    DEBUG((EFI_D_INFO, "MMIO64[%d] (%lx,%lx)\n", PlatCommInfo->Mmio64Base[Index], PlatCommInfo->Mmio64Size[Index]));
  }

  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    SYS_MMIO_ATTRIB,
    PlatCommInfo->Pci64Base,
    PlatCommInfo->Pci64Size
    );

  return Status;
}





