/* $NoKeywords:$ */

/**
 * @file
 *
 * Fabric MMIO map manager Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Fabric
 */
/*****************************************************************************
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

#ifndef _FABRIC_RESOURCE_MANAGER_SERVICES_PROTOCOL_H_
#define _FABRIC_RESOURCE_MANAGER_SERVICES_PROTOCOL_H_
#include "HGPI.h"
#pragma pack (push, 1)

/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */
/// FABRIC_MMIO_ATTRIBUTE.MmioType
#define MMIO_BELOW_4G            0   // Non-Prefetchable MMIO
#define MMIO_ABOVE_4G            1   // Non-Prefetchable MMIO
#define P_MMIO_BELOW_4G          2   // Prefetchable MMIO
#define P_MMIO_ABOVE_4G          3   // Prefetchable MMIO
#define NON_PCI_DEVICE_BELOW_4G  4   // For non-discoverable devices, such as IO APIC, GPIO, MP0/1 mailbox, IOMMU...
#define NON_PCI_DEVICE_ABOVE_4G  5   // For non-discoverable devices, such as IO APIC, GPIO, MP0/1 mailbox, IOMMU...

/// FABRIC_TARGET.TgtType
#define TARGET_PCI_BUS  0
#define TARGET_RB       1

/// Alignment
#define ALIGN_256M  0xFFFFFFF
#define ALIGN_4M    0x3FFFFF
#define ALIGN_1M    0xFFFFF
#define ALIGN_512K  0x7FFFF
#define ALIGN_64K   0x0FFFF
#define ALIGN_8K    0x01FFF
#define ALIGN_4K    0x00FFF

// current Protocol revision
#define HYGON_FABRIC_RESOURCE_PROTOCOL_REV  0x02

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
/// DF target
typedef struct _FABRIC_TARGET {
  UINT16    PciBusNum : 8;      ///< PCI bus number
  UINT16    SocketNum : 3;      ///< Socket number
  UINT16    DieNum    : 2;      ///< Logical Io Die number
  UINT16    RbNum     : 2;      ///< Logical Rb number
  UINT16    TgtType   : 1;      ///< Indicator target type
                                ///< 0 - TARGET_PCI_BUS - set up an MMIO region for the device on a certain PCI bus
                                ///< 1 - TARGET_RB     - set up an MMIO region for the device on a certain Socket, DIE
} FABRIC_TARGET;

/// MMIO attribute
typedef struct _FABRIC_MMIO_ATTRIBUTE {
  UINT8    ReadEnable  : 1;     ///< Indicator whether the range is readable
  UINT8    WriteEnable : 1;     ///< Indicator whether the range is writable
  UINT8    NonPosted   : 1;     ///< Indicator whether the range is posted
  UINT8    CpuDis      : 1;     ///< CPU core memory accesses with ReqIo=1 and address in this MMIO range are directed to compatibility address space.
  UINT8                : 1;     ///< Reserved
  UINT8    MmioType    : 3;     ///< We have 5 pools per Die. 0) MMIO_BELOW_4G 1) MMIO_ABOVE_4G 2) P_MMIO_BELOW_4G 3) P_MMIO_ABOVE_4G 4) NON_PCI_DEVICE_BELOW_4G
                                ///< This function is restricted to support NON_PCI_DEVICE_BELOW_4G only
                                ///< Other 4 pools are supported by FabricGetAvailableResource
} FABRIC_MMIO_ATTRIBUTE;

typedef struct _FABRIC_ADDR_APERTURE {
  UINT64    Base;
  UINT64    Size;
  UINT64    Alignment;          ///< Alignment bit map. 0xFFFFF means 1MB align
} FABRIC_ADDR_APERTURE;

/// Resource for each RootBridge
typedef struct _FABRIC_RESOURCE_FOR_EACH_RB {
  FABRIC_ADDR_APERTURE    NonPrefetchableMmioSizeAbove4G[MAX_SOCKETS_SUPPORTED][MAX_IOD_PER_SOCKET][MAX_RBS_PER_IOD];
  FABRIC_ADDR_APERTURE    PrefetchableMmioSizeAbove4G[MAX_SOCKETS_SUPPORTED][MAX_IOD_PER_SOCKET][MAX_RBS_PER_IOD];
  FABRIC_ADDR_APERTURE    NonPrefetchableMmioSizeBelow4G[MAX_SOCKETS_SUPPORTED][MAX_IOD_PER_SOCKET][MAX_RBS_PER_IOD];
  FABRIC_ADDR_APERTURE    PrefetchableMmioSizeBelow4G[MAX_SOCKETS_SUPPORTED][MAX_IOD_PER_SOCKET][MAX_RBS_PER_IOD];
  FABRIC_ADDR_APERTURE    IO[MAX_SOCKETS_SUPPORTED][MAX_IOD_PER_SOCKET][MAX_RBS_PER_IOD];
} FABRIC_RESOURCE_FOR_EACH_RB;

/// Current status
typedef struct _FABRIC_ADDR_SPACE_SIZE {
  UINT32    IoSize;              ///< IO size required by system resources
  UINT32    IoSizeReqInc;        ///< The amount needed over the current size
  UINT32    MmioSizeBelow4G;     ///< Below 4G Mmio size required by system resources
  UINT32    MmioSizeBelow4GReqInc; ///< The amount needed over the current size
  UINT64    MmioSizeAbove4G;     ///< Above 4G Mmio size required by system resources
  UINT64    MmioSizeAbove4GReqInc; ///< The amount needed over the current size
} FABRIC_ADDR_SPACE_SIZE;

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */
typedef struct _FABRIC_RESOURCE_MANAGER_PROTOCOL FABRIC_RESOURCE_MANAGER_PROTOCOL;

/// Function prototype
typedef EFI_STATUS (EFIAPI *FABRIC_ALLOCATE_MMIO)(
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL *This,         ///< Protocol Instance
  IN OUT   UINT64                           *BaseAddress,  ///< Starting address of the requested MMIO range.
  IN OUT   UINT64                           *Length,       ///< Length of the requested MMIO range.
  IN       UINT64                            Alignment,    ///< Alignment bit map. 0xFFFFF means 1MB align
  IN       FABRIC_TARGET                     Target,       ///< Indicator target type
                                                           ///< 0 - TARGET_PCI_BUS - allocate an MMIO region for the device on a certain PCI bus
                                                           ///< 1 - TARGET_RB     - allocate an MMIO region for the device on a certain Socket, DIE
  IN OUT   FABRIC_MMIO_ATTRIBUTE            *Attributes    ///< We have 5 pools per Die. 0) MMIO_BELOW_4G 1) MMIO_ABOVE_4G 2) P_MMIO_BELOW_4G 3) P_MMIO_ABOVE_4G 4) NON_PCI_DEVICE_BELOW_4G
                                                           ///< This function is restricted to support NON_PCI_DEVICE_BELOW_4G only
                                                           ///< Other 4 pools are supported by FabricGetAvailableResource
  );

/// Function prototype
typedef EFI_STATUS (EFIAPI *FABRIC_ALLOCATE_IO)(
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL *This,         ///< Protocol Instance
  IN OUT   UINT32                           *BaseAddress,  ///< Starting address of the requested IO range.
  IN OUT   UINT32                           *Length,       ///< Length of the requested IO range.
  IN       FABRIC_TARGET                     Target        ///< Indicator target type
                                                           ///< 0 - TARGET_PCI_BUS - allocate an IO region for the device on a certain PCI bus
                                                           ///< 1 - TARGET_RB     - allocate an IO region for the device on a certain Socket, Root bridge
  );

/// Function prototype
typedef EFI_STATUS (EFIAPI *FABRIC_GET_AVAILABLE_RESOURCE)(
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL  *This,                             ///< Protocol Instance
  IN       FABRIC_RESOURCE_FOR_EACH_RB      *ResourceForEachRb                  ///< Get available DF resource size for each Root bridge
  );

/// Function prototype
typedef EFI_STATUS (EFIAPI *FABRIC_REALLOCATE_RESOURCE_FOR_EACH_RB)(
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL  *This,                             ///< Protocol Instance
  IN       FABRIC_RESOURCE_FOR_EACH_RB      *ResourceSizeForEachRb,             ///< Re-allocate DF resource size for each Root bridge
  IN       FABRIC_ADDR_SPACE_SIZE            *SpaceStatus                       ///< Report current status
  );

/// Function prototype
typedef EFI_STATUS (EFIAPI *FABRIC_RESOURCE_RESTORE_DEFAULT)(
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL  *This                              ///< Protocol Instance
  );

/// Function prototype
typedef EFI_STATUS (EFIAPI *FABRIC_ENABLE_VGA_MMIO)(
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL  *This,                             ///< Protocol Instance
  IN       FABRIC_TARGET                     Target                             ///< Indicator target type
                                                                                ///< 0 - TARGET_PCI_BUS
                                                                                ///< 1 - TARGET_RB
  );

/// Fabric resource manager Protocol Structure
struct _FABRIC_RESOURCE_MANAGER_PROTOCOL {
  // See the Forward Declaration above
  UINTN                                     Revision;                           ///< Revision
  FABRIC_ALLOCATE_MMIO                      FabricAllocateMmio;                 ///< Pointer to function
  FABRIC_ALLOCATE_IO                        FabricAllocateIo;                   ///< Pointer to function
  FABRIC_GET_AVAILABLE_RESOURCE             FabricGetAvailableResource;         ///< Pointer to function
  FABRIC_REALLOCATE_RESOURCE_FOR_EACH_RB    FabricReallocateResourceForEachRb;  ///< Pointer to function
  FABRIC_RESOURCE_RESTORE_DEFAULT           FabricResourceRestoreDefault;       ///< Pointer to function
  FABRIC_ENABLE_VGA_MMIO                    FabricEnableVgaMmio;                ///< Pointer to function
};

extern EFI_GUID  gHygonFabricResourceManagerServicesProtocolGuid;
extern EFI_GUID  gHygonResourceSizeForEachRbGuid;
extern EFI_GUID  gHygonResourceDistributionGuid;

#pragma pack (pop)
#endif // _FABRIC_RESOURCE_MANAGER_SERVICES_PROTOCOL_H_
