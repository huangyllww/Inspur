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
****************************************************************************
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/BaseLib.h>
#include "HYGON.h"
#include <RtcIntrDetBase.h>
#include <Library/DebugLib.h>
#include <Library/RtcIntrDetLib.h>
#include "Fch.h"
#include <Library/IoLib.h>
#include <Library/FchBaseLib.h>
#include "Filecode.h"

#define FILECODE  LIBRARY_RTCINTRDETLIB_RTCINTRDETLIB_FILECODE

/**
  DumpRtcRam24 is to Dump Ram24 Data that is Rtc Intrusion Record
  @param[out]  RtcRamData     pointer to ram24
  @retval      VOID
**/
VOID 
DumpRtcRam24 (
  IN RTCRAM_DATA     *RtcRamData
  )
{
  UINT8             RecordIndex;

  //Dump Records
  for (RecordIndex = 0; RecordIndex < INTR_MAX_RECORD; RecordIndex++) {
    DEBUG ((EFI_D_INFO, " RecordIndex %d : 0x%08x \n", RecordIndex, RtcRamData->Intr_Record[RecordIndex]));
  
    DEBUG ((DEBUG_INFO, "   Intrusion Year %d Month %d Day %d,  Hour %d Minute %d\n",
               RtcRamData->Intr_Record[RecordIndex].Field.Year, 
               RtcRamData->Intr_Record[RecordIndex].Field.Month, 
               RtcRamData->Intr_Record[RecordIndex].Field.Day, 
               RtcRamData->Intr_Record[RecordIndex].Field.Hour, 
               RtcRamData->Intr_Record[RecordIndex].Field.Minute));
  }
}

/**
  ReadRtcRam24 is to read Ram24 Data that is Rtc Intrusion Record
  @param[in,out]   Byte     pointer to ram24
  @retval       VOID
**/
VOID 
ReadRtcRam24 (
  IN OUT UINT8     *Byte
  )
{
  UINT8             RecordIndex;
  UINT8             Byteoffset;

  //Set ExtraRtcCmosEn to access RAM24
  FchRwPmio (FCH_PMIOA_REG54, AccessWidth32, ~(UINT32) BIT30, (UINT32) BIT30);

  //Set I_RtcRsvBit7 to Switch RAM24
  FchRwPmio (FCH_PMIOA_REG4C, AccessWidth8, ~(UINT32) BIT7, (UINT32) BIT7);
  
  //Read Ram24
  for (RecordIndex = 0; RecordIndex < INTR_MAX_RECORD; RecordIndex++) {
    for (Byteoffset = 0; Byteoffset < SINGLE_RECORD_BYTE; Byteoffset++) {
      IoWrite8 (FCH_IOMAP_REG72, (Byteoffset * 0x40 + RecordIndex + RAM24_OFFSET));
      *Byte = IoRead8 (FCH_IOMAP_REG73);
      Byte ++;   
    }
  }

  //Clear I_RtcRsvBit7
  FchRwPmio (FCH_PMIOA_REG4C, AccessWidth8, ~(UINT32) BIT7, 0);

  //Clear ExtraRtcCmosEn 
  FchRwPmio (FCH_PMIOA_REG54, AccessWidth32, ~(UINT32) BIT30, 0);
}  
