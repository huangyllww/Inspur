/** @file

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ProjectEarlyDxe.c
Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ByoCommLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Pi/PiBootMode.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/BiosIdLib.h>
#include <Library/PrintLib.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <Library/ByoCommLib.h>
#include <Protocol/ByoEarlySimpleTextOutProtocol.h>
#include <Library/PlatformCommLib.h>
#include <SetupVariable.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PlatformCommLib.h>
#include <Protocol/PlatHwInfoProtocol.h>

EFI_STATUS
EFIAPI
NetLibIp6ToStr (
  IN         EFI_IPv6_ADDRESS  *Ip6Address,
  OUT        CHAR16            *String,
  IN         UINTN             StringSize
  );

EFI_STATUS PrjStatusCodeHandleInit(CONST SETUP_DATA *SetupHob);




VOID
EFIAPI
PrjPlatformHwInfoReadyCallBack (
  IN      EFI_EVENT                 Event,
  IN      VOID                      *Context
  )
{
  EFI_STATUS                 Status;
  UINTN                      Index;
  CHAR8                      *Buffer;
  UINT8                      x = 1, y = 1;
  PLAT_HW_INFO               *PlatHwInfo;
  UINTN                      BufferSize = 256;
  PLAT_DIMM_INFO             *DimmInfo;
  CHAR8                      *s;
  BYO_EARLY_SIMPLE_TEXT_OUTPUT_PROTOCOL  *EarlySto = NULL;
  CHAR8                                  UnitChar;
  UINTN                                  Size;
  PLATFORM_COMM_INFO                     *Info;
  CHAR16                                 IpStr[46];
  CHAR8                                  *BmcVer;
  UINTN                                  i;
  UINT8                                  Socket;
  UINT8                                  Channel;
  UINT8                                  Dimm;
  UINT8                                  ThisIndex;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  if(1){ //j_t, Assert when get DIMM Info.
    return;
  }

  Status = gBS->LocateProtocol(&gPlatformHwInfoReadyProtocolGuid, NULL, (VOID**)&PlatHwInfo);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  Status = gBS->LocateProtocol(&gByoEarlySimpleTextOutProtocolGuid, NULL, (VOID**)&EarlySto);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "EarlySto not found\n"));
    EarlySto = NULL;
  }

  Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  Buffer = AllocatePool(BufferSize);
  if(Buffer == NULL){
    return;
  }
  ASSERT(PlatHwInfo->CpuName != NULL);

  for(Index=0; Index<PlatHwInfo->DimmInfoCount; Index++){

    DimmInfo = &PlatHwInfo->DimmInfo[Index];

// Locator: CPU0_DIMMK0
// Bank Locator: P0 CHANNEL K
// SSCCCCD
    DimmInfo->Socket  = 0;
    DimmInfo->Channel = 0;
    DimmInfo->Dimm    = 0;
    s = AsciiStrStr(DimmInfo->BankLocator, "P");
    if(s != NULL && s[1] >= '0' && s[1] <= '9'){
      DimmInfo->Socket = s[1] - '0';
    }
    s = AsciiStrStr(DimmInfo->BankLocator, "CHANNEL ");
    if(s != NULL && s[8] >= 'A' && s[8] <= 'L'){
      DimmInfo->Channel = s[8] - 'A';
    }
    s = AsciiStrStr(DimmInfo->DeviceLocator, "DIMM");
    if(s != NULL && (s[5] == '0' || s[5] == '1')){
      DimmInfo->Dimm = s[5] - '0';
    }

    DimmInfo->Index = 0xFF;
  }

  ThisIndex = 0;
  for(Socket=0; Socket<2; Socket++){
    for(Channel=0; Channel<12; Channel++){
      for(Dimm=0; Dimm<2; Dimm++){
        for(Index=0; Index<PlatHwInfo->DimmInfoCount; Index++){
          DimmInfo = &PlatHwInfo->DimmInfo[Index];
          if(DimmInfo->Index == 0xFF && DimmInfo->Socket == Socket && DimmInfo->Channel == Channel && DimmInfo->Dimm == Dimm){
            DimmInfo->Index = ThisIndex++;
            DEBUG((EFI_D_INFO, "I:%d\n", DimmInfo->Index));
          }
        }  
      }
    }
  }

  DEBUG((EFI_D_OEM, "\n\n========== DIMM INFO ==========>\n"));

  for(Index=0; Index<PlatHwInfo->DimmInfoCount; Index++){

    for(i=0; i<PlatHwInfo->DimmInfoCount; i++){
      if(PlatHwInfo->DimmInfo[i].Index == Index){
        break;
      }
    }
    ASSERT(i<PlatHwInfo->DimmInfoCount);
    if(i >= PlatHwInfo->DimmInfoCount){
      continue;
    }
    DimmInfo = &PlatHwInfo->DimmInfo[i];

    if(DimmInfo->Present && !DimmInfo->NoTrained){
      Size = LibUpdateMBSizeUnit(DimmInfo->DimmSizeMB, &UnitChar);
      AsciiSPrint(
        Buffer,
        BufferSize,
        "[Socket %d Channel %d Dimm %d] Manu:%a, PN:%a, Size:%d%cB, %dRx%d, Type:%a, ECC:%a, SN:%a",
        DimmInfo->Socket, DimmInfo->Channel, DimmInfo->Dimm,
        DimmInfo->Manufacturer,
        DimmInfo->PartNumber,
        Size,
        UnitChar,
        DimmInfo->Ranks,
        DimmInfo->DevWidth,
        DimmInfo->TypeStr,
        DimmInfo->Ecc ? "Yes" : "No",
        DimmInfo->SerialNumber
        );
      TrimStr8(Buffer);
      DEBUG((EFI_D_OEM, "%a\n", Buffer));

    }else if(DimmInfo->NoTrained) {
      DEBUG((EFI_D_OEM, "[Socket %d Channel %d Dimm %d] DIMM Present, but Not Trained\n", DimmInfo->Socket, DimmInfo->Channel, DimmInfo->Dimm));
    } else {
      DEBUG((EFI_D_OEM, "[Socket %d Channel %d Dimm %d] DIMM Not Present\n", DimmInfo->Socket, DimmInfo->Channel, DimmInfo->Dimm));
    }
  }


  AsciiSPrint(Buffer, BufferSize, "CPU: %a @%dMHz * %d", PlatHwInfo->CpuName, PlatHwInfo->CpuFreq, PlatHwInfo->CpuCount);
  if(EarlySto != NULL){  
    EarlySto->OutputString(x, y++, Buffer);
  }
  DEBUG((EFI_D_OEM, "%a\n", Buffer));

  Size = LibUpdateMBSizeUnit(PlatHwInfo->DimmTotalSizeMB, &UnitChar);
  AsciiSPrint(Buffer, BufferSize, "MEM: %d%cB, %d MT/s", Size, UnitChar, PlatHwInfo->MemSpeed);
  if(EarlySto != NULL){  
    EarlySto->OutputString(x, y++, Buffer);
  }
  DEBUG((EFI_D_OEM, "%a\n", Buffer));

  if(EarlySto != NULL){ 

    if(Info->BmcIpType == 4){
      UnicodeSPrint(IpStr, sizeof(IpStr), L"%d.%d.%d.%d", 
        Info->BmcIp.v4.Addr[0], Info->BmcIp.v4.Addr[1], Info->BmcIp.v4.Addr[2], Info->BmcIp.v4.Addr[3]);
    } else if(Info->BmcIpType == 6){
      Status = NetLibIp6ToStr(&Info->BmcIp.v6, IpStr, sizeof(IpStr));
      if(EFI_ERROR(Status)){
        IpStr[0] = 0;
      }
    } else {
      IpStr[0] = 0;
    }
    
    BmcVer = PcdGetPtr(PcdBmcFwVerStr8);
    if(BmcVer[0] || IpStr[0]){
      AsciiSPrint(Buffer, BufferSize, "BMC: %a%a%a%s", 
                    BmcVer[0] == 0 ? "" : "v", BmcVer,
                    IpStr[0]  == 0 ? "" : " IP ", IpStr
                    );
      EarlySto->OutputString(x, y++, Buffer);
    }

  }
  
  DEBUG((EFI_D_OEM, "\n\n"));

  FreePool(Buffer);
}



EFI_STATUS
ProjectEarlyDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  VOID                *Registration;
  CONST SETUP_DATA    *SetupHob;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  SetupHob = (CONST SETUP_DATA*)GetSetupDataHobData();

  EfiCreateProtocolNotifyEvent (
    &gPlatformHwInfoReadyProtocolGuid,
    TPL_CALLBACK,
    PrjPlatformHwInfoReadyCallBack,
    NULL,
    &Registration
    );

  PrjStatusCodeHandleInit(SetupHob);
  return EFI_SUCCESS;
}



