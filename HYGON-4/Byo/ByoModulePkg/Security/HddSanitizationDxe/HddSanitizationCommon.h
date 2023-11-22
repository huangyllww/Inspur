/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  HddSanitizationCommon.h

Abstract:
  Hdd HddSanitization DXE driver, header file.

Revision History:

**/


#ifndef _HDD_SANITIZE_COMMON_H_
#define _HDD_SANITIZE_COMMON_H_




HDD_SANITIZE_DEVICE_ENTRY *
HddSanitizationGetConfigFormEntryByIndex (
  IN UINTN Index
  );

HDD_PASSWORD_INFO *
GetHdpInfo (
  VOID    *DevInfo
);

OPAL_FEATURE_INFO *
GetOpalInfo (
  VOID    *DevInfo
);

/*
  Erase Timer Call Back for Periodically refresh the progress bar.
  Percentages are calculated at one second intervals: Percent = ElapsedTimeInSeconds / EraseUnitTimeOutInSeconds.
*/
VOID 
EFIAPI 
EraseTimerInSecondsCallBack (
  EFI_EVENT           Event, 
  VOID                *Context
  );

UINTN
UefiLibGetStringWidth (
  IN  CHAR16               *String,
  IN  BOOLEAN              LimitLen,
  IN  UINTN                MaxWidth,
  OUT UINTN                *Offset
  );

VOID
EFIAPI 
ResetSystemNull (
  IN EFI_RESET_TYPE           ResetType,
  IN EFI_STATUS               ResetStatus,
  IN UINTN                    DataSize,
  IN VOID                     *ResetData OPTIONAL
  );

/**
  Show Hdd Erase Progress

  @param[in] Type         Draw Type
                          HDD_ERASE_DRAW_TYPE_START   The first drawing
                          HDD_ERASE_DRAW_TYPE_RUN     The updating drawing
                          HDD_ERASE_DRAW_TYPE_END     The end drawing
  @param[in] Rate         Take the whole number without keeping the decimal
  @param[in] Info         Hdd Erase Info String
  @param[in] Context      Hdd Erase Progress Bar Draw Funtion Context

  @retval EFI_SUCCESS     Draw Success
  @retval Other           Draw Error

**/
EFI_STATUS
ShowTextProgress (
  IN UINTN     Type,
  IN UINT64    Rate,
  IN CHAR16    *Info,
  IN VOID      *Context
  );

EFI_STATUS
ShowGuiProgress (
  IN UINTN     Type,
  IN UINT64    Rate,
  IN CHAR16    *Info,
  IN VOID      *Context
  );

#endif