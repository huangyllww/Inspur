/*++
==========================================================================================
      NOTICE: Copyright (c) 2006 - 2009 Byosoft Corporation. All rights reserved.
              This program and associated documentation (if any) is furnished
              under a license. Except as permitted by such license,no part of this
              program or documentation may be reproduced, stored divulged or used
              in a public system, or transmitted in any form or by any means
              without the express written consent of Byosoft Corporation.
==========================================================================================
Module Name:
  UsbMouse.c

Abstract:
  USB Module file.

Revision History:
  ----------------------------------------------------------------------------------------
  Rev   Date        Name    Description
  ----------------------------------------------------------------------------------------
  ----------------------------------------------------------------------------------------
--*/

//
// Driver Produced Protocol Prototypes
//
#include <Protocol/UsbPolicy.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/ReportStatusCodeLib.h>
#include "UsbMouse.h"
#include "../UsbLib/hid.h"
#include "../UsbLib/UsbLib.h"
#include "../UsbBus/UsbBus.h"
#include "../UsbVirtualKbc/UsbVirtualKbc.h"

#define   EBDA_SEG_ADDRESS                      0x40E
#define   EBDA_DATABUFFER_START_ADD             0x28
#define   EBDA_MOUSE_RMOTECALL_OFFSET           0x22

extern BOOLEAN         mLegacyFreeSupport;
UINT32  gEbdaAddress;
STATIC  UINT8 gDataIndex = 0;
extern  USB_VIRTUAL_KBC_DEV *mVirtualKBCDev;
extern USB_MOUSE_COMMUNICATION_PROTOCOL   *mUsbMouseCommunicationInstance;
/*
 _________________________________________________________________________________

 Wait for input buffer empty.
 _________________________________________________________________________________
*/
BOOLEAN
WaitKbcIBFull (VOID)
{
    UINT8 KbdCmd;
    UINT16 TimeOut;

    for (TimeOut = 0; TimeOut < MS_IN_EMPTY_COUNTER; TimeOut++) {
        KbdCmd = SmmKbcIoRead8 (KBC_CMD);
        if ((KbdCmd & KBC_IBE) == KBC_IBE)
            return TRUE;
        Stall (4);
    }
    return FALSE;
}

/*
 _________________________________________________________________________________

 Wait for output buffer full.
 _________________________________________________________________________________
*/
BOOLEAN
WaitKbcIBFEmpty (VOID)
{
    UINT8 KbdCmd;
    UINT16 TimeOut;

    //
    // BUGBUG ac 20090521 : The default timeout value for waiting IBE bit to
    //                      be cleared is too short on some platform KB
    //                      controller. Required more fine tune.
    //
    //  for (TimeOut = 0; TimeOut < MS_IN_EMPTY_COUNTER; TimeOut++) {
    //

    for (TimeOut = 0; TimeOut < 300000; TimeOut++) {
        KbdCmd = SmmKbcIoRead8(KBC_CMD);
        if ((KbdCmd & KBC_IBE) == 0) {
            return TRUE;
        }
        Stall (4);
    }

    return FALSE;
}

BOOLEAN
WaitKbcOBFEmpty (VOID)
{
    UINT8 KbdCmd;
    UINT16 TimeOut;

    for (TimeOut = 0; TimeOut < MS_IN_EMPTY_COUNTER; TimeOut++) {
        KbdCmd = SmmKbcIoRead8 (KBC_CMD);
        if ((KbdCmd & KBC_OBF) == 0) {
            return TRUE;
        }
        Stall (4);
    }
    return FALSE;
}

UINT32
GetEbdaAddress(
VOID
)
{
  UINT16               EbdaSeg;
  UINT32               EbdaPtr;

  EbdaSeg =*(UINT16*) EBDA_SEG_ADDRESS;
  EbdaPtr = (UINT32)EbdaSeg;
  EbdaPtr = EbdaPtr << 4;
  return EbdaPtr;
}

VOID
WriteDataToEbdaForMouse(
   IN  UINT8  Offset,
   IN  UINT8  Value
)
{
  (*(UINT8 *)(UINTN)(gEbdaAddress+EBDA_DATABUFFER_START_ADD+Offset)) = Value;
}

STATIC
EFI_STATUS
SendMouseData(
    IN VOID         *Data,
    IN UINTN        DataLength,
    IN VOID         *Context,
    IN UINT32       Status
)
{
    USB_MOUSE_DEV         *UsbMouseDevice;
    EFI_USB_IO_PROTOCOL   *UsbIo;
    MS_CTRL_DATA          *MsCtrl;
    UINT8                 TempData;
    UINT8                 TempCmd;
    if (mLegacyFreeSupport) {
      gEbdaAddress = GetEbdaAddress();
    }
    MsCtrl = (MS_CTRL_DATA *) Context;
    UsbMouseDevice = USB_MOUSE_DEV_FROM_CONTROL_DATA (MsCtrl);
    UsbIo = UsbMouseDevice->UsbIo;
    if (MsCtrl->QueueIndex == 0) {
        goto ON_EXIT;
    }
  if (!mLegacyFreeSupport) {

    TempData = SmmKbcIoRead8 (KBC_CMD);

    //ANC-20200729: only need to check output buffer status for mouse data,BFT : CHX002-202007074
    if ((TempData & KBC_OBF) == KBC_OBF) {
        MsCtrl->QueueIndex =0;
        goto ON_EXIT;
    }
    if (MsCtrl->Kbcstatus == 0) {
      if (mVirtualKBCDev) {
        MsCtrl->Kbcstatus = mVirtualKBCDev->VirtualKbcData.KBCCmdReg;
      } else {
        MsCtrl->Kbcstatus = KBC_CMD_INT | KBC_CMD_INT2 | KBC_CMD_SYS;
      }
    }
    if ((MsCtrl->Kbcstatus & KBC_CMD_INT12) == 0) {
        MsCtrl->QueueIndex = 0;
        goto ON_EXIT;
    }
    /*
    if ((MsCtrl->Kbcstatus & KBC_MS_INTERFACE) == KBC_MS_INTERFACE) {
    MsCtrl->QueueIndex = 0;
    goto ON_EXIT;
    }
    */
    TempCmd = MsCtrl->Queue[MsCtrl->QueueIndex].Cmd;
    if (TempCmd == KBC_DISABLE_MOUSE) {
        SmmKbcIoWrite8 (KBC_CMD, TempCmd);
        --MsCtrl->QueueIndex;
        goto ON_EXIT;
    }

    //send mouse data
    if (!((TempCmd == KBC_DISABLE_MOUSE) || (TempCmd == KBC_ENABLE_MOUSE))) {
    SmmKbcIoWrite8 (KBC_CMD, TempCmd);

        TempData = MsCtrl->Queue[MsCtrl->QueueIndex].Data;
        SmmKbcIoWrite8 (KBC_DATA, TempData);
        --MsCtrl->QueueIndex;
        goto ON_EXIT;
    }

    //send enable mouse command
    if (TempCmd == KBC_ENABLE_MOUSE) {
        SmmKbcIoWrite8 (KBC_CMD, TempCmd);
        --MsCtrl->QueueIndex;

        goto ON_EXIT;
    }
  } else {
    //
    //if Legacy free support
    //
    TempCmd = MsCtrl->Queue[MsCtrl->QueueIndex].Cmd;
    if (TempCmd == KBC_DISABLE_MOUSE) {
      --MsCtrl->QueueIndex;
    }

    TempCmd = MsCtrl->Queue[MsCtrl->QueueIndex].Cmd;
    if (!(TempCmd == KBC_DISABLE_MOUSE) || (TempCmd == KBC_ENABLE_MOUSE)) {
      TempData = MsCtrl->Queue[MsCtrl->QueueIndex].Data;
      //
      //according  gDataIndex to decide insert the data to EBDA +28h.
      //
      WriteDataToEbdaForMouse(gDataIndex,TempData);
      gDataIndex++;
      if (gDataIndex == 3) {
        gDataIndex = 0;
      }
      --MsCtrl->QueueIndex;
    }

    TempCmd = MsCtrl->Queue[MsCtrl->QueueIndex].Cmd;
    if (TempCmd == KBC_ENABLE_MOUSE) {
      --MsCtrl->QueueIndex;
    }
  }

ON_EXIT:
    if (MsCtrl->QueueIndex == 0) {
        if ((MsCtrl->Kbcstatus != 0) && ((MsCtrl->Kbcstatus & KBC_MS_INTERFACE) == 0))
            SmmKbcIoWrite8 (KBC_CMD, KBC_ENABLE_MOUSE);

        MsCtrl->Kbcstatus = 0;
        UsbIo->UsbAsyncInterruptOnlyTransfer (
            UsbIo,
            FALSE,
            USBMS_KBC_CMD_INTERVAL,
            SendMouseData,
            MsCtrl
        );
        UsbMouseDevice->QueuePollingActive = FALSE;
    }

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
OnMouseInterruptComplete (
    IN  VOID          *Data,
    IN  UINTN         DataLength,
    IN  VOID          *Context,
    IN  UINT32        Result
)
/*++

  Routine Description:
    Handler function for USB mouse's asynchronous interrupt transfer.

  Arguments:
    Data       A pointer to a buffer that is filled with key data which is
               retrieved via asynchronous interrupt transfer.
    DataLength Indicates the size of the data buffer.
    Context    Pointing to USB_KB_DEV instance.
    Result     Indicates the result of the asynchronous interrupt transfer.

  Returns:
    EFI_SUCCESS      - Success
    EFI_DEVICE_ERROR - Hardware Error
--*/
{
    USB_MOUSE_DEV       *UsbMouseDevice;
    EFI_USB_IO_PROTOCOL *UsbIo;
    UINT8               *CurKeyCodeBuffer;
    UINT8               TempData;
    UINT32              UsbResult;
    MS_CTRL_DATA        *MsDataPtr;
    BOOT_CFG            *BootCfg;    
    UINT32              CurrentMovementX;
    UINT32              CurrentMovementY;


    ASSERT (Context);
    UsbMouseDevice = (USB_MOUSE_DEV *) Context;
    UsbIo          = UsbMouseDevice->UsbIo;

    //
    // Analyzes the Result and performs corresponding action.
    //
    if (Result != EFI_USB_NOERROR) {
        if ((Result & EFI_USB_ERR_STALL) == EFI_USB_ERR_STALL) {
            UsbClearEndpointHalt (
                UsbIo,
                UsbMouseDevice->IntEndpointDescriptor.EndpointAddress,
                &UsbResult
            );
        }

        UsbIo->UsbAsyncInterruptTransfer (
            UsbIo,
            UsbMouseDevice->IntEndpointDescriptor.EndpointAddress,
            FALSE,
            0,
            0,
            NULL,
            NULL
        );
        return EFI_DEVICE_ERROR;
    }

    if (DataLength == 0 || Data == NULL) {
        return EFI_SUCCESS;
    }

    //
    // Skip this packet if mouse interface is disabled.
    //
    /* mVirtualKBCDev may be NULL, when disable legacy free driver
    if (mVirtualKBCDev == NULL) {
        return EFI_SUCCESS;
    }
    */
    CurKeyCodeBuffer = (UINT8 *) Data;
    MsDataPtr = &UsbMouseDevice->MsCtrlData;
    // mUsbMouseCommunicationInstance may be NULL, when PcdTextSetupMouseEnable is false.
    if (mUsbMouseCommunicationInstance != NULL) {
      mUsbMouseCommunicationInstance->MouseDataSize = (UINT8)DataLength;
      mUsbMouseCommunicationInstance->XLogicMax = UsbMouseDevice->XLogicMax;
      mUsbMouseCommunicationInstance->XLogicMin = UsbMouseDevice->XLogicMin;
      mUsbMouseCommunicationInstance->YLogicMax = UsbMouseDevice->YLogicMax;
      mUsbMouseCommunicationInstance->YLogicMin = UsbMouseDevice->YLogicMin;
      mUsbMouseCommunicationInstance->ZLogicMax = UsbMouseDevice->ZLogicMax;
      mUsbMouseCommunicationInstance->ZLogicMin = UsbMouseDevice->ZLogicMin;
      mUsbMouseCommunicationInstance->IsAbsMouse= UsbMouseDevice->IsAbsMouse;
    }

    GET_BOOT_CFG(BootCfg);
    if (mUsbMouseCommunicationInstance != NULL && BootCfg->BootStart == 0){
      if ((mUsbMouseCommunicationInstance->MouseDataStatus & BIT0) == 0x00){
        if((DataLength == 3) || (DataLength == 4)){
        mUsbMouseCommunicationInstance->CurrentMouseData0[USB_MS_BTN_DATA] = CurKeyCodeBuffer[USB_MS_BTN_DATA];
        mUsbMouseCommunicationInstance->CurrentMouseData0[USB_MS_X_DATA] = CurKeyCodeBuffer[USB_MS_X_DATA];
        mUsbMouseCommunicationInstance->CurrentMouseData0[USB_MS_Y_DATA] = CurKeyCodeBuffer[USB_MS_Y_DATA];
        mUsbMouseCommunicationInstance->MouseDataStatus |= BIT0;
        }
        if((DataLength == 5) || (DataLength == 6)){
            if(UsbMouseDevice->IsAbsMouse){
                mUsbMouseCommunicationInstance->CurrentMouseData0[USB_MS_BTN_DATA] = CurKeyCodeBuffer[USB_MS_BTN_DATA];

                CurrentMovementX = (UINT32)((CurKeyCodeBuffer[USB_MS_Y_DATA]<<8)|CurKeyCodeBuffer[USB_MS_X_DATA]);
                mUsbMouseCommunicationInstance->CurrentMouseData3[0] = (UINT16)CurrentMovementX;

                CurrentMovementY = (UINT32)((CurKeyCodeBuffer[4]<<8)|CurKeyCodeBuffer[3]);
                mUsbMouseCommunicationInstance->CurrentMouseData3[1] = (UINT16)CurrentMovementY;

                mUsbMouseCommunicationInstance->MouseDataStatus |= BIT0;
            }
        }
      } else {
        if ((CurKeyCodeBuffer[USB_MS_BTN_DATA] & PS2_MOUSE_ALLBTN) != (mUsbMouseCommunicationInstance->CurrentMouseData0[USB_MS_BTN_DATA] & PS2_MOUSE_ALLBTN)){
          mUsbMouseCommunicationInstance->CurrentMouseData1[USB_MS_BTN_DATA] = CurKeyCodeBuffer[USB_MS_BTN_DATA];
          mUsbMouseCommunicationInstance->CurrentMouseData1[USB_MS_X_DATA] = 0;
          mUsbMouseCommunicationInstance->CurrentMouseData1[USB_MS_Y_DATA] = 0;
          mUsbMouseCommunicationInstance->MouseDataStatus |= BIT1;
          mUsbMouseCommunicationInstance->MouseDataStatus |= BIT7;
        }
      }
      goto ProcExit;
    }

    
    if (MsDataPtr->QueueIndex == 0) {

        MsDataPtr->QueueIndex = 1;
        MsDataPtr->Queue[MsDataPtr->QueueIndex].Cmd = KBC_ENABLE_MOUSE;

        ++MsDataPtr->QueueIndex;
        MsDataPtr->Queue[MsDataPtr->QueueIndex].Cmd = KBC_SEND_MOUSE_DATA;
        MsDataPtr->Queue[MsDataPtr->QueueIndex].Data = (~(CurKeyCodeBuffer[USB_MS_Y_DATA]) + 1);

        ++MsDataPtr->QueueIndex;
        MsDataPtr->Queue[MsDataPtr->QueueIndex].Cmd = KBC_SEND_MOUSE_DATA;
        MsDataPtr->Queue[MsDataPtr->QueueIndex].Data = CurKeyCodeBuffer[USB_MS_X_DATA];

        ++MsDataPtr->QueueIndex;
        MsDataPtr->Queue[MsDataPtr->QueueIndex].Cmd = KBC_SEND_MOUSE_DATA;
        TempData = CurKeyCodeBuffer[USB_MS_BTN_DATA];
        TempData &= PS2_MOUSE_ALLBTN;
        TempData |= PS2_MOUSE_DATA;
        if (((~(CurKeyCodeBuffer[USB_MS_Y_DATA]) + 1) & PS2_MOUSE_XYSIGN) == PS2_MOUSE_XYSIGN)
            TempData |= PS2_MOUSE_YSIGN;

        if ((CurKeyCodeBuffer[USB_MS_X_DATA] & PS2_MOUSE_XYSIGN) == PS2_MOUSE_XYSIGN)
            TempData |= PS2_MOUSE_XSIGN;

        MsDataPtr->Queue[MsDataPtr->QueueIndex].Data = TempData;

        ++MsDataPtr->QueueIndex;
        MsDataPtr->Queue[MsDataPtr->QueueIndex].Cmd = KBC_DISABLE_MOUSE;

        if (!UsbMouseDevice->QueuePollingActive) {
            UsbIo->UsbAsyncInterruptOnlyTransfer (
                UsbIo,
                TRUE,
                USBMS_KBC_CMD_INTERVAL,
                SendMouseData,
                &UsbMouseDevice->MsCtrlData
            );
            UsbMouseDevice->QueuePollingActive = TRUE;
        }
    }


ProcExit:
    return EFI_SUCCESS;
}

//
// Get an item from report descriptor
//
STATIC
UINT8 *
GetNextItem (
    IN  UINT8    *StartPos,
    IN  UINT8    *EndPos,
    OUT HID_ITEM *HidItem
)
/*++

Routine Description:

  Get Next Item

Arguments:

  StartPos  - Start Position
  EndPos    - End Position
  HidItem   - HidItem to return

Returns:
  Position

--*/
{
    UINT8 Temp;

    if (EndPos <= StartPos) {
      return NULL;
    }

    Temp = *StartPos;
    StartPos++;
    //
    // Bit format of prefix byte:
    // Bits 0-1: Size
    // Bits 2-3: Type
    // Bits 4-7: Tag
    //
    HidItem->Type = BitFieldRead8 (Temp, 2, 3);
    HidItem->Tag  = BitFieldRead8 (Temp, 4, 7);

    if (HidItem->Tag == HID_ITEM_TAG_LONG) {
        //
        // Long Items are not supported by HID rev1.0,
        // although we try to parse it.
        //
        HidItem->Format = HID_ITEM_FORMAT_LONG;

        if ((EndPos - StartPos) >= 2) {
            HidItem->Size = *StartPos++;
            HidItem->Tag  = *StartPos++;

            if ((EndPos - StartPos) >= HidItem->Size) {
                HidItem->Data.LongData = StartPos;
                StartPos += HidItem->Size;
                return StartPos;
            }
        }
    } else {
        HidItem->Format = HID_ITEM_FORMAT_SHORT;
        HidItem->Size   = BitFieldRead8 (Temp, 0, 1);
        switch (HidItem->Size) {

        case 0:
            //
            // No data
            //
            return StartPos;

        case 1:
            //
            // One byte data
            //
            if ((EndPos - StartPos) >= 1) {
                HidItem->Data.U8 = *StartPos++;
                return StartPos;
            }

        case 2:
            //
            // Two byte data
            //
            if ((EndPos - StartPos) >= 2) {
                CopyMem (&HidItem->Data.U16, StartPos, sizeof (UINT16));
                StartPos += 2;
                return StartPos;
            }

        case 3:
            //
            // 4 byte data, adjust size
            //
            HidItem->Size = 4;
            if ((EndPos - StartPos) >= 4) {
                CopyMem (&HidItem->Data.U32, StartPos, sizeof (UINT32));
                StartPos += 4;
                return StartPos;
            }
        }
    }

    return NULL;
}

STATIC
UINT32
GetItemData (
    IN  HID_ITEM *HidItem
)
/*++

Routine Description:

  Get Item Data

Arguments:

  HidItem - HID_ITEM

Returns:
  HidItem Data


--*/
{
    //
    // Get Data from HID_ITEM structure
    //
    switch (HidItem->Size) {

    case 1:
        return HidItem->Data.U8;

    case 2:
        return HidItem->Data.U16;

    case 4:
        return HidItem->Data.U32;
    }

    return 0;
}

UINT8 mUsagePage = 0;

EFI_STATUS
ParseMouseReportDescriptor (
    IN  USB_MOUSE_DEV   *UsbMouse,
    IN  UINT8           *ReportDescriptor,
    IN  UINTN           ReportSize
)

{
    UINT32     Data;
    UINT8      *DescriptorEnd;
    UINT8      *ptr;
    HID_ITEM   HidItem;
    UINT8      XY;
    UINT8      Wheel;

    DescriptorEnd = ReportDescriptor + ReportSize;
    XY=Wheel=0;

    ptr           = GetNextItem (ReportDescriptor, DescriptorEnd, &HidItem);

    while (ptr != NULL) {
        if (HidItem.Format != HID_ITEM_FORMAT_SHORT) {
            //
            // Long Item is not supported at current HID revision
            //
            return EFI_UNSUPPORTED;
        }
        Data = GetItemData (&HidItem);
        //ParseHidItem (UsbMouse, &HidItem);

        //
        // Main Type
        //
        if (HidItem.Type == HID_ITEM_TYPE_MAIN ){
            switch (HidItem.Tag)
            {
            case HID_MAIN_ITEM_TAG_INPUT:
                /**
                        Data Value description
                +======+==================+==============+
                |      |         0        |      1       |
                +======+==================+==============+
                | BIT0 | Data             | Constant     |
                | BIT1 | Array            | Variable     |
                | BIT2 | Absolute         | Relative     |
                | BIT3 | No Warp          | Warp         |
                | BIT4 | Linear           | Non Linear   |
                | BIT5 | Preferred State  | No Preferred |
                | BIT6 | No Null Position | Null State   |
                | BIT7 | Non Volatile     | Volatile     |
                +======+==================+==============+
                **/
                if(XY == mUsagePage){
                    if(Data & BIT2){
                        UsbMouse->IsAbsMouse = FALSE;
                    }else{
                        UsbMouse->IsAbsMouse = TRUE;
                    }
                }

                XY++;//Distinguish wheel data
                break;
            
            default:
                break;
            }
        }

        //
        // Global Type
        //
        if (HidItem.Type == HID_ITEM_TYPE_GLOBAL ){
            switch(HidItem.Tag){
                case HID_GLOBAL_ITEM_TAG_USAGE_PAGE :
                    mUsagePage++;
                    if(Data == 0x09){
                      // Button Page
                      UsbMouse->PrivateData.ButtonDetected = TRUE;
                    }
                    break;
                case HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM :
                    if(XY == mUsagePage){
                        UsbMouse->XLogicMin = Data;
                        UsbMouse->YLogicMin = Data;
                    }
                    if(Wheel == mUsagePage){
                        UsbMouse->ZLogicMin = Data;
                    }
                    break;
                case HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM :
                    if(XY == mUsagePage){
                        UsbMouse->XLogicMax = Data;
                        UsbMouse->YLogicMax = Data;
                    }
                    if(Wheel == mUsagePage){
                        UsbMouse->ZLogicMax = Data;
                    }
                    break;
                case HID_GLOBAL_ITEM_TAG_REPORT_SIZE :
                    if(XY == mUsagePage){
                        UsbMouse->XYReportSize = (UINT8)Data;
                    }
                case HID_GLOBAL_ITEM_TAG_REPORT_ID :
                case HID_GLOBAL_ITEM_TAG_REPORT_COUNT :
                default:
                    break;
            }
        }

        //
        // Local Type
        //
        if(HidItem.Type == HID_ITEM_TYPE_LOCAL){
            switch(HidItem.Tag){
                case HID_LOCAL_ITEM_TAG_USAGE :
                    if(Data == 0x30 || Data == 0x31){
                        XY = mUsagePage;
                    }
                    if(Data == 0x38){
                        Wheel = mUsagePage;
                    }
                    break;
                case HID_LOCAL_ITEM_TAG_USAGE_MINIMUM :
                    if (UsbMouse->PrivateData.ButtonDetected) {
                        UsbMouse->PrivateData.ButtonMinIndex = (UINT8) Data;
                    }
                    break;
                case HID_LOCAL_ITEM_TAG_USAGE_MAXIMUM :
                    if (UsbMouse->PrivateData.ButtonDetected) {
                        UsbMouse->PrivateData.ButtonMaxIndex = (UINT8) Data;
                    }
                    break;
            }
        }

        ptr = GetNextItem (ptr, DescriptorEnd, &HidItem);
    }

    UsbMouse->NumberOfButtons                 = (UINT8) (UsbMouse->PrivateData.ButtonMaxIndex - UsbMouse->PrivateData.ButtonMinIndex + 1);

    return EFI_SUCCESS;
}

STATIC
EFI_STATUS
InitializeUsbMouseDevice (
    IN  USB_MOUSE_DEV           *UsbMouseDev
)
/*++

  Routine Description:
    Initialize the Usb Mouse Device.

  Arguments:
    UsbMouseDev         - Device instance to be initialized

  Returns:
    EFI_SUCCESS         - Success
    EFI_DEVICE_ERROR    - Init error.
    EFI_OUT_OF_RESOURCES- Can't allocate memory
--*/
{
    EFI_USB_IO_PROTOCOL     *UsbIo;
    UINT8                   Protocol;
    EFI_STATUS              Status;
    EFI_USB_HID_DESCRIPTOR  MouseHidDesc;
    UINT8                   *ReportDesc;

    UsbIo = UsbMouseDev->UsbIo;

    //
    // Get HID descriptor
    //
    Status = UsbGetHidDescriptor (
                 UsbIo,
                 UsbMouseDev->InterfaceDescriptor.InterfaceNumber,
                 &MouseHidDesc
             );

    if (EFI_ERROR (Status)) {
        return Status;
    }

    //
    // Get Report descriptor
    //
    if (MouseHidDesc.HidClassDesc[0].DescriptorType != 0x22) {
        return EFI_UNSUPPORTED;
    }

    ReportDesc = AllocateZeroPool (
                     MouseHidDesc.HidClassDesc[0].DescriptorLength
                 );
    if (ReportDesc == NULL) {
        return EFI_OUT_OF_RESOURCES;
    }

    Status = UsbGetReportDescriptor (
                 UsbIo,
                 UsbMouseDev->InterfaceDescriptor.InterfaceNumber,
                 MouseHidDesc.HidClassDesc[0].DescriptorLength,
                 ReportDesc
             );

    if (EFI_ERROR (Status)) {
        FreePool (ReportDesc);
        return Status;
    }

    //
    // Parse report descriptor
    //
    Status = ParseMouseReportDescriptor (
                 UsbMouseDev,
                 ReportDesc,
                 MouseHidDesc.HidClassDesc[0].DescriptorLength
             );

    if (EFI_ERROR (Status)) {
        FreePool (ReportDesc);
        return Status;
    }

    //
    // Here we just assume interface 0 is the mouse interface
    //
    UsbGetProtocolRequest (
        UsbIo,
        UsbMouseDev->InterfaceDescriptor.InterfaceNumber,
        &Protocol
    );

    if (Protocol != BOOT_PROTOCOL) {
        Status = UsbSetProtocolRequest (
                     UsbIo,
                     UsbMouseDev->InterfaceDescriptor.InterfaceNumber,
                     BOOT_PROTOCOL
                 );

        if (EFI_ERROR (Status)) {
            FreePool (ReportDesc);
            return EFI_DEVICE_ERROR;
        }
    }

    //
    // Set indefinite Idle rate for USB Mouse
    //
    UsbSetIdleRequest (
        UsbIo,
        UsbMouseDev->InterfaceDescriptor.InterfaceNumber,
        0,
        0
    );

    FreePool (ReportDesc);

    return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
UsbMouseReset (
    IN  USB_MOUSE_DEV       *UsbMouseDevice,
    IN  BOOLEAN             ExtendedVerification
)
/*++

  Routine Description:
    Reset the mouse device, see SIMPLE POINTER PROTOCOL.

  Arguments:
    This                  - Protocol instance pointer.
    ExtendedVerification  - Ignored here/

  Returns:
    EFI_SUCCESS

--*/
{
    EFI_STATUS          Status;
    EFI_USB_IO_PROTOCOL *UsbIo;

    UsbIo           = UsbMouseDevice->UsbIo;

    UsbMouseDevice->StateChanged = FALSE;
    UsbMouseDevice->QueuePollingActive = FALSE;

    if (!ExtendedVerification) {
        return EFI_SUCCESS;
    }

    //
    // Exhaustive reset
    //
    Status = InitializeUsbMouseDevice (UsbMouseDevice);
    if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}

VOID
UsbUpdatedBdaMouseFlag (
    IN BOOLEAN Flag
)
{
    UINT8 *BdaInstallHardware;

    BdaInstallHardware = (UINT8 *)(UINTN)BDA_HARDWARE;
    *BdaInstallHardware |= BDA_HARDWARE_BIT_MOUSE;
}

EFI_STATUS
EFIAPI
UsbConnectMouseDriver (
    IN USB_INTERFACE        *UsbIf
)
/*++

  Routine Description:
    Start.

  Arguments:
    This       - EFI_DRIVER_BINDING_PROTOCOL
    Controller - Controller handle
    RemainingDevicePath - EFI_DEVICE_PATH_PROTOCOL
  Returns:
    EFI_SUCCESS          - Success
    EFI_OUT_OF_RESOURCES - Can't allocate memory
    EFI_UNSUPPORTED      - The Start routine fail
--*/
{
    EFI_STATUS                    Status;
    EFI_USB_IO_PROTOCOL           *UsbIo;
    USB_MOUSE_DEV                 *UsbMouseDevice;
    EFI_USB_ENDPOINT_DESCRIPTOR   EndpointDescriptor;
    UINT8                         EndpointNumber;
    UINT8                         Index;
    UINT8                         EndpointAddr;
    UINT8                         PollingInterval;
    UINT8                         PacketSize;
    BOOLEAN                       Found;
    USB_CFG                       *UsbCfg;

    UsbMouseDevice = NULL;
    Found          = FALSE;

    UsbIo = &UsbIf->UsbIo;
    UsbMouseDevice = AllocateZeroPool (sizeof (USB_MOUSE_DEV));
    if (UsbMouseDevice == NULL) {
        return EFI_OUT_OF_RESOURCES;
    }

    //
    // Initialize UsbKeyboardDevice
    //
    UsbMouseDevice->UsbIo = UsbIo;
    UsbIf->DriverContext[UsbIf->LunCount] = UsbMouseDevice;
    UsbIf->LunCount++;

    //
    // Get interface & endpoint descriptor
    //
    UsbIo->UsbGetInterfaceDescriptor (
        UsbIo,
        &UsbMouseDevice->InterfaceDescriptor
    );

    EndpointNumber = UsbMouseDevice->InterfaceDescriptor.NumEndpoints;

    for (Index = 0; Index < EndpointNumber; Index++) {
        UsbIo->UsbGetEndpointDescriptor (
            UsbIo,
            Index,
            &EndpointDescriptor
        );

        if (((EndpointDescriptor.Attributes & (BIT0 | BIT1)) == USB_ENDPOINT_INTERRUPT) &&
            ((EndpointDescriptor.EndpointAddress & USB_ENDPOINT_DIR_IN) != 0)) {
            //
            // We only care interrupt endpoint here
            //
            CopyMem(&UsbMouseDevice->IntEndpointDescriptor, &EndpointDescriptor, sizeof(EndpointDescriptor));
            Found = TRUE;
            break;
        }
    }

    if (!Found) {
        //
        // Report Status Code to indicate that there is no USB mouse
        //
        REPORT_STATUS_CODE (
         EFI_ERROR_CODE | EFI_ERROR_MINOR,
         (EFI_PERIPHERAL_MOUSE | EFI_P_EC_NOT_DETECTED)
         );
        // No interrupt endpoint found, then return unsupported.
        //
        FreePool(UsbMouseDevice);
        return EFI_UNSUPPORTED;
    }

    UsbMouseDevice->Signature = USB_MOUSE_DEV_SIGNATURE;

    //
    // Set USB Configuration Information
    //
    GET_USB_CFG(UsbCfg);

    UsbCfg->UsbDeviceInfor.HasUSBMouse = 1;

    //
    // Reset USB Mouse Device
    //
    Status = UsbMouseReset (
                 UsbMouseDevice,
                 TRUE
             );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    //
    // submit async interrupt transfer
    //
    EndpointAddr    = UsbMouseDevice->IntEndpointDescriptor.EndpointAddress;
    PollingInterval = USBMS_POLLING_INTERVAL;
    PacketSize      = (UINT8) (UsbMouseDevice->IntEndpointDescriptor.MaxPacketSize);

    Status = UsbIo->UsbAsyncInterruptTransfer (
                 UsbIo,
                 EndpointAddr,
                 TRUE,
                 PollingInterval,
                 PacketSize,
                 OnMouseInterruptComplete,
                 UsbMouseDevice
             );

    if (EFI_ERROR (Status)) {
        FreePool (UsbMouseDevice);
        return Status;
    }
    UsbUpdatedBdaMouseFlag(TRUE);

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UsbDisconnectMouseDriver (
    IN USB_INTERFACE        *UsbIf
)
/*++

  Routine Description:
    Report Status Code in Usb Bot Driver

  Arguments:
    DevicePath  - Use this to get Device Path
    CodeType    - Status Code Type
    CodeValue   - Status Code Value

  Returns:
    None

--*/
{
    USB_MOUSE_DEV               *UsbMouseDevice;
    EFI_USB_IO_PROTOCOL         *UsbIo;

    UsbMouseDevice = (USB_MOUSE_DEV *) (UsbIf->DriverContext[0]);
    UsbIo = UsbMouseDevice->UsbIo;

    //
    // Destroy asynchronous interrupt transfer
    //
    UsbIo->UsbAsyncInterruptTransfer (
        UsbIo,
        UsbMouseDevice->IntEndpointDescriptor.EndpointAddress,
        FALSE,
        UsbMouseDevice->IntEndpointDescriptor.Interval,
        0,
        NULL,
        NULL
    );

    UsbIo->UsbAsyncInterruptOnlyTransfer (
        UsbIo,
        FALSE,
        USBMS_KBC_CMD_INTERVAL,
        SendMouseData,
        &UsbMouseDevice->MsCtrlData
    );

    FreePool (UsbMouseDevice);

    return EFI_SUCCESS;
}
