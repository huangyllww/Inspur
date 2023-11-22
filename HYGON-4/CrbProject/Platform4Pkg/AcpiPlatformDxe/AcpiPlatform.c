

#include "AcpiPlatform.h"
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DxeServicesLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <AcpiRam.h>
#include <Library/PlatformCommLib.h>
#include <Protocol/PciEnumerationComplete.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/FabricNumaServicesProtocol.h>
#include <Protocol/FabricTopologyServices.h>
#include <Protocol/ReportRasUceProtocol.h>
#include <Library/ByoHygonFabricLib.h>


EFI_STATUS BuildMpTable();


STATIC EFI_GUID gSetupBmcCfgFileGuid =
  {0xCEDC062A, 0x84F6, 0x4ffb, {0x8E, 0xD5, 0xA4, 0x3D, 0x8E, 0xBD, 0x5B, 0x50}};


SETUP_DATA           *gSetupData;
EFI_ACPI_RAM_DATA    *gAcpiRam;
PLATFORM_COMM_INFO   *gPlatCommInfo;


STATIC UINT8 gGnvsAmlArray[] = {
  0x5B, 0x80,               // OpRegionOp
  'G', 'N', 'V', 'S',       // Name                         +0
  0x00,                     // RegionSpace(SystemMemory)
  0x0C,                     // DWordPrefix
  0x00, 0x00, 0xFF, 0xFF,   // default base                 +6
  0x0B,                     // WordPrefix
  0x55, 0xAA                // default length               +11
};



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




VOID
UpdateDsdt (
  EFI_ACPI_COMMON_HEADER   *AcpiHdr
  )
{
  UINT8                           *CurrPtrEnd;
  UINT8                           *CurrPtrStart;
  UINT8                           *DsdtPointer;
  BOOLEAN                         GnvsFound = FALSE;


  CurrPtrStart    = (UINT8*)(AcpiHdr + 1);
  CurrPtrEnd = CurrPtrStart + AcpiHdr->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER);
  for (DsdtPointer = CurrPtrStart; DsdtPointer <= CurrPtrEnd; DsdtPointer++) {
    switch(ReadUnaligned32((UINT32*)DsdtPointer)) {
      case (SIGNATURE_32('G', 'N', 'V', 'S')):
        if(CompareMem(DsdtPointer-2, gGnvsAmlArray, sizeof(gGnvsAmlArray))==0){
          *(UINT32*)(DsdtPointer + 6) = (UINT32)(UINTN)gAcpiRam;
          *(UINT16*)(DsdtPointer +11) = sizeof(EFI_ACPI_RAM_DATA);
          DEBUG((EFI_D_INFO, "GNVS Found! -> (%X,%X)\n", (UINT32)(UINTN)gAcpiRam, sizeof(EFI_ACPI_RAM_DATA)));
          GnvsFound = TRUE;
        }
        break;
    }

    if(GnvsFound){
      break;
    }
  }
}




VOID
UpdateApicTable (
  EFI_ACPI_COMMON_HEADER   *AcpiHdr
  )
{
  EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE *LocalX2Apic;
  UINTN                                         Index, Count;
  HYGON_REPORT_UCE_PROTOCOL                     *ReportUce;
  EFI_STATUS                                    Status;

  DEBUG((EFI_D_INFO, "UpdateApicTable\n"));

  Status = gBS->LocateProtocol (
                       &gHygonReportRasUceProtocolGuid,
                       NULL,
                       (VOID**)&ReportUce
                       );
  if (EFI_ERROR (Status)) {
    return;
  }

  LocalX2Apic = (EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE*)((UINTN)AcpiHdr + sizeof(EFI_ACPI_4_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER));
  Count     = AcpiHdr->Length;
  Index     = sizeof(EFI_ACPI_4_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER);
  while(Index < Count){
    if(LocalX2Apic->Type == EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC){
      if(ReportUce->IsThisCoreUce(ReportUce, LocalX2Apic->X2ApicId)){
        LocalX2Apic->Flags = 0;
        DEBUG((EFI_D_INFO, "disable cpu core(apic:%d)\n", LocalX2Apic->X2ApicId));
      }
    } 
    if(LocalX2Apic->Length == 0){
      break;
    }
    Index      += LocalX2Apic->Length;
    LocalX2Apic = (EFI_ACPI_4_0_PROCESSOR_LOCAL_X2APIC_STRUCTURE*)((UINTN)LocalX2Apic + LocalX2Apic->Length);
  }

  DEBUG((EFI_D_INFO, "UpdateApicTableExit\n"));  
}


EFI_STATUS
PlatformUpdateTables (
  IN OUT EFI_ACPI_COMMON_HEADER   *Table
  )
{
  EFI_ACPI_DESCRIPTION_HEADER                *TableHeader;
  EFI_ACPI_6_0_FIXED_ACPI_DESCRIPTION_TABLE  *Fadt;


  TableHeader = (EFI_ACPI_DESCRIPTION_HEADER*)Table;

  if (Table->Signature != EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE) {
    CopyMem(TableHeader->OemId, PcdGetPtr(PcdAcpiDefaultOemId), sizeof(TableHeader->OemId));

    // Skip OEM table ID and creator information for DSDT, SSDT and PSDT tables, since these are
    // created by an ASL compiler and the creator information is useful.
    if (Table->Signature != EFI_ACPI_2_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE &&
        Table->Signature != EFI_ACPI_2_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE &&
        Table->Signature != EFI_ACPI_2_0_PERSISTENT_SYSTEM_DESCRIPTION_TABLE_SIGNATURE
        ) {
      TableHeader->OemTableId      = PcdGet64(PcdAcpiDefaultOemTableId);
      TableHeader->OemRevision     = PcdGet32(PcdAcpiDefaultOemRevision);
      TableHeader->CreatorId       = PcdGet32(PcdAcpiDefaultCreatorId);
      TableHeader->CreatorRevision = PcdGet32(PcdAcpiDefaultCreatorId);
    }
  }

  switch (Table->Signature) {

    case EFI_ACPI_1_0_APIC_SIGNATURE:
      UpdateApicTable(Table);
      break;
    
    case EFI_ACPI_2_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
      UpdateDsdt(Table);
      break;

    case EFI_ACPI_6_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE:
      Fadt = (EFI_ACPI_6_0_FIXED_ACPI_DESCRIPTION_TABLE*)Table;
      if(gSetupData->PcieAspm == PCIE_ASPM_DISABLE){
        DEBUG((EFI_D_INFO, "Fadt Disable ASPM\n"));
        Fadt->IaPcBootArch |= EFI_ACPI_6_0_PCIE_ASPM_CONTROLS;
      } else {
        DEBUG((EFI_D_INFO, "Fadt Enable ASPM\n"));        
        Fadt->IaPcBootArch &= ~EFI_ACPI_6_0_PCIE_ASPM_CONTROLS;
      }
      if(PcdGet8(PcdHygonCStateMode)){
        Fadt->PLvl2Lat = 100;
      } else {
        Fadt->PLvl2Lat = 101;
      }
      Fadt->SmiCmd = PcdGet16(PcdHygonFchCfgSmiCmdPortAddr);
      break;
      
    default:
      break;
  }

  return EFI_SUCCESS;
}






VOID
EFIAPI
AcpiAllDriversConnectedCallBack (
  IN      EFI_EVENT                 Event,
  IN      VOID                      *Context
  )
{
  VOID                           *Interface;
  EFI_STATUS                     Status;
  VOID                           *Dummy;
  PLAT_HOST_INFO_PROTOCOL        *HostInfo;
  EFI_DEVICE_PATH_PROTOCOL       *UartDp;
  UINT16                         IoBase;
  EFI_ACPI_TABLE_PROTOCOL        *AcpiTable;
  PLATFORM_COMM_INFO             *Info = gPlatCommInfo;
  UINTN                          Index;
  UINTN                          Count;


  Status = gBS->LocateProtocol(&gBdsAllDriversConnectedProtocolGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  Status = gBS->LocateProtocol(&gEfiAcpiTableProtocolGuid, NULL, &AcpiTable);
  ASSERT_EFI_ERROR (Status);

  BuildAcpiMcfgTable(AcpiTable);
  BuildAcpiMadtTable(AcpiTable, Info);
  BuildAcpiCpuSsdtTable(AcpiTable);
//BuildMpTable();

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, (VOID**)&HostInfo);
  if(!EFI_ERROR(Status)){
    UartDp = HostInfo->GetPlatUcrDp(NULL, &IoBase);
    if(UartDp != NULL){
      BuildAcpiSpcrTable(AcpiTable, UartDp, IoBase);
    }
  }

  ASSERT(Info->VgaUid < PCI_HOST_MAX_COUNT);
  gAcpiRam->VgaUid = Info->VgaUid;

  if(!gSetupData->NumaDisable){
    Count = ARRAY_SIZE(gAcpiRam->Pxm);
    Status = LibGetPxm(gBS, gAcpiRam->Pxm, &Count);
    DEBUG((EFI_D_INFO, "LibGetPxm %d, %d\n", Count, Info->PciHostCount));
    ASSERT(!EFI_ERROR(Status));
    ASSERT(Count == Info->PciHostCount);
  }  

  Status = gBS->LocateProtocol(&gSetupBmcCfgFileGuid, NULL, (VOID**)&Dummy);
  if(!EFI_ERROR(Status)){
    gAcpiRam->BmcPresent = 1;
  } else {
    gAcpiRam->BmcPresent = 0;
  } 
  DEBUG((EFI_D_INFO, "BmcPresent:%d\n", gAcpiRam->BmcPresent));

  gAcpiRam->RpCount = (UINT8)Info->PciHostCount;

  for(Index=0;Index<Info->PciHostCount;Index++){
    gAcpiRam->RpMmioBase[Index] = Info->MmioBase[Index];
    gAcpiRam->RpMmioSize[Index] = Info->MmioSize[Index];
    gAcpiRam->RpIoBase[Index] = Info->IoBase[Index];
    gAcpiRam->RpIoSize[Index] = Info->IoSize[Index];
    gAcpiRam->RpMmio64Base[Index] = Info->Mmio64Base[Index];
    gAcpiRam->RpMmio64Size[Index] = Info->Mmio64Size[Index];
    gAcpiRam->RpBusBase[Index] = Info->BusBase[Index];
    gAcpiRam->RpBusLimit[Index] = Info->BusLimit[Index];

    DEBUG((EFI_D_INFO, "  IO[%d] %X %X\n", Index, gAcpiRam->RpIoBase[Index], gAcpiRam->RpIoSize[Index]));
    DEBUG((EFI_D_INFO, "MMIO[%d] %X %X\n", Index, gAcpiRam->RpMmioBase[Index], gAcpiRam->RpMmioSize[Index]));
    DEBUG((EFI_D_INFO, "MM64[%d] %lX %lX\n", Index, gAcpiRam->RpMmio64Base[Index], gAcpiRam->RpMmio64Size[Index]));
    DEBUG((EFI_D_INFO, " Bus[%d] %X %X\n", Index, gAcpiRam->RpBusBase[Index], gAcpiRam->RpBusLimit[Index]));
  }

  if(gAcpiRam->RpIoBase[0] == 0){
    gAcpiRam->RpIoBase[0]  = 0xD00;
    gAcpiRam->RpIoSize[0] -= 0xD00;
  }
  DEBUG((EFI_D_INFO, "-> IO[0] %X %X\n", gAcpiRam->RpIoBase[0], gAcpiRam->RpIoSize[0]));  
}



EFI_STATUS
EFIAPI
AcpiPlatformInit (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv2;
  INTN                          Instance;
  EFI_ACPI_COMMON_HEADER        *CurrentTable;
  UINTN                         TableKey;
  UINT32                        FvStatus;
  UINTN                         Size;
  UINTN                         FvHandleCount;
  EFI_HANDLE                    *FvHandleBuffer = NULL;
  CHAR8                         *Sign;
  UINTN                         Index;
  VOID                          *Registration;
  EFI_PHYSICAL_ADDRESS          MemoryAddress;
  BOOLEAN                       NeedHandleTable;
  BOOLEAN                       TableFileFound;


  DEBUG((EFI_D_INFO, "AcpiPlatformInit\n"));

  Status = gBS->LocateProtocol(&gEfiAcpiTableProtocolGuid, NULL, &AcpiTable);
  ASSERT_EFI_ERROR (Status);

  gSetupData    = (SETUP_DATA*)GetSetupDataHobData();
  gPlatCommInfo = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  MemoryAddress = SIZE_4GB - 1;
  Size = sizeof(EFI_ACPI_RAM_DATA);
  Status = gBS->AllocatePages(
                  AllocateMaxAddress,
                  EfiACPIMemoryNVS,
                  EFI_SIZE_TO_PAGES(Size),
                  &MemoryAddress
                  );
  ASSERT(!EFI_ERROR(Status));
  gAcpiRam = (EFI_ACPI_RAM_DATA*)(UINTN)MemoryAddress;
  ZeroMem(gAcpiRam, sizeof(EFI_ACPI_RAM_DATA));
  gAcpiRam->Signature = ACPI_RAM_DATA_SIGNATURE;
  gAcpiRam->PcieBaseAddress = (UINT32)FixedPcdGet64(PcdPciExpressBaseAddress);
  gAcpiRam->PcieBaseLength  = (UINT32)FixedPcdGet64(PcdPciExpressBaseSize);
  gAcpiRam->S4En = gSetupData->S4Support;
  if(CompareGuid(PcdGetPtr(PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm20HgfTpmGuid)){
    gAcpiRam->fTPMPresent = 1;
  }
  DEBUG((EFI_D_INFO, "fTPMPresent:%d, AcpiRam(%x,%x)\n", \
    gAcpiRam->fTPMPresent, gAcpiRam, sizeof(EFI_ACPI_RAM_DATA)));


  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &FvHandleCount,
                  &FvHandleBuffer
                  );
  ASSERT(!EFI_ERROR(Status));
  if(EFI_ERROR(Status) || FvHandleCount==0){
    goto ProcExit;
  }

  TableFileFound = FALSE;
  for (Index = 0; Index < FvHandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    FvHandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID**)&Fv2
                    );
    ASSERT(!EFI_ERROR(Status));

    Instance = 0;
    while(1){
      CurrentTable = NULL;
      Size = 0;
      Status = Fv2->ReadSection (
                      Fv2,
                      (EFI_GUID*)PcdGetPtr(PcdAcpiTableStorageFile),
                      EFI_SECTION_RAW,
                      Instance++,
                      (VOID**)&CurrentTable,
                      &Size,
                      &FvStatus
                      );
      if(EFI_ERROR(Status)){
        break;
      }

      TableFileFound = TRUE;

      Sign = (CHAR8*)(&CurrentTable->Signature);
      DEBUG((EFI_D_INFO, "%c%c%c%c\n", Sign[0], Sign[1], Sign[2], Sign[3]));
      ASSERT(Size >= CurrentTable->Length);

      NeedHandleTable = TRUE;
      if(gSetupData->HpetDis && CurrentTable->Signature == EFI_ACPI_3_0_HIGH_PRECISION_EVENT_TIMER_TABLE_SIGNATURE){
        NeedHandleTable = FALSE;
      }

      if(NeedHandleTable){
        PlatformUpdateTables(CurrentTable);
        AcpiTableUpdateChksum(CurrentTable);
        Status = AcpiTable->InstallAcpiTable (
                              AcpiTable,
                              CurrentTable,
                              CurrentTable->Length,
                              &TableKey
                              );
        ASSERT_EFI_ERROR(Status);
      }
      gBS->FreePool(CurrentTable);
      CurrentTable = NULL;
    }

    if(TableFileFound){
      break;
    }
  }

  if (FvHandleBuffer != NULL) {
    gBS->FreePool(FvHandleBuffer);
  }

  EfiCreateProtocolNotifyEvent (
    &gBdsAllDriversConnectedProtocolGuid,
    TPL_CALLBACK,
    AcpiAllDriversConnectedCallBack,
    NULL,
    &Registration
    );

ProcExit:
  return EFI_SUCCESS;
}





