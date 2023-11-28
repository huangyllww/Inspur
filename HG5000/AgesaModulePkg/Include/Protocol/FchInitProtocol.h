/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH DXE
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH DXE
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 ******************************************************************************
 */

#ifndef _FCH_INIT_PROTOCOL_H_
#define _FCH_INIT_PROTOCOL_H_


extern EFI_GUID gFchInitProtocolGuid;

typedef struct _FCH_INIT_PROTOCOL FCH_INIT_PROTOCOL;
/// FCH USB Over Current Data Block
typedef struct {
  UINT32       ChipType;                           ///< Bitfield: Applied chip types
  UINT8        Usb1OcPinMap[5];       ///< USB1 over current pin mapping
  UINT8        Usb2OcPinMap[5];       ///< USB2 over current pin mapping
  UINT8        Usb3OcPinMap[4];       ///< USB3 over current pin mapping
  UINT8        Usb4OcPinMap[2];       ///< USB4 over current pin mapping
  UINT8        Xhci0OcPinMap[2];      ///< XHCI0 over current pin mapping
  UINT8        Xhci1OcPinMap[2];      ///< XHCI1 over current pin mapping
} USB_OC;


//
// Protocol prototypes
//
typedef
VOID
(EFIAPI *FP_FCH_USB_OC) (
  USB_OC      *UsbOc
  );

//
// Rev2 for USB OC mapping after ZP
//
typedef EFI_STATUS (EFIAPI *FP_FCH_USB_OC2) (
  IN       CONST FCH_INIT_PROTOCOL   *This,              ///< FCH INIT Protocol
  IN       UINT8                     Socket,             ///< CPU/Socket number in system
  IN       UINT8                     Port,               ///< Bitmap (Bit0 - Port0, Bit1 - Port1, etc.) to disable USB3 ports
  IN       UINT8                     OCPin               ///< Bitmap (Bit0 - Port0, Bit1 - Port1, etc.) to disable USB2 ports
);

//
// USB port disable function
//
typedef EFI_STATUS (EFIAPI *FP_FCH_USB_PORT_DISABLE2) (
  IN       CONST FCH_INIT_PROTOCOL   *This,              ///< FCH INIT Protocol
  IN       UINT8                     Socket,             ///< CPU/Socket number in system
  IN       UINT32                    USB3DisableMap,     ///< Bitmap (Bit0 - Port0, Bit1 - Port1, etc.) to disable USB3 ports
  IN       UINT32                    USB2DisableMap      ///< Bitmap (Bit0 - Port0, Bit1 - Port1, etc.) to disable USB2 ports
);


/// FCH INIT Protocol
typedef struct _FCH_INIT_PROTOCOL {
  UINTN                     Revision;                 ///< Protocol Revision
  UINTN                     FchRev;                   ///< FCH Revision
  VOID                      *FchPolicy;               ///< Fch Config Data Block
  VOID                      *FchPtPolicy;             ///< PT Data Block
  FP_FCH_USB_OC             FpUsbOverCurrent;         ///< Service: USB Over Current
  FP_FCH_USB_OC2            FpUsbOverCurrent2;        ///< Service: USB Over Current2
  FP_FCH_USB_PORT_DISABLE2  FpUsbPortDisable2;        ///< Service: USB Port Disable2
} FCH_INIT_PROTOCOL;

// current PPI revision
#define FCH_INIT_REV  0x01

#endif // _FCH_INIT_PROTOCOL_H_
