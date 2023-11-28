

#include "AcpiPlatform.h"
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DxeServicesLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <AcpiRam.h>
#include <Library/PlatformCommLib.h>
#include <Protocol/PciEnumerationComplete.h>


SETUP_DATA           *gSetupData;
EFI_ACPI_RAM_DATA    *gAcpiRam;


STATIC UINT8 gGnvsAmlArray[] = {
  0x5B, 0x80,               // OpRegionOp
  'G', 'N', 'V', 'S',       // Name                         +0
  0x00,                     // RegionSpace(SystemMemory)
  0x0C,                     // DWordPrefix
  0x00, 0x00, 0xFF, 0xFF,   // default base                 +6
  0x0B,                     // WordPrefix
  0x55, 0xAA                // default length               +11
};  

/*
UINT32 GetDsdtTable(VOID)
{
  EFI_ACPI_4_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Rsdp;
  EFI_ACPI_DESCRIPTION_HEADER                   *Xsdt;
  UINTN                                         Index;
  UINT64                                        *XTableAddress;
  UINTN                                         TableCount;
  EFI_STATUS                                    Status;	
  EFI_ACPI_DESCRIPTION_HEADER                   *AcpiHdr;
	EFI_ACPI_4_0_FIXED_ACPI_DESCRIPTION_TABLE     *Fadt;
  

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
    if (AcpiHdr->Signature == EFI_ACPI_4_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
      Fadt = (EFI_ACPI_4_0_FIXED_ACPI_DESCRIPTION_TABLE*)AcpiHdr;
      AcpiHdr = (EFI_ACPI_DESCRIPTION_HEADER*)(UINTN)Fadt->XDsdt;
      ASSERT(AcpiHdr->Signature == EFI_ACPI_4_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE);
      return (UINT32)(UINTN)AcpiHdr;
    }
  }

  ASSERT(FALSE);
  return 0;
}
*/


VOID AcpiAfterConnectPciRootBridgeHook()
{
  PLATFORM_COMM_INFO             *Info;
  UINTN                          Index;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Info = GetPlatformCommInfo();

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

    DEBUG((EFI_D_INFO, "  MMIO[%d] %X %X\n", Index, gAcpiRam->RpIoBase[Index], gAcpiRam->RpIoSize[Index]));
    DEBUG((EFI_D_INFO, "    IO[%d] %X %X\n", Index, gAcpiRam->RpMmioBase[Index], gAcpiRam->RpMmioSize[Index]));
    DEBUG((EFI_D_INFO, "MMIO64[%d] %lX %lX\n", Index, gAcpiRam->RpMmio64Base[Index], gAcpiRam->RpMmio64Size[Index]));    
    DEBUG((EFI_D_INFO, "   Bus[%d] %X %X\n", Index, gAcpiRam->RpBusBase[Index], gAcpiRam->RpBusLimit[Index])); 
  }

  if(gAcpiRam->RpIoBase[0] == 0){
    gAcpiRam->RpIoBase[0]  = 0xD00;
    gAcpiRam->RpIoSize[0] -= 0xD00;
  }

  DEBUG((EFI_D_INFO, "-> IO[0] %X %X\n", gAcpiRam->RpIoBase[0], gAcpiRam->RpIoSize[0]));  
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
          DEBUG((EFI_D_ERROR, "GNVS Found! -> (%X,%X)\n", (UINT32)(UINTN)gAcpiRam, sizeof(EFI_ACPI_RAM_DATA)));
          GnvsFound = TRUE;
        }
        break; 
    }

    if(GnvsFound){
      break;
    }
  }
}

 

EFI_STATUS
PlatformUpdateTables (
  IN OUT EFI_ACPI_COMMON_HEADER   *Table
  )
{
  EFI_ACPI_DESCRIPTION_HEADER     *TableHeader;


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
    case EFI_ACPI_2_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:  
      UpdateDsdt(Table);
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
  PLATFORM_COMM_INFO             *Info;


  Status = gBS->LocateProtocol(&gBdsAllDriversConnectedProtocolGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  Info = GetPlatformCommInfo();  
  ASSERT(Info->VgaDieId < PCI_HOST_MAX_COUNT);
  gAcpiRam->VgaIoEn[Info->VgaDieId] = 1;  
}


void UpdatefTpmPresent()
{
  if(CompareGuid (PcdGetPtr(PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm20HgfTpmGuid)){
    gAcpiRam->fTpmPresent = 1;
  }
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

	
  DEBUG((EFI_D_INFO, __FUNCTION__"()\n"));

  gSetupData = (SETUP_DATA*)GetSetupDataHobData();

  Status = gBS->LocateProtocol(&gEfiAcpiTableProtocolGuid, NULL, &AcpiTable);
  ASSERT_EFI_ERROR (Status);

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

  UpdatefTpmPresent();
  
//BuildAcpiSpcrTable(AcpiTable);
  BuildAcpiMadtTable(AcpiTable);
  BuildAcpiCpuSsdtTable(AcpiTable);
  BuildAcpiMcfgTable(AcpiTable);

  EfiCreateProtocolNotifyEvent (
    &gBdsAllDriversConnectedProtocolGuid,
    TPL_CALLBACK,
    AcpiAllDriversConnectedCallBack,
    NULL,
    &Registration
    );  
  
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

      Sign = (CHAR8*)(&CurrentTable->Signature);
      DEBUG((EFI_D_ERROR, "%c%c%c%c\n", Sign[0], Sign[1], Sign[2], Sign[3]));
      ASSERT(Size >= CurrentTable->Length);
      
      PlatformUpdateTables(CurrentTable);
      AcpiTableUpdateChksum(CurrentTable);
      Status = AcpiTable->InstallAcpiTable (
                            AcpiTable,
                            CurrentTable,
                            CurrentTable->Length,
                            &TableKey
                            );
      ASSERT_EFI_ERROR(Status);
      gBS->FreePool(CurrentTable);
      CurrentTable = NULL;      
    }  
  }
  
  if (FvHandleBuffer != NULL) {
    gBS->FreePool(FvHandleBuffer);
  }

  Status = gBS->InstallMultipleProtocolInterfaces(
                  &ImageHandle,
                  &gEfiAfterConnectPciRootBridgeGuid, AcpiAfterConnectPciRootBridgeHook,
                  NULL
                  );

ProcExit:         
  return EFI_SUCCESS;
}





