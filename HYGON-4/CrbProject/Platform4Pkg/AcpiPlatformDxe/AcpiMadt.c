
#include <IndustryStandard/Acpi.h>
#include "AcpiPlatform.h"
#include <IndustryStandard/SerialPortConsoleRedirectionTable.h>
#include <Library/PlatformCommLib.h>
#include <Guid/Acpi.h>
#include <Library/ByoHygonFabricLib.h>
#include <Library/IoLib.h>


STATIC EFI_ACPI_4_0_LOCAL_X2APIC_NMI_STRUCTURE gx2Nmi[] = {
  {
    EFI_ACPI_4_0_LOCAL_X2APIC_NMI,
    sizeof(EFI_ACPI_4_0_LOCAL_X2APIC_NMI_STRUCTURE),
    5,
    0xFFFFFFFF,
    1,
    {0, 0, 0}
  }
};


STATIC EFI_ACPI_4_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE  gIso[] = {
  {
    EFI_ACPI_1_0_INTERRUPT_SOURCE_OVERRIDE,                   // Type
    sizeof (EFI_ACPI_4_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE),// Length
    0x00,                                                     // Bus - ISA
    0x00,                                                     // Source - IRQ0
    0x00000002,                                               // Global System Interrupt - IRQ2
    0x0000                                                    // Flags - Conforms to specifications of the bus
  },
  {
    EFI_ACPI_1_0_INTERRUPT_SOURCE_OVERRIDE,                   // Type
    sizeof (EFI_ACPI_4_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE),// Length
    0x00,                                                     // Bus - ISA
    0x09,                                                     // Source - IRQx
    0x00000009,                                               // Global System Interrupt - IRQx
    0x000F                                                    // Flags - Level-tiggered, Active Low
  },
};


UINT32 GetIoApicMaxEntry(UINT32 IoApicAddress)
{
  UINT32  Data32;

  MmioWrite32(IoApicAddress, 1);
  Data32 = (MmioRead32(IoApicAddress+0x10) >> 16) + 1;
  DEBUG((EFI_D_INFO, "GetIoApicMaxEntry(%x):%x\n", IoApicAddress, Data32));
  return Data32;
}

EFI_STATUS SetAllApicId(PLATFORM_COMM_INFO *Info)
{
  UINT32  Index;
  UINT8   NbApicId = PcdGet8(PcdCfgGnbIoapicId);
  UINT32  EntryCount = 0;
  UINT32  EntryCountList[PCI_HOST_MAX_COUNT];
  UINT32  SbEntryCount;
  
  Info->SbApicId   = PcdGet8(PcdCfgFchIoapicId);
  Info->SbApicBase = 0xFEC00000;

  SbEntryCount = GetIoApicMaxEntry(Info->SbApicBase);
  for(Index=0; Index<Info->PciHostCount; Index++){
    EntryCountList[Index] = GetIoApicMaxEntry(Info->NbApicBase[Index]);
  }

  MmioWrite32(Info->SbApicBase, 0);
  MmioWrite32(Info->SbApicBase+0x10, Info->SbApicId << 24);

  for(Index=0; Index<Info->PciHostCount; Index++){
    if(Index){
      EntryCount += EntryCountList[Index-1];
    } else {
      EntryCount += SbEntryCount;
    }
    Info->GSIBase[Index] = EntryCount;
    
    Info->NbApicId[Index] = NbApicId;
    MmioWrite32(Info->NbApicBase[Index], 0);
    MmioWrite32(Info->NbApicBase[Index]+0x10, NbApicId << 24);
    NbApicId++;
  }

  return EFI_SUCCESS;
}


VOID
BuildAcpiMadtTable (
  EFI_ACPI_TABLE_PROTOCOL  *AcpiTable,
  PLATFORM_COMM_INFO       *Info   
  )
{
  CPU_APIC_ID_INFO                                      *CpuApicIdTable;
  UINTN                                                 CpuCount;
  UINTN                                                 Index;
  UINTN                                                 TableSize;
  EFI_ACPI_4_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER   *Madt;
  EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE         *LocalApic;
  EFI_ACPI_4_0_IO_APIC_STRUCTURE                        *IoApic, *p;
  EFI_ACPI_4_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE      *Iso;
  EFI_STATUS                                            Status;
  UINTN                                                 TableKey;
  UINT32                                                TempApicId;
  UINTN                                                 Count;


  DEBUG((EFI_D_INFO, "BuildAcpiMadtTable\n"));

  GetCpuLocalApicInfo(&CpuApicIdTable, &CpuCount);
  Status = SetAllApicId(Info);
  ASSERT_EFI_ERROR (Status);

  TableSize = sizeof(EFI_ACPI_4_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER) +
              sizeof(EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE) * CpuCount +
              sizeof(gx2Nmi) +
              sizeof(EFI_ACPI_4_0_IO_APIC_STRUCTURE) * (Info->PciHostCount + 1) +
              sizeof(gIso);
  Madt = AllocateZeroPool(TableSize);
  ASSERT(Madt != NULL);

  Madt->Header.Signature = EFI_ACPI_1_0_APIC_SIGNATURE;
  Madt->Header.Length    = (UINT32)TableSize;
  Madt->Header.Revision  = 3;
  Madt->LocalApicAddress = 0xFEE00000;
  Madt->Flags            = EFI_ACPI_4_0_PCAT_COMPAT;

  LocalApic = (EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE*)(Madt+1);

  if(Info->CpuSmtMode){
    TempApicId = 0;
    Count      = CpuCount/2;
    for(Index=0; Index<Count; Index++){
      LocalApic[Index].Type = EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC;
      LocalApic[Index].Length = sizeof(EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE);
      LocalApic[Index].AcpiProcessorUid = TempApicId;
      LocalApic[Index].X2ApicId = CpuApicIdTable[TempApicId].ApicId;
      LocalApic[Index].Flags = CpuApicIdTable[TempApicId].Flags;
      TempApicId +=2;
    }
    for(TempApicId = 1; Index < CpuCount; Index++){
      LocalApic[Index].Type = EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC;
      LocalApic[Index].Length = sizeof(EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE);
      LocalApic[Index].AcpiProcessorUid = TempApicId;
      LocalApic[Index].X2ApicId = CpuApicIdTable[TempApicId].ApicId;
      LocalApic[Index].Flags = CpuApicIdTable[TempApicId].Flags;
      TempApicId +=2;
    }

  } else {
    for(Index = 0; Index < CpuCount; Index++){
      LocalApic[Index].Type = EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC;
      LocalApic[Index].Length = sizeof(EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE);
      LocalApic[Index].AcpiProcessorUid = (UINT32)Index;
      LocalApic[Index].X2ApicId = CpuApicIdTable[Index].ApicId;
      LocalApic[Index].Flags  = CpuApicIdTable[Index].Flags;
    }
  }

  CopyMem(&LocalApic[Index], gx2Nmi, sizeof(gx2Nmi));

  IoApic = (EFI_ACPI_4_0_IO_APIC_STRUCTURE*)((UINTN)&LocalApic[Index] + sizeof(gx2Nmi));
  IoApic[0].Type = EFI_ACPI_1_0_IO_APIC;
  IoApic[0].Length = sizeof(EFI_ACPI_1_0_IO_APIC_STRUCTURE);
  IoApic[0].IoApicId = (UINT8)Info->SbApicId;
  IoApic[0].IoApicAddress = Info->SbApicBase;
  IoApic[0].GlobalSystemInterruptBase = 0;
  DEBUG((EFI_D_INFO, "SB IoApic:%X Id:%X\n", IoApic[0].IoApicAddress, IoApic[0].IoApicId));
  
  Count = Info->PciHostCount;
  p = &IoApic[1];
  for(Index=0; Index<Count; Index++){
    p->Type = EFI_ACPI_1_0_IO_APIC;
    p->Length = sizeof(EFI_ACPI_1_0_IO_APIC_STRUCTURE);
    p->IoApicId = (UINT8)Info->NbApicId[Index];
    p->IoApicAddress = Info->NbApicBase[Index];
    p->GlobalSystemInterruptBase = Info->GSIBase[Index];
    DEBUG((EFI_D_INFO, "NB IoApic:%X Id:%X B:%x\n", \
      p->IoApicAddress, p->IoApicId, p->GlobalSystemInterruptBase));
    p++;
  }

  Iso = (EFI_ACPI_4_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE*)(&IoApic[Info->PciHostCount + 1]);
  CopyMem(Iso, gIso, sizeof(gIso));

  PlatformUpdateTables((EFI_ACPI_COMMON_HEADER*)Madt);
  AcpiTableUpdateChksum(Madt);
  Status = AcpiTable->InstallAcpiTable(
                        AcpiTable,
                        Madt,
                        TableSize,
                        &TableKey
                        );
  ASSERT_EFI_ERROR (Status);

  FreePool(CpuApicIdTable);
  FreePool(Madt);
}


