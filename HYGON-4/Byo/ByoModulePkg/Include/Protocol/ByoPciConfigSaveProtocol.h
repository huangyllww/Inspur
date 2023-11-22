/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_PCI_DEV_SAVE_CONFIG_PROTOCOL_H__
#define __BYO_PCI_DEV_SAVE_CONFIG_PROTOCOL_H__


typedef struct _BYO_SAVE_PCI_DEVICE_CONFIG_PROTOCOL  BYO_SAVE_PCI_DEVICE_CONFIG_PROTOCOL;

typedef 
EFI_STATUS
(EFIAPI *REGISTER_PCI_DEVICE_TO_SAVE) (
  IN UINT16   Seg,
  IN UINT8    Bus,
  IN UINT8    Dev,
  IN UINT8    Func
);

struct _BYO_SAVE_PCI_DEVICE_CONFIG_PROTOCOL {
  REGISTER_PCI_DEVICE_TO_SAVE              RegisterPciDeviceToSave;
};

extern EFI_GUID gByoSavePciDeviceConfigProtocolGuid;

#endif