/**
 * @file
 *
 * AMD Integrated Debug Debug_library Routines
 *
 * Contains all functions related to HDTOUT
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 */
/*****************************************************************************
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 ******************************************************************************
 */
#ifndef _AMD_IDS_DEBUGPRINT_LIB_H_
#define _AMD_IDS_DEBUGPRINT_LIB_H_
#define AMD_IDS_DP_FLAG_ALL       0xFFFFFFFFFFFFFFFFull
/// HDTOUT Header.
typedef struct _HDTOUT_HEADER {
  UINT32 Signature;               ///< 0 0xDB1099CC
  UINT16 Version;                 ///< 4 HDTOUT version.
  UINT16 BufferSize;              ///< 6 Size in bytes.
  UINT16 DataIndex;               ///< 8 Data Index.
  UINT8  PrintCtrl;               ///< 10 0 off no print  1 on print
  UINT8  NumBreakpointUnit;       ///< 11 default 0 no bp unit others number of bp unit
  UINT32 FuncListAddr;            ///< 12 32 bit address to the list of functions that script can execute
  UINT8  ConsoleType;             ///< 16 Console type - deprecated
  UINT8  Event;                   ///< 17 Event type. - deprecated
  UINT8  OutBufferMode;           ///< 18 Off:stack mode, On: heap mode - deprecated
  UINT32 EnableMask;              ///< 19 Bitmap to select which part should be streamed out
  UINT64 ConsoleFilter;           ///< 23 Filter use to select which part should be streamed out
  UINT8  BspOnlyFlag;             ///< 31 1 Only Enable Bsp output, 0 enable On All cores
  UINT8  Reserved[56 - 32];       ///< 32 Reserved for header expansion

  CHAR8  BreakpointList[300];     ///< 56 Breakpoint list
  CHAR8  StatusStr[156];          ///< 356 Shows current node, DCT, CS,...
  CHAR8  Data[2];                 ///< 512 HDTOUT content. Its size will be determined by BufferSize.
} HDTOUT_HEADER;

BOOLEAN
AmdIdsDpHdtOutSupport (
  VOID
  );

VOID
AmdIdsDpHdtOutPrint (
  IN      CHAR8   *Buffer,
  IN      UINTN BufferSize
  );

BOOLEAN
AmdIdsDpHdtOutGetHeader (
  IN OUT   HDTOUT_HEADER **pHdtoutHeaderPtr
  );

VOID
AmdIdsDpHdtOutRegisterInit (
  VOID
  );

VOID
AmdIdsDpHdtOutBufferModeInit (
  );

VOID
AmdIdsDebugPrintf (
  IN      UINT64      FilterFlag,
  IN      CONST CHAR8 *Format,
  IN      VA_LIST     Marker
  );

#endif //_AMD_IDS_DEBUGPRINT_LIB_H_