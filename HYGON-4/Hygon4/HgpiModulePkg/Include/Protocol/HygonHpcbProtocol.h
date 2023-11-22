/* $NoKeywords:$ */

/**
 * @file
 *
 * HPCB service PPI prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HPCB
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
 *
 ***************************************************************************/
#include "HpcbCommon.h"

#ifndef _HYGON_HPCB_SERVICE_PROTOCOL_H_
  #define _HYGON_HPCB_SERVICE_PROTOCOL_H_

  /*----------------------------------------------------------------------------------------
   *                    T Y P E D E F S     A N D     S T R U C T U R E S
   *                            COMPONENT Locality Distance Information Table
   *----------------------------------------------------------------------------------------
   */

  /*----------------------------------------------------------------------------------------
   *                 D E F I N I T I O N S     A N D     M A C R O S
   *----------------------------------------------------------------------------------------
   */

  ///
  /// Forward declaration for the HYGON_HPCB_SERVICE_PROTOCOL.
  ///
  typedef struct _HYGON_HPCB_SERVICE_PROTOCOL HYGON_HPCB_SERVICE_PROTOCOL;

  /// Function used to set active HPCB instance
  typedef
VOID
  (EFIAPI *FP_SET_ACTIVE_HPCB_INSTANCE)(
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT8             Instance
  );

  /// Function used to flush HPCB data
  typedef
EFI_STATUS
  (EFIAPI *FP_FLUSH_HPCB_DATA)(
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This
  );

  /// Function used to update CBS HPCB data
  typedef
EFI_STATUS
  (*FP_UPDATE_CBS_HPCB_DATA) (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT8             *HpcbTypeData,
  IN       UINT32            SizeInByte,
  IN       BOOLEAN           Internal
  );

  /// Function used to get the value of an HPCB config token
  typedef
EFI_STATUS
  (EFIAPI *FP_GET_CONFIG_PARAM)(
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  );

  /// Function used to set the value of an HPCB config token
  typedef
EFI_STATUS
  (EFIAPI *FP_SET_CONFIG_PARAM)(
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  );

  /// Function used to get the value of an HPCB CBS token
  typedef
EFI_STATUS
  (EFIAPI *FP_GET_CBS_PARAM)(
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  );

  /// Function used to set the value of an HPCB CBS token
  typedef
EFI_STATUS
  (EFIAPI *FP_SET_CBS_PARAM)(
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  );

  /// Function used to get the DRAM POST Package repair entries
  typedef
EFI_STATUS
  (EFIAPI *FP_GET_DRAM_POSTPKG_REPAIR_ENTRY)(
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN OUT   DRRP_REPAIR_ENTRY *Entry,
  IN OUT   UINT32            *NumOfEntries
  );

  /// Function used to clear the DRAM POST Package repair entries
  typedef
EFI_STATUS
  (EFIAPI *FP_CLEAR_DRAM_POSTPKG_REPAIR_ENTRY)(
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This
  );

  /// Function used to add a DRAM POST Package repair entry
  typedef
EFI_STATUS
  (EFIAPI *FP_ADD_DRAM_POSTPKG_REPAIR_ENTRY)(
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       DRRP_REPAIR_ENTRY *Entry
  );

  /// Function used to remove a DRAM POST Package repair entry
  typedef
EFI_STATUS
  (EFIAPI *FP_REMOVE_DRAM_POSTPKG_REPAIR_ENTRY)(
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       DRRP_REPAIR_ENTRY *Entry
  );

// byo230831 + >>
  typedef
EFI_STATUS
  (EFIAPI *FP_RELOAD_HPCB)(
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This
  );
// byo230831 + <<

  ///
  /// Protocol prototype
  ///
  /// Defines HYGON_HPCB_SERVICE_PPI, which publish the HPCB service across all programs
  ///
  struct _HYGON_HPCB_SERVICE_PROTOCOL {
    FP_SET_ACTIVE_HPCB_INSTANCE            HpcbSetActiveInstance;           ///< Set the active instance of HPCB
    FP_FLUSH_HPCB_DATA                     HpcbFlushData;                   ///< Flush HPCB data back to the SPI ROM
    FP_UPDATE_CBS_HPCB_DATA                HpcbUpdateCbsData;               ///< Update CBS HPCB data
    FP_GET_CONFIG_PARAM                    HpcbGetConfigParameter;          ///< Get an HPCB configuration parameter
    FP_SET_CONFIG_PARAM                    HpcbSetConfigParameter;          ///< Set an HPCB configuration parameter
    FP_GET_CBS_PARAM                       HpcbGetCbsParameter;             ///< Get an HPCB CBS parameter
    FP_SET_CBS_PARAM                       HpcbSetCbsParameter;             ///< Set an HPCB CBS parameter
    FP_GET_DRAM_POSTPKG_REPAIR_ENTRY       HpcbGetDramPostPkgRepairEntries; ///< Retrieve DRAM Post Package Repair Entries
    FP_CLEAR_DRAM_POSTPKG_REPAIR_ENTRY     HpcbClearDramPostPkgRepairEntry; ///< Clear DRAM Post Package Repair Entries
    FP_ADD_DRAM_POSTPKG_REPAIR_ENTRY       HpcbAddDramPostPkgRepairEntry;   ///< Add a DRAM Post Package Repair Entry
    FP_REMOVE_DRAM_POSTPKG_REPAIR_ENTRY    HpcbRemoveDramPostPkgRepairEntry; ///< Remove a DRAM Post Package Repair Entry
    FP_RELOAD_HPCB                         HpcbReload;                // byo230831 +
  };

  extern EFI_GUID  gHygonHpcbDxeServiceProtocolGuid;
  extern EFI_GUID  gHygonHpcbSmmServiceProtocolGuid;

#endif //_HYGON_HPCB_SERVICE_PROTOCOL_H_
