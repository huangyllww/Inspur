/* $NoKeywords:$ */

/**
 * @file
 *
 * NBIO Config Codec verb table.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: GNB
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
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  <Library/BaseLib.h>
#include  <HGPI.h>
#include  <Gnb.h>
#include  <Library/IdsLib.h>
#include  <Library/HygonBaseLib.h>
#include  <Library/GnbLib.h>
#include  <Library/GnbMemAccLib.h>
#include  <Library/NbioAzaliaLib.h>
#include  <Filecode.h>

#define FILECODE  LIBRARY_NBIOAZALIALIB_NBIOAZALIALIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
TimeOutStall (
  IN       UINTN              Microseconds
  )
{
  UINTN  uSec;
  UINT8  Data;

  // Simplified implementation
  uSec = Microseconds / 2;
  while (uSec != 0) {
    LibHygonIoRead (AccessS3SaveWidth8, 0x80, &Data, NULL);
    uSec--;
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Audio controller reset
 *
 *
 * @param[in]  HdaBaseAddress      BAR0 Address Map
 * @param[in]  StdHeader           Standard configuration header
 *
 */
HGPI_STATUS
ControllerReset (
  IN      UINT32            HdaBaseAddress,
  IN      HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  Value32;
  UINT32  Loops;
  BOOLEAN ResetFlag = FALSE;

  //
  // Enter Reset Status
  // 
  GnbLibMemRead (HdaBaseAddress + HDA_GLOBAL_CONTROL, AccessWidth32, &Value32, StdHeader);
  Value32 &= ~BIT0;
  Value32 |= BIT8;
  GnbLibMemWrite (HdaBaseAddress + HDA_GLOBAL_CONTROL, AccessWidth32, &Value32, StdHeader);

  for (Loops = 0; Loops < TIMEOUT_LOOP_TIMES; Loops++) {
    Value32 = 0;
    GnbLibMemRead (HdaBaseAddress + HDA_GLOBAL_CONTROL, AccessWidth32, &Value32, StdHeader);
    if ((Value32 & BIT0) == 0) {
      IDS_HDT_CONSOLE (GNB_TRACE, "    ::Now, in reset state.\n");
      ResetFlag = TRUE;
      break;
    }

    TimeOutStall (TIMEOUT_STALL_10);
  }

  if (ResetFlag) {
    //
    // Eixt Reset Status
    //
    GnbLibMemRead (HdaBaseAddress + HDA_GLOBAL_CONTROL, AccessWidth32, &Value32, StdHeader);
    Value32 |= BIT0;
    GnbLibMemWrite (HdaBaseAddress + HDA_GLOBAL_CONTROL, AccessWidth32, &Value32, StdHeader);

    for (Loops = 0; Loops < TIMEOUT_LOOP_TIMES; Loops++) {
      Value32 = 0;
      GnbLibMemRead (HdaBaseAddress + HDA_GLOBAL_CONTROL, AccessWidth32, &Value32, StdHeader);
      if ((Value32 & BIT0) == BIT0) {
        IDS_HDT_CONSOLE (GNB_TRACE, "    ::Now, exit reset state.\n");
        return HGPI_SUCCESS;
      }

      TimeOutStall (TIMEOUT_STALL_10);
    }
  }

  return HGPI_ERROR;
}

/*----------------------------------------------------------------------------------------*/

/**
 * wait for Immediate Status result valid
 *
 *
 * @param[in]  HdaBaseAddress      BAR0 Address Map
 * @param[in]  StdHeader           Standard configuration header
 *
 */
HGPI_STATUS
WaitImmediateStatus (
  IN      UINT32            HdaBaseAddress,
  IN      HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  BitValue;
  UINT32  Loops;

  for (Loops = 0; Loops < TIMEOUT_LOOP_TIMES; Loops++) {
    BitValue = 0;
    GnbLibMemRead (HdaBaseAddress + HDA_IMM_COMMAND_STATUS, AccessWidth32, &BitValue, StdHeader);
    if ((BitValue & BIT1) == BIT1) {
      return HGPI_SUCCESS;
    }

    TimeOutStall (TIMEOUT_STALL_10);
  }

  return HGPI_ERROR;
}

/*----------------------------------------------------------------------------------------*/

/**
 * send HD Audio Codec Command
 *
 *
 * @param[in]       HdaBaseAddress      BAR0 Address Map
 * @param[in]       StdHeader           Standard configuration header
 * @param[in, out]  CodecCommandData    Command data
 * @param[in]       ReadBack            Retrieve the response
 *
 */
HGPI_STATUS
SendCodecCommand (
  IN       UINT32            HdaBaseAddress,
  IN       HYGON_CONFIG_PARAMS *StdHeader,
  IN OUT   UINT32            *CodecCommandData,
  IN       BOOLEAN           ReadBack
  )
{
  HGPI_STATUS  Status;
  UINT32       BitValue;
  UINT32       Loops;

  Status = HGPI_ERROR;
  if (CodecCommandData == NULL) {
    return Status;
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "    ::SendCodecCommand Data = %x\n", (UINT32)*CodecCommandData);

  //
  // Clear Immediate Command Status
  //
  GnbLibMemRead (HdaBaseAddress + HDA_IMM_COMMAND_STATUS, AccessWidth32, &BitValue, StdHeader);
  BitValue &= ~BIT0;
  GnbLibMemWrite (HdaBaseAddress + HDA_IMM_COMMAND_STATUS, AccessWidth32, &BitValue, StdHeader);

  for (Loops = 0; Loops < TIMEOUT_LOOP_TIMES; Loops++) {
    BitValue = 0;
    GnbLibMemRead (HdaBaseAddress + HDA_IMM_COMMAND_STATUS, AccessWidth32, &BitValue, StdHeader);
    if ((BitValue & BIT0 ) == 0) {
      Status = HGPI_SUCCESS;
      break;
    }

    TimeOutStall (TIMEOUT_STALL_10);
  }

  if (Status == HGPI_SUCCESS) {
    GnbLibMemWrite (HdaBaseAddress + HDA_IMM_COMMAND_OUTPUT_INTERFACE, AccessWidth32, CodecCommandData, StdHeader);
    BitValue |= BIT0;
    GnbLibMemWrite (HdaBaseAddress + HDA_IMM_COMMAND_STATUS, AccessWidth32, &BitValue, StdHeader);
    Status = WaitImmediateStatus (HdaBaseAddress, StdHeader);
    if (Status == HGPI_SUCCESS) {
      if (ReadBack == TRUE) {
        GnbLibMemRead (HdaBaseAddress + HDA_IMM_RESPONSE_INPUT_INTERFACE, AccessWidth32, CodecCommandData, StdHeader);
      }
    }
  }

  return Status;
}


/*----------------------------------------------------------------------------------------*/

/**
 * Mask SDI Empty Receive
 *
 *
 * @param[in]       HdaBaseAddress      BAR0 Address Map
 * @param[in]       StdHeader           Standard configuration header
 *
 */
VOID
MaskSdiEmptyReceive (
  IN       UINT32            HdaBaseAddress,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT32       Value;

  //
  // Mask SDI Empty Receive
  //
  GnbLibMemRead (HdaBaseAddress + HDA_REGBANK_CHECK, AccessWidth32, &Value, StdHeader);
  Value |= BIT2;
  GnbLibMemWrite (HdaBaseAddress + HDA_REGBANK_CHECK, AccessWidth32, &Value, StdHeader);
}


/*----------------------------------------------------------------------------------------*/

/**
 * Config Codec verb table
 *
 *
 * @param[in]       HdaBaseAddress      BAR0 Address Map
 * @param[in]       VerbTable           Codec Verbtable address
 *
 */
HGPI_STATUS
NbioConfigureVerbTable (
  IN      UINT32            HdaBaseAddress,
  IN      VOID              *VerbTable
  )
{
  HGPI_STATUS            Status;
  UINT16                 ChannelNum;
  UINT32                 CodecStateMap;
  UINT32                 CodecMask;
  UINT32                 VendorDeviceId;
  UINT32                 VerbTableData;
  CODEC_VERB_TABLE_LIST  *VerbTableList;
  CODEC_VERB_TABLE_LIST  *VerbTableAddress;

  VerbTableAddress = (CODEC_VERB_TABLE_LIST *)VerbTable;
  VerbTableData    = 0;

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioConfigureVerbTable: HdaBaseAddress = %x, CodecVerbTable = %x\n", HdaBaseAddress, VerbTable);

  if (HdaBaseAddress == 0 || VerbTableAddress == NULL) {
    IDS_HDT_CONSOLE (GNB_TRACE, "    ERROR::HdaBaseAddress == 0 || VerbTableAddress == NULL\n");
    return HGPI_ERROR;
  }

  //
  // Perform CRST causes the controller to exit its Reset state and de-assert the link RESET# signal.
  //
  Status = ControllerReset (HdaBaseAddress, NULL);
  if (Status == HGPI_ERROR) {
    IDS_HDT_CONSOLE (GNB_TRACE, "    ERROR::Active Controller fail! \n");
    return Status;
  }

  //
  // To find out all codec through identification of the state change status
  // "STATESTS" bits.
  //
  TimeOutStall (TIMEOUT_STALL_100);
  GnbLibMemRead (HdaBaseAddress + HDA_WAKE_ENABLE, AccessWidth32, &CodecStateMap, NULL);
  CodecStateMap = (CodecStateMap & 0xf0000) >> 16;
  IDS_HDT_CONSOLE (GNB_TRACE, "    ::CodecStateMap = %x\n", CodecStateMap);

  //
  // Mask SDI empty receive
  //
  MaskSdiEmptyReceive(HdaBaseAddress, NULL);

  //
  // To confirm every SDIN if there is a codec on link.
  //
  for (ChannelNum = 0; ChannelNum < HDA_MAX_SDIN_NUM; ChannelNum++) {
    CodecMask = (1 << ChannelNum);
    if (CodecStateMap & CodecMask) {
      //
      // To read the Codec's vendor and device ID.
      //
      VendorDeviceId  = 0xF0000;
      VendorDeviceId |= (ChannelNum << 28);
      Status = SendCodecCommand (HdaBaseAddress, NULL, &VendorDeviceId, TRUE);
      IDS_HDT_CONSOLE (GNB_TRACE, "    ::Read the Codec's VendorDeviceId = %x\n", VendorDeviceId);
      if (Status == HGPI_SUCCESS) {
        VerbTableList = VerbTableAddress;
        while (VerbTableList->CodecId != 0xFFFFFFFF) {
          IDS_HDT_CONSOLE (GNB_TRACE, "    ::CodecId = %x, VendorDeviceId = %x\n", VerbTableList->CodecId, VendorDeviceId);
          if (VendorDeviceId == VerbTableList->CodecId) {
            //
            // Deliver Verb Entry
            //
            VerbTableList++;
            while (VerbTableList->CodecId != 0xFFFFFFFF) {
              VerbTableData  = VerbTableList->CodecId;
              VerbTableData |= (ChannelNum << 28);
              Status = SendCodecCommand (HdaBaseAddress, NULL, &VerbTableData, FALSE);
              if (Status == HGPI_ERROR) {
                IDS_HDT_CONSOLE (GNB_TRACE, "    ERROR::CodecId = %x, Deliver Verb Data = %x fail! \n", VendorDeviceId, VerbTableData);
                break;
              }

              VerbTableList++;
            }
          } else {
            while (VerbTableList->CodecId != 0xFFFFFFFF) {
              VerbTableList++;
            }
          }

          //
          // Finished, break to search the next SDIN to detect Codec.
          //
          VerbTableList++;
          if (Status == HGPI_ERROR) {
            IDS_HDT_CONSOLE (GNB_TRACE, "    ERROR::CodecId = %x, Deliver Verb Data = %x fail! \n", VendorDeviceId, VerbTableData);
            break;
          }

          if (VerbTableList->CodecId == 0xFFFFFFFF) {
            IDS_HDT_CONSOLE (GNB_TRACE, "NbioConfigureVerbTable: End\n");
            break;
          }
        }
      } else {
        IDS_HDT_CONSOLE (GNB_TRACE, "    ERROR::Read the Codec's Failed.\n");
      }
    }
  }

  return Status;
}
