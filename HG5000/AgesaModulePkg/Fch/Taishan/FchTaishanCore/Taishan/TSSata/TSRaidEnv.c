/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch SATA controller (Raid mode)
 *
 * Init SATA Raid features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309083 $   @e \$Date: 2014-12-09 09:28:24 -0800 (Tue, 09 Dec 2014) $
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
#include "Filecode.h"
#define FILECODE PROC_FCH_TAISHAN_TSSATA_TSRAIDENV_FILECODE

extern VOID  FchInitEnvSataRaidProgram  (IN VOID  *FchDataPtr);

//
// Declaration of local functions
//

/**
 * FchInitEnvSataRaid - Config SATA Raid controller before PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvSataRaid  (
  IN  VOID     *FchDataPtr
  )
{
  FchInitEnvSataRaidProgram  (FchDataPtr);
}

VOID
FchInitEnvSataRaidTS  (
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;
  UINT8                  PkgType;
  UINT32                 RegEbx;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  // Class code
  //
  FchSmnRW (DieBusNum, FCH_SMN_SATA_NBIF_STRAP13, 0x00, 0x00010400, StdHeader);
  //
  // Device ID
  //
  FchSmnRW (DieBusNum, FCH_SMN_SATA_NBIF_STRAP0, 0xFFFF0000, TAISHAN_FCH_SATA_RAID_DID, StdHeader);

  //
  // Set PI for AM4
  //
  AsmCpuid (0x80000001, NULL, &RegEbx, NULL, NULL);
  PkgType = (UINT8)(RegEbx >> 28);
  if (PkgType == PKG_AM4) {
    //
    // only port2/3 enabled on AM4
    //
    FchSmnRW (DieBusNum, FCH_SMN_SATA_CONTROL_BAR5 + FCH_SATA_BAR5_REG0C, 0xFFFFFF00, 0x0C, StdHeader);
    FchSmnRW (DieBusNum, FCH_SMN_SATA_CONTROL_BAR5 + FCH_SATA_BAR5_REG00, 0xFFFFFFE0, 0x01, StdHeader);
  }
  FchTSSataShutdownUnconnectedSataPortClock (DieBusNum, FchDataPtr);

  //
  // SSID
  //
  if (LocalCfgPtr->Sata.SataRaidSsid != NULL ) {
    //RwPci ((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG2C, AccessWidth32, 0x00, LocalCfgPtr->Sata.SataAhciSsid, StdHeader);
    FchSmnRW (DieBusNum, FCH_TS_SATA_SMN_PCICFG + 0x4C, 0x00, LocalCfgPtr->Sata.SataRaidSsid, StdHeader);
  }
}
