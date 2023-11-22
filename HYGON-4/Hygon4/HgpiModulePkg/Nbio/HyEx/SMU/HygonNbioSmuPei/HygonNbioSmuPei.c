/* $NoKeywords:$ */

/**
 * @file
 *
 * HygonNbioSmuPei Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonNbioSmuPei
 *
 */
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
 ******************************************************************************
 */
#include <PiPei.h>
#include <Filecode.h>
#include <GnbHsio.h>
#include <Gnb.h>
#include <HygonNbioSmuPei.h>
#include <GnbRegisters.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/PcdLib.h>
#include <Library/HygonPspBaseLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/NbioHandleLib.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Ppi/NbioBaseServicesSTPpi.h>
#include <Ppi/NbioPcieServicesPpi.h>
#include <Ppi/HygonCoreTopologyServicesPpi.h>
#include <Guid/GnbNbioBaseSTInfoHob.h>
#include <IdsHookId.h>
#include <Library/HygonIdsHookLib.h>

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

extern
PEI_HYGON_NBIO_SMU_SERVICES_PPI  mNbioSmuServicePpi;

extern
HYGON_CORE_TOPOLOGY_SERVICES_PPI  mHygonCoreTopologyServicesPpi;

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define FILECODE  NBIO_SMU_HYEX_HYGONNBIOSMUPEI_HYGONNBIOSMUPEI_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
DispatchSmuCallback (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  );

EFI_STATUS
EFIAPI
InstallSmuPpi (
  IN CONST EFI_PEI_SERVICES         **PeiServices
  );

/*----------------------------------------------------------------------------------------
 *                    P P I   N O T I F Y   D E S C R I P T O R S
 *----------------------------------------------------------------------------------------
 */

STATIC EFI_PEI_PPI_DESCRIPTOR  mNbioSmuServicesPpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonNbioSmuServicesPpiGuid,
  &mNbioSmuServicePpi
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mNotifySmuPpi = {
  EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
  &gHygonNbioBaseServicesStPpiGuid,
  DispatchSmuCallback
};

EFI_STATUS
EFIAPI
InstallSmuPpi (
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS  Status;

  IDS_HDT_CONSOLE (MAIN_FLOW, "InstallSmuPpi\n");
  Status = (**PeiServices).InstallPpi (PeiServices, &mNbioSmuServicesPpiList);  // This driver is done.

  return Status;
}

STATIC EFI_PEI_PPI_DESCRIPTOR  mHygonCoreTopologyServicesPpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonCoreTopologyServicesPpiGuid,
  &mHygonCoreTopologyServicesPpi
};

EFI_STATUS
EFIAPI
InstallCoreToplogyServicesPpi (
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS  Status;

  IDS_HDT_CONSOLE (MAIN_FLOW, "InstallCoreToplogyServicesPpi\n");
  Status = (**PeiServices).InstallPpi (PeiServices, &mHygonCoreTopologyServicesPpiList);  // This driver is done.

  return Status;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Dispatch Smu Ppi callback
 * This function should use the information from memory config to initialize NBIO top of memory
 *
 *
 *
 * @param[in]  PeiServices       Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  NotifyDescriptor  NotifyDescriptor pointer
 * @param[in]  Ppi               Ppi pointer
 * @retval     EFI_STATUS
 */
EFI_STATUS
EFIAPI
DispatchSmuCallback (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS  Status;

  HGPI_TESTPOINT (TpDispatchSmuCallbackEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "DispatchSmuCallback Entry\n");
  Status = InstallSmuPpi (PeiServices);
  Status = InstallCoreToplogyServicesPpi (PeiServices);
  IDS_HDT_CONSOLE (MAIN_FLOW, "DispatchSmuCallback Exit\n");
  HGPI_TESTPOINT (TpDispatchSmuCallbackExit, NULL);

  return Status;
}

EFI_STATUS
EFIAPI
HygonNbioSmuPeiHyExEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                        Status;
  PEI_HYGON_NBIO_BASE_SERVICES_PPI  *NbioBaseServicesPpi;

  HGPI_TESTPOINT (TpNbioSmuPeiEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioSmuHyExPeiEntry Entry\n");
  IDS_HOOK (IDS_HOOK_NBIO_SMU_INIT, NULL, NULL);
  //
  // Waiting for Nbio Base Sevice ready.
  //
  Status = PeiServicesLocatePpi (
             &gHygonNbioBaseServicesStPpiGuid,
             0,
             NULL,
             (VOID **)&NbioBaseServicesPpi
             );
  if (Status == EFI_SUCCESS) {
    Status = InstallSmuPpi (PeiServices);
    Status = InstallCoreToplogyServicesPpi (PeiServices);
  } else {
    //
    // Notify event
    //
    Status = (**PeiServices).NotifyPpi (PeiServices, &mNotifySmuPpi);
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioSmuHyExPeiEntry Exit\n");
  HGPI_TESTPOINT (TpNbioSmuPeiExit, NULL);

  return Status;
}
