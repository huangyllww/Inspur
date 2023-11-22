/**
 * @file
 *
 * HYGON Integrated Debug Hook Routines
 *
 * Contains all functions related to IDS Hook
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  IDS
 */
/*****************************************************************************
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

/*----------------------------------------------------------------------------------------
*                             M O D U L E S    U S E D
*----------------------------------------------------------------------------------------
*/
#include <Uefi.h>
#include <Pi/PiMultiPhase.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/HygonIdsHookLib.h>
#include <Library/HygonHeapLib.h>
#include <Filecode.h>

#define FILECODE  LIBRARY_HYGONIDSHOOKLIB_HYGONIDSHOOKLIB_FILECODE

/// Control IDS HOOK supported or not. TRUE: IDS HOOK supported. FALSE: IDS HOOK unsupported
BOOLEAN
IsIdsHookEnabled (
  )
{
  return PcdGetBool (PcdHygonIdsHookEnable);
}

/// IDS HOOK dispatcher, it will loop and process the table report by GetIdsHookTable
IDS_HOOK_STATUS
IdsHookDispatcher (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
  )
{
  HGPI_STATUS       Status;
  IDS_HOOK_STATUS   IdsStatus;
  IDS_HOOK_STATUS   ReturnStatus;
  IDS_HOOK_TABLE    *IdsHookTable;
  IDS_HOOK_ELEMENT  *IdsHookElement;
  BOOLEAN           IdsHookExecuted;

  ReturnStatus = IDS_HOOK_ERROR;
  Status = HGPI_ERROR;
  IdsHookExecuted = FALSE;
  IdsHookTable    = NULL;
  Status = GetIdsHookTable (&IdsHookTable);
  if ((Status != HGPI_SUCCESS) || (IdsHookTable == NULL)) {
    return IDS_HOOK_UNSUPPORTED;
  }

  // Do simple check, Signature & Revision
  if ((IdsHookTable->Header.Signature != IDS_HOOK_TABLE_SIGNATURE) ||
      (IdsHookTable->Header.Revision != IDS_HOOK_TABLE_REV_1)) {
    ASSERT (FALSE);
    return IDS_HOOK_UNSUPPORTED;
  }

  IdsHookElement = IdsHookTable->pIdsHook;
  while (IdsHookElement->HookId != IDS_HOOK_ID_END) {
    if (IdsHookElement->HookId == HookId) {
      IDS_HDT_CONSOLE (IDS_TRACE, "IdsHookFunc HookId %x\n", HookId);
      // Call work function of specific HookId
      IdsStatus = IdsHookElement->IdsHookFunc (HookId, Handle, Data);
      IdsHookExecuted = TRUE;
      if (IdsStatus != IDS_HOOK_SUCCESS) {
        ReturnStatus = IdsStatus;
      }
    }

    IdsHookElement++;
  }

  if ((IdsHookElement->HookId == IDS_HOOK_ID_END) && (IdsHookExecuted == FALSE)) {
    ReturnStatus = IDS_HOOK_UNSUPPORTED;
  }

  return ReturnStatus;
}

/// Entry function of IDS HOOK
IDS_HOOK_STATUS
IdsHookEntry (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
  )
{
  IDS_HOOK_STATUS  Status;

  // Call IP Driver self implemented IDS Hook functions
  Status = IdsHookDispatcher (HookId, Handle, Data);

  // Call External entry if haven't been implemented in IP driver
  if ((Status == IDS_HOOK_UNSUPPORTED)) {
    Status = IdsHookExtEntry (HookId, Handle, Data);
  }

  return Status;
}

#define IDSNVTABLE_FAULT_TOLERANT_SIZE  100
#define IDSNVTABLE_WATCHER_SIZE         (sizeof (UINT32))

/**
 *
 * Read IDS NV value
 *
 * It searches the IDS NV structure from  Heap until the Nv Id is found and return the NV value
 * in the table. Otherwise, return HGPI_UNSUPPORTED.
 *
 * @param[in] IdsNvIdSAT       IDS NV ID
 * @param[in,out] IdsNvValue    Pointer of IDS NV Value
 *
 * @retval HGPI_UNSUPPORTED  NV ID is not found in the structure
 *
 **/
HGPI_STATUS
HygonIdsNvReader (
  IN       IDS_NV_ID IdsNvIdSAT,
  IN OUT   UINT64 *IdsNvValue
  )
{
  LOCATE_HEAP_PTR       LocateHeapParams;
  ALLOCATE_HEAP_PARAMS  AllocateHeapParams;
  UINT32                IdsNvTableSize;
  IDS_HOOK_STATUS       IdsHookStatus;
  IDS_NV_TABLE          *IdsNvTable;
  IDS_NV_RECORD_CMN     *IdsNvRecord;
  UINTN                 IdsNvRecordSize;
  HGPI_STATUS           Status;

  // Locate IDS NV table store in HEAP
  LocateHeapParams.BufferPtr    = NULL;
  LocateHeapParams.BufferHandle = IDS_NV_TABLE_DATA_HANDLE;
  Status = HeapLocateBuffer (&LocateHeapParams, NULL);
  if ((Status == HGPI_SUCCESS) && (LocateHeapParams.BufferPtr == NULL)) {
    return HGPI_UNSUPPORTED;
  }

  IdsNvTable = (IDS_NV_TABLE *)LocateHeapParams.BufferPtr;
  if (Status != HGPI_SUCCESS) {
    // If fail, Initialize the structure by calling IdsExtLib and
    // Store the data to new allocated HEAP
    // Set Size to 0, to get the real size
    IdsNvTable     = NULL;
    IdsNvTableSize = 0;
    GetIdsNvTable (IdsNvTable, &IdsNvTableSize);
    if (IdsNvTableSize == 0) {
      // Can't get the IdsNvTable through IdsExtLib, error exit
      return HGPI_UNSUPPORTED;
    }

    AllocateHeapParams.BufferHandle = IDS_NV_TABLE_DATA_HANDLE;
    AllocateHeapParams.Persist = HEAP_BOOTTIME_SYSTEM_MEM;
    // Add 100 additional bytes for fault-tolerant
    // Add 4 additional bytes for IdsExtLib memory overlap Check
    AllocateHeapParams.RequestedBufferSize = IdsNvTableSize + IDSNVTABLE_FAULT_TOLERANT_SIZE + IDSNVTABLE_WATCHER_SIZE;
    AllocateHeapParams.BufferPtr = NULL;
    Status = HeapAllocateBuffer (&AllocateHeapParams, NULL);
    ASSERT (AllocateHeapParams.BufferPtr != NULL);
    if ((Status != HGPI_SUCCESS) || (AllocateHeapParams.BufferPtr == NULL)) {
      return HGPI_UNSUPPORTED;
    }

    // Fill the watcher dword to the tail of buffer
    *((UINT32 *)(AllocateHeapParams.BufferPtr + IdsNvTableSize + IDSNVTABLE_FAULT_TOLERANT_SIZE)) = 0x55AA;
    // Call IdsExtLib for init the buffer
    IdsHookStatus = GetIdsNvTable (AllocateHeapParams.BufferPtr, &IdsNvTableSize);
    ASSERT (IdsHookStatus == IDS_HOOK_SUCCESS);
    // Verify the watcher
    ASSERT (*((UINT32 *)(AllocateHeapParams.BufferPtr + IdsNvTableSize + IDSNVTABLE_FAULT_TOLERANT_SIZE)) == 0x55AA);
    IdsNvTable = (IDS_NV_TABLE *)AllocateHeapParams.BufferPtr;
  }

  ASSERT (IdsNvTable->Header.Signature == IDS_NV_TABLE_SIGNATURE);
  IdsNvRecordSize = 0;
  IdsNvRecord     = (IDS_NV_RECORD_CMN *)&(IdsNvTable->NvRecords);
  // Search the NV table to find the according record
  while (IdsNvRecord->Id != IDS_NV_ID_END) {
    switch (IdsNvRecord->Attrib.size) {
      case IDS_NV_ATTRIB_SIZE_BYTE:
        *IdsNvValue     = (UINT64)((IDS_NV_RECORD_U8 *)IdsNvRecord)->Value;
        IdsNvRecordSize = sizeof (IDS_NV_RECORD_U8);
        break;
      case IDS_NV_ATTRIB_SIZE_WORD:
        *IdsNvValue     = (UINT64)((IDS_NV_RECORD_U16 *)IdsNvRecord)->Value;
        IdsNvRecordSize = sizeof (IDS_NV_RECORD_U16);
        break;
      case IDS_NV_ATTRIB_SIZE_DWORD:
        *IdsNvValue     = (UINT64)((IDS_NV_RECORD_U32 *)IdsNvRecord)->Value;
        IdsNvRecordSize = sizeof (IDS_NV_RECORD_U32);
        break;
      case IDS_NV_ATTRIB_SIZE_QWORD:
        *IdsNvValue     = (UINT64)((IDS_NV_RECORD_U64 *)IdsNvRecord)->Value;
        IdsNvRecordSize = sizeof (IDS_NV_RECORD_U64);
        break;
      default:
        ASSERT (FALSE);
        break;
    }

    if (IdsNvRecord->Id == IdsNvIdSAT) {
      break;
    }

    IdsNvRecord = (IDS_NV_RECORD_CMN *)((UINT8 *)IdsNvRecord + IdsNvRecordSize);
  }

  return (IdsNvRecord->Id == IDS_NV_ID_END) ? HGPI_UNSUPPORTED : HGPI_SUCCESS;
}
