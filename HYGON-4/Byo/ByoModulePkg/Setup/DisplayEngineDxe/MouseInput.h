/** @file
  FormDisplay protocol to show Form

Copyright (c) 2013 - 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.


**/

#ifndef __MOUSE_INPUT_H__
#define __MOUSE_INPUT_H__

#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/HiiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Guid/MdeModuleHii.h>
#include <Protocol/SetupMouseProtocol.h>

#include "FormDisplay.h"


typedef enum {
  SETUP_AREA_TITLE,
  SETUP_AREA_MENU,
  SETUP_AREA_HOTKEY,
  SETUP_AREA_SCROLL_BAR,
  SETUP_AREA_HELP,
  SETUP_AREA_MAX
} SETUP_AREA_TYPE;

SETUP_AREA_TYPE
GetSetupEreaType (
  MOUSE_ACTION_INFO    *Mouse,
  UINTN    LayoutStyle
  );
  
LIST_ENTRY *
MouseGetMenuOption ( 
  IN MOUSE_ACTION_INFO *Mouse,
  IN LIST_ENTRY    *TopOfScreen
  );

BOOLEAN
MouseGotoFormset (
  IN MOUSE_ACTION_INFO *Mouse
  );

#endif
