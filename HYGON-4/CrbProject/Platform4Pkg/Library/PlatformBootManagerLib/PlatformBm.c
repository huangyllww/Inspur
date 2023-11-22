/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PlatformBm.c

Abstract:
  The platform support PlatformBootManagerLib.

Revision History:

**/

#include "PlatformBm.h"
#include <SysMiscCfg.h>
#include <ByoStatusCode.h>
#include <Uefi/UefiSpec.h>
#include <Library/HiiLib.h>
#include <Library/HobLib.h>
#include <Library/BootLogoLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/ByoCommLib.h>
#include <Library/UefiBootManagerLib.h>
#include <Library/PerformanceLib.h>
#include <Library/PrintLib.h>
#include <Library/ByoBootManagerLib.h>
#include <Library/Tcg2PhysicalPresenceLib.h>
#include <Library/TcgPhysicalPresenceLib.h>
#include <ByoBootGroup.h>
#include <Protocol/PciIo.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/UsbIo.h>
#include <Protocol/DeferredImageLoad.h>
#include <Guid/GlobalVariable.h>
#include <Guid/CapsuleVendor.h>
#include <Pi/PiStatusCode.h>
#include <Protocol/ByoSmiFlashProtocol.h>
#include <IndustryStandard/Pci22.h>
#include <IndustryStandard/Pci30.h>
#include <Library/PlatformCommLib.h>
#include <Protocol/NetworkInterfaceIdentifier.h>

STATIC EFI_GUID gDriverSnpDxeGuid = {0xA2F436EA, 0xA127, 0x4EF8, {0x95, 0x7C, 0x80, 0x48, 0x60, 0x6F, 0xF6, 0x70}};

VOID
PostUpdateProgress (
  IN UINTN   Progress
  );

VOID
BmConnectAllDriversToAllControllers (
  VOID
  );


STATIC POST_HOT_KEY_CTX gDefaultPostHotKeyCtx[] = {
  {
    {SCAN_F2, CHAR_NULL},
    NULL,
    0,
    HOTKEY_BOOT_SETUP,
    NULL,
    PcdGetPtr (PcdBootManagerMenuFile),
    NULL,
    L"Press [F2]     to enter setup and select boot options.\n",
    L"Setup"
  },
  {
    {SCAN_F7, CHAR_NULL},
    NULL,
    0,
    HOTKEY_BOOT_MENU,
    NULL,
    &gBootMenuFileGuid,
    NULL,
    L"Press [F7]     to show boot menu options.\n",
    L"BootMenu"
  },
  {
    {SCAN_NULL, CHAR_CARRIAGE_RETURN},
    NULL,
    0,
    HOTKEY_BOOT_PASS,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },
};

UINT8 gDefaultUefiBootGroupOrder[] = {
  BM_MENU_TYPE_UEFI_HDD,
  BM_MENU_TYPE_UEFI_PXE,
  BM_MENU_TYPE_UEFI_ODD, 
  BM_MENU_TYPE_UEFI_USB_DISK,
  BM_MENU_TYPE_UEFI_USB_ODD,  
  BM_MENU_TYPE_UEFI_OTHERS
  };

POST_HOT_KEY_CTX    *gPostHotKeyCtx  = gDefaultPostHotKeyCtx;
UINTN               gPostHotKeyCount = sizeof(gDefaultPostHotKeyCtx) / sizeof(gDefaultPostHotKeyCtx[0]);
EFI_HII_HANDLE      gStringPlatformBmHandle;
STATIC UINT8        gEraseCountLine  = 0;
STATIC UINTN        gMaxStrLen       = 32;
STATIC UINTN        gHotKeyAttribute = 0;
STATIC UINT16       gBdsWaitTimeOut;

VOID
PrepareAndRegisterHotKey (
  VOID
  )
{
  EFI_STATUS                      Status;
  PLAT_HOST_INFO_PROTOCOL         *PlatHostInfo;
  EFI_KEY_DATA                    KeyData;
  UINTN                           Index;
  POST_HOT_KEY_CTX                *Ctx;
  INTN                            OptionNumber;
  BOOLEAN                         IsBootCategory = TRUE;
  CHAR16                          *Desc = L"";
  EFI_BOOT_MANAGER_LOAD_OPTION    BootManagerMenu;

  
  //
  // Locate gPlatHostInfoProtocolGuid to get hotkey, or use the default
  //
  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, (VOID **)&PlatHostInfo);
  if (!EFI_ERROR(Status) && PlatHostInfo->HotKey != NULL && PlatHostInfo->HotKeyCount != 0) {
    gPostHotKeyCtx  = PlatHostInfo->HotKey;
    gPostHotKeyCount = PlatHostInfo->HotKeyCount;
  }

  ZeroMem(&KeyData, sizeof(KeyData));
  gHotKeyAttribute = 0;
  for (Index = 0; Index < gPostHotKeyCount; Index++) {
    Ctx = &gPostHotKeyCtx[Index];
    gHotKeyAttribute |= Ctx->Attribute & HOTKEY_ATTRIBUTE_GLOBAL_MASK;
    if ((Ctx->Key.ScanCode == SCAN_NULL && Ctx->Key.UnicodeChar == CHAR_NULL) || (Ctx->BootType == HOTKEY_BOOT_NONE)) {
      Ctx->Attribute |= HOTKEY_ATTRIBUTE_DISABLED;
    }

    //
    // 1) Filter out disabled hotkey
    //
    if (Ctx->Attribute & HOTKEY_ATTRIBUTE_DISABLED) {
      continue;
    }

    //
    // 2) build key option
    //
    if (((Ctx->Attribute & HOTKEY_ATTRIBUTE_OEM_HANDLER) == 0) && (Ctx->FileName!=NULL)) {
      if (Ctx->BootOptionName != NULL) {
        //
        // set description of setup
        //
        if (Ctx->BootType == HOTKEY_BOOT_SETUP) {
          Desc = SETUP_BOOT_STRING;
        } else {
          Desc = Ctx->BootOptionName;
        }          
      }
      if ((Ctx->BootType == HOTKEY_BOOT_PXE) || (Ctx->BootType == HOTKEY_BOOT_SETUP) ||            \
          (Ctx->BootType == HOTKEY_BOOT_MENU) || (Ctx->BootType == HOTKEY_BOOT_OS_BACKUP_RESTORE)) {
        IsBootCategory = FALSE;
      }

      OptionNumber = LoadOptionNumberUnassigned;
      if(Ctx->BootType == HOTKEY_BOOT_SETUP){
        Status = EfiBootManagerGetBootManagerMenu(&BootManagerMenu);
        if(!EFI_ERROR(Status)){
          OptionNumber = (INTN)BootManagerMenu.OptionNumber;
          EfiBootManagerFreeLoadOption(&BootManagerMenu);
          DEBUG((EFI_D_INFO, "setup %d\n", OptionNumber));
        }
      }

      if(OptionNumber == LoadOptionNumberUnassigned){
        OptionNumber = RegisterFvFileBootOption (
                         Ctx->FileName,
                         Desc,
                         IsBootCategory,
                         NULL,
                         0
                         );
      }
      DEBUG((EFI_D_INFO, "RegisterFvFileBootOption %s No:%d\n", Desc, OptionNumber));
      Status = EfiBootManagerAddKeyOptionVariable (
           NULL, (UINT16)OptionNumber, 0, &Ctx->Key, NULL
           );
    }
  }
}

STATIC
VOID
ConnectRootBridge (
  VOID
  )
{
  UINTN                            RootBridgeHandleCount;
  EFI_HANDLE                       *RootBridgeHandleBuffer = NULL;
  UINTN                            RootBridgeIndex;


  InvokeHookProtocol(gBS, &gEfiBeforeConnectPciRootBridgeGuid);

  RootBridgeHandleCount = 0;
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiPciRootBridgeIoProtocolGuid,
         NULL,
         &RootBridgeHandleCount,
         &RootBridgeHandleBuffer
         );
  for (RootBridgeIndex = 0; RootBridgeIndex < RootBridgeHandleCount; RootBridgeIndex++) {
    gBS->ConnectController(RootBridgeHandleBuffer[RootBridgeIndex], NULL, NULL, FALSE);
  }

  InvokeHookProtocol(gBS, &gEfiAfterAllPciIoGuid); 

  if(RootBridgeHandleBuffer!=NULL){FreePool(RootBridgeHandleBuffer);}	

  InvokeHookProtocol(gBS, &gEfiAfterConnectPciRootBridgeGuid); 

  DEBUG((EFI_D_INFO, "RootBrigdeConnected\n"));
}

STATIC
EFI_STATUS 
GetVideoController (
    EFI_HANDLE  **DisplayHost,
    UINTN       *HostCount
  )
{
  EFI_STATUS                Status;
  UINTN                     HandleCount;
  EFI_HANDLE                *HandleBuffer = NULL;
  UINTN                     Index;
  EFI_HANDLE                VideoController = NULL;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  UINT8                     ClassCode[3];
  PLAT_HOST_INFO_PROTOCOL   *ptHostInfo = NULL;
  EFI_DEVICE_PATH_PROTOCOL  *IgdPciDp = NULL;
  UINTN                     IgdPciDpSize = 0;
  EFI_HANDLE                IgdHandle = NULL;
  EFI_HANDLE                PegHandle = NULL;
  EFI_DEVICE_PATH_PROTOCOL  *PciDp;
  UINT32                    SystemMiscCfg;
  EFI_HANDLE                *VideoHandles = NULL;
  EFI_HANDLE                Temp;
  UINT32                    PciId;
  UINT32                    DidVid;
  

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  *HostCount = 0;
  SystemMiscCfg = PcdGet32(PcdSystemMiscConfig);

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, (VOID **)&ptHostInfo);
  if(!EFI_ERROR(Status)){
    IgdPciDp = ptHostInfo->IgdDp;
    IgdPciDpSize = ptHostInfo->IgdDpSize;
  }
  DEBUG((EFI_D_INFO, "IgdPciDp:%X, IgdPciDpSize:%X\n", IgdPciDp, IgdPciDpSize)); 

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status) || HandleCount == 0) {
    goto ProcExit;
  }

  for (Index = 0; Index < HandleCount; Index++) {

    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
    ASSERT(!EFI_ERROR (Status));

    if (PciIo->RomSize == 0) {
      // Skip PCI Device which RomSize is 0.
      continue;
    }

    Status = PciIo->Pci.Read (
                      PciIo,
                      EfiPciIoWidthUint8,
                      0x9,
                      3,
                      ClassCode
                      );
    if (EFI_ERROR (Status)) {
      DEBUG((EFI_D_ERROR, "ClassCode Read error:%r\n", Status));
      continue;
    }

    Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, PCI_VENDOR_ID_OFFSET, 1, &DidVid);

    if(!IsGfxClassCode(ClassCode, DidVid)){
      continue;
    }

    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID**)&PciDp);
    if (EFI_ERROR (Status)) {
      DEBUG((EFI_D_ERROR, "Dp not found\n"));
      continue;
    }
    if(IgdHandle == NULL && IgdPciDp != NULL && CompareMem(PciDp, IgdPciDp, IgdPciDpSize) == 0){
      IgdHandle = HandleBuffer[Index];
      PcdSet32S(PcdSystemMiscConfig, SystemMiscCfg | SYS_MISC_CFG_IGD_PRESENT);
    } else {
      if(PegHandle == NULL){
        PegHandle = HandleBuffer[Index];
        Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciIoWidthUint32,
                          0,
                          1,
                          &PciId
                          ); 
        if(!EFI_ERROR(Status) && ptHostInfo != NULL){
          ptHostInfo->AddOnGfxId = PciId;
        }
        
      }
    }
  
    VideoHandles = ReallocatePool (
                     sizeof(EFI_HANDLE) * (*HostCount),
                     sizeof(EFI_HANDLE) * (*HostCount + 1),
                     VideoHandles
                     );
    VideoHandles[(*HostCount)++] = HandleBuffer[Index];

  }

  if(SystemMiscCfg & SYS_MISC_CFG_PRI_VGA_IGD){
    if(IgdHandle != NULL){
      VideoController = IgdHandle;
    } else {
      VideoController = PegHandle;
    }
  } else {
    if(PegHandle != NULL){
      VideoController = PegHandle;
    } else {
      VideoController = IgdHandle;
    }
  }

  if(VideoController != NULL){
    for(Index=0;Index<*HostCount;Index++){
      if(VideoHandles[Index] == VideoController){
        break;
      }
    }
    if(Index != 0){
      Temp = VideoHandles[Index];
      VideoHandles[Index] = VideoHandles[0];
      VideoHandles[0] = Temp;
    }
  }

ProcExit:
  if(HandleBuffer != NULL){
    FreePool(HandleBuffer);
  }
  if(*HostCount != 0){
    *DisplayHost = VideoHandles;
    Status = EFI_SUCCESS;
    DEBUG((EFI_D_INFO, "%a HostCount:%X\n", __FUNCTION__, *HostCount));
    if(IgdHandle != NULL){
      
    }
  } else {
    Status = EFI_NOT_FOUND;
  }
  return Status;
}

STATIC
BOOLEAN IsDevPathSerialPort(EFI_DEVICE_PATH_PROTOCOL *Dp)
{
  EFI_DEVICE_PATH_PROTOCOL  *Next;

  Next = Dp;
  while (!IsDevicePathEndType(Next)) {
    if ((Next->Type == ACPI_DEVICE_PATH) && (Next->SubType == ACPI_DP) &&
        (((ACPI_HID_DEVICE_PATH *) Next)->HID == EISA_PNP_ID (0x501))) {
      return TRUE;
    }
    if(Next->Type == MESSAGING_DEVICE_PATH && Next->SubType == MSG_UART_DP){
      return TRUE;
    }
    Next = NextDevicePathNode(Next);
  }

  return FALSE;
}

STATIC
VOID RemoveInvalidConOutVar(EFI_HANDLE FirstGfxHandle)
{
  EFI_DEVICE_PATH_PROTOCOL  *VarConsole;

  EFI_DEVICE_PATH_PROTOCOL  *Temp; 
  EFI_DEVICE_PATH_PROTOCOL  *Temp2;
  UINTN                     DevicePathSize;
  EFI_DEVICE_PATH_PROTOCOL  *Instance;
  UINTN                     InstanceSize;
  EFI_STATUS                Status;
  EFI_HANDLE                Handle;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  UINT8                     ClassCode[3];
  BOOLEAN                   NeedRemove;
  UINT8                     *NewVar;
  UINTN                     Index = 0;
  BOOLEAN                   IsUart;
  EFI_DEVICE_PATH_PROTOCOL  *End;
  

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  VarConsole = BootManagerGetVariableAndSize (
                 L"ConOut",
                 &gEfiGlobalVariableGuid,
                 &DevicePathSize
                 );
  if(VarConsole == NULL){
    return;
  }

  DEBUG((EFI_D_INFO, "Old "));
  ShowDevicePathDxe(gBS, VarConsole);

  NewVar = AllocateZeroPool(DevicePathSize);
  if(NewVar == NULL){
    goto ProcExit;
  }

  Temp = VarConsole;
  while(1){
    Instance = GetNextDevicePathInstance(&Temp, &InstanceSize);
    if(Instance == NULL){
      break;
    }

    IsUart = IsDevPathSerialPort(Instance);
    
    NeedRemove = FALSE;
    if(!IsUart){
      Temp2 = Instance;
      Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Temp2, &Handle);
      if(!EFI_ERROR(Status)){
        Status = gBS->HandleProtocol(Handle, &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
        PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 9, sizeof(ClassCode), ClassCode);
        if(ClassCode[2] != 3){
          NeedRemove = TRUE;
        } else {
          if(FirstGfxHandle != NULL && FirstGfxHandle != Handle){
            NeedRemove = TRUE;          
          }
        }
      }
    }

    if(!NeedRemove){
      CopyMem(NewVar + Index, Instance, InstanceSize);
      Index += InstanceSize;
    } else {
      DEBUG((EFI_D_INFO, "Remove "));
      ShowDevicePathDxe(gBS, Instance);
    }

    FreePool(Instance);
  }

  if(Index){

    Temp = (EFI_DEVICE_PATH_PROTOCOL*)NewVar;
    End  = (EFI_DEVICE_PATH_PROTOCOL*)((UINTN)Temp + Index - 4);
    while (Temp < End) {
      if(IsDevicePathEndType(Temp)){
        Temp->SubType = END_INSTANCE_DEVICE_PATH_SUBTYPE;
      }
      Temp = NextDevicePathNode(Temp);
    }
    
    DEBUG((EFI_D_INFO, "New "));
    ShowDevicePathDxe(gBS, (EFI_DEVICE_PATH_PROTOCOL*)NewVar);
  }

  Status = gRT->SetVariable(
                  L"ConOut", 
                  &gEfiGlobalVariableGuid, 
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  Index,
                  NewVar
                  );
  DEBUG((EFI_D_INFO, "Set ConOut: %r L:%d\n", Status, Index));

ProcExit:
  FreePool(VarConsole);
  if(NewVar != NULL){
    FreePool(NewVar);
  }
}

STATIC
EFI_STATUS 
GetHostGopHandle (
  EFI_HANDLE                 PciHandle, 
  EFI_HANDLE                 *GopHandle,
  EFI_DEVICE_PATH_PROTOCOL   **ppGopDp
  )
{
  EFI_STATUS                 Status;
  EFI_DEVICE_PATH_PROTOCOL   *PciDp;
  EFI_DEVICE_PATH_PROTOCOL   *GopDp;  
  UINTN                      HandleCount;
  EFI_HANDLE                 *HandleBuffer = NULL;
  UINTN                      Index;
  UINTN                      PciDpSize;
  UINTN                      GopDpSize;


  Status = gBS->HandleProtocol(PciHandle, &gEfiDevicePathProtocolGuid, (VOID**)&PciDp);
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "%a() (L%d) %r\n", __FUNCTION__, __LINE__, Status));
    goto ProcExit;
  }    

  ShowDevicePathDxe(gBS, PciDp);

  PciDpSize = GetDevicePathSize(PciDp);

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status) || HandleCount == 0) {
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }

  DEBUG((EFI_D_INFO, "GOPs:%d, PciDpSize:%d\n", HandleCount, PciDpSize));

  Status = EFI_NOT_FOUND;
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol(HandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID**)&GopDp);
    if(EFI_ERROR(Status)){
      continue;
    }
    GopDpSize = GetDevicePathSize(GopDp);
    DEBUG((EFI_D_INFO, "GopDpSize:%d\n", GopDpSize));    

    if(PciDpSize > 4 && GopDpSize >= PciDpSize && CompareMem(GopDp, PciDp, PciDpSize-4) == 0){
      *GopHandle = HandleBuffer[Index];
      *ppGopDp   = GopDp;
      Status = EFI_SUCCESS;
      break;
    }
  }

ProcExit:
  if(HandleBuffer != NULL){
    FreePool(HandleBuffer);
  }  
  DEBUG((EFI_D_INFO, "%a() Exit:%r\n", __FUNCTION__, Status));
  return Status;
}


CONST ACPI_ADR_DEVICE_PATH gAcpiAdrVgaDpNode = {
  {
    ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    { sizeof (ACPI_ADR_DEVICE_PATH), 0 },
  },
  ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0)
};


/**
  Find the platform active active video controller and connect it.

  @retval EFI_NOT_FOUND There is no active video controller.
  @retval EFI_SUCCESS   The video controller is connected.
**/
STATIC
EFI_STATUS
UpdateGopDp (
  VOID
  )
{
  EFI_HANDLE                 *DisplayHost = NULL;
  UINTN                      HostCount;
  UINTN                      ThisCount;
  EFI_DEVICE_PATH_PROTOCOL   *GopDp;
  EFI_DEVICE_PATH_PROTOCOL   *PciDp;
  EFI_STATUS                 Status;
  UINTN                      Index;
  UINT32                     SystemMiscCfg;
  UINTN                      Count = 0;
  EFI_PCI_IO_PROTOCOL        *PciIo;
  UINT32                     PciId = 0xFFFFFFFF;
  UINT8                      Csm;


  Status = GetVideoController(&DisplayHost, &HostCount);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  PcdSet64S(PcdFirstVideoHostHandle, (UINT64)(UINTN)DisplayHost[0]);

  SystemMiscCfg = PcdGet32(PcdSystemMiscConfig);

  if((SystemMiscCfg & SYS_MISC_CFG_DUAL_VGA) && HostCount > 1){
    if(PcdGet32(PcdVideoHorizontalResolution) == 0){                  // wz200822 +
      Status = PcdSet32S (PcdVideoHorizontalResolution, 1024);
      Status = PcdSet32S (PcdVideoVerticalResolution, 768);
    }                                                                 // wz200822 +
  }

  Csm = PcdGet8(PcdLegacyBiosSupport);
  
  //
  // Try to connect the PCI device path, so that GOP dirver could start on this 
  // device and create child handles with GraphicsOutput Protocol installed
  // on them, then we get device paths of these child handles and select 
  // them as possible console device.
  //

  RemoveInvalidConOutVar(Csm ? DisplayHost[0] : NULL);

  ThisCount = HostCount;
  if(Csm && HostCount > 1){
    ThisCount = 1;
  }

  for(Index=0;Index<ThisCount;Index++){

    DEBUG((EFI_D_INFO, "DisplayHost[%d]\n", Index));

    Status = gBS->HandleProtocol (
                    DisplayHost[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID**)&PciDp
                    );   
    if(EFI_ERROR(Status)){
      continue;
    }

// patch 0x0750126F
    Status = gBS->HandleProtocol (
                    DisplayHost[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID**)&PciIo
                    );   
    if(!EFI_ERROR(Status)){
      PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0, 1, &PciId);
    }
    DEBUG((EFI_D_INFO, "PciId:%08x\n", PciId));
    if(PciId == 0x0750126F && !Csm){
      GopDp = AppendDevicePathNode(PciDp, NULL);
    } else {
      GopDp = AppendDevicePathNode(PciDp, (EFI_DEVICE_PATH_PROTOCOL*)&gAcpiAdrVgaDpNode);
    }
    ShowDevicePathDxe(gBS, GopDp);
    Status = EfiBootManagerUpdateConsoleVariable(ConOut, GopDp, NULL);
    FreePool(GopDp);
    if(!EFI_ERROR(Status)){
      Count++;
    }
  }

  if(DisplayHost != NULL){
    FreePool(DisplayHost);
  }

  if(Count){
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

VOID
UpdateCsm16UCR (
  IN UINT16   UartIoBase
  )
{
  UINTN                      Ebda;

  Ebda = (*(UINT16*)(UINTN)0x40E) << 4;

  *((volatile UINT8*) (Ebda + 0x1C2))  = 1;                  //flag for CSM support
  *((volatile UINT16*)(Ebda + 0x1C3)) = UartIoBase;
}








UINT32 GetUartAcpiUid(EFI_DEVICE_PATH_PROTOCOL *Instance)
{
  EFI_DEVICE_PATH_PROTOCOL  *Next;

  Next = Instance;
  while (!IsDevicePathEndType(Next)) {
    if (Next->Type == ACPI_DEVICE_PATH && Next->SubType == ACPI_DP &&
        ((ACPI_HID_DEVICE_PATH*)Next)->HID == EISA_PNP_ID(0x0501)) {
      return ((ACPI_HID_DEVICE_PATH*)Next)->UID;
    }
    Next = NextDevicePathNode(Next);
  }

  return 0xFFFFFFFF;
}

typedef struct {
  BOOLEAN                   Match;
  UINT32                    Uid;    
  EFI_DEVICE_PATH_PROTOCOL  *Dp;
  UINTN                     DpSize;
} UART_DP_INFO;


VOID SyncUartInfo(UART_DP_INFO *InfoA, UINTN InfoACount, UART_DP_INFO *InfoB, UINTN InfoBCount)
{
  UINTN  IndexA;
  UINTN  IndexB;

  for(IndexA=0;IndexA<InfoACount;IndexA++){
    for(IndexB=0;IndexB<InfoBCount;IndexB++){
      if(InfoB[IndexB].Uid == 0xFFFFFFFF || InfoB[IndexB].Match){
        continue;
      }
      if(InfoB[IndexB].Uid == InfoA[IndexA].Uid){
        if(InfoA[IndexA].DpSize == InfoB[IndexB].DpSize && 
          CompareMem(InfoA[IndexA].Dp, InfoB[IndexB].Dp, InfoA[IndexA].DpSize) == 0){
          InfoA[IndexA].Match = TRUE;
          InfoB[IndexB].Match = TRUE;
        }
        break;
      }
    }
  }
}


VOID ClearUartInfoMatchStatus(UART_DP_INFO *Info, UINTN InfoCount)
{
  UINTN  Index;

  if(InfoCount == 0){
    return;
  }

  for(Index=0;Index<InfoCount;Index++){
    Info[Index].Match = FALSE;
  }
}

VOID FreeUartDpInfo(UART_DP_INFO *Info, UINTN InfoCount)
{
  UINTN  Index;

  if(InfoCount == 0){
    return;
  }

  for(Index=0;Index<InfoCount;Index++){
    if(Info[Index].Dp != NULL){
      FreePool(Info[Index].Dp);
    }
  }

  FreePool(Info);
}


VOID UartDpInfoInit(EFI_DEVICE_PATH_PROTOCOL *DpIn, UART_DP_INFO **pInfo, UINTN *pInfoCount)
{
  EFI_DEVICE_PATH_PROTOCOL        *Instance;
  UINTN                           Size;  
  EFI_DEVICE_PATH_PROTOCOL        *Dp = DpIn;
  UART_DP_INFO                    *Info = NULL;
  UINTN                           InfoCount = 0;
  
  while(1){
    Instance = GetNextDevicePathInstance(&Dp, &Size);
    if(Instance == NULL){
      break;
    }
    Info = (UART_DP_INFO*)ReallocatePool (
                            sizeof(UART_DP_INFO) * InfoCount,
                            sizeof(UART_DP_INFO) * (InfoCount + 1),
                            Info
                            ); 
    Info[InfoCount].Dp     = Instance;
    Info[InfoCount].Uid    = GetUartAcpiUid(Instance);  
    Info[InfoCount].Match  = FALSE;
    Info[InfoCount].DpSize = GetDevicePathSize(Instance);
    InfoCount++;
  }

  *pInfo = Info;
  *pInfoCount = InfoCount;
}



EFI_STATUS
UpdateSerialConsoleVariable(
  PLAT_HOST_INFO_PROTOCOL *HostInfo
  ) 
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  VOID                            *VarConsole;  
  UINTN                           ConoutInfoCount = 0;
  UART_DP_INFO                    *ConoutInfo = NULL;
  UINTN                           ConInInfoCount = 0;
  UART_DP_INFO                    *ConInInfo = NULL;  
  UINTN                           UcrInfoCount = 0;
  UART_DP_INFO                    *UcrInfo = NULL;
  UINTN                           Index;
  EFI_DEVICE_PATH_PROTOCOL        *TargetDp = NULL;
  EFI_DEVICE_PATH_PROTOCOL        *tmpDp;
  UINT16                          UartIoBase = 0;
  EFI_DEVICE_PATH_PROTOCOL        *UcrDp;
  

  DEBUG((EFI_D_INFO, "UpdateSerialConsoleVariable\n"));

  if(HostInfo->GetPlatUcrDp != NULL){
    UcrDp = HostInfo->GetPlatUcrDp(NULL, &UartIoBase);
    DEBUG((EFI_D_INFO, "PlatUcrDp "));
    ShowDevicePathDxe(gBS, UcrDp);
  } else {
    UcrDp = NULL;
  }

  Status = GetEfiGlobalVariable2(L"ConOut", (VOID**)&VarConsole, NULL);
  if(!EFI_ERROR(Status)){
    DEBUG((EFI_D_INFO, "ConOut "));
    ShowDevicePathDxe(gBS, (EFI_DEVICE_PATH_PROTOCOL*)VarConsole);
    UartDpInfoInit(VarConsole, &ConoutInfo, &ConoutInfoCount);
    FreePool(VarConsole);
  }
    
  Status = GetEfiGlobalVariable2(L"ConIn", (VOID**)&VarConsole, NULL);
  if(!EFI_ERROR(Status)){
    DEBUG((EFI_D_INFO, "ConIn "));    
    ShowDevicePathDxe(gBS, (EFI_DEVICE_PATH_PROTOCOL*)VarConsole);    
    UartDpInfoInit(VarConsole, &ConInInfo, &ConInInfoCount);
    FreePool(VarConsole);    
  }

  UartDpInfoInit(UcrDp, &UcrInfo, &UcrInfoCount);


  SyncUartInfo(UcrInfo, UcrInfoCount, ConoutInfo, ConoutInfoCount);
  for(Index=0;Index<ConoutInfoCount;Index++){
    if(ConoutInfo[Index].Uid == 0xFFFFFFFF || ConoutInfo[Index].Match){
      tmpDp = AppendDevicePathInstance(TargetDp, ConoutInfo[Index].Dp);
      if(TargetDp != NULL){
        FreePool(TargetDp);
      }
      TargetDp = tmpDp;
    }
  }
  for(Index=0;Index<UcrInfoCount;Index++){
    if(!UcrInfo[Index].Match){
      tmpDp = AppendDevicePathInstance(TargetDp, UcrInfo[Index].Dp);
      if(TargetDp != NULL){
        FreePool(TargetDp);
      }
      TargetDp = tmpDp;
    }
  }
  DEBUG((EFI_D_INFO, "Target ConOut "));
  ShowDevicePathDxe(gBS, TargetDp);
  Status = gRT->SetVariable(
                  L"ConOut", 
                  &gEfiGlobalVariableGuid, 
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  GetDevicePathSize(TargetDp),
                  TargetDp
                  );

  if(TargetDp != NULL){
    FreePool(TargetDp);
    TargetDp = NULL;
  }
  ClearUartInfoMatchStatus(UcrInfo, UcrInfoCount);
  SyncUartInfo(UcrInfo, UcrInfoCount, ConInInfo, ConInInfoCount);
  for(Index=0;Index<ConInInfoCount;Index++){
    if(ConInInfo[Index].Uid == 0xFFFFFFFF || ConInInfo[Index].Match){
      tmpDp = AppendDevicePathInstance(TargetDp, ConInInfo[Index].Dp);
      if(TargetDp != NULL){
        FreePool(TargetDp);
      }
      TargetDp = tmpDp;
    }
  }
  for(Index=0;Index<UcrInfoCount;Index++){
    if(!UcrInfo[Index].Match){
      tmpDp = AppendDevicePathInstance(TargetDp, UcrInfo[Index].Dp);
      if(TargetDp != NULL){
        FreePool(TargetDp);
      }
      TargetDp = tmpDp;
    }
  }
  DEBUG((EFI_D_INFO, "Target ConIn "));
  ShowDevicePathDxe(gBS, TargetDp);  
  Status = gRT->SetVariable(
                  L"ConIn", 
                  &gEfiGlobalVariableGuid, 
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  GetDevicePathSize(TargetDp),
                  TargetDp
                  );


  FreeUartDpInfo(UcrInfo, UcrInfoCount);
  FreeUartDpInfo(ConoutInfo, ConoutInfoCount);
  FreeUartDpInfo(ConInInfo, ConInInfoCount);

  if(UartIoBase != 0){
    UpdateCsm16UCR(UartIoBase);
  }
  if(TargetDp != NULL){
    FreePool(TargetDp);
  }
  return Status;
}



VOID
ExitPmAuth (
  VOID
  )
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));  

  //
  // Inform the SMM infrastructure that we're entering BDS and may run 3rd party code hereafter 
  // NOTE: We can NOT put it to PlatformBdsInit, because many boot script touch PCI BAR. :-(
  //       We have to connect PCI root bridge, allocate resource, then ExitPmAuth().
  //
  SignalProtocolEvent(gBS, &gExitPmAuthProtocolGuid, FALSE);

  EfiEventGroupSignal(&gEfiEndOfDxeEventGroupGuid);
  
  // //
  // // NOTE: We need install DxeSmmReadyToLock directly here because many boot script is added via ExitPmAuth callback.
  // // If we install them at same callback, these boot script will be rejected because BootScript Driver runs first to lock them done.
  // // So we seperate them to be 2 different events, ExitPmAuth is last chance to let platform add boot script. DxeSmmReadyToLock will
  // // make boot script save driver lock down the interface.
  // //
  SignalProtocolEvent(gBS, &gEfiDxeSmmReadyToLockProtocolGuid, FALSE);

}

STATIC
VOID
DeleteShellBootOption (
  VOID
  )
{
  EFI_BOOT_MANAGER_LOAD_OPTION  *NvBootOptions;
  UINTN                         NvBootOptionCount;
  UINTN                         Index;

  NvBootOptions = EfiBootManagerGetLoadOptions (&NvBootOptionCount, LoadOptionTypeBoot);
  for (Index = 0; Index < NvBootOptionCount; Index++) {
    if (StrCmp(NvBootOptions[Index].Description, SHELL_BOOT_STRING) == 0) {
      EfiBootManagerDeleteLoadOptionVariable (NvBootOptions[Index].OptionNumber, LoadOptionTypeBoot);
      break;
    }
  }
}

STATIC
VOID 
AddShellBootOption(
  VOID
  )
{
  DEBUG((EFI_D_INFO, "AddShellBootOption\n"));

  if ((PcdGet32(PcdSystemMiscConfig) & SYS_MISC_CFG_SHELL_EN) != 0) {   
    RegisterFvFileBootOption (
      (EFI_GUID*)PcdGetPtr(PcdShellFile),
      SHELL_BOOT_STRING,
      TRUE,
      NULL,
      0
      );
  } else {
    DeleteShellBootOption ();
  }
}

STATIC
VOID
HandleTcgPhysicalPresence (
  VOID
  )
{
  EFI_GUID  *TpmInstanceGuid;
  BOOLEAN   NeedConfirm = FALSE;
  BOOLEAN   IsTpm12 = FALSE;
  BOOLEAN   IsTpm2  = FALSE;
  

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  TpmInstanceGuid = (EFI_GUID *)PcdGetPtr(PcdTpmInstanceGuid);
  if (CompareGuid(TpmInstanceGuid, &gEfiTpmDeviceInstanceTpm20DtpmGuid) || 
      CompareGuid(TpmInstanceGuid, &gEfiTpmDeviceInstanceTpm20HgfTpmGuid)){
    DEBUG((EFI_D_INFO, "TPM2.0\n"));
    IsTpm2 = TRUE;
    NeedConfirm = Tcg2PhysicalPresenceLibNeedUserConfirm();
  } else if (CompareGuid(TpmInstanceGuid, &gEfiTpmDeviceInstanceTpm12Guid)){
    DEBUG((EFI_D_INFO, "TPM1.2\n"));
    IsTpm12 = TRUE;
    NeedConfirm = TcgPhysicalPresenceLibNeedUserConfirm();
  }

  if(NeedConfirm){
    EfiBootManagerConnectAllDefaultConsoles ();
  }

  if(IsTpm2){
    Tcg2PhysicalPresenceLibProcessRequest(NULL); 
  }
  if(IsTpm12){
    TcgPhysicalPresenceLibProcessRequest();
  }
}





VOID
EFIAPI
PlatPromptHotKeyAppEnter (
    IN CONST EFI_BOOT_MANAGER_LOAD_OPTION *BootOption
  )
{
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut = gST->ConOut;
  CHAR16                            *Str = NULL;
  CHAR16                            *Str2 = NULL;  
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *FvDp;
  EFI_DEVICE_PATH_PROTOCOL          *Dp;
  CHAR16                            Buffer[65];


  Dp = BootOption->FilePath;
  while (!IsDevicePathEnd(Dp)) {
    if (DevicePathType(Dp) == MEDIA_DEVICE_PATH && DevicePathSubType(Dp) == MEDIA_PIWG_FW_FILE_DP) {
      FvDp = (MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*)Dp;
      if(CompareGuid(&FvDp->FvFileName, (EFI_GUID*)PcdGetPtr(PcdBootManagerMenuFile))){
        Str = HiiGetString(gStringPlatformBmHandle, STRING_TOKEN(STR_SETUP), NULL);
      } else if(CompareGuid(&FvDp->FvFileName, &gBootMenuFileGuid)){
        Str = HiiGetString(gStringPlatformBmHandle, STRING_TOKEN(STR_BOOT_MENU), NULL);
      } else if(CompareGuid(&FvDp->FvFileName, &gPxeBootAppGuid)){
        Str = HiiGetString(gStringPlatformBmHandle, STRING_TOKEN(STR_PXE_BOOT), NULL);
      } else if(CompareGuid(&FvDp->FvFileName, &gOSBackupRestoreFileGuid)){
        Str = HiiGetString(gStringPlatformBmHandle, STRING_TOKEN(STR_OS_BACKUP_RECOVERY), NULL);
      }
      break;
    }
    Dp = NextDevicePathNode(Dp);
  }

  Str2 = HiiGetString(gStringPlatformBmHandle, STRING_TOKEN(STR_ENTER), NULL);
  UnicodeSPrint(Buffer, sizeof(Buffer), L"%s%s...", Str2, Str == NULL ? BootOption->Description : Str);
  if(Str != NULL){
    FreePool(Str);
  }
  FreePool(Str2);

  ConOut->ClearScreen(gST->ConOut);
  ConOut->SetCursorPosition(ConOut, 1, 1);
  ConOut->OutputString(ConOut, Buffer);
  gBS->Stall(1000000);

}


/**
  Do the platform specific action before the console is connected.
  1) Connect PCIRootBridge;
  2) Update console variable:add input/output device to ConIn/ConOut,and update varibale;
  3) Register Shell;
  4) ExitPmAuth(): Especially:signal gEfiEndOfDxeEventGroupGuid && gEfiDxeSmmReadyToLockProtocolGuid
**/
VOID
EFIAPI
PlatformBootManagerBeforeConsole (
  VOID
  )
{
  EFI_STATUS                          Status = EFI_SUCCESS;
  UINTN                               Index;
  PLAT_HOST_INFO_PROTOCOL             *HostInfo;
  EFI_INPUT_KEY                       Enter;

  InvokeHookProtocol(gBS, &gByoBdsEnterHookGuid);

  gBdsWaitTimeOut = PcdGet16(PcdPlatformBootTimeOut);
  gPromptHotKeyAppEnterFunc = PlatPromptHotKeyAppEnter;
  
  //
  // Connect PCIRootBridge;
  //
  ConnectRootBridge();

  //
  // Update console variable
  //
  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, (VOID **)&HostInfo);
  ASSERT(!EFI_ERROR(Status));

  //
  // Update ConIn
  //
  DEBUG((EFI_D_INFO, "ConInDpCount:%d\n", HostInfo->ConInDpCount));
  for (Index = 0; Index < HostInfo->ConInDpCount; Index++) {
    Status = EfiBootManagerUpdateConsoleVariable (ConIn, HostInfo->ConInDp[Index], NULL); // it will add DevicePath of keyboard
    DEBUG((EFI_D_INFO, "UpdateVar_ConIn:%r\n", Status));
  }

  //
  // Connect display device and update ConOut
  //
  Status = UpdateGopDp();
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "UpdateGopDp error\n"));
  }

  //
  // Update Serial console
  //
  UpdateSerialConsoleVariable(HostInfo);

  //
  // Register Shell
  //
  AddShellBootOption();

  Enter.ScanCode    = SCAN_NULL;
  Enter.UnicodeChar = CHAR_CARRIAGE_RETURN;
  Status = EfiBootManagerRegisterContinueKeyOption (0, &Enter, NULL);
  ASSERT_EFI_ERROR (Status);
  
  gRT->SetVariable (
       EFI_CAPSULE_VARIABLE_NAME,
       &gEfiCapsuleVendorGuid,
       EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
       0,
       NULL
       );
  HandleTcgPhysicalPresence();

  //
  // ExitPmAuth and signal gEfiEndOfDxeEventGroupGuid && gEfiDxeSmmReadyToLockProtocolGuid
  //
  InvokeHookProtocol(gBS, &gPlatBeforeExitPmAuthProtocolGuid);

  PERF_START(NULL, "ExitPmAuth", "BDS", 0); 
  ExitPmAuth();
  PERF_END(NULL, "ExitPmAuth", "BDS", 0);

  EfiBootManagerDispatchDeferredImages (); // Load OptionRom of PCIE Device
}

VOID
EFIAPI
ByoPrintPostString (
  IN UINTN     Row,
  IN UINTN     Column,
  IN CHAR16    *String,
  IN UINTN     Attribute
  )
{
  if(String == NULL){
    return;
  }

  if(Attribute != 0){
    gST->ConOut->SetAttribute (gST->ConOut, Attribute);
  }
  gST->ConOut->SetCursorPosition (gST->ConOut, Column, Row);
  gST->ConOut->OutputString (gST->ConOut, String);
}

STATIC
VOID
PrintBootPrompt (
  VOID
  )
{
  UINTN                     Index;
  POST_HOT_KEY_CTX          *Ctx;
  UINTN                     MyStrLen;
  EFI_KEY_DATA              KeyData;
  
  for (Index = 0; Index < gPostHotKeyCount; Index++) {
    Ctx = &gPostHotKeyCtx[Index];

    if (Ctx->Attribute & (HOTKEY_ATTRIBUTE_NO_PROMPT | HOTKEY_ATTRIBUTE_DISABLED)) {
      continue;
    }
    if (Ctx->ScrPrompt == NULL) {
      continue;
    }

    if ((Ctx->Attribute & HOTKEY_ATTRIBUTE_OEM_DRAW) && Ctx->Handler != NULL) {
      ZeroMem(&KeyData, sizeof(EFI_KEY_DATA));
      KeyData.Key.ScanCode = 0xFFFF;
      KeyData.Key.UnicodeChar = 0;
      Ctx->Handler(&KeyData);
    } else {
      ByoPrintPostString(gEraseCountLine++, 2, Ctx->ScrPrompt, 0);
    }

    MyStrLen = StrLen(Ctx->ScrPrompt);
    if (gMaxStrLen < MyStrLen){
      gMaxStrLen = MyStrLen;
    }
  }

}

STATIC
VOID
CleanBootPrompt (
  VOID
  )
{
  UINT8                         Row;
  CHAR16                        *DummyStr = NULL;
  EFI_KEY_DATA                  KeyData;
  UINTN                         Index;
  POST_HOT_KEY_CTX              *Ctx;

  DEBUG((EFI_D_INFO, "%a gEraseCountLine:%d\n", __FUNCTION__, gEraseCountLine));

  for (Index = 0; Index < gPostHotKeyCount; Index++) {
    Ctx = &gPostHotKeyCtx[Index];
    if (Ctx->Attribute & (HOTKEY_ATTRIBUTE_NO_PROMPT | HOTKEY_ATTRIBUTE_DISABLED)) {
      continue;
    }    
    if ((Ctx->Attribute & HOTKEY_ATTRIBUTE_OEM_DRAW) && Ctx->Handler != NULL) {
      ZeroMem(&KeyData, sizeof(EFI_KEY_DATA));
      KeyData.Key.ScanCode = 0xFFFF;
      KeyData.Key.UnicodeChar = 1;
      Ctx->Handler(&KeyData); 
    }
  }

  if (gEraseCountLine) {
    DummyStr = AllocatePool((gMaxStrLen+1) * sizeof(CHAR16));
    ASSERT(DummyStr != NULL);
    SetMem16(DummyStr, gMaxStrLen * sizeof(CHAR16), ' ');
    DummyStr[gMaxStrLen] = 0;

    for (Row = 0; Row < gEraseCountLine; Row++) {
      ByoPrintPostString(Row, 2, DummyStr, 0);
    }

    if (DummyStr != NULL) {
      FreePool(DummyStr);
    }
  }

  if (!(gHotKeyAttribute & HOTKEY_ATTRIBUTE_NO_PROGRESS_BAR)) {
    PostUpdateProgress(0);
  }
  gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0);
}



 

/**
  Do the platform specific action after the console is connected.
  1) Enable Logo;
  2) Connect the rest of devices, and SignalAllDriversConnected
  3) Enumerate all possible boot options.
  4) Install Legacy OpRom
  5) Register platform-specific boot options,like F2(Enter Setup)

**/
VOID
EFIAPI
PlatformBootManagerAfterConsole (
  VOID
  )
{
  EFI_STATUS                        Status;
  EFI_BOOT_MANAGER_LOAD_OPTION      *BootOptions;
  UINTN                             BootOptionsCount;
  UINTN                             DataSize;
  UINT64                            OsIndication;
  EFI_BOOT_MODE                     BootMode;
  BYO_SMIFLASH_PROTOCOL             *ByoSmiFlash;
  UINT32                            SystemMiscCfg;
  BOOLEAN                           IsNormalBoot;
  UINT8                             BootModeType = PcdGet8(PcdBiosBootModeType);
  NETWORK_IDENTIFIER_PROTOCOL       *NetworkIdentifierProtocol = NULL;
  UINT8                             *NetWorkId = NULL;
  UINT8                             NetWorkIdLen = 0x0;

  SystemMiscCfg = PcdGet32(PcdSystemMiscConfig);

  InvokeHookProtocol(gBS, &gBdsAfterConnectAllDefConsoleProtocolGuid); // NumLock
  InvokeHookProtocol(gBS, &gPlatAfterConsoleStartProtocolGuid);

  BootMode = GetBootModeHob();

  if(BootMode == BOOT_IN_RECOVERY_MODE){
    InvokeHookProtocol(gBS, &gPlatBdsRecoveryStartProtocolGuid);
    REPORT_STATUS_CODE (EFI_ERROR_CODE, BSC_BIOS_RECOVERY_BDS_START); 
    Status = gBS->LocateProtocol (
                    &gByoSmiFlashProtocolGuid,
                    NULL,
                    (VOID**)&ByoSmiFlash
                    );
    if(!EFI_ERROR(Status)){
      ByoSmiFlash->Recovery(ByoSmiFlash);
    }
    CpuDeadLoop();
  }
  
  //
  // Enable Logo and check keyboard
  //
  if (gST->ConOut!=NULL) {
    gST->ConOut->EnableCursor(gST->ConOut, FALSE);
  }
  if (!(PcdGet32(PcdSystemMiscConfig) & SYS_MISC_CFG_DIS_SHOW_LOGO)) {
    BootLogoEnableLogo();
  }

  if (PcdGetBool(PcdPwdNetworkAuthEnable)) {
    //
    // Connect Network Driver before further connect Secure Storage Device
    //
    ConnectNetworkDriverWithPciIo();
    //
    // For support Power-on authentication with Network, If Bios Boot Mode Type if Legacy, Get NetworkId through ByoLocalNetworkIdProtocol.
    // When it's done, Disconnect Nic Controller and Unload SnpDxe Driver For Legacy PXE boot won't be affected.
    //
    if (BootModeType == BIOS_BOOT_LEGACY_OS || PcdGet8(PcdPxeOpRomLaunchPolicy) != ROM_POLICY_UEFI_FIRST) {
      NetWorkId = AllocateZeroPool(NETWORK_ID_MAX_SIZE);
      ASSERT(NetWorkId != NULL);
      NetWorkIdLen = (UINT8)NETWORK_ID_MAX_SIZE;

      Status = gBS->LocateProtocol (&gByoNetworkIdentifierProtocolGuid, NULL, (VOID **)&NetworkIdentifierProtocol);
      if (Status != EFI_SUCCESS) {
        DEBUG((EFI_D_INFO, "%a  Locate Byo NetworkId Cache Protocol Failed!\n", __FUNCTION__));
      } else {
        Status = NetworkIdentifierProtocol->GetNetworkId (NetWorkId, &NetWorkIdLen);
        DEBUG((EFI_D_INFO, "%a Status : %r %x \n", __FUNCTION__, Status, NetWorkIdLen));
        DumpMem8(NetWorkId,NetWorkIdLen);
      }
      DisconnectNetworkDriverWithPciIo();
      UnloadDriverWithFileGuid(&gDriverSnpDxeGuid);
      ZeroMem(NetWorkId, NetWorkIdLen);
      FreePool(NetWorkId);
      NetWorkId = NULL;
      NetWorkIdLen = 0x0;
    }
  }

  //
  // Connect the rest of devices
  //
  //EfiBootManagerConnectAll ();
  if (!(SystemMiscCfg & SYS_MISC_CFG_CONNECT_ALL)) {
    BmConnectAllDriversToAllControllers ();
    PcdSet32S(PcdSystemMiscConfig, SystemMiscCfg | SYS_MISC_CFG_CONNECT_ALL);
  }

  InvokeHookProtocol(gBS, &gPlatAfterConnectSequenceProtocolGuid);
  //
  // Install Legacy OpRom
  //
  PERF_START(NULL, "LegacyOpRom", "BDS", 0);
  InstallAdditionalOpRom();
  PERF_END(NULL, "LegacyOpRom", "BDS", 0);  
  
  SignalProtocolEvent(gBS, &gBdsAllDriversConnectedProtocolGuid, FALSE);

  BootOptions = EfiBootManagerGetLoadOptions (&BootOptionsCount, LoadOptionTypeBoot);

  //
  // Enumerate all possible boot options.
  //
  if (!(SystemMiscCfg & SYS_MISC_CFG_UPD_BOOTORDER)) {
    EfiBootManagerRefreshAllBootOption ();
    PcdSet32S(PcdSystemMiscConfig, SystemMiscCfg | SYS_MISC_CFG_UPD_BOOTORDER);
  }

  ByoSortBootOptions(BootOptions, BootOptionsCount, NULL, 0, NULL);
  EfiBootManagerFreeLoadOptions(BootOptions, BootOptionsCount);  

  //
  // Check OsIndication to confirm whether we need to register hotkey
  //
  DataSize = sizeof (UINT64);
  Status = gRT->GetVariable (
                  EFI_OS_INDICATIONS_VARIABLE_NAME,
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &DataSize,
                  &OsIndication
                  );
  if (EFI_ERROR(Status)) {
    OsIndication = 0;
  }  

  if (((OsIndication & EFI_OS_INDICATIONS_BOOT_TO_FW_UI) == 0)
    && (BootModeType != BOOT_ON_FLASH_UPDATE)
    && (BootModeType != BOOT_IN_RECOVERY_MODE)) {
    
    //
    // Register platform-specific boot options
    //
    PrepareAndRegisterHotKey();
    IsNormalBoot = TRUE;
  } else {
    IsNormalBoot = FALSE;
  }

  InvokeHookProtocol(gBS, &gPlatAfterConsoleEndProtocolGuid);
  if(IsNormalBoot){
    InvokeHookProtocol(gBS, &gByoBdsWaitHookProtocolGuid);
    PrintBootPrompt();
  }
}







VOID
PostUpdateProgress (
  IN UINTN   Progress
  )
{
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut;
  UINTN                             Columns;
  UINTN                             Rows;
  UINTN                             OldPx, OldPy;
  UINTN                             Attribute;
  CHAR16                            *Buffer = NULL;
  UINTN                             PosX;


  if (Progress > 100) {
    return;
  }

  ConOut = gST->ConOut;
  ConOut->QueryMode(ConOut, ConOut->Mode->Mode, &Columns, &Rows);
  OldPx = ConOut->Mode->CursorColumn;
  OldPy = ConOut->Mode->CursorRow;
  Attribute = ConOut->Mode->Attribute;

  ConOut->SetCursorPosition(ConOut, 2, Rows-1);
  Buffer = AllocateZeroPool((Columns-4+1) * sizeof(CHAR16));
  if(Buffer == NULL){
    return;
  }

  if(Progress == 0){
    SetMem16(Buffer, (Columns-4) * sizeof(CHAR16), L' ');
    ConOut->SetAttribute(ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
    ConOut->OutputString(ConOut, Buffer);
  } else {
    PosX = (Columns-4) * Progress / 100;
    SetMem16(Buffer, PosX * sizeof(CHAR16), L' ');
    Buffer[PosX] = 0;
    ConOut->SetAttribute(ConOut, EFI_BLACK | EFI_BACKGROUND_MAGENTA);
    ConOut->OutputString(ConOut, Buffer);
  }

  FreePool(Buffer);
  ConOut->SetAttribute(ConOut, Attribute);
  ConOut->SetCursorPosition(ConOut, OldPx, OldPy);
}






/**
  This function is called each second during the boot manager waits the timeout.

  @param TimeoutRemain  The remaining timeout.
**/
VOID
EFIAPI
PlatformBootManagerWaitCallback (
  UINT16          TimeoutRemain
  )
{
  CHAR16                    *Str = NULL;
  CHAR16                    StrBuffer[64];
  UINTN                     Columns;
  UINTN                     Rows;

  if(TimeoutRemain == 0xffff){
    return;
  }

  gST->ConOut->QueryMode(gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);
  gST->ConOut->SetCursorPosition(gST->ConOut, 2, Rows-3);
  Str = HiiGetString(gStringPlatformBmHandle, STRING_TOKEN(STR_PRESS_KEY_IN_SECOND), NULL);
  UnicodeSPrint(StrBuffer, sizeof(StrBuffer), Str, TimeoutRemain);
  gST->ConOut->OutputString(gST->ConOut, StrBuffer);
  
  //
  // Show progress according to attribute of hotkey
  //
  if (!(gHotKeyAttribute & HOTKEY_ATTRIBUTE_NO_PROGRESS_BAR) && gBdsWaitTimeOut) {
    PostUpdateProgress((gBdsWaitTimeOut - TimeoutRemain) * 100 / gBdsWaitTimeOut);
  }

  if ((TimeoutRemain == 0)) {
    CleanBootPrompt();
  }

  FreePool(Str);
}

/**
  The function is called when no boot option could be launched,
  including platform recovery options and options pointing to applications
  built into firmware volumes.

  If this function returns, BDS attempts to enter an infinite loop.
**/
VOID
EFIAPI
PlatformBootManagerUnableToBoot (
  VOID
  )
{
  EFI_STATUS                   Status;
//EFI_INPUT_KEY                Key;
  EFI_BOOT_MANAGER_LOAD_OPTION BootManagerMenu;
//UINTN                        Index;

  REPORT_STATUS_CODE(EFI_ERROR_CODE, BSC_BOOT_DEVICE_NOT_FOUND); 
  InvokeHookProtocol(gBS, &gByoNoBootDeviceProtocolGuid);

  //
  // BootManagerMenu doesn't contain the correct information when return status
  // is EFI_NOT_FOUND.
  //
  Status = EfiBootManagerGetBootManagerMenu (&BootManagerMenu);
  if (EFI_ERROR (Status)) {
    return;
  }
  //
  // Normally BdsDxe does not print anything to the system console, but this is
  // a last resort -- the end-user will likely not see any DEBUG messages
  // logged in this situation.
  //
  // AsciiPrint() will NULL-check gST->ConOut internally. We check gST->ConIn
  // here to see if it makes sense to request and wait for a keypress.
  //

#if 0  
  if (gST->ConIn != NULL) {
    AsciiPrint (
      "%a: No bootable option or device was found.\n"
      "%a: Press any key to enter the Boot Manager Menu.\n",
      gEfiCallerBaseName,
      gEfiCallerBaseName
      );
    Status = gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, &Index);
    ASSERT_EFI_ERROR (Status);
    ASSERT (Index == 0);

    //
    // Drain any queued keys.
    //
    while (!EFI_ERROR (gST->ConIn->ReadKeyStroke (gST->ConIn, &Key))) {
      //
      // just throw away Key
      //
    }
  }
#endif

  for (;;) {
    EfiBootManagerBoot (&BootManagerMenu);
  }	
}

EFI_STATUS
EFIAPI
PlatformBmConstructor (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
) 
{
  gStringPlatformBmHandle = HiiAddPackages (
                            &gEfiCallerIdGuid,
                            ImageHandle,
                            STRING_ARRAY_NAME,
                            NULL
                            );
  ASSERT (gStringPlatformBmHandle!= NULL);
  return EFI_SUCCESS;
}
