/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Library
 *
 * Contains interface to the AMD S3 save library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 312770 $   @e \$Date: 2015-02-11 13:24:42 +0800 (Wed, 11 Feb 2015) $
 *
 */
/*
 ******************************************************************************
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
 **/
#include "AMD.h"
#include "Filecode.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdS3SaveLib.h"
#include "Library/IdsLib.h"
#include <Library/PciLib.h>

#define FILECODE LIBRARY_AMDS3SAVELIB_WOS3SAVE_AMDWOS3SAVELIB_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
AmdS3IoWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  LibAmdIoWrite (Width, (UINT16) Address, Value, StdHeader);

  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3IoRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  LibAmdIoRMW (Width, (UINT16) Address, Data, DataMask, StdHeader);

  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3IoPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  LibAmdIoPoll (Width, (UINT16) Address, Data, DataMask, Delay, StdHeader);

  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3MemWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  LibAmdMemWrite (Width, Address, Value, StdHeader);

  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3MemRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  LibAmdMemRMW (Width, Address, Data, DataMask, StdHeader);

  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3MemPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  LibAmdMemPoll (Width, Address, Data, DataMask, Delay, StdHeader);

  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3PciWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  switch (Width) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    PciWrite8 (PciAddress, *((UINT8 *) Value));
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    PciWrite16 (PciAddress, *((UINT16 *) Value));
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    PciWrite32 (PciAddress, *((UINT32 *) Value));
    break;
  default:
    IDS_ERROR_TRAP;
  }

  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3PciRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  TempData;
  UINT32  TempMask;
  LibAmdGetDataFromPtr (Width, Data,  DataMask, &TempData, &TempMask);

  switch (Width) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    PciAndThenOr8 (PciAddress, (UINT8) (~TempMask), (UINT8) TempData);
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    PciAndThenOr16 (PciAddress, (UINT16) (~TempMask), (UINT16) TempData);
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    PciAndThenOr32 (PciAddress, (~TempMask), TempData);
    break;
  default:
    IDS_ERROR_TRAP;
  }

  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptIoWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Value
  )
{
  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptIoRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask
  )
{
  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptIoPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay
  )
{
  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptMemWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Value
  )
{
  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptMemRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask
  )
{
  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptMemPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINT64        Address,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay
  )
{
  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptPciWrite (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Value
  )
{
  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptPciRMW (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Data,
  IN       VOID          *DataMask
  )
{
  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptPciPoll (
  IN       ACCESS_WIDTH  Width,
  IN       UINTN         PciAddress,
  IN       VOID          *Data,
  IN       VOID          *DataMask,
  IN       UINT64        Delay
  )
{
  return AGESA_SUCCESS;
}

AGESA_STATUS
AmdS3SaveScriptDispatch (
  IN       VOID          *EntryPoint,
  IN       VOID          *Context
  )
{
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

