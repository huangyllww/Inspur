/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Fabric ACPI Domain infor for SRAT & CDIT.
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
#include "FabricAcpiDomainInfo.h"
#include "FabricAcpiTable.h"
#include <Library/BaseLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersST.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/HygonIdsHookLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/FabricNumaServicesProtocol.h>
#include <Protocol/FabricTopologyServices.h>
#include "Filecode.h"

#define FILECODE  FABRIC_HYEX_FABRICDXE_FABRICACPIDOMAININFO_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
BuildDomainInfoHyEx (
  VOID
  );

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

BOOLEAN      mDomainInfoValid = FALSE;
UINT32       mNumberOfDomains = 0;
DOMAIN_INFO  mDomainInfo[MAX_SOCKETS_SUPPORTED * MAX_CDDS_PER_SOCKET];
CDD_INFO     mCddInfo[MAX_SOCKETS_SUPPORTED][MAX_CDDS_PER_SOCKET];

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
FabricStGetDomainInfo (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
     OUT   UINT32                         *NumberOfDomainsInSystem,
     OUT   DOMAIN_INFO                   **DomainInfo,
     OUT   UINT32                         *NumberOfCxlDomainsInSystem,
     OUT   CXL_DOMAIN_INFO               **CxlDomainInfo
  );

EFI_STATUS
EFIAPI
FabricStDramDomainXlat (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
  IN       UINTN                           Socket,
  IN       UINTN                           Cdd,
     OUT   UINT32                         *Domain
  );

EFI_STATUS
EFIAPI
FabricStGetMaxDomains (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
  IN       UINT8                           *MaxDramDomains,
  IN       UINT8                           *MaxCxlDomains
  );

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
STATIC FABRIC_NUMA_SERVICES_PROTOCOL  mFabricNumaServicesProtocol = {
  0x2,
  FabricStGetDomainInfo,
  FabricStDramDomainXlat,
  NULL,
  FabricStGetMaxDomains
};

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
FabricStNumaServicesProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  BuildDomainInfoHyEx ();
  return (gBS->InstallProtocolInterface (
                &ImageHandle,
                &gHygonFabricNumaServicesProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mFabricNumaServicesProtocol
                ));
}

EFI_STATUS
EFIAPI
FabricStGetDomainInfo (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
     OUT   UINT32                         *NumberOfDomainsInSystem,
     OUT   DOMAIN_INFO                   **DomainInfo,
     OUT   UINT32                         *NumberOfCxlDomainsInSystem,
     OUT   CXL_DOMAIN_INFO               **CxlDomainInfo
  )
{
  ASSERT (mDomainInfoValid);

  if ((NumberOfDomainsInSystem == NULL) && (DomainInfo == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (NumberOfDomainsInSystem != NULL) {
    *NumberOfDomainsInSystem = mNumberOfDomains;
  }

  if (DomainInfo != NULL) {
    *DomainInfo = &mDomainInfo[0];
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FabricStDramDomainXlat (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
  IN       UINTN                           Socket,
  IN       UINTN                           Cdd,
     OUT   UINT32                         *Domain
  )
{
  ASSERT (mDomainInfoValid);

  if (Socket >= FabricTopologyGetNumberOfSocketPresent ()) {
    return EFI_INVALID_PARAMETER;
  }

  if (Cdd >= MAX_CDDS_PER_SOCKET) {
    return EFI_INVALID_PARAMETER;
  }

  if (Domain == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Domain = mCddInfo[Socket][Cdd].Domain;
  return EFI_SUCCESS;
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  Returns the maximum number of domains in the system
 *
 *  Parameters:
 *    @param[in]       This    Pointer to the FABRIC_NUMA_SERVICES_PROTOCOL instance.
 *
 *    @return          UINT8   Maximum number of domains in the system
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
FabricStGetMaxDomains (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
  IN       UINT8                           *MaxDramDomains,
  IN       UINT8                           *MaxCxlDomains
  )
{
  if (MaxDramDomains != NULL) {
    *MaxDramDomains = PcdGet8 (PcdHygonNumberOfPhysicalSocket) * (UINT8)FabricTopologyGetNumberOfCddsOnSocket (0);
  }

  if (MaxCxlDomains != NULL) {
    *MaxCxlDomains = 0;
  }
  return EFI_SUCCESS;
}

VOID
BuildDomainInfoHyEx (
  VOID
  )
{
  UINTN                        NumberOfSockets;
  UINTN                        NumberOfCddsOnSocket;
  UINTN                        CddsPresentOnSocket;
  UINT32                       i;
  UINT32                       j;
  UINT32                       k;
  UINT32                       MapPairIndex;
  UINT32                       CddsAccountedForInDomain[(MAX_SOCKETS_SUPPORTED * MAX_CDDS_PER_SOCKET)];
  UINT32                       TotalEntities[(MAX_SOCKETS_SUPPORTED * MAX_CDDS_PER_SOCKET)];
  UINT32                       LogicalDieId;
  DRAM_INFO                    DramInfo[NUMBER_OF_DRAM_REGIONS];
  DRAM_BASE_ADDRESS_REGISTER   DramBaseAddr;
  DRAM_LIMIT_ADDRESS_REGISTER  DramLimitAddr;
  UINT8                        NumaNodePerSocket;

  ASSERT (!mDomainInfoValid);
  ASSERT (mNumberOfDomains == 0);
  
  NumaNodePerSocket = PcdGet8 (PcdNumaNodePerSocket);

  // Collect raw data
  for (i = 0; i < NUMBER_OF_DRAM_REGIONS; i++) {
    DramBaseAddr.Value = FabricRegisterAccRead (0, 0, DRAMBASEADDR0_FUNC, (DRAMBASEADDR0_REG + (i * DRAM_REGION_REGISTER_OFFSET)), IOMS0_INSTANCE_ID);
    if (DramBaseAddr.Field.AddrRngVal == 1) {
      DramLimitAddr.Value = FabricRegisterAccRead (0, 0, DRAMLIMITADDR0_FUNC, (DRAMLIMITADDR0_REG + (i * DRAM_REGION_REGISTER_OFFSET)), IOMS0_INSTANCE_ID);
      FabricTopologyGetLocationFromFabricId (DramLimitAddr.HyExField.DstFabricID, &DramInfo[i].Socket, &LogicalDieId, NULL);
      DramInfo[i].Cdd = (UINT32)GetCddPhysicalDieId (LogicalDieId) - FABRIC_ID_CDD0_DIE_NUM;
      DramInfo[i].SocketMask = 0xFFFFFFFF << DramBaseAddr.Field.IntLvNumSockets;
      DramInfo[i].CddMask = 0xFFFFFFFF << DramLimitAddr.HyExField.IntLvNumDies;
      DramInfo[i].SocketCount = 1 << DramBaseAddr.Field.IntLvNumSockets;
      DramInfo[i].CddCount = 1 << DramLimitAddr.HyExField.IntLvNumDies;
    } else {
      DramInfo[i].SocketCount = 0;
      DramInfo[i].CddCount    = 0;
      DramInfo[i].Socket     = 0xFF;
      DramInfo[i].Cdd        = 0xFF;
      DramInfo[i].SocketMask = 0;
      DramInfo[i].CddMask    = 0;
    }
  }

  // Initialize domain info to none
  for (i = 0; i < (MAX_SOCKETS_SUPPORTED * MAX_CDDS_PER_SOCKET); i++) {
    CddsAccountedForInDomain[i] = 0;
    TotalEntities[i]    = 0;
    mDomainInfo[i].Type = MaxDomainType;
  }

  // Initialize die info to none
  for (i = 0; i < MAX_SOCKETS_SUPPORTED; i++) {
    for (j = 0; j < MAX_CDDS_PER_SOCKET; j++) {
      mCddInfo[i][j].Domain = 0xFF;
      for (k = 0; k < MAX_CHANNELS_PER_CDD_HYEX; k++) {
        mCddInfo[i][j].DramMapPair[k] = 0xFF;
      }
    }
  }

  // Build die and domain info
  NumberOfSockets = FabricTopologyGetNumberOfSocketPresent ();
  ASSERT (NumberOfSockets <= MAX_SOCKETS_SUPPORTED);
  for (i = 0; i < NumberOfSockets; i++) {
    NumberOfCddsOnSocket = FabricTopologyGetNumberOfCddsOnSocket (i);
    CddsPresentOnSocket  = FabricTopologyGetCddsPresentOnSocket (i);
    ASSERT (NumberOfCddsOnSocket <= MAX_CDDS_PER_SOCKET);

    if (NumaNodePerSocket > (UINT8)NumberOfCddsOnSocket) {
      NumaNodePerSocket = (UINT8)NumberOfCddsOnSocket;
    }

    for (j = 0; j < MAX_CDDS_PER_SOCKET; j++) {
      if (!IS_CDD_PRESENT (j, CddsPresentOnSocket)) {
        continue;
      }

      mCddInfo[i][j].Domain = mNumberOfDomains;
      CddsAccountedForInDomain[mNumberOfDomains]++;
      MapPairIndex = 0;
      for (k = 0; k < NUMBER_OF_DRAM_REGIONS; k++) {
        if ((DramInfo[k].Socket == (i & DramInfo[k].SocketMask)) && (DramInfo[k].Cdd == (j & DramInfo[k].CddMask))) {
          ASSERT (MapPairIndex < MAX_CHANNELS_PER_CDD_HYEX);
          mCddInfo[i][j].DramMapPair[MapPairIndex++] = k;
        }
      }

      if (MapPairIndex != 1) {
        mDomainInfo[mNumberOfDomains].Type = NoIntlv;
        mDomainInfo[mNumberOfDomains].Intlv.None.Socket  = i;
        mDomainInfo[mNumberOfDomains].Intlv.None.CddMap |= (1 << j);
        TotalEntities[mNumberOfDomains] = (UINT32)NumberOfCddsOnSocket / (UINT32)NumaNodePerSocket;
        
      } else if (CddsAccountedForInDomain[mNumberOfDomains] == 1) {
        if (DramInfo[mCddInfo[i][j].DramMapPair[0]].SocketCount > 1) {
          mDomainInfo[mNumberOfDomains].Type = SocketIntlv;
          mDomainInfo[mNumberOfDomains].Intlv.Socket.SocketCount = DramInfo[mCddInfo[i][j].DramMapPair[0]].SocketCount;
          TotalEntities[mNumberOfDomains] = mDomainInfo[mNumberOfDomains].Intlv.Socket.SocketCount * DramInfo[mCddInfo[i][j].DramMapPair[0]].CddCount;
          mDomainInfo[mNumberOfDomains].Intlv.Socket.SocketMap |= (1 << i);
          
        } else if (DramInfo[mCddInfo[i][j].DramMapPair[0]].CddCount > 1) {
          mDomainInfo[mNumberOfDomains].Type = DieIntlv;
          mDomainInfo[mNumberOfDomains].Intlv.Cdd.CddCount = DramInfo[mCddInfo[i][j].DramMapPair[0]].CddCount;
          TotalEntities[mNumberOfDomains] = mDomainInfo[mNumberOfDomains].Intlv.Cdd.CddCount;
          mDomainInfo[mNumberOfDomains].Intlv.Cdd.CddMap |= (1 << j);
          
        } else {
          mDomainInfo[mNumberOfDomains].Type = NoIntlv;
          TotalEntities[mNumberOfDomains] = (UINT32)NumberOfCddsOnSocket / (UINT32)NumaNodePerSocket;
          mDomainInfo[mNumberOfDomains].Intlv.None.Socket = i;
          mDomainInfo[mNumberOfDomains].Intlv.None.CddMap |= (1 << j);
        }
      }

      if (CddsAccountedForInDomain[mNumberOfDomains] == TotalEntities[mNumberOfDomains]) {
        mNumberOfDomains++;
      }
    }
  }

  mDomainInfoValid = TRUE;
}
