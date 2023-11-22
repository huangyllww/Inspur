/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef _SETUP_ITEMS_H_
#define _SETUP_ITEMS_H_

//
// {0xcee2ff3a, 0xa442, 0x4cb0, {0xb9, 0x2d, 0xd4, 0xcd, 0xd2, 0x3f, 0x56, 0xaa}};
//
extern EFI_GUID gByoSetupItemsDBGuid;

#define MAX_EFI_VARIABLE_NAME_LENGTH 40

#define BKDRHASH(p, h) while ( *p != '\0' ) h = (h*131) + *p++;

typedef struct {
  UINT32     VariableIndex;
  UINT16     ItemOffset;
  UINT16     ItemLength;
  //CHAR8      *ItemName;
  UINT32     ItemNameHash;
} SETUP_ITEM_INFO;

typedef struct {
  EFI_GUID   VariableGuid;
  UINT32     VariableLen;
  UINT32     Attribute;
  CHAR16     VariableName[MAX_EFI_VARIABLE_NAME_LENGTH];
} SETUP_VARIABLE_TABLE;

//
// Setup Item DataBase Layout
//
typedef struct {
  UINT32 SetupVariableNumber;
  UINT32 SetupItemNumber;
  SETUP_VARIABLE_TABLE VariableTable[];
  // SETUP_ITEM_INFO [];
} SETUP_DATABASE_LAYOUT;

//
// This structure is used to record the updated option.
//
typedef struct {
  //
  // A Null-terminated string that is the name of the vendor's variable.
  //
  CHAR16   VariableName[64];
  //
  // A unique identifier for the vendor.
  //
  EFI_GUID VendorGuid;
  //
  // Option offset to the variable storage.
  //
  UINT16   Offset;
  //
  // Update Type is MinValue, MaxValue, DefaultValue, Attribute as the above
  //
  UINT32   UpdateType;
  //
  // UpdateValueCount is the count of UpdateValue
  //
  UINT64   UpdateValueCount;
  //
  // Update value matches update type.
  //
  UINT64   UpdateValue[];
} SETUP_UPDATE_DATA_INFO;

// oneof may have one or more option
#define EFI_IFR_ONE_OF_OP              0x05
// checkbox has two option, 0 or 1
#define EFI_IFR_CHECKBOX_OP            0x06
// numeric is value region: min ~ max
#define EFI_IFR_NUMERIC_OP             0x07

#define EFI_IFR_ORDERED_LIST_OP             0x23

typedef struct  {
  UINT8                    OpCode;
  UINT8                    Count;
  UINT8                    DataType; // 1, 2, 4, 8
  UINT8                    Value [1];
  // union {
  //   UINT8   U8;
  //   UINT16  U16;
  //   UINT32  U32;
  //   UINT64  U64;
  // } Value [1]; // Value [0] is the default value; Value [1..Count-1] is valid value. In OrderedList mode,value[1..Count-1] is the value
} OPTIONS_DATA;

#define DATA_TYPE_UINT8      0x00
#define DATA_TYPE_UINT16     0x01
#define DATA_TYPE_UINT32     0x02
#define DATA_TYPE_UINT64     0x03

#pragma pack(1)
typedef struct {
  CHAR16     VariableName[MAX_EFI_VARIABLE_NAME_LENGTH];
  EFI_GUID   VariableGuid;
  UINT32     Attribute;
  UINT64     VariableLen;
  UINT8      Variabledata[];
} VARIABLE_STRUCT;
#pragma pack()

#endif
