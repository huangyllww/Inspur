/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_BIOS_SIGN_VERIFY_PPI_H__
#define __BYO_BIOS_SIGN_VERIFY_PPI_H__

#include <Base.h>


typedef  
BOOLEAN 
(EFIAPI *BYO_BIOS_SIGN_VERIFY) (
  VOID      *ImageData, 
  UINTN     ImageSize
  );  
  

typedef struct {
  BYO_BIOS_SIGN_VERIFY  Verify;
} BYO_BIOS_SIGN_VERIFY_PPI;

extern EFI_GUID gByoBiosSignVerifyPpiGuid;


#endif
