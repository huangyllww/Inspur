/* $NoKeywords:$ */
/**
 * @file
 *
 * Base Fabric MMIO map manager Lib implementation for ZEN ZP
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright 2008 - 2016 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
 ******************************************************************************
 */
#ifndef _FABRIC_MMIO_MAP_MANAGER_ZEN_ZP_LIB_H_
#define _FABRIC_MMIO_MAP_MANAGER_ZEN_ZP_LIB_H_

#include "Porting.h"
#include "Library/FabricMmioMapManagerLib.h"

#pragma pack (push, 1)

/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */
#define MMIO_BASE_ADDRESS_REG_0     0x200
#define MMIO_LIMIT_ADDRESS_REG_0    0x204
#define MMIO_ADDRESS_CONTROL_REG_0  0x208

#define ZEN_MAX_SOCKET              2
#define ZEN_MAX_DIE_PER_SOCKET      4

#define ZEN_MMIO_MIN_SIZE           0x10000l

#define SIZE_16M_ALIGN              0xFFFFFFFFFF000000l

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
/// MMIO Region
typedef struct _FABRIC_MMIO_REGION {
  UINT64  Base;                 ///< Base
  UINT64  Size;                 ///< Size
  UINT64  UsedSizeFromTop;      ///< Already used size from top
  UINT64  UsedSizeFromBottom;   ///< Already used size from bottom
} FABRIC_MMIO_REGION;

/// MMIO Manager
typedef struct _FABRIC_MMIO_MANAGER {
  FABRIC_MMIO_REGION MmioRegionAbove4G[ZEN_MAX_SOCKET][ZEN_MAX_DIE_PER_SOCKET];     // MMIO which is above 4G for Socket[0~1], Die[0~3]
  FABRIC_MMIO_REGION MmioRegionBelow4G[ZEN_MAX_SOCKET][ZEN_MAX_DIE_PER_SOCKET];     // MMIO which is below 4G for Socket[0~1], Die[0~3]
  BOOLEAN            AllocateMmioOnThisDie[ZEN_MAX_SOCKET][ZEN_MAX_DIE_PER_SOCKET]; // Allocate MMIO which is below 4G on this DIE?
  BOOLEAN            Die0Has2ndMmioBelow4G;                                         // Indicate if Die 0 has a 2nd MMIO which is below 4G
  UINT8              Die02ndMmioPairBelow4G;                                        // Since AllocateMmioOnThisDie[i][j] is FALSE, use this one for DIE0's 2nd MMIO, Die02ndMmioPair = (i << 4) | j
  BOOLEAN            Die0Has2ndMmioAbove4G;                                         // Indicate if Die 0 has a 2nd MMIO which is above 4G
  UINT8              Die02ndMmioPairAbove4G;                                        // MMIO pair which is used for DIE0 2nd MMIO above 4G
} FABRIC_MMIO_MANAGER;


/// MMIO address control
typedef union {
  struct {
    UINT32 RE:1;          ///< Read enable
    UINT32 WE:1;          ///< Write enable
    UINT32 CpuDis:1;      ///< CPU Disable
    UINT32 :1;            ///< Reserved
    UINT32 DstFabricID:8; ///< Destination FabricID of the IOS
    UINT32 NP:1;          ///< Non-Posted
    UINT32 :19;           ///< Reserved
  } Field;
  UINT32  Value;
} MMIO_ADDR_CTRL;



#pragma pack (pop)
#endif // _FABRIC_MMIO_MAP_MANAGER_ZEN_ZP_LIB_H_
