/* $NoKeywords:$ */
/**
 * @file
 *
 * Base Fabric MMIO map manager Lib implementation for ZEN ZP
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
#include <Library/AmdS3SaveLib.h>
#include <Library/AmdHeapLib.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>
#include <Library/FabricResourceSizeForEachDieLib.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include "FabricResourceManagerZenZpLib.h"

#define FILECODE LIBRARY_FABRICRESOURCEMANAGERZENZPLIB_FABRICRESOURCEINITZENZPLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

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
 * Initialize DF resource registers for each Die.
 *
 */
EFI_STATUS
FabricResourceInit (
  )
{
  UINT8                         SocketNumber;
  UINT8                         DieNumberPerSocket;
  BOOLEAN                       SetResourceBaseOnNv;
  EFI_STATUS                    Status;
  EFI_STATUS                    CalledStatus;
  FABRIC_RESOURCE_FOR_EACH_DIE  ResourceSizeForEachDie;
  FABRIC_MMIO_MANAGER          *FabricMmioManager;
  FABRIC_IO_MANAGER            *FabricIoManager;
  ALLOCATE_HEAP_PARAMS          AllocateHeapParams;

  Status = EFI_SUCCESS;
  CalledStatus = EFI_SUCCESS;
  FabricMmioManager = NULL;
  FabricIoManager = NULL;

  SocketNumber = (UINT8) FabricTopologyGetNumberOfProcessorsPresent ();
  DieNumberPerSocket = (UINT8) FabricTopologyGetNumberOfDiesOnSocket (0);

  SetResourceBaseOnNv = FALSE;

  if ((!PcdGetBool (PcdAmdFabricResourceDefaultMap)) && (FabricGetResourceSizeForEachDie (&ResourceSizeForEachDie) == EFI_SUCCESS)) {
    SetResourceBaseOnNv = TRUE;
  }

  AllocateHeapParams.RequestedBufferSize = sizeof (FABRIC_MMIO_MANAGER);
  AllocateHeapParams.BufferHandle        = AMD_MMIO_MANAGER;
  AllocateHeapParams.Persist             = HEAP_SYSTEM_MEM;
  if (HeapAllocateBuffer (&AllocateHeapParams, NULL) == AGESA_SUCCESS) {
    FabricMmioManager = (FABRIC_MMIO_MANAGER *) AllocateHeapParams.BufferPtr;
  }

  AllocateHeapParams.RequestedBufferSize = sizeof (FABRIC_IO_MANAGER);
  AllocateHeapParams.BufferHandle        = AMD_IO_MANAGER;
  AllocateHeapParams.Persist             = HEAP_SYSTEM_MEM;
  if (HeapAllocateBuffer (&AllocateHeapParams, NULL) == AGESA_SUCCESS) {
    FabricIoManager = (FABRIC_IO_MANAGER *) AllocateHeapParams.BufferPtr;
  }

  if  (FabricMmioManager != NULL) {
    if (SetResourceBaseOnNv) {
      // Get NvVariable successfully, try to init MMIO base on it
      IDS_HDT_CONSOLE (CPU_TRACE, "  Init MMIO base on NV variable\n");
      CalledStatus = FabricZenZpInitMmioBaseOnNvVariable (FabricMmioManager, &ResourceSizeForEachDie, NULL, SocketNumber, DieNumberPerSocket, TRUE);
    }
    if ((CalledStatus != EFI_SUCCESS) || (!SetResourceBaseOnNv)) {
      // Can't get NvVariable or init MMIO base on NvVariable failed
      IDS_HDT_CONSOLE (CPU_TRACE, "  Init MMIO equally\n");
      CalledStatus = FabricZenZpInitMmioEqually (FabricMmioManager, SocketNumber, DieNumberPerSocket);
      Status = (CalledStatus > Status) ? CalledStatus : Status;
    }
  } else {
    Status = EFI_ABORTED;
  }

  if  (FabricIoManager != NULL) {
    if (SetResourceBaseOnNv) {
      // Get NvVariable successfully, try to init IO base on it
      IDS_HDT_CONSOLE (CPU_TRACE, "  Init IO base on NV variable\n");
      CalledStatus = FabricZenZpInitIoBaseOnNvVariable (FabricIoManager, &ResourceSizeForEachDie, NULL, SocketNumber, DieNumberPerSocket, TRUE);
    }
    if ((CalledStatus != EFI_SUCCESS) || (!SetResourceBaseOnNv)) {
      // Can't get NvVariable or init IO base on NvVariable failed
      IDS_HDT_CONSOLE (CPU_TRACE, "  Init IO equally\n");
      CalledStatus = FabricZenZpInitIoEqually (FabricIoManager, SocketNumber, DieNumberPerSocket);
      Status = (CalledStatus > Status) ? CalledStatus : Status;
    }
  } else {
    Status = EFI_ABORTED;
  }

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricZenZpInitMmioEqually
 *
 * Initialize MMIO registers for each Die evenly.
 *
 * @param[in, out]    FabricMmioManager        Point to FABRIC_MMIO_MANAGER
 * @param[in]         SocketNumber             System socket count
 * @param[in]         DieNumberPerSocket       Die number per socket
 *
 */
EFI_STATUS
FabricZenZpInitMmioEqually (
  IN       FABRIC_MMIO_MANAGER *FabricMmioManager,
  IN       UINT8                SocketNumber,
  IN       UINT8                DieNumberPerSocket
  )
{
  UINT8  i;
  UINT8  j;
  UINT8  SystemDieNumber;
  UINT8  DieNumberAbovePcieCfg;
  UINT8  DieNumberAbovePcieCfgCopy;
  UINT8  DieNumberMmioHasInitialized;
  UINT8  DstFabricIDSysOffset;
  UINT64 TotalAvailableSize;
  UINT64 SizeAbovePcieCfg;
  UINT64 SizeBelowPcieCfg;
  UINT64 MmioBaseAddr;
  UINT64 MmioSize;
  UINT64 MmioSize16MAligned;
  UINT64 MmioSizeRemained;
  UINT64 MmioLimitAbove4G;
  UINT64 TOM;
  UINT64 TOM2;
  UINT64 PciCfgSpace;
  UINT64 BottomOfCompat;
  UINT64 MmioSizeBelowHole;
  UINT64 MmioSizeAboveHole;
  BOOLEAN AbovePcieCfgIsTooSmall;
  BOOLEAN BelowPcieCfgIsTooSmall;
  BOOLEAN ReservedRegionAlreadySet;

  SystemDieNumber = DieNumberPerSocket * SocketNumber;
  BottomOfCompat = BOTTOM_OF_COMPAT;
  /// @todo For ZP B2 and later, 0xFED0_0000 ~ 0xFED0_FFFF should be POSTED
  /// @todo BottomOfCompat = ADDITIONAL_POSTED_REGION_UNDER_DIE0_END + 1;
  ReservedRegionAlreadySet = FALSE;  // Indicate if PcdAmdBottomMmioReservedForDie0 ~ BottomOfCompat is set

  // System information
  PciCfgSpace = (AsmReadMsr64 (0xC0010058) >> 2) & 0xF;                   // Get bus range from MSR_C001_0058[5:2][BusRange]
  PciCfgSpace = MultU64x64 (((UINT64) 1 << PciCfgSpace), (1024 * 1024));  // The size of configuration space is 1MB times the number of buses
  TOM = AsmReadMsr64 (0xC001001A);
  TOM2 = AsmReadMsr64 (0xC001001D);
  IDS_HDT_CONSOLE (CPU_TRACE, "  TOM: %lX, TOM2: %lX, Pcie configuration space: %lX ~ %lX\n", TOM, TOM2, PcdGet64 (PcdPciExpressBaseAddress), (PcdGet64 (PcdPciExpressBaseAddress) + PciCfgSpace));

  // Below 4G
  //    +---------------------------------+ +--+ 4G
  //    |  BIOS FW                        |
  //    |  Local APIC and etc.            |          Leave as undescribed, so it's a COMPAT region
  //    |       Undescribed               |
  //    +---------------------------------+ +--+ 0xFEC0_0000 (BottomOfCompat), from TOM to BottomOfCompat is MMIO space
  //    |       Reserved for DIE0         |          Reserved this region for some devices which need a fixed MMIO space
  //    +---------------------------------+ +--+ 0xFE00_0000 (PcdAmdBottomMmioReservedForDie0), from TOM to PcdAmdBottomMmioReservedForDie0 is the region that AllocateMmio function could use
  //    |  MMIO Above PCIe Cfg            |
  //    |                                 |
  //    |                                 |
  //    +---------------------------------+ +--+ PcdPciExpressBaseAddress + Size (defined by MSR_C001_0058[5:2][BusRange])
  //    |  PCIe Configuration Space       |
  //    |                                 |
  //    +---------------------------------+ +--+ PcdPciExpressBaseAddress (recommend to set PcdPciExpressBaseAddress equal to TOM)
  //    |                                 |
  //    |  MMIO Below PCIe Cfg            |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    +---------------------------------+ +--+ TOM
  //    |                                 |
  //    |  DRAM                           |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    +---------------------------------+ +--+ 0x0000_0000

  // Above 4G
  //    +---------------------------------+ +--+ 0xFFFD_0000_0000 (2^48 - 12G) or 0x7FD_0000_0000 (8T - 12G)
  //    |                                 |
  //    |   MMIO FOR DIE 7                |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    +---------------------------------+
  //    |                                 |
  //    |   ......                        |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    +---------------------------------+
  //    |                                 |
  //    |   MMIO FOR DIE 2                |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    +---------------------------------+
  //    |                                 |
  //    |   MMIO FOR DIE 1                |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    +---------------------------------+
  //    |                                 |
  //    |   MMIO FOR DIE 0                |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    |                                 |
  //    +---------------------------------+ +--+ TOM2 or 0x100_0000_0000

  AbovePcieCfgIsTooSmall = FALSE;
  BelowPcieCfgIsTooSmall = FALSE;

  ASSERT (PcdGet32 (PcdAmdBottomMmioReservedForDie0) >= (PcdGet64 (PcdPciExpressBaseAddress) + PciCfgSpace));
  ASSERT (PcdGet64 (PcdPciExpressBaseAddress) >= TOM);
  ASSERT (BottomOfCompat >= PcdGet32 (PcdAmdBottomMmioReservedForDie0));

  SizeAbovePcieCfg = PcdGet32 (PcdAmdBottomMmioReservedForDie0) - PcdGet64 (PcdPciExpressBaseAddress) - PciCfgSpace;
  SizeBelowPcieCfg = PcdGet64 (PcdPciExpressBaseAddress) - TOM;
  TotalAvailableSize = SizeAbovePcieCfg + SizeBelowPcieCfg;

  if (SystemDieNumber > 1) {
    // For single DIE system, we don't want to check MMIO size above/below PcieCfg

    MmioSize = DivU64x32 (SizeBelowPcieCfg, SystemDieNumber);
    if (SizeAbovePcieCfg < DivU64x32 (MultU64x64 (MmioSize, 8), 10)) {
      // If SizeAbovePcieCfg < 80% of (SizeBelowPcieCfg / SystemDieNumber), ignore this region
      AbovePcieCfgIsTooSmall = TRUE;
      TotalAvailableSize -= SizeAbovePcieCfg;
      IDS_HDT_CONSOLE (CPU_TRACE, "  AbovePcieCfgIsTooSmall\n");
    }

    MmioSize = DivU64x32 (SizeAbovePcieCfg, SystemDieNumber);
    if (SizeBelowPcieCfg < DivU64x32 (MultU64x64 (MmioSize, 8), 10)) {
      // If SizeBelowPcieCfg < 80% of (SizeAbovePcieCfg / SystemDieNumber), ignore this region
      BelowPcieCfgIsTooSmall = TRUE;
      TotalAvailableSize -= SizeBelowPcieCfg;
      IDS_HDT_CONSOLE (CPU_TRACE, "  BelowPcieCfgIsTooSmall\n");
    }
    ASSERT (TotalAvailableSize != 0);
  } else {
    //1 Die
    AbovePcieCfgIsTooSmall = TRUE;
    BelowPcieCfgIsTooSmall = FALSE;
  }
  
  MmioSize = DivU64x32 (TotalAvailableSize, SystemDieNumber);
  MmioSize16MAligned = MmioSize & SIZE_16M_ALIGN;
  ASSERT ((!AbovePcieCfgIsTooSmall) || (!BelowPcieCfgIsTooSmall));

  // Setup MMIO below 4G
  MmioBaseAddr = 0;
  if (!AbovePcieCfgIsTooSmall) {
    //   1. find out how many DIEs would have MMIO above PCIe Configuration Space
    DieNumberAbovePcieCfg = (UINT8) DivU64x32 (SizeAbovePcieCfg, (UINT32) MmioSize);
    if ((SizeAbovePcieCfg - MultU64x64 (MmioSize, DieNumberAbovePcieCfg)) > DivU64x32 (MmioSize, 2)) {
      DieNumberAbovePcieCfg++;
    }
    if (DieNumberAbovePcieCfg == 0) {
      // it could make sure single DIE system would always set the space above PcieCfg as MMIO in step 3
      DieNumberAbovePcieCfg++;
    }
    MmioSize = DivU64x32 (SizeAbovePcieCfg, DieNumberAbovePcieCfg);
    MmioSizeRemained = SizeAbovePcieCfg - MultU64x64 (MmioSize16MAligned, (DieNumberAbovePcieCfg - 1));
    MmioBaseAddr = PcdGet32 (PcdAmdBottomMmioReservedForDie0);
  } else {
    //   2. if there's no MMIO above PCIe Configuration Space,
    DieNumberAbovePcieCfg = 0;
    MmioSizeRemained = SizeBelowPcieCfg - MultU64x64 (MmioSize16MAligned, (SystemDieNumber - 1));
    MmioBaseAddr = PcdGet64 (PcdPciExpressBaseAddress);
  }
  //   3. allocate MMIO for each DIE
  DieNumberMmioHasInitialized = 0;
  DieNumberAbovePcieCfgCopy = DieNumberAbovePcieCfg;
  for (i = 0; i < SocketNumber; i++) {
    for (j = 0; j < DieNumberPerSocket; j++) {
      FabricMmioManager->AllocateMmioBelow4GOnThisDie[i][j] = TRUE;
      if (DieNumberAbovePcieCfg != 0) {
        // above PCIe Configuration Space
        if (DieNumberMmioHasInitialized == 1) {
          MmioSizeRemained = MmioSize16MAligned;
        }
        MmioBaseAddr = MmioBaseAddr - MmioSizeRemained;
        DieNumberAbovePcieCfg--;
      } else if (!BelowPcieCfgIsTooSmall) {
        // below PCIe Configuration Space
        if (DieNumberAbovePcieCfgCopy == DieNumberMmioHasInitialized) {
          MmioSize = DivU64x32 (SizeBelowPcieCfg, (SystemDieNumber - DieNumberMmioHasInitialized));
          MmioSize16MAligned = MmioSize & SIZE_16M_ALIGN;
          MmioSizeRemained = SizeBelowPcieCfg - MultU64x64 (MmioSize16MAligned, (SystemDieNumber - DieNumberMmioHasInitialized - 1));
          MmioBaseAddr = PcdGet64 (PcdPciExpressBaseAddress);
        }
        if (DieNumberMmioHasInitialized == (DieNumberAbovePcieCfgCopy + 1)) {
          MmioSizeRemained = MmioSize16MAligned;
        }
        MmioBaseAddr = MmioBaseAddr - MmioSizeRemained;
      } else {
        ASSERT (FALSE);
      }

      DstFabricIDSysOffset = (UINT8) FabricTopologyGetDieSystemOffset (i, j);
      if ((i == 0) && (j == 0) && (!AbovePcieCfgIsTooSmall)) {
        // if Die0's MMIO is above PCIe Configuration Space, include PcdAmdBottomMmioReservedForDie0 ~ BottomOfCompat as well
        FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((i * MAX_DIES_PER_SOCKET + j) * 2), DstFabricIDSysOffset, MmioBaseAddr, (MmioSizeRemained + BottomOfCompat - PcdGet32 (PcdAmdBottomMmioReservedForDie0)));
        ReservedRegionAlreadySet = TRUE;
      } else {
        FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((i * MAX_DIES_PER_SOCKET + j) * 2), DstFabricIDSysOffset, MmioBaseAddr, MmioSizeRemained);
      }
      // Prefetchable -> Non Prefetchable -> Non Pci
      // Prefetchable/Non Prefetchable 80/20 Ratio
      FabricMmioManager->MmioRegionBelow4G[i][j].SizeNonPci = (PcdGet32 (PcdAmdMmioSizePerDieForNonPciDevice) > MMIO_MIN_NON_PCI_SIZE) ? PcdGet32 (PcdAmdMmioSizePerDieForNonPciDevice): MMIO_MIN_NON_PCI_SIZE;
      FabricMmioManager->MmioRegionBelow4G[i][j].SizePrefetch = (DivU64x32 (MultU64x64 ((MmioSizeRemained - FabricMmioManager->MmioRegionBelow4G[i][j].SizeNonPci), 2), 5)) & SIZE_16M_ALIGN;
      FabricMmioManager->MmioRegionBelow4G[i][j].BasePrefetch = MmioBaseAddr;
      FabricMmioManager->MmioRegionBelow4G[i][j].BaseNonPrefetch = FabricMmioManager->MmioRegionBelow4G[i][j].BasePrefetch + FabricMmioManager->MmioRegionBelow4G[i][j].SizePrefetch;
      FabricMmioManager->MmioRegionBelow4G[i][j].BaseNonPci = (MmioBaseAddr + MmioSizeRemained - FabricMmioManager->MmioRegionBelow4G[i][j].SizeNonPci) & (~ZEN_NON_PCI_MMIO_ALIGN_MASK);
      FabricMmioManager->MmioRegionBelow4G[i][j].SizeNonPrefetch = FabricMmioManager->MmioRegionBelow4G[i][j].BaseNonPci - FabricMmioManager->MmioRegionBelow4G[i][j].BaseNonPrefetch;
      FabricMmioManager->MmioRegionBelow4G[i][j].UsedSizeNonPci = 0;
      FabricMmioManager->MmioRegionBelow4G[i][j].UsedSizePrefetch = 0;
      FabricMmioManager->MmioRegionBelow4G[i][j].UsedSizeNonPrefetch = 0;
      FabricMmioManager->MmioRegionBelow4G[i][j].AlignNonPrefetch = 0;
      FabricMmioManager->MmioRegionBelow4G[i][j].AlignPrefetch = 0;
      IDS_HDT_CONSOLE (CPU_TRACE, "    BasePrefetch    0x%lX, SizePrefetch    0x%lX\n", FabricMmioManager->MmioRegionBelow4G[i][j].BasePrefetch, FabricMmioManager->MmioRegionBelow4G[i][j].SizePrefetch);
      IDS_HDT_CONSOLE (CPU_TRACE, "    BaseNonPrefetch 0x%lX, SizeNonPrefetch 0x%lX\n", FabricMmioManager->MmioRegionBelow4G[i][j].BaseNonPrefetch, FabricMmioManager->MmioRegionBelow4G[i][j].SizeNonPrefetch);
      IDS_HDT_CONSOLE (CPU_TRACE, "    BaseNonPci      0x%lX, SizeNonPci      0x%lX\n", FabricMmioManager->MmioRegionBelow4G[i][j].BaseNonPci, FabricMmioManager->MmioRegionBelow4G[i][j].SizeNonPci);

      DieNumberMmioHasInitialized++;
    }
  }

  //   4. if there's a spare MMIO register pair, try to set undescribed space (above or below PCIe Configuration) as Die0's 2nd MMIO
  if (SystemDieNumber < (ZEN_MAX_DIE_PER_SOCKET * ZEN_MAX_SOCKET)) {
    if ((AbovePcieCfgIsTooSmall && (SizeAbovePcieCfg != 0)) || ((SizeBelowPcieCfg != 0) && (BelowPcieCfgIsTooSmall || (SystemDieNumber == 1)))) {
      for (i = 0; i < ZEN_MAX_SOCKET; i++) {
        for (j = 0; j < ZEN_MAX_DIE_PER_SOCKET; j++) {
          if (FabricMmioManager->AllocateMmioBelow4GOnThisDie[i][j] == FALSE) {
            IDS_HDT_CONSOLE (CPU_TRACE, "  Die0 has 2nd MMIO below 4G\n");
            FabricMmioManager->Die02ndMmioPairBelow4G = (UINT8) ((i << 4) | j);
            if (AbovePcieCfgIsTooSmall) {
              MmioBaseAddr = PcdGet64 (PcdPciExpressBaseAddress) + PciCfgSpace;
              MmioSize = SizeAbovePcieCfg;
              FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((i * MAX_DIES_PER_SOCKET + j) * 2), 0, MmioBaseAddr, (MmioSize + BottomOfCompat - PcdGet32 (PcdAmdBottomMmioReservedForDie0)));
              ReservedRegionAlreadySet = TRUE;
            } else {
              // for single DIE system, DIE0 always set the space above PcieCfg as MMIO in step 3
              MmioBaseAddr = TOM;
              MmioSize = SizeBelowPcieCfg;
              FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((i * MAX_DIES_PER_SOCKET + j) * 2), 0, MmioBaseAddr, MmioSize);
            }

            FabricMmioManager->MmioRegionBelow4G[i][j].SizeNonPci = 0;
            //FabricMmioManager->MmioRegionBelow4G[i][j].SizePrefetch = MmioSize;
            FabricMmioManager->MmioRegionBelow4G[i][j].SizePrefetch = (DivU64x32 (MultU64x64 (MmioSize, 2), 5)) & SIZE_16M_ALIGN;
            FabricMmioManager->MmioRegionBelow4G[i][j].SizeNonPrefetch = MmioSize - FabricMmioManager->MmioRegionBelow4G[i][j].SizePrefetch;
            FabricMmioManager->MmioRegionBelow4G[i][j].BasePrefetch = MmioBaseAddr;
            FabricMmioManager->MmioRegionBelow4G[i][j].BaseNonPrefetch = MmioBaseAddr + FabricMmioManager->MmioRegionBelow4G[i][j].SizePrefetch;
            FabricMmioManager->MmioRegionBelow4G[i][j].BaseNonPci = 0;
            FabricMmioManager->MmioRegionBelow4G[i][j].UsedSizeNonPci = 0;
            FabricMmioManager->MmioRegionBelow4G[i][j].UsedSizePrefetch = 0;
            FabricMmioManager->MmioRegionBelow4G[i][j].UsedSizeNonPrefetch = 0;
            FabricMmioManager->MmioRegionBelow4G[i][j].AlignNonPrefetch = 0;
            FabricMmioManager->MmioRegionBelow4G[i][j].AlignPrefetch = 0;

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

  //   5. Distribute MMIO above 4G evenly
  MmioBaseAddr = (TOM2 > 0x100000000)? TOM2 : 0x100000000; // Check if TOM2 > 4G
  MmioLimitAbove4G = 0x7FD00000000; // SMEE wold be enabled as default, so set limit to 0x7FD00000000

  if (PcdGet64 (PcdAmdMmioAbove4GLimit) < MmioLimitAbove4G) {
    MmioLimitAbove4G = (PcdGet64 (PcdAmdMmioAbove4GLimit) + 1) & 0xFFFFFFFFFFFF0000;
  }

  // there's a hole at 0xFD_0000_0000 ~ 0x100_0000_0000
  MmioSizeBelowHole = 0;
  MmioSizeAboveHole = 0;

  // calculate Mmio size below/above this hole
  if (MmioBaseAddr < 0xFD00000000) {
    MmioSizeBelowHole = (MmioLimitAbove4G < 0xFD00000000)? MmioLimitAbove4G : 0xFD00000000;
    MmioSizeBelowHole = MmioSizeBelowHole - MmioBaseAddr;    
  }

  if (MmioLimitAbove4G > 0x10000000000) {
    MmioSizeAboveHole = (MmioBaseAddr > 0x10000000000)? MmioBaseAddr : 0x10000000000;
    MmioSizeAboveHole = MmioLimitAbove4G - MmioSizeAboveHole;    
  }

  if ((MmioSizeBelowHole == 0) && (MmioSizeAboveHole == 0)) {
    MmioLimitAbove4G = 0; // set limit to 0, so there's no MMIO space above 4G
    IDS_HDT_CONSOLE (CPU_TRACE, "    WARNING: There's no MMIO space above 4G\n");
  } else {
    if (MmioSizeAboveHole > MmioSizeBelowHole) {
      MmioBaseAddr = (TOM2 > 0x10000000000)? TOM2 : 0x10000000000; // Check if TOM2 > 1T
    } else {
      MmioLimitAbove4G = (MmioLimitAbove4G < 0xFD00000000)? MmioLimitAbove4G : 0xFD00000000;
    }
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "    Above 4G MMIO base is %lX, limit is %lX\n", MmioBaseAddr, MmioLimitAbove4G);

  if (MmioLimitAbove4G > MmioBaseAddr) {
    MmioSize = MmioLimitAbove4G - MmioBaseAddr;
    MmioSize16MAligned = DivU64x32 (MmioSize, SystemDieNumber) & SIZE_16M_ALIGN;
    MmioSizeRemained = MmioSize - MultU64x64 (MmioSize16MAligned, (SystemDieNumber - 1));

    for (i = 0; i < SocketNumber; i++) {
      for (j = 0; j < DieNumberPerSocket; j++) {
        FabricMmioManager->AllocateMmioAbove4GOnThisDie[i][j] = TRUE;
        if ((i == (SocketNumber - 1)) && (j == (DieNumberPerSocket - 1))) {
          MmioSize16MAligned = MmioSizeRemained;
        }
        if (MmioSize16MAligned < ZEN_MMIO_MIN_SIZE) {
          continue;
        }
        DstFabricIDSysOffset = (UINT8) FabricTopologyGetDieSystemOffset (i, j);
        if ((i == (SocketNumber - 1)) && (j == (DieNumberPerSocket - 1))) {
          // workaround to set last DIE's MMIO limit to 0xFFFF_FFFF_FFFF
          FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((i * MAX_DIES_PER_SOCKET + j) * 2 + 1), DstFabricIDSysOffset, MmioBaseAddr, (0x1000000000000 - MmioBaseAddr));
        } else {
          FabricZenZpSetMmioReg (SocketNumber, DieNumberPerSocket, ((i * MAX_DIES_PER_SOCKET + j) * 2 + 1), DstFabricIDSysOffset, MmioBaseAddr, MmioSize16MAligned);
        }
        FabricMmioManager->MmioRegionAbove4G[i][j].SizeNonPci = 0;
        FabricMmioManager->MmioRegionAbove4G[i][j].SizePrefetch = (DivU64x32 (MultU64x64 ((MmioSize16MAligned - FabricMmioManager->MmioRegionAbove4G[i][j].SizeNonPci), 2), 5)) & SIZE_16M_ALIGN;
        FabricMmioManager->MmioRegionAbove4G[i][j].SizeNonPrefetch = MmioSize16MAligned - FabricMmioManager->MmioRegionAbove4G[i][j].SizeNonPci - FabricMmioManager->MmioRegionAbove4G[i][j].SizePrefetch;
        FabricMmioManager->MmioRegionAbove4G[i][j].BasePrefetch = MmioBaseAddr;
        FabricMmioManager->MmioRegionAbove4G[i][j].BaseNonPrefetch = FabricMmioManager->MmioRegionAbove4G[i][j].BasePrefetch + FabricMmioManager->MmioRegionAbove4G[i][j].SizePrefetch;
        FabricMmioManager->MmioRegionAbove4G[i][j].BaseNonPci = 0;
        FabricMmioManager->MmioRegionAbove4G[i][j].UsedSizeNonPci = 0;
        FabricMmioManager->MmioRegionAbove4G[i][j].UsedSizePrefetch = 0;
        FabricMmioManager->MmioRegionAbove4G[i][j].UsedSizeNonPrefetch = 0;
        FabricMmioManager->MmioRegionAbove4G[i][j].AlignNonPrefetch = 0;
        FabricMmioManager->MmioRegionAbove4G[i][j].AlignPrefetch = 0;

        IDS_HDT_CONSOLE (CPU_TRACE, "    BasePrefetch    0x%lX, SizePrefetch    0x%lX\n", FabricMmioManager->MmioRegionAbove4G[i][j].BasePrefetch, FabricMmioManager->MmioRegionAbove4G[i][j].SizePrefetch);
        IDS_HDT_CONSOLE (CPU_TRACE, "    BaseNonPrefetch 0x%lX, SizeNonPrefetch 0x%lX\n", FabricMmioManager->MmioRegionAbove4G[i][j].BaseNonPrefetch, FabricMmioManager->MmioRegionAbove4G[i][j].SizeNonPrefetch);

        MmioBaseAddr += MmioSize16MAligned;
      }
    }
  }

  //   6. if there's a spare MMIO register pair, set 0xFED0_0000 ~ 0xFED0_FFFF as posted
  FabricZenZpAdditionalMmioSetting (FabricMmioManager, SocketNumber, DieNumberPerSocket, BottomOfCompat, ReservedRegionAlreadySet);
  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricZenZpInitIoEqually
 *
 * Initialize IO registers for each Die evenly.
 *
 * @param[in, out]    FabricIoManager          Point to FABRIC_IO_MANAGER
 * @param[in]         SocketNumber             System socket count
 * @param[in]         DieNumberPerSocket       Die number per socket
 *
 */
EFI_STATUS
FabricZenZpInitIoEqually (
  IN       FABRIC_IO_MANAGER   *FabricIoManager,
  IN       UINT8                SocketNumber,
  IN       UINT8                DieNumberPerSocket
  )
{
  UINT8             i;
  UINT8             j;
  UINT8             RegIndex;
  UINT8             DstFabricIDSysOffset;
  UINT32            IoBase;
  UINT32            IoSize;
  UINT32            IoSizeForThisDie;
  UINT32            LegacyIoSize;

  IoBase = 0;
  RegIndex = 0;
  LegacyIoSize = X86_LEGACY_IO_SIZE;
  IDS_HDT_CONSOLE (CPU_TRACE, "    reserve 0x%X IO size for legacy devices\n", LegacyIoSize);

  // most IO registers have a 16bit limit
  // reserve LegacyIoSize for Die 0
  IoSize = ((0x10000 - LegacyIoSize) / (SocketNumber * DieNumberPerSocket)) & 0xFFFFF000; // 12 bit alignment

  for (i = 0; i < SocketNumber; i++) {
    for (j = 0; j < DieNumberPerSocket; j++) {
      DstFabricIDSysOffset = (UINT8) FabricTopologyGetDieSystemOffset (i, j);
      if ((i == (SocketNumber - 1)) && (j == (DieNumberPerSocket - 1))) {
        // 1. first, check if it's the last Die. Set IO limit up to 0x1FFFFFF for last Die.
        IoSizeForThisDie = X86IO_LIMIT - IoBase;
      } else if ((i == 0) && (j == 0)) {
        // 2. second, if it's Die 0 and it's not the last Die, plus size with LegacyIoSize
        IoSizeForThisDie = IoSize + LegacyIoSize;
      } else {
        IoSizeForThisDie = IoSize;
      }

      FabricZenZpSetIoReg (SocketNumber, DieNumberPerSocket, RegIndex, DstFabricIDSysOffset, IoBase, IoSizeForThisDie);

      if ((i == 0) && (j == 0)) {
        // Die 0 has an additional IO region for legacy devices, size is LegacyIoSize
        FabricIoManager->IoRegion[i][j].IoBase = IoBase + LegacyIoSize;
        FabricIoManager->IoRegion[i][j].IoSize = IoSizeForThisDie - LegacyIoSize;
      } else {
        FabricIoManager->IoRegion[i][j].IoBase = IoBase;
        FabricIoManager->IoRegion[i][j].IoSize = IoSizeForThisDie;
      }
      FabricIoManager->IoRegion[i][j].IoUsed = 0;

      IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x Die%x has IO base 0x%X size 0x%X\n", i, j, FabricIoManager->IoRegion[i][j].IoBase, FabricIoManager->IoRegion[i][j].IoSize);

      IoBase += IoSizeForThisDie;
      RegIndex++;
    }
  }

  FabricIoManager->GlobalCtrl = TRUE;

  return EFI_SUCCESS;
}




