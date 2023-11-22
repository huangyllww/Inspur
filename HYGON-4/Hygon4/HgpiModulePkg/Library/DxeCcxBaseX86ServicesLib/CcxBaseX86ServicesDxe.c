/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CCX BASE Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CCX
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

#include "Uefi.h"
#include <Library/BaseLib.h>
#include "HYGON.h"
#include "Filecode.h"
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/CcxBaseX86Lib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/CcxBaseServicesProtocol.h>
#include "CcxBaseX86ServicesDxe.h"

#define FILECODE  LIBRARY_DXECCXBASEX86SERVICESLIB_CCXBASEX86SERVICESDXE_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
STATIC CCX_BASE_SERVICES_PROTOCOL  mCcxBaseServices = {
  0x1,
  CcxGetMaxPhysAddr,
  CcxGetMaxProcCap
};

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
CcxBaseServicesProtocolInstall (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  return (gBS->InstallMultipleProtocolInterfaces (
                 &ImageHandle,
                 &gHygonCcxBaseServicesProtocolGuid,
                 &mCcxBaseServices,
                 NULL
                 ));
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  Returns the maximum physical address size
 *
 *  Parameters:
 *    @param[in]       This    Pointer to the CCX_BASE_SERVICES_PROTOCOL instance.
 *
 *    @return          UINT8   Maximum physical address size
 *
 *---------------------------------------------------------------------------------------
 */
UINT8
EFIAPI
CcxGetMaxPhysAddr (
  IN       CCX_BASE_SERVICES_PROTOCOL *This
  )
{
  UINT8       MemEncryptPhysAddWidth;
  UINT32      LFuncExt;
  CPUID_DATA  CpuId;

  MemEncryptPhysAddWidth = 0;

  LFuncExt = CcxGetLFuncExt ();

  if (LFuncExt >= 0x8000001F) {
    // Get value of CPUID_Fn8000001F_EBX[MemEncryptPhysAddWidth]
    AsmCpuid (
      0x8000001F,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
    MemEncryptPhysAddWidth = (UINT8)((CpuId.EBX_Reg >> 6) & 0x3F);
  }

  return CcxGetPhysAddrSize () - MemEncryptPhysAddWidth;
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  Returns the maximum processor capacity in the system
 *
 *  Parameters:
 *    @param[in]       This    Pointer to the CCX_BASE_SERVICES_PROTOCOL instance.
 *
 *    @return          UINT8   Maximum processor capacity
 *
 *---------------------------------------------------------------------------------------
 */
UINT8
EFIAPI
CcxGetMaxProcCap (
  IN       CCX_BASE_SERVICES_PROTOCOL *This
  )
{
  return ((1 << CcxGetApicIdCoreIdSize ()) * PcdGet8 (PcdHygonNumberOfPhysicalSocket));
}
