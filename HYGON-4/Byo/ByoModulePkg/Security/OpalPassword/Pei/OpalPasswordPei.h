/** @file
  Opal Password PEI driver which is used to unlock Opal Password for S3.

Copyright (c) 2022, Byosoft Corporation. All rights reserved.<BR>

This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.


**/

#ifndef _OPAL_PASSWORD_PEI_H_
#define _OPAL_PASSWORD_PEI_H_

#include <PiPei.h>

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/LockBoxLib.h>
#include <Library/TcgStorageOpalLib.h>
#include <Library/Tcg2PhysicalPresenceLib.h>
#include <Library/PeiServicesTablePointerLib.h>

#include <Protocol/StorageSecurityCommand.h>

#include <Ppi/IoMmu.h>
#include <Ppi/StorageSecurityCommand.h>

#include <HdpS3Info.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/UefiLib.h>

//
// This is a tricky solution. After getting SSC Ppi, we need to get extra NVMe info from NVMe Passtru Ppi
// 
#include "../Bus/Pci/NvmExpressPei/NvmExpressPei.h"
#include <Library/ByoCommLib.h>

//
// According to ATA spec, the max length of hdd password is 32 bytes
//
#define HDD_PASSWORD_MAX_LENGTH         32
//
// ATA_SEC Nvme Command
//
#define SPSP_UNLOCK             0x0002
#define SPSP_FREEZE             0x0005
//
// The generic command timeout value (unit in us) for Storage Security Command
// PPI ReceiveData/SendData services
//
#define SSC_PPI_GENERIC_TIMEOUT                  30000000
#define OPAL_MAX_PASSWORD_SIZE                   32
#pragma pack(1)

#define OPAL_PEI_DEVICE_SIGNATURE       SIGNATURE_32 ('o', 'p', 'd', 's')

typedef struct {
  UINT16            Segment;
  UINT8             Bus;
  UINT8             Device;
  UINT8             Function;
  UINT8             Reserved;
} OPAL_PCI_DEVICE;

typedef struct {
  UINT32                      Length;
  OPAL_PCI_DEVICE             Device;
  UINT8                       PasswordLength;
  UINT8                       Password[OPAL_MAX_PASSWORD_SIZE];
  UINT16                      OpalBaseComId;
  UINT32                      DevicePathLength;
  EFI_DEVICE_PATH_PROTOCOL    DevicePath[];
} OPAL_DEVICE_LOCKBOX_DATA;

#define OPAL_DEVICE_LOCKBOX_GUID  { 0xe6adde5c, 0xb7a3, 0x450f, { 0x9c, 0xc0, 0x95, 0xe5, 0xb, 0xb1, 0x3f, 0x33 } }

typedef struct {
  UINTN                                    Signature;
  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    Sscp;
  OPAL_DEVICE_LOCKBOX_DATA                 *Device;
  VOID                                     *Context;
  EDKII_PEI_STORAGE_SECURITY_CMD_PPI       *SscPpi;
  UINTN                                    DeviceIndex;
} OPAL_PEI_DEVICE;

#define OPAL_PEI_DEVICE_FROM_THIS(a)    \
  CR (a, OPAL_PEI_DEVICE, Sscp, OPAL_PEI_DEVICE_SIGNATURE)

#pragma pack()

#endif // _OPAL_PASSWORD_PEI_H_

