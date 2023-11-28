/** @file

Copyright (c) 2006 - 2011, Byosoft Corporation.<BR>
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


//
// SST25 Serial Flash Status Register definitions
//
#define SF_SR_BUSY        0x01      // Indicates if internal write operation is in progress
#define SF_SR_WEL         0x02      // Indicates if device is memory write enabled
#define SF_SR_BP0         0x04      // Block protection bit 0
#define SF_SR_BP1         0x08      // Block protection bit 1
#define SF_SR_BP2         0x10      // Block protection bit 2
#define SF_SR_BP3         0x20      // Block protection bit 3
#define SF_SR_AAI         0x40      // Auto Address Increment Programming status
#define SF_SR_BPL         0x80      // Block protection lock-down

#define SF_SR_WPE         0x3C      // Enable write protection on all blocks for SST25 

//
// SST26 Serial Flash Status Register definitions
//
// #define SF_SR_BUSY        0x01      // Indicates if internal write operation is in progress
// #define SF_SR_WEL         0x02      // Indicates if device is memory write enabled
#define SF_SR_WSE         0x04      // Write Suspend-Erase status
#define SF_SR_WSP         0x08      // Write Suspend-Program status
#define SF_SR_WPLD        0x10      // Indicates if the Block-Protection register is locked-down
#define SF_SR_SEC         0x20      // Security ID status
                                    // bit 6 resevered
                                    // bit 7 Busy status

//
// SST26 Serial Flash configuration Register definitions
//
#define SF_CF_IOC         0x02      // disable the WP# and HOLD# pin
#define SF_CF_BPNV        0x08      // indicates whether any block has been permanently locked with the nVWLDR
#define SF_CF_WPEN        0x80      // enable the WP# pin for the hardware write-protection function
                                    // others are reserved
                                    
//
// SST26 Serial Flash Block Protection Register Write-protected Value (defaule value) 
//
// BPR[143:0] = 5555 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF
#define SF_BPR_WPV0        0xFFFFFFFF
#define SF_BPR_WPV1        0xFFFFFFFF
#define SF_BPR_WPV2        0xFFFFFFFF
#define SF_BPR_WPV3        0xFFFFFFFF
#define SF_BPR_WPV5        0x00005555


//
// General Operation Instruction definitions for the Serial Flash Device
// only for SPI_INIT_TABLE
//
#define SF_INST_NOP             0x00     // No operation
#define SF_INST_WRSR            0x01     // Write Status Registerconfiguration register
#define SF_INST_PROG            0x02     // Byte Program    
#define SF_INST_READ            0x03     // Read
#define SF_INST_WRDI            0x04     // Write Disable
#define SF_INST_RDSR            0x05     // Read Status Register
#define SF_INST_WREN            0x06     // Write Enable
#define SF_INST_HS_READ         0x0B     // High-speed Read 
#define SF_INST_SERASE          0x20     // Sector Erase (4KB)
#define SF_INST_64KB_ERASE      0xD8     // Block Erase (64KB for SST25 or 8KB/32KB/64KB depending on addressfor SST26) 
#define SF_INST_CERASE          0xC7     // Chip Erase

#define SF_INST_JEDEC_READ_ID   0x9F     // JEDEC Read ID

// for SST25 series only

#define SF_INST_EWSR            0x50     // Enable Write Status Register      
#define SF_INST_READ_ID         0xAB     // Read ID
#define SF_INST_BERASE          0x52     // block Erase(32K)

// for SST26 series only 

#define SF_INST_RDCR            0x35     // Read Configuration register
#define SF_INST_RBPR            0x72     // Read Block-protection register
#define SF_INST_WBPR            0x42     // Write Block-protection register
#define SF_INST_LBPR            0x8D     // lock-down Block-protection register
#define SF_INST_ULBPR           0x98     // Global Block-protection register
//
// Prefix Opcode Index on the host SPI controller
//
typedef enum {
    SPI_WREN = 0,             // Prefix Opcode 0: Write Enable
    SPI_EWSR = 1,             // Prefix Opcode 1: Enable Write Status Register
    SPI_NOP  = 1,             // Prefix Opcode 1: No Operation
} PREFIX_OPCODE_INDEX;

//
// Opcode Menu Index on the host SPI controller
//
typedef enum {
    SPI_READ_ID = 0,           // Opcode 0: READ ID, Read cycle with address
    SPI_READ    = 1,           // Opcode 1: READ, Read cycle with address
    SPI_RDSR    = 2,           // Opcode 2: Read Status Register, No address
    SPI_WRDI    = 3,           // Opcode 3: Write Disable, No address
    SPI_RBPR    = 3,           // Opcode 3: Read Block Protection Register,No address
    SPI_SERASE  = 4,           // Opcode 4: Sector Erase (4KB), Write cycle with address
    SPI_BERASE  = 5,           // Opcode 5: Block Erase (32KB), Write cycle with address
    SPI_PROG    = 6,           // Opcode 6: Page Program, Write cycle with address
    SPI_WRSR    = 7,           // Opcode 7: Write Status Register, No address
    SPI_ULBPR   = 7,           // Opcode 7: Global block protection unlock,No address
} SPI_OPCODE_INDEX;












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

    mInitTable.BiosStartOffset = pDeviceTable->Size - PcdGet32(PcdFlashAreaSize); // this will be set by Fixed PCD value.
    mInitTable.BiosSize = PcdGet32(PcdFlashAreaSize);                             // this will be set by Fixed PCD value.
}

EFI_STATUS
EnableBlockProtectionForSST25 (
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
    SpiData = 0;  
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

EFI_STATUS
EnableBlockProtectionForSST26 (
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
		DEBUG((EFI_D_ERROR,"HX debug:flash block protection Status before unlock:%X\n",SpiStatus));
    }
	return Status;
}


EFI_STATUS
EnableBlockProtection (
    IN CONST NV_MEDIA_DEVICE_PROTOCOL* this,
    IN  BOOLEAN ProtectionEnable
)
{
	EFI_STATUS          Status;
    NV_DEVICE_INSTANCE  *DeviceInstance;
	UINT8		  FlashPartId[3];

    DeviceInstance = DEVICE_INSTANCE_FROM_DEVICEPROTOCOL (this);

    Status = DeviceInstance->SpiProtocol->Execute(
    				DeviceInstance->SpiProtocol,
    				SPI_READ_ID,
    				SPI_WREN,
    				TRUE,
    				FALSE,     // 20161010-HX:No Atomic cycle
    				FALSE,     // 20161010-HX:read
    				0,
    				3,
    				FlashPartId,
    				EnumSpiRegionAll
    				);
	if(EFI_ERROR(Status)){
		return Status;
	}
	
	DEBUG((EFI_D_INFO, "HX debug-DeviceSense ID:%02X%02X%02X\n", FlashPartId[0], FlashPartId[1], FlashPartId[2]));

	if(FlashPartId[0]==0xBF)
	{
		if(FlashPartId[1]==0x25)
		{
			DEBUG((EFI_D_INFO, "HX debug- firstly Disable flash block protection\n"));
			Status = EnableBlockProtectionForSST25 (this, ProtectionEnable);
   			 if (EFI_ERROR (Status)) {
       			 return EFI_ACCESS_DENIED;
   			 }
		}else if(FlashPartId[1]==0x26){
			DEBUG((EFI_D_INFO, "HX debug- firstly Disable flash block protection for SST26\n"));
			Status = EnableBlockProtectionForSST26 (this, ProtectionEnable);
   			if (EFI_ERROR (Status)) {
       			 return EFI_ACCESS_DENIED;
   		 	}
		}else{
			DEBUG((EFI_D_INFO, "HX debug- unknown flash decide\n"));
			return EFI_UNSUPPORTED;
		}
	}
	return Status;
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
    Status=EnableBlockProtection(this,FALSE);
    if(EFI_ERROR(Status)){
      return EFI_ACCESS_DENIED;
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


//DEBUG((EFI_D_INFO, "SpiRead\n"));

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


    DeviceInstance = DEVICE_INSTANCE_FROM_DEVICEPROTOCOL (this);

    LbaAddress = (UINTN)(0xFFFFFFFF) - (DeviceInstance->FlashSize * DeviceInstance->Number) + 1;
    
    if ((Address < LbaAddress) ||
            ((Address + Length - 1) > (UINTN)(0xFFFFFFFF)))
        return EFI_INVALID_PARAMETER;

    if (DeviceInstance->SpiProtocol == NULL)
        return EFI_UNSUPPORTED;

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
                 (UINT32) SpiAddress,
                 (UINT32) Length,
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

    LbaAddress = (UINTN)(0xFFFFFFFF) - (DeviceInstance->FlashSize * DeviceInstance->Number) + 1;

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


