/** @file

Copyright (c) 2006 - 2018, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  AdvancedFormCallback.c

Abstract:
  AdvancedFormCallback Setup Rountines

Revision History:


**/

#include <PlatformSetupDxe.h>

EFI_STATUS
EFIAPI
AdvancedFormCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
  )
{
#if 0
  EFI_STATUS            Status = EFI_SUCCESS;
  SETUP_DATA            CurrentSetupData;
  SETUP_DATA            SetupData;
  UINTN                 VariableSize;

  if (Action != EFI_BROWSER_ACTION_CHANGING) {
    return EFI_SUCCESS;
  }

  DEBUG((EFI_D_INFO, "AdvancedFormCallback A:%X K:%X T:%X\n", Action, KeyValue, Type));

  VariableSize = sizeof(SETUP_DATA);
  Status = gRT->GetVariable(PLATFORM_SETUP_VARIABLE_NAME, &gPlatformSetupVariableGuid, NULL, &VariableSize, &SetupData);

  HiiGetBrowserData(
      &gPlatformSetupVariableGuid,
      PLATFORM_SETUP_VARIABLE_NAME,
      sizeof(SETUP_DATA),
      (UINT8*)&CurrentSetupData);

  switch (KeyValue) {

    case KEY_VALUE_ADVANCED_USBPORT0_VAL:
      if (Value->u8) {
        if (SetupData.Die0XHCIPort[1] != 0) CurrentSetupData.Die0XHCIPort[1] = 1;
      } else {
        CurrentSetupData.Die0XHCIPort[1] = 0;
      }
      break;

    case KEY_VALUE_ADVANCED_USBPORT1_VAL:
      if (Value->u8) {
        if (SetupData.Die0XHCIPort[3] != 0) CurrentSetupData.Die0XHCIPort[3] = 1;
      } else {
        CurrentSetupData.Die0XHCIPort[3] = 0;
      }
      break;

    case KEY_VALUE_ADVANCED_USBPORT2_VAL:
      if (Value->u8) {
        if (SetupData.Die1XHCIPort[1] != 0) CurrentSetupData.Die1XHCIPort[1] = 1;
      } else {
        CurrentSetupData.Die1XHCIPort[1] = 0;
      }
      break;

    case KEY_VALUE_ADVANCED_USBPORT3_VAL:
      if (Value->u8) {
        if (SetupData.Die1XHCIPort[3] != 0) CurrentSetupData.Die1XHCIPort[3] = 1;
      } else {
        CurrentSetupData.Die1XHCIPort[3] = 0;
      }
      break;

    case KEY_VALUE_ADVANCED_USBPORT4_VAL:
      if (Value->u8) {
        if (SetupData.Die4XHCIPort[1] != 0) CurrentSetupData.Die4XHCIPort[1] = 1;
      } else {
        CurrentSetupData.Die4XHCIPort[1] = 0;
      }
      break;

    case KEY_VALUE_ADVANCED_USBPORT5_VAL:
      if (Value->u8) {
        if (SetupData.Die4XHCIPort[3] != 0) CurrentSetupData.Die4XHCIPort[3] = 1;
      } else {
        CurrentSetupData.Die4XHCIPort[3] = 0;
      }
      break;

    case KEY_VALUE_ADVANCED_USBPORT6_VAL:
      if (Value->u8) {
        if (SetupData.Die5XHCIPort[1] != 0) CurrentSetupData.Die5XHCIPort[1] = 1;
      } else {
        CurrentSetupData.Die5XHCIPort[1] = 0;
      }
      break;

    case KEY_VALUE_ADVANCED_USBPORT7_VAL:
      if (Value->u8) {
        if (SetupData.Die5XHCIPort[3] != 0) CurrentSetupData.Die5XHCIPort[3] = 1;
      } else {
        CurrentSetupData.Die5XHCIPort[3] = 0;
      }
      break;

  default:
    break;

  }

  HiiSetBrowserData(
      &gPlatformSetupVariableGuid,
      PLATFORM_SETUP_VARIABLE_NAME,
      sizeof(SETUP_DATA),
      (UINT8*)&CurrentSetupData,
      NULL);

  return Status;
#endif

  return EFI_UNSUPPORTED;
}


