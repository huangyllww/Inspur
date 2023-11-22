/* $NoKeywords:$ */

/**
 * @file
 *
 * HPCB.h
 *
 * HGPI PSP PMU Block
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: HGPI
 * @e sub-project: (Mem)
 *
 **/
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
* ***************************************************************************
*
*/

#ifndef _HPPB_H_
#define _HPPB_H_
// #include "psp_fw_image.h"

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */
/// ===============================================================================
/// _PSP_PMU_STRUCT
/// This data structure is used to pass wrapper parameters to the memory configuration code
///
typedef struct _PSP_PMU_STRUCT {
  PMU_FIRMWARE    PmuFirmwareImage;           ///< PMU Firmware Image
} PSP_PMU_STRUCT;

// HGPI PSP PMU Signature.
#define HPPB_SIGNATURE  { '!', '!', 'B', 'P', 'P', 'A' } ///< HPPB Signature with 6 characters limited.
#define HPPB_VERSION    0x0001                           // HPPB Version
/// HYGON HPCB_HEADER Header
typedef struct {
  CHAR8     Signature[6];                     ///< HPPB signature
  CHAR8     TerminatorNull;                   ///< null terminated string
  CHAR8     TerminatorNull1;                  ///< null terminated string
  UINT32    Version;                          ///< Version
  UINT32    Family;                           ///< Family
  UINT32    DddrType;                         ///< DDR Type
} HPPB_HEADER;

/// ===============================================================================
/// HGPI PSP PMU BLOCK
/// This data structure is used to pass PMU information to the PSP
///
typedef struct _HGPI_PSP_PMU_BLOCK_STRUCT {
  PSP_FW_IMAGE_HEADER    PspFwImageHeader;        ///< PSP FW Image Header
  IN HPPB_HEADER         Header;                  ///< HPPB Header
  IN PSP_PMU_STRUCT      memPspPmu;               ///< PMU Image Buffer
} HGPI_PSP_PMU_BLOCK_STRUCT;

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

HGPI_STATUS
VerifyAppb (
  IN OUT   UINT32 HgpiAppbVersion,
  IN OUT   UINT32 BiosAppbVersion,
  IN       UINT32 Family,
  IN       UINT32 DDRType,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  );

#endif /* _HPPB_H_ */
