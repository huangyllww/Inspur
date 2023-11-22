

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ByoCommLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/PlatHwInfoProtocol.h>
#include <Protocol/Smbios.h>




STATIC CHAR8 *gDimmTypeString[] = {"SODIMM DDR5","UDIMM DDR5","RDIMM DDR5","LRDIMM DDR5","UnKnown"};


VOID
EFIAPI
PlatAmdSmbiosDxeInitReadyCallBack (
  IN      EFI_EVENT                 Event,
  IN      VOID                      *Context
  )
{
  VOID                       *Dummy;
  EFI_STATUS                 Status;
  EFI_SMBIOS_HANDLE          SmbiosHandle;
  EFI_SMBIOS_TYPE            SmbiosType;
  EFI_SMBIOS_TABLE_HEADER    *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER   p;
  UINT32                     SingleSize;
  UINTN                      Index;
  EFI_SMBIOS_PROTOCOL        *Smbios;
  CHAR8                      *PartNumberNew;
  UINTN                      Size;
  CHAR8                      *CpuName = NULL;
  UINTN                      CpuSockets;
  UINTN                      DimmSlots;
  PLAT_HW_INFO               *HwInfo = NULL;
  PLAT_DIMM_INFO             *DimmInfo = NULL;
  UINT8                      DimmType;
  CHAR8                      *Dw;
  PLATFORM_COMM_INFO         *PlatCommInfo;
  UINT16                     CpuFreq = 0;
  CHAR8                      *s;
  

  Status = gBS->LocateProtocol(&gHygonSmbiosDxeInitCompleteProtocolGuid, NULL, (VOID**)&Dummy);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);


  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if(EFI_ERROR(Status)){
    return;
  }

  PlatCommInfo = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  CpuSockets = 0;
  while(1){
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status)){
      break;
    }
    p.Hdr = SmbiosHdr;

    if(p.Type4->CoreCount == 0){
      continue;
    }

    if(CpuName == NULL){
      CpuName = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type4->ProcessorVersion);
      CpuFreq = p.Type4->CurrentSpeed;
    }
    CpuSockets++;
  }

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
  DimmSlots = 0;
  while(1){
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status)){
      break;
    }
    DimmSlots++;
  }

  HwInfo = (PLAT_HW_INFO*)AllocateZeroPool(sizeof(PLAT_HW_INFO));
  if(HwInfo == NULL){
    goto ErrExit;
  }

  DimmInfo = (PLAT_DIMM_INFO*)AllocateZeroPool(sizeof(PLAT_DIMM_INFO) * DimmSlots);
  if(DimmInfo == NULL){
    goto ErrExit;
  }  

  if(CpuName == NULL){
    HwInfo->CpuName = "unknown";
  } else {
    HwInfo->CpuName = AllocateCopyPool(AsciiStrSize(CpuName), CpuName);
  }
  HwInfo->CpuCount = CpuSockets;


  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
  Index = 0;
  while(1){
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status)){
      break;
    }
    p.Hdr = SmbiosHdr;
    s = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type17->Manufacturer);
    DimmInfo[Index].Manufacturer = AllocateCopyPool(AsciiStrSize(s), s);
    s = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type17->PartNumber);
    DimmInfo[Index].PartNumber = AllocateCopyPool(AsciiStrSize(s), s);   
    s = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type17->SerialNumber);
    DimmInfo[Index].SerialNumber = AllocateCopyPool(AsciiStrSize(s), s);
    s = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type17->BankLocator);
    DimmInfo[Index].BankLocator = AllocateCopyPool(AsciiStrSize(s), s);
    s = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type17->DeviceLocator);
    DimmInfo[Index].DeviceLocator = AllocateCopyPool(AsciiStrSize(s), s);

    SingleSize = p.Type17->Size;
    if(SingleSize == 0 || SingleSize == 0xFFFF){
      SingleSize = 0;
    } else if(SingleSize == 0x7FFF){
      SingleSize = p.Type17->ExtendedSize & (~BIT31);   // MB
    } else {
      if(SingleSize & BIT15){                           // unit in KB.
        SingleSize = (SingleSize&(~BIT15)) >> 10;       // to MB
      }else{                                            // unit in MB.
      }
    }
    DimmInfo[Index].DimmSizeMB = SingleSize;
    HwInfo->DimmTotalSizeMB += SingleSize;


    if(SingleSize){

      Size = AsciiStrSize(DimmInfo[Index].PartNumber);
      PartNumberNew = AllocatePool(Size);
      ASSERT(PartNumberNew != NULL);
      AsciiStrCpyS(PartNumberNew, Size, DimmInfo[Index].PartNumber);
      TrimStr8(PartNumberNew);
      DimmInfo[Index].PartNumber = PartNumberNew;
      
      if(HwInfo->MemSpeed == 0){
        HwInfo->MemSpeed = p.Type17->ConfiguredMemoryClockSpeed;
      }
      switch(p.Type17->FormFactor){
        case MemoryFormFactorDimm:
          if(p.Type17->TypeDetail.Registered)
            DimmType = 2; //RDIMM
          else if(p.Type17->TypeDetail.Unbuffered)
            DimmType = 1; //UDIMM
          else
            DimmType = 3; //LRDIMM
          break;
        case MemoryFormFactorSodimm:
          DimmType = 0;     //SODIMM
          break;
        default:
          DimmType = 4;     //UnKnown
          break;
      }
      DimmInfo[Index].TypeStr = gDimmTypeString[DimmType];

      Dw = LibSmbiosGetTypeEnd(p.Type17) - 4;
      if(Dw[0] == 'W' && Dw[1] >= '0' && Dw[1] <= '9' && Dw[2] >= '0' && Dw[2] <= '9'){
        DimmInfo[Index].DevWidth = (Dw[1] - '0') * 10 + (Dw[2] - '0');
      }
      DimmInfo[Index].Ranks = (p.Type17->Attributes)&0xF;
      DimmInfo[Index].Ecc   = (p.Type17->TotalWidth > p.Type17->DataWidth);

      DimmInfo[Index].Present = 1;
      HwInfo->DimmCount++;

    }else {
      Dw = LibSmbiosGetTypeEnd(p.Type17) - 4;
      if(CompareMem(Dw, "MNT", 4) == 0){
        DimmInfo[Index].Present   = 1;
        DimmInfo[Index].NoTrained = 1;
      }
    }

    Index++;
  }


  for(Index=0; Index<DimmSlots; Index++){

    if(DimmInfo[Index].DimmSizeMB == 0){
      continue;
    }

    if(HwInfo->FirstDimmSizeMB == 0){
      HwInfo->FirstDimmSizeMB = DimmInfo[Index].DimmSizeMB;
    }

    if(HwInfo->FirstDimmTypeStr == NULL){
      HwInfo->FirstDimmTypeStr = DimmInfo[Index].TypeStr;
    }
    
    if(HwInfo->FirstDimmManu == NULL){
      HwInfo->FirstDimmManu = DimmInfo[Index].Manufacturer;
    } else {
      if(AsciiStrCmp(HwInfo->FirstDimmManu, DimmInfo[Index].Manufacturer) != 0){
        HwInfo->IsDimmMixed = TRUE;
        break;
      }
    }
    if(HwInfo->FirstDimmPn == NULL){
      HwInfo->FirstDimmPn = DimmInfo[Index].PartNumber;
    } else {
      if(AsciiStrCmp(HwInfo->FirstDimmPn, DimmInfo[Index].PartNumber) != 0){
        HwInfo->IsDimmMixed = TRUE;
        break;
      }
    }
  }

  if(HwInfo->FirstDimmManu == NULL){HwInfo->FirstDimmManu = "Unknown";}
  if(HwInfo->FirstDimmPn == NULL)  {HwInfo->FirstDimmPn = "";}
  if(HwInfo->FirstDimmTypeStr == NULL) {HwInfo->FirstDimmTypeStr = "";}

  HwInfo->CpuFreq       = CpuFreq;
  HwInfo->CpuPhySockets = PlatCommInfo->CpuPhySockets;
  HwInfo->DimmInfo = DimmInfo;
  HwInfo->DimmInfoCount = DimmSlots;
  HwInfo->Signature = PLAT_HW_INFO_SIGNATURE;

  Status = gBS->InstallProtocolInterface (
                  &gImageHandle,
                  &gPlatformHwInfoReadyProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  HwInfo
                  );
  return;


ErrExit:
  if(HwInfo != NULL){
    FreePool(HwInfo);
  }
  if(DimmInfo != NULL){
    FreePool(DimmInfo);
  }  
}


EFI_STATUS
GetHwInfo (
    VOID
  )
{
  VOID  *Registration;

  EfiCreateProtocolNotifyEvent (
    &gHygonSmbiosDxeInitCompleteProtocolGuid,
    TPL_CALLBACK,
    PlatAmdSmbiosDxeInitReadyCallBack,
    NULL,
    &Registration
    );
  return EFI_SUCCESS;
}

