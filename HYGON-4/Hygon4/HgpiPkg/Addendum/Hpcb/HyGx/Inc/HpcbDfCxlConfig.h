/* $NoKeywords:$ */

/**
 * @file
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
 *
 ***************************************************************************/

#ifndef _HPCB_DF_CXL_CONFIG_H_
#define _HPCB_DF_CXL_CONFIG_H_

#define HPCB_DF_TYPE_CXL_CONFIG                   0x00CC

#define MAX_CXL_REGIONS                           16

#define CXL_256MB_ALIGN                           28  //2^28 = SIZE_256MB
#define CXL_512MB_ALIGN                           29  //2^29 = SIZE_512MB
#define CXL_1GB_ALIGN                             30  //2^30 = SIZE_1GB
#define CXL_2GB_ALIGN                             31  //2^31 = SIZE_2GB
#define CXL_4GB_ALIGN                             32  //2^32 = SIZE_4GB

#pragma pack (push, 1)

typedef struct _CXL_REGION_DESCRIPTOR {
  BOOLEAN       Valid;                            ///< 1-This region is valid; 0- Invalid
  UINT8         SocketId;                         ///< Socket Id
  UINT8         LogicalDieId;                     ///< IOD Logical Die Id
  UINT8         NbioMap;                          ///< Bit n set means NBIO n should be included in this region.
  UINT8         IntlvSize;                        ///< See DF_MEM_INTLV_SIZE，default auto
  UINT8         MemType;                          ///< 0 - Volatile; 1 - Persistent
  UINT16        Alignment;                        ///< Required alignment.
  UINT64        MemSize;                          ///< Raw size.  
} CXL_REGION_DESCRIPTOR;

typedef struct _CXL_MEM_INFO_STRUCT {
  CXL_REGION_DESCRIPTOR  CxlRegion[MAX_CXL_REGIONS];
} CXL_MEM_INFO_STRUCT;

#pragma pack (pop)

#endif // _HPCB_DF_CXL_CONFIG_H_
