
#include <IndustryStandard/Acpi.h>
#include "AcpiPlatform.h"
#include <IndustryStandard/SerialPortConsoleRedirectionTable.h>
#include <Library/PlatformCommLib.h>
#include <Protocol/PciEnumerationComplete.h>
#include <Guid/Acpi.h>



STATIC EFI_ACPI_1_0_LOCAL_APIC_NMI_STRUCTURE  gNmi[] = {  
  {
    EFI_ACPI_1_0_LOCAL_APIC_NMI,
    sizeof(EFI_ACPI_1_0_LOCAL_APIC_NMI_STRUCTURE),
    0xFF,
    0x05,
    0x01
  }
};

STATIC EFI_ACPI_1_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE  gIso[] = {
  {
    EFI_ACPI_1_0_INTERRUPT_SOURCE_OVERRIDE,                   // Type
    sizeof (EFI_ACPI_1_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE),// Length
    0x00,                                                     // Bus - ISA
    0x00,                                                     // Source - IRQ0
    0x00000002,                                               // Global System Interrupt - IRQ2
    0x0000                                                    // Flags - Conforms to specifications of the bus
  },    
  {
    EFI_ACPI_1_0_INTERRUPT_SOURCE_OVERRIDE,                   // Type
    sizeof (EFI_ACPI_1_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE),// Length
    0x00,                                                     // Bus - ISA
    0x09,                                                     // Source - IRQx
    0x00000009,                                               // Global System Interrupt - IRQx
    0x000F                                                    // Flags - Level-tiggered, Active Low
  }, 
};

STATIC UINTN gIoApicOffset;




UINT32 GetMadtTable(VOID)
{
  EFI_ACPI_4_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Rsdp;
  EFI_ACPI_DESCRIPTION_HEADER                   *Xsdt;
  UINTN                                         Index;
  UINT64                                        *XTableAddress;
  UINTN                                         TableCount;
  EFI_STATUS                                    Status;	
  EFI_ACPI_DESCRIPTION_HEADER                   *AcpiHdr;
	

  Status = EfiGetSystemConfigurationTable(&gEfiAcpiTableGuid, &Rsdp);
  ASSERT(!EFI_ERROR(Status));	
  if (EFI_ERROR(Status)) {
    return 0;
  }

  Xsdt = (EFI_ACPI_DESCRIPTION_HEADER*)(UINTN)Rsdp->XsdtAddress;
  TableCount = (Xsdt->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(UINT64);
  XTableAddress = (UINT64 *)(Xsdt + 1);
  for (Index = 0; Index < TableCount; Index++) {
    AcpiHdr = (EFI_ACPI_DESCRIPTION_HEADER*)(UINTN)XTableAddress[Index];
    if (AcpiHdr->Signature == EFI_ACPI_1_0_APIC_SIGNATURE) {
      return (UINT32)(UINTN)AcpiHdr;
    }
  }

  ASSERT(FALSE);
  return 0;
}

UINT8 gIoAPicVector[PCI_HOST_MAX_COUNT + 1] = {0, 0x18, 0x38, 0x58, 0x78, 0x98, 0xB8, 0xD8, 0xF8}; 


UINT8 GetIoApicMaxEntry(UINT32 IoApicAddress)
{
  MmioWrite8(IoApicAddress, 1);
  return (UINT8)(MmioRead32(IoApicAddress+0x10) >> 16) + 1;
}

UINT8 GetIoApicId(UINT32 IoApicAddress)
{
  MmioWrite8(IoApicAddress, 0);
  return (UINT8)(MmioRead32(IoApicAddress+0x10) >> 24);
}


VOID
UpdateMadtAfterPciEnum (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS                     Status;
  VOID                           *Interface;
  EFI_ACPI_DESCRIPTION_HEADER    *AcpiHdr;  
  EFI_ACPI_1_0_IO_APIC_STRUCTURE *IoApic;
  PLATFORM_COMM_INFO             *Info;
  UINTN                          Index;

  Status = gBS->LocateProtocol(&gEfiPciEnumerationCompleteProtocolGuid, NULL, (VOID**)&Interface);
  if (EFI_ERROR(Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  AcpiHdr = (EFI_ACPI_DESCRIPTION_HEADER*)(UINTN)GetMadtTable();
  ASSERT(AcpiHdr != NULL);
  IoApic = (EFI_ACPI_1_0_IO_APIC_STRUCTURE*)((UINTN)AcpiHdr + gIoApicOffset);
 
  Info = GetPlatformCommInfo();

// Index: base + 0
// Data : base + 10
//
// 00[27:24] - ID
// 01[23:16] - vector

  IoApic[0].Type = EFI_ACPI_1_0_IO_APIC;
  IoApic[0].Length = sizeof(EFI_ACPI_1_0_IO_APIC_STRUCTURE);
  IoApic[0].IoApicId = (UINT8)Info->SbApicId;
  IoApic[0].IoApicAddress = Info->SbApicBase;
  IoApic[0].SystemVectorBase = gIoAPicVector[0];
  DEBUG((EFI_D_INFO, "SB IoApic:%X Id:%X C:%d %X\n", IoApic[0].IoApicAddress, IoApic[0].IoApicId, \
    GetIoApicMaxEntry(IoApic[0].IoApicAddress), GetIoApicId(IoApic[0].IoApicAddress)));
  
  for(Index=1; Index<Info->PciHostCount+1; Index++){
    IoApic[Index].Type = EFI_ACPI_1_0_IO_APIC;
    IoApic[Index].Length = sizeof (EFI_ACPI_1_0_IO_APIC_STRUCTURE);
    IoApic[Index].IoApicId = (UINT8)Info->NbApicId[Index-1];
    IoApic[Index].IoApicAddress = Info->NbApicBase[Index-1];
    IoApic[Index].SystemVectorBase = gIoAPicVector[Index];
    DEBUG((EFI_D_INFO, "NB IoApic:%X Id:%X C:%d %X\n", IoApic[Index].IoApicAddress, IoApic[Index].IoApicId, \
      GetIoApicMaxEntry(IoApic[Index].IoApicAddress), GetIoApicId(IoApic[Index].IoApicAddress)));
  }
  
  AcpiTableUpdateChksum(AcpiHdr);
  
}






VOID
BuildAcpiMadtTable (
  EFI_ACPI_TABLE_PROTOCOL  *AcpiTable
	)
{
  CPU_APIC_ID_INFO                                      *CpuApicIdTable;
  UINTN                                                 CpuCount;
  UINTN                                                 CpuIndex;
  UINTN                                                 TableSize;
  EFI_ACPI_1_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER   *Madt;
  EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC_STRUCTURE           *LocalApic;
  EFI_ACPI_1_0_IO_APIC_STRUCTURE                        *IoApic;
  EFI_ACPI_1_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE      *Iso;
  EFI_STATUS                                            Status;
  UINTN                                                 TableKey;
  VOID                                                  *Registration;
  PLATFORM_COMM_INFO                                    *Info;
  UINT8                                                 TempApicId;

  
  Info = GetPlatformCommInfo();
  GetCpuLocalApicInfo(&CpuApicIdTable, &CpuCount);

  TableSize = sizeof(EFI_ACPI_1_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER) +
              sizeof(EFI_ACPI_2_0_PROCESSOR_LOCAL_APIC_STRUCTURE) * CpuCount + 
              sizeof(gNmi) +
              sizeof(EFI_ACPI_1_0_IO_APIC_STRUCTURE) * (Info->PciHostCount + 1) +
              sizeof(gIso); 
  Madt = AllocateZeroPool(TableSize);
  ASSERT(Madt != NULL);

  Madt->Header.Signature = EFI_ACPI_1_0_APIC_SIGNATURE;
  Madt->Header.Length    = (UINT32)TableSize;
  Madt->Header.Revision  = 3;
  Madt->LocalApicAddress = 0xFEE00000;
  Madt->Flags            = EFI_ACPI_1_0_PCAT_COMPAT;

  LocalApic = (EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC_STRUCTURE*)(Madt+1);

  if(PcdGet8 (PcdAmdSmtMode) == 1){
    TempApicId = 0;
    for(CpuIndex=0; CpuIndex<(CpuCount >>1); CpuIndex++){
      LocalApic[CpuIndex].Type = EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC;
      LocalApic[CpuIndex].Length = sizeof(EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC_STRUCTURE);   
      LocalApic[CpuIndex].AcpiProcessorId = (UINT8)TempApicId;
      LocalApic[CpuIndex].ApicId = CpuApicIdTable[TempApicId].ApicId;   
      LocalApic[CpuIndex].Flags  = CpuApicIdTable[TempApicId].Flags;
      TempApicId +=2;
    }

    TempApicId = 1;
    for(; CpuIndex < CpuCount; CpuIndex++){
      LocalApic[CpuIndex].Type = EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC;
      LocalApic[CpuIndex].Length = sizeof(EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC_STRUCTURE);   
      LocalApic[CpuIndex].AcpiProcessorId = TempApicId;//(UINT8)CpuIndex;
      LocalApic[CpuIndex].ApicId = CpuApicIdTable[TempApicId].ApicId;
      LocalApic[CpuIndex].Flags  = CpuApicIdTable[TempApicId].Flags;
      TempApicId +=2;
    }

  } else {
    for(CpuIndex = 0; CpuIndex < CpuCount; CpuIndex++){
      LocalApic[CpuIndex].Type = EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC;
      LocalApic[CpuIndex].Length = sizeof(EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC_STRUCTURE);   
      LocalApic[CpuIndex].AcpiProcessorId = (UINT8)CpuIndex;
      LocalApic[CpuIndex].ApicId = CpuApicIdTable[CpuIndex].ApicId;
      LocalApic[CpuIndex].Flags  = CpuApicIdTable[CpuIndex].Flags;
    }
  }

  CopyMem(&LocalApic[CpuIndex], gNmi, sizeof(gNmi));

  IoApic = (EFI_ACPI_1_0_IO_APIC_STRUCTURE*)((UINTN)&LocalApic[CpuIndex] + sizeof(gNmi));
  gIoApicOffset = (UINTN)IoApic - (UINTN)Madt;

  Iso = (EFI_ACPI_1_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE*)(&IoApic[Info->PciHostCount + 1]);
  CopyMem(Iso, gIso, sizeof(gIso));

  PlatformUpdateTables((EFI_ACPI_COMMON_HEADER*)Madt);
  AcpiTableUpdateChksum(Madt);
  Status = AcpiTable->InstallAcpiTable(
                        AcpiTable,
                        Madt,
                        TableSize,
                        &TableKey
                        );
  DEBUG((EFI_D_INFO, "%a L%d %r\n", __FUNCTION__, __LINE__, Status));

  EfiCreateProtocolNotifyEvent (
    &gEfiPciEnumerationCompleteProtocolGuid,
    TPL_CALLBACK,
    UpdateMadtAfterPciEnum,
    NULL,
    &Registration
    ); 
  
  FreePool(CpuApicIdTable);
}



