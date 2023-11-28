/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Memory API, and related functions.
 *
 * Contains code that initializes memory
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Mem
 * @e \$Revision:  $   @e \$Date:  $
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
#ifndef _AMD_MEM_PPR_PROTOCOL_H_
#define _AMD_MEM_PPR_PROTOCOL_H_

#include <PiDxe.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Guid/HobList.h>

#define AMD_MEM_MAX_SOCKETS_SUPPORTED   2  ///< Max number of sockets in system
#define AMD_MEM_MAX_DIES_PER_SOCKET     4   ///< Max dies per socket
#define AMD_MEM_MAX_CHANNELS_PER_SOCKET 8   ///< Max Channels per sockets
#define AMD_MEM_MAX_CHANNELS_PER_DIE    2   ///< Max channels per die
#define AMD_MEM_MAX_DIMMS_PER_CHANNEL   2   ///< Max dimms per die

typedef struct _AMD_DIMM_INFO {
    UINT8  SocketId;
    UINT8  DieId;
    UINT8  ChannelId;
    UINT8  Chipselect;
} AMD_DIMM_INFO;

typedef struct _AMD_PPR_INFO {
    BOOLEAN IsValidRecord;
    BOOLEAN DpprSupported;
    BOOLEAN SpprSupported;
    UINT8   DeviceWidth;
    UINT32  SerialNumber;
} AMD_PPR_INFO;

//
// AMD_MEM_DIMM_SPD_DATA_STRUCT
//
typedef struct _AMD_MEM_PPR_DATA_TABLE {
  AMD_PPR_INFO    DimmSpdInfo[AMD_MEM_MAX_SOCKETS_SUPPORTED * AMD_MEM_MAX_CHANNELS_PER_SOCKET * AMD_MEM_MAX_DIMMS_PER_CHANNEL];
} AMD_MEM_PPR_DATA_TABLE;

//
// Forward declaration for the AMD_ACPI_CPU_SSDT_SERVICES_PROTOCOL.
//
typedef struct _AMD_POST_PACKAGE_REPAIR_INFO_PROTOCOL AMD_POST_PACKAGE_REPAIR_INFO_PROTOCOL;


typedef
EFI_STATUS
(EFIAPI *AMD_GET_PPR_INFO) (
  IN       AMD_POST_PACKAGE_REPAIR_INFO_PROTOCOL    *This,
  IN       AMD_DIMM_INFO                            *AmdDimmInfo,
     OUT   AMD_PPR_INFO                             *PprInfo
  );

//
// AMD Post Package Repair Protocol Info
//
struct _AMD_POST_PACKAGE_REPAIR_INFO_PROTOCOL {
  AMD_MEM_PPR_DATA_TABLE          AmdPprArray;
  AMD_GET_PPR_INFO                AmdGetPprInfo;
};

extern EFI_GUID  gAmdPostPackageRepairInfoProtocolGuid;

#endif