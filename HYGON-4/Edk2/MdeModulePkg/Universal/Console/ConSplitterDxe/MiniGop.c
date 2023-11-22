#include "ConSplitter.h"

EFI_GRAPHICS_OUTPUT_PROTOCOL         mMiniGop;
EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE    mMiniGopMode;
EFI_GRAPHICS_OUTPUT_MODE_INFORMATION mMiniGopInfo;
BOOLEAN                              mMiniGopEnable;
TEXT_OUT_SPLITTER_QUERY_DATA         mMiniModeBuffer[3];

EFI_STATUS
EFIAPI
MiniGopBlt (IN  EFI_GRAPHICS_OUTPUT_PROTOCOL            *This,
            IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL           *BltBuffer,
            IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION       BltOperation,
            IN  UINTN                                   SourceX,
            IN  UINTN                                   SourceY,
            IN  UINTN                                   DestinationX,
            IN  UINTN                                   DestinationY,
            IN  UINTN                                   Width,
            IN  UINTN                                   Height,
            IN  UINTN                                   Delta)
{
  EFI_STATUS Status;
  UINTN      Dx = 0;
  UINTN      Dy = 0;

  if (mMiniGopEnable) {
    Dx = (mConOut.GraphicsOutput.Mode->Info->HorizontalResolution - This->Mode->Info->HorizontalResolution) / 2;
    Dy = (mConOut.GraphicsOutput.Mode->Info->VerticalResolution - This->Mode->Info->VerticalResolution) / 2;
    if (Delta == 0) {
      Delta = Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
    }

    switch (BltOperation) {
      case EfiBltVideoFill:
      case EfiBltBufferToVideo:
        Status = mConOut.GraphicsOutput.Blt (&mConOut.GraphicsOutput,
                                              BltBuffer,
                                              BltOperation,
                                              SourceX,
                                              SourceY,
                                              Dx + DestinationX,
                                              Dy + DestinationY,
                                              Width,
                                              Height,
                                              Delta
                                              );
        break;
      case EfiBltVideoToBltBuffer:
        Status = mConOut.GraphicsOutput.Blt (&mConOut.GraphicsOutput,
                                              BltBuffer,
                                              BltOperation,
                                              Dx + SourceX,
                                              Dy + SourceY,
                                              DestinationX,
                                              DestinationY,
                                              Width,
                                              Height,
                                              Delta
                                              );
        break;
      case EfiBltVideoToVideo:
        Status = mConOut.GraphicsOutput.Blt (&mConOut.GraphicsOutput,
                                              BltBuffer,
                                              BltOperation,
                                              Dx + SourceX,
                                              Dy + SourceY,
                                              Dx + DestinationX,
                                              Dy + DestinationY,
                                              Width,
                                              Height,
                                              Delta
                                              );
        break;
      default:
        Status = RETURN_INVALID_PARAMETER;
        break;
    }
  } else {
    Status = mConOut.GraphicsOutput.Blt (&mConOut.GraphicsOutput,
                                          BltBuffer,
                                          BltOperation,
                                          SourceX,
                                          SourceY,
                                          DestinationX,
                                          DestinationY,
                                          Width,
                                          Height,
                                          Delta
                                          );
  }

  return Status;
}

EFI_STATUS
EFIAPI
MiniGopQueryMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL          *This,
  IN  UINT32                                ModeNumber,
  OUT UINTN                                 *SizeOfInfo,
  OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  **Info
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (mMiniGopEnable) {
    if (ModeNumber != 0) {
      return RETURN_INVALID_PARAMETER;
    }

    *SizeOfInfo = sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);
    *Info = AllocateCopyPool(*SizeOfInfo, &mMiniGopInfo);
  } else {
    Status = mConOut.GraphicsOutput.QueryMode(&mConOut.GraphicsOutput, ModeNumber, SizeOfInfo, Info);
  }

  return Status;
}

EFI_STATUS
EFIAPI
MiniGopSetMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
  IN  UINT32                       ModeNumber
  )
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  UINT32                                NewMode = 0;
  UINT32                                MaxHorizontalResolution = 0;
  UINT32                                MaxVerticalResolution = 0;
  UINT32                                Index;
  UINTN                                 SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info;

  if (mMiniGopEnable) {

    //
    // Set Max Resolution
    //
    for (Index = 0; Index < mConOut.GraphicsOutput.Mode->MaxMode; Index++) {
      Status = mConOut.GraphicsOutput.QueryMode(&mConOut.GraphicsOutput, Index, &SizeOfInfo, &Info);
      if (!EFI_ERROR(Status)) {
        if (Info->HorizontalResolution <= PcdGet32 (PcdRealVideoHorizontalResolution) &&
            Info->VerticalResolution <= PcdGet32 (PcdRealVideoVerticalResolution)) {
          if (MaxHorizontalResolution <= Info->HorizontalResolution && MaxVerticalResolution < Info->VerticalResolution) {
            MaxHorizontalResolution = Info->HorizontalResolution;
            MaxVerticalResolution   = Info->VerticalResolution;
            NewMode = Index;
          }
        }
        FreePool(Info);
      }
    }

    Status = mConOut.GraphicsOutput.SetMode(&mConOut.GraphicsOutput, NewMode);
    mMiniGop.Mode->Mode = ModeNumber;
    ASSERT(MaxHorizontalResolution >= mMiniGop.Mode->Info->HorizontalResolution);
    ASSERT(MaxVerticalResolution >= mMiniGop.Mode->Info->VerticalResolution);
  } else {
    Status = mConOut.GraphicsOutput.SetMode(&mConOut.GraphicsOutput, ModeNumber);
    mMiniGop.Mode = mConOut.GraphicsOutput.Mode;
  }

  return Status;
}

VOID
InitMiniGop (VOID)
{
  if (PcdGet32 (PcdRealVideoHorizontalResolution) == 0 || PcdGet32 (PcdRealVideoVerticalResolution) == 0) {
    mMiniGopEnable = FALSE;
  } else {
    mMiniGopEnable = TRUE;
  }

  //
  // Init Mini Gop
  //
  mMiniGop.SetMode                            = MiniGopSetMode;
  mMiniGop.QueryMode                          = MiniGopQueryMode;
  mMiniGop.Blt                                = MiniGopBlt;
  if (mMiniGopEnable) {
    mMiniGop.Mode                             = &mMiniGopMode;
    mMiniGop.Mode->Mode                       = 0xFF;
    mMiniGop.Mode->Info                       = &mMiniGopInfo;
    mMiniGop.Mode->SizeOfInfo                 = sizeof (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);
    mMiniGop.Mode->MaxMode                    = 1;
    mMiniGop.Mode->FrameBufferBase            = 0;
    mMiniGop.Mode->FrameBufferSize            = 0;
    mMiniGop.Mode->Info->Version              = 0;
    mMiniGop.Mode->Info->HorizontalResolution = PcdGet32 (PcdVideoHorizontalResolution);
    mMiniGop.Mode->Info->VerticalResolution   = PcdGet32 (PcdVideoVerticalResolution);
    mMiniGop.Mode->Info->PixelFormat          = PixelBltOnly;
    ZeroMem (&mMiniGop.Mode->Info->PixelFormat, sizeof (EFI_PIXEL_BITMASK));
    mMiniGop.Mode->Info->PixelsPerScanLine    = mMiniGop.Mode->Info->HorizontalResolution;
  } else {
    mMiniGop.Mode                             = mConOut.GraphicsOutput.Mode;
  }

  if (mMiniGopEnable) {
    MiniGopSetMode(&mMiniGop, 0);
  }
}

