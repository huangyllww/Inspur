/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/


#include <Uefi.h>
#include <PiDxe.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/FileInfo.h>
#include <Guid/GlobalVariable.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/HiiLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootManagerLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/ScsiIo.h>
#include <Protocol/AtaPassThru.h>
#include <Protocol/BlockIo.h>
#include <Library/ByoCommLib.h>
#include <Library/NetLib.h>
#include <Library/BaseLib.h>
#include <Protocol/ByoSmiFlashProtocol.h>
#include <Library/SetupUiLib.h>
#include <Protocol/FileExplorer.h>
#include <Library/BiosIdLib.h>
#include <Protocol/ByoBackupRecoveryProtocol.h>
#include <Protocol/LoadFile.h>
#include <Protocol/DisplayProtocol.h>

EFI_HII_HANDLE gHiiHandle;

typedef enum  {
  UPDATE_MODE_ALL,
  UPDATE_MODE_DEFAULT,
  UPDATE_MODE_NETWORK,
  UPDATE_MODE_NETWORK_ALL,
  UPDATE_MODE_MAX
} UPDATE_MODE;

#define  MaxIpv4Len   15
#define  MaxIpv6Len   39

EFI_GUID   HttpBootDxeGuid = { 0xecebcb00, 0xd9c8, 0x11e4, { 0xaf, 0x3d, 0x8c, 0xdc, 0xd4, 0x26, 0xc9, 0x73 } };

#if 0
EFI_STATUS 
ReadFileFromUsb (
  CHAR16  *Name,
  UINT8   **FileData,
  UINTN   *FileSize
  )
{
  EFI_STATUS                       Status;
  UINTN                            Index;
  UINTN                            HandleCount;
  EFI_HANDLE                       *Handles = NULL;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *FS;  
  EFI_FILE_PROTOCOL                *RootFile = NULL;
  EFI_DEVICE_PATH_PROTOCOL         *DevPath;
  BOOLEAN                          IsUsb;

  
  Status = gBS->LocateHandleBuffer (
                 ByProtocol,
                 &gEfiSimpleFileSystemProtocolGuid,
                 NULL,
                 &HandleCount,
                 &Handles
                 );
  if(EFI_ERROR(Status) || HandleCount == 0){
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }  
  
  for (Index = 0; Index < HandleCount; Index++) {
    IsUsb = FALSE;
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID**)&DevPath
                    );    
    if(!EFI_ERROR(Status)){
      while (!IsDevicePathEnd(DevPath)) {
        if ((DevicePathType(DevPath) == MESSAGING_DEVICE_PATH) &&
            (DevicePathSubType(DevPath) == MSG_USB_DP)) {
          IsUsb = TRUE;    
          break;
        }
        DevPath = NextDevicePathNode(DevPath);
      }    
    }

    DEBUG((EFI_D_INFO, "FS[%d]: IsUsb:%d\n", Index, IsUsb));
    if(!IsUsb){
      continue;
    }
    
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiSimpleFileSystemProtocolGuid,
                    (VOID**)&FS
                    );
    ASSERT(!EFI_ERROR (Status));

    if(RootFile != NULL){
      RootFile->Close(RootFile);
      RootFile = NULL;
    }  
  	Status = FS->OpenVolume(FS, &RootFile);
  	if(EFI_ERROR(Status)){
      DEBUG((EFI_D_ERROR, "OpenRootFile Error:%r\n", Status));
      continue;
    }  

    Status = RootFileReadFile(gBS, RootFile, Name, FileData, FileSize);
  	if(!EFI_ERROR(Status)){
      break;
    } 
  }

  if(Index >= HandleCount){
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  } 

ProcExit:
  if(Handles != NULL){
    FreePool(Handles);
  }  
  if(RootFile != NULL){
    RootFile->Close(RootFile);
  } 
  return Status;
}
#endif

/**
  This function checks the HTTP(S) URI scheme.

  @param[in]    Uri              The pointer to the URI string.

  @retval EFI_SUCCESS            The URI scheme is valid.
  @retval EFI_INVALID_PARAMETER  The URI scheme is not HTTP or HTTPS.
  @retval EFI_ACCESS_DENIED      HTTP is disabled and the URI is HTTP.

**/
EFI_STATUS
HttpCheckUriScheme (
  IN      CHAR8    *Uri,
  OUT     BOOLEAN  *IsIpv4
  )
{
  UINTN              Index;
  CHAR8              *Ip = NULL;
  CHAR16             *Ipv6 = NULL;
  EFI_IPv4_ADDRESS   Ip4Address;
  EFI_IPv6_ADDRESS   Ip6Address;
  UINTN              IpLen;
  UINTN              PrefixLen;
  EFI_STATUS         Status;

  Status = EFI_SUCCESS;
  *IsIpv4 = TRUE;
  //
  // Convert the scheme to all lower case.
  //
  for (Index = 0; Index < AsciiStrLen (Uri); Index++) {
    if (Uri[Index] == ':') {
      break;
    }

    if ((Uri[Index] >= 'A') && (Uri[Index] <= 'Z')) {
      Uri[Index] -= (CHAR8)('A' - 'a');
    }
  }

  //
  // Return EFI_INVALID_PARAMETER if the URI is not HTTP or HTTPS.
  //
  if (AsciiStrnCmp (Uri, "http://", 7) == 0) {
    Index = 7;
    PrefixLen = 7;
  } else if (AsciiStrnCmp (Uri, "https://", 8) == 0) {
    Index = 8;
    PrefixLen = 8;
  } else {
    DEBUG ((DEBUG_ERROR, " %a: Invalid Uri.\n",__FUNCTION__));
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }

  if(Uri[PrefixLen] == '[') {//"[ipv6]"
    *IsIpv4 = FALSE;
    PrefixLen++;
  }

  IpLen = 0;
  if (*IsIpv4) {
    while (Uri[Index] != '/')// "ipv4/"
    {
      if (Uri[Index] == '\0' || IpLen > MaxIpv4Len) {//Invalid ip,such as 192.168 or 192.168.121.1154
        Status = EFI_INVALID_PARAMETER;
        goto ProcExit;
      }
      Index++;
      IpLen++;
    }
  } else {
    while (Uri[Index + 1] != L']')
    {
      if (Uri[Index] == '\0' || IpLen > MaxIpv6Len) {//Invalid ip,such as X:X or X:X:X:X:X:X:X:X:X
        Status = EFI_INVALID_PARAMETER;
        goto ProcExit;
      }
      Index++;
      IpLen++;
    }
  }

  Ip = AllocatePool((IpLen + 1) * sizeof(CHAR8));
  CopyMem(Ip,&Uri[PrefixLen],IpLen);
  Ip[IpLen] = '\0';

  if (*IsIpv4) {
    Status = NetLibAsciiStrToIp4(Ip,&Ip4Address);
      if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_ERROR, " %a: Invalid IPv4 string, Status:%r\n",__FUNCTION__,Status));
        Status = EFI_INVALID_PARAMETER;
        goto ProcExit;
      }

    Status = Ipv4AddrCheck(&Ip4Address,TYPE_IP4_ADDR);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, " %a: Invalid Ipv4 address,Status:%r\n",__FUNCTION__,Status));
      Status = EFI_INVALID_PARAMETER;
      goto ProcExit;
    }
  } else {
    Status = NetLibAsciiStrToIp6(Ip,&Ip6Address);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, " %a: Invalid IPv6 string, Status:%r\n",__FUNCTION__,Status));
      Status = EFI_INVALID_PARAMETER;
      goto ProcExit;
    }

    Ipv6 = AllocatePool((IpLen + 1) * sizeof(CHAR16));
    AsciiStrToUnicodeStrS(Ip,Ipv6,IpLen + 1);
    Status = Ipv6AddrCheck(Ipv6);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, " %a: Invalid Ipv6 address,Status:%r\n",__FUNCTION__,Status));
      Status = EFI_INVALID_PARAMETER;
      goto ProcExit;
    }
  }
  //
  // HTTP is disabled, return EFI_ACCESS_DENIED if the URI is HTTP.
  //
  if (!PcdGetBool (PcdAllowHttpConnections) && (AsciiStrnCmp (Uri, "http://", 7) == 0)) {
    DEBUG ((DEBUG_ERROR, " %a: HTTP is disabled.\n",__FUNCTION__));
    Status = EFI_ACCESS_DENIED;
    goto ProcExit;
  }

ProcExit:
  if (Ip != NULL) {
    FreePool(Ip);
  }

  if (Ipv6 != NULL) {
    FreePool(Ipv6);
  }

  return Status;
}

CHAR8*
GetUserInputUri (
  BOOLEAN     *IsIpv4
)
{
  CHAR16                                  *Uri = NULL;
  UINTN                                   UriLen;
  CHAR8                                   *AsciiUri = NULL;
  EDKII_FORM_DISPLAY_ENGINE_PROTOCOL      *DisplayProtocol;
  EFI_STATUS                              Status;
  CHAR16                                  *NetworkUpdateTitle = NULL;
  CHAR16                                  *NetworkUpdateTitle1 = NULL;
  CHAR16                                  *NetworkUpdateTitle2 = NULL;
  CHAR16                                  *NetworkUpdateTitle3 = NULL;

  Status = gBS->LocateProtocol (
                  &gEdkiiFormDisplayEngineProtocolGuid,
                  NULL,
                  (VOID**) &DisplayProtocol
                 );
  if(EFI_ERROR(Status)) {
    DEBUG((DEBUG_INFO,"Locate gEdkiiFormDisplayEngineProtocolGuid error:%r\n",Status));
    return NULL;
  }
  Status = EFI_INVALID_PARAMETER;
  NetworkUpdateTitle1 = HiiGetString(gHiiHandle, STRING_TOKEN(STR_NETWORK_UPDATE_TITLE1), NULL);
  NetworkUpdateTitle2 = HiiGetString(gHiiHandle, STRING_TOKEN(STR_NETWORK_UPDATE_TITLE2), NULL);
  NetworkUpdateTitle3 = HiiGetString(gHiiHandle, STRING_TOKEN(STR_NETWORK_UPDATE_TITLE3), NULL);
  NetworkUpdateTitle = NetworkUpdateTitle1;
  while(Status == EFI_INVALID_PARAMETER) {
    Status = DisplayProtocol->ReadString(NetworkUpdateTitle,255,&Uri);
    if (EFI_ERROR(Status)) {
      return NULL;
    }

    if (Uri[0] == L'\0') {
      NetworkUpdateTitle = NetworkUpdateTitle3;
      Status = EFI_INVALID_PARAMETER;
      continue;
    }

    UriLen = StrLen(Uri) + 1;
    AsciiUri = AllocateZeroPool(UriLen);
    ASSERT(AsciiUri != NULL);
    UnicodeStrToAsciiStrS (Uri, AsciiUri, UriLen);
    FreePool(Uri);
    Status = HttpCheckUriScheme(AsciiUri,IsIpv4);
    if (EFI_ERROR(Status)) {
      NetworkUpdateTitle = NetworkUpdateTitle2;
    }
  }

  if (NetworkUpdateTitle1 != NULL) {
    FreePool(NetworkUpdateTitle1);
  }

  if (NetworkUpdateTitle2 != NULL) {
    FreePool(NetworkUpdateTitle2);
  }

  if (NetworkUpdateTitle3 != NULL) {
    FreePool(NetworkUpdateTitle3);
  }

  return AsciiUri;
}

EFI_DEVICE_PATH_PROTOCOL*
CreateUriDevicePath(
  CHAR8      *UriStr
)
{
  EFI_DEVICE_PATH_PROTOCOL    *UriDevicePath;
  URI_DEVICE_PATH             *TempUriDevicePath;
  UINTN                       UriLen;

  if (UriStr == NULL) {
    DEBUG((DEBUG_INFO,"CreateUriDevicePath:invalid parameter\n"));
    return NULL;
  }

  UriLen = AsciiStrLen(UriStr);
  TempUriDevicePath  = (URI_DEVICE_PATH *) CreateDeviceNode (
                                MESSAGING_DEVICE_PATH,
                                MSG_URI_DP,
                                (UINT16) (sizeof (URI_DEVICE_PATH) + UriLen)
                                );
  while (UriLen--) {
    TempUriDevicePath->Uri[UriLen] = UriStr[UriLen];
  }

  UriDevicePath = AppendDevicePathNode(NULL,(EFI_DEVICE_PATH_PROTOCOL*)TempUriDevicePath);
  FreePool(TempUriDevicePath);
  return UriDevicePath;
}

EFI_STATUS
HttpLoadFile(
  INTN                             IpType,
  EFI_DEVICE_PATH_PROTOCOL         *UriDevicePath,
  UINT8                            **Buffer,
  UINTN                            *BufferSize
  )
{
  EFI_STATUS                       Status;
  EFI_LOAD_FILE_PROTOCOL           *LoadFile;
  EFI_HANDLE                       *Handles;
  UINTN                            HandleCount;
  UINTN                            Index;
  EFI_DEVICE_PATH_PROTOCOL         *DevicePath;
  UINT16                           BootCurrent;
  UINTN                            VariableSize;
  BOOLEAN                          IsConnectHttp = FALSE;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiLoadFileProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  if (EFI_ERROR(Status) || HandleCount == 0) {
    return EFI_HTTP_ERROR;
  }
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol(
                Handles[Index],
                &gEfiDevicePathProtocolGuid,
                (VOID **)&DevicePath
                );
    if (!EFI_ERROR(Status)) {
      while (!IsDevicePathEnd (DevicePath)) {
        if (DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH && DevicePathSubType (DevicePath) == IpType) {
          while (!IsDevicePathEnd (DevicePath)) {
            DevicePath = NextDevicePathNode (DevicePath);
            if (DevicePathSubType (DevicePath) == MSG_URI_DP) {
              IsConnectHttp = TRUE;
              Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiLoadFileProtocolGuid,
                    (VOID **)&LoadFile
                    );
              *BufferSize = 0;
              *Buffer = NULL;
              gST->ConOut->ClearScreen(gST->ConOut);
              VariableSize = sizeof (BootCurrent);
              Status = gRT->GetVariable (
                        EFI_BOOT_CURRENT_VARIABLE_NAME,
                        &gEfiGlobalVariableGuid,
                        NULL,
                        &VariableSize,
                        &BootCurrent
                        );
              if (!EFI_ERROR (Status) && (VariableSize == sizeof (BootCurrent))) {
                Status = gRT->SetVariable(
                      EFI_BOOT_CURRENT_VARIABLE_NAME,
                      &gEfiGlobalVariableGuid,
                      0,
                      0,
                      NULL
                      );
              }
              Status = LoadFile->LoadFile(LoadFile,
                                          UriDevicePath,
                                          TRUE,
                                          BufferSize,
                                          (VOID*)*Buffer
                                        );
              if (Status == EFI_BUFFER_TOO_SMALL) {
                *Buffer = AllocatePool(*BufferSize);
                Status = LoadFile->LoadFile(LoadFile,
                                UriDevicePath,
                                TRUE,
                                BufferSize,
                                (VOID*)*Buffer
                                );
                gRT->SetVariable(
                      EFI_BOOT_CURRENT_VARIABLE_NAME,
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      VariableSize,
                      &BootCurrent
                      );
                if (!EFI_ERROR(Status)) {
                  if (Handles != NULL) {
                    FreePool(Handles);
                  }
                  return Status;
                }
              }
              break;
            }
          }
          break;
        }
        DevicePath = NextDevicePathNode (DevicePath);
      }
    }
  }
  if (Handles != NULL) {
    FreePool(Handles);
  }
  if (IsConnectHttp) {
    return EFI_LOAD_ERROR;
  } else {
    return EFI_HTTP_ERROR;
  }
}

VOID
ConnectHttp(
  INTN      IpType
  )
{
  EFI_BOOT_MANAGER_LOAD_OPTION     *BootOptions;
  UINTN                            BootOptionCount;
  UINTN                            Index;
  EFI_DEVICE_PATH_PROTOCOL         *DevicePath;
  EFI_STATUS                       Status;
  BOOLEAN                          IsFirst = TRUE;
  EFI_DEVICE_PATH_PROTOCOL         *NetworkStackDevicePath = NULL;
  EFI_HANDLE                       Handle;

  BootOptions = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);
  for (Index = 0; Index < BootOptionCount; Index++) {
    DevicePath = BootOptions[Index].FilePath;
    while (!IsDevicePathEnd (DevicePath)) {
      if (DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH && DevicePathSubType (DevicePath) == IpType) {
        while(!IsDevicePathEnd (DevicePath)) {
          DevicePath = NextDevicePathNode (DevicePath);
          if (DevicePathSubType (DevicePath) == MSG_URI_DP) {
            DevicePath = BootOptions[Index].FilePath;
            Status = gBS->LocateDevicePath (&gEfiLoadFileProtocolGuid, &DevicePath, &Handle);
            if (EFI_ERROR(Status)) {
              if (IsFirst) {
                Status = GetFileDevicePathFromAnyFv (&HttpBootDxeGuid,EFI_SECTION_PE32,0,&NetworkStackDevicePath);
                if (EFI_ERROR(Status)) {
                  Status = gRT->SetVariable (
                                  EFI_BOOT_CURRENT_VARIABLE_NAME,
                                  &gEfiGlobalVariableGuid,
                                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                                  sizeof (UINT16),
                                  &BootOptions[Index].OptionNumber
                                  );
                  if (EFI_ERROR(Status)) {
                    DEBUG((DEBUG_INFO,"%a:Set EFI_BOOT_CURRENT_VARIABLE_NAME error!\n",__FUNCTION__));
                  }
                  InvokeHookProtocol (gBS, &gEfiReadyToBootProtocolGuid);
                  IsFirst = FALSE;
                }
              }
              Status = EfiBootManagerConnectDevicePath(BootOptions[Index].FilePath,NULL);
              gBS->Stall (5000000);
            }
            break;
          }
        }
        break;
      }
      DevicePath = NextDevicePathNode (DevicePath);
    }
  }
  EfiBootManagerFreeLoadOptions (BootOptions, BootOptionCount);
  if (NetworkStackDevicePath != NULL) {
    FreePool(NetworkStackDevicePath);
  }
}

EFI_STATUS
GetBiosImageFromNetwork (
  UINT8     **FileData,
  UINTN     *FileSize,
  BOOLEAN   *IsNullUri
  )
{
  EFI_STATUS                       Status;
  EFI_DEVICE_PATH_PROTOCOL         *UriDevicePath;
  BOOLEAN                          IsIpv4;
  BOOLEAN                          IsInfiniteRetry;
  INTN                             IpType;
  UINT8                            HttpRetry;
  UINT16                           HttpRetrylimites;
  BOOLEAN                          IsFirst = TRUE;

  *IsNullUri = FALSE;
  UriDevicePath = CreateUriDevicePath(GetUserInputUri(&IsIpv4));
  if (UriDevicePath == NULL) {
    *IsNullUri = TRUE;
    return EFI_NOT_FOUND;
  }

  if (IsIpv4) {
    IpType = MSG_IPv4_DP;
  } else {
    IpType = MSG_IPv6_DP;
  }

  ShowDevicePathDxe(gBS,UriDevicePath);

  HttpRetry = PcdGet8(PcdRetryPxeBoot);
  HttpRetrylimites = PcdGet16(PcdPxeRetrylimites);

  if (HttpRetrylimites == PXE_RETRY_FOREVER_VALUE) {
    IsInfiniteRetry = TRUE;
  } else {
    IsInfiniteRetry = FALSE;
  }

  do
  {
    Status = HttpLoadFile(IpType,UriDevicePath,FileData,FileSize);
    if (!EFI_ERROR (Status)) {
      break;
    }
    if (IsInfiniteRetry) {
      HttpRetrylimites = PXE_RETRY_FOREVER_VALUE;
    }
    if (IsFirst) {
      if (Status == EFI_HTTP_ERROR) {
        ConnectHttp(IpType);
        Status = HttpLoadFile(IpType,UriDevicePath,FileData,FileSize);
        if (!EFI_ERROR (Status)) {
          break;
        }
      }
      IsFirst = FALSE;
    }
  } while (HttpRetry == 1 && HttpRetrylimites--);
  FreePool(UriDevicePath);
  return Status;
}

VOID ShowFileNotFound()
{
  CHAR16  *Str;

  Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_FILE_NOT_FOUND), NULL);
  UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, Str, NULL);
  FreePool(Str);
}


VOID ShowFileNotMatch(
  IN UINT8       ErrorType
  )
{
  CHAR16  *Str;

  if (ErrorType == 0) {
    Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_FILE_SIGN_MATCH), NULL);
  }else{
    Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_FILE_NOT_MATCH), NULL);
  }
  UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, Str, NULL);
  FreePool(Str);
}


EFI_STATUS 
LoadFileByDp (
  EFI_DEVICE_PATH_PROTOCOL *Dp,
  UINT8                    **FileData,
  UINTN                    *FileSize  
  )
{
  EFI_STATUS                       Status;
  EFI_HANDLE                       Handle;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *FS; 
  EFI_FILE_PROTOCOL                *RootFile = NULL;
  CHAR16                           *FileName;


  ShowDevicePathDxe(gBS, Dp);

  Status = gBS->LocateDevicePath(&gEfiSimpleFileSystemProtocolGuid, &Dp, &Handle);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }  
  
  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiSimpleFileSystemProtocolGuid,
                  (VOID**)&FS
                  );
  ASSERT(!EFI_ERROR(Status));
  
	Status = FS->OpenVolume(FS, &RootFile);
	if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "OpenRootFile Error:%r\n", Status));
    goto ProcExit;
  }  

  if(DevicePathType(Dp) == MEDIA_DEVICE_PATH && DevicePathSubType(Dp) == MEDIA_FILEPATH_DP){
    FileName = ((FILEPATH_DEVICE_PATH*)Dp)->PathName;
  } else {
    Status = EFI_UNSUPPORTED;
    goto ProcExit;
  }

  Status = RootFileReadFile(gBS, RootFile, FileName, (void**)FileData, FileSize);

ProcExit:
  if(RootFile != NULL){
    RootFile->Close(RootFile);
  }   
  return Status;
}

CHAR16 gCopyrightStr1[] = L"**************************************************************";
CHAR16 gCopyrightStr2[] = L"*                   Byosoft Flash Update                     *";
CHAR16 gCopyrightStr3[] = L"*         Copyright(C) 2006-2021, Byosoft Co.,Ltd.           *";
CHAR16 gCopyrightStr4[] = L"*                   All rights reserved                      *";
CHAR16 gCopyrightStr5[] = L"**************************************************************\n\r\n\r";

EFI_STATUS
ShowCopyRightsAndWarning (
  VOID
)
{
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut = gST->ConOut;
  UINTN                             Columns;
  UINTN                             Rows;
  UINTN                             PrintX, PrintY;
  UINTN                             Len;
  BIOS_ID_INFO                      BiosIdInfo;
  EFI_STATUS                        Status;
  CHAR16                            *Year;

  Status = GetBiosIdInfoFromHob(&BiosIdInfo);
  if (!EFI_ERROR(Status) && (Year = StrStr(gCopyrightStr3, L"2021"))!=NULL) {
    Year[0] = BiosIdInfo.BiosDate[6];
    Year[1] = BiosIdInfo.BiosDate[7];
    Year[2] = BiosIdInfo.BiosDate[8];
    Year[3] = BiosIdInfo.BiosDate[9];
    DEBUG((EFI_D_INFO, "Year:%c%c%c%c\n", Year[0], Year[1], Year[2], Year[3]));
  }

  ConOut->SetAttribute(ConOut, EFI_BACKGROUND_BLACK | EFI_WHITE | EFI_BRIGHT);
  ConOut->ClearScreen (ConOut);
  ConOut->EnableCursor(ConOut, FALSE);  
  ConOut->SetAttribute(ConOut, EFI_BACKGROUND_BLACK | EFI_YELLOW);

  ConOut->QueryMode(ConOut, ConOut->Mode->Mode, &Columns, &Rows);
  Len = StrLen(gCopyrightStr1);
  PrintX = Len >= Columns ? 0 : ((Columns - Len)/2);
  PrintY = 0;

  ConOut->SetCursorPosition(ConOut, PrintX, PrintY++);
  ConOut->OutputString(ConOut, gCopyrightStr1);
  ConOut->SetCursorPosition(ConOut, PrintX, PrintY++);
  ConOut->OutputString(ConOut, gCopyrightStr2);
  ConOut->SetCursorPosition(ConOut, PrintX, PrintY++);
  ConOut->OutputString(ConOut, gCopyrightStr3);
  ConOut->SetCursorPosition(ConOut, PrintX, PrintY++);
  ConOut->OutputString(ConOut, gCopyrightStr4);
  ConOut->SetCursorPosition(ConOut, PrintX, PrintY++);
  ConOut->OutputString(ConOut, gCopyrightStr5);

  gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_WHITE | EFI_BRIGHT);
  
  return EFI_SUCCESS;
}



VOID 
ShowSetupSignVerifyProgress (
  IN UINTN     Persent
  )
{
         CHAR16   *Str;
         CHAR16   Buffer[64];
  STATIC UINTN    ProgressValue;

  if(Persent == 0xFFFF){
    ProgressValue = 0xFFFF;
    return;
  }
  
  if(ProgressValue == Persent || Persent > 100){
    return;
  }

  Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_SIGN_VERIFY_PRESENT), NULL);
  UnicodeSPrint(Buffer, sizeof(Buffer), Str, Persent);
  FreePool(Str);

  CreatePopUp (
    EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
    NULL,
    Buffer,
    NULL
    );

  ProgressValue = Persent;
}



/**
  Show Bios Update Progress

  @param[in] Type         Draw Type
                          BIOS_UPDATE_DRAW_TYPE_START   The first drawing
                          BIOS_UPDATE_DRAW_TYPE_RUN     The updating drawing
                          BIOS_UPDATE_DRAW_TYPE_END     The end drawing
  @param[in] Rate         0-10000 Two decimal places
  @param[in] Info         Bios Update Info String
  @param[in] Context      Bios Update Progress Bar Draw Function Context

  @retval EFI_SUCCESS     Draw Success
  @retval Other           Draw Error

**/
static
EFI_STATUS
BiosUpdataShowProgressCallback (
  IN UINTN     Type,
  IN UINT64    Rate,
  IN CHAR16    *Info,
  IN VOID      *Context
  )
{
  UINT32                               Width;
  UINT32                               Height;
  EFI_STATUS                           Status;
  CHAR16                               RateStr[100];
  static EFI_HANDLE                    Windows;
  static VOID                          *ProgressBar = NULL;
  static EFI_GRAPHICS_OUTPUT_PROTOCOL  *Gop = NULL;

  switch (Type) {
    case BIOS_UPDATA_DRAW_TYPE_ENTRY:
      if (ProgressBar != NULL) {
        DeleteDynamicallyUpdatePop(ProgressBar);
        ProgressBar = NULL;
      }
      if (Windows != NULL) {
        UnRegisterWindows(Windows);
        Windows = NULL;
      }
      Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **) &Gop);
      ASSERT(!EFI_ERROR(Status));
      Width = Gop->Mode->Info->HorizontalResolution / 3;
      Height = Gop->Mode->Info->VerticalResolution / 5;
      Windows = RegisterWindows ((Gop->Mode->Info->HorizontalResolution - Width) / 2,
                                    (Gop->Mode->Info->VerticalResolution - Height) / 2,
                                    Width,
                                    Height,
                                    TRUE
                                    );
      ASSERT(Windows != NULL);
      ProgressBar = AddDynamicallyUpdatePop (Windows, Info);
      ASSERT(ProgressBar != NULL);

      RefreshDynamicallyUpdatePop(ProgressBar, TRUE, NULL);
      break;

    case BIOS_UPDATA_DRAW_TYPE_REFRESH:
      if (ProgressBar == NULL) {
        break;
      }
      if (Info == NULL) {
        break;
      }
      UnicodeSPrint(RateStr, sizeof(RateStr), L"%s %d %%", Info, Rate/100);
      RefreshDynamicallyUpdatePop(ProgressBar, FALSE, RateStr);
      break;

    case BIOS_UPDATA_DRAW_TYPE_ERROR:
    case BIOS_UPDATA_DRAW_TYPE_EXIT:
      if (ProgressBar != NULL) {
        DeleteDynamicallyUpdatePop(ProgressBar);
        ProgressBar = NULL;
      }
      if (Windows != NULL) {
        UnRegisterWindows(Windows);
        Windows = NULL;
      }
      break;

    case BIOS_UPDATA_DRAW_TYPE_INFO:
      if (ProgressBar == NULL) {
        break;
      }
      if (Info == NULL) {
        break;
      }
      if (Info[0] == L'\n' && Info[1] == 0) {
        break;
      }
      RefreshDynamicallyUpdatePop(ProgressBar, FALSE, Info);
      break;

    default:
      if (Info != NULL) {
        DEBUG((DEBUG_ERROR, "%a(%d) %s\n", __FILE__, __LINE__, Info));
      }
      break;
  }

  return EFI_SUCCESS;
}


BOOLEAN
IsEnableNetwork(
  VOID
)
{
  EFI_STATUS                       Status;
  EFI_HANDLE                       *Handles;
  UINTN                            HandleCount;
  UINTN                            Index;
  EFI_DEVICE_PATH_PROTOCOL         *DevicePath = NULL;
  EFI_BOOT_MANAGER_LOAD_OPTION     *BootOptions;
  UINTN                            BootOptionCount;

  Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiLoadFileProtocolGuid,
                    NULL,
                    &HandleCount,
                    &Handles
                    );
  if (!EFI_ERROR(Status) && HandleCount != 0) {
    for (Index = 0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol(
                  Handles[Index],
                  &gEfiDevicePathProtocolGuid,
                  (VOID **)&DevicePath
                  );
      if (!EFI_ERROR(Status)) {
        while (!IsDevicePathEnd (DevicePath)){
          if (DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH && DevicePathSubType (DevicePath) == MSG_URI_DP) {
            if (Handles != NULL) {
              FreePool(Handles);
            }
            return TRUE;
          }
          DevicePath = NextDevicePathNode (DevicePath);
        }
      }
    }
  }
  BootOptions = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);
  for (Index = 0; Index < BootOptionCount; Index++) {
    DevicePath = BootOptions[Index].FilePath;
    while (!IsDevicePathEnd (DevicePath)) {
      if (DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH && DevicePathSubType (DevicePath) == MSG_URI_DP) {
        if (Handles != NULL) {
          FreePool(Handles);
        }
        EfiBootManagerFreeLoadOptions (BootOptions, BootOptionCount);
        return TRUE;
      }
      DevicePath = NextDevicePathNode (DevicePath);
    }
  }
  if (Handles != NULL) {
    FreePool(Handles);
  }
  EfiBootManagerFreeLoadOptions (BootOptions, BootOptionCount);
  return FALSE;
}

VOID BiosUpdate(
  UPDATE_MODE     UpdateMode
)
{
  EFI_STATUS                  Status;
  UINT8                       *FileData = NULL;
  UINTN                       FileSize;
  BYO_SMIFLASH_PROTOCOL       *ByoSmiFlash;
  UINTN                       Index;
  CHAR16                      *Str; 
  CHAR16                      *Str2;
  CHAR16                      *Str3;
  CHAR16                      *StrUpdateError;
  SELECTION_TYPE              Choice;
  EFI_FILE_EXPLORER_PROTOCOL  *FileExplorer;
  EFI_DEVICE_PATH_PROTOCOL    *FileDp = NULL;
  UINTN                       Dummy;                 // wz200707 +
  UINTN                       *HotkeyDis = 0;        // wz200707 +
  UINT8                       ErrorType;
  UINT32                      Width;
  UINT32                      Height;
  EFI_HANDLE                  Windows;
  VOID                        *ProgressBar = NULL;
  CHAR16                      TimeStr[100];
  EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop = NULL;
  CHAR16                       *NetworkInfo = NULL;
  BOOLEAN                      IsNullUri;

  if (UpdateMode > UPDATE_MODE_MAX || UpdateMode < 0) {
    ASSERT(FALSE);
  }
// wz200707 + >>  
  Status = gBS->LocateProtocol(&gByoSetupHotKeyDisableProtocolGuid, NULL, (VOID**)&HotkeyDis);
  if(EFI_ERROR(Status)){
    HotkeyDis = &Dummy;
  }

  *HotkeyDis = BIT0;
// wz200707 + <<
  if(UpdateMode == UPDATE_MODE_NETWORK || UpdateMode == UPDATE_MODE_NETWORK_ALL) {
    if(!IsEnableNetwork()){
      NetworkInfo = HiiGetString(gHiiHandle, STRING_TOKEN(UPDATE_BIOS_NETWORK_INFO), NULL);
      UiConfirmDialogEx (DIALOG_ATTRIBUTE_RECOVER, DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, NetworkInfo, NULL);
      goto ProcExit;
    }
    Status = GetBiosImageFromNetwork(&FileData,&FileSize,&IsNullUri);
    if(EFI_ERROR(Status)){
      if (!IsNullUri) {
        Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_FILE_LOAD_FAILED), NULL);
        UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, Str, NULL);
        FreePool(Str);
      }
      goto ProcExit;
    }
    } else {
      Status = gBS->LocateProtocol(&gEfiFileExplorerProtocolGuid, NULL, (void**)&FileExplorer);
      if(EFI_ERROR(Status)){
        goto ProcExit;
    }
    
    Status = FileExplorer->ChooseFile(NULL, L".bin", NULL, &FileDp);
    if(EFI_ERROR(Status)){
      goto ProcExit;
    }

    Status = LoadFileByDp(FileDp, &FileData, &FileSize);
    if(EFI_ERROR(Status)){
      goto ProcExit;
    }
  }

  Status = gBS->LocateProtocol (
                  &gByoSmiFlashProtocolGuid,
                  NULL,
                  (VOID**)&ByoSmiFlash
                  );
  ASSERT(!EFI_ERROR(Status));

  if (FixedPcdGetBool(PcdGuiEnable)) {
    if (ByoSmiFlash->SetShowProgressCallBack != NULL) {
      ByoSmiFlash->SetShowProgressCallBack(ByoSmiFlash, BiosUpdataShowProgressCallback, NULL);
    }
  } else {
    ShowSetupSignVerifyProgress(0xFFFF);
    if(ByoSmiFlash->SetVerifyProgressCallBack != NULL){
      ByoSmiFlash->SetVerifyProgressCallBack(ByoSmiFlash, ShowSetupSignVerifyProgress);
    }
  }

  Status = ByoSmiFlash->BiosCheck(ByoSmiFlash, FileData, FileSize, NULL);
  if(EFI_ERROR(Status)){

    if (Status == EFI_SECURITY_VIOLATION) {
      ErrorType = 0;
    }else {
      ErrorType = 1;
    }
    ShowFileNotMatch(ErrorType);
    goto ProcExit;    
  }

  if (!PcdGetBool(PcdUpdateNvVariableEnable)) {
    Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_UPDATE_BIOS_WARN1_1), NULL);
    Str2 = HiiGetString(gHiiHandle, STRING_TOKEN(STR_UPDATE_BIOS_WARN2_1), NULL);
  } else {
    Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_UPDATE_BIOS_WARN1_2), NULL);
    Str2 = HiiGetString(gHiiHandle, STRING_TOKEN(STR_UPDATE_BIOS_WARN2_2), NULL);
  }
  Str3 = HiiGetString(gHiiHandle, STRING_TOKEN(STR_UPDATE_BIOS_WARN3), NULL);
  Choice = UiConfirmDialog(DIALOG_YESNO, NULL, NULL, TEXT_ALIGIN_CENTER, Str, Str2, Str3, NULL);
  FreePool(Str);
  FreePool(Str2);
  FreePool(Str3);
  if (Choice != SELECTION_YES) {
    goto ProcExit;
  }

  if (!FixedPcdGetBool(PcdGuiEnable)) {
    ShowCopyRightsAndWarning ();
  }

  if (UpdateMode == UPDATE_MODE_ALL || UpdateMode == UPDATE_MODE_NETWORK_ALL){
    Status = ByoSmiFlash->UpdateAll(ByoSmiFlash, FileData, FileSize);
  } else {
    Status = ByoSmiFlash->DefaultUpdate(ByoSmiFlash, FileData, FileSize);
  }
  
  if(EFI_ERROR(Status)) {
    StrUpdateError = HiiGetString(gHiiHandle, STRING_TOKEN(STR_UPDATE_ERROR), NULL);
    SetOemDialogColor(DLG_OEM_COLOR_RED);
    UiConfirmDialog(DIALOG_NO_KEY, NULL, NULL, TEXT_ALIGIN_CENTER, StrUpdateError, NULL);
    FreePool (StrUpdateError);
    CpuDeadLoop();
  } else {
    if (FixedPcdGetBool(PcdGuiEnable)) {
      Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **) &Gop);
      ASSERT(!EFI_ERROR(Status));
      Width = Gop->Mode->Info->HorizontalResolution / 3;
      Height = Gop->Mode->Info->VerticalResolution / 5;
      Windows = RegisterWindows ((Gop->Mode->Info->HorizontalResolution - Width) / 2,
                                    (Gop->Mode->Info->VerticalResolution - Height) / 2,
                                    Width,
                                    Height,
                                    TRUE
                                    );
      ASSERT(Windows != NULL);
      ProgressBar = AddDynamicallyUpdatePop (Windows, NULL);
      ASSERT(ProgressBar != NULL);
      Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_UPDATA_SUCCESS_TIMEOUT), NULL);
      UnicodeSPrint(TimeStr, sizeof(TimeStr), Str, 5);
      RefreshDynamicallyUpdatePop(ProgressBar, TRUE, TimeStr);
      gBS->Stall (1000000);

      for (Index = 5; Index > 0; Index--) {
        UnicodeSPrint(TimeStr, sizeof(TimeStr), Str, Index);
        RefreshDynamicallyUpdatePop(ProgressBar, FALSE, TimeStr);
        gBS->Stall (1000000);
      }

      FreePool(Str);
      DeleteDynamicallyUpdatePop(ProgressBar);
      UnRegisterWindows(Windows);
    } else {
      gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_YELLOW);
      Print(L"Flash is updated successfully!\n\n");
      for (Index = 5; Index > 0; Index--) {
        Print (L"\rSystem will reset in %d second(s)", Index);
        gBS->Stall (1000000);
      }
    }
    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);    
  }

ProcExit:
  *HotkeyDis = 0;                                             // wz200707 +
  if(FileDp != NULL){
    FreePool(FileDp);
  }
  if(FileData != NULL){
    FreePool(FileData);
  }
  if (NetworkInfo != NULL){
    FreePool(NetworkInfo);
  }
  DEBUG((EFI_D_INFO, "%a %r\n", __FUNCTION__, Status));
  return;
}

VOID BiosUpdateDefault()
{
  UPDATE_MODE   UpdateMode = UPDATE_MODE_DEFAULT;
  BiosUpdate(UpdateMode);
}

VOID BiosUpdateAll()
{
  UPDATE_MODE   UpdateMode = UPDATE_MODE_ALL;
  BiosUpdate(UpdateMode);
}

VOID BiosUpdateNetwork()
{
  UPDATE_MODE   UpdateMode = UPDATE_MODE_NETWORK;
  BiosUpdate(UpdateMode);
}

VOID BiosUpdateNetworkAll()
{
  UPDATE_MODE   UpdateMode = UPDATE_MODE_NETWORK_ALL;
  BiosUpdate(UpdateMode);
}

BOOLEAN
EFIAPI
BackupImagetoUsb (
  IN EFI_DEVICE_PATH_PROTOCOL  *FilePath
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        FileHandle;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Fs;
  EFI_FILE_PROTOCOL                 *Root = NULL;
  EFI_FILE_PROTOCOL                 *File = NULL;
  CHAR16                            *FileName = L"\\BackupImage.bin";
  CHAR16                            *FilePathName = NULL;
  BYO_SMIFLASH_PROTOCOL             *ByoSmiFlash;
  UINT8                             *CurrentImage = NULL;
  UINT8                             *TepmImage = NULL;
  UINT32                            BiosSize;
  UINT32                             Offset;
  UINT32                            Index;
  UINTN                             Count;
  UINTN                             SendSize;
  UINTN                             TotalSize;
  BOOLEAN                           IsQuit = FALSE;
  CHAR16                            *DevicePathText;
  CHAR16                            *StrFileSystemUnwritable;

  BiosSize = PcdGet32(PcdFlashAreaSize);

  Status = gBS->LocateProtocol (
                  &gByoSmiFlashProtocolGuid,
                  NULL,
                  (VOID**)&ByoSmiFlash
                  );
  ASSERT(!EFI_ERROR(Status));

  Status = gBS->LocateDevicePath (&gEfiSimpleFileSystemProtocolGuid, &FilePath, &FileHandle);
  ASSERT(!EFI_ERROR(Status));

  DevicePathText = ConvertDevicePathToText (FilePath, FALSE, FALSE);
  if (StrLen(DevicePathText) == 1) { //root directory
    TotalSize =  StrSize (FileName);
    FilePathName = AllocatePool(TotalSize);
    StrCpyS(FilePathName, TotalSize / sizeof (CHAR16), FileName);
  } else {
    TotalSize =  StrSize(DevicePathText) + StrSize (FileName);
    FilePathName = AllocatePool(TotalSize);
    StrCpyS(FilePathName, StrSize(DevicePathText) / sizeof (CHAR16), DevicePathText);
    StrCatS(FilePathName, TotalSize / sizeof (CHAR16), FileName);
  }

  Status = gBS->HandleProtocol (
                 FileHandle,
                 &gEfiSimpleFileSystemProtocolGuid,
                 (VOID**)&Fs
                 );
  ASSERT (!EFI_ERROR(Status));

  Status = Fs->OpenVolume (Fs, &Root);
  if (EFI_ERROR(Status)) {
    goto Exit;
  }

  if (!FixedPcdGetBool(PcdGuiEnable)) {
    ShowCopyRightsAndWarning ();
  }

  CurrentImage = AllocatePool(BiosSize);
  ASSERT (CurrentImage != NULL);
  TepmImage = CurrentImage;
  Offset = 0;
  Count = BiosSize / SIZE_4KB;
  for (Index = 0;Index < Count;Index++) {
    Print(L"\r%s     (%d%%)", L"  Reading BIOS", Index * 100 / Count);
    Status = ByoSmiFlash->Read(ByoSmiFlash, Offset, CurrentImage, SIZE_4KB);
    if (EFI_ERROR(Status)) {
      goto Exit;
    }
    Offset += SIZE_4KB;
    CurrentImage = (UINT8*)((UINTN)TepmImage + Offset);
  }
  Count = BiosSize % SIZE_4KB;
  if (Count) {
    Status = ByoSmiFlash->Read(ByoSmiFlash, Offset, CurrentImage, (UINT32)Count);
    if (EFI_ERROR(Status)) {
      goto Exit;
    }
  }
  CurrentImage = TepmImage;
  Print(L"\r%s     (%d%%)\n", L"  Reading BIOS", 100);

  Status = Root->Open(Root, &File, FilePathName, EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0);
  if (EFI_ERROR(Status)) {
    goto Exit;
  }

  Count = BiosSize / SIZE_4KB;
  for (Index = 0; Index < Count; Index ++) {
    Print(L"\r%s      (%d%%)", L"  Backup BIOS", Index * 100 / Count);
    SendSize = SIZE_4KB;
    Status   = File->Write(File, &SendSize, (VOID *)CurrentImage);
    if (EFI_ERROR(Status)) {
      goto Exit;
    }
    CurrentImage = (UINT8*)((UINTN)CurrentImage + SIZE_4KB);
  }
  Count = BiosSize % SIZE_4KB;
  if (Count) {
    SendSize = Count;
    Status   = File->Write(File, &SendSize, (VOID *)CurrentImage);
    if (EFI_ERROR(Status)) {
      goto Exit;
    }
  }
  Print(L"\r%s      (%d%%)\n", L"  Backup BIOS", 100);

  gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_YELLOW);
  for (Index = 3; Index > 0; Index--) {
    Print (L"\rSystem will return in %d second(s)", Index);
    gBS->Stall (1000000);
  }
  IsQuit = TRUE;

Exit:
  if (!IsQuit) {
    StrFileSystemUnwritable = HiiGetString (gHiiHandle,  STRING_TOKEN (STR_FOLDER_UNWRITEABLE), NULL);
    UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, StrFileSystemUnwritable, NULL);
  }
  if (FilePathName != NULL) {
    FreePool(FilePathName);
  }
  if (File != NULL) {
    File->Close(File);
  }
  return TRUE;
}

VOID 
BiosBackup (
  IN  BOOLEAN      BackupToFlash
)
{
  EFI_STATUS                       Status;
  BYO_SMIFLASH_PROTOCOL            *ByoSmiFlash;
  CHAR16                           *Str;
  SELECTION_TYPE                   Choice;
  UINT32                           BiosSize;
  CHAR16                           *StrBackupError;
  UINT8                            *CurrentImage = NULL;
  UINT8                            *BackupImage = NULL;
  UINTN                            Dummy;
  UINTN                            *HotkeyDis = 0;
  EFI_FILE_EXPLORER_PROTOCOL       *FileExplorer;
  EFI_DEVICE_PATH_PROTOCOL         *FileDp = NULL;
  EFI_TPL                          OldTpl;
  UINT32                           Offset;
  UINT32                           FlashBlockSize;
  UINT32                           Index;
  UINTN                            NoSimpleFsHandles = 0;
  EFI_HANDLE                       *SimpleFsHandle = NULL;
  EFI_DEVICE_PATH_PROTOCOL         *DP = NULL;
  EFI_DEVICE_PATH_PROTOCOL         *UsbDp = NULL;
  EFI_DEVICE_PATH_PROTOCOL         *DeviceNode = NULL;
  EFI_DEVICE_PATH_PROTOCOL         *TempDP = NULL;
  CHAR16                           *StrFileSystemUnwritable;
  BOOLEAN                           IsUsbExist = TRUE;

  BiosSize = PcdGet32(PcdFlashAreaSize);

  Status = gBS->LocateProtocol(&gByoSetupHotKeyDisableProtocolGuid, NULL, (VOID**)&HotkeyDis);
  if(EFI_ERROR(Status)){
    HotkeyDis = &Dummy;
  }
  
  *HotkeyDis = BIT0;
    
  Status = gBS->LocateProtocol (
                  &gByoSmiFlashProtocolGuid,
                  NULL,
                  (VOID**)&ByoSmiFlash
                  );
  ASSERT(!EFI_ERROR(Status));

  if (BackupToFlash) {
    Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_BACKUP_BIOS_WARN_1), NULL);
  } else {
    Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_BACKUP_BIOS_WARN_2), NULL);
  }
  Choice = UiConfirmDialog(DIALOG_YESNO, NULL, NULL, TEXT_ALIGIN_CENTER, Str, NULL);
  FreePool (Str);
  if (Choice != SELECTION_YES) {
    goto ProcExit;
  }

  CurrentImage = AllocatePool(BiosSize);
  ASSERT (CurrentImage != NULL);

  BackupImage = AllocatePool(BiosSize);
  ASSERT (BackupImage != NULL);

  FlashBlockSize = ByoSmiFlash->GetFlashBlockSize();

  if (BackupToFlash) {
    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
    if (!FixedPcdGetBool(PcdGuiEnable)) {
      ShowCopyRightsAndWarning ();
    }

    Print(L"  Reading BIOS, please wait...\n");
    Status = ByoSmiFlash->Read(ByoSmiFlash, 0, CurrentImage, BiosSize);
    if (EFI_ERROR(Status)) {
      goto ProcExit;
    }
    
    Status = ByoSmiFlash->BackupRead(ByoSmiFlash, 0, BackupImage, BiosSize);
    if (EFI_ERROR(Status)) {
      goto ProcExit;
    }
    Print(L"  Read BIOS SUCCESS\n");

    for (Offset = 0; Offset < BiosSize; Offset += FlashBlockSize) {
      if (CompareMem (CurrentImage + Offset, BackupImage + Offset, FlashBlockSize)) {
        Status = ByoSmiFlash->ProgramBackupFlash(ByoSmiFlash, Offset, CurrentImage + Offset, FlashBlockSize);
      }
      if (EFI_ERROR(Status)) {
        StrBackupError = HiiGetString(gHiiHandle, STRING_TOKEN(STR_BACKUP_ERROR), NULL);
        SetOemDialogColor(DLG_OEM_COLOR_RED);
        UiConfirmDialog(DIALOG_NO_KEY, NULL, NULL, TEXT_ALIGIN_CENTER, StrBackupError, NULL);
        FreePool (StrBackupError);
        CpuDeadLoop();
      }
      Print(L"\r%s     (%d%%)", L"  Backup BIOS", Offset * 100 / BiosSize);
    }
    Print(L"\r%s     (%d%%)\n", L"  Backup BIOS", 100);

    gBS->RestoreTPL (OldTpl);
    gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_YELLOW);
    for (Index = 3; Index > 0; Index--) {
      Print (L"\rSystem will return in %d second(s)", Index);
      gBS->Stall (1000000);
    }
  } else {
    Status = gBS->LocateProtocol(&gEfiFileExplorerProtocolGuid, NULL, (void**)&FileExplorer);
    if (EFI_ERROR(Status)) {
      goto ProcExit;
    }

    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiSimpleFileSystemProtocolGuid,
                    NULL,
                    &NoSimpleFsHandles,
                    &SimpleFsHandle
                    );
    if (EFI_ERROR(Status)) {
      IsUsbExist = FALSE;
      goto ProcExit;
    }

    DeviceNode = (EFI_DEVICE_PATH_PROTOCOL *) AllocatePool (END_DEVICE_PATH_LENGTH);
    ASSERT (DeviceNode != NULL);
    SetDevicePathEndNode (DeviceNode);
    DeviceNode->SubType = END_INSTANCE_DEVICE_PATH_SUBTYPE;

    for (Index = 0;Index < NoSimpleFsHandles;Index++) {
      DP = DevicePathFromHandle(SimpleFsHandle[Index]);
      if (IsUsbDp(DP)) {
        if (UsbDp != NULL) {
          TempDP = UsbDp;
          UsbDp = AppendDevicePathNode (UsbDp, DeviceNode);
          if (TempDP != NULL) {
            FreePool(TempDP);
          }
          TempDP = UsbDp;
          UsbDp = AppendDevicePath(UsbDp,DP);
          if (TempDP != NULL) {
            FreePool(TempDP);
          }
        } else {
          UsbDp = DuplicateDevicePath(DP);
        }
      }
    }
    if (UsbDp == NULL) {
      IsUsbExist = FALSE;
      goto ProcExit;
    }
    Status = FileExplorer->ChooseFile(UsbDp, L"", BackupImagetoUsb, &FileDp);
    if (EFI_ERROR(Status)) {
      goto ProcExit;
    }
  }

ProcExit:
  *HotkeyDis = 0;
  if (!IsUsbExist) {
    StrFileSystemUnwritable = HiiGetString (gHiiHandle,  STRING_TOKEN (STR_NO_USB_DEVICE), NULL);
    UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, StrFileSystemUnwritable, NULL);
  }
  if (CurrentImage != NULL) {
    FreePool (CurrentImage);
  }
  if (BackupImage != NULL) {
    FreePool (BackupImage);
  }
  if (SimpleFsHandle != NULL) {
    FreePool(SimpleFsHandle);
  }
  if (DeviceNode != NULL) {
    FreePool(DeviceNode);
  }
  if (UsbDp != NULL) {
    FreePool(UsbDp);
  }
  DEBUG((EFI_D_INFO, "%a %r\n", __FUNCTION__, Status));

  return;
}

VOID
BiosRecovery (
  IN  BOOLEAN      RecoveryFormFlash
)
{
  EFI_STATUS                       Status;
  BYO_SMIFLASH_PROTOCOL            *ByoSmiFlash;
  CHAR16                           *Str;
  SELECTION_TYPE                   Choice;
  UINT8                            *BackupImage = NULL;
  UINT8                            *CurrentImage = NULL;
  UINT8                            *FileData = NULL;
  UINT32                           BiosSize;
  CHAR16                           *StrRecoveryError;
  UINTN                            Dummy;
  UINT32                           Offset;
  UINTN                            Index;
  UINTN                            *HotkeyDis = 0;
  EFI_FILE_EXPLORER_PROTOCOL       *FileExplorer;
  EFI_DEVICE_PATH_PROTOCOL         *FileDp = NULL;
  UINTN                            FileSize;
  EFI_TPL                          OldTpl;
  UINT32                           FlashBlockSize;
  UINT8                            ErrorType;
  UINTN                            Count;
  UINT8                            *TempImage = NULL;
  UINTN                            NoSimpleFsHandles = 0;
  EFI_HANDLE                       *SimpleFsHandle = NULL;
  EFI_DEVICE_PATH_PROTOCOL         *DP = NULL;
  EFI_DEVICE_PATH_PROTOCOL         *UsbDp = NULL;
  EFI_DEVICE_PATH_PROTOCOL         *DeviceNode = NULL;
  EFI_DEVICE_PATH_PROTOCOL         *TempDP = NULL;
  CHAR16                           *StrFileSystemUnwritable;
  BOOLEAN                           IsUsbExist = TRUE;

  BiosSize = PcdGet32(PcdFlashAreaSize);

  Status = gBS->LocateProtocol(&gByoSetupHotKeyDisableProtocolGuid, NULL, (VOID**)&HotkeyDis);
  if(EFI_ERROR(Status)){
    HotkeyDis = &Dummy;
  }
    
  *HotkeyDis = BIT0;
      
  Status = gBS->LocateProtocol (
                  &gByoSmiFlashProtocolGuid,
                  NULL,
                  (VOID**)&ByoSmiFlash
                  );
  ASSERT(!EFI_ERROR(Status));

  if (RecoveryFormFlash) {
    Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_RECOVERY_BIOS_WARN_1), NULL);
  } else {
    Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_RECOVERY_BIOS_WARN_2), NULL);
  }
  Choice = UiConfirmDialog(DIALOG_YESNO, NULL, NULL, TEXT_ALIGIN_CENTER, Str, NULL);
  FreePool (Str);
  if (Choice != SELECTION_YES) {
    goto ProcExit;
  }

  BackupImage = AllocatePool(BiosSize);
  ASSERT (BackupImage != NULL);

  CurrentImage =  AllocatePool(BiosSize);
  ASSERT (CurrentImage != NULL);

  FlashBlockSize = ByoSmiFlash->GetFlashBlockSize();

  if (RecoveryFormFlash) {
    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
    if (!FixedPcdGetBool(PcdGuiEnable)) {
      ShowCopyRightsAndWarning ();
    }

    Print(L"  Reading BIOS, please wait...\n");
    Status = ByoSmiFlash->Read(ByoSmiFlash, 0, CurrentImage, BiosSize);
    if (EFI_ERROR(Status)) {
      goto ProcExit;
    }

    Status = ByoSmiFlash->BackupRead(ByoSmiFlash, 0, BackupImage, BiosSize);
    if (EFI_ERROR(Status)) {
      goto ProcExit;
    }
    Print(L"  Read BIOS SUCCESS\n");

    for (Offset = 0; Offset < BiosSize; Offset += FlashBlockSize) {
      if (CompareMem (CurrentImage + Offset, BackupImage + Offset, FlashBlockSize)) {
        Status = ByoSmiFlash->ProgramFlash(ByoSmiFlash, Offset, BackupImage + Offset, FlashBlockSize);
      }
      if (EFI_ERROR(Status)) {
        StrRecoveryError = HiiGetString(gHiiHandle, STRING_TOKEN(STR_RECOVERY_ERROR), NULL);
        SetOemDialogColor(DLG_OEM_COLOR_RED);
        UiConfirmDialog(DIALOG_NO_KEY, NULL, NULL, TEXT_ALIGIN_CENTER, StrRecoveryError, NULL);
        FreePool (StrRecoveryError);
        CpuDeadLoop();
      }
      Print(L"\r%s     (%d%%)", L"  Recovery Bios Region", Offset * 100 / BiosSize);
    }
    Print(L"\r%s     (%d%%)\n", L"  Recovery Bios Region", 100);
  } else {
    Status = gBS->LocateProtocol(&gEfiFileExplorerProtocolGuid, NULL, (void**)&FileExplorer);
    if (EFI_ERROR(Status)) {
      goto ProcExit;
    }

    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiSimpleFileSystemProtocolGuid,
                    NULL,
                    &NoSimpleFsHandles,
                    &SimpleFsHandle
                    );
    if (EFI_ERROR(Status)) {
      IsUsbExist = FALSE;
      goto ProcExit;
    }

    DeviceNode = (EFI_DEVICE_PATH_PROTOCOL *) AllocatePool (END_DEVICE_PATH_LENGTH);
    ASSERT (DeviceNode != NULL);
    SetDevicePathEndNode (DeviceNode);
    DeviceNode->SubType = END_INSTANCE_DEVICE_PATH_SUBTYPE;

    for (Index = 0;Index < NoSimpleFsHandles;Index++) {
      DP = DevicePathFromHandle(SimpleFsHandle[Index]);
      if (IsUsbDp(DP)) {
        if (UsbDp != NULL) {
          TempDP = UsbDp;
          UsbDp = AppendDevicePathNode (UsbDp, DeviceNode);
          if (TempDP != NULL) {
            FreePool(TempDP);
          }
          TempDP = UsbDp;
          UsbDp = AppendDevicePath(UsbDp,DP);
          if (TempDP != NULL) {
            FreePool(TempDP);
          }
        } else {
          UsbDp = DuplicateDevicePath(DP);
        }
      }
    }
    if (UsbDp == NULL) {
      IsUsbExist = FALSE;
      goto ProcExit;
    }
    Status = FileExplorer->ChooseFile(UsbDp, L".bin", NULL, &FileDp);
    if (EFI_ERROR(Status)) {
      goto ProcExit;
    }

    Status = LoadFileByDp(FileDp, &FileData, &FileSize);
    if (EFI_ERROR(Status)) {
      goto ProcExit;
    }

    Status = ByoSmiFlash->BiosCheck(ByoSmiFlash, FileData, FileSize, NULL);
    if (EFI_ERROR(Status)) {
      if (Status == EFI_SECURITY_VIOLATION) {
        ErrorType = 0;
      } else {
        ErrorType = 1;
      }
      ShowFileNotMatch(ErrorType);
      goto ProcExit;    
    }

    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
    if (!FixedPcdGetBool(PcdGuiEnable)) {
      ShowCopyRightsAndWarning ();
    }

    TempImage = CurrentImage;
    Offset = 0;
    Count = BiosSize / SIZE_4KB;
    for (Index = 0;Index < Count;Index++) {
      Print(L"\r%s      (%d%%)", L"  Reading Current Image", Index * 100 / Count);
      Status = ByoSmiFlash->Read(ByoSmiFlash, Offset, CurrentImage, SIZE_4KB);
      if (EFI_ERROR(Status)) {
        goto ProcExit;
      }
      Offset += SIZE_4KB;
      CurrentImage = (UINT8*)((UINTN)TempImage + Offset);
    }
    Count = BiosSize % SIZE_4KB;
    if (Count) {
      Status = ByoSmiFlash->Read(ByoSmiFlash, Offset, CurrentImage, (UINT32)Count);
      if (EFI_ERROR(Status)) {
        goto ProcExit;
      }
    }
    CurrentImage = TempImage;
    Print(L"\r%s      (%d%%)\n", L"  Reading Current Image", 100);

    TempImage = BackupImage;
    Offset = 0;
    Count = BiosSize / SIZE_4KB;
    for (Index = 0;Index < Count;Index++) {
      Print(L"\r%s       (%d%%)", L"  Reading Backup Image", Index * 100 / Count);
      Status = ByoSmiFlash->BackupRead(ByoSmiFlash, Offset, BackupImage, SIZE_4KB);
      if (EFI_ERROR(Status)) {
        goto ProcExit;
      }
      Offset += SIZE_4KB;
      BackupImage = (UINT8*)((UINTN)TempImage + Offset);
    }
    Count = BiosSize % SIZE_4KB;
    if (Count) {
      Status = ByoSmiFlash->BackupRead(ByoSmiFlash, Offset, BackupImage, (UINT32)Count);
      if (EFI_ERROR(Status)) {
        goto ProcExit;
      }
    }
    BackupImage = TempImage;
    Print(L"\r%s       (%d%%)\n", L"  Reading Backup Image", 100);

    for (Offset = 0; Offset < BiosSize; Offset += FlashBlockSize) {
      if (CompareMem (BackupImage + Offset, FileData + Offset, FlashBlockSize)) {
        Status = ByoSmiFlash->ProgramBackupFlash(ByoSmiFlash, Offset, FileData + Offset, FlashBlockSize);
      }
      if (EFI_ERROR(Status)) {
        StrRecoveryError = HiiGetString(gHiiHandle, STRING_TOKEN(STR_RECOVERY_ERROR), NULL);
        SetOemDialogColor(DLG_OEM_COLOR_RED);
        UiConfirmDialog(DIALOG_NO_KEY, NULL, NULL, TEXT_ALIGIN_CENTER, StrRecoveryError, NULL);
        FreePool (StrRecoveryError);
        CpuDeadLoop();
      }
      Print(L"\r%s     (%d%%)", L"  Recovery Backup Region", Offset * 100 / BiosSize);
    }
    Print(L"\r%s     (%d%%)\n",L"  Recovery Backup Region", 100);

    for (Offset = 0; Offset < BiosSize; Offset += FlashBlockSize) {
      if (CompareMem (CurrentImage + Offset, FileData + Offset, FlashBlockSize)) {
        Status = ByoSmiFlash->ProgramFlash(ByoSmiFlash, Offset, FileData + Offset, FlashBlockSize);
      }
      if (EFI_ERROR(Status)) {
        StrRecoveryError = HiiGetString(gHiiHandle, STRING_TOKEN(STR_RECOVERY_ERROR), NULL);
        SetOemDialogColor(DLG_OEM_COLOR_RED);
        UiConfirmDialog(DIALOG_NO_KEY, NULL, NULL, TEXT_ALIGIN_CENTER, StrRecoveryError, NULL);
        FreePool (StrRecoveryError);
        CpuDeadLoop();
      }
      Print(L"\r%s       (%d%%)", L"  Recovery Bios Region", Offset * 100 / BiosSize);
    }
    Print(L"\r%s       (%d%%)\n", L"  Recovery Bios Region", 100);
  }

  gBS->RestoreTPL (OldTpl);
  gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_YELLOW);
  Print(L"Flash is Recovery successfully!\n\n");
  for (Index = 5; Index > 0; Index--) {
    Print (L"\rSystem will reset in %d second(s)", Index);
    gBS->Stall (1000000);
  }
  gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);    

ProcExit:
  *HotkeyDis = 0;
  if (!IsUsbExist) {
    StrFileSystemUnwritable = HiiGetString (gHiiHandle,  STRING_TOKEN (STR_NO_USB_DEVICE), NULL);
    UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, StrFileSystemUnwritable, NULL);
  }
  if (CurrentImage != NULL) {
    FreePool (CurrentImage);
  }
  if (BackupImage != NULL) {
    FreePool (BackupImage);
  }
  if (FileDp != NULL) {
    FreePool (FileDp);
  }
  if (SimpleFsHandle != NULL) {
    FreePool(SimpleFsHandle);
  }
  if (DeviceNode != NULL) {
    FreePool(DeviceNode);
  }
  if (UsbDp != NULL) {
    FreePool(UsbDp);
  }
  DEBUG((EFI_D_INFO, "%a %r\n", __FUNCTION__, Status));
  return;
}

STATIC BYO_BACKUP_RECOVERY_PROTOCOL mBackupRecoveryProtocol = {
  BiosBackup,
  BiosRecovery
};

EFI_STATUS
EFIAPI
BiosUpdateEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS  Status;

  gHiiHandle = HiiAddPackages (
                &gEfiCallerIdGuid,
                ImageHandle,
                STRING_ARRAY_NAME,
                NULL
                );
  ASSERT(gHiiHandle != NULL);

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gByoSetupBiosUpdateProtocolGuid, BiosUpdateDefault,
                  &gByoSetupBiosUpdateAllProtocolGuid, BiosUpdateAll,
                  &gByoSetupBiosUpdateNetworkProtocolGuid,BiosUpdateNetwork,
                  &gByoSetupBiosUpdateAllNetworkProtocolGuid,BiosUpdateNetworkAll,
                  &gByoBackupRecoveryProtocolGuid, &mBackupRecoveryProtocol,
                  NULL
                  );
  ASSERT(!EFI_ERROR(Status));
  
  return Status;
}
