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

#ifndef _AMD_RAS_SMM_PROTOCOL_H_
#define _AMD_RAS_SMM_PROTOCOL_H_

#include "AmdRas.h"

extern EFI_GUID gAmdRasSmmProtocolGuid;

// current PPI revision
#define AMD_RAS_SMM_REV  0x01

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
typedef struct _AMD_RAS_SMM_PROTOCOL AMD_RAS_SMM_PROTOCOL;

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
(EFIAPI *SET_SMI_TRIG_IOCYCLE) (
  IN       UINT64 SmiTrigIoCycleData
);


typedef
EFI_STATUS
(EFIAPI *GET_ALL_LOCAL_SMI_STATUS) (
  IN       LOCAL_SMI_STATUS* pLocalSmiStatusList
);

typedef
EFI_STATUS
(EFIAPI *SEARCH_MCA_ERROR) (
  IN OUT   RAS_MCA_ERROR_INFO* RasMcaErrorInfo
);

typedef
EFI_STATUS
(EFIAPI *RAS_SMM_EXIT_TYPE) (
  IN       UINTN ProcessorNumber,
  IN       UINTN SmiExitType
);

typedef
EFI_STATUS
(EFIAPI *GET_SMM_SAVE_STATE_BASE) (
  IN       UINTN ProcessorNumber,
  OUT      UINT64* SmmSaveStateBase
);

typedef
EFI_STATUS
(EFIAPI *SET_MCA_CLOAK_CFG) (
   IN       UINTN ProcessorNumber,
   IN       UINT64 CloakValue,
   IN       UINT64 UnCloakValue
);

typedef
EFI_STATUS
(EFIAPI *CLR_MCA_STATUS) (
  IN       UINTN             ProcessorNumber,
  IN       AMD_MCA_BANK_NUM  McaBankNumber,
  IN       BOOLEAN           IsWrMsr
);

typedef
EFI_STATUS
(EFIAPI *MAP_SYMBOL_TO_DRAM_DEVICE) (
  IN       AMD_RAS_SMM_PROTOCOL *This,
  IN       RAS_MCA_ERROR_INFO   *RasMcaErrorInfo,
  IN       NORMALIZED_ADDRESS   *NormalizedAddress,
  IN       UINT8                BankIndex,
  OUT      UINT32               *DeviceStart,
  OUT      UINT32               *DeviceEnd,
  OUT      UINT8                *DeviceType
);


/// Protocol Structure
struct _AMD_RAS_SMM_PROTOCOL {
  MCA_ERROR_ADDR_TRANSLATE         McaErrorAddrTranslate;    /// MCA_ADDR address Translate
  TRANSLATE_SYSADDR_TO_CS          TranslateSysAddrToCS;     /// System Address Translate
  SET_SMI_TRIG_IOCYCLE             SetSmiTrigIoCycle;        /// Set SmiTrigIoCycle
  GET_ALL_LOCAL_SMI_STATUS         GetAllLocalSmiStatus;     /// Get all Local Smi Status
  SEARCH_MCA_ERROR                 SearchMcaError;           /// Search Mca Error
  RAS_SMM_EXIT_TYPE                RasSmmExitType;           /// Set SMM exit interrupt type
  GET_SMM_SAVE_STATE_BASE          GetSmmSaveStateBase;      /// Get CPU SMM Save State Base Address
  SET_MCA_CLOAK_CFG                SetMcaCloakCfg;           ///
  CLR_MCA_STATUS                   ClrMcaStatus;             /// Clear MCA_STATUS MSR
  MAP_SYMBOL_TO_DRAM_DEVICE        MapSymbolToDramDevice;    /// Map the DRAM device to symbol
};

#endif //_AMD_RAS_SMM_PROTOCOL_H_
