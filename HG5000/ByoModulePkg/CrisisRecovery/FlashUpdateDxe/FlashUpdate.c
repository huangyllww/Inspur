/*++

Module Name:

  FlashUpdate.c

Abstract:

  This file contains flash update functions when system is under
  recovery mode or flash update mode.

--*/
#include "FlashUpdate.h"
#include <Protocol\NvMediaAccess.h>
#include <Pi\PiHob.h>
#include <Library\UefiBootServicesTableLib.h>
#include <Library\HobLib.h>
#include <Library\DebugLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Uefi\UefiSpec.h>
#include <Protocol\Spi.h>
#include <Protocol\FirmwareVolumeBlock.h>
#include <Library\BaseLib.h>
#include <Library\MemoryAllocationLib.h>
#include <Uefi\UefiInternalFormRepresentation.h>
#include <Library\PcdLib.h>
#include <Library\BaseMemoryLib.h>
#include <Protocol\SimpleTextIn.h>
#include <Library/CacheMaintenanceLib.h>
#include <ByoBiosInfo2.h>
#include <SecTable.h>
#include <Library/PrintLib.h>





EFI_STATUS
ReadKeyStroke (
  IN OUT EFI_INPUT_KEY      *Key
  );

/**
  Reset system according to a input attribute

  @param  Attribute             a value to determine what type reset
                                system will do

  @return  VOID

**/
VOID
ResetSystemByAttribute (
  UINT16        Attribute
  )
{
  EFI_STATUS        Status;
  UINT16            ResetType;
  EFI_INPUT_KEY     Key;
  UINTN             Index;
  UINTN             Seconds = 5;
  

  ResetType = Attribute & POWER_MARK;
  switch (ResetType) {
    
    case SHUT_DOWN:
      gST->ConOut->SetAttribute(gST->ConOut, EFI_BACKGROUND_BLACK | EFI_YELLOW);
      while(Seconds){
        Print (L"\rShutdown in %d seconds", Seconds);
        gBS->Stall(1000000);
        Seconds--;
      }
      gRT->ResetSystem (EfiResetShutdown, EFI_SUCCESS, 0, NULL);
      break;

    case RESET_COLD:
      for (Index = 10; Index > 0; Index --) {
        Status = ReadKeyStroke (&Key);
        if (!EFI_ERROR(Status)) {
          //
          // if ESC key, clean message and go to deadloop
          //
          if (Key.ScanCode == 0x0017) {
            Print (L"\r                                                  ");
            goto DeadLoop;
          }
          break;
        }
        Print (L"\rSystem will do reset in");
        gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_RED);
        Print (L" %d ", Index);
        gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_YELLOW);
        Print (L"seconds...");
        gBS->Stall (1000 * 1000);
      }
      gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
      break;

    case RESET_WARM:
      for (Index = 10; Index > 0; Index --) {
        Status = ReadKeyStroke (&Key);
        if (!EFI_ERROR(Status)) {
          //
          // if ESC key, clean message and go to deadloop
          //
          if (Key.ScanCode == 0x0017) {
            Print (L"\r                                                  ");
            goto DeadLoop;
          }
          break;
        }
        Print (L"\rSystem will do reset in");
        gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_RED);
        Print (L" %d ", Index);
        gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_YELLOW);
        Print (L"seconds..");
        gBS->Stall (1000 * 1000);
      }
      gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
      break;
      
    default:
      break;
  }

DeadLoop:
  CpuDeadLoop ();
}



/**
  This is a internal function to show ByoSoft copyrights ans warning messages

  @param BootMode         Indicate which mode system is under

  @retval EFI_SUCCESS     Show copyrights successfully

**/
EFI_STATUS
ShowCopyRightsAndWarning (
  IN EFI_BOOT_MODE      BootMode
)
{
  gST->ConOut->SetAttribute(gST->ConOut, EFI_BACKGROUND_BLACK | EFI_WHITE | EFI_BRIGHT);
  gST->ConOut->ClearScreen (gST->ConOut);
  gST->ConOut->EnableCursor(gST->ConOut, FALSE);
  
  gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_YELLOW);

  Print(L"              **************************************************************\n");
  Print(L"              *                   Byosoft Flash Update                     *\n");
  Print(L"              *         Copyright(C) 2006-2021, Byosoft Co.,Ltd.           *\n");
  Print(L"              *                   All rights reserved                      *\n");
  Print(L"              **************************************************************\n");

  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    Print(L"Warning: System is in Recovery Mode. Please don't shutdown system during erasing/programming flash!\n");
  } else if (BootMode == BOOT_ON_FLASH_UPDATE) {
    Print(L"Warning: System is in Flash Update Mode. Please don't shutdown system during erasing/programming flash!\n");
  }

  gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_WHITE | EFI_BRIGHT);
  
  return EFI_SUCCESS;
}




EFI_STATUS  
UpdateSpiBlock (
  NV_MEDIA_ACCESS_PROTOCOL *NvAcc,
  UINT8                    *Src,
  UINT8                    *Target,
  UINT32                   BlockBase,
  UINT32                   BlockSize
)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINTN       Temp;
  
  
  DEBUG((EFI_D_INFO, __FUNCTION__"(%X,%X,%X,%X)\n", (UINTN)Src, (UINTN)Target, BlockBase, BlockSize));  

  if(CompareMem(Src, Target, BlockSize) == 0){
    DEBUG((EFI_D_INFO, "Equal\n"));
    goto ProcExit;
  }

  Status = NvAcc->Erase(NvAcc, BlockBase, BlockSize, SPI_MEDIA_TYPE);
  if(EFI_ERROR(Status)){goto ProcExit;}
  Status = NvAcc->Write(NvAcc, BlockBase, Src, BlockSize, SPI_MEDIA_TYPE);
  if(EFI_ERROR(Status)){goto ProcExit;}

  Temp = BlockSize;  
  Status = NvAcc->Read(NvAcc, BlockBase, Target, &Temp, SPI_MEDIA_TYPE);
  if(EFI_ERROR(Status)){goto ProcExit;}
  
  if(CompareMem(Src, Target, BlockSize)){
    DEBUG((EFI_D_INFO, "Verify Err\n"));     
    Status = EFI_DEVICE_ERROR;
    goto ProcExit;    
  } else {
    DEBUG((EFI_D_INFO, "Verify OK\n")); 
  }
  
ProcExit:
  return Status;  
}   



EFI_STATUS 
GetBiosFileFvMain (
  IN  UINT8             *NewBios,
  IN  UINTN             NewBiosSize,
  OUT UINT8             **FvMain
)
{
  UINTN                       Index;
  UINTN                       Count;
  BIOS_SEC_TABLE_V1           *SecTable;
  EFI_STATUS                  Status = EFI_NOT_FOUND;
  EFI_FIRMWARE_VOLUME_HEADER  *FvHdr;
  UINT32                      Offset;


  Index = NewBiosSize & 0xFFFFFFF0;
  Count = 0;
  SecTable = NULL;
  
  while(Index > 16){
    Index -= 16;
    if(*(UINT64*)(NewBios+Index) == BIOS_SEC_TABLE_SIGN){
      SecTable = (BIOS_SEC_TABLE_V1*)(NewBios+Index);
      break;
    }
    Count += 16;
    if(Count > SIZE_1KB){
      break;
    }
  }
  if(SecTable == NULL){
    goto ProcExit;
  }

  Offset = SecTable->FvMainStart - SecTable->BiosStartAddress;
  FvHdr = (EFI_FIRMWARE_VOLUME_HEADER*)(NewBios + Offset);
  if(FvHdr->Signature != EFI_FVH_SIGNATURE){
    goto ProcExit;
  }

  *FvMain = NewBios + Offset;
  Status = EFI_SUCCESS;

ProcExit:
  return Status;
}




EFI_STATUS
GetBiosInfo2 (
  IN  UINT8                   *NewBios,
  IN  UINTN                   NewBiosSize,
  OUT MY_BYO_BIOS_INFO2_TMP   **BiosInfo
  )
{
  MY_BYO_BIOS_INFO2_TMP  *Info;
  UINTN          Index;
  VOID           *p;
  EFI_STATUS     Status;
  UINT8          *FvMain;
  UINTN          Size;
  

  Status = GetBiosFileFvMain(NewBios, NewBiosSize, &FvMain);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  Status = EFI_NOT_FOUND;
  for(Index=0;Index<SIZE_1KB;Index+=16){
    Info = (MY_BYO_BIOS_INFO2_TMP*)&FvMain[Index];
    if(Info->Header.Signature == BYO_BIOS_INFO2_SIGNATURE){
      Size = Info->Header.HeaderLength + Info->Header.Step_NUM * sizeof(FLASH_STEP);
      p = AllocatePool(Size);
      ASSERT(p!=NULL);
      CopyMem(p, Info, Size);
      *BiosInfo = (MY_BYO_BIOS_INFO2_TMP*)p;
      Status = EFI_SUCCESS;
      break;
    }      
  }

ProcExit:
  return Status;
}



#if 0
EFI_STATUS
HandleBiosBlockUpdate (
  BIOS_IMAGE_BLOCK_INFO     *BiosBlockInfo,
  UINT16                    Behavior,
  UINT8                     *NewBios,
  NV_MEDIA_ACCESS_PROTOCOL  *NvAcc,
  BYO_BIOS_INFO             *BiosInfo
  )
{
  UINT32            BlockBase;
  UINTN             Count;  
  UINT8             *Src;
  UINT8             *Target;
  UINTN             Index;
  CHAR8             *Str;
  BOOLEAN           ToUpdate = FALSE;
  UINT32            SpiAddress;
  UINT32            BlockSize;
  UINTN             Temp;
  EFI_STATUS        Status;      
  UINT8             *OldData = NULL;
  CHAR8             UpdateItemStr[64];
  

  switch(BiosBlockInfo->Type){
    case BIOS_BLOCK_TYPE_FV_SEC:
      Str = "FvSec";
      ToUpdate = TRUE;
      break;
      
    case BIOS_BLOCK_TYPE_FV_BB:
      Str = "FvBB";
      ToUpdate = TRUE;      
      break;
      
    case BIOS_BLOCK_TYPE_FV_MAIN:
      Str = "FvMain";
      ToUpdate = TRUE;      
      break;
      
    case BIOS_BLOCK_TYPE_FV_MICROCODE:
      Str = "MicroCode";
      if(Behavior & MICROCODE_UPDATE){
        ToUpdate = TRUE;   
      }
      break;

    case BIOS_BLOCK_TYPE_FV_NVRAM:
      Str = "Nvram";
      if(Behavior & NVSTORAGE_VARIABLE_UPDATE){
        ToUpdate = TRUE; 
      }
      break;
      
    case BIOS_BLOCK_TYPE_FV_LOGO:
      Str = "Logo";
      ToUpdate = TRUE;
      break;
      
    case BIOS_BLOCK_TYPE_ROM_HOLE:
      Str = "RomHole";
      ToUpdate = TRUE;
      break;
      
    case BIOS_BLOCK_TYPE_ROM_HOLE_SMBIOS:
      Str = "Smbios";
      break;

    case BIOS_BLOCK_TYPE_FV_BB_BU:
      Str = "FvBB_BU";
      if(Behavior & BB_BK_UPDATE){
        ToUpdate = TRUE; 
      }
      break;

    default:
      Str = "";
      break;
  }

  if(!ToUpdate){
    return EFI_SUCCESS;
  }

  if(BiosBlockInfo->Sign == 0){
    AsciiSPrint(UpdateItemStr, sizeof(UpdateItemStr), "%a", Str);
  } else {
    AsciiSPrint(UpdateItemStr, sizeof(UpdateItemStr), "%a_%d", Str, BiosBlockInfo->Sign+1);
  }

  DEBUG((EFI_D_INFO, "%a L:%X\n", UpdateItemStr, BiosBlockInfo->Size));

  OldData = AllocatePages(EFI_SIZE_TO_PAGES(BiosBlockInfo->Size));
  if(OldData == NULL){
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }

  if((BiosBlockInfo->Size % SIZE_64KB) != 0){
    BlockSize = SIZE_4KB;
  } else {
    BlockSize = SIZE_64KB;
  }

  for(SpiAddress=0; SpiAddress<BiosBlockInfo->Size; SpiAddress+=BlockSize){
    Temp = BlockSize;
    Status = NvAcc->Read(NvAcc, BiosBlockInfo->Base+SpiAddress, OldData+SpiAddress, &Temp, SPI_MEDIA_TYPE);    
    if(EFI_ERROR(Status)){goto ProcExit;}
    Print(L"\rReading %a(%02d%%)          ", UpdateItemStr, (SpiAddress+BlockSize)*100/BiosBlockInfo->Size); 
  }   

  Count = BiosBlockInfo->Size/BlockSize;  
  Src   = NewBios + BiosBlockInfo->Base - BiosInfo->Header.BiosBaseAddr;
  Target = OldData;
  BlockBase = BiosBlockInfo->Base;
  
  for(Index=0;Index<Count;Index++){
    Status = UpdateSpiBlock(NvAcc, Src, Target, BlockBase, BlockSize);
    if(EFI_ERROR(Status)){goto ProcExit;}      
    BlockBase += BlockSize;
    Src       += BlockSize;
    Target    += BlockSize;
    Print(L"\rUpdating %a(%02d%%)         ", UpdateItemStr, (Index+1)*100/Count); 
  }

  Print(L"\n");
  Status = EFI_SUCCESS;

ProcExit:
  if(OldData != NULL){
    FreePages(OldData, EFI_SIZE_TO_PAGES(BiosBlockInfo->Size));
  }
  return Status;
}
#endif






/**
  This procedure is used to update flash part and shutdown
  system by behavior parameter

  @param  BootMode              Indicate the system boot mode
  @param  Behavior              Indicate which fv part will be updated and
                                do which system reset type
  @param  FDImageBaseAddress    New BIOS image base address
  @param  FDImageLength         New BIOS image length

  @return  EFI_SUCCESS          Flash is updated successfully

**/
EFI_STATUS
FlashUpdate (
  EFI_BOOT_MODE           BootMode,
  UINT16                  Behavior,
  EFI_PHYSICAL_ADDRESS    FDImageBaseAddress,
  UINT64                  FDImageLength
  )
{
  NV_MEDIA_ACCESS_PROTOCOL                    *NvAcc;
  EFI_STATUS                                  Status;  
  UINTN                                       Index;
  MY_BYO_BIOS_INFO2_TMP                       *BiosInfo = NULL;
  UINT8                                       *NewBios;
  UINTN                                       NewBiosSize;
  UINT32                                      Op;
    

  DEBUG((EFI_D_INFO, "BM:%X, BH:%X, Image(%lX,%lX)\n", BootMode, Behavior, FDImageBaseAddress, FDImageLength));
  
  Status = gBS->LocateProtocol (
             &gEfiNvMediaAccessProtocolGuid,
             NULL,
             (VOID**)&NvAcc
             );
  ASSERT(!EFI_ERROR(Status));
  if(EFI_ERROR(Status)){goto ProcExit;}

  ShowCopyRightsAndWarning (BootMode);

  NewBios = (UINT8*)(UINTN)FDImageBaseAddress;
  NewBiosSize = (UINTN)FDImageLength;





//-----------------------------------------------------------------------------
// 1. Get Bios Info.
  Status = GetBiosInfo2(NewBios, NewBiosSize, &BiosInfo);
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "GetBiosInfo Error\n"));
    goto ProcExit;
  }

  if(BiosInfo->Header.BIOS_Size != (UINT32)FDImageLength){
    DEBUG((EFI_D_ERROR, "Bios Size Not Match\n"));
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;    
  }


  for(Index=0;Index<BiosInfo->Header.Step_NUM;Index++){
    Op = BiosInfo->Step[Index].Op;
    if(Op == OP_RESERVE_OEM__ || Op == OP_RESERVE_NVM__){

    }
  }

//DEBUG((EFI_D_INFO, "BiosUpdateOK!\n"));

ProcExit:
  if(BiosInfo!=NULL){FreePool(BiosInfo);}
  return EFI_UNSUPPORTED;
}



