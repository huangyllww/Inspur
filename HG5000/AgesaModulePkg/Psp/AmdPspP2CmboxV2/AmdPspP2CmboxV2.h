/* $NoKeywords:$ */
/**
 * @file
 *
 * PspP2Cmbox Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AMD PSP UEFI Driver
 * @e sub-project   PspP2Cmbox Driver
 * @e \$Revision$   @e \$Date$
 *
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

#ifndef _PSP_P2C_MBOX_H_
#define _PSP_P2C_MBOX_H_

#include <Base.h>
#include <Pi/PiFirmwareVolume.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/FchSmmSwDispatch2.h>
#include <Protocol/FchSmmMiscDispatch.h>

#include <Library/AmdPspFlashAccLib.h>

#include "AGESA.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Library/AmdPspMboxLibV2.h>
#include <Protocol/PspMboxSmmBufferAddressProtocol.h>

#define EFI_LBA_LIST_TERMINATOR   0xFFFFFFFFFFFFFFFFULL


EFI_STATUS
EFIAPI
ProcessPspCommand (
  IN UINT64                 Cmd,
  IN MBOX_DEFAULT_BUFFER   *MboxBuffer
);
EFI_STATUS
PspFlashAccSmmCommunicateHandler (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context,
  IN OUT VOID    *CommBuffer,
  IN OUT UINTN   *CommBufferSize
  );
/// SMI command target NV region
typedef enum {
  SMI_TARGET_NVRAM = 0,           ///< PSP NVRAM
  SMI_TARGET_TOKEN_UNLOCK = 1,            ///< PSP token unlock data
  SMI_TARGET_VM_GUARD = 2,            ///< VM Guard data region
  SMI_TARGET_COREMCE_DATA = 3,   ///< core machine check exception
  SMI_TARGET_NO_USE = 0xFE,           ///< Not used
  SMI_TARGET_END = 0xFF,            ///< End flag
} SMI_TARGET_ID;

/// Directory type
typedef enum {
  DIR_TYPE_PSP = 0,               ///< PSP Directory
  DIR_TYPE_BIOS = 1,                ///< BIOS Directory
  DIR_TYPE_END = 0xFF,                ///< End flag
} DIR_TYPE;

///< Structure describe PSP NV Region
typedef struct _PSP_NV_DESCRIPTOR {
  UINT32           TargetID;            ///< Target NV ID, defined in SMI_TARGET_ID
  DIR_TYPE         Directory;           ///< Directory Type, defined in DIR_TYPE
  UINT32           DirEntryID;            ///< Entry ID
  BOOLEAN          Detected;            ///< Flag to present if entry detected
  UINT64           NvRegionBase;            ///< Region base
  UINT32           NvRegionSize;            ///< Region Size
} PSP_NV_DESCRIPTOR;
#endif  //_PSP_P2C_MBOX_H_
