/* $NoKeywords:$ */
/**
 * @file
 *
 * APCB DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  APCB
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
#include <PiDxe.h>
#include <Guid/EventGroup.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include "AGESA.h"
#include "Firmwares/ZP/APCB.h"
#include "ApcbCommon.h"
#include "ApcbZpDxe.h"
#include "../Tokens/Tokens.h"
#include <Library/ApcbZpLib.h>
#include <Protocol/AmdApcbProtocol.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Filecode.h>

#define FILECODE PSP_APCB_APCBZPDXE_APCBZPDXE_FILECODE

extern EFI_GUID gAmdApcbDxeServiceProtocolGuid;

STATIC AMD_APCB_SERVICE_PROTOCOL mApcbDxeServiceProtocol = {
  mApcbSetActiveInstance,
  mApcbFlushData,
  mApcbUpdateCbsData,
  mApcbGetConfigParameter,
  mApcbSetConfigParameter,
  mApcbGetCbsParameter,
  mApcbSetCbsParameter,
  mApcbGetDramPostPkgRepairEntry,
  mApcbClearDramPostPkgRepairEntry,
  mApcbAddDramPostPkgRepairEntry,
  mApcbRemoveDramPostPkgRepairEntry
};

UINT8                     mActiveApcbInstance = 0;


/*++

Routine Description:

  This function sets the active APCB instance
  TODO: Not needed for now. Reserved for future compatibility

--*/
VOID
mApcbSetActiveInstance (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT8             Instance
  )
{
  mActiveApcbInstance = Instance;
}

BOOLEAN
mApcbCheckInstance (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This
  )
{
  if (mActiveApcbInstance == 0xFF) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Error: Uninitialized active APCB instance\n");
    return FALSE;
  }

  return TRUE;
}

/*++

Routine Description:

  This function flush the APCB data back to SPI ROM

--*/
EFI_STATUS
mApcbFlushData (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This
  )
{
  // Write back Shadow Copy in DRAM
  IDS_HDT_CONSOLE_PSP_TRACE ("Write back APCB to SPI\n");
  return AmdPspWriteBackApcbShadowCopy ();
}

/*++

Routine Description:

  This function udpates the APCB data for CBS backend

--*/
EFI_STATUS
mApcbUpdateCbsData (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT8             *ApcbTypeData,
  IN       UINT32            SizeInByte,
  IN       BOOLEAN           Internal
  )
{
  EFI_STATUS                Status;
  UINT16                    Type;
  APCB_HEADER               *ApcbShadowCopy;
  UINT8                     *NewApcb;
  UINT32                    NewApcbSize;

  if (!mApcbCheckInstance (This)) {
    return EFI_INVALID_PARAMETER;
  }

  Type = Internal ? APCB_TYPE_CBS_DEBUG_PARAMETERS : APCB_TYPE_CBS_COMMON_PARAMETERS;
  ApcbShadowCopy = GetApcbShadowCopy ();
  NewApcbSize = ApcbShadowCopy->SizeOfApcb + SizeInByte + sizeof (APCB_PARAM_ATTRIBUTE) * 2 + ALIGN_SIZE_OF_TYPE;
  NewApcb = NULL;
  NewApcb = AllocateZeroPool (NewApcbSize);

  Status = ApcbReplaceType (APCB_GROUP_CBS, Type, 0, ApcbTypeData, SizeInByte, NewApcb);
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
 *   This function gets the APCB config parameter
 *
 *  @param[in]     TokenId      APCB token ID defined in APCB_COMMON_CONFIG_ID of ApcbCommon.h
 *  @param[in,out] SizeInByte   Set to non-NULL to get Size in bytes of the APCB token
 *  @param[in,out] Value        Value of APCB token ID
 *
 *  @retval EFI_SUCCESS    Get APCB value successfully
 *  @retval EFI_NOT_FOUND  Can't find the according APCB token
 *  @retval EFI_INVALID_PARAMETER  Invalid parameters
 *
 **/
EFI_STATUS
mApcbGetConfigParameter (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  )
{
  if (!mApcbCheckInstance (This)) {
    return EFI_INVALID_PARAMETER;
  }

  return AmdPspApcbGetCfgParameter (mTranslateConfigTokenId(TokenId), SizeInByte, Value);
}

/**
 *
 * Routine Description:
 *
 *   This function sets the APCB config parameter
 *
 *  @param[in]     TokenId      APCB token ID defined in APCB_COMMON_CONFIG_ID of ApcbCommon.h
 *  @param[in,out] SizeInByte   Set to non-NULL to get Size in bytes of the APCB token
 *  @param[in,out] Value        Value of APCB token ID
 *
 *  @retval EFI_SUCCESS    Set APCB value successfully
 *  @retval EFI_NOT_FOUND  Can't find the according APCB token
 *  @retval EFI_INVALID_PARAMETER  Invalid parameters
 *
 **/
EFI_STATUS
mApcbSetConfigParameter (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  )
{
  if (!mApcbCheckInstance (This)) {
    return EFI_INVALID_PARAMETER;
  }

  return AmdPspApcbSetCfgParameter (mTranslateConfigTokenId(TokenId), SizeInByte, Value);
}

/**
 *
 * Routine Description:
 *
 *   This function gets the APCB CBS parameter
 *
 *  @param[in]     TokenId      APCB token ID defined in APCB_COMMON_CBS_EXT_ID of ApcbCommon.h
 *  @param[in,out] SizeInByte   Set to non-NULL to get Size in bytes of the APCB token
 *  @param[in,out] Value        Value of APCB token ID
 *
 *  @retval EFI_SUCCESS    Get APCB value successfully
 *  @retval EFI_NOT_FOUND  Can't find the according APCB token
 *  @retval EFI_INVALID_PARAMETER  Invalid parameters
 *
 **/
EFI_STATUS
mApcbGetCbsParameter (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  )
{
  if (!mApcbCheckInstance (This)) {
    return EFI_INVALID_PARAMETER;
  }

  return AmdPspApcbGetCbsCmnParameter (mTranslateCbsExtTokenId(TokenId), SizeInByte, Value);
}

/**
 *
 * Routine Description:
 *
 *   This function sets the APCB CBS parameter
 *
 *  @param[in]     TokenId      APCB token ID defined in APCB_COMMON_CBS_EXT_ID of ApcbCommon.h
 *  @param[in,out] SizeInByte   Set to non-NULL to get Size in bytes of the APCB token
 *  @param[in,out] Value        Value of APCB token ID
 *
 *  @retval EFI_SUCCESS    Set APCB value successfully
 *  @retval EFI_NOT_FOUND  Can't find the according APCB token
 *  @retval EFI_INVALID_PARAMETER  Invalid parameters
 *
 **/
EFI_STATUS
mApcbSetCbsParameter (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  )
{
  if (!mApcbCheckInstance (This)) {
    return EFI_INVALID_PARAMETER;
  }

  return AmdPspApcbSetCbsCmnParameter (mTranslateCbsExtTokenId(TokenId), SizeInByte, Value);
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
mApcbGetDramPostPkgRepairEntry (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN OUT   DRRP_REPAIR_ENTRY *Entry,
  IN OUT   UINT32            *NumOfEntries
  )
{
  EFI_STATUS                Status;
  UINT8                     i;
  APCB_DPPRCL_REPAIR_ENTRY  *RepairEntryZp;
  UINT32                    SizeInByte;

  if (!mApcbCheckInstance (This)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = ApcbGetDramPostPkgRepairEntries (&RepairEntryZp, &SizeInByte);
  if (EFI_SUCCESS == Status) {
    if (NumOfEntries != NULL) {
      *NumOfEntries = SizeInByte / sizeof (APCB_DPPRCL_REPAIR_ENTRY);
    } else {
      return EFI_INVALID_PARAMETER;
    }

    if (Entry != NULL) {
      for (i = 0; i < *NumOfEntries; i ++, Entry ++, RepairEntryZp ++) {
        Entry->Valid             = RepairEntryZp->Valid;
        Entry->Bank              = RepairEntryZp->Bank;
        Entry->RankMultiplier    = RepairEntryZp->RankMultiplier;
        Entry->Device            = RepairEntryZp->Device;
        Entry->TargetDevice      = RepairEntryZp->TargetDevice;
        Entry->ChipSelect        = RepairEntryZp->ChipSelect;
        Entry->Column            = RepairEntryZp->Column;
        Entry->RepairType        = RepairEntryZp->RepairType;
        Entry->Row               = RepairEntryZp->Row;
        Entry->Socket            = RepairEntryZp->Socket;
        Entry->Channel           = RepairEntryZp->Channel;
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
mApcbClearDramPostPkgRepairEntry (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This
  )
{
  if (!mApcbCheckInstance (This)) {
    return EFI_INVALID_PARAMETER;
  }

  return ApcbClearDramPostPkgRepairEntry ();
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
mApcbAddDramPostPkgRepairEntry (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       DRRP_REPAIR_ENTRY *Entry
  )
{
  APCB_DPPRCL_REPAIR_ENTRY  RepairEntryZp;

  if (!mApcbCheckInstance (This)) {
    return EFI_INVALID_PARAMETER;
  }

  RepairEntryZp.Valid         = 1;
  RepairEntryZp.Bank          = Entry->Bank;
  RepairEntryZp.RankMultiplier= Entry->RankMultiplier;
  RepairEntryZp.Device        = Entry->Device;
  RepairEntryZp.TargetDevice  = Entry->TargetDevice;
  RepairEntryZp.ChipSelect    = Entry->ChipSelect;
  RepairEntryZp.Column        = Entry->Column;
  RepairEntryZp.RepairType    = Entry->RepairType;
  RepairEntryZp.Row           = Entry->Row;
  RepairEntryZp.Socket        = Entry->Socket;
  RepairEntryZp.Channel       = Entry->Channel;

  return ApcbAddDramPostPkgRepairEntry (&RepairEntryZp);
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
mApcbRemoveDramPostPkgRepairEntry (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       DRRP_REPAIR_ENTRY *Entry
  )
{
  APCB_DPPRCL_REPAIR_ENTRY  RepairEntryZp;

  if (!mApcbCheckInstance (This)) {
    return EFI_INVALID_PARAMETER;
  }

  RepairEntryZp.Valid         = 1;
  RepairEntryZp.Bank          = Entry->Bank;
  RepairEntryZp.RankMultiplier= Entry->RankMultiplier;
  RepairEntryZp.Device        = Entry->Device;
  RepairEntryZp.TargetDevice  = Entry->TargetDevice;
  RepairEntryZp.ChipSelect    = Entry->ChipSelect;
  RepairEntryZp.Column        = Entry->Column;
  RepairEntryZp.RepairType    = Entry->RepairType;
  RepairEntryZp.Row           = Entry->Row;
  RepairEntryZp.Socket        = Entry->Socket;
  RepairEntryZp.Channel       = Entry->Channel;

  return ApcbRemoveDramPostPkgRepairEntry (&RepairEntryZp);
}


EFI_STATUS
AmdApcbZpDxeDriverEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                  Status;
  EFI_HANDLE                  Handle;

  AGESA_TESTPOINT (TpApcbDxeEntry, NULL);
  IDS_HDT_CONSOLE_PSP_TRACE ("APCB DXE Driver Entry\n");
  Handle = NULL;
  //Install APCB service Protocol
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdApcbDxeServiceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mApcbDxeServiceProtocol
                  );
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("APCB DXE Driver Exit\n");
  AGESA_TESTPOINT (TpApcbDxeExit, NULL);
  return EFI_SUCCESS;
}
