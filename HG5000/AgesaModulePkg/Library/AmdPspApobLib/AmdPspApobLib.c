/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP APOB related functions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309172 $   @e \$Date: 2014-12-10 21:13:24 +0800 (Wed, 10 Dec 2014) $
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
#include "Uefi.h"
#include <Pi/PiMultiPhase.h>
#include "AGESA.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Library/AmdPspMboxLibV2.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/AmdPspApobLib.h>
#include <Library/FchBaseLib.h>
#include <Library/AmdSocBaseLib.h>
#include <Library/AmdCapsuleLib.h>
#include <Filecode.h>

#define FILECODE LIBRARY_AMDPSPAPOBLIB_AMDPSPAPOBLIB_FILECODE

extern EFI_GUID gAmdPspApobHobGuid;


/**
 *  Get APOBLib Info Data
 *
 * @param[in,out]  **ApobInfo      Pointer to fill the APOB Info
 *
 *  @retval EFI_SUCCESS       The APOB pointer get successfully
 *
 **/
EFI_STATUS
AmdPspGetApobInfo (
  IN OUT   APOBLIB_INFO  **ApobInfo
  )
{
  EFI_HOB_GUID_TYPE                 *GuidHob;

  ASSERT (ApobInfo != NULL);
  if (ApobInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  *ApobInfo = NULL;
  GuidHob = GetFirstGuidHob (&gAmdPspApobHobGuid);

  if (GuidHob == NULL) {
    return EFI_UNSUPPORTED;
  }

  *ApobInfo = (APOBLIB_INFO *) GET_GUID_HOB_DATA (GuidHob);

  return EFI_SUCCESS;
}
/**
 *  Return Array of Entry instance as specified GroupId, DataTypeId
 *
 *  @param[in] GroupID GroupId of Apob entry
 *  @param[in] DataTypeID DataTypeID of Apob entry
 *  @param[in] ReadFromSpiCopy TRUE force read from SPI copy
 *  @param[out] NumofEntry Number of entry found of specific GroupId, DataTypeId
 *  @param[out] ApobEntries Point to the arrary which used to hold the ApobEntries
 *              ApobEntries arrary need be allocated outside of the routine
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
AmdPspGetApobEntry (
  IN       UINT32  GroupID,
  IN       UINT32  DataTypeID,
  IN       BOOLEAN  ReadFromSpiCopy,
     OUT   UINT32  *NumofEntry,
     OUT   APOB_TYPE_HEADER **ApobEntries
  )
{
  UINT8 *ApobEntryBin;
  APOB_TYPE_HEADER *ApobEntry;
  UINT32 _NumofEntry;
  APOB_HEADER *ApobHeaderPtr;
  APOBLIB_INFO *ApobInfoPtr;
  EFI_STATUS Status;
  TYPE_ATTRIB       TypeAttrib;
  UINT64            EntryAddress;
  UINT32            EntrySize;
  UINT64            EntryDest;
  UINT8             SleepType;

  *NumofEntry = 0;
  *ApobEntries = NULL;
//IDS_HDT_CONSOLE_PSP_TRACE ("AmdPspGetApobEntry %x %x\n", GroupID, DataTypeID);          // winddy -
  SleepType = FchReadSleepType ();
  //Get APOB from NV if S3 path
  if ((SleepType == 0x03) || (AmdCapsuleGetStatus ()) || ReadFromSpiCopy) {
    if (BIOSEntryInfo (APOB_NV_COPY, INSTANCE_IGNORED, &TypeAttrib, &EntryAddress, &EntrySize, &EntryDest) == FALSE) {
      IDS_HDT_CONSOLE_PSP_TRACE ("APOB NV Entry not found\n");
      return EFI_NOT_FOUND;
    }
    ApobHeaderPtr = (APOB_HEADER *) (UINTN) EntryAddress;
  } else {
    Status = AmdPspGetApobInfo (&ApobInfoPtr);
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("APOBLIB Info data uninitialized\n");
      return Status;
    }

    if (ApobInfoPtr->Supported == FALSE) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Error detected in the APOB DATA\n");
      return EFI_UNSUPPORTED;
    }
    ApobHeaderPtr = (APOB_HEADER *) (UINTN) ApobInfoPtr->ApobAddr;
//  IDS_HDT_CONSOLE_PSP_TRACE ("ApobInfoPtr %x Supported %x ApobAddr %llx ApobSize %x\n", ApobInfoPtr, ApobInfoPtr->Supported, ApobInfoPtr->ApobAddr, ApobInfoPtr->ApobSize);   // winddy -
  }


//IDS_HDT_CONSOLE_PSP_TRACE ("AmdPspGetApobEntry %x %x\n", GroupID, DataTypeID);       // winddy -
//IDS_HDT_CONSOLE_PSP_TRACE ("APOB Addr %x\n", ApobHeaderPtr);                         // winddy -

  if (ApobHeaderPtr->Version >= 5) {
    ApobEntryBin = (UINT8 *) ApobHeaderPtr;
    ApobEntryBin += ApobHeaderPtr->OffsetOfFirstEntry;
  } else {
    ApobEntryBin = (UINT8 *) (ApobHeaderPtr + 1);
  }
  ApobEntry = (APOB_TYPE_HEADER *) ApobEntryBin;
  _NumofEntry = 0;

  while (ApobEntryBin  < ((UINT8 *) ApobHeaderPtr + ApobHeaderPtr->Size)) {
//  IDS_HDT_CONSOLE_PSP_TRACE ("ApobEntry @0x%x GroupID 0x%x TypeId 0x%x\n", ApobEntry, ApobEntry->GroupID, ApobEntry->DataTypeID);     // winddy -
    if ((ApobEntry->GroupID == GroupID) &&
        (ApobEntry->DataTypeID == DataTypeID)) {
      //Found a matched entry
      _NumofEntry++;
      *ApobEntries = ApobEntry;
      ApobEntries++;
//    IDS_HDT_CONSOLE_PSP_TRACE ("Type found @0x%x\n", ApobEntry);               // winddy -
    }
    ApobEntryBin += (ApobEntry->TypeSize);
    ApobEntry = (APOB_TYPE_HEADER *) ApobEntryBin;
  }

  *NumofEntry = _NumofEntry;
//IDS_HDT_CONSOLE_PSP_TRACE ("Total  %d entries found\n", _NumofEntry);          // winddy -

  if (_NumofEntry == 0) {
    IDS_HDT_CONSOLE_PSP_TRACE ("AmdPspGetApobEntry(%X,%X) Not Found\n", GroupID, DataTypeID);      // winddy +
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
 *  Return ApobApcbRecoveryFlag
 *
 *  @param[in, out] APCB Recovery Flag
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
AmdPspGetApobApcbRecoveryFlag (
  IN OUT   BOOLEAN  *ApcbRecoveryFlag
  )
{
  EFI_STATUS Status;
  APOB_APCB_BOOT_INFO_STRUCT *ApobEntry;
  ApobEntry = NULL;
  *ApcbRecoveryFlag = FALSE;
  if (PcdGetBool (PcdAmdPspApcbRecoveryEnable) == FALSE) {
    // Alway return FALSE, if ApcbRecovery feature disabled
    return EFI_SUCCESS;
  }
  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_APCB_BOOT_INFO_TYPE, 0, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get ApobApcbRecoveryFlag\n");
    return EFI_NOT_FOUND;
  }
  *ApcbRecoveryFlag = ApobEntry->ApcbRecoveryFlag;
  return Status;
}

/**
 *  Return ApobApcbRecoveryFlag
 *
 *  @param[in, out] APCB Active Instance
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
AmdPspGetApobApcbInstance (
  IN OUT   UINT8  *ApcbInstance
  )
{
  EFI_STATUS Status;
  APOB_APCB_BOOT_INFO_STRUCT *ApobEntry;
  ApobEntry = NULL;
  *ApcbInstance = 0;
  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_APCB_BOOT_INFO_TYPE, 0, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get ApobApcbInstance\n");
    return EFI_NOT_FOUND;
  }
  *ApcbInstance = (UINT8) ApobEntry->ApcbActiveInstance;
  return Status;
}

/**
 *  Return DimmCfgUpdatedFlag
 *
 *  @param[in, out] DimmCfgUpdatedFlag
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
AmdPspGetDimmCfgUpdatedFlag (
  IN OUT   BOOLEAN  *DimmCfgUpdatedFlag
  )
{
  EFI_STATUS Status;
  APOB_APCB_BOOT_INFO_STRUCT *ApobEntry;
  ApobEntry = NULL;
  *DimmCfgUpdatedFlag = FALSE;

  Status = AmdPspGetApobEntryInstance (APOB_MEM, APOB_APCB_BOOT_INFO_TYPE, 0, FALSE, (APOB_TYPE_HEADER **) &ApobEntry);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get DimmCfgUpdatedFlag\n");
    return EFI_NOT_FOUND;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("DimmCfgUpdatedFlag %x\n", ApobEntry->DimmConfigurationUpdated);
  *DimmCfgUpdatedFlag = ApobEntry->DimmConfigurationUpdated;
  return Status;
}
/**
 *  Return Entry instance as specified GroupId, DataTypeId, InstanceId
 *
 *  @param[in] GroupID GroupId of Apob entry
 *  @param[in] DataTypeID DataTypeID of Apob entry
 *  @param[out] InstanceID InstanceID of Apob entry
 *  @param[in] ReadFromSpiCopy TRUE force read from SPI copy
 *  @param[out] ApobEntry Point to the arrary which used to hold the ApobEntries
 *              ApobEntries arrary need be allocated outside of the routine
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
AmdPspGetApobEntryInstance (
  IN       UINT32  GroupID,
  IN       UINT32  DataTypeID,
  IN       UINT32  InstanceID,
  IN       BOOLEAN  ReadFromSpiCopy,
     OUT   APOB_TYPE_HEADER **ApobEntry
  )
{
  UINT32 i;
  UINT32 NumofInstance;
  APOB_TYPE_HEADER *_ApobEntries[APOB_ENTRY_INSTANCE_MAX];
  UINT32 _NumofEntry;
  APOB_TYPE_HEADER *_ApobEntry;

  *ApobEntry = NULL;
  _ApobEntry = NULL;
  IDS_HDT_CONSOLE_PSP_TRACE ("AmdPspGetApobEntryInstance %x %x %x\n", GroupID, DataTypeID, InstanceID);

  _NumofEntry = 0;
  AmdPspGetApobEntry (GroupID, DataTypeID, ReadFromSpiCopy, &_NumofEntry, &_ApobEntries[0]);
  if (_NumofEntry == 0) {
    IDS_HDT_CONSOLE_PSP_TRACE ("No entry found\n");
    return EFI_NOT_FOUND;
  }
  //Scan the Arrary to get the specific instance
  NumofInstance = 0;
  for (i = 0; i < _NumofEntry; i++) {
    if (_ApobEntries[i]->InstanceID == InstanceID) {
      NumofInstance++;
      _ApobEntry = _ApobEntries[i];
    }
  }
  //Instance ID can't be duplicated
  ASSERT (NumofInstance <= 1);

  if (NumofInstance == 0) {
    return EFI_NOT_FOUND;
  } else if (NumofInstance == 1) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Instance found @0x%x\n", _ApobEntry);
    *ApobEntry = _ApobEntry;
    return EFI_SUCCESS;
  } else {
    return EFI_UNSUPPORTED;
  }
}

/**
 *  Get APOB address, and save to the HOB
 *
 *
 *  @retval EFI_SUCCESS       The APOB save to HOB successfully
 *
 **/
EFI_STATUS
EFIAPI
AmdPspApobLibConstructor (
  VOID
  )
{
  APOB_HEADER *ApobHeaderPtr;
  EFI_STATUS Status;
  VOID *GuidDataPtr;
  TYPE_ATTRIB       TypeAttrib;
  UINT64            EntryAddress;
  UINT32            EntrySize;
  UINT64            EntryDest;
  APOBLIB_INFO *ApobInfoPtr;
  APOBLIB_INFO ApobInfo;
  UINT8 SleepType;

  //Exit if BR program
  if (SocFamilyIdentificationCheck (F15_BR_RAW_ID)) {
    return EFI_SUCCESS;
  }

  SleepType = FchReadSleepType ();
  if ((SleepType != 0x03) && (AmdCapsuleGetStatus () == FALSE)) {
    if (AmdPspGetApobInfo (&ApobInfoPtr) != EFI_SUCCESS) {
      //APOB Info data haven't been initialized
      IDS_HDT_CONSOLE_PSP_TRACE ("Init APOB HOB Info struct\n");
      //Check the existence of APOB entry in BIOS DIR, if not exit
      if (BIOSEntryInfo (BIOS_APOB_INFO, INSTANCE_IGNORED, &TypeAttrib, &EntryAddress, &EntrySize, &EntryDest) == FALSE) {
        IDS_HDT_CONSOLE_PSP_TRACE ("APOB Entry not found\n");
        return EFI_SUCCESS;
      }

      ApobHeaderPtr = (APOB_HEADER *) (UINTN) EntryDest;
      IDS_HDT_CONSOLE_PSP_TRACE ("APOB Addr %x\n", ApobHeaderPtr);
      //Signature verification failed
      if (ApobHeaderPtr->Signature != APOB_SIGNATURE) {
        IDS_HDT_CONSOLE_PSP_TRACE ("APOB SIG Check fail, APOB is not supported\n");
        return EFI_SUCCESS;
      }

      // Check APOB Size
      ASSERT (ApobHeaderPtr->Size != 0);
      ASSERT (ApobHeaderPtr->Size != 0xFFFFFFFFul);
      if ((ApobHeaderPtr->Size == 0) ||
          (ApobHeaderPtr->Size == 0xFFFFFFFFul)) {
        IDS_HDT_CONSOLE_PSP_TRACE ("APOB Size Check fail\n");
        return EFI_SUCCESS;
      }
      ApobInfo.Supported = TRUE;
      ApobInfo.ApobAddr = EntryDest;
      ApobInfo.ApobSize = ApobHeaderPtr->Size;

      //Publish the Info data to the HOB
      GuidDataPtr = BuildGuidDataHob (&gAmdPspApobHobGuid, (VOID *) &ApobInfo, EFI_PAGES_TO_SIZE (EFI_SIZE_TO_PAGES (sizeof (APOBLIB_INFO))));
      Status = ((GuidDataPtr == NULL) ? EFI_BAD_BUFFER_SIZE : EFI_SUCCESS);
      if (EFI_ERROR (Status)) {
        IDS_HDT_CONSOLE_PSP_TRACE ("APOB HOB Build fail\n");
      }
      IDS_HDT_CONSOLE_PSP_TRACE ("APOB HOB Build Success\n");
      //Reseve APOB region through Memory allocation HOB
      BuildMemoryAllocationHob (EntryDest, EFI_PAGES_TO_SIZE (EFI_SIZE_TO_PAGES (ApobHeaderPtr->Size)), EfiBootServicesData);
    }
  }
  return EFI_SUCCESS;
}
