/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPM Library for CPM Table Access
 *
 * Contains interface to the HYGON CPM library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  Library
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

#include <HygonCpmBase.h>
#include <HygonCpmTable.h>
#include CPM_PPI_DEFINITION (HygonCpmTablePpi)

/*----------------------------------------------------------------------------------------*/

/**
 * Get the pointer of HYGON CPM Table with the required signature.
 *
 * @param[in]     This        The Pointer of CPM Table Ppi or Protocol
 * @param[in]     TableId     CPM Table Signature
 *
 * @retval        The pointer of CPM table
 */
VOID *
EFIAPI
CpmGetTablePtr (
  IN       VOID                        *This,
  IN       UINT32                      TableId
  )
{
  return CpmSearchAndUpdateTable (This, TableId, 0);
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get the pointer of HYGON CPM Table with the required signature. The table can be rewritable.
 *
 * @param[in]     This        The Pointer of CPM Table Ppi or Protocol
 * @param[in]     TableId     CPM Table Signature
 *
 * @retval        The pointer of CPM table
 */
VOID *
EFIAPI
CpmGetTablePtr2 (
  IN       VOID                        *This,
  IN       UINT32                      TableId
  )
{
  return CpmSearchAndUpdateTable (This, TableId, 3);
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get the pointer of HYGON CPM Table with the required signature and update the status
 * in the table list.
 *
 * @param[in]     This        The Pointer of CPM Table Ppi or Protocol
 * @param[in]     TableId     CPM Table Signature
 * @param[in]     Flag        Table Status to be updated
 *                              0: Keep original status
 *                              1: Enable the table
 *                              2: Disable the table
 *                              3: Make the table to re-writable
 *                              4: Enable and Make the table to re-writable
 *
 * @retval        The pointer of CPM table
 */
VOID *
EFIAPI
CpmSearchAndUpdateTable (
  IN       VOID                        *This,
  IN       UINT32                      TableId,
  IN       UINT8                       Flag
  )
{
  HYGON_CPM_MAIN_TABLE           *MainTablePtr;
  HYGON_CPM_TABLE_LIST           *TableListPtr;
  HYGON_CPM_HOB_HEADER           *HobTablePtr;
  HYGON_CPM_TABLE_COMMON_HEADER  *TableHeaderPtr;
  UINT32                         PlatformMask;
  UINT32                         i;

  MainTablePtr = ((HYGON_CPM_TABLE_PPI *)This)->MainTablePtr;
  PlatformMask = 1 << MainTablePtr->CurrentPlatformId;
  TableListPtr = MainTablePtr->TableInHobList.Pointer;
  HobTablePtr  = MainTablePtr->HobTablePtr.Pointer;

  if (TableListPtr) {
    for (i = 0; i < TableListPtr->Number; i++) {
      if (TableListPtr->Item[i].TableId == TableId) {
        TableHeaderPtr = (HYGON_CPM_TABLE_COMMON_HEADER *)TableListPtr->Item[i].SubTable.Pointer;
        if (!TableHeaderPtr->PlatformMask || TableHeaderPtr->PlatformMask & PlatformMask) {
          if (Flag == 1) {
            TableListPtr->Item[i].TableId &= 0x7FFFFFFF;
          } else if (Flag == 2) {
            TableListPtr->Item[i].TableId |= 0x80000000;
          } else if (Flag == 3 || Flag == 4) {
            if (Flag == 4) {
              TableListPtr->Item[i].TableId &= 0x7FFFFFFF;
            }

            if (((UINT64)TableHeaderPtr & 0xFF000000) == 0xFF000000) {
              if (HobTablePtr->BufferItem == 0 || HobTablePtr->BufferSize < TableHeaderPtr->TableSize) {
                CpmPostCode (CpmTpErrorNoEnoughSizeInHobBuffer);
                CPM_DEADLOOP ();
              } else {
                TableListPtr->Item[i].SubTable.Raw = (UINT64)((UINT8 *)HobTablePtr + HobTablePtr->BufferOffset);
                TableListPtr->Item[i].Flag = 1;
                CpmCopyMem (TableListPtr->Item[i].SubTable.Pointer, TableHeaderPtr, TableHeaderPtr->TableSize);
                HobTablePtr->BufferOffset += TableHeaderPtr->TableSize;
                HobTablePtr->BufferSize   -= TableHeaderPtr->TableSize;
                TableListPtr->Size += TableHeaderPtr->TableSize;
              }
            }
          }

          return TableListPtr->Item[i].SubTable.Pointer;
        }
      }
    }
  }

  return NULL;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get the table size.
 *
 * @param[in]     TablePtr    The Pointer of CPM Table
 *
 * @retval        The size of CPM table
 */
UINT16
EFIAPI
CpmGetTableSize (
  IN       VOID    *TablePtr
  )
{
  UINT16  Size;

  Size = 0;
  if (TablePtr) {
    Size = ((HYGON_CPM_TABLE_COMMON_HEADER *)TablePtr)->TableSize;
  }

  return (Size);
}

/*----------------------------------------------------------------------------------------*/

/**
 * Generate CPM Table lists according to which driver to use.
 *
 * @param[in]     TableListPtr        CPM table list pointer from CPM OEM driver
 * @param[in]     PlatformId          Current Platform Id
 * @param[in]     TableInHobListPtr   The Pointer of HYGON CPM Table List for the table in HOB
 *
 */
VOID
EFIAPI
CpmGenerateTableList (
  IN       VOID                *TableListPtr,
  IN       UINT16              PlatformId,
  IN OUT   HYGON_CPM_TABLE_LIST  *TableInHobListPtr
  )
{
  UINT32                         PlatformMask;
  HYGON_CPM_TABLE_COMMON_HEADER  **TablePtr;

  TablePtr = TableListPtr;

  if (PlatformId < 32) {
    PlatformMask = 1 << PlatformId;
  } else {
    PlatformMask = 0xFFFFFFFF;
  }

  TableInHobListPtr->Number = 0;
  TableInHobListPtr->Size   = 0;

  while (*TablePtr != NULL) {
    if (((*TablePtr)->PlatformMask & PlatformMask) || ((*TablePtr)->PlatformMask == 0)) {
      TableInHobListPtr->Item[TableInHobListPtr->Number].SubTable.Raw     = 0;
      TableInHobListPtr->Item[TableInHobListPtr->Number].SubTable.Pointer = *TablePtr;
      TableInHobListPtr->Item[TableInHobListPtr->Number].TableId = (*TablePtr)->TableSignature;
      if (((*TablePtr)->Attribute & (CPM_OVERRIDE | CPM_PEI)) == (CPM_OVERRIDE | CPM_PEI)) {
        TableInHobListPtr->Item[TableInHobListPtr->Number].Flag = 1;
        TableInHobListPtr->Size += CpmGetTableSize (*TablePtr);
      } else {
        TableInHobListPtr->Item[TableInHobListPtr->Number].Flag = 0;
      }

      TableInHobListPtr->Number++;
    }

    TablePtr++;
  }

  if (TableInHobListPtr->Number > HYGON_TABLE_LIST_ITEM_SIZE) {
    CpmPostCode (CpmTpErrorTableNumberOverflow);
    CPM_DEADLOOP ();
  }

  TableInHobListPtr->Header.TableSize = (UINT16)(sizeof (HYGON_CPM_TABLE_LIST) - (HYGON_TABLE_LIST_ITEM_SIZE - TableInHobListPtr->Number - HYGON_TABLE_LIST_ADDITIONAL_ITEM_SIZE) * sizeof (HYGON_CPM_TABLE_ITEM));
  TableInHobListPtr->Size += TableInHobListPtr->Header.TableSize + sizeof (HYGON_CPM_HOB_HEADER);
}

/*----------------------------------------------------------------------------------------*/

/**
 * Copy a memory block between two buffers.
 *
 * @param[in]     destination   The destination buffer address
 * @param[in]     source        The source buffer address
 * @param[in]     size          The memory size to copy
 *
 */
VOID
EFIAPI
CpmCopyMem (
  IN OUT   VOID    *destination,
  IN       VOID    *source,
  IN       UINTN    size
  )
{
  UINTN  i;
  UINT8  *Src;
  UINT8  *Dst;

  if (source != NULL && destination != NULL && size != 0 && source != destination) {
    Src = source;
    Dst = destination;
    for (i = 0; i < size; i++) {
      *Dst = *Src;
      Src++;
      Dst++;
    }
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Copy CPM table and table list to memory area and return the Main Table Pointer if found.
 *
 * @param[in]     TableInHobListPtr   The Pointer of HYGON CPM Table List for the table in HOB
 * @param[in]     HobTablePtr         HOB buffer to  store CPM table
 *
 * @retval        CPM Main Table Pointer if non-zero
 */
VOID *
EFIAPI
CpmCopyTableListToMemory (
  IN       HYGON_CPM_TABLE_LIST  *TableInHobListPtr,
  IN OUT   VOID                *HobTablePtr
  )
{
  UINT16  size;
  UINT16  i;
  VOID    *MainTablePtr;
  VOID    *HobPtr;

  MainTablePtr = NULL;
  HobPtr = (VOID *)((UINT8 *)HobTablePtr + sizeof (HYGON_CPM_HOB_HEADER));
  for (i = 0; i < TableInHobListPtr->Number; i++) {
    if (TableInHobListPtr->Item[i].Flag == 1) {
      if (TableInHobListPtr->Item[i].TableId == CPM_SIGNATURE_MAIN_TABLE) {
        MainTablePtr = HobPtr;
      }

      size = CpmGetTableSize (TableInHobListPtr->Item[i].SubTable.Pointer);
      CpmCopyMem (HobPtr, TableInHobListPtr->Item[i].SubTable.Pointer, size);
      TableInHobListPtr->Item[i].SubTable.Raw = (UINT64)HobPtr;
      HobPtr = (VOID *)((UINT8 *)HobPtr + size);
    }
  }

  CpmCopyMem (HobPtr, TableInHobListPtr, TableInHobListPtr->Header.TableSize);

  ((HYGON_CPM_HOB_HEADER *)HobTablePtr)->HeaderPtr.Raw    = (UINT64)HobTablePtr;
  ((HYGON_CPM_HOB_HEADER *)HobTablePtr)->MainTablePtr.Raw = (UINT64)MainTablePtr;
  ((HYGON_CPM_HOB_HEADER *)HobTablePtr)->BufferOffset     = TableInHobListPtr->Size;
  ((HYGON_CPM_HOB_HEADER *)HobTablePtr)->BufferSize = HYGON_TABLE_ADDITIONAL_BUFFER_SIZE;
  ((HYGON_CPM_HOB_HEADER *)HobTablePtr)->BufferItem = HYGON_TABLE_LIST_ADDITIONAL_ITEM_SIZE;

  if (MainTablePtr) {
    ((HYGON_CPM_MAIN_TABLE *)MainTablePtr)->TableInRomList.Raw = 0;
    ((HYGON_CPM_MAIN_TABLE *)MainTablePtr)->TableInRamList.Raw = 0;
    ((HYGON_CPM_MAIN_TABLE *)MainTablePtr)->TableInHobList.Raw = (UINT64)HobPtr;
    ((HYGON_CPM_MAIN_TABLE *)MainTablePtr)->HobTablePtr.Raw    = (UINT64)HobTablePtr;
  }

  return MainTablePtr;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Recalculate the size of CPM table.
 *
 * @param[in]     HobTablePtr         HOB buffer to store CPM table
 *
 * @retval        Table List Pointer if non-zero
 */
HYGON_CPM_TABLE_LIST *
EFIAPI
CpmAdjustTableList (
  IN       VOID                *HobTablePtr
  )
{
  HYGON_CPM_TABLE_LIST           *TableListPtr;
  HYGON_CPM_MAIN_TABLE           *MainTablePtr;
  HYGON_CPM_HOB_HEADER           *HobHeaderPtr;
  UINT16                         Size;
  UINT16                         TableNumber;
  UINT16                         i;
  HYGON_CPM_TABLE_COMMON_HEADER  *TableHeaderPtr;

  Size = 0;
  TableNumber  = 0;
  HobHeaderPtr = (HYGON_CPM_HOB_HEADER *)HobTablePtr;
  MainTablePtr = (HYGON_CPM_MAIN_TABLE *)(HobHeaderPtr->MainTablePtr.Pointer);
  TableListPtr = (HYGON_CPM_TABLE_LIST *)(MainTablePtr->TableInHobList.Pointer);

  for (i = 0; i < TableListPtr->Number; i++) {
    TableHeaderPtr = (HYGON_CPM_TABLE_COMMON_HEADER *)TableListPtr->Item[i].SubTable.Pointer;
    if ((TableHeaderPtr->PlatformMask & (1 << MainTablePtr->CurrentPlatformId)) || (TableHeaderPtr->PlatformMask == 0)) {
      TableListPtr->Item[TableNumber].SubTable.Raw = TableListPtr->Item[i].SubTable.Raw;
      TableListPtr->Item[TableNumber].TableId = TableListPtr->Item[i].TableId;
      TableListPtr->Item[TableNumber].Flag    = 1;
      Size = Size + CpmGetTableSize (TableListPtr->Item[TableNumber].SubTable.Pointer);
      TableNumber++;
    }
  }

  if (TableNumber > HYGON_TABLE_LIST_ITEM_SIZE) {
    CpmPostCode (CpmTpErrorTableNumberOverflow);
    CPM_DEADLOOP ();
  }

  TableListPtr->Number = TableNumber;
  TableListPtr->Header.TableSize = (UINT16)(sizeof (HYGON_CPM_TABLE_LIST) - (HYGON_TABLE_LIST_ITEM_SIZE - TableListPtr->Number - HYGON_TABLE_LIST_ADDITIONAL_ITEM_SIZE) * sizeof (HYGON_CPM_TABLE_ITEM));
  TableListPtr->Size = TableListPtr->Header.TableSize + sizeof (HYGON_CPM_HOB_HEADER) + Size;
  return TableListPtr;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Adjust the pointer in CPM tables after it is copied to new area.
 *
 * @param[in]     TablePtr    The Pointer of buffer to store CPM table lists and CPM Table
 *
 */
VOID
EFIAPI
CpmRelocateTableList (
  IN OUT   VOID        *TablePtr
  )
{
  UINT64                Relocate;
  UINT8                 i;
  HYGON_CPM_MAIN_TABLE  *MainTablePtr;
  HYGON_CPM_HOB_HEADER  *HobHeaderPtr;
  HYGON_CPM_TABLE_LIST  *TableListPtr;

  HobHeaderPtr = (HYGON_CPM_HOB_HEADER *)TablePtr;
  Relocate     = (UINT8 *)HobHeaderPtr - (UINT8 *)HobHeaderPtr->HeaderPtr.Pointer;
  MainTablePtr = (HYGON_CPM_MAIN_TABLE *)((UINT8 *)HobHeaderPtr->MainTablePtr.Pointer + Relocate);
  TableListPtr = (HYGON_CPM_TABLE_LIST *)((UINT8 *)MainTablePtr->TableInHobList.Pointer + Relocate);

  for (i = 0; i < TableListPtr->Number; i++) {
    if (TableListPtr->Item[i].Flag == 1) {
      TableListPtr->Item[i].SubTable.Pointer = (UINT8 *)TableListPtr->Item[i].SubTable.Pointer + Relocate;
    }
  }

  HobHeaderPtr->HeaderPtr.Pointer      = HobHeaderPtr;
  HobHeaderPtr->MainTablePtr.Pointer   = MainTablePtr;
  MainTablePtr->TableInHobList.Pointer = TableListPtr;
  MainTablePtr->HobTablePtr.Pointer    = TablePtr;
}

/*----------------------------------------------------------------------------------------*/

/**
 * If there is no CPM table with the same signature to exist, add a new table. Otherwise,
 * use the new table to replace the orignal one.
 *
 * @param[in]     This        The Pointer of CPM Table Ppi or Protocol
 * @param[in]     TablePtr    CPM Table to add
 *
 * @retval        Original CPM Table with same signature
 */
VOID *
EFIAPI
CpmAddTable (
  IN       VOID                *This,
  IN       VOID                *TablePtr
  )
{
  HYGON_CPM_MAIN_TABLE           *MainTablePtr;
  HYGON_CPM_HOB_HEADER           *HobTablePtr;
  HYGON_CPM_TABLE_LIST           *TableInHobListPtr;
  HYGON_CPM_TABLE_COMMON_HEADER  *NewTable;
  HYGON_CPM_TABLE_COMMON_HEADER  *OldTable;

  MainTablePtr = ((HYGON_CPM_TABLE_PPI *)This)->MainTablePtr;
  HobTablePtr  = (HYGON_CPM_HOB_HEADER *)MainTablePtr->HobTablePtr.Pointer;
  TableInHobListPtr = (HYGON_CPM_TABLE_LIST *)MainTablePtr->TableInHobList.Pointer;
  NewTable = TablePtr;

  OldTable = CpmSearchAndUpdateTable (This, NewTable->TableSignature, 3);

  if (OldTable) {
    CpmCopyMem (OldTable, NewTable, OldTable->TableSize);
  } else {
    OldTable = CpmSearchAndUpdateTable (This, NewTable->TableSignature | 0x80000000, 4);
    if (OldTable) {
      CpmCopyMem (OldTable, NewTable, OldTable->TableSize);
    } else {
      if (HobTablePtr->BufferItem == 0 || HobTablePtr->BufferSize < NewTable->TableSize) {
        CpmPostCode (CpmTpErrorNoEnoughSizeInHobBuffer);
        CPM_DEADLOOP ();
      } else {
        HobTablePtr->BufferItem--;
        OldTable = (HYGON_CPM_TABLE_COMMON_HEADER *)((UINT8 *)HobTablePtr + HobTablePtr->BufferOffset);
        CpmCopyMem (OldTable, NewTable, NewTable->TableSize);
        HobTablePtr->BufferOffset += NewTable->TableSize;
        HobTablePtr->BufferSize   -= NewTable->TableSize;
        TableInHobListPtr->Item[TableInHobListPtr->Number].TableId = NewTable->TableSignature;
        TableInHobListPtr->Item[TableInHobListPtr->Number].Flag    = 1;
        TableInHobListPtr->Item[TableInHobListPtr->Number].SubTable.Raw = (UINT64)OldTable;
        TableInHobListPtr->Number++;
        TableInHobListPtr->Size += NewTable->TableSize;
      }
    }
  }

  return OldTable;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Disable CPM table with the same signature to be found.
 *
 * @param[in]     This        The Pointer of CPM Table Ppi or Protocol
 * @param[in]     TablePtr    CPM Table to add
 *
 * @retval        Original CPM Table with same signature
 */
VOID *
EFIAPI
CpmRemoveTable (
  IN       VOID                *This,
  IN       VOID                *TablePtr
  )
{
  HYGON_CPM_TABLE_COMMON_HEADER  *OldTable;

  OldTable = TablePtr;

  return CpmSearchAndUpdateTable (This, OldTable->TableSignature, 2);
}
