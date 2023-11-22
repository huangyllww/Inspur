#include "PlatformDxe.h"

CHAR16                        *gPromptString;
UINTN                         gPromptStringLen;

EFI_STATUS
PrintHideOpromMsgPrompt (
  IN  UINT32      ScreenLine,
  IN  CHAR8       *Format,
  ...
  )
{
  EFI_STATUS                     Status;
  CHAR16                         Buffer[0x100];
  VA_LIST                        Marker;
  UINTN                          RightColumn;
  UINTN                          BottomRow;
  STATIC UINT32                  Row = 0;
  UINTN                          Index, IndexRow;
  UINTN                          PrintHeight;
  UINTN                          PrintX, PrintY;
  CHAR16                         Space[0x100];
  INT32                          SavedAttribute;
  INT32                          SavedCursorColumn;
  INT32                          SavedCursorRow;
  BOOLEAN                        SavedCursorVisible;

  //
  // Convert the DEBUG() message to a Unicode String
  //
  SetMem (&Buffer[0], sizeof (Buffer), 0x0);
  VA_START (Marker, Format);
  UnicodeVSPrintAsciiFormat (Buffer, sizeof(Buffer),  Format, Marker);
  VA_END (Marker);

  SavedAttribute     = gST->ConOut->Mode->Attribute;
  SavedCursorColumn  = gST->ConOut->Mode->CursorColumn;
  SavedCursorRow     = gST->ConOut->Mode->CursorRow;
  SavedCursorVisible = gST->ConOut->Mode->CursorVisible;

  gST->ConOut->SetAttribute (gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  //
  // Print on right and bottom of screen.
  //
  RightColumn = 0;
  BottomRow = 0;
  Status= gST->ConOut->QueryMode (
                 gST->ConOut,
                 gST->ConOut->Mode->Mode,
                 &RightColumn,
                 &BottomRow
                 );
  if (EFI_ERROR(Status)) {
    Row = 0;
    return Status;
  }
  if (RightColumn == 0) {
    RightColumn = 100;
  }
  if (BottomRow == 0) {
    BottomRow = 30;
  }


  if (ScreenLine == 0) {
    Row = 0;
  } else if (ScreenLine == 0xFF) {
    Row++;
  } else {
    Row = ScreenLine;
  }

  PrintHeight = 1;
  if (Row >= PrintHeight) {
    Row = 0;
  }
  PrintX = 4;
  PrintY = 0;

  //
  //Clean all background.
  //
  for(Index = 0; Index < RightColumn - PrintX; Index++ ) {
    Space[Index] = L' ';
  }
  Space[Index] = CHAR_NULL;
  
  if (!Row) {
    for(IndexRow = 0; IndexRow < PrintHeight; IndexRow++ ) {
      gST->ConOut->SetCursorPosition(gST->ConOut, PrintX + gPromptStringLen, (PrintY + IndexRow));
      gST->ConOut->EnableCursor(gST->ConOut, FALSE);
      gST->ConOut->OutputString(gST->ConOut, Space);  //Output to all display devices.
    }
  }

  //
  //Print the Debug string.
  //
  if (Buffer[0] != CHAR_NULL) {
    gST->ConOut->SetCursorPosition(gST->ConOut, PrintX, (PrintY + Row));
    gST->ConOut->EnableCursor(gST->ConOut, FALSE);
    gST->ConOut->OutputString(gST->ConOut, Buffer);  //Output to all display devices.
  } else {
    gST->ConOut->SetCursorPosition(gST->ConOut, PrintX, (PrintY + IndexRow));
    gST->ConOut->EnableCursor(gST->ConOut, FALSE);
    gST->ConOut->OutputString(gST->ConOut, Space);  //Output to all display devices.
  }

  gST->ConOut->SetAttribute (gST->ConOut, SavedAttribute);
  gST->ConOut->SetCursorPosition(gST->ConOut, SavedCursorColumn, SavedCursorRow);
  gST->ConOut->EnableCursor(gST->ConOut, SavedCursorVisible);
  return EFI_SUCCESS;
}

EFI_STATUS
HideOpromMsgPrompt (
  )
{
  STATIC UINTN  WarnCount = 0;
  UINTN         Mode;

  Mode = WarnCount % 3;

  switch (Mode) {
    case 0:
      PrintHideOpromMsgPrompt (0xFF, "%s .  ", gPromptString);
      break;

    case 1:
      PrintHideOpromMsgPrompt (0xFF, "%s .. ", gPromptString);
      break;

    case 2:
      PrintHideOpromMsgPrompt (0xFF, "%s ...", gPromptString);
      break;

    default:
      PrintHideOpromMsgPrompt (0xFF, "%s .  ", gPromptString);
      break;
  }

  WarnCount ++;
  return EFI_SUCCESS; 
}

VOID
ShowHideOpromMsg (
  )
{
  EFI_STATUS  Status;

  if (GetBootModeHob() == BOOT_IN_RECOVERY_MODE) {
    return;
  }

  gPromptStringLen = 0;
  gPromptString = HiiGetString (gHiiHandle, STRING_TOKEN (STR_HIDE_LEGACY_OPROM_INFO_PROMPT), NULL);
  gPromptStringLen = StrLen(gPromptString);

  Status = HideOpromMsgPrompt ();

  return;
}


VOID
CleanHideOpromMsg (
  VOID
  )
{

  gPromptStringLen = 0;
  PrintHideOpromMsgPrompt (0xFF, "", NULL);

  return;
}
