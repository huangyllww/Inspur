/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  StatusCodeBeepLib.h

Abstract:
  Header file for StatusCodeBeepLib.

Revision History:

**/

#ifndef _STATUS_CODE_BEEP_LIB_H_
#define _STATUS_CODE_BEEP_LIB_H_

EFI_STATUS
EFIAPI
LibDoStatusCodeBeep (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value
  );

#endif