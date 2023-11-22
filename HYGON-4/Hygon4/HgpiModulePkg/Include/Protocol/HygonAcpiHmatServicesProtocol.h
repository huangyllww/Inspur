/* $NoKeywords:$ */
/**
 * @file
 *
 * HYGON HMAT Services Protocol prototype definition
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
 ******************************************************************************
 */
 
#ifndef _HYGON_ACPI_HMAT_SERVICES_PROTOCOL_H_
#define _HYGON_ACPI_HMAT_SERVICES_PROTOCOL_H_

///
/// Heterogeneous Memory Attribute Table
///
typedef struct {
  UINT32    Signature;
  UINT32    Length;
  UINT8     Revision;
  UINT8     Checksum;
  UINT8     OemId[6];
  UINT64    OemTableId;
  UINT32    OemRevision;
  UINT32    CreatorId;
  UINT32    CreatorRevision;
  UINT8     Reserved[4];
} HMAT_HEADER;

///
/// HMAT Structure Header
///
typedef struct {
  UINT16    Type;
  UINT8     Reserved[2];
  UINT32    Length;
} HMAT_STRUCTURE_HEADER;

///
/// Flags of Memory Proximity Domain Attributes Structure
///
typedef union {
  UINT16 Value;
  struct {
    UINT16 InitiatorProximityDomainValid :1;
    UINT16 Reserved                      :15;
  } Fields;
} HMAT_MEM_PROXIMITY_DOMAIN_ATTRIBUTES_FLAGS;

///
/// Memory Proximity Domain Attributes Structure
///
typedef struct {
  UINT16                                      Type;
  UINT8                                       Reserved[2];
  UINT32                                      Length;
  HMAT_MEM_PROXIMITY_DOMAIN_ATTRIBUTES_FLAGS  Flags;
  UINT8                                       Reserved1[2];
  UINT32                                      InitiatorProximityDomain;
  UINT32                                      MemoryProximityDomain;
  UINT8                                       Reserved2[20];
} HMAT_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES_STRUCTURE;

///
/// Flags of System Locality Latency and Bandwidth Information Structure
///
typedef union {
  UINT8 Value;
  struct {
    UINT8    MemoryHierarchy  :4;
    UINT8    AccessAttributes :2;
    UINT8    Reserved         :2;
  } Fields;
} HMAT_SYS_LOCALITY_LATENCY_BANDWIDTH_INFO_FLAGS;

///
/// System Locality Latency and Bandwidth Information Structure
///
typedef struct {
  UINT16                                          Type;
  UINT8                                           Reserved[2];
  UINT32                                          Length;
  HMAT_SYS_LOCALITY_LATENCY_BANDWIDTH_INFO_FLAGS  Flags;
  UINT8                                           DataType;
  UINT8                                           MinTransferSize;
  UINT8                                           Reserved1;
  UINT32                                          NumberOfInitiatorProximityDomains;
  UINT32                                          NumberOfTargetProximityDomains;
  UINT8                                           Reserved2[4];
  UINT64                                          EntryBaseUnit;
} HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_STRUCTURE;

///
/// Cache Attributes of Memory Side Cache Information Structure
///
typedef union {
  UINT32 Value;
  struct {
    UINT32   TotalCacheLevels    :4;
    UINT32   CacheLevel          :4;
    UINT32   CacheAssociativity  :4;
    UINT32   WritePolicy         :4;
    UINT32   CacheLineSize       :16;
  } Fields;
} HMAT_MEM_SIDE_CACHE_ATTRIBUTES;

///
/// Memory Side Cache Information Structure
///
typedef struct {
  UINT16                          Type;
  UINT8                           Reserved[2];
  UINT32                          Length;
  UINT32                          MemoryProximityDomain;
  UINT8                           Reserved1[4];
  UINT64                          MemorySideCacheSize;
  HMAT_MEM_SIDE_CACHE_ATTRIBUTES  CacheAttributes;
  UINT8                           Reserved2[2];
  UINT16                          NumberOfSmbiosHandles;
} HMAT_MEMORY_SIDE_CACHE_INFO_STRUCTURE;

/*----------------------------------------------------------------------------------------
 *                    T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define HMAT_REVISION                                                                 0x02

#define HMAT_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES_TYPE                                  0x00
#define HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_TYPE                          0x01
#define HMAT_MEMORY_SIDE_CACHE_INFO_TYPE                                              0x02

/// The revision number of HYGON_ACPI_HMAT_SERVICES_PROTOCOL.
#define HYGON_ACPI_HMAT_SERVICES_PROTOCOL_REVISION                                       1

#define HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_DATA_TYPE_ACCESS_LATENCY         0
#define HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_DATA_TYPE_READ_LATENCY           1
#define HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_DATA_TYPE_WRITE_LATENCY          2
#define HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_DATA_TYPE_ACCESS_BANDWIDTH       3
#define HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_DATA_TYPE_READ_BANDWIDTH         4
#define HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_DATA_TYPE_WRITE_BANDWIDTH        5

/// Forward declaration for the HYGON_ACPI_HMAT_SERVICES_PROTOCOL.
typedef struct _HYGON_ACPI_HMAT_SERVICES_PROTOCOL HYGON_ACPI_HMAT_SERVICES_PROTOCOL;

/**
 * @brief Create the HMAT structure.
 *
 * @param[in]      This                Pointer to the ACPI HMAT services protocol instance. @see HYGON_ACPI_HMAT_SERVICES_PROTOCOL
 * @param[in]      TablePtr            Pointer to the header of HMAT ACPI table structure.
 * @param[in, out] TableEnd            Point to the end of this table
 *
 * @return EFI_STATUS - status of the operation.
 */
typedef
EFI_STATUS
(EFIAPI *HYGON_HMAT_SERVICES_CREATE_STRUCTURE) (
  IN     HYGON_ACPI_HMAT_SERVICES_PROTOCOL      *This,
  IN     VOID                                   *TableHeaderPtr,
  IN OUT UINT8                                 **TableEnd
  );

/**
 * @brief Free the buffer of the added HMAT structures.
 *
 * @param[in]      This                Pointer to the ACPI HMAT services protocol instance. @see HYGON_ACPI_HMAT_SERVICES_PROTOCOL
 *
 * @return EFI_STATUS - status of the operation.
 */
typedef
EFI_STATUS
(EFIAPI *HYGON_HMAT_SERVICES_FREE_BUFFER) (
  IN     HYGON_ACPI_HMAT_SERVICES_PROTOCOL        *This
  );

/**
 * @brief Add the Memory Proximity Domain Attributes Structure
 *
 * @param[in]      This                        Pointer to the ACPI HMAT services protocol instance. @see HYGON_ACPI_HMAT_SERVICES_PROTOCOL
 * @param[in]      Flags                       Flags @see HMAT_MEM_PROXIMITY_DOMAIN_ATTRIBUTES_FLAGS
 * @param[in]      InitiatorProximityDomain    Proximity Domain for the Attached Initiator
 * @param[in]      MemoryProximityDomain       Proximity Domain for the Memory
 *
 * @return EFI_STATUS - status of the operation.
 */
typedef
EFI_STATUS
(EFIAPI *HYGON_HMAT_SERVICES_ADD_MEM_PROXIMITY_DOMAIN_ATTRIBUTES) (
  IN     HYGON_ACPI_HMAT_SERVICES_PROTOCOL              *This,
  IN     HMAT_MEM_PROXIMITY_DOMAIN_ATTRIBUTES_FLAGS      Flags,
  IN     UINT32                                          InitiatorProximityDomain,
  IN     UINT32                                          MemoryProximityDomain
  );

/**
 * @brief Add the System Locality Latency and Bandwidth Information Structure
 *
 * @param[in]      This                               Pointer to the ACPI HMAT services protocol instance. @see HYGON_ACPI_HMAT_SERVICES_PROTOCOL
 * @param[in]      Flags                              Flags @see HMAT_SYS_LOCALITY_LATENCY_BANDWIDTH_INFO_FLAGS
 * @param[in]      DataType                           Data Type
 * @param[in]      MinTransferSize                    MinTransferSize
 * @param[in]      NumberOfInitiatorProximityDomains  Number of Initiator Proximity Domains
 * @param[in]      NumberOfTargetProximityDomains     Number of Target Proximity Domains
 * @param[in]      EntryBaseUnit                      Entry Base Unit
 * @param[in]      InitiatorProximityDomains          Array of Initiator Proximity Domain List
 * @param[in]      TargetProximityDomains             Array of Target Proximity Domain List
 * @param[in]      LatencyBandwidthEntries            Two-dimension array of Latency / bandwidth values
 *
 * @return EFI_STATUS - status of the operation.
 */
typedef
EFI_STATUS
(EFIAPI *HYGON_HMAT_SERVICES_ADD_SYS_LOCALITY_LATENCY_BANDWIDTH_INFO) (
  IN     HYGON_ACPI_HMAT_SERVICES_PROTOCOL              *This,
  IN     HMAT_SYS_LOCALITY_LATENCY_BANDWIDTH_INFO_FLAGS  Flags,
  IN     UINT8                                           DataType,
  IN     UINT8                                           MinTransferSize,
  IN     UINT32                                          NumberOfInitiatorProximityDomains,
  IN     UINT32                                          NumberOfTargetProximityDomains,
  IN     UINT64                                          EntryBaseUnit,
  IN     UINT32                                         *InitiatorProximityDomains,
  IN     UINT32                                         *TargetProximityDomains,
  IN     UINT16                                         *LatencyBandwidthEntries
  );

/**
 * @brief Add the Memory Side Cache Information Structure
 *
 * @param[in]      This                   Pointer to the ACPI HMAT services protocol instance. @see HYGON_ACPI_HMAT_SERVICES_PROTOCOL
 * @param[in]      MemoryProximityDomain  Proximity Domain for the Memory
 * @param[in]      MemorySideCacheSize    Memory Side Cache Size
 * @param[in]      CacheAttributes        Cache Attributes. @see HMAT_MEM_SIDE_CACHE_ATTRITUBES
 * @param[in]      NumberOfSmbiosHandles  Number of SMBIOS handles
 * @param[in]      SmbiosHandles          SMBIOS handles
 *
 * @return EFI_STATUS - status of the operation.
 */
typedef
EFI_STATUS
(EFIAPI *HYGON_HMAT_SERVICES_ADD_MEM_SIDE_CACHE_INFO) (
  IN     HYGON_ACPI_HMAT_SERVICES_PROTOCOL         *This,
  IN     UINT32                                     MemoryProximityDomain,
  IN     UINT64                                     MemorySideCacheSize,
  IN     HMAT_MEM_SIDE_CACHE_ATTRIBUTES             CacheAttributes,
  IN     UINT16                                     NumberOfSmbiosHandles,
  IN     UINT16                                    *SmbiosHandles
  );

/// When installed, the HMAT Services Protocol produces a collection of services to generate HMAT.
struct _HYGON_ACPI_HMAT_SERVICES_PROTOCOL {
  UINTN                                                        Revision;                              ///< Revision Number.
  HYGON_HMAT_SERVICES_CREATE_STRUCTURE                         CreateStructure;                       ///< @see HYGON_HMAT_SERVICES_CREATE_STRUCTURE
  HYGON_HMAT_SERVICES_FREE_BUFFER                              FreeBuffer;                            ///< @see HYGON_HMAT_SERVICES_FREE_BUFFER
  HYGON_HMAT_SERVICES_ADD_MEM_PROXIMITY_DOMAIN_ATTRIBUTES      AddMemoryProximityDomainAttributes;    ///< @see HYGON_HMAT_SERVICES_ADD_MEM_PROXIMITY_DOMAIN_ATTRIBUTES
  HYGON_HMAT_SERVICES_ADD_SYS_LOCALITY_LATENCY_BANDWIDTH_INFO  AddSystemLocalityLatencyBandwidthInfo; ///< @see HYGON_HMAT_SERVICES_ADD_SYS_LOCALITY_LATENCY_BANDWIDTH_INFO
  HYGON_HMAT_SERVICES_ADD_MEM_SIDE_CACHE_INFO                  AddMemorySideCacheInfo;                ///< @see HYGON_HMAT_SERVICES_ADD_MEM_SIDE_CACHE_INFO
};

/// GUID for ACPI HMAT services protocol
extern EFI_GUID gHygonAcpiHmatServicesProtocolGuid;

#endif // _HYGON_ACPI_HMAT_SERVICES_PROTOCOL_H_
