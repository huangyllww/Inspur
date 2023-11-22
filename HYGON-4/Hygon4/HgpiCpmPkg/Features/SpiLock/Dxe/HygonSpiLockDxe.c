/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPM GPIO Initialization
 *
 * Contains CPM code to perform GPIO Init under DXE
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  GpioInit
 *
 */
/*****************************************************************************
 *
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 *
 ******************************************************************************
 */

#include <HygonCpmDxe.h>
#include <Protocol/SmmCommunication.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Library/DebugLib.h>
#include <Fch.h>

EFI_STATUS
DxeSpiLock (
  IN  UINT32    LockBaseAddr,
  IN  UINT16    LockSize,
  IN  UINT8     LockUnit,
  IN  UINT8     RangeSelecter
  )
{
  FCH_LCP_ROM_PROTECT_REG  SpiLockReg;
  UINT16                   UnitCount;
  UINT16                   UnitSelect = 64;

  DEBUG ((EFI_D_INFO, "DxeSpiLock Entry\n"));
  // protect range 0
  if(LockUnit == 0) {
    UnitSelect = 4;         // 0:4KB 1:64KB
  } else if(LockUnit == 1) {
    UnitSelect = 64;
  } else {
    return EFI_INVALID_PARAMETER;
  }

  UnitCount = (LockSize/UnitSelect)-1;
  SpiLockReg.Field.RomBase      = LockBaseAddr >> 12;
  SpiLockReg.Field.Range        = (UINT8)UnitCount;
  SpiLockReg.Field.RangeUnit    = LockUnit;
  SpiLockReg.Field.WriteProtect = 1;
  DEBUG ((EFI_D_INFO, "RomBase %x  Rang %x  RangUnit %x\n", SpiLockReg.Field.RomBase, SpiLockReg.Field.Range, SpiLockReg.Field.RangeUnit));
  switch(RangeSelecter) {
    case 0:
      DEBUG ((EFI_D_INFO, "SpiLockReg.LpcRomProtect %x", SpiLockReg.LpcRomProtect));
      PciWrite32 ((LPC_BUS_DEV_FUN << 12) + FCH_LPC_REG50, SpiLockReg.LpcRomProtect);
      break;
    case 1:
      PciWrite32 ((LPC_BUS_DEV_FUN << 12) + FCH_LPC_REG54, SpiLockReg.LpcRomProtect);
      break;
    case 2:
      PciWrite32 ((LPC_BUS_DEV_FUN << 12) + FCH_LPC_REG58, SpiLockReg.LpcRomProtect);
      break;
    case 3:
      PciWrite32 ((LPC_BUS_DEV_FUN << 12) + FCH_LPC_REG5C, SpiLockReg.LpcRomProtect);
      break;
    default:
      break;
  }

  DEBUG ((EFI_D_INFO, "DxeSpiLock Exit\n"));
  return EFI_SUCCESS;
}

EFI_STATUS
SpiLockInitEntryPoint (
  IN               EFI_HANDLE             ImageHandle,
  IN               EFI_SYSTEM_TABLE       *SystemTable

  )
{
  SPI_LOCK_PROTOCOL  *FchSpiLockProtocol = NULL;
  EFI_HANDLE         Handle;
  EFI_STATUS         Status = EFI_SUCCESS;

  if(!PcdGetBool (PcdFchSpiLockSupport)) {
    return EFI_SUCCESS;
  }

  Status = gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  sizeof (SPI_LOCK_PROTOCOL),
                  &FchSpiLockProtocol
                  );

  if (EFI_ERROR (Status)) {
    return (Status);
  }

  FchSpiLockProtocol->Revision = 0x01;
  FchSpiLockProtocol->SpiLock  = DxeSpiLock;
  Handle = ImageHandle;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gFchSpiLockGuid,
                  EFI_NATIVE_INTERFACE,
                  FchSpiLockProtocol
                  );

  if (EFI_ERROR (Status)) {
    return (Status);
  }

  return Status;
}
