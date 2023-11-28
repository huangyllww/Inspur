/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric ACPI MSCT.
 *
 * This function implements routines for MSCT creation.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ****************************************************************************
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

#include "AGESA.h"
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/FabricTopologyServices.h>
#include <Protocol/AmdCoreTopologyProtocol.h>
#include <Protocol/FabricNumaServicesProtocol.h>
#include <Protocol/AmdAcpiSratServicesProtocol.h>
#include <Protocol/AmdAcpiMsctServicesProtocol.h>
#include "Filecode.h"

#define FILECODE FABRIC_ZP_FABRICZPDXE_FABRICACPIMSCT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define MAX_PROXIMITY_DOMAINS 8
/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

EFI_STATUS
EFIAPI
FabricGetMsctInfo (
  IN       AMD_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL *This,
     OUT   UINT32                                 *NumPopProxDomains,
     OUT   UINT32                                 *MaxNumProxDomains,
     OUT   MSCT_PROX_DOMAIN_INFO                  **MsctInfo
  );

AGESA_STATUS
GetNumberOfThreadsOnDie (
  IN       UINTN  Socket,
  IN       UINTN  Die,
     OUT   UINTN  *NumberOfThreadsOnDie
  );

STATIC AMD_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL mFabricAcpiMsctServicesProtocol = {
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
FabricZpAcpiMsctProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return (gBS->InstallProtocolInterface (
               &ImageHandle,
               &gAmdFabricAcpiMsctServicesProtocolGuid,
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
 *    @param[in]      This                       Pointer to AMD_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL instance
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
  IN       AMD_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL *This,
     OUT   UINT32                                 *NumPopProxDomains,
     OUT   UINT32                                 *MaxNumProxDomains,
     OUT   MSCT_PROX_DOMAIN_INFO                  **MsctInfo
  )
{

  UINT8                         MaxProxDomains;
  UINTN                         NumberOfDiesOnSocket;
  UINTN                         NumberOfSockets;
  UINTN                         NumberOfThreadsOnDie;
  UINT32                        NumberOfThreadsOnSocket;
  UINT32                        NumberOfThreadsOnSystem;
  UINT32                        i;
  UINT32                        j;
  UINT32                        CurrDomain;
  UINT32                        NumberOfDomainsInSystem;
  UINT32                        MemoryInfoCtr;
  EFI_STATUS                    CalledStatus;
  DOMAIN_TYPE                   CurrDomainType;
  DOMAIN_INFO                   *DomainInfo;
  MEMORY_INFO                   *MemoryInfo;
  MSCT_PROX_DOMAIN_INFO         MsctDomains[MAX_PROXIMITY_DOMAINS];
  FABRIC_NUMA_SERVICES_PROTOCOL *FabricNumaServices;
  AMD_FABRIC_ACPI_SRAT_SERVICES_PROTOCOL *FabricSratServices;
  AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  AMD_CORE_TOPOLOGY_SERVICES_PROTOCOL    *CoreTopology;


  if (gBS->LocateProtocol (&gAmdFabricNumaServicesProtocolGuid, NULL, (VOID **) &FabricNumaServices) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  if (gBS->LocateProtocol (&gAmdFabricAcpiSratServicesProtocolGuid, NULL, (VOID **) &FabricSratServices) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  if (gBS->LocateProtocol (&gAmdFabricTopologyServicesProtocolGuid, NULL, (VOID **) &FabricTopology) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  if (gBS->LocateProtocol (&gAmdCoreTopologyServicesProtocolGuid, NULL, (VOID **) &CoreTopology) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  MaxProxDomains = FabricNumaServices->GetMaxDomains (FabricNumaServices);

  NumberOfThreadsOnSystem = 0;

  ASSERT (MaxProxDomains <= MAX_PROXIMITY_DOMAINS);

  if (FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  if (FabricTopology->GetProcessorInfo (FabricTopology, 0, &NumberOfDiesOnSocket, NULL, NULL, NULL) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  if (GetNumberOfThreadsOnDie (0, 0, &NumberOfThreadsOnDie) == AGESA_ERROR) {
    return EFI_ABORTED;
  }

  NumberOfThreadsOnSocket = (UINT32) (NumberOfDiesOnSocket * NumberOfThreadsOnDie);
  NumberOfThreadsOnSystem = ((UINT32) NumberOfSockets * NumberOfThreadsOnSocket);

  // Initialize MsctDomains
  for (i = 0; i < MaxProxDomains; i++) {
    MsctDomains[i].ProxDomain = 0;
    MsctDomains[i].MaxProcCap = 0;
    MsctDomains[i].MaxMemCap = 0;
  }

  FabricNumaServices->GetDomainInfo (FabricNumaServices, &NumberOfDomainsInSystem, &DomainInfo);
  for (i = 0; i < NumberOfDomainsInSystem ; i++) {
    CurrDomainType = DomainInfo[i].Type;
    ASSERT (CurrDomainType < MaxDomainType);

    MsctDomains[i].ProxDomain = i;

    if (CurrDomainType == SocketIntlv) {
      // If a domain is socket interleaved, all the threads on the system are on the same domain.
      // Use CoreTopology to get the total threads in the system.
      MsctDomains[i].MaxProcCap = NumberOfThreadsOnSystem;
    } else if (CurrDomainType == DieIntlv) {
      // If a domain is die interleaved, all the threads on the socket are on the same domain.
      MsctDomains[i].MaxProcCap = NumberOfThreadsOnSocket;
    } else if (CurrDomainType == NoIntlv) {
      // If a domain is not interleaved, only the threads on each die are on the same domain.
      MsctDomains[i].MaxProcCap = (UINT32) NumberOfThreadsOnDie;
    }
  }

  // Obtain memory affinity information for each domain
  CalledStatus = FabricSratServices->GetMemoryInfo (FabricSratServices, &MemoryInfoCtr, &MemoryInfo);
  if (CalledStatus == EFI_SUCCESS) {
    // Update MaxMemCap field
    for (i = 0; i < MemoryInfoCtr; i++) {
      CurrDomain = MemoryInfo[i].Domain;

      for (j = 0; j < NumberOfDomainsInSystem; j++) {
        if (CurrDomain == MsctDomains[j].ProxDomain) {
          MsctDomains[j].MaxMemCap += MemoryInfo[i].RegionSize;
          break;
        }
      }
    }
  }

  *MsctInfo = &MsctDomains[0];
  *NumPopProxDomains = NumberOfDomainsInSystem;
  *MaxNumProxDomains = MaxProxDomains;

  return EFI_SUCCESS;
}


/**
 *---------------------------------------------------------------------------------------
 *
 *  GetNumberOfThreadsOnDie
 *
 *  Description:
 *    Returns the number of enabled thread on a die
 *
 *  Parameters:
 *    @param[in]      Socket                     Socket to read data from
 *    @param[in]      Die                        Die to read data from
 *    @param[out]     NumberOfThreadsOnDie       Number of enabled threads on the specified socket and die

 *
 *    @retval         AGESA_ERROR                Services protocol not available
 *                    AGESA_SUCCESS              Data successfully obtained
 *
 *---------------------------------------------------------------------------------------
 */
AGESA_STATUS
GetNumberOfThreadsOnDie (
  IN       UINTN  Socket,
  IN       UINTN  Die,
     OUT   UINTN  *NumberOfThreadsOnDie
  )
{
  UINTN                                  NumberOfComplexes;
  UINTN                                  NumberOfCores;
  UINTN                                  NumberOfThreads;
  AMD_CORE_TOPOLOGY_SERVICES_PROTOCOL    *CoreTopology;

  if (gBS->LocateProtocol (&gAmdCoreTopologyServicesProtocolGuid, NULL, (VOID **) &CoreTopology) != EFI_SUCCESS) {
    return AGESA_ERROR;
  }

  if (CoreTopology->GetCoreTopologyOnDie (CoreTopology, Socket, Die, &NumberOfComplexes, &NumberOfCores, &NumberOfThreads ) != EFI_SUCCESS) {
    return AGESA_ERROR;
  }

  *NumberOfThreadsOnDie = NumberOfComplexes * (NumberOfCores * NumberOfThreads);


  return AGESA_SUCCESS;
}