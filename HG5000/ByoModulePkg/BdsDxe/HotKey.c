

#include "Bds.h"
#include <Protocol/PlatHostInfoProtocol.h>
#include <Library/LogoLib.h>
#include <Protocol/BootLogo.h>
#include <Library/HiiLib.h>
#include <Library/TimerLib.h>


STATIC POST_HOT_KEY_CTX gDefaultPostHotKeyCtx[] = {
  {
    {SCAN_F2, CHAR_NULL},
    NULL,
    0,
    HOTKEY_BOOT_SETUP,
    NULL,
    &gSetupFileGuid,
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


POST_HOT_KEY_CTX    *gPostHotKeyCtx  = gDefaultPostHotKeyCtx;
UINTN               gPostHotKeyCount = sizeof(gDefaultPostHotKeyCtx)/sizeof(gDefaultPostHotKeyCtx[0]);
STATIC UINT8        gEraseCountLine  = 0;
STATIC UINTN        gMaxStrLen       = 32;
STATIC UINTN        gHotkeyBootType  = HOTKEY_BOOT_NONE;
STATIC EFI_KEY_DATA gHotKeyData;
STATIC UINTN        gHotKeyAttribute = 0;
STATIC EFI_EVENT    gHotKeyTimerEvent = NULL;

/**
  Function waits for a given event to fire, or for an optional timeout to expire.

  @param   Event              The event to wait for
  @param   Timeout            An optional timeout value in 100 ns units.

  @retval  EFI_SUCCESS      Event fired before Timeout expired.
  @retval  EFI_TIME_OUT     Timout expired before Event fired..

**/
STATIC
EFI_STATUS
BdsWaitForSingleEvent (
  IN  EFI_EVENT                  Event,
  IN  UINT64                     Timeout
  )
{
  UINTN       Index;
  EFI_STATUS  Status;
  EFI_EVENT   TimerEvent;
  EFI_EVENT   WaitList[2];


  if (Timeout != 0) {
    //
    // Create a timer event
    //
    Status = gBS->CreateEvent(EVT_TIMER, 0, NULL, NULL, &TimerEvent);
    if (!EFI_ERROR (Status)) {
      gBS->SetTimer (
             TimerEvent,
             TimerRelative,
             Timeout
             );
      WaitList[0] = Event;
      WaitList[1] = TimerEvent;
      Status = gBS->WaitForEvent(2, WaitList, &Index);
      ASSERT_EFI_ERROR(Status);
      gBS->CloseEvent (TimerEvent);
      if (Index == 1) {
        Status = EFI_TIMEOUT;
      }
    }
  } else {
    //
    // No timeout... just wait on the event
    //
    Status = gBS->WaitForEvent (1, &Event, &Index);
    ASSERT (!EFI_ERROR (Status));
    ASSERT (Index == 0);
  }

  return Status;
}



/**
  The function reads user inputs.

**/
STATIC
VOID
BdsReadKeys (
  VOID
  )
{
  EFI_STATUS         Status;
  EFI_INPUT_KEY      Key;

  while (gST->ConIn != NULL) {
    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);

    if (EFI_ERROR (Status)) {
      //
      // No more keys.
      //
      break;
    }
  }

}




STATIC
VOID
EFIAPI
PlatformBootManagerWaitCallback (
  UINT16  Timeout,  
  UINT16  TimeoutRemain
  )
{
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Foreground;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Background;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
  EFI_STATUS                    Status;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *TxtInEx;
  EFI_KEY_DATA                  KeyData;
  BOOLEAN                       PausePressed;

    //
    // Show progress
    //
    SetMem (&Foreground, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0xff);
    SetMem (&Background, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0x0);
    SetMem (&Color,      sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0xff);

    ShowProgress (
        Foreground,
        Background,
        NULL,
        Color,
        (Timeout - TimeoutRemain) * 100 / Timeout,
        0
        );

  //
  // Pause on PAUSE key
  //
  Status = gBS->HandleProtocol (gST->ConsoleInHandle, &gEfiSimpleTextInputExProtocolGuid, (VOID **) &TxtInEx);
  ASSERT_EFI_ERROR (Status);

  PausePressed = FALSE;

  while (TRUE) {
    Status = TxtInEx->ReadKeyStrokeEx (TxtInEx, &KeyData);
    if (EFI_ERROR (Status)) {
      break;
    }

    if (KeyData.Key.ScanCode == SCAN_PAUSE) {
      PausePressed = TRUE;
      break;
    }
  }

  //
  // Loop until non-PAUSE key pressed
  //
  while (PausePressed) {
    Status = TxtInEx->ReadKeyStrokeEx (TxtInEx, &KeyData);
    if (!EFI_ERROR (Status)) {
      DEBUG ((
        EFI_D_INFO, "[PauseCallback] %x/%x %x/%x\n",
        KeyData.Key.ScanCode, KeyData.Key.UnicodeChar, 
        KeyData.KeyState.KeyShiftState, KeyData.KeyState.KeyToggleState
        ));
      PausePressed = (BOOLEAN) (KeyData.Key.ScanCode == SCAN_PAUSE);
    }
  }
}
 

STATIC BOOLEAN IsPlatformHotKeyPressed()
{
  return (gHotkeyBootType != HOTKEY_BOOT_NONE);
}


STATIC
VOID
BdsWait (
  IN EFI_EVENT      HotkeyTriggered,
  IN UINT32         TotalTimeout
  )
{
  EFI_STATUS                Status;
  UINT32                    TimeoutRemain;
  UINT16                    Seconds;
  UINT16                    LastSeconds = 0xFFFF;
  CHAR16                    StrBuffer[64];
  PLAT_HOST_INFO_PROTOCOL   *HostInfo;
  EFI_DEVICE_PATH_PROTOCOL  *Dp;
  BOOLEAN                   UcrEnable = TRUE; 
  BOOLEAN                   LineAdd = FALSE;
  UINTN                     SecondLine = 0;
  CHAR16                    *Str = NULL;

  
  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &HostInfo);
  if(HostInfo->GetPlatUcrDp == NULL){
    UcrEnable = FALSE;
  }
  if(UcrEnable){
    Dp = HostInfo->GetPlatUcrDp(NULL, NULL);
    if(Dp == NULL){
      UcrEnable = FALSE;
    } else {
      UcrEnable = TRUE;
    }
  }

  TotalTimeout = PcdGet16(PcdPostPromptTimeOut);
  if(UcrEnable && TotalTimeout < 5){
    TotalTimeout = 5;
  }

  TotalTimeout = TotalTimeout * 10;
  if(TotalTimeout == 0){
    TotalTimeout = 5;                       // give min 0.5s delay
  }

  TimeoutRemain = TotalTimeout;
  Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_PRESS_KEY_IN_SECOND), NULL);

  while (TimeoutRemain != 0) {

 //if(!(gHotKeyAttribute & HOTKEY_ATTRIBUTE_NO_PROGRESS_BAR)){
      PlatformBootManagerWaitCallback(TotalTimeout, TimeoutRemain);
 //}
    
    BdsReadKeys();
    if(IsPlatformHotKeyPressed()){
        break;
    }
  
    // Can be removed after all keyboard drivers invoke callback in timer callback.
    Status = BdsWaitForSingleEvent(HotkeyTriggered, 1000000);
    if (!EFI_ERROR (Status)) {
      break;
    }

    if (TotalTimeout != 255 * 10) {
      TimeoutRemain--;

      if(!(gHotKeyAttribute & HOTKEY_ATTRIBUTE_NO_TIMEOUT_PROMPT)){
        Seconds = TimeoutRemain/10;
        if(TimeoutRemain%10){
          Seconds++;
        }

        if(!LineAdd){
          SecondLine = gEraseCountLine + 1;
          gEraseCountLine += 2;
          LineAdd = TRUE;
        }                   // empty line

        if(LastSeconds == Seconds){
          continue;
        } else {
          LastSeconds = Seconds;
        }
        
        ByoPrintPostString(SecondLine, 2, L"                                               ", 0);
        UnicodeSPrint(StrBuffer, sizeof(StrBuffer), Str, Seconds);
        ByoPrintPostString(SecondLine, 2, StrBuffer, 0);
      }
    }
  }

  FreePool(Str);
}



STATIC
VOID
PrintBootPrompt (
  VOID
  )
{
  UINTN             Index;
  POST_HOT_KEY_CTX  *Ctx;
  UINTN             MyStrLen;
  EFI_KEY_DATA      KeyData;
  

  for(Index=0;Index<gPostHotKeyCount;Index++){
    Ctx = &gPostHotKeyCtx[Index];

    if(Ctx->Attribute & (HOTKEY_ATTRIBUTE_NO_PROMPT | HOTKEY_ATTRIBUTE_DISABLED)){
      continue;
    }
    if(Ctx->ScrPrompt == NULL){
      continue;
    }

    if((Ctx->Attribute & HOTKEY_ATTRIBUTE_OEM_DRAW) && Ctx->Handler != NULL){
      ZeroMem(&KeyData, sizeof(EFI_KEY_DATA));
      KeyData.Key.ScanCode = 0xFFFF;
      KeyData.Key.UnicodeChar = 0;
      Ctx->Handler(&KeyData);
    } else {
      ByoPrintPostString(gEraseCountLine++, 2, Ctx->ScrPrompt, 0);
    }

    MyStrLen = StrLen(Ctx->ScrPrompt) + 2;
    if(gMaxStrLen < MyStrLen){gMaxStrLen = MyStrLen;}
  }

}



STATIC
VOID
CleanBootPrompt (
  VOID
  )
{
  UINT8                         Row;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Foreground;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Background;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
  CHAR16                        *DummyStr = NULL;
  EFI_KEY_DATA                  KeyData;
  UINTN                         Index;
  POST_HOT_KEY_CTX              *Ctx;


  DEBUG((EFI_D_INFO, "%a gEraseCountLine:%d\n", __FUNCTION__, gEraseCountLine));

  for(Index=0;Index<gPostHotKeyCount;Index++){
    Ctx = &gPostHotKeyCtx[Index];
    if(Ctx->Attribute & (HOTKEY_ATTRIBUTE_NO_PROMPT | HOTKEY_ATTRIBUTE_DISABLED)){
      continue;
    }    
    if((Ctx->Attribute & HOTKEY_ATTRIBUTE_OEM_DRAW) && Ctx->Handler != NULL){
      ZeroMem(&KeyData, sizeof(EFI_KEY_DATA));
      KeyData.Key.ScanCode = 0xFFFF;
      KeyData.Key.UnicodeChar = 1;
      Ctx->Handler(&KeyData); 
    }
  }

  if(gEraseCountLine){
    DummyStr = AllocatePool((gMaxStrLen+1) * sizeof(CHAR16));
    ASSERT(DummyStr != NULL);
    SetMem16(DummyStr, gMaxStrLen * sizeof(CHAR16), ' ');
    DummyStr[gMaxStrLen] = 0;


    for (Row = 0; Row < gEraseCountLine; Row++) {
      ByoPrintPostString(Row, 2, DummyStr, 0);
    }

    if(DummyStr != NULL){
      FreePool(DummyStr);
    }
  }

  if(!(gHotKeyAttribute & HOTKEY_ATTRIBUTE_NO_PROGRESS_BAR)){
    SetMem (&Foreground, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0xff);
    SetMem (&Background, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0x0);
    SetMem (&Color,      sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0xff);
    ShowProgress (
      Foreground,
      Background,
      L"                                                               ",
      Color,
      0,
      0
      );
  }
  gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0);

}







STATIC EFI_STATUS KeyNotifyCallBack(EFI_KEY_DATA *KeyData)
{
  UINTN             Index;
  POST_HOT_KEY_CTX  *Ctx;

  if(IsPlatformHotKeyPressed()){
    goto ProcExit;
  }

  for(Index=0;Index<gPostHotKeyCount;Index++){
    Ctx = &gPostHotKeyCtx[Index];
    if(Ctx->Attribute & HOTKEY_ATTRIBUTE_DISABLED){
      continue;
    }    
    if(CompareMem(&KeyData->Key, &Ctx->Key, sizeof(EFI_INPUT_KEY)) == 0){
      if(Ctx->IsKeyMatch != NULL && !Ctx->IsKeyMatch(KeyData)){
        continue;
      }
      gHotkeyBootType = Ctx->BootType;
      CopyMem(&gHotKeyData, KeyData, sizeof(EFI_KEY_DATA));
      break;
    }
  }

ProcExit:
  return EFI_SUCCESS;
}



STATIC VOID RegisterPlatformHotKey()
{
  EFI_STATUS                         Status;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *TextInEx;
  EFI_KEY_DATA                       KeyData;
  UINTN                              Index;
  POST_HOT_KEY_CTX                   *Ctx;


  Status = gBS->HandleProtocol(gST->ConsoleInHandle, &gEfiSimpleTextInputExProtocolGuid, (VOID**)&TextInEx);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  ZeroMem(&KeyData, sizeof(KeyData));

  for(Index=0;Index<gPostHotKeyCount;Index++){
    Ctx = &gPostHotKeyCtx[Index];

    gHotKeyAttribute |= Ctx->Attribute & HOTKEY_ATTRIBUTE_GLOBAL_MASK;

    if((Ctx->Key.ScanCode == SCAN_NULL && Ctx->Key.UnicodeChar == CHAR_NULL) || Ctx->BootType == HOTKEY_BOOT_NONE){
      Ctx->Attribute |= HOTKEY_ATTRIBUTE_DISABLED;
    } 
    if(Ctx->Attribute & HOTKEY_ATTRIBUTE_DISABLED){
      continue;
    }
    CopyMem(&KeyData, &Ctx->Key, sizeof(EFI_INPUT_KEY));
    Status = TextInEx->RegisterKeyNotify(TextInEx, &KeyData, KeyNotifyCallBack, &Ctx->RegHandle);
    if(EFI_ERROR(Status)){
      DEBUG((EFI_D_ERROR, "HotKey[%d] Reg:%r\n", Index, Status));
    }  
  }        

  gBS->CreateEvent (EVT_TIMER, 0, NULL, NULL, &gHotKeyTimerEvent);
  gBS->SetTimer(gHotKeyTimerEvent, TimerRelative, 5000000);

ProcExit:
  return;
}





VOID BdsHotKeyBoot()
{
  EFI_BOOT_MANAGER_LOAD_OPTION  BootOption;
  UINTN                         Index;
  POST_HOT_KEY_CTX              *Ctx;
  CHAR16                        *Desc = L"";

  
  for(Index=0;Index<gPostHotKeyCount;Index++){
    Ctx = &gPostHotKeyCtx[Index];
    if(Ctx->Attribute & HOTKEY_ATTRIBUTE_DISABLED){
      continue;
    }    
    if(gHotkeyBootType == Ctx->BootType){

      if(Ctx->Attribute & HOTKEY_ATTRIBUTE_OEM_HANDLER){
        ASSERT(Ctx->Handler != NULL);
        SetBootLogoInvalid(gBS);
        Ctx->Handler(&gHotKeyData);
        
      } else if(Ctx->FileName!=NULL){
        if(Ctx->BootOptionName != NULL){
          Desc = Ctx->BootOptionName;
        }
       if(gHotkeyBootType == HOTKEY_BOOT_OEM2){
          Ctx->Handler(&gHotKeyData);
        }
        SetBootLogoInvalid(gBS);
        CreateFvBootOption(Ctx->FileName, Desc, &BootOption, FALSE, NULL, 0);
        ByoEfiBootManagerBoot(&BootOption);
        EfiBootManagerFreeLoadOption(&BootOption);
      }
      
      break;
    }
  }
  
}


STATIC VOID UnregisterPlatfromHotKey()
{
  UINTN                              Index;
  EFI_STATUS                         Status;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *TextInEx;

  Status = gBS->HandleProtocol(gST->ConsoleInHandle, &gEfiSimpleTextInputExProtocolGuid, (VOID**)&TextInEx);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  for(Index=0;Index<gPostHotKeyCount;Index++){
    if(gPostHotKeyCtx[Index].RegHandle != NULL){
        Status = TextInEx->UnregisterKeyNotify(TextInEx, gPostHotKeyCtx[Index].RegHandle);
      if(EFI_ERROR(Status)){      
        DEBUG((EFI_D_ERROR, "HotKey[%d] UnReg:%r\n", Index, Status));
      }  
    }  
  }

ProcExit:
  return;
}


VOID BdsPrepareHotKey()
{
  PLAT_HOST_INFO_PROTOCOL         *PlatHostInfo;
  EFI_STATUS                      Status;

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &PlatHostInfo);
  if(!EFI_ERROR(Status) && PlatHostInfo->HotKey != NULL && PlatHostInfo->HotKeyCount != 0){
    gPostHotKeyCtx  = PlatHostInfo->HotKey;
    gPostHotKeyCount = PlatHostInfo->HotKeyCount;
  }
}


VOID BdsHandleHotKey(  
  EFI_BOOT_MODE                   BootMode,
  EFI_EVENT                       HotkeyTriggered
  )
{
  UINT32    PromptTimeOut;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if(BootMode != BOOT_ON_FLASH_UPDATE && BootMode != BOOT_IN_RECOVERY_MODE){

    PromptTimeOut = PcdGet16(PcdPostPromptTimeOut);
    RegisterPlatformHotKey();
    BdsReadKeys();    

    if(PromptTimeOut){
      StopWarningPrompt((VOID*)PcdGet64(PcdWaringPrintEvent));
      PrintBootPrompt();
      InvokeHookProtocol(gBS, &gByoBdsWaitHookProtocolGuid);
      PERF_START (NULL, "BdsWait", "BDS", 0);
      BdsWait(HotkeyTriggered, PromptTimeOut);
      PERF_END   (NULL, "BdsWait", "BDS", 0);
      CleanBootPrompt();
    }
  }
}



VOID BdsStopHotKey()
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if(gHotKeyTimerEvent == NULL){
    return;
  }

  while(1){
    if(IsPlatformHotKeyPressed()){
      break;
    }
    if(gBS->CheckEvent(gHotKeyTimerEvent) == EFI_SUCCESS){
      break;
    }
    MicroSecondDelay(5000);
  }

  gBS->CloseEvent(gHotKeyTimerEvent);
  gHotKeyTimerEvent = NULL;
  
  UnregisterPlatfromHotKey();
}





UINTN ByoEfiBootManagerGetCurHotKey()
{
  return gHotkeyBootType;
}

VOID  ByoEfiBootManagerSetCurHotKey(UINTN HotKey)
{
  gHotkeyBootType = HotKey;
}
