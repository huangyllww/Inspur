/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH DXE lib
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*
*****************************************************************************
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
****************************************************************************
*/
#include <Library/UefiBootServicesTableLib.h>

#include <Library/UefiRuntimeServicesTableLib.h>

#include <Library/FchDxeLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#define FILECODE UEFI_DXE_FCHDXELIB_FCHDXELIB_FILECODE

#define FCH_IOMAP_REGCD6  0xcd6
#define FCH_PMIOA_REG60   0x60


/*----------------------------------------------------------------------------------------*/
/**
 * I/O Read Access
 *
 * @param[in]       EfiWidth     Access width
 * @param[in]       Address      IO address
 * @param[out]      Value        Pointer to data buffer
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchIoRead (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT16                                   Address,
     OUT   VOID                                     *Value
  )
{
  switch (EfiWidth) {
  case EfiPciWidthUint8:
    *(UINT8 *) Value = IoRead8 (Address);
    break;
  case EfiPciWidthUint16:
    *(UINT16 *) Value = IoRead16 (Address);
    break;
  case EfiPciWidthUint32:
    *(UINT32 *) Value = IoRead32 (Address);
    break;
  default:
    ASSERT (FALSE);
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * I/O Write Access
 *
 * @param[in]       EfiWidth     Access width
 * @param[in]       Address      IO address
 * @param[in]       Value        Pointer to data buffer
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchIoWrite (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT16                                   Address,
     OUT   VOID                                     *Value
  )
{
  switch (EfiWidth) {
  case EfiPciWidthUint8:
    IoWrite8 (Address, *(UINT8 *) Value);
    break;
  case EfiPciWidthUint16:
    IoWrite16 (Address, *(UINT16 *) Value);
    break;
  case EfiPciWidthUint32:
    IoWrite32 (Address, *(UINT32 *) Value);
    break;
  default:
    ASSERT (FALSE);
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Memory Read Access
 *
 * @param[in]       EfiWidth     Access width
 * @param[in]       Address      Memory address
 * @param[out]      Value        Pointer to data buffer
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchMemRead (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT32                                   Address,
     OUT   VOID                                     *Value
  )
{
  switch ( EfiWidth ) {
  case EfiPciWidthUint8:
    *((UINT8*)Value) = *((volatile UINT8*) ((UINTN)Address));
    break;

  case EfiPciWidthUint16:
    *((UINT16*)Value) = *((volatile UINT16*) ((UINTN)Address));
    break;

  case EfiPciWidthUint32:
    *((UINT32*)Value) = *((volatile UINT32*) ((UINTN)Address));
    break;

  default:
    ASSERT (FALSE);
    break;
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Memory Write Access
 *
 * @param[in]       EfiWidth     Access width
 * @param[in]       Address      Memory address
 * @param[out]      Value        Pointer to data buffer
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchMemWrite (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT32                                   Address,
  IN       VOID                                     *Value
  )
{
  switch ( EfiWidth ) {
  case EfiPciWidthUint8 :
    *((volatile UINT8*) ((UINTN)Address)) = *((UINT8*)Value);
    break;

  case EfiPciWidthUint16:
    *((volatile UINT16*) ((UINTN)Address)) = *((UINT16*)Value);
    break;

  case EfiPciWidthUint32:
    *((volatile UINT32*) ((UINTN)Address)) = *((UINT32*)Value);
    break;

  default:
    ASSERT (FALSE);
    break;
  }
}


UINT32
ModifyDataByWidth (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN OUT   VOID                                     *Data,
  IN       VOID                                     *AndMask,
  IN       VOID                                     *OrMask
  )
{
  UINT32   TempValue;

  switch (EfiWidth) {
  case EfiPciWidthUint8:
    TempValue = ((UINT32)*(UINT8 *)Data & (UINT32)*(UINT8 *) AndMask) | (UINT32)*(UINT8 *) OrMask;
    break;
  case EfiPciWidthUint16:
    TempValue = ((UINT32)*(UINT16 *)Data & (UINT32)*(UINT16 *) AndMask) | (UINT32)*(UINT16 *) OrMask;
    break;
  case EfiPciWidthUint32:
    TempValue = (*(UINT32 *)Data & *(UINT32 *) AndMask) | *(UINT32 *) OrMask;
    break;
  default:
    ASSERT (FALSE);
    TempValue = 0;
  }
  return TempValue;
}


/*----------------------------------------------------------------------------------------*/
/**
 * I/O Read-Modify-Write Access
 *
 * @param[in]       EfiWidth     Access width
 * @param[in]       Address      IO address
 * @param[in]       *AndMask     Pointer to And Mask
 * @param[in]       *OrMask      Pointer to Or Mask
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchIoRw (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT16                                   Address,
  IN       VOID                                     *AndMask,
  IN       VOID                                     *OrMask
  )
{
  UINT32     FchDxeLibData32;

  LibFchIoRead (EfiWidth, Address, &FchDxeLibData32);
  FchDxeLibData32 = ModifyDataByWidth (EfiWidth, &FchDxeLibData32, AndMask, OrMask);
  LibFchIoWrite (EfiWidth, Address, &FchDxeLibData32);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Memory Read-Modify-Write Access
 *
 * @param[in]       EfiWidth     Access width
 * @param[in]       Address      IO address
 * @param[in]       *AndMask     Pointer to And Mask
 * @param[in]       *OrMask      Pointer to Or Mask
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchMemRw (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT32                                   Address,
  IN       VOID                                     *AndMask,
  IN       VOID                                     *OrMask
  )
{
  UINT32     FchDxeLibData32;

  LibFchMemRead (EfiWidth, Address, &FchDxeLibData32);
  FchDxeLibData32 = ModifyDataByWidth (EfiWidth, &FchDxeLibData32, AndMask, OrMask);
  LibFchMemWrite (EfiWidth, Address, &FchDxeLibData32);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Indirect I/O Read Access
 *
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchIndirectIoRead (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT16                                   IoBase,
  IN       UINT8                                    IndexAddress,
     OUT   VOID                                     *Value
  )
{
  UINT8    i;
  UINT8    ByteCount;

  ByteCount = (UINT8) EfiWidth + 1;
  for (i = 0; i < ByteCount; i++, IndexAddress++) {
    LibFchIoWrite (EfiPciWidthUint8, IoBase, &IndexAddress);
    LibFchIoRead (EfiPciWidthUint8, IoBase + 1, (UINT8 *)Value + i);
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Indirect I/O Write Access
 *
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchIndirectIoWrite (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT16                                   IoBase,
  IN       UINT8                                    IndexAddress,
  IN       VOID                                     *Value
  )
{
  UINT8    i;
  UINT8    ByteCount;

  ByteCount = (UINT8) EfiWidth + 1;
  for (i = 0; i < ByteCount; i++, IndexAddress++) {
    LibFchIoWrite (EfiPciWidthUint8, IoBase, &IndexAddress);
    LibFchIoWrite (EfiPciWidthUint8, IoBase + 1, (UINT8 *)Value + i);
  }
}


VOID
LibFchGetAcpiMmioBase (
  OUT   UINT32         *AcpiMmioBase
  )
{
  UINT32    Value32;

  Value32 = 0xFED80000;
  *AcpiMmioBase = Value32;
}


VOID
LibFchGetAcpiPmBase (
  OUT   UINT16         *AcpiPmBase
  )
{
  LibFchIndirectIoRead (EfiPciWidthUint16, FCH_IOMAP_REGCD6, FCH_PMIOA_REG60, AcpiPmBase);
}



/*----------------------------------------------------------------------------------------*/
/**
 * PCI Read Access
 *
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchPciRead (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT32                                   Address,
  IN       VOID                                     *Value
  )
{
  UINTN  PciAddress;

  PciAddress = ((Address >> 4) & ~0xFFF) + (Address & 0xFFF);

  switch (EfiWidth) {
  case EfiPciWidthUint8:
    *((UINT8 *) Value) = PciRead8 (PciAddress);
    break;
  case EfiPciWidthUint16:
    *((UINT16 *) Value) = PciRead16 (PciAddress);
    break;
  case EfiPciWidthUint32:
    *((UINT32 *) Value) = PciRead32 (PciAddress);
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * PCI Write Access
 *
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibFchPciWrite (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT32                                   Address,
  IN       VOID                                     *Value
  )
{
  UINTN  PciAddress;

  PciAddress = ((Address >> 4) & ~0xFFF) + (Address & 0xFFF);

  switch (EfiWidth) {
  case EfiPciWidthUint8:
    PciWrite8 (PciAddress, *((UINT8 *) Value));
    break;
  case EfiPciWidthUint16:
    PciWrite16 (PciAddress, *((UINT16 *) Value));
    break;
  case EfiPciWidthUint32:
    PciWrite32 (PciAddress, *((UINT32 *) Value));
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}





