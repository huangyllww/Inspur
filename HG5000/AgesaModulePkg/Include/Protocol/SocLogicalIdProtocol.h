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
#ifndef _SOC_LOGICAL_ID_PROTOCOL_H_
#define _SOC_LOGICAL_ID_PROTOCOL_H_

//----------------------------------------------------
//
//  SOC Logical ID Service Functions
//
//-------------------------------------------------------

#include <SocLogicalId.h>

///
/// Forward declaration for the AMD_SOC_LOGICAL_ID_PROTOCOL.
///
typedef struct _AMD_SOC_LOGICAL_ID_PROTOCOL AMD_SOC_LOGICAL_ID_PROTOCOL;

/*---------------------------------------------------------------------------------------*/
/**
 * This service retrieves the logical family and revision for the executing core.
 *
 * @param[in]  This                           A pointer to the AMD_SOC_LOGICAL_ID_PROTOCOL instance.
 * @param[out] LogicalId                      Pointer to the logical ID for the executing core.
 *
 * @retval EFI_SUCCESS                        The executing core's logical ID was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER              All output parameter pointers are NULL.
 * @retval EFI_DEVICE_ERROR                   Unknown processor family found.
 *
 **/
typedef
EFI_STATUS
(EFIAPI *AMD_SOC_LOGICAL_ID_GET_LOGICAL_ID_ON_CURRENT_CORE) (
     IN    AMD_SOC_LOGICAL_ID_PROTOCOL                     *This,
     OUT   SOC_LOGICAL_ID                                  *LogicalId
  );

/*---------------------------------------------------------------------------------------*/
/**
 * This service retrieves the logical family and revision for a particular processor.
 *
 * If no processor is installed in Socket, then EFI_INVALID_PARAMETER is returned.
 *
 * @param[in]  This                           A pointer to the AMD_SOC_LOGICAL_ID_PROTOCOL instance.
 * @param[in]  Socket                         Zero-based socket number to check.
 * @param[out] LogicalId                      Pointer to the logical ID for the given socket.
 *
 * @retval EFI_SUCCESS                        The given socket's logical ID was retrieved successfully.
 * @retval EFI_INVALID_PARAMETER              Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER              All output parameter pointers are NULL.
 * @retval EFI_DEVICE_ERROR                   Unknown processor family found.
 *
 **/
typedef
EFI_STATUS
(EFIAPI *AMD_SOC_LOGICAL_ID_GET_LOGICAL_ID_ON_GIVEN_SOCKET) (
     IN    AMD_SOC_LOGICAL_ID_PROTOCOL                     *This,
     IN    UINTN                                           Socket,
     OUT   SOC_LOGICAL_ID                                  *LogicalId
  );

///
/// When installed, the SoC Logical ID protocol produces a collection
/// of services that provide the logical ID translation for installed
/// processors.
///
typedef struct _AMD_SOC_LOGICAL_ID_PROTOCOL {
  UINTN                                               Revision;                   ///< Revision Number
  AMD_SOC_LOGICAL_ID_GET_LOGICAL_ID_ON_CURRENT_CORE   GetLogicalIdOnCurrentCore;  ///< Get logical revision of the executing core
  AMD_SOC_LOGICAL_ID_GET_LOGICAL_ID_ON_GIVEN_SOCKET   GetLogicalIdOnGivenSocket;  ///< Get logical revision of the given socket
} AMD_SOC_LOGICAL_ID_PROTOCOL;

extern EFI_GUID gAmdSocLogicalIdProtocolGuid;

// Current protocol revision
#define SOC_LOGICAL_ID_PROTOCOL_REVISION   0x00

#endif
