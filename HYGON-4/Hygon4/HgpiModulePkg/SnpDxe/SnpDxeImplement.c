/* $NoKeywords */

/**
 * @file
 *
 * SnpDxeImplement.c
 *
 * Contains Ethernet Implement Driver for the DXE phase.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: FDK
 * @e sub-project: UEFI
 * @e version: $Revision: 300489 $ @e date: $Date: 2014-08-08 14:36:46 -0700 (Fri, 08 Aug 2014) $
 *
 */
/*****************************************************************************
*
* * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
*
* HYGON is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with HYGON. This header does *NOT* give you permission to use the Materials
* or any rights under HYGON's intellectual property. Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions. If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY: The Materials and all other information, identified as
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
* THE POSSIBILITY OF SUCH DAMAGES. BECAUSE SOME JURISDICTIONS PROHIBIT THE
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
* available to you. Additionally, HYGON retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
*****************************************************************************
*/

#include "SnpDxe.h"
#include <Library/PcdLib.h>

EFI_STATUS
ImplementStart (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  //
  // Set simple network state to Started and return success.
  //
  Private->Mode.State = EfiSimpleNetworkStarted;
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementStop (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  if ( Private->LinkStsTimerEvent != NULL) {
    gBS->CloseEvent (Private->LinkStsTimerEvent);
  }

  //
  // Set simple network state to Stopped and return success.
  //
  Private->Mode.State = EfiSimpleNetworkStopped;
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementInit (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  SetLanSpeed (Private);
  XgmacInit (Private, Private->xgmac_pdata.xgmac_regs);
  XpcsInit (Private);
  ConfigExtMiiRegs (Private);
  xgbe_an_run (Private);

  UpdateLinkStatus (Private);
  //
  // Set simple network state to Initialized and return success.
  //
  Private->Mode.State = EfiSimpleNetworkInitialized;
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementReset (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementShutdown (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  if ( Private->LinkStsTimerEvent != NULL) {
    gBS->SetTimer (Private->LinkStsTimerEvent, TimerCancel, 0);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ImplementRecvFilterEnable (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  Private->Mode.ReceiveFilterSetting = 1;

  return EFI_SUCCESS;
}

EFI_STATUS
ImplementRecvFilterDisable (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementRecvFilterRead (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementSetStnAddr (
  IN       ENET_DXE_PRIVATE_DATA *Private,
  IN       EFI_MAC_ADDRESS *NewMacAddr
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementStatistics (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementIp2Mac (
  IN       ENET_DXE_PRIVATE_DATA *Private,
  IN       BOOLEAN             IPv6,
  IN       EFI_IP_ADDRESS      *IP,
  IN       EFI_MAC_ADDRESS     *MAC
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementNvDataRead (
  IN       ENET_DXE_PRIVATE_DATA *Private,
  IN       UINTN      Offset,
  IN       UINTN      BufferSize,
  IN       VOID       *Buffer
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementGetStatus (
  IN       ENET_DXE_PRIVATE_DATA *Private,
  IN       UINT32     *InterruptStatusPtr,
  IN       VOID       **TransmitBufferListPtr
  )
{
  PXE_DB_GET_STATUS  *Db = NULL;

  if (TransmitBufferListPtr != NULL) {
    Db = Private->Db;
    if (Private->RecyclTxBufCount == 0) {
      *TransmitBufferListPtr = NULL;
      DEBUG ((EFI_D_ERROR, "RecyclTxBufCount == 0\n"));
    } else {
      DEBUG ((EFI_D_ERROR, "RecyclTxBufCount == %d \n",Private->RecyclTxBufCount));
      *TransmitBufferListPtr = (VOID *) (UINTN) Db->TxBuffer[Private->RecyclTxBufCount-1]; 
      Private->RecyclTxBufCount--;
    }
  }

  if (InterruptStatusPtr != NULL) {
    *InterruptStatusPtr  = EFI_SIMPLE_NETWORK_TRANSMIT_INTERRUPT;
    *InterruptStatusPtr |= EFI_SIMPLE_NETWORK_RECEIVE_INTERRUPT;
  }

  UpdateLinkStatus (Private);
  return EFI_SUCCESS;
}

EFI_STATUS
ImplementFillHeader (
  IN       ENET_DXE_PRIVATE_DATA *Private,
  IN       VOID            *MacHeaderPtr,
  IN       UINTN           HeaderSize,
  IN       VOID            *Buffer,
  IN       UINTN           BufferSize,
  IN       EFI_MAC_ADDRESS *DestAddr,
  IN       EFI_MAC_ADDRESS *SrcAddr,
  IN       UINT16          *ProtocolPtr
  )
{
  ETHER_HEAD  *EnetHeader = NULL;

  if (SrcAddr == NULL) {
    SrcAddr = &Private->Mode.CurrentAddress;
  }

  EnetHeader = (ETHER_HEAD *)MacHeaderPtr;

  CopyMem (EnetHeader->DstMac, DestAddr, NET_ETHER_ADDR_LEN);
  CopyMem (EnetHeader->SrcMac, SrcAddr, NET_ETHER_ADDR_LEN);

  EnetHeader->EtherType = HTONS (*ProtocolPtr);

  return EFI_SUCCESS;
}

EFI_STATUS
ImplementTransmit (
  IN       ENET_DXE_PRIVATE_DATA *Private,
  IN       VOID       *Buffer,
  IN       UINTN      BufferSize
  )
{
  // int i;
  // unsigned char* buf;

  if (BufferSize > ENET_MAX_PACKET_SIZE) {
    return EFI_OUT_OF_RESOURCES;
  }

  if ((BufferSize == 0) || (Buffer == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

 #if 0
    buf = (unsigned char *)Buffer;
    DEBUG ((EFI_D_ERROR, "tx packet:\n"));
    for(i = 0; i < BufferSize; i++) {
      DEBUG ((EFI_D_ERROR, "%x ", buf[i]));
      if((i+1)%16 == 0) {
        DEBUG ((EFI_D_ERROR, "\n"));
      }
    }

 #endif
  return xgmac_pre_xmit (Private, Buffer, BufferSize);
}

EFI_STATUS
ImplementReceive (
  IN       ENET_DXE_PRIVATE_DATA *Private,
  IN       VOID            *Buffer,
  IN       UINTN           *BufferSize,
  IN       UINTN           *HeaderSize,
  IN       EFI_MAC_ADDRESS *SrcAddr,
  IN       EFI_MAC_ADDRESS *DestAddr,
  IN       UINT16          *Protocol
  )
{
  EFI_STATUS   Status = EFI_NOT_READY;
  ENET_HEADER  *EnetHeader = NULL;

  Status = xgmac_dev_read (Private, Buffer, BufferSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (HeaderSize != NULL) {
    *HeaderSize = sizeof (ENET_HEADER);
  }

  EnetHeader = (ENET_HEADER *)Buffer;

  if (SrcAddr != NULL) {
    ZeroMem (SrcAddr, sizeof (EFI_MAC_ADDRESS));
    CopyMem (SrcAddr->Addr, EnetHeader->SrcAddr, ENET_ADDR_LEN);
  }

  if (DestAddr != NULL) {
    ZeroMem (DestAddr, sizeof (EFI_MAC_ADDRESS));
    CopyMem (DestAddr->Addr, EnetHeader->DstAddr, ENET_ADDR_LEN);
  }

  if (Protocol != NULL) {
    *Protocol = NTOHS (EnetHeader->Type);
  }

  Status = EFI_SUCCESS;

  return Status;
}
