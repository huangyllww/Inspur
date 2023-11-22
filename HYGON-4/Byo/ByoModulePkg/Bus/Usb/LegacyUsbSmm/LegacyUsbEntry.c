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
  LegacyUsbEntry.c

Abstract:
  USB Module file.

Revision History:

  Bug 1989:   Changed to use dynamic software SMI value instead of hard coding.
  TIME:       2011-6-15
  $AUTHOR:    Peng Xianbing
  $REVIEWERS:
  $SCOPE:     Define SwSmi value range build a PolicyData table for csm16 to
              get SwSMI value.
  $TECHNICAL:
  $END-------------------------------------------------------------------------
--*/
#include <Library/UefiLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include <protocol/SmmStatusCode.h>
#include <protocol/SwSmiValuePolicyData.h>

#include "LegacyUsb.h"
#include "../UsbBoot/UsbBoot.h"


USB_MOUSE_COMMUNICATION_PROTOCOL   *mUsbMouseCommunicationInstance;
UINT8                              *mDisableKBCtrlAltDelReset = NULL;
PLATFORM_USB_DEVICE_SUPPORT_TABLE  *mPlatformUsbDeviceSupportTable = NULL;
UINT8                              *gSystemInLegacyPtr = NULL;

char IbvPointer[] = "Copyright (C) 2006-2010, BYOSOFT, Inc. All Rights Reserved.";
char UsbVersion[] = "BU2001 06/01/2010";

EFI_SMM_CPU_PROTOCOL                *mSmmCpu = NULL;
EFI_HANDLE                          mLegacyUsbHandle = NULL;
volatile USB_SMMCALL_COMM           *mSmmCallComm;
SMMCALL_ENTRY                       *mSmmCallTablePtr;

SMMCALL_ENTRY UsbSmmCallTable[__ID_LastSmmCall];

extern BOOLEAN mIfLegacyUsbStarted;
extern EFI_PHYSICAL_ADDRESS          mUsbMemAddr;
extern USB_VIRTUAL_KBC_DEV           *mVirtualKBCDev;

EFI_USB_POLICY_PROTOCOL mUsbPolicyProtocol;
EFI_USB_POLICY_PROTOCOL *mUsbPolicyProtocolPtr = NULL;

UINT8                   mPcdKeepTrap64hEnableValue;

UINT32                  mUsbWaitPortStableStall;
EFI_STATUS
SyncLedState(
    IN  EFI_HANDLE                    DispatchHandle,
    IN CONST VOID                    *DispatchContext,
    IN OUT VOID                       *CommBuffer,
    IN OUT UINTN                      *CommBufferSize
);

EFI_STATUS
UsbKeyboardTimerEvent(
    IN  EFI_HANDLE                    DispatchHandle,
    IN CONST VOID                    *DispatchContext,
    IN OUT VOID                       *CommBuffer,
    IN OUT UINTN                      *CommBufferSize
);

EFI_STATUS
DoSmmCallEntry(
    IN  EFI_HANDLE                    DispatchHandle,
    IN CONST VOID                    *Context,
    IN OUT VOID                       *CommBuffer,
    IN OUT UINTN                      *CommBufferSize
)
{
    SMMCALL_ENTRY SmmCallFunc;

    EFI_SMM_SW_REGISTER_CONTEXT   *DispatchContext;
    DispatchContext = (EFI_SMM_SW_REGISTER_CONTEXT *)Context;

    if (DispatchContext->SwSmiInputValue == SMMCALL_SMI_VALUE) {
        SmmCallFunc = UsbSmmCallTable[mSmmCallComm->CallId];

        switch (mSmmCallComm->Argc) {

        case 0:
            *(mSmmCallComm->Return) = SmmCallFunc();
            break;
        case 1:
            *(mSmmCallComm->Return) = SmmCallFunc(mSmmCallComm->Argv[0]);
            break;
        case 2:
            *(mSmmCallComm->Return) = SmmCallFunc(mSmmCallComm->Argv[0], mSmmCallComm->Argv[1]);
            break;
        case 3:
            *(mSmmCallComm->Return) = SmmCallFunc(mSmmCallComm->Argv[0], mSmmCallComm->Argv[1],
                                                  mSmmCallComm->Argv[2]);
            break;
        case 4:
            *(mSmmCallComm->Return) = SmmCallFunc(mSmmCallComm->Argv[0], mSmmCallComm->Argv[1],
                                                  mSmmCallComm->Argv[2], mSmmCallComm->Argv[3]);
            break;
        case 5:
            *(mSmmCallComm->Return) = SmmCallFunc(mSmmCallComm->Argv[0], mSmmCallComm->Argv[1],
                                                  mSmmCallComm->Argv[2], mSmmCallComm->Argv[3], mSmmCallComm->Argv[4]);
            break;
        case 6:
            *(mSmmCallComm->Return) = SmmCallFunc(mSmmCallComm->Argv[0], mSmmCallComm->Argv[1],
                                                  mSmmCallComm->Argv[2], mSmmCallComm->Argv[3], mSmmCallComm->Argv[4], mSmmCallComm->Argv[5]);
            break;
        case 7:
            *(mSmmCallComm->Return) = SmmCallFunc(mSmmCallComm->Argv[0], mSmmCallComm->Argv[1],
                                                  mSmmCallComm->Argv[2], mSmmCallComm->Argv[3], mSmmCallComm->Argv[4], mSmmCallComm->Argv[5],
                                                  mSmmCallComm->Argv[6]);
            break;
        case 8:
            *(mSmmCallComm->Return) = SmmCallFunc(mSmmCallComm->Argv[0], mSmmCallComm->Argv[1],
                                                  mSmmCallComm->Argv[2], mSmmCallComm->Argv[3], mSmmCallComm->Argv[4], mSmmCallComm->Argv[5],
                                                  mSmmCallComm->Argv[6], mSmmCallComm->Argv[7]);
            break;
        case 9:
            *(mSmmCallComm->Return) = SmmCallFunc(mSmmCallComm->Argv[0], mSmmCallComm->Argv[1],
                                                  mSmmCallComm->Argv[2], mSmmCallComm->Argv[3], mSmmCallComm->Argv[4], mSmmCallComm->Argv[5],
                                                  mSmmCallComm->Argv[6], mSmmCallComm->Argv[7], mSmmCallComm->Argv[8]);
            break;

        case 10:
            *(mSmmCallComm->Return) = SmmCallFunc(mSmmCallComm->Argv[0], mSmmCallComm->Argv[1],
                                                  mSmmCallComm->Argv[2], mSmmCallComm->Argv[3], mSmmCallComm->Argv[4], mSmmCallComm->Argv[5],
                                                  mSmmCallComm->Argv[6], mSmmCallComm->Argv[7], mSmmCallComm->Argv[8], mSmmCallComm->Argv[9]);
            break;

        case 11:
            *(mSmmCallComm->Return) = SmmCallFunc(mSmmCallComm->Argv[0], mSmmCallComm->Argv[1],
                                                  mSmmCallComm->Argv[2], mSmmCallComm->Argv[3], mSmmCallComm->Argv[4], mSmmCallComm->Argv[5],
                                                  mSmmCallComm->Argv[6], mSmmCallComm->Argv[7], mSmmCallComm->Argv[8], mSmmCallComm->Argv[9],
                                                  mSmmCallComm->Argv[10]);
            break;

        case 12:
            *(mSmmCallComm->Return) = SmmCallFunc(mSmmCallComm->Argv[0], mSmmCallComm->Argv[1],
                                                  mSmmCallComm->Argv[2], mSmmCallComm->Argv[3], mSmmCallComm->Argv[4], mSmmCallComm->Argv[5],
                                                  mSmmCallComm->Argv[6], mSmmCallComm->Argv[7], mSmmCallComm->Argv[8], mSmmCallComm->Argv[9],
                                                  mSmmCallComm->Argv[10], mSmmCallComm->Argv[11]);
            break;

        default:
            *(mSmmCallComm->Return) = EFI_UNSUPPORTED;
        }
    }

    return EFI_SUCCESS;
}

CSM16_SMMCALL_COMM *
PointCommBuf (
    IN    VOID
)
/*++
--*/
{
    CSM16_SMMCALL_COMM    *CommBuf;
    UINT16                *pSegOfEbda;
    UINT32                mToEbda;

    pSegOfEbda = (UINT16 *)(UINTN)0x40E;
    mToEbda    = (UINT32)(((UINTN)(*pSegOfEbda) << 4) + 0x90);
    CommBuf    = (CSM16_SMMCALL_COMM *)(UINTN)mToEbda;

    return CommBuf;
}

EFI_STATUS
DoCsm16SmmCall(
    IN  EFI_HANDLE                    DispatchHandle,
    IN CONST VOID                    *DispatchContext,
    IN OUT VOID                       *CommBuffer,
    IN OUT UINTN                      *CommBufferSize
)
{
    CSM16_SMMCALL_COMM                        *CommBuf;
    UINT64                                    Handle;
    EFI_LBA                                   Lba;

    CommBuf = PointCommBuf();
    if (CSM16_DISABLE_USB_DEVICE == CommBuf->CallId) {
        if (mIfLegacyUsbStarted) {
            EndLegacyUsb();
        }
        return EFI_SUCCESS;
    }
    if (mIfLegacyUsbStarted == FALSE) {
        return EFI_DEVICE_ERROR;
#if 0
        if (StartLegacyUsb() != EFI_SUCCESS) {
            CommBuf->StatusCode = 0xffffffff;
            return EFI_DEVICE_ERROR;
        }
#endif
    }

    switch (CommBuf->CallId) {
    case CSM16_GET_USB_BOOT_DEVICE_INFO:
        Csm16GetUsbBootDeviceInfo();
        CommBuf->StatusCode = 0;
        break;

    case CSM16_READ_USB_BOOT_DEVICE:
        Handle = CommBuf->Arguments[1];
        Handle = (LShiftU64(Handle,32) & 0xffffffff00000000) + CommBuf->Arguments[0];
        Lba = CommBuf->Arguments[3];
        Lba = (LShiftU64(Lba,32) & 0xffffffff00000000) + CommBuf->Arguments[2];
        CommBuf->StatusCode = Csm16ReadUsbBootDevice(
                                  Handle, Lba, CommBuf->Arguments[4], CommBuf->Arguments[5]);
        break;

    case CSM16_WRITE_USB_BOOT_DEVICE:
        Handle = CommBuf->Arguments[1];
        Handle = (LShiftU64(Handle, 32) & 0xffffffff00000000) + CommBuf->Arguments[0];
        Lba = CommBuf->Arguments[3];
        Lba = (LShiftU64(Lba, 32) & 0xffffffff00000000) + CommBuf->Arguments[2];
        CommBuf->StatusCode = Csm16WriteUsbBootDevice(
                                  Handle, Lba, CommBuf->Arguments[4], CommBuf->Arguments[5]);
        break;

    case CSM16_VERIFY_USB_BOOT_DEVICE:
        Handle = CommBuf->Arguments[1];
        Handle = (LShiftU64(Handle,32) & 0xffffffff00000000) + CommBuf->Arguments[0];
        Lba = CommBuf->Arguments[3];
        Lba = (LShiftU64(Lba,32) & 0xffffffff00000000) + CommBuf->Arguments[2];
        CommBuf->StatusCode = Csm16VerifyUsbBootDevice(
                                  Handle, Lba, CommBuf->Arguments[4]);

    default:
        break;
    }

    return EFI_SUCCESS;
}

VOID ConstructSmmCallTable(VOID)
{
    INIT_SMMCALL(StartLegacyUsb);
    INIT_SMMCALL(EndLegacyUsb);

    INIT_SMMCALL(GetOhcInfo);
    INIT_SMMCALL(OhciReset);
    INIT_SMMCALL(OhciGetState);
    INIT_SMMCALL(OhciSetState);
    INIT_SMMCALL(OhciControlTransfer);
    INIT_SMMCALL(OhciBulkTransfer);
    INIT_SMMCALL(OhciAsyncInterruptTransfer);
    INIT_SMMCALL(OhciSyncInterruptTransfer);
    INIT_SMMCALL(OhciIsochronousTransfer);
    INIT_SMMCALL(OhciAsyncIsochronousTransfer);
    INIT_SMMCALL(OhciGetRootHubPortNumber);
    INIT_SMMCALL(OhciGetRootHubPortStatus);
    INIT_SMMCALL(OhciSetRootHubPortFeature);
    INIT_SMMCALL(OhciClearRootHubPortFeature);

    INIT_SMMCALL(Ohci2GetCapability);
    INIT_SMMCALL(Ohci2Reset);
    INIT_SMMCALL(Ohci2GetState);
    INIT_SMMCALL(Ohci2SetState);
    INIT_SMMCALL(Ohci2ControlTransfer);
    INIT_SMMCALL(Ohci2BulkTransfer);
    INIT_SMMCALL(Ohci2AsyncInterruptTransfer);
    INIT_SMMCALL(Ohci2SyncInterruptTransfer);
    INIT_SMMCALL(Ohci2IsochronousTransfer);
    INIT_SMMCALL(Ohci2AsyncIsochronousTransfer);
    INIT_SMMCALL(Ohci2GetRootHubPortStatus);
    INIT_SMMCALL(Ohci2SetRootHubPortFeature);
    INIT_SMMCALL(Ohci2ClearRootHubPortFeature);

    INIT_SMMCALL(GetUhcInfo);
    INIT_SMMCALL(UhciReset);
    INIT_SMMCALL(UhciGetState);
    INIT_SMMCALL(UhciSetState);
    INIT_SMMCALL(UhciControlTransfer);
    INIT_SMMCALL(UhciBulkTransfer);
    INIT_SMMCALL(UhciAsyncInterruptTransfer);
    INIT_SMMCALL(UhciSyncInterruptTransfer);
    INIT_SMMCALL(UhciIsochronousTransfer);
    INIT_SMMCALL(UhciAsyncIsochronousTransfer);
    INIT_SMMCALL(UhciGetRootHubPortNumber);
    INIT_SMMCALL(UhciGetRootHubPortStatus);
    INIT_SMMCALL(UhciSetRootHubPortFeature);
    INIT_SMMCALL(UhciClearRootHubPortFeature);

    INIT_SMMCALL(Uhci2GetCapability);
    INIT_SMMCALL(Uhci2Reset);
    INIT_SMMCALL(Uhci2GetState);
    INIT_SMMCALL(Uhci2SetState);
    INIT_SMMCALL(Uhci2ControlTransfer);
    INIT_SMMCALL(Uhci2BulkTransfer);
    INIT_SMMCALL(Uhci2AsyncInterruptTransfer);
    INIT_SMMCALL(Uhci2SyncInterruptTransfer);
    INIT_SMMCALL(Uhci2IsochronousTransfer);
    INIT_SMMCALL(Uhci2AsyncIsochronousTransfer);
    INIT_SMMCALL(Uhci2GetRootHubPortStatus);
    INIT_SMMCALL(Uhci2SetRootHubPortFeature);
    INIT_SMMCALL(Uhci2ClearRootHubPortFeature);

    INIT_SMMCALL(GetEhcInfo);
    INIT_SMMCALL(EhcGetCapability);
    INIT_SMMCALL(EhcReset);
    INIT_SMMCALL(EhcGetState);
    INIT_SMMCALL(EhcSetState);
    INIT_SMMCALL(EhcControlTransfer);
    INIT_SMMCALL(EhcBulkTransfer);
    INIT_SMMCALL(EhcAsyncInterruptTransfer);
    INIT_SMMCALL(EhcSyncInterruptTransfer);
    INIT_SMMCALL(EhcIsochronousTransfer);
    INIT_SMMCALL(EhcAsyncIsochronousTransfer);
    INIT_SMMCALL(EhcGetRootHubPortStatus);
    INIT_SMMCALL(EhcSetRootHubPortFeature);
    INIT_SMMCALL(EhcClearRootHubPortFeature);

    INIT_SMMCALL(GetXhcInfo);
    INIT_SMMCALL(XhcGetCapability);
    INIT_SMMCALL(XhcReset);
    INIT_SMMCALL(XhcGetState);
    INIT_SMMCALL(XhcSetState);
    INIT_SMMCALL(XhcControlTransfer);
    INIT_SMMCALL(XhcBulkTransfer);
    INIT_SMMCALL(XhcAsyncInterruptTransfer);
    INIT_SMMCALL(XhcSyncInterruptTransfer);
    INIT_SMMCALL(XhcIsochronousTransfer);
    INIT_SMMCALL(XhcAsyncIsochronousTransfer);
    INIT_SMMCALL(XhcGetRootHubPortStatus);
    INIT_SMMCALL(XhcSetRootHubPortFeature);
    INIT_SMMCALL(XhcClearRootHubPortFeature);

    INIT_SMMCALL(ScanUsbBus);
    INIT_SMMCALL(UsbIoControlTransfer);
    INIT_SMMCALL(UsbIoBulkTransfer);
    INIT_SMMCALL(UsbIoAsyncInterruptTransfer);
    INIT_SMMCALL(UsbIoSyncInterruptTransfer);
    INIT_SMMCALL(UsbIoIsochronousTransfer);
    INIT_SMMCALL(UsbIoAsyncIsochronousTransfer);
    INIT_SMMCALL(UsbIoGetDeviceDescriptor);
    INIT_SMMCALL(UsbIoGetActiveConfigDescriptor);
    INIT_SMMCALL(UsbIoGetInterfaceDescriptor);
    INIT_SMMCALL(UsbIoGetEndpointDescriptor);
    INIT_SMMCALL(UsbIoGetStringDescriptor);
    INIT_SMMCALL(UsbIoGetSupportedLanguages);
    INIT_SMMCALL(UsbIoPortReset);
}

EFI_STATUS
USBLegacyDisable(
    IN  EFI_HANDLE                    DispatchHandle,
    IN CONST VOID                     *DispatchContext,
    IN OUT VOID                       *CommBuffer,
    IN OUT UINTN                      *CommBufferSize
)
{
    EndLegacyUsb();
    return EFI_SUCCESS;
}

/**
  Notification function of EFI_END_OF_DXE_EVENT_GROUP_GUID event group.

  This is a notification function registered on EFI_END_OF_DXE_EVENT_GROUP_GUID event group.

  @param  Event        Event whose notification function is being invoked.
  @param  Context      Pointer to the notification function's context.

**/
VOID
EFIAPI
LegacyUsbPolicyEventNotify (
  EFI_EVENT                               Event,
  VOID                                    *Context
  )
{
  EFI_STATUS   Status;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status = gBS->LocateProtocol (&gUsbPolicyGuid, NULL, (VOID **)&mUsbPolicyProtocolPtr);
  if (!EFI_ERROR(Status)) {
    CopyMem (&mUsbPolicyProtocol, mUsbPolicyProtocolPtr, sizeof (EFI_USB_POLICY_PROTOCOL));
    mUsbPolicyProtocolPtr = &mUsbPolicyProtocol;
    gBS->CloseEvent (Event);
  } else {
    mUsbPolicyProtocolPtr = NULL;
  }
}

/**
  Check if the USB device matched by USB device support rule.

  @param[in]  UsbIf             The interface to search in.
  @param[in]  UsbSupportRule    USB support rule.

  @retval TRUE                  The USB device matched.
  @retval FALSE                 The USB device does not matched.

**/
BOOLEAN
IsUsbDeviceMatched (
  IN USB_INTERFACE            *UsbIf,
  IN USB_DEVICE_SUPPORT_RULE  *UsbSupportRule
  )
{
  EFI_USB_DEVICE_DESCRIPTOR    *DevDesc;
  EFI_USB_INTERFACE_DESCRIPTOR *IfDescriptor;
  CHAR16                       *ToText;

  DevDesc      = &(UsbIf->Device->DevDesc->Desc);
  IfDescriptor = &(UsbIf->IfSetting->Desc);

  ToText = ConvertDevicePathToText (UsbSupportRule->DevicePath, FALSE, FALSE);
  DEBUG ((DEBUG_INFO, "DevicePath = %S UsbSupportRule->DeviceClass = %d\n", ToText, UsbSupportRule->DeviceClass));
  if (ToText != NULL) {
    FreePool(ToText);
  }
  if (UsbSupportRule->DevicePath != NULL) {
    if (GetDevicePathSize (UsbSupportRule->DevicePath) > GetDevicePathSize(UsbIf->DevicePath)) {
      // If device path of rule is larger
      return FALSE;
    }
    if (CompareMem (UsbSupportRule->DevicePath, UsbIf->DevicePath, GetDevicePathSize (UsbSupportRule->DevicePath) - 4) != 0) {
      // If device path of rule
      return FALSE;
    }
  }
  if (UsbSupportRule->IdVendor != 0xFFFF &&
      UsbSupportRule->IdVendor != DevDesc->IdVendor) {
    return FALSE;
  }
  if (UsbSupportRule->IdProduct != 0xFFFF &&
      UsbSupportRule->IdProduct != DevDesc->IdProduct) {
    return FALSE;
  }
  if (UsbSupportRule->DeviceClass != 0xFFFF &&
      UsbSupportRule->DeviceClass != IfDescriptor->InterfaceClass) {
    return FALSE;
  }
  if (UsbSupportRule->DeviceSubClass != 0xFF &&
      UsbSupportRule->DeviceSubClass != IfDescriptor->InterfaceSubClass) {
    return FALSE;
  }
  if (UsbSupportRule->DeviceProtocol != 0xFF &&
      UsbSupportRule->DeviceProtocol != IfDescriptor->InterfaceProtocol) {
    return FALSE;
  }
  return TRUE;
}

/**
  Check if the USB device is supported or not

  @param[in]  UsbIf             The interface to search in.

  @retval TRUE                  The USB device is supported.
  @retval FALSE                 The USB device is not supported.

**/
BOOLEAN
IsUsbDeviceSupport (
  IN USB_INTERFACE            *UsbIf
  )
{
  EFI_USB_DEVICE_DESCRIPTOR    *DevDesc;
  EFI_USB_INTERFACE_DESCRIPTOR *IfDescriptor;
  UINT32                       RuleIndex;
  UINT16                       Priority;
  BOOLEAN                      Supported;
  CHAR16                       *DevicePathText;

  if (mPlatformUsbDeviceSupportTable == NULL) {
    return TRUE;
  }

  DevDesc      = &(UsbIf->Device->DevDesc->Desc);
  IfDescriptor = &(UsbIf->IfSetting->Desc);
  DevicePathText = ConvertDevicePathToText (UsbIf->DevicePath, FALSE, FALSE);
  DEBUG ((DEBUG_INFO, "UsbIf->DevicePath = %S DevDesc->DeviceClass = %d\n", DevicePathText, DevDesc->DeviceClass));
  if (DevicePathText != NULL) {
    FreePool (DevicePathText);
  }
  DEBUG ((DEBUG_INFO, "Descriptor->IdVendor         = %x\n", DevDesc->IdVendor));
  DEBUG ((DEBUG_INFO, "Descriptor->IdProduct        = %x\n", DevDesc->IdProduct));
  DEBUG ((DEBUG_INFO, "IfDescriptor->DeviceClass    = %x\n", IfDescriptor->InterfaceClass));
  DEBUG ((DEBUG_INFO, "IfDescriptor->DeviceSubClass = %x\n", IfDescriptor->InterfaceSubClass));
  DEBUG ((DEBUG_INFO, "IfDescriptor->DeviceProtocol = %x\n", IfDescriptor->InterfaceProtocol));

  if (DevDesc->DeviceClass == 9) {
    // HUB is supported always
    return TRUE;
  }

  Supported = TRUE;
  Priority  = 0;
  for (RuleIndex = 0; RuleIndex < mPlatformUsbDeviceSupportTable->Header.RuleCount; RuleIndex++) {
    if (IsUsbDeviceMatched (UsbIf, &mPlatformUsbDeviceSupportTable->Rules[RuleIndex])) {
      if (mPlatformUsbDeviceSupportTable->Rules[RuleIndex].Priority > Priority) {
        Supported = (mPlatformUsbDeviceSupportTable->Rules[RuleIndex].SupportProperty.Bits.Support == 1) ? TRUE : FALSE;
        Priority  = mPlatformUsbDeviceSupportTable->Rules[RuleIndex].Priority;
        DEBUG ((DEBUG_INFO, "IsUsbDeviceMatched Matched, Supported[%d] Priority[%d]\n", Supported, Priority));  
      }
    }
  }

  return Supported;
}

EFI_STATUS
EFIAPI
SmmLegacyUsbSupport (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  EFI_STATUS                         Status;
  PLATFORM_USB_DEVICE_SUPPORT_TABLE  *PlatformUsbDeviceSupportTable = NULL;
  UINTN                              PlatformUsbDeviceSupportTableSize;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  Status = gBS->LocateProtocol (&gPlatformUsbDeviceSupportProtocolGuid, NULL, (VOID **)&PlatformUsbDeviceSupportTable);
  if (!EFI_ERROR (Status) && PlatformUsbDeviceSupportTable != NULL) {
    PlatformUsbDeviceSupportTableSize = PlatformUsbDeviceSupportTable->Header.RuleCount * sizeof (USB_DEVICE_SUPPORT_RULE) + sizeof (PLATFORM_USB_DEVICE_SUPPORT_TABLE);
    mPlatformUsbDeviceSupportTable = AllocateCopyPool (PlatformUsbDeviceSupportTableSize, PlatformUsbDeviceSupportTable);
  }
  if(mUsbPolicyProtocolPtr == NULL ){
    Status = gBS->LocateProtocol (&gUsbPolicyGuid, NULL, (VOID **)&mUsbPolicyProtocolPtr);
    if (!EFI_ERROR(Status)) {
      CopyMem (&mUsbPolicyProtocol, mUsbPolicyProtocolPtr, sizeof (EFI_USB_POLICY_PROTOCOL));
      mUsbPolicyProtocolPtr = &mUsbPolicyProtocol;
    } else {
      mUsbPolicyProtocolPtr = NULL;
    }
  }


  return Status;
}

EFI_STATUS
EFIAPI
LegacyUsbEntryPoint (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS                         Status;
    EFI_SMM_SW_DISPATCH2_PROTOCOL      *SwDispatch;
    EFI_SMM_SW_REGISTER_CONTEXT        SwContext;
    EFI_HANDLE                         SwHandle;
    EFI_LEGACY_USB_INF_PROTOCOL        *LegacyUsbInf;
    EFI_PHYSICAL_ADDRESS               MemAddress;
    UINTN                              Pages;
    EFI_HANDLE                         UsbMouseCommunicationHandle = NULL;
    UINT8                              *Data8;
    VOID                               *Registration;

    mPcdKeepTrap64hEnableValue = PcdGet8 (PcdKeepTrap64hEnable);
    mUsbWaitPortStableStall    = PcdGet32 (PcdUsbWaitPortStableStall) * USB_BUS_1_MILLISECOND;

    Status = gSmst->SmmRegisterProtocolNotify (&gEfiSmmReadyToLockProtocolGuid, SmmLegacyUsbSupport, &Registration);
    if (EFI_ERROR(Status)) {
      return EFI_UNSUPPORTED;
    }

    Status = gBS->LocateProtocol (&gUsbPolicyGuid, NULL, (VOID **)&mUsbPolicyProtocolPtr);
    if (!EFI_ERROR(Status)) {
      CopyMem (&mUsbPolicyProtocol, mUsbPolicyProtocolPtr, sizeof (EFI_USB_POLICY_PROTOCOL));
      mUsbPolicyProtocolPtr = &mUsbPolicyProtocol;
    } else {
      mUsbPolicyProtocolPtr = NULL;
    }

    //
    // Get SMM CPU protocol
    //
    Status = gSmst->SmmLocateProtocol (
                      &gEfiSmmCpuProtocolGuid,
                      NULL,
                      (VOID **)&mSmmCpu
                      );
    ASSERT_EFI_ERROR (Status);

    Pages = 3 + USB_MEM_DEFAULT_PAGES + XHC_RESERVED_PAGES;
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiReservedMemoryType,
                    Pages,
                    &mUsbMemAddr
                    );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    } else {
      DEBUG((EFI_D_INFO, "LegacyUsbEntryPoint.mUsbMemAddr:%lX,P:%X\n", mUsbMemAddr, Pages));
    }

/*
    if (!PcdGetBool (PcdKbcPresent)) {
      Status = gBS->AllocatePool (
                      EfiReservedMemoryType,
                      sizeof (USB_VIRTUAL_KBC_DEV),
                      (VOID **) &mVirtualKBCDev
                      );
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR(Status)) {
        return EFI_OUT_OF_RESOURCES;
      } else {
        DEBUG((EFI_D_ERROR, "UsbVirtualkbcInitialize.mVirtualKBCDev:%lX,S:%X\n",
                             (UINT64)(UINTN)mVirtualKBCDev,
                             sizeof(USB_VIRTUAL_KBC_DEV)
                             ));
      }
      ZeroMem (mVirtualKBCDev, sizeof (USB_VIRTUAL_KBC_DEV));
    } else {
      mVirtualKBCDev = NULL;
    }
*/
    Status = gBS->AllocatePool (
                 EfiBootServicesData,
                 sizeof (EFI_LEGACY_USB_INF_PROTOCOL),
                 &LegacyUsbInf
             );
    if (EFI_ERROR (Status)) {
        return EFI_OUT_OF_RESOURCES;
    }

    Status = gBS->AllocatePages (
                     AllocateAnyPages,
                     EfiReservedMemoryType,
                     1,
                     &MemAddress
                     );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    } else {
      // DEBUG((EFI_D_ERROR, "USB.SmmCallCommBuf:%lX, 1\n", MemAddress));
    }		

    LegacyUsbInf->SmmCallCommBuf = (USB_SMMCALL_COMM *)(UINTN)MemAddress;
    ConstructSmmCallTable();
    mSmmCallComm     = LegacyUsbInf->SmmCallCommBuf;
    mSmmCallTablePtr = LegacyUsbInf->SmmCallTablePtr = UsbSmmCallTable;

    Status = gBS->InstallMultipleProtocolInterfaces (
                    &mLegacyUsbHandle,
                    &gEfiLegacyUsbInfProtocolGuid,
                    LegacyUsbInf,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
    SwHandle = NULL;
    Status = gSmst->SmmInstallProtocolInterface (
                      &SwHandle,
                      &gEfiLegacyUsbInfProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      LegacyUsbInf
                      );
    SwHandle = NULL;

    Status = gSmst->SmmLocateProtocol(&gEfiSmmSwDispatch2ProtocolGuid, NULL, &SwDispatch);
    if (EFI_ERROR(Status)) {
        return EFI_UNSUPPORTED;
    }

    SwContext.SwSmiInputValue = SMMCALL_SMI_VALUE;
    Status = SwDispatch->Register (
                 SwDispatch,
                 DoSmmCallEntry,
                 &SwContext,
                 &SwHandle
             );
    if (EFI_ERROR(Status)) {
        return EFI_UNSUPPORTED;
    }

    SwContext.SwSmiInputValue = USBSyncLED_SMI;
    Status = SwDispatch->Register (
                 SwDispatch,
                 SyncLedState,
                 &SwContext,
                 &SwHandle
             );
    if (EFI_ERROR(Status)) {
        return EFI_UNSUPPORTED;
    }

    SwContext.SwSmiInputValue = USB_KEYBOARD_EVENT;
    Status = SwDispatch->Register (
                 SwDispatch,
                 UsbKeyboardTimerEvent,
                 &SwContext,
                 &SwHandle
             );

    SwContext.SwSmiInputValue = CSM16_SMMCALL_SMI_VALUE;
    Status = SwDispatch->Register (
                 SwDispatch,
                 DoCsm16SmmCall,
                 &SwContext,
                 &SwHandle
             );
    if (EFI_ERROR(Status)) {
        return EFI_UNSUPPORTED;
    }

    SwContext.SwSmiInputValue = USB_LEGACY_DIS;
    Status = SwDispatch->Register (
                           SwDispatch,
                           USBLegacyDisable,
                           &SwContext,
                           &SwHandle
                           );
    // Setup Mouse support
    if (PcdGetBool(PcdTextSetupMouseEnable)){
      Status = gBS->AllocatePool (
                      EfiReservedMemoryType,
                      sizeof (USB_MOUSE_COMMUNICATION_PROTOCOL),
                      (VOID**)&mUsbMouseCommunicationInstance
                      );
      if (EFI_ERROR (Status)) {
        DEBUG((EFI_D_ERROR, "BYO USB Mouse DXE Support Fail!\n"));
        mUsbMouseCommunicationInstance = NULL;
      } else {
        DEBUG((EFI_D_INFO, "BYO USB Mouse DXE Support %X\n", mUsbMouseCommunicationInstance));
        SetMem(mUsbMouseCommunicationInstance, sizeof (USB_MOUSE_COMMUNICATION_PROTOCOL), 0);
        mUsbMouseCommunicationInstance->MouseDataStatus = 0x00;
        mUsbMouseCommunicationInstance->MouseDataSize   = 0x04; //Hard Code for craft code
	
        Status = gBS->InstallMultipleProtocolInterfaces (
                        &UsbMouseCommunicationHandle,
                        &gEfiUsbMouseCommunicationProtocolGuid,
                        mUsbMouseCommunicationInstance,
                        NULL
                        );
        ASSERT_EFI_ERROR (Status); 
     }
  }

  mDisableKBCtrlAltDelReset = (UINT8*)(UINTN)PcdGet64(PcdDisableKBCtrlAltDelResetDataPtr);
  if(mDisableKBCtrlAltDelReset == NULL){
    Status = gBS->AllocatePool(
                    EfiReservedMemoryType,
                    sizeof(UINT8),
                    (VOID**)&Data8
                    );
    ASSERT(!EFI_ERROR(Status));
    *Data8 = 0;
    PcdSet64S(PcdDisableKBCtrlAltDelResetDataPtr, (UINTN)Data8);
    mDisableKBCtrlAltDelReset = Data8;
  }

  gSystemInLegacyPtr = (UINT8*)(UINTN)PcdGet64(PcdSystemInLegacyPtr);
  if(gSystemInLegacyPtr == NULL){
    Status = gBS->AllocatePool(
                    EfiReservedMemoryType,
                    sizeof(UINT8),
                    (VOID**)&Data8
                    );
    ASSERT(!EFI_ERROR(Status));
    *Data8 = 0;
    PcdSet64S(PcdSystemInLegacyPtr, (UINTN)Data8);
    gSystemInLegacyPtr = Data8;
  }
  DEBUG((EFI_D_INFO, "SystemInLegacyPtr:%x\n", gSystemInLegacyPtr));
    
  return EFI_SUCCESS;
}



