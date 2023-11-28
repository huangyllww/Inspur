/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP common service PROTOCOL prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
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
#ifndef _AMD_PSP_COMMONSERVICE_PROTOCOL_H_
#define _AMD_PSP_COMMONSERVICE_PROTOCOL_H_

#include <AmdPspDirectory.h>

///Function used to get Ftpm control area address
typedef
BOOLEAN
(*FP_GET_FTPM_CONTROLAREA) (
  IN OUT   VOID *FtpmControlArea
  );

///Function used to turn on/restore PSP MMIO decode
typedef
VOID
(*FP_SWITCH_PSP_MMIO_DECODE) (
  IN       BOOLEAN SwitchFlag,
  IN OUT   UINT32 *RegisterCopy
  );

///Function used to check PSP Device present by checking fuse receipt copy
typedef
BOOLEAN
(*FP_CHECK_PSP_DEVICE_PRESENT) (
  VOID
  );

///Function used to check Ftpm support
typedef
EFI_STATUS
(*FP_CHECK_FTPM_CAPS) (
  IN OUT  UINT32 *Caps
  );

typedef
BOOLEAN
(*FP_PSPENTRYINFO) (
  IN      PSP_DIRECTORY_ENTRY_TYPE    EntryType,
  IN OUT  UINT64                      *EntryAddress,
  IN      UINT32                      *EntrySize
  );

typedef
BOOLEAN
(* FP_CONDITIONER) (
  IN       VOID        *Context
  );

typedef
BOOLEAN
(*FP_PSP_LIB_TIMEOUT) (
  IN       UINT64              uSec,
  IN       FP_CONDITIONER      Conditioner,
  IN       VOID                *Context
  );

typedef
VOID
(*FP_ACQUIRE_PSPSMIREGMUTEX) (
  VOID
  );

typedef
VOID
(*FP_RELEASE_PSPSMIREGMUTEX) (
  VOID
  );
//
// PROTOCOL prototype
//
// Defines AMD_PSP_COMMON_SERVICE_PROTOCOL, which public the common PSP service across all programs
//

/// Define PSP_FTPM_PROTOCOL
typedef struct _AMD_PSP_COMMON_SERVICE_PROTOCOL {
  FP_GET_FTPM_CONTROLAREA       GetFtpmControlArea;     ///< Get FTPM control Area
  FP_SWITCH_PSP_MMIO_DECODE     SwitchPspMmioDecode;     ///< Turn on/off Psp MMIO
  FP_CHECK_PSP_DEVICE_PRESENT   CheckPspDevicePresent;     ///< Check if PSP Device Present
  FP_CHECK_FTPM_CAPS            CheckFtpmCaps;     ///< Check if Ftpm supported
  FP_PSPENTRYINFO               PSPEntryInfo;           ///< Get specific PSP Entry information
  FP_PSP_LIB_TIMEOUT            PspLibTimeOut;     ///< Delay function
  FP_ACQUIRE_PSPSMIREGMUTEX     AcquirePspSmiRegMutex;  ///< Acquire Psp SmiReg Mutex
  FP_RELEASE_PSPSMIREGMUTEX     ReleasePspSmiRegMutex;  ///< Release Psp SmiReg Mutex
} AMD_PSP_COMMON_SERVICE_PROTOCOL;

extern EFI_GUID gAmdPspCommonServiceProtocolGuid;

#endif //_AMD_PSP_COMMONSERVICE_PROTOCOL_H_
