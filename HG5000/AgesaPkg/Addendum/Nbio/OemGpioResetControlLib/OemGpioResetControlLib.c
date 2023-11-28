/* $NoKeywords:$ */
/**
 * @file
 *
 * Platform Gpio Reset Control Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdGpioResetControl
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
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
#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Library/PeiServicesLib.h>
#include <AMD.h>
#include <Library/OemGpioResetControlLib.h>
/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Local delay function
 *
 *
 *
 * @param[in]  PeiServices    Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  xus            Microseconds to delay
 *
 */
VOID
STATIC
delay_xus (
  IN  UINT32    xus
  )
{
  UINTN               uSec;
  PCI_ADDR            GnbPciAddress;
  UINT32              Value;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Register = 0xB8;
  //Simplified implementation
  uSec = xus << 2;
  while (uSec != 0) {
    Value = PciRead32 ((UINTN) GnbPciAddress.AddressValue);
    uSec--;
  }
}

#define US_TO_REFCLK(x) x
#define SysTimeDelay(x)   delay_xus(PeiServices, x)

/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to write all register spaces.
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[in]  Value             The value to write
 */
UINT32
SmnRegisterRead (
  IN       UINT32              BusNumber,
  IN       UINT32              Address
  )
{

  PCI_ADDR      GnbPciAddress;
  UINT32        Value;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Bus = BusNumber;

  GnbPciAddress.Address.Register = 0xB8;
  PciWrite32 ((UINTN) GnbPciAddress.AddressValue, Address);

  GnbPciAddress.Address.Register = 0xBC;
  Value = PciRead32 ((UINTN) GnbPciAddress.AddressValue);
  return Value;
}

/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to write all register spaces.
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[in]  Value             The value to write
 */
VOID
SmnRegisterWrite (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       UINT32              Value
  )
{
  PCI_ADDR      GnbPciAddress;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Bus = BusNumber;

  GnbPciAddress.Address.Register = 0xB8;
  PciWrite32 ((UINTN) GnbPciAddress.AddressValue, Address);

  GnbPciAddress.Address.Register = 0xBC;
  PciWrite32 ((UINTN) GnbPciAddress.AddressValue, Value);

  return;
}


/*----------------------------------------------------------------------------------------*/
/*
 *  Routine (byte width) to write all register spaces.
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[in]  Value             The value to write
 */
UINT8
SmnRegisterRead8 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address
  )
{

  PCI_ADDR      GnbPciAddress;
  UINT8        Value;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Bus = BusNumber;

  GnbPciAddress.Address.Register = 0xB8;
  PciWrite32 ((UINTN) GnbPciAddress.AddressValue, Address);

  GnbPciAddress.Address.Register = 0xBC + (Address & 0x3);
  Value = PciRead8 ((UINTN) GnbPciAddress.AddressValue);
  return Value;
}

/*----------------------------------------------------------------------------------------*/
/*
 *  Routine (byte width) to write all register spaces.
 *
 *
 * @param[in]  BusNumber         Bus number of D0F0 of the target die
 * @param[in]  Address           Register offset, but PortDevice
 * @param[in]  Value             The value to write
 */
VOID
SmnRegisterWrite8 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       UINT8               Value
  )
{
  PCI_ADDR      GnbPciAddress;

  GnbPciAddress.AddressValue = 0;
  GnbPciAddress.Address.Bus = BusNumber;

  GnbPciAddress.Address.Register = 0xB8;
  PciWrite32 ((UINTN) GnbPciAddress.AddressValue, Address);

  GnbPciAddress.Address.Register = 0xBC + (Address & 0x3);
  PciWrite8 ((UINTN) GnbPciAddress.AddressValue, Value);

  return;
}


/**
 Gpio reset control.

 @param[in]         FcnData             Function data
 @param[in]         ResetInfo           Reset information

 @retval            EFI_SUCCESS       Function returns successfully
 @retval            EFI_UNSUPPORTED   Function is not supported
*/
EFI_STATUS
AgesaGpioSlotResetControl (
  IN      UINTN                     FcnData,
  IN      GPIO_RESET_INFO           *ResetInfo
  )
{
  UINT32    BusNumber;

  BusNumber = (UINT32) FcnData;

  // In Agesa or platform bios, revert the setting back to HW default, before calling dxio_fw to do PCIe training
  // Address: 0x2D02568 = 0xD20000 (Assert High)
  SmnRegisterWrite (BusNumber, 0x2D02568, 0xD20000);

  // Set back to PCIe_RESET function
  SmnRegisterWrite8 (BusNumber, 0x2d01d1a, 0);

  return EFI_SUCCESS;
}

