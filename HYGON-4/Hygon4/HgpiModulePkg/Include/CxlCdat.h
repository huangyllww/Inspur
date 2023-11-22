/** @file

CXL CDAT LIB.

**/
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

#ifndef __CXL_CDAT_H__
#define __CXL_CDAT_H__

#include <Uefi.h>

#define MAX_DSMAD_PER_CXL_DEVICE             2

#define MAX_DSMAS_RECORDS_PER_DSMAD          1
#define MAX_DSLBIS_RECORDS_PER_DSMAD         4
#define MAX_DSMSCIS_RECORDS_PER_DSMAD        1
#define MAX_DSIS_RECORDS_PER_DSMAD           1
#define MAX_DSEMTS_RECORDS_PER_DSMAD         1

#define MAX_DSMAS_RECORDS_PER_CXL_DEVICE     (MAX_DSMAD_PER_CXL_DEVICE * MAX_DSMAS_RECORDS_PER_DSMAD)
#define MAX_DSLBIS_RECORDS_PER_CXL_DEVICE    (MAX_DSMAD_PER_CXL_DEVICE * MAX_DSLBIS_RECORDS_PER_DSMAD)
#define MAX_DSMSCIS_RECORDS_PER_CXL_DEVICE   (MAX_DSMAD_PER_CXL_DEVICE * MAX_DSMSCIS_RECORDS_PER_DSMAD)
#define MAX_DSIS_RECORDS_PER_CXL_DEVICE      (MAX_DSMAD_PER_CXL_DEVICE * MAX_DSIS_RECORDS_PER_DSMAD)
#define MAX_DSEMTS_RECORDS_PER_CXL_DEVICE    (MAX_DSMAD_PER_CXL_DEVICE * MAX_DSEMTS_RECORDS_PER_DSMAD)

typedef enum {
  CdatTypeDsmas = 0,
  CdatTypeDslbis,
  CdatTypeDsmscis,
  CdatTypeDsis,
  CdatTypeDsemts,
  CdatTypeSslbis
} CDAT_TYPE;

typedef struct {
  UINT32  Length;
  UINT8   Revision;
  UINT8   Checksum;
  UINT8   Reserved[6];
  UINT32  Sequence;
  // CDAT structure at byte offset 16, variable length
} CDAT_HEADER;

typedef struct {
  UINT8      Type;
  UINT8      Reserved1;
  UINT16     Length;
  UINT8      Handle;
  UINT8      Flags;
  UINT8      Reserved2[2];
  UINT64     DpaBase;
  UINT64     DpaLength;
} DSMAS_CDAT;

typedef struct {
  UINT8      Type;
  UINT8      Reserved1;
  UINT16     Length;
  UINT8      Handle;
  UINT8      Flags;
  UINT8      DataType;
  UINT8      Reserved2;
  UINT64     EntryBaseUnit;
  UINT16     Entry[3];
  UINT8      Reserved3[2];
} DSLBIS_CDAT;

typedef struct {
  UINT8      Type;
  UINT8      Reserved1;
  UINT16     Length;
  UINT8      Handle;
  UINT8      Reserved2[3];
  UINT64     MemSideCacheSize;
  UINT32     CacheAttributes;
} DSMSCIS_CDAT;

typedef struct {
  UINT8      Type;
  UINT8      Reserved1;
  UINT16     Length;
  UINT8      Flags;
  UINT8      Handle;
  UINT8      Reserved2[2];
} DSIS_CDAT;

typedef struct {
  UINT8      Type;
  UINT8      Reserved1;
  UINT16     Length;
  UINT8      Handle;
  UINT8      EfiTypeAndAttributes;
  UINT8      Reserved2[2];
  UINT64     DpaOffset;
  UINT64     DpaLength;
} DSEMTS_CDAT;

typedef struct {
  UINT8      Type;
  UINT8      Reserved1;
  UINT16     Length;
  UINT8      DataType;
  UINT8      Reserved2[3];
  UINT64     EntryBaseUnit;
// SSLBE Entry array of variable length
} SSLBIS_CDAT;

typedef struct {
  UINT16     XPortId;
  UINT16     YPortId;
  UINT16     LatencyBandwidth;
  UINT8      Reserved[2];
} SSLBE_ENTRY;

typedef struct {
  CDAT_HEADER   Header;
  VOID          *Entries;
} CDAT_TABLE;

#endif
