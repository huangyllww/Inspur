/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Library
 *
 * Contains interface to the HYGON HGPI library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Lib
 *
 */
/*
 ******************************************************************************
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

/*****************************************************************************
 *
 * This software package can be used to enable the Overclocking of certain
 * HYGON processors and its use is subject to the terms and conditions of the
 * HYGON Overclocking Waiver. Enabling overclocking through use of the low-level
 * routines included in this package and operating an HYGON processor outside of
 * the applicable HYGON product specifications will void any HYGON warranty and can
 * result in damage to the processor or the system into which the processor has
 * been integrated. The user of this software assumes, and HYGON disclaims, all
 * risk, liability, costs and damages relating to or arising from the overclocking
 * of HYGON processors.
 *
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Uefi.h"
#include <Library/BaseLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/HpcbSatLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/HygonPspBaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HygonPspHpobLib.h>
#include <Filecode.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  LIBRARY_HPCBLIB_DRAMPOSTPACKAGEREPAIR_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          G L O B A L        V A L U E S
 *----------------------------------------------------------------------------------------
 */

extern UINT8    mHpcbInstance;
extern BOOLEAN  mHpcbAtRuntime;           // byo231107 -

EFI_STATUS
InternalHpcbLocateType (
  IN       UINT16   GroupId,
  IN       UINT16   TypeId,
  IN       UINT16   InstanceId,
  IN       UINT32   *Size,
  IN OUT   VOID     **TypeData
  );

/*++

Routine Description:

  This function retrieves the DRAM POST Package repair entries

--*/
EFI_STATUS
HpcbGetDramPostPkgRepairEntries (
  IN OUT   HPCB_DPPRCL_REPAIR_ENTRY **pHpcbDppRepairEntries,
  IN OUT   UINT32                   *SizeInByte
  )
{
  EFI_STATUS  Status;
  BOOLEAN     HpcbRecoveryFlag;

  Status = EFI_SUCCESS;

  // Exit service, if recovery flag set
  if (mHpcbAtRuntime == FALSE) {          // byo231107 -
    Status = HygonPspGetHpobHpcbRecoveryFlag (&HpcbRecoveryFlag);
    if (HpcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("HPCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  // Exit service, if recovery flag set
  if (CheckPspRecoveryFlag () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  Status = InternalHpcbLocateType (HPCB_GROUP_MEMORY, HPCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, 0, SizeInByte, (VOID *)pHpcbDppRepairEntries);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Cannot locate DDR4 Post Package Repair Entries\n");
    return Status;
  }

  return EFI_SUCCESS;
}

/*++

Routine Description:

  This function clears the DRAM POST Package repair entries

--*/
EFI_STATUS
HpcbClearDramPostPkgRepairEntry (
  VOID
  )
{
  EFI_STATUS                Status;
  BOOLEAN                   HpcbRecoveryFlag;
  HPCB_DPPRCL_REPAIR_ENTRY  HpcbDppRepairEntries[2];
  HPCB_HEADER               *HpcbShadowCopy;
  UINT32                    NewHpcbSize;
  UINT8                     *NewHpcb;

  Status  = EFI_SUCCESS;
  NewHpcb = NULL;

  // Exit service, if recovery flag set
  if (mHpcbAtRuntime == FALSE) {          // byo231107 -
    Status = HygonPspGetHpobHpcbRecoveryFlag (&HpcbRecoveryFlag);
    if (HpcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("HPCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  // Exit service, if recovery flag set
  if (CheckPspRecoveryFlag () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  SetMem (&HpcbDppRepairEntries[0], sizeof (HPCB_DPPRCL_REPAIR_ENTRY) * 2, 0);

  // Leave only one clean entry in the HPCB data of HPCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR
  HpcbShadowCopy = GetHpcbShadowCopy ();
  NewHpcbSize    = HpcbShadowCopy->SizeOfHpcb + sizeof (HPCB_DPPRCL_REPAIR_ENTRY) + ALIGN_SIZE_OF_TYPE;
  NewHpcb = AllocateZeroPool (NewHpcbSize);
  IDS_HDT_CONSOLE_PSP_TRACE ("ALLOCATE[%x:%x]\n", NewHpcb, NewHpcbSize);
  ASSERT (NewHpcb != NULL);
  if (NewHpcb == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  // Shadow Copy pointer will be updated in below routine
  Status = HpcbReplaceType (HPCB_GROUP_MEMORY, HPCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, 0, (UINT8 *)&HpcbDppRepairEntries[0], sizeof (HPCB_DPPRCL_REPAIR_ENTRY), NewHpcb);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Cannot replace HPCB type data\n");
    return Status;
  }

  return EFI_SUCCESS;
}

VOID
CopyDramPostPkgRepairEntry (
  IN       HPCB_DPPRCL_REPAIR_ENTRY *DestEntry,
  IN       HPCB_DPPRCL_REPAIR_ENTRY *SrcEntry
  )
{
  ASSERT (DestEntry != NULL);
  ASSERT (SrcEntry != NULL);

  CopyMem (
    (VOID *)DestEntry,
    (VOID *)SrcEntry,
    sizeof (HPCB_DPPRCL_REPAIR_ENTRY)
    );
}

BOOLEAN
CompareDramPostPkgRepairEntry (
  IN       HPCB_DPPRCL_REPAIR_ENTRY *DestEntry,
  IN       HPCB_DPPRCL_REPAIR_ENTRY *SrcEntry
  )
{
  ASSERT (DestEntry != NULL);
  ASSERT (SrcEntry != NULL);
  if (CompareMem (
        (VOID *)DestEntry,
        (VOID *)SrcEntry,
        sizeof (HPCB_DPPRCL_REPAIR_ENTRY)
        ) == 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/*++

Routine Description:

  This function adds a DRAM POST Package repair entry

--*/
EFI_STATUS
HpcbAddDramPostPkgRepairEntry (
  IN       HPCB_DPPRCL_REPAIR_ENTRY *Entry
  )
{
  EFI_STATUS                Status;
  BOOLEAN                   HpcbRecoveryFlag;
  HPCB_DPPRCL_REPAIR_ENTRY  *HpcbDppRepairEntries;
  HPCB_DPPRCL_REPAIR_ENTRY  *NewHpcbDppRepairEntries;
  UINT32                    SizeInByte;
  UINT16                    i;
  HPCB_HEADER               *HpcbShadowCopy;
  UINT32                    NewHpcbSize;
  UINT8                     *NewHpcb;
  BOOLEAN                   EmptyEntryFound;
  UINT16                    NewEntryId;

  Status = EFI_SUCCESS;
  EmptyEntryFound = FALSE;
  NewHpcbDppRepairEntries = NULL;
  NewHpcb = NULL;

  // Exit service, if recovery flag set
  if (mHpcbAtRuntime == FALSE) {          // byo231107 -
    Status = HygonPspGetHpobHpcbRecoveryFlag (&HpcbRecoveryFlag);
    if (HpcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("HPCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  // Exit service, if recovery flag set
  if (CheckPspRecoveryFlag () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  Status = InternalHpcbLocateType (HPCB_GROUP_MEMORY, HPCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, 0, &SizeInByte, (VOID *)&HpcbDppRepairEntries);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Cannot locate DDR4 Post Package Repair Entries\n");
    return Status;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("Entry number = %d\n", SizeInByte / sizeof (HPCB_DPPRCL_REPAIR_ENTRY));

  // Check if the target entry already exists
  for (i = 0; i < SizeInByte / sizeof (HPCB_DPPRCL_REPAIR_ENTRY); i++) {
    if (CompareDramPostPkgRepairEntry (&HpcbDppRepairEntries[i], Entry)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Duplicate DDR4 Post Package Repair Entry found\n");
      return EFI_SUCCESS;
    }
  }

  // Try adding the entry to the existing space first
  for (i = 0; i < SizeInByte / sizeof (HPCB_DPPRCL_REPAIR_ENTRY); i++) {
    if (0 == HpcbDppRepairEntries[i].Valid) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Try adding the entry to the existing space first\n");
      EmptyEntryFound = TRUE;
      CopyDramPostPkgRepairEntry (&HpcbDppRepairEntries[i], Entry);
      break;
    }
  }

  if (FALSE == EmptyEntryFound) {
    // Not enough space. Try increasing the size of the type data

    NewEntryId = (UINT16)SizeInByte / sizeof (HPCB_DPPRCL_REPAIR_ENTRY);

    if (NewEntryId >= PcdGet32 (PcdHygonMemCfgMaxPostPackageRepairEntries)) {
      if (PcdGetBool (PcdDppRepairEntryOverride) == FALSE) {
        return EFI_OUT_OF_RESOURCES;
      }

      IDS_HDT_CONSOLE_PSP_TRACE ("Too many Post Package Repair Entries requested.\n");

      // copy HpcbDppRepairEntries[1:31] to HpcbDppRepairEntries[0:30]
      CopyMem (HpcbDppRepairEntries, &HpcbDppRepairEntries[1], (PcdGet32 (PcdHygonMemCfgMaxPostPackageRepairEntries) - 1)*sizeof (HPCB_DPPRCL_REPAIR_ENTRY));
      // return EFI_OUT_OF_RESOURCES;

      // if NewEntryId == 32, allocate 32 entry space
      NewHpcbDppRepairEntries = AllocateZeroPool (SizeInByte);
      ASSERT (NewHpcbDppRepairEntries != NULL);

      if (NewHpcbDppRepairEntries == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      NewEntryId = (UINT16)(PcdGet32 (PcdHygonMemCfgMaxPostPackageRepairEntries) - 1); // the new entry is added to the rear of the HpcbDppRepairEntry array
    } else {
      NewHpcbDppRepairEntries = AllocateZeroPool (SizeInByte + sizeof (HPCB_DPPRCL_REPAIR_ENTRY));
      ASSERT (NewHpcbDppRepairEntries != NULL);
      if (NewHpcbDppRepairEntries == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      NewEntryId = (UINT16)SizeInByte / sizeof (HPCB_DPPRCL_REPAIR_ENTRY);
    }

    CopyMem (NewHpcbDppRepairEntries, HpcbDppRepairEntries, SizeInByte);
    // NewEntryId = (UINT16) SizeInByte / sizeof (HPCB_DPPRCL_REPAIR_ENTRY);
    CopyDramPostPkgRepairEntry (&NewHpcbDppRepairEntries[NewEntryId], Entry);
    HpcbShadowCopy = GetHpcbShadowCopy ();

    if ((UINT16)SizeInByte / sizeof (HPCB_DPPRCL_REPAIR_ENTRY) >= PcdGet32 (PcdHygonMemCfgMaxPostPackageRepairEntries)) {
      // If Dpp Repair entry is equal to PcdHygonMemCfgMaxPostPackageRepairEntries, no more space is added, we just add the last entry to the rear of arrary
      NewHpcbSize = HpcbShadowCopy->SizeOfHpcb;
    } else {
      NewHpcbSize = HpcbShadowCopy->SizeOfHpcb + sizeof (HPCB_DPPRCL_REPAIR_ENTRY) + ALIGN_SIZE_OF_TYPE;
    }

    NewHpcb = AllocateZeroPool (NewHpcbSize);
    IDS_HDT_CONSOLE_PSP_TRACE ("ALLOCATE[%x:%x]\n", NewHpcb, NewHpcbSize);
    ASSERT (NewHpcb != NULL);
    if (NewHpcb == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    // Shadow Copy pointer will be updated in below routine
    if ((UINT16)SizeInByte / sizeof (HPCB_DPPRCL_REPAIR_ENTRY) >= PcdGet32 (PcdHygonMemCfgMaxPostPackageRepairEntries)) {
      Status = HpcbReplaceType (HPCB_GROUP_MEMORY, HPCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, 0, (UINT8 *)&NewHpcbDppRepairEntries[0], SizeInByte, NewHpcb);
    } else {
      Status = HpcbReplaceType (HPCB_GROUP_MEMORY, HPCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, 0, (UINT8 *)&NewHpcbDppRepairEntries[0], SizeInByte + sizeof (HPCB_DPPRCL_REPAIR_ENTRY), NewHpcb);
    }

    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Cannot replace HPCB type data\n");
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/*++

Routine Description:

  This function removes a DRAM POST Package repair entry

--*/
EFI_STATUS
HpcbRemoveDramPostPkgRepairEntry (
  IN       HPCB_DPPRCL_REPAIR_ENTRY *Entry
  )
{
  EFI_STATUS                Status;
  BOOLEAN                   HpcbRecoveryFlag;
  HPCB_DPPRCL_REPAIR_ENTRY  *HpcbDppRepairEntries;
  UINT32                    SizeInByte;
  UINT16                    i;
  BOOLEAN                   TargetEntryFound;

  Status = EFI_SUCCESS;

  // Exit service, if recovery flag set
  if (mHpcbAtRuntime == FALSE) {          // byo231107 -
    Status = HygonPspGetHpobHpcbRecoveryFlag (&HpcbRecoveryFlag);
    if (HpcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("HPCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  // Exit service, if recovery flag set
  if (CheckPspRecoveryFlag () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  Status = InternalHpcbLocateType (HPCB_GROUP_MEMORY, HPCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, 0, &SizeInByte, (VOID *)&HpcbDppRepairEntries);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Cannot locate DDR4 Post Package Repair Entries\n");
    return Status;
  }

  TargetEntryFound = FALSE;
  for (i = 0; i < SizeInByte / sizeof (HPCB_DPPRCL_REPAIR_ENTRY); i++) {
    if (CompareDramPostPkgRepairEntry (&HpcbDppRepairEntries[i], Entry)) {
      TargetEntryFound = TRUE;
      HpcbDppRepairEntries[i].Valid = 0;
    }
  }

  if (!TargetEntryFound) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Cannot find the DDR4 Post Package Repair Entry\n");
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}
