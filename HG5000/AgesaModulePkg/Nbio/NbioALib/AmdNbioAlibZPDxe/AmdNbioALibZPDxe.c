/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdNbioALibZPDxe Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioALibZPDxe
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
 *
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
 ******************************************************************************
 */
#include <Filecode.h>
#include <PiDxe.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AmdNbioBaseServicesProtocol.h>
#include <Library/UefiLib.h>
#include <Protocol/AcpiTable.h>
#include <Library/NbioIommuIvrsLib.h>
#include <Protocol/AmdNbioAlibServicesProtocol.h>
#include <CcxRegistersZP.h>
#include "AlibSsdtZP.h"
#include "AlibSsdtStub.h"
#include "AmdNbioAlibCallback.h"

#define FILECODE NBIO_NBIOALIB_AMDNBIOALIBZPDXE_AMDNBIOALIBZPDXE_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
AmdGetAlibAddress (
  IN       DXE_AMD_NBIO_ALIB_SERVICES_PROTOCOL      *This,
     OUT   VOID                                     **AlibAddressOutput
  )
{
  UINT32                               PackageType;

  PackageType = LibAmdGetPackageType ((AMD_CONFIG_PARAMS *) NULL);
  if (((1 << ZP_SOCKET_SP3) == PackageType) || ((1 << ZP_SOCKET_SP4) == PackageType)) {
    *AlibAddressOutput = &AlibSsdtStub;
  } else if ((1 << ZP_SOCKET_SP3r2) == PackageType) {
    *AlibAddressOutput = &AlibSsdtZP;
  } else {
    *AlibAddressOutput = &AlibSsdtZP;
  }

}

DXE_AMD_NBIO_ALIB_SERVICES_PROTOCOL mDxeAmdNbioAlibServicesProtocol = {
  AMD_ALIB_SERVICES_DXE_REVISION,
  AmdGetAlibAddress
};

/**
 *---------------------------------------------------------------------------------------
 *  InstallALibAcpiTable
 *
 *  Description:
 *     notification event handler for install ALib Acpi Table
 *  Parameters:
 *    @param[in]     Event      Event whose notification function is being invoked.
 *    @param[in]     *Context   Pointer to the notification function's context.
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
EFIAPI
InstallALibAcpiTable (
  IN       EFI_EVENT  Event,
  IN       VOID       *Context
  )
{
  EFI_ACPI_TABLE_PROTOCOL *AcpiTable;
  EFI_STATUS              Status;
  VOID                    *AlibBuffer;

  IDS_HDT_CONSOLE (MAIN_FLOW, "InstallALibAcpiTable Start\n");
  AlibBuffer = NULL;

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  if (EFI_ERROR (Status)) {
    return;
  }
  //
  // Create ACPI ALIB SSDT table
  //
  PcieAlibBuildAcpiTable ((VOID **)&AlibBuffer);


  //
  // Close event, so it will not be invoked again.
  //
  gBS->CloseEvent (Event);
  IDS_HDT_CONSOLE (MAIN_FLOW, "InstallALibAcpiTable End\n");
}


EFI_STATUS
EFIAPI
AmdNbioAlibZpDxeEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           Handle;
  VOID *Registration;

  AGESA_TESTPOINT (TpNbioAlibDxeEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioAlibEntry\n");
  Handle = NULL;

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdNbioALibServicesProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mDxeAmdNbioAlibServicesProtocol
                  );
  EfiCreateProtocolNotifyEvent (&gEfiAcpiTableProtocolGuid, TPL_CALLBACK, InstallALibAcpiTable, NULL, &Registration);

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioAlibExit\n");
  AGESA_TESTPOINT (TpNbioAlibDxeExit, NULL);

  return Status;
}
