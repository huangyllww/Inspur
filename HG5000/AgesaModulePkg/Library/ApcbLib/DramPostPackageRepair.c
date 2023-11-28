/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Library
 *
 * Contains interface to the AMD AGESA library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
 ******************************************************************************
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

/*****************************************************************************
 *
 * This software package can be used to enable the Overclocking of certain
 * AMD processors and its use is subject to the terms and conditions of the
 * AMD Overclocking Waiver. Enabling overclocking through use of the low-level
 * routines included in this package and operating an AMD processor outside of
 * the applicable AMD product specifications will void any AMD warranty and can
 * result in damage to the processor or the system into which the processor has
 * been integrated. The user of this software assumes, and AMD disclaims, all
 * risk, liability, costs and damages relating to or arising from the overclocking
 * of AMD processors.
 *
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Uefi.h"
#include <Library/BaseLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/ApcbZpLib.h>
#include <Library/AmdSocBaseLib.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/AmdPspApobLib.h>
#include <Filecode.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE        LIBRARY_APCBLIB_DRAMPOSTPACKAGEREPAIR_FILECODE

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

extern UINT8                     mApcbInstance;
extern BOOLEAN                   mAtRuntime;

EFI_STATUS
InternalApcbLocateType (
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
ApcbGetDramPostPkgRepairEntries (
  IN OUT   APCB_DPPRCL_REPAIR_ENTRY **pApcbDppRepairEntries,
  IN OUT   UINT32                   *SizeInByte
  )
{
  EFI_STATUS                Status;
  BOOLEAN                   ApcbRecoveryFlag;

  Status  = EFI_SUCCESS;

  //Exit service, if recovery flag set
  if (mAtRuntime == FALSE) {
    Status = AmdPspGetApobApcbRecoveryFlag (&ApcbRecoveryFlag);
    if (ApcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("APCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  //Exit service, if recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  Status = InternalApcbLocateType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, 0, SizeInByte, (VOID *) pApcbDppRepairEntries);
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
ApcbClearDramPostPkgRepairEntry (
  VOID
  )
{
  EFI_STATUS                Status;
  BOOLEAN                   ApcbRecoveryFlag;
  APCB_DPPRCL_REPAIR_ENTRY  ApcbDppRepairEntries[2];
  APCB_HEADER               *ApcbShadowCopy;
  UINT32                    NewApcbSize;
  UINT8                     *NewApcb;

  Status  = EFI_SUCCESS;
  NewApcb = NULL;

  //Exit service, if recovery flag set
  if (mAtRuntime == FALSE) {
    Status = AmdPspGetApobApcbRecoveryFlag (&ApcbRecoveryFlag);
    if (ApcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("APCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  //Exit service, if recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  SetMem (&ApcbDppRepairEntries[0], sizeof (APCB_DPPRCL_REPAIR_ENTRY) * 2, 0);

  // Leave only one clean entry in the APCB data of APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR
  ApcbShadowCopy = GetApcbShadowCopy ();
  NewApcbSize = ApcbShadowCopy->SizeOfApcb + sizeof (APCB_DPPRCL_REPAIR_ENTRY) + ALIGN_SIZE_OF_TYPE;
  NewApcb = AllocateZeroPool (NewApcbSize);
  IDS_HDT_CONSOLE_PSP_TRACE ("ALLOCATE[%x:%x]\n", NewApcb, NewApcbSize);
  ASSERT (NewApcb != NULL);
  if (NewApcb == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //Shadow Copy pointer will be updated in below routine
  Status = ApcbReplaceType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, 0, (UINT8 *)&ApcbDppRepairEntries[0], sizeof (APCB_DPPRCL_REPAIR_ENTRY), NewApcb);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Cannot replace APCB type data\n");
    return Status;
  }

  return EFI_SUCCESS;
}

VOID
CopyDramPostPkgRepairEntry (
  IN       APCB_DPPRCL_REPAIR_ENTRY *DestEntry,
  IN       APCB_DPPRCL_REPAIR_ENTRY *SrcEntry
  )
{
  ASSERT (DestEntry != NULL);
  ASSERT (SrcEntry != NULL);

  CopyMem (
      (VOID *) DestEntry,
      (VOID *) SrcEntry,
      sizeof (APCB_DPPRCL_REPAIR_ENTRY)
      );
}

BOOLEAN
CompareDramPostPkgRepairEntry (
  IN       APCB_DPPRCL_REPAIR_ENTRY *DestEntry,
  IN       APCB_DPPRCL_REPAIR_ENTRY *SrcEntry
  )
{
  ASSERT (DestEntry != NULL);
  ASSERT (SrcEntry != NULL);
  if (CompareMem (
        (VOID *) DestEntry,
        (VOID *) SrcEntry,
        sizeof (APCB_DPPRCL_REPAIR_ENTRY)
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
ApcbAddDramPostPkgRepairEntry (
  IN       APCB_DPPRCL_REPAIR_ENTRY *Entry
  )
{
  EFI_STATUS                Status;
  BOOLEAN                   ApcbRecoveryFlag;
  APCB_DPPRCL_REPAIR_ENTRY  *ApcbDppRepairEntries;
  APCB_DPPRCL_REPAIR_ENTRY  *NewApcbDppRepairEntries;
  UINT32                    SizeInByte;
  UINT16                    i;
  APCB_HEADER               *ApcbShadowCopy;
  UINT32                    NewApcbSize;
  UINT8                     *NewApcb;
  BOOLEAN                   EmptyEntryFound;
  UINT16                    NewEntryId;

  Status                  = EFI_SUCCESS;
  EmptyEntryFound         = FALSE;
  NewApcbDppRepairEntries = NULL;
  NewApcb                 = NULL;

  //Exit service, if recovery flag set
  if (mAtRuntime == FALSE) {
    Status = AmdPspGetApobApcbRecoveryFlag (&ApcbRecoveryFlag);
    if (ApcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("APCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  //Exit service, if recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  Status = InternalApcbLocateType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, 0, &SizeInByte, (VOID *) &ApcbDppRepairEntries);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Cannot locate DDR4 Post Package Repair Entries\n");
    return Status;
  }
  
  IDS_HDT_CONSOLE_PSP_TRACE ("PS-531: Entry number = %d\n", SizeInByte / sizeof (APCB_DPPRCL_REPAIR_ENTRY));
  
  // Check if the target entry already exists
  for (i = 0; i < SizeInByte / sizeof (APCB_DPPRCL_REPAIR_ENTRY); i ++) {
    if (CompareDramPostPkgRepairEntry (&ApcbDppRepairEntries[i], Entry)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Duplicate DDR4 Post Package Repair Entry found\n");
      return EFI_SUCCESS;
    }
  }

  // Try adding the entry to the existing space first
  for (i = 0; i < SizeInByte / sizeof (APCB_DPPRCL_REPAIR_ENTRY); i ++) {
    if (0 == ApcbDppRepairEntries[i].Valid) {
      IDS_HDT_CONSOLE_PSP_TRACE ("PS-531: Try adding the entry to the existing space first\n");
      EmptyEntryFound = TRUE;
      CopyDramPostPkgRepairEntry (&ApcbDppRepairEntries[i], Entry);
      break;
    }
  }

  if (FALSE == EmptyEntryFound) {
    // Not enough space. Try increasing the size of the type data
	//PS-531
	NewEntryId = (UINT16) SizeInByte / sizeof (APCB_DPPRCL_REPAIR_ENTRY);

    if (NewEntryId >= PcdGet32 (PcdAmdMemCfgMaxPostPackageRepairEntries)) {

      if (PcdGetBool (PcdDppRepairEntryOverride) == FALSE)
      {
          return EFI_OUT_OF_RESOURCES;
      }
        
      IDS_HDT_CONSOLE_PSP_TRACE ("Too many Post Package Repair Entries requested.\n");      
      //PS-531
      //copy ApcbDppRepairEntries[1:31] to ApcbDppRepairEntries[0:30]
      CopyMem (ApcbDppRepairEntries, &ApcbDppRepairEntries[1], (PcdGet32 (PcdAmdMemCfgMaxPostPackageRepairEntries) - 1)*sizeof (APCB_DPPRCL_REPAIR_ENTRY));
      //return EFI_OUT_OF_RESOURCES;
      
      //if NewEntryId == 32, allocate 32 entry space
      NewApcbDppRepairEntries = AllocateZeroPool (SizeInByte);
      ASSERT (NewApcbDppRepairEntries != NULL);
      
      if (NewApcbDppRepairEntries == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      
      NewEntryId = (UINT16)(PcdGet32 (PcdAmdMemCfgMaxPostPackageRepairEntries) - 1); //the new entry is added to the rear of the ApcbDppRepairEntry array
    } else {
        NewApcbDppRepairEntries = AllocateZeroPool (SizeInByte + sizeof (APCB_DPPRCL_REPAIR_ENTRY));
        ASSERT (NewApcbDppRepairEntries != NULL);
        if (NewApcbDppRepairEntries == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        NewEntryId = (UINT16) SizeInByte / sizeof (APCB_DPPRCL_REPAIR_ENTRY); //PS-531
    }
    
    CopyMem (NewApcbDppRepairEntries, ApcbDppRepairEntries, SizeInByte);
    //NewEntryId = (UINT16) SizeInByte / sizeof (APCB_DPPRCL_REPAIR_ENTRY); //PS-531
    CopyDramPostPkgRepairEntry (&NewApcbDppRepairEntries[NewEntryId], Entry);
    ApcbShadowCopy = GetApcbShadowCopy ();
    
    if ((UINT16) SizeInByte / sizeof (APCB_DPPRCL_REPAIR_ENTRY) >= PcdGet32 (PcdAmdMemCfgMaxPostPackageRepairEntries))
    {
        //If Dpp Repair entry is equal to PcdAmdMemCfgMaxPostPackageRepairEntries, no more space is added, we just add the last entry to the rear of arrary
        NewApcbSize = ApcbShadowCopy->SizeOfApcb;
    }else {
        NewApcbSize = ApcbShadowCopy->SizeOfApcb + sizeof (APCB_DPPRCL_REPAIR_ENTRY) + ALIGN_SIZE_OF_TYPE;
    }
    
    NewApcb = AllocateZeroPool (NewApcbSize);
    IDS_HDT_CONSOLE_PSP_TRACE ("ALLOCATE[%x:%x]\n", NewApcb, NewApcbSize);
    ASSERT (NewApcb != NULL);
    if (NewApcb == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    //Shadow Copy pointer will be updated in below routine
    if ((UINT16) SizeInByte / sizeof (APCB_DPPRCL_REPAIR_ENTRY) >= PcdGet32 (PcdAmdMemCfgMaxPostPackageRepairEntries)) {
        Status = ApcbReplaceType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, 0, (UINT8 *)&NewApcbDppRepairEntries[0], SizeInByte, NewApcb);
    }else {
        Status = ApcbReplaceType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, 0, (UINT8 *)&NewApcbDppRepairEntries[0], SizeInByte + sizeof (APCB_DPPRCL_REPAIR_ENTRY), NewApcb);
    }
    
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Cannot replace APCB type data\n");
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
ApcbRemoveDramPostPkgRepairEntry (
  IN       APCB_DPPRCL_REPAIR_ENTRY *Entry
  )
{
  EFI_STATUS                Status;
  BOOLEAN                   ApcbRecoveryFlag;
  APCB_DPPRCL_REPAIR_ENTRY  *ApcbDppRepairEntries;
  UINT32                    SizeInByte;
  UINT16                    i;
  BOOLEAN                   TargetEntryFound;

  Status  = EFI_SUCCESS;

  //Exit service, if recovery flag set
  if (mAtRuntime == FALSE) {
    Status = AmdPspGetApobApcbRecoveryFlag (&ApcbRecoveryFlag);
    if (ApcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("APCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  //Exit service, if recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  Status = InternalApcbLocateType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, 0, &SizeInByte, (VOID *) &ApcbDppRepairEntries);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Cannot locate DDR4 Post Package Repair Entries\n");
    return Status;
  }

  TargetEntryFound = FALSE;
  for (i = 0; i < SizeInByte / sizeof (APCB_DPPRCL_REPAIR_ENTRY); i ++) {
    if (CompareDramPostPkgRepairEntry (&ApcbDppRepairEntries[i], Entry)) {
      TargetEntryFound                        = TRUE;
      ApcbDppRepairEntries[i].Valid           = 0;
    }
  }

  if (!TargetEntryFound) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Cannot find the DDR4 Post Package Repair Entry\n");
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}
