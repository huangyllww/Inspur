/* $NoKeywords:$ */

/**
 * @file
 *
 * HPCB DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HPCB
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
#include <PiDxe.h>
#include <Guid/EventGroup.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include "HGPI.h"
#include <HPCB.h>
#include "HpcbCommon.h"
#include "HpcbSatDxe.h"
#include "../Tokens/Tokens.h"
#include <Library/HpcbSatLib.h>
#include <Protocol/HygonHpcbProtocol.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Filecode.h>

#define FILECODE  PSP_HPCB_HPCBSATDXE_HPCBSATDXE_FILECODE

extern EFI_GUID  gHygonHpcbDxeServiceProtocolGuid;

STATIC HYGON_HPCB_SERVICE_PROTOCOL  mHpcbDxeServiceProtocol = {
  mHpcbSetActiveInstance,
  mHpcbFlushData,
  mHpcbUpdateCbsData,
  mHpcbGetConfigParameter,
  mHpcbSetConfigParameter,
  mHpcbGetCbsParameter,
  mHpcbSetCbsParameter,
  mHpcbGetDramPostPkgRepairEntry,
  mHpcbClearDramPostPkgRepairEntry,
  mHpcbAddDramPostPkgRepairEntry,
  mHpcbRemoveDramPostPkgRepairEntry,           // byo230831 -
  mHpcbReload                                  // byo230831 +
};

UINT8  mActiveHpcbInstance = 0;

/*++

Routine Description:

  This function sets the active HPCB instance
  TODO: Not needed for now. Reserved for future compatibility

--*/
VOID
mHpcbSetActiveInstance (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT8             Instance
  )
{
  mActiveHpcbInstance = Instance;
}

BOOLEAN
mHpcbCheckInstance (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This
  )
{
  if (mActiveHpcbInstance == 0xFF) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Error: Uninitialized active HPCB instance\n");
    return FALSE;
  }

  return TRUE;
}

/*++

Routine Description:

  This function flush the HPCB data back to SPI ROM

--*/
EFI_STATUS
mHpcbFlushData (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This
  )
{
  // Write back Shadow Copy in DRAM
  IDS_HDT_CONSOLE_PSP_TRACE ("Write back HPCB to SPI\n");
  return HygonPspWriteBackHpcbShadowCopy ();
}

/*++

Routine Description:

  This function udpates the HPCB data for CBS backend

--*/
EFI_STATUS
mHpcbUpdateCbsData (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT8             *HpcbTypeData,
  IN       UINT32            SizeInByte,
  IN       BOOLEAN           Internal
  )
{
  EFI_STATUS   Status;
  UINT16       Type;
  HPCB_HEADER  *HpcbShadowCopy;
  UINT8        *NewHpcb;
  UINT32       NewHpcbSize;

  if (!mHpcbCheckInstance (This)) {
    return EFI_INVALID_PARAMETER;
  }

  Type = Internal ? HPCB_TYPE_CBS_DEBUG_PARAMETERS : HPCB_TYPE_CBS_COMMON_PARAMETERS;
  HpcbShadowCopy = GetHpcbShadowCopy ();
  NewHpcbSize    = HpcbShadowCopy->SizeOfHpcb + SizeInByte + sizeof (HPCB_PARAM_ATTRIBUTE) * 2 + ALIGN_SIZE_OF_TYPE;
  NewHpcb = NULL;
  NewHpcb = AllocateZeroPool (NewHpcbSize);

  Status = HpcbReplaceType (HPCB_GROUP_CBS, Type, 0, HpcbTypeData, SizeInByte, NewHpcb);
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return Status;
  }

  return EFI_SUCCESS;
}

/**
 *
 * Routine Description:
 *
 *   This function gets the HPCB config parameter
 *
 *  @param[in]     TokenId      HPCB token ID defined in HPCB_COMMON_CONFIG_ID of HpcbCommon.h
 *  @param[in,out] SizeInByte   Set to non-NULL to get Size in bytes of the HPCB token
 *  @param[in,out] Value        Value of HPCB token ID
 *
 *  @retval EFI_SUCCESS    Get HPCB value successfully
 *  @retval EFI_NOT_FOUND  Can't find the according HPCB token
 *  @retval EFI_INVALID_PARAMETER  Invalid parameters
 *
 **/
EFI_STATUS
mHpcbGetConfigParameter (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  )
{
  if (!mHpcbCheckInstance (This)) {
    return EFI_INVALID_PARAMETER;
  }

  return HygonPspHpcbGetCfgParameter (mTranslateConfigTokenId (TokenId), SizeInByte, Value);
}

/**
 *
 * Routine Description:
 *
 *   This function sets the HPCB config parameter
 *
 *  @param[in]     TokenId      HPCB token ID defined in HPCB_COMMON_CONFIG_ID of HpcbCommon.h
 *  @param[in,out] SizeInByte   Set to non-NULL to get Size in bytes of the HPCB token
 *  @param[in,out] Value        Value of HPCB token ID
 *
 *  @retval EFI_SUCCESS    Set HPCB value successfully
 *  @retval EFI_NOT_FOUND  Can't find the according HPCB token
 *  @retval EFI_INVALID_PARAMETER  Invalid parameters
 *
 **/
EFI_STATUS
mHpcbSetConfigParameter (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  )
{
  if (!mHpcbCheckInstance (This)) {
    return EFI_INVALID_PARAMETER;
  }

  return HygonPspHpcbSetCfgParameter (mTranslateConfigTokenId (TokenId), SizeInByte, Value);
}

/**
 *
 * Routine Description:
 *
 *   This function gets the HPCB CBS parameter
 *
 *  @param[in]     TokenId      HPCB token ID defined in HPCB_COMMON_CBS_EXT_ID of HpcbCommon.h
 *  @param[in,out] SizeInByte   Set to non-NULL to get Size in bytes of the HPCB token
 *  @param[in,out] Value        Value of HPCB token ID
 *
 *  @retval EFI_SUCCESS    Get HPCB value successfully
 *  @retval EFI_NOT_FOUND  Can't find the according HPCB token
 *  @retval EFI_INVALID_PARAMETER  Invalid parameters
 *
 **/
EFI_STATUS
mHpcbGetCbsParameter (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  )
{
  if (!mHpcbCheckInstance (This)) {
    return EFI_INVALID_PARAMETER;
  }

  return HygonPspHpcbGetCbsCmnParameter (mTranslateCbsExtTokenId (TokenId), SizeInByte, Value);
}

/**
 *
 * Routine Description:
 *
 *   This function sets the HPCB CBS parameter
 *
 *  @param[in]     TokenId      HPCB token ID defined in HPCB_COMMON_CBS_EXT_ID of HpcbCommon.h
 *  @param[in,out] SizeInByte   Set to non-NULL to get Size in bytes of the HPCB token
 *  @param[in,out] Value        Value of HPCB token ID
 *
 *  @retval EFI_SUCCESS    Set HPCB value successfully
 *  @retval EFI_NOT_FOUND  Can't find the according HPCB token
 *  @retval EFI_INVALID_PARAMETER  Invalid parameters
 *
 **/
EFI_STATUS
mHpcbSetCbsParameter (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  )
{
  if (!mHpcbCheckInstance (This)) {
    return EFI_INVALID_PARAMETER;
  }

  return HygonPspHpcbSetCbsCmnParameter (mTranslateCbsExtTokenId (TokenId), SizeInByte, Value);
}

/**
 *
 * Routine Description:
 *
 *   This function retrieves the DRAM POST Package repair entries
 *
 *  @param[in, out] Entry      pointer to the DRAM POST Package repair entries to be retrieved
 *
 *  @param[in, out] NumOfEntries  Number of DRAM POST Package repair entries to be returned
 *
 *  @retval EFI_SUCCESS    DRAM POST Package repair entries cleared successfully
 *
 **/
EFI_STATUS
mHpcbGetDramPostPkgRepairEntry (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN OUT   DRRP_REPAIR_ENTRY *Entry,
  IN OUT   UINT32            *NumOfEntries
  )
{
  EFI_STATUS                Status;
  UINT8                     i;
  HPCB_DPPRCL_REPAIR_ENTRY  *RepairEntrySat;
  UINT32                    SizeInByte;

  if (!mHpcbCheckInstance (This)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = HpcbGetDramPostPkgRepairEntries (&RepairEntrySat, &SizeInByte);
  if (EFI_SUCCESS == Status) {
    if (NumOfEntries != NULL) {
      *NumOfEntries = SizeInByte / sizeof (HPCB_DPPRCL_REPAIR_ENTRY);
    } else {
      return EFI_INVALID_PARAMETER;
    }

    if (Entry != NULL) {
      for (i = 0; i < *NumOfEntries; i++, Entry++, RepairEntrySat++) {
        Entry->Valid = RepairEntrySat->Valid;
        Entry->Bank  = RepairEntrySat->Bank;
        Entry->RankMultiplier = RepairEntrySat->RankMultiplier;
        Entry->Device = RepairEntrySat->Device;
        Entry->TargetDevice = RepairEntrySat->TargetDevice;
        Entry->ChipSelect   = RepairEntrySat->ChipSelect;
        Entry->Column     = RepairEntrySat->Column;
        Entry->RepairType = RepairEntrySat->RepairType;
        Entry->Row     = RepairEntrySat->Row;
        Entry->Socket  = RepairEntrySat->Socket;
        Entry->Channel = RepairEntrySat->Channel;
      }
    }
  }

  return Status;
}

/**
 *
 * Routine Description:
 *
 *   This function clears the DRAM POST Package repair entries
 *
 *  @retval EFI_SUCCESS    DRAM POST Package repair entries cleared successfully
 *
 **/
EFI_STATUS
mHpcbClearDramPostPkgRepairEntry (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This
  )
{
  if (!mHpcbCheckInstance (This)) {
    return EFI_INVALID_PARAMETER;
  }

  return HpcbClearDramPostPkgRepairEntry ();
}

/**
 *
 * Routine Description:
 *
 *   This function adds a DRAM POST Package repair entry
 *
 *  @param[in]     Entry      DRAM POST Package repair entry to be added
 *
 *  @retval EFI_SUCCESS    DRAM POST Package repair entry added successfully
 *
 **/
EFI_STATUS
mHpcbAddDramPostPkgRepairEntry (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       DRRP_REPAIR_ENTRY *Entry
  )
{
  HPCB_DPPRCL_REPAIR_ENTRY  RepairEntrySat;

  if (!mHpcbCheckInstance (This)) {
    return EFI_INVALID_PARAMETER;
  }

  RepairEntrySat.Valid = 1;
  RepairEntrySat.Bank  = Entry->Bank;
  RepairEntrySat.RankMultiplier = Entry->RankMultiplier;
  RepairEntrySat.Device = Entry->Device;
  RepairEntrySat.TargetDevice = Entry->TargetDevice;
  RepairEntrySat.ChipSelect   = Entry->ChipSelect;
  RepairEntrySat.Column     = Entry->Column;
  RepairEntrySat.RepairType = Entry->RepairType;
  RepairEntrySat.Row     = Entry->Row;
  RepairEntrySat.Socket  = Entry->Socket;
  RepairEntrySat.Channel = Entry->Channel;

  return HpcbAddDramPostPkgRepairEntry (&RepairEntrySat);
}

/**
 *
 * Routine Description:
 *
 *  This function removes a DRAM POST Package repair entry
 *
 *  @param[in]     Entry      DRAM POST Package repair entry to be removed
 *
 *  @retval EFI_SUCCESS    DRAM POST Package repair entry removed successfully
 *
 **/
EFI_STATUS
mHpcbRemoveDramPostPkgRepairEntry (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       DRRP_REPAIR_ENTRY *Entry
  )
{
  HPCB_DPPRCL_REPAIR_ENTRY  RepairEntrySat;

  if (!mHpcbCheckInstance (This)) {
    return EFI_INVALID_PARAMETER;
  }

  RepairEntrySat.Valid = 1;
  RepairEntrySat.Bank  = Entry->Bank;
  RepairEntrySat.RankMultiplier = Entry->RankMultiplier;
  RepairEntrySat.Device = Entry->Device;
  RepairEntrySat.TargetDevice = Entry->TargetDevice;
  RepairEntrySat.ChipSelect   = Entry->ChipSelect;
  RepairEntrySat.Column     = Entry->Column;
  RepairEntrySat.RepairType = Entry->RepairType;
  RepairEntrySat.Row     = Entry->Row;
  RepairEntrySat.Socket  = Entry->Socket;
  RepairEntrySat.Channel = Entry->Channel;

  return HpcbRemoveDramPostPkgRepairEntry (&RepairEntrySat);
}

// byo230831 + >>
EFI_STATUS
mHpcbReload (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This
  )
{
  return EFI_UNSUPPORTED;
}
// byo230831 + <<

EFI_STATUS
HygonHpcbSatDxeDriverEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;

  HGPI_TESTPOINT (TpHpcbDxeEntry, NULL);
  IDS_HDT_CONSOLE_PSP_TRACE ("HPCB DXE Driver Entry\n");
  Handle = NULL;
  // Install HPCB service Protocol
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gHygonHpcbDxeServiceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mHpcbDxeServiceProtocol
                  );
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("HPCB DXE Driver Exit\n");
  HGPI_TESTPOINT (TpHpcbDxeExit, NULL);
  return EFI_SUCCESS;
}
