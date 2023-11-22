/** @file
  The file describes the PPI which notifies other drivers
  of the PEIM being initialized by the PEI Dispatcher.

Copyright (c) 2018 - 2022, Byosoft Corporation. All rights reserved.<BR>

This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef _BYO_RESTORE_PCI_DEVICE_CONFIG_PPI_H_
#define _BYO_RESTORE_PCI_DEVICE_CONFIG_PPI_H_

#define BYO_RESTORE_PCI_DEVICE_CONFIG_PPI_GUID \
  { \
    0x1ad46d2e, 0x5563, 0x4eee, { 0x89, 0xf1, 0xc0, 0xe0, 0x55, 0xc0, 0x5d, 0xff } \
  }


typedef struct _BYO_RESTORE_DEVICE_PCI_CONFIG_PPI BYO_RESTORE_DEVICE_PCI_CONFIG_PPI;


typedef
EFI_STATUS
(EFIAPI *RESTORE_DEVICE_PCI_CONFIG) (
  VOID
  );

struct _BYO_RESTORE_DEVICE_PCI_CONFIG_PPI {
  RESTORE_DEVICE_PCI_CONFIG  RestoreDevicePciConfig;
};

extern EFI_GUID gByoRestorePciDeviceConfigPpiGuid;

#endif