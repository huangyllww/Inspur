/* $NoKeywords:$ */
/**
 * @file
 *
 * CCX base Services Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
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
 *
 ***************************************************************************/
#ifndef _AMD_ACPI_CPU_SSDT_SERVICES_PROTOCOL_H_
#define _AMD_ACPI_CPU_SSDT_SERVICES_PROTOCOL_H_
#pragma pack (push, 1)

#define MAX_CORE_NUMBER_PER_SOCKET 64

/// P-state structure for each state
typedef struct {
  IN OUT   UINT32               PStateEnable;                ///< Pstate enable
  IN OUT   UINT32               CoreFreq;                    ///< MHz
  IN OUT   UINT32               Power;                       ///< milliWatts
  IN OUT   UINT32               SwPstateNumber;              ///< Software P-state number
} S_PSTATE_VALUES;

/// P-state structure for socket
typedef struct {
  IN OUT   UINT8                SocketNumber;                ///< Physical socket number of this socket
  IN OUT   UINT8                TotalLogicalCores;           ///< Logical core number in this socket
  IN OUT   BOOLEAN              CreateAcpiTables;            ///< Create table flag
  IN OUT   UINT8                LocalApicId[MAX_CORE_NUMBER_PER_SOCKET]; ///< Local Apic Id
  IN OUT   UINT8                PStateMaxValue;              ///< Max p-state number in this core
  IN OUT   UINT32               TransitionLatency;           ///< Transition latency
  IN OUT   BOOLEAN              IsPsdDependent;              ///< Dependent or Independent PSD
  IN OUT   S_PSTATE_VALUES      PStateStruct[1];             ///< P state structure
} AMD_PSTATE_SOCKET_INFO;

/// P-state structure for whole system
typedef struct {
  IN OUT   UINT8                TotalSocketInSystem;         ///< Total node number in system
  IN OUT   UINT32               SizeOfBytes;                 ///< Structure size
  IN OUT   AMD_PSTATE_SOCKET_INFO PStateSocketStruct[1];     ///< P state structure for socket
} AMD_PSTATE_SYS_INFO;


/// C-state structure
typedef struct {
  IN OUT   BOOLEAN              IsCstateEnabled;         ///< Is Cstate enabled
  IN OUT   UINT32               IoCstateAddr;            ///< Io Cstate address
  IN OUT   BOOLEAN              IsCsdGenerated;          ///< Is _CSD needed
  IN OUT   BOOLEAN              IsMonitorMwaitSupported; ///< Should C1 FFH be declared
} AMD_CSTATE_INFO;

///
/// Forward declaration for the AMD_ACPI_CPU_SSDT_SERVICES_PROTOCOL.
///
typedef struct _AMD_ACPI_CPU_SSDT_SERVICES_PROTOCOL AMD_ACPI_CPU_SSDT_SERVICES_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *AMD_SSDT_SERVICES_GET_PSTATE_INFO) (
  IN       AMD_ACPI_CPU_SSDT_SERVICES_PROTOCOL    *This,
     OUT   AMD_PSTATE_SYS_INFO                   **PstateSysInfoPtr
  );

typedef
EFI_STATUS
(EFIAPI *AMD_SSDT_SERVICES_GET_CSTATE_INFO) (
  IN       AMD_ACPI_CPU_SSDT_SERVICES_PROTOCOL    *This,
     OUT   AMD_CSTATE_INFO                       **CstateInfo
  );

typedef
UINT32
(EFIAPI *AMD_SSDT_SERVICES_GET_PSD_DOMAIN_FOR_INDEP) (
  IN       AMD_ACPI_CPU_SSDT_SERVICES_PROTOCOL    *This,
  IN       UINT8                                   LocalApicId
  );

///
/// ACPI Processor SSDT services protocol
///
struct _AMD_ACPI_CPU_SSDT_SERVICES_PROTOCOL {
  UINTN                                            Revision;               ///< Revision Number
  AMD_SSDT_SERVICES_GET_PSTATE_INFO                GetPstateInfo;          ///< Get Pstate information for whole system
  AMD_SSDT_SERVICES_GET_CSTATE_INFO                GetCstateInfo;          ///< Get Cstate information
  AMD_SSDT_SERVICES_GET_PSD_DOMAIN_FOR_INDEP       GetPsdDomain;           ///< Get PDS domain for independency
};


extern EFI_GUID gAmdAcpiCpuSsdtServicesProtocolGuid;

#pragma pack (pop)
#endif // _AMD_ACPI_CPU_SSDT_SERVICES_PROTOCOL_H_
