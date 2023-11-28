/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PchSpiFlashLib.c

Abstract:

Revision History:

**/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/PcdLib.h>
#include <SpiCmd.h>



EFI_STATUS
EnableBlockProtectionWithNVWRSR_W(
    IN CONST NV_MEDIA_DEVICE_PROTOCOL* this,
    IN  BOOLEAN ProtectionEnable
)
{
  EFI_STATUS          Status;
  UINT8               SpiData[2];
  UINT8               SpiStatus[2];
  NV_DEVICE_INSTANCE  *DeviceInstance;


  ZeroMem(SpiData, sizeof(SpiData));
  ZeroMem(SpiStatus, sizeof(SpiStatus));
  
  DeviceInstance = DEVICE_INSTANCE_FROM_DEVICEPROTOCOL (this);
  if (ProtectionEnable) {
    SpiData[0] = SF_SR_WPE;
  } else {
    SpiData[0] = 0;
  }
	Status = DeviceInstance->SpiProtocol->Execute (
             DeviceInstance->SpiProtocol,
             SPI_RDSR,
             SPI_WREN,
             TRUE,
             FALSE,
             FALSE,
             0,
             sizeof(SpiStatus),
             SpiStatus,
             EnumSpiRegionBios
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
	DEBUG((EFI_D_ERROR,"HX debug:flash block protection Status before unlock:%02X%02X\n",SpiStatus[1],SpiStatus[0]));
  
  Status = DeviceInstance->SpiProtocol->Execute (
             DeviceInstance->SpiProtocol,
             SPI_WRSR,
             SPI_WREN,
             TRUE,
             TRUE,
             TRUE,
             0,
             sizeof(SpiData),
             SpiData,
             EnumSpiRegionAll
             );
  if (EFI_ERROR (Status)) {
      return Status;
  }
  
  Status = DeviceInstance->SpiProtocol->Execute (
             DeviceInstance->SpiProtocol,
             SPI_RDSR,
             SPI_WREN,
             TRUE,
             FALSE,
             FALSE,
             0,
             sizeof(SpiStatus),
             SpiStatus,
             EnumSpiRegionBios
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  DEBUG((EFI_D_ERROR,"HX debug:flash block protection Status after unlock:%02X%02X\n",SpiStatus[1],SpiStatus[0]));
  if ((SpiStatus[0] & SpiData[0]) != SpiData[0]) {
    Status = EFI_DEVICE_ERROR;
  }
  return Status;
}



EFI_STATUS
EnableBlockProtectionWithNVWRSR_B(
    IN CONST NV_MEDIA_DEVICE_PROTOCOL* this,
    IN  BOOLEAN ProtectionEnable
)
{
  EFI_STATUS          Status;
  UINT8               SpiData;
  UINT8               SpiStatus;
  NV_DEVICE_INSTANCE  *DeviceInstance;


  DeviceInstance = DEVICE_INSTANCE_FROM_DEVICEPROTOCOL (this);
  SpiStatus=0x01;
  if (ProtectionEnable) {
    SpiData = SF_SR_WPE;
  } else {
    SpiData = 0;
  }
 
  Status = DeviceInstance->SpiProtocol->Execute (
            DeviceInstance->SpiProtocol,
            SPI_RDSR,
            SPI_WREN,
            TRUE,
            FALSE,
            FALSE,
            0,
            1,
            &SpiStatus,
            EnumSpiRegionBios
            );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  DEBUG((EFI_D_ERROR,"HX debug:flash block protection Status before unlock:%X\n",SpiStatus));
  
  Status = DeviceInstance->SpiProtocol->Execute (
            DeviceInstance->SpiProtocol,
            SPI_WRSR,
            SPI_WREN,
            TRUE,
            TRUE,
            TRUE,
            0,
            1,
            &SpiData,
            EnumSpiRegionAll
            );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = DeviceInstance->SpiProtocol->Execute (
            DeviceInstance->SpiProtocol,
            SPI_RDSR,
            SPI_WREN,
            TRUE,
            FALSE,
            FALSE,
            0,
            1,
            &SpiStatus,
            EnumSpiRegionBios
            );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  DEBUG((EFI_D_ERROR,"HX debug:flash block protection Status after unlock:%X\n",SpiStatus));
  if ((SpiStatus & SpiData) != SpiData) {
    Status = EFI_DEVICE_ERROR;
  }
  return Status;
}


EFI_STATUS
EnableBlockProtectionWithVolWRSR(
    IN CONST NV_MEDIA_DEVICE_PROTOCOL* this,
    IN  BOOLEAN ProtectionEnable
)
{
  EFI_STATUS          Status;
  UINT8               SpiData;
  UINT8               SpiStatus;
  NV_DEVICE_INSTANCE  *DeviceInstance;

  
  DeviceInstance = DEVICE_INSTANCE_FROM_DEVICEPROTOCOL (this);

  SpiStatus = 0x01;
  if (ProtectionEnable) {
    SpiData = SF_SR_WPE;
  } else {
    SpiData = 0;
  }
 
  Status = DeviceInstance->SpiProtocol->Execute (
            DeviceInstance->SpiProtocol,
            SPI_RDSR,
            SPI_WREN,
            TRUE,
            FALSE,
            FALSE,
            0,
            1,
            &SpiStatus,
            EnumSpiRegionBios
            );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  DEBUG((EFI_D_ERROR,"HX debug:flash block protection Status before unlock:%X\n", SpiStatus));

  //
  // Always disable block protection to workaround tool issue.
  // Feature may be re-enabled in a future bios.
  //

  Status = DeviceInstance->SpiProtocol->Execute (
            DeviceInstance->SpiProtocol,
            SPI_WRSR,
            SPI_EWSR,
            TRUE,
            TRUE,
            TRUE,
            0,
            1,
            &SpiData,
            EnumSpiRegionAll
            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = DeviceInstance->SpiProtocol->Execute (
            DeviceInstance->SpiProtocol,
            SPI_RDSR,
            SPI_WREN,
            TRUE,
            FALSE,
            FALSE,
            0,
            1,
            &SpiStatus,
            EnumSpiRegionBios
            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG((EFI_D_ERROR,"HX debug:flash block protection Status after unlock:%X\n",SpiStatus));
  if ((SpiStatus & SpiData) != SpiData) {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
   
}



#if 0
EFI_STATUS
EnableBlockProtectionWithULBPR (
    IN CONST NV_MEDIA_DEVICE_PROTOCOL* this,
    IN  BOOLEAN ProtectionEnable
)
{
  EFI_STATUS          Status;
  NV_DEVICE_INSTANCE  *DeviceInstance;
  UINT8               SpiStatus;


  DeviceInstance = DEVICE_INSTANCE_FROM_DEVICEPROTOCOL (this);

  //
  // Always disable block protection to workaround tool issue.
  // Feature may be re-enabled in a future bios.
  //
  if (ProtectionEnable) {
    return EFI_SUCCESS;
  } else {

    Status = DeviceInstance->SpiProtocol->Execute (
              DeviceInstance->SpiProtocol,
              SPI_ULBPR,
              SPI_WREN,
              FALSE,
              TRUE,      //Atomic cycle:execute WPEN before globally unlock
              FALSE,
              0,
              0,
              NULL,
              EnumSpiRegionAll
              );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = DeviceInstance->SpiProtocol->Execute(
              DeviceInstance->SpiProtocol,
              SPI_RBPR,
              SPI_WREN,
              TRUE,
              FALSE,
              FALSE,
              0,
              2,
              &SpiStatus,
              EnumSpiRegionAll
              );
    if(EFI_ERROR(Status)){
      return Status;
    }		
    DEBUG((EFI_D_ERROR,"HX debug:flash block protection Status before unlock:%X\n", SpiStatus));
  }
  
  return Status;
}
#endif






VOID 
SpiFlashPatch (
  NV_DEVICE_INSTANCE        *NvDev
  )
{
  EFI_STATUS   Status;


  if(NvDev->InitTable->VendorId == SF_VENDOR_ID_SST){
    if(NvDev->InitTable->DeviceId0 == SF_DEVICE_ID0_26VF064B){
    	Status = NvDev->SpiProtocol->Execute (
	                 NvDev->SpiProtocol,
	                 SPI_UNLOCK,
	                 SPI_WREN,
	                 FALSE,
	                 TRUE,
	                 FALSE,
	                 0,
	                 0,
	                 NULL,
	                 EnumSpiRegionAll
	                 );
      DEBUG((EFI_D_INFO, "%a (L%d) %r\n", __FUNCTION__, __LINE__, Status));
    } else if(NvDev->InitTable->DeviceId0 == 0x25){

    }
  }
}



SPI_INIT_TABLE  mInitTable;

void InitSpiTable (
  IN OUT SPI_INIT_TABLE         **pInitTable,
  IN DEVICE_TABLE               *pDeviceTable,
  IN SPI_OPCODE_MENU_ENTRY      *pOpcodeMenuList,
  IN UINT8                      *PrefixOpcodeList
  )
{
  INTN i;

  *pInitTable = &mInitTable;
  mInitTable.VendorId = pDeviceTable->VendorId;
  mInitTable.DeviceId0 = pDeviceTable->DeviceId0;
  mInitTable.DeviceId1 = pDeviceTable->DeviceId1;

  for (i=0; i < SPI_NUM_OPCODE; i++) {
    mInitTable.OpcodeMenu[i].Type = pOpcodeMenuList[i].Type;
    mInitTable.OpcodeMenu[i].Code = pOpcodeMenuList[i].Code;
    mInitTable.OpcodeMenu[i].Frequency = pOpcodeMenuList[i].Frequency;
    mInitTable.OpcodeMenu[i].Operation = pOpcodeMenuList[i].Operation;
  }

  for (i = 0; i < SPI_NUM_PREFIX_OPCODE; i++) {
    mInitTable.PrefixOpcode[i] = PrefixOpcodeList[i];
  }

  mInitTable.BiosStartOffset = pDeviceTable->Size - PcdGet32(PcdFlashAreaSize);
  mInitTable.BiosSize = PcdGet32(PcdFlashAreaSize);
}



EFI_STATUS
device_info (
    IN CONST NV_MEDIA_DEVICE_PROTOCOL* this,
    IN OUT MEDIA_BLOCK_MAP**           MapInfo
)
{
  NV_DEVICE_INSTANCE  *DeviceInstance;

  DeviceInstance = DEVICE_INSTANCE_FROM_DEVICEPROTOCOL (this);
  *MapInfo = DeviceInstance->BlockMap;
  return EFI_SUCCESS;
}




EFI_STATUS
device_sense (
    IN CONST NV_MEDIA_DEVICE_PROTOCOL* this
)
{
  EFI_STATUS          Status;
  NV_DEVICE_INSTANCE  *DeviceInstance;

  DEBUG((EFI_D_ERROR,"================device sense=================\n"));
  DeviceInstance = DEVICE_INSTANCE_FROM_DEVICEPROTOCOL (this);

  if (DeviceInstance->SpiProtocol == NULL)
    return EFI_UNSUPPORTED;

  if (DeviceInstance->PlatformAccessProtocol)
    DeviceInstance->PlatformAccessProtocol->Enable(DeviceInstance->PlatformAccessProtocol);

  Status = DeviceInstance->SpiProtocol->Init (DeviceInstance->SpiProtocol, DeviceInstance->InitTable);
  if(EFI_ERROR(Status)){
  	return EFI_UNSUPPORTED;
  }

  if (DeviceInstance->PlatformAccessProtocol)
    DeviceInstance->PlatformAccessProtocol->Disable(DeviceInstance->PlatformAccessProtocol);

  return Status;
}




EFI_STATUS
devcie_read (
    IN CONST NV_MEDIA_DEVICE_PROTOCOL  *this,
    IN UINTN                           Address,
    IN OUT UINT8                       *Buffer,
    IN OUT UINTN                       *Length
)
{
  NV_DEVICE_INSTANCE        *DeviceInstance;
  UINTN                     BiosStart;


  DEBUG((EFI_D_INFO, "SpiRead() %X L:%X\n", Address, *Length));

  DeviceInstance = DEVICE_INSTANCE_FROM_DEVICEPROTOCOL (this);
  BiosStart = 0xFFFFFFFF - DeviceInstance->InitTable->BiosSize + 1;
  if ((Address < BiosStart) || ((Address + *Length - 1) > (UINTN)(0xFFFFFFFF))){
    return EFI_INVALID_PARAMETER;
  }

  WriteBackInvalidateDataCacheRange ((VOID*)Address, *Length);
  CopyMem(Buffer, (VOID*)Address, *Length);

  return EFI_SUCCESS;
}



EFI_STATUS
device_write (
    IN CONST NV_MEDIA_DEVICE_PROTOCOL* this,
    IN UINTN Address,
    IN UINT8* Buffer,
    IN UINTN Length
)
{
  EFI_STATUS                Status;
  UINTN                     LbaAddress;
  UINTN                     SpiAddress;
  NV_DEVICE_INSTANCE        *DeviceInstance;


//DEBUG((EFI_D_INFO, "SpiWrite A:%X L:%X\n", Address, Length));

  DeviceInstance = DEVICE_INSTANCE_FROM_DEVICEPROTOCOL (this);

  LbaAddress = 0xFFFFFFFF - DeviceInstance->InitTable->FlashSize + 1;
  
  if ((Address < LbaAddress) || ((Address + Length - 1) > (UINTN)(0xFFFFFFFF))){
    DEBUG((EFI_D_INFO, "LbaAddress:%X\n", LbaAddress));
    return EFI_INVALID_PARAMETER;
  }

  if (DeviceInstance->SpiProtocol == NULL){
      DEBUG((EFI_D_ERROR, "SpiProtocol is NULL\n"));
      return EFI_UNSUPPORTED;
  }

  if (DeviceInstance->PlatformAccessProtocol)
      DeviceInstance->PlatformAccessProtocol->Enable(DeviceInstance->PlatformAccessProtocol);

  SpiAddress = Address - LbaAddress;

  Status = DeviceInstance->SpiProtocol->Execute(
             DeviceInstance->SpiProtocol,
             SPI_PROG,
             SPI_WREN,
             TRUE,
             TRUE,
             TRUE,
             (UINT32)SpiAddress,
             (UINT32)Length,
             Buffer,
             EnumSpiRegionAll
             );

  if (DeviceInstance->PlatformAccessProtocol)
      DeviceInstance->PlatformAccessProtocol->Disable(DeviceInstance->PlatformAccessProtocol);

  WriteBackInvalidateDataCacheRange ((VOID *) Address, Length);
  
  return Status;
}



EFI_STATUS
device_erase (
    IN CONST NV_MEDIA_DEVICE_PROTOCOL* this,
    IN UINTN Address,
    IN UINTN Length
)
{
  EFI_STATUS  Status;
  UINTN       LbaAddress;
  UINTN       BlockCount;
  UINTN       SpiAddress;
  UINT8       OpcodeIndex;

 
  NV_DEVICE_INSTANCE  *DeviceInstance;

  Status =EFI_SUCCESS;
  DeviceInstance = DEVICE_INSTANCE_FROM_DEVICEPROTOCOL (this);

  LbaAddress = 0xFFFFFFFF - DeviceInstance->InitTable->FlashSize + 1;

  if ((Address < LbaAddress) ||
          ((Address + Length - 1) > (UINTN)(0xFFFFFFFF)))
      return EFI_INVALID_PARAMETER;

  if ((Length % DeviceInstance->SectorSize) != 0) {
      return EFI_UNSUPPORTED;
  }

  if (DeviceInstance->SpiProtocol == NULL)
      return EFI_UNSUPPORTED;

  if (DeviceInstance->PlatformAccessProtocol)
      DeviceInstance->PlatformAccessProtocol->Enable(DeviceInstance->PlatformAccessProtocol);

  //
  // Convert memory mapped address to SPI address
  //
  SpiAddress = (UINT32) (Address - LbaAddress);
  BlockCount = Length / DeviceInstance->SectorSize;

  if(DeviceInstance->SectorSize == SIZE_64KB){
    OpcodeIndex = SPI_BERASE;
  } else if(DeviceInstance->SectorSize == SIZE_4KB){
    OpcodeIndex = SPI_SERASE;
  } else {
    return EFI_UNSUPPORTED;
  }
  
  while (BlockCount > 0) {
      Status = DeviceInstance->SpiProtocol->Execute(
                   DeviceInstance->SpiProtocol,
                   OpcodeIndex,
                   SPI_WREN,
                   FALSE,
                   TRUE,
                   FALSE,
                   (UINT32) SpiAddress,
                   0,
                   NULL,
                   EnumSpiRegionAll
               );
      if (EFI_ERROR(Status)) {
          break;
      }
      SpiAddress += DeviceInstance->SectorSize;
      BlockCount--;
  }

  if (DeviceInstance->PlatformAccessProtocol){
    DeviceInstance->PlatformAccessProtocol->Disable(DeviceInstance->PlatformAccessProtocol);
  }
  
  WriteBackInvalidateDataCacheRange ((VOID *) Address, Length);

  return Status;
}



EFI_STATUS
device_lock (
    IN CONST NV_MEDIA_DEVICE_PROTOCOL* this,
    IN UINTN Address,
    IN UINTN Length
)
{
  return EFI_UNSUPPORTED;
}


