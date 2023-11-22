/** @file
  Helper functions to parse HID report descriptor and items.

Copyright (c) 2004 - 2010, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "UsbMouseAbsolutePointer.h"

/**
  Get next HID item from report descriptor.

  This function retrieves next HID item from report descriptor, according to
  the start position.
  According to USB HID Specification, An item is piece of information
  about the device. All items have a one-byte prefix that contains
  the item tag, item type, and item size.
  There are two basic types of items: short items and long items.
  If the item is a short item, its optional data size may be 0, 1, 2, or 4 bytes.
  Only short item is supported here.

  @param  StartPos          Start position of the HID item to get.
  @param  EndPos            End position of the range to get the next HID item.
  @param  HidItem           Buffer for the HID Item to return.

  @return Pointer to end of the HID item returned.
          NULL if no HID item retrieved.

**/
UINT8 *
GetNextHidItem (
  IN  UINT8     *StartPos,
  IN  UINT8     *EndPos,
  OUT HID_ITEM  *HidItem
  )
{
  UINT8  Temp;

  if (EndPos <= StartPos) {
    return NULL;
  }

  Temp = *StartPos;
  StartPos++;

  //
  // Bit format of prefix byte:
  // Bits 0-1: Size
  // Bits 2-3: Type
  // Bits 4-7: Tag
  //
  HidItem->Type = BitFieldRead8 (Temp, 2, 3);
  HidItem->Tag  = BitFieldRead8 (Temp, 4, 7);

  if (HidItem->Tag == HID_ITEM_TAG_LONG) {
    //
    // Long Items are not supported, although we try to parse it.
    //
    HidItem->Format = HID_ITEM_FORMAT_LONG;

    if ((EndPos - StartPos) >= 2) {
      HidItem->Size = *StartPos++;
      HidItem->Tag  = *StartPos++;

      if ((EndPos - StartPos) >= HidItem->Size) {
        HidItem->Data.LongData = StartPos;
        StartPos              += HidItem->Size;
        return StartPos;
      }
    }
  } else {
    HidItem->Format = HID_ITEM_FORMAT_SHORT;
    HidItem->Size   = BitFieldRead8 (Temp, 0, 1);

    switch (HidItem->Size) {
      case 0:
        //
        // No data
        //
        return StartPos;

      case 1:
        //
        // 1-byte data
        //
        if ((EndPos - StartPos) >= 1) {
          HidItem->Data.Uint8 = *StartPos++;
          return StartPos;
        }

      case 2:
        //
        // 2-byte data
        //
        if ((EndPos - StartPos) >= 2) {
          CopyMem (&HidItem->Data.Uint16, StartPos, sizeof (UINT16));
          StartPos += 2;
          return StartPos;
        }

      case 3:
        //
        // 4-byte data, adjust size
        //
        HidItem->Size = 4;
        if ((EndPos - StartPos) >= 4) {
          CopyMem (&HidItem->Data.Uint32, StartPos, sizeof (UINT32));
          StartPos += 4;
          return StartPos;
        }
    }
  }

  return NULL;
}

UINT32
HidBitFiledReadU32 (
  IN  VOID   *Buf,
  IN  UINTN  BufBits,
  IN  UINTN  Offset,
  IN  UINTN  Size
  )
{
  UINT8   *HidUint8;
  UINT32  Operand = 0;
  UINTN   StartBit;
  UINTN   EndBit;

  ASSERT ((Size <= 32) && (Size > 2));
  ASSERT ((Offset + Size) <= BufBits);
  HidUint8 = (UINT8 *)Buf;

  StartBit = Offset % 8;
  EndBit   = StartBit + Size - 1;

  if (StartBit) {
    UINT64  HidUint64 = (*((UINT64 *)(HidUint8 + (Offset / 8))));
    HidUint64 = BitFieldRead64 (HidUint64, StartBit, EndBit);
    Operand   = (UINT32)HidUint64;
  } else {
    Operand = *((UINT32 *)(HidUint8 + (Offset / 8)));
    Operand = BitFieldRead32 (Operand, StartBit, EndBit);
  }

  return Operand;
}

INT32
HidBitFiledReadS32 (
  IN  VOID   *Buf,
  IN  UINTN  BufBits,
  IN  UINTN  Offset,
  IN  UINTN  Size
  )
{
  UINT32  Operand = 0;
  UINTN   Sign;

  Operand = HidBitFiledReadU32 (Buf, BufBits, Offset, Size);
  Sign    = (Operand & ((UINT32)1 << (Size - 1)));
  if (Sign) {
    return (INT32)((~Operand + 1) & (~((UINT32)-2 << (Size - 1)))) * (-1);
  } else {
    return (INT32)Operand;
  }
}

/**
  Free all report and registered fields. The Field->usage is allocated behind
  the field, so we need only to Freepool(field) itself.

  @param  MouseDev   Usb Mouse Device instance that contain reports to be closed

**/
VOID
HidFreeReport (
  IN  HID_MOUSE_DEV  *MouseDev
  )
{
  UINTN            Index;
  HID_REPORT_ENUM  *ReportEnum;
  HID_REPORT       *Report;
  UINTN            FieldIdx;

  ReportEnum = &(MouseDev->InputReport);

  if (ReportEnum == NULL) {
    return;
  }

  for (Index = 0; Index < HID_REPORT_ID_MAX; Index++) {
    Report = ReportEnum->ReportIdMap[Index];
    if (Report != NULL) {
      FieldIdx = 0;
      while (FieldIdx < Report->FieldMax) {
        FreePool (Report->Field[FieldIdx++]);
      }

      FreePool (Report);
    }
  }

  ZeroMem ((VOID *)ReportEnum, sizeof (HID_REPORT_ENUM));
  InitializeListHead (&ReportEnum->ReportList);

  MouseDev->ApplicationMax = 0;
}

/**
  Begin a collection. The type/usage is pushed on the stack.

  @param  Parser      Pointer to Usb mouse HID report parser
  @param  Type        Collection Type

  @retval EFI_SUCCESS           The collection is successfully opened and defined
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate pool

**/
EFI_STATUS
BeginCollection (
  IN  HID_PARSER  *Parser,
  IN  UINT32      Type
  )
{
  HID_COLLECTION  *Collection;
  UINT32          Usage;

  Usage = Parser->Local.Usage[0];
  if (Parser->CollectionStackTop == HID_COLLECTION_STACK_SIZE) {
    DEBUG ((DEBUG_ERROR, "Collection stack overflow!\n"));
    return EFI_ABORTED;
  }

  Collection        = &Parser->CollectionStack[Parser->CollectionStackTop];
  Collection->Type  = Type;
  Collection->Usage = Usage;
  Collection->Tier  = Parser->CollectionStackTop++;

  if (Type == HID_COLLECTION_APPLICATION) {
    Parser->MouseDev->ApplicationMax++;
  }

  return EFI_SUCCESS;
}

/**
  End a collection.

  @param  Parser  Pointer to Usb mouse HID report parser

**/
EFI_STATUS
EndCollection (
  IN  HID_PARSER  *Parser
  )
{
  if (Parser->CollectionStackTop == 0) {
    DEBUG ((DEBUG_ERROR, "Collection stack underflow\n"));
    return EFI_ABORTED;
  }

  Parser->CollectionStackTop--;
  return EFI_SUCCESS;
}

/**
  Get unsign-data from HID item.

  This function retrieves unsign-data from HID item.
  It only supports short items, which has 4 types of data:
  0, 1, 2, or 4 bytes.

  @param  HidItem       Pointer to the HID item.

  @return The unsign-data of HID item.

**/
UINT32
ItemUnsignData (
  IN  HID_ITEM  *HidItem
  )
{
  //
  // Get unsign-data from HID item.
  //
  switch (HidItem->Size) {
    case 1:
      return HidItem->Data.Uint8;
    case 2:
      return HidItem->Data.Uint16;
    case 4:
      return HidItem->Data.Uint32;
  }

  return 0;
}

/**
  Get sign-data from HID item.

  This function retrieves sign-data from HID item.
  It only supports short items, which has 4 types of data:
  0, 1, 2, or 4 bytes.

  @param  HidItem       Pointer to the HID item.

  @return The sign-data of HID item.

**/
INT32
ItemSignData (
  IN  HID_ITEM  *HidItem
  )
{
  //
  // Get sign-data from HID item.
  //
  switch (HidItem->Size) {
    case 1:
      return HidItem->Data.Int8;
    case 2:
      return HidItem->Data.Int16;
    case 4:
      return HidItem->Data.Int32;
  }

  return 0;
}

/**
  Create a new field for this report.

  @param  Parser      Pointer to Usb mouse HID report parser

  @return The field has just been create

**/
HID_FIELD *
HidCreateField (
  IN HID_PARSER  *Parser
  )
{
  HID_FIELD       *Field;
  UINT32          Usages;
  UINT32          Index;
  HID_COLLECTION  *Collection;

  Usages = MAX (Parser->Local.UsageIndex, Parser->Global.ReportCount);
  Field  = AllocateZeroPool (
             (sizeof (HID_FIELD) +
              Usages * sizeof (HID_USAGE))
             );
  if (Field == NULL) {
    return NULL;
  }

  Field->Usage       = (HID_USAGE *)(Field + 1);
  Field->UsageMax    = Usages;
  Field->ReportSize  = Parser->Global.ReportSize;
  Field->ReportCount = Parser->Global.ReportCount;
  Field->LogicalMin  = Parser->Global.LogicalMin;
  Field->LogicalMax  = Parser->Global.LogicalMax;

  for (Index = Parser->CollectionStackTop; Index > 0; Index--) {
    Collection = &Parser->CollectionStack[Index - 1];
    switch (Collection->Type) {
      case HID_COLLECTION_APPLICATION:
        Field->Application = Collection->Usage;
        break;

      case HID_COLLECTION_LOGICAL:
        Field->Logical = Collection->Usage;
        break;

      case HID_COLLECTION_PHYSICAL:
        Field->Physical = Collection->Usage;
        break;

      default:
        break;
    }
  }

  for (Index = 0; Index < Usages; Index++) {
    UINT32  Temp = Index;

    /*
     * Duplicate the last usage we parsed if we have excess values
     */
    if (Index >= Parser->Local.UsageIndex) {
      Temp = Parser->Local.UsageIndex - 1;
    }

    Field->Usage[Index].HidUsage   = Parser->Local.Usage[Temp];
    Field->Usage[Index].UsageIndex = Index;
  }

  return Field;
}

/**
  Add a new field to corresponding report.

  @param  Parser      Pointer to Usb mouse HID report parser
  @param  ReportType  Report type (Input, Output, Feature)
  @param  Flag        Field data flag

  @retval EFI_SUCCESS  The field is successfully added
  @retval EFI_ABORTED  An error occurred

**/
EFI_STATUS
AddFieldToReport (
  IN HID_PARSER  *Parser,
  IN UINT16      ReportType,
  IN UINT32      Flags
  )
{
  HID_REPORT_ENUM  *ReportEnum;
  HID_REPORT       *Report;
  HID_FIELD        *Field;
  UINT32           ReportId;
  UINT32           Offset;

  if (Parser == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ReportId = Parser->Global.ReportId;
  if (ReportId >= HID_REPORT_ID_MAX) {
    DEBUG ((DEBUG_INFO, "%a[%d]: Id overflow\n", __FUNCTION__, __LINE__));
    return EFI_UNSUPPORTED;
  }

  ReportEnum = &(Parser->MouseDev->InputReport);
  Report     = ReportEnum->ReportIdMap[ReportId];
  if (Report == NULL) {
    Report = AllocateZeroPool (sizeof (HID_REPORT));
    if (Report == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    ReportEnum->PrefixId              = (ReportId != 0) ? TRUE : ReportEnum->PrefixId;
    Report->Id                        = ReportId;
    Report->Type                      = ReportType;
    Report->BitSize                   = 0;
    Report->FieldMax                  = 0;
    Report->Application               = (Parser->CollectionStackTop == 0) ? 0 : Parser->CollectionStack[0].Usage;
    ReportEnum->ReportIdMap[ReportId] = Report;
    InsertTailList (&ReportEnum->ReportList, &Report->List);
  }

  if (Report->FieldMax == HID_REPORT_FIELDS_MAX) {
    DEBUG ((DEBUG_INFO, "Too many fields in report\n"));
    return EFI_UNSUPPORTED;
  }

  Offset           = Report->BitSize;
  Report->BitSize += Parser->Global.ReportSize * Parser->Global.ReportCount;
  if (Report->BitSize > (HID_MAX_BUFFER_SIZE - 1) << 3) {
    DEBUG ((DEBUG_INFO, "Report is too long\n"));
    return EFI_UNSUPPORTED;
  }

  if (!Parser->Local.UsageIndex) {
    //
    // Skip padding fields
    //
    DEBUG ((DEBUG_INFO, "%a: Skip padding fields!\n", __FUNCTION__));
    return EFI_SUCCESS;
  }

  Field = HidCreateField (Parser);
  if (Field == NULL) {
    return EFI_ABORTED;
  }

  Field->BitOffset            = Offset;
  Field->Flags                = Flags;
  Field->ReportType           = ReportType;
  Field->Index                = Report->FieldMax++;
  Report->Field[Field->Index] = Field;
  DEBUG ((DEBUG_INFO, "Field->BitOffset%d\n", Field->BitOffset));
  DEBUG ((DEBUG_INFO, "Field->Index%d\n", Field->Index));

  return EFI_SUCCESS;
}

/**
   a usage join the temporary parser queue.

  @param  Parser    Pointer to Usb mouse HID report parser
  @param  Usage     Item Usage
  @param  Size      Item Size

  @retval EFI_SUCCESS  The usage is successfully added
  @retval EFI_ABORTED  Usage index exceeded

**/
EFI_STATUS
UsageJoinQueue (
  IN  HID_PARSER  *Parser,
  IN  UINT32      Usage,
  IN  UINT8       Size
  )
{
  UINT32  IntactUsage;

  if (Parser->Local.UsageIndex >= HID_MAX_USAGES) {
    DEBUG ((DEBUG_ERROR, "UsageIndex overflow\n"));
    return EFI_ABORTED;
  }

  IntactUsage                                   = Size > 2 ? Usage : USAGE_ID_PREFIX_PAGE (Usage, Parser->Global.UsagePage);
  Parser->Local.Usage[Parser->Local.UsageIndex] = IntactUsage;

  Parser->Local.UsageSize[Parser->Local.UsageIndex] = Size;
  Parser->Local.UsageIndex++;
  return EFI_SUCCESS;
}

/**
  Process a global item.

  @param  Parser      Pointer to Usb mouse HID report parser
  @param  Item        The global item that will be parsed

  @retval EFI_SUCCESS     Parsed successfully
  @retval EFI_ABORTED     An error occurred in this parsing
  @retval EFI_UNSUPPORTED Unknown global tag

**/
EFI_STATUS
HidGlobalParser (
  IN  HID_PARSER  *Parser,
  IN  HID_ITEM    *Item
  )
{
  UINT32  DataUint32;
  INT32   DataInt32;

  DataUint32 = ItemUnsignData (Item);
  DataInt32  = ItemSignData (Item);
  switch (Item->Tag) {
    case HID_GLOBAL_ITEM_TAG_USAGE_PAGE:
      Parser->Global.UsagePage = DataUint32;
      break;

    case HID_GLOBAL_ITEM_TAG_PUSH:
      if (Parser->GlobalStackTop == HID_GLOBAL_STACK_SIZE) {
        DEBUG ((DEBUG_ERROR, "Stack overflow\n"));
        return EFI_ABORTED;
      }

      CopyMem (
        (VOID *)(Parser->GlobalStack + Parser->GlobalStackTop),
        (VOID *)(&Parser->Global),
        sizeof (HID_GLOBAL)
        );
      Parser->GlobalStackTop++;
      break;

    case HID_GLOBAL_ITEM_TAG_POP:
      if (Parser->GlobalStackTop == 0) {
        DEBUG ((DEBUG_ERROR, "Stack underflow\n"));
        return EFI_ABORTED;
      }

      Parser->GlobalStackTop--;
      CopyMem (
        (VOID *)(&Parser->Global),
        (VOID *)(Parser->GlobalStack + Parser->GlobalStackTop),
        sizeof (HID_GLOBAL)
        );
      break;

    case HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM:
      Parser->Global.LogicalMin = DataInt32;
      break;

    case HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM:
      if (DataInt32 < Parser->Global.LogicalMin) {
        DEBUG ((DEBUG_ERROR, "Invalid logical maxinum\n"));
        return EFI_ABORTED;
      }

      Parser->Global.LogicalMax = DataInt32;
      break;

    case HID_GLOBAL_ITEM_TAG_REPORT_SIZE:
      Parser->Global.ReportSize = DataUint32;
      break;

    case HID_GLOBAL_ITEM_TAG_REPORT_COUNT:
      if (DataUint32 > HID_MAX_USAGES) {
        DEBUG ((DEBUG_ERROR, "Invalid report count %d\n", Parser->Global.ReportCount));
        return EFI_ABORTED;
      }

      Parser->Global.ReportCount = DataUint32;
      break;

    case HID_GLOBAL_ITEM_TAG_REPORT_ID:
      if ((DataUint32 == 0) || (DataUint32 >= HID_REPORT_ID_MAX)) {
        DEBUG ((DEBUG_ERROR, "Invalid ReportID %d\n", Parser->Global.ReportId));
        return EFI_ABORTED;
      }

      Parser->Global.ReportId = DataUint32;
      break;

    default:
      DEBUG ((DEBUG_WARN, "Alternative-ignored/Unknown global tag 0x%x\n", Item->Tag));
      return EFI_SUCCESS;
  }

  return EFI_SUCCESS;
}

/**
  Process a local item.

  @param  Parser      Pointer to Usb mouse HID report parser
  @param  Item        The local item that will be parsed

  @retval EFI_SUCCESS     Parsed successfully
  @retval EFI_ABORTED     An error occurred in this parsing

**/
EFI_STATUS
HidLocalParser (
  IN  HID_PARSER  *Parser,
  IN  HID_ITEM    *Item
  )
{
  UINT32  Data;
  UINT32  Index;
  UINT32  Count;

  Data = ItemUnsignData (Item);
  switch (Item->Tag) {
    case HID_LOCAL_ITEM_TAG_DELIMITER:
      DEBUG ((DEBUG_INFO, "Unsupport delimiter item\n"));
      return EFI_UNSUPPORTED;

    case HID_LOCAL_ITEM_TAG_USAGE:
      return UsageJoinQueue (Parser, Data, Item->Size);

    case HID_LOCAL_ITEM_TAG_USAGE_MINIMUM:
      Parser->Local.UsageMin = Data;
      return EFI_SUCCESS;

    case HID_LOCAL_ITEM_TAG_USAGE_MAXIMUM:
      Count = Data - Parser->Local.UsageMin;
      if (Count + Parser->Local.UsageIndex > HID_MAX_USAGES) {
        DEBUG ((DEBUG_WARN, "UsageIndex overflow\n"));
        return EFI_UNSUPPORTED;
      }

      for (Index = Parser->Local.UsageMin; Index <= Data; Index++) {
        if (EFI_ERROR (UsageJoinQueue (Parser, Index, Item->Size))) {
          DEBUG ((DEBUG_INFO, "Join failed\n"));
          return EFI_ABORTED;
        }
      }

      return EFI_SUCCESS;

    default:
      DEBUG ((DEBUG_INFO, "unknown local item tag 0x%x\n", Item->Tag));
      return EFI_SUCCESS;
  }
}

/**
  Process a main item.

  The absolute pointer only needs to process data from the usb mouse,
  so the parser selectively ignores the output, featere main-item

  @param  Parser      Pointer to Usb mouse HID report parser
  @param  Item        The main item that will be parsed

  @retval EFI_SUCCESS     Parsed successfully
  @retval EFI_ABORTED     An error occurred in this parsing

**/
EFI_STATUS
HidMainParser (
  IN  HID_PARSER  *Parser,
  IN  HID_ITEM    *Item
  )
{
  UINT32      Data;
  EFI_STATUS  Status = EFI_SUCCESS;

  Data = ItemUnsignData (Item);
  switch (Item->Tag) {
    case HID_MAIN_ITEM_TAG_BEGIN_COLLECTION:
      Status = BeginCollection (Parser, Data & 0xff);
      break;

    case HID_MAIN_ITEM_TAG_END_COLLECTION:
      Status = EndCollection (Parser);
      break;

    case HID_MAIN_ITEM_TAG_INPUT:
      Status = AddFieldToReport (Parser, HID_INPUT_REPORT, Data);
      break;

    case HID_MAIN_ITEM_TAG_OUTPUT:
    case HID_MAIN_ITEM_TAG_FEATURE:
      DEBUG ((DEBUG_INFO, "alternative Item ignored 0x%x\n", Item->Tag));
      break;

    default:
      DEBUG ((DEBUG_WARN, "unknown main item tag 0x%x\n", Item->Tag));
      break;
  }

  /*
   * Empty the pre-local
   */
  ZeroMem ((VOID *)(&Parser->Local), sizeof (HID_LOCAL));

  return Status;
}

/**
  Process a reserved item.

**/
EFI_STATUS
HidReservedParser (
  IN  HID_PARSER  *Parser,
  IN  HID_ITEM    *Item
  )
{
  DEBUG ((DEBUG_INFO, "Reserved item[tag 0x%x] type, \n", Item->Tag));
  return EFI_SUCCESS;
}

/**
  Check the report and update some mouse device mode values

  This function mainly checks the main button, x, y, and scroll wheel of the mouse.
  Support x, y, and wheel data below 32 bits report.
  If the data corresponding to the field usage is absolute, the absolute pointer
  device mode's LogicalMax/Min is updated with the LogicalMax/Min value in the field.

  @param  Parser             Pointer to Usb mouse HID report parser

  @retval EFI_SUCCESS        This report is supported and update some mode information
  @retval EFI_UNSUPPORTED    The report is not supported

**/
EFI_STATUS
ReportCheckAndModeAdjust (
  IN  HID_PARSER  *Parser
  )
{
  HID_REPORT_ENUM  *InReportEnum;
  HID_MOUSE_DEV    *MouseDev;
  HID_REPORT       *Report;
  HID_FIELD        *HidField;
  HID_USAGE        *Usage;
  UINTN            Index;
  UINTN            ReportId = 0;
  UINTN            UsageRef;

  MouseDev     = Parser->MouseDev;
  InReportEnum = &(MouseDev->InputReport);

  while (ReportId < HID_REPORT_ID_MAX) {
    Report = InReportEnum->ReportIdMap[ReportId++];
    if (Report) {
      for (Index = 0; Index < Report->FieldMax; Index++) {
        HidField = Report->Field[Index];
        for (UsageRef = 0; UsageRef < HidField->UsageMax; UsageRef++) {
          Usage = HidField->Usage + UsageRef;
          switch (Usage->HidUsage) {
            case HID_USAGE_BUTTON_1:
              if (HidField->ReportSize > 1) {
                return EFI_UNSUPPORTED;
              }

              break;

            case HID_USAGE_AXIS_X:
              if ((HidField->Flags & HID_DATA_RELATIVE_FLAG) == 0) {
                //
                // The data is absolute
                //
                MouseDev->Mode.AbsoluteMaxX = (INT64)HidField->LogicalMax;
                MouseDev->Mode.AbsoluteMinX = (INT64)HidField->LogicalMin;
              }

              if (HidField->ReportSize > 32) {
                return EFI_UNSUPPORTED;
              }

              break;

            case HID_USAGE_AXIS_Y:
              if ((HidField->Flags & HID_DATA_RELATIVE_FLAG) == 0) {
                //
                // The data is absolute
                //
                MouseDev->Mode.AbsoluteMaxY = (INT64)HidField->LogicalMax;
                MouseDev->Mode.AbsoluteMinY = (INT64)HidField->LogicalMin;
              }

              if (HidField->ReportSize > 32) {
                return EFI_UNSUPPORTED;
              }

              break;

            case HID_USAGE_MOUSE_WHEEL:
              if ((HidField->Flags & HID_DATA_RELATIVE_FLAG) == 0) {
                //
                // The data is absolute
                //
                MouseDev->Mode.AbsoluteMaxZ = (INT64)HidField->LogicalMax;
                MouseDev->Mode.AbsoluteMinZ = (INT64)HidField->LogicalMin;
              }

              if (HidField->ReportSize > 32) {
                return EFI_UNSUPPORTED;
              }

              break;

            default:
              continue;
              break;
          }

          if (HidField->LogicalMax <= HidField->LogicalMin) {
            DEBUG ((DEBUG_INFO, "%a:Illegal maximum and minimum\n", __FUNCTION__));
            return EFI_UNSUPPORTED;
          }
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Parse Mouse Report Descriptor.

  According to USB HID Specification, report descriptors are
  composed of pieces of information. Each piece of information
  is called an Item. This function retrieves each item from
  the report descriptor and updates USB_MOUSE_ABSOLUTE_POINTER_DEV.

  There are three item types: Main, Global, and Local.
  This function parses these types of HID items according
  to tag info.

  @param  MouseDev                 The instance of USB_MOUSE_ABSOLUTE_POINTER_DEV
  @param  ReportDescriptor         Report descriptor to parse
  @param  ReportSize               Report descriptor size

  @retval EFI_SUCCESS              Report descriptor successfully parsed.
  @retval EFI_UNSUPPORTED          Report descriptor contains long item.

**/
EFI_STATUS
HidParseReportDesc (
  IN  HID_MOUSE_DEV  *MouseDev,
  IN  UINT8          *ReportDescriptor,
  IN  UINTN          ReportSize
  )
{
  HID_PARSER  *Parser;
  HID_ITEM    Item;
  UINT8       *DescEnd;
  UINT8       *DescStart;
  UINT8       *Next;
  EFI_STATUS  Status;

  Parser = AllocateZeroPool (sizeof (HID_PARSER));
  if (Parser == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Parser->MouseDev = MouseDev;
  DescEnd          = ReportDescriptor + ReportSize;
  DescStart        = ReportDescriptor;

  while ((Next = GetNextHidItem (DescStart, DescEnd, &Item)) != NULL) {
    DescStart = Next;
    if (Item.Format != HID_ITEM_FORMAT_SHORT) {
      DEBUG ((DEBUG_WARN, "Unsupport long item!\n"));
      goto Done;
    }

    switch (Item.Type) {
      case HID_ITEM_TYPE_MAIN:
        Status = HidMainParser (Parser, &Item);
        break;

      case HID_ITEM_TYPE_GLOBAL:
        Status = HidGlobalParser (Parser, &Item);
        break;

      case HID_ITEM_TYPE_LOCAL:
        Status = HidLocalParser (Parser, &Item);
        break;

      default:
        Status = HidReservedParser (Parser, &Item);
        break;
    }

    if (EFI_ERROR (Status)) {
      DEBUG ((
        DEBUG_ERROR,
        "Item %u %u %u %u parsing failed\n",
        Item.Format,
        Item.Size,
        Item.Type,
        Item.Tag
        ));
      goto Done;
    }
  }

  if (DescStart == DescEnd) {
    if ((Parser->CollectionStackTop)) {
      DEBUG ((DEBUG_WARN, "CollectionStack error\n"));
      goto Done;
    }

    Status = ReportCheckAndModeAdjust (Parser);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "%a[%d]: Unsupport hid device\n", __FUNCTION__, __LINE__));
      goto Done;
    }

    FreePool (Parser);
    return EFI_SUCCESS;
  }

Done:

  FreePool (Parser);
  HidFreeReport (Parser->MouseDev);

  return EFI_UNSUPPORTED;
}

/**
  Parse USB mouse input data (i.e., report) and update the abs pointer status.

  @param  MouseDev        The instance of USB_MOUSE_ABSOLUTE_POINTER_DEV
  @param  Data            Input data(Report) from the mouse
  @param  DataLength      Indicates the size of the data buffer in bytes.

  @retval EFI_SUCCESS      Asynchronous interrupt transfer is handled successfully.
  @retval EFI_DEVICE_ERROR Hardware error occurs.

**/
EFI_STATUS
MouseHidReportInputPaser (
  IN  HID_MOUSE_DEV  *MouseDev,
  IN  VOID           *Data,
  IN  UINTN          DataLength
  )
{
  HID_REPORT_ENUM             *InReportEnum;
  UINT8                       ReportId;
  UINT8                       *Input;
  HID_REPORT                  *Report;
  HID_FIELD                   *HidField;
  HID_USAGE                   *Usage;
  UINTN                       Index;
  UINTN                       UsageRef;
  INT32                       HidInt32;
  UINTN                       Offset;
  UINTN                       Bits;
  UINTN                       InputBits;
  EFI_ABSOLUTE_POINTER_STATE  *AbsState;

  AbsState     = &MouseDev->State;
  InReportEnum = &(MouseDev->InputReport);
  Input        = (UINT8 *)Data;
  if (InReportEnum->PrefixId) {
    ReportId = Input[0];
    Input++;
    InputBits = (DataLength - 1) * 8;
  } else {
    ReportId  = 0;
    InputBits = DataLength * 8;
  }

  Report = InReportEnum->ReportIdMap[ReportId];
  if ((Report != NULL) && (Report->BitSize <= InputBits)) {
    for (Index = 0; Index < Report->FieldMax; Index++) {
      HidField = Report->Field[Index];
      for (UsageRef = 0; UsageRef < HidField->UsageMax; UsageRef++) {
        Usage  = HidField->Usage + UsageRef;
        Offset = HidField->BitOffset + (Usage->UsageIndex) * HidField->ReportSize;
        Bits   = HidField->ReportSize;
        switch (Usage->HidUsage) {
          case HID_USAGE_BUTTON_1:
            Bits                    = MIN ((HidField->UsageMax - Usage->UsageIndex), 32);
            AbsState->ActiveButtons = HidBitFiledReadU32 (
                                        (VOID *)Input,
                                        InputBits,
                                        Offset,
                                        Bits
                                        );
            UsageRef = HidField->UsageMax - 1;
            break;

          case HID_USAGE_AXIS_X:
            HidInt32 = HidBitFiledReadS32 (
                         (VOID *)Input,
                         InputBits,
                         Offset,
                         Bits
                         );
            if (HidField->Flags & HID_DATA_RELATIVE_FLAG) {
              AbsState->CurrentX = MIN (
                                     MAX (
                                       (INT64)(AbsState->CurrentX) + HidInt32,
                                       (INT64)(MouseDev->Mode.AbsoluteMinX)
                                       ),
                                     (INT64)(MouseDev->Mode.AbsoluteMaxX)
                                     );
            } else {
              AbsState->CurrentX = HidInt32;
            }

            break;

          case HID_USAGE_AXIS_Y:
            HidInt32 = HidBitFiledReadS32 (
                         (VOID *)Input,
                         InputBits,
                         Offset,
                         Bits
                         );
            if (HidField->Flags & HID_DATA_RELATIVE_FLAG) {
              AbsState->CurrentY = MIN (
                                     MAX (
                                       (INT64)(AbsState->CurrentY) + HidInt32,
                                       (INT64)(MouseDev->Mode.AbsoluteMinY)
                                       ),
                                     (INT64)(MouseDev->Mode.AbsoluteMaxY)
                                     );
            } else {
              AbsState->CurrentY = HidInt32;
            }

            break;

          case HID_USAGE_MOUSE_WHEEL:
            HidInt32 = HidBitFiledReadS32 (
                         (VOID *)Input,
                         InputBits,
                         Offset,
                         Bits
                         );
            if (HidField->Flags & HID_DATA_RELATIVE_FLAG) {
              AbsState->CurrentZ = MIN (
                                     MAX (
                                       (INT64)(AbsState->CurrentZ) + HidInt32,
                                       (INT64)(MouseDev->Mode.AbsoluteMinZ)
                                       ),
                                     (INT64)(MouseDev->Mode.AbsoluteMaxZ)
                                     );
            } else {
              AbsState->CurrentZ = (UINT64)((INT64)HidInt32);
            }

            break;

          default:
            break;
        }
      }
    }

    MouseDev->StateChanged = TRUE;
    return EFI_SUCCESS;
  } else {
    DEBUG ((DEBUG_INFO, "Invalid Report!\n"));
    return EFI_DEVICE_ERROR;
  }
}
