/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric MMIO initialization base on NV variable for ZEN ZP
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
 *
 * Copyright 2008 - 2018 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software and documentation (if
 * any) (collectively, the "Materials") pursuant to the terms and conditions of
 * the Software License Agreement included with the Materials.  If you do not
 * have a copy of the Software License Agreement, contact your AMD
 * representative for a copy.
 *
 * You agree that you will not reverse engineer or decompile the Materials, in
 * whole or in part, except as allowed by applicable law.
 *
 * WARRANTY DISCLAIMER:  THE MATERIALS ARE PROVIDED "AS IS" WITHOUT WARRANTY OF
 * ANY KIND.  AMD DISCLAIMS ALL WARRANTIES, EXPRESS, IMPLIED, OR STATUTORY,
 * INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE, NON-INFRINGEMENT, THAT THE
 * MATERIALS WILL RUN UNINTERRUPTED OR ERROR-FREE OR WARRANTIES ARISING FROM
 * CUSTOM OF TRADE OR COURSE OF USAGE.  THE ENTIRE RISK ASSOCIATED WITH THE USE
 * OF THE MATERIAL IS ASSUMED BY YOU.  Some jurisdictions do not allow the
 * exclusion of implied warranties, so the above exclusion may not apply to
 * You.
 *
 * LIMITATION OF LIABILITY AND INDEMNIFICATION:  AMD AND ITS LICENSORS WILL
 * NOT, UNDER ANY CIRCUMSTANCES BE LIABLE TO YOU FOR ANY PUNITIVE, DIRECT,
 * INCIDENTAL, INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES ARISING FROM USE OF
 * THE MATERIALS OR THIS AGREEMENT EVEN IF AMD AND ITS LICENSORS HAVE BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.  In no event shall AMD's total
 * liability to You for all damages, losses, and causes of action (whether in
 * contract, tort (including negligence) or otherwise) exceed the amount of
 * $100 USD. You agree to defend, indemnify and hold harmless AMD and its
 * licensors, and any of their directors, officers, employees, affiliates or
 * agents from and against any and all loss, damage, liability and other
 * expenses (including reasonable attorneys' fees), resulting from Your use of
 * the Materials or violation of the terms and conditions of this Agreement.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS:  The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgment of AMD's proprietary rights in them.
 *
 * EXPORT RESTRICTIONS: The Materials may be subject to export restrictions as
 * stated in the Software License Agreement.
 *******************************************************************************
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Porting.h"
#include "AMD.h"
#include "Filecode.h"
#include <FabricRegistersZP.h>
#include <Library/BaseLib.h>
#include <Library/AmdBaseLib.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Library/FabricResourceSizeForEachDieLib.h>
#include "FabricResourceManagerZenZpLib.h"

#define FILECODE LIBRARY_FABRICRESOURCEMANAGERZENZPLIB_FABRICRESOURCEINITBASEONNVZENZP_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
UINT32 mOverSizeBelowPcieMin = 0xFFFFFFFF;
UINT32 mOverSizeAbovePcieMin = 0xFFFFFFFF;
UINT32 mAlignmentMask = 0;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
BOOLEAN
ArrangeMmioBelow4G (
  IN       FABRIC_RESOURCE_FOR_EACH_DIE  *MmioSizeForEachDie,
  IN       UINT64                         MmioBaseAddrAbovePcieCfg,
  IN       UINT64                         MmioLimitAbovePcieCfg,
  IN       UINT64                         MmioBaseAddrBelowPcieCfg,
  IN       UINT64                         MmioLimitBelowPcieCfg,
  IN OUT   BOOLEAN                       *MmioIsAbovePcieCfg,
  IN       UINT8                          SocketNumber,
  IN       UINT8                          DieNumberPerSocket,
  IN       FABRIC_MMIO_MANAGER           *FabricMmioManager,
  IN       BOOLEAN                        SetDfRegisters
  );

BOOLEAN
GetNextCombination (
  IN       UINT8                          NumberOfDieBelowPcieCfg,
  IN OUT   BOOLEAN                       *MmioIsAbovePcieCfg,
  IN       UINT8                          SocketNumber,
  IN       UINT8                          DieNumberPerSocket
  );

BOOLEAN
TryThisCombination (
  IN       FABRIC_RESOURCE_FOR_EACH_DIE  *MmioSizeForEachDie,
  IN       UINT64                         MmioBaseAddrAbovePcieCfg,
  IN       UINT64                         MmioLimitAbovePcieCfg,
  IN       UINT64                         MmioBaseAddrBelowPcieCfg,
  IN       UINT64                         MmioLimitBelowPcieCfg,
  IN OUT   BOOLEAN                       *MmioIsAbovePcieCfg,
  IN       UINT8                          SocketNumber,
  IN       UINT8                          DieNumberPerSocket,
  IN       FABRIC_MMIO_MANAGER           *FabricMmioManager,
  IN       BOOLEAN                        SetDfRegisters
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/*---------------------------------------------------------------------------------------*/
/**
 * FabricZenZpInitIoBaseOnNvVariable
 *
 * Initialize IO registers for each Die base on NV variable.
 *
 * @param[in, out]    FabricIoManager          Point to FABRIC_IO_MANAGER
 * @param[in]         IoSizeForEachDie         How much IO size is required for each Die
 * @param[in, out]    SpaceStaus               Current status
 * @param[in]         SocketNumber             System socket count
 * @param[in]         DieNumberPerSocket       Die number per socket
 * @param[in]         SetDfRegisters           TRUE  - Set DF MMIO registers
 *                                             FALSE - Not set DF MMIO registers, just calculate if user's requirment could be satisfied.
 *
 */
EFI_STATUS
FabricZenZpInitIoBaseOnNvVariable (
  IN       FABRIC_IO_MANAGER   *FabricIoManager,
  IN       FABRIC_RESOURCE_FOR_EACH_DIE *IoSizeForEachDie,
  IN       FABRIC_ADDR_SPACE_SIZE       *SpaceStatus,
  IN       UINT8                SocketNumber,
  IN       UINT8                DieNumberPerSocket,
  IN       BOOLEAN              SetDfRegisters
  )
{
  UINT8             i;
  UINT8             j;
  UINT8             RegIndex;
  UINT8             DstFabricIDSysOffset;
  UINT32            IoBase;
  UINT32            IoSize;
  UINT32            LegacyIoSize;

  IoBase = 0;
  RegIndex = 0;
  LegacyIoSize = X86_LEGACY_IO_SIZE;
  IDS_HDT_CONSOLE (CPU_TRACE, "    reserve 0x%X IO size for legacy devices\n", LegacyIoSize);

  for (i = 0; i < SocketNumber; i++) {
    for (j = 0; j < DieNumberPerSocket; j++) {
      if (SpaceStatus != NULL) {
        SpaceStatus->IoSize += (UINT32) IoSizeForEachDie->IO[i][j].Size;
      }
      
      IDS_HDT_CONSOLE (CPU_TRACE, "    Socket %d die %d request IO size = 0x%X \n", i, j, IoSizeForEachDie->IO[i][j].Size);

      DstFabricIDSysOffset = (UINT8) FabricTopologyGetDieSystemOffset (i, j);
      if ((i == (SocketNumber - 1)) && (j == (DieNumberPerSocket - 1))) {
        // 1. first, check if it's the last Die. Set IO limit up to 0x1FFFFFF for last Die.
        IoSize = X86IO_LIMIT - IoBase;
      } else if ((i == 0) && (j == 0)) {
        // 2. second, if it's Die 0 and it's not the last Die, plus size with LegacyIoSize
        IoSize = (UINT32) IoSizeForEachDie->IO[i][j].Size + LegacyIoSize;
      } else {
        IoSize = (UINT32) IoSizeForEachDie->IO[i][j].Size;
      }

      if (IoSize == 0) {
        continue;
      }

      if ((FabricIoManager != NULL) && SetDfRegisters) {
        FabricZenZpSetIoReg (SocketNumber, DieNumberPerSocket, RegIndex, DstFabricIDSysOffset, IoBase, IoSize);

        if ((i == 0) && (j == 0)) {
          FabricIoManager->IoRegion[i][j].IoBase = IoBase + LegacyIoSize;
          FabricIoManager->IoRegion[i][j].IoSize = IoSize - LegacyIoSize;
        } else {
          FabricIoManager->IoRegion[i][j].IoBase = IoBase;
          FabricIoManager->IoRegion[i][j].IoSize = IoSize;
        }
        FabricIoManager->IoRegion[i][j].IoUsed = 0;
        
        IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x Die%x has IO base 0x%X size 0x%X\n", i, j, FabricIoManager->IoRegion[i][j].IoBase, FabricIoManager->IoRegion[i][j].IoSize);
      }
      
      IoBase += IoSize;
      RegIndex++;
    }
  }

  if (SpaceStatus != NULL) {
    if (SpaceStatus->IoSize > X86IO_LIMIT) {
      SpaceStatus->IoSizeReqInc = SpaceStatus->IoSize - X86IO_LIMIT;
    } else {
      SpaceStatus->IoSizeReqInc = 0;
    }
    IDS_HDT_CONSOLE (CPU_TRACE, "  Space Status: IoSize %X, IoSizeReqInc %X\n", SpaceStatus->IoSize, SpaceStatus->IoSize);
  }

  FabricIoManager->GlobalCtrl = TRUE;

  ASSERT (IoBase <= X86IO_LIMIT);
  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricZenZpInitMmioBaseOnNvVariable
 *
 * Initialize MMIO registers for each Die base on NV variable.
 *
 * @param[in, out]    FabricMmioManager        Point to FABRIC_MMIO_MANAGER
 * @param[in]         MmioSizeForEachDie       How much MMIO size is required for each Die
 * @param[in, out]    SpaceStaus               Current status
 * @param[in]         SocketNumber             System socket count
 * @param[in]         DieNumberPerSocket       Die number per socket
 * @param[in]         SetDfRegisters           TRUE  - Set DF MMIO registers
 *                                             FALSE - Not set DF MMIO registers, just calculate if user's requirment could be satisfied.
 *
 */
EFI_STATUS
FabricZenZpInitMmioBaseOnNvVariable (
  IN       FABRIC_MMIO_MANAGER *FabricMmioManager,
  IN       FABRIC_RESOURCE_FOR_EACH_DIE *MmioSizeForEachDie,
  IN       FABRIC_ADDR_SPACE_SIZE       *SpaceStatus,
  IN       UINT8                SocketNumber,
  IN       UINT8                DieNumberPerSocket,
  IN       BOOLEAN              SetDfRegisters
  )
{
  UINT8  i;
  UINT8  j;
  UINT8  DstFabricIDSysOffset;
  UINT64 MmioLimitAbove4G;
  UINT64 MmioBaseAddr;
  UINT64 MmioBaseAddrNextDie;
  UINT64 MmioBaseAddrPrefetchable;
  UINT64 MmioBaseAddrNonPrefetchable;
  UINT64 SizePrefetchable;
  UINT64 SizeNonPrefetchable;
  UINT64 MmioBaseAddrAbovePcieCfg;
  UINT64 MmioBaseAddrBelowPcieCfg;
  UINT64 MmioLimitAbovePcieCfg;
  UINT64 MmioLimitBelowPcieCfg;
  UINT64 TOM;
  UINT64 TOM2;
  UINT64 PciCfgSpace;
  UINT64 AlignMask;
  UINT64 AlignMaskP;
  UINT64 MmioSizeBelowHole;
  UINT64 MmioSizeAboveHole;
  EFI_STATUS Status;
  BOOLEAN MmioIsAbovePcieCfg[MAX_SOCKETS_SUPPORTED * MAX_DIES_PER_SOCKET];
  BOOLEAN EnoughSpaceAbove4G;
  BOOLEAN LargeAlignFirst;
  BOOLEAN LastCombinationWork;

  Status = EFI_SUCCESS;

  // If MmioSizeForEachDie is not NULL, we should record current space status
  // Init global variable
  if (SpaceStatus != NULL) {
    mOverSizeBelowPcieMin = 0xFFFFFFFF;
    mOverSizeAbovePcieMin = 0xFFFFFFFF;
    mAlignmentMask = 0;
  }

  // System information
  PciCfgSpace = (AsmReadMsr64 (0xC0010058) >> 2) & 0xF;                   // Get bus range from MSR_C001_0058[5:2][BusRange]
  PciCfgSpace = MultU64x64 (((UINT64) 1 << PciCfgSpace), (1024 * 1024));  // The size of configuration space is 1MB times the number of buses
  TOM = AsmReadMsr64 (0xC001001A);
  TOM2 = AsmReadMsr64 (0xC001001D);
  IDS_HDT_CONSOLE (CPU_TRACE, "  TOM: %lX, TOM2: %lX, \n  Pcie configuration space: %lX ~ %lX\n", TOM, TOM2, PcdGet64 (PcdPciExpressBaseAddress), (PcdGet64 (PcdPciExpressBaseAddress) + PciCfgSpace));

  ASSERT (PcdGet32 (PcdAmdBottomMmioReservedForDie0) >= (PcdGet64 (PcdPciExpressBaseAddress) + PciCfgSpace));
  ASSERT (PcdGet64 (PcdPciExpressBaseAddress) >= TOM);

  // Calculate size of above 4G
  MmioBaseAddrNextDie = (TOM2 > 0x100000000)? TOM2 : 0x100000000; // Check if TOM2 > 4G
  MmioLimitAbove4G = 0x7FD00000000; // SMEE wold be enabled as default, so set limit to 0x7FD00000000

  if (PcdGet64 (PcdAmdMmioAbove4GLimit) < MmioLimitAbove4G) {
    MmioLimitAbove4G = (PcdGet64 (PcdAmdMmioAbove4GLimit) + 1) & 0xFFFFFFFFFFFF0000;
    if (MmioLimitAbove4G <= MmioBaseAddrNextDie) {
      MmioLimitAbove4G = MmioBaseAddrNextDie;
    }
  }

  // there's a hole at 0xFD_0000_0000 ~ 0x100_0000_0000
  MmioSizeBelowHole = 0;
  MmioSizeAboveHole = 0;

  // calculate Mmio size below/above this hole
  if (MmioBaseAddrNextDie < 0xFD00000000) {
    MmioSizeBelowHole = (MmioLimitAbove4G < 0xFD00000000)? MmioLimitAbove4G : 0xFD00000000;
    MmioSizeBelowHole = MmioSizeBelowHole - MmioBaseAddrNextDie;    
  }

  if (MmioLimitAbove4G > 0x10000000000) {
    MmioSizeAboveHole = (MmioBaseAddrNextDie > 0x10000000000)? MmioBaseAddrNextDie : 0x10000000000;
    MmioSizeAboveHole = MmioLimitAbove4G - MmioSizeAboveHole;    
  }

  if ((MmioSizeBelowHole == 0) && (MmioSizeAboveHole == 0)) {
    MmioLimitAbove4G = 0; // set limit to 0, so there's no MMIO space above 4G
    IDS_HDT_CONSOLE (CPU_TRACE, "    WARNING: There's no MMIO space above 4G\n");
  } else {
    if (MmioSizeAboveHole > MmioSizeBelowHole) {
      MmioBaseAddrNextDie = (TOM2 > 0x10000000000)? TOM2 : 0x10000000000; // Check if TOM2 > 1T
    } else {
      MmioLimitAbove4G = (MmioLimitAbove4G < 0xFD00000000)? MmioLimitAbove4G : 0xFD00000000;
    }
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "    Above 4G MMIO base is %lX, limit is %lX\n", MmioBaseAddrNextDie, MmioLimitAbove4G);

  LargeAlignFirst = TRUE;
  EnoughSpaceAbove4G = TRUE;
  for (i = 0; i < SocketNumber; i++) {
    for (j = 0; j < DieNumberPerSocket; j++) {
      // Calculate reqiured size, it's for output parameter 'SpaceStatus'
      if (SpaceStatus != NULL) {
        SpaceStatus->MmioSizeAbove4G += MmioSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Size + MmioSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Size;
        SpaceStatus->MmioSizeBelow4G += (UINT32) (MmioSizeForEachDie->PrefetchableMmioSizeBelow4G[i][j].Size + MmioSizeForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Size);
      }

      // If there's no MMIO request for above 4G, try next one
      if ((MmioSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Size + MmioSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Size) == 0) {
        continue;
      }

      AlignMask  = MmioSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Alignment;
      AlignMaskP = MmioSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Alignment;
      MmioBaseAddr = MmioBaseAddrNextDie;
      if ((LargeAlignFirst && (MmioSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Alignment >= MmioSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Alignment)) ||
          ((!LargeAlignFirst) && (MmioSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Alignment <= MmioSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Alignment))) {
        // Prefetchable -> Non Prefetchable
        MmioBaseAddrPrefetchable = (MmioBaseAddr + AlignMaskP) & (~AlignMaskP);
        MmioBaseAddrNonPrefetchable = (MmioBaseAddrPrefetchable + MmioSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Size + AlignMask) & (~AlignMask);
        SizePrefetchable = MmioBaseAddrNonPrefetchable - MmioBaseAddrPrefetchable;
        SizeNonPrefetchable = MmioSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Size;
        MmioBaseAddrNextDie = MmioBaseAddrNonPrefetchable + SizeNonPrefetchable;
      } else {
        // Non Prefetchable -> Prefetchable
        MmioBaseAddrNonPrefetchable = (MmioBaseAddr + AlignMask) & (~AlignMask);
        MmioBaseAddrPrefetchable = (MmioBaseAddrNonPrefetchable + MmioSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Size + AlignMaskP) & (~AlignMaskP);
        SizeNonPrefetchable = MmioBaseAddrPrefetchable - MmioBaseAddrNonPrefetchable;
        SizePrefetchable = MmioSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Size;
        MmioBaseAddrNextDie = MmioBaseAddrPrefetchable + SizePrefetchable;
      }
      LargeAlignFirst = !LargeAlignFirst;

      if (SizeNonPrefetchable == 0) {
        MmioBaseAddrNonPrefetchable = 0;
      }
      if (SizePrefetchable == 0) {
        MmioBaseAddrPrefetchable = 0;
      }

      // Check if space is enough
      if (MmioBaseAddrNextDie > MmioLimitAbove4G) {
        EnoughSpaceAbove4G = FALSE;
        IDS_HDT_CONSOLE (CPU_TRACE, "  No enough size above 4G\n");
        Status = EFI_OUT_OF_RESOURCES;
      }

      if (SetDfRegisters && EnoughSpaceAbove4G && (FabricMmioManager != NULL)) {
        //   Set MMIO above 4G
        if ((MmioSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Size == 0) &&
            (MmioSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Size == 0)) {
          continue;
        }

        IDS_HDT_CONSOLE (CPU_TRACE, "  User Request above 4G: Socket%x Die%x Prefetch MMIO Size %lX, AlignBit %X\n", i, j, MmioSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Size, MmioSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Alignment);
        IDS_HDT_CONSOLE (CPU_TRACE, "                           Non Prefetch MMIO Size %lX, AlignBit %X\n", MmioSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Size, MmioSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Alignment);
        DstFabricIDSysOffset = (UINT8) FabricTopologyGetDieSystemOffset (i, j);
        if ((i == (SocketNumber - 1)) && (j == (DieNumberPerSocket - 1))) {
          // workaround to set last DIE's MMIO limit to 0xFFFF_FFFF_FFFF
          FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((i * MAX_DIES_PER_SOCKET + j) * 2 + 1), DstFabricIDSysOffset, MmioBaseAddr, (0x1000000000000 - MmioBaseAddr));
        } else {
          FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((i * MAX_DIES_PER_SOCKET + j) * 2 + 1), DstFabricIDSysOffset, MmioBaseAddr, (MmioBaseAddrNextDie - MmioBaseAddr));
        }
        FabricMmioManager->AllocateMmioAbove4GOnThisDie[i][j] = TRUE;
        FabricMmioManager->MmioRegionAbove4G[i][j].BaseNonPci = 0;  // don't have Non Pci MMIO for above 4G
        FabricMmioManager->MmioRegionAbove4G[i][j].BasePrefetch = MmioBaseAddrPrefetchable;
        FabricMmioManager->MmioRegionAbove4G[i][j].BaseNonPrefetch = MmioBaseAddrNonPrefetchable;
        FabricMmioManager->MmioRegionAbove4G[i][j].SizeNonPci = 0;
        FabricMmioManager->MmioRegionAbove4G[i][j].SizePrefetch = SizePrefetchable;
        FabricMmioManager->MmioRegionAbove4G[i][j].SizeNonPrefetch = SizeNonPrefetchable;
        FabricMmioManager->MmioRegionAbove4G[i][j].UsedSizeNonPci = 0;
        FabricMmioManager->MmioRegionAbove4G[i][j].UsedSizePrefetch = 0;
        FabricMmioManager->MmioRegionAbove4G[i][j].UsedSizeNonPrefetch = 0;
        FabricMmioManager->MmioRegionAbove4G[i][j].AlignNonPrefetch = MmioSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Alignment;
        FabricMmioManager->MmioRegionAbove4G[i][j].AlignPrefetch = MmioSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Alignment;

        IDS_HDT_CONSOLE (CPU_TRACE, "    BasePrefetch    0x%lX, SizePrefetch    0x%lX\n", FabricMmioManager->MmioRegionAbove4G[i][j].BasePrefetch, FabricMmioManager->MmioRegionAbove4G[i][j].SizePrefetch);
        IDS_HDT_CONSOLE (CPU_TRACE, "    BaseNonPrefetch 0x%lX, SizeNonPrefetch 0x%lX\n", FabricMmioManager->MmioRegionAbove4G[i][j].BaseNonPrefetch, FabricMmioManager->MmioRegionAbove4G[i][j].SizeNonPrefetch);
      }
    }
  }

  // Calculate size of below 4G
  MmioBaseAddrAbovePcieCfg = PcdGet64 (PcdPciExpressBaseAddress) + PciCfgSpace;
  MmioBaseAddrBelowPcieCfg = TOM;
  MmioLimitAbovePcieCfg    = PcdGet32 (PcdAmdBottomMmioReservedForDie0);
  MmioLimitBelowPcieCfg    = PcdGet64 (PcdPciExpressBaseAddress);

  LastCombinationWork = FALSE;
  if (FabricGetResourceDistribution (&MmioIsAbovePcieCfg[0]) == EFI_SUCCESS) {
    // Get distribution information from NV, try it first
    if (TryThisCombination (MmioSizeForEachDie, MmioBaseAddrAbovePcieCfg, MmioLimitAbovePcieCfg, MmioBaseAddrBelowPcieCfg, MmioLimitBelowPcieCfg, MmioIsAbovePcieCfg, SocketNumber, DieNumberPerSocket, FabricMmioManager, SetDfRegisters)) {
      // It works! No need to find out a new combination that which Die is above Pcie Cfg
      IDS_HDT_CONSOLE (CPU_TRACE, "  Use combination in NV\n");
      LastCombinationWork = TRUE;
    }
  }

  if (!LastCombinationWork) {
    if (ArrangeMmioBelow4G (MmioSizeForEachDie, MmioBaseAddrAbovePcieCfg, MmioLimitAbovePcieCfg, MmioBaseAddrBelowPcieCfg, MmioLimitBelowPcieCfg, MmioIsAbovePcieCfg, SocketNumber, DieNumberPerSocket, FabricMmioManager, SetDfRegisters)) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  Save combination to NV\n");
      FabricSetResourceDistribution (&MmioIsAbovePcieCfg[0]);
    } else {
      IDS_HDT_CONSOLE (CPU_TRACE, "  No enough size below 4G\n");
      Status = EFI_OUT_OF_RESOURCES;
    }
  }

  if (SpaceStatus != NULL) {
    ASSERT ((mOverSizeBelowPcieMin == 0) || (mOverSizeAbovePcieMin == 0));
    SpaceStatus->MmioSizeAbove4GReqInc = (MmioBaseAddrNextDie > MmioLimitAbove4G) ? (MmioBaseAddrNextDie - MmioLimitAbove4G) : 0;
    SpaceStatus->MmioSizeBelow4GReqInc = mOverSizeBelowPcieMin + mOverSizeAbovePcieMin;
    if (SpaceStatus->MmioSizeBelow4GReqInc != 0) {
      SpaceStatus->MmioSizeBelow4GReqInc = (SpaceStatus->MmioSizeBelow4GReqInc + mAlignmentMask) & (~mAlignmentMask);
    }
    IDS_HDT_CONSOLE (CPU_TRACE, "  Space Status: MmioSizeAbove4G %lX, MmioSizeAbove4GReqInc %lX\n", SpaceStatus->MmioSizeAbove4G, SpaceStatus->MmioSizeAbove4GReqInc);
    IDS_HDT_CONSOLE (CPU_TRACE, "  Space Status: MmioSizeBelow4G %lX, MmioSizeBelow4GReqInc %lX\n", SpaceStatus->MmioSizeBelow4G, SpaceStatus->MmioSizeBelow4GReqInc);
  }

  return Status;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Try to arrange MMIO below 4G
 *
 *
 * @param[in]         MmioSizeForEachDie          Required MMIO size for each Die
 * @param[in]         MmioBaseAddrAbovePcieCfg    MmioBaseAddrAbovePcieCfg
 * @param[in]         MmioLimitAbovePcieCfg       MmioLimitAbovePcieCfg
 * @param[in]         MmioBaseAddrBelowPcieCfg    MmioBaseAddrBelowPcieCfg
 * @param[in]         MmioLimitBelowPcieCfg       MmioLimitBelowPcieCfg
 * @param[in, out]    MmioIsAbovePcieCfg          An BOOLEAN array, indicate which Die's MMIO is above Pcie Cfg
 * @param[in]         SocketNumber                System socket count
 * @param[in]         DieNumberPerSocket          Die number per socket
 * @param[in]         FabricMmioManager           Point to FABRIC_MMIO_MANAGER
 * @param[in]         SetDfRegisters              TRUE  - Set DF MMIO registers
 *                                                FALSE - Not set DF MMIO registers, just calculate if user's requirment could be satisfied.
 *
 * @retval            TRUE
 *                    FALSE
 */
BOOLEAN
ArrangeMmioBelow4G (
  IN       FABRIC_RESOURCE_FOR_EACH_DIE  *MmioSizeForEachDie,
  IN       UINT64                         MmioBaseAddrAbovePcieCfg,
  IN       UINT64                         MmioLimitAbovePcieCfg,
  IN       UINT64                         MmioBaseAddrBelowPcieCfg,
  IN       UINT64                         MmioLimitBelowPcieCfg,
  IN OUT   BOOLEAN                       *MmioIsAbovePcieCfg,
  IN       UINT8                          SocketNumber,
  IN       UINT8                          DieNumberPerSocket,
  IN       FABRIC_MMIO_MANAGER           *FabricMmioManager,
  IN       BOOLEAN                        SetDfRegisters
  )
{
  UINT8   NumberOfDieBelowPcieCfg;
  BOOLEAN GetAnCombination;
  BOOLEAN NextCombination;

  GetAnCombination = FALSE;

  // Try to put 0 Die, 1 Die, 2 Dies... ZEN_MAX_SYSTEM_DIE_COUNT Dies below Pcie Cfg
  for (NumberOfDieBelowPcieCfg = 0; NumberOfDieBelowPcieCfg <= (SocketNumber * DieNumberPerSocket); NumberOfDieBelowPcieCfg++) {
    // 1. Default, all Dies are above Pcie Cfg
    LibAmdMemFill (MmioIsAbovePcieCfg, TRUE, (sizeof (BOOLEAN) * (MAX_SOCKETS_SUPPORTED * MAX_DIES_PER_SOCKET)), NULL);
    NextCombination = TRUE;

    // 2. First, try to put Die 7 to Die (ZEN_MAX_SYSTEM_DIE_COUNT - NumberOfDieBelowPcieCfg) below Pcie Cfg
    if (NumberOfDieBelowPcieCfg > 0 ) {
      LibAmdMemFill ((MmioIsAbovePcieCfg + (SocketNumber * DieNumberPerSocket) - NumberOfDieBelowPcieCfg), FALSE, (sizeof (BOOLEAN) * NumberOfDieBelowPcieCfg), NULL);
    }

    while (NextCombination) {
      // 3. Try this combination
      GetAnCombination = TryThisCombination (MmioSizeForEachDie, MmioBaseAddrAbovePcieCfg, MmioLimitAbovePcieCfg, MmioBaseAddrBelowPcieCfg, MmioLimitBelowPcieCfg, MmioIsAbovePcieCfg, SocketNumber, DieNumberPerSocket, FabricMmioManager, FALSE);
      if (GetAnCombination) {
        break;
      }
      // 4. If we can't make it, try to another combination
      NextCombination = GetNextCombination (NumberOfDieBelowPcieCfg, MmioIsAbovePcieCfg, SocketNumber, DieNumberPerSocket);
    }
    if ((GetAnCombination) || (MmioLimitBelowPcieCfg == MmioBaseAddrBelowPcieCfg)) {
      // If we already got an combination or there's no space above Pcie Cfg, then break
      break;
    }

  }

  if (GetAnCombination) {
    TryThisCombination (MmioSizeForEachDie, MmioBaseAddrAbovePcieCfg, MmioLimitAbovePcieCfg, MmioBaseAddrBelowPcieCfg, MmioLimitBelowPcieCfg, MmioIsAbovePcieCfg, SocketNumber, DieNumberPerSocket, FabricMmioManager, SetDfRegisters);
  }

  return GetAnCombination;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Get a different combination
 *
 *
 * @param[in]         NumberOfDieBelowPcieCfg     How many Dies MMIO should be put below Pcie Cfg
 * @param[in, out]    MmioIsAbovePcieCfg          An BOOLEAN array, indicate which Die's MMIO is above Pcie Cfg
 * @param[in]         SocketNumber                System socket count
 * @param[in]         DieNumberPerSocket          Die number per socket
 *
 * @retval            TRUE                        Get a combination
 *                    FALSE                       All combinations have been tried
 */
BOOLEAN
GetNextCombination (
  IN       UINT8                          NumberOfDieBelowPcieCfg,
  IN OUT   BOOLEAN                       *MmioIsAbovePcieCfg,
  IN       UINT8                          SocketNumber,
  IN       UINT8                          DieNumberPerSocket
  )
{
  UINT8  MoveThisOne;
  UINT8  ResetTheseDie;

  // From Die 0, there're total number of 'ResetTheseDie' sequential DIE, whose MMIO is below Pcie Cfg
  for (ResetTheseDie = 0; ResetTheseDie < NumberOfDieBelowPcieCfg; ResetTheseDie++) {
    if (*(MmioIsAbovePcieCfg + ResetTheseDie) == TRUE) {
      break;
    }
  }

  // if ResetTheseDie == NumberOfDieBelowPcieCfg
  // All combinations have been tried.
  if (ResetTheseDie == NumberOfDieBelowPcieCfg) {
    return FALSE;
  }

  // After number of ResetTheseDie DIEs, find out the first Die whose MMIO is below Pcie Cfg.
  for (MoveThisOne = ResetTheseDie + 1; MoveThisOne < (SocketNumber * DieNumberPerSocket); MoveThisOne++) {
    if (*(MmioIsAbovePcieCfg + MoveThisOne) == FALSE) {
      break;
    }
  }
  ASSERT (MoveThisOne < (SocketNumber * DieNumberPerSocket));
  *(MmioIsAbovePcieCfg + MoveThisOne) = TRUE;
  *(MmioIsAbovePcieCfg + MoveThisOne - 1) = FALSE;
  LibAmdMemFill (MmioIsAbovePcieCfg, TRUE, (sizeof (BOOLEAN) * ResetTheseDie), NULL);
  LibAmdMemFill ((MmioIsAbovePcieCfg + MoveThisOne - 1 - ResetTheseDie), FALSE, (sizeof (BOOLEAN) * ResetTheseDie), NULL);

  return TRUE;
}

/*---------------------------------------------------------------------------------------*/
/**
 * If there's enough space for current combination
 *
 *
 * @param[in]         MmioSizeForEachDie          Required MMIO size for each Die
 * @param[in]         MmioBaseAddrAbovePcieCfg    MmioBaseAddrAbovePcieCfg
 * @param[in]         MmioLimitAbovePcieCfg       MmioLimitAbovePcieCfg
 * @param[in]         MmioBaseAddrBelowPcieCfg    MmioBaseAddrBelowPcieCfg
 * @param[in]         MmioLimitBelowPcieCfg       MmioLimitBelowPcieCfg
 * @param[in, out]    MmioIsAbovePcieCfg          An BOOLEAN array, indicate which Die's MMIO is above Pcie Cfg
 * @param[in]         SocketNumber                System socket count
 * @param[in]         DieNumberPerSocket          Die number per socket
 * @param[in]         FabricMmioManager           Point to FABRIC_MMIO_MANAGER
 * @param[in]         SetDfRegisters              TRUE  - Set DF MMIO registers
 *                                                FALSE - Not set DF MMIO registers, just calculate if user's requirment could be satisfied.
 *
 * @retval            TRUE
 *                    FALSE
 */
BOOLEAN
TryThisCombination (
  IN       FABRIC_RESOURCE_FOR_EACH_DIE  *MmioSizeForEachDie,
  IN       UINT64                         MmioBaseAddrAbovePcieCfg,
  IN       UINT64                         MmioLimitAbovePcieCfg,
  IN       UINT64                         MmioBaseAddrBelowPcieCfg,
  IN       UINT64                         MmioLimitBelowPcieCfg,
  IN OUT   BOOLEAN                       *MmioIsAbovePcieCfg,
  IN       UINT8                          SocketNumber,
  IN       UINT8                          DieNumberPerSocket,
  IN       FABRIC_MMIO_MANAGER           *FabricMmioManager,
  IN       BOOLEAN                        SetDfRegisters
  )
{
  UINT8  i;
  UINT8  j;
  UINT8  SocketLoop;
  UINT8  DieLoop;
  UINT8  DstFabricIDSysOffset;
  UINT64 MmioBaseAddr; // To caculate oversize, we must use UINT64 here for all address, size
  UINT64 MmioCeiling;
  UINT64 MmioBaseAddrPrefetchable;
  UINT64 MmioBaseAddrNonPrefetchable;
  UINT64 MmioBaseAddrNonPci;
  UINT64 Die0SecondMmioSize;
  UINT64 SizePrefetchable;
  UINT64 SizeNonPrefetchable;
  UINT64 SizeNonPci;
  UINT64 AlignMask;
  UINT64 AlignMaskP;
  UINT64 AlignMaskNonPci;
  UINT64 BottomOfCompat;
  UINT64 OverSizeAbovePcieCfg;
  UINT64 OverSizeBelowPcieCfg;
  UINT64 AlignForFirstMmioRegionAbovePcieCfg;
  UINT64 AlignForFirstMmioRegionBelowPcieCfg;
  BOOLEAN AlreadyGotAlignForFirstMmioRegionAbovePcieCfg;
  BOOLEAN AlreadyGotAlignForFirstMmioRegionBelowPcieCfg;
  BOOLEAN BigAlignFirstAbovePcieCfg;
  BOOLEAN BigAlignFirstBelowPcieCfg;
  BOOLEAN BigAlignFirst;
  BOOLEAN ReservedRegionAlreadySet;
  BOOLEAN OverSizeFlag;

  BigAlignFirstAbovePcieCfg = TRUE;
  BigAlignFirstBelowPcieCfg = TRUE;
  OverSizeFlag = FALSE;
  AlreadyGotAlignForFirstMmioRegionAbovePcieCfg = FALSE;
  AlreadyGotAlignForFirstMmioRegionBelowPcieCfg = FALSE;
  SizeNonPci = (PcdGet32 (PcdAmdMmioSizePerDieForNonPciDevice) > MMIO_MIN_NON_PCI_SIZE) ? PcdGet32 (PcdAmdMmioSizePerDieForNonPciDevice): MMIO_MIN_NON_PCI_SIZE;
  AlignMaskNonPci = ZEN_NON_PCI_MMIO_ALIGN_MASK;
  AlignForFirstMmioRegionAbovePcieCfg = 0;
  AlignForFirstMmioRegionBelowPcieCfg = 0;

  BottomOfCompat = BOTTOM_OF_COMPAT;
  /// @todo For ZP B2 and later, 0xFED0_0000 ~ 0xFED0_FFFF should be POSTED
  /// @todo BottomOfCompat = ADDITIONAL_POSTED_REGION_UNDER_DIE0_END + 1;
  ReservedRegionAlreadySet = FALSE;  // Indicate if PcdAmdBottomMmioReservedForDie0 ~ BottomOfCompat is set

  for (i = SocketNumber; i > 0; i--) {
    for (j = DieNumberPerSocket; j > 0; j--) {
      SocketLoop = i - 1;
      DieLoop = j - 1;

      if ((MmioSizeForEachDie->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Size + MmioSizeForEachDie->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Size + SizeNonPci) == 0) {
        continue;
      }

      AlignMask  = MmioSizeForEachDie->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Alignment;
      AlignMaskP = MmioSizeForEachDie->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Alignment;

      if (*(MmioIsAbovePcieCfg + SocketLoop * ZEN_MAX_DIE_PER_SOCKET + DieLoop)) {
        // Mmio is above PcieCfg
        BigAlignFirst = BigAlignFirstAbovePcieCfg;
        MmioBaseAddr = MmioBaseAddrAbovePcieCfg;
        BigAlignFirstAbovePcieCfg = !BigAlignFirstAbovePcieCfg;

        if (!AlreadyGotAlignForFirstMmioRegionAbovePcieCfg) {
          // Save alignment for the first MMIO region, it will be used for calculate oversize
          AlignForFirstMmioRegionAbovePcieCfg = (AlignMask > AlignMaskP) ? AlignMask : AlignMaskP;
          AlreadyGotAlignForFirstMmioRegionAbovePcieCfg = TRUE;
        }
      } else {
        // Mmio is below PcieCfg
        BigAlignFirst = BigAlignFirstBelowPcieCfg;
        MmioBaseAddr = MmioBaseAddrBelowPcieCfg;
        BigAlignFirstBelowPcieCfg = !BigAlignFirstBelowPcieCfg;

        if (!AlreadyGotAlignForFirstMmioRegionBelowPcieCfg) {
          // Save alignment for the first MMIO region, it will be used for calculate oversize
          AlignForFirstMmioRegionBelowPcieCfg = (AlignMask > AlignMaskP) ? AlignMask: AlignMaskP;
          AlreadyGotAlignForFirstMmioRegionBelowPcieCfg = TRUE;
        }
      }

      if (BigAlignFirst) {
        if (MmioSizeForEachDie->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Alignment >= MmioSizeForEachDie->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Alignment) {
          // Prefetchable -> Non Prefetchable -> Non Pci
          MmioBaseAddrPrefetchable = (MmioBaseAddr + AlignMaskP) & (~AlignMaskP);
          MmioBaseAddrNonPrefetchable = (MmioBaseAddrPrefetchable + MmioSizeForEachDie->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Size + AlignMask) & (~AlignMask);
          MmioBaseAddrNonPci = (MmioBaseAddrNonPrefetchable + MmioSizeForEachDie->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Size + AlignMaskNonPci) & (~AlignMaskNonPci);
          SizePrefetchable = MmioBaseAddrNonPrefetchable - MmioBaseAddrPrefetchable;
          SizeNonPrefetchable = MmioBaseAddrNonPci - MmioBaseAddrNonPrefetchable;
          MmioCeiling = MmioBaseAddrNonPci + SizeNonPci;
        } else {
          // Non Prefetchable -> Prefetchable -> Non Pci
          MmioBaseAddrNonPrefetchable = (MmioBaseAddr + AlignMask) & (~AlignMask);
          MmioBaseAddrPrefetchable = (MmioBaseAddrNonPrefetchable + MmioSizeForEachDie->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Size + AlignMaskP) & (~AlignMaskP);
          MmioBaseAddrNonPci = (MmioBaseAddrPrefetchable + MmioSizeForEachDie->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Size + AlignMaskNonPci) & (~AlignMaskNonPci);
          SizeNonPrefetchable = MmioBaseAddrPrefetchable - MmioBaseAddrNonPrefetchable;
          SizePrefetchable = MmioBaseAddrNonPci - MmioBaseAddrPrefetchable;
          MmioCeiling = MmioBaseAddrNonPci + SizeNonPci;
        }
      } else {
        if (MmioSizeForEachDie->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Alignment <= MmioSizeForEachDie->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Alignment) {
          // Non Pci -> Prefetchable -> Non Prefetchable
          MmioBaseAddrNonPci = (MmioBaseAddr + AlignMaskNonPci) & (~AlignMaskNonPci);
          MmioBaseAddrPrefetchable = (MmioBaseAddrNonPci + SizeNonPci + AlignMaskP) & (~AlignMaskP);
          MmioBaseAddrNonPrefetchable = (MmioBaseAddrPrefetchable + MmioSizeForEachDie->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Size + AlignMask) & (~AlignMask);
          SizePrefetchable = MmioBaseAddrNonPrefetchable - MmioBaseAddrPrefetchable;
          SizeNonPrefetchable = MmioSizeForEachDie->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Size;
          MmioCeiling = MmioBaseAddrNonPrefetchable + SizeNonPrefetchable;
        } else {
          // Non Pci -> Non Prefetchable -> Prefetchable
          MmioBaseAddrNonPci = (MmioBaseAddr + AlignMaskNonPci) & (~AlignMaskNonPci);
          MmioBaseAddrNonPrefetchable = (MmioBaseAddrNonPci + SizeNonPci + AlignMask) & (~AlignMask);
          MmioBaseAddrPrefetchable = (MmioBaseAddrNonPrefetchable + MmioSizeForEachDie->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Size + AlignMaskP) & (~AlignMaskP);
          SizeNonPrefetchable = MmioBaseAddrPrefetchable - MmioBaseAddrNonPrefetchable;
          SizePrefetchable = MmioSizeForEachDie->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Size;
          MmioCeiling = MmioBaseAddrPrefetchable + SizePrefetchable;
        }
      }

      if (SizeNonPrefetchable == 0) {
        MmioBaseAddrNonPrefetchable = 0;
      }
      if (SizePrefetchable == 0) {
        MmioBaseAddrPrefetchable = 0;
      }

      // Check if space is enough
      if (*(MmioIsAbovePcieCfg + SocketLoop * ZEN_MAX_DIE_PER_SOCKET + DieLoop)) {
        // Mmio is above PcieCfg
        MmioBaseAddrAbovePcieCfg = MmioCeiling;
        if (MmioBaseAddrAbovePcieCfg > MmioLimitAbovePcieCfg) {
          OverSizeFlag = TRUE;
        }
      } else {
        // Mmio is below PcieCfg
        MmioBaseAddrBelowPcieCfg = MmioCeiling;
        if (MmioBaseAddrBelowPcieCfg > MmioLimitBelowPcieCfg) {
          OverSizeFlag = TRUE;
        }
      }

      // Set DF MMIO registers
      if ((!OverSizeFlag) && SetDfRegisters && (FabricMmioManager != NULL)) {
        IDS_HDT_CONSOLE (CPU_TRACE, "  User Request below 4G: Socket%x Die%x Prefetch MMIO Size %lX, AlignBit %X\n", SocketLoop, DieLoop, MmioSizeForEachDie->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Size, MmioSizeForEachDie->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Alignment);
        IDS_HDT_CONSOLE (CPU_TRACE, "                           Non Prefetch MMIO Size %lX, AlignBit %X\n", MmioSizeForEachDie->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Size, MmioSizeForEachDie->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Alignment);
        ASSERT (BottomOfCompat >= PcdGet32 (PcdAmdBottomMmioReservedForDie0));

        DstFabricIDSysOffset = (UINT8) FabricTopologyGetDieSystemOffset (SocketLoop, DieLoop);
        if ((SocketLoop == 0) && (DieLoop == 0)) {
          if (*(MmioIsAbovePcieCfg + SocketLoop * ZEN_MAX_DIE_PER_SOCKET + DieLoop)) {
            FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((SocketLoop * MAX_DIES_PER_SOCKET + DieLoop) * 2), DstFabricIDSysOffset, MmioBaseAddr, (BottomOfCompat - MmioBaseAddr));
            MmioBaseAddrAbovePcieCfg = BottomOfCompat;
            ReservedRegionAlreadySet = TRUE;
          } else {
            FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((SocketLoop * MAX_DIES_PER_SOCKET + DieLoop) * 2), DstFabricIDSysOffset, MmioBaseAddr, (MmioLimitBelowPcieCfg - MmioBaseAddr));
            MmioBaseAddrBelowPcieCfg = MmioLimitBelowPcieCfg;
          }
        } else {
          FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((SocketLoop * MAX_DIES_PER_SOCKET + DieLoop) * 2), DstFabricIDSysOffset, MmioBaseAddr, (MmioCeiling - MmioBaseAddr));
        }

        FabricMmioManager->AllocateMmioBelow4GOnThisDie[SocketLoop][DieLoop] = TRUE;
        FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop].BaseNonPci = MmioBaseAddrNonPci;
        FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop].BasePrefetch = MmioBaseAddrPrefetchable;
        FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop].BaseNonPrefetch = MmioBaseAddrNonPrefetchable;
        FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop].SizeNonPci = SizeNonPci;
        FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop].SizePrefetch = SizePrefetchable;
        FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop].SizeNonPrefetch = SizeNonPrefetchable;
        FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop].UsedSizeNonPci = 0;
        FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop].UsedSizePrefetch = 0;
        FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop].UsedSizeNonPrefetch = 0;
        FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop].AlignNonPrefetch = MmioSizeForEachDie->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Alignment;
        FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop].AlignPrefetch = MmioSizeForEachDie->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop].Alignment;

        IDS_HDT_CONSOLE (CPU_TRACE, "    BasePrefetch    0x%lX, SizePrefetch    0x%lX\n", FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop].BasePrefetch, FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop].SizePrefetch);
        IDS_HDT_CONSOLE (CPU_TRACE, "    BaseNonPrefetch 0x%lX, SizeNonPrefetch 0x%lX\n", FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop].BaseNonPrefetch, FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop].SizeNonPrefetch);
        IDS_HDT_CONSOLE (CPU_TRACE, "    BaseNonPci      0x%lX, SizeNonPci      0x%lX\n", FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop].BaseNonPci, FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop].SizeNonPci);
      }
    }
  }

  // If SetDfRegisters if FALSE, this function is called by FabricReallocateResourceForEachDie
  // And we should record which combination has the minimum gap
  if (!SetDfRegisters) {
    if (OverSizeFlag) {
      if (MmioBaseAddrAbovePcieCfg > MmioLimitAbovePcieCfg) {
        OverSizeAbovePcieCfg = MmioBaseAddrAbovePcieCfg - MmioLimitAbovePcieCfg;
      } else {
        OverSizeAbovePcieCfg = 0;
      }
      if (MmioBaseAddrBelowPcieCfg > MmioLimitBelowPcieCfg) {
        OverSizeBelowPcieCfg = MmioBaseAddrBelowPcieCfg - MmioLimitBelowPcieCfg;
      } else {
        OverSizeBelowPcieCfg = 0;
      }
      if ((OverSizeAbovePcieCfg == 0) || (OverSizeBelowPcieCfg == 0)) {
        // At least above Pcie Cfg or below Pcie Cfg must have enough space, if both of them are overflow, it's not a legal combination
        if ((OverSizeAbovePcieCfg + OverSizeBelowPcieCfg) < (mOverSizeAbovePcieMin + mOverSizeBelowPcieMin)) {
          mOverSizeAbovePcieMin = (UINT32) OverSizeAbovePcieCfg;
          mOverSizeBelowPcieMin = (UINT32) OverSizeBelowPcieCfg;
        }
        if (OverSizeAbovePcieCfg != 0) {
          mAlignmentMask = (UINT32) AlignForFirstMmioRegionAbovePcieCfg;
        } else {
          mAlignmentMask = (UINT32) AlignForFirstMmioRegionBelowPcieCfg;
        }
      }
    } else {
      mOverSizeAbovePcieMin = 0;
      mOverSizeBelowPcieMin = 0;
    }
  }

  // Die0's 2nd MMIO
  if ((!OverSizeFlag) && SetDfRegisters && (FabricMmioManager != NULL)) {
    if (((MmioBaseAddrAbovePcieCfg + ZEN_MMIO_MIN_SIZE) <= MmioLimitAbovePcieCfg) || ((MmioBaseAddrBelowPcieCfg + ZEN_MMIO_MIN_SIZE) <= MmioLimitBelowPcieCfg)) {
      if ((MmioSizeForEachDie->Die0SecondNonPrefetchableMmioSizeBelow4G.Size != 0) || (MmioSizeForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Size != 0)) {
        if ((SocketNumber * DieNumberPerSocket) < (ZEN_MAX_DIE_PER_SOCKET * ZEN_MAX_SOCKET)) {
          for (i = 0; i < ZEN_MAX_SOCKET; i++) {
            for (j = 0; j < ZEN_MAX_DIE_PER_SOCKET; j++) {
              if (FabricMmioManager->AllocateMmioBelow4GOnThisDie[i][j] == FALSE) {
                IDS_HDT_CONSOLE (CPU_TRACE, "  Die0 has 2nd MMIO below 4G\n");
                FabricMmioManager->Die02ndMmioPairBelow4G = (UINT8) ((i << 4) | j);
                if (MmioBaseAddrAbovePcieCfg < MmioLimitAbovePcieCfg) {
                  FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((i * MAX_DIES_PER_SOCKET + j) * 2), 0, MmioBaseAddrAbovePcieCfg, (BottomOfCompat - MmioBaseAddrAbovePcieCfg));
                  MmioBaseAddr = MmioBaseAddrAbovePcieCfg;
                  Die0SecondMmioSize = MmioLimitAbovePcieCfg - MmioBaseAddrAbovePcieCfg;
                  ReservedRegionAlreadySet = TRUE;
                } else {
                  FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((i * MAX_DIES_PER_SOCKET + j) * 2), 0, MmioBaseAddrBelowPcieCfg, (MmioLimitBelowPcieCfg - MmioBaseAddrBelowPcieCfg));
                  MmioBaseAddr = MmioBaseAddrBelowPcieCfg;
                  Die0SecondMmioSize = MmioLimitBelowPcieCfg - MmioBaseAddrBelowPcieCfg;
                }
                // Distribute Die0's 2nd MMIO base on the same ratio as 1st MMIO
                AlignMask  = MmioSizeForEachDie->Die0SecondNonPrefetchableMmioSizeBelow4G.Alignment;
                AlignMaskP = MmioSizeForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Alignment;
                if (MmioSizeForEachDie->Die0SecondNonPrefetchableMmioSizeBelow4G.Size == 0) {
                  SizePrefetchable = Die0SecondMmioSize;
                  SizeNonPrefetchable = 0;
                } else if (MmioSizeForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Size == 0) {
                  SizePrefetchable = 0;
                  SizeNonPrefetchable = Die0SecondMmioSize;
                } else {
                  SizePrefetchable = MultU64x64 (DivU64x32 (Die0SecondMmioSize, ((UINT32) (MmioSizeForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Size + MmioSizeForEachDie->Die0SecondNonPrefetchableMmioSizeBelow4G.Size))), MmioSizeForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Size);
                  SizeNonPrefetchable = Die0SecondMmioSize - SizePrefetchable;
                }

                if (SizePrefetchable != 0) {
                  MmioBaseAddrPrefetchable = (MmioBaseAddr + AlignMaskP) & (~AlignMaskP);
                  SizePrefetchable = SizePrefetchable;
                } else {
                  MmioBaseAddrPrefetchable = MmioBaseAddr;
                  SizePrefetchable = 0;
                }

                if (SizeNonPrefetchable != 0) {
                  MmioBaseAddrNonPrefetchable = (MmioBaseAddrPrefetchable + SizePrefetchable + AlignMask) & (~AlignMask);
                  SizeNonPrefetchable = MmioBaseAddr + Die0SecondMmioSize - MmioBaseAddrNonPrefetchable;
                } else {
                  MmioBaseAddrNonPrefetchable = MmioBaseAddrPrefetchable + SizePrefetchable;
                  SizeNonPrefetchable = 0;
                }

                if (SizeNonPrefetchable == 0) {
                  MmioBaseAddrNonPrefetchable = 0;
                }
                if (SizePrefetchable == 0) {
                  MmioBaseAddrPrefetchable = 0;
                }

                FabricMmioManager->MmioRegionBelow4G[i][j].BasePrefetch = MmioBaseAddrPrefetchable;
                FabricMmioManager->MmioRegionBelow4G[i][j].BaseNonPrefetch = MmioBaseAddrNonPrefetchable;
                FabricMmioManager->MmioRegionBelow4G[i][j].BaseNonPci = 0;
                FabricMmioManager->MmioRegionBelow4G[i][j].SizePrefetch = SizePrefetchable;
                FabricMmioManager->MmioRegionBelow4G[i][j].SizeNonPrefetch = SizeNonPrefetchable;
                FabricMmioManager->MmioRegionBelow4G[i][j].SizeNonPci = 0;
                FabricMmioManager->MmioRegionBelow4G[i][j].UsedSizePrefetch = 0;
                FabricMmioManager->MmioRegionBelow4G[i][j].UsedSizeNonPrefetch = 0;
                FabricMmioManager->MmioRegionBelow4G[i][j].UsedSizeNonPci = 0;
                FabricMmioManager->MmioRegionBelow4G[i][j].AlignNonPrefetch = MmioSizeForEachDie->Die0SecondNonPrefetchableMmioSizeBelow4G.Alignment;
                FabricMmioManager->MmioRegionBelow4G[i][j].AlignPrefetch = MmioSizeForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Alignment;

                IDS_HDT_CONSOLE (CPU_TRACE, "    BasePrefetch    0x%lX, SizePrefetch    0x%lX\n", FabricMmioManager->MmioRegionBelow4G[i][j].BasePrefetch, FabricMmioManager->MmioRegionBelow4G[i][j].SizePrefetch);
                IDS_HDT_CONSOLE (CPU_TRACE, "    BaseNonPrefetch 0x%lX, SizeNonPrefetch 0x%lX\n", FabricMmioManager->MmioRegionBelow4G[i][j].BaseNonPrefetch, FabricMmioManager->MmioRegionBelow4G[i][j].SizeNonPrefetch);
                IDS_HDT_CONSOLE (CPU_TRACE, "    BaseNonPci      0x%lX, SizeNonPci      0x%lX\n", FabricMmioManager->MmioRegionBelow4G[i][j].BaseNonPci, FabricMmioManager->MmioRegionBelow4G[i][j].SizeNonPci);

                FabricMmioManager->Die0Has2ndMmioBelow4G = TRUE;
                break;
              }
            }
            if (FabricMmioManager->Die0Has2ndMmioBelow4G) {
              break;
            }
          }
        }
      }
    }

    // If there's a spare MMIO register pair, set 0xFED0_0000 ~ 0xFED0_FFFF as posted
    FabricZenZpAdditionalMmioSetting (FabricMmioManager, SocketNumber, DieNumberPerSocket, BottomOfCompat, ReservedRegionAlreadySet);
  }


  return (!OverSizeFlag);
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricZenZpAdditionalMmioSetting
 *
 * If reserved region for Die0 (PcdAmdBottomMmioReservedForDie0 ~ BottomOfCompat) has not been set,
 * or that region doesn't include ADDITIONAL_POSTED_REGION_UNDER_DIE0_START ~ ADDITIONAL_POSTED_REGION_UNDER_DIE0_END,
 * set ADDITIONAL_POSTED_REGION_UNDER_DIE0_START ~ ADDITIONAL_POSTED_REGION_UNDER_DIE0_END as POSTED
 *
 * @param[in, out]    FabricMmioManager           Point to FABRIC_MMIO_MANAGER
 * @param[in]         SocketNumber                System socket count
 * @param[in]         DieNumberPerSocket          Die number per socket
 * @param[in]         BottomOfCompat              From BottomOfCompat to 4G is COMPAT region
 * @param[in]         ReservedRegionAlreadySet    PcdAmdBottomMmioReservedForDie0 ~ BottomOfCompat has been set or not
 *
 */
VOID
FabricZenZpAdditionalMmioSetting (
  IN       FABRIC_MMIO_MANAGER *FabricMmioManager,
  IN       UINT8                SocketNumber,
  IN       UINT8                DieNumberPerSocket,
  IN       UINT64               BottomOfCompat,
  IN       BOOLEAN              ReservedRegionAlreadySet
  )
{
  UINT8   i;
  UINT8   j;
  UINT64  Base;
  UINT64  Size;
  BOOLEAN AlreadySet;

  AlreadySet = FALSE;
  Base = ADDITIONAL_POSTED_REGION_UNDER_DIE0_START;
  Size = ADDITIONAL_POSTED_REGION_UNDER_DIE0_END - ADDITIONAL_POSTED_REGION_UNDER_DIE0_START + 1;

  // If reserved region for Die0 (PcdAmdBottomMmioReservedForDie0 ~ BottomOfCompat) has not been set
  // or that region doesn't include ADDITIONAL_POSTED_REGION_UNDER_DIE0_START ~ ADDITIONAL_POSTED_REGION_UNDER_DIE0_END
  if ((!ReservedRegionAlreadySet) ||
      ((ReservedRegionAlreadySet) && (BottomOfCompat <= ADDITIONAL_POSTED_REGION_UNDER_DIE0_END))) {
    if (Size >= ZEN_MMIO_MIN_SIZE) {
      for (i = 0; i < ZEN_MAX_SOCKET; i++) {
        for (j = 0; j < ZEN_MAX_DIE_PER_SOCKET; j++) {
          // Find out a spare MMIO register pair
          if (FabricMmioManager->AllocateMmioBelow4GOnThisDie[i][j] == FALSE) {
            if ((FabricMmioManager->Die0Has2ndMmioBelow4G) &&
                (((FabricMmioManager->Die02ndMmioPairBelow4G >> 4) & 0xF) == i) &&
                ((FabricMmioManager->Die02ndMmioPairBelow4G & 0xF) == j)) {
              continue;
            }
            FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((i * MAX_DIES_PER_SOCKET + j) * 2), 0, Base, Size);
            AlreadySet = TRUE;
            break;
          }

          if (FabricMmioManager->AllocateMmioAbove4GOnThisDie[i][j] == FALSE) {
            FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((i * MAX_DIES_PER_SOCKET + j) * 2 + 1), 0, Base, Size);
            AlreadySet = TRUE;
            break;
          }
        }
        if (AlreadySet) {
          break;
        }
      }
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricZenZpSetMmioReg
 *
 * Set MMIO register pairs according to input parameters
 *
 * @param[in]         TotalSocket                 System socket count
 * @param[in]         DiePerSocket                Die number per socket
 * @param[in]         MmioPairIndex               Which MMIO register pair should be set
 * @param[in]         DstFabricIDSysOffset        Fabric ID system offset
 * @param[in]         BaseAddress                 Base address of MMIO region
 * @param[in]         Length                      Length of MMIO region
 *
 */
VOID
FabricZenZpSetMmioReg (
  IN       UINT8  TotalSocket,
  IN       UINT8  DiePerSocket,
  IN       UINT8  MmioPairIndex,
  IN       UINT8  DstFabricIDSysOffset,
  IN       UINT64 BaseAddress,
  IN       UINT64 Length
  )
{
  UINTN  i;
  UINTN  j;
  MMIO_ADDR_CTRL MmioAddrCtrlReg;

  ASSERT (Length >= ZEN_MMIO_MIN_SIZE);
  BaseAddress = (BaseAddress + 0xFFFF) & 0xFFFFFFFFFFFF0000;
  MmioAddrCtrlReg.Value = FabricRegisterAccRead (0, 0, 0x0, (MMIO_ADDRESS_CONTROL_REG_0 + MmioPairIndex * 0x10), FABRIC_REG_ACC_BC);
  for (i = 0; i < TotalSocket; i++) {
    for (j = 0; j < DiePerSocket; j++) {
      FabricRegisterAccWrite (i, j, 0x0, (MMIO_BASE_ADDRESS_REG_0  + MmioPairIndex * 0x10), FABRIC_REG_ACC_BC, (UINT32) (BaseAddress >> 16), TRUE);
      FabricRegisterAccWrite (i, j, 0x0, (MMIO_LIMIT_ADDRESS_REG_0 + MmioPairIndex * 0x10), FABRIC_REG_ACC_BC, (UINT32) ((BaseAddress + Length - 1) >> 16), TRUE);
      MmioAddrCtrlReg.Field.RE = 1;
      MmioAddrCtrlReg.Field.WE = 1;
      MmioAddrCtrlReg.Field.DstFabricID &= 0x1F;
      MmioAddrCtrlReg.Field.DstFabricID |= DstFabricIDSysOffset;
      FabricRegisterAccWrite (i, j, 0x0, (MMIO_ADDRESS_CONTROL_REG_0 + MmioPairIndex * 0x10), FABRIC_REG_ACC_BC, MmioAddrCtrlReg.Value, TRUE);
    }
  }
  IDS_HDT_CONSOLE (CPU_TRACE, "  AGESA set MMIO pair #%X, 0x%X0000 ~ 0x%XFFFF DstFabricID: 0x%X\n", MmioPairIndex, (UINT32) (BaseAddress >> 16), (UINT32) ((BaseAddress + Length - 1) >> 16), MmioAddrCtrlReg.Field.DstFabricID);
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricZenZpSetIoReg
 *
 * Set IO register pairs according to input parameters
 *
 * @param[in]         TotalSocket                 System socket count
 * @param[in]         DiePerSocket                Die number per socket
 * @param[in]         RegIndex                    Which IO register should be set
 * @param[in]         DstFabricIDSysOffset        Fabric ID system offset
 * @param[in]         IoBase                      Base address of IO region
 * @param[in]         IoSize                      Length of IO region
 *
 */
VOID
FabricZenZpSetIoReg (
  IN       UINT8  TotalSocket,
  IN       UINT8  DiePerSocket,
  IN       UINT8  RegIndex,
  IN       UINT8  DstFabricIDSysOffset,
  IN       UINT32 IoBase,
  IN       UINT32 IoSize
  )
{
  UINTN  i;
  UINTN  j;
  X86IO_BASE_ADDR  IoBaseReg;
  X86IO_LIMIT_ADDR IoLimitReg;

  IoBaseReg.Value  = 0;
  IoLimitReg.Value = FabricRegisterAccRead (0, 0, 0x0, (X86IO_LIMIT_ADDRESS_REG0 + RegIndex * 8), FABRIC_REG_ACC_BC);

  for (i = 0; i < TotalSocket; i++) {
    for (j = 0; j < DiePerSocket; j++) {
      IoBaseReg.Field.RE = 1;
      IoBaseReg.Field.WE = 1;
      IoBaseReg.Field.IOBase = (IoBase & 0xFFFFF000) >> 12;
      FabricRegisterAccWrite (i, j, 0x0, (X86IO_BASE_ADDRESS_REG0 + RegIndex * 8), FABRIC_REG_ACC_BC, IoBaseReg.Value, TRUE);

      IoLimitReg.Field.DstFabricID &= 0x1F;
      IoLimitReg.Field.DstFabricID |= DstFabricIDSysOffset;
      IoLimitReg.Field.IOLimit = ((IoBase + IoSize - 1) & 0xFFFFF000) >> 12;
      FabricRegisterAccWrite (i, j, 0x0, (X86IO_LIMIT_ADDRESS_REG0 + RegIndex * 8), FABRIC_REG_ACC_BC, IoLimitReg.Value, TRUE);
    }
  }
  IDS_HDT_CONSOLE (CPU_TRACE, "  AGESA set IO pair #%X, 0x%X000 ~ 0x%XFFF DstFabricID: 0x%X\n", RegIndex, (UINT32) (IoBase >> 12), (UINT32) ((IoBase + IoSize - 1) >> 12), IoLimitReg.Field.DstFabricID);

}




