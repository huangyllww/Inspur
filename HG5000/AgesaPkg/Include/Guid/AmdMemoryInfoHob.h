/* $NoKeywords:$ */
 /**
 * @file
 *
 * Amd Memory Info Hob GUID.
 *
 * Contains GUID Declaration for Memory Info Hob
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  UEFI
 * @e \$Revision: 309083 $   @e \$Date: 2014-12-09 11:28:24 -0600 (Tue, 09 Dec 2014) $
 *
 */
/*
 ****************************************************************************
 * AMD Generic Encapsulated Software Architecture
 *
 * Description: AmdMemoryInfoHob.h - Memory Info Hob GUID
 *
 *****************************************************************************
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
 */

#ifndef _AMD_MEMORY_INFO_HOB_H_
#define _AMD_MEMORY_INFO_HOB_H_

extern EFI_GUID gAmdMemoryInfoHobGuid;

#pragma pack (push, 1)

/// Memory descriptor structure for each memory rang
typedef struct {
  UINT64  Base;                             ///< Base address of memory rang
  UINT64  Size;                             ///< Size of memory rang
  UINT32  Attribute;                        ///< Attribute of memory rang
  UINT32  Reserved;                         ///< For alignment purpose
} AMD_MEMORY_RANGE_DESCRIPTOR;

#define AMD_MEMORY_ATTRIBUTE_AVAILABLE      0x1
#define AMD_MEMORY_ATTRIBUTE_UMA            0x2
#define AMD_MEMORY_ATTRIBUTE_MMIO           0x3
#define AMD_MEMORY_ATTRIBUTE_RESERVED       0x4

/// Memory info HOB structure
typedef struct  {
  UINT32              Version;                          ///< Version of HOB structure
  BOOLEAN             AmdMemoryVddioValid;              ///< This field determines if Vddio is valid
  UINT16              AmdMemoryVddio;                   ///< Vddio Voltage
  BOOLEAN             AmdMemoryVddpVddrValid;           ///< This field determines if VddpVddr is valid
  UINT8               AmdMemoryVddpVddr;                ///< VddpVddr voltage
  BOOLEAN             AmdMemoryFrequencyValid;          ///< Memory Frequency Valid
  UINT32              AmdMemoryFrequency;               ///< Memory Frquency
  UINT32              AmdMemoryDdrMaxRate;              ///< Memory DdrMaxRate
  UINT32              NumberOfDescriptor;               ///< Number of memory range descriptor
  AMD_MEMORY_RANGE_DESCRIPTOR Ranges[1];                ///< Memory ranges array
} AMD_MEMORY_INFO_HOB;

#pragma pack (pop)

#define AMD_MEMORY_INFO_HOB_VERISION        0x00000110ul  // Ver: 00.00.01.10

#endif // _AMD_MEMORY_INFO_HOB_H_

