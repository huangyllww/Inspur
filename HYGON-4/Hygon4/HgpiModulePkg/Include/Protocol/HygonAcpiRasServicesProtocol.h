/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON RAS Services Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Library
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

#ifndef _HYGON_ACPI_RAS_SERVICES_PROTOCOL_H_
#define _HYGON_ACPI_RAS_SERVICES_PROTOCOL_H_
#pragma pack (push, 1)

/*----------------------------------------------------------------------------------------
 *                    T Y P E D E F S     A N D     S T R U C T U R E S
 *                            System Resource Affinity Table
 *----------------------------------------------------------------------------------------
 */
/// Initial data of HYGON_MC_BANK
typedef struct _HYGON_MC_BANK_INIT_DATA {
  UINT64    CtrlInitData;         ///< Initial data of CtrlInitDataLSD
  UINT32    CtrlRegMSRAddr;       ///< Initial data of CtrlRegMSRAddr
  UINT32    StatRegMSRAddr;       ///< Initial data of StatRegMSRAddr
  UINT32    AddrRegMSRAddr;       ///< Initial data of AddrRegMSRAddr
  UINT32    MiscRegMSRAddr;       ///< Initial data of MiscRegMSRAddr
} HYGON_MC_BANK_INIT_DATA;

/// Initial data of Machine Check
typedef struct _HYGON_MC_INIT_DATA {
  UINT64                     GlobCapInitData;  ///< Check global capability register
  UINT64                     GlobCtrlInitData; ///< Check global control register
  UINT8                      ClrStatusOnInit;  ///< Indicates if the status information in this machine check
                                               ///< bank is to be cleared during system initialization
  UINT8                      StatusDataFormat; ///< Indicates the format of the data in the status register
  UINT8                      ConfWriteEn;      ///< This field indicates whether configuration parameters may be
                                               ///< modified by the OS. If the bit for the associated parameter is
                                               ///< set, the parameter is writable by the OS.
  UINT8                      McBankNum;        ///< Number of MCA Bank
  HYGON_MC_BANK_INIT_DATA    *McBankInitData;  ///< Pointer to Initial data of Mc Bank
} HYGON_MC_INIT_DATA;

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */
///
/// Forward declaration for the HYGON_FABRIC_ACPI_SRAT_SERVICES_PROTOCOL.
///
typedef struct _HYGON_CCX_ACPI_RAS_SERVICES_PROTOCOL HYGON_CCX_ACPI_RAS_SERVICES_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *HYGON_RAS_SERVICES_GET_MC_BANK_INFO)(
  IN       HYGON_CCX_ACPI_RAS_SERVICES_PROTOCOL         *This,
  OUT   HYGON_MC_INIT_DATA                          **McBankInfo
  );

///
/// When installed, the SRAT Services Protocol produces a collection of
/// services that return various information to generate SRAT
///
struct _HYGON_CCX_ACPI_RAS_SERVICES_PROTOCOL {
  UINTN                                  Revision;      ///< Revision Number
  HYGON_RAS_SERVICES_GET_MC_BANK_INFO    GetMcBankInfo; ///< Get Machine Check bank infomation
};

extern EFI_GUID  gHygonCcxAcpiRasServicesProtocolGuid;

#pragma pack (pop)
#endif // _HYGON_ACPI_RAS_SERVICES_PROTOCOL_H_
