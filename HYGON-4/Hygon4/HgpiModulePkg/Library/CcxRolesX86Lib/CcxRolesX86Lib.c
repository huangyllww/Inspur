/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CCX Roles Library - x86
 *
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

#include <Library/BaseLib.h>
#include "HYGON.h"
#include "Filecode.h"
#include <Library/CcxBaseX86Lib.h>
#include <Library/HygonSocBaseLib.h>

#define FILECODE  LIBRARY_CCXROLESX86LIB_CCXROLESX86LIB_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------------------*/

/**
 *  Is this the BSP core?
 *
 *  @param[in,out]   StdHeader        Header for library and services
 *
 *  @retval          TRUE             Is BSP core
 *  @retval          FALSE            Is not BSP Core
 *
 */
BOOLEAN
CcxIsBsp (
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT64  ApicBar;

  ApicBar = AsmReadMsr64 (0x0000001B);
  return (BOOLEAN)((ApicBar & BIT8) != 0);
}

/*---------------------------------------------------------------------------------------*/

/**
 *  Is the current core a primary core of it's compute unit?
 *
 *  @param[in]  StdHeader   Config handle for library and services.
 *
 *  @retval        TRUE  Is Primary Core
 *  @retval        FALSE Is not Primary Core
 *
 */
BOOLEAN
CcxIsComputeUnitPrimary (
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT32      ThreadsPerCore;
  CPUID_DATA  CpuId;

  AsmCpuid (
    0x8000001E,
    &(CpuId.EAX_Reg),
    &(CpuId.EBX_Reg),
    &(CpuId.ECX_Reg),
    &(CpuId.EDX_Reg)
    );
  ThreadsPerCore = (((CpuId.EBX_Reg >> 8) & 0xFF) + 1);

  return (BOOLEAN)((CcxGetInitialApicID (StdHeader) % ThreadsPerCore) == 0);
}

/*---------------------------------------------------------------------------------------*/

/**
 *  Is the current core a primary core of its complex?
 *
 *  @param[in]  StdHeader   Config handle for library and services.
 *
 *  @retval        TRUE  Is Primary Core
 *  @retval        FALSE Is not Primary Core
 *
 */
BOOLEAN
CcxIsComplexPrimary (
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  MaskValue;
  UINT32  CpuModel;
  
  CpuModel = GetHygonSocModel ();

  switch (CpuModel) {
    case HYGON_GX_CPU:
      MaskValue = 0x0F;
      break;

    case HYGON_EX_CPU:
      MaskValue = 0x07;
      break;
      
    default:
      MaskValue = 0x1F;
      break;
  }

  return (BOOLEAN)((CcxGetInitialApicID (StdHeader) & MaskValue) == 0);
}

/*---------------------------------------------------------------------------------------*/

/**
 *  Is the current core a primary core of it's node?
 *
 *  @param[in]  StdHeader   Config handle for library and services.
 *
 *  @retval        TRUE  Is Primary Core
 *  @retval        FALSE Is not Primary Core
 *
 */
BOOLEAN
CcxIsDiePrimary (
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  MaskValue;
  UINT32  CpuModel;
  
  CpuModel = GetHygonSocModel ();

  switch (CpuModel) {
    case HYGON_EX_CPU:
    case HYGON_GX_CPU:
      MaskValue = 0x1F;
      break;
      
    default:
      MaskValue = 0x3F;
      break;
  }

  return (BOOLEAN)((CcxGetInitialApicID (StdHeader) & MaskValue) == 0);
}

/*---------------------------------------------------------------------------------------*/

/**
 *  Is the current core a primary core of it's node?
 *
 *  @param[in]  StdHeader   Config handle for library and services.
 *
 *  @retval        TRUE  Is Primary Core
 *  @retval        FALSE Is not Primary Core
 *
 */
BOOLEAN
CcxIsSocketPrimary (
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  MaskValue;
  UINT32  CpuModel;
  
  CpuModel = GetHygonSocModel ();

  switch (CpuModel) {
    case HYGON_EX_CPU:
      MaskValue = 0x7F;
      break;
    
    case HYGON_GX_CPU:
      MaskValue = 0xFF;
      break;
    
    default:
      MaskValue = 0xFF;
      break;
  }

  return (BOOLEAN)((CcxGetInitialApicID (StdHeader) & MaskValue) == 0);     // only support 1socket 4CDD
}
