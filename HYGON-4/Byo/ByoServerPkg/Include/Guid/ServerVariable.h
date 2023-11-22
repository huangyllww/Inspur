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

#ifndef _SERVER_VARIABLE_H_
#define _SERVER_VARIABLE_H_




#define SERVER_VARIABLE_GUID  { 0x95c39972, 0x21ee, 0x4573, {0x8a, 0xf7, 0x4d, 0x9f, 0x58, 0xee, 0xc8, 0x31}}




#define LOAD_CUSTOMIZED_VARIABLE_NAME   L"LoadSetupDefaults"

extern EFI_GUID gEfiServerVariableGuid;
#endif

