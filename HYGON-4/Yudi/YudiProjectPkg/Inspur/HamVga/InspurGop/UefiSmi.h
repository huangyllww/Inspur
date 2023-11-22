/*Copyright (c) 2007, Intelligraphics, Inc.                                                      

Module Name:

  UefiSmi.h
    
Abstract:

  Siliconmotion Controller Driver

Revision History*/

//
// Siliconmotion Controller Driver
//

#ifndef _SMI_GOP_H_
#define _SMI_GOP_H_

#include <Uefi.h>
#include <Protocol/UgaDraw.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/PciIo.h>
#include <Protocol/DriverSupportedEfiVersion.h>
#include <Protocol/EdidOverride.h>
#include <Protocol/EdidDiscovered.h>
#include <Protocol/EdidActive.h>
#include <Protocol/DevicePath.h>

#include <Library/PostCodeLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/TimerLib.h>

#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Acpi.h>

#include "BuildFlag.h"

#define SMI_UEFI_DRV_VERSION    "INSPUR_EFI750 v1.00.04.00"

//
//PCI Configuration Header values
//
/* This definition is PCI VID and PID */
#define NS_PCI_VENDOR_ID			0x1bd4
#define NS_DEVICE_ID_SM750			0x0750

#define NUM_SEQ_REGS        5
#define NUM_CRTC_REGS       25
#define NUM_GFX_REGS        9
#define NUM_ATTR_REGS       21
#define VGA_CR_INDEX        0x3D4
#define VGA_CR_DATA         0x3D5
#define VGA_SEQ_INDEX       0x3C4
#define VGA_SEQ_DATA        0x3C5
#define VGA_GR_INDEX        0x3CE
#define VGA_GR_DATA         0x3CF
#define VGA_ATTR_REG        0x3C0
#define VGA_ATTR_REG_R      0x3C1
#define VGA_DAC_MASK        0x3C6
#define VGA_DAC_WRITE_ADDR  0x3C8
#define VGA_DAC_DATA        0x3C9
#define VGA_STAT_1_REG      0x3DA
#define VGA_MISC_OUT_REG_R  0x3CC

#define SMIDPRBASE          0x00008000
#define SMIVPRBASE          0x0000C000
#define SMICPRBASE          0x0000E000
#define SMIIOBASE           0x00300000

#define SMI_BITBLT          0x00000000
#define SMI_RECT_FILL       0x00010000
#define SMI_START_ENGINE    0x80000000
#define SMI_LEFT_TO_RIGHT   0x00000000
#define SMI_RIGHT_TO_LEFT   0x08000000

//
//	SMI PCI BAR definition
//
#define		PCI_BAR_FRAMEBUFFER		0
#define		PCI_BAR_MMIO			1

//
// Silicon Motion Graphical Mode Data
//
#define SMI_GRAPHICS_OUTPUT_MODE_COUNT 4

typedef struct {
  UINT32  HorizontalResolution;
  UINT32  VerticalResolution;
  UINT32  ColorDepth;
  UINT32  RefreshRate;
} SMI_GRAPHICS_OUTPUT_MODE_DATA;

//
// SMI Private Data Structure
//
#define SMI_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('S', 'M', '5', '0')

#define GRAPHICS_OUTPUT_INVALIDE_MODE_NUMBER  0xffff

typedef struct {
  UINT64                                       Signature;
  EFI_HANDLE                                   Handle;
  EFI_PCI_IO_PROTOCOL                          *PciIo;
  UINT64                                OriginalPciAttributes;

  //
  // GOP Private Data
  //
  EFI_GRAPHICS_OUTPUT_PROTOCOL                 GraphicsOutput;
  EFI_DEVICE_PATH_PROTOCOL              *GopDevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *UgaDevicePath;
  
  // Mode Data
  BOOLEAN                                      HardwareNeedsStarting;
  SMI_GRAPHICS_OUTPUT_MODE_DATA                ModeData[SMI_GRAPHICS_OUTPUT_MODE_COUNT];
  UINT8                                        *LineBuffer;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL                *VirtualBuffer;
} SMI_PRIVATE_DATA;

#define SMI_PRIVATE_DATA_FROM_GRAPHICS_OUTPUT_THIS(a) \
  CR(a, SMI_PRIVATE_DATA, GraphicsOutput, SMI_PRIVATE_DATA_SIGNATURE)

//
// Global Variables
//
extern EFI_DRIVER_BINDING_PROTOCOL  gSmiDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL  gSmiComponentName;

//
// Io Registers defined by VGA
//
#define CRTC_ADDRESS_REGISTER   0x3d4
#define CRTC_DATA_REGISTER      0x3d5
#define SEQ_ADDRESS_REGISTER    0x3c4
#define SEQ_DATA_REGISTER       0x3c5
#define GRAPH_ADDRESS_REGISTER  0x3ce
#define GRAPH_DATA_REGISTER     0x3cf
#define ATT_ADDRESS_REGISTER    0x3c0
#define MISC_OUTPUT_REGISTER    0x3c2
#define INPUT_STATUS_1_REGISTER 0x3da
#define DAC_PIXEL_MASK_REGISTER 0x3c6
#define PALETTE_INDEX_REGISTER  0x3c8
#define PALETTE_DATA_REGISTER   0x3c9

UINT32
SMI_READ_MMIO(
  SMI_PRIVATE_DATA  *Private,
  UINTN             Offset
);
VOID
SMI_WRITE_MMIO (
  SMI_PRIVATE_DATA  *Private,
  UINTN                 Offset,
  UINT32                Data
  );

VOID
SMI_WRITE_MMIO_EX (
  SMI_PRIVATE_DATA  *Private,
  UINTN                 Offset,
  UINT32                Data,
  UINT32				Mask
  );
UINT32  
SMI_PCI_READ (
  SMI_PRIVATE_DATA  *Private,
  UINT32                Offset
);

VOID
SMI_PrintRegs (
  SMI_PRIVATE_DATA *Private
  );



//
// GOP Hardware abstraction internal worker functions
//
EFI_STATUS
SmiGraphicsOutputConstructor (
  SMI_PRIVATE_DATA  *Private
  );
/*++

Routine Description:

  TODO: Add function description

Arguments:

  Private - TODO: add argument description

Returns:

  TODO: add return values

--*/

EFI_STATUS
SmiGraphicsOutputDestructor (
  SMI_PRIVATE_DATA  *Private
  );
/*++

Routine Description:

  TODO: Add function description

Arguments:

  Private - TODO: add argument description

Returns:

  TODO: add return values

--*/

//
// EFI 1.1 driver model prototypes for SMI GOP
//
EFI_STATUS
EFIAPI
SmiDriverEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );
/*++

Routine Description:

  TODO: Add function description

Arguments:

  ImageHandle - TODO: add argument description
  SystemTable - TODO: add argument description

Returns:

  TODO: add return values

--*/

//
// EFI_DRIVER_BINDING_PROTOCOL Protocol Interface
//
EFI_STATUS
EFIAPI
SmiControllerDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  );
/*++

Routine Description:

  TODO: Add function description

Arguments:

  This                - TODO: add argument description
  Controller          - TODO: add argument description
  RemainingDevicePath - TODO: add argument description

Returns:

  TODO: add return values

--*/

EFI_STATUS
EFIAPI
SmiControllerDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  );
/*++

Routine Description:

  TODO: Add function description

Arguments:

  This                - TODO: add argument description
  Controller          - TODO: add argument description
  RemainingDevicePath - TODO: add argument description

Returns:

  TODO: add return values

--*/

EFI_STATUS
EFIAPI
SmiControllerDriverStop (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN UINTN                        NumberOfChildren,
  IN EFI_HANDLE                   *ChildHandleBuffer
  );
/*++

Routine Description:

  TODO: Add function description

Arguments:

  This              - TODO: add argument description
  Controller        - TODO: add argument description
  NumberOfChildren  - TODO: add argument description
  ChildHandleBuffer - TODO: add argument description

Returns:

  TODO: add return values

--*/

#endif
