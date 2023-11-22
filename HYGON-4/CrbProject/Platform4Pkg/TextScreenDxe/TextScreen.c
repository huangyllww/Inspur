/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

By:
  winddy_zhang

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/


#include <Library/HiiLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <ByoBootGroup.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>
#include <protocol/DiskInfo.h>
#include <IndustryStandard/Atapi.h>
#include <Library/PlatformCommLib.h>
#include <Library/ByoCommLib.h>
#include <Protocol/BdsBootManagerProtocol.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/AtaPassThru.h>
#include <SysMiscCfg.h>
#include <Guid/MdeModuleHii.h>
#include <EfiServerManagement.h>
#include <Library/NetLib.h>
#include <Protocol/PlatHwInfoProtocol.h>


extern EFI_GUID gEfiIpmiTransportProtocolGuid;


EFI_HII_HANDLE    mHiiHandle = NULL;
EFI_GUID mTextScreenStringPackGuid = {
  0xDCEFF379, 0xD6E5, 0x41C9, { 0xBD, 0x83, 0xB1, 0xC4, 0x64, 0x51, 0xC1, 0x77 }
};

#define CHARACTER_NUMBER_FOR_VALUE  30

UINTN     MaxWidth  = 0;
UINTN     EraseLine = 0;



UINTN
GetStringWidth (
  IN CHAR16               *String
  )
{
  UINTN Index;
  UINTN Count;
  UINTN IncrementValue;

  ASSERT (String != NULL);
  if (String == NULL) {
    return 0;
  }

  Index           = 0;
  Count           = 0;
  IncrementValue  = 1;

  do {
    //
    // Advance to the null-terminator or to the first width directive
    //
    for (;
         (String[Index] != NARROW_CHAR) && (String[Index] != WIDE_CHAR) && (String[Index] != 0);
         Index++, Count = Count + IncrementValue
        )
      ;

    //
    // We hit the null-terminator, we now have a count
    //
    if (String[Index] == 0) {
      break;
    }
    //
    // We encountered a narrow directive - strip it from the size calculation since it doesn't get printed
    // and also set the flag that determines what we increment by.(if narrow, increment by 1, if wide increment by 2)
    //
    if (String[Index] == NARROW_CHAR) {
      //
      // Skip to the next character
      //
      Index++;
      IncrementValue = 1;
    } else {
      //
      // Skip to the next character
      //
      Index++;
      IncrementValue = 2;
    }
  } while (String[Index] != 0);

  //
  // Increment by one to include the null-terminator in the size
  //
  Count++;

  return Count * sizeof (CHAR16);
}




VOID
TextScreenPrintString(
  IN CONST CHAR16  *Format,
  ...
  )
{
  VA_LIST     Marker;
  CHAR16      *Buffer;
  UINTN       BufferSize;
  UINTN       Return;

  BufferSize = (100 + 1) * sizeof (CHAR16);

  Buffer = (CHAR16 *) AllocatePool(BufferSize);
  ASSERT (Buffer != NULL);

  VA_START (Marker, Format);
  Return = UnicodeVSPrint (Buffer, BufferSize, Format, Marker);
  VA_END (Marker);

  if (Return > 0) {
    gST->ConOut->OutputString (gST->ConOut, Buffer);
    MaxWidth = MAX(GetStringWidth(Buffer), MaxWidth);
    EraseLine++;
  }
  FreePool (Buffer);
}




BOOLEAN
IsOsDisk (
  IN  EFI_BOOT_MANAGER_LOAD_OPTION    *Option
  )
{
  UINTN    Len;

  Len = StrLen(Option->Description);
  if(Len == 0){
    return FALSE;
  }

  if(Option->Description[Len-1] != L')'){           // need end with ')'
    return FALSE;    
  }
  
  if(StrStr(Option->Description, L"(") == NULL){
    return FALSE;
  }

  return TRUE;
}





VOID ShowMemoryInfo(PLAT_HW_INFO *PlatHwInfo)
{
  CHAR16                            *MemTotalStr;
  CHAR16                            *MixedMemStr;
  CHAR16                            *TotalStr;
  UINTN                             Size1, Size2;
  CHAR8                             UnitChar1, UnitChar2;
  
  
  MemTotalStr = HiiGetString(mHiiHandle, STRING_TOKEN(STR_SYS_MEMORY), NULL);
  MixedMemStr = HiiGetString(mHiiHandle, STRING_TOKEN(STR_MIXED_DIMM), NULL);
  TotalStr    = HiiGetString(mHiiHandle, STRING_TOKEN(STR_TOTAL), NULL);

  if(!PlatHwInfo->IsDimmMixed){
    Size1 = LibUpdateMBSizeUnit(PlatHwInfo->FirstDimmSizeMB, &UnitChar1), 
    Size2 = LibUpdateMBSizeUnit(PlatHwInfo->DimmTotalSizeMB, &UnitChar2),   
    TextScreenPrintString (
      L"%s: %a %a(%a %d%cB) * %d, %s %d%cB\n", 
      MemTotalStr, 
      PlatHwInfo->FirstDimmManu, 
      PlatHwInfo->FirstDimmPn,
      PlatHwInfo->FirstDimmTypeStr, 
      Size1,
      UnitChar1,
      PlatHwInfo->DimmCount, 
      TotalStr, 
      Size2,
      UnitChar2
      );
  } else {
    Size2 = LibUpdateMBSizeUnit(PlatHwInfo->DimmTotalSizeMB, &UnitChar2),   
    TextScreenPrintString (
      L"%s: %s * %d, %s %d%cB\n", 
      MemTotalStr, 
      MixedMemStr, 
      PlatHwInfo->DimmCount, 
      TotalStr, 
      Size2,
      UnitChar2
      );
  }

  FreePool(MemTotalStr);
  FreePool(MixedMemStr);
  FreePool(TotalStr);  
}

VOID ShowBmcInfo(PLATFORM_COMM_INFO *Info)
{
  CHAR16                            *BmcIpAddressStr;
  CHAR16                            *BmcVersionStr;
  CHAR16                            *InvalidStr;
  CHAR16                            Str[46];
  EFI_STATUS                        Status;
  UINT8                             *p;
  CHAR8                             *BmcVer;
  

  DEBUG((EFI_D_INFO, "ShowBmcInfo\n"));

  BmcIpAddressStr = HiiGetString(mHiiHandle, STRING_TOKEN(STR_BMC_IP), NULL);
  BmcVersionStr   = HiiGetString(mHiiHandle, STRING_TOKEN(STR_BMC_VERSION), NULL);
  InvalidStr      = HiiGetString(mHiiHandle, STRING_TOKEN(STR_BMC_IP_INVALID), NULL);  

  BmcVer = PcdGetPtr(PcdBmcFwVerStr8);
  if(BmcVer[0] != 0){
    TextScreenPrintString(L"%s: %a\n", BmcVersionStr, BmcVer);
  }
  if(Info->BmcIpType == 4){
    p = Info->BmcIp.v4.Addr;
    UnicodeSPrint(Str, sizeof(Str), L"%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
    TextScreenPrintString(L"%s: %s\n", BmcIpAddressStr, Str);
  } else if(Info->BmcIpType == 6){
    Status = NetLibIp6ToStr(&Info->BmcIp.v6, Str, sizeof(Str));
    if(!EFI_ERROR(Status)){
      TextScreenPrintString(L"%s: %s\n", BmcIpAddressStr, Str);
    }
  } else {
    TextScreenPrintString(L"%s: %s\n", BmcIpAddressStr, InvalidStr);
  }

  FreePool(BmcIpAddressStr);
  FreePool(BmcVersionStr);
  FreePool(InvalidStr);
}



VOID ShowBiosInfo(EFI_SMBIOS_PROTOCOL *Smbios)
{
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  EFI_SMBIOS_TYPE                   SmbiosType; 
  EFI_STATUS                        Status;
  EFI_SMBIOS_TABLE_HEADER           *SmbiosHdr;
  CHAR8                             *BiosVer;
  CHAR8                             *BiosDate;
  CHAR16                            *BiosVersionDateStr;
  SMBIOS_STRUCTURE_POINTER          p;
  

  BiosVersionDateStr = HiiGetString(mHiiHandle, STRING_TOKEN(STR_BIOS_VERSION_DATE), NULL);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_BIOS_INFORMATION;
  Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
  ASSERT(!EFI_ERROR(Status));
  p.Hdr = SmbiosHdr;
  BiosVer = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type0->BiosVersion);
  BiosDate = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type0->BiosReleaseDate);

  TextScreenPrintString(L"%s: %a %a\n", BiosVersionDateStr, BiosVer, BiosDate);
  
  FreePool(BiosVersionDateStr);
}



VOID ShowCpuInfo(PLAT_HW_INFO *PlatHwInfo)
{
  CHAR16                            *CpuInfoStr;

  CpuInfoStr = HiiGetString(mHiiHandle, STRING_TOKEN(STR_CPU_INFO), NULL);
  if(PlatHwInfo->CpuName != NULL && PlatHwInfo->CpuCount){
    TextScreenPrintString(L"%s: %a @%dMHz * %d\n", CpuInfoStr, PlatHwInfo->CpuName, PlatHwInfo->CpuFreq, PlatHwInfo->CpuCount);
  }
  FreePool(CpuInfoStr);
}



VOID ShowOsDiskInfo()
{
  EFI_STATUS                      Status;
  UINTN                           Index;
  EFI_BDS_BOOT_MANAGER_PROTOCOL   *BdsBootManager;
  BOOLEAN                         OsHDDExist;

  OsHDDExist = FALSE;
  Status = gBS->LocateProtocol (&gEfiBootManagerProtocolGuid,
                                NULL,
                                (VOID **)&BdsBootManager);
  if (EFI_ERROR (Status)) {
    return ;
  }
  
  for (Index = 0; Index < BdsBootManager->BootOptionsCount ; Index++) {
    if (((BdsBootManager->BootOptions[Index].Attributes & LOAD_OPTION_HIDDEN) != 0) || ((BdsBootManager->BootOptions[Index].Attributes & LOAD_OPTION_ACTIVE) == 0)) {
      continue;
    }
    if (BdsBootManager->BootOptions[Index].GroupType != BM_MENU_TYPE_UEFI_HDD) {
      continue;
    }
    if (!IsOsDisk(&BdsBootManager->BootOptions[Index])) { 
      continue;
    }
    OsHDDExist = TRUE;
    TextScreenPrintString(L"%s\n", BdsBootManager->BootOptions[Index].Description);
  }
  if (OsHDDExist) {
    gBS->Stall(2000000);
  }
}



// SMART RETURN STATUS
//   Features = DAh
//   LBA_Mid  = 4Fh
//   LBA_High = C2h
//   Command  = B0h
// If the device has not detected a threshold exceeded condit ion, the device sets 
// the LBA Mid register to 4Fh and the LBA High register to C2h. If the device has 
// detected a threshold exceeded condition, the device sets the LBA Mid register 
// to F4h and the LBA High register to 2Ch. 

EFI_STATUS 
CheckSmart (
  EFI_ATA_PASS_THRU_PROTOCOL      *AtaPassThru,
  UINT16                          Port,
  UINT16                          PortMp,
  EFI_HANDLE                      Handle,
  UINT16                          HostIndex,
  BOOLEAN                         *HasError
  )
{
  EFI_STATUS                        Status;
  EFI_ATA_COMMAND_BLOCK             Acb;
  EFI_ATA_PASS_THRU_COMMAND_PACKET  Packet;
  EFI_ATA_STATUS_BLOCK              *Asb;


  if (AtaPassThru == NULL || Handle == NULL || HasError == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *HasError = FALSE;

  Asb = (EFI_ATA_STATUS_BLOCK*)AllocateAlignedPages (
                                 EFI_SIZE_TO_PAGES (sizeof(EFI_ATA_STATUS_BLOCK)),
                                 AtaPassThru->Mode->IoAlign
                                 );
  if (Asb == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (Asb, sizeof (EFI_ATA_STATUS_BLOCK));
  ZeroMem (&Acb, sizeof (EFI_ATA_COMMAND_BLOCK));
  Acb.AtaCommand      = ATA_CMD_SMART;
  Acb.AtaFeatures     = ATA_SMART_RETURN_STATUS;
  Acb.AtaCylinderLow  = ATA_CONSTANT_4F;
  Acb.AtaCylinderHigh = ATA_CONSTANT_C2;

  ZeroMem(&Packet, sizeof(Packet));
  Packet.Protocol = EFI_ATA_PASS_THRU_PROTOCOL_ATA_NON_DATA;
  Packet.Length   = EFI_ATA_PASS_THRU_LENGTH_NO_DATA_TRANSFER;
  Packet.Asb      = Asb;
  Packet.Acb      = &Acb;
  Packet.Timeout  = EFI_TIMER_PERIOD_SECONDS(3);

  Status = AtaPassThru->PassThru(
                          AtaPassThru,
                          Port,
                          PortMp,
                          &Packet,
                          NULL
                          );

  DEBUG ((EFI_D_INFO, "%d(%X) SATA(%X,%X) %r %X %X\n", HostIndex, AtaPassThru, Port, PortMp, Status, Acb.AtaCylinderLow, Acb.AtaCylinderHigh));

  if(!EFI_ERROR(Status)){
    if (Acb.AtaCylinderLow == 0xf4 && Acb.AtaCylinderHigh == 0x2c) {
      if(HostIndex == 0x8000){
        TextScreenPrintString(L"SATA%d S.M.A.R.T ERROR\n", Port);
      } else {
        TextScreenPrintString(L"SATA%d-%d S.M.A.R.T ERROR\n", HostIndex, Port);
      }
      Status = EFI_DEVICE_ERROR;
      *HasError = TRUE;
    }
  }

  FreeAlignedPages(Asb, EFI_SIZE_TO_PAGES(sizeof(EFI_ATA_STATUS_BLOCK)));
  return Status;
}



BOOLEAN ShowHddSmartStatus()
{
  EFI_STATUS                      Status;
  UINTN                           HandleCount;
  EFI_HANDLE                      *HandleBuffer = NULL;
  UINTN                           Index;
  UINT16                          SataPort;
  UINT16                          SataPortMp; 
  EFI_ATA_PASS_THRU_PROTOCOL      *AtaPassThru;
  PLAT_HOST_INFO_PROTOCOL         *ptHostInfo;
  UINT16                          HostIndex;
  BOOLEAN                         ThisHasError;
  BOOLEAN                         HasError = FALSE;


  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &ptHostInfo);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiAtaPassThruProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }
  DEBUG((EFI_D_INFO, "AtaPassThru#:%d\n", HandleCount));

  for (Index = 0; Index < HandleCount; Index++) {

    HostIndex = ptHostInfo->GetSataHostIndex(HandleBuffer[Index]);
    if(HostIndex == 0xFFFF){
      continue;
    }

    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiAtaPassThruProtocolGuid,
                    &AtaPassThru
                    );

    SataPort = 0xFFFF;
    while (TRUE) {
      Status = AtaPassThru->GetNextPort(AtaPassThru, &SataPort);
      if (EFI_ERROR (Status)) {
        break;
      }
      SataPortMp = 0xFFFF;
      while (TRUE) {
        Status = AtaPassThru->GetNextDevice(AtaPassThru, SataPort, &SataPortMp);
        if (EFI_ERROR (Status)) {
          break;
        }

        CheckSmart(AtaPassThru, SataPort, SataPortMp, HandleBuffer[Index], HostIndex, &ThisHasError);
        HasError |= ThisHasError;
      }
    }
  }

ProcExit:
  if (HandleBuffer != NULL){
    FreePool(HandleBuffer);
  }
  return HasError;
}


VOID DisplayTextMessage (VOID)
{
  EFI_STATUS                Status;  
  EFI_SMBIOS_PROTOCOL       *Smbios;
  PLAT_HW_INFO              *PlatHwInfo; 
  UINT32                    SystemMiscCfg;
  BOOLEAN                   HasError = FALSE;
  CHAR16                    *Str;
  PLAT_HOST_INFO_PROTOCOL   *PlatHostInfo;
  UINTN                     EventIndex;
  EFI_INPUT_KEY             Key;
  UINT16                    BdsWaitTime;
  PLATFORM_COMM_INFO        *Info;  


  if(!PcdGetBool(PcdPostTextScreenEnable)){
    return;
  }

  gST->ConOut->SetCursorPosition (gST->ConOut, 0, 0);

  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID**)&Smbios
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  Status = gBS->LocateProtocol(&gPlatformHwInfoReadyProtocolGuid, NULL, (VOID**)&PlatHwInfo);
  if (EFI_ERROR (Status)) {
    return;
  }

  Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  ShowBiosInfo(Smbios);
  ShowBmcInfo(Info);
  ShowCpuInfo(PlatHwInfo);
  ShowMemoryInfo(PlatHwInfo);
  ShowOsDiskInfo();

  //
  //show HDD S.M.A.R.T status
  //
  SystemMiscCfg = PcdGet32(PcdSystemMiscConfig);
  if(SystemMiscCfg & SYS_MISC_CFG_HDD_SMART_CHECK){
    HasError = ShowHddSmartStatus();
  }

  if(HasError){
    Str = HiiGetString(mHiiHandle, STRING_TOKEN(STR_KEY_CONTINUE), NULL);
    TextScreenPrintString(L"\n%s\n", Str);
    FreePool(Str);

    Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &PlatHostInfo);
    if(!EFI_ERROR(Status)){
      PlatHostInfo->StopWdg();
    }
    
    while (TRUE) {
      Status = gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &EventIndex);
      if(EFI_ERROR(Status)){
        break;
      }
      Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
      if(Key.ScanCode == SCAN_NULL && Key.UnicodeChar == CHAR_CARRIAGE_RETURN){
        break;
      }
    }
  } else {
    BdsWaitTime = PcdGet16(PcdPlatformBootTimeOut);
    if(BdsWaitTime < 1){
      gBS->Stall(1000000);
    }
  }
}



VOID
CleanScreenTextEvent (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  UINTN    Row;
  CHAR16   Space[0x101];
  UINTN    Index;

  gBS->CloseEvent (Event);

  if(!PcdGetBool(PcdPostTextScreenEnable)){
    return;
  }

  for(Index = 0; Index < MaxWidth; Index++ ) {
    Space[Index] = L' ';
  }
  Space[Index] = CHAR_NULL;

  for (Row = 0; Row < EraseLine; Row++){
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, Row);
    gST->ConOut->OutputString(gST->ConOut, Space);
  }
  gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0);
}

EFI_STATUS
EFIAPI
TextScreenEntry (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS   Status;
  EFI_EVENT    Event;
  mHiiHandle = HiiAddPackages (
            &mTextScreenStringPackGuid,
            ImageHandle,
            TextScreenStrings,
            NULL
            );
  ASSERT(mHiiHandle != NULL);
  
  Status = gBS->InstallMultipleProtocolInterfaces(
              &ImageHandle,
              &gByoBdsWaitHookProtocolGuid, DisplayTextMessage,
              NULL
              );
  DEBUG((EFI_D_INFO,"Install BdsWaitHookProtocol %r\n",Status));

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             CleanScreenTextEvent,
             NULL,
             &Event
             );
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), Craete CleanScreenText :%r.\n", Status));

  return Status;
}


