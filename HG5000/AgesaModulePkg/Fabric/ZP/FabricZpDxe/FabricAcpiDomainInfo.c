/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric ACPI Domain infor for SRAT & CDIT.
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
#include "Porting.h"
#include "AMD.h"
#include "FabricAcpiDomainInfo.h"
#include "FabricAcpiTable.h"
#include <Library/BaseLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersZP.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/FabricNumaServicesProtocol.h>
#include <Protocol/FabricTopologyServices.h>
#include "Filecode.h"


#define FILECODE FABRIC_ZP_FABRICZPDXE_FABRICACPIDOMAININFO_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
BuildDomainInfo (
  VOID
  );

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

BOOLEAN      mDomainInfoValid = FALSE;
UINT32       mNumberOfDomains = 0;
DOMAIN_INFO  mDomainInfo[MAX_SOCKETS * MAX_DIES_PER_SOCKET];
DIE_INFO     mDieInfo[MAX_SOCKETS][MAX_DIES_PER_SOCKET];

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
FabricZpGetDomainInfo (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
     OUT   UINT32                         *NumberOfDomainsInSystem,
     OUT   DOMAIN_INFO                   **DomainInfo
  );

EFI_STATUS
EFIAPI
FabricZpDomainXlat (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
  IN       UINTN                           Socket,
  IN       UINTN                           Die,
     OUT   UINT32                         *Domain
  );

UINT8
EFIAPI
FabricZpGetMaxDomains (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This
  );

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
STATIC FABRIC_NUMA_SERVICES_PROTOCOL  mFabricNumaServicesProtocol = {
  0x1,
  FabricZpGetDomainInfo,
  FabricZpDomainXlat,
  FabricZpGetMaxDomains
};

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
FabricZpNumaServicesProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  BuildDomainInfo ();
  return (gBS->InstallProtocolInterface (
                &ImageHandle,
                &gAmdFabricNumaServicesProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mFabricNumaServicesProtocol
                ));
}

EFI_STATUS
EFIAPI
FabricZpGetDomainInfo (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
     OUT   UINT32                         *NumberOfDomainsInSystem,
     OUT   DOMAIN_INFO                   **DomainInfo
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
FabricZpDomainXlat (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This,
  IN       UINTN                           Socket,
  IN       UINTN                           Die,
     OUT   UINT32                         *Domain
  )
{
  ASSERT (mDomainInfoValid);

  if (Socket >= FabricTopologyGetNumberOfProcessorsPresent ()) {
    return EFI_INVALID_PARAMETER;
  }

  if (Die >= FabricTopologyGetNumberOfDiesOnSocket (Socket)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Domain == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Domain = mDieInfo[Socket][Die].Domain;
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
UINT8
EFIAPI
FabricZpGetMaxDomains (
  IN       FABRIC_NUMA_SERVICES_PROTOCOL  *This
  )
{
  return (PcdGet8 (PcdAmdNumberOfPhysicalSocket) * (UINT8) FabricTopologyGetNumberOfDiesOnSocket (0));
}

VOID
BuildDomainInfo (
  VOID
  )
{
  UINTN                        NumberOfSockets;
  UINTN                        NumberOfDiesOnSocket;
  UINT32                       i;
  UINT32                       j;
  UINT32                       k;
  UINT32                       MapPairIndex;
  UINT32                       DiesAccountedForInDomain[(MAX_SOCKETS * MAX_DIES_PER_SOCKET)];
  UINT32                       TotalEntities[(MAX_SOCKETS * MAX_DIES_PER_SOCKET)];
  DRAM_INFO                    DramInfo[NUMBER_OF_DRAM_REGIONS];
  DRAM_BASE_ADDRESS_REGISTER   DramBaseAddr;
  DRAM_LIMIT_ADDRESS_REGISTER  DramLimitAddr;

  ASSERT (!mDomainInfoValid);
  ASSERT (mNumberOfDomains == 0);

  // Collect raw data
  for (i = 0; i < NUMBER_OF_DRAM_REGIONS; i++) {
    DramBaseAddr.Value = FabricRegisterAccRead (0, 0, DRAMBASEADDR0_FUNC, (DRAMBASEADDR0_REG + (i * DRAM_REGION_REGISTER_OFFSET)), ZP_IOMS0_INSTANCE_ID);
    if (DramBaseAddr.Field.AddrRngVal == 1) {
      DramLimitAddr.Value = FabricRegisterAccRead (0, 0, DRAMLIMITADDR0_FUNC, (DRAMLIMITADDR0_REG + (i * DRAM_REGION_REGISTER_OFFSET)), ZP_IOMS0_INSTANCE_ID);
      DramInfo[i].Socket = ((DramLimitAddr.Field.DstFabricID >> FABRIC_ID_SOCKET_SHIFT) & FABRIC_ID_SOCKET_SIZE_MASK);
      DramInfo[i].Die = ((DramLimitAddr.Field.DstFabricID >> FABRIC_ID_DIE_SHIFT) & FABRIC_ID_DIE_SIZE_MASK);
      DramInfo[i].SocketMask = 0xFFFFFFFF << DramLimitAddr.Field.IntLvNumSockets;
      DramInfo[i].DieMask = 0xFFFFFFFF << DramLimitAddr.Field.IntLvNumDies;
      DramInfo[i].SocketCount = 1 << DramLimitAddr.Field.IntLvNumSockets;
      DramInfo[i].DieCount = 1 << DramLimitAddr.Field.IntLvNumDies;
    } else {
      DramInfo[i].SocketCount = 0;
      DramInfo[i].DieCount = 0;
      DramInfo[i].Socket = 0xFF;
      DramInfo[i].Die = 0xFF;
      DramInfo[i].SocketMask = 0;
      DramInfo[i].DieMask = 0;
    }
  }

  // Initialize domain info to none
  for (i = 0; i < (MAX_SOCKETS * MAX_DIES_PER_SOCKET); i++) {
    DiesAccountedForInDomain[i] = 0;
    TotalEntities[i] = 0;
    mDomainInfo[i].Type = MaxDomainType;
  }

  // Initialize die info to none
  for (i = 0; i < MAX_SOCKETS; i++) {
    for (j = 0; j < MAX_DIES_PER_SOCKET; j++) {
      mDieInfo[i][j].Domain = 0xFF;
      for (k = 0; k < MAX_CHANNELS_PER_DIE; k++) {
        mDieInfo[i][j].DramMapPair[k] = 0xFF;
      }
    }
  }

  // Build die and domain info
  NumberOfSockets = FabricTopologyGetNumberOfProcessorsPresent ();
  ASSERT (NumberOfSockets <= MAX_SOCKETS);
  for (i = 0; i < NumberOfSockets; i++) {
    NumberOfDiesOnSocket = FabricTopologyGetNumberOfDiesOnSocket (i);
    ASSERT (NumberOfDiesOnSocket <= MAX_DIES_PER_SOCKET);
    for (j = 0; j < NumberOfDiesOnSocket; j++) {
      mDieInfo[i][j].Domain = mNumberOfDomains;
      DiesAccountedForInDomain[mNumberOfDomains]++;
      MapPairIndex = 0;
      for (k = 0; k < NUMBER_OF_DRAM_REGIONS; k++) {
        if ((DramInfo[k].Socket == (i & DramInfo[k].SocketMask)) && (DramInfo[k].Die == (j & DramInfo[k].DieMask))) {
          ASSERT (MapPairIndex < MAX_CHANNELS_PER_DIE);
          mDieInfo[i][j].DramMapPair[MapPairIndex++] = k;
        }
      }
      if (MapPairIndex != 1)  {
        mDomainInfo[mNumberOfDomains].Type = NoIntlv;
        mDomainInfo[mNumberOfDomains].Intlv.None.Socket = i;
        mDomainInfo[mNumberOfDomains].Intlv.None.Die = j;
        TotalEntities[mNumberOfDomains] = 1;
      } else if (DiesAccountedForInDomain[mNumberOfDomains] == 1) {
        if (DramInfo[mDieInfo[i][j].DramMapPair[0]].SocketCount > 1) {
          mDomainInfo[mNumberOfDomains].Type = SocketIntlv;
          mDomainInfo[mNumberOfDomains].Intlv.Socket.SocketCount = DramInfo[mDieInfo[i][j].DramMapPair[0]].SocketCount;
          TotalEntities[mNumberOfDomains] = mDomainInfo[mNumberOfDomains].Intlv.Socket.SocketCount * DramInfo[mDieInfo[i][j].DramMapPair[0]].DieCount;
          mDomainInfo[mNumberOfDomains].Intlv.Socket.SocketMap |= (1 << i);
        } else if (DramInfo[mDieInfo[i][j].DramMapPair[0]].DieCount > 1) {
          mDomainInfo[mNumberOfDomains].Type = DieIntlv;
          mDomainInfo[mNumberOfDomains].Intlv.Die.DieCount = DramInfo[mDieInfo[i][j].DramMapPair[0]].DieCount;
          TotalEntities[mNumberOfDomains] = mDomainInfo[mNumberOfDomains].Intlv.Die.DieCount;
          mDomainInfo[mNumberOfDomains].Intlv.Die.DieMap |= (1 << j);
        } else {
          mDomainInfo[mNumberOfDomains].Type = NoIntlv;
          TotalEntities[mNumberOfDomains] = 1;
          mDomainInfo[mNumberOfDomains].Intlv.None.Socket = i;
          mDomainInfo[mNumberOfDomains].Intlv.None.Die = j;
        }
      }
      if (DiesAccountedForInDomain[mNumberOfDomains] == TotalEntities[mNumberOfDomains]) {
        mNumberOfDomains++;
      }
    }
  }

  mDomainInfoValid = TRUE;
}

