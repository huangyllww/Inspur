/* $NoKeywords:$ */

/**
 * @file
 *
 * JedecNvdimmAcpi.h
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  JedecNvdimm
 *
 */
/*
 ******************************************************************************
 *
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 *
 ******************************************************************************
 */

/** @file JedecNvdimmAcpi.h
    Header file for JedecNvdimmAcpi Driver

**/

#ifndef _JEDEC_NVDIMM_ACPI_H_
#define _JEDEC_NVDIMM_ACPI_H_

#pragma pack (push, 1)

///< NFIT header
typedef struct {
  UINT8     Sign[4];                  ///< NFIT, Signature for the NVDIMM Firmware Interface Table
  UINT32    Length;                   ///< Length, in bytes, of the entire table
  UINT8     Revision;                 ///< 1
  UINT8     Checksum;                 ///< Entire table must sum to zero.
  UINT8     OemId[6];                 ///< OEM ID
  UINT8     OemTableId[8];            ///< OEM Tabled ID
  UINT32    OemRev;                   ///< OEM Revision
  UINT8     CreatorId[4];             ///< Creator ID
  UINT32    CreatorRev;               ///< Creator Revision
  UINT32    Reserved;                 ///< Reserved
} NFIT_HEADER;

///< System Physical Address (SPA) Range Structure
///< Type 0
// #define EFI_MEMORY_UC               0x00000001
// #define EFI_MEMORY_WC               0x00000002
// #define EFI_MEMORY_WT               0x00000004
// #define EFI_MEMORY_WB               0x00000008
// #define EFI_MEMORY_UCE              0x00000010
// #define EFI_MEMORY_WP               0x00001000
// #define EFI_MEMORY_RP               0x00002000
// #define EFI_MEMORY_XP               0x00004000
// #define EFI_MEMORY_NV               0x00008000
// #define EFI_MEMORY_MORE_RELIABLE    0x00010000

typedef struct {
  UINT16      Type;                    ///< 0 - System Physical Address Range Structure
  UINT16      Length;                  ///< Length in bytes for entire structure
  UINT16      SpaRangeStructIndex;     ///< Used by NVDIMM Region Mapping Structure to uniquely refer to this structure.
  UINT16      Flags;                   ///< Flags
  UINT32      Reserved;                ///< Reserved
  UINT32      ProximityDomain;         ///< This number must match with corresponding entry in the SRAT table
  EFI_GUID    AddrRangeTypeGUID;       ///< GUID that defines the type of the Address Range Type
  UINT64      SpaRangeBase;            ///< Start Address of the System Physical Address Range
  UINT64      SpaRangeLength;          ///< Range Length of the region in bytes
  UINT64      AddrRangeMemMappingAttr; ///< Memory mapping attributes for this address range
} SPA_RANGE_STRUCT;

///< NVDIMM Region Mapping Structure
///< Type 1
typedef struct {
  UINT16    Type;                     ///< 1 - NVDIMM Region Mapping Structure
  UINT16    Length;                   ///< Length in bytes for entire structure
  UINT32    NfitDeviceHandle;         ///< The _ADR of the NVDIMM device containing the NVDIMM region
  UINT16    NvdimmPhysicalId;         ///< Handle for the SMBIOS Type 17 structure describing the NVDIMM containing the NVDIMM region
  UINT16    NvdimmRegionId;           ///< Unique identifier for the NVDIMM region
  UINT16    SpaRangeStructIndex;      ///< The SPA range, if any, associated with the NVDIMM region
  UINT16    NvdimmCtrlRegionStructIndex;///< The index of the NVDIMM Control Region Structure for the NVDIMM region
  UINT64    NvdimmRegionSize;         ///< In bytes. The size of the NVDIMM region.
  UINT64    RegionOffset;             ///< In bytes. The Starting Offset for the NVDIMM region in the Interleave Set
  UINT64    NvdimmPhysicalAddrRegionBase;///< In bytes. The base physical address within the NVDIMM of the NVDIMM region
  UINT16    InterleaveStructIndex;    ///< The Interleave Structure, if any, for the NVDIMM region
  UINT16    InterleaveWay;            ///< Number of NVDIMMs in the interleave set
  UINT16    NvdimmStateFlags;         ///< NVDIMM state flags
  UINT16    Reserved;                 ///< Reserved
} REGION_MAPPING_STRUCT;

///< NVDIMM Control Region Structure
///< Type 4
typedef struct {
  UINT16    Type;                          ///< 4 - NVDIMM Control Region Structure
  UINT16    Length;                        ///< Length in bytes for entire structure
  UINT16    ControlRegionStructIndex;      ///< Index Number uniquely identifies the NVDIMM Control Region Structure
  UINT16    VendorId;                      ///< Byte 0 set to SPD byte 320 and byte 1 set to SPD byte 321
  UINT16    DeviceId;                      ///< Byte 0 set to SPD byte 192 and byte 1 set to SPD byte 193
  UINT16    RevisionId;                    ///< Byte 0 set to SPD byte 349 and byte 1 is reserved, set to 0
  UINT16    SubSysVendorId;                ///< Byte 0 set to SPD byte 194 and byte 1 set to SPD byte 195
  UINT16    SubSysDeviceId;                ///< Byte 0 set to SPD byte 196 and byte 1 set to SPD byte 197
  UINT16    SubSysRevisionId;              ///< Byte 0 set to SPD byte 198 and byte 1 is resreved, set to 0
  UINT8     ValidField;                    ///<
  UINT8     ManufacturingLocation;         ///< Set to SPD byte 322
  UINT16    ManufacturingDate;             ///< Byte 0 set to SPD byte 323 and byte 1 set to SPD byte 324
  UINT16    Reserved;                      ///< Reserved
  UINT8     SerialNumber[4];               ///< Byte 0 - 3 set to SPD byte 325 - 328
  UINT16    RegionFormatInterfaceCode;     ///<
  UINT16    NumOfBlockCtrlWindow;          ///<
  UINT64    SizeOfBlockCtrlWindow;         ///<
  UINT64    CmdRegOffsetInBlockCtrlWindow; ///<
  UINT64    SizeOfCmdRegInBlockCtrlWindow; ///<
  UINT64    StatusRegOffsetInBlockCtrlWindow; ///<
  UINT64    SizeOfStatusRegInBlockCtrlWindow; ///<
  UINT16    CtrlRegionFlag;           ///<
  UINT8     Reserved1[6];
} CONTROL_REGION_STRUCT;

///< SMBIOS Management Information Structure
///< Type 3
typedef struct {
  UINT16    Type;
  UINT16    Length;
  UINT32    Reserved;
  // UINT8  Data[];
} SMBIOS_MGMT_INFO_STRUCT;

#pragma pack (pop)

#endif // _JEDEC_NVDIMM_ACPI_H_
