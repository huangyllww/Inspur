/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:

Abstract:

Revision History:

**/

#include <Base.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <IndustryStandard/Acpi.h>
#include <Protocol/PciHotPlugInit.h>
#include <Protocol/DevicePath.h>
#include <PciDevicePath.h>


#define EFI_PCI_HOTPLUG_DRIVER_PRIVATE_SIGNATURE   SIGNATURE_32('_', 'H', 'P', '_')

typedef struct {
  UINTN                           Signature;
  EFI_HANDLE                      Handle;
  EFI_PCI_HOT_PLUG_INIT_PROTOCOL  HotPlugInit;
} PCI_HOT_PLUG_INSTANCE;

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL  *Dp;
  UINT64                    ReservedMmio;
  UINT8                     ReservedBusCount;
} HOT_PLUG_BRIDGE_INFO;

// crb 0, (1,4)
// crb 1, (1,1)
//
// BD PciRoot(0x2)/Pci(0x3,0x1)/Pci(0x0,0x0) 
// BD PciRoot(0x5)/Pci(0x1,0x1)/Pci(0x0,0x0)

STATIC BOARDCARD_PATH1 gRp014Dp = {DP_ROOT(2), DP_PCI(0x03, 0x01), DP_END};
STATIC BOARDCARD_PATH1 gRp111Dp = {DP_ROOT(5), DP_PCI(0x01, 0x01), DP_END};

HOT_PLUG_BRIDGE_INFO gPciBridgeInfo[] = {
  {(EFI_DEVICE_PATH_PROTOCOL*)&gRp014Dp, SIZE_256MB, 32 },
  {(EFI_DEVICE_PATH_PROTOCOL*)&gRp111Dp, SIZE_256MB, 32 },    
};



BOOLEAN
CompareDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath1,
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath2
  )
{
  UINTN Size1;
  UINTN Size2;

  Size1 = GetDevicePathSize (DevicePath1);
  Size2 = GetDevicePathSize (DevicePath2);

  if (Size1 != Size2) {
    return FALSE;
  }

  if (CompareMem (DevicePath1, DevicePath2, Size1) != 0) {
    return FALSE;
  }

  return TRUE;
}


HOT_PLUG_BRIDGE_INFO *
GetResourceByDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL *PhpcDevicePath
  )
{
  HOT_PLUG_BRIDGE_INFO  *p = gPciBridgeInfo;
  UINTN                 Index;

  for (Index=0; Index<ARRAY_SIZE(gPciBridgeInfo); Index++) {
    if (CompareDevicePath (p[Index].Dp, PhpcDevicePath)) {
      return &p[Index];
    }
  }
  return NULL;
}





EFI_STATUS
EFIAPI
InitializeRootHpc (
  IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL     *This,
  IN  EFI_DEVICE_PATH_PROTOCOL           *PhpcDevicePath,
  IN  UINT64                             PhpcPciAddress,
  IN  EFI_EVENT                          Event, OPTIONAL
  OUT EFI_HPC_STATE                      *PhpcState
  )
{
  if (Event != NULL) {
    gBS->SignalEvent(Event);
  }

  *PhpcState = EFI_HPC_STATE_INITIALIZED;
  return EFI_SUCCESS;
}



EFI_STATUS
EFIAPI
GetRootHpcList (
  IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL    *This,
  OUT UINTN                             *PhpcCount,
  OUT EFI_HPC_LOCATION                  **PhpcList
  )
{
  HOT_PLUG_BRIDGE_INFO  *p = gPciBridgeInfo;
  EFI_HPC_LOCATION      *HpcLocation;
  UINTN                 Index;
  

  HpcLocation = (EFI_HPC_LOCATION*)AllocatePool(sizeof(EFI_HPC_LOCATION) * ARRAY_SIZE(gPciBridgeInfo));
  ASSERT(HpcLocation != NULL);

  *PhpcCount = ARRAY_SIZE(gPciBridgeInfo);
  *PhpcList  = HpcLocation;

  for(Index=0; Index<ARRAY_SIZE(gPciBridgeInfo); Index++) {
    HpcLocation[Index].HpcDevicePath = p[Index].Dp;
    HpcLocation[Index].HpbDevicePath = p[Index].Dp;
  }

  return EFI_SUCCESS;
}




EFI_STATUS
EFIAPI
GetResourcePadding (
  IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL      *This,
  IN  EFI_DEVICE_PATH_PROTOCOL            *PhpcDevicePath,
  IN  UINT64                              PhpcPciAddress,
  OUT EFI_HPC_STATE                       *PhpcState,
  OUT VOID                                **Padding,
  OUT EFI_HPC_PADDING_ATTRIBUTES          *Attributes
  )
{
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *PaddingResource;
  HOT_PLUG_BRIDGE_INFO              *HotplugBridgeInfo;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  HotplugBridgeInfo = GetResourceByDevicePath(PhpcDevicePath);
  DEBUG((EFI_D_INFO, "HotplugBridgeInfo:%p\n", HotplugBridgeInfo));
  
  PaddingResource   = AllocateZeroPool(sizeof(EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR)*2 + sizeof(EFI_ACPI_END_TAG_DESCRIPTOR));
  ASSERT(PaddingResource != NULL);
  *Padding = (VOID*)PaddingResource;
  *Attributes                   = EfiPaddingPciBus;
  
  PaddingResource->Desc         = 0x8A;
  PaddingResource->Len          = sizeof(EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
  PaddingResource->ResType      = ACPI_ADDRESS_SPACE_TYPE_MEM;
  PaddingResource->GenFlag      = 0x0;
  PaddingResource->AddrSpaceGranularity = 64;  
  PaddingResource->SpecificFlag = 0;
  PaddingResource->AddrRangeMin = 0;
  PaddingResource->AddrRangeMax = 0;
  if (HotplugBridgeInfo != NULL) {
    PaddingResource->AddrLen      = HotplugBridgeInfo->ReservedMmio;
    PaddingResource->AddrRangeMax = SIZE_1MB - 1;
  }

  PaddingResource++;
  PaddingResource->Desc         = 0x8A;
  PaddingResource->Len          = sizeof(EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
  PaddingResource->ResType      = ACPI_ADDRESS_SPACE_TYPE_BUS;
  PaddingResource->GenFlag      = 0x0;
  PaddingResource->SpecificFlag = 0;
  PaddingResource->AddrRangeMin = 0;
  PaddingResource->AddrRangeMax = 0;
  if (HotplugBridgeInfo != NULL) {
    PaddingResource->AddrLen = HotplugBridgeInfo->ReservedBusCount - 1;
  }
 
  PaddingResource++;
  ((EFI_ACPI_END_TAG_DESCRIPTOR *)PaddingResource)->Desc     = ACPI_END_TAG_DESCRIPTOR;
  ((EFI_ACPI_END_TAG_DESCRIPTOR *)PaddingResource)->Checksum = 0x0;

  *PhpcState = EFI_HPC_STATE_INITIALIZED | EFI_HPC_STATE_ENABLED;

  return EFI_SUCCESS;
}


STATIC PCI_HOT_PLUG_INSTANCE  gPciHotPlug = { 
  EFI_PCI_HOTPLUG_DRIVER_PRIVATE_SIGNATURE, 
  NULL,
  {
    GetRootHpcList,
    InitializeRootHpc,
    GetResourcePadding
  }
};

EFI_STATUS
EFIAPI
PciHotPlugDxeEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), j_d, \n", __LINE__));
  Status = gBS->InstallProtocolInterface (
                  &gPciHotPlug.Handle,
                  &gEfiPciHotPlugInitProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gPciHotPlug.HotPlugInit
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}


