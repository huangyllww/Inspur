/*++

Copyright (c) 2007, Intelligraphics, Inc.                          

Module Name:

  UefiSmiGraphicsOutput.c

Abstract:

  This file produces the graphics abstraction of Graphics Output Protocol. It is called by 
  UefiSmi.c file which deals with the EFI 1.1/2.0 driver model.
  This file just does graphics.

--*/

#include "UefiSmi.h"
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

#ifdef NSFPGA
	//Remove small VGA mode #define INIT_COUNT 4
	#define INIT_COUNT 2
#else
	#define INIT_COUNT 1
#endif

static SMI_REGISTER 	SMI_CHIP_INIT[]={
	//REGISTER		DATA			MASK
#ifdef NSFPGA
	{0x0802b0,	   0x00064691,	 	0x0ffffffff},		//25Mhz	
	{0x0802b4,	   0x0,				0x0ffffffff},		//25Mhz
	{0x0802b8,	   0x000a45e0,	 	0x0ffffffff},		//28Mhz
	{0x0802bc,	   0x0,				0x0ffffffff}		//28Mhz
#else
	{0x0802bc,	   0x000010,	 	0x00007ffff}		//28Mhz
#endif
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

//
// Table of supported video modes
//
static SMI_VIDEO_MODES  SmiVideoModes[] = {
  //Remove small VGA mode {  640, 480, 32, 60, PRIMARY_640x480_60HZ},  
  //{  800, 600, 32, 60, PRIMARY_800x600_60HZ},  
  {  1024, 768, 32, 60, PRIMARY_1024x768_60HZ},  
  {  1280, 1024, 32, 60, PRIMARY_1280x1024_60HZ}  
};

//
// Local Function Prototypes
//
static unsigned long
GetPhysicalBaseAddress(
  SMI_PRIVATE_DATA  *Private
);

static VOID
InitChip(
  SMI_PRIVATE_DATA  *Private
);
static VOID
InitializeGraphicsMode (
  SMI_PRIVATE_DATA  *Private,
  SMI_VIDEO_MODES   *ModeData
  );

static VOID
ClearScreen (
  SMI_PRIVATE_DATA  *Private
  );
static VOID
FillScreenRGB (
  SMI_PRIVATE_DATA  *Private
  );

static VOID
outb (
  SMI_PRIVATE_DATA  *Private,
  UINTN                  Offset,
  UINT8                  Data
  );

static UINT8
inb (
  SMI_PRIVATE_DATA  *Private,
  UINTN                  Offset
  );

static VOID
outd (
  SMI_PRIVATE_DATA  *Private,
  UINTN                  Offset,
  UINT32                 Data
  );

static UINT32
ind (
  SMI_PRIVATE_DATA  *Private,
  UINTN                  Offset
  );



//
// Graphics Output Protocol Member Functions
//
EFI_STATUS
EFIAPI
SmiGraphicsOutputQueryMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL          *This,
  IN  UINT32                                ModeNumber,
  OUT UINTN                                 *SizeOfInfo,
  OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  **Info
  )
/*++

Routine Description:

  Graphics Output protocol interface to query video mode

  Arguments:
    This                  - Protocol instance pointer.
    ModeNumber            - The mode number to return information on.
    Info                  - Caller allocated buffer that returns information about ModeNumber.
    SizeOfInfo            - A pointer to the size, in bytes, of the Info buffer.

  Returns:
    EFI_SUCCESS           - Mode information returned.
    EFI_BUFFER_TOO_SMALL  - The Info buffer was too small.
    EFI_DEVICE_ERROR      - A hardware error occurred trying to retrieve the video mode.
    EFI_NOT_STARTED       - Video display is not initialized. Call SetMode ()
    EFI_INVALID_PARAMETER - One of the input args was NULL.

--*/
{
  SMI_PRIVATE_DATA  *Private;

  SMIDEBUG("SmiGraphicsOutputQueryMode +\n");

  Private = SMI_PRIVATE_DATA_FROM_GRAPHICS_OUTPUT_THIS (This);

  if (Private->HardwareNeedsStarting) {
    return EFI_NOT_STARTED;
  }

  if (Info == NULL || SizeOfInfo == NULL || ModeNumber >= This->Mode->MaxMode) {
    return EFI_INVALID_PARAMETER;
  }

  *Info = AllocatePool (sizeof (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION));

  if (*Info == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  *SizeOfInfo = sizeof (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);

  //(*Info)->Version = 1004;
  (*Info)->HorizontalResolution = Private->ModeData[ModeNumber].HorizontalResolution;
  (*Info)->VerticalResolution   = Private->ModeData[ModeNumber].VerticalResolution;

  (*Info)->PixelFormat = PixelBlueGreenRedReserved8BitPerColor;
  (*Info)->PixelsPerScanLine = (*Info)->HorizontalResolution;
  SMIDEBUG("SmiGraphicsOutputQueryMode -\n");
  return EFI_SUCCESS;
}


/*++

Routine Description:

  Graphics Output protocol interface to set video mode

  Arguments:
    This             - Protocol instance pointer.
    ModeNumber       - The mode number to be set.

  Returns:
    EFI_SUCCESS      - Graphics mode was changed.
    EFI_DEVICE_ERROR - The device had an error and could not complete the request.
    EFI_UNSUPPORTED  - ModeNumber is not supported by this device.

--*/
EFI_STATUS
EFIAPI
SmiGraphicsOutputSetMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL * This,
  IN  UINT32                       ModeNumber
  )
{
  SMI_PRIVATE_DATA  *Private;
  SMI_GRAPHICS_OUTPUT_MODE_DATA    *ModeData;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR     *FrameBufDesc;
  
  SMIDEBUG("SmiGraphicsOutputSetMode +\n");

  Private = SMI_PRIVATE_DATA_FROM_GRAPHICS_OUTPUT_THIS (This);

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (ModeNumber >= This->Mode->MaxMode) {
    return EFI_UNSUPPORTED;
  }

  if (ModeNumber == This->Mode->Mode) {
    return EFI_SUCCESS;
  }

  ModeData = &Private->ModeData[ModeNumber];

  //
  // Allocate Buffers
  //
  if (Private->LineBuffer) {
    gBS->FreePool (Private->LineBuffer);
    Private->LineBuffer = NULL;
  }
  Private->LineBuffer = AllocatePool (ModeData->HorizontalResolution  * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  if (Private->LineBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (Private->VirtualBuffer) {
    gBS->FreePool (Private->VirtualBuffer);
    Private->VirtualBuffer = NULL;
  }
  Private->VirtualBuffer = 
        (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) AllocateZeroPool (((ModeData->HorizontalResolution + 15) & ~15) * 
        ModeData->VerticalResolution * 
        (ModeData->ColorDepth / 8));
  if (Private->VirtualBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  InitializeGraphicsMode (Private, &SmiVideoModes[ModeNumber]);

  This->Mode->Mode = ModeNumber;
  This->Mode->Info->HorizontalResolution = ModeData->HorizontalResolution;
  This->Mode->Info->VerticalResolution   = ModeData->VerticalResolution;
  This->Mode->Info->PixelFormat          = PixelBlueGreenRedReserved8BitPerColor;
  This->Mode->Info->PixelsPerScanLine    = ModeData->HorizontalResolution;
  This->Mode->SizeOfInfo                 = sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);

  Private->PciIo->GetBarAttributes (
                        Private->PciIo,
                        0,
                        NULL,
                        (VOID**) &FrameBufDesc
                        );

  This->Mode->FrameBufferBase = FrameBufDesc->AddrRangeMin;
  This->Mode->FrameBufferSize = ModeData->HorizontalResolution * ModeData->VerticalResolution * 4;
  SMIDEBUG("SmiGraphicsOutputSetMode x=%d y=%d\n", ModeData->HorizontalResolution, ModeData->VerticalResolution);
  Private->HardwareNeedsStarting  = FALSE;
  
  SMIDEBUG("SmiGraphicsOutputSetMode -\n");

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
EFI_STATUS Status;
      Status = Private->PciIo->Mem.Read(
                            Private->PciIo,
                            EfiPciIoWidthUint32,
                            PCI_BAR_MMIO,//Point to Bar 1(MMIO base address),
                            Offset,
                            1,
                            &Data
                            );
	ASSERT_EFI_ERROR(Status);
	return Data;
}
VOID
SMI_WRITE_MMIO(
  SMI_PRIVATE_DATA  *Private,
  UINTN             Offset,
  UINT32            Data
)
{
EFI_STATUS Status;

  Status = Private->PciIo->Mem.Write (
                      Private->PciIo,
                      EfiPciIoWidthUint32,
                      PCI_BAR_MMIO,//Point to Bar 1(MMIO base address)
                      Offset,
                      1,
                      &Data
                      );

	ASSERT_EFI_ERROR(Status);
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
  UINT32          Pci;
  //
  // Read the PCI Configuration Header from the PCI Device
  //
   SMIDEBUG("SMI_PCI_READ +\n");
   Private->PciIo->Pci.Read (
                      	Private->PciIo,
                        EfiPciIoWidthUint32,
                        Offset,
                        sizeof (Pci) / sizeof (UINT32),
                        &Pci
                        );
    SMIDEBUG("SMI_PCI_READ -\n");
	return Pci;

}
static unsigned long
GetPhysicalBaseAddress (
  SMI_PRIVATE_DATA  *Private
)
{
   return	SMI_PCI_READ (Private, 0x10);
	
}
UINT8
SMI_8_IN_I(SMI_PRIVATE_DATA  *Private, int indexPort, int dataPort, UINT8 index)
{
  outb(Private, SMIIOBASE+indexPort, index);
  return(inb(Private, SMIIOBASE+dataPort));
}

void
SMI_8_OUT_I(SMI_PRIVATE_DATA  *Private, int indexPort, int dataPort, UINT8 index, UINT8 data)
{
  outb(Private, SMIIOBASE+indexPort, index);
  outb(Private, SMIIOBASE+dataPort, data);
}

UINT8
SMI_8_IN(SMI_PRIVATE_DATA  *Private, int port)
{
  return(inb(Private, SMIIOBASE+port));
}

void
SMI_8_OUT(SMI_PRIVATE_DATA  *Private, int port, UINT8 data)
{
  outb(Private, SMIIOBASE+port, data);
}

/*++

Routine Description:

  Graphics Output protocol instance to block transfer for SMI device

Arguments:

  This          - Pointer to Graphics Output protocol instance
  BltBuffer     - The data to transfer to screen
  BltOperation  - The operation to perform
  SourceX       - The X coordinate of the source for BltOperation
  SourceY       - The Y coordinate of the source for BltOperation
  DestinationX  - The X coordinate of the destination for BltOperation
  DestinationY  - The Y coordinate of the destination for BltOperation
  Width         - The width of a rectangle in the blt rectangle in pixels
  Height        - The height of a rectangle in the blt rectangle in pixels
  Delta         - Not used for EfiBltVideoFill and EfiBltVideoToVideo operation.
                  If a Delta of 0 is used, the entire BltBuffer will be operated on.
                  If a subrectangle of the BltBuffer is used, then Delta represents
                  the number of bytes in a row of the BltBuffer.

Returns:

  EFI_INVALID_PARAMETER - Invalid parameter passed in
  EFI_SUCCESS - Blt operation success

--*/
EFI_STATUS
EFIAPI
SmiGraphicsOutputBlt (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL          *This,
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL         *BltBuffer, OPTIONAL
  IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION     BltOperation,
  IN  UINTN                                 SourceX,
  IN  UINTN                                 SourceY,
  IN  UINTN                                 DestinationX,
  IN  UINTN                                 DestinationY,
  IN  UINTN                                 Width,
  IN  UINTN                                 Height,
  IN  UINTN                                 Delta
  )
{
  SMI_PRIVATE_DATA                *Private;
  SMI_GRAPHICS_OUTPUT_MODE_DATA   *Mode;
  EFI_TPL                         OriginalTPL;
  UINT32                          CurrentMode;
  UINTN                           SrcY, DstY;
  UINTN                           Offset, pcount;
  UINT8                           *BltMemSrc, *BltMemDst;
  UINTN                           PixelWidth;
  UINTN                           WidthInBytes;
  UINTN                           HorizontalResolution;
  UINTN                           VerticalResolution;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *FBuffer;

  SMIDEBUG("SmiGraphicsOutputBlt +\n");
  Private = SMI_PRIVATE_DATA_FROM_GRAPHICS_OUTPUT_THIS (This);
  CurrentMode = This->Mode->Mode;
  Mode = &Private->ModeData[CurrentMode];
  FBuffer = Private->VirtualBuffer;
  HorizontalResolution = (Mode->HorizontalResolution + 15) & ~15;
  VerticalResolution =  Mode->VerticalResolution;
  PixelWidth = Mode->ColorDepth / 8;
  WidthInBytes = Width * PixelWidth;
  if ((BltOperation < 0) || (BltOperation >= EfiGraphicsOutputBltOperationMax)) {
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
    Delta = Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  }

  //
  // We need to fill the Virtual Screen buffer with the blt data.
  // The virtual screen is upside down, as the first row is the bootom row of
  // the image.
  //
  SMIDEBUG("CurrentMode=%d\n", CurrentMode);
  SMIDEBUG("HorizontalResolution=%d\n", HorizontalResolution);
  SMIDEBUG("VerticalResolution=%d\n", VerticalResolution);
  SMIDEBUG("ColorDepth=%d\n", Mode->ColorDepth);
  SMIDEBUG("SourceX=%d,SourceY=%d,DestinationX=%d,DestinationY=%d,Width=%d,Height=%d\n", SourceX,SourceY,DestinationX,DestinationY,Width,Height);
  //
  // Make sure the SourceX, SourceY, DestinationX, DestinationY, Width, and Height parameters
  // are valid for the operation and the current screen geometry.
  //
  if (BltOperation == EfiBltVideoToBltBuffer) {
    //
    // Video to BltBuffer: Source is Video, destination is BltBuffer
    //
    if (SourceY + Height > VerticalResolution) {
      return EFI_INVALID_PARAMETER;
    }

    if (SourceX + Width > HorizontalResolution) {
      return EFI_INVALID_PARAMETER;
    }
  } else {
    //
    // BltBuffer to Video: Source is BltBuffer, destination is Video
    //
    if (DestinationY + Height > VerticalResolution) {
      return EFI_INVALID_PARAMETER;
    }

    if (DestinationX + Width > HorizontalResolution) {
      return EFI_INVALID_PARAMETER;
    }
  }
  //
  // We have to raise to TPL Notify, so we make an atomic write the frame buffer.
  // We would not want a timer based event (Cursor, ...) to come in while we are
  // doing this operation.
  //
  OriginalTPL = gBS->RaiseTPL (TPL_NOTIFY);

  switch (BltOperation) {
  case EfiBltVideoToBltBuffer:
    //
    // Video to BltBuffer: Source is Video, destination is BltBuffer
    //
    SMIDEBUG("EfiBltVideoToBltBuffer \n");
#ifdef SHADOWFB
	for (SrcY = SourceY, DstY = DestinationY; DstY < (Height + DestinationY); SrcY++, DstY++) {
	  Offset = (SrcY * HorizontalResolution + SourceX) * PixelWidth;
	  BltMemSrc = (UINT8 *) FBuffer + Offset;
	  BltMemDst = (UINT8 *)BltBuffer + DstY * Delta + DestinationX * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
	  gBS->CopyMem (BltMemDst, BltMemSrc, WidthInBytes);
	}
#else
	for (SrcY = SourceY, DstY = DestinationY; DstY < (Height + DestinationY); SrcY++, DstY++) {
	  Offset = ((SrcY * Private->ModeData[CurrentMode].HorizontalResolution) + SourceX) * (Private->ModeData[CurrentMode].ColorDepth / 8);
	  Private->LineBuffer = (UINT8 *)BltBuffer + DstY * Delta + DestinationX * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
	  Private->PciIo->Mem.Read (
	                        Private->PciIo,
	                        EfiPciIoWidthUint32,
	                        PCI_BAR_FRAMEBUFFER,
	                        Offset,
	                        Width,
	                        Private->LineBuffer
	                        );
	}
#endif
    break;

  case EfiBltVideoToVideo:
    //
    // Perform SMI hardware acceleration for Video to Video Bitblts
    //
    SMIDEBUG("deVideoMem2VideoMemBlt \n");
	#ifdef SHADOWFB
	for (pcount = 0; pcount < Height; pcount++) {
	  if (DestinationY <= SourceY) {
		  SrcY	= SourceY + pcount;
		  DstY	= DestinationY + pcount;
	  }
	  else
	  {
		  SrcY	= SourceY + Height - pcount - 1;
		  DstY	= DestinationY + Height - pcount - 1;
	  }
	
	  Offset = (DstY * HorizontalResolution + DestinationX) * PixelWidth;
	  BltMemDst = (UINT8 *) FBuffer + Offset;
	  gBS->CopyMem (
		  BltMemDst,
		  (UINT8 *) FBuffer + (SrcY * HorizontalResolution + SourceX) * PixelWidth,
		  WidthInBytes
		  );
	  Private->PciIo->Mem.Write (
						  Private->PciIo,
						  EfiPciIoWidthUint32,
						  PCI_BAR_FRAMEBUFFER,
						  Offset,
						  Width,
						  BltMemDst
						  );
	}
#else
for (pcount = 0, SrcY = SourceY, DstY = DestinationY; DstY < (Height + DestinationY); pcount++, SrcY++, DstY++) {
	  Offset = ((SrcY * Private->ModeData[CurrentMode].HorizontalResolution) + SourceX) * (Private->ModeData[CurrentMode].ColorDepth / 8);
	  Private->PciIo->Mem.Read (
	                        Private->PciIo,
	                        EfiPciIoWidthUint32,
	                        PCI_BAR_FRAMEBUFFER,
	                        Offset,
	                        Width,
	                        (UINT8 *)Private->VirtualBuffer + pcount * Width * (Private->ModeData[CurrentMode].ColorDepth / 8)
	                        );
	}
	for (pcount = 0, SrcY = SourceY, DstY = DestinationY; SrcY < (Height + SourceY); pcount++, SrcY++, DstY++) {
	  OffsetDst = ((DstY * Private->ModeData[CurrentMode].HorizontalResolution) + DestinationX) * (Private->ModeData[CurrentMode].ColorDepth / 8);
	  Private->PciIo->Mem.Write (
	                        Private->PciIo,
	                        EfiPciIoWidthUint32,
	                        PCI_BAR_FRAMEBUFFER,
	                        OffsetDst,
	                        Width,
		  					(UINT8 *)Private->VirtualBuffer + pcount * Width * (Private->ModeData[CurrentMode].ColorDepth / 8)
	                        );
	}
#endif
   break;

  case EfiBltVideoFill:
    // 
    // EfiBltVideoFill
    //
  SMIDEBUG("deRectFill \n");
#ifdef SHADOWFB
	BltMemSrc = (UINT8 *) BltBuffer;
	BltMemDst = (UINT8 *) FBuffer + (DestinationY * HorizontalResolution + DestinationX) * PixelWidth;
	for (pcount = 0; pcount < Width; pcount++) {
		gBS->CopyMem (
			  BltMemDst,
			  BltMemSrc,
			  PixelWidth
			  );
		BltMemDst += PixelWidth;
	}

	BltMemSrc = (UINT8 *) FBuffer + (DestinationY * HorizontalResolution + DestinationX) * PixelWidth;
	for (DstY = DestinationY + 1; DstY < (Height + DestinationY); DstY++) {
		Offset = (DstY * HorizontalResolution + DestinationX) * PixelWidth;
		gBS->CopyMem (
			  (UINT8 *)FBuffer + Offset,
			  BltMemSrc,
			  WidthInBytes
			  );
	}
	for (DstY = DestinationY; DstY < (Height + DestinationY); DstY++) {
	    Offset = (DstY * HorizontalResolution + DestinationX) * PixelWidth;
	    BltMemSrc = (UINT8 *)FBuffer + Offset;

	    Private->PciIo->Mem.Write (
							Private->PciIo,
							EfiPciIoWidthFillUint32,
							PCI_BAR_FRAMEBUFFER,
							Offset,
							Width,
							BltMemSrc
							);
	}
#else
	for (SrcY = SourceY, DstY = DestinationY; SrcY < (Height + SourceY); SrcY++, DstY++) {
	    Offset = ((DstY * HorizontalResolution) + DestinationX) * PixelWidth;
           
           Private->PciIo->Mem.Write (
                                    Private->PciIo,
                                    EfiPciIoWidthFillUint32,
                                    PCI_BAR_FRAMEBUFFER,
                                    Offset,
                                    Width,
                                    (UINT8 *)BltBuffer
                                    );
	}
#endif
    break;


  case EfiBltBufferToVideo:
    //
    // EfiBltBufferToVideo
    //
	SMIDEBUG("deSystemMem2VideoMemBlt \n");

#ifdef SHADOWFB
	for (SrcY = SourceY, DstY = DestinationY; SrcY < (Height + SourceY); SrcY++, DstY++) {
	    Offset = (DstY * HorizontalResolution + DestinationX) * PixelWidth;
		BltMemDst = (UINT8 *) FBuffer  + Offset;
		BltMemSrc = (UINT8 *) BltBuffer + (SrcY * Delta) + SourceX * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
		gBS->CopyMem (BltMemDst, BltMemSrc, WidthInBytes);
		Private->PciIo->Mem.Write (
							Private->PciIo,
							EfiPciIoWidthUint32,
							PCI_BAR_FRAMEBUFFER,
							Offset,
							Width,
							BltMemDst
							);
	}
#else
	for (SrcY = SourceY, DstY = DestinationY; SrcY < (Height + SourceY); SrcY++, DstY++) {
	  Private->LineBuffer = (UINT8 *)BltBuffer + SrcY * Delta + SourceX * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
	  Offset = ((DstY * Private->ModeData[CurrentMode].HorizontalResolution) + DestinationX) * (Private->ModeData[CurrentMode].ColorDepth / 8);

	  Private->PciIo->Mem.Write (
	                        Private->PciIo,
	                        EfiPciIoWidthUint32,
	                        PCI_BAR_FRAMEBUFFER,
	                        Offset,
	                        Width,
	                        Private->LineBuffer
	                        );
	}
#endif
    break;
  }

  gBS->RestoreTPL (OriginalTPL);
  SMIDEBUG("SmiGraphicsOutputBlt -\n");
  return EFI_SUCCESS;
}

//
// Construction and Destruction functions
//

EFI_STATUS
SmiGraphicsOutputConstructor (
  SMI_PRIVATE_DATA  *Private
  )
/*++

Routine Description:

    Constructor for the Graphics Output Protocol -- initialize -
    specific variables, information.

Arguments:

  Private - Pointer to private data structure

Returns:

  EFI_SUCCESS if initialization completes without error

--*/
{
  EFI_STATUS                   Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput;
  UINTN                        Index;
  
  SMIDEBUG("SmiGraphicsOutputConstructor +\n");

  //
  // Fill in Private->GraphicsOutput protocol
  //
  GraphicsOutput            = &Private->GraphicsOutput;

  GraphicsOutput->QueryMode = SmiGraphicsOutputQueryMode;
//  GraphicsOutput->GetMode	= SmiGraphicsOutputGetMode;
  GraphicsOutput->SetMode   = SmiGraphicsOutputSetMode;
  GraphicsOutput->Blt       = SmiGraphicsOutputBlt;

  //
  // Initialize the private data
  //
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE),
                  (VOID **) &Private->GraphicsOutput.Mode
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION),
                  (VOID **) &Private->GraphicsOutput.Mode->Info
                  );
  if (EFI_ERROR (Status)) {
    gBS->FreePool (Private->GraphicsOutput.Mode);
    return Status;
  }

  Private->GraphicsOutput.Mode->MaxMode = SMI_GRAPHICS_OUTPUT_MODE_COUNT;
  Private->GraphicsOutput.Mode->Mode    = GRAPHICS_OUTPUT_INVALIDE_MODE_NUMBER;

  for (Index = 0; Index < Private->GraphicsOutput.Mode->MaxMode; Index++) {
    Private->ModeData[Index].HorizontalResolution = SmiVideoModes[Index].Width;
    Private->ModeData[Index].VerticalResolution   = SmiVideoModes[Index].Height;
    Private->ModeData[Index].ColorDepth           = SmiVideoModes[Index].ColorDepth;
    Private->ModeData[Index].RefreshRate          = SmiVideoModes[Index].RefreshRate;
  }

  Private->HardwareNeedsStarting  = TRUE;
  Private->LineBuffer = NULL;

  //
  // Allocate Buffers
  //
	Private->VirtualBuffer = NULL;
	Private->VirtualBuffer = AllocatePool (1920 * 1080 * 4);
	if (Private->VirtualBuffer == NULL) {
	  return EFI_OUT_OF_RESOURCES;
	}

  //
  // Initialize the hardware
  //
  InitChip(Private);
  GraphicsOutput->SetMode (GraphicsOutput, 1);
  /* OPTION:  SMI can Draw Logo here */
  
  SMIDEBUG("SmiGraphicsOutputConstructor -\n");

  return EFI_SUCCESS;
}

/*++

Routine Description:
    Clean up after closing the Graphics Output Protocol

Arguments:

Returns:

  None

--*/
// Clean up after destroying the GOP class
EFI_STATUS
SmiGraphicsOutputDestructor (
  SMI_PRIVATE_DATA  *Private
  )
{
  SMIDEBUG("SmiGraphicsOutputDestructor +\n");
  if (Private->GraphicsOutput.Mode != NULL) {
    if (Private->GraphicsOutput.Mode->Info != NULL) {
      gBS->FreePool (Private->GraphicsOutput.Mode->Info);
    }
    gBS->FreePool (Private->GraphicsOutput.Mode);
  }
  SMIDEBUG("SmiGraphicsOutputDestructor -\n");
  return EFI_SUCCESS;
}

/*++

Routine Description:

  Output a byte to a memory-mapped register on the 

Arguments:

  Private - Pointer to private data structure
  Address - Offset of the register in the memory-map
  Data    - Data to be written

Returns:

  VOID

--*/
static VOID
outb (
  SMI_PRIVATE_DATA  *Private,
  UINTN                           Offset,
  UINT8                           Data
  )
{
  Private->PciIo->Mem.Write (
                      Private->PciIo,
                      EfiPciIoWidthUint8,
                      EFI_PCI_IO_PASS_THROUGH_BAR,
                      Offset,
                      1,
                      &Data
                      );

}

/*++

Routine Description:

  Input a byte from a memory-mapped register.

Arguments:

  Private - Pointer to private data structure
  Address - Offset of the register in the memory-map

Returns:

  byte register value

--*/
static UINT8
inb (
  SMI_PRIVATE_DATA  *Private,
  UINTN                  Offset
  )
{
  UINT8 Data;

  Private->PciIo->Io.Read (
                      Private->PciIo,
                      EfiPciIoWidthUint8,
                      EFI_PCI_IO_PASS_THROUGH_BAR,
                      Offset,
                      1,
                      &Data
                      );
  return Data;
}

/*++

Routine Description:

  Output a dword to a memory-mapped register

Arguments:

  Private - Pointer to private data structure
  Address - Offset of the register in the memory-map
  Data    - Data to be written

Returns:

  Void

--*/
static VOID
outd (
  SMI_PRIVATE_DATA  *Private,
  UINTN                          Offset,
  UINT32                           Data
  )
{
  Private->PciIo->Mem.Write (
                      Private->PciIo,
                      EfiPciIoWidthUint32,
                      EFI_PCI_IO_PASS_THROUGH_BAR,
                      Offset,
                      1,
                      &Data
                      );
}



/*++

Routine Description:

  Read a dword from a memory mapped register

Arguments:

  Private - Pointer to private data structure
  Address - Offset of the register in the memory-map

Returns:

  Dword register value

--*/
static UINT32
ind (
  SMI_PRIVATE_DATA  *Private,
  UINTN                  Offset
  )
{
  UINT32 Data;

  Private->PciIo->Io.Read (
                      Private->PciIo,
                      EfiPciIoWidthUint32,
                      EFI_PCI_IO_PASS_THROUGH_BAR,
                      Offset,
                      1,
                      &Data
                      );
  return Data;
}

/*++

Routine Description:

  Fill the framebuffer with the color black.

Arguments:

  Private -- pointer to the private data structure

Returns:

  Void

--*/
static VOID
ClearScreen (
  SMI_PRIVATE_DATA  *Private
  )
{
  UINT32  Color;

  Color = 0;

  SMIDEBUG("Cls +\n" );

/* OPT::  Put in a solid-fill blit  */

  Private->PciIo->Mem.Write (
                        Private->PciIo,
                        EfiPciIoWidthFillUint32,
                        PCI_BAR_FRAMEBUFFER,
                        0,
                        1280*1024,
                        &Color
                        );
  SMIDEBUG("Cls -\n" );
}

/*++

Routine Description:

  Fill the framebuffer with the color black.

Arguments:

  Private -- pointer to the private data structure

Returns:

  Void

--*/
static VOID
FillScreenRGB (
  SMI_PRIVATE_DATA  *Private
  )
{
  UINT32  RColor=0xff0000;
  UINT32  GColor=0x00ff00;
  UINT32  BColor=0x0000ff;
  SMIDEBUG("FillScreenRGB +\n");

/* OPT::  Put in a solid-fill blit  */

  Private->PciIo->Mem.Write (
                        Private->PciIo,
                        EfiPciIoWidthFillUint32,
                        PCI_BAR_FRAMEBUFFER,
                        0,
                        800*200,
                        &RColor
                        );
  Private->PciIo->Mem.Write (
                        Private->PciIo,
                        EfiPciIoWidthFillUint32,
                        PCI_BAR_FRAMEBUFFER,
                        800*200*4,
                        800*200,
                        &GColor
                        );

  Private->PciIo->Mem.Write (
                        Private->PciIo,
                        EfiPciIoWidthFillUint32,
                        PCI_BAR_FRAMEBUFFER,
						800*400*4,
                        800*200,
                        &BColor
                        );
  gBS->Stall(10000000); // delay 10S
  SMIDEBUG("FillScreenRGB -\n");
}

/*++

Routine Description:

  Hardware initialization

Arguments:

  Private   - pointer to the private data structure
*/
static VOID
InitChip( SMI_PRIVATE_DATA  *Private)
{
int i;
	// initial default setting
	SMIDEBUG("InitChip +\n" );
	for (i=0; i<INIT_COUNT;i++)
	{
		SMIDEBUG("index= %08X data=%08x mask=%08x\n",(UINT32)SMI_CHIP_INIT[i].INDEX,SMI_CHIP_INIT[i].DATA,SMI_CHIP_INIT[i].MASK);
	    SMI_WRITE_MMIO_EX(Private, SMI_CHIP_INIT[i].INDEX, SMI_CHIP_INIT[i].DATA, SMI_CHIP_INIT[i].MASK);
    	SMIDEBUG("Return %08X\n", (UINT32)SMI_READ_MMIO(Private, SMI_CHIP_INIT[i].INDEX) );
	}
	SMIDEBUG("InitChip -\n");

}
static VOID
InitializeGraphicsMode (
  SMI_PRIVATE_DATA  *Private,
  SMI_VIDEO_MODES   *ModeData
  )
/*++

Routine Description:

  Hardware initialization and modeset code

Arguments:

  Private   - pointer to the private data structure
  ModeData  - Physical mode description, register values

Returns:

  Void

--*/
{
  SMIDEBUG("InitializeGraphicsMode +\n");

  //InitChip(Private);
  //resetFrameBufferMemory(Private);
  setModeEx(Private, ModeData->pModeParam, (unsigned long)ModeData->Width, (unsigned long)ModeData->ColorDepth);
  //FillScreenRGB (Private);
  ClearScreen(Private);
  //SMI_PrintRegs(Private); 
  //deInit(Private);
  
  SMIDEBUG("InitializeGraphicsMode -\n");

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

  SMIDEBUG("START register dump ------------------\n");
  SMIDEBUG("\n\nSystem Reg    x0       x4       x8       xC");
    for (i = 0x00; i <= 0x70; i += 4)
  {
    if ((i & 0xF) == 0x0) SMIDEBUG("\n%02X|", i);
    SMIDEBUG(" %08X", SMI_READ_MMIO(Private, i) );
  }
  SMIDEBUG("\n0000000000000088| %08X", SMI_READ_MMIO(Private, 0x88));
  SMIDEBUG("\n\nPANEL REG    x0       x4       x8       xC");
    for (i = 0x80000; i <= 0x80040; i += 4)
  {
    if ((i & 0xF) == 0x0) SMIDEBUG("\n%02X|", i);
    SMIDEBUG(" %08X", SMI_READ_MMIO(Private, i) );
  }
  SMIDEBUG("\n\nCRT REG    x0       x4       x8       xC");
    for (i = 0x80200; i <= 0x80240; i += 4)
  {
    if ((i & 0xF) == 0x0) SMIDEBUG("\n%02X|", i);
    SMIDEBUG(" %08X", SMI_READ_MMIO(Private, i) );
  }
  SMIDEBUG("\n\n2D REG    x0       x4       x8       xC");
    for (i = 0x100000; i <= 0x10004c; i += 4)
  {
    if ((i & 0xF) == 0x0) SMIDEBUG("\n%02X|", i );
    SMIDEBUG(" %08X", SMI_READ_MMIO(Private, i) );
  }

  SMIDEBUG("\n\n");
  SMIDEBUG("END register dump --------------------\n");
}
