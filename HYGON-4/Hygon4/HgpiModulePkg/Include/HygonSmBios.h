/* $NoKeywords:$ */

/**
 * @file
 *
 * HygonSmmCommunication.h
 *
 * Contains definition for HygonSmmCommunication
 *
 */
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
 *
 ***************************************************************************/

#ifndef _HYGON_SMBIOS_H_
#define _HYGON_SMBIOS_H_

#pragma pack (push, 1)

///
/// Reference SMBIOS 2.7, chapter 6.1.2.
/// The UEFI Platform Initialization Specification reserves handle number FFFEh for its
/// EFI_SMBIOS_PROTOCOL.Add() function to mean "assign an unused handle number automatically."
/// This number is not used for any other purpose by the SMBIOS specification.
///
#define HYGON_SMBIOS_HANDLE_PI_RESERVED  0xFFFE

#define HYGON_EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION           4
#define HYGON_EFI_SMBIOS_TYPE_CACHE_INFORMATION               7
#define HYGON_EFI_SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY           16
#define HYGON_EFI_SMBIOS_TYPE_MEMORY_DEVICE                   17
#define HYGON_EFI_SMBIOS_TYPE_32BIT_MEMORY_ERROR_INFORMATION  18
#define HYGON_EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS     19
#define HYGON_EFI_SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS    20

///
/// Types 0 through 127 (7Fh) are reserved for and defined by this
/// specification. Types 128 through 256 (80h to FFh) are available for system- and OEM-specific information.
///
typedef UINT8 HYGON_SMBIOS_TYPE;

///
/// Specifies the structure's handle, a unique 16-bit number in the range 0 to 0FFFEh (for version
/// 2.0) or 0 to 0FEFFh (for version 2.1 and later). The handle can be used with the Get SMBIOS
/// Structure function to retrieve a specific structure; the handle numbers are not required to be
/// contiguous. For v2.1 and later, handle values in the range 0FF00h to 0FFFFh are reserved for
/// use by this specification.
/// If the system configuration changes, a previously assigned handle might no longer exist.
/// However once a handle has been assigned by the BIOS, the BIOS cannot re-assign that handle
/// number to another structure.
///
typedef UINT16 HYGON_SMBIOS_HANDLE;

///
/// The Smbios structure header.
///
typedef struct {
  HYGON_SMBIOS_TYPE      Type;
  UINT8                  Length;
  HYGON_SMBIOS_HANDLE    Handle;
} HYGON_SMBIOS_STRUCTURE;

///
/// Text strings associated with a given SMBIOS structure are returned in the dmiStrucBuffer, appended directly after
/// the formatted portion of the structure. This method of returning string information eliminates the need for
/// application software to deal with pointers embedded in the SMBIOS structure. Each string is terminated with a null
/// (00h) BYTE and the set of strings is terminated with an additional null (00h) BYTE. When the formatted portion of
/// a SMBIOS structure references a string, it does so by specifying a non-zero string number within the structure's
/// string-set. For example, if a string field contains 02h, it references the second string following the formatted portion
/// of the SMBIOS structure. If a string field references no string, a null (0) is placed in that string field. If the
/// formatted portion of the structure contains string-reference fields and all the string fields are set to 0 (no string
/// references), the formatted section of the structure is followed by two null (00h) BYTES.
///
typedef UINT8 HYGON_SMBIOS_TABLE_STRING;

///
/// Processor Information - Voltage.
///
typedef struct {
  UINT8    ProcessorVoltageCapability5V       : 1;
  UINT8    ProcessorVoltageCapability3_3V     : 1;
  UINT8    ProcessorVoltageCapability2_9V     : 1;
  UINT8    ProcessorVoltageCapabilityReserved : 1; ///< Bit 3, must be zero.
  UINT8    ProcessorVoltageReserved           : 3; ///< Bits 4-6, must be zero.
  UINT8    ProcessorVoltageIndicateLegacy     : 1;
} HYGON_PROCESSOR_VOLTAGE;

///
/// Processor ID Field Description
///
typedef struct {
  UINT32    ProcessorSteppingId : 4;
  UINT32    ProcessorModel      :     4;
  UINT32    ProcessorFamily     :    4;
  UINT32    ProcessorType       :      2;
  UINT32    ProcessorReserved1  : 2;
  UINT32    ProcessorXModel     :    4;
  UINT32    ProcessorXFamily    :   8;
  UINT32    ProcessorReserved2  : 4;
} HYGON_PROCESSOR_SIGNATURE;

typedef struct {
  UINT32    ProcessorFpu       : 1;
  UINT32    ProcessorVme       : 1;
  UINT32    ProcessorDe        : 1;
  UINT32    ProcessorPse       : 1;
  UINT32    ProcessorTsc       : 1;
  UINT32    ProcessorMsr       : 1;
  UINT32    ProcessorPae       : 1;
  UINT32    ProcessorMce       : 1;
  UINT32    ProcessorCx8       : 1;
  UINT32    ProcessorApic      : 1;
  UINT32    ProcessorReserved1 : 1;
  UINT32    ProcessorSep       : 1;
  UINT32    ProcessorMtrr      : 1;
  UINT32    ProcessorPge       : 1;
  UINT32    ProcessorMca       : 1;
  UINT32    ProcessorCmov      : 1;
  UINT32    ProcessorPat       : 1;
  UINT32    ProcessorPse36     : 1;
  UINT32    ProcessorPsn       : 1;
  UINT32    ProcessorClfsh     : 1;
  UINT32    ProcessorReserved2 : 1;
  UINT32    ProcessorDs        : 1;
  UINT32    ProcessorAcpi      : 1;
  UINT32    ProcessorMmx       : 1;
  UINT32    ProcessorFxsr      : 1;
  UINT32    ProcessorSse       : 1;
  UINT32    ProcessorSse2      : 1;
  UINT32    ProcessorSs        : 1;
  UINT32    ProcessorReserved3 : 1;
  UINT32    ProcessorTm        : 1;
  UINT32    ProcessorReserved4 : 2;
} HYGON_PROCESSOR_FEATURE_FLAGS;

typedef struct {
  HYGON_PROCESSOR_SIGNATURE        Signature;
  HYGON_PROCESSOR_FEATURE_FLAGS    FeatureFlags;
} HYGON_PROCESSOR_ID_DATA;

///
/// Processor Information (Type 4).
///
/// The information in this structure defines the attributes of a single processor;
/// a separate structure instance is provided for each system processor socket/slot.
/// For example, a system with an IntelDX2 processor would have a single
/// structure instance, while a system with an IntelSX2 processor would have a structure
/// to describe the main CPU, and a second structure to describe the 80487 co-processor.
///
typedef struct {
  HYGON_SMBIOS_STRUCTURE       Hdr;
  HYGON_SMBIOS_TABLE_STRING    Socket;
  UINT8                        ProcessorType;
  UINT8                        ProcessorFamily;
  HYGON_SMBIOS_TABLE_STRING    ProcessorManufacture;
  HYGON_PROCESSOR_ID_DATA      ProcessorId;
  HYGON_SMBIOS_TABLE_STRING    ProcessorVersion;
  HYGON_PROCESSOR_VOLTAGE      Voltage;
  UINT16                       ExternalClock;
  UINT16                       MaxSpeed;
  UINT16                       CurrentSpeed;
  UINT8                        Status;
  UINT8                        ProcessorUpgrade;
  UINT16                       L1CacheHandle;
  UINT16                       L2CacheHandle;
  UINT16                       L3CacheHandle;
  HYGON_SMBIOS_TABLE_STRING    SerialNumber;
  HYGON_SMBIOS_TABLE_STRING    AssetTag;
  HYGON_SMBIOS_TABLE_STRING    PartNumber;
  //
  // Add for smbios 2.5
  //
  UINT8                        CoreCount;
  UINT8                        EnabledCoreCount;
  UINT8                        ThreadCount;
  UINT16                       ProcessorCharacteristics;
  //
  // Add for smbios 2.6
  //
  UINT16                       ProcessorFamily2;
  //
  // Add for smbios 3.0
  //
  UINT16                       CoreCount2;
  UINT16                       EnabledCoreCount2;
  UINT16                       ThreadCount2;
} HYGON_SMBIOS_TABLE_TYPE4;

///
/// Cache Information - SRAM Type.
///
typedef struct {
  UINT16    Other         : 1;
  UINT16    Unknown       : 1;
  UINT16    NonBurst      : 1;
  UINT16    Burst         : 1;
  UINT16    PipelineBurst : 1;
  UINT16    Synchronous   : 1;
  UINT16    Asynchronous  : 1;
  UINT16    Reserved      : 9;
} HYGON_CACHE_SRAM_TYPE_DATA;

///
/// Cache Information (Type 7).
///
/// The information in this structure defines the attributes of CPU cache device in the system.
/// One structure is specified for each such device, whether the device is internal to
/// or external to the CPU module.  Cache modules can be associated with a processor structure
/// in one or two ways, depending on the SMBIOS version.
///
typedef struct {
  HYGON_SMBIOS_STRUCTURE        Hdr;
  HYGON_SMBIOS_TABLE_STRING     SocketDesignation;
  UINT16                        CacheConfiguration;
  UINT16                        MaximumCacheSize;
  UINT16                        InstalledSize;
  HYGON_CACHE_SRAM_TYPE_DATA    SupportedSRAMType;
  HYGON_CACHE_SRAM_TYPE_DATA    CurrentSRAMType;
  UINT8                         CacheSpeed;
  UINT8                         ErrorCorrectionType;
  UINT8                         SystemCacheType;
  UINT8                         Associativity;
  //
  // Add for smbios 3.1.0
  //
  UINT32                        MaximumCacheSize2;
  UINT32                        InstalledSize2;
} HYGON_SMBIOS_TABLE_TYPE7;

///
/// Physical Memory Array - Location.
///
typedef enum {
  HygonMemoryArrayLocationOther                 = 0x01,
  HygonMemoryArrayLocationUnknown               = 0x02,
  HygonMemoryArrayLocationSystemBoard           = 0x03,
  HygonMemoryArrayLocationIsaAddonCard          = 0x04,
  HygonMemoryArrayLocationEisaAddonCard         = 0x05,
  HygonMemoryArrayLocationPciAddonCard          = 0x06,
  HygonMemoryArrayLocationMcaAddonCard          = 0x07,
  HygonMemoryArrayLocationPcmciaAddonCard       = 0x08,
  HygonMemoryArrayLocationProprietaryAddonCard  = 0x09,
  HygonMemoryArrayLocationNuBus                 = 0x0A,
  HygonMemoryArrayLocationPc98C20AddonCard      = 0xA0,
  HygonMemoryArrayLocationPc98C24AddonCard      = 0xA1,
  HygonMemoryArrayLocationPc98EAddonCard        = 0xA2,
  HygonMemoryArrayLocationPc98LocalBusAddonCard = 0xA3
} HYGON_SMEMORY_ARRAY_LOCATION;

///
/// Physical Memory Array - Use.
///
typedef enum {
  HygonMemoryArrayUseOther          = 0x01,
  HygonMemoryArrayUseUnknown        = 0x02,
  HygonMemoryArrayUseSystemMemory   = 0x03,
  HygonMemoryArrayUseVideoMemory    = 0x04,
  HygonMemoryArrayUseFlashMemory    = 0x05,
  HygonMemoryArrayUseNonVolatileRam = 0x06,
  HygonMemoryArrayUseCacheMemory    = 0x07
} HYGON_SMEMORY_ARRAY_USE;

///
/// Physical Memory Array - Error Correction Types.
///
typedef enum {
  HygonMemoryErrorCorrectionOther        = 0x01,
  HygonMemoryErrorCorrectionUnknown      = 0x02,
  HygonMemoryErrorCorrectionNone         = 0x03,
  HygonMemoryErrorCorrectionParity       = 0x04,
  HygonMemoryErrorCorrectionSingleBitEcc = 0x05,
  HygonMemoryErrorCorrectionMultiBitEcc  = 0x06,
  HygonMemoryErrorCorrectionCrc          = 0x07
} HYGON_SMEMORY_ERROR_CORRECTION;

///
/// Physical Memory Array (Type 16).
///
/// This structure describes a collection of memory devices that operate
/// together to form a memory address space.
///
typedef struct {
  HYGON_SMBIOS_STRUCTURE    Hdr;
  UINT8                     Location;                       ///< The enumeration value from HYGON_MEMORY_ARRAY_LOCATION.
  UINT8                     Use;                            ///< The enumeration value from HYGON_MEMORY_ARRAY_USE.
  UINT8                     MemoryErrorCorrection;          ///< The enumeration value from HYGON_MEMORY_ERROR_CORRECTION.
  UINT32                    MaximumCapacity;
  UINT16                    MemoryErrorInformationHandle;
  UINT16                    NumberOfMemoryDevices;
  //
  // Add for smbios 2.7
  //
  UINT64                    ExtendedMaximumCapacity;
} HYGON_SMBIOS_TABLE_TYPE16;

///
/// Memory Device - Form Factor.
///
typedef enum {
  HygonMemoryFormFactorOther           = 0x01,
  HygonMemoryFormFactorUnknown         = 0x02,
  HygonMemoryFormFactorSimm            = 0x03,
  HygonMemoryFormFactorSip             = 0x04,
  HygonMemoryFormFactorChip            = 0x05,
  HygonMemoryFormFactorDip             = 0x06,
  HygonMemoryFormFactorZip             = 0x07,
  HygonMemoryFormFactorProprietaryCard = 0x08,
  HygonMemoryFormFactorDimm            = 0x09,
  HygonMemoryFormFactorTsop            = 0x0A,
  HygonMemoryFormFactorRowOfChips      = 0x0B,
  HygonMemoryFormFactorRimm            = 0x0C,
  HygonMemoryFormFactorSodimm          = 0x0D,
  HygonMemoryFormFactorSrimm           = 0x0E,
  HygonMemoryFormFactorFbDimm          = 0x0F
} HYGON_SMEMORY_FORM_FACTOR;

///
/// Memory Device - Type
///
typedef enum {
  HygonMemoryTypeOther                          = 0x01,
  HygonMemoryTypeUnknown                        = 0x02,
  HygonMemoryTypeDram                           = 0x03,
  HygonMemoryTypeEdram                          = 0x04,
  HygonMemoryTypeVram                           = 0x05,
  HygonMemoryTypeSram                           = 0x06,
  HygonMemoryTypeRam                            = 0x07,
  HygonMemoryTypeRom                            = 0x08,
  HygonMemoryTypeFlash                          = 0x09,
  HygonMemoryTypeEeprom                         = 0x0A,
  HygonMemoryTypeFeprom                         = 0x0B,
  HygonMemoryTypeEprom                          = 0x0C,
  HygonMemoryTypeCdram                          = 0x0D,
  HygonMemoryType3Dram                          = 0x0E,
  HygonMemoryTypeSdram                          = 0x0F,
  HygonMemoryTypeSgram                          = 0x10,
  HygonMemoryTypeRdram                          = 0x11,
  HygonMemoryTypeDdr                            = 0x12,
  HygonMemoryTypeDdr2                           = 0x13,
  HygonMemoryTypeDdr2FbDimm                     = 0x14,
  HygonMemoryTypeDdr3                           = 0x18,
  HygonMemoryTypeFbd2                           = 0x19,
  HygonMemoryTypeDdr4                           = 0x1A,
  HygonMemoryTypeLpddr                          = 0x1B,
  HygonMemoryTypeLpddr2                         = 0x1C,
  HygonMemoryTypeLpddr3                         = 0x1D,
  HygonMemoryTypeLpddr4                         = 0x1E,
  HygonMemoryTypeLogicalNonVolatileDevice       = 0x1F, 
  HygonMemoryTypeHBM                            = 0x20, 
  HygonMemoryTypeHBM2                           = 0x21,
  HygonMemoryTypeDdr5                           = 0x22,
  HygonMemoryTypeLpddr5                         = 0x23,
} HYGON_MEMORY_DEVICE_TYPE;

typedef struct {
  UINT16    Reserved     : 1;
  UINT16    Other        : 1;
  UINT16    Unknown      : 1;
  UINT16    FastPaged    : 1;
  UINT16    StaticColumn : 1;
  UINT16    PseudoStatic : 1;
  UINT16    Rambus       : 1;
  UINT16    Synchronous  : 1;
  UINT16    Cmos         : 1;
  UINT16    Edo          : 1;
  UINT16    WindowDram   : 1;
  UINT16    CacheDram    : 1;
  UINT16    Nonvolatile  : 1;
  UINT16    Registered   : 1;
  UINT16    Unbuffered   : 1;
  UINT16    LrDimm       : 1;
} HYGON_MEMORY_DEVICE_TYPE_DETAIL;

///
/// Memory Device (Type 17).
///
/// This structure describes a single memory device that is part of
/// a larger Physical Memory Array (Type 16).
/// Note:  If a system includes memory-device sockets, the SMBIOS implementation
/// includes a Memory Device structure instance for each slot, whether or not the
/// socket is currently populated.
///
typedef struct {
  HYGON_SMBIOS_STRUCTURE             Hdr;
  UINT16                             MemoryArrayHandle;
  UINT16                             MemoryErrorInformationHandle;
  UINT16                             TotalWidth;
  UINT16                             DataWidth;
  UINT16                             Size;
  UINT8                              FormFactor;                ///< The enumeration value from HYGON_MEMORY_FORM_FACTOR.
  UINT8                              DeviceSet;
  HYGON_SMBIOS_TABLE_STRING          DeviceLocator;
  HYGON_SMBIOS_TABLE_STRING          BankLocator;
  UINT8                              MemoryType;                ///< The enumeration value from HYGON_MEMORY_DEVICE_TYPE.
  HYGON_MEMORY_DEVICE_TYPE_DETAIL    TypeDetail;
  UINT16                             Speed;
  HYGON_SMBIOS_TABLE_STRING          Manufacturer;
  HYGON_SMBIOS_TABLE_STRING          SerialNumber;
  HYGON_SMBIOS_TABLE_STRING          AssetTag;
  HYGON_SMBIOS_TABLE_STRING          PartNumber;
  //
  // Add for smbios 2.6
  //
  UINT8                              Attributes;
  //
  // Add for smbios 2.7
  //
  UINT32                             ExtendedSize;
  UINT16                             ConfiguredMemoryClockSpeed;
  //
  // Add for smbios 2.8.0
  //
  UINT16                             MinimumVoltage;
  UINT16                             MaximumVoltage;
  UINT16                             ConfiguredVoltage;
  //
  // Add for smbios 3.2
  //
  UINT8                              MemoryTechnology; ///< Memory technology type for this memory device
  UINT16                             MemoryOperatingModeCapability; ///< The operating modes supported by this memory device
  HYGON_SMBIOS_TABLE_STRING          FirmwareVersion;      ///< String number for the firmware version of this memory device
  UINT16                             ModuleManufacturerId; ///< The two-byte module manufacturer ID found in the SPD of this memory device; LSB first.
  UINT16                             ModuleProductId;      ///< The two-byte module product ID found in the SPD of this memory device; LSB first
  UINT16                             MemorySubsystemControllerManufacturerId; // < The two-byte memory subsystem controller manufacturer ID found in the SPD of this memory device; LSB first
  UINT16                             MemorySubsystemControllerProductId; // < The two-byte memory subsystem controller product ID found in the SPD of this memory device; LSB first
  UINT64                             NonvolatileSize;               ///< Size of the Non-volatile portion of the memory device in Bytes, if any.
  UINT64                             VolatileSize;                  ///< Size of the Volatile portion of the memory device in Bytes, if any.
  UINT64                             CacheSize;                     ///< Size of the Cache portion of the memory device in Bytes, if any.
  UINT64                             LogicalSize;                   ///< Size of the Logical memory device in Bytes.
  UINT32                             ExtendedSpeed;                 ///< Extended speed of the memory device. Identifies the maximum capable speed of the device
  UINT32                             ExtendedConfiguredMemorySpeed; ///< Extended configured memory speed of the memory device. identifies the configured speed of the memory device
} HYGON_SMBIOS_TABLE_TYPE17_32;       // byo230906 -




// byo230831 + >>
typedef struct {
  HYGON_SMBIOS_STRUCTURE        Hdr;
  UINT16                        MemoryArrayHandle;
  UINT16                        MemoryErrorInformationHandle;
  UINT16                        TotalWidth;
  UINT16                        DataWidth;
  UINT16                        Size;
  UINT8                         FormFactor;                     ///< The enumeration value from AMD_MEMORY_FORM_FACTOR.
  UINT8                         DeviceSet;
  HYGON_SMBIOS_TABLE_STRING     DeviceLocator;
  HYGON_SMBIOS_TABLE_STRING     BankLocator;
  UINT8                         MemoryType;                     ///< The enumeration value from AMD_MEMORY_DEVICE_TYPE.
  HYGON_MEMORY_DEVICE_TYPE_DETAIL TypeDetail;
  UINT16                        Speed;
  HYGON_SMBIOS_TABLE_STRING     Manufacturer;
  HYGON_SMBIOS_TABLE_STRING     SerialNumber;
  HYGON_SMBIOS_TABLE_STRING     AssetTag;
  HYGON_SMBIOS_TABLE_STRING     PartNumber;
  //
  // Add for smbios 2.6
  //  
  UINT8                     Attributes;
  //
  // Add for smbios 2.7
  //
  UINT32                    ExtendedSize;
  UINT16                    ConfiguredMemoryClockSpeed;
  //
  // Add for smbios 2.8.0
  //
  UINT16                    MinimumVoltage;
  UINT16                    MaximumVoltage;
  UINT16                    ConfiguredVoltage;

} HYGON_SMBIOS_TABLE_TYPE17_31;


typedef struct {
  HYGON_SMBIOS_STRUCTURE          Hdr;
  UINT16                        MemoryArrayHandle;
  UINT16                        MemoryErrorInformationHandle;
  UINT16                        TotalWidth;
  UINT16                        DataWidth;
  UINT16                        Size;
  UINT8                         FormFactor;                     ///< The enumeration value from HYGON_MEMORY_FORM_FACTOR.
  UINT8                         DeviceSet;
  HYGON_SMBIOS_TABLE_STRING       DeviceLocator;
  HYGON_SMBIOS_TABLE_STRING       BankLocator;
  UINT8                         MemoryType;                     ///< The enumeration value from HYGON_MEMORY_DEVICE_TYPE.
  HYGON_MEMORY_DEVICE_TYPE_DETAIL TypeDetail;
  UINT16                        Speed;
  HYGON_SMBIOS_TABLE_STRING       Manufacturer;
  HYGON_SMBIOS_TABLE_STRING       SerialNumber;
  HYGON_SMBIOS_TABLE_STRING       AssetTag;
  HYGON_SMBIOS_TABLE_STRING       PartNumber;
  //
  // Add for smbios 2.6
  //  
  UINT8                     Attributes;
  //
  // Add for smbios 2.7
  //
  UINT32                    ExtendedSize;
  UINT16                    ConfiguredMemoryClockSpeed;
  //
  // Add for smbios 2.8.0
  //
  UINT16                    MinimumVoltage;
  UINT16                    MaximumVoltage;
  UINT16                    ConfiguredVoltage;
  //
  // Add for smbios 3.2
  //
  UINT8                     MemoryTechnology;       ///< Memory technology type for this memory device
  UINT16                    MemoryOperatingModeCapability; ///< The operating modes supported by this memory device
  HYGON_SMBIOS_TABLE_STRING   FirmwareVersion;        ///< String number for the firmware version of this memory device
  UINT16                    ModuleManufacturerId;   ///< The two-byte module manufacturer ID found in the SPD of this memory device; LSB first.
  UINT16                    ModuleProductId;        ///< The two-byte module product ID found in the SPD of this memory device; LSB first
  UINT16                    MemorySubsystemControllerManufacturerId; //< The two-byte memory subsystem controller manufacturer ID found in the SPD of this memory device; LSB first
  UINT16                    MemorySubsystemControllerProductId; //< The two-byte memory subsystem controller product ID found in the SPD of this memory device; LSB first
  UINT64                    NonvolatileSize;        ///< Size of the Non-volatile portion of the memory device in Bytes, if any.
  UINT64                    VolatileSize;           ///< Size of the Volatile portion of the memory device in Bytes, if any.
  UINT64                    CacheSize;              ///< Size of the Cache portion of the memory device in Bytes, if any. 
  UINT64                    LogicalSize;            ///< Size of the Logical memory device in Bytes.
  //
  // Add for smbios 3.3.0
  //
  UINT32                                    ExtendedSpeed;                  // Speed == 0xFFFF
  UINT32                                    ExtendedConfiguredMemorySpeed;  // ConfiguredMemoryClockSpeed == 0xFFFF
} HYGON_SMBIOS_TABLE_TYPE17;

// byo230831 + <<




///
/// 32-bit Memory Error Information - Error Type.
///
typedef enum {
  HygonMemoryErrorOther            = 0x01,
  HygonMemoryErrorUnknown          = 0x02,
  HygonMemoryErrorOk               = 0x03,
  HygonMemoryErrorBadRead          = 0x04,
  HygonMemoryErrorParity           = 0x05,
  HygonMemoryErrorSigleBit         = 0x06,
  HygonMemoryErrorDoubleBit        = 0x07,
  HygonMemoryErrorMultiBit         = 0x08,
  HygonMemoryErrorNibble           = 0x09,
  HygonMemoryErrorChecksum         = 0x0A,
  HygonMemoryErrorCrc              = 0x0B,
  HygonMemoryErrorCorrectSingleBit = 0x0C,
  HygonMemoryErrorCorrected        = 0x0D,
  HygonMemoryErrorUnCorrectable    = 0x0E
} HYGON_MEMORY_ERROR_TYPE;

///
/// 32-bit Memory Error Information - Error Granularity.
///
typedef enum {
  HygonMemoryGranularityOther             = 0x01,
  HygonMemoryGranularityOtherUnknown      = 0x02,
  HygonMemoryGranularityDeviceLevel       = 0x03,
  HygonMemoryGranularityMemPartitionLevel = 0x04
} HYGON_MEMORY_ERROR_GRANULARITY;

///
/// 32-bit Memory Error Information - Error Operation.
///
typedef enum {
  HygonMemoryErrorOperationOther        = 0x01,
  HygonMemoryErrorOperationUnknown      = 0x02,
  HygonMemoryErrorOperationRead         = 0x03,
  HygonMemoryErrorOperationWrite        = 0x04,
  HygonMemoryErrorOperationPartialWrite = 0x05
} HYGON_MEMORY_ERROR_OPERATION;

///
/// 32-bit Memory Error Information (Type 18).
///
/// This structure identifies the specifics of an error that might be detected
/// within a Physical Memory Array.
///
typedef struct {
  HYGON_SMBIOS_STRUCTURE    Hdr;
  UINT8                     ErrorType;                  ///< The enumeration value from HYGON_MEMORY_ERROR_TYPE.
  UINT8                     ErrorGranularity;           ///< The enumeration value from HYGON_MEMORY_ERROR_GRANULARITY.
  UINT8                     ErrorOperation;             ///< The enumeration value from HYGON_MEMORY_ERROR_OPERATION.
  UINT32                    VendorSyndrome;
  UINT32                    MemoryArrayErrorAddress;
  UINT32                    DeviceErrorAddress;
  UINT32                    ErrorResolution;
} HYGON_SMBIOS_TABLE_TYPE18;

///
/// Memory Array Mapped Address (Type 19).
///
/// This structure provides the address mapping for a Physical Memory Array.
/// One structure is present for each contiguous address range described.
///
typedef struct {
  HYGON_SMBIOS_STRUCTURE    Hdr;
  UINT32                    StartingAddress;
  UINT32                    EndingAddress;
  UINT16                    MemoryArrayHandle;
  UINT8                     PartitionWidth;
  //
  // Add for smbios 2.7
  //
  UINT64                    ExtendedStartingAddress;
  UINT64                    ExtendedEndingAddress;
} HYGON_SMBIOS_TABLE_TYPE19;

///
/// Memory Device Mapped Address (Type 20).
///
/// This structure maps memory address space usually to a device-level granularity.
/// One structure is present for each contiguous address range described.
///
typedef struct {
  HYGON_SMBIOS_STRUCTURE    Hdr;
  UINT32                    StartingAddress;
  UINT32                    EndingAddress;
  UINT16                    MemoryDeviceHandle;
  UINT16                    MemoryArrayMappedAddressHandle;
  UINT8                     PartitionRowPosition;
  UINT8                     InterleavePosition;
  UINT8                     InterleavedDataDepth;
  //
  // Add for smbios 2.7
  //
  UINT64                    ExtendedStartingAddress;
  UINT64                    ExtendedEndingAddress;
} HYGON_SMBIOS_TABLE_TYPE20;

#pragma pack (pop)
#endif // _HYGON_SMBIOS_H_
