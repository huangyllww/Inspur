
#include "PlatformBm.h"
#include <Library/PerformanceLib.h>
#include <Library/ByoCommLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/BootLogoLib.h>

#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/LegacyBios.h>

#include <IndustryStandard/Pci.h>
#include <SysMiscCfg.h>


EFI_STATUS
InstallAddOnOpRom (
  EFI_HANDLE                    PciHandle,
  EFI_PCI_IO_PROTOCOL           *PciIo,
  EFI_LEGACY_BIOS_PROTOCOL      *LegacyBios
  )
{
  EFI_STATUS  Status;
  UINT64      Supports;
  UINTN       Flags;
  

  Status = PciIo->Attributes (
                    PciIo,
                    EfiPciIoAttributeOperationSupported,
                    0,
                    &Supports
                    );
  if (!EFI_ERROR (Status)) {
    Supports &= EFI_PCI_DEVICE_ENABLE;
    Status = PciIo->Attributes (
                      PciIo,
                      EfiPciIoAttributeOperationEnable,
                      Supports,
                      NULL
                      );
  }
  if (EFI_ERROR (Status)) {
    goto ProcExit;
  }

  Status = LegacyBios->CheckPciRom (
                         LegacyBios,
                         PciHandle,
                         NULL,
                         NULL,
                         &Flags
                         );
  if (EFI_ERROR (Status)) {
    goto ProcExit;
  }

  Status = LegacyBios->InstallPciRom (
                         LegacyBios,
                         PciHandle,
                         NULL,
                         &Flags,
                         NULL,
                         NULL,
                         NULL,
                         NULL
                         );

ProcExit:
  return Status;
}




BOOLEAN
IsContainImage (
  IN VOID            *RomImage,
  IN UINT64          RomSize,
  IN UINT8           CodeType
  )
{
  PCI_EXPANSION_ROM_HEADER  *RomHeader;
  PCI_DATA_STRUCTURE        *RomPcir;
  UINT8                     Indicator;

  Indicator = 0;
  RomHeader = RomImage;
  if (RomHeader == NULL || RomSize == 0) {
    return FALSE;
  }

  do {
    if (RomHeader->Signature != PCI_EXPANSION_ROM_HEADER_SIGNATURE) {
      RomHeader = (PCI_EXPANSION_ROM_HEADER *) ((UINT8 *) RomHeader + 512);
      continue;
    }

    //
    // The PCI Data Structure must be DWORD aligned.
    //
    if (RomHeader->PcirOffset == 0 ||
        (RomHeader->PcirOffset & 3) != 0 ||
        (UINT8 *) RomHeader + RomHeader->PcirOffset + sizeof (PCI_DATA_STRUCTURE) > (UINT8 *) RomImage + RomSize) {
      break;
    }

    RomPcir = (PCI_DATA_STRUCTURE *) ((UINT8 *) RomHeader + RomHeader->PcirOffset);
    if (RomPcir->Signature != PCI_DATA_STRUCTURE_SIGNATURE) {
      break;
    }

    if (RomPcir->CodeType == CodeType) {
      return TRUE;
    }

    Indicator = RomPcir->Indicator;
    RomHeader = (PCI_EXPANSION_ROM_HEADER *) ((UINT8 *) RomHeader + RomPcir->ImageLength * 512);
  } while (((UINT8 *) RomHeader < (UINT8 *) RomImage + RomSize) && ((Indicator & 0x80) == 0x00));

  return FALSE;
}



VOID
EFIAPI
InstallAdditionalOpRom (
  VOID
  )
{
  EFI_LEGACY_BIOS_PROTOCOL      *LegacyBios;
  EFI_STATUS                    Status;
  UINTN                         HandleCount;
  EFI_HANDLE                    *HandleBuffer = NULL;
  UINTN                         Index;
  UINTN                         TableIndex;
  UINTN                         TableCount;	
  UINTN                         Flags;
  VOID                          *LocalRomImage;
  UINTN                         LocalRomSize;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  UINT16                        VendorId;
  UINT16                        DeviceId;
  BOOLEAN                       RunCheck = FALSE;
  UINT8                         ClassCode[3];
  PLAT_HOST_INFO_PROTOCOL       *ptHostInfo;
  ADDITIONAL_ROM_TABLE          *RomTable;
  UINT8                         BootModeType = PcdGet8(PcdBiosBootModeType);
  

  DEBUG((EFI_D_INFO, "InstallAdditionalOpRom\n"));

  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **)&LegacyBios
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "LegacyBios Not found! Skip\n"));
    goto ProcExit;
  }

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, (VOID **)&ptHostInfo);
  ASSERT(!EFI_ERROR(Status));

  TableCount = ptHostInfo->OptionRomTableSize/sizeof(ADDITIONAL_ROM_TABLE);
  RomTable   = ptHostInfo->OptionRomTable;

  if(BootModeType == BIOS_BOOT_UEFI_OS){            // UEFI ONLY
    for (Index = 0; Index < TableCount; Index++) {
      RomTable[Index].Enable = FALSE;
    }
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  ASSERT(!EFI_ERROR(Status));

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    ASSERT(!EFI_ERROR(Status));

    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, PCI_VENDOR_ID_OFFSET, 1, &VendorId);
    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, PCI_DEVICE_ID_OFFSET, 1, &DeviceId);

    PciIo->Pci.Read (
                 PciIo,
                 EfiPciIoWidthUint8,
                 PCI_CLASSCODE_OFFSET,
                 sizeof(ClassCode),
                 &ClassCode[0]
                 );

    for (TableIndex = 0; TableIndex < TableCount; TableIndex++) {
      if(!RomTable[TableIndex].Enable) {
        continue;
      }
      if(RomTable[TableIndex].RunCheck == NULL){
        RunCheck = (RomTable[TableIndex].VendorId == VendorId && RomTable[TableIndex].DeviceId == DeviceId);
      } else {
        RunCheck = RomTable[TableIndex].RunCheck(HandleBuffer[Index], PciIo, ptHostInfo);
      }
	  
      if(RunCheck){

        if(ClassCode[2] == PCI_CLASS_NETWORK && 
           IsContainImage(PciIo->RomImage, PciIo->RomSize, PCI_CODE_TYPE_PCAT_IMAGE)){
          InstallAddOnOpRom(HandleBuffer[Index], PciIo, LegacyBios);
          break;
        }
        
        Status = GetSectionFromAnyFv (
                   RomTable[TableIndex].RomImageGuid,
                   EFI_SECTION_RAW,
                   0,
                   &LocalRomImage,
                   &LocalRomSize
                   );
        if (!EFI_ERROR (Status) && LocalRomImage != NULL && LocalRomSize != 0) {
          Status = LegacyBios->InstallPciRom (
                                 LegacyBios,
                                 HandleBuffer[Index],
                                 &LocalRomImage,
                                 &Flags,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL
                                 );
          break;
        }
      }
    }
    if(TableIndex < TableCount){
      continue;
    }			
  

    if (PciIo->RomImage == NULL || PciIo->RomSize == 0) {
      continue;
    }

    if (((ClassCode[2] == PCI_CLASS_DISPLAY) && (ClassCode[1] == PCI_CLASS_DISPLAY_VGA)) ||
        ((ClassCode[2] == PCI_CLASS_OLD) && (ClassCode[1] == PCI_CLASS_OLD_VGA))) {
      continue;
    }

    if(ClassCode[2] == PCI_CLASS_NETWORK || 
       ClassCode[2] == PCI_CLASS_MASS_STORAGE ||
       (ClassCode[2] == PCI_CLASS_SERIAL && ClassCode[1] == PCI_CLASS_SERIAL_FIBRECHANNEL)){
      InstallAddOnOpRom(HandleBuffer[Index], PciIo, LegacyBios);
    }  

  }

  //
  // enable logo at the end of InstallAdditionalOpRom()
  //
  if ((BootModeType != BIOS_BOOT_UEFI_OS) && (!(PcdGet32(PcdSystemMiscConfig) & SYS_MISC_CFG_DIS_SHOW_LOGO))) {
    BootLogoEnableLogo ();
  }

ProcExit:
  if(HandleBuffer != NULL){
    gBS->FreePool(HandleBuffer);
  }
  return;
}


