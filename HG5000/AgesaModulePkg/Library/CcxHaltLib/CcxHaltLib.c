/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Halt Library
 *
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/CcxHaltLib.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  LIBRARY_CCXHALTLIB_CCXHALTLIB_FILECODE

/*---------------------------------------------------------------------------------------*/
/**
 * Update AP MTRR settings list according to PCD
 *
 *
 * @param[in, out]  ApMtrrSettingsList List of MTRR settings for AP
 *
 */
VOID
UpdateApMtrrSettings (
  IN OUT   AP_MTRR_SETTINGS  *ApMtrrSettingsList
  )
{
  while (ApMtrrSettingsList->MsrAddr != CPU_LIST_TERMINAL) {
    switch (ApMtrrSettingsList->MsrAddr) {
    case AMD_AP_MTRR_FIX64k_00000:
      if (PcdGet64 (PcdAmdFixedMtrr250) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr250);
      }
      break;
    case AMD_AP_MTRR_FIX16k_80000:
      if (PcdGet64 (PcdAmdFixedMtrr258) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr258);
      }
      break;
    case AMD_AP_MTRR_FIX16k_A0000:
      if (PcdGet64 (PcdAmdFixedMtrr259) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr259);
      }
      break;
    case AMD_AP_MTRR_FIX4k_C0000:
      if (PcdGet64 (PcdAmdFixedMtrr268) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr268);
      }
      break;
    case AMD_AP_MTRR_FIX4k_C8000:
      if (PcdGet64 (PcdAmdFixedMtrr269) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr269);
      }
      break;
    case AMD_AP_MTRR_FIX4k_D0000:
      if (PcdGet64 (PcdAmdFixedMtrr26A) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr26A);
      }
      break;
    case AMD_AP_MTRR_FIX4k_D8000:
      if (PcdGet64 (PcdAmdFixedMtrr26B) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr26B);
      }
      break;
    case AMD_AP_MTRR_FIX4k_E0000:
      if (PcdGet64 (PcdAmdFixedMtrr26C) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr26C);
      }
      break;
    case AMD_AP_MTRR_FIX4k_E8000:
      if (PcdGet64 (PcdAmdFixedMtrr26D) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr26D);
      }
      break;
    case AMD_AP_MTRR_FIX4k_F0000:
      if (PcdGet64 (PcdAmdFixedMtrr26E) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr26E);
      }
      break;
    case AMD_AP_MTRR_FIX4k_F8000:
      if (PcdGet64 (PcdAmdFixedMtrr26F) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr26F);
      }
      break;
    default:
      break;
    }
    ApMtrrSettingsList++;
  }
}
