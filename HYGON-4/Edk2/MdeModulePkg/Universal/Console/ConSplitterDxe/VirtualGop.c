/** @file
  This code supports the implementation of the Virtual Gop protocol

Copyright (c) 2009 - 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/VirtualGop.h>
#include "ConSplitter.h"

typedef struct _VIRTUAL_GOP_RECT
{
    UINT32    Left;
    UINT32    Top;
    UINT32    Right;
    UINT32    Bottom;
} VIRTUAL_GOP_RECT;

VIRTUAL_GOP_PROTOCOL            mDMProtocol            = {0};
BOOLEAN                         mBufferToVideoSyncFlag = TRUE;
VIRTUAL_GOP_RECT                mAutoBufferToVideoRect = {0};
BOOLEAN                         mVideoToBufferSyncFlag = TRUE;
VIRTUAL_GOP_RECT                mAutoVideoToBufferRect = {0};
static UINT32                   mHorizontalResolution  = 0;
static UINT32                   mVerticalResolution    = 0;
EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *mVGopBuffer           = NULL;

BOOLEAN
ReSetResolutionRatio(
  IN UINT32 CurHorizontalResolution,
  IN UINT32 CurVerticalResolution
  )
{
  if ((mHorizontalResolution != CurHorizontalResolution) ||
      (mVerticalResolution != CurVerticalResolution) ||
      (mVGopBuffer == NULL)) {

    if (mVGopBuffer != NULL) {
      FreePool(mVGopBuffer);
    }

    mHorizontalResolution = CurHorizontalResolution;
    mVerticalResolution   = CurVerticalResolution;
    mVGopBuffer = AllocateZeroPool(mHorizontalResolution * mVerticalResolution *sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

    return TRUE;
  }

  return FALSE;
}

EFI_STATUS
EFIAPI
VirtualGopBlt (IN  EFI_GRAPHICS_OUTPUT_PROTOCOL            *This,
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
  UINTN                         Index;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *GopBuffer;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *CopyBuffer;
  UINTN                         ImageWidth;

  ReSetResolutionRatio(This->Mode->Info->HorizontalResolution, This->Mode->Info->VerticalResolution);

  if (DestinationX + Width > mHorizontalResolution) {
    Width = mHorizontalResolution - DestinationX;
  }

  if (DestinationY + Height > mVerticalResolution) {
    Height = mVerticalResolution - DestinationY;
  }

  if (!mBufferToVideoSyncFlag) {
    if (mAutoBufferToVideoRect.Left > DestinationX)              mAutoBufferToVideoRect.Left   = (UINT32)(DestinationX);
    if (mAutoBufferToVideoRect.Right < (DestinationX + Width))   mAutoBufferToVideoRect.Right  = (UINT32)(DestinationX + Width);
    if (mAutoBufferToVideoRect.Top > DestinationY)               mAutoBufferToVideoRect.Top    = (UINT32)(DestinationY);
    if (mAutoBufferToVideoRect.Bottom < (DestinationY + Height)) mAutoBufferToVideoRect.Bottom = (UINT32)(DestinationY + Height);
  } else {
    mAutoBufferToVideoRect.Left   = (UINT32)(DestinationX);
    mAutoBufferToVideoRect.Right  = (UINT32)(DestinationX + Width);
    mAutoBufferToVideoRect.Top    = (UINT32)(DestinationY);
    mAutoBufferToVideoRect.Bottom = (UINT32)(DestinationY + Height);
    mBufferToVideoSyncFlag = FALSE;
  }

  GopBuffer = mVGopBuffer;
  ImageWidth = Delta != 0 ? Delta / sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) : Width;

  switch (BltOperation) {
    case EfiBltBufferToVideo:
      for (Index = 0; Index < Height; Index++) {
        CopyMem(GopBuffer + ((DestinationY + Index) * mHorizontalResolution) + DestinationX,
                BltBuffer + ((SourceY + Index) * ImageWidth) + SourceX,
                sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width);
      }
      break;
    case EfiBltVideoFill:
      for (Index = 0; Index < Height; Index++) {
        SetMem32(GopBuffer + ((DestinationY + Index) * mHorizontalResolution) + DestinationX,
                 sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width,
                 *((UINT32*)BltBuffer));
      }
      break;
    case EfiBltVideoToBltBuffer:
      for (Index = 0; Index < Height; Index++) {
        CopyMem(BltBuffer + ((DestinationY + Index) * ImageWidth) + DestinationX,
                GopBuffer + ((SourceY + Index) * mHorizontalResolution) + SourceX,
                sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width);
      }
      break;
    case EfiBltVideoToVideo:
      CopyBuffer = AllocatePool(Width * Height * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
      for (Index = 0; Index < Height; Index++) {
        CopyMem(CopyBuffer + (Index * Width),
                GopBuffer + ((SourceY + Index) * mHorizontalResolution) + SourceX,
                sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width);
      }
      for (Index = 0; Index < Height; Index++) {
        CopyMem(GopBuffer + ((DestinationY + Index) * mHorizontalResolution) + DestinationX,
                CopyBuffer + (Index * Width),
                sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width);
      }
      FreePool(CopyBuffer);
      break;
    default:
      DEBUG((DEBUG_ERROR, "%a(%d) VirtualGopBlt Error. %d\n", __FILE__, __LINE__, BltOperation));
      return EFI_UNSUPPORTED;
      break;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
OverrideVirtualGopBlt (IN  EFI_GRAPHICS_OUTPUT_PROTOCOL            *This,
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
  if (mDMProtocol.IsVirtualGop) {
    return VirtualGopBlt (This,
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
  } else {
    if (!mVideoToBufferSyncFlag) {
      if (mAutoVideoToBufferRect.Left > DestinationX)              mAutoVideoToBufferRect.Left   = (UINT32)(DestinationX);
      if (mAutoVideoToBufferRect.Right < (DestinationX + Width))   mAutoVideoToBufferRect.Right  = (UINT32)(DestinationX + Width);
      if (mAutoVideoToBufferRect.Top > DestinationY)               mAutoVideoToBufferRect.Top    = (UINT32)(DestinationY);
      if (mAutoVideoToBufferRect.Bottom < (DestinationY + Height)) mAutoVideoToBufferRect.Bottom = (UINT32)(DestinationY + Height);
    } else {
      mAutoVideoToBufferRect.Left   = (UINT32)(DestinationX);
      mAutoVideoToBufferRect.Right  = (UINT32)(DestinationX + Width);
      mAutoVideoToBufferRect.Top    = (UINT32)(DestinationY);
      mAutoVideoToBufferRect.Bottom = (UINT32)(DestinationY + Height);
      mVideoToBufferSyncFlag = FALSE;
    }
    return ConSplitterGraphicsOutputBlt (This,
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
}

EFI_STATUS
DMVirtualGopEnable (
  IN  VIRTUAL_GOP_PROTOCOL           *This,
  IN  BOOLEAN                        Enable
  )
{
  This->IsVirtualGop = Enable;
  return EFI_SUCCESS;
}

EFI_STATUS
DMVirtualGopSync(
  IN  VIRTUAL_GOP_PROTOCOL              *This,
  IN  VIRTUAL_GOP_SYNC_TYPE             Type,
  IN  UINT32                            X,       OPTIONAL
  IN  UINT32                            Y,       OPTIONAL
  IN  UINT32                            Width,   OPTIONAL
  IN  UINT32                            Height   OPTIONAL
  )
{
  if (ReSetResolutionRatio(mConOut.GraphicsOutput.Mode->Info->HorizontalResolution, mConOut.GraphicsOutput.Mode->Info->VerticalResolution)) {
    return RETURN_DEVICE_ERROR;
  }

  switch (Type) {
    case VGopAutoBltBufferToVideo:
      if (mBufferToVideoSyncFlag == FALSE) {
        ConSplitterGraphicsOutputBlt (&mConOut.GraphicsOutput,
                                      mVGopBuffer,
                                      EfiBltBufferToVideo,
                                      mAutoBufferToVideoRect.Left,
                                      mAutoBufferToVideoRect.Top,
                                      mAutoBufferToVideoRect.Left,
                                      mAutoBufferToVideoRect.Top,
                                      mAutoBufferToVideoRect.Right - mAutoBufferToVideoRect.Left,
                                      mAutoBufferToVideoRect.Bottom - mAutoBufferToVideoRect.Top,
                                      mHorizontalResolution * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                                      );
        mBufferToVideoSyncFlag = TRUE;
      }
      break;
    case VGopAutoBltVideoToBuffer:
      if (mVideoToBufferSyncFlag == FALSE) {
        ConSplitterGraphicsOutputBlt (&mConOut.GraphicsOutput,
                                      mVGopBuffer,
                                      EfiBltVideoToBltBuffer,
                                      mAutoVideoToBufferRect.Left,
                                      mAutoVideoToBufferRect.Top,
                                      mAutoVideoToBufferRect.Left,
                                      mAutoVideoToBufferRect.Top,
                                      mAutoVideoToBufferRect.Right - mAutoVideoToBufferRect.Left,
                                      mAutoVideoToBufferRect.Bottom - mAutoVideoToBufferRect.Top,
                                      mHorizontalResolution * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                                      );
        mVideoToBufferSyncFlag = TRUE;
      }
      break;
    case VGopBltBufferToVideo:
        ConSplitterGraphicsOutputBlt (&mConOut.GraphicsOutput,
                                      mVGopBuffer,
                                      EfiBltBufferToVideo,
                                      0,
                                      0,
                                      0,
                                      0,
                                      mHorizontalResolution,
                                      mVerticalResolution,
                                      mHorizontalResolution * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                                      );
      break;
    case VGopBltVideoToBuffer:
      if (mVideoToBufferSyncFlag == FALSE) {
        ConSplitterGraphicsOutputBlt (&mConOut.GraphicsOutput,
                                      mVGopBuffer,
                                      EfiBltVideoToBltBuffer,
                                      0,
                                      0,
                                      0,
                                      0,
                                      mHorizontalResolution,
                                      mVerticalResolution,
                                      mHorizontalResolution * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                                      );
        mVideoToBufferSyncFlag = TRUE;
      }
      break;
    default:
      DEBUG((DEBUG_ERROR, "%a(%d) DMVirtualGopSync Error.%d \n", __FILE__, __LINE__, Type));
      return EFI_UNSUPPORTED;
      break;
  }

  return EFI_SUCCESS;;
}

VOID
EFIAPI
InstallVirtualGop (
  VOID
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;

  mDMProtocol.IsVirtualGop          = FALSE;
  mDMProtocol.VirtualGopSync        = DMVirtualGopSync;
  mDMProtocol.EnableVirtualGop      = DMVirtualGopEnable;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mConOut.VirtualHandle,
                  &gVirtualGopProtocolGuid,
                  &mDMProtocol,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
      goto Exit;
  }

Exit:
  DEBUG((DEBUG_INFO, "INFO [GOP]: InstallVirtualGop exit - code=%r\n", Status));

  return;
}

VOID
EFIAPI
UnInstallVirtualGop (
  VOID
  )
{
  gBS->UninstallMultipleProtocolInterfaces (
            mConOut.VirtualHandle,
            &gVirtualGopProtocolGuid,
            &mDMProtocol,
            NULL
            );

  if (mVGopBuffer != NULL) {
    FreePool(mVGopBuffer);
    mVGopBuffer = NULL;
  }
}
