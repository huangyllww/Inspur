/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH common
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
#include "FchPlatform.h"
#define FILECODE PROC_FCH_COMMON_FCHCOMMON_FILECODE

/**
 * ReadFchChipsetRevision - Get FCH chipset revision
 *
 *
 * @param[in] StdHeader - Amd Configuration Parameters.
 *
 */
UINT8
ReadFchChipsetRevision (
  IN AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8    FchChipSetRevision;
  UINT8    FchMiscRegister;
  ReadPmio (FCH_PMIOA_REGC8 + 1, AccessWidth8, &FchMiscRegister, StdHeader);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGC8 + 1, AccessWidth8, ~(UINT32) BIT7, 0);
  ReadPci ((SMBUS_BUS_DEV_FUN << 16) + FCH_CFG_REG08, AccessWidth8, &FchChipSetRevision, StdHeader);
  WritePmio (FCH_PMIOA_REGC8 + 1, AccessWidth8, &FchMiscRegister, StdHeader);
  return FchChipSetRevision;
}

/**
 * ReadSocType - Get Soc ID
 *
 *
 *
 */
UINT32
ReadSocType (
  VOID
  )
{
  UINT32    SocId;

  ReadPci ((AMD_D0F0 << 16) + FCH_CFG_REG00, AccessWidth32, &SocId, NULL);

  return SocId;
}

/**
 * CheckZP - Check if ZP Soc
 *
 *
 *
 */
BOOLEAN
CheckZP (
  VOID
  )
{
  if (ReadSocType () == SOC_ZPID || ReadSocType () == HYGON_SOC_ZPID) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
 * ReadSocDieBusNum - Get Bus Number of given Die
 *
 *
 *
 */
UINT32
ReadSocDieBusNum (
  UINT8  Die
  )
{
  UINT16    VendorID;
  UINT32    DieBusNum;

  ReadPci (((((Die + 24) << 3) + 0) << 16) + FCH_CFG_REG00, AccessWidth16, &VendorID, NULL);
//  if (VendorID != 0x1022) {
	if (VendorID != AMD_VID && VendorID != HYGON_VID) {

    return 0xffffffff;
  }

  ReadPci (((((Die + 24) << 3) + 0) << 16) + 0x84, AccessWidth32, &DieBusNum, NULL);
  DieBusNum &= 0xff;
  return DieBusNum;
}
