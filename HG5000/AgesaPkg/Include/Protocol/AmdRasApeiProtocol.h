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

#ifndef _AMD_RAS_APEI_PROTOCOL_H_
#define _AMD_RAS_APEI_PROTOCOL_H_

#include "AmdRas.h"

extern EFI_GUID gAmdRasApeiProtocolGuid;

// current PPI revision
#define AMD_RAS_APEI_REV  0x01

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
typedef struct _AMD_RAS_APEI_PROTOCOL AMD_RAS_APEI_PROTOCOL;

// Protocol prototype
typedef
EFI_STATUS
(EFIAPI *AMD_ADD_BOOT_ERROR_RECORD_ENTRY) (
  IN    UINT8   *ErrorRecord,
  IN    UINT32  RecordLen,
  IN    UINT8   ErrorType,
  IN    UINT8   SeverityType
);

typedef
EFI_STATUS
(EFIAPI *ADD_HEST_ERROR_SOURCE_ENTRY) (
  IN    UINT8   *pErrorRecord,
  IN    UINT32  RecordLen
);

typedef
EFI_STATUS
(EFIAPI *MCA_ERROR_ADDR_TRANSLATE) (
  IN       NORMALIZED_ADDRESS *NormalizedAddress,
  OUT      UINT64             *SystemMemoryAddress,
  OUT      DIMM_INFO          *DimmInfo
);

typedef
EFI_STATUS
(EFIAPI *TRANSLATE_SYSADDR_TO_CS) (
  IN       UINT64             *SystemMemoryAddress,
  OUT      NORMALIZED_ADDRESS *NormalizedAddress,
  OUT      DIMM_INFO          *DimmInfo
);

typedef
EFI_STATUS
(EFIAPI *SEARCH_MCA_ERROR) (
  IN OUT   RAS_MCA_ERROR_INFO* RasMcaErrorInfo
);

typedef
EFI_STATUS
(EFIAPI *ADD_MULTIPLE_HEST_ERROR_SOURCE_ENTRY) (
  IN    UINT8   *pErrorRecord,
  IN    UINT32  RecordLen,
  IN    UINT32  RecordCount
);

/// RAS APEI Protocol Structure
struct _AMD_RAS_APEI_PROTOCOL {
  MCA_ERROR_ADDR_TRANSLATE         			McaErrorAddrTranslate;    /// MCA_ADDR address Translate
  TRANSLATE_SYSADDR_TO_CS          			TranslateSysAddrToCS;     /// System Address Translate
  AMD_ADD_BOOT_ERROR_RECORD_ENTRY  			AddBootErrorRecordEntry;  /// Boot error record to be added
  ADD_HEST_ERROR_SOURCE_ENTRY               AddHestErrorSourceEntry;  /// HEST error source to be added
  SEARCH_MCA_ERROR                          SearchMcaError;           /// Search Mca Error
  ADD_MULTIPLE_HEST_ERROR_SOURCE_ENTRY      AddMultipleHestErrorSourceEntry;  /// HEST multiple error source to be added
};

#endif //_AMD_RAS_APEI_PROTOCOL_H_
