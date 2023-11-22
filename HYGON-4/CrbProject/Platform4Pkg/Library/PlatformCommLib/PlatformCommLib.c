
#include <Uefi.h>
#include <Pi/PiBootMode.h>      // HobLib.h +
#include <Pi/PiHob.h>           // HobLib.h +
#include <Uefi/UefiSpec.h>
#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/Atapi.h>
#include <IndustryStandard/Smbios.h>
#include <Uefi/UefiAcpiDataTable.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/AtaPassThru.h>
#include <Protocol/ScsiPassThruExt.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/PciIo.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/ScsiIo.h>
#include <PlatS3Record.h>
#include <Library/ByoRtcLib.h>


extern EFI_GUID gPlatformSetupVariableGuid;




VOID *GetSetupDataHobData(VOID)
{
  EFI_PEI_HOB_POINTERS  GuidHob;
  VOID                  *SetupData;

  GuidHob.Raw = GetFirstGuidHob(&gPlatformSetupVariableGuid);
  ASSERT(GuidHob.Raw != NULL);
  SetupData = (VOID*)(GuidHob.Guid+1);

  return SetupData;
}


VOID *GetPlatformCommInfo(VOID)
{
  EFI_PEI_HOB_POINTERS  GuidHob;
  VOID                  *Info;

  GuidHob.Raw = GetFirstGuidHob(&gEfiPlatformCommInfoGuid);
  ASSERT(GuidHob.Raw != NULL);
  Info = (VOID*)(GuidHob.Guid+1);
  ASSERT((*(UINT32*)Info) == PLAT_COMM_INFO_SIGN);

  return Info;
}

VOID *GetHygonCbsHobData(VOID)
{
  EFI_PEI_HOB_POINTERS  GuidHob;
  VOID                  *Cbs;

  GuidHob.Raw = GetFirstGuidHob(&gHygonCbsVariableHobGuid);
  ASSERT(GuidHob.Raw != NULL);
  Cbs = (VOID*)(GuidHob.Guid+1);

  return Cbs;
}




VOID *GetS3RecordTable()
{
  PLATFORM_S3_RECORD               *S3Record;

  S3Record = (PLATFORM_S3_RECORD*)(UINTN)PcdGet32(PcdS3RecordAddr);
  ASSERT(S3Record->Signature == PLAT_S3_RECORD_SIGNATURE);

  return (VOID*)S3Record;
}

VOID SetS3RecordTable(UINT32 Address)
{
  PcdSet32S(PcdS3RecordAddr, Address);
}




VOID
ConvertIdentifyDataFormat (
  IN  UINT8   *Data,
  IN  UINTN   DataSize
  )
{
  UINTN  Index;
  UINT8  Data8;

  ASSERT(DataSize!=0 && Data!=NULL);

// here has a hidden bug:
// Data size may be odd number, so it will destroy data out of range.
  DataSize &= ~BIT0;          // Fix: make it be even.
  for(Index = 0; Index < DataSize; Index += 2){
    Data8         = Data[Index];
    Data[Index]   = Data[Index+1];
    Data[Index+1] = Data8;
  }

}



#define DIDVID_MTT_S50_GFX                 0x01051ED5




UINT32 m3DControllerHaveVideoDidVid[] = {
  DIDVID_MTT_S50_GFX,
};

BOOLEAN
IsGfxClassCode (
  UINT8  ClassCode[3],
  UINT32 DIDVID
  )
{
  UINTN  Index;

  if(ClassCode[2] == 3 && ClassCode[1] == 0 && ClassCode[0] == 0){
    return TRUE;
  }

  if(ClassCode[2] == 3 && ClassCode[1] == 2 && ClassCode[0] == 0){
    for (Index = 0; Index < ARRAY_SIZE (m3DControllerHaveVideoDidVid); Index++) {
      //
      // Some devices have video output, we need to check it
      //
      if (DIDVID == m3DControllerHaveVideoDidVid[Index]) {
        return TRUE;
      }
    }
    //
    // We assume class code 3 2 0 devices have no video output as default
    //
    return FALSE;
  }

  return FALSE;
}




UINTN LibUpdateMBSizeUnit(UINTN SizeMB, CHAR8 *UnitChar)
{
  UINTN        Size;

  if(SizeMB >= 1024 && (SizeMB % 1024) == 0){
    Size      = SizeMB / 1024;
    *UnitChar = 'G';
  } else {
    Size     = SizeMB;
    *UnitChar = 'M';
  }

  return Size;
}



CHAR8 *LibGetPcieLinkSpeedStr(UINT8 LinkSpeed)
{
  CHAR8  *Str;

  switch(LinkSpeed){
    case 1:
      Str = "Gen1";
      break;
      
    case 2:
      Str = "Gen2";
      break;
      
    case 4:  
      Str = "Gen3";
      break; 
      
    case 8:
      Str = "Gen4";            
      break;
      
    case 16:
      Str = "Gen5";            
      break; 

    default:
      Str = "?";
      break;
  }

  return Str;
}

