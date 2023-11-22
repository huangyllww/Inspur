/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include "SmiFlash.h"
#include <Library/MemoryAllocationLib.h>


typedef struct {
     UINT32   Signature;
     UINT32   UpdateRev;
     UINT16   Year;
     UINT8    Day;
     UINT8    Month;
     UINT32   ProcessorId;
     UINT32   Checksum;   // entire file including the header.  +10
     UINT32   LoaderRevision;
     UINT32   ChipVer;    // +18h
     UINT32   DataSize;
     UINT32   TotalSize;
     UINT32   Reserved2;
     UINT32   Reserved3;
     UINT32   Reserved4;
} ZX_CPU_MICROCODE_HEADER;

#define ZX_MICROCODE_SIGNATURE      SIGNATURE_32('R', 'R', 'A', 'S')

#define DEFAULT_MC_ALIGNMENT           16
#define DEFAULT_MC_PAD_BYTE_VALUE      0xFF
#define MC_AREA_INFO_MAX_COUNT         16

typedef struct {
  UINT32 Cpuid;
  UINT32 ChipVer;
  UINT32 Offset;
  UINT32 Size;
} MC_AREA_INFO;


EFI_STATUS
ProgramFlash (
  IN  UINT32  Offset,
  IN  UINT32  Size,
  IN  UINT64  Buffer
);


EFI_STATUS HandleUcodeRequest(ROM_HOLE_PARAMETER *Param)
{
  UINT8                    *UcodeData;
  UINTN                    UcodeSize;
  ZX_CPU_MICROCODE_HEADER  *McHdr;
  ZX_CPU_MICROCODE_HEADER  *NewMcHdr;  
  UINT8                    *Buffer = NULL;
  UINT8                    *NewBuffer = NULL;  
  UINT8                    *McArea;
  UINTN                    McAreaSize;
  UINTN                    Index;
  UINT8                    *McEnd;
  UINT32                   McSingleSize;
  MC_AREA_INFO             McAreaInfo[MC_AREA_INFO_MAX_COUNT];
  UINTN                    McInfoCount = 0;
  UINTN                    MatchIndex = 0xFF;
  EFI_STATUS               Status = EFI_INVALID_PARAMETER;
  UINTN                    OldTotalSize;
  UINTN                    Offset;
  

  if(Param == NULL){
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }
  
  UcodeData = (UINT8*)(UINTN)Param->Buffer;
  UcodeSize = Param->BufferSize;

  if(UcodeData == NULL || UcodeSize == 0 || UcodeSize <= sizeof(ZX_CPU_MICROCODE_HEADER)){
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;    
  }

  NewMcHdr = (ZX_CPU_MICROCODE_HEADER*)UcodeData;
  if(NewMcHdr->Signature != ZX_MICROCODE_SIGNATURE ||
     NewMcHdr->TotalSize != UcodeSize ||
     (UcodeSize & 3) != 0 ||
      CalculateSum32((UINT32*)NewMcHdr, NewMcHdr->TotalSize)!=0){
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }

  McArea = (UINT8*)(UINTN)PcdGet32(PcdFlashNvStorageMicrocodeBase);
  McAreaSize = PcdGet32(PcdFlashNvStorageMicrocodeSize);
  if(McArea == NULL || McAreaSize == 0){
    Status = EFI_UNSUPPORTED;
    goto ProcExit;    
  }

  Buffer = AllocatePool(McAreaSize);
  if(Buffer == NULL){
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }
  NewBuffer = AllocatePool(McAreaSize);
  if(NewBuffer == NULL){
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }  

  CopyMem(Buffer, McArea, McAreaSize);
  ZeroMem(McAreaInfo, sizeof(McAreaInfo));
  SetMem(NewBuffer, McAreaSize, 0xFF);

  McHdr = (ZX_CPU_MICROCODE_HEADER*)Buffer;
  McEnd = Buffer + McAreaSize - sizeof(ZX_CPU_MICROCODE_HEADER);
  while((UINT8*)McHdr < McEnd){
    if(McHdr->Signature != ZX_MICROCODE_SIGNATURE || 
       CalculateSum32((UINT32*)McHdr, McHdr->TotalSize)!=0){
      break; 
    }  

    McSingleSize = ALIGN_VALUE(McHdr->TotalSize, DEFAULT_MC_ALIGNMENT);

    if(McInfoCount < MC_AREA_INFO_MAX_COUNT){
      if(NewMcHdr->ProcessorId == McHdr->ProcessorId && NewMcHdr->ChipVer == McHdr->ChipVer){
        MatchIndex = McInfoCount;
        DEBUG((EFI_D_INFO, "match@%d\n", MatchIndex));
      }
      McAreaInfo[McInfoCount].Cpuid   = McHdr->ProcessorId;
      McAreaInfo[McInfoCount].ChipVer = McHdr->ChipVer;
      McAreaInfo[McInfoCount].Offset  = (UINT32)((UINTN)McHdr - (UINTN)Buffer);
      McAreaInfo[McInfoCount].Size    = McSingleSize;
      DEBUG((EFI_D_INFO, "ucode[%d] %X V%d +%X L:%X\n", McInfoCount, \
        McAreaInfo[McInfoCount].Cpuid, McAreaInfo[McInfoCount].ChipVer, \
        McAreaInfo[McInfoCount].Offset, McAreaInfo[McInfoCount].Size));
      McInfoCount++;
    }
 
    McHdr = (ZX_CPU_MICROCODE_HEADER*)((UINTN)McHdr + McSingleSize);  
  }

  OldTotalSize = (UINTN)McHdr - (UINTN)Buffer;

  if(MatchIndex == 0xFF){
    if(OldTotalSize + NewMcHdr->TotalSize > McAreaSize){
      Status = EFI_VOLUME_FULL;
      goto ProcExit;
    }
    CopyMem(NewBuffer, Buffer, OldTotalSize); 
    CopyMem(NewBuffer + OldTotalSize, NewMcHdr, NewMcHdr->TotalSize);
    
  } else {
    if(OldTotalSize - McAreaInfo[MatchIndex].Size + NewMcHdr->TotalSize > McAreaSize){
      Status = EFI_VOLUME_FULL;
      goto ProcExit;
    }
    Offset = 0;
    for(Index=0;Index<McInfoCount;Index++){
      if(Index != MatchIndex){
        CopyMem(NewBuffer + Offset, Buffer + McAreaInfo[Index].Offset, McAreaInfo[Index].Size);
        Offset += McAreaInfo[Index].Size;
      } else {
        CopyMem(NewBuffer + Offset, NewMcHdr, NewMcHdr->TotalSize);
        Offset += ALIGN_VALUE(NewMcHdr->TotalSize, DEFAULT_MC_ALIGNMENT);
      }
    }
  }

  Offset = (UINTN)McArea - PcdGet32(PcdFlashAreaBaseAddress);
  for(Index=0;Index<McAreaSize;Index+=SIZE_4KB){
    Status = ProgramFlash (
               (UINT32)(Offset + Index), 
               SIZE_4KB, 
               (UINTN)(NewBuffer + Index)
               );
    if(EFI_ERROR(Status)){
      break;
    }
  }

ProcExit:
  if(Buffer != NULL){
    FreePool(Buffer);
  }
  if(NewBuffer != NULL){
    FreePool(NewBuffer);
  }  

  return Status;
}


