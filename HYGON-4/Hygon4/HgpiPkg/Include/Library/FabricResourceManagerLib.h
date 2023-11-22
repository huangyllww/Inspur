/* $NoKeywords:$ */

/**
 * @file
 *
 * Low-level Fabric MMIO map manager Services library
 *
 * Contains interface to the family specific fabric MMIO map manager library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Lib
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
 **/

#ifndef _FABRIC_RESOURCE_MANAGER_LIB_H_
#define _FABRIC_RESOURCE_MANAGER_LIB_H_

#include "Porting.h"
#include "HYGON.h"
#include <Uefi/UefiBaseType.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>
#pragma pack (push, 1)

/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

/// MMIO Region
typedef struct _FABRIC_MMIO_REGION {
  UINT64    BaseNonPci;          ///< Base address of non-discoverable devices
  UINT64    SizeNonPci;          ///< Total size of non-discoverable devices
  UINT64    UsedSizeNonPci;      ///< Already used size of non-discoverable devices
  UINT64    BaseNonPrefetch;     ///< Base address of non prefetchable
  UINT64    SizeNonPrefetch;     ///< Total size of non prefetchable
  UINT64    UsedSizeNonPrefetch; ///< Already used size of non prefetchable
  UINT64    BasePrefetch;        ///< Base address of prefetchable
  UINT64    SizePrefetch;        ///< Total size of prefetchable
  UINT64    UsedSizePrefetch;    ///< Already used size of prefetchable
  UINT64    AlignNonPrefetch;    ///< Alignment bit map. For example, 0xFFFFF means 1MB alignment
  UINT64    AlignPrefetch;       ///< Alignment bit. For example, 0xFFFFF means 1MB alignment
} FABRIC_MMIO_REGION;

/// MMIO Manager
typedef struct _FABRIC_MMIO_MANAGER {
  FABRIC_MMIO_REGION    MmioRegionAbove4G[MAX_SOCKETS_SUPPORTED][MAX_IOD_PER_SOCKET][MAX_RBS_PER_IOD];  ///< MMIO which is above 4G for Socket[0~7], Die[0~3], Rb[0~3]
  FABRIC_MMIO_REGION    MmioRegionBelow4G[MAX_SOCKETS_SUPPORTED][MAX_IOD_PER_SOCKET][MAX_RBS_PER_IOD];  ///< MMIO which is below 4G for Socket[0~7], Die[0~3], Rb[0~3]
  BOOLEAN               AllocateMmioAbove4GOnThisRb[MAX_SOCKETS_SUPPORTED][MAX_IOD_PER_SOCKET][MAX_RBS_PER_IOD]; ///< Allocate MMIO which is above 4G on this RB
  BOOLEAN               AllocateMmioBelow4GOnThisRb[MAX_SOCKETS_SUPPORTED][MAX_IOD_PER_SOCKET][MAX_RBS_PER_IOD]; ///< Allocate MMIO which is below 4G on this RB
} FABRIC_MMIO_MANAGER;

/// IO Region
typedef struct _FABRIC_IO_REGION {
  UINT32    IoBase;              ///< IO base address
  UINT32    IoSize;              ///< IO size
  UINT32    IoUsed;              ///< IO used
} FABRIC_IO_REGION;

/// IO Manager
typedef struct _FABRIC_IO_MANAGER {
  BOOLEAN             GlobalCtrl;                                                        ///< FALSE - forbid FabricAllocateIo
  FABRIC_IO_REGION    IoRegion[MAX_SOCKETS_SUPPORTED][MAX_IOD_PER_SOCKET][MAX_RBS_PER_IOD]; ///< IO region for Socket[0~7], Die[0~3], Rb[0~3]
} FABRIC_IO_MANAGER;

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */

EFI_STATUS
FabricAllocateMmio (
  IN OUT   UINT64 *BaseAddress,
  IN OUT   UINT64 *Length,
  IN       UINT64                 Alignment,
  IN       FABRIC_TARGET          Target,
  IN OUT   FABRIC_MMIO_ATTRIBUTE *Attributes
  );

EFI_STATUS
FabricAllocateIo (
  IN OUT   UINT32 *BaseAddress,
  IN OUT   UINT32 *Length,
  IN       FABRIC_TARGET          Target
  );

EFI_STATUS
FabricGetAvailableResource (
  IN       FABRIC_RESOURCE_FOR_EACH_RB    *ResourceSizeForEachRb        ///< Get available DF resource size for each Rb
  );

EFI_STATUS
FabricReallocateResourceForEachRb (
  IN       FABRIC_RESOURCE_FOR_EACH_RB    *ResourceSizeForEachRb,   ///< User request
  IN       FABRIC_ADDR_SPACE_SIZE          *SpaceStatus             ///< Report current status
  );

EFI_STATUS
FabricResourceRestoreDefault (
  );

EFI_STATUS
FabricEnableVgaMmio (
  IN       FABRIC_TARGET          Target
  );

#pragma pack (pop)
#endif // _FABRIC_RESOURCE_MANAGER_LIB_H_
