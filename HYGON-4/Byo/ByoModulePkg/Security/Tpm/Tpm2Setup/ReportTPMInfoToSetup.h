/** @file

Copyright (c) 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ReportTPMInfoToSetup.h

Abstract:
  Common head file for ReportTPMInfoToSetup.

Revision History:

TIME:
$AUTHOR:
$REVIEWERS:
$SCOPE:
$TECHNICAL:


T.O.D.O

$END--------------------------------------------------------------------------------------------------------------------

**/

#ifndef _REPORT_TPM_INFO_TO_SETUP_H_
#define _REPORT_TPM_INFO_TO_SETUP_H_

#define MAX_VALUE_LENGTH                         0x30

#define MANU_ID_NTZ                              0x005A544E
#define MANU_ID_IFX                              0x00584649
#define MANU_ID_STM                              0x204D5453
#define MANU_ID_INTC                             0x43544E49
#define MANU_ID_HYG                              0x00475948
#define MANU_ID_NTC                              0x0043544E

#pragma pack(1)

typedef struct {
  UINT32  Manu_ID;
  CHAR8*  Manu_Name;
} MANU_NAME_INFO;

#pragma pack()

#endif
