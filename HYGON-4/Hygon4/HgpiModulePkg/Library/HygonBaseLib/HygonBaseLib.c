/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Library
 *
 * Contains interface to the HYGON HGPI library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Lib
 *
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Filecode.h>
#include <Library/HygonBaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE           LIBRARY_HYGONBASELIB_HYGONBASELIB_FILECODE
#define NB_CFG             0xC001001Ful
#define MSR_MMIO_Cfg_Base  0xC0010058ul             // MMIO Configuration Base Address Register

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/

/**
 * Read IO port
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] IoAddress     IO port address
 * @param[in] Value         Pointer to save data
 * @param[in] StdHeader     Standard configuration header
 *
 */
VOID
LibHygonIoRead (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
  OUT   VOID *Value,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  switch (AccessWidth) {
    case AccessWidth8:
    case AccessS3SaveWidth8:
      *(UINT8 *)Value = IoRead8 (IoAddress);
      break;
    case AccessWidth16:
    case AccessS3SaveWidth16:
      *(UINT16 *)Value = IoRead16 (IoAddress);
      break;
    case AccessWidth32:
    case AccessS3SaveWidth32:
      *(UINT32 *)Value = IoRead32 (IoAddress);
      break;
    default:
      ASSERT (FALSE);
  }
}

/*---------------------------------------------------------------------------------------*/

/**
 * Write IO port
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] IoAddress     IO port address
 * @param[in] Value         Pointer to data
 * @param[in] StdHeader     Standard configuration header
 *
 */
VOID
LibHygonIoWrite (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
  IN       VOID *Value,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  switch (AccessWidth) {
    case AccessWidth8:
    case AccessS3SaveWidth8:
      IoWrite8 (IoAddress, *(UINT8 *)Value);
      break;
    case AccessWidth16:
    case AccessS3SaveWidth16:
      IoWrite16 (IoAddress, *(UINT16 *)Value);
      break;
    case AccessWidth32:
    case AccessS3SaveWidth32:
      IoWrite32 (IoAddress, *(UINT32 *)Value);
      break;
    default:
      ASSERT (FALSE);
  }
}

/*---------------------------------------------------------------------------------------*/

/**
 * IO read modify write
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] IoAddress     IO address
 * @param[in] Data          OR data
 * @param[in] DataMask      Mask to be used before data write back to register.
 * @param[in] StdHeader     Standard configuration header
 *
 */
VOID
LibHygonIoRMW (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
  IN       VOID *Data,
  IN       VOID *DataMask,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  TempData;
  UINT32  TempMask;
  UINT32  Value;

  LibHygonGetDataFromPtr (AccessWidth, Data, DataMask, &TempData, &TempMask);
  LibHygonIoRead (AccessWidth, IoAddress, &Value, StdHeader);
  Value = (Value & (~TempMask)) | TempData;
  LibHygonIoWrite (AccessWidth, IoAddress, &Value, StdHeader);
}

/*---------------------------------------------------------------------------------------*/

/**
 * Poll IO register
 *
 *  Poll register until (RegisterValue & DataMask) ==  Data
 *
 * @param[in] AccessWidth   Access width
 * @param[in] IoAddress     IO address
 * @param[in] Data          Data to compare
 * @param[in] DataMask      And mask
 * @param[in] Delay         Poll for time in 100ns (not supported)
 * @param[in] StdHeader     Standard configuration header
 *
 */
VOID
LibHygonIoPoll (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
  IN       VOID *Data,
  IN       VOID *DataMask,
  IN       UINT64 Delay,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  TempData;
  UINT32  TempMask;
  UINT32  Value;

  LibHygonGetDataFromPtr (AccessWidth, Data, DataMask, &TempData, &TempMask);
  do {
    LibHygonIoRead (AccessWidth, IoAddress, &Value, StdHeader);
  } while (TempData != (Value & TempMask));
}

/*---------------------------------------------------------------------------------------*/

/**
 * Read memory/MMIO
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] MemAddress    Memory address
 * @param[in] Value         Pointer to data
 * @param[in] StdHeader     Standard configuration header
 *
 */
VOID
LibHygonMemRead (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
  OUT   VOID *Value,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  if (sizeof (UINTN) == sizeof (UINT32)) {
    ASSERT (MemAddress <= 0xFFFFFFFF);
  }

  switch (AccessWidth) {
    case AccessWidth8:
    case AccessS3SaveWidth8:
      *(UINT8 *)Value = MmioRead8 ((UINTN)MemAddress);
      break;
    case AccessWidth16:
    case AccessS3SaveWidth16:
      *(UINT16 *)Value = MmioRead16 ((UINTN)MemAddress);
      break;
    case AccessWidth32:
    case AccessS3SaveWidth32:
      *(UINT32 *)Value = MmioRead32 ((UINTN)MemAddress);
      break;
    default:
      ASSERT (FALSE);
  }
}

/*---------------------------------------------------------------------------------------*/

/**
 * Write memory/MMIO
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] MemAddress    Memory address
 * @param[in] Value         Pointer to data
 * @param[in] StdHeader     Standard configuration header
 *
 */
VOID
LibHygonMemWrite (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
  IN       VOID *Value,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  if (sizeof (UINTN) == sizeof (UINT32)) {
    ASSERT (MemAddress <= 0xFFFFFFFF);
  }

  switch (AccessWidth) {
    case AccessWidth8:
    case AccessS3SaveWidth8:
      MmioWrite8 ((UINTN)MemAddress, *((UINT8 *)Value));
      break;
    case AccessWidth16:
    case AccessS3SaveWidth16:
      MmioWrite16 ((UINTN)MemAddress, *((UINT16 *)Value));
      break;
    case AccessWidth32:
    case AccessS3SaveWidth32:
      MmioWrite32 ((UINTN)MemAddress, *((UINT32 *)Value));
      break;
    default:
      ASSERT (FALSE);
  }
}

/*---------------------------------------------------------------------------------------*/

/**
 * Memory/MMIO read modify write
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] MemAddress    Memory address
 * @param[in] Data          OR data
 * @param[in] DataMask      Mask to be used before data write back to register.
 * @param[in] StdHeader     Standard configuration header
 *
 */
VOID
LibHygonMemRMW (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
  IN       VOID *Data,
  IN       VOID *DataMask,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  TempData;
  UINT32  TempMask;
  UINT32  Value;

  LibHygonGetDataFromPtr (AccessWidth, Data, DataMask, &TempData, &TempMask);
  LibHygonMemRead (AccessWidth, MemAddress, &Value, StdHeader);
  Value = (Value & (~TempMask)) | TempData;
  LibHygonMemWrite (AccessWidth, MemAddress, &Value, StdHeader);
}

/*---------------------------------------------------------------------------------------*/

/**
 * Poll Mmio
 *
 *  Poll register until (RegisterValue & DataMask) ==  Data
 *
 * @param[in] AccessWidth   Access width
 * @param[in] MemAddress    Memory address
 * @param[in] Data          Data to compare
 * @param[in] DataMask      AND mask
 * @param[in] Delay         Poll for time in 100ns (not supported)
 * @param[in] StdHeader     Standard configuration header
 *
 */
VOID
LibHygonMemPoll (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
  IN       VOID *Data,
  IN       VOID *DataMask,
  IN       UINT64 Delay,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  TempData;
  UINT32  TempMask;
  UINT32  Value;

  LibHygonGetDataFromPtr (AccessWidth, Data, DataMask, &TempData, &TempMask);
  do {
    LibHygonMemRead (AccessWidth, MemAddress, &Value, StdHeader);
  } while (TempData != (Value & TempMask));
}

/*---------------------------------------------------------------------------------------*/

/**
 * Get MMIO base address for PCI accesses
 *
 * @param[out] MmioAddress   PCI MMIO base address
 * @param[out] MmioSize      Size of region in bytes
 * @param[in]  StdHeader     Standard configuration header
 *
 * @retval    TRUE          MmioAddress/MmioSize are valid
 */
BOOLEAN
GetPciMmioAddress (
  OUT   UINT64            *MmioAddress,
  OUT   UINT32            *MmioSize,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  BOOLEAN  MmioIsEnabled;
  UINT32   EncodedSize;
  UINT64   LocalMsrRegister;

  MmioIsEnabled    = FALSE;
  LocalMsrRegister = AsmReadMsr64 (MSR_MMIO_Cfg_Base);
  if ((LocalMsrRegister & BIT0) != 0) {
    *MmioAddress  = LocalMsrRegister & 0xFFFFFFFFFFF00000;
    EncodedSize   = (UINT32)RShiftU64 ((LocalMsrRegister & 0x3C), 2);
    *MmioSize     = ((1 << EncodedSize) * 0x100000);
    MmioIsEnabled = TRUE;
  }

  return MmioIsEnabled;
}

/*---------------------------------------------------------------------------------------*/

/**
 * Locate next capability pointer
 *
 *  Given a SBDFO this routine will find the next PCI capabilities list entry.
 *  if the end of the list is reached, or if a problem is detected, then ILLEGAL_SBDFO is
 *  returned.
 *  To start a new search from the head of the list, specify a SBDFO with an offset of zero.
 *
 * @param[in,out] Address       Pci address
 * @param[in]     StdHeader     Standard configuration header
 */
VOID
LibHygonPciFindNextCap (
  IN OUT   PCI_ADDR *Address,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  PCI_ADDR  Base;
  UINT32    Offset;
  UINT32    Temp;
  PCI_ADDR  TempAddress;

  ASSERT (Address != NULL);
  ASSERT (*(UINT32 *)Address != ILLEGAL_SBDFO);

  Base.AddressValue = Address->AddressValue;
  Offset = Base.Address.Register;
  Base.Address.Register = 0;

  Address->AddressValue = (UINT32)ILLEGAL_SBDFO;

  // Verify that the SBDFO points to a valid PCI device SANITY CHECK
  Temp = PciRead32 (Base.AddressValue);
  if (Temp == 0xFFFFFFFF) {
    ASSERT (FALSE);
    return; // There is no device at this address
  }

  // Verify that the device supports a capability list
  TempAddress.AddressValue = Base.AddressValue + 0x04;
  Temp = PciBitFieldRead32 (TempAddress.AddressValue, 20, 20);
  if (Temp == 0) {
    return; // This PCI device does not support capability lists
  }

  if (Offset != 0) {
    // If we are continuing on an existing list
    TempAddress.AddressValue = Base.AddressValue + Offset;
    Temp = PciBitFieldRead32 (TempAddress.AddressValue, 8, 15);
  } else {
    // We are starting on a new list
    TempAddress.AddressValue = Base.AddressValue + 0x34;
    Temp = PciBitFieldRead32 (TempAddress.AddressValue, 0, 7);
  }

  if (Temp == 0) {
    return; // We have reached the end of the capabilities list
  }

  // Error detection and recovery- The statement below protects against
  // PCI devices with broken PCI capabilities lists.  Detect a pointer
  // that is not uint32 aligned, points into the first 64 reserved DWORDs
  // or points back to itself.
  if (((Temp & 3) != 0) || (Temp == Offset) || (Temp < 0x40)) {
    ASSERT (FALSE);
    return;
  }

  Address->AddressValue = Base.AddressValue + Temp;
  return;
}

/*---------------------------------------------------------------------------------------*/

/**
 *  Insert SMBIOS NULL terminated string at Destination address
 *
 * @param[in,out] Destination   Pointer to Destination string pointer
 * @param[in]     DestMax       Pointer to max size of buffer
 * @param[in]     Source        Pointer to Source String
 *
 * @retval        EFI_STATUS    Status of placing string
 *
 */
EFI_STATUS
EFIAPI
LibHygonInsertSmbiosString (
  IN OUT   CHAR8       **Destination,
  IN       UINTN       *DestMax,
  IN       CONST CHAR8 *Source
  )
{
  EFI_STATUS  Status;

  if ((Source != NULL) && (Source[0] != 0)) {
    Status = AsciiStrCatS (*Destination, *DestMax, Source);
    *Destination += AsciiStrSize (Source);
    *DestMax     -= AsciiStrSize (Source);
  } else {
    Status = AsciiStrCpyS (*Destination, *DestMax, "Unknown");
    *Destination += AsciiStrSize ("Unknown");
    *DestMax     -= AsciiStrSize ("Unknown");
  }

  return Status;
}

/*---------------------------------------------------------------------------------------*/

/**
 * Set memory with value
 *
 *
 * @param[in,out] Destination   Pointer to memory range
 * @param[in]     Value         Value to set memory with
 * @param[in]     FillLength    Size of the memory range
 * @param[in]     StdHeader     Standard configuration header (Optional)
 */
VOID
LibHygonMemFill (
  IN       VOID *Destination,
  IN       UINT8 Value,
  IN       UINTN FillLength,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  SetMem (Destination, FillLength, Value);
}

/*---------------------------------------------------------------------------------------*/

/**
 * Copy memory
 *
 *
 * @param[in,out] Destination   Pointer to destination buffer
 * @param[in]     Source        Pointer to source buffer
 * @param[in]     CopyLength    buffer length
 * @param[in]     StdHeader     Standard configuration header (Optional)
 */
VOID
LibHygonMemCopy (
  IN       VOID *Destination,
  IN       VOID *Source,
  IN       UINTN CopyLength,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  CopyMem (Destination, Source, CopyLength);
}

/*---------------------------------------------------------------------------------------*/

/**
 * Returns the package type mask for the processor
 *
 *
 * @param[in]     AccessWidth     Access width
 * @param[in]     Data            data
 * @param[in]     DataMask        data
 * @param[out]    TemData         typecast data
 * @param[out]    TempDataMask    typecast data
 */
VOID
LibHygonGetDataFromPtr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       VOID         *Data,
  IN       VOID         *DataMask,
  OUT   UINT32       *TemData,
  OUT   UINT32       *TempDataMask
  )
{
  switch (AccessWidth) {
    case AccessWidth8:
    case AccessS3SaveWidth8:
      *TemData = (UINT32)*(UINT8 *)Data;
      *TempDataMask = (UINT32)*(UINT8 *)DataMask;
      break;
    case AccessWidth16:
    case AccessS3SaveWidth16:
      *TemData = (UINT32)*(UINT16 *)Data;
      *TempDataMask = (UINT32)*(UINT16 *)DataMask;
      break;
    case AccessWidth32:
    case AccessS3SaveWidth32:
      *TemData = *(UINT32 *)Data;
      *TempDataMask = *(UINT32 *)DataMask;
      break;
    default:
      IDS_ERROR_TRAP;
  }
}

/*---------------------------------------------------------------------------------------*/

/**
 * Returns the package type mask for the processor
 *
 *
 * @param[in]     AccessWidth     Access width
 * @retval        Width in number of bytes
 */
UINT8
LibHygonAccessWidth (
  IN       ACCESS_WIDTH AccessWidth
  )
{
  UINT8  Width;

  switch (AccessWidth) {
    case AccessWidth8:
    case AccessS3SaveWidth8:
      Width = 1;
      break;
    case AccessWidth16:
    case AccessS3SaveWidth16:
      Width = 2;
      break;
    case AccessWidth32:
    case AccessS3SaveWidth32:
      Width = 4;
      break;
    case AccessWidth64:
    case AccessS3SaveWidth64:
      Width = 8;
      break;
    default:
      Width = 0;
      IDS_ERROR_TRAP;
  }

  return Width;
}
