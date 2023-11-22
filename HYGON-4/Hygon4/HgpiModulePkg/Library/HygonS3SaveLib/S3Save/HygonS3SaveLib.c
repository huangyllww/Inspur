/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Library
 *
 * Contains interface to the HYGON S3 save library
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
 **/
#include <PiDxe.h>
#include <Guid/EventGroup.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/SmmCommunication.h>
#include <Protocol/DxeSmmReadyToLock.h>
#include "HYGON.h"
#include "Library/HygonBaseLib.h"
#include "Library/HygonS3SaveLib.h"
#include "Library/IdsLib.h"
#include "HygonS3SaveLibData.h"
#include "HygonSmmCommunication.h"
#include <Library/PciLib.h>
#include "Filecode.h"
#define FILECODE  LIBRARY_HYGONS3SAVELIB_S3SAVE_HYGONS3SAVELIB_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
UINT64
STATIC
HygonS3GetDataFromPtr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       VOID         *Data
  );

S3_BOOT_SCRIPT_LIB_WIDTH
STATIC
HygonS3GetAccessWidth (
  IN       ACCESS_WIDTH AccessWidth
  );

HGPI_STATUS
HygonS3SaveCacheScriptData (
  IN      UINT16      OpCode,
  IN      ...
  );

/*----------------------------------------------------------------------------------------
 *                         G L O B A L        D A T A
 *----------------------------------------------------------------------------------------
 */
HYGON_S3_LIB_PRIVATE_DATA  *mHygonS3LibPrivateData = NULL;
EFI_EVENT                  mS3ScriptLockEvent = NULL;
EFI_EVENT                  mCloseTableEvent   = NULL;
UINT32                     mMMIOSize    = 0;
UINT64                     mMMIOAddress = 0;

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
HGPI_STATUS
HygonS3IoWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Value,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  HGPI_STATUS  HgpiStatus;

  LibHygonIoWrite (Width, (UINT16)Address, Value, StdHeader);
  HgpiStatus = HygonS3SaveScriptIoWrite (Width, Address, Value);

  return HgpiStatus;
}

HGPI_STATUS
HygonS3IoRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  HGPI_STATUS  HgpiStatus;

  LibHygonIoRMW (Width, (UINT16)Address, Data, DataMask, StdHeader);
  HgpiStatus = HygonS3SaveScriptIoRMW (Width, Address, Data, DataMask);

  return HgpiStatus;
}

HGPI_STATUS
HygonS3IoPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  HGPI_STATUS  HgpiStatus;

  LibHygonIoPoll (Width, (UINT16)Address, Data, DataMask, Delay, StdHeader);
  HgpiStatus = HygonS3SaveScriptIoPoll (Width, Address, Data, DataMask, Delay);

  return HgpiStatus;
}

HGPI_STATUS
HygonS3MemWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Value,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  HGPI_STATUS  HgpiStatus;

  LibHygonMemWrite (Width, Address, Value, StdHeader);
  HgpiStatus = HygonS3SaveScriptMemWrite (Width, Address, Value);

  return HgpiStatus;
}

HGPI_STATUS
HygonS3MemRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  HGPI_STATUS  HgpiStatus;

  LibHygonMemRMW (Width, Address, Data, DataMask, StdHeader);
  HgpiStatus = HygonS3SaveScriptMemRMW (Width, Address, Data, DataMask);

  return HgpiStatus;
}

HGPI_STATUS
HygonS3MemPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  HGPI_STATUS  HgpiStatus;

  LibHygonMemPoll (Width, Address, Data, DataMask, Delay, StdHeader);
  HgpiStatus = HygonS3SaveScriptMemPoll (Width, Address, Data, DataMask, Delay);

  return HgpiStatus;
}

HGPI_STATUS
HygonS3PciWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Value,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  HGPI_STATUS  HgpiStatus;

  switch (Width) {
    case AccessWidth8:
    case AccessS3SaveWidth8:
      PciWrite8 (PciAddress, *((UINT8 *)Value));
      break;
    case AccessWidth16:
    case AccessS3SaveWidth16:
      PciWrite16 (PciAddress, *((UINT16 *)Value));
      break;
    case AccessWidth32:
    case AccessS3SaveWidth32:
      PciWrite32 (PciAddress, *((UINT32 *)Value));
      break;
    default:
      IDS_ERROR_TRAP;
  }

  HgpiStatus = HygonS3SaveScriptPciWrite (Width, PciAddress, Value);

  return HgpiStatus;
}

HGPI_STATUS
HygonS3PciRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT32       TempData;
  UINT32       TempMask;
  HGPI_STATUS  HgpiStatus;

  LibHygonGetDataFromPtr (Width, Data, DataMask, &TempData, &TempMask);

  switch (Width) {
    case AccessWidth8:
    case AccessS3SaveWidth8:
      PciAndThenOr8 (PciAddress, (UINT8)(~TempMask), (UINT8)TempData);
      break;
    case AccessWidth16:
    case AccessS3SaveWidth16:
      PciAndThenOr16 (PciAddress, (UINT16)(~TempMask), (UINT16)TempData);
      break;
    case AccessWidth32:
    case AccessS3SaveWidth32:
      PciAndThenOr32 (PciAddress, (~TempMask), TempData);
      break;
    default:
      IDS_ERROR_TRAP;
  }

  HgpiStatus = HygonS3SaveScriptPciRMW (Width, PciAddress, Data, DataMask);

  return HgpiStatus;
}

HGPI_STATUS
HygonS3SaveScriptIoWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Value
  )
{
  RETURN_STATUS  CalledStatus;
  HGPI_STATUS    HgpiStatus;

  if (mHygonS3LibPrivateData->S3ScriptLock == FALSE) {
    CalledStatus = S3BootScriptSaveIoWrite (HygonS3GetAccessWidth (Width), Address, 1, Value);
    HgpiStatus   = (CalledStatus == RETURN_SUCCESS) ? HGPI_SUCCESS : HGPI_ERROR;
  } else {
    HgpiStatus = HygonS3SaveCacheScriptData (
                   HYGON_S3LIB_BOOT_SCRIPT_IO_WRITE_OP,
                   HygonS3GetAccessWidth (Width),
                   Address,
                   HygonS3GetDataFromPtr (Width, Value)
                   );
  }

  return HgpiStatus;
}

HGPI_STATUS
HygonS3SaveScriptIoRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask
  )
{
  UINT64         DataMaskTemp;
  RETURN_STATUS  CalledStatus;
  HGPI_STATUS    HgpiStatus;

  DataMaskTemp = HygonS3GetDataFromPtr (Width, DataMask);
  DataMaskTemp = ~DataMaskTemp;

  if (mHygonS3LibPrivateData->S3ScriptLock == FALSE) {
    CalledStatus = S3BootScriptSaveIoReadWrite (HygonS3GetAccessWidth (Width), Address, Data, &DataMaskTemp);
    HgpiStatus   = (CalledStatus == RETURN_SUCCESS) ? HGPI_SUCCESS : HGPI_ERROR;
  } else {
    HgpiStatus = HygonS3SaveCacheScriptData (
                   HYGON_S3LIB_BOOT_SCRIPT_IO_RMW_OP,
                   HygonS3GetAccessWidth (Width),
                   Address,
                   HygonS3GetDataFromPtr (Width, Data),
                   DataMaskTemp
                   );
  }

  return HgpiStatus;
}

HGPI_STATUS
HygonS3SaveScriptIoPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay
  )
{
  RETURN_STATUS  CalledStatus;
  HGPI_STATUS    HgpiStatus;

  if (mHygonS3LibPrivateData->S3ScriptLock == FALSE) {
    CalledStatus = S3BootScriptSaveIoPoll (HygonS3GetAccessWidth (Width), Address, Data, DataMask, Delay);
    HgpiStatus   = (CalledStatus == RETURN_SUCCESS) ? HGPI_SUCCESS : HGPI_ERROR;
  } else {
    HgpiStatus = HygonS3SaveCacheScriptData (
                   HYGON_S3LIB_BOOT_SCRIPT_IO_POLL_OP,
                   HygonS3GetAccessWidth (Width),
                   Address,
                   HygonS3GetDataFromPtr (Width, Data),
                   HygonS3GetDataFromPtr (Width, DataMask),
                   Delay
                   );
  }

  return HgpiStatus;
}

HGPI_STATUS
HygonS3SaveScriptMemWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Value
  )
{
  RETURN_STATUS  CalledStatus;
  HGPI_STATUS    HgpiStatus;

  if (mHygonS3LibPrivateData->S3ScriptLock == FALSE) {
    CalledStatus = S3BootScriptSaveMemWrite (HygonS3GetAccessWidth (Width), Address, 1, Value);
    HgpiStatus   = (CalledStatus == RETURN_SUCCESS) ? HGPI_SUCCESS : HGPI_ERROR;
  } else {
    HgpiStatus = HygonS3SaveCacheScriptData (
                   HYGON_S3LIB_BOOT_SCRIPT_MEM_WRITE_OP,
                   HygonS3GetAccessWidth (Width),
                   Address,
                   HygonS3GetDataFromPtr (Width, Value)
                   );
  }

  return HgpiStatus;
}

HGPI_STATUS
HygonS3SaveScriptMemRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask
  )
{
  UINT64         DataMaskTemp;
  RETURN_STATUS  CalledStatus;
  HGPI_STATUS    HgpiStatus;

  DataMaskTemp = HygonS3GetDataFromPtr (Width, DataMask);
  DataMaskTemp = ~DataMaskTemp;
  if (mHygonS3LibPrivateData->S3ScriptLock == FALSE) {
    CalledStatus = S3BootScriptSaveMemReadWrite (HygonS3GetAccessWidth (Width), Address, Data, &DataMaskTemp);
    HgpiStatus   = (CalledStatus == RETURN_SUCCESS) ? HGPI_SUCCESS : HGPI_ERROR;
  } else {
    HgpiStatus = HygonS3SaveCacheScriptData (
                   HYGON_S3LIB_BOOT_SCRIPT_MEM_RMW_OP,
                   HygonS3GetAccessWidth (Width),
                   Address,
                   HygonS3GetDataFromPtr (Width, Data),
                   DataMaskTemp
                   );
  }

  return HgpiStatus;
}

HGPI_STATUS
HygonS3SaveScriptMemPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay
  )
{
  RETURN_STATUS  CalledStatus;
  HGPI_STATUS    HgpiStatus;

  UINTN  LoopTimes;

  //
  // According to the spec, the interval between 2 polls is 100ns,
  // but the unit of Duration for S3BootScriptSaveMemPoll() is microsecond(1000ns).
  // Duration * 1000ns * LoopTimes = Delay * 100ns
  // Duration will be minimum 1(microsecond) to be minimum deviation,
  // so LoopTimes = Delay / 10.
  //
  LoopTimes = (Delay + 9) / 10;
  if (mHygonS3LibPrivateData->S3ScriptLock == FALSE) {
    CalledStatus = S3BootScriptSaveMemPoll (HygonS3GetAccessWidth (Width), Address, DataMask, Data, 1, LoopTimes);
    HgpiStatus   = (CalledStatus == RETURN_SUCCESS) ? HGPI_SUCCESS : HGPI_ERROR;
  } else {
    HgpiStatus = HygonS3SaveCacheScriptData (
                   HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL_OP,
                   HygonS3GetAccessWidth (Width),
                   Address,
                   HygonS3GetDataFromPtr (Width, Data),
                   HygonS3GetDataFromPtr (Width, DataMask),
                   LoopTimes
                   );
  }

  return HgpiStatus;
}

HGPI_STATUS
HygonS3SaveScriptPciWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Value
  )
{
  UINT64  BootScriptPciAddress;

  // Translate PCI Write to MMIO write due PCILIB can't support PCI register offset above 0xFF
  BootScriptPciAddress = mMMIOAddress + (PciAddress & 0x0FFFFFFF);
  return HygonS3SaveScriptMemWrite (Width, BootScriptPciAddress, Value);
}

HGPI_STATUS
HygonS3SaveScriptPciRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Data,
  IN       VOID          *DataMask
  )
{
  UINT64  BootScriptPciAddress;

  // Translate PCI Write to MMIO write due PCILIB can't support PCI register offset above 0xFF
  BootScriptPciAddress = mMMIOAddress + (PciAddress & 0x0FFFFFFF);
  return HygonS3SaveScriptMemRMW (Width, BootScriptPciAddress, Data, DataMask);
}

HGPI_STATUS
HygonS3SaveScriptPciPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay
  )
{
  UINT64  BootScriptPciAddress;

  // Translate PCI Write to MMIO write due PCILIB can't support PCI register offset above 0xFF
  BootScriptPciAddress = mMMIOAddress + (PciAddress & 0x0FFFFFFF);

  return HygonS3SaveScriptMemPoll (Width, BootScriptPciAddress, Data, DataMask, Delay);
}

HGPI_STATUS
HygonS3SaveScriptDispatch (
  IN       VOID          *EntryPoint,
  IN       VOID          *Context
  )
{
  RETURN_STATUS  CalledStatus;
  HGPI_STATUS    HgpiStatus;

  if (mHygonS3LibPrivateData->S3ScriptLock == FALSE) {
    CalledStatus = S3BootScriptSaveDispatch2 (EntryPoint, Context);
    HgpiStatus   = (CalledStatus == RETURN_SUCCESS) ? HGPI_SUCCESS : HGPI_ERROR;
  } else {
    HgpiStatus = HygonS3SaveCacheScriptData (HYGON_S3LIB_BOOT_SCRIPT_DISPATCH_OP, EntryPoint, Context);
  }

  return HgpiStatus;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
UINT64
STATIC
HygonS3GetDataFromPtr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       VOID         *Data
  )
{
  UINT64  TempData;

  TempData = 0;
  switch (AccessWidth) {
    case AccessWidth8:
      TempData = (UINT64)(*((UINT8 *)Data));
      break;
    case AccessWidth16:
      TempData = (UINT64)(*((UINT16 *)Data));
      break;
    case AccessWidth32:
      TempData = (UINT64)(*((UINT32 *)Data));
      break;
    case AccessWidth64:
      TempData = (UINT64)(*((UINT64 *)Data));
      break;
    default:
      IDS_ERROR_TRAP;
      break;
  }

  return TempData;
}

S3_BOOT_SCRIPT_LIB_WIDTH
STATIC
HygonS3GetAccessWidth (
  IN       ACCESS_WIDTH AccessWidth
  )
{
  S3_BOOT_SCRIPT_LIB_WIDTH  S3BootScriptAccessWidth;

  switch (AccessWidth) {
    case AccessWidth8:
      S3BootScriptAccessWidth = S3BootScriptWidthUint8;
      break;
    case AccessWidth16:
      S3BootScriptAccessWidth = S3BootScriptWidthUint16;
      break;
    case AccessWidth32:
      S3BootScriptAccessWidth = S3BootScriptWidthUint32;
      break;
    case AccessWidth64:
      S3BootScriptAccessWidth = S3BootScriptWidthUint64;
      break;
    default:
      S3BootScriptAccessWidth = S3BootScriptWidthUint8;
      IDS_ERROR_TRAP;
      break;
  }

  return S3BootScriptAccessWidth;
}

VOID
PrepareOpRecord (
  IN       UINT16      OpCode,
  IN       VA_LIST     Marker,
  IN       VOID        *OpDataBuf,
  IN OUT   UINTN       *OpDataLength
  )
{
  ((HYGON_S3LIB_BOOT_SCRIPT_COMMON *)OpDataBuf)->OpCode = OpCode;
  switch (OpCode) {
    case HYGON_S3LIB_BOOT_SCRIPT_IO_WRITE_OP:
      ((HYGON_S3LIB_BOOT_SCRIPT_IO_WRITE *)OpDataBuf)->Width   = VA_ARG (Marker, S3_BOOT_SCRIPT_LIB_WIDTH);
      ((HYGON_S3LIB_BOOT_SCRIPT_IO_WRITE *)OpDataBuf)->Address = VA_ARG (Marker, UINT64);
      ((HYGON_S3LIB_BOOT_SCRIPT_IO_WRITE *)OpDataBuf)->Value   = VA_ARG (Marker, UINT64);
      *OpDataLength = sizeof (HYGON_S3LIB_BOOT_SCRIPT_IO_WRITE);
      break;
    case HYGON_S3LIB_BOOT_SCRIPT_IO_RMW_OP:
      ((HYGON_S3LIB_BOOT_SCRIPT_IO_RMW *)OpDataBuf)->Width    = VA_ARG (Marker, S3_BOOT_SCRIPT_LIB_WIDTH);
      ((HYGON_S3LIB_BOOT_SCRIPT_IO_RMW *)OpDataBuf)->Address  = VA_ARG (Marker, UINT64);
      ((HYGON_S3LIB_BOOT_SCRIPT_IO_RMW *)OpDataBuf)->Data     =  VA_ARG (Marker, UINT64);
      ((HYGON_S3LIB_BOOT_SCRIPT_IO_RMW *)OpDataBuf)->DataMask =  VA_ARG (Marker, UINT64);
      *OpDataLength = sizeof (HYGON_S3LIB_BOOT_SCRIPT_IO_RMW);
      break;
    case HYGON_S3LIB_BOOT_SCRIPT_IO_POLL_OP:
      ((HYGON_S3LIB_BOOT_SCRIPT_IO_POLL *)OpDataBuf)->Width    = VA_ARG (Marker, S3_BOOT_SCRIPT_LIB_WIDTH);
      ((HYGON_S3LIB_BOOT_SCRIPT_IO_POLL *)OpDataBuf)->Address  = VA_ARG (Marker, UINT64);
      ((HYGON_S3LIB_BOOT_SCRIPT_IO_POLL *)OpDataBuf)->Data     =  VA_ARG (Marker, UINT64);
      ((HYGON_S3LIB_BOOT_SCRIPT_IO_POLL *)OpDataBuf)->DataMask =  VA_ARG (Marker, UINT64);
      ((HYGON_S3LIB_BOOT_SCRIPT_IO_POLL *)OpDataBuf)->Delay    =  VA_ARG (Marker, UINT64);
      *OpDataLength = sizeof (HYGON_S3LIB_BOOT_SCRIPT_IO_POLL);
      break;
    case HYGON_S3LIB_BOOT_SCRIPT_MEM_WRITE_OP:
      ((HYGON_S3LIB_BOOT_SCRIPT_MEM_WRITE *)OpDataBuf)->Width   = VA_ARG (Marker, S3_BOOT_SCRIPT_LIB_WIDTH);
      ((HYGON_S3LIB_BOOT_SCRIPT_MEM_WRITE *)OpDataBuf)->Address = VA_ARG (Marker, UINT64);
      ((HYGON_S3LIB_BOOT_SCRIPT_MEM_WRITE *)OpDataBuf)->Value   = VA_ARG (Marker, UINT64);
      *OpDataLength = sizeof (HYGON_S3LIB_BOOT_SCRIPT_MEM_WRITE);
      break;
    case HYGON_S3LIB_BOOT_SCRIPT_MEM_RMW_OP:
      ((HYGON_S3LIB_BOOT_SCRIPT_MEM_RMW *)OpDataBuf)->Width    = VA_ARG (Marker, S3_BOOT_SCRIPT_LIB_WIDTH);
      ((HYGON_S3LIB_BOOT_SCRIPT_MEM_RMW *)OpDataBuf)->Address  = VA_ARG (Marker, UINT64);
      ((HYGON_S3LIB_BOOT_SCRIPT_MEM_RMW *)OpDataBuf)->Data     =  VA_ARG (Marker, UINT64);
      ((HYGON_S3LIB_BOOT_SCRIPT_MEM_RMW *)OpDataBuf)->DataMask =  VA_ARG (Marker, UINT64);
      *OpDataLength = sizeof (HYGON_S3LIB_BOOT_SCRIPT_MEM_RMW);
      break;
    case HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL_OP:
      ((HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL *)OpDataBuf)->Width     = VA_ARG (Marker, S3_BOOT_SCRIPT_LIB_WIDTH);
      ((HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL *)OpDataBuf)->Address   = VA_ARG (Marker, UINT64);
      ((HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL *)OpDataBuf)->Data      =  VA_ARG (Marker, UINT64);
      ((HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL *)OpDataBuf)->DataMask  =  VA_ARG (Marker, UINT64);
      ((HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL *)OpDataBuf)->LoopTimes =  VA_ARG (Marker, UINTN);
      *OpDataLength = sizeof (HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL);
      break;
    case HYGON_S3LIB_BOOT_SCRIPT_PCI_WRITE_OP:
      ((HYGON_S3LIB_BOOT_SCRIPT_PCI_WRITE *)OpDataBuf)->Width   = VA_ARG (Marker, S3_BOOT_SCRIPT_LIB_WIDTH);
      ((HYGON_S3LIB_BOOT_SCRIPT_PCI_WRITE *)OpDataBuf)->Segment = VA_ARG (Marker, UINT16);
      ((HYGON_S3LIB_BOOT_SCRIPT_PCI_WRITE *)OpDataBuf)->Address = VA_ARG (Marker, UINT64);
      ((HYGON_S3LIB_BOOT_SCRIPT_PCI_WRITE *)OpDataBuf)->Value   = VA_ARG (Marker, UINT64);
      *OpDataLength = sizeof (HYGON_S3LIB_BOOT_SCRIPT_PCI_WRITE);
      break;
    case HYGON_S3LIB_BOOT_SCRIPT_PCI_RMW_OP:
      ((HYGON_S3LIB_BOOT_SCRIPT_PCI_RMW *)OpDataBuf)->Width    = VA_ARG (Marker, S3_BOOT_SCRIPT_LIB_WIDTH);
      ((HYGON_S3LIB_BOOT_SCRIPT_PCI_RMW *)OpDataBuf)->Segment  = VA_ARG (Marker, UINT16);
      ((HYGON_S3LIB_BOOT_SCRIPT_PCI_RMW *)OpDataBuf)->Address  = VA_ARG (Marker, UINT64);
      ((HYGON_S3LIB_BOOT_SCRIPT_PCI_RMW *)OpDataBuf)->Data     =  VA_ARG (Marker, UINT64);
      ((HYGON_S3LIB_BOOT_SCRIPT_PCI_RMW *)OpDataBuf)->DataMask =  VA_ARG (Marker, UINT64);
      *OpDataLength = sizeof (HYGON_S3LIB_BOOT_SCRIPT_PCI_RMW);
      break;
    case HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL_OP:
      ((HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL *)OpDataBuf)->Width    = VA_ARG (Marker, S3_BOOT_SCRIPT_LIB_WIDTH);
      ((HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL *)OpDataBuf)->Segment  = VA_ARG (Marker, UINT16);
      ((HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL *)OpDataBuf)->Address  = VA_ARG (Marker, UINT64);
      ((HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL *)OpDataBuf)->Data     =  VA_ARG (Marker, UINT64);
      ((HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL *)OpDataBuf)->DataMask =  VA_ARG (Marker, UINT64);
      ((HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL *)OpDataBuf)->Delay    =  VA_ARG (Marker, UINT64);
      *OpDataLength = sizeof (HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL);
      break;
    case HYGON_S3LIB_BOOT_SCRIPT_DISPATCH_OP:
      ((HYGON_S3LIB_BOOT_SCRIPT_DISPATCH *)OpDataBuf)->EntryPoint = VA_ARG (Marker, VOID *);
      ((HYGON_S3LIB_BOOT_SCRIPT_DISPATCH *)OpDataBuf)->Context    = VA_ARG (Marker, VOID *);
      *OpDataLength = sizeof (HYGON_S3LIB_BOOT_SCRIPT_DISPATCH);
      break;
    default:
      ASSERT (FALSE);
      break;
  }
}

HGPI_STATUS
ValidateSaveTable (
  )
{
  HYGON_S3_LIB_BOOT_SCRIPT_TABLE_HEADER  *HygonS3LibTblHdr;

  // Validate the save table
  HygonS3LibTblHdr = (HYGON_S3_LIB_BOOT_SCRIPT_TABLE_HEADER *)mHygonS3LibPrivateData->TableBase;
  ASSERT (HygonS3LibTblHdr->Sinature == HYGON_S3LIB_BOOT_SCRIPT_TABLE_SIG);
  if (HygonS3LibTblHdr->Sinature != HYGON_S3LIB_BOOT_SCRIPT_TABLE_SIG) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "HygonS3Lib table Signature check fail\n");
    return HGPI_ERROR;
  }

  ASSERT (*((UINT32 *)(mHygonS3LibPrivateData->TableBase + HygonS3LibTblHdr->TableLength)) == HYGON_S3LIB_TABLE_WATCHER);
  if (*((UINT32 *)(mHygonS3LibPrivateData->TableBase + HygonS3LibTblHdr->TableLength)) != HYGON_S3LIB_TABLE_WATCHER) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "HygonS3Lib table boundary check fail\n");
    return HGPI_ERROR;
  }

  return HGPI_SUCCESS;
}

HGPI_STATUS
HygonS3SaveCacheScriptData (
  IN      UINT16      OpCode,
  IN      ...
  )
{
  UINT8                                  *TableData;
  HYGON_S3_LIB_BOOT_SCRIPT_TABLE_HEADER  *HygonS3LibTblHdr;
  HYGON_S3LIB_BOOT_SCRIPT_DATA           OpDataBuf[2];
  UINTN                                  OpDataLength;
  VA_LIST                                Marker;

  VA_START (Marker, OpCode);
  HygonS3LibTblHdr = NULL;
  //
  // Cache the Script if EFI S3Script Locked and Hygon S3 Table still open
  if ((mHygonS3LibPrivateData->S3ScriptLock == TRUE) &&
      (mHygonS3LibPrivateData->CloseTable == FALSE)) {
    if (mHygonS3LibPrivateData->TableBase == NULL) {
      // S3 table haven't been allocated
      // reserve 4 bytes for data over boundary check
      HygonS3LibTblHdr = AllocateRuntimeZeroPool (HYGON_S3LIB_BUFFER_INIT_SIZE + HYGON_S3LIB_TABLE_WATCHER_SIZE);
      ASSERT (HygonS3LibTblHdr != NULL);
      if (HygonS3LibTblHdr == NULL) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "HygonS3Lib table allocate fail\n");
        return HGPI_ERROR;
      }

      // Update the private data
      mHygonS3LibPrivateData->TableBase = (UINT8 *)(HygonS3LibTblHdr);
      // Init the Table
      HygonS3LibTblHdr->Sinature = HYGON_S3LIB_BOOT_SCRIPT_TABLE_SIG;
      HygonS3LibTblHdr->Version  = HYGON_S3LIB_BOOT_SCRIPT_TABLE_VER_1;

      HygonS3LibTblHdr->TableLength    = HYGON_S3LIB_BUFFER_INIT_SIZE;
      HygonS3LibTblHdr->NextDataOffset = OFFSET_OF (HYGON_S3_LIB_BOOT_SCRIPT_TABLE_HEADER, Data);
      // Fill the Watcher
      *((UINT32 *)(mHygonS3LibPrivateData->TableBase + HygonS3LibTblHdr->TableLength)) = HYGON_S3LIB_TABLE_WATCHER;
    } else {
      HygonS3LibTblHdr = (HYGON_S3_LIB_BOOT_SCRIPT_TABLE_HEADER *)mHygonS3LibPrivateData->TableBase;
    }

    if (ValidateSaveTable () != HGPI_SUCCESS) {
      return HGPI_ERROR;
    }

    // Prepare the OP record buffer and get its length according OpCode
    ZeroMem (OpDataBuf, sizeof (OpDataBuf));
    OpDataLength = 0;
    PrepareOpRecord (OpCode, Marker, OpDataBuf, &OpDataLength);

    // Check if the buffer size is enough to hold the new record
    if ((HygonS3LibTblHdr->NextDataOffset + OpDataLength + HYGON_S3LIB_TABLE_END_SIZE) > HygonS3LibTblHdr->TableLength ) {
      // Don't have enough space
      // Allocate new space with big size
      TableData = AllocateRuntimeZeroPool (HygonS3LibTblHdr->TableLength + HYGON_S3LIB_BUFFER_INC_SIZE + HYGON_S3LIB_TABLE_WATCHER_SIZE);
      ASSERT (TableData != NULL);
      if (TableData == NULL) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "HygonS3SaveLib allocate new Pool fail %x\n", (HygonS3LibTblHdr->TableLength + HYGON_S3LIB_BUFFER_INC_SIZE + HYGON_S3LIB_TABLE_WATCHER_SIZE));
        return HGPI_ERROR;
      }

      // Copy data to new buffer
      CopyMem (TableData, mHygonS3LibPrivateData->TableBase, HygonS3LibTblHdr->TableLength);
      FreePool (mHygonS3LibPrivateData->TableBase);
      // Update global private data
      mHygonS3LibPrivateData->TableBase = TableData;
      // Update Table Header with new size infomation
      HygonS3LibTblHdr = (HYGON_S3_LIB_BOOT_SCRIPT_TABLE_HEADER *)TableData;
      HygonS3LibTblHdr->TableLength += HYGON_S3LIB_BUFFER_INC_SIZE;
      // Fill the Watcher
      *((UINT32 *)(mHygonS3LibPrivateData->TableBase + HygonS3LibTblHdr->TableLength)) = HYGON_S3LIB_TABLE_WATCHER;
    }

    // Copy the OP record to table
    CopyMem (mHygonS3LibPrivateData->TableBase + HygonS3LibTblHdr->NextDataOffset, OpDataBuf, OpDataLength);
    HygonS3LibTblHdr->NextDataOffset += OpDataLength;
  }

  return HGPI_SUCCESS;
}

VOID
HygonS3SaveBootScriptLockCallBack (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS  Status;
  VOID        *Interface;

  //
  // Try to locate it because EfiCreateProtocolNotifyEvent will trigger it once when registration.
  // Just return if it is not found.
  //
  Status = gBS->LocateProtocol (
                  &gEfiDxeSmmReadyToLockProtocolGuid,
                  NULL,
                  &Interface
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  // Make sure the hook ONLY called one time.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  mHygonS3LibPrivateData->S3ScriptLock = TRUE;
}

VOID
HygonS3SaveCloseTableCallBack (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                             Status;
  HYGON_S3_LIB_BOOT_SCRIPT_TABLE_HEADER  *HygonS3LibTblHdr;
  UINT8                                  *Buffer;
  EFI_SMM_COMMUNICATE_HEADER             *SmmCommBuff;
  HYGON_SMM_COMMUNICATION_S3SCRIPT       *HygonSmmS3ScriptBuff;
  EFI_SMM_COMMUNICATION_PROTOCOL         *SmmCommunication;
  UINTN                                  SmmCommBufferSize;

  // Make sure the hook ONLY called one time.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonS3SaveCloseTableCallBack\n");
  if (mHygonS3LibPrivateData->TableBase != NULL) {
    if (ValidateSaveTable () != HGPI_SUCCESS) {
      return;
    }

    // Fill the END flag
    HygonS3LibTblHdr = (HYGON_S3_LIB_BOOT_SCRIPT_TABLE_HEADER *)mHygonS3LibPrivateData->TableBase;
    *((UINT16 *)(mHygonS3LibPrivateData->TableBase + HygonS3LibTblHdr->NextDataOffset)) = HYGON_S3LIB_TABLE_END;

    // Init SMM communication buffer header
    Buffer = (UINT8 *)(UINTN)PcdGet64 (PcdHygonSmmCommunicationAddress);

    ASSERT (Buffer != NULL);
    if (Buffer != NULL) {
      ZeroMem (Buffer, HYGON_SMM_COMMUNICATION_S3SCRIPT_BUFFER_SIZE);
      SmmCommBuff = (EFI_SMM_COMMUNICATE_HEADER *)Buffer;
      CopyGuid (&SmmCommBuff->HeaderGuid, &gHygonSmmCommunicationHandleGuid);
      SmmCommBuff->MessageLength = sizeof (HYGON_SMM_COMMUNICATION_S3SCRIPT);
      // Init PSP SMM communicate private data
      HygonSmmS3ScriptBuff     = (HYGON_SMM_COMMUNICATION_S3SCRIPT *)&SmmCommBuff->Data;
      HygonSmmS3ScriptBuff->id = HYGON_SMM_COMMUNICATION_ID_S3SCRIPT;
      HygonSmmS3ScriptBuff->PrivateDataAddress = mHygonS3LibPrivateData;

      // Communicate HYGON SMM Handle
      SmmCommunication = NULL;
      Status = gBS->LocateProtocol (&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **)&SmmCommunication);
      ASSERT (Status == EFI_SUCCESS);
      if (SmmCommunication == NULL) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "HygonS3SaveLib locate SmmCommunicationProtocol fail \n");
        return;
      }

      // Communicate HYGON SMM communication handler to save boot script inside SMM
      SmmCommBufferSize = HYGON_SMM_COMMUNICATION_S3SCRIPT_BUFFER_SIZE;
      SmmCommunication->Communicate (SmmCommunication, Buffer, &SmmCommBufferSize);

      // Close Table
      mHygonS3LibPrivateData->CloseTable = TRUE;
      // Free the memory buffer
      FreePool (mHygonS3LibPrivateData->TableBase);
    }
  } else {
    IDS_HDT_CONSOLE (MAIN_FLOW, "mHygonS3LibPrivateData is NULL\n");
  }
}

EFI_STATUS
EFIAPI
HygonS3SaveLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                 Status;
  HYGON_S3_LIB_PRIVATE_DATA  *HygonS3LibPrivateData;
  VOID                       *Registration;

  // Locate the global data structure via PCD
  HygonS3LibPrivateData = NULL;
  HygonS3LibPrivateData = (HYGON_S3_LIB_PRIVATE_DATA *)(UINTN)PcdGet64 (PcdHygonS3LibPrivateDataAddress);

  ASSERT (GetPciMmioAddress (&mMMIOAddress, &mMMIOSize, NULL) == TRUE);

  if (HygonS3LibPrivateData == NULL) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "HygonS3LibPrivateData allocation\n");
    // Global data structure haven't init yet
    HygonS3LibPrivateData = AllocateRuntimeZeroPool (sizeof (HYGON_S3_LIB_PRIVATE_DATA));
    ASSERT (HygonS3LibPrivateData != NULL);
    if (HygonS3LibPrivateData == NULL) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "HygonS3LibPrivateData allocate fail\n");
      return EFI_OUT_OF_RESOURCES;
    }

    HygonS3LibPrivateData->S3ScriptLock = FALSE;
    HygonS3LibPrivateData->CloseTable   = FALSE;
    HygonS3LibPrivateData->TableBase    = NULL;

    // Set the global data
    PcdSet64S (PcdHygonS3LibPrivateDataAddress, (UINT64)(UINTN)HygonS3LibPrivateData);

    //
    // create event to notify the SMM Lock event
    //
    mS3ScriptLockEvent = EfiCreateProtocolNotifyEvent (
                           &gEfiDxeSmmReadyToLockProtocolGuid,
                           TPL_CALLBACK,
                           HygonS3SaveBootScriptLockCallBack,
                           NULL,
                           &Registration
                           );
    ASSERT (mS3ScriptLockEvent != NULL);
    if (mS3ScriptLockEvent == NULL) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "HygonS3SaveLib create BootScriptLock Event fail\n");
      return EFI_UNSUPPORTED;
    }

    // Create the hook for ReadyToBoot, ExitBootService will not be called for Non-UEFI system
    /// @todo need to make sure it the last timepoint of HGPI
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    HygonS3SaveCloseTableCallBack,
                    NULL,
                    &gEfiEventReadyToBootGuid,
                    &mCloseTableEvent
                    );

    ASSERT (Status == EFI_SUCCESS);
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "HygonS3SaveLib create CloseTable Event fail\n");
      return EFI_UNSUPPORTED;
    }
  }

  // Init Driver scope global value
  mHygonS3LibPrivateData = HygonS3LibPrivateData;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
HygonS3SaveLibDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  //
  // Close events
  //
  if (mS3ScriptLockEvent != NULL) {
    Status = gBS->CloseEvent (mS3ScriptLockEvent);
    ASSERT (Status == EFI_SUCCESS);
  }

  if (mCloseTableEvent != NULL) {
    Status = gBS->CloseEvent (mCloseTableEvent);
    ASSERT (Status == EFI_SUCCESS);
  }

  return EFI_SUCCESS;
}
