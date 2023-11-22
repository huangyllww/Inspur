/** @file
  Display backup and recovery.

Copyright (c) 2016 - 2022, Byosoft Corporation. All rights reserved.<BR>
This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.


**/

#include "DisplayBackupRecovery.h"

EFI_DRIVER_BINDING_PROTOCOL  gDisplayBackupRecoveryDriverBinding = {
  DisplayBackupRecoveryDriverBindingSupported,
  DisplayBackupRecoveryDriverBindingStart,
  DisplayBackupRecoveryDriverBindingStop,
  0xa,
  NULL,
  NULL
};

static EFI_HANDLE                      mDeviceHandle;
static EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *mSerialTextOut = NULL;
static TERMINAL_DATA_BACKUP_PROTOCOL   *mTerminalDataBackup = NULL;

/**
  Display Backup.

  @param This              Return Display Data

  @retval EFI_SUCCESS      The backup successfully.
  @retval Other            The backup fail.
**/
EFI_STATUS
EFIAPI 
DisplayBackup( 
  OUT BACKUP_RECOVER_DATA **Data
  )
{
  EFI_STATUS                            Status;
  BACKUP_RECOVER_DATA                   *Display;
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *GraphicsOutput = NULL;
  UINTN                                 DataSize;
  UINTN                                 Column;
  UINTN                                 Row;
  UINTN                                 SerialColumn;
  UINTN                                 SerialRow;

  ASSERT(Data != NULL);

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **) &GraphicsOutput);
  if (EFI_ERROR (Status)) {
    GraphicsOutput = NULL;
  }

  if (GraphicsOutput == NULL && (mSerialTextOut == NULL || mTerminalDataBackup == NULL)) {
    return EFI_UNSUPPORTED;
  }

  Status = gST->ConOut->QueryMode(gST->ConOut, gST->ConOut->Mode->Mode, &Column, &Row);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (mSerialTextOut != NULL && mTerminalDataBackup != NULL) {
    Status = mSerialTextOut->QueryMode(mSerialTextOut, mSerialTextOut->Mode->Mode, &SerialColumn, &SerialRow);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    ASSERT (Column == SerialColumn && Row == SerialRow);
  }

  //
  // Allocate Data Buffer
  //
  DataSize = sizeof(BACKUP_RECOVER_DATA);
  if (GraphicsOutput != NULL) {
    DataSize += (GraphicsOutput->Mode->Info->HorizontalResolution * GraphicsOutput->Mode->Info->VerticalResolution * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  }

  if (mSerialTextOut != NULL && mTerminalDataBackup != NULL) {
    DataSize += (Column * Row * sizeof(CHAR16)) + (Column * Row * sizeof(UINTN));
  }

  Display = AllocatePool (DataSize);
  ASSERT(Display != NULL);
  if (GraphicsOutput != NULL) {
    Display->GopData = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)((UINT8*)Display + sizeof(BACKUP_RECOVER_DATA));
  } else {
    Display->GopData = NULL;
  }

  if (mSerialTextOut != NULL && mTerminalDataBackup != NULL) {
    if (GraphicsOutput != NULL) {
      Display->SerialPortStringData = (CHAR16*)((UINT8*)Display->GopData + (GraphicsOutput->Mode->Info->HorizontalResolution * GraphicsOutput->Mode->Info->VerticalResolution * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)));
    } else {
      Display->SerialPortStringData = (CHAR16*)((UINT8*)Display + sizeof(BACKUP_RECOVER_DATA));
    }
    Display->SerialPortAttributeData = (UINTN*)((UINT8*)Display->SerialPortStringData + (Column * Row * sizeof(CHAR16)));
  } else {
    Display->SerialPortStringData = NULL;
    Display->SerialPortAttributeData = NULL;
  }

  //
  // ConOut Backup
  //
  CopyMem (&Display->ConOutMode, gST->ConOut->Mode, sizeof(EFI_SIMPLE_TEXT_OUTPUT_MODE));
  Display->ConOutColumn = Column;
  Display->ConOutRow = Row;

  //
  // Gop Backup
  //
  if (GraphicsOutput != NULL) {
    CopyMem (&Display->GopMode, GraphicsOutput->Mode, sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE));
    CopyMem (&Display->GopInfo, GraphicsOutput->Mode->Info, sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION));
    Display->GopMode.Info = &Display->GopInfo;
    Status = GraphicsOutput->Blt (
                              GraphicsOutput,
                              Display->GopData,
                              EfiBltVideoToBltBuffer,
                              0,
                              0,
                              0,
                              0,
                              GraphicsOutput->Mode->Info->HorizontalResolution,
                              GraphicsOutput->Mode->Info->VerticalResolution,
                              0
                              );
    if (EFI_ERROR (Status)) {
      FreePool(Display);
      return Status;
    }
  }

  if (mSerialTextOut != NULL && mTerminalDataBackup != NULL) {
    Status = mTerminalDataBackup->GetData(mTerminalDataBackup, 0, 0, 0, 0, &Display->SerialPortStringData, &Display->SerialPortAttributeData);
    if (EFI_ERROR (Status)) {
      FreePool(Display);
      return Status;
    }
  }

  *Data = Display;
  return EFI_SUCCESS;
}


static
EFI_STATUS
GopSetMode(
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *GraphicsOutput,
  UINT32                       HorizontalResolution,
  UINT32                       VerticalResolution
  )
{
  EFI_STATUS                            Status;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info;
  UINT32                                Index;
  UINTN                                 SizeOfInfo;

  for (Index = 0; Index < GraphicsOutput->Mode->MaxMode; Index++) {

    Status = GraphicsOutput->QueryMode(GraphicsOutput, Index, &SizeOfInfo, &Info);
    if (EFI_ERROR(Status)) {
      continue;
    }

    if (Info->HorizontalResolution == HorizontalResolution &&
        Info->VerticalResolution == VerticalResolution) {

      FreePool(Info);
      return GraphicsOutput->SetMode(GraphicsOutput, Index);
    }
    FreePool(Info);
  }

  return RETURN_UNSUPPORTED;
}


static
EFI_STATUS
RecoverGopMode (
  UINT32                       HorizontalResolution,
  UINT32                       VerticalResolution
  )
{
  EFI_STATUS Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *GraphicsOutput;

  //
  // Console Gop
  //
  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **) &GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (GraphicsOutput->Mode->Info->HorizontalResolution != HorizontalResolution ||
      GraphicsOutput->Mode->Info->VerticalResolution   != VerticalResolution) {
      return GopSetMode(GraphicsOutput, HorizontalResolution, VerticalResolution);
  }

  return EFI_SUCCESS;
}

static
EFI_STATUS
SimpleTextSetMode (
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut,
  UINTN Column,
  UINTN Row
  )
{
  INT32                             Index;
  EFI_STATUS                        Status;
  UINTN                             OutColumn;
  UINTN                             OutRow;

  for (Index = 0; Index < ConOut->Mode->MaxMode; Index++) {

    Status = ConOut->QueryMode(ConOut, ConOut->Mode->Mode, &OutColumn, &OutRow);
    if (EFI_ERROR(Status)) {
      continue;
    }

    if (OutColumn != Column || OutRow != Row) {
      continue;
    }
    
    return ConOut->SetMode(ConOut, Index);
  }

  return EFI_SUCCESS;
}

static
EFI_STATUS
RecoverSimpleTextMode (
  UINTN Column,
  UINTN Row
  )
{
  EFI_STATUS                        Status;
  UINTN                             OutColumn;
  UINTN                             OutRow;

  //
  // gST->ConOut
  //
  Status = gST->ConOut->QueryMode(gST->ConOut, gST->ConOut->Mode->Mode, &OutColumn, &OutRow);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (OutColumn != Column || OutRow != Row) {
    return SimpleTextSetMode(gST->ConOut, Column, Row);
  }

  return EFI_SUCCESS;
}


/**
  Display Recover.

  @param This              The pointer to this protocol instance.

  @retval EFI_SUCCESS      The recover successfully.
  @retval Other            The recover fail.
**/
EFI_STATUS
EFIAPI 
DisplayRecoverEx(
  IN BACKUP_RECOVER_DATA *Data,
  IN UINT32              Attribute
  )
{
  EFI_STATUS                            Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *GraphicsOutput;
  UINTN                                 Column;
  UINTN                                 Row;
  UINTN                                 OldAttribute;
  UINTN                                 CurAttribute;
  CHAR16                                *StringBuffer;
  UINTN                                 StringLen;


  ASSERT(Data != NULL);

  //
  // Get Display info
  //
  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **) &GraphicsOutput);
  if (EFI_ERROR (Status)) {
    GraphicsOutput = NULL;
  }

  if (GraphicsOutput == NULL && (mSerialTextOut == NULL || mTerminalDataBackup == NULL)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Gop Mode Recover
  //
  if (GraphicsOutput != NULL && Data->GopData != NULL){
    Status = RecoverGopMode(Data->GopMode.Info->HorizontalResolution, Data->GopMode.Info->VerticalResolution);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  //
  // ConOut Mode Recover
  //
  Status = RecoverSimpleTextMode(Data->ConOutColumn, Data->ConOutRow);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // ConOut Recover
  //
  if (gST->ConOut->Mode->Mode != Data->ConOutMode.Mode) {
    gST->ConOut->SetMode(gST->ConOut, Data->ConOutMode.Mode);
  }
  gST->ConOut->EnableCursor(gST->ConOut, Data->ConOutMode.CursorVisible);
  gST->ConOut->SetAttribute(gST->ConOut, Data->ConOutMode.Attribute);
  gST->ConOut->SetCursorPosition(gST->ConOut, Data->ConOutMode.CursorColumn, Data->ConOutMode.CursorRow);

  //
  // Gop Recover
  //
  if (GraphicsOutput != NULL && Data->GopData != NULL){
    if ((Attribute & BYO_DISPLAY_RECOVER_ATTRIBUTE_VIDEO) == BYO_DISPLAY_RECOVER_ATTRIBUTE_VIDEO) {
      Status = GraphicsOutput->Blt (
                                GraphicsOutput,
                                Data->GopData,
                                EfiBltBufferToVideo,
                                0,
                                0,
                                0,
                                0,
                                GraphicsOutput->Mode->Info->HorizontalResolution,
                                GraphicsOutput->Mode->Info->VerticalResolution,
                                0
                                );
    }
  }

  if (((Attribute & BYO_DISPLAY_RECOVER_ATTRIBUTE_SERIAL_PORT) == BYO_DISPLAY_RECOVER_ATTRIBUTE_SERIAL_PORT) &&
      (Data->SerialPortStringData != NULL) &&
      (Data->SerialPortAttributeData != NULL) &&
      (mSerialTextOut != NULL) && 
      (mTerminalDataBackup != NULL)) {
      
      StringBuffer = AllocatePool(sizeof(CHAR16) * (Data->ConOutColumn + 1));
      for (Row = 0; Row < Data->ConOutRow; Row++) {

        OldAttribute     = Data->SerialPortAttributeData[Row * Data->ConOutColumn];
        StringBuffer[0]  = Data->SerialPortStringData[Row * Data->ConOutColumn];
        StringLen        = 1;

        mSerialTextOut->SetCursorPosition(mSerialTextOut, 0, Row);
        for (Column = 1; Column < Data->ConOutColumn; Column++) {
          if(!Data->SerialPortStringData[Row * Data->ConOutColumn + Column]) {
            continue;
          }
          CurAttribute = Data->SerialPortAttributeData[Row * Data->ConOutColumn + Column];
          if (CurAttribute != OldAttribute) {
            StringBuffer[StringLen] = 0;
            mSerialTextOut->SetAttribute(mSerialTextOut, OldAttribute);
            mSerialTextOut->OutputString(mSerialTextOut, StringBuffer);

            StringLen                 = 0;
            OldAttribute              = CurAttribute;
            StringBuffer[StringLen++] = Data->SerialPortStringData[Row * Data->ConOutColumn + Column];
          } else {
            StringBuffer[StringLen++] = Data->SerialPortStringData[Row * Data->ConOutColumn + Column];
          }

          if (Column == (Data->ConOutColumn - 1)) {
            StringBuffer[StringLen] = 0;
            mSerialTextOut->SetAttribute(mSerialTextOut, OldAttribute);
            mSerialTextOut->OutputString(mSerialTextOut, StringBuffer);
          }
        }
      }
      FreePool(StringBuffer);
  }

  return Status;
}

EFI_STATUS
EFIAPI 
DisplayRecover(
  IN BACKUP_RECOVER_DATA *Data
  )
{
  return DisplayRecoverEx(Data, BYO_DISPLAY_RECOVER_ATTRIBUTE_ALL);
}

EFI_STATUS
EFIAPI
DisplayBackupRecoveryDriverBindingSupported (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   ControllerHandle,
  IN  EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath  OPTIONAL
  )
{
  EFI_STATUS Status;

  if (mTerminalDataBackup != NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gTerminalDataBackupProtocolGuid,
                  NULL,
                  This->DriverBindingHandle,
                  mDeviceHandle,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiConsoleOutDeviceGuid,
                  NULL,
                  This->DriverBindingHandle,
                  mDeviceHandle,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
DisplayBackupRecoveryDriverBindingStart (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   ControllerHandle,
  IN  EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
{
  EFI_STATUS Status;

  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiSimpleTextOutProtocolGuid,
                  (VOID **)&mSerialTextOut,
                  This->DriverBindingHandle,
                  mDeviceHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR(Status)) {
    goto Exit;
  }

  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gTerminalDataBackupProtocolGuid,
                  (VOID **)&mTerminalDataBackup,
                  This->DriverBindingHandle,
                  mDeviceHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR(Status)) {
    goto Exit;
  }

Exit:
  if (EFI_ERROR(Status)) {
    if (mTerminalDataBackup != NULL) {
      gBS->CloseProtocol (
              ControllerHandle,
              &gTerminalDataBackupProtocolGuid,
              This->DriverBindingHandle,
              mDeviceHandle
              );
    }
    mSerialTextOut = NULL;
    mTerminalDataBackup = NULL;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
DisplayBackupRecoveryDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   ControllerHandle,
  IN  UINTN                        NumberOfChildren,
  IN  EFI_HANDLE                   *ChildHandleBuffer
  )
{
  gBS->CloseProtocol (
          ControllerHandle,
          &gTerminalDataBackupProtocolGuid,
          This->DriverBindingHandle,
          mDeviceHandle
          );

  mSerialTextOut = NULL;
  mTerminalDataBackup = NULL;

  return EFI_SUCCESS;
}

/**
  Entrypoint of this module.

  This function is the entrypoint of this module. It installs the Edkii
  Platform Logo protocol.

  @param  ImageHandle       The firmware allocated handle for the EFI image.
  @param  SystemTable       A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
InitializeDisplayBackupRecovery (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
{
  EFI_STATUS                           Status;
  BYO_DISPLAY_BACKUP_RECOVER_PROTOCOL  *DisplayBackupRecover;

  mDeviceHandle = NULL;
  DisplayBackupRecover = AllocateZeroPool(sizeof(BYO_DISPLAY_BACKUP_RECOVER_PROTOCOL));
  DisplayBackupRecover->DispalyBackup  = DisplayBackup;
  DisplayBackupRecover->DispalyRecover = DisplayRecover;
  DisplayBackupRecover->DispalyRecoverEx = DisplayRecoverEx;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mDeviceHandle,
                  &gDisplayBackupRecoverProtocolGuid, DisplayBackupRecover,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &gDisplayBackupRecoveryDriverBinding,
             ImageHandle,
             NULL,
             NULL
             );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
