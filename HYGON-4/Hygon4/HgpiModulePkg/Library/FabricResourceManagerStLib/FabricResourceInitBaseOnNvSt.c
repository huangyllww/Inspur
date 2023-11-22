/* $NoKeywords:$ */

/**
 * @file
 *
 * Fabric MMIO initialization base on NV variable for DHARMA SAT
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Library
 *
 */
/*
 ******************************************************************************
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD.  All Rights Reserved.
 *
 * HYGON is granting you permission to use this software and documentation (if
 * any) (collectively, the "Materials") pursuant to the terms and conditions of
 * the Software License Agreement included with the Materials.  If you do not
 * have a copy of the Software License Agreement, contact your HYGON
 * representative for a copy.
 *
 * You agree that you will not reverse engineer or decompile the Materials, in
 * whole or in part, except as allowed by applicable law.
 *
 * WARRANTY DISCLAIMER:  THE MATERIALS ARE PROVIDED "AS IS" WITHOUT WARRANTY OF
 * ANY KIND.  HYGON DISCLAIMS ALL WARRANTIES, EXPRESS, IMPLIED, OR STATUTORY,
 * INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE, NON-INFRINGEMENT, THAT THE
 * MATERIALS WILL RUN UNINTERRUPTED OR ERROR-FREE OR WARRANTIES ARISING FROM
 * CUSTOM OF TRADE OR COURSE OF USAGE.  THE ENTIRE RISK ASSOCIATED WITH THE USE
 * OF THE MATERIAL IS ASSUMED BY YOU.  Some jurisdictions do not allow the
 * exclusion of implied warranties, so the above exclusion may not apply to
 * You.
 *
 * LIMITATION OF LIABILITY AND INDEMNIFICATION:  HYGON AND ITS LICENSORS WILL
 * NOT, UNDER ANY CIRCUMSTANCES BE LIABLE TO YOU FOR ANY PUNITIVE, DIRECT,
 * INCIDENTAL, INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES ARISING FROM USE OF
 * THE MATERIALS OR THIS AGREEMENT EVEN IF HYGON AND ITS LICENSORS HAVE BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.  In no event shall HYGON's total
 * liability to You for all damages, losses, and causes of action (whether in
 * contract, tort (including negligence) or otherwise) exceed the amount of
 * $100 USD. You agree to defend, indemnify and hold harmless HYGON and its
 * licensors, and any of their directors, officers, employees, affiliates or
 * agents from and against any and all loss, damage, liability and other
 * expenses (including reasonable attorneys' fees), resulting from Your use of
 * the Materials or violation of the terms and conditions of this Agreement.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS:  The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgment of HYGON's proprietary rights in them.
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
#include "HYGON.h"
#include "Filecode.h"
#include <FabricRegistersST.h>
#include <Library/BaseLib.h>
#include <Library/HygonBaseLib.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Library/FabricResourceSizeForEachRbLib.h>
#include "FabricResourceManagerStLib.h"
#include <Library/PcdLib.h>

#define FILECODE  LIBRARY_FABRICRESOURCEMANAGERSTLIB_FABRICRESOURCEINITBASEONNVST_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
UINT32  mOverSizeBelowPcieMin = 0xFFFFFFFF;
UINT32  mOverSizeAbovePcieMin = 0xFFFFFFFF;
UINT32  mAlignmentMask = 0;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
BOOLEAN
ArrangeMmioBelow4G (
  IN       FABRIC_RESOURCE_FOR_EACH_RB  *MmioSizeForEachRb,
  IN       UINT64                         MmioBaseAddrAbovePcieCfg,
  IN       UINT64                         MmioLimitAbovePcieCfg,
  IN       UINT64                         MmioBaseAddrBelowPcieCfg,
  IN       UINT64                         MmioLimitBelowPcieCfg,
  IN OUT   BOOLEAN                       *MmioIsAbovePcieCfg,
  IN       UINT8                          SocketNumber,
  IN       UINT8                          IodNumberPerSocket,
  IN       UINT8                          RbNumberPerIod,
  IN       FABRIC_MMIO_MANAGER           *FabricMmioManager,
  IN       BOOLEAN                        SetDfRegisters
  );

BOOLEAN
GetNextCombination (
  IN       UINT8                          NumberOfRbBelowPcieCfg,
  IN OUT   BOOLEAN                       *MmioIsAbovePcieCfg,
  IN       UINT8                          SocketNumber,
  IN       UINT8                          IodNumberPerSocket,
  IN       UINT8                          RbNumberPerIod
  );

BOOLEAN
TryThisCombination (
  IN       FABRIC_RESOURCE_FOR_EACH_RB  *MmioSizeForEachRb,
  IN       UINT64                         MmioBaseAddrAbovePcieCfg,
  IN       UINT64                         MmioLimitAbovePcieCfg,
  IN       UINT64                         MmioBaseAddrBelowPcieCfg,
  IN       UINT64                         MmioLimitBelowPcieCfg,
  IN OUT   BOOLEAN                       *MmioIsAbovePcieCfg,
  IN       UINT8                          SocketNumber,
  IN       UINT8                          IodNumberPerSocket,
  IN       UINT8                          RbNumberPerIod,
  IN       FABRIC_MMIO_MANAGER           *FabricMmioManager,
  IN       BOOLEAN                        SetDfRegisters
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/*---------------------------------------------------------------------------------------*/

/**
 * FabricStInitIoBaseOnNvVariable
 *
 * Initialize IO registers for each root bridge base on NV variable.
 *
 * @param[in, out]    FabricIoManager          Point to FABRIC_IO_MANAGER
 * @param[in]         IoSizeForEachRb          How much IO size is required for each Rb
 * @param[in, out]    SpaceStaus               Current status
 * @param[in]         SocketNumber             System socket count
 * @param[in]         RbNumberPerSocket        Rb number per socket
 * @param[in]         SetDfRegisters           TRUE  - Set DF MMIO registers
 *                                             FALSE - Not set DF MMIO registers, just calculate if user's requirment could be satisfied.
 *
 */
EFI_STATUS
FabricStInitIoBaseOnNvVariable (
  IN       FABRIC_IO_MANAGER   *FabricIoManager,
  IN       FABRIC_RESOURCE_FOR_EACH_RB *IoSizeForEachRb,
  IN       FABRIC_ADDR_SPACE_SIZE       *SpaceStatus,
  IN       UINT8                SocketNumber,
  IN       UINT8                IodNumberPerSocket,
  IN       UINT8                RbNumberPerIod,
  IN       BOOLEAN              SetDfRegisters
  )
{
  UINT8   i;
  UINT8   j;
  UINT8   k;
  UINT8   RegIndex;
  UINT16  DstFabricID;
  UINT32  IoBase;
  UINT32  IoSize;
  UINT32  LegacyIoSize;

  IoBase       = 0;
  RegIndex     = 0;
  LegacyIoSize = X86_LEGACY_IO_SIZE;
  IDS_HDT_CONSOLE (CPU_TRACE, "    reserve 0x%X IO size for legacy devices\n", LegacyIoSize);

  for (i = 0; i < SocketNumber; i++) {
    for (j = 0; j < IodNumberPerSocket; j++) {
      for (k = 0; k < RbNumberPerIod; k++) {
        if (SpaceStatus != NULL) {
          SpaceStatus->IoSize += (UINT32)IoSizeForEachRb->IO[i][j][k].Size;
        }

        IDS_HDT_CONSOLE (CPU_TRACE, "    Socket %d Rb %d request IO size = 0x%X \n", i, j, IoSizeForEachRb->IO[i][j][k].Size);

        DstFabricID = (UINT16)FabricTopologyGetHostBridgeSystemFabricID (i, j, k);
        if ((i == (SocketNumber - 1)) && (j == (IodNumberPerSocket - 1)) && (k == (RbNumberPerIod - 1))) {
          // 1. first, check if it's the last root bridge. Set IO limit up to 0x1FFFFFF for last root bridge.
          IoSize = X86IO_LIMIT - IoBase;
        } else if ((i == 0) && (j == 0) && (k == 0)) {
          // 2. second, if it's root bridge 0 and it's not the last root bridge, plus size with LegacyIoSize
          IoSize = (UINT32)IoSizeForEachRb->IO[i][j][k].Size + LegacyIoSize;
        } else {
          IoSize = (UINT32)IoSizeForEachRb->IO[i][j][k].Size;
        }

        if (IoSize == 0) {
          continue;
        }

        if ((FabricIoManager != NULL) && SetDfRegisters) {
          FabricStSetIoReg (SocketNumber, IodNumberPerSocket, RegIndex, DstFabricID, IoBase, IoSize);

          if ((i == 0) && (j == 0) && (k == 0)) {
            FabricIoManager->IoRegion[i][j][k].IoBase = IoBase + LegacyIoSize;
            FabricIoManager->IoRegion[i][j][k].IoSize = IoSize - LegacyIoSize;
          } else {
            FabricIoManager->IoRegion[i][j][k].IoBase = IoBase;
            FabricIoManager->IoRegion[i][j][k].IoSize = IoSize;
          }

          FabricIoManager->IoRegion[i][j][k].IoUsed = 0;

          IDS_HDT_CONSOLE (CPU_TRACE, "  Socket %d IOD %d Rb %d has IO base 0x%X size 0x%X\n", i, j, k, FabricIoManager->IoRegion[i][j][k].IoBase, FabricIoManager->IoRegion[i][j][k].IoSize);
        }

        IoBase += IoSize;
        RegIndex++;
      }
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
 * FabricStInitMmioBaseOnNvVariable
 *
 * Initialize MMIO registers for each root bridge base on NV variable.
 *
 * @param[in, out]    FabricMmioManager        Point to FABRIC_MMIO_MANAGER
 * @param[in]         MmioSizeForEachRb        How much MMIO size is required for each Rb
 * @param[in, out]    SpaceStaus               Current status
 * @param[in]         SocketNumber             System socket count
 * @param[in]         RbNumberPerSocket        Rb number per socket
 * @param[in]         SetDfRegisters           TRUE  - Set DF MMIO registers
 *                                             FALSE - Not set DF MMIO registers, just calculate if user's requirment could be satisfied.
 *
 */
EFI_STATUS
FabricStInitMmioBaseOnNvVariable (
  IN       FABRIC_MMIO_MANAGER *FabricMmioManager,
  IN       FABRIC_RESOURCE_FOR_EACH_RB *MmioSizeForEachRb,
  IN       FABRIC_ADDR_SPACE_SIZE       *SpaceStatus,
  IN       UINT8                SocketNumber,
  IN       UINT8                IodNumberPerSocket,
  IN       UINT8                RbNumberPerIod,
  IN       BOOLEAN              SetDfRegisters
  )
{
  UINT8       i;
  UINT8       j;
  UINT8       k;
  UINT16      DstFabricID;
  UINT64      MmioLimitAbove4G;
  UINT64      MmioBaseAddr;
  UINT64      MmioBaseAddrNextRb;
  UINT64      MmioBaseAddrPrefetchable;
  UINT64      MmioBaseAddrNonPrefetchable;
  UINT64      SizePrefetchable;
  UINT64      SizeNonPrefetchable;
  UINT64      MmioBaseAddrAbovePcieCfg;
  UINT64      MmioBaseAddrBelowPcieCfg;
  UINT64      MmioLimitAbovePcieCfg;
  UINT64      MmioLimitBelowPcieCfg;
  UINT64      TOM;
  UINT64      TOM2;
  UINT64      PciCfgSpace;
  UINT64      AlignMask;
  UINT64      AlignMaskP;
  EFI_STATUS  Status;
  BOOLEAN     MmioIsAbovePcieCfg[MAX_SOCKETS_SUPPORTED * MAX_IOD_PER_SOCKET * MAX_RBS_PER_IOD];
  BOOLEAN     EnoughSpaceAbove4G;
  BOOLEAN     LargeAlignFirst;
  BOOLEAN     LastCombinationWork;

  Status = EFI_SUCCESS;

  // If MmioSizeForEachRb is not NULL, we should record current space status
  // Init global variable
  if (SpaceStatus != NULL) {
    mOverSizeBelowPcieMin = 0xFFFFFFFF;
    mOverSizeAbovePcieMin = 0xFFFFFFFF;
    mAlignmentMask = 0;
  }

  // System information
  PciCfgSpace = (AsmReadMsr64 (0xC0010058) >> 2) & 0xF;                   // Get bus range from MSR_C001_0058[5:2][BusRange]
  PciCfgSpace = MultU64x64 (((UINT64)1 << PciCfgSpace), (1024 * 1024));   // The size of configuration space is 1MB times the number of buses
  TOM  = AsmReadMsr64 (0xC001001A);
  TOM2 = AsmReadMsr64 (0xC001001D);
  IDS_HDT_CONSOLE (CPU_TRACE, "  TOM: %lX, TOM2: %lX, \n  Pcie configuration space: %lX ~ %lX\n", TOM, TOM2, PcdGet64 (PcdPciExpressBaseAddress), (PcdGet64 (PcdPciExpressBaseAddress) + PciCfgSpace));

  ASSERT (PcdGet32 (PcdHygonBottomMmioReservedForPrimaryRb) >= (PcdGet64 (PcdPciExpressBaseAddress) + PciCfgSpace));
  ASSERT (PcdGet64 (PcdPciExpressBaseAddress) >= TOM);

  // Calculate size of above 4G
  MmioBaseAddrNextRb = (TOM2 > 0x100000000)? TOM2 : 0x100000000; // Check if TOM2 > 4G

  if (!PcdGetBool (PcdHygonHgpiSmee)) {    //SMEE disable
      MmioLimitAbove4G = (UINT64) 1 << PcdGet8 (PcdHygonAbove4GMmioLimitBit);
  } else {                                
      if (PcdGet8 (PcdHygonAbove4GMmioLimitBit) > 45) {  //SMEE enable MMIO address is 45bit or below 45bit
        MmioLimitAbove4G = (UINT64) 1 << 45; 
      } else {
        MmioLimitAbove4G = (UINT64) 1 << PcdGet8 (PcdHygonAbove4GMmioLimitBit);
      }
  }
  MmioLimitAbove4G -= 0x300000000; //MMIO limit - 12G

  if (PcdGet64 (PcdHygonMmioAbove4GLimit) < MmioLimitAbove4G) {
    MmioLimitAbove4G = (PcdGet64 (PcdHygonMmioAbove4GLimit) + 1) & 0xFFFFFFFFFFFF0000;
    if (MmioLimitAbove4G <= MmioBaseAddrNextRb) {
      MmioLimitAbove4G = MmioBaseAddrNextRb;
    }
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "    Above 4G MMIO base is %lX, limit is %lX\n", MmioBaseAddrNextRb, MmioLimitAbove4G);

  LargeAlignFirst    = TRUE;
  EnoughSpaceAbove4G = TRUE;
  for (i = 0; i < SocketNumber; i++) {
    for (j = 0; j < IodNumberPerSocket; j++) {
      for (k = 0; k < RbNumberPerIod; k++) {
        // Calculate reqiured size, it's for output parameter 'SpaceStatus'
        if (SpaceStatus != NULL) {
          SpaceStatus->MmioSizeAbove4G += MmioSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Size + MmioSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Size;
          SpaceStatus->MmioSizeBelow4G += (UINT32)(MmioSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Size + MmioSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Size);
        }

        // If there's no MMIO request for above 4G, try next one
        if ((MmioSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Size + MmioSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Size) == 0) {
          continue;
        }

        AlignMask    = MmioSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Alignment;
        AlignMaskP   = MmioSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Alignment;
        MmioBaseAddr = MmioBaseAddrNextRb;
        if ((LargeAlignFirst && (MmioSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Alignment >= MmioSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Alignment)) ||
            ((!LargeAlignFirst) && (MmioSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Alignment <= MmioSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Alignment))) {
          // Prefetchable -> Non Prefetchable
          MmioBaseAddrPrefetchable    = (MmioBaseAddr + AlignMaskP) & (~AlignMaskP);
          MmioBaseAddrNonPrefetchable = (MmioBaseAddrPrefetchable + MmioSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Size + AlignMask) & (~AlignMask);
          SizePrefetchable    = MmioBaseAddrNonPrefetchable - MmioBaseAddrPrefetchable;
          SizeNonPrefetchable = MmioSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Size;
          MmioBaseAddrNextRb  = MmioBaseAddrNonPrefetchable + SizeNonPrefetchable;
        } else {
          // Non Prefetchable -> Prefetchable
          MmioBaseAddrNonPrefetchable = (MmioBaseAddr + AlignMask) & (~AlignMask);
          MmioBaseAddrPrefetchable    = (MmioBaseAddrNonPrefetchable + MmioSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Size + AlignMaskP) & (~AlignMaskP);
          SizeNonPrefetchable = MmioBaseAddrPrefetchable - MmioBaseAddrNonPrefetchable;
          SizePrefetchable    = MmioSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Size;
          MmioBaseAddrNextRb  = MmioBaseAddrPrefetchable + SizePrefetchable;
        }

        LargeAlignFirst = !LargeAlignFirst;

        if (SizeNonPrefetchable == 0) {
          MmioBaseAddrNonPrefetchable = 0;
        }

        if (SizePrefetchable == 0) {
          MmioBaseAddrPrefetchable = 0;
        }

        // Check if space is enough
        if (MmioBaseAddrNextRb > MmioLimitAbove4G) {
          EnoughSpaceAbove4G = FALSE;
          IDS_HDT_CONSOLE (CPU_TRACE, "  No enough size above 4G\n");
          Status = EFI_OUT_OF_RESOURCES;
        }

        if (SetDfRegisters && EnoughSpaceAbove4G && (FabricMmioManager != NULL)) {
          // Set MMIO above 4G
          if ((MmioSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Size == 0) &&
              (MmioSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Size == 0)) {
            continue;
          }

          IDS_HDT_CONSOLE (CPU_TRACE, "  User Request above 4G: Socket %d Rb %d Prefetch MMIO Size %lX, AlignBit %X\n", i, j, MmioSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Size, MmioSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Alignment);
          IDS_HDT_CONSOLE (CPU_TRACE, "                           Non Prefetch MMIO Size %lX, AlignBit %X\n", MmioSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Size, MmioSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Alignment);

          DstFabricID = (UINT16)FabricTopologyGetHostBridgeSystemFabricID (i, j, k);
          if ((i == (SocketNumber - 1)) && (j == (IodNumberPerSocket - 1)) && (k == (RbNumberPerIod - 1))) {
            // workaround to set last root bridge's MMIO limit to 0xFFFF_FFFF_FFFF
            FabricStSetMmioReg (
              SocketNumber,
              IodNumberPerSocket,
              ((i * IodNumberPerSocket * RbNumberPerIod + j * RbNumberPerIod + k) * 2 + 1),
              DstFabricID,
              MmioBaseAddr,
              (0x1000000000000 - MmioBaseAddr)
              );
          } else {
            FabricStSetMmioReg (
              SocketNumber,
              IodNumberPerSocket,
              ((i * IodNumberPerSocket * RbNumberPerIod + j * RbNumberPerIod + k) * 2 + 1),
              DstFabricID,
              MmioBaseAddr,
              (MmioBaseAddrNextRb - MmioBaseAddr)
              );
          }

          FabricMmioManager->AllocateMmioAbove4GOnThisRb[i][j][k]           = TRUE;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].BaseNonPci          = 0; // don't have Non Pci MMIO for above 4G
          FabricMmioManager->MmioRegionAbove4G[i][j][k].BasePrefetch        = MmioBaseAddrPrefetchable;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].BaseNonPrefetch     = MmioBaseAddrNonPrefetchable;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].SizeNonPci          = 0;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].SizePrefetch        = SizePrefetchable;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].SizeNonPrefetch     = SizeNonPrefetchable;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].UsedSizeNonPci      = 0;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].UsedSizePrefetch    = 0;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].UsedSizeNonPrefetch = 0;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].AlignNonPrefetch    = MmioSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Alignment;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].AlignPrefetch       = MmioSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Alignment;

          IDS_HDT_CONSOLE (CPU_TRACE, "    BasePrefetch    0x%lX, SizePrefetch    0x%lX\n", FabricMmioManager->MmioRegionAbove4G[i][j][k].BasePrefetch, FabricMmioManager->MmioRegionAbove4G[i][j][k].SizePrefetch);
          IDS_HDT_CONSOLE (CPU_TRACE, "    BaseNonPrefetch 0x%lX, SizeNonPrefetch 0x%lX\n", FabricMmioManager->MmioRegionAbove4G[i][j][k].BaseNonPrefetch, FabricMmioManager->MmioRegionAbove4G[i][j][k].SizeNonPrefetch);
        }
      }
    }
  }

  // Calculate size of below 4G
  MmioBaseAddrAbovePcieCfg = PcdGet64 (PcdPciExpressBaseAddress) + PciCfgSpace;
  MmioBaseAddrBelowPcieCfg = TOM;
  MmioLimitAbovePcieCfg    = PcdGet32 (PcdHygonBottomMmioReservedForPrimaryRb);
  MmioLimitBelowPcieCfg    = PcdGet64 (PcdPciExpressBaseAddress);

  LastCombinationWork = FALSE;
  if (FabricGetResourceDistribution (&MmioIsAbovePcieCfg[0]) == EFI_SUCCESS) {
    // Get distribution information from NV, try it first
    if (TryThisCombination (
          MmioSizeForEachRb,
          MmioBaseAddrAbovePcieCfg,
          MmioLimitAbovePcieCfg,
          MmioBaseAddrBelowPcieCfg,
          MmioLimitBelowPcieCfg,
          MmioIsAbovePcieCfg,
          SocketNumber,
          IodNumberPerSocket,
          RbNumberPerIod,
          FabricMmioManager,
          SetDfRegisters
          )) {
      // It works! No need to find out a new combination that which root bridge is above Pcie Cfg
      IDS_HDT_CONSOLE (CPU_TRACE, "  Use combination in NV\n");
      LastCombinationWork = TRUE;
    }
  }

  if (!LastCombinationWork) {
    if (ArrangeMmioBelow4G (
          MmioSizeForEachRb,
          MmioBaseAddrAbovePcieCfg,
          MmioLimitAbovePcieCfg,
          MmioBaseAddrBelowPcieCfg,
          MmioLimitBelowPcieCfg,
          MmioIsAbovePcieCfg,
          SocketNumber,
          IodNumberPerSocket,
          RbNumberPerIod,
          FabricMmioManager,
          SetDfRegisters
          )) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  Save combination to NV\n");
      FabricSetResourceDistribution (&MmioIsAbovePcieCfg[0]);
    } else {
      IDS_HDT_CONSOLE (CPU_TRACE, "  No enough size below 4G\n");
      Status = EFI_OUT_OF_RESOURCES;
    }
  }

  if (SpaceStatus != NULL) {
    ASSERT ((mOverSizeBelowPcieMin == 0) || (mOverSizeAbovePcieMin == 0));
    SpaceStatus->MmioSizeAbove4GReqInc = (MmioBaseAddrNextRb > MmioLimitAbove4G) ? (MmioBaseAddrNextRb - MmioLimitAbove4G) : 0;
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
 * @param[in]         MmioSizeForEachRb           Required MMIO size for each Rb
 * @param[in]         MmioBaseAddrAbovePcieCfg    MmioBaseAddrAbovePcieCfg
 * @param[in]         MmioLimitAbovePcieCfg       MmioLimitAbovePcieCfg
 * @param[in]         MmioBaseAddrBelowPcieCfg    MmioBaseAddrBelowPcieCfg
 * @param[in]         MmioLimitBelowPcieCfg       MmioLimitBelowPcieCfg
 * @param[in, out]    MmioIsAbovePcieCfg          An BOOLEAN array, indicate which root bridge's MMIO is above Pcie Cfg
 * @param[in]         SocketNumber                System socket count
 * @param[in]         RbNumberPerSocket           Rb number per socket
 * @param[in]         FabricMmioManager           Point to FABRIC_MMIO_MANAGER
 * @param[in]         SetDfRegisters              TRUE  - Set DF MMIO registers
 *                                                FALSE - Not set DF MMIO registers, just calculate if user's requirment could be satisfied.
 *
 * @retval            TRUE
 *                    FALSE
 */
BOOLEAN
ArrangeMmioBelow4G (
  IN       FABRIC_RESOURCE_FOR_EACH_RB  *MmioSizeForEachRb,
  IN       UINT64                         MmioBaseAddrAbovePcieCfg,
  IN       UINT64                         MmioLimitAbovePcieCfg,
  IN       UINT64                         MmioBaseAddrBelowPcieCfg,
  IN       UINT64                         MmioLimitBelowPcieCfg,
  IN OUT   BOOLEAN                       *MmioIsAbovePcieCfg,
  IN       UINT8                          SocketNumber,
  IN       UINT8                          IodNumberPerSocket,
  IN       UINT8                          RbNumberPerIod,
  IN       FABRIC_MMIO_MANAGER           *FabricMmioManager,
  IN       BOOLEAN                        SetDfRegisters
  )
{
  UINT8    NumberOfRbBelowPcieCfg;
  UINT8    TotalRbNumber;
  BOOLEAN  GetAnCombination;
  BOOLEAN  NextCombination;

  GetAnCombination = FALSE;

  TotalRbNumber = SocketNumber * IodNumberPerSocket * RbNumberPerIod;

  // Try to put 0 Rb, 1 Rb, 2 Rbs... ST_MAX_SYSTEM_RB_COUNT Rbs below Pcie Cfg
  for (NumberOfRbBelowPcieCfg = 0; NumberOfRbBelowPcieCfg <= TotalRbNumber; NumberOfRbBelowPcieCfg++) {
    // 1. Default, all Rbs are above Pcie Cfg
    LibHygonMemFill (MmioIsAbovePcieCfg, TRUE, (sizeof (BOOLEAN) * MAX_SOCKETS_SUPPORTED * MAX_IOD_PER_SOCKET * MAX_RBS_PER_IOD), NULL);
    NextCombination = TRUE;

    // 2. First, try to put Rb 7 to Rb (ST_MAX_SYSTEM_RB_COUNT - NumberOfRbBelowPcieCfg) below Pcie Cfg
    if (NumberOfRbBelowPcieCfg > 0 ) {
      LibHygonMemFill ((MmioIsAbovePcieCfg + TotalRbNumber - NumberOfRbBelowPcieCfg), FALSE, (sizeof (BOOLEAN) * NumberOfRbBelowPcieCfg), NULL);
    }

    while (NextCombination) {
      // 3. Try this combination
      GetAnCombination = TryThisCombination (
                           MmioSizeForEachRb,
                           MmioBaseAddrAbovePcieCfg,
                           MmioLimitAbovePcieCfg,
                           MmioBaseAddrBelowPcieCfg,
                           MmioLimitBelowPcieCfg,
                           MmioIsAbovePcieCfg,
                           SocketNumber,
                           IodNumberPerSocket,
                           RbNumberPerIod,
                           FabricMmioManager,
                           FALSE
                           );
      if (GetAnCombination) {
        break;
      }

      // 4. If we can't make it, try to another combination
      NextCombination = GetNextCombination (NumberOfRbBelowPcieCfg, MmioIsAbovePcieCfg, SocketNumber, IodNumberPerSocket, RbNumberPerIod);
    }

    if ((GetAnCombination) || (MmioLimitBelowPcieCfg == MmioBaseAddrBelowPcieCfg)) {
      // If we already got an combination or there's no space above Pcie Cfg, then break
      break;
    }
  }

  if (GetAnCombination) {
    TryThisCombination (
      MmioSizeForEachRb,
      MmioBaseAddrAbovePcieCfg,
      MmioLimitAbovePcieCfg,
      MmioBaseAddrBelowPcieCfg,
      MmioLimitBelowPcieCfg,
      MmioIsAbovePcieCfg,
      SocketNumber,
      IodNumberPerSocket,
      RbNumberPerIod,
      FabricMmioManager,
      SetDfRegisters
      );
  }

  return GetAnCombination;
}

/*---------------------------------------------------------------------------------------*/

/**
 * Get a different combination
 *
 *
 * @param[in]         NumberOfRbBelowPcieCfg      How many Rbs MMIO should be put below Pcie Cfg
 * @param[in, out]    MmioIsAbovePcieCfg          An BOOLEAN array, indicate which Rb's MMIO is above Pcie Cfg
 * @param[in]         SocketNumber                System socket count
 * @param[in]         RbNumberPerSocket           Rb number per socket
 *
 * @retval            TRUE                        Get a combination
 *                    FALSE                       All combinations have been tried
 */
BOOLEAN
GetNextCombination (
  IN       UINT8                          NumberOfRbBelowPcieCfg,
  IN OUT   BOOLEAN                       *MmioIsAbovePcieCfg,
  IN       UINT8                          SocketNumber,
  IN       UINT8                          IodNumberPerSocket,
  IN       UINT8                          RbNumberPerIod
  )
{
  UINT8  MoveThisOne;
  UINT8  ResetTheseRb;
  UINT8  TotalRbNumber;

  TotalRbNumber = SocketNumber * IodNumberPerSocket * RbNumberPerIod;

  // From Rb 0, there're total number of 'ResetTheseRb' sequential Rb, whose MMIO is below Pcie Cfg
  for (ResetTheseRb = 0; ResetTheseRb < NumberOfRbBelowPcieCfg; ResetTheseRb++) {
    if (*(MmioIsAbovePcieCfg + ResetTheseRb) == TRUE) {
      break;
    }
  }

  // if ResetTheseRb == NumberOfRbBelowPcieCfg
  // All combinations have been tried.
  if (ResetTheseRb == NumberOfRbBelowPcieCfg) {
    return FALSE;
  }

  // After number of ResetTheseRb RBs, find out the first RB whose MMIO is below Pcie Cfg.
  for (MoveThisOne = ResetTheseRb + 1; MoveThisOne < TotalRbNumber; MoveThisOne++) {
    if (*(MmioIsAbovePcieCfg + MoveThisOne) == FALSE) {
      break;
    }
  }

  ASSERT (MoveThisOne < TotalRbNumber);
  *(MmioIsAbovePcieCfg + MoveThisOne)     = TRUE;
  *(MmioIsAbovePcieCfg + MoveThisOne - 1) = FALSE;
  LibHygonMemFill (MmioIsAbovePcieCfg, TRUE, (sizeof (BOOLEAN) * ResetTheseRb), NULL);
  LibHygonMemFill ((MmioIsAbovePcieCfg + MoveThisOne - 1 - ResetTheseRb), FALSE, (sizeof (BOOLEAN) * ResetTheseRb), NULL);

  return TRUE;
}

/*---------------------------------------------------------------------------------------*/

/**
 * If there's enough space for current combination
 *
 *
 * @param[in]         MmioSizeForEachRb           Required MMIO size for each Rb
 * @param[in]         MmioBaseAddrAbovePcieCfg    MmioBaseAddrAbovePcieCfg
 * @param[in]         MmioLimitAbovePcieCfg       MmioLimitAbovePcieCfg
 * @param[in]         MmioBaseAddrBelowPcieCfg    MmioBaseAddrBelowPcieCfg
 * @param[in]         MmioLimitBelowPcieCfg       MmioLimitBelowPcieCfg
 * @param[in, out]    MmioIsAbovePcieCfg          An BOOLEAN array, indicate which Rb's MMIO is above Pcie Cfg
 * @param[in]         SocketNumber                System socket count
 * @param[in]         RbNumberPerSocket           Rb number per socket
 * @param[in]         FabricMmioManager           Point to FABRIC_MMIO_MANAGER
 * @param[in]         SetDfRegisters              TRUE  - Set DF MMIO registers
 *                                                FALSE - Not set DF MMIO registers, just calculate if user's requirment could be satisfied.
 *
 * @retval            TRUE
 *                    FALSE
 */
BOOLEAN
TryThisCombination (
  IN       FABRIC_RESOURCE_FOR_EACH_RB   *MmioSizeForEachRb,
  IN       UINT64                         MmioBaseAddrAbovePcieCfg,
  IN       UINT64                         MmioLimitAbovePcieCfg,
  IN       UINT64                         MmioBaseAddrBelowPcieCfg,
  IN       UINT64                         MmioLimitBelowPcieCfg,
  IN OUT   BOOLEAN                       *MmioIsAbovePcieCfg,
  IN       UINT8                          SocketNumber,
  IN       UINT8                          IodNumberPerSocket,
  IN       UINT8                          RbNumberPerIod,
  IN       FABRIC_MMIO_MANAGER           *FabricMmioManager,
  IN       BOOLEAN                        SetDfRegisters
  )
{
  UINT8    i;
  UINT8    j;
  UINT8    k;
  UINT8    SocketLoop;
  UINT8    DieLoop;
  UINT8    RbLoop;
  UINT16   DstFabricID;
  UINT64   MmioBaseAddr; // To caculate oversize, we must use UINT64 here for all address, size
  UINT64   MmioCeiling;
  UINT64   MmioBaseAddrPrefetchable;
  UINT64   MmioBaseAddrNonPrefetchable;
  UINT64   MmioBaseAddrNonPci;
  UINT64   SizePrefetchable;
  UINT64   SizeNonPrefetchable;
  UINT64   SizeNonPci;
  UINT64   AlignMask;
  UINT64   AlignMaskP;
  UINT64   AlignMaskNonPci;
  UINT64   BottomOfCompat;
  UINT64   OverSizeAbovePcieCfg;
  UINT64   OverSizeBelowPcieCfg;
  UINT64   AlignForFirstMmioRegionAbovePcieCfg;
  UINT64   AlignForFirstMmioRegionBelowPcieCfg;
  BOOLEAN  AlreadyGotAlignForFirstMmioRegionAbovePcieCfg;
  BOOLEAN  AlreadyGotAlignForFirstMmioRegionBelowPcieCfg;
  BOOLEAN  BigAlignFirstAbovePcieCfg;
  BOOLEAN  BigAlignFirstBelowPcieCfg;
  BOOLEAN  BigAlignFirst;
  BOOLEAN  ReservedRegionAlreadySet;
  BOOLEAN  OverSizeFlag;

  BigAlignFirstAbovePcieCfg = TRUE;
  BigAlignFirstBelowPcieCfg = TRUE;
  OverSizeFlag = FALSE;
  AlreadyGotAlignForFirstMmioRegionAbovePcieCfg = FALSE;
  AlreadyGotAlignForFirstMmioRegionBelowPcieCfg = FALSE;
  SizeNonPci = (PcdGet32 (PcdHygonMmioSizePerRbForNonPciDevice) > MMIO_MIN_NON_PCI_SIZE) ? PcdGet32 (PcdHygonMmioSizePerRbForNonPciDevice) : MMIO_MIN_NON_PCI_SIZE;
  AlignMaskNonPci = ST_NON_PCI_MMIO_ALIGN_MASK;
  AlignForFirstMmioRegionAbovePcieCfg = 0;
  AlignForFirstMmioRegionBelowPcieCfg = 0;

  BottomOfCompat = BOTTOM_OF_COMPAT;
  /// @todo 0xFED0_0000 ~ 0xFED0_FFFF should be POSTED
  /// @todo BottomOfCompat = ADDITIONAL_POSTED_REGION_UNDER_PRIMARY_RB_END + 1;
  ReservedRegionAlreadySet = FALSE;  // Indicate if PcdHygonBottomMmioReservedForPrimaryRb ~ BottomOfCompat is set

  for (i = SocketNumber; i > 0; i--) {
    for (j = IodNumberPerSocket; j > 0; j--) {
      for (k = RbNumberPerIod; k > 0; k--) {
        SocketLoop = i - 1;
        DieLoop    = j - 1;
        RbLoop     = k - 1;

        if ((MmioSizeForEachRb->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Size + MmioSizeForEachRb->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Size + SizeNonPci) == 0) {
          continue;
        }

        AlignMask  = MmioSizeForEachRb->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Alignment;
        AlignMaskP = MmioSizeForEachRb->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Alignment;

        if (*(MmioIsAbovePcieCfg + SocketLoop * IodNumberPerSocket * RbNumberPerIod + DieLoop * RbNumberPerIod + RbLoop)) {
          // Mmio is above PcieCfg
          BigAlignFirst = BigAlignFirstAbovePcieCfg;
          MmioBaseAddr  = MmioBaseAddrAbovePcieCfg;
          BigAlignFirstAbovePcieCfg = !BigAlignFirstAbovePcieCfg;

          if (!AlreadyGotAlignForFirstMmioRegionAbovePcieCfg) {
            // Save alignment for the first MMIO region, it will be used for calculate oversize
            AlignForFirstMmioRegionAbovePcieCfg = (AlignMask > AlignMaskP) ? AlignMask : AlignMaskP;
            AlreadyGotAlignForFirstMmioRegionAbovePcieCfg = TRUE;
          }
        } else {
          // Mmio is below PcieCfg
          BigAlignFirst = BigAlignFirstBelowPcieCfg;
          MmioBaseAddr  = MmioBaseAddrBelowPcieCfg;
          BigAlignFirstBelowPcieCfg = !BigAlignFirstBelowPcieCfg;

          if (!AlreadyGotAlignForFirstMmioRegionBelowPcieCfg) {
            // Save alignment for the first MMIO region, it will be used for calculate oversize
            AlignForFirstMmioRegionBelowPcieCfg = (AlignMask > AlignMaskP) ? AlignMask : AlignMaskP;
            AlreadyGotAlignForFirstMmioRegionBelowPcieCfg = TRUE;
          }
        }

        if (BigAlignFirst) {
          if (MmioSizeForEachRb->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Alignment >= MmioSizeForEachRb->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Alignment) {
            // Prefetchable -> Non Prefetchable -> Non Pci
            MmioBaseAddrPrefetchable    = (MmioBaseAddr + AlignMaskP) & (~AlignMaskP);
            MmioBaseAddrNonPrefetchable = (MmioBaseAddrPrefetchable + MmioSizeForEachRb->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Size + AlignMask) & (~AlignMask);
            MmioBaseAddrNonPci  = (MmioBaseAddrNonPrefetchable + MmioSizeForEachRb->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Size + AlignMaskNonPci) & (~AlignMaskNonPci);
            SizePrefetchable    = MmioBaseAddrNonPrefetchable - MmioBaseAddrPrefetchable;
            SizeNonPrefetchable = MmioBaseAddrNonPci - MmioBaseAddrNonPrefetchable;
            MmioCeiling = MmioBaseAddrNonPci + SizeNonPci;
          } else {
            // Non Prefetchable -> Prefetchable -> Non Pci
            MmioBaseAddrNonPrefetchable = (MmioBaseAddr + AlignMask) & (~AlignMask);
            MmioBaseAddrPrefetchable    = (MmioBaseAddrNonPrefetchable + MmioSizeForEachRb->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Size + AlignMaskP) & (~AlignMaskP);
            MmioBaseAddrNonPci  = (MmioBaseAddrPrefetchable + MmioSizeForEachRb->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Size + AlignMaskNonPci) & (~AlignMaskNonPci);
            SizeNonPrefetchable = MmioBaseAddrPrefetchable - MmioBaseAddrNonPrefetchable;
            SizePrefetchable    = MmioBaseAddrNonPci - MmioBaseAddrPrefetchable;
            MmioCeiling = MmioBaseAddrNonPci + SizeNonPci;
          }
        } else {
          if (MmioSizeForEachRb->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Alignment <= MmioSizeForEachRb->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Alignment) {
            // Non Pci -> Prefetchable -> Non Prefetchable
            MmioBaseAddrNonPci = (MmioBaseAddr + AlignMaskNonPci) & (~AlignMaskNonPci);
            MmioBaseAddrPrefetchable    = (MmioBaseAddrNonPci + SizeNonPci + AlignMaskP) & (~AlignMaskP);
            MmioBaseAddrNonPrefetchable = (MmioBaseAddrPrefetchable + MmioSizeForEachRb->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Size + AlignMask) & (~AlignMask);
            SizePrefetchable    = MmioBaseAddrNonPrefetchable - MmioBaseAddrPrefetchable;
            SizeNonPrefetchable = MmioSizeForEachRb->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Size;
            MmioCeiling = MmioBaseAddrNonPrefetchable + SizeNonPrefetchable;
          } else {
            // Non Pci -> Non Prefetchable -> Prefetchable
            MmioBaseAddrNonPci = (MmioBaseAddr + AlignMaskNonPci) & (~AlignMaskNonPci);
            MmioBaseAddrNonPrefetchable = (MmioBaseAddrNonPci + SizeNonPci + AlignMask) & (~AlignMask);
            MmioBaseAddrPrefetchable    = (MmioBaseAddrNonPrefetchable + MmioSizeForEachRb->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Size + AlignMaskP) & (~AlignMaskP);
            SizeNonPrefetchable = MmioBaseAddrPrefetchable - MmioBaseAddrNonPrefetchable;
            SizePrefetchable    = MmioSizeForEachRb->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Size;
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
        if (*(MmioIsAbovePcieCfg + SocketLoop * IodNumberPerSocket * RbNumberPerIod + DieLoop * RbNumberPerIod + RbLoop)) {
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
          IDS_HDT_CONSOLE (CPU_TRACE, "  User Request below 4G: Socket %d Iod %d Rb %d Prefetch MMIO Size %lX, AlignBit %X\n", SocketLoop, DieLoop, RbLoop, MmioSizeForEachRb->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Size, MmioSizeForEachRb->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Alignment);
          IDS_HDT_CONSOLE (CPU_TRACE, "                           Non Prefetch MMIO Size %lX, AlignBit %X\n", MmioSizeForEachRb->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Size, MmioSizeForEachRb->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Alignment);
          ASSERT (BottomOfCompat >= PcdGet32 (PcdHygonBottomMmioReservedForPrimaryRb));

          DstFabricID = (UINT16)FabricTopologyGetHostBridgeSystemFabricID (SocketLoop, DieLoop, RbLoop);
          if ((SocketLoop == 0) && (DieLoop == 0) && (RbLoop == 0)) {
            if (*(MmioIsAbovePcieCfg + SocketLoop * IodNumberPerSocket * RbNumberPerIod + DieLoop * RbNumberPerIod + RbLoop)) {
              FabricStSetMmioReg (
                SocketNumber,
                IodNumberPerSocket,
                ((SocketLoop * IodNumberPerSocket * RbNumberPerIod + DieLoop * RbNumberPerIod + RbLoop) * 2),
                DstFabricID,
                MmioBaseAddr,
                (BottomOfCompat - MmioBaseAddr)
                );

              MmioBaseAddrAbovePcieCfg = BottomOfCompat;
              ReservedRegionAlreadySet = TRUE;
            } else {
              FabricStSetMmioReg (
                SocketNumber,
                IodNumberPerSocket,
                ((SocketLoop * IodNumberPerSocket * RbNumberPerIod + DieLoop * RbNumberPerIod + RbLoop) * 2),
                DstFabricID,
                MmioBaseAddr,
                (MmioLimitBelowPcieCfg - MmioBaseAddr)
                );

              MmioBaseAddrBelowPcieCfg = MmioLimitBelowPcieCfg;
            }
          } else {
            FabricStSetMmioReg (
              SocketNumber,
              IodNumberPerSocket,
              ((SocketLoop * IodNumberPerSocket * RbNumberPerIod + DieLoop * RbNumberPerIod + RbLoop) * 2),
              DstFabricID,
              MmioBaseAddr,
              (MmioCeiling - MmioBaseAddr)
              );
          }

          FabricMmioManager->AllocateMmioBelow4GOnThisRb[SocketLoop][DieLoop][RbLoop]           = TRUE;
          FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop][RbLoop].BaseNonPci          = MmioBaseAddrNonPci;
          FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop][RbLoop].BasePrefetch        = MmioBaseAddrPrefetchable;
          FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop][RbLoop].BaseNonPrefetch     = MmioBaseAddrNonPrefetchable;
          FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop][RbLoop].SizeNonPci          = SizeNonPci;
          FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop][RbLoop].SizePrefetch        = SizePrefetchable;
          FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop][RbLoop].SizeNonPrefetch     = SizeNonPrefetchable;
          FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop][RbLoop].UsedSizeNonPci      = 0;
          FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop][RbLoop].UsedSizePrefetch    = 0;
          FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop][RbLoop].UsedSizeNonPrefetch = 0;
          FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop][RbLoop].AlignNonPrefetch    = MmioSizeForEachRb->NonPrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Alignment;
          FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop][RbLoop].AlignPrefetch       = MmioSizeForEachRb->PrefetchableMmioSizeBelow4G[SocketLoop][DieLoop][RbLoop].Alignment;

          IDS_HDT_CONSOLE (
            CPU_TRACE,
            "    BasePrefetch    0x%lX, SizePrefetch    0x%lX\n",
            FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop][RbLoop].BasePrefetch,
            FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop][RbLoop].SizePrefetch
            );

          IDS_HDT_CONSOLE (
            CPU_TRACE,
            "    BaseNonPrefetch 0x%lX, SizeNonPrefetch 0x%lX\n",
            FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop][RbLoop].BaseNonPrefetch,
            FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop][RbLoop].SizeNonPrefetch
            );

          IDS_HDT_CONSOLE (
            CPU_TRACE,
            "    BaseNonPci      0x%lX, SizeNonPci      0x%lX\n",
            FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop][RbLoop].BaseNonPci,
            FabricMmioManager->MmioRegionBelow4G[SocketLoop][DieLoop][RbLoop].SizeNonPci
            );
        }
      }
    }

    // If SetDfRegisters if FALSE, this function is called by FabricReallocateResourceForEachRb
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
            mOverSizeAbovePcieMin = (UINT32)OverSizeAbovePcieCfg;
            mOverSizeBelowPcieMin = (UINT32)OverSizeBelowPcieCfg;
          }

          if (OverSizeAbovePcieCfg != 0) {
            mAlignmentMask = (UINT32)AlignForFirstMmioRegionAbovePcieCfg;
          } else {
            mAlignmentMask = (UINT32)AlignForFirstMmioRegionBelowPcieCfg;
          }
        }
      } else {
        mOverSizeAbovePcieMin = 0;
        mOverSizeBelowPcieMin = 0;
      }
    }
  }

  return (!OverSizeFlag);
}

/*---------------------------------------------------------------------------------------*/

/**
 * FabricStSetMmioReg
 *
 * Set MMIO register pairs according to input parameters
 *
 * @param[in]         TotalSocket                 System socket count
 * @param[in]         IodNumberPerSocket          IOD number per socket
 * @param[in]         MmioPairIndex               Which MMIO register pair should be set
 * @param[in]         DstFabricID                 IOMS Fabric ID
 * @param[in]         BaseAddress                 Base address of MMIO region
 * @param[in]         Length                      Length of MMIO region
 *
 */
VOID
FabricStSetMmioReg (
  IN       UINT8  TotalSocket,
  IN       UINT8  IodNumberPerSocket,
  IN       UINT8  MmioPairIndex,
  IN       UINT16 DstFabricID,
  IN       UINT64 BaseAddress,
  IN       UINT64 Length
  )
{
  UINTN           i, j, k;
  UINTN           CddsPresent, CddId;
  MMIO_ADDR_CTRL  MmioAddrCtrlReg;
  UINT64          Value64;

  //ASSERT (Length >= ST_MMIO_MIN_SIZE);
  Value64 = Length > 0 ? Length : 1;
  BaseAddress = (BaseAddress + 0xFFFF) & 0xFFFFFFFFFFFF0000;

  CddId = FabricTopologyGetFirstPhysCddIdOnSocket (0);
  MmioAddrCtrlReg.Value    = CddFabricRegisterAccRead (0, CddId, MMIO_SPACE_FUNC, (MMIO_ADDRESS_CONTROL_REG_0 + MmioPairIndex * 0x0C), FABRIC_REG_ACC_BC);
  if (Length > 0) {
    MmioAddrCtrlReg.Field.RE = 1;
    MmioAddrCtrlReg.Field.WE = 1;
  } else {
    MmioAddrCtrlReg.Field.RE = 0;
    MmioAddrCtrlReg.Field.WE = 0;
  }
  MmioAddrCtrlReg.Field.DstFabricID = DstFabricID;

  for (i = 0; i < TotalSocket; i++) {
    // Write IO Die
    for (k = 0; k < IodNumberPerSocket; k++) {
      FabricRegisterAccWrite (i, k, MMIO_SPACE_FUNC, (MMIO_BASE_ADDRESS_REG_0  + MmioPairIndex * 0x0C), FABRIC_REG_ACC_BC, (UINT32)(BaseAddress >> 16), TRUE);
      FabricRegisterAccWrite (i, k, MMIO_SPACE_FUNC, (MMIO_LIMIT_ADDRESS_REG_0 + MmioPairIndex * 0x0C), FABRIC_REG_ACC_BC, (UINT32)((BaseAddress + Value64 - 1) >> 16), TRUE);
      FabricRegisterAccWrite (i, k, MMIO_SPACE_FUNC, (MMIO_ADDRESS_CONTROL_REG_0 + MmioPairIndex * 0x0C), FABRIC_REG_ACC_BC, MmioAddrCtrlReg.Value, TRUE);
    }

    // Write CDD
    CddsPresent = FabricTopologyGetCddsPresentOnSocket (i);
    for (j = 0; j < MAX_CDDS_PER_SOCKET; j++) {
      if (!IS_CDD_PRESENT (j, CddsPresent)) {
        continue;
      }

      CddFabricRegisterAccWrite (i, j, MMIO_SPACE_FUNC, (MMIO_BASE_ADDRESS_REG_0  + MmioPairIndex * 0x0C), FABRIC_REG_ACC_BC, (UINT32)(BaseAddress >> 16), TRUE);
      CddFabricRegisterAccWrite (i, j, MMIO_SPACE_FUNC, (MMIO_LIMIT_ADDRESS_REG_0 + MmioPairIndex * 0x0C), FABRIC_REG_ACC_BC, (UINT32)((BaseAddress + Value64 - 1) >> 16), TRUE);
      CddFabricRegisterAccWrite (i, j, MMIO_SPACE_FUNC, (MMIO_ADDRESS_CONTROL_REG_0 + MmioPairIndex * 0x0C), FABRIC_REG_ACC_BC, MmioAddrCtrlReg.Value, TRUE);
    }
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "  HGPI set MMIO pair #%X, 0x%X0000 ~ 0x%XFFFF DstFabricID: 0x%X\n", MmioPairIndex, (UINT32)(BaseAddress >> 16), (UINT32)((BaseAddress + Value64 - 1) >> 16), MmioAddrCtrlReg.Field.DstFabricID);
}

/*---------------------------------------------------------------------------------------*/

/**
 * FabricStSetIoReg
 *
 * Set IO register pairs according to input parameters
 *
 * @param[in]         TotalSocket                 System socket count
 * @param[in]         IodNumberPerSocket          Iod number per socket
 * @param[in]         RegIndex                    Which IO register should be set
 * @param[in]         DstFabricID                 IOMS Fabric ID
 * @param[in]         IoBase                      Base address of IO region
 * @param[in]         IoSize                      Length of IO region
 *
 */
VOID
FabricStSetIoReg (
  IN       UINT8  TotalSocket,
  IN       UINT8  IodNumberPerSocket,
  IN       UINT8  RegIndex,
  IN       UINT16 DstFabricID,
  IN       UINT32 IoBase,
  IN       UINT32 IoSize
  )
{
  UINTN             i, j, k;
  UINTN             CddsPresent;
  X86IO_BASE_ADDR   IoBaseReg;
  X86IO_LIMIT_ADDR  IoLimitReg;

  IoBaseReg.Value  = 0;
  IoLimitReg.Value = 0;

  if ((IoBase % 0x1000 == 0) && 
     ((IoSize % 0x1000 == 0) && (IoSize >= 0x1000)) &&
     ((IoBase + IoSize) <= 0x10000)) {
    //
    // check valid IoBase & IoSize,
    //    must be integer multiple of 0x1000
    //
    IoBaseReg.Field.RE       = 1;
    IoBaseReg.Field.WE       = 1;
    IoBaseReg.Field.IOBase   = (IoBase & 0xFFFFF000) >> 12;
    IoLimitReg.Field.IOLimit = ((IoBase + IoSize - 1) & 0xFFFFF000) >> 12;
  }
  IoLimitReg.Field.DstFabricID = DstFabricID;

  for (i = 0; i < TotalSocket; i++) {
    // Write IO Die
    for (k = 0; k < IodNumberPerSocket; k++) {
      FabricRegisterAccWrite (i, k, 0x0, (X86IO_BASE_ADDRESS_REG0 + RegIndex * 8), FABRIC_REG_ACC_BC, IoBaseReg.Value, TRUE);
      FabricRegisterAccWrite (i, k, 0x0, (X86IO_LIMIT_ADDRESS_REG0 + RegIndex * 8), FABRIC_REG_ACC_BC, IoLimitReg.Value, TRUE);
    }

    // Write CDD
    CddsPresent = FabricTopologyGetCddsPresentOnSocket (i);
    for (j = 0; j < MAX_CDDS_PER_SOCKET; j++) {
      if (!IS_CDD_PRESENT (j, CddsPresent)) {
        continue;
      }

      CddFabricRegisterAccWrite (i, j, 0x0, (X86IO_BASE_ADDRESS_REG0 + RegIndex * 8), FABRIC_REG_ACC_BC, IoBaseReg.Value, TRUE);
      CddFabricRegisterAccWrite (i, j, 0x0, (X86IO_LIMIT_ADDRESS_REG0 + RegIndex * 8), FABRIC_REG_ACC_BC, IoLimitReg.Value, TRUE);
    }
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "  HGPI set IO pair #%X, 0x%X000 ~ 0x%XFFF DstFabricID: 0x%X\n", RegIndex, (UINT32)(IoBase >> 12), (UINT32)((IoBase + IoSize - 1) >> 12), IoLimitReg.Field.DstFabricID);
}
