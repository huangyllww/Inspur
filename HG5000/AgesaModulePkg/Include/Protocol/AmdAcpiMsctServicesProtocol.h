/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD MSCT Services Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Protocol
 * @e \$Revision:  $   @e \$Date:  $
 */
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

#ifndef _AMD_ACPI_MSCT_SERVICES_PROTOCOL_H_
#define _AMD_ACPI_MSCT_SERVICES_PROTOCOL_H_

#pragma pack (push, 1)

/// Format for MSCT Header
typedef struct {
  UINT8  Signature[4];        ///< 'MSCT' Signature for the Maximum System Characteristics Table
  UINT32 Length;              ///< Length, in bytes, of the entire MSCT
  UINT8  Revision;            ///< 1
  UINT8  Checksum;            ///< Entire table must sum to zero
  UINT8  OemId[6];            ///< OEM ID
  UINT8  OemTableId[8];       ///< For the MSCT, the table ID is the manufacturer model ID
  UINT32 OemRevision;         ///< OEM revision of MSCT for supplied OEM Table ID
  UINT8  CreatorId[4];        ///< Vendor ID of utility that created the table
  UINT32 CreatorRev;          ///< Revision of utility that created the table
  UINT32 OffsetProxDomInfo;   ///< Offset in bytes to the Proximity Domain Information Structure table entry
  UINT32 MaxProxDom;          ///< Maximum number of Proximity Domains ever possible in the system
  UINT32 MaxClkDom;           ///< Maximum number of Clock Domains ever possible in the system
  UINT64 MaxPhysAddr;         ///< Maximum Physical Address ever possible in the system
} MSCT_HEADER;

/// Format for Maximum Proximity Domain Information Structure
typedef struct {
  UINT8  Revision;            ///< 1
  UINT8  Length;              ///< 22
  UINT32 ProximityLow;        ///< Starting proximity domain for the range that this structure is providing information
  UINT32 ProximityHigh;       ///< Ending proximity domain for the range that this structure is providing information
  UINT32 MaxProcCap;          ///< Maximum Procesor Capacity of each of the Proximity Domains specified in the range
  UINT64 MaxMemCap;           ///< Maximum Memory Capacity (in bytes) of the Proximity Domains specified in the range
} MSCT_PROX_DOMAIN_INFO_STRUCT;

/// Format for Maximum Proximity Domain Structure
typedef struct {
  UINT32        ProxDomain;   ///< Proximity Domain
  UINT32        MaxProcCap;   ///< Maximum (Actual) Processor Capacity
  UINT64        MaxMemCap;    ///< Maximum (Actual) Memory Capacity
} MSCT_PROX_DOMAIN_INFO;

/// Forward declaration for AMD_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL
typedef struct _AMD_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL AMD_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *AMD_MSCT_SERVICES_GET_MSCT_INFO) (
  IN       AMD_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL  *This,
     OUT   UINT32                                  *HighestProxDomain,
     OUT   UINT32                                  *MaxNumProxDomains,
     OUT   MSCT_PROX_DOMAIN_INFO                   **MsctDomainInfo
  );

/// When installed, the MSCT Services Protocol produces a collection of
/// services related to MSCT
struct _AMD_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL {
  UINTN                                                Revision;                    ///< Revision Number
  AMD_MSCT_SERVICES_GET_MSCT_INFO                      GetMsctInfo;                 ///< Returns information to populate MSCT maximum proximity structures
};

extern EFI_GUID gAmdFabricAcpiMsctServicesProtocolGuid;

#pragma pack (pop)
#endif // _AMD_ACPI_MSCT_SERVICES_PROTOCOL_H_