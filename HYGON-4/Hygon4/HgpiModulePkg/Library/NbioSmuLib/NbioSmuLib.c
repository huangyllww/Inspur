/* $NoKeywords:$ */

/**
 * @file
 *
 * SMU services
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
#include  <HGPI.h>
#include  <Gnb.h>
#include  <Filecode.h>
#include  <Library/GnbLib.h>
#include  <Library/IdsLib.h>
#include  <Library/HygonBaseLib.h>
#include  <Library/NbioHandleLib.h>
#include  <Library/NbioSmuLib.h>
#include  <Library/CpuLib.h>
#include  <Library/HygonS3SaveLib.h>
#include  <Library/SmnAccessLib.h>
#include  <GnbRegisters.h>
#include  <Library/HygonSmnAddressLib.h>
#include  <Library/HygonSocBaseLib.h>

#define FILECODE  LIBRARY_NBIOSMULIB_NBIOSMULIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#ifndef INVALID_SMU_MSG
  #define INVALID_SMU_MSG  0xFF
#endif
#ifndef BIOSSMC_Result_FATAL
  #define BIOSSMC_Result_FATAL  ((UINT8)0xFC)       // i.e. -4
#endif

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

#define   D0F0xB8_ADDRESS  0xB8

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to write SMU register space
 *
 *
 * @param[in] GnbPciAddress      PCI_ADDR
 * @param[in] Address            Register offset, but PortDevice
 * @param[in] Value              The value to write
 * @retval    HGPI_STATUS
 */
VOID
STATIC
SmuRegisterWrite (
  IN       PCI_ADDR            GnbPciAddress,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags
  )
{
  SmnRegisterWrite (GnbPciAddress.Address.Bus, Address, Value, Flags);
  return;
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to read SMU register space
 *
 *
 *
 * @param[in]  GnbPciAddress      PCI_ADDR
 * @param[in]  Address            Register offset, but PortDevice
 * @param[out] Value              Return value
 * @retval     HGPI_STATUS
 */
VOID
STATIC
SmuRegisterRead (
  IN       PCI_ADDR            GnbPciAddress,
  IN       UINT32              Address,
  OUT   VOID                *Value,
  IN       UINT32              Flags
  )
{
  SmnRegisterRead (GnbPciAddress.Address.Bus, Address, Value);
  return;
}

/*----------------------------------------------------------------------------------------*/

/*
 *  Routine to Init Arguments
 *
 *
 *
 * @param[in, out]  SmuArg Request Argument
 */
VOID
NbioSmuServiceCommonInitArguments (
  IN OUT   UINT32                   *SmuArg
  )
{
  LibHygonMemFill (SmuArg, 0x00, 24, NULL);
}

/*----------------------------------------------------------------------------------------*/

/**
 * SMU service request
 *
 *
 * @param[in]      NbioPciAddress  PCI_ADDR of this NBIO
 * @param[in]      RequestId       Request ID
 * @param[in, out] RequestArgument Request Argument
 * @param[in]      AccessFlags     See GNB_ACCESS_FLAGS_* definitions
 */
UINT32
NbioSmuServiceRequest (
  IN       PCI_ADDR                 NbioPciAddress,
  IN       UINT32                   PhysicalDieId,
  IN       UINT32                   RequestId,
  IN OUT   UINT32                   *RequestArgument,
  IN       UINT32                   AccessFlags
  )
{
  UINT32  RxSmuMessageResponse;
  UINT32  RxSmuMessageResponseS3;
  UINT32  DataMask;
  UINT32  Address;
  UINT32  CpuModel;
  UINT32  ResponseAddress;
  UINT32  Arg0Address;
  UINT32  Arg1Address;
  UINT32  Arg2Address;
  UINT32  Arg3Address;
  UINT32  Arg4Address;
  UINT32  Arg5Address;
  UINT32  MsgAddress;

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioSmuServiceRequest Enter\n");

  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_EX_CPU) {
    ResponseAddress = MP1_C2PMSG_RESPONSE_ADDRESS_HYEX;
    Arg0Address = MP1_C2PMSG_ARGUMENT_0_ADDRESS_HYEX;
    Arg1Address = MP1_C2PMSG_ARGUMENT_1_ADDRESS_HYEX;
    Arg2Address = MP1_C2PMSG_ARGUMENT_2_ADDRESS_HYEX;
    Arg3Address = MP1_C2PMSG_ARGUMENT_3_ADDRESS_HYEX;
    Arg4Address = MP1_C2PMSG_ARGUMENT_4_ADDRESS_HYEX;
    Arg5Address = MP1_C2PMSG_ARGUMENT_5_ADDRESS_HYEX;
    MsgAddress = MP1_C2PMSG_MESSAGE_ADDRESS_HYEX;

  } else if (CpuModel == HYGON_GX_CPU) {
    ResponseAddress = MP1_C2PMSG_RESPONSE_ADDRESS_HYGX;
    Arg0Address = MP1_C2PMSG_ARGUMENT_0_ADDRESS_HYGX;
    Arg1Address = MP1_C2PMSG_ARGUMENT_1_ADDRESS_HYGX;
    Arg2Address = MP1_C2PMSG_ARGUMENT_2_ADDRESS_HYGX;
    Arg3Address = MP1_C2PMSG_ARGUMENT_3_ADDRESS_HYGX;
    Arg4Address = MP1_C2PMSG_ARGUMENT_4_ADDRESS_HYGX;
    Arg5Address = MP1_C2PMSG_ARGUMENT_5_ADDRESS_HYGX;
    MsgAddress = MP1_C2PMSG_MESSAGE_ADDRESS_HYGX;

  } else {
    IDS_HDT_CONSOLE (GNB_TRACE, "NbioSmuServiceRequest CPU model INVALID!!\n");
    return SMC_Result_OK;
  }

  DataMask = 0xFFFFFFFF;

  if (RequestId == INVALID_SMU_MSG) {
    IDS_HDT_CONSOLE (GNB_TRACE, "NbioSmuServiceRequest RequestId INVALID!!\n");
    return SMC_Result_OK;
  }

  IDS_HDT_CONSOLE (NB_MISC, "  Service Request 0x%x\n", RequestId);
  IDS_HDT_CONSOLE (NB_MISC, "  Service Request Argument 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", RequestArgument[0], RequestArgument[1], RequestArgument[2], RequestArgument[3], RequestArgument[4], RequestArgument[5]);

  SmuRegisterRead (NbioPciAddress, IOD_SPACE (PhysicalDieId, ResponseAddress), &RxSmuMessageResponse, 0);

  // 1 Clear Response
  RxSmuMessageResponse = 0;
  SmuRegisterWrite (NbioPciAddress, IOD_SPACE (PhysicalDieId, ResponseAddress), &RxSmuMessageResponse, AccessFlags);

  // 2 Write message arguments
  SmuRegisterWrite (NbioPciAddress, IOD_SPACE (PhysicalDieId, Arg0Address), &RequestArgument[0], AccessFlags);
  SmuRegisterWrite (NbioPciAddress, IOD_SPACE (PhysicalDieId, Arg1Address), &RequestArgument[1], AccessFlags);
  SmuRegisterWrite (NbioPciAddress, IOD_SPACE (PhysicalDieId, Arg2Address), &RequestArgument[2], AccessFlags);
  SmuRegisterWrite (NbioPciAddress, IOD_SPACE (PhysicalDieId, Arg3Address), &RequestArgument[3], AccessFlags);
  SmuRegisterWrite (NbioPciAddress, IOD_SPACE (PhysicalDieId, Arg4Address), &RequestArgument[4], AccessFlags);
  SmuRegisterWrite (NbioPciAddress, IOD_SPACE (PhysicalDieId, Arg5Address), &RequestArgument[5], AccessFlags);

  // 3 Write message ID
  SmuRegisterWrite (NbioPciAddress, IOD_SPACE (PhysicalDieId, MsgAddress), &RequestId, AccessFlags);

  // 4 Poll Response until non-zero
  do {
    SmuRegisterRead (NbioPciAddress, IOD_SPACE (PhysicalDieId, ResponseAddress), &RxSmuMessageResponse, 0);
  } while (RxSmuMessageResponse == 0x0);

  /// @todo - implement S3 save for resume
  if (AccessFlags) {
    Address = IOD_SPACE (PhysicalDieId, ResponseAddress);
    HygonS3SaveScriptPciWrite (AccessWidth32, NbioPciAddress.AddressValue | 0xB8, &Address);
    RxSmuMessageResponseS3 = 0x1;
    DataMask = 0xFFFFFFFF;
    HygonS3SaveScriptPciPoll (AccessWidth32, NbioPciAddress.AddressValue | 0xBC, &RxSmuMessageResponseS3, &DataMask, 0xffffffff);
  }

  while (RxSmuMessageResponse == BIOSSMC_Result_FATAL) {
    IDS_HDT_CONSOLE (GNB_TRACE, "SMU Assert AccessFlags\n");
    ASSERT (FALSE);   // ASSERT if asserts are enabled
  }

  // 5 Read updated SMU message arguments
  SmuRegisterRead (NbioPciAddress, IOD_SPACE (PhysicalDieId, Arg0Address), &RequestArgument[0], 0);
  SmuRegisterRead (NbioPciAddress, IOD_SPACE (PhysicalDieId, Arg1Address), &RequestArgument[1], 0);
  SmuRegisterRead (NbioPciAddress, IOD_SPACE (PhysicalDieId, Arg2Address), &RequestArgument[2], 0);
  SmuRegisterRead (NbioPciAddress, IOD_SPACE (PhysicalDieId, Arg3Address), &RequestArgument[3], 0);
  SmuRegisterRead (NbioPciAddress, IOD_SPACE (PhysicalDieId, Arg4Address), &RequestArgument[4], 0);
  SmuRegisterRead (NbioPciAddress, IOD_SPACE (PhysicalDieId, Arg5Address), &RequestArgument[5], 0);

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioSmuServiceRequest Exit RequestId = 0x%x, Response = 0x%x\n", RequestId, RxSmuMessageResponse);
  return (RxSmuMessageResponse);
}

/*----------------------------------------------------------------------------------------*/

/**
 * SMU firmware test
 *
 *
 * @param[in]  NbioHandle     Pointer to GNB_HANDLE
 */
HGPI_STATUS
NbioSmuFirmwareTest (
  IN       GNB_HANDLE               *NbioHandle
  )
{
  UINT32  SmuArg[6];
  UINT32  RequestId;

  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioSmuFirmwareTest Enter\n");

  NbioSmuServiceCommonInitArguments (SmuArg);
  SmuArg[0] = 0xAA55AA55;
  RequestId = SMC_MSG_TestMessage;

  NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, RequestId, SmuArg, 0);

  IDS_HDT_CONSOLE (MAIN_FLOW, "  Service Test Return %x\n", SmuArg[0]);
  IDS_HDT_CONSOLE (MAIN_FLOW, "NbioSmuFirmwareTest Exit\n");

  if (SmuArg[0] == 0) {
    return HGPI_ERROR;
  }

  return HGPI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * HSIO firmware request - Common function
 *
 *
 * @param[in]      NbioHandle      Pointer to GNB_HANDLE
 * @param[in]      HsioMBoxRequest HSIO Mailbox Request ID
 * @param[in, out] HsioMBoxArg     SMU Argument structure - [0] entry reserved for HSIO Mailbox Request ID
 * @param[in]      AccessFlags     See GNB_ACCESS_FLAGS_* definitions
 */
UINT32
NbioHsioServiceRequest (
  IN       GNB_HANDLE               *NbioHandle,
  IN       UINT32                   PhysicalDieId,
  IN       UINT32                   HsioMBoxRequest,
  IN OUT   UINT32                   *HsioMBoxArg,
  IN       UINT32                   AccessFlags
  )
{
  UINT32  RetVal;

  *HsioMBoxArg = HsioMBoxRequest;
  RetVal = NbioSmuServiceRequest (
             NbioGetHostPciAddress (NbioHandle),
             PhysicalDieId,
             SMC_MSG_HsioTestMessage,
             HsioMBoxArg,
             AccessFlags
             );
  IDS_HDT_CONSOLE (
    GNB_TRACE,
    "Return Args = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, ",
    HsioMBoxArg[0],
    HsioMBoxArg[1],
    HsioMBoxArg[2],
    HsioMBoxArg[3],
    HsioMBoxArg[4],
    HsioMBoxArg[5]
    );

  return RetVal;
}

/*----------------------------------------------------------------------------------------*/

/**
 * SMU S3 Entry Notification
 *
 */
VOID
SmuNotifyS3Entry (
  )
{
  PCI_ADDR  NbioPciAddress;
  UINT32    RequestId;
  UINT32    RxSmuMessageResponse;
  UINT32    CpuModel;
  UINT32    ResponseAddress;
  UINT32    Arg0Address;
  UINT32    Arg1Address;
  UINT32    Arg2Address;
  UINT32    Arg3Address;
  UINT32    Arg4Address;
  UINT32    Arg5Address;
  UINT32    MsgAddress;

  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_EX_CPU) {
    ResponseAddress = MP1_C2PMSG_RESPONSE_ADDRESS_HYEX;
    Arg0Address = MP1_C2PMSG_ARGUMENT_0_ADDRESS_HYEX;
    Arg1Address = MP1_C2PMSG_ARGUMENT_1_ADDRESS_HYEX;
    Arg2Address = MP1_C2PMSG_ARGUMENT_2_ADDRESS_HYEX;
    Arg3Address = MP1_C2PMSG_ARGUMENT_3_ADDRESS_HYEX;
    Arg4Address = MP1_C2PMSG_ARGUMENT_4_ADDRESS_HYEX;
    Arg5Address = MP1_C2PMSG_ARGUMENT_5_ADDRESS_HYEX;
    MsgAddress = MP1_C2PMSG_MESSAGE_ADDRESS_HYEX;

  } else if (CpuModel == HYGON_GX_CPU) {
    ResponseAddress = MP1_C2PMSG_RESPONSE_ADDRESS_HYGX;
    Arg0Address = MP1_C2PMSG_ARGUMENT_0_ADDRESS_HYGX;
    Arg1Address = MP1_C2PMSG_ARGUMENT_1_ADDRESS_HYGX;
    Arg2Address = MP1_C2PMSG_ARGUMENT_2_ADDRESS_HYGX;
    Arg3Address = MP1_C2PMSG_ARGUMENT_3_ADDRESS_HYGX;
    Arg4Address = MP1_C2PMSG_ARGUMENT_4_ADDRESS_HYGX;
    Arg5Address = MP1_C2PMSG_ARGUMENT_5_ADDRESS_HYGX;
    MsgAddress = MP1_C2PMSG_MESSAGE_ADDRESS_HYGX;

  } else {
    IDS_HDT_CONSOLE (GNB_TRACE, "SmuNotifyS3Entry CPU model INVALID!!\n");
    return;
  }

  NbioPciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0);

  SmuRegisterRead (NbioPciAddress, IOD_SPACE (0, ResponseAddress), &RxSmuMessageResponse, 0);

  // 1 Clear Response
  RxSmuMessageResponse = 0;
  SmuRegisterWrite (NbioPciAddress, IOD_SPACE (0, ResponseAddress), &RxSmuMessageResponse, 0);

  // 2 Clear Message Arguments
  SmuRegisterWrite (NbioPciAddress, IOD_SPACE (0, Arg0Address), &RxSmuMessageResponse, 0);
  SmuRegisterWrite (NbioPciAddress, IOD_SPACE (0, Arg1Address), &RxSmuMessageResponse, 0);
  SmuRegisterWrite (NbioPciAddress, IOD_SPACE (0, Arg2Address), &RxSmuMessageResponse, 0);
  SmuRegisterWrite (NbioPciAddress, IOD_SPACE (0, Arg3Address), &RxSmuMessageResponse, 0);
  SmuRegisterWrite (NbioPciAddress, IOD_SPACE (0, Arg4Address), &RxSmuMessageResponse, 0);
  SmuRegisterWrite (NbioPciAddress, IOD_SPACE (0, Arg5Address), &RxSmuMessageResponse, 0);

  // 3 Write message ID
  RequestId = SMC_MSG_SleepEntry;
  SmuRegisterWrite (NbioPciAddress, IOD_SPACE (0, MsgAddress), &RequestId, 0);

  // Halt execution
  CpuSleep ();

  return;
}
