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

#ifndef _AMDBOARDID_H_
#define _AMDBOARDID_H_

/// The value of Platform Revision ID

typedef enum {
  REVISION_A =                    0x01,           ///< FCH Revision ID for KB
  REVISION_B =                    0x02,           ///< FCH Revision ID for ML
  REVISION_C =                    0x03,           ///< FCH Revision ID for CZ
  REVISION_D =                    0x04,           ///< FCH Revision ID for NL
} PLATFORM_REVISION_ID;

/// The value of Platform Revision ID

typedef enum {
  DIESEL =                 0x01,           ///< Platfrom Board ID for Diesel
  DIESEL_DEBUG =           0x02,           ///< Platfrom Board ID for Diesel Debug
  DIESEL_DAP =             0x03,           ///< Platfrom Board ID for Diesel DAP
  DIESEL_SLT =             0x04,           ///< Platfrom Board ID for Diesel SLT
  SPEEDWAY =               0x05,           ///< Platfrom Board ID for Speedway
  TURBO =                  0x06,           ///< Platfrom Board ID for Turbo
  CLUTCH =                 0x07,           ///< Platfrom Board ID for Clutch
  OCTANE =                 0x08,           ///< Platfrom Board ID for Octane
  OCTANE_SLT =             0x08,           ///< Platfrom Board ID for Octane SLT
  OCTANE_DAP =             0x08,           ///< Platfrom Board ID for Octane DAP
  ETHANOL =                0x0C,           ///< Platfrom Board ID for Ethanol
  ETHANOL_DEBUG =          0x0D,           ///< Platfrom Board ID for Ethanol-Debug
  GRANDSTAND =             0x0E,           ///< Platfrom Board ID for Grandstand
  NITROUS =                0x0F,           ///< Platfrom Board ID for Nitrous
  UNKONWN_PLATFORM =       0xFF,           ///< Unknown Platform
} PLATFORM_BOARD_ID;

/// Device and function number of PCI device
typedef struct {
  UINT8                 BoardId;          ///< Platform Identification
  UINT8                 Reserved1;        ///< Platform Identification
  UINT8                 RevisionId;       ///< Platform Identification
  UINT8                 Reserved2;        ///< Platform Identification
} AMD_PLATFORM_ID;

/// Eeprom Root table
typedef struct {
  EFI_GUID              TableSignature;               ///< EEPROM Table Signature
  AMD_PLATFORM_ID       PlatformId;                   ///< Platform Identification
  UINT32                ApcbInstance;                 ///< APCB Instance Number in the BIOS Directory
  UINT16                MajorRevision;                ///< EEPROM Major Revision
  UINT16                MinorRevision;                ///< EEPROM Minor Revision
  UINT32                Checksum;                     ///< Root Table Checksum
} AMD_EEPROM_ROOT_TABLE;

typedef enum {
  HYGON_DIESEL =           0,
  HYGON_SIMNOW =           2,
  HYGON_35N16 =            4,
  HYGON_65N32 =            5,
  HYGON_65N16 =            7,
  HYGON_DM1SLT =           11,
  HYGON_52D16 =            13,
  HYGON_35N16SL1R2 =       16,
  HYGON_65N32SL1R2 =       17,
  SUGON_W550_DM1SLT =      20,
  HYGON_35N16_2DIE_4DIE =  40,
  HYGON_65N32_2DIE_4DIE =  50,
  UNKONWN_ID =             0xFF,
} HYGON_BOARD_ID;

#endif // _AMDBOARDID_H_

