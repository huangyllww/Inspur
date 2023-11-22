/* $NoKeywords:$ */

/**
 * @file
 *
 * Service procedure to access heap.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: LIB
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Filecode.h>
#include <Library/BaseLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/HygonHeapLib.h>

#define FILECODE  LIBRARY_HYGONHEAPPEILIB_HYGONHEAPPEILIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern EFI_GUID  gHygonHeapHobGuid;

#define HEAP_BUFFER_NODE_SIGNATURE  SIGNATURE_32 ('H', 'E', 'A', 'P')
/// Heap Node
typedef struct _HEAP_BUFFER_NODE {
  UINT32    Signature;                            ///< HEAP_BUFFER_NODE Signature
  UINT32    BufferHandle;                         ///< An unique ID of buffer.
  UINT32    BufferSize;                           ///< Size of buffer.
  UINT8     Persist;                              ///< A flag. If marked, to be stored and passed to HygonInitLate.
  UINT8     Valid;                                ///< PEI HOB doesn't provide a function for deallocate the HOB, this flag FALSE identify this node is invalid
  // UINT8         Data;                             ///< Heap Data
} HEAP_BUFFER_NODE;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
/**
 * Check If Persist field is valid
 *
 *
 * @param[in]  Persist               Persist field of HEAP_BUFFER_NODE structure
 *
 * @retval     TRUE                  Persist field Valid
 * @retval     FALSE                 Persist field Invalid
 */
BOOLEAN
STATIC
IsPersistValid (
  UINT8         Persist
  )
{
  //
  if ((Persist == HEAP_DO_NOT_EXIST_YET) ||
      (Persist == HEAP_LOCAL_CACHE) ||
      (Persist == HEAP_TEMP_MEM) ||
      (Persist == HEAP_SYSTEM_MEM) ||
      (Persist == HEAP_DO_NOT_EXIST_ANYMORE) ||
      (Persist == HEAP_S3_RESUME) ||
      (Persist == HEAP_BOOTTIME_SYSTEM_MEM) ||
      (Persist == HEAP_RUNTIME_SYSTEM_MEM)) {
    return TRUE;
  }

  return FALSE;
}

/**
 * Allocates space for a new buffer in the heap
 *
 * This function will allocate new buffer by calling PEI HOB service, the allocated memory
 * will be filled with zeros
 *
 * @param[in,out]  AllocateHeapParams structure pointer containing the size of the
 *                                   desired new region, its handle, and the
 *                                   return pointer.
 * @param[in,out]  StdHeader         Config handle for library and services.
 *
 * @retval         HGPI_SUCCESS     No error
 * @retval         HGPI_BOUNDS_CHK  Handle already exists, or not enough
 *                                   free space
 * @retval         HGPI_UNSUPPORTED Input Parameter check fail
 * @retval         HGPI_ERROR       Locate Heap Fail
 *
 */
HGPI_STATUS
HeapAllocateBuffer (
  IN OUT   ALLOCATE_HEAP_PARAMS *AllocateHeapParams,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  HGPI_STATUS       Status;
  HEAP_BUFFER_NODE  HeapBufferNode;
  LOCATE_HEAP_PTR   LocateHeapPtr;
  VOID              *HobPtr;

  // Input Parameters check
  if (IsPersistValid (AllocateHeapParams->Persist) == FALSE) {
    return HGPI_UNSUPPORTED;
  }

  // Check duplicated handle
  LocateHeapPtr.BufferHandle = AllocateHeapParams->BufferHandle;
  if (HeapLocateBuffer (&LocateHeapPtr, NULL) == HGPI_SUCCESS) {
    return HGPI_BOUNDS_CHK;
  }

  // Create HEAP private information as supplement for EFI GUID HOB
  HeapBufferNode.Signature    = HEAP_BUFFER_NODE_SIGNATURE;
  HeapBufferNode.BufferHandle = AllocateHeapParams->BufferHandle;
  HeapBufferNode.BufferSize   = AllocateHeapParams->RequestedBufferSize;
  HeapBufferNode.Persist = AllocateHeapParams->Persist;
  HeapBufferNode.Valid   = TRUE;
  // Create a Guid Hob, with request size plus the node size
  HobPtr = BuildGuidDataHob (&gHygonHeapHobGuid, &HeapBufferNode, sizeof (HEAP_BUFFER_NODE) + AllocateHeapParams->RequestedBufferSize);

  if (HobPtr != NULL) {
    AllocateHeapParams->BufferPtr = (UINT8 *)(HobPtr) + sizeof (HEAP_BUFFER_NODE);
    // Fill the allocate HEAP with INT3 machine code
    // SetMem (AllocateHeapParams->BufferPtr, AllocateHeapParams->RequestedBufferSize, 0xCC);
    SetMem (AllocateHeapParams->BufferPtr, AllocateHeapParams->RequestedBufferSize, 0);
  }

  Status = (HobPtr == NULL) ? HGPI_ERROR : HGPI_SUCCESS;
  return Status;
}

/**
*  Internal routine used to find the matched HEAP Node in HOB List
*
* @param[in,out]  HeapHandle An unique ID of Heap buffer
*
  @return The matched HEAP GUID HOB among the whole HOB list, NULL if not founded
*/
VOID *
InternalGetHeapBufferNode (
  UINT32        HeapHandle
  )
{
  EFI_HOB_GUID_TYPE  *HeapGuidHob;
  HEAP_BUFFER_NODE   *HeapBufferNode;

  HeapGuidHob = GetFirstGuidHob (&gHygonHeapHobGuid);
  while (HeapGuidHob != NULL) {
    HeapBufferNode = GET_GUID_HOB_DATA (HeapGuidHob);

    // Check If node matches
    if ((HeapBufferNode->BufferHandle == HeapHandle) &&
        (HeapBufferNode->Signature == HEAP_BUFFER_NODE_SIGNATURE) &&
        (HeapBufferNode->Valid == TRUE)) {
      return GET_GUID_HOB_DATA (HeapGuidHob);
    }

    // Due GetNextGuidHob function does not skip the starting HOB pointer unconditionally: it returns HobStart back if HobStart itself meets the requirement;
    // Move to Next Hob Entry
    HeapGuidHob = GET_NEXT_HOB (HeapGuidHob);
    HeapGuidHob = GetNextGuidHob (&gHygonHeapHobGuid, HeapGuidHob);
  }

  return NULL;
}

/**
 * Deallocates a previously allocated buffer in the heap PEI HOB service
 *
 * This function will deallocate buffer by calling
 *
 * @param[in]      BufferHandle      Handle of the buffer to free.
 * @param[in]      StdHeader         Config handle for library and services.
 *
 * @retval         HGPI_SUCCESS     No error
 * @retval         HGPI_BOUNDS_CHK  Handle does not exist on the heap
 *
 */
HGPI_STATUS
HeapDeallocateBuffer (
  IN       UINT32 BufferHandle,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  HEAP_BUFFER_NODE  *HeapBufferNode;

  HeapBufferNode = (HEAP_BUFFER_NODE *)InternalGetHeapBufferNode (BufferHandle);

  if (HeapBufferNode == NULL) {
    // Can't find the specific Heap in HOB List
    return HGPI_BOUNDS_CHK;
  }

  // Invalid the Node
  HeapBufferNode->Valid = FALSE;
  return HGPI_SUCCESS;
}

/**
 * Locates a previously allocated buffer on the heap.
 *
 * This function searches the heap for a buffer with the desired handle, and
 * returns a pointer to the buffer.
 *
 * @param[in,out]  LocateHeap     Structure containing the buffer's handle,
 *                                   and the return pointer.
 * @param[in]      StdHeader         Config handle for library and services.
 *
 * @retval         HGPI_SUCCESS     No error
 * @retval         HGPI_BOUNDS_CHK  Handle does not exist on the heap
 *
 */
HGPI_STATUS
HeapLocateBuffer (
  IN OUT   LOCATE_HEAP_PTR *LocateHeap,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  HEAP_BUFFER_NODE  *HeapBufferNode;

  HeapBufferNode = (HEAP_BUFFER_NODE *)InternalGetHeapBufferNode (LocateHeap->BufferHandle);

  if (HeapBufferNode == NULL) {
    // Can't find the specific Heap in HOB List
    LocateHeap->BufferPtr  = NULL;
    LocateHeap->BufferSize = 0;
    return HGPI_BOUNDS_CHK;
  }

  LocateHeap->BufferPtr  = ((UINT8 *)HeapBufferNode + sizeof (HEAP_BUFFER_NODE));
  LocateHeap->BufferSize = HeapBufferNode->BufferSize;
  return HGPI_SUCCESS;
}
