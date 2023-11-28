
#include <PiDxe.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/OEMBadging.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/UgaDraw.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/ByoCommLib.h>
#include <IndustryStandard/Bmp.h>
#include <Protocol/BootLogo.h>
#include <Protocol/JpegDecoder.h>
#include <Library/PrintLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/GlobalVariable.h>
#include <Library/HobLib.h>
#include <Library/LogoLib.h>
#include <SetupVariable.h>

EFI_EVENT gWarningPromptEvent;
CHAR16    *gWarningPromptString;
UINTN    gWarningPromptStringLen;

VOID
CleanHideOpromMsg (
  VOID
  );


EFI_STATUS
DebugSc (
  IN  UINT32      ScLine,
  IN  CHAR8       *Format,
  ...
  )
{
  EFI_STATUS    Status;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Foreground;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Background;
  CHAR16      Buffer[0x100];
  VA_LIST     Marker;
  UINTN   RightColumn;
  UINTN   BottomRow;
  static UINT32      Row = 0;
  UINTN    Index, IndexRow;
  UINTN    PrintHeight;
  UINTN    PrintX, PrintY;
  CHAR16      Space[0x100];

  //
  // Convert the DEBUG() message to a Unicode String
  //
  SetMem (&Buffer[0], sizeof (Buffer), 0x0);
  VA_START (Marker, Format);
  UnicodeVSPrintAsciiFormat (Buffer, sizeof(Buffer),  Format, Marker);
  VA_END (Marker);

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


  if (ScLine == 0) {
    Row = 0;
  } else if (ScLine == 0xFF) {
    Row++;
  } else {
    Row = ScLine;
  }
  
  PrintHeight = 1;
  if (Row >= PrintHeight) {
    Row = 0;
  }
  PrintX = 4;
  PrintY = BottomRow - 3;
 	
  SetMem (&Foreground, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0xff);
  SetMem (&Background, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0x0);

  //
  //Clean all background.
  //
  for(Index = 0; Index < RightColumn -PrintX; Index++ ) {
    Space[Index] = L' ';
  }
  Space[Index] = CHAR_NULL;
  
  if (!Row) {    
    for(IndexRow = 0; IndexRow < PrintHeight; IndexRow++ ) {
      gST->ConOut->SetCursorPosition(gST->ConOut, PrintX + gWarningPromptStringLen, (PrintY + IndexRow));
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

  DEBUG((EFI_D_ERROR, __FUNCTION__"(), ScLine %d-%s.\n", Row, Buffer)); 
  return EFI_SUCCESS;
}

EFI_STATUS
WarningPrompt (
  IN  EFI_EVENT    Event,
  IN  VOID    *Context
  )
{
  static UINTN    WarnCount = 0;
  UINTN    Mode;

  Mode = WarnCount % 3;
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), WarnCount :%d.\n", WarnCount, gWarningPromptString));

  switch (Mode) {
    case 0:
      DebugSc (0xFF, "%s .", gWarningPromptString);
      break;
	  
    case 1:
      DebugSc (0xFF, "%s ..", gWarningPromptString);
      break;
	  
    case 2:
      DebugSc (0xFF, "%s ...", gWarningPromptString);
      break;
	  
    default:
      DebugSc (0xFF, "%s .", gWarningPromptString);
      break;
  }
  
  WarnCount ++;
  return EFI_SUCCESS; 
}


EFI_EVENT
StartWarningPrompt (
  CHAR16    *PromptString
  )
{
  EFI_STATUS    Status;
  UINTN    Size;
  CHAR16    *String;
  EFI_BOOT_MODE  BootMode;


  BootMode = GetBootModeHob();
  if(BootMode==BOOT_IN_RECOVERY_MODE){
      return NULL;
  }

  gWarningPromptStringLen = 0;
  if (PromptString == NULL) {
    return NULL;
  }
  //
  //Create 1s Timer event.
  //
  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  (EFI_EVENT_NOTIFY) WarningPrompt,
                  NULL,
                  &gWarningPromptEvent
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  Status = gBS->SetTimer(gWarningPromptEvent, TimerPeriodic, 1000 * 10000);
  if (EFI_ERROR (Status)) {
    gBS->CloseEvent(gWarningPromptEvent);
    return NULL;
  }

  gWarningPromptStringLen = StrLen(PromptString);  
  Size = gWarningPromptStringLen * sizeof(CHAR16);
  String = AllocateZeroPool (Size + 2);
  if (!IsEnglishLang()) {
    gWarningPromptStringLen = 2*gWarningPromptStringLen;
  }
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), gWarningPromptStringLen :0x%x.\n", gWarningPromptStringLen));

  if (String != NULL) {
     gWarningPromptString = CopyMem (String, PromptString, Size);
  }  
  if (gWarningPromptString == NULL) {
    gWarningPromptString = L"System will boot soon";
  }
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), PromptString :%s.\n", gWarningPromptString));

  PcdSet64(PcdWaringPrintEvent,(UINT64)gWarningPromptEvent);
  return gWarningPromptEvent;
}


VOID
StopWarningPrompt (
  EFI_EVENT    PromptEvent
  )
{

  if (PromptEvent) {
    gBS->CloseEvent(PromptEvent);
  } else {
    gBS->CloseEvent(gWarningPromptEvent);
  }
  
  if (gWarningPromptString) {
    FreePool (gWarningPromptString);
  }
  gWarningPromptStringLen = 0;
  DebugSc (0xFF, "", NULL);

  return;
}

extern EFI_GUID gByoPostLogoFileGuid;

/**
  Convert a *.BMP graphics image to a GOP blt buffer. If a NULL Blt buffer
  is passed in a GopBlt buffer will be allocated by this routine. If a GopBlt
  buffer is passed in it will be used if it is big enough.

  @param  BmpImage      Pointer to BMP file
  @param  BmpImageSize  Number of bytes in BmpImage
  @param  GopBlt        Buffer containing GOP version of BmpImage.
  @param  GopBltSize    Size of GopBlt in bytes.
  @param  PixelHeight   Height of GopBlt/BmpImage in pixels
  @param  PixelWidth    Width of GopBlt/BmpImage in pixels

  @retval EFI_SUCCESS           GopBlt and GopBltSize are returned.
  @retval EFI_UNSUPPORTED       BmpImage is not a valid *.BMP image
  @retval EFI_BUFFER_TOO_SMALL  The passed in GopBlt buffer is not big enough.
                                GopBltSize will contain the required size.
  @retval EFI_OUT_OF_RESOURCES  No enough buffer to allocate.

**/
EFI_STATUS
ConvertBmpToGopBlt (
  IN     VOID      *BmpImage,
  IN     UINTN     BmpImageSize,
  IN OUT VOID      **GopBlt,
  IN OUT UINTN     *GopBltSize,
     OUT UINTN     *PixelHeight,
     OUT UINTN     *PixelWidth
  )
{
  UINT8                         *Image;
  UINT8                         *ImageHeader;
  BMP_IMAGE_HEADER              *BmpHeader;
  BMP_COLOR_MAP                 *BmpColorMap;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt;
  UINT64                        BltBufferSize;
  UINTN                         Index;
  UINTN                         Height;
  UINTN                         Width;
  UINTN                         ImageIndex;
  BOOLEAN                       IsAllocated;

  BmpHeader = (BMP_IMAGE_HEADER *) BmpImage;

  if (BmpHeader->CharB != 'B' || BmpHeader->CharM != 'M') {
    return EFI_UNSUPPORTED;
  }

  //
  // Doesn't support compress.
  //
  if (BmpHeader->CompressionType != 0) {
    return EFI_UNSUPPORTED;
  }

  //
  // Calculate Color Map offset in the image.
  //
  Image       = BmpImage;
  BmpColorMap = (BMP_COLOR_MAP *) (Image + sizeof (BMP_IMAGE_HEADER));

  //
  // Calculate graphics image data address in the image
  //
  Image         = ((UINT8 *) BmpImage) + BmpHeader->ImageOffset;
  ImageHeader   = Image;

  //
  // Calculate the BltBuffer needed size.
  //
  BltBufferSize = MultU64x32 ((UINT64) BmpHeader->PixelWidth, BmpHeader->PixelHeight);
  //
  // Ensure the BltBufferSize * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) doesn't overflow
  //
  if (BltBufferSize > DivU64x32 ((UINTN) ~0, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL))) {
      return EFI_UNSUPPORTED;
   }
  BltBufferSize = MultU64x32 (BltBufferSize, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

  IsAllocated   = FALSE;
  if (*GopBlt == NULL) {
    //
    // GopBlt is not allocated by caller.
    //
    *GopBltSize = (UINTN) BltBufferSize;
    *GopBlt     = AllocatePool (*GopBltSize);
    IsAllocated = TRUE;
    if (*GopBlt == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  } else {
    //
    // GopBlt has been allocated by caller.
    //
    if (*GopBltSize < (UINTN) BltBufferSize) {
      *GopBltSize = (UINTN) BltBufferSize;
      return EFI_BUFFER_TOO_SMALL;
    }
  }

  *PixelWidth   = BmpHeader->PixelWidth;
  *PixelHeight  = BmpHeader->PixelHeight;

  //
  // Convert image from BMP to Blt buffer format
  //
  BltBuffer = *GopBlt;
  for (Height = 0; Height < BmpHeader->PixelHeight; Height++) {
    Blt = &BltBuffer[(BmpHeader->PixelHeight - Height - 1) * BmpHeader->PixelWidth];
    for (Width = 0; Width < BmpHeader->PixelWidth; Width++, Image++, Blt++) {
      switch (BmpHeader->BitPerPixel) {
      case 1:
        //
        // Convert 1-bit (2 colors) BMP to 24-bit color
        //
        for (Index = 0; Index < 8 && Width < BmpHeader->PixelWidth; Index++) {
          Blt->Red    = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Red;
          Blt->Green  = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Green;
          Blt->Blue   = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Blue;
          Blt++;
          Width++;
        }

        Blt--;
        Width--;
        break;

      case 4:
        //
        // Convert 4-bit (16 colors) BMP Palette to 24-bit color
        //
        Index       = (*Image) >> 4;
        Blt->Red    = BmpColorMap[Index].Red;
        Blt->Green  = BmpColorMap[Index].Green;
        Blt->Blue   = BmpColorMap[Index].Blue;
        if (Width < (BmpHeader->PixelWidth - 1)) {
          Blt++;
          Width++;
          Index       = (*Image) & 0x0f;
          Blt->Red    = BmpColorMap[Index].Red;
          Blt->Green  = BmpColorMap[Index].Green;
          Blt->Blue   = BmpColorMap[Index].Blue;
        }
        break;

      case 8:
        //
        // Convert 8-bit (256 colors) BMP Palette to 24-bit color
        //
        Blt->Red    = BmpColorMap[*Image].Red;
        Blt->Green  = BmpColorMap[*Image].Green;
        Blt->Blue   = BmpColorMap[*Image].Blue;
        break;

      case 24:
        //
        // It is 24-bit BMP.
        //
        Blt->Blue   = *Image++;
        Blt->Green  = *Image++;
        Blt->Red    = *Image;
        break;

      default:
        //
        // Other bit format BMP is not supported.
        //
        if (IsAllocated) {
          FreePool (*GopBlt);
          *GopBlt = NULL;
        }
        return EFI_UNSUPPORTED;
        break;
      };

    }

    ImageIndex = (UINTN) (Image - ImageHeader);
    if ((ImageIndex % 4) != 0) {
      //
      // Bmp Image starts each row on a 32-bit boundary!
      //
      Image = Image + (4 - (ImageIndex % 4));
    }
  }

  return EFI_SUCCESS;
}

/**
  Convert a *.JPG graphics image to a GOP blt buffer. If a NULL Blt buffer
  is passed in a GopBlt buffer will be allocated by this routine. If a GopBlt
  buffer is passed in it will be used if it is big enough.

  @param  JpgImage      Pointer to BMP file
  @param  JpgImageSize  Number of bytes in JpgImage
  @param  GopBlt        Buffer containing GOP version of JPgImage.
  @param  GopBltSize    Size of GopBlt in bytes.
  @param  PixelHeight   Height of GopBlt/JpgImage in pixels
  @param  PixelWidth    Width of GopBlt/JpgImage in pixels

  @retval EFI_SUCCESS           GopBlt and GopBltSize are returned.
  @retval EFI_UNSUPPORTED       JpgImage is not a valid *.JPG image
  @retval EFI_BUFFER_TOO_SMALL  The passed in GopBlt buffer is not big enough.
                                GopBltSize will contain the required size.
  @retval EFI_OUT_OF_RESOURCES  No enough buffer to allocate.

**/
EFI_STATUS
ConvertJpgToGopBlt (
  IN     VOID      *JpgImage,
  IN     UINTN     JpgImageSize,
  IN OUT VOID      **Buffer,
  IN OUT UINTN     *GopBltSize,
     OUT UINTN     *Height,
     OUT UINTN     *Width
  )
{
  EFI_STATUS                Status;
  EFI_JPEG_DECODER_STATUS   JpegDecodeStatus;
  EFI_JPEG_DECODER_PROTOCOL * JpegDecoder ;

  *Buffer = NULL;
  *Width  = 0;
  *Height = 0;
  Status  = gBS->LocateProtocol (&gEfiJpegDecoderProtocolGuid, NULL, (VOID **) &JpegDecoder);
  if(EFI_ERROR( Status )){
    return EFI_UNSUPPORTED;
  }

  Status = JpegDecoder->DecodeImage(
    JpegDecoder,
    JpgImage,
    JpgImageSize,
    (UINT8**)Buffer,
    GopBltSize,
    Height,
    Width,
    &JpegDecodeStatus
    );

  return Status;
}

/**
  Use SystemTable Conout to stop video based Simple Text Out consoles from going
  to the video device. Put up LogoFile on every video device that is a console.

  @param[in]  LogoFile   File name of logo to display on the center of the screen.

  @retval EFI_SUCCESS     ConsoleControl has been flipped to graphics and logo displayed.
  @retval EFI_UNSUPPORTED Logo not found

**/
EFI_STATUS
EFIAPI
ShowPostLogo (
    VOID  
  )
{
  EFI_STATUS                    Status;
  UINT32                        SizeOfX;
  UINT32                        SizeOfY;
  INTN                          DestX;
  INTN                          DestY;
  UINT8                         *ImageData;
  UINTN                         ImageSize;
  UINTN                         Height;
  UINTN                         Width;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt = NULL;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput;
  EFI_BOOT_LOGO_PROTOCOL        *BootLogo;
  UINTN                         BltSize;
  BOOLEAN                       NeedFreeImageData = FALSE;
  UINTN                         PosY;

  SETUP_DATA                    SetupData;
  UINTN    Size;
    
  Size = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &SetupData
                  ); 
  if (!EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, __FUNCTION__"(L %d), HideBrandLogo :%d.\n", __LINE__, SetupData.HideBrandLogo));
    if(SetupData.HideBrandLogo == 1){
       if(IsEnglishLang()){
         StartWarningPrompt (L"Driver Loading");
       }else{
         StartWarningPrompt (L"驱动加载中");
       }
      return EFI_NOT_READY;
    }
  }

  Status = gBS->HandleProtocol(gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID**)&GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  ImageData = (UINT8*)(UINTN)PcdGet32(PcdLogoDataAddress);
  ImageSize = PcdGet32(PcdLogoDataSize);
  
  if(ImageData == NULL || ImageSize == 0 || (*(UINT64*)(ImageData) == 0xFFFFFFFFFFFFFFFF)){
    Status = LibReadFileFromFv(
               gBS, 
               &gByoPostLogoFileGuid,
               EFI_SECTION_RAW,
               PcdGet32(PcdByoLogoInstance),
               &ImageData,
               &ImageSize
               );
    if(EFI_ERROR(Status)){
      goto ProcExit;
    } 
    NeedFreeImageData = TRUE;
    DEBUG((EFI_D_INFO, "Image(%X, %X)\n", ImageData, ImageSize));
  }

  BootLogo = NULL;
  gBS->LocateProtocol(&gEfiBootLogoProtocolGuid, NULL, (VOID**)&BootLogo);

  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  CleanHideOpromMsg();
  SizeOfX = GraphicsOutput->Mode->Info->HorizontalResolution;
  SizeOfY = GraphicsOutput->Mode->Info->VerticalResolution;

  Status = ConvertBmpToGopBlt (
              ImageData,
              ImageSize,
              (VOID**)&Blt,
              &BltSize,
              &Height,
              &Width
              );
  if(EFI_ERROR (Status)) {
    Status = ConvertJpgToGopBlt (
              ImageData,
              ImageSize,
              (VOID**)&Blt,
              &BltSize,
              &Height,
              &Width
              );
  }
  if(EFI_ERROR (Status)) {
    goto ProcExit;
  }

  if(Width > SizeOfX || Height > SizeOfY){
    Status = EFI_UNSUPPORTED;
    goto ProcExit;
  }

  DestX = (SizeOfX - Width) / 2;
  DestY = (SizeOfY - Height) / 2;

  Status = GraphicsOutput->Blt (
                      GraphicsOutput,
                      Blt,
                      EfiBltBufferToVideo,
                      0,
                      0,
                      DestX,
                      DestY,
                      Width,
                      Height,
                      Width * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                      );
  if (!EFI_ERROR (Status)) {
    BootLogo->SetBootLogo (BootLogo, Blt, DestX, DestY, Width, Height);
  }
  
  PosY        = SizeOfY * 48 / 50;
  
  if(IsEnglishLang()){
    StartWarningPrompt (L"Driver Loading");
  }else{
    StartWarningPrompt (L"驱动加载中");
  }


ProcExit:
  if(Blt != NULL){FreePool(Blt);}
  if(NeedFreeImageData){FreePool(ImageData);}
  return Status;
}




/**
  Use SystemTable Conout to turn on video based Simple Text Out consoles. The 
  Simple Text Out screens will now be synced up with all non video output devices

  @retval EFI_SUCCESS     UGA devices are back in text mode and synced up.

**/
EFI_STATUS
EFIAPI
DisableQuietBoot (
  VOID
  )
{

  //
  // Enable Cursor on Screen
  //
  gST->ConOut->EnableCursor (gST->ConOut, TRUE);
  return EFI_SUCCESS;
}


/**

  Show progress bar with title above it. It only works in Graphics mode.


  @param TitleForeground Foreground color for Title.
  @param TitleBackground Background color for Title.
  @param Title           Title above progress bar.
  @param ProgressColor   Progress bar color.
  @param Progress        Progress (0-100)
  @param PreviousValue   The previous value of the progress.

  @retval  EFI_STATUS       Success update the progress bar

**/
EFI_STATUS
EFIAPI
ShowProgress (
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL TitleForeground,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL TitleBackground,
  IN CHAR16                        *Title,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL ProgressColor,
  IN UINTN                         Progress,
  IN UINTN                         PreviousValue
  )
{
  EFI_STATUS                     Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL   *GraphicsOutput;
  UINT32                         SizeOfX;
  UINT32                         SizeOfY;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  Color;
  UINTN                          BlockHeight;
  UINTN                          BlockWidth;
  UINTN                          BlockNum;
  UINTN                          PosX;
  UINTN                          PosY;
  UINTN                          Index;

  if (Progress > 100) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol (
                  gST->ConsoleOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **) &GraphicsOutput
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  SizeOfX = GraphicsOutput->Mode->Info->HorizontalResolution;
  SizeOfY = GraphicsOutput->Mode->Info->VerticalResolution;

  BlockWidth  = SizeOfX / 100;
  BlockHeight = SizeOfY / 50;

  BlockNum    = Progress;

  PosX        = 0;
  PosY        = SizeOfY * 48 / 50;

  if (BlockNum == 0) {
    //
    // Clear progress area
    //
    SetMem (&Color, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0x0);

    if (Title != NULL) {
      Status = GraphicsOutput->Blt (
                        GraphicsOutput,
                        &Color,
                        EfiBltVideoFill,
                        0,
                        0,
                        0,
                        PosY - EFI_GLYPH_HEIGHT - 1,
                        SizeOfX,
                        SizeOfY - (PosY - EFI_GLYPH_HEIGHT - 1),
                        SizeOfX * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                        );
    } else {
      Status = GraphicsOutput->Blt (
                        GraphicsOutput,
                        &Color,
                        EfiBltVideoFill,
                        0,
                        0,
                        0,
                        PosY - 1,
                        SizeOfX,
                        SizeOfY - (PosY - 1),
                        SizeOfX * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                        );
    }
  }
  
  //
  // Show progress by drawing blocks
  //
  for (Index = PreviousValue; Index < BlockNum; Index++) {
    PosX = Index * BlockWidth;
    Status = GraphicsOutput->Blt (
                        GraphicsOutput,
                        &ProgressColor,
                        EfiBltVideoFill,
                        0,
                        0,
                        PosX,
                        PosY,
                        BlockWidth - 1,
                        BlockHeight,
                        (BlockWidth) * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                        );
  }

  if (Title != NULL) {
  PrintXY (
    (SizeOfX - StrLen (Title) * EFI_GLYPH_WIDTH) / 2,
    PosY - EFI_GLYPH_HEIGHT - 1,
    &TitleForeground,
    &TitleBackground,
    Title
    );
  }

  return EFI_SUCCESS;
}

BOOLEAN
IsEnglishLang (
  VOID
  )
{
  EFI_STATUS    Status;
  UINTN    DataSize = 0;
  CHAR8    *Language = NULL;

  Status = gRT->GetVariable (EFI_PLATFORM_LANG_VARIABLE_NAME, &gEfiGlobalVariableGuid, NULL, &DataSize, Language);
  if (!EFI_ERROR(Status)) {
    return FALSE;
  }
  
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Language = AllocatePool(DataSize + 1);
    if (Language == NULL) {
      return FALSE;
    }
    Status = gRT->GetVariable (EFI_PLATFORM_LANG_VARIABLE_NAME, &gEfiGlobalVariableGuid, NULL, &DataSize, Language);
    if (!EFI_ERROR(Status)) {  
      DEBUG ((EFI_D_ERROR, "BeEnglish, Language :%a.\n", Language));
      if (!AsciiStriCmp(Language, "en-US")) {	  	        
        gBS->FreePool(Language); 
        return TRUE;
      }
    }
    gBS->FreePool(Language); 
  }
    
  return FALSE;
}

VOID
CleanHideOpromMsg (
  VOID
  )
{
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Foreground;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Background;

  SetMem (&Foreground, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0xff);
  SetMem (&Background, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0x0);
  
  PrintXY (
          0,
          EFI_GLYPH_HEIGHT,
          &Foreground,
          &Background,
          L"                                                               "
          );
}

VOID
ShowHideOpromMsg (
  VOID
  )
{
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Foreground;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Background;
  CHAR16                            *TmpStr;
  static UINTN                      WarnCount = 0;
  UINTN                             Mode;
  BOOLEAN                           IsEnglish = FALSE;
  UINTN                             PosX = 0;

  if (WarnCount == 0) {
    ShowPostLogo();
    StopWarningPrompt((VOID*)PcdGet64(PcdWaringPrintEvent));
  }

  Mode = WarnCount % 3;

  IsEnglish = IsEnglishLang();
  if (WarnCount == 0) {
    if (IsEnglish) {
      TmpStr = L"Hide loading legacy driver information .";
    } else {
      TmpStr = L"隐藏加载传统驱动程序信息 .";
    }
  } else {
    if(IsEnglish){
      PosX = 39 * EFI_GLYPH_WIDTH;
    } else {
      PosX = (12 * 16 + EFI_GLYPH_WIDTH);
    }
    if (Mode == 0) {
      TmpStr = L".";
    } else if (Mode == 1) {
      TmpStr = L"..";
    } else {
      TmpStr = L"...";
    }
    
  }

  SetMem (&Foreground, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0xff);
  SetMem (&Background, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0x0);

  PrintXY (
            PosX,
            EFI_GLYPH_HEIGHT,
            &Foreground,
            &Background,
            TmpStr
            );

  WarnCount++;
}

