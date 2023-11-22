/*++

Copyright (c) 2004, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  Smi.h
    
Abstract:

  SMI Controller Driver

Revision History

--*/

//
// SMI Controller Driver
//

#ifndef _SMI_H_
#define _SMI_H_

#include "Tiano.h"
#include "Pci22.h"
#include "BuildFlag.h"
#include "EfiDriverLib.h"
#include EFI_PROTOCOL_DEFINITION (DriverBinding)
#include EFI_PROTOCOL_DEFINITION (DevicePath)
#include EFI_PROTOCOL_DEFINITION (PciIo)
#include EFI_PROTOCOL_DEFINITION (UgaDraw)

//
// SMI PCI Configuration Header values
//
#define SILICON_MOTION_VENDOR_ID            0x126F
#define SMI_DEVICE_ID                       0x0750

//#define SMI_ALTERNATE_DEVICE_ID 0x00a0
#define		PCI_BAR_FRAMEBUFFER		0
#define		PCI_BAR_MMIO			1

//
// SMI Graphical Mode Data
//
#define SMI_UGA_DRAW_MODE_COUNT 8

typedef struct {
  UINT32  HorizontalResolution;
  UINT32  VerticalResolution;
  UINT32  ColorDepth;
  UINT32  RefreshRate;
} SMI_UGA_DRAW_MODE_DATA;

//
// SMI Private Data Structure
//
#define SMI_PRIVATE_DATA_SIGNATURE  EFI_SIGNATURE_32 ('S', 'M', '5', '0')

typedef struct {
  UINT64                                Signature;
  EFI_HANDLE                            Handle;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  EFI_UGA_DRAW_PROTOCOL                 UgaDraw;

  //
  // UGA Draw Private Data
  //
  BOOLEAN                               HardwareNeedsStarting;
  UINTN                                 CurrentMode;
  UINTN                                 MaxMode;
  SMI_UGA_DRAW_MODE_DATA  ModeData[SMI_UGA_DRAW_MODE_COUNT];
  UINT8                                 *LineBuffer;
} SMI_PRIVATE_DATA;

#define SMI_PRIVATE_DATA_FROM_UGA_DRAW_THIS(a) \
  CR(a, SMI_PRIVATE_DATA, UgaDraw, SMI_PRIVATE_DATA_SIGNATURE)

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
// UGA Draw Hardware abstraction internal worker functions
//
EFI_STATUS
SmiUgaDrawConstructor (
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
;

EFI_STATUS
SmiUgaDrawDestructor (
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
;

//
// EFI 1.1 driver model prototypes for SMI UGA Draw
//
EFI_STATUS
EFIAPI
SmiDriverEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  ImageHandle - TODO: add argument description
  SystemTable - TODO: add argument description

Returns:

  TODO: add return values

--*/
;

//
// EFI_DRIVER_BINDING_PROTOCOL Protocol Interface
//
EFI_STATUS
EFIAPI
SmiControllerDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
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
;

EFI_STATUS
EFIAPI
SmiControllerDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
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
;

EFI_STATUS
EFIAPI
SmiControllerDriverStop (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN UINTN                        NumberOfChildren,
  IN EFI_HANDLE                   *ChildHandleBuffer
  )
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
;

#endif
