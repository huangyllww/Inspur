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
#include <Library/HygonHeapLib.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>
#include <Library/FabricResourceSizeForEachRbLib.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include "FabricResourceManagerStLib.h"

#define FILECODE  LIBRARY_FABRICRESOURCEMANAGERSTLIB_FABRICRESOURCEMANAGERSTLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
FabricStGetRemainingSizeForThisRegion (
  IN       FABRIC_MMIO_REGION *MmioRegion,
  IN       UINT64             *Size,
  IN       UINT64              Alignment,
  IN       UINT8               MmioType
  );

UINT64
FabricStGetBaseAddress (
  IN       FABRIC_MMIO_REGION *MmioRegion,
  IN       UINT64              Length,
  IN       UINT64              Alignment,
  IN       UINT8               MmioType
  );

EFI_STATUS
FabricGetUsedResourceSize (
  IN       FABRIC_RESOURCE_FOR_EACH_RB    *ResourceSizeForEachRb
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
  UINT32                    Socket;
  UINT32                    Die;
  UINT32                    Rb;
  UINT16                    DstFabricID;
  UINTN                     Register;
  UINT64                    SizeA;
  UINT64                    SizeB;
  CFG_ADDRESS_MAP_REGISTER  BusMap;
  LOCATE_HEAP_PTR           LocateHeapParams;
  FABRIC_MMIO_MANAGER       *FabricMmioManager;
  FABRIC_MMIO_REGION        *MmioRegion;
  EFI_STATUS                ReturnStatus;
  HGPI_STATUS               Status;

  ReturnStatus = EFI_SUCCESS;
  *BaseAddress = 0;
  FabricMmioManager = NULL;
  // Find MMIO manager in heap
  LocateHeapParams.BufferHandle = HYGON_MMIO_MANAGER;

  Status = HeapLocateBuffer (&LocateHeapParams, NULL);
  FabricMmioManager = (FABRIC_MMIO_MANAGER *)LocateHeapParams.BufferPtr;
  ASSERT (FabricMmioManager != NULL);
  if ((Status != HGPI_SUCCESS) || (LocateHeapParams.BufferPtr == NULL)) {
    return EFI_OUT_OF_RESOURCES;
  }

  // Check input parameters
  if (*Length == 0) {
    return EFI_SUCCESS;
  }

  // Find out Socket/Rb
  DstFabricID = 0xFFFF;
  if (Target.TgtType == TARGET_PCI_BUS) {
    for (Register = CFGADDRESSMAP0_REG; Register <= CFGADDRESSMAP15_REG; Register += 4) {
      BusMap.Value = FabricRegisterAccRead (0, 0, CFGADDRESSMAP_FUNC, Register, FABRIC_REG_ACC_BC);
      IDS_HDT_CONSOLE (CPU_TRACE, "  Bus Cfg Register 0x%X value is 0x%X\n", Register, BusMap.Value);

      if ((BusMap.Field.RE == 1) && (BusMap.Field.WE == 1) && (BusMap.Field.BusNumLimit >= Target.PciBusNum) && (BusMap.Field.BusNumBase <= Target.PciBusNum)) {
        DstFabricID = (UINT16)BusMap.Field.DstFabricID;
        break;
      }
    }

    IDS_HDT_CONSOLE (CPU_TRACE, "  Found bus cfg Register = 0x%X\n", Register);
    if (Register > CFGADDRESSMAP15_REG) {
      return EFI_ABORTED;
    }

    ASSERT (DstFabricID != 0xFFFF);
    FabricTopologyGetLocationFromFabricId (DstFabricID, &Socket, &Die, &Rb);
    IDS_HDT_CONSOLE (CPU_TRACE, "  Allocate Mmio for Socket %d Die %d Rb %d \n", Socket, Die, Rb);
  } else {
    Socket = (UINT8)(Target.SocketNum);
    Die    = (UINT8)(Target.DieNum);
    Rb     = (UINT8)(Target.RbNum);
  }

  SizeA = 0;
  SizeB = 0;
  if ((Attributes->MmioType == MMIO_BELOW_4G) ||
      (Attributes->MmioType == P_MMIO_BELOW_4G) ||
      (Attributes->MmioType == NON_PCI_DEVICE_BELOW_4G)) {
    //
    // Below 4G
    //

    // return EFI_ABORTED if no MMIO on this DIE
    if (FabricMmioManager->AllocateMmioBelow4GOnThisRb[Socket][Die][Rb] == FALSE) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  ERROR: No below 4G MMIO on Socket %X Rb %X\n", Socket, Rb);
      return EFI_ABORTED;
    }

    // Find out MmioRegion for this Die
    MmioRegion = &FabricMmioManager->MmioRegionBelow4G[Socket][Die][Rb];
    FabricStGetRemainingSizeForThisRegion (MmioRegion, &SizeA, Alignment, Attributes->MmioType);

    *BaseAddress = FabricStGetBaseAddress (MmioRegion, *Length, Alignment, Attributes->MmioType);
  } else if ((Attributes->MmioType == MMIO_ABOVE_4G) ||
             (Attributes->MmioType == P_MMIO_ABOVE_4G) ||
             (Attributes->MmioType == NON_PCI_DEVICE_ABOVE_4G)) {
    //
    // Above 4G
    //
    // return EFI_ABORTED if no MMIO on this RB
    if (FabricMmioManager->AllocateMmioAbove4GOnThisRb[Socket][Die][Rb] == FALSE) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  ERROR: No above 4G MMIO on Socket %X Rb %X\n", Socket, Rb);
      return EFI_ABORTED;
    }

    MmioRegion = &FabricMmioManager->MmioRegionAbove4G[Socket][Die][Rb];
    FabricStGetRemainingSizeForThisRegion (MmioRegion, &SizeA, Alignment, Attributes->MmioType);
    if (SizeA >= (*Length)) {
      // Have enough space
      *BaseAddress = FabricStGetBaseAddress (MmioRegion, *Length, Alignment, Attributes->MmioType);
    }
  }

  if (*BaseAddress == 0) {
    *Length = (SizeA > SizeB) ? SizeA : SizeB;
    *BaseAddress = 0;
    ReturnStatus = EFI_OUT_OF_RESOURCES;
  }

  if (ReturnStatus == EFI_SUCCESS) {
    Attributes->NonPosted   = 0;
    Attributes->ReadEnable  = 1;
    Attributes->WriteEnable = 1;
    Attributes->CpuDis = 0;
    IDS_HDT_CONSOLE (CPU_TRACE, "  Allocate MMIO from 0x%lX ~ 0x%lX\n", *BaseAddress, (*BaseAddress + *Length - 1));
  } else if (ReturnStatus == EFI_OUT_OF_RESOURCES) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  No enough space, the biggest MMIO size is 0x%X0000\n", (UINT32)(*Length >> 16));
    // FabricAllocateMmio is restricted for NON_PCI_DEVICE_BELOW_4G only
    // So if there's no enough space, ASSERT here
    if (Attributes->MmioType == NON_PCI_DEVICE_BELOW_4G) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  Please increase PcdHygonMmioSizePerRbForNonPciDevice\n");
      ASSERT (FALSE);
    } else if (Attributes->MmioType == NON_PCI_DEVICE_ABOVE_4G) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  Please increase PcdHygonMmioSizePerRbForNonPciDeviceAbove4G\n");
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
  UINT32                    Socket;
  UINT32                    Die;
  UINT32                    Rb;
  UINT16                    DstFabricID;
  UINTN                     Register;
  CFG_ADDRESS_MAP_REGISTER  BusMap;
  LOCATE_HEAP_PTR           LocateHeapParams;
  FABRIC_IO_MANAGER         *FabricIoManager;
  FABRIC_IO_REGION          *IoRegion;
  EFI_STATUS                ReturnStatus;

  ReturnStatus = EFI_SUCCESS;

  FabricIoManager = NULL;
  // Find MMIO manager in heap
  LocateHeapParams.BufferHandle = HYGON_IO_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == HGPI_SUCCESS) {
    FabricIoManager = (FABRIC_IO_MANAGER *)LocateHeapParams.BufferPtr;
  }

  ASSERT (FabricIoManager != NULL);

  if (!FabricIoManager->GlobalCtrl) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  Error : FabricIoManager->GlobalCtrl is false ! \n");
    return EFI_OUT_OF_RESOURCES;
  }

  // Find out Socket/Die
  DstFabricID = 0xFFFF;
  if (Target.TgtType == TARGET_PCI_BUS) {
    for (Register = CFGADDRESSMAP0_REG; Register <= CFGADDRESSMAP15_REG; Register += 4) {
      BusMap.Value = FabricRegisterAccRead (0, 0, CFGADDRESSMAP_FUNC, Register, FABRIC_REG_ACC_BC);
      if ((BusMap.Field.RE == 1) && (BusMap.Field.WE == 1) && (BusMap.Field.BusNumLimit >= Target.PciBusNum) && (BusMap.Field.BusNumBase <= Target.PciBusNum)) {
        DstFabricID = (UINT16)BusMap.Field.DstFabricID;
        break;
      }
    }

    if (Register > CFGADDRESSMAP15_REG) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  Error : Bus not found ! \n");
      return EFI_ABORTED;
    }

    ASSERT (DstFabricID != 0xFFFF);
    FabricTopologyGetLocationFromFabricId (DstFabricID, &Socket, &Die, &Rb);
  } else {
    Socket = (UINT8)(Target.SocketNum);
    Die    = (UINT8)(Target.DieNum);
    Rb     = (UINT8)(Target.RbNum);
  }

  //
  IoRegion = &FabricIoManager->IoRegion[Socket][Die][Rb];
  if (IoRegion->IoSize >= (*Length + IoRegion->IoUsed)) {
    *BaseAddress      = IoRegion->IoBase + IoRegion->IoSize - IoRegion->IoUsed - *Length;
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
 * Get available DF resource (MMIO/IO) for each Rb
 *
 * @param[in, out]    ResourceSizeForEachRb     Avaiable DF resource (MMIO/IO) for each Rb
 *
 * @retval            EFI_SUCCESS           Success to get available resource
 *                    EFI_ABORTED           Can't get information of MMIO or IO
 */
EFI_STATUS
FabricGetAvailableResource (
  IN       FABRIC_RESOURCE_FOR_EACH_RB    *ResourceSizeForEachRb
  )
{
  UINT8                i;
  UINT8                j;
  UINT8                k;
  UINT8                SocketNumber;
  UINT8                IodNumberPerSocket;
  UINT8                RbsNumberPerIod;
  LOCATE_HEAP_PTR      LocateHeapParams;
  FABRIC_MMIO_MANAGER  *FabricMmioManager;
  FABRIC_MMIO_REGION   *MmioRegion;
  FABRIC_IO_MANAGER    *FabricIoManager;
  FABRIC_IO_REGION     *IoRegion;

  FabricMmioManager = NULL;
  FabricIoManager   = NULL;

  LocateHeapParams.BufferHandle = HYGON_MMIO_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == HGPI_SUCCESS) {
    FabricMmioManager = (FABRIC_MMIO_MANAGER *)LocateHeapParams.BufferPtr;
  }

  LocateHeapParams.BufferHandle = HYGON_IO_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == HGPI_SUCCESS) {
    FabricIoManager = (FABRIC_IO_MANAGER *)LocateHeapParams.BufferPtr;
  }

  if ((FabricMmioManager == NULL) && (FabricIoManager == NULL)) {
    return EFI_ABORTED;
  }

  SocketNumber = (UINT8)FabricTopologyGetNumberOfSocketPresent ();
  IodNumberPerSocket = (UINT8)FabricTopologyGetNumberOfLogicalDiesOnSocket (0);
  RbsNumberPerIod    = (UINT8)FabricTopologyGetNumberOfRootBridgesOnDie (0, 0);

  for (i = 0; i < MAX_SOCKETS_SUPPORTED; i++) {
    for (j = 0; j < MAX_IOD_PER_SOCKET; j++) {
      for (k = 0; k < MAX_RBS_PER_IOD; k++) {
        if ((i >= SocketNumber) || (j >= IodNumberPerSocket) || (k >= RbsNumberPerIod)) {
          // Rb doesn't exist
          ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Size = 0;
          ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Size = 0;
          ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Size    = 0;
          ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Size    = 0;
          ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Base = 0;
          ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Base = 0;
          ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Base    = 0;
          ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Base    = 0;
          ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Alignment = 0;
          ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Alignment    = 0;
          ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Alignment = 0;
          ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Alignment    = 0;

          ResourceSizeForEachRb->IO[i][j][k].Size = 0;
          ResourceSizeForEachRb->IO[i][j][k].Base = 0;
        } else {
          if ((FabricMmioManager->AllocateMmioAbove4GOnThisRb[i][j][k]) == FALSE) {
            // No MMIO on this Rb
            ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Size = 0;
            ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Size    = 0;
            ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Base = 0;
            ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Base    = 0;
            ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Alignment = 0;
            ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Alignment    = 0;
          } else {
            // Only report prefetchable & non-prefetchable MMIO size
            MmioRegion = &FabricMmioManager->MmioRegionAbove4G[i][j][k];
            ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Size = MmioRegion->SizeNonPrefetch - MmioRegion->UsedSizeNonPrefetch;
            ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Size    = MmioRegion->SizePrefetch - MmioRegion->UsedSizePrefetch;
            ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Base = MmioRegion->BaseNonPrefetch + MmioRegion->UsedSizeNonPrefetch;
            ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Base    = MmioRegion->BasePrefetch + MmioRegion->UsedSizePrefetch;
            ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Alignment = MmioRegion->AlignNonPrefetch;
            ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Alignment    = MmioRegion->AlignPrefetch;
            IDS_HDT_CONSOLE (CPU_TRACE, "  Socket %d Iod %d Rb %d has non-prefetch base 0x%lX size 0x%lX align %x MMIO above 4G available\n", i, j, k, ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Base, ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Size, ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Alignment);
            IDS_HDT_CONSOLE (CPU_TRACE, "  Socket %d Iod %d Rb %d has prefetchable base 0x%lX size 0x%lX align %x MMIO above 4G available\n", i, j, k, ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Base, ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Size, ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Alignment);
          }

          if ((FabricMmioManager->AllocateMmioBelow4GOnThisRb[i][j][k]) == FALSE) {
            // No MMIO on this RB
            ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Size = 0;
            ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Size    = 0;
            ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Base = 0;
            ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Base    = 0;
            ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Alignment = 0;
            ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Alignment    = 0;
          } else {
            // Only report prefetchable & non-prefetchable MMIO size
            MmioRegion = &FabricMmioManager->MmioRegionBelow4G[i][j][k];
            ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Size = MmioRegion->SizeNonPrefetch - MmioRegion->UsedSizeNonPrefetch;
            ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Size    =  MmioRegion->SizePrefetch - MmioRegion->UsedSizePrefetch;
            ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Base = MmioRegion->BaseNonPrefetch + MmioRegion->UsedSizeNonPrefetch;
            ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Base    =  MmioRegion->BasePrefetch + MmioRegion->UsedSizePrefetch;
            ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Alignment = MmioRegion->AlignNonPrefetch;
            ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Alignment    = MmioRegion->AlignPrefetch;
            IDS_HDT_CONSOLE (CPU_TRACE, "  Socket %d Iod %d Rb %d has non-prefetch base 0x%lX size 0x%lX align %x MMIO below 4G available\n", i, j, k, ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Base, ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Size, ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Alignment);
            IDS_HDT_CONSOLE (CPU_TRACE, "  Socket %d Iod %d Rb %d has prefetchable base 0x%lX size 0x%lX align %x MMIO below 4G available\n", i, j, k, ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Base, ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Size, ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Alignment);
          }

          IoRegion = &FabricIoManager->IoRegion[i][j][k];
          ResourceSizeForEachRb->IO[i][j][k].Base = IoRegion->IoBase;
          ResourceSizeForEachRb->IO[i][j][k].Size = IoRegion->IoSize - IoRegion->IoUsed;
          IDS_HDT_CONSOLE (CPU_TRACE, "  Socket %d Iod %d Rb %d has IO base 0x%X size 0x%X\n", i, j, k, ResourceSizeForEachRb->IO[i][j][k].Base, ResourceSizeForEachRb->IO[i][j][k].Size);
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/

/**
 * FabricReallocateResourceForEachRb
 *
 * ResourceSize = UsedSize + UserRequestSize (input)
 * Save ResourceSize to NV variable
 *
 * @param[in, out]    ResourceSizeForEachRb     Avaiable DF resource (MMIO/IO) size for each Rb
 * @param[in, out]    SpaceStatus               Current status
 *
 * @retval            EFI_SUCCESS               Save user's request to NV variable successfully
 *                    EFI_OUT_OF_RESOURCES      No enough resource
 */
EFI_STATUS
FabricReallocateResourceForEachRb (
  IN       FABRIC_RESOURCE_FOR_EACH_RB      *ResourceSizeForEachRb,
  IN       FABRIC_ADDR_SPACE_SIZE            *SpaceStatus
  )
{
  UINT8                        i;
  UINT8                        j;
  UINT8                        k;
  UINT8                        SocketNumber;
  UINT8                        IodNumberPerSocket;
  UINT8                        RbsNumberPerIod;
  EFI_STATUS                   Status;
  EFI_STATUS                   CalledStatus;
  FABRIC_RESOURCE_FOR_EACH_RB  CurrentUsed;

  SocketNumber = (UINT8)FabricTopologyGetNumberOfSocketPresent ();
  IodNumberPerSocket = (UINT8)FabricTopologyGetNumberOfLogicalDiesOnSocket (0);
  RbsNumberPerIod    = (UINT8)FabricTopologyGetNumberOfRootBridgesOnDie (0, 0);

  for (i = 0; i < MAX_SOCKETS_SUPPORTED; i++) {
    for (j = 0; j < MAX_IOD_PER_SOCKET; j++) {
      for (k = 0; k < MAX_RBS_PER_IOD; k++) {
        if ((i >= SocketNumber) || (j >= IodNumberPerSocket) || (k >= RbsNumberPerIod)) {
          // Rb doesn't exist
          ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Size = 0;
          ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Size = 0;
          ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Size    = 0;
          ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Size    = 0;
          ResourceSizeForEachRb->IO[i][j][k].Size = 0;

          ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Alignment = 0;
          ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Alignment = 0;
          ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Alignment    = 0;
          ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Alignment    = 0;
          continue;
        }
      }
    }
  }

  // Get how much space is used
  FabricGetUsedResourceSize (&CurrentUsed);

  Status = FabricSetResourceSizeForEachRb (&CurrentUsed, ResourceSizeForEachRb);

  if (SpaceStatus != NULL) {
    LibHygonMemFill (SpaceStatus, 0, sizeof (FABRIC_ADDR_SPACE_SIZE), NULL);
  }

  CalledStatus = FabricStInitMmioBaseOnNvVariable (NULL, ResourceSizeForEachRb, SpaceStatus, SocketNumber, IodNumberPerSocket, RbsNumberPerIod, FALSE);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  CalledStatus = FabricStInitIoBaseOnNvVariable (NULL, ResourceSizeForEachRb, SpaceStatus, SocketNumber, IodNumberPerSocket, RbsNumberPerIod, FALSE);
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
  FabricClearResourceSizeForEachRb ();

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
  UINT8                     i;
  UINT8                     SocketNumber;
  UINT16                    DstFabricID;
  UINTN                     Register;
  CFG_ADDRESS_MAP_REGISTER  BusMap;
  VGA_EN_REGISTER           VgaEn;
  MMIO_ADDR_CTRL            MmioAddrCtrlReg;

  SocketNumber = (UINT8)FabricTopologyGetNumberOfSocketPresent ();

  // Find out DstFabricID
  DstFabricID = 0xFFFF;
  if (Target.TgtType == TARGET_PCI_BUS) {
    for (Register = CFGADDRESSMAP0_REG; Register <= CFGADDRESSMAP15_REG; Register += 4) {
      BusMap.Value = FabricRegisterAccRead (0, 0, CFGADDRESSMAP_FUNC, Register, FABRIC_REG_ACC_BC);
      if ((BusMap.Field.RE == 1) && (BusMap.Field.WE == 1) && (BusMap.Field.BusNumLimit >= Target.PciBusNum) && (BusMap.Field.BusNumBase <= Target.PciBusNum)) {
        DstFabricID = (UINT16)BusMap.Field.DstFabricID;
        break;
      }
    }

    if (Register > CFGADDRESSMAP15_REG) {
      return EFI_ABORTED;
    }

    ASSERT (DstFabricID != 0xFFFF);
  } else {
    DstFabricID = (UINT16)FabricTopologyGetHostBridgeSystemFabricID (Target.SocketNum, Target.DieNum, Target.RbNum);
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "  FabricEnableVgaMmio Fabric ID = 0x%X \n", DstFabricID);

  // We have to get Fabric IOMS Instance ID from MMIO control register
  MmioAddrCtrlReg.Value = FabricRegisterAccRead (0, 0, MMIO_SPACE_FUNC, (MMIO_ADDRESS_CONTROL_REG_0), FABRIC_REG_ACC_BC);
  for (i = 0; i < SocketNumber; i++) {
    VgaEn.Value = FabricRegisterAccRead (i, 0, VGA_EN_FUNC, VGA_EN_REG, FABRIC_REG_ACC_BC);
    VgaEn.Field.VgaEn_VE     = 1;
    VgaEn.Field.VgaEn_CpuDis = 0;
    VgaEn.Field.VgaEn_DstFabricID = DstFabricID;
    FabricRegisterAccWrite (i, 0, VGA_EN_FUNC, VGA_EN_REG, FABRIC_REG_ACC_BC, VgaEn.Value, TRUE);
  }

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *           L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/

/**
 * FabricStGetRemainingSizeForThisRegion
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
FabricStGetRemainingSizeForThisRegion (
  IN       FABRIC_MMIO_REGION *MmioRegion,
  IN       UINT64             *Size,
  IN       UINT64              Alignment,
  IN       UINT8               MmioType
  )
{
  UINT64  Base;
  UINT64  BaseAligned;
  UINT64  AlignMask;

  Base = 0;

  if ((MmioType == MMIO_BELOW_4G) || (MmioType == MMIO_ABOVE_4G)) {
    Base  = MmioRegion->BaseNonPrefetch + MmioRegion->UsedSizeNonPrefetch;
    *Size = MmioRegion->SizeNonPrefetch - MmioRegion->UsedSizeNonPrefetch;
  } else if ((MmioType == P_MMIO_BELOW_4G) || (MmioType == P_MMIO_ABOVE_4G)) {
    Base  = MmioRegion->BasePrefetch + MmioRegion->UsedSizePrefetch;
    *Size = MmioRegion->SizePrefetch - MmioRegion->UsedSizePrefetch;
  } else if ((MmioType == NON_PCI_DEVICE_BELOW_4G) || (MmioType == NON_PCI_DEVICE_ABOVE_4G)) {
    Base  = MmioRegion->BaseNonPci + MmioRegion->UsedSizeNonPci;
    *Size = MmioRegion->SizeNonPci - MmioRegion->UsedSizeNonPci;
  } else {
    ASSERT (FALSE);
    *Size = 0;
  }

  AlignMask   = Alignment;
  BaseAligned = (Base + AlignMask) & (~AlignMask);
  if ((*Size) >= (BaseAligned - Base)) {
    *Size -= BaseAligned - Base;
  } else {
    *Size = 0;
  }
}

/*---------------------------------------------------------------------------------------*/

/**
 * FabricStGetBaseAddress
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
FabricStGetBaseAddress (
  IN       FABRIC_MMIO_REGION *MmioRegion,
  IN       UINT64              Length,
  IN       UINT64              Alignment,
  IN       UINT8               MmioType
  )
{
  UINT64  MmioBaseAddress;
  UINT64  MmioBaseAddressAligned;
  UINT64  MmioRemainingSize;
  UINT64  AlignMask;

  MmioBaseAddress = 0;
  MmioBaseAddressAligned = 0;
  MmioRemainingSize = 0;

  if ((MmioType == MMIO_BELOW_4G) || (MmioType == MMIO_ABOVE_4G)) {
    MmioBaseAddress   = MmioRegion->BaseNonPrefetch + MmioRegion->UsedSizeNonPrefetch;
    MmioRemainingSize = MmioRegion->SizeNonPrefetch - MmioRegion->UsedSizeNonPrefetch;
  } else if ((MmioType == P_MMIO_BELOW_4G) || (MmioType == P_MMIO_ABOVE_4G)) {
    MmioBaseAddress   = MmioRegion->BasePrefetch + MmioRegion->UsedSizePrefetch;
    MmioRemainingSize = MmioRegion->SizePrefetch - MmioRegion->UsedSizePrefetch;
  } else if ((MmioType == NON_PCI_DEVICE_BELOW_4G) || (MmioType == NON_PCI_DEVICE_ABOVE_4G)) {
    MmioBaseAddress   = MmioRegion->BaseNonPci + MmioRegion->UsedSizeNonPci;
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
      } else if ((MmioType == NON_PCI_DEVICE_BELOW_4G) || (MmioType == NON_PCI_DEVICE_ABOVE_4G)) {
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
 * Sub-routine of FabricReallocateResourceForEachRb
 * Get used DF resource size for each Rb
 *
 * @param[in, out]    ResourceSizeForEachRb    Used Mmio size for each Rb
 *
 */
EFI_STATUS
FabricGetUsedResourceSize (
  IN       FABRIC_RESOURCE_FOR_EACH_RB    *ResourceSizeForEachRb
  )
{
  UINT8                i;
  UINT8                j;
  UINT8                k;
  UINT8                SocketNumber;
  UINT8                IodNumberPerSocket;
  UINT8                RbNumberPerIod;
  LOCATE_HEAP_PTR      LocateHeapParams;
  FABRIC_MMIO_MANAGER  *FabricMmioManager;
  FABRIC_MMIO_REGION   *MmioRegion;
  FABRIC_IO_MANAGER    *FabricIoManager;
  FABRIC_IO_REGION     *IoRegion;

  FabricMmioManager = NULL;
  FabricIoManager   = NULL;
  LibHygonMemFill (ResourceSizeForEachRb, 0, sizeof (FABRIC_RESOURCE_FOR_EACH_RB), NULL);

  LocateHeapParams.BufferHandle = HYGON_MMIO_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == HGPI_SUCCESS) {
    FabricMmioManager = (FABRIC_MMIO_MANAGER *)LocateHeapParams.BufferPtr;
  }

  LocateHeapParams.BufferHandle = HYGON_IO_MANAGER;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == HGPI_SUCCESS) {
    FabricIoManager = (FABRIC_IO_MANAGER *)LocateHeapParams.BufferPtr;
  }

  SocketNumber = (UINT8)FabricTopologyGetNumberOfSocketPresent ();
  IodNumberPerSocket = (UINT8)FabricTopologyGetNumberOfLogicalDiesOnSocket (0);
  RbNumberPerIod     = (UINT8)FabricTopologyGetNumberOfRootBridgesOnDie (0, 0);

  for (i = 0; i < MAX_SOCKETS_SUPPORTED; i++) {
    for (j = 0; j < MAX_IOD_PER_SOCKET; j++) {
      for (k = 0; k < MAX_RBS_PER_IOD; k++) {
        if ((i >= SocketNumber) || (j >= IodNumberPerSocket) || (k >= RbNumberPerIod)) {
          // Root bridge doesn't exist
          ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Size = 0;
          ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Size = 0;
          ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Size    = 0;
          ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Size    = 0;
          ResourceSizeForEachRb->IO[i][j][k].Size = 0;
        } else {
          if ((FabricMmioManager->AllocateMmioAbove4GOnThisRb[i][j][k]) == FALSE) {
            // No MMIO on this Root bridge
            ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Size = 0;
            ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Size    = 0;
          } else {
            MmioRegion = &FabricMmioManager->MmioRegionAbove4G[i][j][k];
            ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Size = MmioRegion->UsedSizeNonPrefetch;
            ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Size    = MmioRegion->UsedSizePrefetch;
            IDS_HDT_CONSOLE (CPU_TRACE, "  Socket %d Iod %d Rb %d has non-prefetch 0x%lX, prefetchable 0x%lX MMIO above 4G used\n", i, j, k, ResourceSizeForEachRb->NonPrefetchableMmioSizeAbove4G[i][j][k].Size, ResourceSizeForEachRb->PrefetchableMmioSizeAbove4G[i][j][k].Size);
          }

          if ((FabricMmioManager->AllocateMmioBelow4GOnThisRb[i][j][k]) == FALSE) {
            // No MMIO on this Root bridge
            ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Size = 0;
            ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Size    = 0;
          } else {
            MmioRegion = &FabricMmioManager->MmioRegionBelow4G[i][j][k];
            ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Size = MmioRegion->UsedSizeNonPrefetch;
            ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Size    = MmioRegion->UsedSizePrefetch;
            IDS_HDT_CONSOLE (CPU_TRACE, "  Socket %d Iod %d Rb %d has non-prefetch 0x%lX, prefetchable 0x%lX MMIO below 4G used\n", i, j, k, ResourceSizeForEachRb->NonPrefetchableMmioSizeBelow4G[i][j][k].Size, ResourceSizeForEachRb->PrefetchableMmioSizeBelow4G[i][j][k].Size);
          }

          IoRegion = &FabricIoManager->IoRegion[i][j][k];
          ResourceSizeForEachRb->IO[i][j][k].Size = IoRegion->IoUsed;
          IDS_HDT_CONSOLE (CPU_TRACE, "  Socket %d Iod %d Rb %d has 0x%X IO space used\n", i, j, k, ResourceSizeForEachRb->IO[i][j][k].Size);
        }
      }
    }
  }

  return EFI_SUCCESS;
}
