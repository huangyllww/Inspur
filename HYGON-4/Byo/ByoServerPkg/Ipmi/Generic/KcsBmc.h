/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  KcsBmc.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef _EFI_KCS_BMC_H
#define _EFI_KCS_BMC_H

#define KCS_WRITE_START       0x61
#define KCS_WRITE_END         0x62
#define KCS_READ              0x68
#define KCS_GET_STATUS        0x60
#define KCS_ABORT             0x60

#define KCS_ABORT_RETRY_COUNT 1

typedef enum {
  KcsIdleState,
  KcsReadState,
  KcsWriteState,
  KcsErrorState
} KCS_STATE;

typedef union {
  UINT8     RawData;
  struct {
  UINT8 Obf : 1;
  UINT8 Ibf : 1;
  UINT8 SmAtn : 1;
  UINT8 CD : 1;
  UINT8 Oem1 : 1;
  UINT8 Oem2 : 1;
  UINT8 State : 2;
  } Status;
} EFI_KCS_STATUS;

#endif
