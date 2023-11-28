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
 ******************************************************************************
 */

#ifndef _AMD_SOC_H_
#define _AMD_SOC_H_

/// CPUID related registers
#define AMD_CPUID_FMF               0x80000001ul  // Family Model Features information
#define AMD_CPUID_APICID_LPC_BID    0x00000001ul  // Local APIC ID, Logical Processor Count, Brand ID

#define CPUID_STEPPING_MASK      0x0000000Ful
#define CPUID_BASE_MODEL_MASK    0x000000F0ul
#define CPUID_BASE_FAMILY_MASK   0x00000F00ul
#define CPUID_EXT_MODEL_MASK     0x000F0000ul
#define CPUID_EXT_FAMILY_MASK    0x0FF00000ul

#define RAW_FAMILY_ID_MASK (UINT32) (CPUID_EXT_FAMILY_MASK | CPUID_EXT_MODEL_MASK | CPUID_BASE_FAMILY_MASK)

#define F17_ZP_RAW_ID   0x00800F00ul
#define F18_DN_RAW_ID   0x00900F00ul


/// Family 17 package type
typedef enum {
  ZP_SP4   = 1,           ///< Embedded, two ZP die MCM, single and dual socket
  ZP_AM4   = 2,           ///< Desptop, single die, single socket
  ZP_SP4r2 = 3,           ///< Embedded, single die, single socket
  ZP_SP3   = 4,           ///< Server, four ZP die MCM, single socket
  ZP_DM1   = 6,           ///< Server, SCM, double socket
  TR_SP3r2 = 7,           ///< Two TR die MCM, single socket
  ZP_UNKNOWN  = 0xFF      ///< Unknown package type
} FAMILY17_PACKAGE_TYPE;

#define ZP_PKG_SP4       1
#define ZP_PKG_AM4       2
#define ZP_PKG_SP4r2     3
#define ZP_PKG_SP3       4
#define ZP_PKG_DM1       6
#define ZP_PKG_SP3r2     7

#define F15_BR_RAW_ID   0x00660F00ul

/// Bristol package type
typedef enum {
  BR_FP4   = 0,           ///< Notebook BGA Package
  BR_AM4   = 2,           ///< Performance class with DDR4 support
  BR_FM2r2 = 3,           ///< DesktopuPGA Package
  BR_UNKNOWN  = 0xFF      ///< Unknown package type
} BR_PACKAGE_TYPE;

#define BR_PKG_FP4       0
#define BR_PKG_AM4       2
#define BR_PKG_FM2r2     3

#define F17_RV_RAW_ID   0x00810F00ul

/// Raven package type
typedef enum {
  RV_FP5   = 0,           ///< Notebook BGA Package
  RV_AM4   = 2,           ///< Performance class with DDR4 support
  RV_UNKNOWN  = 0xFF      ///< Unknown package type
} RV_PACKAGE_TYPE;

#define RV_PKG_FP5       0
#define RV_PKG_AM4       2

/// SOC ID Structure
typedef struct _SOC_ID_STRUCT {
  UINT32            SocFamilyID;    ///< SOC family ID
  UINT8             PackageType;    ///< SOC package type
} SOC_ID_STRUCT;

//Move to separate public header file.
typedef enum {
  PCD_BOOL = 0,             ///< PCD data type : boolean
  PCD_UINT8,                ///< PCD data type : byte
  PCD_UINT16,               ///< PCD data type : word
  PCD_UINT32,               ///< PCD data type : dword
  PCD_UINT64,               ///< PCD data type : qword
  PCD_PTR                   ///< PCD data type : pointer
} AMD_PCD_DATA_TYPE;

/**
 * An AMD AGESA Configuration Parameter Group List.
 */

typedef struct {
  UINTN BufferSize;         ///< PCD PTR buffer size
  void   *Buffer;           ///< PCD PTR buffer pointer
} AMD_PCD_PTR;


typedef struct {
  EFI_GUID *AmdConfigurationParameterPcdGuid;     ///< PCD GUID
  UINTN AmdConfigurationParameterPcdTokenNumber;  ///< PCD Token number
  UINT64   Value;                                 ///< Configurtion Parameter overwrite value.
  AMD_PCD_DATA_TYPE AmdPcdDataType;               ///< Configurtion Parameter PCD data type.
} AMD_PCD_LIST;

typedef struct {
  EFI_GUID *PlalformGroupGuid;          ///< Group List GUID pass from platform BIOS
  UINT32   NumberOfPcdEntries;          ///< number of PCD in list.
  AMD_PCD_LIST   *ListEntry;            ///< Group List version.
} AMD_GROUP_LIST_HEADER;

typedef struct {
  UINT32   Version;                     ///< Group List version.
  UINT32   NumberOfGroupsList;          ///< number of groups list.
  AMD_GROUP_LIST_HEADER   *GroupList;   ///< Group List entry
  EFI_GUID *PlalformGroupGuid;          ///< Group List GUID pass from platform BIOS
} AMD_PCD_PLATFORM_GROUP_LIST;

#endif  // _AMD_SOC_H_
