/** @file
  The internal header file defines the common structures for PEI and DXE modules.

Copyright (c) 2019 - 2022, Byosoft Corporation. All rights reserved.<BR>
This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.


**/

#ifndef __TCG2_INTERNAL_H__
#define __TCG2_INTERNAL_H__

#define EFI_TCG2_EVENT_LOG_FORMAT_DEFAULT    EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2
#define EFI_TCG2_EVENT_LOG_FORMAT_ALL        (EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2 | EFI_TCG2_EVENT_LOG_FORMAT_TCG_2)

#define TPM_INSTANCE_ID_LIST  { \
  {TPM_DEVICE_INTERFACE_NONE,           TPM_DEVICE_NULL},      \
  {TPM_DEVICE_INTERFACE_TPM12,          TPM_DEVICE_1_2},       \
  {TPM_DEVICE_INTERFACE_TPM20_DTPM,     TPM_DEVICE_2_0_DTPM},  \
  {TPM_DEVICE_INTERFACE_TPM20_HGFTPM,   TPM_DEVICE_2_0_HG_FTPM},  \
  {TCM_DEVICE_INTERFACE_TCM,            TCM_DEVICE_TCM}, \
}

typedef struct {
  GUID       TpmInstanceGuid;
  UINT8      TpmDevice;
} TPM_INSTANCE_ID;

#endif
