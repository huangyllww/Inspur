/* $NoKeywords:$ */

/**
 * @file
 *
 * Base Fabric MMIO map manager Lib implementation for DHARMA SAT
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
#include <Library/HygonS3SaveLib.h>
#include <Library/HygonHeapLib.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>
#include <Library/FabricResourceSizeForEachRbLib.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include "FabricResourceManagerStLib.h"
#include <Library/PcdLib.h>

#define FILECODE  LIBRARY_FABRICRESOURCEMANAGERSTLIB_FABRICRESOURCEINITSTLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

DF_IO_RESOURCE DfIoInfoFor16RB[16] = {
    { 0x1000, 0x3FFF }, // 12KB
    { 0x4000, 0x5FFF }, // 8KB
    { 0x5FFF, 0x5FFF },
    { 0x5FFF, 0x5FFF },
    { 0x6000, 0x7FFF }, // 8KB
    { 0x8000, 0x9FFF }, // 8KB
    { 0x9FFF, 0x9FFF },
    { 0x9FFF, 0x9FFF },
    { 0xA000, 0xBFFF }, // 8KB
    { 0xC000, 0xDFFF }, // 8KB
    { 0xDFFF, 0xDFFF },
    { 0xDFFF, 0xDFFF },
    { 0xE000, 0xEFFF }, // 4KB
    { 0xF000, 0xFFFF }, // 4KB
    { 0xF000, 0xF000 },
    { 0xF000, 0xF000 }, //no resource
};

DF_IO_RESOURCE  DfIoInfoFor8RB[8] = {
  { 0x1000, 0x3FFF },
  { 0x4000, 0x5FFF },
  { 0x6000, 0x7FFF },
  { 0x8000, 0x9FFF },
  { 0xA000, 0xBFFF },
  { 0xC000, 0xDFFF },
  { 0xE000, 0xEFFF },
  { 0xF000, 0xFFFF }
};

DF_IO_RESOURCE  DfIoInfoFor4RB[4] = {
  { 0x1000, 0x3FFF },
  { 0x4000, 0x7FFF },
  { 0x8000, 0xBFFF },
  { 0xC000, 0xFFFF },
};

DF_IO_RESOURCE  DfIoInfoFor2RB[2] = {
  { 0x1000, 0x8FFF },
  { 0x9000, 0xFFFF },
};

DF_IO_RESOURCE  DfIoInfoFor1RB[1] = {
  { 0x1000, 0xFFFF },
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/

/**
 * FabricResourceInit
 *
 * Initialize DF resource registers for each Rb.
 *
 */
EFI_STATUS
FabricResourceInit (
  )
{
  UINT8                        SocketNumber;
  UINT8                        IodsPerSocket;
  UINT8                        RbsPerIod;
  BOOLEAN                      SetResourceBaseOnNv;
  EFI_STATUS                   Status;
  EFI_STATUS                   CalledStatus;
  FABRIC_RESOURCE_FOR_EACH_RB  ResourceSizeForEachRb;
  FABRIC_MMIO_MANAGER          *FabricMmioManager;
  FABRIC_IO_MANAGER            *FabricIoManager;
  ALLOCATE_HEAP_PARAMS         AllocateHeapParams;

  Status = EFI_SUCCESS;
  CalledStatus = EFI_SUCCESS;
  FabricMmioManager = NULL;
  FabricIoManager   = NULL;

  SocketNumber  = (UINT8)FabricTopologyGetNumberOfSocketPresent ();
  IodsPerSocket = (UINT8)FabricTopologyGetNumberOfLogicalDiesOnSocket (0);
  RbsPerIod     = (UINT8)FabricTopologyGetNumberOfRootBridgesOnDie (0, 0);

  IDS_HDT_CONSOLE (CPU_TRACE, "  Total Socket Number = %d, Iod Number per socket = %d, Rb Number per Iod = %d \n", SocketNumber, IodsPerSocket, RbsPerIod);

  SetResourceBaseOnNv = FALSE;

  if ((!PcdGetBool (PcdHygonFabricResourceDefaultMap)) && (FabricGetResourceSizeForEachRb (&ResourceSizeForEachRb) == EFI_SUCCESS)) {
    SetResourceBaseOnNv = TRUE;
  }

  AllocateHeapParams.RequestedBufferSize = sizeof (FABRIC_MMIO_MANAGER);
  AllocateHeapParams.BufferHandle = HYGON_MMIO_MANAGER;
  AllocateHeapParams.Persist = HEAP_SYSTEM_MEM;
  if (HeapAllocateBuffer (&AllocateHeapParams, NULL) == HGPI_SUCCESS) {
    FabricMmioManager = (FABRIC_MMIO_MANAGER *)AllocateHeapParams.BufferPtr;
  }

  AllocateHeapParams.RequestedBufferSize = sizeof (FABRIC_IO_MANAGER);
  AllocateHeapParams.BufferHandle = HYGON_IO_MANAGER;
  AllocateHeapParams.Persist = HEAP_SYSTEM_MEM;
  if (HeapAllocateBuffer (&AllocateHeapParams, NULL) == HGPI_SUCCESS) {
    FabricIoManager = (FABRIC_IO_MANAGER *)AllocateHeapParams.BufferPtr;
  }

  if  (FabricMmioManager != NULL) {
    if (SetResourceBaseOnNv) {
      // Get NvVariable successfully, try to init MMIO base on it
      IDS_HDT_CONSOLE (CPU_TRACE, "  Init MMIO base on NV variable\n");
      CalledStatus = FabricStInitMmioBaseOnNvVariable (FabricMmioManager, &ResourceSizeForEachRb, NULL, SocketNumber, IodsPerSocket, RbsPerIod, TRUE);
    }

    if ((CalledStatus != EFI_SUCCESS) || (!SetResourceBaseOnNv)) {
      // Can't get NvVariable or init MMIO base on NvVariable failed
      IDS_HDT_CONSOLE (CPU_TRACE, "  Init MMIO equally\n");
      CalledStatus = FabricStInitMmioEqually (FabricMmioManager, SocketNumber, IodsPerSocket, RbsPerIod);
      Status = (CalledStatus > Status) ? CalledStatus : Status;
    }
  } else {
    Status = EFI_ABORTED;
  }

  if  (FabricIoManager != NULL) {
    if (SetResourceBaseOnNv) {
      // Get NvVariable successfully, try to init IO base on it
      IDS_HDT_CONSOLE (CPU_TRACE, "  Init IO base on NV variable\n");
      CalledStatus = FabricStInitIoBaseOnNvVariable (FabricIoManager, &ResourceSizeForEachRb, NULL, SocketNumber, IodsPerSocket, RbsPerIod, TRUE);
    }

    if ((CalledStatus != EFI_SUCCESS) || (!SetResourceBaseOnNv)) {
      // Can't get NvVariable or init IO base on NvVariable failed
      IDS_HDT_CONSOLE (CPU_TRACE, "  Init IO equally\n");
      CalledStatus = FabricStInitIoEqually (FabricIoManager, SocketNumber, IodsPerSocket, RbsPerIod);
      Status = (CalledStatus > Status) ? CalledStatus : Status;
    }
  } else {
    Status = EFI_ABORTED;
  }

  return Status;
}

/*---------------------------------------------------------------------------------------*/

/**
 * FabricStInitMmioEqually
 *
 * Initialize MMIO registers for each Rb evenly.
 *
 * @param[in, out]    FabricMmioManager        Point to FABRIC_MMIO_MANAGER
 * @param[in]         SocketNumber             System socket count
 * @param[in]         RbNumberPerSocket        Root bridge number per socket
 *
 */
EFI_STATUS
FabricStInitMmioEqually (
  IN       FABRIC_MMIO_MANAGER *FabricMmioManager,
  IN       UINT8                SocketNumber,
  IN       UINT8                IodNumberPerSocket,
  IN       UINT8                RbNumberPerIod
  )
{
  UINT8    i;
  UINT8    j;
  UINT8    k;
  UINT8    SystemRbNumber;
  UINT8    RbNumberAbovePcieCfg;
  UINT8    RbNumberAbovePcieCfgCopy;
  UINT8    RbNumberMmioHasInitialized;
  UINT16   DstFabricID;
  UINT64   TotalAvailableSize;
  UINT64   SizeAbovePcieCfg;
  UINT64   SizeBelowPcieCfg;
  UINT64   MmioBaseAddr;
  UINT64   MmioSize;
  UINT64   MmioSize16MAligned;
  UINT64   MmioSizeRemained;
  UINT64   MmioLimitAbove4G;
  UINT64   TOM;
  UINT64   TOM2;
  UINT64   PciCfgSpace;
  UINT64   BottomOfCompat;
  BOOLEAN  AbovePcieCfgIsTooSmall;
  BOOLEAN  BelowPcieCfgIsTooSmall;
  BOOLEAN  ReservedRegionAlreadySet;

  ASSERT (IodNumberPerSocket != 0 && SocketNumber != 0);
  SystemRbNumber = RbNumberPerIod * IodNumberPerSocket * SocketNumber;
  BottomOfCompat = BOTTOM_OF_COMPAT;
  /// @todo 0xFED0_0000 ~ 0xFED0_FFFF should be POSTED
  /// @todo BottomOfCompat = ADDITIONAL_POSTED_REGION_UNDER_PRIMARY_RB_END + 1;
  ReservedRegionAlreadySet = FALSE;  // Indicate if PcdHygonBottomMmioReservedForPrimaryRb ~ BottomOfCompat is set

  // System information
  PciCfgSpace = (AsmReadMsr64 (0xC0010058) >> 2) & 0xF;                   // Get bus range from MSR_C001_0058[5:2][BusRange]
  PciCfgSpace = MultU64x64 (((UINT64)1 << PciCfgSpace), (1024 * 1024));   // The size of configuration space is 1MB times the number of buses
  TOM  = AsmReadMsr64 (0xC001001A);
  TOM2 = AsmReadMsr64 (0xC001001D);
  IDS_HDT_CONSOLE (CPU_TRACE, "  TOM: %lX, TOM2: %lX, Pcie configuration space: %lX ~ %lX\n", TOM, TOM2, PcdGet64 (PcdPciExpressBaseAddress), (PcdGet64 (PcdPciExpressBaseAddress) + PciCfgSpace));

  // Below 4G
  // +---------------------------------+ +--+ 4G
  // |  BIOS FW                        |
  // |  Local APIC and etc.            |          Leave as undescribed, so it's a COMPAT region
  // |       Undescribed               |
  // +---------------------------------+ +--+ 0xFEC0_0000 (BottomOfCompat), from TOM to BottomOfCompat is MMIO space
  // |       Reserved for Primary RB   |          Reserved this region for some devices which need a fixed MMIO space
  // +---------------------------------+ +--+ 0xFE00_0000 (PcdHygonBottomMmioReservedForPrimaryRb), from TOM to PcdHygonBottomMmioReservedForPrimaryRb is the region that AllocateMmio function could use
  // |  MMIO Above PCIe Cfg            |
  // |                                 |
  // |                                 |
  // +---------------------------------+ +--+ PcdPciExpressBaseAddress + Size (defined by MSR_C001_0058[5:2][BusRange])
  // |  PCIe Configuration Space       |
  // |                                 |
  // +---------------------------------+ +--+ PcdPciExpressBaseAddress (recommend to set PcdPciExpressBaseAddress equal to TOM)
  // |                                 |
  // |  MMIO Below PCIe Cfg            |
  // |                                 |
  // |                                 |
  // |                                 |
  // |                                 |
  // +---------------------------------+ +--+ TOM
  // |                                 |
  // |  DRAM                           |
  // |                                 |
  // |                                 |
  // |                                 |
  // |                                 |
  // |                                 |
  // |                                 |
  // |                                 |
  // |                                 |
  // |                                 |
  // |                                 |
  // |                                 |
  // |                                 |
  // |                                 |
  // +---------------------------------+ +--+ 0x0000_0000

  // Above 4G
  // +---------------------------------+ +--+ 0xFFFD_0000_0000 (2^48 - 12G) or 0x7FD_0000_0000 (8T - 12G)
  // |                                 |
  // |   MMIO FOR RootBridge 7         |
  // |                                 |
  // |                                 |
  // |                                 |
  // |                                 |
  // +---------------------------------+
  // |                                 |
  // |   ......                        |
  // |                                 |
  // |                                 |
  // |                                 |
  // |                                 |
  // +---------------------------------+
  // |                                 |
  // |   MMIO FOR RootBridge 2         |
  // |                                 |
  // |                                 |
  // |                                 |
  // |                                 |
  // +---------------------------------+
  // |                                 |
  // |   MMIO FOR RootBridge 1         |
  // |                                 |
  // |                                 |
  // |                                 |
  // |                                 |
  // +---------------------------------+
  // |                                 |
  // |   MMIO FOR RootBridge 0         |
  // |                                 |
  // |                                 |
  // |                                 |
  // |                                 |
  // +---------------------------------+ +--+ TOM2 or 0x100_0000_0000

  AbovePcieCfgIsTooSmall = FALSE;
  BelowPcieCfgIsTooSmall = FALSE;

  ASSERT (PcdGet32 (PcdHygonBottomMmioReservedForPrimaryRb) >= (PcdGet64 (PcdPciExpressBaseAddress) + PciCfgSpace));
  ASSERT (PcdGet64 (PcdPciExpressBaseAddress) >= TOM);
  ASSERT (BottomOfCompat >= PcdGet32 (PcdHygonBottomMmioReservedForPrimaryRb));

  SizeAbovePcieCfg   = PcdGet32 (PcdHygonBottomMmioReservedForPrimaryRb) - PcdGet64 (PcdPciExpressBaseAddress) - PciCfgSpace;
  SizeBelowPcieCfg   = PcdGet64 (PcdPciExpressBaseAddress) - TOM;
  TotalAvailableSize = SizeAbovePcieCfg + SizeBelowPcieCfg;

  if (SystemRbNumber > 1) {
    // For single RB system, we don't want to check MMIO size above/below PcieCfg

    MmioSize = DivU64x32 (SizeBelowPcieCfg, SystemRbNumber);
    if (SizeAbovePcieCfg < DivU64x32 (MultU64x64 (MmioSize, 8), 10)) {
      // If SizeAbovePcieCfg < 80% of (SizeBelowPcieCfg / SystemRbNumber), ignore this region
      AbovePcieCfgIsTooSmall = TRUE;
      TotalAvailableSize    -= SizeAbovePcieCfg;
      IDS_HDT_CONSOLE (CPU_TRACE, "  AbovePcieCfgIsTooSmall\n");
    }

    MmioSize = DivU64x32 (SizeAbovePcieCfg, SystemRbNumber);
    if (SizeBelowPcieCfg < DivU64x32 (MultU64x64 (MmioSize, 8), 10)) {
      // If SizeBelowPcieCfg < 80% of (SizeAbovePcieCfg / SystemRbNumber), ignore this region
      BelowPcieCfgIsTooSmall = TRUE;
      TotalAvailableSize    -= SizeBelowPcieCfg;
      IDS_HDT_CONSOLE (CPU_TRACE, "  BelowPcieCfgIsTooSmall\n");
    }

    ASSERT (TotalAvailableSize != 0);
  } else {
    // 1 Rb
    AbovePcieCfgIsTooSmall = TRUE;
    BelowPcieCfgIsTooSmall = FALSE;
  }

  MmioSize = DivU64x32 (TotalAvailableSize, SystemRbNumber);
  MmioSize16MAligned = MmioSize & SIZE_16M_ALIGN;
  ASSERT ((!AbovePcieCfgIsTooSmall) || (!BelowPcieCfgIsTooSmall));

  // Setup MMIO below 4G
  MmioBaseAddr = 0;
  if (!AbovePcieCfgIsTooSmall) {
    // 1. find out how many Rbs would have MMIO above PCIe Configuration Space
    RbNumberAbovePcieCfg = (UINT8)DivU64x32 (SizeAbovePcieCfg, (UINT32)MmioSize);
    if ((SizeAbovePcieCfg - MultU64x64 (MmioSize, RbNumberAbovePcieCfg)) > DivU64x32 (MmioSize, 2)) {
      RbNumberAbovePcieCfg++;
    }

    if (RbNumberAbovePcieCfg == 0) {
      // it could make sure single RB system would always set the space above PcieCfg as MMIO in step 3
      RbNumberAbovePcieCfg++;
    }

    MmioSize = DivU64x32 (SizeAbovePcieCfg, RbNumberAbovePcieCfg);
    MmioSizeRemained = SizeAbovePcieCfg - MultU64x64 (MmioSize16MAligned, (RbNumberAbovePcieCfg - 1));
    MmioBaseAddr     = PcdGet32 (PcdHygonBottomMmioReservedForPrimaryRb);
  } else {
    // 2. if there's no MMIO above PCIe Configuration Space,
    RbNumberAbovePcieCfg = 0;
    MmioSizeRemained     = SizeBelowPcieCfg - MultU64x64 (MmioSize16MAligned, (SystemRbNumber - 1));
    MmioBaseAddr = PcdGet64 (PcdPciExpressBaseAddress);
  }

  // 3. allocate MMIO for each root bridge
  RbNumberMmioHasInitialized = 0;
  RbNumberAbovePcieCfgCopy   = RbNumberAbovePcieCfg;
  for (i = 0; i < SocketNumber; i++) {
    for (j = 0; j < IodNumberPerSocket; j++) {
      for (k = 0; k < RbNumberPerIod; k++) {
        FabricMmioManager->AllocateMmioBelow4GOnThisRb[i][j][k] = TRUE;
        if (RbNumberAbovePcieCfg != 0) {
          // above PCIe Configuration Space
          if (RbNumberMmioHasInitialized == 1) {
            MmioSizeRemained = MmioSize16MAligned;
          }

          MmioBaseAddr = MmioBaseAddr - MmioSizeRemained;
          RbNumberAbovePcieCfg--;
        } else if (!BelowPcieCfgIsTooSmall) {
          // below PCIe Configuration Space
          if (RbNumberAbovePcieCfgCopy == RbNumberMmioHasInitialized) {
            MmioSize = DivU64x32 (SizeBelowPcieCfg, (SystemRbNumber - RbNumberMmioHasInitialized));
            MmioSize16MAligned = MmioSize & SIZE_16M_ALIGN;
            MmioSizeRemained   = SizeBelowPcieCfg - MultU64x64 (MmioSize16MAligned, (SystemRbNumber - RbNumberMmioHasInitialized - 1));
            MmioBaseAddr = PcdGet64 (PcdPciExpressBaseAddress);
          }

          if (RbNumberMmioHasInitialized == (RbNumberAbovePcieCfgCopy + 1)) {
            MmioSizeRemained = MmioSize16MAligned;
          }

          MmioBaseAddr = MmioBaseAddr - MmioSizeRemained;
        } else {
          ASSERT (FALSE);
        }

        DstFabricID = (UINT16)FabricTopologyGetHostBridgeSystemFabricID (i, j, k);
        if ((i == 0) && (j == 0) && (k == 0) && (!AbovePcieCfgIsTooSmall)) {
          // if primary Rb's MMIO is above PCIe Configuration Space, include PcdHygonBottomMmioReservedForPrimaryRb ~ BottomOfCompat as well
          FabricStSetMmioReg (
            SocketNumber,
            IodNumberPerSocket,
            ((i * IodNumberPerSocket * RbNumberPerIod + j * RbNumberPerIod + k) * 2),
            DstFabricID,
            MmioBaseAddr,
            (MmioSizeRemained + BottomOfCompat - PcdGet32 (PcdHygonBottomMmioReservedForPrimaryRb))
            );
          ReservedRegionAlreadySet = TRUE;
        } else {
          FabricStSetMmioReg (
            SocketNumber,
            IodNumberPerSocket,
            ((i * IodNumberPerSocket * RbNumberPerIod + j * RbNumberPerIod + k) * 2),
            DstFabricID,
            MmioBaseAddr,
            MmioSizeRemained
            );
        }

        // Prefetchable -> Non Prefetchable -> Non Pci
        // Prefetchable/Non Prefetchable 80/20 Ratio
        FabricMmioManager->MmioRegionBelow4G[i][j][k].SizeNonPci          = (PcdGet32 (PcdHygonMmioSizePerRbForNonPciDevice) > MMIO_MIN_NON_PCI_SIZE) ? PcdGet32 (PcdHygonMmioSizePerRbForNonPciDevice) : MMIO_MIN_NON_PCI_SIZE;
        FabricMmioManager->MmioRegionBelow4G[i][j][k].SizePrefetch        = (DivU64x32 (MultU64x64 ((MmioSizeRemained - FabricMmioManager->MmioRegionBelow4G[i][j][k].SizeNonPci), 2), 5)) & SIZE_16M_ALIGN;
        FabricMmioManager->MmioRegionBelow4G[i][j][k].BasePrefetch        = MmioBaseAddr;
        FabricMmioManager->MmioRegionBelow4G[i][j][k].BaseNonPrefetch     = FabricMmioManager->MmioRegionBelow4G[i][j][k].BasePrefetch + FabricMmioManager->MmioRegionBelow4G[i][j][k].SizePrefetch;
        FabricMmioManager->MmioRegionBelow4G[i][j][k].BaseNonPci          = (MmioBaseAddr + MmioSizeRemained - FabricMmioManager->MmioRegionBelow4G[i][j][k].SizeNonPci) & (~ST_NON_PCI_MMIO_ALIGN_MASK);
        FabricMmioManager->MmioRegionBelow4G[i][j][k].SizeNonPrefetch     = FabricMmioManager->MmioRegionBelow4G[i][j][k].BaseNonPci - FabricMmioManager->MmioRegionBelow4G[i][j][k].BaseNonPrefetch;
        FabricMmioManager->MmioRegionBelow4G[i][j][k].UsedSizeNonPci      = 0;
        FabricMmioManager->MmioRegionBelow4G[i][j][k].UsedSizePrefetch    = 0;
        FabricMmioManager->MmioRegionBelow4G[i][j][k].UsedSizeNonPrefetch = 0;
        FabricMmioManager->MmioRegionBelow4G[i][j][k].AlignNonPrefetch    = 0;
        FabricMmioManager->MmioRegionBelow4G[i][j][k].AlignPrefetch       = 0;
        IDS_HDT_CONSOLE (CPU_TRACE, "    BasePrefetch    0x%lX, SizePrefetch    0x%lX\n", FabricMmioManager->MmioRegionBelow4G[i][j][k].BasePrefetch, FabricMmioManager->MmioRegionBelow4G[i][j][k].SizePrefetch);
        IDS_HDT_CONSOLE (CPU_TRACE, "    BaseNonPrefetch 0x%lX, SizeNonPrefetch 0x%lX\n", FabricMmioManager->MmioRegionBelow4G[i][j][k].BaseNonPrefetch, FabricMmioManager->MmioRegionBelow4G[i][j][k].SizeNonPrefetch);
        IDS_HDT_CONSOLE (CPU_TRACE, "    BaseNonPci      0x%lX, SizeNonPci      0x%lX\n", FabricMmioManager->MmioRegionBelow4G[i][j][k].BaseNonPci, FabricMmioManager->MmioRegionBelow4G[i][j][k].SizeNonPci);

        RbNumberMmioHasInitialized++;
      }
    }
  }

  // 5. Distribute MMIO above 4G evenly
  MmioBaseAddr     = (TOM2 > 0x100000000) ? TOM2 : 0x100000000; // Check if TOM2 > 4G

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
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "    Above 4G MMIO base is %lX, limit is %lX\n", MmioBaseAddr, MmioLimitAbove4G);

  if (MmioLimitAbove4G > MmioBaseAddr) {
    MmioSize = MmioLimitAbove4G - MmioBaseAddr;
    MmioSize16MAligned = DivU64x32 (MmioSize, SystemRbNumber) & SIZE_16M_ALIGN;
    MmioSizeRemained   = MmioSize - MultU64x64 (MmioSize16MAligned, (SystemRbNumber - 1));

    for (i = 0; i < SocketNumber; i++) {
      for (j = 0; j < IodNumberPerSocket; j++) {
        for (k = 0; k < RbNumberPerIod; k++) {
          FabricMmioManager->AllocateMmioAbove4GOnThisRb[i][j][k] = TRUE;
          if ((i == (SocketNumber - 1)) && (j == (IodNumberPerSocket - 1)) && (k == (RbNumberPerIod - 1))) {
            MmioSize16MAligned = MmioSizeRemained;
          }

          if (MmioSize16MAligned < ST_MMIO_MIN_SIZE) {
            continue;
          }

          DstFabricID = (UINT16)FabricTopologyGetHostBridgeSystemFabricID (i, j, k);
          if ((i == (SocketNumber - 1)) && (j == (IodNumberPerSocket - 1)) && (k == (RbNumberPerIod - 1))) {
            // workaround to set last Rb's MMIO limit to 0xFFFF_FFFF_FFFF
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
              MmioSize16MAligned
              );
          }

          FabricMmioManager->MmioRegionAbove4G[i][j][k].SizeNonPci = (PcdGet32 (PcdHygonMmioSizePerRbForNonPciDeviceAbove4G) > MMIO_MIN_NON_PCI_SIZE_ABOVE_4G) ? PcdGet32 (
                                                                                                                                                                   PcdHygonMmioSizePerRbForNonPciDeviceAbove4G
                                                                                                                                                                   ) : MMIO_MIN_NON_PCI_SIZE_ABOVE_4G;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].SizePrefetch    = (DivU64x32 (MultU64x64 ((MmioSize16MAligned - FabricMmioManager->MmioRegionAbove4G[i][j][k].SizeNonPci), 2), 5)) & SIZE_16M_ALIGN;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].SizeNonPrefetch = MmioSize16MAligned - FabricMmioManager->MmioRegionAbove4G[i][j][k].SizeNonPci - FabricMmioManager->MmioRegionAbove4G[i][j][k].SizePrefetch;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].BasePrefetch    = MmioBaseAddr;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].BaseNonPrefetch = FabricMmioManager->MmioRegionAbove4G[i][j][k].BasePrefetch + FabricMmioManager->MmioRegionAbove4G[i][j][k].SizePrefetch;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].BaseNonPci          = FabricMmioManager->MmioRegionAbove4G[i][j][k].BaseNonPrefetch + FabricMmioManager->MmioRegionAbove4G[i][j][k].SizeNonPrefetch;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].UsedSizeNonPci      = 0;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].UsedSizePrefetch    = 0;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].UsedSizeNonPrefetch = 0;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].AlignNonPrefetch    = 0;
          FabricMmioManager->MmioRegionAbove4G[i][j][k].AlignPrefetch       = 0;

          IDS_HDT_CONSOLE (CPU_TRACE, "    BasePrefetch    0x%lX, SizePrefetch    0x%lX\n", FabricMmioManager->MmioRegionAbove4G[i][j][k].BasePrefetch, FabricMmioManager->MmioRegionAbove4G[i][j][k].SizePrefetch);
          IDS_HDT_CONSOLE (CPU_TRACE, "    BaseNonPrefetch 0x%lX, SizeNonPrefetch 0x%lX\n", FabricMmioManager->MmioRegionAbove4G[i][j][k].BaseNonPrefetch, FabricMmioManager->MmioRegionAbove4G[i][j][k].SizeNonPrefetch);
          IDS_HDT_CONSOLE (CPU_TRACE, "    BaseNonPci      0x%lX, SizeNonPci      0x%lX\n", FabricMmioManager->MmioRegionAbove4G[i][j][k].BaseNonPci, FabricMmioManager->MmioRegionAbove4G[i][j][k].SizeNonPci);

          MmioBaseAddr += MmioSize16MAligned;
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/

/**
 * FabricStInitIoEqually
 *
 * Initialize IO registers for each Rb evenly.
 *
 * @param[in, out]    FabricIoManager          Point to FABRIC_IO_MANAGER
 * @param[in]         SocketNumber             System socket count
 * @param[in]         RbNumberPerSocket        Rb number per socket
 *
 */
EFI_STATUS
FabricStInitIoEqually (
  IN       FABRIC_IO_MANAGER   *FabricIoManager,
  IN       UINT8                SocketNumber,
  IN       UINT8                IodNumberPerSocket,
  IN       UINT8                RbNumberPerIod
  )
{
  UINT8           i;
  UINT8           j;
  UINT8           k;
  UINT8           RegIndex;
  UINT8           TotalRbNumber;
  UINT16          DstFabricID;
  UINT32          IoBase;
  UINT32          IoLimit;
  UINT32          IoSizeForThisRb;
  DF_IO_RESOURCE  *DfIoResource = NULL;

  IoBase   = 0;
  RegIndex = 0;
  IDS_HDT_CONSOLE (CPU_TRACE, "FabricStInitIoEqually ...\n");

  // most IO registers have a 16bit limit
  // reserve LegacyIoSize for primary Rb
  ASSERT (IodNumberPerSocket != 0 && SocketNumber != 0);

  TotalRbNumber = SocketNumber * IodNumberPerSocket * RbNumberPerIod;
  ASSERT (TotalRbNumber <= 16);

  switch(TotalRbNumber) {
    case 1:
      DfIoResource = &DfIoInfoFor1RB[0];
      break;
    case 2:
      DfIoResource = &DfIoInfoFor2RB[0];
      break;
    case 4:
      DfIoResource = &DfIoInfoFor4RB[0];
      break;
    case 8:
      DfIoResource = &DfIoInfoFor8RB[0];
      break;
    case 16:
      DfIoResource = &DfIoInfoFor16RB[0];
      break;
    default:
      IDS_HDT_CONSOLE (CPU_TRACE, "    Invalid RB number=%d\n", TotalRbNumber);
      ASSERT (FALSE);
      break;
  }

  if (DfIoResource == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for (i = 0; i < SocketNumber; i++) {
    for (j = 0; j < IodNumberPerSocket; j++) {
      for (k = 0; k < RbNumberPerIod; k++) {
        DstFabricID = (UINT16)FabricTopologyGetHostBridgeSystemFabricID (i, j, k);

        if ((i == 0) && (j == 0) && (k == 0)) {
          IoBase = 0;
        } else {
          IoBase = DfIoResource[i * IodNumberPerSocket * RbNumberPerIod + j * RbNumberPerIod + k].IoBase;
        }

        IoLimit = DfIoResource[i * IodNumberPerSocket * RbNumberPerIod + j * RbNumberPerIod + k].IoLimit;

        if ((IoLimit + 1 - IoBase) < 0x1000) {
          // not 4KB boundary, invalid resource then set size to 0
          IoSizeForThisRb = 0;
        } else {
          IoSizeForThisRb = (IoLimit + 1) - IoBase;
        }

        FabricStSetIoReg (SocketNumber, IodNumberPerSocket, RegIndex, DstFabricID, IoBase, IoSizeForThisRb);

        if ((IoBase == 0) && (IoSizeForThisRb != 0)) {
          // Assignable IO resource from 0x1000 to 0xFFFF,
          // if RBs >= 16, IO resource not enough for every RB,
          // then skip not exist resource setting from 'DfIoResource' table
          IoBase = X86_LEGACY_IO_SIZE;
          IoSizeForThisRb = (IoLimit + 1) - IoBase;
        }

        FabricIoManager->IoRegion[i][j][k].IoBase = IoBase;
        FabricIoManager->IoRegion[i][j][k].IoSize = IoSizeForThisRb;

        FabricIoManager->IoRegion[i][j][k].IoUsed = 0;

        IDS_HDT_CONSOLE (CPU_TRACE, "  Socket %d Rb %d has IO base 0x%X size 0x%X\n", i, j, FabricIoManager->IoRegion[i][j][k].IoBase, FabricIoManager->IoRegion[i][j][k].IoSize);

        RegIndex++;
      }
    }
  }

  FabricIoManager->GlobalCtrl = TRUE;

  return EFI_SUCCESS;
}
