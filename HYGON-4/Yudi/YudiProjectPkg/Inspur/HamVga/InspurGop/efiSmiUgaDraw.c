/*++

Copyright (c) 2004, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  SmiUgaDraw.c

Abstract:

  This file produces the graphics abstration of UGA Draw. It is called by 
  Smi.c file which deals with the EFI 1.1 driver model. 
  This file just does graphics.

--*/

#include "efiSmi.h"
#include "smi2d.h"
#include "regde.h"
#include "smiMode.h"

//
// Register structure
//
typedef struct {
UINTN	INDEX;
UINT32 DATA;
UINT32 MASK;
} SMI_REGISTER;

//
// Video Mode structure
//
typedef struct {
  UINT32  Width;
  UINT32  Height;
  UINT32  ColorDepth;
  UINT32  RefreshRate;
  mode_parameter_t *pModeParam;
} SMI_VIDEO_MODES;

//
// chip initial default setting
//

#define INIT_COUNT 12
static SMI_REGISTER 	SMI_CHIP_INIT[]={
//REGISTER		DATA			MASK
{0x000070,	   0x2045c,		 0x00007ffff},			// default Mclk 290Mhz
{0x000064,	   0x020407,	 0x00007ffff},	
{0x000068,	   0x020408,	 0x00007ffff},	
{0x00004C,	   0x000004,	 0x000000004},	
{0x000004,	   0x01000024,	 0x0ffffffff},
{0x00005c,	   0x020a1c,	 0x00007ffff},	
{0x000060,	   0x020a1c,	 0x00007ffff},	
{0x000064,	   0x020a1c,	 0x00007ffff},	
{0x000070,	   0x020451,	 0x00007ffff},	
{0x000074,	   0x01b << 16,	 0x001 << 16},	
{0x000044,	   0x000eff,	 0x00000ffff},	
{0x000048,	   0x000eff,	 0x00000ffff},
{0x000004,	   0x01000064,	 0x0ffffffff}		   // reset memory	
// data path simul or LCD
//{0x80000,	(0 << 28)|(0xf <<24) |(1<<14)| (1 << 8)|(1<<2), (3 << 28)|(0xf <<24) |(1<<14)| (1 << 8)|(1<<2)},
//{0x80200,	(00 << 22) | (2 << 18) | (1 << 8) | (01),(3 << 22) | (3 << 18) || (1 << 10) | (1 << 8) | (3)}
};
// [4:3]
static mode_parameter_t PRIMARY_640x480_60HZ[] =
{
 { 800, 640, 656, 96, NEG, 525, 480, 490, 2, NEG, 25175000, 31469, 60, NEG}
};
static mode_parameter_t PRIMARY_800x600_60HZ[] =
{
 {1056, 800, 840,128, POS, 628, 600, 601, 4, POS, 40000000, 37879, 60, NEG}
};

static mode_parameter_t PRIMARY_1024x768_60HZ[] =
{
 {1344,1024,1048,136, NEG, 806, 768, 771, 6, NEG, 65000000, 48363, 60, NEG}
};

static mode_parameter_t PRIMARY_1280x1024_60HZ[] =
{
 {1688,1280,1328,112, POS,1066,1024,1025, 3, POS,108000000, 63981, 60, NEG}
};
// [16:9] wide screen
/* 1024 x 600  [16:9] 1.7 */
static mode_parameter_t PRIMARY_1024x600_60HZ[] =
{
 {1313,1024,1064,104, POS, 622, 600, 601, 3, POS, 49000000, 37319, 60, NEG}
};
static mode_parameter_t PRIMARY_1280x720_60HZ[] =
{
 {1664,1280,1336,136, POS, 746, 720, 721, 3, POS, 74481000, 44760, 60, NEG}
};

//
// Table of supported video modes
//
static SMI_VIDEO_MODES  SmiVideoModes[] = {
  {  640, 480, 8, 60, PRIMARY_640x480_60HZ},  
  {  800, 600, 8, 60, PRIMARY_800x600_60HZ},  
  {  1024, 768, 8, 60, PRIMARY_1024x768_60HZ},  
  {  1280, 1024, 8, 60, PRIMARY_1280x1024_60HZ},
  {  640, 480, 32, 60, PRIMARY_640x480_60HZ},  
  {  800, 600, 32, 60, PRIMARY_800x600_60HZ},  
  {  1024, 768, 32, 60, PRIMARY_1024x768_60HZ},  
  {  1280, 1024, 32, 60, PRIMARY_1280x1024_60HZ}
};

static VOID
InitChip(
  SMI_PRIVATE_DATA  *Private
);

//
// Local Function Prototypes
//
VOID
InitializeGraphicsMode (
  SMI_PRIVATE_DATA  *Private,
  SMI_VIDEO_MODES   *ModeData
  );

VOID
SetPaletteColor (
  SMI_PRIVATE_DATA  *Private,
  UINTN                           Index,
  UINT8                           Red,
  UINT8                           Green,
  UINT8                           Blue
  );

VOID
SetDefaultPalette (
  SMI_PRIVATE_DATA  *Private
  );

STATIC
VOID
ClearScreen (
  SMI_PRIVATE_DATA  *Private
  );

VOID
DrawLogo (
  SMI_PRIVATE_DATA  *Private
  );

VOID
outb (
  SMI_PRIVATE_DATA  *Private,
  UINTN                           Address,
  UINT8                           Data
  );

VOID
outw (
  SMI_PRIVATE_DATA  *Private,
  UINTN                           Address,
  UINT16                          Data
  );

UINT8
inb (
  SMI_PRIVATE_DATA  *Private,
  UINTN                           Address
  );

UINT16
inw (
  SMI_PRIVATE_DATA  *Private,
  UINTN                           Address
  );

//
// UGA Draw Protocol Member Functions
//
EFI_STATUS
EFIAPI
SmiUgaDrawGetMode (
  IN  EFI_UGA_DRAW_PROTOCOL *This,
  OUT UINT32                *HorizontalResolution,
  OUT UINT32                *VerticalResolution,
  OUT UINT32                *ColorDepth,
  OUT UINT32                *RefreshRate
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  This                  - TODO: add argument description
  HorizontalResolution  - TODO: add argument description
  VerticalResolution    - TODO: add argument description
  ColorDepth            - TODO: add argument description
  RefreshRate           - TODO: add argument description

Returns:

  EFI_NOT_STARTED - TODO: Add description for return value
  EFI_INVALID_PARAMETER - TODO: Add description for return value
  EFI_SUCCESS - TODO: Add description for return value

--*/
{
  SMI_PRIVATE_DATA  *Private;

  Private = SMI_PRIVATE_DATA_FROM_UGA_DRAW_THIS (This);

  if (Private->HardwareNeedsStarting) {
    return EFI_NOT_STARTED;
  }

  if ((HorizontalResolution == NULL) ||
      (VerticalResolution == NULL)   ||
      (ColorDepth == NULL)           ||
      (RefreshRate == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *HorizontalResolution = Private->ModeData[Private->CurrentMode].HorizontalResolution;
  *VerticalResolution   = Private->ModeData[Private->CurrentMode].VerticalResolution;
  *ColorDepth           = Private->ModeData[Private->CurrentMode].ColorDepth;
  *RefreshRate          = Private->ModeData[Private->CurrentMode].RefreshRate;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmiUgaDrawSetMode (
  IN  EFI_UGA_DRAW_PROTOCOL *This,
  IN  UINT32                HorizontalResolution,
  IN  UINT32                VerticalResolution,
  IN  UINT32                ColorDepth,
  IN  UINT32                RefreshRate
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  This                  - TODO: add argument description
  HorizontalResolution  - TODO: add argument description
  VerticalResolution    - TODO: add argument description
  ColorDepth            - TODO: add argument description
  RefreshRate           - TODO: add argument description

Returns:

  EFI_OUT_OF_RESOURCES - TODO: Add description for return value
  EFI_SUCCESS - TODO: Add description for return value
  EFI_NOT_FOUND - TODO: Add description for return value

--*/
{
  SMI_PRIVATE_DATA  *Private;
  UINTN                           Index;

  Private = SMI_PRIVATE_DATA_FROM_UGA_DRAW_THIS (This);
  EfiDebugPrint(EFI_D_ERROR, "SmiUgaDrawSetMode + \n");
  EfiDebugPrint(EFI_D_ERROR, "HorizontalResolution = %d, VerticalResolution = %d, ColorDepth = %d, RefreshRate = %d\n", 
  				HorizontalResolution,
  				VerticalResolution,
  				ColorDepth,
  				RefreshRate);

  for (Index = 0; Index < Private->MaxMode; Index++) {

    if (HorizontalResolution != Private->ModeData[Index].HorizontalResolution) {
      continue;
    }

    if (VerticalResolution != Private->ModeData[Index].VerticalResolution) {
      continue;
    }

    if (ColorDepth != Private->ModeData[Index].ColorDepth) {
      continue;
    }

    if (RefreshRate != Private->ModeData[Index].RefreshRate) {
      continue;
    }

    if (Private->LineBuffer) {
      gBS->FreePool (Private->LineBuffer);
    }

    Private->LineBuffer = NULL;
    Private->LineBuffer = EfiLibAllocatePool (HorizontalResolution);
    if (Private->LineBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    InitializeGraphicsMode (Private, &SmiVideoModes[Index]);

    Private->CurrentMode            = Index;

    Private->HardwareNeedsStarting  = FALSE;

    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
SmiUgaDrawBlt (
  IN  EFI_UGA_DRAW_PROTOCOL     *This,
  IN  EFI_UGA_PIXEL             *BltBuffer, OPTIONAL
  IN  EFI_UGA_BLT_OPERATION     BltOperation,
  IN  UINTN                     SourceX,
  IN  UINTN                     SourceY,
  IN  UINTN                     DestinationX,
  IN  UINTN                     DestinationY,
  IN  UINTN                     Width,
  IN  UINTN                     Height,
  IN  UINTN                     Delta
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  This          - TODO: add argument description
  BltBuffer     - TODO: add argument description
  BltOperation  - TODO: add argument description
  SourceX       - TODO: add argument description
  SourceY       - TODO: add argument description
  DestinationX  - TODO: add argument description
  DestinationY  - TODO: add argument description
  Width         - TODO: add argument description
  Height        - TODO: add argument description
  Delta         - TODO: add argument description

Returns:

  EFI_INVALID_PARAMETER - TODO: Add description for return value
  EFI_INVALID_PARAMETER - TODO: Add description for return value
  EFI_INVALID_PARAMETER - TODO: Add description for return value
  EFI_INVALID_PARAMETER - TODO: Add description for return value
  EFI_INVALID_PARAMETER - TODO: Add description for return value
  EFI_INVALID_PARAMETER - TODO: Add description for return value
  EFI_SUCCESS - TODO: Add description for return value

--*/
{
  SMI_PRIVATE_DATA  *Private;
  EFI_TPL                         OriginalTPL;
  UINTN                           DstY;
  UINTN                           SrcY;
  EFI_UGA_PIXEL                   *Blt;
  UINTN                           X;
//  UINT8                           Pixel;
//  UINT32                          WidePixel;
//  UINTN                           ScreenWidth;
  UINTN                           Offset;
//  UINTN                           SourceOffset;
  UINT32					  ulTemp;
  UINT32					  SrcPitch,DstPitch;	
//  UINT32                          CurrentMode;

  Private = SMI_PRIVATE_DATA_FROM_UGA_DRAW_THIS (This);

  if ((BltOperation < 0) || (BltOperation >= EfiUgaBltMax)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Width == 0 || Height == 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // If Delta is zero, then the entire BltBuffer is being used, so Delta
  // is the number of bytes in each row of BltBuffer.  Since BltBuffer is Width pixels size,
  // the number of bytes in each row can be computed.
  //
  if (Delta == 0) {
    Delta = Width * sizeof (EFI_UGA_PIXEL);
  }
        /* 
         * Pitch value calculation in Bytes.
         * Usually, it is (screen width) * (byte per pixel).
         * However, there are cases that screen width is not 16 pixel aligned, which is
         * a requirement for some OS and the hardware itself.
         * For standard 4:3 resolutions: 320, 640, 800, 1024 and 1280, they are all
         * 16 pixel aligned and pitch is simply (screen width) * (byte per pixel).
         *   
         * However, 1366 resolution, for example, has to be adjusted for 16 pixel aligned.
         */
        ulTemp = (Private->ModeData[Private->CurrentMode].HorizontalResolution + 15) & ~15; /* This calculation has no effect on 640, 800, 1024 and 1280. */		
        DstPitch = SrcPitch = ulTemp * (Private->ModeData[Private->CurrentMode].ColorDepth / 8);

  //
  // We need to fill the Virtual Screen buffer with the blt data.
  // The virtual screen is upside down, as the first row is the bootom row of
  // the image.
  //

  //
  // Make sure the SourceX, SourceY, DestinationX, DestinationY, Width, and Height parameters
  // are valid for the operation and the current screen geometry.
  //
  if (BltOperation == EfiUgaVideoToBltBuffer) {
    //
    // Video to BltBuffer: Source is Video, destination is BltBuffer
    //
    if (SourceY + Height > Private->ModeData[Private->CurrentMode].VerticalResolution) {
      return EFI_INVALID_PARAMETER;
    }

    if (SourceX + Width > Private->ModeData[Private->CurrentMode].HorizontalResolution) {
      return EFI_INVALID_PARAMETER;
    }
  } else {
    //
    // BltBuffer to Video: Source is BltBuffer, destination is Video
    //
    if (DestinationY + Height > Private->ModeData[Private->CurrentMode].VerticalResolution) {
      return EFI_INVALID_PARAMETER;
    }

    if (DestinationX + Width > Private->ModeData[Private->CurrentMode].HorizontalResolution) {
      return EFI_INVALID_PARAMETER;
    }
  }
  //
  // We have to raise to TPL Notify, so we make an atomic write the frame buffer.
  // We would not want a timer based event (Cursor, ...) to come in while we are
  // doing this operation.
  //
  OriginalTPL = gBS->RaiseTPL (EFI_TPL_NOTIFY);
  deInit(Private);

  switch (BltOperation) {
  case EfiUgaVideoToBltBuffer:
    //
    // Video to BltBuffer: Source is Video, destination is BltBuffer
    //
    for (SrcY = SourceY, DstY = DestinationY; DstY < (Height + DestinationY); SrcY++, DstY++) {

      Offset = (SrcY * Private->ModeData[Private->CurrentMode].HorizontalResolution) + SourceX;
      if (((Offset & 0x03) == 0) && ((Width & 0x03) == 0)) {
        Private->PciIo->Mem.Read (
                              Private->PciIo,
                              EfiPciIoWidthUint32,
                              0,
                              Offset,
                              Width >> 2,
                              Private->LineBuffer
                              );
      } else {
        Private->PciIo->Mem.Read (
                              Private->PciIo,
                              EfiPciIoWidthUint8,
                              0,
                              Offset,
                              Width,
                              Private->LineBuffer
                              );
      }

      for (X = 0; X < Width; X++) {
        Blt         = (EFI_UGA_PIXEL *) ((UINT8 *) BltBuffer + (DstY * Delta) + (DestinationX + X) * sizeof (EFI_UGA_PIXEL));

        Blt->Red    = (UINT8) (Private->LineBuffer[X] & 0xe0);
        Blt->Green  = (UINT8) ((Private->LineBuffer[X] & 0x1c) << 3);
        Blt->Blue   = (UINT8) ((Private->LineBuffer[X] & 0x03) << 6);
      }
    }

			break;       
  case EfiUgaVideoToVideo:
    //
    // Perform SMI hardware acceleration for Video to Video Bitblts
    //
		deVideoMem2VideoMemBlt(
			Private,
			0,  /* Address of source: offset in frame buffer */
			(UINT32) SrcPitch, /* Pitch value of source surface in BYTE */
			(UINT32) SourceX,
			(UINT32) SourceY,     /* Starting coordinate of source surface */
			0,  /* Address of destination: offset in frame buffer */
			DstPitch, /* Pitch value of destination surface in BYTE */
			Private->ModeData[Private->CurrentMode].ColorDepth ,    /* Color depth of destination surface */
			(UINT32)DestinationX,
			(UINT32)DestinationY,     /* Starting coordinate of destination surface */
			(UINT32)Width, 
			(UINT32)Height, /* width and height of rectangle in pixel value */
			ROP2_COPY);   /* ROP value */
    	break;

  case EfiUgaVideoFill:
        deRectFill(Private,
				   0,  /* Address of source: offset in frame buffer */
				   SrcPitch, /* Pitch value of source surface in BYTE */
				   Private->ModeData[Private->CurrentMode].ColorDepth ,    /* Color depth of destination surface */
		           (UINT32) DestinationX,
		           (UINT32) DestinationY,
                   (UINT32) Width,
                   (UINT32) Height,
				   ((UINT32 *)BltBuffer)[0] ,    /* Color depth of destination surface */
                   ROP2_COPY);
    	break;
  case EfiUgaBltBufferToVideo:
    for (SrcY = SourceY, DstY = DestinationY; SrcY < (Height + SourceY); SrcY++, DstY++) {

      for (X = 0; X < Width; X++) {
        Blt                     = (EFI_UGA_PIXEL *) ((UINT8 *) BltBuffer + (SrcY * Delta) + (SourceX + X) * sizeof (EFI_UGA_PIXEL));
        Private->LineBuffer[X]  = (UINT8) ((Blt->Red & 0xe0) | ((Blt->Green >> 3) & 0x1c) | ((Blt->Blue >> 6) & 0x03));
      }

      Offset = (DstY * Private->ModeData[Private->CurrentMode].HorizontalResolution) + DestinationX;

      if (((Offset & 0x03) == 0) && ((Width & 0x03) == 0)) {
        Private->PciIo->Mem.Write (
                              Private->PciIo,
                              EfiPciIoWidthUint32,
                              0,
                              Offset,
                              Width >> 2,
                              Private->LineBuffer
                              );
      } else {
        Private->PciIo->Mem.Write (
                              Private->PciIo,
                              EfiPciIoWidthUint8,
                              0,
                              Offset,
                              Width,
                              Private->LineBuffer
                              );
      }
    }

	   break;  
  }

  gBS->RestoreTPL (OriginalTPL);

  return EFI_SUCCESS;
}

//
// Construction and Destruction functions
//

EFI_STATUS
SmiUgaDrawConstructor (
  SMI_PRIVATE_DATA  *Private
  )
/*++

Routine Description:

Arguments:

Returns:

  None

--*/
// TODO:    Private - add argument and description to function comment
// TODO:    EFI_SUCCESS - add return value to function comment
{
  EFI_UGA_DRAW_PROTOCOL *UgaDraw;
  UINTN                 Index;

  //
  // Fill in Private->UgaDraw protocol
  //
  UgaDraw           = &Private->UgaDraw;

  UgaDraw->GetMode  = SmiUgaDrawGetMode;
  UgaDraw->SetMode  = SmiUgaDrawSetMode;
  UgaDraw->Blt      = SmiUgaDrawBlt;

  //
  // Initialize the private data
  //
  Private->MaxMode      = SMI_UGA_DRAW_MODE_COUNT;
  Private->CurrentMode  = 0;
  for (Index = 0; Index < Private->MaxMode; Index++) {
    Private->ModeData[Index].HorizontalResolution = SmiVideoModes[Index].Width;
    Private->ModeData[Index].VerticalResolution   = SmiVideoModes[Index].Height;
    Private->ModeData[Index].ColorDepth           = SmiVideoModes[Index].ColorDepth;
    Private->ModeData[Index].RefreshRate          = SmiVideoModes[Index].RefreshRate;
  }

  Private->HardwareNeedsStarting  = TRUE;
  Private->LineBuffer             = NULL;

  //
  // Initialize the hardware
  //
  UgaDraw->SetMode (
            UgaDraw,
            Private->ModeData[Private->CurrentMode].HorizontalResolution,
            Private->ModeData[Private->CurrentMode].VerticalResolution,
            Private->ModeData[Private->CurrentMode].ColorDepth,
            Private->ModeData[Private->CurrentMode].RefreshRate
            );
  DrawLogo (Private);

  return EFI_SUCCESS;
}

EFI_STATUS
SmiUgaDrawDestructor (
  SMI_PRIVATE_DATA  *Private
  )
/*++

Routine Description:

Arguments:

Returns:

  None

--*/
// TODO:    Private - add argument and description to function comment
// TODO:    EFI_SUCCESS - add return value to function comment
{
  return EFI_SUCCESS;
}

//
// The following routines are MMIO abstraction macros implemented to make it 
// easier to follow through the modesetting code.
//
UINT32
SMI_READ_MMIO(
  SMI_PRIVATE_DATA  *Private,
  UINTN             Offset
)
{
UINT32 Data;
      Private->PciIo->Mem.Read(
                            Private->PciIo,
                            EfiPciIoWidthUint32,
                            PCI_BAR_MMIO,//Point to Bar 1(MMIO base address),
                            Offset,
                            1,
                            &Data
                            );

	return Data;
}
VOID
SMI_WRITE_MMIO(
  SMI_PRIVATE_DATA  *Private,
  UINTN             Offset,
  UINT32            Data
)
{

  Private->PciIo->Mem.Write (
                      Private->PciIo,
                      EfiPciIoWidthUint32,
                      PCI_BAR_MMIO,//Point to Bar 1(MMIO base address)
                      Offset,
                      1,
                      &Data
                      );

}

VOID
SMI_WRITE_MMIO_EX(
  SMI_PRIVATE_DATA  *Private,
  UINTN                          Offset,
  UINT32                         Data,
  UINT32						 Mask
)
{
UINT32	value;
	value = SMI_READ_MMIO(Private, Offset);
	value &= ~Mask;
	Data |= value;
	SMI_WRITE_MMIO(Private, Offset, Data);
}

UINT32  
SMI_PCI_READ (
  SMI_PRIVATE_DATA  *Private,
  UINT32                Offset
)
{
//  EFI_STATUS          Status;
//  EFI_PCI_IO_PROTOCOL *PciIo=NULL;
  UINT32          Pci;
  //
  // Read the PCI Configuration Header from the PCI Device
  //
   Private->PciIo->Pci.Read (
                      	Private->PciIo,
                        EfiPciIoWidthUint32,
                        Offset,
                        sizeof (Pci) / sizeof (UINT32),
                        &Pci
                        );
	return Pci;

}

VOID
outb (
  SMI_PRIVATE_DATA  *Private,
  UINTN                           Address,
  UINT8                           Data
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  Private - TODO: add argument description
  Address - TODO: add argument description
  Data    - TODO: add argument description

Returns:

  TODO: add return values

--*/
{
  Private->PciIo->Io.Write (
                      Private->PciIo,
                      EfiPciIoWidthUint8,
                      EFI_PCI_IO_PASS_THROUGH_BAR,
                      Address,
                      1,
                      &Data
                      );
}

VOID
outw (
  SMI_PRIVATE_DATA  *Private,
  UINTN                           Address,
  UINT16                          Data
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  Private - TODO: add argument description
  Address - TODO: add argument description
  Data    - TODO: add argument description

Returns:

  TODO: add return values

--*/
{
  Private->PciIo->Io.Write (
                      Private->PciIo,
                      EfiPciIoWidthUint16,
                      EFI_PCI_IO_PASS_THROUGH_BAR,
                      Address,
                      1,
                      &Data
                      );
}

UINT8
inb (
  SMI_PRIVATE_DATA  *Private,
  UINTN                           Address
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  Private - TODO: add argument description
  Address - TODO: add argument description

Returns:

  TODO: add return values

--*/
{
  UINT8 Data;

  Private->PciIo->Io.Read (
                      Private->PciIo,
                      EfiPciIoWidthUint8,
                      EFI_PCI_IO_PASS_THROUGH_BAR,
                      Address,
                      1,
                      &Data
                      );
  return Data;
}

UINT16
inw (
  SMI_PRIVATE_DATA  *Private,
  UINTN                           Address
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  Private - TODO: add argument description
  Address - TODO: add argument description

Returns:

  TODO: add return values

--*/
{
  UINT16  Data;

  Private->PciIo->Io.Read (
                      Private->PciIo,
                      EfiPciIoWidthUint16,
                      EFI_PCI_IO_PASS_THROUGH_BAR,
                      Address,
                      1,
                      &Data
                      );
  return Data;
}

VOID
SetPaletteColor (
  SMI_PRIVATE_DATA  *Private,
  UINTN                           Index,
  UINT8                           Red,
  UINT8                           Green,
  UINT8                           Blue
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  Private - TODO: add argument description
  Index   - TODO: add argument description
  Red     - TODO: add argument description
  Green   - TODO: add argument description
  Blue    - TODO: add argument description

Returns:

  TODO: add return values

--*/
{
  outb (Private, PALETTE_INDEX_REGISTER, (UINT8) Index);
  outb (Private, PALETTE_DATA_REGISTER, (UINT8) (Red >> 2));
  outb (Private, PALETTE_DATA_REGISTER, (UINT8) (Green >> 2));
  outb (Private, PALETTE_DATA_REGISTER, (UINT8) (Blue >> 2));
}

VOID
SetDefaultPalette (
  SMI_PRIVATE_DATA  *Private
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  Private - TODO: add argument description

Returns:

  TODO: add return values

--*/
{
  UINTN Index;
  UINTN RedIndex;
  UINTN GreenIndex;
  UINTN BlueIndex;

  Index = 0;
  for (RedIndex = 0; RedIndex < 8; RedIndex++) {
    for (GreenIndex = 0; GreenIndex < 8; GreenIndex++) {
      for (BlueIndex = 0; BlueIndex < 4; BlueIndex++) {
        SetPaletteColor (Private, Index, (UINT8) (RedIndex << 5), (UINT8) (GreenIndex << 5), (UINT8) (BlueIndex << 6));
        Index++;
      }
    }
  }
}

STATIC
VOID
ClearScreen (
  SMI_PRIVATE_DATA  *Private
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  Private - TODO: add argument description

Returns:

  TODO: add return values

--*/
{
  UINT32  Color;

  Color = 0;
  Private->PciIo->Mem.Write (
                        Private->PciIo,
                        EfiPciIoWidthFillUint32,
                        0,
                        0,
                        0x100000 >> 2,
                        &Color
                        );
}

VOID
DrawLogo (
  SMI_PRIVATE_DATA  *Private
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  Private - TODO: add argument description

Returns:

  TODO: add return values

--*/
{
  UINTN Offset;
  UINTN X;
  UINTN Y;
  UINTN ScreenWidth;
  UINTN ScreenHeight;
  UINT8 Color;

  ScreenWidth   = Private->ModeData[Private->CurrentMode].HorizontalResolution;
  ScreenHeight  = Private->ModeData[Private->CurrentMode].VerticalResolution;

  Offset        = 0;
  for (Y = 0; Y < ScreenHeight; Y++) {
    for (X = 0; X < ScreenWidth; X++) {
      Color                   = (UINT8) (256 * (X + Y) / (ScreenWidth + ScreenHeight));
      Private->LineBuffer[X]  = Color;
    }

    Private->PciIo->Mem.Write (
                          Private->PciIo,
                          EfiPciIoWidthUint32,
                          0,
                          Offset + (Y * ScreenWidth),
                          ScreenWidth >> 2,
                          Private->LineBuffer
                          );
  }
}

static VOID
InitChip( SMI_PRIVATE_DATA  *Private)
{
int i;
	// initial default setting
    	EfiDebugPrint(EFI_D_ERROR, "InitChip +\n" );
	for (i=0; i<INIT_COUNT;i++)
	{
    	EfiDebugPrint(EFI_D_ERROR, "index= %08X data=%08x mask=%08x\n",(UINT32)SMI_CHIP_INIT[i].INDEX,SMI_CHIP_INIT[i].DATA,SMI_CHIP_INIT[i].MASK );
	  SMI_WRITE_MMIO_EX(Private, SMI_CHIP_INIT[i].INDEX, SMI_CHIP_INIT[i].DATA, SMI_CHIP_INIT[i].MASK);
    	EfiDebugPrint(EFI_D_ERROR, "Return %08X\n", (UINT32)SMI_READ_MMIO(Private, SMI_CHIP_INIT[i].INDEX));
	}
    	EfiDebugPrint(EFI_D_ERROR, "InitChip -\n" );

}

VOID
InitializeGraphicsMode (
  SMI_PRIVATE_DATA  *Private,
  SMI_VIDEO_MODES   *ModeData
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  Private   - TODO: add argument description
  ModeData  - TODO: add argument description

Returns:

  TODO: add return values

--*/
{
//  UINT8 Byte;
//  UINTN Index;

  EfiDebugPrint(EFI_D_ERROR, "InitializeGraphicsMode +\n");
  InitChip(Private);
  setModeEx(Private, ModeData->pModeParam, (UINT32)ModeData->Width, (UINT32)ModeData->ColorDepth);
  //FillScreenRGB (Private); 
  //SMI_PrintRegs(Private);
  EfiDebugPrint(EFI_D_ERROR, "InitializeGraphicsMode -\n");
  ClearScreen (Private);
}
VOID
SMI_PrintRegs(
  SMI_PRIVATE_DATA *Private
  )
  /*++

Routine Description:

  Graphics Register Dump Routine -- this routine will print the contents of 
  SMI graphics registers to the debug port.

Arguments:

  Private   - pointer to the private data structure

Returns:

  Void

--*/
{
  long i;

  EfiDebugPrint(EFI_D_ERROR, "START register dump ------------------\n");
  EfiDebugPrint(EFI_D_ERROR, "\n\nSystem Reg    x0       x4       x8       xC");
    for (i = 0x00; i <= 0x70; i += 4)
  {
    if ((i & 0xF) == 0x0) EfiDebugPrint(EFI_D_ERROR, "\n%02X|", i);
    EfiDebugPrint(EFI_D_ERROR, " %08X", SMI_READ_MMIO(Private, i));
  }
  EfiDebugPrint(EFI_D_ERROR, "\n\nPANEL REG    x0       x4       x8       xC");
    for (i = 0x80000; i <= 0x80040; i += 4)
  {
    if ((i & 0xF) == 0x0) EfiDebugPrint(EFI_D_ERROR, "\n%02X|", i);
    EfiDebugPrint(EFI_D_ERROR, " %08X", SMI_READ_MMIO(Private, i));
  }
  EfiDebugPrint(EFI_D_ERROR, "\n\nCRT REG    x0       x4       x8       xC");
    for (i = 0x80200; i <= 0x80240; i += 4)
  {
    if ((i & 0xF) == 0x0) EfiDebugPrint(EFI_D_ERROR, "\n%02X|", i);
    EfiDebugPrint(EFI_D_ERROR, " %08X", SMI_READ_MMIO(Private, i));
  }
  EfiDebugPrint(EFI_D_ERROR, "\n\n2D REG    x0       x4       x8       xC");
    for (i = 0x100000; i <= 0x10004c; i += 4)
  {
    if ((i & 0xF) == 0x0) EfiDebugPrint(EFI_D_ERROR, "\n%02X|", i);
    EfiDebugPrint(EFI_D_ERROR, " %08X", SMI_READ_MMIO(Private, i));
  }

  EfiDebugPrint(EFI_D_ERROR, "\n\n");
  EfiDebugPrint(EFI_D_ERROR, "END register dump --------------------\n");
}
