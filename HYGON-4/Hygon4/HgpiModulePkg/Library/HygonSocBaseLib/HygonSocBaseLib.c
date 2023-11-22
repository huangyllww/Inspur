/* $NoKeywords:$ */

/**
 * @file
 *
 * HygonSocBaseLib.c
 *
 * Feature Soc common functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: HGPI
 * @e sub-project: SOC
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

#include <Library/BaseLib.h>
#include <Include/Filecode.h>
#include <Library/PcdLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/BaseSocLogicalIdXlatLib.h>

#define FILECODE  LIBRARY_HYGONSOCBASELIB_HYGONSOCBASELIB_FILECODE

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

/*---------------------------------------------------------------------------------------*/

/**
 * Returns the raw family / model / stepping for the executing core
 *
 * @returns       Raw CPU ID for the executing core
 */
UINT32
BaseGetRawIdOnExecutingCore (
  VOID
  )
{
  CPUID_DATA  CpuId;

  AsmCpuid (
    HYGON_CPUID_FMF,
    &(CpuId.EAX_Reg),
    &(CpuId.EBX_Reg),
    &(CpuId.ECX_Reg),
    &(CpuId.EDX_Reg)
    );

  return CpuId.EAX_Reg;
}

/**
 *  Get SOC Model number
 *
 *  @param[in,out]   void
 *
 *  @retval          SOC model number
 *
 */
UINT32
GetHygonSocModel (
  VOID
  )
{
  UINT32  RegEax;
  UINT32  ModelNumber;

  AsmCpuid (0x80000001, &RegEax, NULL, NULL, NULL);
  ModelNumber = (RegEax >> 4) & 0xF;

  return ModelNumber;
}


/*---------------------------------------------------------------------------------------*/

/**
 * Returns the FamilyId for the executing core
 *
 * @returns       FamilyId for the executing core
 */
UINT32
GetFamilyId (
  VOID
  )
{
  UINT32  RawId;
  UINT32  BaseFamilyId;
  UINT32  ExtFamilyId;

  RawId = BaseGetRawIdOnExecutingCore ();
  BaseFamilyId = (RawId & CPUID_BASE_FAMILY_MASK) >> CPUID_BASE_FAMILY_OFFSET;
  ExtFamilyId  = (RawId & CPUID_EXT_FAMILY_MASK)  >> CPUID_EXT_FAMILY_OFFSET;

  return (UINT32)(BaseFamilyId + ExtFamilyId);
}

/*---------------------------------------------------------------------------------------*/

/**
 * Returns the PkgType for the executing core
 *
 * @returns       PkgType for the executing core
 */
UINT8
GetSocPkgType (
  VOID
  )
{
  CPUID_DATA  CpuId;

  AsmCpuid (
    HYGON_CPUID_FMF,
    &(CpuId.EAX_Reg),
    &(CpuId.EBX_Reg),
    &(CpuId.ECX_Reg),
    &(CpuId.EDX_Reg)
    );

  // Check Hardware Identification

  if (F18_DM_RAW_ID != (CpuId.EAX_Reg & RAW_FAMILY_ID_MASK)) {
    return ST_UNKNOWN;
  }

  return ((CpuId.EBX_Reg >> 28) & 0xF);
}

/**
 * Returns the logical family / revision for the executing core
 *
 * @param[out]    LogicalId   Executing core's logical faily and logical revision
 *
 * @retval        TRUE        Executing core's logical ID was successfully found
 * @retval        FALSE       The executing core's family is unknown
 */
BOOLEAN
BaseGetLogicalIdOnExecutingCore (
  OUT SOC_LOGICAL_ID  *LogicalId
  )
{
  return (BaseSocConvertRawToLogical (BaseGetRawIdOnExecutingCore (), LogicalId));
}

/**
 *
 *
 *      This function check SOC family identification ID
 *
 *     @param[in]   NULL
 *
 *     @return          TRUE -  Hardware identification ID check pass.
 *     @return          FALSE - Hardware identification ID check fail.
 */
BOOLEAN
SocFamilyIdentificationCheck (
  VOID
  )
{
  UINT32  SocFMSRawId;

  SocFMSRawId = BaseGetRawIdOnExecutingCore ();
  if (F18_DM_RAW_ID == (SocFMSRawId & RAW_FAMILY_ID_MASK)) {
    return TRUE;
  }

  return FALSE;
}

BOOLEAN
HygonPcdInit (
  IN       UINT32               NumberOfPcdEntries,
  IN       HYGON_PCD_LIST         *HygonPcdList
  )
{
  HYGON_PCD_LIST  *PlatformPcdList;
  HYGON_PCD_PTR   *Pcd_Ptr_Buff;
  UINT32          i;

  PlatformPcdList = HygonPcdList;
  for (i = 0; i < NumberOfPcdEntries; i++) {
    switch (PlatformPcdList[i].HygonPcdDataType) {
      case PCD_BOOL:
        LibPcdSetExBoolS (PlatformPcdList[i].HygonConfigurationParameterPcdGuid, PlatformPcdList[i].HygonConfigurationParameterPcdTokenNumber, (BOOLEAN)PlatformPcdList[i].Value);
        break;
      case PCD_UINT8:
        LibPcdSetEx8S (PlatformPcdList[i].HygonConfigurationParameterPcdGuid, PlatformPcdList[i].HygonConfigurationParameterPcdTokenNumber, (UINT8)PlatformPcdList[i].Value);
        break;

      case PCD_UINT16:
        LibPcdSetEx16S (PlatformPcdList[i].HygonConfigurationParameterPcdGuid, PlatformPcdList[i].HygonConfigurationParameterPcdTokenNumber, (UINT16)PlatformPcdList[i].Value);
        break;

      case PCD_UINT32:
        LibPcdSetEx32S (PlatformPcdList[i].HygonConfigurationParameterPcdGuid, PlatformPcdList[i].HygonConfigurationParameterPcdTokenNumber, (UINT32)PlatformPcdList[i].Value);
        break;

      case PCD_UINT64:
        LibPcdSetEx64S (PlatformPcdList[i].HygonConfigurationParameterPcdGuid, PlatformPcdList[i].HygonConfigurationParameterPcdTokenNumber, (UINT64)PlatformPcdList[i].Value);
        break;

      case PCD_PTR:
        Pcd_Ptr_Buff = (HYGON_PCD_PTR *)(UINTN)PlatformPcdList[i].Value;
        LibPcdSetExPtrS (PlatformPcdList[i].HygonConfigurationParameterPcdGuid, PlatformPcdList[i].HygonConfigurationParameterPcdTokenNumber, &(Pcd_Ptr_Buff->BufferSize), Pcd_Ptr_Buff->Buffer);
        break;
    }
  }

  return TRUE;
}
