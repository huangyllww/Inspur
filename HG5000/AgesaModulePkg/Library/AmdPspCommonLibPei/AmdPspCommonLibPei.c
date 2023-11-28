/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP V1 Base Library
 *
 * Contains interface to the PSP library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 317842 $   @e \$Date: 2015-04-29 15:06:52 +0800 (Wed, 29 Apr 2015) $
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
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <PiPei.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Ppi/AmdPspCommonServicePpi.h>
#include <Filecode.h>

#define FILECODE LIBRARY_AMDPSPCOMMONLIBPEI_AMDPSPCOMMONLIBPEI_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
GetPspServicePointer (
  AMD_PSP_COMMON_SERVICE_PPI **PspServicePtr
  )
{
  EFI_PEI_SERVICES          **PeiServices;
  EFI_STATUS                Status;
  PeiServices = NULL;
  PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();
  ASSERT (PeiServices != NULL);

  if (PeiServices != NULL) {
    Status  = (*PeiServices)->LocatePpi (
              PeiServices,
              &gAmdPspCommonServicePpiGuid,
              0,
              NULL,
              PspServicePtr
              );
    return Status;
  }
  return EFI_UNSUPPORTED;
}

BOOLEAN
GetFtpmControlArea (
  IN OUT   VOID **FtpmControlArea
  )
{
  EFI_STATUS Status;
  AMD_PSP_COMMON_SERVICE_PPI *PspService;

  Status = GetPspServicePointer (&PspService);

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return PspService->GetFtpmControlArea (FtpmControlArea);
}

VOID
SwitchPspMmioDecode (
  IN       BOOLEAN SwitchFlag,
  IN OUT   UINT32 *RegisterCopy
  )
{
  EFI_STATUS Status;
  AMD_PSP_COMMON_SERVICE_PPI *PspService;

  Status = GetPspServicePointer (&PspService);

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return;
  }
  PspService->SwitchPspMmioDecode (SwitchFlag, RegisterCopy);
}

BOOLEAN
CheckPspDevicePresent (
  VOID
  )
{
  EFI_STATUS Status;
  AMD_PSP_COMMON_SERVICE_PPI *PspService;

  Status = GetPspServicePointer (&PspService);

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }
  return PspService->CheckPspDevicePresent ();
}

EFI_STATUS
CheckFtpmCaps (
  IN OUT   UINT32 *Caps
  )
{
  EFI_STATUS Status;
  AMD_PSP_COMMON_SERVICE_PPI *PspService;

  Status = GetPspServicePointer (&PspService);

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  return PspService->CheckFtpmCaps (Caps);
}

BOOLEAN
PSPEntryInfo (
  IN       PSP_DIRECTORY_ENTRY_TYPE    EntryType,
  IN OUT   UINT64                      *EntryAddress,
  IN       UINT32                      *EntrySize
  )
{
  EFI_STATUS Status;
  AMD_PSP_COMMON_SERVICE_PPI *PspService;

  Status = GetPspServicePointer (&PspService);

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return PspService->PSPEntryInfo (EntryType, EntryAddress, EntrySize);
}

BOOLEAN
PspLibTimeOut (
  IN       UINT64              uSec,
  IN       FP_CONDITIONER      Conditioner,
  IN       VOID                *Context
  )
{
  EFI_STATUS Status;
  AMD_PSP_COMMON_SERVICE_PPI *PspService;

  Status = GetPspServicePointer (&PspService);

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return FALSE;
  }

  return PspService->PspLibTimeOut (uSec, Conditioner, Context);

}

VOID
AcquirePspSmiRegMutex (
  VOID
  )
{
  EFI_STATUS Status;
  AMD_PSP_COMMON_SERVICE_PPI *PspService;

  Status = GetPspServicePointer (&PspService);

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return;
  }

  PspService->AcquirePspSmiRegMutex ();
}

VOID
ReleasePspSmiRegMutex (
  VOID
  )
{
  EFI_STATUS Status;
  AMD_PSP_COMMON_SERVICE_PPI *PspService;

  Status = GetPspServicePointer (&PspService);

  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return;
  }

  PspService->ReleasePspSmiRegMutex ();
}