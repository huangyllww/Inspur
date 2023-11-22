/* $NoKeywords:$ */

/**
 * @file
 *
 * Base Fabric MMIO map manager Lib implementation for DHARMA SAT
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Fabric
 *
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
 */

#ifndef _FABRIC_MMIO_MAP_MANAGER_ST_LIB_H_
#define _FABRIC_MMIO_MAP_MANAGER_ST_LIB_H_

#include "Porting.h"
#include <HGPI.h>
#include <Library/FabricResourceManagerLib.h>

#pragma pack (push, 1)

/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */
#define ST_MMIO_MIN_SIZE                0x10000l
#define ST_NON_PCI_MMIO_ALIGN_MASK      0xFFFFul           // MMIO for non-PCI devices should be 16 bit aligned

#define MMIO_MIN_NON_PCI_SIZE           0x500000           // 5MB is the minimum size of NonPci MMIO pool
#define MMIO_MIN_NON_PCI_SIZE_ABOVE_4G  0x14000000         // 320MB is the minimum size of NonPci MMIO pool Above 4G

#define X86IO_LIMIT                     0x10000            // IO Limit
#define X86_LEGACY_IO_SIZE              0x1000             // IO size which is reserved for legacy devices

#define ST_MAX_SYSTEM_RB_COUNT          16

#define SIZE_16M_ALIGN                  0xFFFFFFFFFF000000l

#define BOTTOM_OF_COMPAT                0xFEC00000ul       // From BOTTOM_OF_COMPAT to 4G would be leaved as undescribed (COMPAT)

#define ADDITIONAL_POSTED_REGION_UNDER_PRIMARY_RB_START  0xFED00000ul
#define ADDITIONAL_POSTED_REGION_UNDER_PRIMARY_RB_END    0xFED0FFFFul

#define FABRIC_ID_SOCKET_DIE_MASK  0x3F0

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

/// MMIO address control
typedef union {
  struct {
    ///< MMIO address control bitfields
    UINT32    RE          : 1; ///< Read enable
    UINT32    WE          : 1; ///< Write enable
    UINT32    CpuDis      : 1; ///< CPU Disable
    UINT32                : 1; ///< Reserved
    UINT32    DstFabricID : 11; ///< Destination FabricID of the IOS, HyEx only 10 bits
    UINT32    NonPosted   : 1;  ///< Non-Posted
    UINT32                : 16; ///< Reserved
  } Field;
  UINT32    Value;
} MMIO_ADDR_CTRL;

/// X86 IO base address
typedef union {
  struct {
    ///< MMIO address control bitfields
    UINT32    RE     : 1;  ///< Read enable
    UINT32    WE     : 1;  ///< Write enable
    UINT32           : 2;  ///< Reserved
    UINT32    VE     : 1;  ///< VE
    UINT32    IE     : 1;  ///< IE
    UINT32           : 6;  ///< Reserved
    UINT32    IOBase : 13; ///< IO Base
    UINT32           : 7;  ///< Reserved
  } Field;
  UINT32    Value;
} X86IO_BASE_ADDR;

/// X86 IO limit address
typedef union {
  struct {
    ///< MMIO address control bitfields
    UINT32    DstFabricID : 11; ///< Destination FabricID of the IOS, HyEx only 10 bits
    UINT32                : 1;  ///< Reserved
    UINT32    IOLimit     : 13; ///< IO Limit
    UINT32                : 7;  ///< Reserved
  } Field;
  UINT32    Value;
} X86IO_LIMIT_ADDR;

typedef struct {
  UINT32    IoBase;
  UINT32    IoLimit;
} DF_IO_RESOURCE;

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
FabricStInitMmioBaseOnNvVariable (
  IN       FABRIC_MMIO_MANAGER *FabricMmioManager,
  IN       FABRIC_RESOURCE_FOR_EACH_RB *MmIoSizeForEachRb,
  IN       FABRIC_ADDR_SPACE_SIZE       *SpaceStatus,
  IN       UINT8                SocketNumber,
  IN       UINT8                IodNumberPerSocket,
  IN       UINT8                RbNumberPerIod,
  IN       BOOLEAN              SetDfRegisters
  );

EFI_STATUS
FabricStInitMmioEqually (
  IN       FABRIC_MMIO_MANAGER *FabricMmioManager,
  IN       UINT8                SocketNumber,
  IN       UINT8                IodNumberPerSocket,
  IN       UINT8                RbNumberPerIod
  );

EFI_STATUS
FabricStInitIoBaseOnNvVariable (
  IN       FABRIC_IO_MANAGER   *FabricIoManager,
  IN       FABRIC_RESOURCE_FOR_EACH_RB *IoSizeForEachRb,
  IN       FABRIC_ADDR_SPACE_SIZE       *SpaceStatus,
  IN       UINT8                SocketNumber,
  IN       UINT8                IodNumberPerSocket,
  IN       UINT8                RbNumberPerIod,
  IN       BOOLEAN              SetDfRegisters
  );

EFI_STATUS
FabricStInitIoEqually (
  IN       FABRIC_IO_MANAGER   *FabricIoManager,
  IN       UINT8                SocketNumber,
  IN       UINT8                IodNumberPerSocket,
  IN       UINT8                RbNumberPerIod
  );

VOID
FabricStAdditionalMmioSetting (
  IN       FABRIC_MMIO_MANAGER *FabricMmioManager,
  IN       UINT8                SocketNumber,
  IN       UINT8                RbNumberPerSocket,
  IN       UINT64               BottomOfCompat,
  IN       BOOLEAN              ReservedRegionAlreadySet
  );

VOID
FabricStSetMmioReg (
  IN       UINT8  TotalSocket,
  IN       UINT8  IodNumberPerSocket,
  IN       UINT8  MmioPairIndex,
  IN       UINT16 DstFabricID,
  IN       UINT64 BaseAddress,
  IN       UINT64 Length
  );

VOID
FabricStSetIoReg (
  IN       UINT8  TotalSocket,
  IN       UINT8  IodNumberPerSocket,
  IN       UINT8  RegIndex,
  IN       UINT16 DstFabricID,
  IN       UINT32 IoBase,
  IN       UINT32 IoSize
  );

#pragma pack (pop)
#endif // _FABRIC_MMIO_MAP_MANAGER_ST_LIB_H_
