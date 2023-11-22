/* $NoKeywords:$ */
/**
 * @file
 *
 * Nbio CXL 1.1 service Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      
 * @e sub-project:  Nbio
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2022 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * HYGON GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(HYGON) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 *
 ***************************************************************************/
 
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#ifndef __NBIO_CXL_MEM_MANAGER_PROTOCOL_H__
#define __NBIO_CXL_MEM_MANAGER_PROTOCOL_H__

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
// Current PROTOCOL revision
#define HYGON_CXL_MANAGER_REVISION     0x00

#define MAX_CXL_MEM_REGION_SUPPORTED     16
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
typedef struct _HYGON_CXL_MANAGER_PROTOCOL HYGON_CXL_MANAGER_PROTOCOL;

#pragma pack (push, 1)

typedef enum {
  SocketIntlv, //Socket Interleave
  DieIntlv,    //Across IOD interleave
  NbioIntLv,   //Across NBIO interleave
  //CsIntLv,   //Two CS interleave
  NoIntLv,     //No interleave
  MaxDomainType
} CXL_INTLV_TYPE;

typedef enum {
  DF_MEM_INTLV_SIZE_256BYTES = 0,  ///< Interleave size 256 bytes
  DF_MEM_INTLV_SIZE_512BYTES = 1,  ///< Interleave size 512 bytes
  DF_MEM_INTLV_SIZE_1KB = 2,       ///< Interleave size 1024 bytes
  DF_MEM_INTLV_SIZE_2KB = 3,       ///< Interleave size 2048 bytes
  DF_MEM_INTLV_SIZE_4KB = 4,       ///< Interleave size 4096 bytes
  DF_MEM_INTLV_SIZE_8KB = 5,       ///< Interleave size 8192 bytes
  DF_MEM_INTLV_SIZE_16KB = 6,      ///< Interleave size 16384 bytes
  DF_MEM_INTLV_SIZE_AUTO = 7,      ///< Auto
} DF_MEM_INTLV_SIZE;

/// Fabric CXL memory Region
typedef struct _FABRIC_CXL_MEM_REGION {
  UINT16  SocketMap;  ///< BIT0: Socket 0, BIT1: Socket 1... 
  UINT16  DieMap;     ///< BIT0: Logical IO Die 0, BIT1: Logical IO Die 1...
  UINT16  NbioMap;    ///< BIT0: NBIO0, BIT1:NBIO1...
  UINT16  IntlvType;
  UINT64  IntlvSize;
  UINT64  Base;       ///< Base address of CXL memory
  UINT64  Size;       ///< Total size of CXL memory
  UINT64  UsedSize;   ///< Already used size of CXL memory
  UINT64  Alignment;
} FABRIC_CXL_MEM_REGION;

/// MMIO Manager
typedef struct _FABRIC_CXL_MANAGER {
  UINT8                 MemRegionCount;
  FABRIC_CXL_MEM_REGION MemRegion[MAX_CXL_MEM_REGION_SUPPORTED];
} FABRIC_CXL_MANAGER;

/// FABRIC_TARGET.TgtType
#define TARGET_PCI_BUS             0
#define TARGET_RB                  1

/// Alignment
#define ALIGN_256M                 0xFFFFFFF

/// DF target
typedef struct _FABRIC_CXL_TARGET {
  UINT16  PciBusNum:8;         ///< PCI bus number
  UINT16  SocketId:3;          ///< Socket number
  UINT16  DieId:2;             ///< Logical IO Die number
  UINT16  RbId:2;              ///< Logical Rb number
  UINT16  TgtType:1;           ///< Indicator target type
                               ///< 0 - TARGET_PCI_BUS - set up an MMIO region for the device on a certain PCI bus
                               ///< 1 - TARGET_RB     - set up an MMIO region for the device on a certain Socket, DIE
} FABRIC_CXL_TARGET;

/// Fabric CXL memory Region
typedef struct _FABRIC_CXL_MEM_RESOURCE {
  UINT16  SocketMap;  ///< BIT0: Socket 0, BIT1: Socket 1...
  UINT16  DieMap;     ///< BIT0: Logical IO Die 0, BIT1: Logical IO Die 1...
  UINT16  NbioMap;    ///< BIT0: NBIO0, BIT1:NBIO1...
  UINT16  IntlvType;
  UINT64  IntlvSize;
  UINT64  Base;       ///< Base address of CXL memory
  UINT64  Size;       ///< Total size of CXL memory
  UINT64  Alignment;
} FABRIC_CXL_MEM_RESOURCE;

///CXL memory Resource for each RootBridge
typedef struct _FABRIC_CXL_AVAIL_RESOURCE {
  UINT8                    MemRegionCount;
  FABRIC_CXL_MEM_RESOURCE  MemRegion[MAX_CXL_MEM_REGION_SUPPORTED];
} FABRIC_CXL_AVAIL_RESOURCE;

///CXL memory Resource for each RootBridge
typedef struct _FABRIC_CXL_USED_RESOURCE {
  UINT8                    MemRegionCount;
  FABRIC_CXL_MEM_RESOURCE  MemRegion[MAX_CXL_MEM_REGION_SUPPORTED];
} FABRIC_CXL_USED_RESOURCE;

#pragma pack (pop)

/// Function prototype
typedef 
EFI_STATUS 
(EFIAPI *FABRIC_CXL_ALLOCATE_MEM) (
  IN       HYGON_CXL_MANAGER_PROTOCOL       *This,             ///< Protocol Instance
  IN OUT   UINT64                           *HostBaseAddress,  ///< Starting address of the requested CXL memory range in system.
  IN OUT   UINT64                           *HdmBaseAddress,   ///< Starting address of the requested CXL memory range in HDM region.
  IN OUT   UINT64                           *Length,           ///< Length of the requested CXL memory range.
  IN       UINT64                            Alignment,        ///< Alignment bit map. 0xFFFFF means 1MB align
  IN       FABRIC_CXL_TARGET                 Target            ///< Indicator target type
                                                               ///< 0 - TARGET_PCI_BUS
                                                               ///< 1 - TARGET_RB
);

typedef EFI_STATUS (EFIAPI *FABRIC_CXL_GET_AVAILABLE_RESOURCE) (
  IN       HYGON_CXL_MANAGER_PROTOCOL       *This,                            ///< Protocol Instance
  IN       FABRIC_CXL_AVAIL_RESOURCE        *CxlResource                      ///< Get available DF CXL resource size
);

typedef EFI_STATUS (EFIAPI *FABRIC_CXL_GET_USED_RESOURCE) (
  IN       HYGON_CXL_MANAGER_PROTOCOL       *This,                            ///< Protocol Instance
  IN       FABRIC_CXL_USED_RESOURCE         *CxlResource                      ///< Get used DF CXL resource size
);

/// The Protocol Definition for CXL manager
struct _HYGON_CXL_MANAGER_PROTOCOL {
  UINT32                                     Revision;   
  FABRIC_CXL_ALLOCATE_MEM                    FabricCxlAllocateMem;
  FABRIC_CXL_GET_AVAILABLE_RESOURCE          FabricCxlGetAvailableResource;
  FABRIC_CXL_GET_USED_RESOURCE               FabricCxlGetUsedResource;
};

extern EFI_GUID gHygonCxlManagerProtocolGuid;

#endif


