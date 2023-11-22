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


#ifndef __PCI_ROOT_PORT_H__
#define __PCI_ROOT_PORT_H__

#include <Guid/SetupGuiCustom.h>


#define SETUP_PCI_ROOT_PORT_FORMSET_GUID \
  { 0xe7063d24, 0x6fbe, 0x4d61, { 0x82, 0x70, 0xde, 0x60, 0xf2, 0x62, 0xf2, 0x9b } }

#define PCI_ROOT_PORT_MAIN_FORM_ID             2
#define PCI_ROOT_PORT_SUBFORM_ID               3

#define LABEL_PCI_RP_MAIN_FORM_START           0x1236
#define LABEL_PCI_RP_MAIN_FORM_END             0x1237

#define LABEL_PCI_RP_SUBFORM_START             0x1238
#define LABEL_PCI_RP_SUBFORM_END               0x1239

#define PCIE_VARSTORE_ID                       0x1223
#define PCIE_RP_QUESTION_ID                    0x1200
#define PCIE_RP_QUESTION_ONEOF_ID              0x1300


extern EFI_GUID gByoSetupPciRootPortFormsetGuid;

#endif