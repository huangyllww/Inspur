/* $NoKeywords:$ */

/**
 * @file
 *
 * Config Fch USB controller
 *
 * Init USB features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: FCH
 *
 */
/*;********************************************************************************
;
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
;*********************************************************************************/
#include "FchPlatform.h"
#include "Filecode.h"
#include <Library/IdsLib.h>
#include <Protocol/FabricTopologyServices.h>

extern  EFI_BOOT_SERVICES       *gBS;
#define FILECODE  PROC_FCH_TAISHAN_TSUSB_TSUSBLATE_FILECODE

/**
   xHC Workaroud when gBS->ExitBootServices() called. //add by zhengtao

  @param  Event                   Pointer to this event
  @param  Context                 Event handler private data

**/
VOID
EFIAPI
XhcExitBootServiceWA (
  EFI_EVENT  Event,
  VOID       *Context
  )

{
  UINT32                                   Data = 0;
  UINT32                                  PortNum = 0; 
  UINTN                                   NumberOfSockets; 
  UINTN                                   TotalNumberRootBridges; 
  UINTN                                   RootBridgesNumberPerIoDie;
  UINT32                                  SocketIndex;
  UINT32                                  UsbIndex;
  UINT32                                  NbioBusNum;
  UINTN                                   TotalNumberOfIoDie;
  EFI_STATUS                              Status;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *FabricTopology;

  Status = gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, (VOID **) &FabricTopology);
  if (EFI_ERROR (Status)) {
    return;
  }

  FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, &TotalNumberOfIoDie, &TotalNumberRootBridges);

  RootBridgesNumberPerIoDie = TotalNumberRootBridges / TotalNumberOfIoDie;

  for (SocketIndex = 0; SocketIndex < NumberOfSockets; SocketIndex++) {
    for (UsbIndex = 0; UsbIndex < RootBridgesNumberPerIoDie; UsbIndex++) {
      if(UsbIndex < 2 ){
        NbioBusNum = (UINT8)ReadSocDieBusNum (SocketIndex,0, UsbIndex);

        for(PortNum = 0; PortNum < 4; PortNum++){
          FchSmnRead (NbioBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB3_PORTSC_HYEX + 0x10 * PortNum), &Data, NULL);
          IDS_HDT_CONSOLE (MAIN_FLOW, "NbioBusNum=0x%x, xHC PORTSC%d = 0x%x before SetU3\n", NbioBusNum, PortNum, Data);

          if(Data & BIT1){
            FchSmnRW (NbioBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB3_PORTSC_HYEX + 0x10 * PortNum), 0xFFFFFFFD, 0x10060, NULL);
            FchStall (200, NULL);
            FchSmnRead (NbioBusNum, USB_SPACE_HYEX (UsbIndex, FCH_TS_USB3_PORTSC_HYEX + 0x10 * PortNum), &Data, NULL);
            IDS_HDT_CONSOLE (MAIN_FLOW, "NbioBusNum=0x%x, xHC PORTSC%d = 0x%x after SetU3\n", NbioBusNum, PortNum, Data);
          }
        }
      }
    }
  }
  gBS->CloseEvent (Event);
}

/**
 * FchInitLateUsb - Config USB controller before OS Boot
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitLateUsb (
  IN  VOID     *FchDataPtr
  )
{
  EFI_STATUS          Status;
  EFI_EVENT           ReadyToBootEvent;

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  XhcExitBootServiceWA,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &ReadyToBootEvent
                  );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "FchDxeInit ..xHCI CreateEventEx Fail..\n");
  } 
}
