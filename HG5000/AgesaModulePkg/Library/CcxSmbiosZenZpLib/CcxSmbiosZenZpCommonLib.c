/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Ccx SMBIOS common code
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
 ******************************************************************************
 *
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


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/BaseLib.h>
#include "Filecode.h"
#include "cpuRegisters.h"
#include "Library/CcxBaseX86Lib.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_CCXSMBIOSZENZPLIB_CCXSMBIOSZENZPCOMMONLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *                           L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *  GetNameString
 *
 *  Description:
 *    Get name string from MSR_C001_00[35:30]
 *
 *  Parameters:
 *    @param[in, out]    *String       Pointer to name string
 *    @param[in]         *StdHeader
 *
 */
VOID
GetNameString (
  IN OUT   CHAR8 *String,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8  i;
  UINT8  StringIndex;
  UINT8  MsrIndex;
  UINT64 MsrData;

  StringIndex = 0;
  for (MsrIndex = 0; MsrIndex <= 5; MsrIndex++) {
    MsrData = AsmReadMsr64 ((MSR_CPUID_NAME_STRING0 + MsrIndex));
    for (i = 0; i < 8; i++) {
      String[StringIndex] = (CHAR8) RShiftU64 (MsrData, (8 * i));
      StringIndex++;
    }
  }
  String[StringIndex] = '\0';
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  IsSourceStrContainTargetStr
 *
 *  Description:
 *    check if source string contains target string.
 *
 *  Parameters:
 *    @param[in, out]    *SourceStr      Pointer to source CHAR array
 *    @param[in, out]    *TargetStr      Pointer to target CHAR array
 *    @param[in]         *StdHeader
 *
 *    @retval            TRUE             Target string is contained in the source string
 *    @retval            FALSE            Target string is not contained in the source string
 */
BOOLEAN
IsSourceStrContainTargetStr (
  IN OUT   CHAR8 *SourceStr,
  IN OUT   CONST CHAR8 *TargetStr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  BOOLEAN IsContained;
  UINT32  SourceStrIndex;
  UINT32  TargetStrIndex;
  CHAR8 TargetChar;

  IsContained = FALSE;
  if ((TargetStr != NULL) && (SourceStr != NULL)) {
    for (SourceStrIndex = 0; SourceStr[SourceStrIndex] != '\0'; SourceStrIndex++) {
      // Compare TrgString with SrcString from frist charactor to the '\0'
      for (TargetStrIndex = 0; TargetStr[TargetStrIndex] != '\0'; TargetStrIndex++) {
        if (TargetStr[TargetStrIndex] != SourceStr[SourceStrIndex + TargetStrIndex]) {
          // if it's not match, try to check the upcase/lowcase
          TargetChar = 0;
          if (TargetStr[TargetStrIndex] >= 'a' && TargetStr[TargetStrIndex] <= 'z') {
            TargetChar = TargetStr[TargetStrIndex] - ('a' - 'A');
          } else if (TargetStr[TargetStrIndex] >= 'A' && TargetStr[TargetStrIndex] <= 'Z') {
            TargetChar = TargetStr[TargetStrIndex] + ('a' - 'A');
          }
          // compare again
          if (TargetChar != SourceStr[SourceStrIndex + TargetStrIndex]) {
            break;
          }
        }
      }

      if ((TargetStr[TargetStrIndex] == '\0') && (TargetStrIndex != 0)) {
        IsContained = TRUE;
        break;
      }
    }
  }
  return IsContained;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  AdjustGranularity
 *
 *  Description:
 *    If cache size is greater than or equal to 32M, then set granularity
 *    to 64K. otherwise, set granularity to 1K
 *
 *  Parameters:
 *    @param[in]    *CacheSizePtr
 *
 *    @retval       CacheSize
 *
 *  Processing:
 *
 */
UINT32
AdjustGranularity (
  IN UINT32 *CacheSizePtr
  )
{
  UINT32 CacheSize;

  if (*CacheSizePtr >= 2047 * 1024) {
    CacheSize = (*CacheSizePtr / 64);
    CacheSize |= 0x80000000;
  } else if (*CacheSizePtr >= 0x8000) {
    CacheSize = (UINT16) (*CacheSizePtr / 64);
    CacheSize |= 0x8000;
  } else {
    CacheSize = (UINT16) *CacheSizePtr;
  }

  return (CacheSize);
}

