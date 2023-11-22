/* $NoKeywords:$ */

/**
 * @file
 *
 * Generate SMBIOS type 4 7 16 17 19 20
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Universal
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

#ifndef _UNIVERSAL_ACPI_HYGON_ACPI_DXE_H_
#pragma pack (push, 1)

#define ACPI_TABLE_MAX_LENGTH  0x100000ul          ///< Reserve 1M for ACPI table

// Defines for Common ACPI
// -----------------------------
#define     SCOPE_OPCODE         0x10
#define     NAME_OPCODE          0x08
#define     METHOD_OPCODE        0x14
#define     PACKAGE_OPCODE       0x12
#define     BUFFER_OPCODE        0x11
#define     BYTE_PREFIX_OPCODE   0x0A
#define     WORD_PREFIX_OPCODE   0x0B
#define     DWORD_PREFIX_OPCODE  0x0C
#define     RETURN_OPCODE        0xA4
#define     ACPI_BUFFER          0x080A0B11ul

/// Format for Header
typedef struct {
  UINT8     Sign[4];                    ///< Signature
  UINT32    TableLength;                ///< Table Length
  UINT8     Revision;                   ///< Revision
  UINT8     Checksum;                   ///< Checksum
  UINT8     OemId[6];                   ///< OEM ID
  UINT8     OemTableId[8];              ///< OEM Tabled ID
  UINT32    OemRev;                     ///< OEM Revision
  UINT8     CreatorId[4];               ///< Creator ID
  UINT32    CreatorRev;                 ///< Creator Revision
} ACPI_TABLE_HEADER;

VOID
ChecksumAcpiTable (
  IN OUT   ACPI_TABLE_HEADER *Table,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  );

#pragma pack (pop)
#endif // _UNIVERSAL_ACPI_HYGON_ACPI_DXE_H_
