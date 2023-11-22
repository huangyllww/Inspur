/*++

  Copyright (c) 2022 Byosoft Corporation. All rights reserved.
  This program and associated documentation (if any) is furnished
  under a license. Except as permitted by such license,no part of this
  program or documentation may be reproduced, stored divulged or used
  in a public system, or transmitted in any form or by any means
  without the express written consent of Byosoft Corporation.

Module Name:
  PostError.h

Abstract:
  Implements the programming of Post Error.

Revision History:

--*/

#ifndef __SETUP_PCI_LIST_DXE_H__
#define __SETUP_PCI_LIST_DXE_H__

#include <Guid/SetupGuiCustom.h>

// {357F4A0C-ADEA-4e07-8648-735A551D82D4}

#define SETUP_PCI_LIST_FORMSET_GUID \
  { 0x357f4a0c, 0xadea, 0x4e07, { 0x86, 0x48, 0x73, 0x5a, 0x55, 0x1d, 0x82, 0xd4 } }

#define PCI_LIST_FORMSET_CLASS_ID  0x10
#define PCI_LIST_FORM_ID           1  
  
#define LABEL_PCI_LIST_START   0x1234
#define LABEL_PCI_LIST_END     0x1235


extern EFI_GUID gByoSetupPciListFormsetGuid;

#endif
