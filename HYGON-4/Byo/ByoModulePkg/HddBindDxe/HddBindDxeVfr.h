/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

By: 
  winddy_zhang

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/


#ifndef __HDD_BIND_DXE_VFR_H__
#define __HDD_BIND_DXE_VFR_H__

#define SETUP_HDD_BIND_FORMSET_GUID \
{ 0xa2247e77, 0xbc1d, 0x4823, { 0x89, 0x62, 0x82, 0x1b, 0x63, 0x73, 0x27, 0xfd } }


#define HDD_BIND_FORMSET_CLASS_ID  0x10
#define HDD_BIND_FORM_ID           1  
  
#define LABEL_HDD_BIND_START   0x1234
#define LABEL_HDD_BIND_END     0x1235

#define HDD_BIND_ONEOF_ITEM_ID     0x1000
#define HDD_BIND_VARSTORE_ID       1


#define HDD_BIND_VAR_NAME          L"HddBind"

#define STRING_ARRAY_NAME          HddBindDxeStrings

typedef struct {
  UINT16  BindHddIndex;
  UINT16  HddBinded;
  UINT16  HddCount;
} HDD_BIND_VAR_DATA;

#endif
