/* $NoKeywords:$ */
/**
 * @file
 *
 *
 * AMD Ccx Microcode Patch Library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 312770 $   @e \$Date: 2015-02-11 13:24:42 +0800 (Wed, 11 Feb 2015) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 **/

#ifndef _CCX_MICROCODE_PATCH_LIB_H_
#define _CCX_MICROCODE_PATCH_LIB_H_
#include "cpuRegisters.h"
#pragma pack (push, 1)

#define MSR_PATCH_LEVEL  0x0000008Bul
#define MSR_PATCH_LOADER 0xC0010020ul
#define AMD_CPUID_FMF    0x80000001ul

/// Microcode patch field definitions
typedef struct {
  UINT32   DateCode;                   ///< Date of patch creation
  UINT32   PatchID;                    ///< Patch level
  UINT16   MicrocodePatchDataID;       ///< Internal use only
  UINT8    MicrocodePatchDataLength;   ///< Internal use only
  UINT8    InitializationFlag;         ///< Internal use only
  UINT32   MicrocodePatchDataChecksum; ///< Doubleword sum of data block
  UINT32   Chipset1DeviceID;           ///< Device ID of 1st Chipset device to match
  UINT32   Chipset2DeviceID;           ///< Device ID of 2nd Chipset device to match
  UINT16   ProcessorRevisionID;        ///< Equivalent ID
  UINT8    Chipset1RevisionID;         ///< Revision level of 1st Chipset device to match
  UINT8    Chipset2RevisionID;         ///< Revision level of 2nd Chipset device to match
  UINT8    BiosApiRevision;            ///< BIOS INT 15 API revision required
  UINT8    Reserved1[3];               ///< Reserved
  UINT32   MatchRegister0;             ///< Internal use only
  UINT32   MatchRegister1;             ///< Internal use only
  UINT32   MatchRegister2;             ///< Internal use only
  UINT32   MatchRegister3;             ///< Internal use only
  UINT32   MatchRegister4;             ///< Internal use only
  UINT32   MatchRegister5;             ///< Internal use only
  UINT32   MatchRegister6;             ///< Internal use only
  UINT32   MatchRegister7;             ///< Internal use only
  UINT8    PatchDataBlock[896];        ///< Raw patch data
  UINT8    Reserved2[896];             ///< Reserved
  UINT8    X86CodePresent;             ///< Boolean to determine if executable code exists
  UINT8    X86CodeEntry[191];          ///< Code to execute if X86CodePresent != 0
} MICROCODE_PATCH;

/// Patch Loader Register
typedef struct {
  UINT64 PatchBase:32;                ///< Linear address of patch header address block
  UINT64 SBZ:32;                      ///< Should be zero
} PATCH_LOADER_MSR;

/// Patch Loader
typedef union {
  UINT64           RawData;           ///< Raw Data
  PATCH_LOADER_MSR BitFields;         ///< BitFields
} PATCH_LOADER;



/* -----------------------------------------------------------------------------*/
/**
 *  Update microcode patch in current processor.
 *
 *  Then reads the patch id, and compare it to the expected, in the Microprocessor
 *  patch block.
 *
 *  @param[in] MicrocodeEquivalenceTable - Pointer to the Equivalence table.
 *  @param[in] EquivalencyEntries        - Number of valid entries in the Equivalence table.
 *  @param[in,out] UcodePatchAddr        - The selected UcodePatch address, return 0 if not found
 *  @param[in] StdHeader                 - Config handle for library and services.
 *
 *  @retval    TRUE   - Patch Loaded Successfully.
 *  @retval    FALSE  - Patch Did Not Get Loaded.
 *
 */
BOOLEAN
LoadMicrocodePatch (
  IN       UINT16            *MicrocodeEquivalenceTable,
  IN       UINT8             EquivalencyEntries,
  IN OUT   UINT64            *UcodePatchAddr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

#pragma pack (pop)
#endif // _CCX_MICROCODE_PATCH_LIB_H_
