/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdSocBaseLib.c
 *
 * Feature Soc common functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: SOC
 * @e \$Revision: 313790 $ @e \$Date: 2015-02-26 23:31:28 -0600 (Thu, 26 Feb 2015) $
 *
 **/
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
  * ***************************************************************************
  *
 */

#include <Library/BaseLib.h>
#include <Include/Filecode.h>
#include <Library/PcdLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/AmdSocBaseLib.h>

#define FILECODE LIBRARY_AMDSOCBASELIB_AMDSOCBASELIB_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function check SOC hardware identification ID
 *
 *     @param[in]   SocId  - the SOC_ID_STRUCT
 *
 *     @return          TRUE -  Hardware identification ID check pass.
 *     @return          FALSE - Hardware identification ID check fail.
 */

BOOLEAN
SocHardwareIdentificationCheck (
  IN    SOC_ID_STRUCT *SocId
  )
{
  CPUID_DATA CpuId;

  AsmCpuid (
      AMD_CPUID_FMF,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );

  // Check Hardware Identification

  if (SocId->SocFamilyID != (CpuId.EAX_Reg & RAW_FAMILY_ID_MASK)) {
    return FALSE;
  }

  if (SocId->PackageType != ((CpuId.EBX_Reg >> 28) & 0xF)) {
    return FALSE;
  }

  return TRUE;
}

/**
 *
 *
 *      This function check SOC family identification ID
 *
 *     @param[in]   SocFamilyID  - FmailyId
 *
 *     @return          TRUE -  Hardware identification ID check pass.
 *     @return          FALSE - Hardware identification ID check fail.
 */

BOOLEAN
SocFamilyIdentificationCheck (
  IN    UINT32 SocFamilyID
  )
{
  CPUID_DATA CpuId;

  AsmCpuid (
      AMD_CPUID_FMF,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );

  // Check Hardware Identification

  if (SocFamilyID != (CpuId.EAX_Reg & RAW_FAMILY_ID_MASK)) {
    return FALSE;
  }

  return TRUE;
}

BOOLEAN
AmdPcdInit(
  IN       UINT32               NumberOfPcdEntries,
  IN       AMD_PCD_LIST         *AmdPcdList
  )
{
  AMD_PCD_LIST *PlatformPcdList;
  AMD_PCD_PTR *Pcd_Ptr_Buff;
  UINT32 i;

  PlatformPcdList = AmdPcdList;
  for (i = 0 ; i < NumberOfPcdEntries ; i++) {
    switch (PlatformPcdList[i].AmdPcdDataType) {
    case PCD_BOOL:
      LibPcdSetExBool (PlatformPcdList[i].AmdConfigurationParameterPcdGuid, PlatformPcdList[i].AmdConfigurationParameterPcdTokenNumber, (BOOLEAN)PlatformPcdList[i].Value);
      break;
    case PCD_UINT8:
      LibPcdSetEx8 (PlatformPcdList[i].AmdConfigurationParameterPcdGuid, PlatformPcdList[i].AmdConfigurationParameterPcdTokenNumber, (UINT8)PlatformPcdList[i].Value);
      break;

    case PCD_UINT16:
      LibPcdSetEx16 (PlatformPcdList[i].AmdConfigurationParameterPcdGuid, PlatformPcdList[i].AmdConfigurationParameterPcdTokenNumber, (UINT16)PlatformPcdList[i].Value);
      break;

    case PCD_UINT32:
      LibPcdSetEx32 (PlatformPcdList[i].AmdConfigurationParameterPcdGuid, PlatformPcdList[i].AmdConfigurationParameterPcdTokenNumber, (UINT32)PlatformPcdList[i].Value);
      break;

    case PCD_UINT64:
      LibPcdSetEx64 (PlatformPcdList[i].AmdConfigurationParameterPcdGuid, PlatformPcdList[i].AmdConfigurationParameterPcdTokenNumber, (UINT64)PlatformPcdList[i].Value);
      break;

    case PCD_PTR:
      Pcd_Ptr_Buff = (AMD_PCD_PTR *)(UINTN)PlatformPcdList[i].Value;
      LibPcdSetExPtr (PlatformPcdList[i].AmdConfigurationParameterPcdGuid, PlatformPcdList[i].AmdConfigurationParameterPcdTokenNumber, &(Pcd_Ptr_Buff->BufferSize), Pcd_Ptr_Buff->Buffer);
      break;
    }
  }
  return TRUE;
}
