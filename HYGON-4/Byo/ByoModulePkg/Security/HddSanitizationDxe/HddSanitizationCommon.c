/** @file

Copyright (c) 2013, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  HddSanitizationCommon.c

Abstract:

Revision History:

**/
#include "HddSanitizationDxe.h"
#include "HddSanitizationCommon.h"

HDP_HII_VENDOR_DEVICE_PATH  mHddSanitizationHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    HDD_SANITIZE_METHOD_CONFIG_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

HDD_SANITIZE_DEVICE_ENTRY *
HddSanitizationGetConfigFormEntryByIndex (
    IN UINTN Index
  )
{
  LIST_ENTRY                     *Entry;
  UINTN                          CurrentIndex;
  HDD_SANITIZE_DEVICE_ENTRY      *HddSanDevEntry;

  CurrentIndex   = 0;
  HddSanDevEntry = NULL;

  EFI_LIST_FOR_EACH (Entry, &mHddSanitizationCfgFormList) {
    if (CurrentIndex == Index) {
      HddSanDevEntry = BASE_CR (Entry, HDD_SANITIZE_DEVICE_ENTRY, Link);
      break;
    }

    CurrentIndex++;
  }

  return HddSanDevEntry;
}


OPAL_FEATURE_INFO *
GetOpalInfo (
  VOID    *DevInfo
)
{
  ATA_DEVICE_INFO    *AtaInfo;
  NVME_DEVICE_INFO   *NvmeInfo;

  if (DEV_INFO_TYPE(DevInfo) == NVME_DEVICE_INFO_SIGN) {
    NvmeInfo = (NVME_DEVICE_INFO *)DevInfo;
    return NvmeInfo->OpalInfo;
  } else if (DEV_INFO_TYPE(DevInfo) == ATA_DEVICE_INFO_SIGN) {
    AtaInfo  = (ATA_DEVICE_INFO *)DevInfo;
    return AtaInfo->OpalInfo;
  } else {
    ASSERT(FALSE);
    return NULL;
  }

}

/*
  Erase Timer Call Back for Periodically refresh the progress bar.
  Percentages are calculated at one second intervals: Percent = ElapsedTimeInSeconds / EraseUnitTimeOutInSeconds.
*/
VOID 
EFIAPI 
EraseTimerInSecondsCallBack (
  EFI_EVENT           Event, 
  VOID                *Context
  )
{
  TIMER_CALLBACK_CTX   *Ctx;
  UINT64               CurrentPercent;
  UINT64               OldPercent;

  Ctx = (TIMER_CALLBACK_CTX *)Context;

  OldPercent = DivU64x32(MultU64x32(Ctx->ElapsedTimeInSeconds, 100), Ctx->EraseUnitTimeOutInSeconds);
  //
  //elapsed time + interval(1 second)
  //
  Ctx->ElapsedTimeInSeconds++;
  CurrentPercent = DivU64x32(MultU64x32(Ctx->ElapsedTimeInSeconds, 100), Ctx->EraseUnitTimeOutInSeconds);
  //
  //Refresh progress box with integer percentage.
  //
  if (CurrentPercent > OldPercent) {
    if (PcdGetBool (PcdGuiEnable)) {
      ShowGuiProgress(HDD_ERASE_DRAW_TYPE_REFRESH, CurrentPercent, NULL, NULL);
    } else {
      ShowTextProgress(HDD_ERASE_DRAW_TYPE_REFRESH, CurrentPercent, NULL, NULL);
    }
  }

}

VOID
EFIAPI 
ResetSystemNull (
  IN EFI_RESET_TYPE           ResetType,
  IN EFI_STATUS               ResetStatus,
  IN UINTN                    DataSize,
  IN VOID                     *ResetData OPTIONAL
  )
{

}

/**
  Draw Erase Progress Box.

  @param[in] DialogColorIndex   Background color on progress box 
  @param[in] BarColorAttrib     Color's attributes of the progress bar 
  @param[in] Percent            Percentage of erase progress
  @param[in] Title              Prompt message of progress box
**/
VOID
EFIAPI
DrawProgressBox (
  IN  UINTN          DialogColorIndex, 
  IN  UINTN          BarColorAttrib,
  IN  UINT8          Percent,
  IN  CHAR16         *Title,
  ...
  )
{
  VA_LIST                          Args;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *ConOut;
  EFI_SIMPLE_TEXT_OUTPUT_MODE      SavedConsoleMode;
  UINTN                            Columns;
  UINTN                            Rows;
  UINTN                            Column;
  UINTN                            Row;
  UINTN                            NumberOfLines;
  UINTN                            MaxLength;
  CHAR16                           *String;
  UINTN                            Length;
  CHAR16                           *Line;
  CHAR16                           *TmpString;
  CHAR16                           NumberStr[6+1];
  UINTN                            NumberStrLen;
  UINTN                            PercentPos;
  UINTN                            ColorAttrib;	

  
  DEBUG((EFI_D_INFO, "%a(%d)\n", __FUNCTION__, Percent));   
  
  if(Percent > 100){Percent = 100;}

  ColorAttrib = EFI_LIGHTBLUE|EFI_BACKGROUND_BLUE;
  if(DialogColorIndex == 1){
    ColorAttrib = EFI_WHITE|EFI_BACKGROUND_BLUE;
  }
	
  VA_START(Args, Title);
  MaxLength = 0;
  NumberOfLines = 0;
  while((String = VA_ARG (Args, CHAR16*)) != NULL) {
    MaxLength = MAX(MaxLength, UefiLibGetStringWidth(String, FALSE, 0, NULL) / 2);
    NumberOfLines++;
  }
  VA_END(Args);
  ASSERT(NumberOfLines != 0);
  ASSERT(MaxLength != 0);
  if(MaxLength < 40){MaxLength = 40;}

  ConOut = gST->ConOut;
  CopyMem(&SavedConsoleMode, ConOut->Mode, sizeof(SavedConsoleMode));
  ConOut->QueryMode(ConOut, SavedConsoleMode.Mode, &Columns, &Rows);
  ConOut->EnableCursor(ConOut, FALSE);
  ConOut->SetAttribute(ConOut, ColorAttrib);
  NumberOfLines = MIN(NumberOfLines, Rows - 8);
  MaxLength = MIN (MaxLength, Columns - 2);
  Row    = (Rows - (NumberOfLines + 8)) / 2;
  Column = (Columns - (MaxLength + 2)) / 2;
  Line = AllocateZeroPool((MaxLength + 3) * sizeof(CHAR16));
  ASSERT(Line != NULL);
  
  SetMem16(Line, (MaxLength + 2) * 2, BOXDRAW_HORIZONTAL);
  Line[0]             = BOXDRAW_DOWN_RIGHT;
  Line[MaxLength + 1] = BOXDRAW_DOWN_LEFT;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row++);
  ConOut->OutputString(ConOut, Line);

  SetMem16(Line, (MaxLength + 2) * 2, L' ');
  Line[0]             = BOXDRAW_VERTICAL;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row);
  Length = UefiLibGetStringWidth(Title, FALSE, 0, NULL) / 2;  
  CopyMem(Line + 1 + (MaxLength - Length) / 2, Title, StrLen(Title)*sizeof(Title[0]));
  ConOut->OutputString(ConOut, Line);
  Row++;
  
  SetMem16(Line, (MaxLength + 2) * 2, BOXDRAW_HORIZONTAL);
  Line[0]             = BOXDRAW_VERTICAL_RIGHT;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL_LEFT;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row++);
  ConOut->OutputString(ConOut, Line);  
  
  SetMem16(Line, (MaxLength + 2) * 2, L' ');
  Line[0]             = BOXDRAW_VERTICAL;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row++);
  ConOut->OutputString(ConOut, Line);  
  
  VA_START (Args, Title);
  while ((String = VA_ARG(Args, CHAR16 *)) != NULL && NumberOfLines > 0) {
    SetMem16(Line, (MaxLength + 2) * 2, L' ');
    Line[0]             = BOXDRAW_VERTICAL;
    Line[MaxLength + 1] = BOXDRAW_VERTICAL;
    Line[MaxLength + 2] = L'\0';
    ConOut->SetCursorPosition(ConOut, Column, Row);
    ConOut->OutputString(ConOut, Line);
    
    Length = UefiLibGetStringWidth(String, FALSE, 0, NULL) / 2;
    if (Length <= MaxLength) {
      ConOut->SetCursorPosition (ConOut, Column + 1 + (MaxLength - Length) / 2, Row++);
      ConOut->OutputString (ConOut, String);
    } else {
      UefiLibGetStringWidth (String, TRUE, MaxLength, &Length);
      TmpString = AllocateZeroPool ((Length + 1) * sizeof (CHAR16));
      ASSERT (TmpString != NULL);
      StrnCpyS(TmpString, MaxLength, String, Length - 3);
      StrCatS(TmpString, MaxLength, L"...");
      ConOut->SetCursorPosition (ConOut, Column + 1, Row++);
      ConOut->OutputString (ConOut, TmpString);
      FreePool(TmpString);
    }
    NumberOfLines--;
  }
  VA_END(Args);

  SetMem16(Line, (MaxLength + 2) * 2, L' ');
  Line[0]             = BOXDRAW_VERTICAL;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row);
  ConOut->OutputString(ConOut, Line);  
  Row++;  
  
  UnicodeSPrint(NumberStr, sizeof(NumberStr), L" %d%% ", Percent);	
  NumberStrLen = StrLen(NumberStr);
//DEBUG((EFI_D_INFO, "NumberStr:[%s], Len:%d, MaxLength:%d\n", NumberStr, NumberStrLen, MaxLength));
  SetMem16(Line, (MaxLength + 2) * 2, BOXDRAW_HORIZONTAL);
  Line[0]             = BOXDRAW_VERTICAL_RIGHT;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL_LEFT;
  Line[MaxLength + 2] = L'\0';
  CopyMem(Line + 1 + (MaxLength - NumberStrLen) / 2, NumberStr, NumberStrLen*sizeof(NumberStr[0]));
  ConOut->SetCursorPosition(ConOut, Column, Row);
  ConOut->OutputString(ConOut, Line);   
  Row++;
  
  SetMem16(Line, (MaxLength + 2) * 2, L' ');
  Line[0]             = BOXDRAW_VERTICAL;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row);
  ConOut->OutputString(ConOut, Line);  
  
  PercentPos = MaxLength*Percent/100;
  ConOut->SetAttribute(ConOut, BarColorAttrib);  
  SetMem16(Line, PercentPos*2, L' ');
  Line[PercentPos] = 0;
  ConOut->SetCursorPosition(ConOut, Column+1, Row);
  ConOut->OutputString(ConOut, Line);
//DEBUG((EFI_D_INFO, "PercentPos:%d, Row:%d, Column:%d Line:[%s]\n", PercentPos, Row, Column, Line));  
  Row++;
 
  ConOut->SetAttribute(ConOut, ColorAttrib);
  SetMem16(Line, (MaxLength + 2) * 2, BOXDRAW_HORIZONTAL);
  Line[0]             = BOXDRAW_UP_RIGHT;
  Line[MaxLength + 1] = BOXDRAW_UP_LEFT;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row);
  ConOut->OutputString(ConOut, Line);
  FreePool(Line);
  
  ConOut->EnableCursor(ConOut, SavedConsoleMode.CursorVisible);
  ConOut->SetCursorPosition(ConOut, SavedConsoleMode.CursorColumn, SavedConsoleMode.CursorRow);
  ConOut->SetAttribute(ConOut, SavedConsoleMode.Attribute);
    
}

/**
  Show Hdd Erase Progress

  @param[in] Type         Draw Type
                          HDD_ERASE_DRAW_TYPE_START   The first drawing
                          HDD_ERASE_DRAW_TYPE_RUN     The updating drawing
                          HDD_ERASE_DRAW_TYPE_END     The end drawing
  @param[in] Rate         Take the whole number without keeping the decimal
  @param[in] Info         Hdd Erase Info String
  @param[in] Context      Hdd Erase Progress Bar Draw Funtion Context

  @retval EFI_SUCCESS     Draw Success
  @retval Other           Draw Error

**/
EFI_STATUS
ShowTextProgress (
  IN UINTN     Type,
  IN UINT64    Rate,
  IN CHAR16    *Info,
  IN VOID      *Context
  )
{
  switch (Type) {
    case HDD_ERASE_DRAW_TYPE_ENTRY:
      BltSaveAndRetore(gBS, TRUE);
      DrawProgressBox(
        1,
        EFI_BLACK|BLT_BACKGROUND_WHITE,
        0,
        gSanitizeWarnigStr,
        gSanitizeNoPowerOffStr[0],
        gSanitizeNoPowerOffStr[1],
        NULL
        );
      break;

    case HDD_ERASE_DRAW_TYPE_EXIT:
      BltSaveAndRetore(gBS, FALSE);
      break;

    case HDD_ERASE_DRAW_TYPE_REFRESH:
      DrawProgressBox(
        1, 
        EFI_BLACK|BLT_BACKGROUND_WHITE,
        (UINT8)Rate,
        gSanitizeWarnigStr, 
        gSanitizeNoPowerOffStr[0], 
        gSanitizeNoPowerOffStr[1],
        NULL
        );
      break;

    default:
      break;
  }
  return EFI_SUCCESS;
}


EFI_STATUS
ShowGuiProgress (
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
    case HDD_ERASE_DRAW_TYPE_ENTRY:
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
                                    FALSE
                                    );
      ASSERT(Windows != NULL);
      ProgressBar = AddDynamicallyUpdatePop (Windows, Info);
      ASSERT(ProgressBar != NULL);

      RefreshDynamicallyUpdatePop(ProgressBar, TRUE, NULL);
      break;

    case HDD_ERASE_DRAW_TYPE_REFRESH:
      if (ProgressBar == NULL) {
        break;
      }
      if (Info != NULL) {
        UnicodeSPrint(RateStr, sizeof(RateStr), L"%s %d %%", Info, Rate);
      } else {
        UnicodeSPrint(RateStr, sizeof(RateStr), L"%d %%", Rate);
      }
      RefreshDynamicallyUpdatePop(ProgressBar, FALSE, RateStr);
      break;

    case HDD_ERASE_DRAW_TYPE_EXIT:
      if (ProgressBar != NULL) {
        DeleteDynamicallyUpdatePop(ProgressBar);
        ProgressBar = NULL;
      }
      if (Windows != NULL) {
        UnRegisterWindows(Windows);
        Windows = NULL;
      }
      break;

    default:
      if (Info != NULL) {
        DEBUG((DEBUG_ERROR, "%a(%d) %s\n", __FILE__, __LINE__, Info));
      }
      break;
  }

  return EFI_SUCCESS;
}