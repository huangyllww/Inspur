/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Fabric ACPI CDIT.
 *
 * This funtion collect CDIT information.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Fabric
 *
 */
/*
 ****************************************************************************
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
#include "Porting.h"
#include "HYGON.h"
#include "Library/HygonBaseLib.h"
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersST.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/HygonIdsHookLib.h>
#include "Library/UefiBootServicesTableLib.h"
#include <Protocol/HygonAcpiCditServicesProtocol.h>
#include <Protocol/FabricNumaServicesProtocol.h>
#include <FabricAcpiTable.h>
#include "Filecode.h"

#define FILECODE  FABRIC_HYGX_FABRICDXE_FABRICACPICDIT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
FabricGetCditInfo (
  IN       HYGON_FABRIC_ACPI_CDIT_SERVICES_PROTOCOL  *This,
  IN OUT   UINT8                                   *DomainCount,
  IN OUT   UINT8                                   *Distance
  );

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

STATIC HYGON_FABRIC_ACPI_CDIT_SERVICES_PROTOCOL  mFabricAcpiCditServicesProtocol = {
  0x1,
  FabricGetCditInfo
};

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
FabricStAcpiCditProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return (gBS->InstallProtocolInterface (
                 &ImageHandle,
                 &gHygonFabricAcpiCditServicesProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &mFabricAcpiCditServicesProtocol
                 ));
}

EFI_STATUS
EFIAPI
FabricGetCditInfo (
  IN       HYGON_FABRIC_ACPI_CDIT_SERVICES_PROTOCOL  *This,
  IN OUT   UINT8                                   *DomainCount,
  IN OUT   UINT8                                   *Distance
  )
{
  UINT32                         NumOfDomain;
  DOMAIN_INFO                    *DomainInfo;
  UINT32                          NumOfCxlDomain;
  CXL_DOMAIN_INFO                *CxlDomainInfo;
  FABRIC_NUMA_SERVICES_PROTOCOL  *FabricNuma;

  // Locate Fabric NUMA Services Protocol
  if (gBS->LocateProtocol (&gHygonFabricNumaServicesProtocolGuid, NULL, (VOID **)&FabricNuma) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  FabricNuma->GetDomainInfo (FabricNuma, &NumOfDomain, &DomainInfo, &NumOfCxlDomain, &CxlDomainInfo);
  *DomainCount = (UINT8) (NumOfDomain + NumOfCxlDomain);

  return FabricGetDistanceInfo (Distance, NumOfDomain, DomainInfo, NumOfCxlDomain, CxlDomainInfo);
}
