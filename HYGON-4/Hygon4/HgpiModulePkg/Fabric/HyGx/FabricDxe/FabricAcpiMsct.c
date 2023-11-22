/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Fabric ACPI MSCT.
 *
 * This function implements routines for MSCT creation.
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

#include "HGPI.h"
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/FabricTopologyServices.h>
#include <Protocol/HygonCoreTopologyProtocol.h>
#include <Protocol/FabricNumaServicesProtocol.h>
#include <Protocol/HygonAcpiSratServicesProtocol.h>
#include <Protocol/HygonAcpiMsctServicesProtocol.h>
#include <Library/HygonBaseLib.h>
#include "Filecode.h"

#define FILECODE  FABRIC_HYGX_FABRICDXE_FABRICACPIMSCT_FILECODE

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
FabricGetMsctInfo (
  IN       HYGON_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL *This,
  OUT   UINT32                                 *NumPopProxDomains,
  OUT   UINT32                                 *MaxNumProxDomains,
  OUT   MSCT_PROX_DOMAIN_INFO                  *MsctInfo
  );

HGPI_STATUS
GetNumberOfThreadsOnCdd (
  IN       UINTN  Socket,
  IN       UINTN  Cdd,
  OUT   UINTN  *NumberOfThreadsOnCdd
  );

STATIC HYGON_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL  mFabricAcpiMsctServicesProtocol = {
  0x1,
  FabricGetMsctInfo
};

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

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
FabricStAcpiMsctProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return (gBS->InstallProtocolInterface (
                 &ImageHandle,
                 &gHygonFabricAcpiMsctServicesProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &mFabricAcpiMsctServicesProtocol
                 ));
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  FabricGetMsctInfo
 *
 *  Description:
 *    This function will return the information necessary to generate MSCT
 *
 *  Parameters:
 *    @param[in]      This                       Pointer to HYGON_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL instance
 *    @param[out]     NumPopProxDomains          Number of populated proximity domains
 *    @param[out]     MaxNumProxDomains          Maximum number of proximity domains on the system
 *    @param[out]     MsctInfo                   Structure that describes MSCT proximity domains
 *
 *    @retval         EFI_ABORTED                Services protocol not available
 *                    EFI_SUCCESS                MSCT data successfully gathered
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
FabricGetMsctInfo (
  IN       HYGON_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL *This,
  OUT      UINT32                                 *NumPopProxDomains,
  OUT      UINT32                                 *MaxNumProxDomains,
  OUT      MSCT_PROX_DOMAIN_INFO                  *MsctInfo
  )
{
  UINT8                                     MaxProxDomains;
  UINT8                                     MaxDramProxDomains;
  UINT8                                     MaxCxlProxDomains;
  UINTN                                     NumberOfCddsOnSocket;
  UINTN                                     CddsPresentOnSocket;
  UINTN                                     NumberOfSockets;
  UINTN                                     NumberOfThreadsOnCdd[MAX_SOCKETS_SUPPORTED][MAX_CDDS_PER_SOCKET];
  UINT32                                    NumberOfThreadsOnSocket[MAX_SOCKETS_SUPPORTED];
  UINT32                                    NumberOfThreadsOnSystem;
  UINT32                                    i;
  UINT32                                    j;
  UINT32                                    CurrDomain;
  UINT32                                    NumberOfDramDomainsInSystem;
  UINT32                                    MemoryInfoCtr;
  EFI_STATUS                                CalledStatus;
  DOMAIN_TYPE                               CurrDomainType;
  DOMAIN_INFO                               *DomainInfo;
  MEMORY_INFO                               *MemoryInfo;
  MSCT_PROX_DOMAIN_INFO                     MsctDomains[MAX_PROXIMITY_DOMAINS];
  FABRIC_NUMA_SERVICES_PROTOCOL             *FabricNumaServices;
  HYGON_FABRIC_ACPI_SRAT_SERVICES_PROTOCOL  *FabricSratServices;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL   *FabricTopology;
  HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL     *CoreTopology;
  UINTN                                     SocketIndex, CddIndex;
  UINT32                                    CddNum;
  UINT32                                    NumberOfCxlDomainsInSystem;
  UINT32                                    TotalDomainsInSystem;
  CXL_DOMAIN_INFO                           *CxlDomainInfo;

  if (gBS->LocateProtocol (&gHygonFabricNumaServicesProtocolGuid, NULL, (VOID **)&FabricNumaServices) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  if (gBS->LocateProtocol (&gHygonFabricAcpiSratServicesProtocolGuid, NULL, (VOID **)&FabricSratServices) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  if (gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, (VOID **)&FabricTopology) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  if (gBS->LocateProtocol (&gHygonCoreTopologyServicesProtocolGuid, NULL, (VOID **)&CoreTopology) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  FabricNumaServices->GetMaxDomains (FabricNumaServices, &MaxDramProxDomains, &MaxCxlProxDomains);

  NumberOfThreadsOnSystem = 0;

  MaxProxDomains = MaxDramProxDomains + MaxCxlProxDomains;
  ASSERT (MaxProxDomains <= MAX_PROXIMITY_DOMAINS);

  if (FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  NumberOfThreadsOnSystem = 0;
  for (SocketIndex = 0; SocketIndex < NumberOfSockets; SocketIndex++) {
    if (FabricTopology->GetCddInfo (FabricTopology, SocketIndex, &NumberOfCddsOnSocket, &CddsPresentOnSocket) != EFI_SUCCESS) {
      return EFI_ABORTED;
    }

    NumberOfThreadsOnSocket[SocketIndex] = 0;
    for (CddIndex = 0; CddIndex < MAX_CDDS_PER_SOCKET; CddIndex++) {
      if (!IS_CDD_PRESENT (CddIndex, CddsPresentOnSocket)) {
        continue;
      }

      if (GetNumberOfThreadsOnCdd (SocketIndex, CddIndex, &NumberOfThreadsOnCdd[SocketIndex][CddIndex]) == HGPI_ERROR) {
        return EFI_ABORTED;
      }

      NumberOfThreadsOnSocket[SocketIndex] += (UINT32)NumberOfThreadsOnCdd[SocketIndex][CddIndex];
      NumberOfThreadsOnSystem += (UINT32)NumberOfThreadsOnCdd[SocketIndex][CddIndex];
    }
  }

  // Initialize MsctDomains
  for (i = 0; i < MaxProxDomains; i++) {
    MsctDomains[i].ProxDomain = 0;
    MsctDomains[i].MaxProcCap = 0;
    MsctDomains[i].MaxMemCap  = 0;
  }

  FabricNumaServices->GetDomainInfo (FabricNumaServices, &NumberOfDramDomainsInSystem, &DomainInfo, &NumberOfCxlDomainsInSystem, &CxlDomainInfo);
  TotalDomainsInSystem = NumberOfDramDomainsInSystem + NumberOfCxlDomainsInSystem;
  for (i = 0; i < TotalDomainsInSystem ; i++) {
    if (i < NumberOfDramDomainsInSystem) {
      CurrDomainType = DomainInfo[i].Type;
    } else {
      CurrDomainType = CxlDomainInfo[i - NumberOfDramDomainsInSystem].Type;
    }

    ASSERT (CurrDomainType < MaxDomainType);

    MsctDomains[i].ProxDomain = i;

    if (i < NumberOfDramDomainsInSystem) {
      if (CurrDomainType == SocketIntlv) {
        // If a domain is socket interleaved, all the threads on the system are on the same domain.
        // Use CoreTopology to get the total threads in the system.
        MsctDomains[i].MaxProcCap = NumberOfThreadsOnSystem;
      } else if (CurrDomainType == DieIntlv) {
        // If a domain is die interleaved, all the threads on the socket are on the same domain.
        MsctDomains[i].MaxProcCap = NumberOfThreadsOnSocket[DomainInfo[i].Intlv.None.Socket];
      } else if (CurrDomainType == NoIntlv) {
        // If a domain is not interleaved, only the threads on each cdd are on the same domain.
        for (CddNum = 0; CddNum < MAX_CDDS_PER_SOCKET; CddNum++) {
          if ((DomainInfo[i].Intlv.None.CddMap & (BIT0 << CddNum)) != 0) {
            MsctDomains[i].MaxProcCap += (UINT32)NumberOfThreadsOnCdd[DomainInfo[i].Intlv.None.Socket][CddNum];
		  }
        }
      }
    } else {
      //CXL NUMA do not contain processors
      MsctDomains[i].MaxProcCap = 0;
    }
  }

  // Obtain memory affinity information for each domain
  CalledStatus = FabricSratServices->GetMemoryInfo (FabricSratServices, &MemoryInfoCtr, &MemoryInfo);
  if (CalledStatus == EFI_SUCCESS) {
    // Update MaxMemCap field
    for (i = 0; i < MemoryInfoCtr; i++) {
      CurrDomain = MemoryInfo[i].Domain;

      for (j = 0; j < TotalDomainsInSystem; j++) {
        if (CurrDomain == MsctDomains[j].ProxDomain) {
          MsctDomains[j].MaxMemCap += MemoryInfo[i].RegionSize;
          break;
        }
      }
    }
  }

  LibHygonMemCopy(MsctInfo, &MsctDomains[0], sizeof(MsctDomains), NULL);
  *NumPopProxDomains = TotalDomainsInSystem;
  *MaxNumProxDomains = MaxProxDomains;

  return EFI_SUCCESS;
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  GetNumberOfThreadsOnCdd
 *
 *  Description:
 *    Returns the number of enabled thread on a die
 *
 *  Parameters:
 *    @param[in]      Socket                     Socket to read data from
 *    @param[in]      Die                        Die to read data from
 *    @param[out]     NumberOfThreadsOnDie       Number of enabled threads on the specified socket and die

 *
 *    @retval         HGPI_ERROR                Services protocol not available
 *                    HGPI_SUCCESS              Data successfully obtained
 *
 *---------------------------------------------------------------------------------------
 */
HGPI_STATUS
GetNumberOfThreadsOnCdd (
  IN       UINTN  Socket,
  IN       UINTN  Cdd,
  OUT      UINTN  *NumberOfThreadsOnCdd
  )
{
  UINTN                                  NumberOfComplexes;
  UINTN                                  NumberOfCores;
  UINTN                                  NumberOfThreads;
  HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL  *CoreTopology;

  if (gBS->LocateProtocol (&gHygonCoreTopologyServicesProtocolGuid, NULL, (VOID **)&CoreTopology) != EFI_SUCCESS) {
    return HGPI_ERROR;
  }

  if (CoreTopology->GetCoreTopologyOnCdd (CoreTopology, Socket, Cdd, &NumberOfComplexes, &NumberOfCores, &NumberOfThreads) != EFI_SUCCESS) {
    return HGPI_ERROR;
  }

  *NumberOfThreadsOnCdd = NumberOfComplexes * (NumberOfCores * NumberOfThreads);

  return HGPI_SUCCESS;
}
