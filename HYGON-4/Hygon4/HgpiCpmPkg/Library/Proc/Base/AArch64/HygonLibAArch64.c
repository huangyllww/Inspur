/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPM library 64bit for IO Access
 *
 * Contains HYGON CPM Library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  Library
 */
/*
 ******************************************************************************
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

#include <HygonCpmBase.h>

UINT8
CpmIoRead8 (
  IN      UINT16 Address
  )
{
  UINT64  X86IoRemapBase;
  UINT8   Data;

  X86IoRemapBase = CFG_CPM_X86_IO_REMAP_BASE;
  // No configuration space access supported
  // ASSERT (!(Address == 0xCF9) && (Address >= 0xCF8) && (Address <= 0xCFF));
  if (!(Address == 0xCF9) && (Address >= 0xCF8) && (Address <= 0xCFF)) {
    Data = 0;
  } else {
    Data = *(volatile UINT8 *)(UINT64)(X86IoRemapBase + Address);
  }

  return Data;
}

UINT16
CpmIoRead16 (
  IN      UINT16 Address
  )
{
  UINT64  X86IoRemapBase;
  UINT16  Data;

  X86IoRemapBase = CFG_CPM_X86_IO_REMAP_BASE;
  // No configuration space access supported
  // ASSERT (!(Address == 0xCF9) && (Address >= 0xCF8) && (Address <= 0xCFF));
  if (!(Address == 0xCF9) && (Address >= 0xCF8) && (Address <= 0xCFF)) {
    Data = 0;
  } else {
    Data = *(volatile UINT16 *)(UINT64)(X86IoRemapBase + Address);
  }

  return Data;
}

UINT32
CpmIoRead32 (
  IN      UINT16 Address
  )
{
  UINT64  X86IoRemapBase;
  UINT32  Data;

  X86IoRemapBase = CFG_CPM_X86_IO_REMAP_BASE;
  // No configuration space access supported
  // ASSERT (!(Address == 0xCF9) && (Address >= 0xCF8) && (Address <= 0xCFF));
  if (!(Address == 0xCF9) && (Address >= 0xCF8) && (Address <= 0xCFF)) {
    Data = 0;
  } else {
    Data = *(volatile UINT32 *)(UINT64)(X86IoRemapBase + Address);
  }

  return Data;
}

VOID
CpmIoWrite8 (
  IN      UINT16 Address,
  IN      UINT8 Data
  )
{
  UINT64  X86IoRemapBase;

  X86IoRemapBase = CFG_CPM_X86_IO_REMAP_BASE;
  // No configuration space access supported
  // ASSERT (!(Address == 0xCF9) && (Address >= 0xCF8) && (Address <= 0xCFF));
  if (!(Address == 0xCF9) && (Address >= 0xCF8) && (Address <= 0xCFF)) {
    // Do nothing
  } else {
    *(volatile UINT8 *)(UINT64)(X86IoRemapBase + Address) = Data;
  }
}

VOID
CpmIoWrite16 (
  IN      UINT16 Address,
  IN      UINT16 Data
  )
{
  UINT64  X86IoRemapBase;

  X86IoRemapBase = CFG_CPM_X86_IO_REMAP_BASE;
  // No configuration space access supported
  // ASSERT (!(Address == 0xCF9) && (Address >= 0xCF8) && (Address <= 0xCFF));
  if (!(Address == 0xCF9) && (Address >= 0xCF8) && (Address <= 0xCFF)) {
    // Do nothing
  } else {
    *(volatile UINT16 *)(UINT64)(X86IoRemapBase + Address) = Data;
  }
}

VOID
CpmIoWrite32 (
  IN      UINT16 Address,
  IN      UINT32 Data
  )
{
  UINT64  X86IoRemapBase;

  X86IoRemapBase = CFG_CPM_X86_IO_REMAP_BASE;
  // No configuration space access supported
  // ASSERT (!(Address == 0xCF9) && (Address >= 0xCF8) && (Address <= 0xCFF));
  if (!(Address == 0xCF9) && (Address >= 0xCF8) && (Address <= 0xCFF)) {
    // Do nothing
  } else {
    *(volatile UINT32 *)(UINT64)(X86IoRemapBase + Address) = Data;
  }
}

UINT64
CpmReadTsc (
  )
{
  UINT32  Tsc_Low;
  UINT32  Tsc_High;
  UINT64  PciMmioBase;
  UINT64  Data;

  PciMmioBase = CFG_CPM_PCI_MMIO_BASE;
  Tsc_Low     = *(volatile UINT32 *)(UINT64)(PciMmioBase + 0x000C3130);
  Tsc_High    = *(volatile UINT32 *)(UINT64)(PciMmioBase + 0x000C3134);
  Data = (((UINT64)Tsc_High) << 32) + (UINT64)Tsc_Low;
  return Data;
}

VOID
CpmCpuidRawRead (
  IN       UINT32 CpuidFcnAddress,
  OUT   CPUID_DATA *Value
  )
{
  UINT64  Address;

  if (Value) {
    Value->EAX_Reg = 0;
    Value->EBX_Reg = 0;
    Value->ECX_Reg = 0;
    Value->EDX_Reg = 0;
    switch (CpuidFcnAddress) {
      case 0x80000001:
        Address = (UINT32)CFG_CPM_PCI_MMIO_BASE;
        Value->EAX_Reg = *(volatile UINT32 *)(UINT64)(Address + 0x000C30FC);
        Value->EBX_Reg = (*(volatile UINT32 *)(UINT64)(Address + 0x000C30EC) & 0x00000700) << 20;
        break;
    }
  }
}

UINT64
CpmMsrRead (
  IN      UINT32 MsrAddress
  )
{
  UINT64  Value;

  Value = 0x0BAD0BAD0BAD0BADull;
  switch (MsrAddress) {
    case 0x0000001B:
      // Simulate MSR_IA32_APIC_BASE[8]
      Value = 0x0000000000000100ull;
      break;
    case 0xC0001015:
      // Simulate MSR_HWCR[24]: TscFreqSel
      Value = 0x0000000001000000ull;
      break;
    case 0xC0010058:
      // Simulate MMIO Configuration Base Address
      Value = CFG_CPM_PCI_MMIO_BASE;
      break;
  }

  return Value;
}

UINT64
CpmMsrWrite (
  IN       UINT32 MsrAddress,
  IN       UINT64 Value
  )
{
  return 0x0BAD0BAD0BAD0BADull;
}
