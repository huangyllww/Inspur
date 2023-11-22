/* $NoKeywords:$ */

/**
 * @file
 *
 * Service procedure to access PCI config space registers
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: NBIO
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

#ifndef _NBIOTABLE_H_
#define _NBIOTABLE_H_

#include "GnbHsio.h"

#pragma pack (push, 1)

#define NBIO_TABLE_FLAGS_FORCE_S3_SAVE  0x00000001ul

typedef UINT8  GNB_TABLE;
typedef UINT8  NBIO_TABLE;

#define __DATA(x)  x

#define _DATA32(Data)  ((UINT32)(__DATA (Data))) & 0xFF, (((UINT32)(__DATA (Data))) >> 8) & 0xFF, (((UINT32)(__DATA (Data))) >> 16) & 0xFF, (((UINT32)(__DATA (Data))) >> 24) & 0xFF
#define _DATA64(Data)  _DATA32 (Data & 0xfffffffful), _DATA32 ((UINT64)Data >> 32)

/// Entry type
typedef enum {
  GnbEntryWr,                     ///< Write register
  GnbEntryPropertyWr,             ///< Write register check property
  GnbEntryFullWr,                 ///< Write Rgister check revision and property
  GnbEntryRmw,                    ///< Read Modify Write register
  GnbEntryPropertyRmw,            ///< Read Modify Write register check property
  GnbEntryRevRmw,                 ///< Read Modify Write register check revision
  GnbEntryFullRmw,                ///< Read Modify Write register check revision and property
  GnbEntryPoll,                   ///< Poll register
  GnbEntryPropertyPoll,           ///< Poll register check property
  GnbEntryFullPoll,               ///< Poll register check property
  GnbEntryCopy,                   ///< Copy field from one register to another
  GnbEntryStall,                  ///< Copy field from one register to another
  GnbEntryTerminate = 0xFF        ///< Terminate table
} GNB_TABLE_ENTRY_TYPE;

#define GNB_ENTRY_WR(RegisterSpaceType, Address, Value) \
  GnbEntryWr, RegisterSpaceType, _DATA32 (Address), _DATA32 (Value)

/// Write register entry
typedef struct {
  UINT8     EntryType;            ///< Entry type
  UINT8     RegisterSpaceType;    ///< Register space
  UINT32    Address;              ///< Register address
  UINT32    Value;                ///< Value
} GNB_TABLE_ENTRY_WR;

#define GNB_ENTRY_PROPERTY_WR(Property, RegisterSpaceType, Address, Value) \
  GnbEntryPropertyWr, _DATA32 (Property), RegisterSpaceType, _DATA32 (Address), _DATA32 (Value)

/// Write register entry
typedef struct {
  UINT8     EntryType;            ///< Entry type
  UINT32    Property;             ///< Property
  UINT8     RegisterSpaceType;    ///< Register space
  UINT32    Address;              ///< Register address
  UINT32    Value;                ///< Value
} GNB_TABLE_ENTRY_PROPERTY_WR;

#define GNB_ENTRY_RMW(RegisterSpaceType, Address, AndMask, OrMask) \
  GnbEntryRmw, RegisterSpaceType, _DATA32 (Address), _DATA32 (AndMask), _DATA32 (OrMask)

/// Read Modify Write data Block
typedef struct {
  UINT8     RegisterSpaceType;    ///< Register space
  UINT32    Address;              ///< Register address
  UINT32    AndMask;              ///< And Mask
  UINT32    OrMask;               ///< Or Mask
} GNB_RMW_BLOCK;

/// Read Modify Write register entry
typedef struct {
  UINT8            EntryType;     ///< Entry type
  GNB_RMW_BLOCK    Data;          ///< Data
} GNB_TABLE_ENTRY_RMW;

#define GNB_ENTRY_FULL_WR(Property, Revision, RegisterSpaceType, Address, Value) \
  GnbEntryFullWr, _DATA32 (Property), _DATA64 (Revision), RegisterSpaceType, _DATA32 (Address), _DATA32 (Value)

/// Write register entry
typedef struct {
  UINT8     EntryType;            ///< Entry type
  UINT32    Property;             ///< Property
  UINT64    Revision;             ///< Revision
  UINT8     RegisterSpaceType;    ///< Register space
  UINT32    Address;              ///< Register address
  UINT32    Value;                ///< Value
} GNB_TABLE_ENTRY_FULL_WR;

#define GNB_ENTRY_PROPERTY_RMW(Property, RegisterSpaceType, Address, AndMask, OrMask) \
  GnbEntryPropertyRmw, _DATA32 (Property), RegisterSpaceType, _DATA32 (Address), _DATA32 (AndMask), _DATA32 (OrMask)

/// Read Modify Write register entry
typedef struct {
  UINT8            EntryType;     ///< Entry type
  UINT32           Property;      ///< Property
  GNB_RMW_BLOCK    Data;          ///< Data
} GNB_TABLE_ENTRY_PROPERTY_RMW;

#define GNB_ENTRY_REV_RMW(Rev, RegisterSpaceType, Address, AndMask, OrMask) \
  GnbEntryRevRmw, _DATA64 (Rev), RegisterSpaceType, _DATA32 (Address), _DATA32 (AndMask), _DATA32 (OrMask)

/// Read Modify Write register entry
typedef struct {
  UINT8            EntryType;     ///< Entry type
  UINT64           Revision;      ///< revision
  GNB_RMW_BLOCK    Data;          ///< Data
} GNB_TABLE_ENTRY_REV_RMW;

#define GNB_ENTRY_FULL_RMW(Property, Revision, RegisterSpaceType, Address, AndMask, OrMask) \
  GnbEntryFullRmw, _DATA32 (Property), _DATA64 (Revision), RegisterSpaceType, _DATA32 (Address), _DATA32 (AndMask), _DATA32 (OrMask)

/// Read Modify Write register entry
typedef struct {
  UINT8            EntryType;     ///< Entry type
  UINT32           Property;      ///< Property
  UINT64           Revision;      ///< Revision
  GNB_RMW_BLOCK    Data;          ///< Data
} GNB_TABLE_ENTRY_FULL_RMW;

#define GNB_ENTRY_POLL(RegisterSpaceType, Address, AndMask, CompareValue) \
  GnbEntryPoll, RegisterSpaceType, _DATA32 (Address), _DATA32 (AndMask), _DATA32 (CompareValue)
/// Poll register entry
typedef struct {
  UINT8     EntryType;            ///< Entry type
  UINT8     RegisterSpaceType;    ///< Register space
  UINT32    Address;              ///< Register address
  UINT32    AndMask;              ///< End mask
  UINT32    CompareValue;         ///< Compare value
} GNB_TABLE_ENTRY_POLL;

#define GNB_ENTRY_PROPERTY_POLL(Property, RegisterSpaceType, Address, AndMask, CompareValue) \
  GnbEntryPropertyPoll, _DATA32 (Property), RegisterSpaceType, _DATA32 (Address), _DATA32 (AndMask), _DATA32 (CompareValue)
/// Poll register entry
typedef struct {
  UINT8     EntryType;            ///< Entry type
  UINT32    Property;             ///< Property
  UINT8     RegisterSpaceType;    ///< Register space
  UINT32    Address;              ///< Register address
  UINT32    AndMask;              ///< End mask
  UINT32    CompareValue;         ///< Compare value
} GNB_TABLE_ENTRY_PROPERTY_POLL;

#define GNB_ENTRY_FULL_POLL(Property, Revision, RegisterSpaceType, Address, AndMask, CompareValue) \
  GnbEntryFullPoll, _DATA32 (Property), _DATA64 (Revision), RegisterSpaceType, _DATA32 (Address), _DATA32 (AndMask), _DATA32 (CompareValue)
/// Poll register entry
typedef struct {
  UINT8     EntryType;            ///< Entry type
  UINT32    Property;             ///< Property
  UINT64    Revision;             ///< Revision
  UINT8     RegisterSpaceType;    ///< Register space
  UINT32    Address;              ///< Register address
  UINT32    AndMask;              ///< End mask
  UINT32    CompareValue;         ///< Compare value
} GNB_TABLE_ENTRY_FULL_POLL;

#define GNB_ENTRY_COPY(DestRegSpaceType, DestAddress, DestFieldOffset, DestFieldWidth, SrcRegisterSpaceType, SrcAddress, SrcFieldOffset, SrcFieldWidth) \
  GnbEntryCopy, DestRegSpaceType, _DATA32 (DestAddress), DestFieldOffset, DestFieldWidth, SrcRegisterSpaceType, _DATA32 (SrcAddress), SrcFieldOffset, SrcFieldWidth

/// Copy register entry
typedef struct {
  UINT8     EntryType;             ///< Entry type
  UINT8     DestRegisterSpaceType; ///< Register space
  UINT32    DestAddress;           ///< Register address
  UINT8     DestFieldOffset;       ///< Field Offset
  UINT8     DestFieldWidth;        ///< Field Width
  UINT8     SrcRegisterSpaceType;  ///< Register space
  UINT32    SrcAddress;            ///< Register address
  UINT8     SrcFieldOffset;        ///< Field Offset
  UINT8     SrcFieldWidth;         ///< Field Width
} GNB_TABLE_ENTRY_COPY;

#define GNB_ENTRY_STALL(Microsecond) \
  GnbEntryStall, _DATA32 (Microsecond)

/// Write register entry
typedef struct {
  UINT8     EntryType;             ///< Entry type
  UINT32    Microsecond;           ///< Value
} GNB_TABLE_ENTRY_STALL;

#define GNB_ENTRY_TERMINATE  GnbEntryTerminate

HGPI_STATUS
GnbProcessTable (
  IN      GNB_HANDLE            *GnbHandle,
  IN      GNB_TABLE             *Table,
  IN      UINT32                Property,
  IN      UINT32                Flags,
  IN      HYGON_CONFIG_PARAMS     *StdHeader
  );

#pragma pack (pop)

#endif
