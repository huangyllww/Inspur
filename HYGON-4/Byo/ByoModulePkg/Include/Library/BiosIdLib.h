/** @file

Copyright (c) 2006 - 2011, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  BiosIdLib.h

Abstract:
  BIOS ID library definitions.

Revision History:

**/



#ifndef _BIOS_ID_LIB_H_
#define _BIOS_ID_LIB_H_
//
// BIOS ID string format:
//
// $(BOARD_ID)$(BOARD_REV).$(OEM_ID).$(BUILD_TYPE)$(VERSION_MAJOR).YYMMDDHHMM
//
// Example: "TNAPCRB1.86C.D0008.1106141018"
//

#include <Guid/BiosIdInfo.h>


EFI_STATUS
EFIAPI
BuildBiosIdInfoHob (
  IN VOID              *FvStart,
  IN BIOS_ID_BOARD_ID  *BoardIdArray,
  IN UINTN              BoardIdCount
  );

EFI_STATUS
EFIAPI
GetBiosIdInfoFromHob (
  IN OUT BIOS_ID_INFO  *BiosIdInfo
  );

EFI_STATUS
EFIAPI
GetBiosIdSection (
  IN  VOID           *FvStart,
  OUT VOID           **SectionHdr
  );

EFI_STATUS
EFIAPI
GetBiosIdInFv (
  OUT BIOS_ID_IMAGE  *BiosIdImage,
  IN  VOID           *FvStart   
  );

EFI_STATUS
GetBiosIdInAnyFv (
  OUT BIOS_ID_IMAGE  *BiosIdImage,
  IN  UINT8          *BiosData,
  IN  UINTN          BiosDataSize
  );

EFI_STATUS
EFIAPI
GetBiosBuildTimeHHMM (
  OUT UINT8     *HH,
  OUT UINT8     *MM
  );

EFI_STATUS
EFIAPI
GetBiosVersionDateTime (
  OUT CHAR16    *BiosVersion, OPTIONAL
  OUT CHAR16    *BiosReleaseDate, OPTIONAL
  OUT CHAR16    *BiosReleaseTime OPTIONAL
  );

EFI_STATUS
EFIAPI
GetBiosId (
  OUT BIOS_ID_IMAGE     *BiosIdImage OPTIONAL
  );
#endif
