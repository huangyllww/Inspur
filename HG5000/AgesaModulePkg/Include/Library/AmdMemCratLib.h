/* $NoKeywords:$ */
/**
 * @file
 *
 * mfCrat.h
 *
 * Feature CRAT table support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 309090 $ @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 **/
/*****************************************************************************
  *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
  * ***************************************************************************
  *
 */


#ifndef _MFCRAT_H_
#define _MFCRAT_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/// DCT Select Interleaving Address Mapping
typedef struct {
  UINT8                 DctSelIntLvAddr;        ///< F2x110 DctSelIntLvAddr
  UINT8                 DctSelFunctionBit;      ///< Lowest function bit to select DCT for interleaving
} DCTSELBIT_ENTRY;

/// CRAT Memory Affinity Info Header
typedef struct {
  UINT8                 NumOfMemAffinityInfoEntries;  ///< Integer that represents the proximity domain to
  UINT32                MemoryWidth;                  ///< Specifies the number of parallel bits of the memory interface
} CRAT_MEMORY_AFFINITY_INFO_HEADER;

/// CRAT Memory Affinity Info Entry
typedef struct {
  UINT8                 Domain;                 ///< Integer that represents the proximity domain to
                                                ///< which the memory belongs
  UINT32                BaseAddressLow;         ///< Low 32Bits of the Base Address of the memory range
  UINT32                BaseAddressHigh;        ///< High 32Bits of the Base Address of the memory range
  UINT32                LengthLow;              ///< Low 32Bits of the length of the memory range
  UINT32                LengthHigh;             ///< High 32Bits of the length of the memory range
} CRAT_MEMORY_AFFINITY_INFO_ENTRY;

CRAT_MEMORY_AFFINITY_INFO_ENTRY *
MakeMemAffinityInfoEntry (
  IN UINT8                            Domain,
  IN UINT64                           Base,
  IN UINT64                           Size,
  IN CRAT_MEMORY_AFFINITY_INFO_ENTRY  *BufferLocPtr
);

UINT32
MemFCratApproximateSize (
  IN OUT   MEM_NB_BLOCK        *NBPtr
  );

UINT64
GetDctInterleavedMemSize (
  IN OUT   MEM_NB_BLOCK        *NBPtr
  );

UINT64
GetBankInterleavedAddress (
  IN OUT   MEM_NB_BLOCK        *NBPtr,
  IN       UINT64              DctBaseAddress,
  IN       UINT8               InterleavingPosition
  );

UINT64
GetChannelInterleavedAddress (
  IN OUT   MEM_NB_BLOCK        *NBPtr,
  IN       UINT64              NodeBaseAddress,
  IN       UINT8               InterleavingPosition
  );

UINT64
GetBaseAddrOfNonInterleavedRegion (
  IN OUT   MEM_NB_BLOCK        *NBPtr,
  IN       UINT8               DctLimit
  );

BOOLEAN
MemFCratSupport (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );
/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

#endif  /* _MFCRAT_H_ */
