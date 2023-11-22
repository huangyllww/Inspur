/* $NoKeywords:$ */
/**
 * @file
 *
 * mtspd5.h
 *
 * Technology SPD support for DDR5
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: HGPI
 * @e sub-project: (Mem/Tech/DDR5)
 *
 **/
/*****************************************************************************
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
  * ***************************************************************************
  *
 */

#ifndef _MTSPD5_H_
#define _MTSPD5_H_

#pragma pack(push, 1)


/****************************************************************************** 
 
 JEDEC STANDARD DDR5 - Serial Presence Detect (SPD) Contents
 
 JESD400-5
 
 Revision 0.93
 April 2020
 
 JEDEC SOLID STATE TECHNOLOGY ASSOCIATION
 
******************************************************************************/

/***************************************************************************************************** 
  1.0 Introduction
  This annex describes the serial presence detect (SPD) values for all DDR5 memory modules. In
  this context, "modules" applies to memory modules like traditional Dual In-line Memory Modules
  (DIMMs) or solder-down motherboard applications. The SPD data provides critical information
  about all modules on the memory channel and is intended to be use by the system's BIOS in order
  to properly initialize and optimize the system memory channels. The storage capacity of the SPD
  non-volatile memory is limited, so a number of techniques are employed to optimize the use of
  these bytes, including overlays and run length limited coding.
 
  All unused entries will be coded as 0x00. All unused bits in defined bytes will be coded as 0 except
  where noted.
 
  Timing parameters in the SPD represent the operation of the module including all DRAMs and
  support devices at the lowest supported supply voltages (see SPD bytes 16 through 18), and are
  valid from tCKAVGmin to tCKAVGmax (see SPD bytes 20 through 23).
 
  To allow for maximum flexibility as devices evolve, SPD fields described in this document may
  support device configuration and timing options that are not included in the JEDEC DDR5 SDRAM
  data sheet (JESD79-5). Please refer to DRAM supplier data sheets or JESD79-5 to determine the
  compatibility of components.

*****************************************************************************************************/

/***************************************************************************************************** 
  SPD Architecture
 
  The SPD contents architecture must support the many variations of module types while remaining
  efficient. A system of overlay information selected through the use of "key bytes", or selectors for
  the type of information to load has been implemented. The following DDR5 module SPD address
  map describes where the individual lookup table entries will be held in the serial non-volatile
  memory.
 
  Consistent with the definition of DDR5 generation SPD devices (SPD5108 and SPD5118) which
  have 16 individual write protection blocks of 64 bytes in length each, the SPD contents are aligned
  with these blocks as follows:
 
  Block           Range             Description
 
  0         0~63      0x000~0x03F   Base Configuration and DRAM Parameters
  1         64~127    0x040~0x07F   Base Configuration and DRAM Parameters
  2         128~191   0x080~0x0BF   Reserved for future use
  3         192~255   0x0C0~0x0FF   Standard Module Parameters -- See annexes A.x for details
  4         256~319   0x100~0x13F   Standard Module Parameters -- See annexes A.x for details
  5         320~383   0x140~0x17F   Standard Module Parameters -- See annexes A.x for details
  6         384~447   0x180~0x1BF   Standard Module Parameters -- See annexes A.x for details
  7         448~509   0x1C0~0x1FD   Reserved for future use
            510~511   0x1FE~0x1FF   CRC for SPD bytes 0~509
  8         512~575   0x200~0x23F   Manufacturing information
  9         576~639   0x240~0x27F   Manufacturing information
  10        640~703   0x280~0x2BF   End User Programmable
  11        704~767   0x2C0~0x2FF   End User Programmable
  12        768~831   0x300~0x33F   End User Programmable
  13        832~895   0x340~0x37F   End User Programmable
  14        896~959   0x380~0x3BF   End User Programmable
  15        960~1023  0x3C0~0x3FF   End User Programmable
 
 *****************************************************************************************************/

//
// Version of the SPD Data Supported by this Header File
//
#define SPD_DOCUMENT_REVISION 0x09

//
// 16 Bit SPD Values
//
typedef  union {
   struct {
    UINT8 Lsb;
    UINT8 Msb;
  } byte;
  UINT16 Value;
} SPD_VALUE16;

//
// General timing structure
//
typedef  union {
   struct {
    UINT8 Lsb       : 8;
    UINT8 Msb       : 8;
  } Field;
  UINT16 Value;
} SPD_TIMING_S;

//
// JEP106 Manufacturer's ID Structure
//
typedef  union {
   struct {
    UINT8 NumContCodes  : 7;
    UINT8 OddParity     : 1;
    UINT8 LastNonZero;
  } Field;
  UINT16 Value;
} SPD_MANUFACTURER_ID_S;

// -----------------------------------------------------------------------------------------------------
// SPD Blocks
//
#define SPD_NUM_BLOCKS        16
#define SPD_BLOCK_LEN         64

typedef enum {
  SpdBlock_BaseConfig_0          = 0x00,
  SpdBlock_BaseConfig_1          = 0x01,
  SpdBlock_Reserved_2            = 0x02,
  SpdBlock_ModuleParms_0         = 0x03,
  SpdBlock_ModuleParms_1         = 0x04,
  SpdBlock_ModuleParms_2         = 0x05,
  SpdBlock_ModuleParms_3         = 0x06,
  SpdBlock_Reserved_7            = 0x07,
  SpdBlock_MfgInfo0              = 0x08,
  SpdBlock_MfgInfo1              = 0x09,
  SpdBlock_EndUserProgrammable0  = 0x0A,
  SpdBlock_EndUserProgrammable1  = 0x0B,
  SpdBlock_EndUserProgrammable2  = 0x0C,
  SpdBlock_EndUserProgrammable3  = 0x0D,
  SpdBlock_EndUserProgrammable4  = 0x0E,
  SpdBlock_EndUserProgrammable5  = 0x0F
} DDR5_SPD_BLOCK_NAMES;

typedef  struct {
  UINT8 Data[SPD_BLOCK_LEN];
} SPD_BLOCK_S;

// -----------------------------------------------------------------------------------------------------
//
// Blocks 0~1:  General Configuration Section: Bytes 0~127 (0x000~0x07F)
//
// This section contains defines parameters that are common to all DDR5 module types with DDR5
// SDRAM as the media type, and provides "key bytes" to allow overlay of module specific
// information.  These bytes are defined when Key Byte 2 contains 18 (0x12), DDR5 SDRAM.
//

// -----------------------------------------------------------------------------------------------------
//
// Byte 0: Number of SPD Bytes
//
#define SPD_NUM_BYTES       0
//
// The least significant nibble of this byte describes the total number of bytes used by the module
// manufacturer for the SPD data and any (optional) specific supplier information. The byte count
// includes the fields for all required and optional data. Bits 6~4 describe the total size of the serial
// memory used to hold the Serial Presence Detect data.
//
typedef  union {
   struct {
    UINT8 Reserved_30      : 4;
    UINT8 SpdBytesTotal    : 3;   // SPD Bytes Total
    UINT8 Reserved_7       : 1;
  } Field;
  UINT8 Value;
} SPD_NUM_BYTES_S;

// SPD Bytes Total Reserved
// Reserved; must be coded as 0 000: Undefined
// 001: 256
// 010: 512
// 011: 1024
// All others reserved
//
// 
#define SPD_DECODE_BYTES_TOTAL(val)  (128 << val)

// -----------------------------------------------------------------------------------------------------
//
// SPD Revision for Base Configuration Parameters
//
#define SPD_REVISION        1
// This byte describes the compatibility level of the encoding of the bytes contained in the SPD
// EEPROM, and the current collection of valid defined bytes. Software should examine the upper
// nibble (Encoding Level) to determine if it can correctly interpret the contents of the module SPD.
// The lower nibble (Additions Level) can optionally be used to determine which additional bytes or
// attribute bits have been defined; however, since any undefined additional byte must be encoded
// as 0x00 or undefined attribute bit must be defined as 0, software can safely detect additional bytes
// and use safe defaults if a zero encoding is read for these bytes.
//
typedef  union {
   struct {
    UINT8 SpdRevisionAdditions         : 4;
    UINT8 SpdRevisionEncodings         : 4;
  } Field;
  UINT8 Value;
} SPD_REVISION_S;  

;

// -----------------------------------------------------------------------------------------------------
//
// Key Byte / Host Bus Command Protocol Type
//
#define SPD_KEY_BYTE        2
//
// This byte is the key byte used by the system BIOS to determine how to interpret all other bytes in
// the SPD EEPROM. The BIOS must check this byte first to ensure that the EEPROM data is
// interpreted correctly. This command protocol is often based on the interface parameters for a
// specific memory device such as a DDR5 SDRAM, however may also be a general media interface
// protocol such as DDR5 NVDIMM-P. Protocol extensions such as the use of registers or data
// buffers must be documented in Byte 3, the Module Type byte, and such differences
// comprehended and accounted for by the system and memory controller. Some non-DRAM
// solutions called -Hybrid- use an extension of a DRAM bus protocol, such as NVDIMM-N, and use
// the code for the emulated DRAM protocol in this byte, such as DDR4 SDRAM.
//
typedef  union {
   struct {
    UINT8 ModuleType      : 8;  // SDRAM / Module Type Corresponding to Key Byte
  } Field;
  UINT8 Value;
} SPD_KEY_BYTE_S;

typedef enum {
  ModTypeReserved0,
  ModTypeFastPageMode,
  ModTypeEDO,
  ModTypePipelinedNibble,
  ModTypeSdram,
  ModTypeRom,
  ModTypeDdrSgram,
  ModTypeDdrSdram,
  ModTypeDdr2Sdram,
  ModTypeDdr2SdramFBDimm,
  ModTypeDdr2SdramFBDimmProbe,
  ModTypeDdr3Sdram,
  ModTypeDdr4Sdram,
  ModTypeReserved13,
  ModTypeDdr4ESdram,
  ModTypeLpddr3Sdram,
  ModTypeLpddr4Sram,
  ModTypeLpddr4xSdram,
  ModTypeDdr5Sdram,
  ModTypeLpddr5Sdram,
  ModTypeDdr5NvdimmP,
  ModTypeModuleTypeMax
} MODULE_TYPE;

// -----------------------------------------------------------------------------------------------------
//
// Key Byte / Module Type
//
#define SPD_KEY_BYTE2       3
//
// This byte is a Key Byte used to index the module specific section of the SPD from bytes 512~639.
// Byte 3 bits 3~0 identifies the SDRAM memory module type, and bits 7~4 describe hybrid memory
// extensions.
// Some modules may have no base memory, but will have only a secondary memory type. For
// example, a Flash-only memory module. These are classified as -hybrid- for the purposes of interpreting
// the SPD. Where base memory parameters apply to this class of hybrid module, these will
// be documented with those bytes in the base section.
//

#define SPD_HYBRID_MEDIA    3
#define HYBRID_MEDIA_SHIFT  4
#define HYBRID_MEDIA_MASK   7
#define NVDIMM_N_HYBRID     1
#define NVDIMM_P_HYBRID     2
#define SPD_HYBRID          3
#define HYBRID_SHIFT        7
#define HYBRID_MASK         1
#define HYBRID_TYPE         1

typedef  union {
   struct {
    UINT8 BaseModuleType  : 4;  // Base Module Type
    UINT8 HybridMedia     : 3;  // Hybrid Media
    UINT8 Hybrid          : 1;  // Hybrid
  } Field;
  UINT8 Value;
} SPD_KEY_BYTE2_S;

#define SPD_BASEMODULE_RDIMM            1
#define SPD_BASEMODULE_UDIMM            2
#define SPD_BASEMODULE_SODIMM           3
#define SPD_BASEMODULE_LRDIMM           4
#define SPD_BASEMODULE_SORDIMM          8
#define SPD_BASEMODULE_DDIMM            10
#define SPD_BASEMODULE_SOLDEREDDOWN     11

typedef enum {
  BaseModReserved0,
  BaseModRdimm,
  BaseModUdimm,
  BaseModSodimm,
  BaseModLrdimm,
  BaseModReserved5,
  BaseModReserved6,
  BaseModReserved7,
  BaseModSoRdimm,
  BaseModReserved9,
  BaseModDDimm,
  BaseModSolderDown,
  BaseModBaseModuleTypeMax
} BASE_MODULE_TYPE;

typedef enum {
  SpdHybridMediaNotHybrid,
  SpdHybridMediaNvdimmN,
  SpdHybridMediaNvdimmP,
  SpdHybridMediaMax
} HYBRID_MEDIA;

typedef enum {
  SpdNotHybrid,
  SpdHybrid
} HYBRID;

// -----------------------------------------------------------------------------------------------------
//
// First SDRAM Density and Package
//
#define SPD_FIRST_DENSITY_PACKAGE    4
// This byte describes the SDRAM package type, loading as seen by the system, and device density
// in Gbits. These values comes from the DDR5 SDRAM data sheet, JESD79-5. This byte applies
// to the even rank SDRAMs for asymmetrical memory configurations, or for all SDRAMs for
// symmetrical configurations.
typedef  union {
   struct {
    UINT8 Density         : 5;  // SDRAM Density Per Die
    UINT8 DiePerPkg       : 3;  // Die Per Package
  } Field;
  UINT8 Value;
} SPD_DENSITY_PACKAGE_S;

typedef enum {
  Density_NoMemory,
  Density_4Gb,
  Density_8Gb,
  Density_12Gb,
  Density_16Gb,
  Density_24Gb,
  Density_32Gb,
  Density_48Gb,
  Density_64Gb,
  DensityInvalid
} DENSITY_PER_DIE;

typedef enum {
  DiePerPkg_1,
  DiePerPkg_Reserved1,
  DiePerPkg_2,
  DiePerPkg_4,
  DiePerPkg_8,
  DiePerPkg_16,
  DiePerPkgInvalid
} DIE_PER_PKG;

#define SPD_DENSITY_TABLE       {0, 4, 8, 12, 16, 24, 32, 48, 64}
#define SPD_DIE_PER_PKG_TABLE   {1, 0, 2, 4, 8, 16}

// -----------------------------------------------------------------------------------------------------
//
// First SDRAM Addressing
//
#define SPD_FIRST_ADDRESSING     5
// This byte describes the number SDRAM column and row address bits. These values comes from
// the DDR5 SDRAM data sheet, JESD79-5. This byte applies to the even rank SDRAMs for
// asymmetrical memory configurations, or for all SDRAMs for symmetrical configurations.
typedef  union {
   struct {
    UINT8 NumRows         : 5;  // SDRAM Row Address Bits
    UINT8 NumColumns      : 3;  // SDRAM Column Address Bits
  } Field;
  UINT8 Value;
} SPD_ADDRESSING_S;

typedef enum {
  TenColumns,
  ElevenColumns,
  NumColumnsInvalid
} NUM_COLUMNS;

#define SPD_DECODE_COL_BITS(Val)    (Val + 10)

typedef enum {
  SixteenRows,
  SeventeenRows,
  EighteenRows,
  NumRowsInvalid
} NUM_ROWS;

#define SPD_DECODE_ROW_BITS(Val)    (Val + 16)

// -----------------------------------------------------------------------------------------------------
//
// First SDRAM I/O Width
//
#define SPD_FIRST_IO_WIDTH    6
//
// This byte describes the number SDRAM I/O bits (DQ) and the number of bank groups. These
// values comes from the DDR5 SDRAM data sheet, JESD79-5. This byte applies to the even rank
// SDRAMs for asymmetrical memory configurations, or for all SDRAMs for symmetrical
// configurations.
typedef  union {
   struct {
    UINT8 Reserved40      : 5;
    UINT8 IoWidth         : 3;  // SDRAM I/O Width
  } Field;
  UINT8 Value;
} SPD_IO_WIDTH_S;

typedef enum {
  x4,
  x8,
  x16,
  x32,
  IoWidthMax
} IO_WIDTH;

#define SPD_DECODE_IO_WIDTH(Val)  (4 << Val)
#define SPD_DECODE_PAGESIZE(Val)  ((Val > x8) ? 1 : 0 )

// -----------------------------------------------------------------------------------------------------
//
// First SDRAM Bank Groups & Banks Per Bank Group
//
#define SPD_FIRST_BANKS    7
//
// This byte describes the number SDRAM banks per bank group. These values comes from the
// DDR5 SDRAM data sheet, JESD79-5. This byte applies to the even rank SDRAMs for
// asymmetrical memory configurations, or for all SDRAMs for symmetrical configurations.
//
typedef  union {
   struct {
    UINT8 BanksPerBG      : 3;  // SDRAM Banks Per Bank Group
    UINT8 Reserved43      : 2;
    UINT8 BankGroups      : 3;  // SDRAM Bank Groups
  } Field;
  UINT8 Value;
} SPD_BANKS_S;

typedef enum {
  OneBank,
  TwoBanks,
  FourBanks,
  BanksPerBGMax
} BANKS_PER_BG;

#define SPD_DECODE_SPD_BANKS_PER_BG(Val)    (1 << Val)

typedef enum {
  OneBg,
  TwoBgs,
  FourBgs,
  EightBgs,
  BgMax
} BANK_GROUPS;

#define SPD_DECODE_BANK_GROUPS(Val)    (1 << Val)

// -----------------------------------------------------------------------------------------------------
//
// Second SDRAM Density and Package
//
#define SPD_SECOND_DENSITY_PKG    8
//
// Same as SPD_FIRST_DENSITY_PKG
//

// -----------------------------------------------------------------------------------------------------
//
// Second SDRAM Addressing
//
#define SPD_SECOND_ADDRESSING     9
//
// Same as SPD_FIRST_ADDRESSING
//

// -----------------------------------------------------------------------------------------------------
//
// Secondary SDRAM I/O Width
//
#define SPD_SECOND_IO_WIDTH       10
//
// Same as SPD_FIRST_IO_WIDTH
//

// -----------------------------------------------------------------------------------------------------
//
// Second SDRAM Bank Groups & Banks Per Bank Group
//
#define SPD_SECOND_BANKS          11
//
// Same as SPD_FIRST_BANKS
//

// -----------------------------------------------------------------------------------------------------
//
// SDRAM Optional Features
//
#define SPD_OPTIONAL_FEATURES    12
// This byte defines support for certain SDRAM features. This value comes from the DDR5 SDRAM
// data sheet, JESD79-5.
typedef  union {
   struct {
    UINT8 Reserved30      : 4;
    UINT8 BL32            : 1;  // BL32
    UINT8 SoftPPR         : 1;  // Soft PPR
    UINT8 PPR             : 2;  // Post Package Repair (PPR)
  } Field;
  UINT8 Value;
} SPD_OPT_FEATURES_S;

// -----------------------------------------------------------------------------------------------------
//
// Reserved Bytes 13 - 15
//
#define SPD_RESERVED_13          13
#define SPD_RESERVED_14          14
#define SPD_RESERVED_15          15

// -----------------------------------------------------------------------------------------------------
//
// SDRAM Nominal Voltage, VDD
//
#define SPD_NOMINAL_VDD     16
// This byte describes the voltage levels for the SDRAM VDD supply only.
typedef  union {
   struct {
    UINT8 Endurant        : 2;    //'Endurant' is defined as the VDD voltage at which the module may be powered without adversely
                                  // affecting the life expectancy or reliability. Operation is not supported at this voltage
    UINT8 Operable        : 2;    // 'Operable' is defined as the VDD voltage at which module operation is allowed using the
                                  // performance values programmed in the SPD
    UINT8 Nominal         : 4;
  } Field;
  UINT8 Value;
} SPD_NOMINAL_VDD_S;

// -----------------------------------------------------------------------------------------------------
//
// SDRAM Nominal Voltage, VDDQ
//
#define SPD_NOMINAL_VDQ     17
// This byte describes the voltage levels for the SDRAM VDDQ supply only.
typedef  union {
   struct {
    UINT8 Endurant        : 2;    //'Endurant' is defined as the VDD voltage at which the module may be powered without adversely
                                  // affecting the life expectancy or reliability. Operation is not supported at this voltage
    UINT8 Operable        : 2;    // 'Operable' is defined as the VDD voltage at which module operation is allowed using the
                                  // performance values programmed in the SPD
    UINT8 Nominal         : 4;
  } Field;
  UINT8 Value;
} SPD_NOMINAL_VDQ_S;

// -----------------------------------------------------------------------------------------------------
//
// SDRAM Nominal Voltage, VPP
//
#define SPD_NOMINAL_VDP     18
// This byte describes the voltage levels for the SDRAM VPP supply only.
typedef  union {
   struct {
    UINT8 Endurant        : 2;    //'Endurant' is defined as the VDD voltage at which the module may be powered without adversely
                                  // affecting the life expectancy or reliability. Operation is not supported at this voltage
    UINT8 Operable        : 2;    // 'Operable' is defined as the VDD voltage at which module operation is allowed using the
                                  // performance values programmed in the SPD
    UINT8 Nominal         : 4;
  } Field;
  UINT8 Value;
} SPD_NOMINAL_VDP_S;

// -----------------------------------------------------------------------------------------------------
//
// Reserved Byte 19
// 
#define SPD_RESERVED_19        19

// -----------------------------------------------------------------------------------------------------
//
// SDRAM Minimum Cycle Time (tCKAVGmin)
// 
#define SPD_TCKAVG_MIN_LSB     20
#define SPD_TCKAVG_MIN_MSB     21
//
// This 16-bit word defines the minimum cycle time for the SDRAM module, in picoseconds (ps). This
// number applies to all applicable components on the module. This byte applies to SDRAM and support
// components as well as the overall capability of the DIMM. This value comes from the DDR5
// SDRAM data sheet, JESD79-5, and support component data sheets.
typedef  union {
   SPD_TIMING_S       Field;
   UINT16 Value;
} SPD_TCKAVG_MIN;

// -----------------------------------------------------------------------------------------------------
//
// SDRAM Maximum Cycle Time (tCKAVGmax)
//
// This 16-bit word defines the maximum cycle time for the SDRAM module, in picoseconds (ps). This
// number applies to all applicable components on the module. This byte applies to SDRAM and support
// components as well as the overall capability of the DIMM. This value comes from the DDR5
// SDRAM data sheet, JESD79-5, and support component data sheets.
//
#define SPD_TCKAVG_MAX_LSB     22
#define SPD_TCKAVG_MAX_MSB     23

// -----------------------------------------------------------------------------------------------------
//
// SDRAM CAS Latencies Supported
// 
//    CAS Latencies Supported, First Byte
//    CAS Latencies Supported, Second Byte
//    CAS Latencies Supported, Third Byte
//    CAS Latencies Supported, Fourth Byte
//    CAS Latencies Supported, Fifth Byte
//
#define SPD_CAS_LATENCIES_SUPPORTED          24
#define SPD_CAS_LATENCIES_SUPPORTED_1        24
#define SPD_CAS_LATENCIES_SUPPORTED_2        25
#define SPD_CAS_LATENCIES_SUPPORTED_3        26
#define SPD_CAS_LATENCIES_SUPPORTED_4        27
#define SPD_CAS_LATENCIES_SUPPORTED_5        28
#define SPD_TCL_BYTES                         5
//
// These bytes define which CAS Latency (CL) values are supported with one bit per possible CAS
// Latency. A 1 in a bit position means that CL is supported, a 0 in that bit position means it is not
// supported. These values come from the DDR5 SDRAM, JESD79-5. CAS Latency Mask covers
// both DBI and non-DBI modes of operation.
//
typedef  struct {
  UINT8 Byte[SPD_TCL_BYTES];
} SPD_SUPPORTED_CAS_LAT_S;

#define SPD_CHECK_CASLAT_SUPPORT(Offset, Val)  ((Offset[SPD_CAS_LATENCIES_SUPPORTED + (((Val - 20)/2) / 8)] >> (((Val - 20) / 2) % 8) & 0x01) == 0x01)

// -----------------------------------------------------------------------------------------------------
//
// Reserved Byte 29
// 
#define SPD_RESERVED_29     29

// -----------------------------------------------------------------------------------------------------
//
// SDRAM Minimum CAS Latency Time (tAAmin)
//
// This 16-bit word defines the minimum CAS Latency in picoseconds (ps). This value comes from
// the DDR5 SDRAM data sheet, JESD79-5.
//
#define SPD_TAA_MIN_LSB     30
#define SPD_TAA_MIN_MSB     31

// -----------------------------------------------------------------------------------------------------
//
// SDRAM Minimum RAS to CAS Delay Time (tRCDmin) in picoseconds
//
// This 16-bit word defines the minimum SDRAM RAS to CAS Delay Time in picoseconds (ps). This
// value comes from the DDR5 SDRAM data sheet, JESD79-5.
//
#define SPD_TRCD_MIN_LSB    32
#define SPD_TRCD_MIN_MSB    33

// -----------------------------------------------------------------------------------------------------
//
// SDRAM Minimum Row Precharge Delay Time (tRPmin) in picoseconds
//
// This 16-bit word defines the minimum SDRAM Row Precharge Delay Time in picoseconds (ps).
// This value comes from the DDR5 SDRAM data sheet, JESD79-5.
//
#define SPD_TRP_MIN_LSB     34
#define SPD_TRP_MIN_MSB     35

// -----------------------------------------------------------------------------------------------------
//
// SDRAM Minimum Active to Precharge Delay Time (tRASmin) in picoseconds
//
// This 16-bit word defines the minimum SDRAM Active to Precharge Delay Time in picoseconds
// (ps). This value comes from the DDR5 SDRAM data sheet, JESD79-5.
//
#define SPD_TRAS_MIN_LSB    36
#define SPD_TRAS_MIN_MSB    37

// -----------------------------------------------------------------------------------------------------
//
// SDRAM Minimum Active to Active/Refresh Delay Time (tRCmin) in picoseconds
//
// This 16-bit word defines the minimum SDRAM Active to Active/Refresh Delay Time in picoseconds
// (ps). This value comes from the DDR5 SDRAM data sheet, JESD79-5.
//
#define SPD_TRC_MIN_LSB     38
#define SPD_TRC_MIN_MSB     39

// -----------------------------------------------------------------------------------------------------
//
// SDRAM Minimum Write Recovery Time (tWRmin) in picoseconds
//
// This 16-bit word defines the minimum SDRAM Write Recovery Time in picoseconds (ps). This
// value comes from the DDR5 SDRAM data sheet, JESD79-5.
//
#define SPD_TWR_MIN_LSB     40
#define SPD_TWR_MIN_MSB     41

// -----------------------------------------------------------------------------------------------------
//
// SDRAM Minimum Refresh Recovery Delay Time (tRFC1min, tRFC1_slr min) in nanoseconds
//
// This 16-bit word defines the minimum SDRAM Refresh Recovery Time Delay in nanoseconds
// (ns). tRFC1 relates to monolithic SDRAMs, tRFC1_slr to 3DS SDRAMs. These values come from
// the DDR5 SDRAM data sheet, JESD79-5.
//
#define SPD_TRFC1_MIN_LSB   42
#define SPD_TRFC1_MIN_MSB   43

// -----------------------------------------------------------------------------------------------------
//
// SDRAM Minimum Refresh Recovery Delay Time (tRFC2min, tRFC2_slr min) in nanoseconds
//
// This 16-bit word defines the minimum SDRAM Refresh Recovery Time Delay in nanoseconds
// (ns). tRFC2 relates to monolithic SDRAMs, tRFC2_slr to 3DS SDRAMs. These values come from
// the DDR5 SDRAM data sheet, JESD79-5.
//
#define SPD_TRFC2_MIN_LSB   44
#define SPD_TRFC2_MIN_MSB   45

// -----------------------------------------------------------------------------------------------------
//
// SDRAM Minimum Refresh Recovery Delay Time (tRFCsbmin, tRFCsb_slr min)
//
// This 16-bit word defines the minimum SDRAM Refresh Recovery Time Delay in nanoseconds
// (ns). tRFCsb relates to monolithic SDRAMs, tRFCsb_slr to 3DS SDRAMs. These values come from
// the DDR5 SDRAM data sheet, JESD79-5.
//
#define SPD_TRFC_SB_MIN_LSB 46
#define SPD_TRFC_SB_MIN_MSB 47

// -----------------------------------------------------------------------------------------------------
//
// SDRAM Minimum Refresh Recovery Delay Time, 3DS Different Logical Rank
//
// This 16-bit word defines the minimum SDRAM Refresh Recovery Time Delay in nanoseconds
// (ns). tRFC1_dlr relates to 3DS SDRAMs only. For monolithic or DDP SDRAMs, these bytes must
// be encoded as 0x0000. These values come from the DDR5 SDRAM data sheet, JESD79-5.
//
#define SPD_TRFC1_DLR_MIN_LSB 48
#define SPD_TRFC1_DLR_MIN_MSB 49

// -----------------------------------------------------------------------------------------------------
//
// SDRAM Minimum Refresh Recovery Delay Time, 3DS Different Logical Rank (tRFC2_dlr min)
//
// This 16-bit word defines the minimum SDRAM Refresh Recovery Time Delay in nanoseconds
// (ns). tRFC2_dlr relates to 3DS SDRAMs only. For monolithic or DDP SDRAMs, these bytes must
// be encoded as 0x0000. These values come from the DDR5 SDRAM data sheet, JESD79-5.
//
#define SPD_TRFC2_DLR_MIN_LSB 50
#define SPD_TRFC2_DLR_MIN_MSB 51

// -----------------------------------------------------------------------------------------------------
//
// SDRAM Minimum Refresh Recovery Delay Time, 3DS Different Logical Rank (tRFCsb_dlr min)
//
// This 16-bit word defines the minimum SDRAM Refresh Recovery Time Delay in nanoseconds
// (ns). tRFCsb_dlr relates to 3DS SDRAMs only. For monolithic SDRAMs, these bytes must be
// encoded as 0x0000. These values come from the DDR5 SDRAM data sheet, JESD79-5.
//
#define SPD_TRFC_SB_DLR_MIN_LSB 52
#define SPD_TRFC_SB_DLR_MIN_MSB 53

// -----------------------------------------------------------------------------------------------------
//
// Primary SDRAM Refresh Management First Byte
//
// These bytes represent the refresh management settings for the SDRAMs used on this module.
// These are identical to the contents of the DDR5 SDRAM mode registers MR58 and MR59. These
// values come from the DDR5 SDRAM data sheet, JESD79-5. If all devices in a group (i.e., First or
// Second) are not matched, the value coded must represent the worst case for any device in the
// group.
//
#define SPD_PRIMARY_REFRESH_MANAGEMENT_FIRST     54

typedef  union {
   struct {
    UINT8 Rfm             : 1;  // RFM Required
    UINT8 Raaimt          : 4;  // Rolling Accumulated ACT Initial Management Threshold (RAAIMT)
    UINT8 Raammt          : 3;  // Rolling Accumulated ACT Maximum Management Threshold (RAAMMT)
  } Field;
  UINT8 Value;
} SPD_REF_MANAGEMENT_FIRST_S;

// -----------------------------------------------------------------------------------------------------
//
// Primary SDRAM Refresh Management Second Byte
//
#define SPD_PRIMARY_REFRESH_MANAGEMENT_SECOND     55

typedef  union {
   struct {
    UINT8 Reserved_50     : 6;
    UINT8 Decrement       : 2;  // RFM RAA Counter Decrement per REF command
  } Field;
  UINT8 Value;
} SPD_REF_MANAGEMENT_SECOND_S;

// -----------------------------------------------------------------------------------------------------
//
// Secondary SDRAM Refresh Management First Byte
//
#define SPD_SECONDARY_REFRESH_MANAGEMENT_FIRST    56

// -----------------------------------------------------------------------------------------------------
//
// Secondary SDRAM Refresh Management Second Byte
//
#define SPD_SECONDARY_REFRESH_MANAGEMENT_SECOND   57

// -----------------------------------------------------------------------------------------------------
//
// Blocks 3~6: Module-Specific Section: Bytes 192~447 (0x0C0~0x1BF)
// 
// This section contains SPD bytes which are specific to families DDR5 module families. Module Type Key
// Byte 3 is used as an index for the encoding of bytes 192~447. The content of bytes 192~447 are described
//  in multiple annexes, one for each memory module family. These bytes are write protected.
//

// -----------------------------------------------------------------------------------------------------
//
// SPD Revision for Module Information
//
#define SPD_MODULE_REVISION 192
//
// This byte defines the SPD revision for bytes 192~447. See SPD byte 1 for details.
//

// -----------------------------------------------------------------------------------------------------
//
// Manufacture ID SPD
//
#define SPD_MANUFACTURER_ID_SPD             194
#define SPD_MANUFACTURER_ID_SPD_1ST_BYTE    194
#define SPD_MANUFACTURER_ID_SPD_2ND_BYTE    195

// -----------------------------------------------------------------------------------------------------
//
// Device Type SPD
//
#define SPD_DEVICE_TYPE_SPD                 196
typedef  union {
   struct {
    UINT8 DevType        : 4;
    UINT8 Reserved_54    : 2;
    UINT8 Reserved_6     : 1;
    UINT8 Installed      : 1;
  } Field;
  UINT8 Value;
} SPD_MODULE_DEVICE_TYPE_SPD_S;

typedef enum {
  Spd5118,
  Spd5108,
  MaxSpdType
} SpdDevTypes;

// -----------------------------------------------------------------------------------------------------
//
// Device Revision SPD
//
// Device revision: This byte is coded as a major revision in bits 7~4 and minor revision in bits 3~0,
// each nibble expressed in BCD. For example, device stepping 3.2 would be coded as 0x32.
//
#define SPD_DEVICE_REVISION_SPD 197
typedef  union {
   struct {
    UINT8 MinorRev        : 4;
    UINT8 MajorRev        : 4;
   } Field;
  UINT8 Value;
} SPD_MODULE_REVISION_S;

// -----------------------------------------------------------------------------------------------------
//
// PMIC0
//
#define SPD_MANUFACTURER_ID_PMIC0                 198
#define SPD_MANUFACTURER_ID_PMIC0_1ST_BYTE        198
#define SPD_MANUFACTURER_ID_PMIC0_2ND_BYTE        199
#define SPD_DEVICE_TYPE_PMIC0                     200
#define SPD_DEVICE_REVISION_PMIC0                 201

// -----------------------------------------------------------------------------------------------------
//
// PMIC1
//
#define SPD_MANUFACTURER_ID_PMIC1                 202
#define SPD_MANUFACTURER_ID_PMIC1_1ST_BYTE        202
#define SPD_MANUFACTURER_ID_PMIC1_2ND_BYTE        203
#define SPD_DEVICE_TYPE_PMIC1                     204
#define SPD_DEVICE_REVISION_PMIC1                 205

// -----------------------------------------------------------------------------------------------------
//
// PMIC2
//
#define SPD_MANUFACTURER_ID_PMIC2                 206
#define SPD_MANUFACTURER_ID_PMIC2_1ST_BYTE        206
#define SPD_MANUFACTURER_ID_PMIC2_2ND_BYTE        207
#define SPD_DEVICE_TYPE_PMIC2                     208
#define SPD_DEVICE_REVISION_PMIC2                 209

typedef  union {
   struct {
    UINT8 DevType        : 4;
    UINT8 Reserved_54    : 2;
    UINT8 Reserved_6     : 1;
    UINT8 Installed      : 1;
  } Field;
  UINT8 Value;
} SPD_MODULE_DEVICE_TYPE_PMIC_S;

typedef enum {
  Pmic5000,
  Pmic5010,
  Pmic5100,
  MaxPmicDevTypes
} PmicDevTypes;

// -----------------------------------------------------------------------------------------------------
//
// Thermal Sensors
//
#define SPD_MANUFACTURER_ID_TS                    210
#define SPD_MANUFACTURER_ID_TS_FIRST_BYTE         210
#define SPD_MANUFACTURER_ID_TS_SECOND_BYTE        211
#define SPD_DEVICE_TYPE_TS                        212
#define SPD_DEVICE_REVISION_TS                    213

typedef  union {
   struct {
    UINT8 DeviceType        : 4;
    UINT8 Reserved54        : 2;
    UINT8 TS1               : 1;
    UINT8 TS0               : 1;
  } Field;
  UINT8 Value;
} SPD_DEVICE_TYPE_TS_S;

typedef enum {
  Ts5111,
  Ts5110,
  MaxTsDevTyped
} TsDevTypes;

// -----------------------------------------------------------------------------------------------------
//
// Module Nominal Height
//
#define SPD_MODULE_HEIGHT     230
//
// This byte defines the nominal height (A dimension) in millimeters of the fully assembled module
// including heat spreaders or other added components. Refer to the relevant JEDEC JC-11 module
// outline (MO) documents for dimension definitions.
//
typedef  union {
   struct{
    UINT8 Height            : 4;  // Module Nominal Height max, in mm (baseline height = 15 mm)
    UINT8 Reserved54        : 4;
  } Field;
  UINT8 Value;
} SPD_MODULE_HEIGHT_S;

#define SPD_MODULE_HEIGHT_DECODE(Val)   (Val + 15)

// -----------------------------------------------------------------------------------------------------
//
// Module Maximum Thickness
//
#define SPD_MODULE_THICKNESS  231
//
// This byte defines the maximum thickness in millimeters of the fully assembled module including
// heat spreaders or other added components above the module circuit board surface, rounding up
// to the next integer. Refer to the relevant JEDEC JC-11 module outline (MO) documents for
// dimension definitions.
//
typedef  union {
   struct {
    UINT8 Front             : 4;  // Module Maximum Thickness max, Front, in mm (baseline thickness = 1 mm)
    UINT8 Back              : 4;  // Module Maximum Thickness max, Back, in mm (baseline thickness = 1 mm)
  } Field;
  UINT8 Value;
} SPD_MODULE_THICKNESS_S;

// -----------------------------------------------------------------------------------------------------
//
// Reference Raw Card Used
//
#define SPD_RAW_CARD          232
//
// This byte indicates which JEDEC reference design raw card was used as the basis for the module
// assembly, if any. Special reference raw card indicator, ZZ, is used when no JEDEC standard raw
// card reference design was used as the basis for the module design. Pre-production modules
// should be encoded as revision 0 in bits 7~5.
//
typedef  union {
   struct {
    UINT8 RefDesign         : 5;  // Reference Design
    UINT8 DesignRev         : 3;  // Design Revision
  } Field;
  UINT8 Value;
} SPD_RAW_CARD_S;

typedef enum {
  RawCard_A,
  RawCard_B,
  RawCard_C,
  RawCard_D,
  RawCard_E,
  RawCard_F,
  RawCard_G,
  RawCard_H,
  RawCard_J,
  RawCard_K,
  RawCard_L,
  RawCard_M,
  RawCard_N,
  RawCard_P,
  RawCard_R,
  RawCard_T,
  RawCard_U,
  RawCard_V,
  RawCard_W,
  RawCard_Y,
  RawCard_AA,
  RawCard_AB,
  RawCard_AC,
  RawCard_AD,
  RawCard_AE,
  RawCard_AF,
  RawCard_AG,
  RawCard_AH,
  RawCard_AJ,
  RawCard_AK,
  RawCard_Reserved,
  RawCard_ZZ
} SPD_RAW_CARD_E;

// -----------------------------------------------------------------------------------------------------
//
// DIMM Attributes
//
#define SPD_ATTRIBUTES        233
//
// This byte indicates the number of rows of DRAM packages (monolithic or 3D stacked) parallel to
// edge connector (independent of DRAM orientation) on each side of the printed circuit board, and
// whether the assembly is covered in a heat spreader. The temperature grade applies to all components
// on the assembly.
//
typedef  union {
   struct {
    UINT8 DramRows          : 2;  // # of rows of DRAMs on Module
    UINT8 HeatSpreader      : 1;  // Heat Spreader
    UINT8 Reserved_3        : 1;
    UINT8 TempGrade         : 4;  // Temperature Grade
  } Field;
  UINT8 Value;
} SPD_ATTRIBUTES_S;

typedef enum {
  TempGradeA1T,
  TempGradeA2T,
  TempGradeA3T,
  TempGradeIT,
  TempGradeST,
  TempGradeET,
  TempGradeRT,
  TempGradeNT,
  TempGradeXT,
  TempGradeMax
} SPD_TEMP_GRADE;

// -----------------------------------------------------------------------------------------------------
//
// Module Organization
//
#define SPD_MODULE_ORG        234
//
// This byte describes the organization of the SDRAM module. Bits 5~3 encode the number of package
// ranks on the module. Bit 6 describes whether the assembly has the same SDRAM density on
// all ranks or has different SDRAM densities on even and odd ranks.
//
typedef  union {
   struct {
    UINT8 Reserved_20       : 3;
    UINT8 RanksPerChannel   : 3;  // Number of Package Ranks per Channel
    UINT8 RankMix           : 1;  // Rank Mix
    UINT8 Reserved_7        : 1;
  } Field;
  UINT8 Value;
} SPD_MODULE_ORG_S;

#define SPD_PACKAGE_RANKS_DECODE(Val)   (Val + 1)

typedef enum {
  RankMixSymmetrical,
  RankMixAsymmetrical
} SPD_RANKMIX_E;

// -----------------------------------------------------------------------------------------------------
//
// Memory Channel Bus Width
//
#define SPD_CHANNEL_BUS_WIDTH 235
//
// This byte describes the width of the SDRAM memory bus on the module. Bits 2~0 encode the primary
// bus width. Bits 4~3 encode the bus extensions such as parity or ECC. Bits 6~5 defines the number
// of channels on each module.
//
typedef  union {
   struct {
    UINT8 Width             : 3;  // Primary bus width per Channel, in bits
    UINT8 WidthExt          : 2;  // Bus width extension per Channel, in bits
    UINT8 NumChannels       : 2;  // Number of Channels per DIMM
    UINT8 Reserved_7        : 1;
  } Field;
  UINT8 Value;
} SPD_CHANNEL_BUS_WIDTH_S;

#define SPD_CHANNEL_BUS_WIDTH_DECODE(Val)       (8 << Val)
#define SPD_CHANNEL_BUS_WIDTH_EXT_DECODE(Val)   (4 * Val)
#define SPD_CHANNELS_PER_DIMM_DECODE(Val)       (Val + 1)


// -----------------------------------------------------------------------------------------------------
//
// Annex A.1: Module Specific Bytes for Solder Down Memory
// 
// (Bytes 192~447, 0x0C0~0x1BF)
// 

// -----------------------------------------------------------------------------------------------------
//
// Annex A.2: Module Specific Bytes for Unbuffered Memory Module Types
// 
// (Bytes 192~447, 0x0C0~0x1BF)
// 

// -----------------------------------------------------------------------------------------------------
//
// Annex A.3: Module Specific Bytes for Registered Memory Module Types
// 
// (Bytes 192~447, 0x0C0~0x1BF)
// 

// -----------------------------------------------------------------------------------------------------
//
// Registering Clock Driver Manufacturer ID Code
// 
// (Used by RDIMM and LRDIMM)
//
#define SPD_MANUFACTURER_ID_RCD                   240
#define SPD_MANUFACTURER_ID_RCD_1ST_BYTE          240
#define SPD_MANUFACTURER_ID_RCD_2ND_BYTE          241
//
// Manufacturer ID Code: Per JEP106.
//

// -----------------------------------------------------------------------------------------------------
//
// Registering Clock Driver Device Type
// 
// (Used by RDIMM and LRDIMM)
//
#define SPD_DEVICE_TYPE_RCD                       242

typedef  union {
   struct {
    UINT8 DevType        : 4;
    UINT8 Reserved_54    : 2;
    UINT8 Reserved_6     : 1;
    UINT8 Installed      : 1;
  } Field;
  UINT8 Value;
} SPD_MODULE_DEVICE_TYPE_RCD_S;

// -----------------------------------------------------------------------------------------------------
//
// Registering Clock Driver Revision
// 
// (Used by RDIMM and LRDIMM)
//
#define SPD_DEVICE_REVISION_RCD                  243

// -----------------------------------------------------------------------------------------------------
// 
// Annex A.4: Module Specific Bytes for Load Reduced Memory Module Types
// 
// (Bytes 192~447, 0x0C0~0x1BF)
// 

// -----------------------------------------------------------------------------------------------------
//
// Data Buffer Manufacturer ID Code
//
#define SPD_MANUFACTURER_ID_DB                   244
#define SPD_MANUFACTURER_ID_DB_1ST_BYTE          244
#define SPD_MANUFACTURER_ID_DB_2ND_BYTE          245
//
// Manufacturer ID Code: Per JEP106.
//

// -----------------------------------------------------------------------------------------------------
//
// Data Buffer Device Type
//
#define SPD_DEVICE_TYPE_DB                       246

typedef  union {
   struct {
    UINT8 DevType        : 4;
    UINT8 Reserved_54    : 2;
    UINT8 Reserved_6     : 1;
    UINT8 Installed      : 1;
  } Field;
  UINT8 Value;
} SPD_MODULE_DEVICE_TYPE_DB_S;

// -----------------------------------------------------------------------------------------------------
//
// Data Buffer Revision
//
#define SPD_DEVICE_REVISION_DB                   247

// -----------------------------------------------------------------------------------------------------
// 
// Annex A.5: Module Specific Bytes for Differential Memory Module Types
// 
// (Bytes 192~447, 0x0C0~0x1BF)
// 
//
// Differential Memory Buffer Manufacturer ID Code
//
#define SPD_MANUFACTURER_ID_DMB                  244
#define SPD_MANUFACTURER_ID_DMB_1ST_BYTE         244
#define SPD_MANUFACTURER_ID_DMB_2ND_BYTE         245
//
// Manufacturer ID Code: Per JEP106.
//

// -----------------------------------------------------------------------------------------------------
//
// Differential Memory Buffer Device Type
//
#define SPD_DEVICE_TYPE_DMB                      246

typedef  union {
   struct {
    UINT8 DevType        : 4;
    UINT8 Reserved_54    : 2;
    UINT8 Reserved_6     : 1;
    UINT8 Installed      : 1;
  } Field;
  UINT8 Value;
} SPD_MODULE_DEVICE_TYPE_DMB_S;

// -----------------------------------------------------------------------------------------------------
//
// Data Buffer Revision
//
#define SPD_DEVICE_REVISION_DB                   247

// -----------------------------------------------------------------------------------------------------
//
// Annex A.6: Non-Volatile (NVDIMM-N) Hybrid Memory Parameters:
// 
// (Bytes 192~447, 0x0C0~0x1BF)
// 

// -----------------------------------------------------------------------------------------------------
//
// Annex A.7: Non-Volatile (NVDIMM-P) Hybrid Memory Parameters:
// 
// (Bytes 192~447, 0x0C0~0x1BF)
// 

// -----------------------------------------------------------------------------------------------------
//
// Byte 510 (0x07E): Cyclical Redundancy Code (CRC) for SPD Bytes 0~509, Least Significant Byte
// Byte 511 (0x07F): Cyclical Redundancy Code (CRC) for SPD Bytes 0~509, Most Significant Byte
//
// This two-byte field contains the calculated CRC for bytes 0~509 (0x000~0x1FD) in the SPD. The
// following algorithm and data structures (shown in C) are to be followed in calculating and checking
// the code.
//
#define SPD_CRC_16                510
#define SPD_CRC_LSB               510
#define SPD_CRC_MSB               511

typedef SPD_VALUE16 SPD_CRC_S;
 
#define SPD_CRC_COUNT             510

// -----------------------------------------------------------------------------------------------------
//
// Blocks 8~9: Manufacturing Information: (Bytes 512~639, 0x200~0x27F)
//

// -----------------------------------------------------------------------------------------------------
//
// Byte 512 (0x200): Module Manufacturer ID Code, First Byte
// Byte 513 (0x201): Module Manufacturer ID Code, Second Byte
// 
//   This two-byte field indicates the manufacturer of the module, and shall be encoded as follows: the
//   first byte is the number of continuation bytes indicated in JEP-106; the second byte is the last nonzero
//   byte of the manufacturer's ID code, again as indicated in JEP-106.
//
#define SPD_MODULE_MANUFACTURER_ID_CODE_1ST_BYTE     512
#define SPD_MODULE_MANUFACTURER_ID_CODE_2nd_BYTE     513

// -----------------------------------------------------------------------------------------------------
//
// Byte 514 (0x202): Module Manufacturing Location
// 
// The module manufacturer shall include an identifier that uniquely defines the manufacturing location
// of the memory module. While the SPD specification will not attempt to present a decode table
// for manufacturing sites, the individual manufacturer must keep track of manufacturing location and
// its appropriate decode represented in this byte.
//
#define SPD_MODULE_MANUFACTURING_LOCATION            514

// -----------------------------------------------------------------------------------------------------
//
// Bytes 515~516 (0x203~0x204): Module Manufacturing Date
// 
// The module manufacturer shall include a date code for the module. The JEDEC definitions for
// bytes 515 and 516 are year and week respectively. These bytes must be represented in Binary
// Coded Decimal (BCD). For example, week 47 in year 2014 would be coded as 0x14 (0001 0100)
// in byte 515 and 0x47 (0100 0111) in byte 516.
// 
// 
#define SPD_MODULE_MANUFACTURING_DATE                515 // Year, Week, Coded in BCD
#define SPD_MODULE_MANUFACTURING_YEAR                515 // Year, Coded in BCD
#define SPD_MODULE_MANUFACTURING_WEEK                516 // Week, Coded in BCD

typedef  union {
   struct {
    UINT8 MfgYear;
    UINT8 MfgWeek;
  } Fields;
  UINT16 Value;
} SPD_MFG_DATE_S;

// -----------------------------------------------------------------------------------------------------
//
// Bytes 517~520 (0x205~0x208): Module Serial Number
// 
// The supplier shall include a unique serial number for the module. The supplier may use whatever
// decode method desired to maintain a unique serial number for
// each module.
// 
// One method of achieving this is by assigning a byte in the field from 517~520 as a tester ID byte
// and using the remaining bytes as a sequential serial number. Bytes 512~520 will then result in a
// nine-byte unique module identifier. Note that part number is not included in this identifier: the supplier
// may not give the same value for Bytes 517~520 to more than one DIMM even if the DIMMs Byte 513,
// Bits 7~0 Byte 512, Bit 7 Byte 512, Bits 6~0 have different part numbers.
#define SPD_MODULE_SERIAL_NUMBER                     517
#define SPD_MODULE_SERIAL_NUMBER_LEN                 (520 - 517 + 1) // 4 Bytes

typedef  struct {
  UINT8 String[SPD_MODULE_SERIAL_NUMBER_LEN];
} SPD_MODULE_SERIAL_NUMBER_S;

// -----------------------------------------------------------------------------------------------------
//
// Bytes 521~550 (0x209~0x226): Module Part Number
// 
// The manufacturer's part number shall be included and is written in ASCII format within these
// bytes. Unused digits are coded as ASCII blanks (0x20).
// Byte 551 (0x227): Module Revision Code
// This refers to the module revision code. While the SPD specification will not attempt to define the
// format for this information, the individual manufacturer may keep track of the revision code and its
// appropriate decode represented in this byte. This revision code refers to the manufacturer's
// assembly revision level and may be different than the raw card revision in SPD bytes 128 and 130.
#define SPD_MODULE_PART_NUMBER                       521
#define SPD_MODULE_PART_NUMBER_LEN                   (550 - 521 + 1) // 20 Bytes

typedef  struct {
  UINT8 String[SPD_MODULE_PART_NUMBER_LEN];
} SPD_MODULE_PART_NUMBER_S;

#define SPD_MODULE_REVISION_CODE      551

// -----------------------------------------------------------------------------------------------------
//
// Byte 552 (0x228): DRAM Manufacturer ID Code, First Byte
// Byte 553 (0x229): DRAM Manufacturer ID Code, Second Byte
// 
// This two-byte field indicates the manufacturer of the DRAM on the module, and shall be encoded
// as follows: the first byte is the number of continuation bytes indicated in JEP-106; the second byte
// is the last non-zero byte of the manufacturer's ID code, again as indicated in JEP-106.
// Example: See bytes 512~513 for example manufacturer codes.

#define SPD_DRAM_MANUFACTURERS_ID_CODE_1ST_BYTE      552
#define SPD_DRAM_MANUFACTURERS_ID_CODE_2nd_BYTE      553

// -----------------------------------------------------------------------------------------------------
//
// Byte 554 (0x22A): DRAM Stepping
// 
// This byte defines the vendor die revision level (often called the "stepping") of the DRAMs on the
// module. This byte is optional. For modules without DRAM stepping information, this byte should be
// programmed to 0xFF.
#define SPD_DRAM_STEPPING                            554

// -----------------------------------------------------------------------------------------------------
//
// Base Configuration and DRAM Parameters 0
//
//  Block           Range             Description 
//  0         0~63      0x000~0x03F   Base Configuration and DRAM Parameters
//
typedef  struct {
  SPD_NUM_BYTES_S                   NumBytes;                       // 0
  SPD_REVISION_S                    Revision;                       // 1
  SPD_KEY_BYTE_S                    KeyByte1;                       // 2
  SPD_KEY_BYTE2_S                   KeyByte2;                       // 3
  SPD_DENSITY_PACKAGE_S             FirstDensity;                   // 4
  SPD_ADDRESSING_S                  FirstAddressing;                // 5
  SPD_IO_WIDTH_S                    FirstIoWidth;                   // 6
  SPD_BANKS_S                       FirstBanks;                     // 7
  SPD_DENSITY_PACKAGE_S             SecondDensity;                  // 8
  SPD_ADDRESSING_S                  SecondAddressing;               // 9
  SPD_IO_WIDTH_S                    SecondIoWidth;                  // 10
  SPD_BANKS_S                       SecondBanks;                    // 11
  SPD_OPT_FEATURES_S                OptionalFeatures;               // 12
  UINT8                             Reserved_13;                    // 13
  UINT8                             Reserved_14;                    // 14
  UINT8                             Reserved_15;                    // 15
  SPD_NOMINAL_VDD_S                 Vdd;                            // 16
  SPD_NOMINAL_VDQ_S                 Vdq;                            // 17
  SPD_NOMINAL_VDP_S                 Vdp;                            // 18
  UINT8                             Reserved_19;                    // 19
  SPD_TIMING_S                      TckAvgMin;                      // 20
  SPD_TIMING_S                      TckAvgMax;                      // 22
  UINT8                             CasLatSupported[SPD_TCL_BYTES]; // 24 - 28
  UINT8                             Reserved_29;                    // 29
  SPD_TIMING_S                      TaaMin;                         // 30
  SPD_TIMING_S                      TrcdMin;                        // 32
  SPD_TIMING_S                      TrpMin;                         // 34
  SPD_TIMING_S                      TrasMin;                        // 36
  SPD_TIMING_S                      TrcMin;                         // 48
  SPD_TIMING_S                      TwrMin;                         // 40
  SPD_TIMING_S                      Trfc1Min;                       // 42
  SPD_TIMING_S                      Trfc2Min;                       // 44
  SPD_TIMING_S                      TrfcSbMin;                      // 46
  SPD_TIMING_S                      Trfc1DlrMin;                    // 48
  SPD_TIMING_S                      Trfc2DlrMin;                    // 50
  SPD_TIMING_S                      Trfc2SbDlrMin;                  // 52
  SPD_REF_MANAGEMENT_FIRST_S        PriRefManagement1st;            // 54
  SPD_REF_MANAGEMENT_SECOND_S       PriRefManagement2nd;            // 55
  SPD_REF_MANAGEMENT_FIRST_S        SecRefManagement1st;            // 56
  SPD_REF_MANAGEMENT_SECOND_S       SecRefManagement2nd;            // 57
  UINT8                             Reserved_58_63[63-57];          // 58 - 63
} SPD_BASE_CONFIG_0_S;

// -----------------------------------------------------------------------------------------------------
//
// SPD BASE CONFIGURATION AND DRAM PRAMETERS
//
// Bytes 0 - 127
//
//
typedef  struct {
  SPD_NUM_BYTES_S                   NumBytes;                       // 0
  SPD_REVISION_S                    Revision;                       // 1
  SPD_KEY_BYTE_S                    KeyByte1;                       // 2
  SPD_KEY_BYTE2_S                   KeyByte2;                       // 3
  SPD_DENSITY_PACKAGE_S             FirstDensity;                   // 4
  SPD_ADDRESSING_S                  FirstAddressing;                // 5
  SPD_IO_WIDTH_S                    FirstIoWidth;                   // 6
  SPD_BANKS_S                       FirstBanks;                     // 7
  SPD_DENSITY_PACKAGE_S             SecondDensity;                  // 8
  SPD_ADDRESSING_S                  SecondAddressing;               // 9
  SPD_IO_WIDTH_S                    SecondIoWidth;                  // 10
  SPD_BANKS_S                       SecondBanks;                    // 11
  SPD_OPT_FEATURES_S                OptionalFeatures;               // 12
  UINT8                             Reserved_13;                    // 13
  UINT8                             Reserved_14;                    // 14
  UINT8                             Reserved_15;                    // 15
  SPD_NOMINAL_VDD_S                 Vdd;                            // 16
  SPD_NOMINAL_VDQ_S                 Vdq;                            // 17
  SPD_NOMINAL_VDP_S                 Vdp;                            // 18
  UINT8                             Reserved_19;                    // 19
  SPD_TIMING_S                      TckAvgMin;                      // 20
  SPD_TIMING_S                      TckAvgMax;                      // 22
  UINT8                             CasLatSupported[SPD_TCL_BYTES]; // 24 - 28
  UINT8                             Reserved_29;                    // 29
  SPD_TIMING_S                      TaaMin;                         // 30
  SPD_TIMING_S                      TrcdMin;                        // 32
  SPD_TIMING_S                      TrpMin;                         // 34
  SPD_TIMING_S                      TrasMin;                        // 36
  SPD_TIMING_S                      TrcMin;                         // 48
  SPD_TIMING_S                      TwrMin;                         // 40
  SPD_TIMING_S                      Trfc1Min;                       // 42
  SPD_TIMING_S                      Trfc2Min;                       // 44
  SPD_TIMING_S                      TrfcSbMin;                      // 46
  SPD_TIMING_S                      Trfc1DlrMin;                    // 48
  SPD_TIMING_S                      Trfc2DlrMin;                    // 50
  SPD_TIMING_S                      Trfc2SbDlrMin;                  // 52
  SPD_REF_MANAGEMENT_FIRST_S        PriRefManagement1st;            // 54
  SPD_REF_MANAGEMENT_SECOND_S       PriRefManagement2nd;            // 55
  SPD_REF_MANAGEMENT_FIRST_S        SecRefManagement1st;            // 56
  SPD_REF_MANAGEMENT_SECOND_S       SecRefManagement2nd;            // 57
  UINT8                             Reserved_58_127[127-57];        // 58 - 127
} SPD_BASE_CONFIG_S;

// -----------------------------------------------------------------------------------------------------
//
// Annex A.0: Common SPD Bytes for All Module Types
// 
// (Bytes 192~239, 0x0C0~0x0EF)
// 
//
typedef  struct{
  SPD_REVISION_S                     ModuleRevision;       // 192
  UINT8                              Reserved_193;         // 193
  SPD_MANUFACTURER_ID_S              SpdMfgID;             // 194
  SPD_MODULE_DEVICE_TYPE_SPD_S       SpdDevType;           // 196
  SPD_MODULE_REVISION_S              SpdRevision;          // 197
  SPD_MANUFACTURER_ID_S              Pmic0MfgId;           // 198
  SPD_MODULE_DEVICE_TYPE_PMIC_S      Pmic0DevType;         // 200
  SPD_MODULE_REVISION_S              Pmic0Revision;        // 201
  SPD_MANUFACTURER_ID_S              Pmic1MfgId;           // 202
  SPD_MODULE_DEVICE_TYPE_PMIC_S      Pmic1DevType;         // 204 
  SPD_MODULE_REVISION_S              Pmic1Revision;        // 205 
  SPD_MANUFACTURER_ID_S              Pmic2MfgId;           // 206
  SPD_MODULE_DEVICE_TYPE_PMIC_S      Pmic2DevType;         // 208
  SPD_MODULE_REVISION_S              Pmic2Revision;        // 209
  SPD_MANUFACTURER_ID_S              TsMfgId;              // 210
  SPD_DEVICE_TYPE_TS_S               TsDevType;            // 212
  SPD_MODULE_REVISION_S              TsRevision;           // 213
  UINT8                              Reserved_214_229[16]; // 214 - 229
  SPD_MODULE_HEIGHT_S                Height;               // 230
  SPD_MODULE_THICKNESS_S             Thickness;            // 231
  SPD_RAW_CARD_S                     RawCard;              // 232
  SPD_ATTRIBUTES_S                   Attributes;           // 233
  SPD_MODULE_ORG_S                   ModuleOrg;            // 234
  SPD_CHANNEL_BUS_WIDTH_S            ChBusWidth;           // 235
  UINT8                              Reserved_236_239[4];  // 236 - 239
} SPD_ANNEX_COMMON_S;

// -----------------------------------------------------------------------------------------------------
//
// Annex A.1: Module Specific Bytes for Solder Down Memory
// 
// (Bytes 192~447, 0x0C0~0x1BF)
// 
// This section defines the encoding of SPD bytes 192~447 when Memory Technology Key Byte 2
// contains the value 0x12 and Module Type Key Byte 3 contains any of the following:
// 
//  - 0xHB, Solder Down
//  - where H refers to the hybrid memory architecture, if any present
// 
// Reserved -- Must be coded as 0x00
typedef  struct{
  SPD_ANNEX_COMMON_S                 Common;                // 192
  UINT8                              Reserved_240_447[208]; // 240 - 447
} SPD_SODOWN_ANNEX_S;

// -----------------------------------------------------------------------------------------------------
//
// Annex A.2: Module Specific Bytes for Unbuffered Memory Module Types
// 
// (Bytes 192~447, 0x0C0~0x1BF)
// 
// This section defines the encoding of SPD bytes 192~447 when Memory Technology Key Byte 2
//  contains the value 0x12 and Module Type Key Byte 3 contains any of the following:
// 
//  - 0x02, UDIMM
//  - 0x03, SODIMM
// 
// Reserved -- Must be coded as 0x00
//
typedef  struct{
  SPD_ANNEX_COMMON_S                 Common;                // 192
  UINT8                              Reserved_240_447[208]; // 240 -447
} SPD_UDIMM_ANNEX_S;

// -----------------------------------------------------------------------------------------------------
//
// Annex A.3: Module Specific Bytes for Registered Memory Module Types
// 
// (Bytes 192~447, 0x0C0~0x1BF)
// 
// This section defines the encoding of SPD bytes 192~447 when Memory Technology Key Byte 2
//  contains the value 0x12 and Module Type Key Byte 3 contains any of the following:
// 
//  - 0xH1, RDIMM
//  - 0xH8, SO-RDIMM
//  - where H refers to the hybrid memory architecture, if any present on the module
//
// 
typedef  struct{
  SPD_ANNEX_COMMON_S                 Common;                // 192 
  SPD_MANUFACTURER_ID_S              RcdManfId;             // 240
  SPD_MODULE_DEVICE_TYPE_RCD_S       RcdDevType;            // 242
  SPD_MODULE_REVISION_S              RcdRevision;           // 243
  UINT8                              Reserved_244_447[204]; // 244 - 447
} SPD_RDIMM_ANNEX_S;

// -----------------------------------------------------------------------------------------------------
// 
// Annex A.4: Module Specific Bytes for Load Reduced Memory Module Types
// 
// (Bytes 192~447, 0x0C0~0x1BF)
// 
// This section defines the encoding of SPD bytes 192~447 when Memory Technology Key Byte 2
// contains the value 0x0C and Module Type Key Byte 3 contains any of the following:
// 
//   -0x04, LRDIMM
//
typedef  struct{
  SPD_ANNEX_COMMON_S                 Common;                // 192
  SPD_MANUFACTURER_ID_S              RcdManfId;             // 240
  SPD_MODULE_DEVICE_TYPE_RCD_S       RcdDevType;            // 242
  SPD_MODULE_REVISION_S              RcdRevision;           // 243
  SPD_MANUFACTURER_ID_S              DbMfgId;               // 244
  SPD_MODULE_DEVICE_TYPE_DB_S        DbDevType;             // 246
  SPD_MODULE_REVISION_S              DbRevision;            // 247
  UINT8                              Reserved_248_447[200]; // 248 - 447
} SPD_LRDIMM_ANNEX_S;

// -----------------------------------------------------------------------------------------------------
// 
// Annex A.5: Module Specific Bytes for Differential Memory Module Types
// 
// (Bytes 192~447, 0x0C0~0x1BF)
// 
// This section defines the encoding of SPD bytes 192~447 when Memory Technology Key Byte 2
// contains the value 0x0C and Module Type Key Byte 3 contains any of the following:
// 
//  - 0xHA, DDIMM
//  - where H refers to the hybrid memory architecture, if any present on the module
//
//
// Differential Memory Buffer Manufacturer ID Code
//
typedef  struct{
  SPD_ANNEX_COMMON_S                 Common;                // 192
  SPD_MANUFACTURER_ID_S              DmbManfId;             // 240
  SPD_MODULE_DEVICE_TYPE_DMB_S       DmbDevType;            // 242
  SPD_MODULE_REVISION_S              DmbRevision;           // 243
  UINT8                              Reserved_244_447[204]; // 244 - 447
} SPD_DDIMM_ANNEX_S;

// -----------------------------------------------------------------------------------------------------
//
// Annex A.6: Non-Volatile (NVDIMM-N) Hybrid Memory Parameters:
// 
// (Bytes 192~447, 0x0C0~0x1BF)
// 
// This section defines the encoding of SPD bytes 192~447 when Module Type Key Byte 3
//  contains the following:
// 
//  - 0x9M, NVDIMM-N
//  - where M refers to the base memory architecture
// 
// NVDIMM types defined at this time are:
//
//    NVDIMM-N Persistent DRAM using NAND flash
//
// This Annex uses the same SPD fields as LRDIMM
typedef  struct{
  SPD_ANNEX_COMMON_S                 Common;                // 192
  SPD_MANUFACTURER_ID_S              RcdManfId;             // 240
  SPD_MODULE_DEVICE_TYPE_RCD_S       RcdDevType;            // 242
  SPD_MODULE_REVISION_S              RcdRevision;           // 243
  SPD_MANUFACTURER_ID_S              DbMfgId;               // 244
  SPD_MODULE_DEVICE_TYPE_DB_S        DbDevType;             // 246
  SPD_MODULE_REVISION_S              DbRevision;            // 247
  UINT8                              Reserved_248_447[200]; // 248 - 447
} SPD_NVDIMM_N_ANNEX_S;

// -----------------------------------------------------------------------------------------------------
//
// Annex A.7: Non-Volatile (NVDIMM-P) Hybrid Memory Parameters:
// 
// (Bytes 192~447, 0x0C0~0x1BF)
// 
// This section defines the encoding of SPD bytes 192~447 when Module Type Key Byte 3
// contains the following:
// 
//  - 0xAM, NVDIMM-P
//  - where M refers to the base memory architecture
// 
//   NVDIMM types defined at this time are:
//
//     NVDIMM-P Transactional credit based memory module
// 
// This Annex uses the same SPD fields as LRDIMM
typedef  struct{
  SPD_ANNEX_COMMON_S                 Common;                // 192
  SPD_MANUFACTURER_ID_S              RcdManfId;             // 240
  SPD_MODULE_DEVICE_TYPE_RCD_S       RcdDevType;            // 242
  SPD_MODULE_REVISION_S              RcdRevision;           // 243
  SPD_MANUFACTURER_ID_S              DbMfgId;               // 244
  SPD_MODULE_DEVICE_TYPE_DB_S        DbDevType;             // 246
  SPD_MODULE_REVISION_S              DbRevision;            // 247
  UINT8                              Reserved_248_447[200]; // 248 - 447
} SPD_NVDIMM_P_ANNEX_S;

// -----------------------------------------------------------------------------------------------------
//
// Standard Module Parameters
//
typedef  union {
  SPD_ANNEX_COMMON_S                 Common;                 // Annex A0 Common 
  SPD_SODOWN_ANNEX_S                 Sodown;                 // Annex A1 Soldered Down
  SPD_UDIMM_ANNEX_S                  Udimm;                  // Annex A2 UDIMM
  SPD_RDIMM_ANNEX_S                  Rdimm;                  // Annex A3 RDIMM
  SPD_LRDIMM_ANNEX_S                 Lrdimm;                 // Annex A4 LRDIMM
  SPD_DDIMM_ANNEX_S                  Ddimm;                  // Annex A5 DDIMM
  SPD_NVDIMM_N_ANNEX_S               Nvdimmn;                // Annex A6 NVDIMM-N
  SPD_NVDIMM_P_ANNEX_S               Nvdimmp;                // Annex A7 NVDIMM-P
} SPD_MODULE_SPECIFIC_S;
 
// -----------------------------------------------------------------------------------------------------
// Blocks 8~9: Manufacturing Information
// 
// (Bytes 512~639, 0x200~0x27F)
//
typedef  struct {
  SPD_MANUFACTURER_ID_S               ModuleMfgId;               // 512
  UINT8                               ModuleMfgLocation;         // 514
  SPD_MFG_DATE_S                      ModuleMfgDate;             // 515
  SPD_MODULE_SERIAL_NUMBER_S          ModuleSerialNumber;        // 517 - 520
  SPD_MODULE_PART_NUMBER_S            ModulePartNumber;          // 521 - 550
  SPD_REVISION_S                      ModuleRevisionCode;        // 551
  SPD_MANUFACTURER_ID_S               DramMfgId;                 // 552
  UINT8                               DramStepping;              // 554
  UINT8                               Reserved_555_639[639-554]; // 555 - 639
} SPD_MANUFACTURING_INFO_S;

// -----------------------------------------------------------------------------------------------------
//
// DDR5 SPD DATA STRUCTURE BY BLOCK
//
typedef  struct {
  SPD_BLOCK_S                         Block[SPD_NUM_BLOCKS];   ///< SpdBlocks 0 - 15
} DDR5_SPD_BLOCKS_S;

// -----------------------------------------------------------------------------------------------------
//
// DDR5 SPD DATA STRUCTURE
//
typedef  struct {
  SPD_BASE_CONFIG_S                   Base;                            ///< 0~127
  UINT8                               Reserved_128_191[191-127];       ///< 128~191
  SPD_MODULE_SPECIFIC_S               ModuleSpec;                      ///< 192~447
  UINT8                               Reserved_448_509[509-447];       ///< 448~509
  SPD_CRC_S                           Crc;                             ///< 510~511
  SPD_MANUFACTURING_INFO_S            MfgInfo;                         ///< 512~639
  SPD_BLOCK_S                         End_User_Programmable_640_703;   ///< 640~703 End User Programmable
  SPD_BLOCK_S                         End_User_Programmable_704_767;   ///< 704~767 End User Programmable
  SPD_BLOCK_S                         End_User_Programmable_760_831;   ///< 760~831 End User Programmable
  SPD_BLOCK_S                         End_User_Programmable_832_895;   ///< 832~895 End User Programmable
  SPD_BLOCK_S                         End_User_Programmable_896_959;   ///< 896~959 End User Programmable
  SPD_BLOCK_S                         End_User_Programmable_960_1023;  ///< 960~1023 End User Programmable
} DDR5_SPD_S;

#pragma pack(pop)

#endif /* _MTSPD5_H_ */

