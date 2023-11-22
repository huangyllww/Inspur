/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef _SCREENSHOT_PROTOCOL_H_
#define _SCREENSHOT_PROTOCOL_H__

#define SCREENSHOT_PROTOCOL_GUID \
  { \
    0xef8877fe, 0x1298, 0x49c2, { 0xa8, 0x4f, 0x41, 0x7c, 0x37, 0x88, 0xf3, 0xbe } \
  };

typedef struct _SCREENSHOT_PROTOCOL SCREENSHOT_PROTOCOL;


/**
  Set Folder Path.

  @param  This                   The pointer to the Screenshot protocol interface.
  @param  FolderDevicePath       The pointer to Path to folder.

  @return EFI_SUCCESS if get valid Folder path.
          others if fail.

**/
typedef
EFI_STATUS
(EFIAPI *SET_CHOOSE_FOLDER_PATH) (
  IN  SCREENSHOT_PROTOCOL      *This,
  IN  EFI_DEVICE_PATH_PROTOCOL *FolderDevicePath
  );

/**
  Save  Screenshot to bmp format.

  @param  This                 The pointer to the Screenshot protocol interface.

  @return EFI_SUCCESS if save screenshot success 
          others if fail.

**/
typedef
EFI_STATUS
(EFIAPI *SAVE_SCREENSHOT) (
   IN  SCREENSHOT_PROTOCOL     *This
  );


struct _SCREENSHOT_PROTOCOL {
  SET_CHOOSE_FOLDER_PATH                 SetFolder;
  SAVE_SCREENSHOT                        SaveScreenshot;
};

extern EFI_GUID gScreenshotProtocolGuid;
#endif