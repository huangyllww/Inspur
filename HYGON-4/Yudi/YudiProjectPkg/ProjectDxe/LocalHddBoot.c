
#include "ProjectDxe.h"
#include <Library/UefiBootManagerLib.h>
#include <Protocol/LegacyBios.h>

VOID 
ProjectUpdateBootOption (
    VOID                          **BootOptions,
    UINTN                         *BootOptionCount
  )
{
  UINTN                         Index;
  UINTN                         Count;
  UINTN                         i;
  EFI_BOOT_MANAGER_LOAD_OPTION  *Options;
  EFI_BOOT_MANAGER_LOAD_OPTION  *p;
  EFI_DEVICE_PATH_PROTOCOL      *Dp;
  EFI_HANDLE                    DevHandle;
  EFI_STATUS                    Status;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  UINT8                         ClassCode[3];
  BOOLEAN                       *InvalidArray = NULL;
  UINTN                         NewCount;
  

  DEBUG((EFI_D_INFO, "ProjectUpdateBootOption\n"));

  if(BootOptions == NULL || BootOptionCount == NULL || *BootOptionCount == 0){
    return;
  }

  Count   = *BootOptionCount;
  Options = *BootOptions;

  if(0){             // LocalHddBootDis

    InvalidArray = (BOOLEAN*)AllocateZeroPool(Count * sizeof(BOOLEAN));
    ASSERT(InvalidArray != NULL);

    NewCount = Count;

    for(Index=0;Index<Count;Index++){
      if(Options[Index].OptionType != LoadOptionTypeBoot){
        continue;
      }
      if((Options[Index].Attributes & LOAD_OPTION_HIDDEN) || !(Options[Index].Attributes & LOAD_OPTION_ACTIVE)){
        continue;
      }

      Dp = Options[Index].FilePath;
      Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Dp, &DevHandle);
      if(!EFI_ERROR(Status)){
        gBS->HandleProtocol(DevHandle, &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
        PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 9, 3, ClassCode);
        DEBUG((EFI_D_INFO, "Class(%x,%x,%x)\n", ClassCode[2], ClassCode[1], ClassCode[0]));
        if(ClassCode[2] == 1 && ClassCode[1] != 0){
          InvalidArray[Index] = TRUE;
          NewCount--;
          DEBUG((EFI_D_INFO, "ignore %s\n", Options[Index].Description));          
        }
      }
    }

    DEBUG((EFI_D_INFO, "Count: %d -> %d\n", Count, NewCount));

    if(Count != NewCount){
      if(NewCount == 0){
        EfiBootManagerFreeLoadOptions(Options, Count);
        *BootOptions     = NULL;
        *BootOptionCount = 0; 
        goto ProcExit;
      }
      
      i = 0;
      p = (EFI_BOOT_MANAGER_LOAD_OPTION*)AllocatePool(sizeof(EFI_BOOT_MANAGER_LOAD_OPTION) * NewCount);
      ASSERT(p != NULL);
      for(Index=0;Index<Count;Index++){
        if(InvalidArray[Index]){
          EfiBootManagerFreeLoadOption(&Options[Index]);
        } else {
          CopyMem(&p[i++], &Options[Index], sizeof(EFI_BOOT_MANAGER_LOAD_OPTION));
        }
      }
      FreePool(*BootOptions);
      *BootOptions     = p;
      *BootOptionCount = NewCount;
    }
  }

ProcExit:
  if(InvalidArray != NULL){
    FreePool(InvalidArray);
  }
}





VOID LegacyLocalHDDBootDisable()
{
  BBS_TABLE                         *BbsTable;
  EFI_LEGACY_BIOS_PROTOCOL          *LegacyBios;
  HDD_INFO                          *HddInfo;
  UINT16                            HddCount;
  UINT16                            BbsCount;
  EFI_STATUS                        Status;
  UINTN                             Index;
 

  DEBUG((EFI_D_INFO, "LegacyLocalHDDBootDisable\n"));

  if(0){             // LocalHddBootDis
    Status = gBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, NULL, (VOID**)&LegacyBios);
    if (!EFI_ERROR(Status)) {
      Status = LegacyBios->GetBbsInfo (
                           LegacyBios,
                           &HddCount,
                           &HddInfo,
                           &BbsCount,
                           &BbsTable
                           );
      if (!EFI_ERROR(Status)) {
        for(Index=0;Index<BbsCount;Index++){
          DEBUG((EFI_D_INFO, "Class(%x,%x)\n", BbsTable[Index].Class, BbsTable[Index].SubClass));
          if (BbsTable[Index].BootPriority == BBS_IGNORE_ENTRY) {
            continue;
          }    
          if(BbsTable[Index].Class == 1 && BbsTable[Index].SubClass != 0){
            BbsTable[Index].BootPriority = BBS_IGNORE_ENTRY;
          }
        }
      }
    }
  }
}




