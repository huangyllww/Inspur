/* $NoKeywords:$ */

/**
 * @file
 *
 * PSP HPOB related functions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  PSP
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
#include "Uefi.h"
#include <Pi/PiMultiPhase.h>
#include "HGPI.h"
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/HygonPspBaseLib.h>
#include <Library/HygonPspMboxLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HygonPspHpobLib.h>
#include <Library/FchBaseLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/HygonCapsuleLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Filecode.h>

#define FILECODE  LIBRARY_HYGONPSPHPOBLIB_HYGONPSPHPOBLIB_FILECODE

#define CDD_ID_MASK       0x000000FF

#define DIE_TYPE_MASK     0x0000FF00
#define DIE_TYPE_SHIFT    8

#define SOCKET_ID_MASK    0x00FF0000
#define SOCKET_ID_SHIFT   16

#define IOD_ID_MASK       0xFF000000
#define IOD_ID_SHIFT      24

extern EFI_GUID gHygonPspHpobHobGuid;

/**
 *  Get HPOB core die instance ID
 *
 * @param[in]  SocketId
 *             CddId          Core Die Id
 *
 *  @retval    Instance ID
 *
 **/
UINT32
HygonPspGetHpobCddInstanceId (
  IN UINT32  SocketId,
  IN UINT32  CddId
  )
{
  UINT32    InstanceId;
  UINT32    HygonCpuModel;

  HygonCpuModel = GetHygonSocModel();

  if (HygonCpuModel == HYGON_EX_CPU) {
    InstanceId = (SocketId << 16) | (HYEX_CORE_DIE_TYPE << 8) | CddId;
  } else {
    InstanceId = (SocketId << 16) | (HYGX_CORE_DIE_TYPE << 8) | CddId;
  }

  return InstanceId;
}

/**
 *  Get HPOB IO die instance ID
 *
 * @param[in]  SocketId
 *             PhysicalDieId   IO die physical Die Id
 *
 *  @retval    Instance ID
 *
 **/
UINT32
HygonPspGetHpobIodInstanceId (
  IN UINT32  SocketId,
  IN UINT32  PhysicalDieId
  )
{
  UINT32    InstanceId;
  UINT32    HygonCpuModel;
  UINTN     LogicalDieId;
  UINTN     DieType;

  HygonCpuModel = GetHygonSocModel();

  if (HygonCpuModel == HYGON_EX_CPU) {
    InstanceId = (SocketId << 16) | (HYEX_IO_DIE_TYPE << 8);
  } else {
    FabricTopologyGetPhysicalIodDieInfo (PhysicalDieId, &LogicalDieId, &DieType);
    if (DieType == IOD_EMEI) {
      //EMEI
      InstanceId = (UINT32)((PhysicalDieId << 24) | (SocketId << 16) | (HYGX_EMEI_DIE_TYPE << 8));
    } else {
      //Dujiang
      InstanceId = (UINT32)((PhysicalDieId << 24) | (SocketId << 16) | (HYGX_DJ_DIE_TYPE << 8));
    }
  }

  return InstanceId;
}

/**
 *  Get HPOBLib Info Data
 *
 * @param[in,out]  **HpobInfo      Pointer to fill the HPOB Info
 *
 *  @retval EFI_SUCCESS       The HPOB pointer get successfully
 *
 **/
EFI_STATUS
HygonPspGetHpobInfo (
  IN OUT   HPOBLIB_INFO  **HpobInfo
  )
{
  EFI_HOB_GUID_TYPE  *GuidHob;

  ASSERT (HpobInfo != NULL);
  if (HpobInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  *HpobInfo = NULL;
  GuidHob   = GetFirstGuidHob (&gHygonPspHpobHobGuid);

  if (GuidHob == NULL) {
    return EFI_UNSUPPORTED;
  }

  *HpobInfo = (HPOBLIB_INFO *)GET_GUID_HOB_DATA (GuidHob);

  return EFI_SUCCESS;
}

/**
 *  Return Array of Entry instance as specified GroupId, DataTypeId
 *
 *  @param[in] GroupID GroupId of Hpob entry
 *  @param[in] DataTypeID DataTypeID of Hpob entry
 *  @param[in] ReadFromSpiCopy TRUE force read from SPI copy
 *  @param[out] NumofEntry Number of entry found of specific GroupId, DataTypeId
 *  @param[out] HpobEntries Point to the arrary which used to hold the HpobEntries
 *              HpobEntries arrary need be allocated outside of the routine
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
HygonPspGetHpobEntry (
  IN       UINT32         GroupID,
  IN       UINT32         DataTypeID,
  IN       UINT32         InstanceID,
  IN       BOOLEAN        ReadFromSpiCopy,
  OUT   UINT32            *NumofEntry,
  OUT   HPOB_TYPE_HEADER **HpobEntries
  )
{
  UINT8             *HpobEntryBin;
  HPOB_TYPE_HEADER  *HpobEntry;
  UINT32            _NumofEntry;
  HPOB_HEADER       *HpobHeaderPtr;
  HPOBLIB_INFO      *HpobInfoPtr;
  EFI_STATUS        Status;
  TYPE_ATTRIB       TypeAttrib;
  UINT64            EntryAddress;
  UINT32            EntrySize;
  UINT64            EntryDest;
  UINT8             SleepType;
  UINT32            HygonCpuModel;
  UINT32            HpobEntryDieType;

  *NumofEntry  = 0;
  *HpobEntries = NULL;
  HygonCpuModel = GetHygonSocModel();

  // IDS_HDT_CONSOLE_PSP_TRACE ("HygonPspGetHpobEntry %x %x\n", GroupID, DataTypeID);
  SleepType = FchReadSleepType ();
  // Get HPOB from NV if S3 path
  if ((SleepType == 0x03) || (HygonCapsuleGetStatus ()) || ReadFromSpiCopy) {
    if (BIOSEntryInfo (HPOB_NV_COPY, INSTANCE_IGNORED, &TypeAttrib, &EntryAddress, &EntrySize, &EntryDest) == FALSE) {
      IDS_HDT_CONSOLE_PSP_TRACE ("HPOB NV Entry not found\n");
      return EFI_NOT_FOUND;
    }

    HpobHeaderPtr = (HPOB_HEADER *)(UINTN)EntryAddress;
  } else {
    Status = HygonPspGetHpobInfo (&HpobInfoPtr);
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("HPOBLIB Info data uninitialized\n");
      return Status;
    }

    if (HpobInfoPtr->Supported == FALSE) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Error detected in the HPOB DATA\n");
      return EFI_UNSUPPORTED;
    }

    HpobHeaderPtr = (HPOB_HEADER *)(UINTN)HpobInfoPtr->HpobAddr;
    IDS_HDT_CONSOLE_PSP_TRACE ("HpobInfoPtr %x Supported %x HpobAddr %llx HpobSize %x\n", HpobInfoPtr, HpobInfoPtr->Supported, HpobInfoPtr->HpobAddr, HpobInfoPtr->HpobSize);
  }

  // IDS_HDT_CONSOLE_PSP_TRACE ("HygonPspGetHpobEntry %x %x\n", GroupID, DataTypeID);
  // IDS_HDT_CONSOLE_PSP_TRACE ("HPOB Addr %x\n", HpobHeaderPtr);

  HpobEntryBin  = (UINT8 *)HpobHeaderPtr;
  HpobEntryBin += HpobHeaderPtr->OffsetOfFirstEntry;
  IDS_HDT_CONSOLE_PSP_TRACE ("HpobHeaderPtr->OffsetOfFirstEntry = 0x%X\n", HpobHeaderPtr->OffsetOfFirstEntry);

  HpobEntry   = (HPOB_TYPE_HEADER *)HpobEntryBin;
  _NumofEntry = 0;

  while (HpobEntryBin  < ((UINT8 *)HpobHeaderPtr + HpobHeaderPtr->Size)) {
    if (HpobEntry->TypeSize == 0) {
      IDS_HDT_CONSOLE_PSP_TRACE ("HpobEntry @0x%x GroupID 0x%x TypeId 0x%x InstanceId 0x%x Size 0x%x\n", HpobEntry, HpobEntry->GroupID, HpobEntry->DataTypeID, HpobEntry->InstanceID, HpobEntry->TypeSize);
      _NumofEntry = 0;
      break;
    }

    if (HygonCpuModel == HYGON_EX_CPU) {
      if ((HpobEntry->GroupID == GroupID) &&
          (HpobEntry->DataTypeID == DataTypeID) &&
          (HpobEntry->InstanceID == InstanceID)) {
        //Found a matched entry
          _NumofEntry = 1;
        *HpobEntries = HpobEntry;
        IDS_HDT_CONSOLE_PSP_TRACE ("Type found @0x%x\n", HpobEntry);
        break;
      }
    }
    
    if (HygonCpuModel == HYGON_GX_CPU) {
      HpobEntryDieType = (HpobEntry->InstanceID & DIE_TYPE_MASK) >> DIE_TYPE_SHIFT;
      if (HpobEntryDieType == ((InstanceID & DIE_TYPE_MASK) >> DIE_TYPE_SHIFT)) {
        if (HpobEntryDieType == HYGX_CORE_DIE_TYPE) {
          // CDD
          if ((HpobEntry->GroupID == GroupID) &&
              (HpobEntry->DataTypeID == DataTypeID) &&
              ((HpobEntry->InstanceID & ~(UINT32)IOD_ID_MASK) == (InstanceID & ~(UINT32)IOD_ID_MASK))) {
            // Found a matched entry
            _NumofEntry  = 1;
            *HpobEntries = HpobEntry;
            IDS_HDT_CONSOLE_PSP_TRACE ("Type found @0x%x GroupID 0x%x TypeId 0x%x InstanceId 0x%x Size 0x%x\n", HpobEntry, HpobEntry->GroupID, HpobEntry->DataTypeID, HpobEntry->InstanceID, HpobEntry->TypeSize);
            break;
          }
        } else {
          // IOD
          if ((HpobEntry->GroupID == GroupID) &&
              (HpobEntry->DataTypeID == DataTypeID) &&
              ((HpobEntry->InstanceID & ~(UINT32)CDD_ID_MASK) == (InstanceID & ~(UINT32)CDD_ID_MASK))) {
            // Found a matched entry
            _NumofEntry  = 1;
            *HpobEntries = HpobEntry;
            IDS_HDT_CONSOLE_PSP_TRACE ("Type found @0x%x GroupID 0x%x TypeId 0x%x InstanceId 0x%x Size 0x%x\n", HpobEntry, HpobEntry->GroupID, HpobEntry->DataTypeID, HpobEntry->InstanceID, HpobEntry->TypeSize);
            break;
          }
        }
      }
    }

    HpobEntryBin += (HpobEntry->TypeSize);
    HpobEntry     = (HPOB_TYPE_HEADER *)HpobEntryBin;
  }

  *NumofEntry = _NumofEntry;
  IDS_HDT_CONSOLE_PSP_TRACE ("Total  %d entries found\n", _NumofEntry);

  if (_NumofEntry == 0) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
 *  Return HpobHpcbRecoveryFlag
 *
 *  @param[in, out] HPCB Recovery Flag
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
HygonPspGetHpobHpcbRecoveryFlag (
  IN OUT   BOOLEAN  *HpcbRecoveryFlag
  )
{
  EFI_STATUS                  Status;
  HPOB_HPCB_BOOT_INFO_STRUCT  *HpobEntry;

  HpobEntry = NULL;
  *HpcbRecoveryFlag = FALSE;
  if (PcdGetBool (PcdHygonPspHpcbRecoveryEnable) == FALSE) {
    // Alway return FALSE, if HpcbRecovery feature disabled
    return EFI_SUCCESS;
  }

  Status = HygonPspGetHpobEntryInstance (HPOB_GROUP_MEM, HPOB_HPCB_BOOT_INFO_TYPE, 0, FALSE, (HPOB_TYPE_HEADER **)&HpobEntry);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get HpobHpcbRecoveryFlag\n");
    return EFI_NOT_FOUND;
  }

  *HpcbRecoveryFlag = HpobEntry->HpcbRecoveryFlag;
  return Status;
}

/**
 *  Return HpobHpcbRecoveryFlag
 *
 *  @param[in, out] HPCB Active Instance
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
HygonPspGetHpobHpcbInstance (
  IN OUT   UINT8  *HpcbInstance
  )
{
  EFI_STATUS                  Status;
  HPOB_HPCB_BOOT_INFO_STRUCT  *HpobEntry;

  HpobEntry     = NULL;
  *HpcbInstance = 0;
  Status = HygonPspGetHpobEntryInstance (HPOB_GROUP_MEM, HPOB_HPCB_BOOT_INFO_TYPE, 0, FALSE, (HPOB_TYPE_HEADER **)&HpobEntry);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get HpobHpcbInstance\n");
    return EFI_NOT_FOUND;
  }

  *HpcbInstance = (UINT8)HpobEntry->HpcbActiveInstance;
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
HygonPspGetDimmCfgUpdatedFlag (
  IN OUT   BOOLEAN  *DimmCfgUpdatedFlag
  )
{
  EFI_STATUS                  Status;
  HPOB_HPCB_BOOT_INFO_STRUCT  *HpobEntry;

  HpobEntry = NULL;
  *DimmCfgUpdatedFlag = FALSE;

  Status = HygonPspGetHpobEntryInstance (HPOB_GROUP_MEM, HPOB_HPCB_BOOT_INFO_TYPE, 0, FALSE, (HPOB_TYPE_HEADER **)&HpobEntry);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Fail to get DimmCfgUpdatedFlag\n");
    return EFI_NOT_FOUND;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("DimmCfgUpdatedFlag %x\n", HpobEntry->DimmConfigurationUpdated);
  *DimmCfgUpdatedFlag = HpobEntry->DimmConfigurationUpdated;
  return Status;
}

/**
 *  Return Entry instance as specified GroupId, DataTypeId, InstanceId
 *
 *  @param[in] GroupID GroupId of Hpob entry
 *  @param[in] DataTypeID DataTypeID of Hpob entry
 *  @param[out] InstanceID InstanceID of Hpob entry
 *  @param[in] ReadFromSpiCopy TRUE force read from SPI copy
 *  @param[out] HpobEntry Point to the arrary which used to hold the HpobEntries
 *              HpobEntries arrary need be allocated outside of the routine
 *
 *  @retval       EFI_SUCCESS       Function succeed
 *  @retval       NON-ZERO          Error occurs
*/
EFI_STATUS
HygonPspGetHpobEntryInstance (
  IN       UINT32  GroupID,
  IN       UINT32  DataTypeID,
  IN       UINT32  InstanceID,
  IN       BOOLEAN  ReadFromSpiCopy,
  OUT   HPOB_TYPE_HEADER **HpobEntry
  )
{
  UINT32            _NumofEntry;
  HPOB_TYPE_HEADER  *_HpobEntry;

  *HpobEntry = NULL;
  _HpobEntry = NULL;
  IDS_HDT_CONSOLE_PSP_TRACE ("HygonPspGetHpobEntryInstance %x %x %x\n", GroupID, DataTypeID, InstanceID);

  _NumofEntry = 0;
  HygonPspGetHpobEntry (GroupID, DataTypeID, InstanceID, ReadFromSpiCopy, &_NumofEntry, &_HpobEntry);
  if (_NumofEntry == 0) {
    IDS_HDT_CONSOLE_PSP_TRACE ("No entry found\n");
    return EFI_NOT_FOUND;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("Instance found @0x%x\n", _HpobEntry);
  *HpobEntry = _HpobEntry;
  return EFI_SUCCESS;
}

/**
 *  Get HPOB address, and save to the HOB
 *
 *
 *  @retval EFI_SUCCESS       The HPOB save to HOB successfully
 *
 **/
EFI_STATUS
EFIAPI
HygonPspHpobLibConstructor (
  VOID
  )
{
  HPOB_HEADER   *HpobHeaderPtr;
  EFI_STATUS    Status;
  VOID          *GuidDataPtr;
  TYPE_ATTRIB   TypeAttrib;
  UINT64        EntryAddress;
  UINT32        EntrySize;
  UINT64        EntryDest;
  HPOBLIB_INFO  *HpobInfoPtr;
  HPOBLIB_INFO  HpobInfo;
  UINT8         SleepType;

  SleepType = FchReadSleepType ();
  if ((SleepType != 0x03) && (HygonCapsuleGetStatus () == FALSE)) {
    if (HygonPspGetHpobInfo (&HpobInfoPtr) != EFI_SUCCESS) {
      // HPOB Info data haven't been initialized
      IDS_HDT_CONSOLE_PSP_TRACE ("Init HPOB HOB Info struct\n");
      // Check the existence of HPOB entry in BIOS DIR, if not exit
      if (BIOSEntryInfo (BIOS_HPOB_INFO, INSTANCE_IGNORED, &TypeAttrib, &EntryAddress, &EntrySize, &EntryDest) == FALSE) {
        IDS_HDT_CONSOLE_PSP_TRACE ("HPOB Entry not found\n");
        return EFI_SUCCESS;
      }

      HpobHeaderPtr = (HPOB_HEADER *)(UINTN)EntryDest;
      IDS_HDT_CONSOLE_PSP_TRACE ("HPOB Addr %x size %x Version %x \n", HpobHeaderPtr, HpobHeaderPtr->Size, HpobHeaderPtr->Version);
      // Signature verification failed
      if (HpobHeaderPtr->Signature != HPOB_SIGNATURE) {
        IDS_HDT_CONSOLE_PSP_TRACE ("HPOB SIG Check fail, HPOB is not supported\n");
        return EFI_SUCCESS;
      }

      // Check HPOB Size
      ASSERT (HpobHeaderPtr->Size != 0);
      ASSERT (HpobHeaderPtr->Size != 0xFFFFFFFFul);
      if ((HpobHeaderPtr->Size == 0) ||
          (HpobHeaderPtr->Size == 0xFFFFFFFFul)) {
        IDS_HDT_CONSOLE_PSP_TRACE ("HPOB Size Check fail\n");
        return EFI_SUCCESS;
      }

      HpobInfo.Supported = TRUE;
      HpobInfo.HpobAddr  = EntryDest;
      HpobInfo.HpobSize  = HpobHeaderPtr->Size;

      // Publish the Info data to the HOB
      GuidDataPtr = BuildGuidDataHob (&gHygonPspHpobHobGuid, (VOID *)&HpobInfo, EFI_PAGES_TO_SIZE (EFI_SIZE_TO_PAGES (sizeof (HPOBLIB_INFO))));
      Status = ((GuidDataPtr == NULL) ? EFI_BAD_BUFFER_SIZE : EFI_SUCCESS);
      if (EFI_ERROR (Status)) {
        IDS_HDT_CONSOLE_PSP_TRACE ("HPOB HOB Build fail\n");
      }

      IDS_HDT_CONSOLE_PSP_TRACE ("HPOB HOB Build Success\n");
      // Reseve HPOB region through Memory allocation HOB
      BuildMemoryAllocationHob (EntryDest, EFI_PAGES_TO_SIZE (EFI_SIZE_TO_PAGES ((UINTN)(HpobHeaderPtr->Size))), EfiBootServicesData);
    }
  }

  return EFI_SUCCESS;
}
