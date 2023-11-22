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

#ifndef _HYGON_IDS_NV_TABLE_H_
#define _HYGON_IDS_NV_TABLE_H_

#pragma pack(push, 1)

#define IDS_NV_TABLE_SIGNATURE  0x564E4924ul        ///< $INV
#define IDS_NV_TABLE_REV_1      0x00000001ul
#define IDS_NV_TABLE_HEADER_REV1_DATA \
  { \
    IDS_NV_TABLE_SIGNATURE, \
    IDS_NV_TABLE_REV_1, \
  }
/// IDS NV table header
typedef struct _IDS_NV_TABLE_HEADER {
  UINT32    Signature;    ///< Signature
  UINT32    Revision;     ///< Revision
  UINT8     Reserved[24]; ///< Rsvd
} IDS_NV_TABLE_HEADER;

typedef UINT16 IDS_NV_ID;
#define IDS_NV_ATTRIB_SIZE_BYTE   0       ///< byte
#define IDS_NV_ATTRIB_SIZE_WORD   1       ///< word
#define IDS_NV_ATTRIB_SIZE_DWORD  2       ///< dword
#define IDS_NV_ATTRIB_SIZE_QWORD  3       ///< qword

/// IDS NV attribute
typedef struct _IDS_NV_ATTRIB {
  UINT8    size     : 2; ///< Size of a NV record
  UINT8    reserved : 6; ///< reserved
} IDS_NV_ATTRIB;

#define IDS_NV_ID_END     0xfffful
#define IDS_NV_VALUE_END  0xfffffffful
#define IDS_NV_RECORD_END \
  { \
    IDS_NV_ID_END, \
    IDS_NV_VALUE_END \
  }
/// IDS NV record generic
typedef struct  _IDS_NV_RECORD_CMN {
  IDS_NV_ID        Id; ///< IDS NV ID
  IDS_NV_ATTRIB    Attrib; ///< IDS NV Attribute
} IDS_NV_RECORD_CMN;

/// IDS NV record for 8 bits
typedef struct  IDS_NV_RECORD_U8 {
  IDS_NV_ID        Id; ///< IDS NV ID
  IDS_NV_ATTRIB    Attrib; ///< IDS NV Attribute
  UINT8            Value; ///< Value
} IDS_NV_RECORD_U8;

/// IDS NV record for 16 bits
typedef struct  IDS_NV_RECORD_U16 {
  IDS_NV_ID        Id; ///< IDS NV ID
  IDS_NV_ATTRIB    Attrib; ///< IDS NV Attribute
  UINT16           Value; ///< Value
} IDS_NV_RECORD_U16;

/// IDS NV record for 32 bits
typedef struct  _IDS_NV_RECORD_U32 {
  IDS_NV_ID        Id; ///< IDS NV ID
  IDS_NV_ATTRIB    Attrib; ///< IDS NV Attribute
  UINT32           Value; ///< Value
} IDS_NV_RECORD_U32;

/// IDS NV record for 64 bits
typedef struct  _IDS_NV_RECORD_U64 {
  IDS_NV_ID        Id; ///< IDS NV ID
  IDS_NV_ATTRIB    Attrib; ///< IDS NV Attribute
  UINT64           Value; ///< Value
} IDS_NV_RECORD_U64;

/// IDS NV table
typedef struct  _IDS_NV_TABLE {
  IDS_NV_TABLE_HEADER    Header;    ///< Header
  UINT8                  NvRecords; ///< Data start
} IDS_NV_TABLE;

#pragma pack(pop)

#endif //#define _HYGON_IDS_NV_TABLE_H_
