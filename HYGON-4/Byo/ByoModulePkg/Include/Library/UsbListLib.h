/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/

#ifndef __BYO_UPDATE_USB_LIST_H__
#define __BYO_UPDATE_USB_LIST_H__



#include <Uefi.h>
#include <Protocol/HiiConfigAccess.h>

VOID
EFIAPI
UpdateUsbList (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_HII_HANDLE    HiiHandle,
  IN UINT16            USBFORMID
  );

VOID
EFIAPI
DebugUSBdeviceInformation (  
  );

#endif

