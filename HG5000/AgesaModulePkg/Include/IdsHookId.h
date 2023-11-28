/* $NoKeywords:$ */
/**
 * @file
 *
 * IdsHookId.h
 *
 * Contains HOOKID definition
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project: (Proc/Ids/Internal)
 * @e \$Revision: 328983 $   @e \$Date: 2015-10-14 20:56:31 +0800 (Wed, 14 Oct 2015) $
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
 *
 ***************************************************************************/

#ifndef _IDS_HOOKID_H_
#define _IDS_HOOKID_H_

typedef UINT32 HOOK_ID;
// 0x00000001 - 0x10000000 for CCX
#define IDS_HOOK_CCX_BEFORE_AP_LAUNCH   0x00000001ul
#define IDS_HOOK_CCX_AFTER_AP_LAUNCH    0x00000002ul
#define IDS_HOOK_CCX_AFTER_PWR_MNG      0x00000003ul
#define IDS_HOOK_CCX_SKIP_UCODE_PATCH   0x00000004ul
#define IDS_HOOK_CCX_SMM                0x00000005ul
#define IDS_HOOK_CCX_SKIP_SMEE          0x00000006ul
#define IDS_HOOK_CCX_SKIP_ACPI_PSTATE_OBJ     0x00000010ul
#define IDS_HOOK_CCX_CUSTOM_PSTATES     0x00000011ul
#define IDS_HOOK_CCX_SKIP_OC_ENABLE     0x00000012ul


// 0x20000001 - 0x40000000 for DF
#define IDS_HOOK_DF_AFTER_AP_LAUNCH     0x20000001ul
#define IDS_HOOK_DF_DXE_INIT            0x20000002ul
#define IDS_HOOK_DF_RAS_INIT            0x20000003ul
#define IDS_HOOK_DF_RAS_INIT2           0x20000004ul
#define IDS_HOOK_DF_CLK_GATING          0x20000005ul
#define IDS_HOOK_PIE_PWR_MGMT_INIT      0x20000006ul
#define IDS_HOOK_PIE_PWR_MGMT_INIT2     0x20000007ul
#define IDS_HOOK_PIE_PWR_MGMT_INIT3     0x20000008ul
#define IDS_HOOK_PIE_REDIRECT_SCRUB_CTRL_INIT 0x20000009ul
#define IDS_HOOK_PIE_DRAM_SCRUB_TIME_INIT     0x2000000Aul
#define IDS_HOOK_FABRIC_PEI_INIT_END    0x2000000Bul


//Special HOOK ID reserved for get IDS NV table

#define IDS_HOOK_BEFORE_MEM_INIT        0x60000001ul

// 0xB0000000 - 0xCFFFFFFF for NBIO
#define IDS_HOOK_NBIO_INIT_EARLY        0xB0000001ul
#define IDS_HOOK_NBIO_INIT_ENV          0xB0000002ul
#define IDS_HOOK_NBIO_BASE_INIT         0xB0000003ul
#define IDS_HOOK_NBIO_LOAD_BUILD_OPTION 0xB0000004ul
#define IDS_HOOK_NBIO_SMUV9_INIT        0xB0000005ul
#define IDS_HOOK_NBIO_DEVICE_REMAP      0xB0000006ul
#define IDS_HOOK_NBIO_PCIE_TOPOLOGY     0xB0000007ul
#define IDS_HOOK_NBIO_PCIE_USER_CONFIG  0xB0000008ul
#define IDS_HOOK_NBIO_PCIE_AER_INIT     0xB0000009ul
#define IDS_HOOK_NBIO_SMUV10_INIT       0xB000000Aul
#define IDS_HOOK_NBIO_SMUV9_INIT_CMN    0xB000000Bul
#define IDS_HOOK_NBIO_AFTER_CCX         0xB000000Cul

// 0xD0000000 - 0xDFFFFFFF for FCH
#define IDS_HOOK_FCH_INIT_RESET         0xD0000001ul
#define IDS_HOOK_FCH_INIT_ENV           0xD0000002ul

// Common
#define IDS_HOOK_BEGINNING_OF_AGESA     0xE0000000ul

#define IDS_HOOK_GET_NV_TABLE           0xFFFFFFFEul
#endif // _IDS_HOOKID_H_
