/** @file
  This module produce main entry for BDS phase - BdsEntry.
  When this module was dispatched by DxeCore, gEfiBdsArchProtocolGuid will be installed
  which contains interface of BdsEntry.
  After DxeCore finish DXE phase, gEfiBdsArchProtocolGuid->BdsEntry will be invoked
  to enter BDS phase.

Copyright (c) 2004 - 2019, Intel Corporation. All rights reserved.<BR>
(C) Copyright 2016-2019 Hewlett Packard Enterprise Development LP<BR>
(C) Copyright 2015 Hewlett-Packard Development Company, L.P.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Bds.h"
#include "Language.h"
#include "HwErrRecSupport.h"
#include <Library/VariablePolicyHelperLib.h>
#include <Protocol/UefiBootManager.h>
#include <Protocol/DevicePathToText.h>
#include <Guid/BootRetryPolicyVariable.h>
#include <Protocol/PciIo.h>
#include <Protocol/BlockIo.h>

typedef enum {
  UEFI_MIN = 0,
  UEFI_HDD,
  UEFI_ODD,
  UEFI_USB_DISK,
  UEFI_USB_ODD,
  UEFI_PXE,
  UEFI_OTHERS,
  UEFI_MAX,
} BOOT_GROUP_TYPE;

VOID
EFIAPI
EfiBootManagerBootFunc (
  IN  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption
  );

BOOLEAN  mSetFirstNetIndex = FALSE;
UINTN    mFirstNetOptionIndex = 0;
BOOLEAN  mIsLegacyMode = TRUE;

EDKII_UEFI_BOOT_MANAGER_PROTOCOL  gEkdiiUefiBootManager = {
  EDKII_UEFI_BOOT_MANAGER_PROTOCOL_REVISION,
  (EDKII_BOOT_MANAGER_UEFI_BOOT)EfiBootManagerBootFunc
};

VOID
BdsReadKeys (
  BOOLEAN  KeyHandle
  );

#define SET_BOOT_OPTION_SUPPORT_KEY_COUNT(a, c)  { \
      (a) = ((a) & ~EFI_BOOT_OPTION_SUPPORT_COUNT) | (((c) << LowBitSet32 (EFI_BOOT_OPTION_SUPPORT_COUNT)) & EFI_BOOT_OPTION_SUPPORT_COUNT); \
      }

///
/// BDS arch protocol instance initial value.
///
EFI_BDS_ARCH_PROTOCOL  gBds = {
  BdsEntry
};

//
// gConnectConInEvent - Event which is signaled when ConIn connection is required
//
EFI_EVENT  gConnectConInEvent = NULL;

///
/// The read-only variables defined in UEFI Spec.
///
CHAR16  *mReadOnlyVariables[] = {
  EFI_PLATFORM_LANG_CODES_VARIABLE_NAME,
  EFI_LANG_CODES_VARIABLE_NAME,
  EFI_BOOT_OPTION_SUPPORT_VARIABLE_NAME,
  EFI_HW_ERR_REC_SUPPORT_VARIABLE_NAME,
  EFI_OS_INDICATIONS_SUPPORT_VARIABLE_NAME
};

CHAR16  *mBdsLoadOptionName[] = {
  L"Driver",
  L"SysPrep",
  L"Boot",
  L"PlatformRecovery"
};

BOOT_RETRY_POLICY_VARIABLE  mBootRetryPolicy;

/**
  Event to Connect ConIn.

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               Pointer to the notification function's context,
                                which is implementation-dependent.

**/
VOID
EFIAPI
BdsDxeOnConnectConInCallBack (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS  Status;

  //
  // When Osloader call ReadKeyStroke to signal this event
  // no driver dependency is assumed existing. So use a non-dispatch version
  //
  Status = EfiBootManagerConnectConsoleVariable (ConIn);
  if (EFI_ERROR (Status)) {
    //
    // Should not enter this case, if enter, the keyboard will not work.
    // May need platfrom policy to connect keyboard.
    //
    DEBUG ((DEBUG_WARN, "[Bds] Connect ConIn failed - %r!!!\n", Status));
  }
}

/**
  Notify function for event group EFI_EVENT_GROUP_READY_TO_BOOT. This is used to
  check whether there is remaining deferred load images.

  @param[in]  Event   The Event that is being processed.
  @param[in]  Context The Event Context.

**/
VOID
EFIAPI
CheckDeferredLoadImageOnReadyToBoot (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                        Status;
  EFI_DEFERRED_IMAGE_LOAD_PROTOCOL  *DeferredImage;
  UINTN                             HandleCount;
  EFI_HANDLE                        *Handles;
  UINTN                             Index;
  UINTN                             ImageIndex;
  EFI_DEVICE_PATH_PROTOCOL          *ImageDevicePath;
  VOID                              *Image;
  UINTN                             ImageSize;
  BOOLEAN                           BootOption;
  CHAR16                            *DevicePathStr;

  //
  // Find all the deferred image load protocols.
  //
  HandleCount = 0;
  Handles     = NULL;
  Status      = gBS->LocateHandleBuffer (
                       ByProtocol,
                       &gEfiDeferredImageLoadProtocolGuid,
                       NULL,
                       &HandleCount,
                       &Handles
                       );
  if (EFI_ERROR (Status)) {
    return;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (Handles[Index], &gEfiDeferredImageLoadProtocolGuid, (VOID **)&DeferredImage);
    if (EFI_ERROR (Status)) {
      continue;
    }

    for (ImageIndex = 0; ; ImageIndex++) {
      //
      // Load all the deferred images in this protocol instance.
      //
      Status = DeferredImage->GetImageInfo (
                                DeferredImage,
                                ImageIndex,
                                &ImageDevicePath,
                                (VOID **)&Image,
                                &ImageSize,
                                &BootOption
                                );
      if (EFI_ERROR (Status)) {
        break;
      }

      DevicePathStr = ConvertDevicePathToText (ImageDevicePath, FALSE, FALSE);
      DEBUG ((DEBUG_LOAD, "[Bds] Image was deferred but not loaded: %s.\n", DevicePathStr));
      if (DevicePathStr != NULL) {
        FreePool (DevicePathStr);
      }
    }
  }

  if (Handles != NULL) {
    FreePool (Handles);
  }
}

/**

  Install Boot Device Selection Protocol

  @param ImageHandle     The image handle.
  @param SystemTable     The system table.

  @retval  EFI_SUCEESS  BDS has finished initializing.
                        Return the dispatcher and recall BDS.Entry
  @retval  Other        Return status from AllocatePool() or gBS->InstallProtocolInterface

**/
EFI_STATUS
EFIAPI
BdsInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;

  //
  // Install protocol interface
  //
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiBdsArchProtocolGuid,
                  &gBds,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  DEBUG_CODE (
    EFI_EVENT   Event;
    //
    // Register notify function to check deferred images on ReadyToBoot Event.
    //
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    CheckDeferredLoadImageOnReadyToBoot,
                    NULL,
                    &gEfiEventReadyToBootGuid,
                    &Event
                    );
    ASSERT_EFI_ERROR (Status);
    );
  return Status;
}

/**
  Function waits for a given event to fire, or for an optional timeout to expire.

  @param   Event              The event to wait for
  @param   Timeout            An optional timeout value in 100 ns units.

  @retval  EFI_SUCCESS      Event fired before Timeout expired.
  @retval  EFI_TIME_OUT     Timout expired before Event fired..

**/
EFI_STATUS
BdsWaitForSingleEvent (
  IN  EFI_EVENT  Event,
  IN  UINT64     Timeout       OPTIONAL
  )
{
  UINTN       Index;
  EFI_STATUS  Status;
  EFI_EVENT   TimerEvent;
  EFI_EVENT   WaitList[2];
  UINTN       TryIndex;

  if (Timeout != 0) {
    //
    // Create a timer event
    //
    Status = gBS->CreateEvent (EVT_TIMER, 0, NULL, NULL, &TimerEvent);
    if (!EFI_ERROR (Status)) {
      //
      // Set the timer event
      //
      gBS->SetTimer (
             TimerEvent,
             TimerPeriodic,
             DivU64x32 (Timeout, 10)
             );

      //
      // Wait for the original event or the timer
      //
      WaitList[0] = Event;
      WaitList[1] = TimerEvent;

      for (TryIndex = 0; TryIndex < 10; TryIndex++) {
        Status = gBS->WaitForEvent (2, WaitList, &Index);
        if (!EFI_ERROR (Status) && (Index == 0)) {
          break;
        }

        BdsReadKeys (TRUE);
      }

      gBS->CloseEvent (TimerEvent);

      //
      // If the timer expired, change the return to timed out
      //
      if (Index == 1) {
        Status = EFI_TIMEOUT;
      }
    }
  } else {
    //
    // No timeout... just wait on the event
    //
    Status = gBS->WaitForEvent (1, &Event, &Index);
    ASSERT (!EFI_ERROR (Status));
    ASSERT (Index == 0);
  }

  return Status;
}

/**
  The function reads user inputs.

**/
VOID
BdsReadKeys (
  BOOLEAN  KeyHandle
  )
{
  EFI_STATUS     Status;
  EFI_INPUT_KEY  Key;
  UINTN          PauseCount = 0;

  if (PcdGetBool (PcdConInConnectOnDemand)) {
    return;
  }

  if (gST->ConIn == NULL) {
    return;
  }

  while (1) {
    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
    if (KeyHandle) {
      if (EFI_ERROR (Status) && ((PauseCount & BIT0) == 0)) {
        break;
      } else if (!EFI_ERROR (Status)) {
        PauseCount++;
        if ((Key.ScanCode != SCAN_PAUSE) || ((PauseCount & BIT0) == 0)) {
          break;
        }
      }
    } else {
      if (EFI_ERROR (Status)) {
        //
        // No more keys.
        //
        break;
      }
    }
  }
}

/**
  The function waits for the boot manager timeout expires or hotkey is pressed.

  It calls PlatformBootManagerWaitCallback each second.

  @param     HotkeyTriggered   Input hotkey event.
**/
VOID
BdsWait (
  IN EFI_EVENT  HotkeyTriggered
  )
{
  EFI_STATUS  Status;
  UINT16      TimeoutRemain;

  DEBUG ((DEBUG_INFO, "[Bds]BdsWait ...Zzzzzzzzzzzz...\n"));

  TimeoutRemain = PcdGet16 (PcdPlatformBootTimeOut);
  while (TimeoutRemain != 0) {
    DEBUG ((DEBUG_INFO, "[Bds]BdsWait(%d)..Zzzz...\n", (UINTN)TimeoutRemain));
    PlatformBootManagerWaitCallback (TimeoutRemain);

    BdsReadKeys (TRUE); // BUGBUG: Only reading can signal HotkeyTriggered
                        //         Can be removed after all keyboard drivers invoke callback in timer callback.

    if (HotkeyTriggered != NULL) {
      Status = BdsWaitForSingleEvent (HotkeyTriggered, EFI_TIMER_PERIOD_SECONDS (1));
      if (!EFI_ERROR (Status)) {
        break;
      }
    } else {
      gBS->Stall (1000000);
    }

    //
    // 0xffff means waiting forever
    // BDS with no hotkey provided and 0xffff as timeout will "hang" in the loop
    //
    if (TimeoutRemain != 0xffff) {
      TimeoutRemain--;
    }
  }

  //
  // If the platform configured a nonzero and finite time-out, and we have
  // actually reached that, report 100% completion to the platform.
  //
  // Note that the (TimeoutRemain == 0) condition excludes
  // PcdPlatformBootTimeOut=0xFFFF, and that's deliberate.
  //
  if ((PcdGet16 (PcdPlatformBootTimeOut) != 0) && (TimeoutRemain == 0)) {
    PlatformBootManagerWaitCallback (0);
  }

  DEBUG ((DEBUG_INFO, "[Bds]Exit the waiting!\n"));
}

/**
  The function will return the GroupType of the DevicePath

  @param DevicePath        Input FilePath of boot option.

  @retval UEFI_HDD         If the input DevicePath is FilePath of UEFI Hard Drive boot option, like SATA hardisks.
  @retval UEFI_ODD         If the input DevicePath is FilePath of UEFI CD/DVD ROM Drive boot option, like SATA CD/DVD.
  @retval UEFI_USB_DISK    If the input DevicePath is FilePath of UEFI USB device boot option, like USB flash disk.
  @retval UEFI_USB_ODD     If the input DevicePath is FilePath of UEFI USB CD/DVD ROM Drive boot option, like USB CD/DVD.
  @retval UEFI_PXE         If the input DevicePath is FilePath of UEFI Network Adapter boot option, like I350 netcard.
  @retval UEFI_OTHERS      If the input DevicePath is not FilePath of the boot option which not falling into any class above.
**/
BOOT_GROUP_TYPE
GetBootTypeFromDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *Dp;
  EFI_HANDLE                DevHandle;
  EFI_STATUS                Status;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  UINT8                     ClassCode[3];
  BOOLEAN                   IsODD;
  EFI_BLOCK_IO_PROTOCOL     *BlockIo;
  EFI_DEVICE_PATH_PROTOCOL  *Next;
  UINTN                     TempSize;
  BOOT_GROUP_TYPE           GroupType = UEFI_MAX;

  IsODD = FALSE;
  Dp    = DevicePath;
  if ((DevicePathType (Dp) == MEDIA_DEVICE_PATH) && (DevicePathSubType (Dp) == MEDIA_HARDDRIVE_DP)) {
    return UEFI_HDD;
  } else if ((DevicePathType (Dp) == BBS_DEVICE_PATH) && (DevicePathSubType (Dp) == BBS_BBS_DP)) {
    return UEFI_MAX;
  }

  Next = GetNextDevicePathInstance (&Dp, &TempSize);
  while (!IsDevicePathEndType (Next)) {
    if ((Next->Type == MESSAGING_DEVICE_PATH) && (Next->SubType == MSG_MAC_ADDR_DP)) {
      return UEFI_PXE;
    }

    if ((Next->Type == MESSAGING_DEVICE_PATH) && ((Next->SubType == MSG_SD_DP) || (Next->SubType == MSG_EMMC_DP))) {
      return UEFI_HDD;
    }

    Next = NextDevicePathNode (Next);
  }

  Dp     = DevicePath;
  Status = gBS->LocateDevicePath (&gEfiBlockIoProtocolGuid, &Dp, &DevHandle);
  if (!EFI_ERROR (Status) && IsDevicePathEnd (Dp)) {
    gBS->HandleProtocol (DevHandle, &gEfiBlockIoProtocolGuid, (VOID **)&BlockIo);
    if (BlockIo->Media->BlockSize == 2048) {
      IsODD = TRUE;
    }
  }

  Dp     = DevicePath;
  Status = gBS->LocateDevicePath (&gEfiPciIoProtocolGuid, &Dp, &DevHandle);
  if (EFI_ERROR (Status)) {
    return UEFI_OTHERS;
  }

  gBS->HandleProtocol (DevHandle, &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
  PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8, 9, 3, ClassCode);
  switch (ClassCode[2]) {
    case 0x01:
      if (IsODD) {
        GroupType = UEFI_ODD;
      } else {
        GroupType = UEFI_HDD;
      }

      break;
    case 0x0C:
      if (ClassCode[1] == 3) {
        if (IsODD) {
          GroupType = UEFI_USB_ODD;
        } else {
          GroupType = UEFI_USB_DISK;
        }
      } else if (ClassCode[1] == 4) {
        GroupType = UEFI_HDD;
      } else {
        GroupType = UEFI_OTHERS;
      }

      break;
    case 0x02:
      GroupType = UEFI_PXE;
      break;
  }

  return GroupType;
}

/**
  Attempt to boot each boot option in the BootOptions array.

  @param BootOptions       Input boot option array.
  @param BootOptionCount   Input boot option count.
  @param BootManagerMenu   Input boot manager menu.

  @retval TRUE  Successfully boot one of the boot options.
  @retval FALSE Failed boot any of the boot options.
**/
BOOLEAN
BootBootOptions (
  IN EFI_BOOT_MANAGER_LOAD_OPTION  *BootOptions,
  IN UINTN                         BootOptionCount
  )
{
  UINTN                     Index;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath      = NULL;
  BOOLEAN                   LastBootIsLegacy = FALSE;
  EFI_DEVICE_PATH_PROTOCOL  *Dp;
  UINTN                     Count         = 0;
  BOOT_GROUP_TYPE           LastGroupType = UEFI_MIN;
  BOOT_GROUP_TYPE           GroupType;
  BOOT_GROUP_TYPE           EndGroupType = UEFI_MAX;
  UINTN                     GroupFirstIndex = 0;
  UINTN                     NextGroupIndex;
  BOOLEAN                   EnableRetry   = FALSE;
  BOOLEAN                   SetFirstIndex = FALSE;
  BOOLEAN                   EndGroupLooped = FALSE;

  //
  // Report Status Code to indicate BDS starts attempting booting from the UEFI BootOrder list.
  //
  REPORT_STATUS_CODE (EFI_PROGRESS_CODE, (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_ATTEMPT_BOOT_ORDER_EVENT));

  //
  // Attempt boot each boot option
  //
  if (BootOptionCount != 0) {
    EndGroupType = GetBootTypeFromDevicePath(BootOptions[BootOptionCount - 1].FilePath);
  }
  
  Index = 0;
  while (Index < BootOptionCount) {
    //
    // Note the index of the first UEFI net option
    //
    if (!mSetFirstNetIndex && (GetBootTypeFromDevicePath(BootOptions[Index].FilePath) == UEFI_PXE)) {
      mSetFirstNetIndex = TRUE;
      mFirstNetOptionIndex = Index;
    }
    if (EnableRetry) {
      Index       = GroupFirstIndex;
      EnableRetry = FALSE;
    }

    Dp = BootOptions[Index].FilePath;
    if (LastBootIsLegacy) {
      if ((DevicePathType (Dp) == BBS_DEVICE_PATH) && (DevicePathSubType (Dp) == BBS_BBS_DP)) {
        Index++;
        continue;
      }
    }

    if (PcdGetBool (PcdPxeRetriedFlag)) {
      //
      // Don't try net options after retrying all net options
      //
      DevicePath = BootOptions[Index].FilePath;
      while (!IsDevicePathEnd (DevicePath)) {
        if ((DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH) &&
            ((DevicePathSubType (DevicePath) == MSG_IPv4_DP) ||
             (DevicePathSubType (DevicePath) == MSG_IPv6_DP) ||
             (DevicePathSubType (DevicePath) == MSG_URI_DP)))
        {
          break;
        }

        DevicePath = NextDevicePathNode (DevicePath);
      }

      if (!IsDevicePathEnd (DevicePath)) {
        Index++;
        continue;
      }
    }

    //
    // According to EFI Specification, if a load option is not marked
    // as LOAD_OPTION_ACTIVE, the boot manager will not automatically
    // load the option.
    //
    if ((BootOptions[Index].Attributes & LOAD_OPTION_ACTIVE) == 0) {
      Index++;
      continue;
    }

    //
    // Retry current GroupType options and note GroupType as LastGroupType for next booting
    //
    GroupType = GetBootTypeFromDevicePath (BootOptions[Index].FilePath);
    if (mBootRetryPolicy.GroupRetryTime != 0) {
      if (((LastGroupType != UEFI_MIN) && (LastGroupType != GroupType))
        || (SetFirstIndex && EndGroupLooped && (Index == BootOptionCount - 1))) {
        NextGroupIndex = Index;
        EnableRetry    = TRUE;
        Count++;
        if (SetFirstIndex && EndGroupLooped && (Index == BootOptionCount - 1)) {
          EndGroupLooped = FALSE;
        }
        if (!mIsLegacyMode && (!PcdGetBool (PcdPxeRetriedFlag) || (LastGroupType != UEFI_PXE))) {
          DEBUG ((DEBUG_INFO, "[BDS]Booted UEFI GroupType:0x%x %d times %a\n", LastGroupType, Count, __FUNCTION__));
        }
        if (Count == mBootRetryPolicy.GroupRetryTime) {
          EnableRetry   = FALSE;
          SetFirstIndex = FALSE;
          Count         = 0;
          if ((LastGroupType == EndGroupType) && (Index == BootOptionCount - 1)) {
            Index = BootOptionCount;
            break;
          } else {
            Index         = NextGroupIndex;
          }
          LastGroupType = GroupType;
        } else {
          Index = GroupFirstIndex;
        }
        if (!SetFirstIndex || (Index != BootOptionCount - 1)) {
          continue;
        }
      }

      LastGroupType = GroupType;
      if (!SetFirstIndex) {
        GroupFirstIndex = Index;
        SetFirstIndex   = TRUE;
        if (GroupType == EndGroupType) {
          EndGroupLooped = FALSE;
        }
      }
    }

    //
    // Boot#### load options with LOAD_OPTION_CATEGORY_APP are executables which are not
    // part of the normal boot processing. Boot options with reserved category values will be
    // ignored by the boot manager.
    //
    if ((BootOptions[Index].Attributes & LOAD_OPTION_CATEGORY) != LOAD_OPTION_CATEGORY_BOOT) {
      if ((GroupType == EndGroupType) && (Index == BootOptionCount - 1) && (mBootRetryPolicy.GroupRetryTime != 0)) {
        EndGroupLooped = TRUE;
        DEBUG((DEBUG_INFO, "Index%d:%s\n", Index, BootOptions[Index].Description));
        continue;
      }
      Index++;
      continue;
    }

    //
    // When AllRetryTime of L"BootRetryPolicy" variable is set, retrying all UEFI options
    // before booting the Legacy boot options
    //
    if (  !LastBootIsLegacy && (mBootRetryPolicy.AllRetryTime > 1) && (DevicePathType (Dp) == BBS_DEVICE_PATH)
       && (DevicePathSubType (Dp) == BBS_BBS_DP))
    {
      break;
    }

    //
    // All the driver options should have been processed since
    // now boot will be performed.
    //
    EfiBootManagerBoot (&BootOptions[Index]);
    if ((DevicePathType (Dp) == BBS_DEVICE_PATH) && (DevicePathSubType (Dp) == BBS_BBS_DP)) {
      LastBootIsLegacy = TRUE;
    } else {
      LastBootIsLegacy = FALSE;
    }

    //
    // Check if PXE retrying has been executed
    //
    if ((mBootRetryPolicy.GroupRetryTime != 0) && (GroupType == UEFI_PXE) && PcdGetBool(PcdPxeRetriedFlag)) {
      EnableRetry = FALSE;
      Index++;
      SetFirstIndex = FALSE;
      Count = mBootRetryPolicy.GroupRetryTime - 1;
      LastGroupType = UEFI_PXE;
      continue;
    }

    if ((GroupType == EndGroupType) && (Index == BootOptionCount - 1) && (mBootRetryPolicy.GroupRetryTime != 0)) {
      EndGroupLooped = TRUE;
      continue;
    }
    Index++;
  }
  return (BOOLEAN)(Index < BootOptionCount);
}

/**
  The function will load and start every Driver####, SysPrep#### or PlatformRecovery####.

  @param  LoadOptions        Load option array.
  @param  LoadOptionCount    Load option count.
**/
VOID
ProcessLoadOptions (
  IN EFI_BOOT_MANAGER_LOAD_OPTION  *LoadOptions,
  IN UINTN                         LoadOptionCount
  )
{
  EFI_STATUS                         Status;
  UINTN                              Index;
  BOOLEAN                            ReconnectAll;
  EFI_BOOT_MANAGER_LOAD_OPTION_TYPE  LoadOptionType;

  ReconnectAll   = FALSE;
  LoadOptionType = LoadOptionTypeMax;

  //
  // Process the driver option
  //
  for (Index = 0; Index < LoadOptionCount; Index++) {
    //
    // All the load options in the array should be of the same type.
    //
    if (Index == 0) {
      LoadOptionType = LoadOptions[Index].OptionType;
    }

    ASSERT (LoadOptionType == LoadOptions[Index].OptionType);
    ASSERT (LoadOptionType != LoadOptionTypeBoot);

    Status = EfiBootManagerProcessLoadOption (&LoadOptions[Index]);

    //
    // Status indicates whether the load option is loaded and executed
    // LoadOptions[Index].Status is what the load option returns
    //
    if (!EFI_ERROR (Status)) {
      //
      // Stop processing if any PlatformRecovery#### returns success.
      //
      if ((LoadOptions[Index].Status == EFI_SUCCESS) &&
          (LoadOptionType == LoadOptionTypePlatformRecovery))
      {
        break;
      }

      //
      // Only set ReconnectAll flag when the load option executes successfully.
      //
      if (!EFI_ERROR (LoadOptions[Index].Status) &&
          ((LoadOptions[Index].Attributes & LOAD_OPTION_FORCE_RECONNECT) != 0))
      {
        ReconnectAll = TRUE;
      }
    }
  }

  //
  // If a driver load option is marked as LOAD_OPTION_FORCE_RECONNECT,
  // then all of the EFI drivers in the system will be disconnected and
  // reconnected after the last driver load option is processed.
  //
  if (ReconnectAll && (LoadOptionType == LoadOptionTypeDriver)) {
    EfiBootManagerDisconnectAll ();
    EfiBootManagerConnectAll ();
  }
}

/**

  Validate input console variable data.

  If found the device path is not a valid device path, remove the variable.

  @param VariableName             Input console variable name.

**/
VOID
BdsFormalizeConsoleVariable (
  IN  CHAR16  *VariableName
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  UINTN                     VariableSize;
  EFI_STATUS                Status;

  GetEfiGlobalVariable2 (VariableName, (VOID **)&DevicePath, &VariableSize);
  if ((DevicePath != NULL) && !IsDevicePathValid (DevicePath, VariableSize)) {
    Status = gRT->SetVariable (
                    VariableName,
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    0,
                    NULL
                    );
    //
    // Deleting variable with current variable implementation shouldn't fail.
    //
    ASSERT_EFI_ERROR (Status);
  }

  if (DevicePath != NULL) {
    FreePool (DevicePath);
  }
}

/**
  Formalize OsIndication related variables.

  For OsIndicationsSupported, Create a BS/RT/UINT64 variable to report caps
  Delete OsIndications variable if it is not NV/BS/RT UINT64.

  Item 3 is used to solve case when OS corrupts OsIndications. Here simply delete this NV variable.

  Create a boot option for BootManagerMenu if it hasn't been created yet

**/
VOID
BdsFormalizeOSIndicationVariable (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT64                        OsIndicationSupport;
  UINT64                        OsIndication;
  UINTN                         DataSize;
  UINT32                        Attributes;
  EFI_BOOT_MANAGER_LOAD_OPTION  BootManagerMenu;

  //
  // OS indicater support variable
  //
  Status = EfiBootManagerGetBootManagerMenu (&BootManagerMenu);
  if (Status != EFI_NOT_FOUND) {
    OsIndicationSupport = EFI_OS_INDICATIONS_BOOT_TO_FW_UI;
    EfiBootManagerFreeLoadOption (&BootManagerMenu);
  } else {
    OsIndicationSupport = 0;
  }

  if (PcdGetBool (PcdPlatformRecoverySupport)) {
    OsIndicationSupport |= EFI_OS_INDICATIONS_START_PLATFORM_RECOVERY;
  }

  if (PcdGetBool (PcdCapsuleOnDiskSupport)) {
    OsIndicationSupport |= EFI_OS_INDICATIONS_FILE_CAPSULE_DELIVERY_SUPPORTED;
  }

  Status = gRT->SetVariable (
                  EFI_OS_INDICATIONS_SUPPORT_VARIABLE_NAME,
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof (UINT64),
                  &OsIndicationSupport
                  );
  //
  // Platform needs to make sure setting volatile variable before calling 3rd party code shouldn't fail.
  //
  ASSERT_EFI_ERROR (Status);

  //
  // If OsIndications is invalid, remove it.
  // Invalid case
  //   1. Data size != UINT64
  //   2. OsIndication value inconsistence
  //   3. OsIndication attribute inconsistence
  //
  OsIndication = 0;
  Attributes   = 0;
  DataSize     = sizeof (UINT64);
  Status       = gRT->GetVariable (
                        EFI_OS_INDICATIONS_VARIABLE_NAME,
                        &gEfiGlobalVariableGuid,
                        &Attributes,
                        &DataSize,
                        &OsIndication
                        );
  if (Status == EFI_NOT_FOUND) {
    return;
  }

  if ((DataSize != sizeof (OsIndication)) ||
      ((OsIndication & ~OsIndicationSupport) != 0) ||
      (Attributes != (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE))
      )
  {
    DEBUG ((DEBUG_ERROR, "[Bds] Unformalized OsIndications variable exists. Delete it\n"));
    Status = gRT->SetVariable (
                    EFI_OS_INDICATIONS_VARIABLE_NAME,
                    &gEfiGlobalVariableGuid,
                    0,
                    0,
                    NULL
                    );
    //
    // Deleting variable with current variable implementation shouldn't fail.
    //
    ASSERT_EFI_ERROR (Status);
  }
}

/**

  Validate variables.

**/
VOID
BdsFormalizeEfiGlobalVariable (
  VOID
  )
{
  //
  // Validate Console variable.
  //
  BdsFormalizeConsoleVariable (EFI_CON_IN_VARIABLE_NAME);
  BdsFormalizeConsoleVariable (EFI_CON_OUT_VARIABLE_NAME);
  BdsFormalizeConsoleVariable (EFI_ERR_OUT_VARIABLE_NAME);

  //
  // Validate OSIndication related variable.
  //
  BdsFormalizeOSIndicationVariable ();
}

static
VOID
SetConsoleInformation (
  VOID
  )
{
  EFI_STATUS                       Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL     *GraphicsOutput;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *SimpleTextOut;
  UINTN                            Column;
  UINTN                            Row;

  Status = gBS->HandleProtocol (
                  gST->ConsoleOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **)&GraphicsOutput
                  );
  if (!EFI_ERROR (Status)) {
    PcdSet32S (PcdVideoHorizontalResolution, (UINT32)GraphicsOutput->Mode->Info->HorizontalResolution);
    PcdSet32S (PcdVideoVerticalResolution, (UINT32)GraphicsOutput->Mode->Info->VerticalResolution);
    PcdSet32S (PcdSetupVideoHorizontalResolution, (UINT32)GraphicsOutput->Mode->Info->HorizontalResolution);
    PcdSet32S (PcdSetupVideoVerticalResolution, (UINT32)GraphicsOutput->Mode->Info->VerticalResolution);
    DEBUG ((
      DEBUG_ERROR,
      "%a(%d) Gop Mode: %d %d\n",
      __FILE__,
      __LINE__,
      GraphicsOutput->Mode->Info->HorizontalResolution,
      GraphicsOutput->Mode->Info->VerticalResolution
      ));
  }

  Status = gBS->HandleProtocol (
                  gST->ConsoleOutHandle,
                  &gEfiSimpleTextOutProtocolGuid,
                  (VOID **)&SimpleTextOut
                  );
  if (!EFI_ERROR (Status)) {
    Status = SimpleTextOut->QueryMode (SimpleTextOut, SimpleTextOut->Mode->Mode, &Column, &Row);
    if (!EFI_ERROR (Status)) {
      PcdSet32S (PcdConOutColumn, (UINT32)Column);
      PcdSet32S (PcdConOutRow, (UINT32)Row);
      PcdSet32S (PcdSetupConOutColumn, (UINT32)Column);
      PcdSet32S (PcdSetupConOutRow, (UINT32)Row);
      DEBUG ((
        DEBUG_ERROR,
        "%a(%d) Text Mode: %d %d\n",
        __FILE__,
        __LINE__,
        Column,
        Row
        ));
    }
  }
}

VOID
DumpConsoleInformation (
  VOID
  )
{
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *Instance;
  EFI_DEVICE_PATH_PROTOCOL              *CopyOfDevicePath;
  UINTN                                 Size;
  EFI_STATUS                            Status;
  CHAR16                                *Str;
  UINT8                                 Index;
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *GraphicsOutput;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *SimpleTextOut;
  UINTN                                 SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info;
  UINTN                                 CurrentColumn;
  UINTN                                 CurrentRow;

  CHAR16  *ConVarName[] = {
    L"ConIn",
    L"ConInDev",
    L"ConOut",
    L"ConOutDev",
  };

  for (Index = 0; Index < ARRAY_SIZE (ConVarName); Index++) {
    DEBUG ((DEBUG_INFO, "Variable \"%s\":\n", ConVarName[Index]));
    Status = GetEfiGlobalVariable2 (ConVarName[Index], (VOID **)&DevicePath, NULL);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "  This consle device may have some issues!\n"));
      continue;
    }

    CopyOfDevicePath = DevicePath;
    while (CopyOfDevicePath != NULL) {
      Instance = GetNextDevicePathInstance (&CopyOfDevicePath, &Size);
      if (Instance == NULL) {
        break;
      }

      Str = ConvertDevicePathToText (
              Instance,
              FALSE,
              TRUE
              );
      if (Str != NULL) {
        DEBUG ((DEBUG_INFO, "  %s\n", Str));
        FreePool (Str);
      }

      FreePool (Instance);
    }

    FreePool (DevicePath);
  }

  Status = gBS->HandleProtocol (
                  gST->ConsoleOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **)&GraphicsOutput
                  );
  if (EFI_ERROR (Status)) {
    GraphicsOutput = NULL;
  }

  Status = gBS->HandleProtocol (
                  gST->ConsoleOutHandle,
                  &gEfiSimpleTextOutProtocolGuid,
                  (VOID **)&SimpleTextOut
                  );
  if (EFI_ERROR (Status)) {
    SimpleTextOut = NULL;
  }

  if ((GraphicsOutput != NULL) && (SimpleTextOut != NULL)) {
    Status = GraphicsOutput->QueryMode (GraphicsOutput, GraphicsOutput->Mode->Mode, &SizeOfInfo, &Info);
    if (EFI_ERROR (Status)) {
      return;
    }

    Status = SimpleTextOut->QueryMode (SimpleTextOut, SimpleTextOut->Mode->Mode, &CurrentColumn, &CurrentRow);
    if (EFI_ERROR (Status)) {
      FreePool (Info);
      return;
    }

    DEBUG ((DEBUG_INFO, "CurrentResolution = %d x %d\n", Info->HorizontalResolution, Info->VerticalResolution));
    DEBUG ((DEBUG_INFO, "CurrentColumn = %d, CurrentRow = %d\n", CurrentColumn, CurrentRow));
    FreePool (Info);
  }
}

/**
  When BootNext exists and it's created by the OS with Short-Form HDD DevicePath, 
  we need to check if it exists in BootOrder and remove it from BootOrder.


  @param LoadOption BootNext option to check.
**/
VOID
CheckAndAjustBootOrder (
  EFI_BOOT_MANAGER_LOAD_OPTION  *LoadOption
  )
{
  EFI_STATUS                      Status;
  UINTN                           Index;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath = NULL;
  UINT16                          *BootOrder = NULL;
  UINTN                           BootOrderSize = 0;

  DevicePath = LoadOption->FilePath;
  if (BmIsAutoCreateBootOption(LoadOption) || (DevicePathType(DevicePath) != MEDIA_DEVICE_PATH)
    || (DevicePathSubType(DevicePath) != MEDIA_HARDDRIVE_DP)) {
    return;
  }

  Status = gRT->GetVariable(L"BootOrder", &gEfiGlobalVariableGuid, NULL, &BootOrderSize, BootOrder);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    BootOrder = AllocatePool(BootOrderSize);
    ASSERT(BootOrder != NULL);

    Status = gRT->GetVariable(L"BootOrder", &gEfiGlobalVariableGuid, NULL, &BootOrderSize, BootOrder);
    if (EFI_ERROR(Status)) {
      FreePool(BootOrder);
    }
  }
  if (BootOrder == NULL) {
    return;
  }

  for (Index = 0; Index < BootOrderSize/sizeof(UINT16); Index++) {
    if (BootOrder[Index] == LoadOption->OptionNumber) {
      break;
    }
  }
  if (Index == BootOrderSize/sizeof(UINT16)) { // not found
    if (BootOrder != NULL) {
      FreePool(BootOrder);
      return;
    }
  }

  if (Index != (BootOrderSize/sizeof(UINT16) - 1)) {
    CopyMem(&BootOrder[Index], &BootOrder[Index + 1], (BootOrderSize/sizeof(UINT16) - Index - 1)*sizeof(UINT16));
  }
  BootOrderSize = BootOrderSize - sizeof(UINT16);
  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  BootOrderSize,
                  BootOrder
                  );
  if (BootOrder != NULL) {
    FreePool(BootOrder);
  }
}

/**

  Service routine for BdsInstance->Entry(). Devices are connected, the
  consoles are initialized, and the boot options are tried.

  @param This             Protocol Instance structure.

**/
VOID
EFIAPI
BdsEntry (
  IN EFI_BDS_ARCH_PROTOCOL  *This
  )
{
  EFI_BOOT_MANAGER_LOAD_OPTION    *LoadOptions;
  UINTN                           LoadOptionCount;
  CHAR16                          *FirmwareVendor;
  EFI_EVENT                       HotkeyTriggered;
  UINT64                          OsIndication;
  UINTN                           DataSize;
  EFI_STATUS                      Status;
  UINT32                          BootOptionSupport;
  UINT16                          BootTimeOut;
  EDKII_VARIABLE_POLICY_PROTOCOL  *VariablePolicy;
  UINTN                           Index;
  EFI_BOOT_MANAGER_LOAD_OPTION    LoadOption;
  UINT16                          *BootNext;
  CHAR16                          BootNextVariableName[sizeof ("Boot####")];
  EFI_BOOT_MANAGER_LOAD_OPTION    BootManagerMenu;
  BOOLEAN                         BootFwUi;
  BOOLEAN                         PlatformRecovery;
  BOOLEAN                         BootSuccess;
  EFI_DEVICE_PATH_PROTOCOL        *FilePath;
  EFI_STATUS                      BootManagerMenuStatus;
  EFI_BOOT_MANAGER_LOAD_OPTION    PlatformDefaultBootOption;
  EFI_MY_HOOK_PROTOCOL            FirstBootFunc;
  EFI_HANDLE                      Handle = NULL;
  CHAR16                          *DevicePathStr;
  UINTN                           Size;
  BOOLEAN                         BootNextExist = FALSE;
  UINT8                           BackAllRetryTime;

  HotkeyTriggered = NULL;
  Status          = EFI_SUCCESS;
  BootSuccess     = FALSE;

  //
  // Insert the performance probe
  //
  PERF_CROSSMODULE_END ("DXE");
  PERF_CROSSMODULE_BEGIN ("BDS");
  DEBUG ((DEBUG_INFO, "[Bds] Entry...\n"));

  //
  // Install gEdkiiUefiBootManagerProtocolGuid
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEdkiiUefiBootManagerProtocolGuid,
                  &gEkdiiUefiBootManager,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Fill in FirmwareVendor and FirmwareRevision from PCDs
  //
  FirmwareVendor      = (CHAR16 *)PcdGetPtr (PcdFirmwareVendor);
  gST->FirmwareVendor = AllocateRuntimeCopyPool (StrSize (FirmwareVendor), FirmwareVendor);
  ASSERT (gST->FirmwareVendor != NULL);
  gST->FirmwareRevision = PcdGet32 (PcdFirmwareRevision);

  //
  // Fixup Tasble CRC after we updated Firmware Vendor and Revision
  //
  gST->Hdr.CRC32 = 0;
  gBS->CalculateCrc32 ((VOID *)gST, sizeof (EFI_SYSTEM_TABLE), &gST->Hdr.CRC32);

  //
  // Validate Variable.
  //
  BdsFormalizeEfiGlobalVariable ();

  //
  // Mark the read-only variables if the Variable Lock protocol exists
  //
  Status = gBS->LocateProtocol (&gEdkiiVariablePolicyProtocolGuid, NULL, (VOID **)&VariablePolicy);
  DEBUG ((DEBUG_INFO, "[BdsDxe] Locate Variable Policy protocol - %r\n", Status));
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < ARRAY_SIZE (mReadOnlyVariables); Index++) {
      Status = RegisterBasicVariablePolicy (
                 VariablePolicy,
                 &gEfiGlobalVariableGuid,
                 mReadOnlyVariables[Index],
                 VARIABLE_POLICY_NO_MIN_SIZE,
                 VARIABLE_POLICY_NO_MAX_SIZE,
                 VARIABLE_POLICY_NO_MUST_ATTR,
                 VARIABLE_POLICY_NO_CANT_ATTR,
                 VARIABLE_POLICY_TYPE_LOCK_NOW
                 );
      ASSERT_EFI_ERROR (Status);
    }
  }

  InitializeHwErrRecSupport ();

  //
  // Initialize L"Timeout" EFI global variable.
  //
  BootTimeOut = PcdGet16 (PcdPlatformBootTimeOut);
  if (BootTimeOut != 0xFFFF) {
    //
    // If time out value equal 0xFFFF, no need set to 0xFFFF to variable area because UEFI specification
    // define same behavior between no value or 0xFFFF value for L"Timeout".
    //
    BdsDxeSetVariableAndReportStatusCodeOnError (
      EFI_TIME_OUT_VARIABLE_NAME,
      &gEfiGlobalVariableGuid,
      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
      sizeof (UINT16),
      &BootTimeOut
      );
  }

  //
  // Initialize L"BootOptionSupport" EFI global variable.
  // Lazy-ConIn implictly disables BDS hotkey.
  //
  BootOptionSupport = EFI_BOOT_OPTION_SUPPORT_APP | EFI_BOOT_OPTION_SUPPORT_SYSPREP;
  if (!PcdGetBool (PcdConInConnectOnDemand)) {
    BootOptionSupport |= EFI_BOOT_OPTION_SUPPORT_KEY;
    SET_BOOT_OPTION_SUPPORT_KEY_COUNT (BootOptionSupport, 3);
  }

  Status = gRT->SetVariable (
                  EFI_BOOT_OPTION_SUPPORT_VARIABLE_NAME,
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof (BootOptionSupport),
                  &BootOptionSupport
                  );
  //
  // Platform needs to make sure setting volatile variable before calling 3rd party code shouldn't fail.
  //
  ASSERT_EFI_ERROR (Status);

  //
  // Cache the "BootNext" NV variable before calling any PlatformBootManagerLib APIs
  // This could avoid the "BootNext" set by PlatformBootManagerLib be consumed in this boot.
  //
  GetEfiGlobalVariable2 (EFI_BOOT_NEXT_VARIABLE_NAME, (VOID **)&BootNext, &DataSize);
  if (DataSize != sizeof (UINT16)) {
    if (BootNext != NULL) {
      FreePool (BootNext);
    }

    BootNext = NULL;
  }
  if (BootNext != NULL) {
    UnicodeSPrint (BootNextVariableName, sizeof (BootNextVariableName), L"Boot%04x", *BootNext);
    Status = EfiBootManagerVariableToLoadOption (BootNextVariableName, &LoadOption);
    if (!EFI_ERROR(Status)) {
      BootNextExist = TRUE;
      CheckAndAjustBootOrder(&LoadOption);
    }
  }

  //
  // Initialize the platform language variables
  //
  InitializeLanguage (TRUE);

  FilePath = FileDevicePath (NULL, EFI_REMOVABLE_MEDIA_FILE_NAME);
  if (FilePath == NULL) {
    DEBUG ((DEBUG_ERROR, "Fail to allocate memory for default boot file path. Unable to boot.\n"));
    CpuDeadLoop ();
  }

  Status = EfiBootManagerInitializeLoadOption (
             &PlatformDefaultBootOption,
             LoadOptionNumberUnassigned,
             LoadOptionTypePlatformRecovery,
             LOAD_OPTION_ACTIVE,
             L"Default PlatformRecovery",
             FilePath,
             NULL,
             0
             );
  ASSERT_EFI_ERROR (Status);

  //
  // System firmware must include a PlatformRecovery#### variable specifying
  // a short-form File Path Media Device Path containing the platform default
  // file path for removable media if the platform supports Platform Recovery.
  //
  if (PcdGetBool (PcdPlatformRecoverySupport)) {
    LoadOptions = EfiBootManagerGetLoadOptions (&LoadOptionCount, LoadOptionTypePlatformRecovery);
    if (EfiBootManagerFindLoadOption (&PlatformDefaultBootOption, LoadOptions, LoadOptionCount) == -1) {
      for (Index = 0; Index < LoadOptionCount; Index++) {
        //
        // The PlatformRecovery#### options are sorted by OptionNumber.
        // Find the the smallest unused number as the new OptionNumber.
        //
        if (LoadOptions[Index].OptionNumber != Index) {
          break;
        }
      }

      PlatformDefaultBootOption.OptionNumber = Index;
      Status                                 = EfiBootManagerLoadOptionToVariable (&PlatformDefaultBootOption);
      ASSERT_EFI_ERROR (Status);
    }

    EfiBootManagerFreeLoadOptions (LoadOptions, LoadOptionCount);
  }

  FreePool (FilePath);

  //
  // Report Status Code to indicate connecting drivers will happen
  //
  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE,
    (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_BEGIN_CONNECTING_DRIVERS)
    );

  //
  // Initialize ConnectConIn event before calling platform code.
  //
  if (PcdGetBool (PcdConInConnectOnDemand)) {
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    BdsDxeOnConnectConInCallBack,
                    NULL,
                    &gConnectConInEventGuid,
                    &gConnectConInEvent
                    );
    if (EFI_ERROR (Status)) {
      gConnectConInEvent = NULL;
    }
  }

  //
  // Do the platform init, can be customized by OEM/IBV
  // Possible things that can be done in PlatformBootManagerBeforeConsole:
  // > Update console variable: 1. include hot-plug devices; 2. Clear ConIn and add SOL for AMT
  // > Register new Driver#### or Boot####
  // > Register new Key####: e.g.: F12
  // > Signal ReadyToLock event
  // > Authentication action: 1. connect Auth devices; 2. Identify auto logon user.
  //
  PERF_INMODULE_BEGIN ("PlatformBootManagerBeforeConsole");
  PlatformBootManagerBeforeConsole ();
  PERF_INMODULE_END ("PlatformBootManagerBeforeConsole");

  //
  // Execute Driver Options
  //
  LoadOptions = EfiBootManagerGetLoadOptions (&LoadOptionCount, LoadOptionTypeDriver);
  ProcessLoadOptions (LoadOptions, LoadOptionCount);
  EfiBootManagerFreeLoadOptions (LoadOptions, LoadOptionCount);

  //
  // Connect consoles
  //
  PERF_INMODULE_BEGIN ("EfiBootManagerConnectAllDefaultConsoles");
  if (PcdGetBool (PcdConInConnectOnDemand)) {
    EfiBootManagerConnectConsoleVariable (ConOut);
    EfiBootManagerConnectConsoleVariable (ErrOut);
    //
    // Do not connect ConIn devices when lazy ConIn feature is ON.
    //
  } else {
    EfiBootManagerConnectAllDefaultConsoles ();
  }

  SetConsoleInformation ();
  PERF_INMODULE_END ("EfiBootManagerConnectAllDefaultConsoles");

  if (PcdGetBool (PcdEarlyVideoSupport)) {
    BdsReadKeys (FALSE);
    EfiBootManagerStartHotkeyService (&HotkeyTriggered);
  }

  //
  //Signal to device console ready
  //
  EfiEventGroupSignal(&gEventAfterConnectConsoleDeviceGuid);

  //
  // Do the platform specific action after the console is ready
  // Possible things that can be done in PlatformBootManagerAfterConsole:
  // > Console post action:
  //   > Dynamically switch output mode from 100x31 to 80x25 for certain senarino
  //   > Signal console ready platform customized event
  // > Run diagnostics like memory testing
  // > Connect certain devices
  // > Dispatch aditional option roms
  // > Special boot: e.g.: USB boot, enter UI
  //
  PERF_INMODULE_BEGIN ("PlatformBootManagerAfterConsole");
  PlatformBootManagerAfterConsole ();
  PERF_INMODULE_END ("PlatformBootManagerAfterConsole");

  DEBUG_CODE (
    DumpConsoleInformation ();
    );

  //
  // Initialize hotkey service
  // normally, hot key promption displays at the end of PlatformBootManagerAfterConsole,
  // so we should start hot key service after PlatformBootManagerAfterConsole.
  //
  if (!PcdGetBool (PcdEarlyVideoSupport)) {
    BdsReadKeys (FALSE);
    EfiBootManagerStartHotkeyService (&HotkeyTriggered);
  }

  //
  // If any component set PcdTestKeyUsed to TRUE because use of a test key
  // was detected, then display a warning message on the debug log and the console
  //
  if (PcdGetBool (PcdTestKeyUsed)) {
    DEBUG ((DEBUG_ERROR, "**********************************\n"));
    DEBUG ((DEBUG_ERROR, "**  WARNING: Test Key is used.  **\n"));
    DEBUG ((DEBUG_ERROR, "**********************************\n"));
  }

  //
  // Boot to Boot Manager Menu when EFI_OS_INDICATIONS_BOOT_TO_FW_UI is set. Skip HotkeyBoot
  //
  DataSize = sizeof (UINT64);
  Status   = gRT->GetVariable (
                    EFI_OS_INDICATIONS_VARIABLE_NAME,
                    &gEfiGlobalVariableGuid,
                    NULL,
                    &DataSize,
                    &OsIndication
                    );
  if (EFI_ERROR (Status)) {
    OsIndication = 0;
  }

  DEBUG_CODE_BEGIN ();
  EFI_BOOT_MANAGER_LOAD_OPTION_TYPE  LoadOptionType;

  DEBUG ((DEBUG_INFO, "[Bds]OsIndication: %016x\n", OsIndication));
  DEBUG ((DEBUG_INFO, "[Bds]=============Begin Load Options Dumping ...=============\n"));
  for (LoadOptionType = 0; LoadOptionType < LoadOptionTypeMax; LoadOptionType++) {
    DEBUG ((
      DEBUG_INFO,
      "  %s Options:\n",
      mBdsLoadOptionName[LoadOptionType]
      ));
    LoadOptions = EfiBootManagerGetLoadOptions (&LoadOptionCount, LoadOptionType);
    for (Index = 0; Index < LoadOptionCount; Index++) {
      DEBUG ((
        DEBUG_INFO,
        "    %s%04x: %s \t\t 0x%04x\n",
        mBdsLoadOptionName[LoadOptionType],
        LoadOptions[Index].OptionNumber,
        LoadOptions[Index].Description,
        LoadOptions[Index].Attributes
        ));
      DevicePathStr = ConvertDevicePathToText (LoadOptions[Index].FilePath, FALSE, FALSE);
      DEBUG ((DEBUG_INFO, "      DevicePath: %s\n", DevicePathStr));
      if (DevicePathStr != NULL) {
        FreePool (DevicePathStr);
      }

      if (LoadOptions[Index].OptionalDataSize > 0) {
        DUMP_HEX (DEBUG_INFO, 0, LoadOptions[Index].OptionalData, LoadOptions[Index].OptionalDataSize, "      OptionalData");
      }
    }

    EfiBootManagerFreeLoadOptions (LoadOptions, LoadOptionCount);
  }

  DEBUG ((DEBUG_INFO, "[Bds]=============End Load Options Dumping=============\n"));
  DEBUG_CODE_END ();

  //
  // BootManagerMenu doesn't contain the correct information when return status is EFI_NOT_FOUND.
  //
  BootManagerMenuStatus = EfiBootManagerGetBootManagerMenu (&BootManagerMenu);

  BootFwUi         = (BOOLEAN)((OsIndication & EFI_OS_INDICATIONS_BOOT_TO_FW_UI) != 0);
  PlatformRecovery = (BOOLEAN)((OsIndication & EFI_OS_INDICATIONS_START_PLATFORM_RECOVERY) != 0);
  //
  // Clear EFI_OS_INDICATIONS_BOOT_TO_FW_UI to acknowledge OS
  //
  if (BootFwUi || PlatformRecovery) {
    OsIndication &= ~((UINT64)(EFI_OS_INDICATIONS_BOOT_TO_FW_UI | EFI_OS_INDICATIONS_START_PLATFORM_RECOVERY));
    Status        = gRT->SetVariable (
                           EFI_OS_INDICATIONS_VARIABLE_NAME,
                           &gEfiGlobalVariableGuid,
                           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                           sizeof (UINT64),
                           &OsIndication
                           );
    //
    // Changing the content without increasing its size with current variable implementation shouldn't fail.
    //
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Launch Boot Manager Menu directly when EFI_OS_INDICATIONS_BOOT_TO_FW_UI is set. Skip HotkeyBoot
  //
  if (BootFwUi && (BootManagerMenuStatus != EFI_NOT_FOUND)) {
    //
    // Follow generic rule, Call BdsDxeOnConnectConInCallBack to connect ConIn before enter UI
    //
    if (PcdGetBool (PcdConInConnectOnDemand)) {
      BdsDxeOnConnectConInCallBack (NULL, NULL);
    }

    //
    // Directly enter the setup page.
    //
    EfiBootManagerBoot (&BootManagerMenu);
  }

  if (!PlatformRecovery) {
    //
    // Execute SysPrep####
    //
    LoadOptions = EfiBootManagerGetLoadOptions (&LoadOptionCount, LoadOptionTypeSysPrep);
    ProcessLoadOptions (LoadOptions, LoadOptionCount);
    EfiBootManagerFreeLoadOptions (LoadOptions, LoadOptionCount);
    //
    // After pressing F11 to enter the OS recovery mode and enter Setup,
    // press ESC to exit the page and directly enter the system without waiting for key input
    //
    if (!BootFwUi) {
      //
      // Execute Key####
      //
      PERF_INMODULE_BEGIN ("BdsTimeOut");
      BdsWait (HotkeyTriggered);
      PERF_INMODULE_END ("BdsTimeOut");
      //
      // BdsReadKeys() can be removed after all keyboard drivers invoke callback in timer callback.
      //
      BdsReadKeys (FALSE);

      EfiBootManagerHotkeyBoot ();
    }

    if (BootNext != NULL) {
      //
      // Delete "BootNext" NV variable before transferring control to it to prevent loops.
      //
      Status = gRT->SetVariable (
                      EFI_BOOT_NEXT_VARIABLE_NAME,
                      &gEfiGlobalVariableGuid,
                      0,
                      0,
                      NULL
                      );
      //
      // Deleting NV variable shouldn't fail unless it doesn't exist.
      //
      ASSERT (Status == EFI_SUCCESS || Status == EFI_NOT_FOUND);

      //
      // Boot to "BootNext"
      //
      if (BootNextExist) {
        EfiBootManagerBoot (&LoadOption);
        EfiBootManagerFreeLoadOption (&LoadOption);
        if ((LoadOption.Status == EFI_SUCCESS) &&
            (BootManagerMenuStatus != EFI_NOT_FOUND) &&
            (LoadOption.OptionNumber != BootManagerMenu.OptionNumber))
        {
          //
          // Boot to Boot Manager Menu upon EFI_SUCCESS
          // Exception: Do not boot again when the BootNext points to Boot Manager Menu.
          //
          EfiBootManagerBoot (&BootManagerMenu);
        }
      }
    }

    //
    // Try to locate boot option from user input, such as BMC
    //
    Status = gBS->LocateProtocol (&gPlatBeforeDefaultBootProtocolGuid, NULL, (VOID **)&FirstBootFunc);
    if (!EFI_ERROR (Status)) {
      FirstBootFunc ();
    }

    //
    // Get L"BootRetryPolicy" variable to control booting order.
    //
    Size   = sizeof (BOOT_RETRY_POLICY_VARIABLE);
    Status = gRT->GetVariable (
                    BOOT_RETRY_POLICY_VAR_NAME,
                    &gBootRetryPolicyVariableGuid,
                    NULL,
                    &Size,
                    &mBootRetryPolicy
                    );
    if (EFI_ERROR (Status)) {
      mBootRetryPolicy.GroupRetryTime = 0;
      mBootRetryPolicy.AllRetryTime   = 0;
    }
    BackAllRetryTime = mBootRetryPolicy.AllRetryTime;
    DEBUG ((
      DEBUG_INFO,
      "[BDS] GroupRetryTime=%d AllRetryTime=%d %a \n",
      mBootRetryPolicy.GroupRetryTime,
      mBootRetryPolicy.AllRetryTime,
      __FUNCTION__
      ));

    PcdSetBoolS(PcdIsAutoBootFromBootBootOptions, TRUE);
    LoadOptions = EfiBootManagerGetLoadOptions (&LoadOptionCount, LoadOptionTypeBoot);
    //
    // Check if Legacy options exist
    //
    for (Index = 0; Index < LoadOptionCount; Index++) {
      if ((LoadOptions[Index].Attributes & LOAD_OPTION_CATEGORY) != LOAD_OPTION_CATEGORY_BOOT) {
        continue;
      }

      if (DevicePathType (LoadOptions[Index].FilePath) != BBS_DEVICE_PATH) {
        mIsLegacyMode = FALSE;
        break;
      }
    }
    if (mIsLegacyMode) {
      mBootRetryPolicy.AllRetryTime = 1;
    }
    DEBUG((EFI_D_INFO, "mIsLegacyMode=%d %a line=%d\n", mIsLegacyMode, __FUNCTION__, __LINE__));

    do {
      //
      // Retry to boot if any of the boot succeeds
      //
      DEBUG ((DEBUG_INFO, "*** Start booting all boot options: ***\n"));
      BootSuccess = BootBootOptions (LoadOptions, LoadOptionCount);
      if ((mBootRetryPolicy.AllRetryTime == 0) && !BootSuccess) {
        break;
      }

      if (mIsLegacyMode || ((PcdGet16(PcdUnlimitedAllRetry) == 0) || (mBootRetryPolicy.AllRetryTime != PcdGet16(PcdUnlimitedAllRetry)))) {
        mBootRetryPolicy.AllRetryTime--;
      }
      PcdSetBoolS (PcdPxeRetriedFlag, FALSE);
    } while (mBootRetryPolicy.AllRetryTime > 0);
    PcdSetBoolS(PcdIsAutoBootFromBootBootOptions, FALSE);
    if (mSetFirstNetIndex && ((mBootRetryPolicy.GroupRetryTime != 0) || (BackAllRetryTime > 1))) {
      PcdSetBoolS (PcdAllRetryEndFlag, TRUE);
      EfiBootManagerBoot (&LoadOptions[mFirstNetOptionIndex]);
      PcdSetBoolS (PcdAllRetryEndFlag, FALSE);
    }
    EfiBootManagerFreeLoadOptions (LoadOptions, LoadOptionCount);
  }

  if (BootManagerMenuStatus != EFI_NOT_FOUND) {
    EfiBootManagerFreeLoadOption (&BootManagerMenu);
  }

  if (!BootSuccess) {
    if (PcdGetBool (PcdPlatformRecoverySupport)) {
      LoadOptions = EfiBootManagerGetLoadOptions (&LoadOptionCount, LoadOptionTypePlatformRecovery);
      ProcessLoadOptions (LoadOptions, LoadOptionCount);
      EfiBootManagerFreeLoadOptions (LoadOptions, LoadOptionCount);
    }
  }

  EfiBootManagerFreeLoadOption (&PlatformDefaultBootOption);

  DEBUG ((DEBUG_ERROR, "[Bds] Unable to boot!\n"));
  PlatformBootManagerUnableToBoot ();
  CpuDeadLoop ();
}

/**
  Set the variable and report the error through status code upon failure.

  @param  VariableName           A Null-terminated string that is the name of the vendor's variable.
                                 Each VariableName is unique for each VendorGuid. VariableName must
                                 contain 1 or more characters. If VariableName is an empty string,
                                 then EFI_INVALID_PARAMETER is returned.
  @param  VendorGuid             A unique identifier for the vendor.
  @param  Attributes             Attributes bitmask to set for the variable.
  @param  DataSize               The size in bytes of the Data buffer. Unless the EFI_VARIABLE_APPEND_WRITE,
                                 or EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS attribute is set, a size of zero
                                 causes the variable to be deleted. When the EFI_VARIABLE_APPEND_WRITE attribute is
                                 set, then a SetVariable() call with a DataSize of zero will not cause any change to
                                 the variable value (the timestamp associated with the variable may be updated however
                                 even if no new data value is provided,see the description of the
                                 EFI_VARIABLE_AUTHENTICATION_2 descriptor below. In this case the DataSize will not
                                 be zero since the EFI_VARIABLE_AUTHENTICATION_2 descriptor will be populated).
  @param  Data                   The contents for the variable.

  @retval EFI_SUCCESS            The firmware has successfully stored the variable and its data as
                                 defined by the Attributes.
  @retval EFI_INVALID_PARAMETER  An invalid combination of attribute bits, name, and GUID was supplied, or the
                                 DataSize exceeds the maximum allowed.
  @retval EFI_INVALID_PARAMETER  VariableName is an empty string.
  @retval EFI_OUT_OF_RESOURCES   Not enough storage is available to hold the variable and its data.
  @retval EFI_DEVICE_ERROR       The variable could not be retrieved due to a hardware error.
  @retval EFI_WRITE_PROTECTED    The variable in question is read-only.
  @retval EFI_WRITE_PROTECTED    The variable in question cannot be deleted.
  @retval EFI_SECURITY_VIOLATION The variable could not be written due to EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACESS
                                 being set, but the AuthInfo does NOT pass the validation check carried out by the firmware.

  @retval EFI_NOT_FOUND          The variable trying to be updated or deleted was not found.
**/
EFI_STATUS
BdsDxeSetVariableAndReportStatusCodeOnError (
  IN CHAR16    *VariableName,
  IN EFI_GUID  *VendorGuid,
  IN UINT32    Attributes,
  IN UINTN     DataSize,
  IN VOID      *Data
  )
{
  EFI_STATUS                 Status;
  EDKII_SET_VARIABLE_STATUS  *SetVariableStatus;
  UINTN                      NameSize;

  Status = gRT->SetVariable (
                  VariableName,
                  VendorGuid,
                  Attributes,
                  DataSize,
                  Data
                  );
  if (EFI_ERROR (Status)) {
    NameSize          = StrSize (VariableName);
    SetVariableStatus = AllocatePool (sizeof (EDKII_SET_VARIABLE_STATUS) + NameSize + DataSize);
    if (SetVariableStatus != NULL) {
      CopyGuid (&SetVariableStatus->Guid, VendorGuid);
      SetVariableStatus->NameSize   = NameSize;
      SetVariableStatus->DataSize   = DataSize;
      SetVariableStatus->SetStatus  = Status;
      SetVariableStatus->Attributes = Attributes;
      CopyMem (SetVariableStatus + 1, VariableName, NameSize);
      CopyMem (((UINT8 *)(SetVariableStatus + 1)) + NameSize, Data, DataSize);

      REPORT_STATUS_CODE_EX (
        EFI_ERROR_CODE,
        PcdGet32 (PcdErrorCodeSetVariable),
        0,
        NULL,
        &gEdkiiStatusCodeDataTypeVariableGuid,
        SetVariableStatus,
        sizeof (EDKII_SET_VARIABLE_STATUS) + NameSize + DataSize
        );

      FreePool (SetVariableStatus);
    }
  }

  return Status;
}
