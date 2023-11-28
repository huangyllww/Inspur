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
#include <Library/AmdHeapLib.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>
#include <Library/FabricResourceSizeForEachDieLib.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include "FabricResourceManagerZenZpLib.h"

#define FILECODE LIBRARY_FABRICRESOURCEMANAGERZENZPLIB_FABRICRESOURCEMANAGERZENZPLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
FabricZenZpGetRemainingSizeForThisRegion (
  IN       FABRIC_MMIO_REGION *MmioRegion,
  IN       UINT64             *Size,
  IN       UINT64              Alignment,
  IN       UINT8               MmioType
  );

UINT64
FabricZenZpGetBaseAddress (
  IN       FABRIC_MMIO_REGION *MmioRegion,
  IN       UINT64              Length,
  IN       UINT64              Alignment,
  IN       UINT8               MmioType
  );

EFI_STATUS
FabricGetUsedResourceSize (
  IN       FABRIC_RESOURCE_FOR_EACH_DIE    *ResourceSizeForEachDie
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * BSC entry point for allocate MMIO
 *
 * program MMIO base/limit/control registers
 *
 * @param[in, out]    BaseAddress        Starting address of the requested MMIO range.
 * @param[in, out]    Length             Length of the requested MMIO range.
 * @param[in]         Alignment          Alignment bit map.
 * @param[in]         Target             PCI bus number/Die number of the requestor.
 * @param[in, out]    Attributes         Attributes of the requested MMIO range indicating whether
 *                                       it is readable/writable/non-posted
 *
 * @retval            EFI_STATUS         EFI_OUT_OF_RESOURCES - The requested range could not be added because there are not
 *                                                              enough mapping resources.
 *                                       EFI_ABORTED          - One or more input parameters are invalid. For example, the
 *                                                              PciBusNumber does not correspond to any device in the system.
 *                                       EFI_SUCCESS          - Success to get an MMIO region
 */
EFI_STATUS
FabricAllocateMmio (
  IN OUT   UINT64 *BaseAddress,
  IN OUT   UINT64 *Length,
  IN       UINT64                 Alignment,
  IN       FABRIC_TARGET          Target,
  IN OUT   FABRIC_MMIO_ATTRIBUTE *Attributes
  )
{
  UINT8                     Socket;
  UINT8                     Die;
  UINT8                     TempSocket;
  UINT8                     TempDie;
  UINT8                     DstFabricID;
  UINTN                     Register;
  UINT64                    SizeA;
  UINT64                    SizeB;
  UINT64                    TempSize;
  CFG_ADDRESS_MAP_REGISTER  BusMap;
  LOCATE_HEAP_PTR           LocateHeapParams;
  FABRIC_MMIO_MANAGER      *FabricMmioManager;
  FABRIC_MMIO_REGION       *MmioRegion;
  FABRIC_MMIO_REGION       *Die02ndMmioRegion;
  EFI_STATUS                ReturnStatus;

  ReturnStatus = EFI_SUCCESS;
  *BaseAddress = 0;
  FabricMmioManager = NULL;
  // Find MMIO manager in heap
  LocateHeapParams.BufferHandle = AMD_MMIO_MANAGER;

  if (HeapLocateBuffer (&LocateHeapParams, NULL) == AGESA_SUCCESS) {
    FabricMmioManager = (FABRIC_MMIO_MANAGER *) LocateHeapParams.BufferPtr;
  }
  ASSERT (FabricMmioManager != NULL);

  // Check input parameters
  if (*Length == 0) {
    return EFI_SUCCESS;
  }
  
  // Find out Socket/Die
  DstFabricID = 0xFF;
  if (Target.TgtType == TARGET_PCI_BUS) {
    for (Register = CFGADDRESSMAP0_REG; Register <= CFGADDRESSMAP7_REG; Register += 4) {
      BusMap.Value = FabricRegisterAccRead (0, 0, CFGADDRESSMAP_FUNC, Register, FABRIC_REG_ACC_BC);
      if ((BusMap.Field.RE == 1) && (BusMap.Field.WE == 1) && (BusMap.Field.BusNumLimit >= Target.PciBusNum) && (BusMap.Field.BusNumBase <= Target.PciBusNum)) {
        DstFabricID = (UINT8) BusMap.Field.DstFabricID;
        DstFabricID &= FABRIC_ID_SOCKET_DIE_MASK;
        break;
      }
    }

    if (Register > CFGADDRESSMAP7_REG) {
      return EFI_ABORTED;
    }
    ASSERT (DstFabricID != 0xFF);
    Socket = (DstFabricID >> 7) & 1;
    Die    = (DstFabricID >> 5) & 3;
  } else {
    Socket = (UINT8) (Target.SocketNum);
    Die = (UINT8) (Target.DieNum);
  }

  SizeA = 0;
  SizeB = 0;
  Die02ndMmioRegion = NULL;
  if ((Attributes->MmioType == MMIO_BELOW_4G) ||
      (Attributes->MmioType == P_MMIO_BELOW_4G) ||
      (Attributes->MmioType == NON_PCI_DEVICE_BELOW_4G)) {
    //
    // Below 4G
    //

    // return EFI_ABORTED if no MMIO on this DIE
    if (FabricMmioManager->AllocateMmioBelow4GOnThisDie[Socket][Die] == FALSE) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  ERROR: No below 4G MMIO on Socket %X Die %X\n", Socket, Die);
      return EFI_ABORTED;
    }

    // Find out MmioRegion for this Die
    MmioRegion = &FabricMmioManager->MmioRegionBelow4G[Socket][Die];
    FabricZenZpGetRemainingSizeForThisRegion (MmioRegion, &SizeA, Alignment, Attributes->MmioType);
    if (FabricMmioManager->Die0Has2ndMmioBelow4G && (Socket == 0) && (Die == 0)) {
      // Die0 may have 2nd MmioRegion
      TempSocket = (FabricMmioManager->Die02ndMmioPairBelow4G >> 4) & 0xF;
      TempDie = FabricMmioManager->Die02ndMmioPairBelow4G & 0xF;
      Die02ndMmioRegion = &FabricMmioManager->MmioRegionBelow4G[TempSocket][TempDie];
      FabricZenZpGetRemainingSizeForThisRegion (Die02ndMmioRegion, &SizeB, Alignment, Attributes->MmioType);
      if ((SizeA > SizeB) && (SizeB != 0)) {
        // Make sure MmioRegion & SizeA points to the smaller one
        // Die02ndMmioRegion & SizeB points to the larger one
        MmioRegion = Die02ndMmioRegion;
        Die02ndMmioRegion = &FabricMmioManager->MmioRegionBelow4G[Socket][Die];

        TempSize = SizeA;
        SizeA = SizeB;
        SizeB = TempSize;
      }
    }
    // Try to allocate MMIO space from the smaller one first
    if (SizeA >= (*Length)) {
      *BaseAddress = FabricZenZpGetBaseAddress (MmioRegion, *Length, Alignment, Attributes->MmioType);
    } else if (SizeB >= (*Length)) {
      *BaseAddress = FabricZenZpGetBaseAddress (Die02ndMmioRegion, *Length, Alignment, Attributes->MmioType);
    }
  } else if ((Attributes->MmioType == MMIO_ABOVE_4G) ||
             (Attributes->MmioType == P_MMIO_ABOVE_4G)) {
    //
    // Above 4G
    //
    // return EFI_ABORTED if no MMIO on this DIE
    if (FabricMmioManager->AllocateMmioAbove4GOnThisDie[Socket][Die] == FALSE) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  ERROR: No above 4G MMIO on Socket %X Die %X\n", Socket, Die);
      return EFI_ABORTED;
    }

    MmioRegion = &FabricMmioManager->MmioRegionAbove4G[Socket][Die];
    FabricZenZpGetRemainingSizeForThisRegion (MmioRegion, &SizeA, Alignment, Attributes->MmioType);
    if (SizeA >= (*Length)) {
      // Have enough space
      *BaseAddress = FabricZenZpGetBaseAddress (MmioRegion, *Length, Alignment, Attributes->MmioType);
    }
  }

  if (*BaseAddress == 0) {
    *Length = (SizeA > SizeB) ? SizeA : SizeB;
    *BaseAddress = 0;
    ReturnStatus = EFI_OUT_OF_RESOURCES;
  }

  if (ReturnStatus == EFI_SUCCESS) {
    Attributes->NonPosted = 0;
    Attributes->ReadEnable = 1;
    Attributes->WriteEnable = 1;
    Attributes->CpuDis = 0;
    IDS_HDT_CONSOLE (CPU_TRACE, "  Allocate MMIO from 0x%lX ~ 0x%lX\n", *BaseAddress, (*BaseAddress + *Length - 1));
  } else if (ReturnStatus == EFI_OUT_OF_RESOURCES) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  No enough space, the biggest MMIO size is 0x%X0000\n", (UINT32) (*Length >> 16));
    // FabricAllocateMmio is restricted for NON_PCI_DEVICE_BELOW_4G only
    // So if there's no enough space, ASSERT here
    if (Attributes->MmioType == NON_PCI_DEVICE_BELOW_4G) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  Please increase PcdAmdMmioSizePerDieForNonPciDevice\n");
      ASSERT (FALSE);
    } 
  }

  return ReturnStatus;
}

/*---------------------------------------------------------------------------------------*/
/**
 * BSC entry point for allocate IO
 *
 * program IO base/limit registers
 *
 * @param[in, out]    BaseAddress        Starting address of the requested MMIO range.
 * @param[in, out]    Length             Length of the requested MMIO range.
 * @param[in]         Target             PCI bus number/Die number of the requestor.
 *
 * @retval            EFI_STATUS         EFI_OUT_OF_RESOURCES - The requested range could not be added because there are not
 *                                                              enough mapping resources.
 *                                       EFI_ABORTED          - One or more input parameters are invalid. For example, the
 *                                                              PciBusNumber does not correspond to any device in the system.
 *                                       EFI_SUCCESS          - Success to get an IO region
 */
EFI_STATUS
FabricAllocateIo (
  IN OUT   UINT32 *BaseAddress,
  IN OUT   UINT32 *Length,
  IN       FABRIC_TARGET          Target
  )
{
  UINT8                     Socket;
  UINT8                     Die;
  UINT8                     DstFabricID;
  UINTN                     Register;
  CFG_ADDRESS_MAP_REGISTER  BusMap;
  LOCATE_HEAP_PTR           LocateHeapParams;
  FABRIC_IO_MANAGER        *FabricIoManager;
  FABRIC_IO_REGION         *IoRegion;
  EFI_STATUS                ReturnStatus;

  ReturnStatus = EFI_SUCCESS;

  FabricIoManager = NULL;
  // Find MMIO manager in heap
  LocateHeapParams.BufferHandle = AMD_IO_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == AGESA_SUCCESS) {
    FabricIoManager = (FABRIC_IO_MANAGER *) LocateHeapParams.BufferPtr;
  }
  ASSERT (FabricIoManager != NULL);

  if (!FabricIoManager->GlobalCtrl) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  Error : FabricIoManager->GlobalCtrl is false ! \n");
    return EFI_OUT_OF_RESOURCES;
  }

  // Find out Socket/Die
  DstFabricID = 0xFF;
  if (Target.TgtType == TARGET_PCI_BUS) {
    for (Register = CFGADDRESSMAP0_REG; Register <= CFGADDRESSMAP7_REG; Register += 4) {
      BusMap.Value = FabricRegisterAccRead (0, 0, CFGADDRESSMAP_FUNC, Register, FABRIC_REG_ACC_BC);
      if ((BusMap.Field.RE == 1) && (BusMap.Field.WE == 1) && (BusMap.Field.BusNumLimit >= Target.PciBusNum) && (BusMap.Field.BusNumBase <= Target.PciBusNum)) {
        DstFabricID = (UINT8) BusMap.Field.DstFabricID;
        DstFabricID &= FABRIC_ID_SOCKET_DIE_MASK;
        break;
      }
    }

    if (Register > CFGADDRESSMAP7_REG) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  Error : Bus not found ! \n");  
      return EFI_ABORTED;
    }
    ASSERT (DstFabricID != 0xFF);
    Socket = (DstFabricID >> 7) & 1;
    Die    = (DstFabricID >> 5) & 3;
  } else {
    Socket = (UINT8) (Target.SocketNum);
    Die = (UINT8) (Target.DieNum);
  }

  // 
  IoRegion = &FabricIoManager->IoRegion[Socket][Die];
  if (IoRegion->IoSize >= (*Length + IoRegion->IoUsed)) {
    *BaseAddress = IoRegion->IoBase + IoRegion->IoSize - IoRegion->IoUsed - *Length;
    IoRegion->IoUsed += *Length;
    IDS_HDT_CONSOLE (CPU_TRACE, "  Allocate IO from 0x%X ~ 0x%X\n", *BaseAddress, (*BaseAddress + *Length - 1));
  } else {
    *Length = IoRegion->IoSize - IoRegion->IoUsed;
    ReturnStatus = EFI_OUT_OF_RESOURCES;
    IDS_HDT_CONSOLE (CPU_TRACE, "  No enough IO space, the biggest IO size is 0x%X\n", *Length);
  }

  return ReturnStatus;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricGetAvailableResource
 *
 * Get available DF resource (MMIO/IO) for each Die
 *
 * @param[in, out]    ResourceForEachDie    Avaiable DF resource (MMIO/IO) for each Die
 *
 * @retval            EFI_SUCCESS           Success to get available resource
 *                    EFI_ABORTED           Can't get information of MMIO or IO
 */
EFI_STATUS
FabricGetAvailableResource (
  IN       FABRIC_RESOURCE_FOR_EACH_DIE    *ResourceForEachDie
  )
{
  UINT8  i;
  UINT8  j;
  UINT8  SocketNumber;
  UINT8  DieNumberPerSocket;
  UINT8  TempSocket;
  UINT8  TempDie;
  LOCATE_HEAP_PTR           LocateHeapParams;
  FABRIC_MMIO_MANAGER      *FabricMmioManager;
  FABRIC_MMIO_REGION       *MmioRegion;
  FABRIC_IO_MANAGER        *FabricIoManager;
  FABRIC_IO_REGION         *IoRegion;

  FabricMmioManager = NULL;
  FabricIoManager = NULL;

  LocateHeapParams.BufferHandle = AMD_MMIO_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == AGESA_SUCCESS) {
    FabricMmioManager = (FABRIC_MMIO_MANAGER *) LocateHeapParams.BufferPtr;
  }

  LocateHeapParams.BufferHandle = AMD_IO_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == AGESA_SUCCESS) {
    FabricIoManager = (FABRIC_IO_MANAGER *) LocateHeapParams.BufferPtr;
  }

  if ((FabricMmioManager == NULL) && (FabricIoManager == NULL)) {
    return EFI_ABORTED;
  }

  SocketNumber = (UINT8) FabricTopologyGetNumberOfProcessorsPresent ();
  DieNumberPerSocket = (UINT8) FabricTopologyGetNumberOfDiesOnSocket (0);

  for (i = 0; i < MAX_SOCKETS_SUPPORTED; i++) {
    for (j = 0; j < MAX_DIES_PER_SOCKET; j++) {
      if ((i >= SocketNumber) || (j >= DieNumberPerSocket)) {
        // Die doesn't exist
        ResourceForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Size = 0;
        ResourceForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Size = 0;
        ResourceForEachDie->PrefetchableMmioSizeAbove4G[i][j].Size = 0;
        ResourceForEachDie->PrefetchableMmioSizeBelow4G[i][j].Size = 0;
        ResourceForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Base = 0;
        ResourceForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Base = 0;
        ResourceForEachDie->PrefetchableMmioSizeAbove4G[i][j].Base = 0;
        ResourceForEachDie->PrefetchableMmioSizeBelow4G[i][j].Base = 0;
        ResourceForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Alignment = 0;
        ResourceForEachDie->PrefetchableMmioSizeAbove4G[i][j].Alignment = 0;
        ResourceForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Alignment = 0;
        ResourceForEachDie->PrefetchableMmioSizeBelow4G[i][j].Alignment = 0;

        ResourceForEachDie->IO[i][j].Size = 0;
        ResourceForEachDie->IO[i][j].Base = 0;
      } else {
        if ((FabricMmioManager->AllocateMmioAbove4GOnThisDie[i][j]) == FALSE) {
          // No MMIO on this DIE
          ResourceForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Size = 0;
          ResourceForEachDie->PrefetchableMmioSizeAbove4G[i][j].Size = 0;
          ResourceForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Base = 0;
          ResourceForEachDie->PrefetchableMmioSizeAbove4G[i][j].Base = 0;
          ResourceForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Alignment = 0;
          ResourceForEachDie->PrefetchableMmioSizeAbove4G[i][j].Alignment = 0;
        } else {
          // Only report prefetchable & non-prefetchable MMIO size
          MmioRegion = &FabricMmioManager->MmioRegionAbove4G[i][j];
          ResourceForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Size = MmioRegion->SizeNonPrefetch - MmioRegion->UsedSizeNonPrefetch;
          ResourceForEachDie->PrefetchableMmioSizeAbove4G[i][j].Size = MmioRegion->SizePrefetch - MmioRegion->UsedSizePrefetch;
          ResourceForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Base = MmioRegion->BaseNonPrefetch + MmioRegion->UsedSizeNonPrefetch;
          ResourceForEachDie->PrefetchableMmioSizeAbove4G[i][j].Base = MmioRegion->BasePrefetch + MmioRegion->UsedSizePrefetch;
          ResourceForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Alignment = MmioRegion->AlignNonPrefetch;
          ResourceForEachDie->PrefetchableMmioSizeAbove4G[i][j].Alignment = MmioRegion->AlignPrefetch;
          IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x Die%x has non-prefetch base 0x%lX size 0x%lX align %x MMIO above 4G available\n", i, j, ResourceForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Base, ResourceForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Size, ResourceForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Alignment);
          IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x Die%x has prefetchable base 0x%lX size 0x%lX align %x MMIO above 4G available\n", i, j, ResourceForEachDie->PrefetchableMmioSizeAbove4G[i][j].Base, ResourceForEachDie->PrefetchableMmioSizeAbove4G[i][j].Size, ResourceForEachDie->PrefetchableMmioSizeAbove4G[i][j].Alignment);
        }

        if ((FabricMmioManager->AllocateMmioBelow4GOnThisDie[i][j]) == FALSE) {
          // No MMIO on this DIE
          ResourceForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Size = 0;
          ResourceForEachDie->PrefetchableMmioSizeBelow4G[i][j].Size = 0;
          ResourceForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Base = 0;
          ResourceForEachDie->PrefetchableMmioSizeBelow4G[i][j].Base = 0;
          ResourceForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Alignment = 0;
          ResourceForEachDie->PrefetchableMmioSizeBelow4G[i][j].Alignment = 0;
        } else {
          // Only report prefetchable & non-prefetchable MMIO size
          MmioRegion = &FabricMmioManager->MmioRegionBelow4G[i][j];
          ResourceForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Size = MmioRegion->SizeNonPrefetch - MmioRegion->UsedSizeNonPrefetch;
          ResourceForEachDie->PrefetchableMmioSizeBelow4G[i][j].Size =  MmioRegion->SizePrefetch - MmioRegion->UsedSizePrefetch;
          ResourceForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Base = MmioRegion->BaseNonPrefetch + MmioRegion->UsedSizeNonPrefetch;
          ResourceForEachDie->PrefetchableMmioSizeBelow4G[i][j].Base =  MmioRegion->BasePrefetch + MmioRegion->UsedSizePrefetch;
          ResourceForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Alignment = MmioRegion->AlignNonPrefetch;
          ResourceForEachDie->PrefetchableMmioSizeBelow4G[i][j].Alignment = MmioRegion->AlignPrefetch;
          IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x Die%x has non-prefetch base 0x%lX size 0x%lX align %x MMIO below 4G available\n", i, j, ResourceForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Base, ResourceForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Size, ResourceForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Alignment);
          IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x Die%x has prefetchable base 0x%lX size 0x%lX align %x MMIO below 4G available\n", i, j, ResourceForEachDie->PrefetchableMmioSizeBelow4G[i][j].Base, ResourceForEachDie->PrefetchableMmioSizeBelow4G[i][j].Size, ResourceForEachDie->PrefetchableMmioSizeBelow4G[i][j].Alignment);
        }

        IoRegion = &FabricIoManager->IoRegion[i][j];
        ResourceForEachDie->IO[i][j].Base = IoRegion->IoBase;
        ResourceForEachDie->IO[i][j].Size = IoRegion->IoSize - IoRegion->IoUsed;
        IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x Die%x has IO base 0x%X size 0x%X\n", i, j, ResourceForEachDie->IO[i][j].Base, ResourceForEachDie->IO[i][j].Size);
      }
    }
  }

  // Die0's 2nd MMIO
  if (FabricMmioManager->Die0Has2ndMmioBelow4G) {
    TempSocket = (FabricMmioManager->Die02ndMmioPairBelow4G >> 4) & 0xF;
    TempDie = FabricMmioManager->Die02ndMmioPairBelow4G & 0xF;
    MmioRegion = &FabricMmioManager->MmioRegionBelow4G[TempSocket][TempDie];

    ResourceForEachDie->Die0SecondNonPrefetchableMmioSizeBelow4G.Size = MmioRegion->SizeNonPrefetch - MmioRegion->UsedSizeNonPrefetch;
    ResourceForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Size = MmioRegion->SizePrefetch - MmioRegion->UsedSizePrefetch;
    ResourceForEachDie->Die0SecondNonPrefetchableMmioSizeBelow4G.Base = MmioRegion->BaseNonPrefetch + MmioRegion->UsedSizeNonPrefetch;
    ResourceForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Base = MmioRegion->BasePrefetch + MmioRegion->UsedSizePrefetch;
    ResourceForEachDie->Die0SecondNonPrefetchableMmioSizeBelow4G.Alignment = MmioRegion->AlignNonPrefetch;
    ResourceForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Alignment = MmioRegion->AlignPrefetch;
    IDS_HDT_CONSOLE (CPU_TRACE, "  Die0's 2nd MMIO non-prefetch base 0x%lX size 0x%lX align %x MMIO\n", ResourceForEachDie->Die0SecondNonPrefetchableMmioSizeBelow4G.Base, ResourceForEachDie->Die0SecondNonPrefetchableMmioSizeBelow4G.Size, ResourceForEachDie->Die0SecondNonPrefetchableMmioSizeBelow4G.Alignment);
    IDS_HDT_CONSOLE (CPU_TRACE, "  Die0's 2nd MMIO prefetchable base 0x%lX size 0x%lX align %x MMIO\n", ResourceForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Base, ResourceForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Size, ResourceForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Alignment);
  } else {
    ResourceForEachDie->Die0SecondNonPrefetchableMmioSizeBelow4G.Alignment = 0;
    ResourceForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Alignment = 0;
    ResourceForEachDie->Die0SecondNonPrefetchableMmioSizeBelow4G.Size = 0;
    ResourceForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Size = 0;
    ResourceForEachDie->Die0SecondNonPrefetchableMmioSizeBelow4G.Base = 0;
    ResourceForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Base = 0;
  }

  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricReallocateResourceForEachDie
 *
 * ResourceSize = UsedSize + UserRequestSize (input)
 * Save ResourceSize to NV variable
 *
 * @param[in, out]    ResourceSizeForEachDie    Avaiable DF resource (MMIO/IO) size for each Die
 * @param[in, out]    SpaceStatus               Current status
 *
 * @retval            EFI_SUCCESS               Save user's request to NV variable successfully
 *                    EFI_OUT_OF_RESOURCES      No enough resource
 */
EFI_STATUS
FabricReallocateResourceForEachDie (
  IN       FABRIC_RESOURCE_FOR_EACH_DIE      *ResourceSizeForEachDie,
  IN       FABRIC_ADDR_SPACE_SIZE            *SpaceStatus
  )
{
  UINT8       i;
  UINT8       j;
  UINT8       SocketNumber;
  UINT8       DieNumberPerSocket;
  EFI_STATUS  Status;
  EFI_STATUS  CalledStatus;
  FABRIC_RESOURCE_FOR_EACH_DIE CurrentUsed;

  SocketNumber = (UINT8) FabricTopologyGetNumberOfProcessorsPresent ();
  DieNumberPerSocket = (UINT8) FabricTopologyGetNumberOfDiesOnSocket (0);

  for (i = 0; i < MAX_SOCKETS_SUPPORTED; i++) {
    for (j = 0; j < MAX_DIES_PER_SOCKET; j++) {
      if ((i >= SocketNumber) || (j >= DieNumberPerSocket)) {
        // Die doesn't exist
        ResourceSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Size = 0;
        ResourceSizeForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Size = 0;
        ResourceSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Size = 0;
        ResourceSizeForEachDie->PrefetchableMmioSizeBelow4G[i][j].Size = 0;
        ResourceSizeForEachDie->IO[i][j].Size = 0;

        ResourceSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Alignment = 0;
        ResourceSizeForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Alignment = 0;
        ResourceSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Alignment = 0;
        ResourceSizeForEachDie->PrefetchableMmioSizeBelow4G[i][j].Alignment = 0;
        continue;
      }
      if (ResourceSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Size == 0) {
        ResourceSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Alignment = 0;
      }
      if (ResourceSizeForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Size == 0) {
        ResourceSizeForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Alignment = 0;
      }
      if (ResourceSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Size == 0) {
        ResourceSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Alignment = 0;
      }
      if (ResourceSizeForEachDie->PrefetchableMmioSizeBelow4G[i][j].Size == 0) {
        ResourceSizeForEachDie->PrefetchableMmioSizeBelow4G[i][j].Alignment = 0;
      }
      if (ResourceSizeForEachDie->Die0SecondNonPrefetchableMmioSizeBelow4G.Size == 0) {
        ResourceSizeForEachDie->Die0SecondNonPrefetchableMmioSizeBelow4G.Alignment = 0;
      }
      if (ResourceSizeForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Size == 0) {
        ResourceSizeForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Alignment = 0;
      }
    }
  }

  // Get how much space is used
  FabricGetUsedResourceSize (&CurrentUsed);

  Status = FabricSetResourceSizeForEachDie (&CurrentUsed, ResourceSizeForEachDie);

  if (SpaceStatus != NULL) {
      LibAmdMemFill (SpaceStatus, 0, sizeof (FABRIC_ADDR_SPACE_SIZE), NULL);
  }
  
  CalledStatus = FabricZenZpInitMmioBaseOnNvVariable (NULL, ResourceSizeForEachDie, SpaceStatus, SocketNumber, DieNumberPerSocket, FALSE);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  CalledStatus = FabricZenZpInitIoBaseOnNvVariable (NULL, ResourceSizeForEachDie, SpaceStatus, SocketNumber, DieNumberPerSocket, FALSE);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  return Status;
}


/*---------------------------------------------------------------------------------------*/
/**
 * FabricResourceRestoreDefault
 *
 * Restore default MMIO/IO distribution strategy by clearing NV variable
 *
 * @retval            EFI_SUCCESS               Success to clear NV variable
 */
EFI_STATUS
FabricResourceRestoreDefault (
  )
{
  FabricClearResourceSizeForEachDie ();

  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricEnableVgaMmio
 *
 * Set VGA Enable register
 *
 * @param[in]         Target                    PCI bus number/Die number of the requestor.
 *
 * @retval            EFI_SUCCESS               Success to set VGA enable registers
 *                    EFI_ABORTED               Can't find destination
 */
EFI_STATUS
FabricEnableVgaMmio (
  IN       FABRIC_TARGET          Target
  )
{
  UINT8  i;
  UINT8  j;
  UINT8  SocketNumber;
  UINT8  DieNumberPerSocket;
  UINT8  DstFabricIDSysOffset;
  UINTN  Register;
  CFG_ADDRESS_MAP_REGISTER  BusMap;
  VGA_EN_REGISTER           VgaEn;
  MMIO_ADDR_CTRL            MmioAddrCtrlReg;

  SocketNumber = (UINT8) FabricTopologyGetNumberOfProcessorsPresent ();
  DieNumberPerSocket = (UINT8) FabricTopologyGetNumberOfDiesOnSocket (0);

  // Find out DstFabricIDSysOffset
  DstFabricIDSysOffset = 0xFF;
  if (Target.TgtType == TARGET_PCI_BUS) {
    for (Register = CFGADDRESSMAP0_REG; Register <= CFGADDRESSMAP7_REG; Register += 4) {
      BusMap.Value = FabricRegisterAccRead (0, 0, CFGADDRESSMAP_FUNC, Register, FABRIC_REG_ACC_BC);
      if ((BusMap.Field.RE == 1) && (BusMap.Field.WE == 1) && (BusMap.Field.BusNumLimit >= Target.PciBusNum) && (BusMap.Field.BusNumBase <= Target.PciBusNum)) {
        DstFabricIDSysOffset = (UINT8) BusMap.Field.DstFabricID;
        DstFabricIDSysOffset &= FABRIC_ID_SOCKET_DIE_MASK;
        break;
      }
    }

    if (Register > CFGADDRESSMAP7_REG) {
      return EFI_ABORTED;
    }
    ASSERT (DstFabricIDSysOffset != 0xFF);
  } else {
    DstFabricIDSysOffset = (UINT8) FabricTopologyGetDieSystemOffset (Target.SocketNum, Target.DieNum);
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "  FabricEnableVgaMmio Fabric ID = 0x%X \n", DstFabricIDSysOffset);

  // We have to get Fabric IOMS Instance ID from MMIO control register
  MmioAddrCtrlReg.Value = FabricRegisterAccRead (0, 0, 0x0, (MMIO_ADDRESS_CONTROL_REG_0), FABRIC_REG_ACC_BC);
  for (i = 0; i < SocketNumber; i++) {
    for (j = 0; j < DieNumberPerSocket; j++) {
      VgaEn.Value = FabricRegisterAccRead (i, j, VGA_EN_FUNC, VGA_EN_REG, FABRIC_REG_ACC_BC);
      VgaEn.Field.VgaEn_VE = 1;
      VgaEn.Field.VgaEn_CpuDis = 0;
      VgaEn.Field.VgaEn_DstFabricID = MmioAddrCtrlReg.Field.DstFabricID & 0x1F;
      VgaEn.Field.VgaEn_DstFabricID |= DstFabricIDSysOffset;
      FabricRegisterAccWrite (i, j, VGA_EN_FUNC, VGA_EN_REG, FABRIC_REG_ACC_BC, VgaEn.Value, TRUE);
    }
  }

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *           L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * FabricZenZpGetRemainingSizeForThisRegion
 *
 * Sub-routine of FabricAllocateMmio
 * Get remaining size of one MMIO region
 *
 * @param[in]         MmioRegion                  Point to a data structure which contains MMIO space information
 * @param[in]         Size                        Remaining size
 * @param[in]         Alignment                   Alignment bit map
 * @param[in]         MmioType                    Mmio type
 *
 */
VOID
FabricZenZpGetRemainingSizeForThisRegion (
  IN       FABRIC_MMIO_REGION *MmioRegion,
  IN       UINT64             *Size,
  IN       UINT64              Alignment,
  IN       UINT8               MmioType
  )
{
  UINT64 Base;
  UINT64 BaseAligned;
  UINT64 AlignMask;

  Base = 0;

  if ((MmioType == MMIO_BELOW_4G) || (MmioType == MMIO_ABOVE_4G)) {
    Base = MmioRegion->BaseNonPrefetch + MmioRegion->UsedSizeNonPrefetch;
    *Size = MmioRegion->SizeNonPrefetch - MmioRegion->UsedSizeNonPrefetch;
  } else if ((MmioType == P_MMIO_BELOW_4G) || (MmioType == P_MMIO_ABOVE_4G)) {
    Base = MmioRegion->BasePrefetch + MmioRegion->UsedSizePrefetch;
    *Size = MmioRegion->SizePrefetch - MmioRegion->UsedSizePrefetch;
  } else if (MmioType == NON_PCI_DEVICE_BELOW_4G) {
    Base = MmioRegion->BaseNonPci + MmioRegion->UsedSizeNonPci;
    *Size = MmioRegion->SizeNonPci - MmioRegion->UsedSizeNonPci;
  } else {
    ASSERT (FALSE);
    *Size = 0;
  }

  AlignMask = Alignment;
  BaseAligned = (Base + AlignMask) & (~AlignMask);
  if ((*Size) >= (BaseAligned - Base)) {
    *Size -= BaseAligned - Base;
  } else {
    *Size = 0;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricZenZpGetBaseAddress
 *
 * Sub-routine of FabricAllocateMmio
 * Get base address of a vailable MMIO space, and assign it to caller
 *
 * @param[in]         MmioRegion                  Point to a data structure which contains MMIO space information
 * @param[in]         Length                      Length of MMIO region which is required by user
 * @param[in]         Alignment                   Alignment bit map
 * @param[in]         MmioType                    Mmio type
 *
 */
UINT64
FabricZenZpGetBaseAddress (
  IN       FABRIC_MMIO_REGION *MmioRegion,
  IN       UINT64              Length,
  IN       UINT64              Alignment,
  IN       UINT8               MmioType
  )
{
  UINT64 MmioBaseAddress;
  UINT64 MmioBaseAddressAligned;
  UINT64 MmioRemainingSize;
  UINT64 AlignMask;

  MmioBaseAddress = 0;
  MmioBaseAddressAligned = 0;
  MmioRemainingSize = 0;

  if ((MmioType == MMIO_BELOW_4G) || (MmioType == MMIO_ABOVE_4G)) {
    MmioBaseAddress = MmioRegion->BaseNonPrefetch + MmioRegion->UsedSizeNonPrefetch;
    MmioRemainingSize = MmioRegion->SizeNonPrefetch - MmioRegion->UsedSizeNonPrefetch;
  } else if ((MmioType == P_MMIO_BELOW_4G) || (MmioType == P_MMIO_ABOVE_4G)) {
    MmioBaseAddress = MmioRegion->BasePrefetch + MmioRegion->UsedSizePrefetch;
    MmioRemainingSize = MmioRegion->SizePrefetch - MmioRegion->UsedSizePrefetch;
  } else if (MmioType == NON_PCI_DEVICE_BELOW_4G) {
    MmioBaseAddress = MmioRegion->BaseNonPci + MmioRegion->UsedSizeNonPci;
    MmioRemainingSize = MmioRegion->SizeNonPci - MmioRegion->UsedSizeNonPci;
  }

  if ((MmioBaseAddress != 0) && (MmioRemainingSize != 0)) {
    AlignMask = Alignment;
    MmioBaseAddressAligned = (MmioBaseAddress + AlignMask) & (~AlignMask);
    if (MmioRemainingSize >= (Length + MmioBaseAddressAligned - MmioBaseAddress)) {
      if ((MmioType == MMIO_BELOW_4G) || (MmioType == MMIO_ABOVE_4G)) {
        MmioRegion->UsedSizeNonPrefetch += Length + MmioBaseAddressAligned - MmioBaseAddress;
      } else if ((MmioType == P_MMIO_BELOW_4G) || (MmioType == P_MMIO_ABOVE_4G)) {
        MmioRegion->UsedSizePrefetch += Length + MmioBaseAddressAligned - MmioBaseAddress;
      } else if (MmioType == NON_PCI_DEVICE_BELOW_4G) {
        MmioRegion->UsedSizeNonPci += Length + MmioBaseAddressAligned - MmioBaseAddress;
      }
    } else {
      MmioBaseAddressAligned = 0;
    }
  }

  return MmioBaseAddressAligned;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricGetUsedResourceSize
 *
 * Sub-routine of FabricReallocateResourceForEachDie
 * Get used DF resource size for each Die
 *
 * @param[in, out]    ResourceSizeForEachDie    Used Mmio size for each Die
 *
 */
EFI_STATUS
FabricGetUsedResourceSize (
  IN       FABRIC_RESOURCE_FOR_EACH_DIE    *ResourceSizeForEachDie
  )
{
  UINT8  i;
  UINT8  j;
  UINT8  TempSocket;
  UINT8  TempDie;
  UINT8  SocketNumber;
  UINT8  DieNumberPerSocket;
  LOCATE_HEAP_PTR           LocateHeapParams;
  FABRIC_MMIO_MANAGER      *FabricMmioManager;
  FABRIC_MMIO_REGION       *MmioRegion;
  FABRIC_IO_MANAGER        *FabricIoManager;
  FABRIC_IO_REGION         *IoRegion;

  FabricMmioManager = NULL;
  FabricIoManager = NULL;
  LibAmdMemFill (ResourceSizeForEachDie, 0, sizeof (FABRIC_RESOURCE_FOR_EACH_DIE), NULL);

  LocateHeapParams.BufferHandle = AMD_MMIO_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == AGESA_SUCCESS) {
    FabricMmioManager = (FABRIC_MMIO_MANAGER *) LocateHeapParams.BufferPtr;
  }

  LocateHeapParams.BufferHandle = AMD_IO_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == AGESA_SUCCESS) {
    FabricIoManager = (FABRIC_IO_MANAGER *) LocateHeapParams.BufferPtr;
  }

  SocketNumber = (UINT8) FabricTopologyGetNumberOfProcessorsPresent ();
  DieNumberPerSocket = (UINT8) FabricTopologyGetNumberOfDiesOnSocket (0);

  for (i = 0; i < MAX_SOCKETS_SUPPORTED; i++) {
    for (j = 0; j < MAX_DIES_PER_SOCKET; j++) {
      if ((i >= SocketNumber) || (j >= DieNumberPerSocket)) {
        // Die doesn't exist
        ResourceSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Size = 0;
        ResourceSizeForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Size = 0;
        ResourceSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Size = 0;
        ResourceSizeForEachDie->PrefetchableMmioSizeBelow4G[i][j].Size = 0;
        ResourceSizeForEachDie->IO[i][j].Size = 0;
      } else {
        if ((FabricMmioManager->AllocateMmioAbove4GOnThisDie[i][j]) == FALSE) {
          // No MMIO on this DIE
          ResourceSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Size = 0;
          ResourceSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Size = 0;
        } else {
          MmioRegion = &FabricMmioManager->MmioRegionAbove4G[i][j];
          ResourceSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Size = MmioRegion->UsedSizeNonPrefetch;
          ResourceSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Size = MmioRegion->UsedSizePrefetch;
          IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x Die%x has non-prefetch 0x%lX, prefetchable 0x%lX MMIO above 4G used\n", i, j, ResourceSizeForEachDie->NonPrefetchableMmioSizeAbove4G[i][j].Size, ResourceSizeForEachDie->PrefetchableMmioSizeAbove4G[i][j].Size);
        }

        if ((FabricMmioManager->AllocateMmioBelow4GOnThisDie[i][j]) == FALSE) {
          // No MMIO on this DIE
          ResourceSizeForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Size = 0;
          ResourceSizeForEachDie->PrefetchableMmioSizeBelow4G[i][j].Size = 0;
        } else {
          MmioRegion = &FabricMmioManager->MmioRegionBelow4G[i][j];
          ResourceSizeForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Size = MmioRegion->UsedSizeNonPrefetch;
          ResourceSizeForEachDie->PrefetchableMmioSizeBelow4G[i][j].Size = MmioRegion->UsedSizePrefetch;
          IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x Die%x has non-prefetch 0x%lX, prefetchable 0x%lX MMIO below 4G used\n", i, j, ResourceSizeForEachDie->NonPrefetchableMmioSizeBelow4G[i][j].Size, ResourceSizeForEachDie->PrefetchableMmioSizeBelow4G[i][j].Size);
        }

        IoRegion = &FabricIoManager->IoRegion[i][j];
        ResourceSizeForEachDie->IO[i][j].Size = IoRegion->IoUsed;
        IDS_HDT_CONSOLE (CPU_TRACE, "  Socket%x Die%x has 0x%X IO space used\n", i, j, ResourceSizeForEachDie->IO[i][j].Size);
      }
    }
  }

  // Die0's 2nd MMIO
  if (FabricMmioManager->Die0Has2ndMmioBelow4G) {
    TempSocket = (FabricMmioManager->Die02ndMmioPairBelow4G >> 4) & 0xF;
    TempDie = FabricMmioManager->Die02ndMmioPairBelow4G & 0xF;
    MmioRegion = &FabricMmioManager->MmioRegionBelow4G[TempSocket][TempDie];

    ResourceSizeForEachDie->Die0SecondNonPrefetchableMmioSizeBelow4G.Size = MmioRegion->UsedSizeNonPrefetch;
    ResourceSizeForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Size = MmioRegion->UsedSizePrefetch;
  } else {
    ResourceSizeForEachDie->Die0SecondNonPrefetchableMmioSizeBelow4G.Size = 0;
    ResourceSizeForEachDie->Die0SecondPrefetchableMmioSizeBelow4G.Size = 0;
  }

  return EFI_SUCCESS;
}




