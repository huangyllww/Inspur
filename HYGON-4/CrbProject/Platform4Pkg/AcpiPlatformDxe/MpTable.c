

#include <IndustryStandard/Acpi.h>
#include "AcpiPlatform.h"
#include <Protocol/Smbios.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/LegacyBiosPlatform.h>
#include <Protocol/Legacy8259.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Library/DevicePathLib.h>

UINT32 GetMadtTable(VOID);


#pragma pack(1)

#define MP_FLOATING_POINTER_SIGNATURE    0x5F504D5F         // _MP_
#define MP_CONFIGURATION_TABLE_SIGNATURE 0x504D4350         // PCMP

typedef struct {
    UINT32  Signature; 
    UINT32  PhysicalAddresPointer;
    UINT8   Length;
    UINT8   VersionNumber;
    UINT8   CheckSum;
    UINT8   DefaultConfiguration;
    UINT8   IMCRflag;
    UINT8   Reserved[3];
} MP_FLOATING_POINTER;

typedef struct {
    UINT32  Signature;
    UINT16  BaseTableLength;
    UINT8   VersionNumber;
    UINT8   CheckSum;
    UINT8   OemId[8];
    UINT8   ProductId[12];
    UINT32  OemTablePointer;
    UINT16  OemTableSize;
    UINT16  BaseTableEntryCount;
    UINT32  LocalApicAddress;
    UINT16  ExtendedTableLength;
    UINT8   ExtendedCheckSum;
    UINT8   Reserved;
} MP_CONFIGURATION_TABLE_HEADER;


#define MP_TABLE_CPU_ENTRY_TYPE                  0
#define MP_TABLE_BUS_ENTRY_TYPE                  1
#define MP_TABLE_IO_APIC_ENTRY_TYPE              2
#define MP_TABLE_INT_ASSIGN_ENTRY_TYPE           3
#define MP_TABLE_LOCAL_INT_ASSIGN_ENTRY_TYPE     4


// [00] [00] [10] [03] [11 0F 90 00] [FF FB 8B 17] 00 00 00 00 00 00 00 00
typedef struct {
    UINT8   EntryType;
    UINT8   LocalApicId;
    UINT8   LocalApicVersion;
    struct {
        UINT8   CpuEnable    : 1;
        UINT8   CpuBootStrap : 1;
        UINT8   Reserved     : 6;
    } CpuFlags;
    UINT32  CpuSignature;
    UINT32  FeatureFlags;
    UINT32  Reserved[2];
} MP_TABLE_CPU_ENTRY;

typedef struct {
    UINT8   EntryType;
    UINT8   BusId;
    UINT8   BusTypeString[6];
} MP_TABLE_BUS_ENTRY;

typedef struct {
    UINT8   EntryType;
    UINT8   IoApicId;
    UINT8   IoApicVersion;
    UINT8   Flags;
    UINT32  IoApicAddress;
} MP_TABLE_IO_APIC_ENTRY;


#define INT_TYPE_INT        0
#define INT_TYPE_NMI        1
#define INT_TYPE_SMI        2
#define INT_TYPE_EXT_INT    3

#define POLARITY_CONFORM_SPEC   0
#define POLARITY_ACTIVE_HIGH    1
#define POLARITY_ACTIVE_LOW     3

#define TRIGGER_MODE_CONFORM_SPEC    0
#define TRIGGER_MODE_EDGE_TRIGGERED  1
#define TRIGGER_MODE_LEVEL_TRIGGERED 3


typedef union {
    UINT8   IsaBusIrq;
    struct {
        UINT8   PciIntSignal    : 2;
        UINT8   PciDeviceNumber : 5;
        UINT8   Reserved        : 1;
    } PciBusIrq;
} SOURCE_IRQ;

typedef struct {
    UINT8   EntryType;
    UINT8   InterruptType;
    UINT16  Flags;    
    UINT8   SourceBusId;
    SOURCE_IRQ SourceBusIrq;
    UINT8   DestIoApicId;
    UINT8   DestIoApicIntinn;
} MP_TABLE_INT_ASSIGN_ENTRY;

typedef struct {
    UINT8   EntryType;
    UINT8   InterruptType;
    struct {
        UINT16  Polarity    : 2;
        UINT16  TriggerMode : 2;
        UINT16  Reserved    : 12;
    } InterruptSignal;
    UINT8   SourceBusId;
    UINT8   SourceBusIrq;
    UINT8   DestLocalApicId;
    UINT8   DestLocalApicItin;
} MP_TABLE_LOCAL_INT_ASSIGN_ENTRY;

#pragma pack()

typedef struct {
  UINT8  Dev;
  UINT8  Func;
  UINT8  LogicNum;
  UINT8  Irq[4];
} HYGON_ROOT_PORT_APIC_INFO;

HYGON_ROOT_PORT_APIC_INFO gHygonRpApicInfo[] = {
  {1, 1, 0, {0, 1, 2, 3}},
  {1, 2, 1, {4, 5, 6, 7}},
  {1, 3, 2, {8, 9, 10, 11}},
  {1, 4, 3, {12, 13, 14, 15}},
  {1, 5, 4, {16, 17, 18, 19}},
  {1, 6, 5, {20, 21, 22, 23}},
  {1, 7, 6, {24, 25, 26, 27}},
  {2, 1, 7, {28, 29, 30, 31}},
  {3, 1, 8, {30, 31, 28, 29}},
  {3, 2, 9, {26, 27, 24, 25}},
  {3, 3, 10, {22, 23, 20, 21}},
  {3, 4, 11, {18, 19, 16, 17}},
  {3, 5, 12, {14, 15, 12, 13}},
  {3, 6, 13, {10, 11, 8, 9}},
  {3, 7, 14, {6, 7, 4, 5}},
  {4, 1, 15, {2, 3, 0, 1}},
  {7, 1, 16, {15, 12, 13, 14}},
  {8, 1, 17, {17, 18, 19, 16}},
};


typedef union {
  EFI_ACPI_2_0_PROCESSOR_LOCAL_APIC_STRUCTURE          *ApicLocal;
  EFI_ACPI_2_0_IO_APIC_STRUCTURE                       *ApicIo;
  EFI_ACPI_2_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE     *ApicIntSrcOR;
  EFI_ACPI_2_0_NON_MASKABLE_INTERRUPT_SOURCE_STRUCTURE *ApicNMIntSrc;
  EFI_ACPI_2_0_LOCAL_APIC_NMI_STRUCTURE                *ApicLocalNmi;
  EFI_ACPI_2_0_LOCAL_APIC_ADDRESS_OVERRIDE_STRUCTURE   *ApicLocalAddrOR;
  EFI_ACPI_2_0_IO_SAPIC_STRUCTURE                      *ApicIoS;
  EFI_ACPI_2_0_PROCESSOR_LOCAL_SAPIC_STRUCTURE         *ApicLocalS;
  EFI_ACPI_2_0_PLATFORM_INTERRUPT_SOURCES_STRUCTURE    *ApicPfIntSrc;
  EFI_ACPI_5_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE        *ApicLocalX2;        // 9
  EFI_ACPI_5_0_LOCAL_X2APIC_NMI_STRUCTURE              *ApicLocalX2Nmi;     // A
  EFI_ACPI_5_0_GIC_STRUCTURE                           *ApicGic;            // B
  EFI_ACPI_5_0_GIC_DISTRIBUTOR_STRUCTURE               *ApicGicd;           // C
  UINT8                                                *Raw;
} MadtApicStructPointer;


VOID *
LegacyCopyDataToShadowRam (
  EFI_LEGACY_BIOS_PROTOCOL   *LegacyBios,
  VOID                       *Data, 
  UINTN                      Size, 
  UINTN                      Alignment,
  UINTN                      Region
  )
{
  EFI_STATUS     Status;
  VOID           *LegacyRegion;
  

  Status = LegacyBios->GetLegacyRegion (
                          LegacyBios,
                          Size,
                          Region,
                          Alignment,
                          &LegacyRegion
                          );
  DEBUG((EFI_D_INFO, "(L%d) %r %X L:%X\n", __LINE__, Status, LegacyRegion, Size));
  if(EFI_ERROR(Status)){
    return NULL;
  }
  ASSERT (((UINTN)LegacyRegion & 0xF) == 0);

  Status = LegacyBios->CopyLegacyRegion (
                          LegacyBios,
                          Size,
                          LegacyRegion,
                          Data
                          );
  ASSERT_EFI_ERROR (Status);

  return LegacyRegion;
}


typedef struct {
  EFI_PCI_IO_PROTOCOL       *PciIo;
  EFI_DEVICE_PATH_PROTOCOL  *Dp;
  UINTN                     DpSize;
  UINT32                    PciId;
  UINT8                     Bus, Dev, Func;
} PCI_INFO;

typedef struct {
  UINT8                     Bus;
  UINT8                     IoApicId;
  UINT8                     Irq[4];
} PCI_BUS_IRQ_INFO;

typedef struct {
  EFI_ACPI_1_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER   *Madt;
  EFI_LEGACY_BIOS_PROTOCOL                              *LegacyBios;
  PROCESSOR_ID_DATA                                     ProcessorId;
  UINTN                                                 LocalApicCount;
  UINTN                                                 IoApicCount; 
  MadtApicStructPointer                                 ApicStruct;
  UINTN                                                 ApicStructCount;
  UINT8                                                 LpcBus;
  UINT8                                                 IoApic[9];
  UINT8                                                 IrqBase[9];
  UINT8                                                 PciHostCount;
  UINT8                                                 PciBusStart[8];
  UINT8                                                 PciBusEnd[8];
  PCI_BUS_IRQ_INFO                                      *BusIrq;
  UINTN                                                 PciBusCount;
  MP_FLOATING_POINTER                                   MpFp;
} MP_TABLE_CTX;


HYGON_ROOT_PORT_APIC_INFO *GetRootPortPciApicInfo(UINTN Bus, PCI_INFO *PciInfo, UINTN PciInfoCount)
{
  UINTN                              Index;
  UINTN                              Target = 0xFFFFFFFF;
  UINTN                              i;


  DEBUG((EFI_D_INFO, "GetRootPortPciApicInfo(Bus:%d)\n", Bus));

  for(Index=0;Index<PciInfoCount;Index++){
    if(PciInfo[Index].Bus == Bus){
      Target = Index;
      break;
    }
  }
  if(Target == 0xFFFFFFFF){
    return NULL;
  }

  DEBUG((EFI_D_INFO, "(%x,%x,%x)\n", PciInfo[Target].Bus, PciInfo[Target].Dev, PciInfo[Target].Func));

  for(Index=0;Index<PciInfoCount;Index++){
    if(PciInfo[Index].PciId != 0x14531D94 && PciInfo[Index].PciId != 0x14541D94){
      continue;
    }
    if(PciInfo[Index].DpSize >= PciInfo[Target].DpSize){
      continue;
    }
    if(CompareMem(PciInfo[Target].Dp, PciInfo[Index].Dp, PciInfo[Index].DpSize-4) != 0){
      continue;
    }

    DEBUG((EFI_D_INFO, "(%x,%x,%x)\n", PciInfo[Index].Bus, PciInfo[Index].Dev, PciInfo[Index].Func));
    
    for(i=0;i<ARRAY_SIZE(gHygonRpApicInfo);i++){
      if(gHygonRpApicInfo[i].Dev == PciInfo[Index].Dev && gHygonRpApicInfo[i].Func == PciInfo[Index].Func){
        return &gHygonRpApicInfo[i];
      }
    }
  }  

  return NULL;
}


EFI_STATUS 
PreparePciInfo (
  MP_TABLE_CTX                        *Ctx,
  EFI_LEGACY_IRQ_ROUTING_ENTRY        *RoutingTable
  )
{
  EFI_STATUS                         Status;
  EFI_HANDLE                         *Handles = NULL;
  UINTN                              HandleCount;
  UINTN                              Index;
  EFI_PCI_IO_PROTOCOL                *PciIo;
  EFI_DEVICE_PATH_PROTOCOL           *Dp;
  UINTN                              Seg, Bus, Dev, Func;
  PCI_INFO                           *p;
  UINTN                              i;
  UINT8                              IrqBase;
  HYGON_ROOT_PORT_APIC_INFO          *ApicInfo;
  UINT8                              IoApicId;
  UINT32                             PciId;
  PCI_INFO                           *PciInfo = NULL;
  UINTN                              PciInfoCount = 0;  


  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  PciInfo = AllocatePool(HandleCount * sizeof(PCI_INFO));
  if(PciInfo == NULL){
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;    
  }
  
  for(Index=0;Index<HandleCount;Index++){

    p = &PciInfo[PciInfoCount];

    Status = gBS->HandleProtocol(
                    Handles[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID**)&PciIo
                    );
    if(EFI_ERROR(Status)){
      continue;
    }

    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0, 1, &PciId);
    
    Status = gBS->HandleProtocol(
                    Handles[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID**)&Dp
                    );
    if(EFI_ERROR(Status)){
      continue;
    }

    PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Func);
    p->PciIo = PciIo;
    p->Dp    = Dp;
    p->PciId = PciId;
    p->Bus   = (UINT8)Bus;
    p->Dev   = (UINT8)Dev;
    p->Func  = (UINT8)Func;
    p->DpSize = GetDevicePathSize(Dp);
    PciInfoCount++;
  }

  Ctx->BusIrq = AllocatePool(Ctx->PciBusCount * sizeof(PCI_BUS_IRQ_INFO));
  if(Ctx->BusIrq == NULL){
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;    
  }

  for(Index=0; Index<Ctx->PciBusCount; Index++){

    Bus = RoutingTable[Index].Bus;

    DEBUG((EFI_D_INFO, "bus:%d\n", Bus));

    for(i=0;i<Ctx->PciHostCount;i++){
      if(Bus >= Ctx->PciBusStart[i] && Bus <= Ctx->PciBusEnd[i]){
        IrqBase = Ctx->IrqBase[1+i];
        IoApicId = Ctx->IoApic[1+i];
        DEBUG((EFI_D_INFO, "IrqBase:0x%x, IoApicId:0x%x\n", IrqBase, IoApicId));
        break;
      }
    }
    
    ApicInfo = GetRootPortPciApicInfo(Bus, PciInfo, PciInfoCount);
    if(ApicInfo == NULL){
      Status = EFI_NOT_FOUND;
      goto ProcExit;
    }

    Ctx->BusIrq[Index].Bus = (UINT8)Bus;
    Ctx->BusIrq[Index].IoApicId = IoApicId;
    Ctx->BusIrq[Index].Irq[0] = IrqBase + ApicInfo->Irq[0];
    Ctx->BusIrq[Index].Irq[1] = IrqBase + ApicInfo->Irq[1];
    Ctx->BusIrq[Index].Irq[2] = IrqBase + ApicInfo->Irq[2];
    Ctx->BusIrq[Index].Irq[3] = IrqBase + ApicInfo->Irq[3];
  }


ProcExit:  
  if(Handles != NULL){
    FreePool(Handles);
  }
  if(PciInfo != NULL){
    FreePool(PciInfo);
  }
  return Status;
}


EFI_STATUS PrepareMpTableInfo(MP_TABLE_CTX *Ctx)
{
  EFI_STATUS                          Status;
  SMBIOS_TABLE_TYPE4                  *Type4;  
  EFI_SMBIOS_HANDLE                   SmbiosHandle;
  EFI_SMBIOS_TYPE                     SmbiosType;
  EFI_SMBIOS_TABLE_HEADER             *SmbiosHdr;   
  EFI_SMBIOS_PROTOCOL                 *Smbios;
  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL   *LegacyBiosPlatform;
  MadtApicStructPointer               ApicStruct;
  UINTN                               Index;
  UINT8                               Type;
  UINT8                               Length;
	UINTN                               HandleCount;
	EFI_HANDLE 		                      *HandleBuffer = NULL;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *RbIo;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR   *AddressSpace;
  UINTN                               MaxBus = 0; 
  EFI_LEGACY_IRQ_ROUTING_ENTRY        *RoutingTable;
  UINTN                               RoutingTableCount;


  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if(EFI_ERROR(Status)){
    return Status;
  }
  Status = gBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, NULL, (VOID**)&Ctx->LegacyBios);
  if (EFI_ERROR(Status)) {
    return Status;
  }  

  Status = gBS->LocateProtocol (&gEfiLegacyBiosPlatformProtocolGuid, NULL, (VOID**)&LegacyBiosPlatform);
  if (EFI_ERROR(Status)) {
    return Status;
  }  
  Status = LegacyBiosPlatform->GetRoutingTable(LegacyBiosPlatform, &RoutingTable, &RoutingTableCount, NULL, NULL, NULL, NULL);
  if (EFI_ERROR(Status)) {
    return Status;
  }  
  Ctx->PciBusCount = RoutingTableCount;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
  ASSERT(!EFI_ERROR(Status));
  Type4 = (SMBIOS_TABLE_TYPE4*)SmbiosHdr;
  CopyMem(&Ctx->ProcessorId, &Type4->ProcessorId, sizeof(Ctx->ProcessorId));

  Ctx->Madt = (EFI_ACPI_1_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER*)(UINTN)GetMadtTable();
  if(Ctx->Madt == NULL){
    return EFI_NOT_FOUND;
  }

  ApicStruct.Raw       = (UINT8*)Ctx->Madt + sizeof(*Ctx->Madt);
  Ctx->ApicStruct.Raw  = ApicStruct.Raw;
  Ctx->ApicStructCount = Ctx->Madt->Header.Length - sizeof(*Ctx->Madt);
  Index                = 0;
  while(Index < Ctx->ApicStructCount){
    Type   = ApicStruct.ApicLocal->Type;
    Length = ApicStruct.ApicLocal->Length;
    
    switch(Type){
      case EFI_ACPI_2_0_PROCESSOR_LOCAL_APIC:
        if((ApicStruct.ApicLocal->ApicId & BIT0) == 0){         // MPS doesn't support HT
          Ctx->LocalApicCount++;
        }
        break;
        
      case EFI_ACPI_2_0_IO_APIC:
        if(Ctx->IoApicCount < ARRAY_SIZE(Ctx->IoApic)){
          Ctx->IoApic[Ctx->IoApicCount]  = ApicStruct.ApicIo->IoApicId;
          Ctx->IrqBase[Ctx->IoApicCount] = (UINT8)ApicStruct.ApicIo->GlobalSystemInterruptBase;
          DEBUG((EFI_D_INFO, "IoApic[%d]:%x %x\n", Ctx->IoApicCount, Ctx->IoApic[Ctx->IoApicCount], Ctx->IrqBase[Ctx->IoApicCount]));
          Ctx->IoApicCount++;
        }
        break;          
    }
    Index += Length;
    ApicStruct.Raw = ApicStruct.Raw + Length;
  }

  DEBUG((EFI_D_INFO, "LocalApicCount:%d, IoApicCount:%d, PciBusCount:%d\n", \
    Ctx->LocalApicCount, Ctx->IoApicCount, Ctx->PciBusCount));

  Ctx->LpcBus = 0xFF;
	Status = gBS->LocateHandleBuffer(
	                ByProtocol, 
	                &gEfiPciRootBridgeIoProtocolGuid, 
	                NULL, 
	                &HandleCount, 
	                &HandleBuffer
	                );
	if(!EFI_ERROR(Status)){
    for(Index=0; Index<HandleCount; Index++){
  		Status = gBS->HandleProtocol(
  		                HandleBuffer[Index],
  		                &gEfiPciRootBridgeIoProtocolGuid, 
                      (void**)&RbIo
  		                );
      Status = RbIo->Configuration(RbIo, &AddressSpace);
      while(AddressSpace->Desc != ACPI_END_TAG_DESCRIPTOR){
        
        if(AddressSpace->Desc == ACPI_ADDRESS_SPACE_DESCRIPTOR && AddressSpace->ResType == 2){
          if(Index < ARRAY_SIZE(Ctx->PciBusStart)){
            DEBUG((EFI_D_INFO, "Bus %x,%x\n", AddressSpace->AddrRangeMin, AddressSpace->AddrRangeMax));
            Ctx->PciBusStart[Index] = (UINT8)AddressSpace->AddrRangeMin;
            Ctx->PciBusEnd[Index]   = (UINT8)AddressSpace->AddrRangeMax;
            Ctx->PciHostCount++;
          }
          if(MaxBus < AddressSpace->AddrRangeMax){
            MaxBus = AddressSpace->AddrRangeMax;
          }
        }          
        AddressSpace = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR*)((UINTN)AddressSpace + 3 + AddressSpace->Len);
      }      
    }
    FreePool(HandleBuffer);
    Ctx->LpcBus = (UINT8)(MaxBus+1);
    DEBUG((EFI_D_INFO, "LpcBus:0x%x\n", Ctx->LpcBus));
  }

  Status = PreparePciInfo(Ctx, RoutingTable);
  return Status;
}


EFI_STATUS BuildMpTable()
{
  MP_TABLE_CTX                   *Ctx = NULL;
  MadtApicStructPointer          ApicStruct;
  UINT8                          Type;
  UINT8                          Length;
  UINTN                          Index;
  EFI_STATUS                     Status;
  MP_TABLE_CPU_ENTRY             *MpCpu;
  MP_TABLE_IO_APIC_ENTRY         *MpIoApic;
  MP_TABLE_BUS_ENTRY             *MpBus;
  MP_TABLE_INT_ASSIGN_ENTRY      *MpInt;
  MP_TABLE_LOCAL_INT_ASSIGN_ENTRY *MpLocalInt;
  UINTN                          TotalSize;
  MP_CONFIGURATION_TABLE_HEADER  *MpCfgTable;
  UINT8                          *p;
  VOID                           *MpCfgTableF0000;
  VOID                           *MpFpF0000;
  UINTN                          EntryCount;
  UINTN                          i;

  
  DEBUG((EFI_D_INFO, "BuildMpTable\n"));

  if (PcdGet8(PcdLegacyBiosSupport) == 0) {
    DEBUG((EFI_D_INFO, "Unsupported\n"));
    return EFI_UNSUPPORTED;
  }  

  Ctx = AllocateZeroPool(sizeof(MP_TABLE_CTX));
  if(Ctx == NULL){
    DEBUG((EFI_D_ERROR, "Ctx == NULL\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }

  Status = PrepareMpTableInfo(Ctx);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "PrepareMpTableInfo:%r\n", Status));
    goto ProcExit;
  }    

  EntryCount = Ctx->LocalApicCount + Ctx->IoApicCount + Ctx->PciBusCount+1 + 15+Ctx->PciBusCount*4 + 2;
  TotalSize = sizeof(MP_CONFIGURATION_TABLE_HEADER) + 
              Ctx->LocalApicCount * sizeof(MP_TABLE_CPU_ENTRY) +
              Ctx->IoApicCount * sizeof(MP_TABLE_IO_APIC_ENTRY) +
              (Ctx->PciBusCount+1)*sizeof(MP_TABLE_BUS_ENTRY) +
              (15+Ctx->PciBusCount*4)*sizeof(MP_TABLE_INT_ASSIGN_ENTRY) +
              2 * sizeof(MP_TABLE_LOCAL_INT_ASSIGN_ENTRY);
  MpCfgTable = (MP_CONFIGURATION_TABLE_HEADER*)AllocateZeroPool(TotalSize);
  if (MpCfgTable == NULL) {
    DEBUG((EFI_D_ERROR, "MpCfgTable == NULL\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }   

  ApicStruct.Raw = Ctx->ApicStruct.Raw;
  Index          = 0;
  p              = (UINT8*)(MpCfgTable+1);
  while(Index < Ctx->ApicStructCount){
    Type = ApicStruct.ApicLocal->Type;
    Length = ApicStruct.ApicLocal->Length;
    
    switch(Type){
      case EFI_ACPI_2_0_PROCESSOR_LOCAL_APIC:
        if((ApicStruct.ApicLocal->ApicId & BIT0) == 0){
          MpCpu = (MP_TABLE_CPU_ENTRY*)p;
          MpCpu->EntryType = MP_TABLE_CPU_ENTRY_TYPE;
          MpCpu->LocalApicId = ApicStruct.ApicLocal->ApicId;
          MpCpu->LocalApicVersion = 0x10;
          MpCpu->CpuFlags.CpuEnable = !!(ApicStruct.ApicLocal->Flags & BIT0);
          if(MpCpu->LocalApicId == 0){
            MpCpu->CpuFlags.CpuBootStrap = 1;
          }
          CopyMem(&MpCpu->CpuSignature, &Ctx->ProcessorId.Signature, sizeof(MpCpu->CpuSignature));
          CopyMem(&MpCpu->FeatureFlags, &Ctx->ProcessorId.FeatureFlags, sizeof(MpCpu->FeatureFlags));
          p += sizeof(MP_TABLE_CPU_ENTRY);
        }
        break;

      case EFI_ACPI_2_0_IO_APIC:
        MpIoApic = (MP_TABLE_IO_APIC_ENTRY*)p;
        MpIoApic->EntryType = MP_TABLE_IO_APIC_ENTRY_TYPE;
        MpIoApic->IoApicId  = ApicStruct.ApicIo->IoApicId;
        MpIoApic->IoApicVersion = 0x21;
        MpIoApic->Flags = 1;
        MpIoApic->IoApicAddress = ApicStruct.ApicIo->IoApicAddress;
        p += sizeof(MP_TABLE_IO_APIC_ENTRY);        
        break;
        
    }
    Index += Length;
    ApicStruct.Raw = ApicStruct.Raw + Length;
  }

  MpBus = (MP_TABLE_BUS_ENTRY*)p;
  for(Index=0; Index<Ctx->PciBusCount; Index++){
    MpBus->EntryType = MP_TABLE_BUS_ENTRY_TYPE;
    MpBus->BusId     = Ctx->BusIrq[Index].Bus;
    CopyMem(MpBus->BusTypeString, "PCI   ", 6);
    MpBus++;
  }
  MpBus->EntryType = MP_TABLE_BUS_ENTRY_TYPE;
  MpBus->BusId     = Ctx->LpcBus;
  CopyMem(MpBus->BusTypeString, "ISA   ", 6);
  MpBus++;
  p = (UINT8*)MpBus;

  MpInt = (MP_TABLE_INT_ASSIGN_ENTRY*)p;
  for(Index=0; Index<16; Index++){
    if(Index == 2){
      continue;
    }
    MpInt->EntryType = MP_TABLE_INT_ASSIGN_ENTRY_TYPE;
    MpInt->InterruptType = INT_TYPE_INT;
    if(Index == 9){
      MpInt->Flags = 0xF;      
    } else {
      MpInt->Flags = 0;
    }
    MpInt->SourceBusId = Ctx->LpcBus;
    MpInt->SourceBusIrq.IsaBusIrq = (UINT8)Index;
    MpInt->DestIoApicId = Ctx->IoApic[0];
    MpInt->DestIoApicIntinn = (UINT8)Index;
    MpInt++;
  } 
  for(Index=0;Index<Ctx->PciBusCount;Index++){
    for(i=0;i<4;i++){
      MpInt->EntryType = MP_TABLE_INT_ASSIGN_ENTRY_TYPE;
      MpInt->InterruptType = INT_TYPE_INT;
      MpInt->SourceBusId   = Ctx->BusIrq[Index].Bus;
      MpInt->SourceBusIrq.PciBusIrq.PciIntSignal = (UINT8)i;
      MpInt->DestIoApicId = Ctx->BusIrq[Index].IoApicId;
      MpInt->DestIoApicIntinn = Ctx->BusIrq[Index].Irq[i];
      MpInt++;
    }
  }

  MpLocalInt = (MP_TABLE_LOCAL_INT_ASSIGN_ENTRY*)MpInt;
  MpLocalInt->EntryType = MP_TABLE_LOCAL_INT_ASSIGN_ENTRY_TYPE;
  MpLocalInt->InterruptType = INT_TYPE_EXT_INT;
  MpLocalInt->InterruptSignal.Polarity = POLARITY_CONFORM_SPEC;
  MpLocalInt->InterruptSignal.TriggerMode = TRIGGER_MODE_CONFORM_SPEC;
  MpLocalInt->InterruptSignal.Reserved = 0;
  MpLocalInt->SourceBusId = 0;
  MpLocalInt->SourceBusIrq = 0;
  MpLocalInt->DestLocalApicId = 0xFF;
  MpLocalInt->DestLocalApicItin = 0;
  MpLocalInt++;

  MpLocalInt->EntryType = MP_TABLE_LOCAL_INT_ASSIGN_ENTRY_TYPE;
  MpLocalInt->InterruptType = INT_TYPE_NMI;
  MpLocalInt->InterruptSignal.Polarity = POLARITY_CONFORM_SPEC;
  MpLocalInt->InterruptSignal.TriggerMode = TRIGGER_MODE_CONFORM_SPEC;
  MpLocalInt->InterruptSignal.Reserved = 0;
  MpLocalInt->SourceBusId = 0;
  MpLocalInt->SourceBusIrq = 0;
  MpLocalInt->DestLocalApicId = 0xFF;
  MpLocalInt->DestLocalApicItin = 1;
  MpLocalInt++;  
  
  MpCfgTable->Signature = MP_CONFIGURATION_TABLE_SIGNATURE;
  MpCfgTable->BaseTableLength = (UINT16)TotalSize;
  MpCfgTable->VersionNumber   = 4;
  CopyMem(MpCfgTable->OemId, "Hygon", 5);
  CopyMem(MpCfgTable->ProductId, "Hygon", 5);
  MpCfgTable->BaseTableEntryCount = (UINT16)EntryCount;
  MpCfgTable->LocalApicAddress = Ctx->Madt->LocalApicAddress;
  MpCfgTable->CheckSum = CalculateCheckSum8((UINT8*)MpCfgTable, TotalSize);

  MpCfgTableF0000 = LegacyCopyDataToShadowRam (
                      Ctx->LegacyBios, 
                      MpCfgTable, 
                      TotalSize, 
                      16, 
                      LEGACY_REGION_SEGMENT_F000
                      );
  if(MpCfgTableF0000 == NULL){
    DEBUG((EFI_D_ERROR, "MpCfgTableF0000 == NULL, TotalSize:%d\n", TotalSize));
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }

  Ctx->MpFp.Signature = MP_FLOATING_POINTER_SIGNATURE;
  Ctx->MpFp.Length    = sizeof(MP_FLOATING_POINTER)/16;
  Ctx->MpFp.VersionNumber = 4;
  Ctx->MpFp.PhysicalAddresPointer = (UINT32)(UINTN)MpCfgTableF0000;
  Ctx->MpFp.CheckSum = CalculateCheckSum8((UINT8*)&Ctx->MpFp, sizeof(MP_FLOATING_POINTER));
  MpFpF0000 = LegacyCopyDataToShadowRam (
                Ctx->LegacyBios, 
                &Ctx->MpFp, 
                sizeof(Ctx->MpFp), 
                16, 
                LEGACY_REGION_SEGMENT_F000
                );
  if(MpFpF0000 == NULL){
    DEBUG((EFI_D_ERROR, "MpFpF0000 == NULL\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }  

ProcExit:
  if(Ctx != NULL){ 
    if(Ctx->BusIrq != NULL){
      FreePool(Ctx->BusIrq);
    }
    FreePool(Ctx);
  }
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "BuildMpTable:%r\n", Status));  
  }  
  return Status;
}


