/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SmiFlash.c

Abstract:
  Provides Access to flash backup Services through SMI

Revision History:

**/

#include "SmiFlash.h"
#include <Framework/SmmCis.h>
#include <Library/IoLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/BiosIdLib.h>
#include <Library/UefiLib.h>
#include <Guid/Acpi.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/PnpSmbios.h>
#include <Token.h>
#include <ByoSmiFlashInfo.h>
#include <Library/ByoCommLib.h>
#include <Guid/SmmVariableCommon.h>
#include <SystemPasswordVariable.h>
#include <Protocol/SmmVariable.h>
#include <SetupVariable.h>
#include <Library/HobLib.h>


//---------------------------------------------------------------------------
NV_MEDIA_ACCESS_PROTOCOL              *mMediaAccess;
EFI_SMM_CPU_PROTOCOL                  *mSmmCpu;
UINT8                                 *BlockBuffer;
BOOLEAN                               EnvPrepared  = FALSE;
UINT16                                AcpiPmEnData = 0;
BIOS_ID_IMAGE                         gBiosIdImage;
UINT8                                 *mDisableKBCtrlAltDelReset = NULL;
extern EFI_PNP_SMBIOS_PROTOCOL        *mPnpSmbiosProtocol = NULL;
UINT8                                 *gResvPage;
SYSTEM_PASSWORD			 				gTse;
EFI_SMM_VARIABLE_PROTOCOL       	  *mSmmVariable       = NULL;


// oa3 Function
#pragma pack (1)
typedef struct {
  UINT32                           MsdmVersion;
  UINT32                           MsdmReserved;
  UINT32                           MdsmDataType;
  UINT32                           MsdmDataReserved;
  UINT32                           MsdmDataLength;
  UINT8                            MsdmData[29];      //5*5 Product Key, including "-"
} EFI_ACPI_MSDM_DATA_STRUCTURE;

//
// MSDM Table structure
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER        Header;
  EFI_ACPI_MSDM_DATA_STRUCTURE       MsdmData;
} EFI_ACPI_MS_DIGITAL_MARKER_TABLE;
#pragma pack ()


UINTN
GetOa3Base(
  VOID
  )
{
  return FLASH_REGION_NVSTORAGE_SUBREGION_NV_SMBIOS_STORE_BASE  + EFI_PAGE_SIZE;
}

VOID  *mRsdp3;


RSA*
d2i_RSAPublicKey(
  IN   void      *Ptr, 
  IN   UINT8     **PubDerKey, 
  IN   UINT32    DataSize
  );


VOID
GetRsdpTable(
  VOID
  )
{
  EFI_STATUS          Status;

  Status = EfiGetSystemConfigurationTable (
             &gEfiAcpiTableGuid, 
             &mRsdp3
             );
  DEBUG((EFI_D_ERROR, "GetRsdp: %r %lx\n", Status, mRsdp3));
}


UINTN
GetMsdmTableAddress(
  VOID
  )
{
  EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Rsdp3;
  EFI_ACPI_DESCRIPTION_HEADER                   *XsdtHdr;
  EFI_ACPI_DESCRIPTION_HEADER                   *TblHdr;
  UINTN                                         TableCount;
  UINT64                                        *TblAddr64;
  UINTN                                         Index;

  DEBUG((EFI_D_ERROR, "%a start\n", __FUNCTION__));

  DEBUG((EFI_D_ERROR, "Get acpi Table hdr\n"));
  Rsdp3 = (EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER *)mRsdp3;
  DEBUG((EFI_D_ERROR, "Found ACPI Table: %lx\n", Rsdp3));

  XsdtHdr = (EFI_ACPI_DESCRIPTION_HEADER*)(UINTN)Rsdp3->XsdtAddress;
  TableCount = (XsdtHdr->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) >> 3;
  TblAddr64  = (UINT64*)((UINT8*)XsdtHdr + sizeof(EFI_ACPI_DESCRIPTION_HEADER));
  DEBUG((EFI_D_ERROR, "Acpi Count: %x, TblAddress: %lx\n", TableCount, TblAddr64));
  for(Index = 0; Index < TableCount; ++Index) {
    TblHdr = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)(TblAddr64[Index]);
    if ((TblHdr->Signature == SIGNATURE_32('S','S','D','M')) || (TblHdr->Signature == SIGNATURE_32('M','S','D','M'))) {
      DEBUG((EFI_D_ERROR, "Found MsdmTable! %lx\n", (UINTN)TblHdr));
      return (UINTN)TblHdr;
    }
  }

  return 0;
}

VOID
CheckOa3(
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_ACPI_MSDM_DATA_STRUCTURE          *NvMsdmData;
  EFI_ACPI_TABLE_PROTOCOL               *AcpiTableProtocol;
  EFI_ACPI_MS_DIGITAL_MARKER_TABLE      *MsdmPointer;
  UINT32                                TableLength;
  UINTN                                 TableHandle;

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, &AcpiTableProtocol);
  if(EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Locate ApcitableProtocol %r\n", Status));
  }

  NvMsdmData = (EFI_ACPI_MSDM_DATA_STRUCTURE *)GetOa3Base();
  DEBUG((EFI_D_ERROR, "nvMsdmData: %x\n", NvMsdmData));
  if(NvMsdmData->MsdmVersion == 0xffffffff) {
    return;
  }

  TableLength = sizeof (EFI_ACPI_MS_DIGITAL_MARKER_TABLE);
  Status = gBS->AllocatePool(EfiACPIMemoryNVS, TableLength, &MsdmPointer); 
  ASSERT (MsdmPointer != NULL);
  if (MsdmPointer == NULL) {
    return;
  }

  //
  //Update the OA30 table header.
  //
  MsdmPointer->Header.Signature = SIGNATURE_32('S','S','D','M');
  MsdmPointer->Header.Length = TableLength;
  MsdmPointer->Header.Revision = 0x01;
  MsdmPointer->Header.CreatorRevision = 0x01;

  if(NvMsdmData->MsdmVersion != 0x54534554){
    MsdmPointer->Header.Signature = SIGNATURE_32('M','S','D','M');
  }

  //
  // Copy MSDM data to the memory pool created in OA table.
  //
  CopyMem (&(MsdmPointer->MsdmData), NvMsdmData, sizeof (EFI_ACPI_MSDM_DATA_STRUCTURE));

  //
  // Add the OA 3.0 table to ACPI memory.
  //
  TableHandle = 0;
  Status = AcpiTableProtocol->InstallAcpiTable (AcpiTableProtocol, MsdmPointer, TableLength, &TableHandle);
  DEBUG((EFI_D_INFO,"%a(%d) Status = %r\n",__FUNCTION__,__LINE__, Status));

}


static
UINT8
EFIAPI
CalculateCheckSum (
  IN      CONST UINT8              *Buffer,
  IN      UINTN                     Length
  )
{
  UINT8     CheckSum;
  UINTN     Count;

  ASSERT (Buffer != NULL);
  ASSERT (Length <= (MAX_ADDRESS - ((UINTN) Buffer) + 1));

  for (CheckSum = 0, Count = 0; Count < Length; Count++) {
    CheckSum = (UINT8) (CheckSum + *(Buffer + Count));
  }

  //
  // Return the checksum based on 2's complement.
  //
  return (UINT8) (0x100 - CheckSum);
}


EFI_STATUS
EFIAPI
UpdateOA30TableToMem (
  EFI_PHYSICAL_ADDRESS         Data
  )
{
  EFI_STATUS                                    Status = EFI_SUCCESS;
  EFI_ACPI_MS_DIGITAL_MARKER_TABLE              *MsdmPointer;
  EFI_PHYSICAL_ADDRESS                          MsdmAddress;
  UINT32                                        BufferSize;
  EFI_ACPI_MSDM_DATA_STRUCTURE                  *Address;

  Address = (EFI_ACPI_MSDM_DATA_STRUCTURE *)(UINTN)Data;

  MsdmPointer = NULL;
  BufferSize = sizeof (EFI_ACPI_MSDM_DATA_STRUCTURE);
  DEBUG((EFI_D_INFO,"OA30SMI:UpdateOA30Table\n"));
  DEBUG((EFI_D_INFO,"%a(%d) Data = %x\n",__FUNCTION__,__LINE__, Data));

  //if ((Address->MsdmVersion != SIGNATURE_32('M','S','D','M')) || (Address->MsdmDataLength != 0x1d)) {
  //  return EFI_NOT_AVAILABLE_YET;
  //}

  MsdmAddress = (EFI_PHYSICAL_ADDRESS)GetMsdmTableAddress();

  DEBUG((EFI_D_INFO,"MsdmAddress:%x\n",MsdmAddress));
  if (MsdmAddress == 0) {
    return EFI_NOT_FOUND;
  }

  MsdmPointer = (EFI_ACPI_MS_DIGITAL_MARKER_TABLE*)(UINTN)MsdmAddress;
  DEBUG((EFI_D_INFO,"MsdmPointer:%x\n",MsdmPointer));
  //
  //Compare the org MSDM Table with updated MSDM table
  //
  if (!CompareMem((UINT8*)(UINTN)&MsdmPointer->MsdmData, Address, sizeof (EFI_ACPI_MSDM_DATA_STRUCTURE))) {
    return EFI_NOT_AVAILABLE_YET;
  }
  CopyMem (&(MsdmPointer->MsdmData), Address, sizeof (EFI_ACPI_MSDM_DATA_STRUCTURE));
  DEBUG((EFI_D_INFO,"OA30SMI: Memory copied\n"));
  MsdmPointer->Header.Signature = SIGNATURE_32('M','S','D','M');
  MsdmPointer->Header.Checksum = 0;
  MsdmPointer->Header.Checksum = CalculateCheckSum((UINT8 *) MsdmPointer, MsdmPointer->Header.Length);
  DEBUG((EFI_D_INFO,"OA30SMI: new check sum = %x\n",MsdmPointer->Header.Checksum));

  return Status;
}


EFI_STATUS
ProgramFlash (
  IN  UINT32  Offset,
  IN  UINT32  Size,
  IN  UINT64  Buffer
)
{
  EFI_STATUS        Status = EFI_SUCCESS;
  UINT32            BiosAddr;
  VOID              *DataBuffer;
  volatile UINT64   *Data64;
  UINTN             Index;


  if(Offset + Size > PcdGet32(PcdFlashAreaSize)){
    return EFI_INVALID_PARAMETER;
  }

  BiosAddr = PcdGet32(PcdFlashAreaBaseAddress) + Offset;
  DataBuffer = (VOID*)(UINTN)Buffer;

  WriteBackInvalidateDataCacheRange((VOID*)(UINTN)BiosAddr, Size);
  if(CompareMem((VOID*)(UINTN)BiosAddr, DataBuffer, Size) == 0){
    DEBUG((EFI_D_INFO, "%X Equal\n", BiosAddr));
    goto ProcExit;
  }

  DEBUG((EFI_D_INFO, "[Update] %X -> %X L:%X\n", DataBuffer, BiosAddr, Size));
  
  Status = mMediaAccess->Erase (
                           mMediaAccess,
                           BiosAddr,
                           Size,
                           SPI_MEDIA_TYPE
                           );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Erase Error:%r\n", Status));
    goto ProcExit;
  }

  WriteBackInvalidateDataCacheRange((VOID*)(UINTN)BiosAddr, Size);
  Data64 = (volatile UINT64*)(UINTN)BiosAddr;
  for(Index=0;Index<Size/8;Index++){
    if(Data64[Index] != 0xFFFFFFFFFFFFFFFF){
      DEBUG((EFI_D_ERROR, "Not -1 at +%X = %X\n", &Data64[Index], Data64[Index]));
      Status = EFI_DEVICE_ERROR;
      goto ProcExit;
    }
  }
  
  Status = mMediaAccess->Write (
                           mMediaAccess,
                           BiosAddr,
                           DataBuffer,
                           Size,
                           SPI_MEDIA_TYPE
                           );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Write Error:%r\n", Status));
    goto ProcExit;
  }

  WriteBackInvalidateDataCacheRange((VOID*)(UINTN)BiosAddr, Size);
  if (CompareMem((VOID*)(UINTN)BiosAddr, DataBuffer, Size)){
    DEBUG((EFI_D_ERROR, "Write Verify Err\n"));
    Status = EFI_DEVICE_ERROR;
  }

ProcExit:
  return Status;
}



EFI_STATUS
WriteFlash (
  IN  UINT32  Offset,
  IN  UINT32  Size,
  IN  UINT64  Buffer
)
{
  EFI_STATUS        Status = EFI_SUCCESS;
  UINT32            BiosAddr;
  VOID              *DataBuffer;
  volatile UINT64   *Data64;
  UINTN             Index;


  if(Offset + Size > PcdGet32(PcdFlashAreaSize)){
    return EFI_INVALID_PARAMETER;
  }

  BiosAddr = PcdGet32(PcdFlashAreaBaseAddress) + Offset;
  DataBuffer = (VOID*)(UINTN)Buffer;

  DEBUG((EFI_D_INFO, "[Write] %X -> %X L:%X\n", DataBuffer, BiosAddr, Size));

  WriteBackInvalidateDataCacheRange((VOID*)(UINTN)BiosAddr, Size);
  Data64 = (volatile UINT64*)(UINTN)BiosAddr;
  for(Index=0;Index<Size/8;Index++){
    if(Data64[Index] != 0xFFFFFFFFFFFFFFFF){
      DEBUG((EFI_D_ERROR, "Not -1 at +%X = %X\n", &Data64[Index], Data64[Index]));
      Status = EFI_DEVICE_ERROR;
      goto ProcExit;
    }
  }
  
  Status = mMediaAccess->Write (
                           mMediaAccess,
                           BiosAddr,
                           DataBuffer,
                           Size,
                           SPI_MEDIA_TYPE
                           );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Write Error:%r\n", Status));
    goto ProcExit;
  }

  WriteBackInvalidateDataCacheRange((VOID*)(UINTN)BiosAddr, Size);
  if (CompareMem((VOID*)(UINTN)BiosAddr, DataBuffer, Size)){
    DEBUG((EFI_D_ERROR, "Write Verify Err\n"));
    Status = EFI_DEVICE_ERROR;
  }

ProcExit:
  return Status;
}



EFI_STATUS
EraseFlash (
  IN  UINT32  Offset,
  IN  UINT32  Size
)
{
  EFI_STATUS        Status = EFI_SUCCESS;
  UINT32            BiosAddr;
  volatile UINT64   *Data64;
  UINTN             Index;


  if(Offset + Size > PcdGet32(PcdFlashAreaSize)){
    return EFI_INVALID_PARAMETER;
  }

  BiosAddr = PcdGet32(PcdFlashAreaBaseAddress) + Offset;

  DEBUG((EFI_D_INFO, "[Erase] %X L:%X\n", BiosAddr, Size));

  Status = mMediaAccess->Erase (
                           mMediaAccess,
                           BiosAddr,
                           Size,
                           SPI_MEDIA_TYPE
                           );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Erase Error:%r\n", Status));
    goto ProcExit;
  }

  WriteBackInvalidateDataCacheRange((VOID*)(UINTN)BiosAddr, Size);
  Data64 = (volatile UINT64*)(UINTN)BiosAddr;
  for(Index=0;Index<Size/8;Index++){
    if(Data64[Index] != 0xFFFFFFFFFFFFFFFF){
      DEBUG((EFI_D_ERROR, "Not -1 at +%X = %X\n", &Data64[Index], Data64[Index]));
      Status = EFI_DEVICE_ERROR;
      goto ProcExit;
    }
  }

ProcExit:
  return Status;
}


EFI_STATUS CheckBiosId(BIOS_ID_IMAGE *Id)
{
  BIOS_ID_IMAGE  *OldBiosId;
  BIOS_ID_STRING *IdStr;
  EFI_STATUS     Status = EFI_OUT_OF_RESOURCES;      // Not match

  OldBiosId = &gBiosIdImage;
  if(CompareMem(OldBiosId->Signature, Id->Signature, sizeof(OldBiosId->Signature))){
    goto ProcExit;
  }

  IdStr = &gBiosIdImage.BiosIdString;
  if(CompareMem(IdStr->BoardId, Id->BiosIdString.BoardId, sizeof(IdStr->BoardId)) ||
     CompareMem(IdStr->OemId, Id->BiosIdString.OemId, sizeof(IdStr->OemId))){
    goto ProcExit;
  }
  
  Status = EFI_SUCCESS;

ProcExit:
  CopyMem(Id, &gBiosIdImage, sizeof(BIOS_ID_IMAGE));
  return Status;
}



VOID 
PatchForUsb ( 
    IN UINT8 PatchFlag 
  )
{
  if(PatchFlag == 1) {
    if(mDisableKBCtrlAltDelReset != NULL){
      *mDisableKBCtrlAltDelReset = 1;
    }
  } else if(PatchFlag == 0) {
    if(mDisableKBCtrlAltDelReset != NULL){
      *mDisableKBCtrlAltDelReset = 0;
    }
  } else if(PatchFlag == 2) {
    mDisableKBCtrlAltDelReset = (UINT8*)(UINTN)PcdGet64(PcdDisableKBCtrlAltDelResetDataPtr);
    //INIT ptr
    if(mDisableKBCtrlAltDelReset == NULL) {
        mDisableKBCtrlAltDelReset = AllocateReservedZeroMemoryBelow4G(gBS, sizeof(UINT8));
        PcdSet64S (PcdDisableKBCtrlAltDelResetDataPtr, (UINT64) (UINTN)mDisableKBCtrlAltDelReset);
    }
    ASSERT(mDisableKBCtrlAltDelReset != NULL);
  }
}



UINT8 CmosRead(UINT8 Address)
{
  IoWrite8(0x70, Address);
  return IoRead8(0x71);
}

VOID CmosWrite(UINT8 Address, UINT8 Data)
{
  IoWrite8(0x70, Address);
  IoWrite8(0x71, Data);
}





VOID
SetFirstPowerOnAfterFlashSpi()
{
	UINT8				 PowerOnFirst;
	CmosWrite(AFTER_FLASH_SPI, 0x0E); 
	PowerOnFirst = CmosRead(AFTER_FLASH_SPI);

  	DEBUG((EFI_D_INFO,"SaveSetupPassword : PowerOnFirst %x\n",PowerOnFirst));
	
 }

EFI_STATUS
SaveSetupPassword()
{
	EFI_STATUS			 Status;
	UINTN				 VarSize =0;
	EFI_GUID  gTseSetupGuid = SYSTEM_PASSWORD_GUID;
	UINT32					TSESetupAddress;
	UINT32					TSESetupAddMedia;
	UINTN	Index;
	TSESetupAddress = PcdGet32(PcdFvPswBase)-PcdGet32(PcdFlashAreaBaseAddress)+SIZE_4KB;
	TSESetupAddMedia = PcdGet32(PcdFvPswBase)+SIZE_4KB;

       ZeroMem(&gTse, sizeof (SYSTEM_PASSWORD));
	VarSize = sizeof(SYSTEM_PASSWORD);
	Status = mSmmVariable->SmmGetVariable (
	            SYSTEM_PASSWORD_NAME, 
	            &gTseSetupGuid, 
	            NULL, 
	            &VarSize, 
	            &gTse
	            );
  	DEBUG((EFI_D_INFO,"Get variable : status = %r\n",Status));
  	DEBUG((EFI_D_INFO,"Get variable : adminpassword present = %x  userpassword present = %x\n",gTse.bHaveAdmin,gTse.bHavePowerOn));
	
	DEBUG((EFI_D_INFO, "\n AdminHash:"));
	for(Index = 0;Index<12;Index++)
	{
		DEBUG((EFI_D_INFO, " %x", gTse.AdminHash[Index]));
		}

	
	DEBUG((EFI_D_INFO, "\n PowerOnHash:"));
	for(Index = 0;Index<12;Index++)
	{
		DEBUG((EFI_D_INFO, " %x", gTse.PowerOnHash[Index]));
		}

	DEBUG((EFI_D_INFO, "\n PowerOn:"));
	for(Index = 0;Index<20;Index++)
	{
		DEBUG((EFI_D_INFO, " %x", gTse.PowerOn[Index]));
		}

	DEBUG((EFI_D_INFO, "\n Admin:"));
	for(Index = 0;Index<20;Index++)
	{
		DEBUG((EFI_D_INFO, " %x", gTse.Admin[Index]));
		}

	
  	DEBUG((EFI_D_INFO,"EnteredType %x VerifyTimes %x VerifyTimesAdmin %x \n",gTse.EnteredType,gTse.VerifyTimes,gTse.VerifyTimesAdmin));

	DEBUG((EFI_D_INFO,"VerifyTimesPop %x RequirePopOnRestart %x ChangePopByUser %x \n",gTse.VerifyTimesPop,gTse.RequirePopOnRestart,gTse.ChangePopByUser));
	
	DEBUG((EFI_D_INFO,"SimplePassword %x PasswordValidDays %x HiddenEnteredType %x RebootTime %x\n",gTse.SimplePassword,gTse.PasswordValidDays,gTse.HiddenEnteredType,gTse.RebootTime));

	DEBUG((EFI_D_INFO,"AdmPwdTime Y %x M %x D %x \n",gTse.AdmPwdTime.Year,gTse.AdmPwdTime.Month,gTse.AdmPwdTime.Day));

	DEBUG((EFI_D_INFO,"PopPwdTime Y %x M %x D %x \n",gTse.PopPwdTime.Year,gTse.PopPwdTime.Month,gTse.PopPwdTime.Day));

	if((gTse.bHaveAdmin == 1) || (gTse.bHavePowerOn == 1)){
		  Status = ProgramFlash (
					 TSESetupAddress, 
					 SIZE_4KB, 
					 (UINT64)&gTse
					 );
    	}
	else{
		
		Status = mMediaAccess->Erase(
								 mMediaAccess,
								 TSESetupAddMedia,
								 SIZE_4KB,
								 SPI_MEDIA_TYPE
								 );

		}
   	return Status;
 }


EFI_STATUS
FlashInterface (
  IN     EFI_HANDLE               DispatchHandle,
  IN     CONST VOID               *Context,        OPTIONAL
  IN OUT VOID                     *CommBuffer,     OPTIONAL
  IN OUT UINTN                    *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS                   Status;
  UINTN                        Index;
  UINTN                        CpuIndex;
  UINT8                        SubFunction;
  BIOS_UPDATE_BLOCK_PARAMETER  *BlockParam;
  UINT32                       RegEax;
  UINT32                       RegEbx;
  EFI_SMM_SAVE_STATE_IO_INFO   IoState;
  UINT16                       AcpiIoBase;
  FD_AREA_INFO                 *FdArea;
  UINTN                        Size;
  UINT8                        CmosData;
  EFI_BOOT_MODE                BootMode;
  SETUP_DATA                   SetupData;
  
  BootMode = GetBootModeHob ();
//  SMM_VARIABLE_COMMUNICATE_HEADER  *VarCommHdr;
 // UINTN                            BufferSize;

  
  AcpiIoBase = PcdGet16(AcpiIoPortBaseAddress);   
  CpuIndex = 0;
  for(Index = 0; Index < gSmst->NumberOfCpus; Index++) {
    Status = mSmmCpu->ReadSaveState (
                        mSmmCpu,
                        sizeof (EFI_SMM_SAVE_STATE_IO_INFO),
                        EFI_SMM_SAVE_STATE_REGISTER_IO,
                        Index,
                        &IoState
                        );
    if(!EFI_ERROR (Status) && (IoState.IoData == SW_SMI_FLASH_SERVICES)) {
      CpuIndex = Index;
      break;
    }
  }
  
  if(Index >= gSmst->NumberOfCpus) {
    CpuDeadLoop ();
  }

  //
  // Ready save state for register
  //
  Status = mSmmCpu->ReadSaveState (
                      mSmmCpu,
                      sizeof (UINT32),
                      EFI_SMM_SAVE_STATE_REGISTER_RAX,
                      CpuIndex,
                      &RegEax
                      );
  ASSERT_EFI_ERROR (Status);

  Status = mSmmCpu->ReadSaveState (
                      mSmmCpu,
                      sizeof (UINT32),
                      EFI_SMM_SAVE_STATE_REGISTER_RBX,
                      CpuIndex,
                      &RegEbx
                      );
  ASSERT_EFI_ERROR (Status);

  SubFunction = (UINT8)(RegEax >> 8);
  DEBUG((EFI_D_ERROR, "SubFunction:%X\n",SubFunction));
  switch (SubFunction) {
    
    case SUBFUNCTION_GET_FD_AREA:
      FdArea = (FD_AREA_INFO*)(UINTN)RegEbx;

      switch(FdArea->Type) {

        default:
          DEBUG((EFI_D_INFO, "T:%X\n", FdArea->Type));
          Status = RETURN_INVALID_PARAMETER;
          break;
        
        case FD_AREA_TYPE_FD:
          FdArea->Offset = 0;
          FdArea->Size   = PcdGet32(PcdFlashAreaSize);
          Status = EFI_SUCCESS;
          break;

        case FD_AREA_TYPE_FV_LOGO:
          FdArea->Offset = PcdGet32(PcdFlashNvLogoBase) - PcdGet32(PcdFlashAreaBaseAddress);
          FdArea->Size   = PcdGet32(PcdFlashNvLogoSize);
          Status = EFI_SUCCESS;
          break;
      }

      if(!EFI_ERROR(Status)){
        DEBUG((EFI_D_INFO, "(%X,%X,%X)\n", FdArea->Type, FdArea->Offset, FdArea->Size));
      }
      
      break;

 /*   case SUBFUNCTION_BIOS_VERIFY:
      VarCommHdr = (SMM_VARIABLE_COMMUNICATE_HEADER*)gResvPage;
      BufferSize = sizeof(SMM_VARIABLE_COMMUNICATE_HEADER);
      ZeroMem(VarCommHdr, BufferSize);
      VarCommHdr->Function = SMM_VARIABLE_FUNCTION_SYNC_NVRAM;
      Status = gSmst->SmiManage (
                        &gEfiSmmVariableProtocolGuid, 
                        NULL, 
                        VarCommHdr, 
                        &BufferSize
                        );
      if(!EFI_ERROR(Status)){
        DEBUG((EFI_D_INFO, "(L%d) sync nvram: %r\n", __LINE__, VarCommHdr->ReturnStatus));
      }
      break;*/
      
     case SUBFUNCTION_BIOS_VERIFY:
      {
        int                         err;
        static EVP_MD_CTX           md_ctx;
        UINT8                       *PubDerKey;
        UINT8                       PubkeyHash1[SHA256_DIGEST_LENGTH];
        UINT8                       *PubkeyHash2 = NULL;
        EVP_PKEY                    *EvpPubKey = NULL;
        RSA                         *PubRsa;
        UINT32                      DataSize;
        UINT8                       *Data;
        BIOS_VERIFY_PARAMETER       *BiosVerifyPtr;
        BIOS_VERIFY_CAPSULE_BUFFER  *CapsuleBuffer;
        EFI_GUID                     SignGuid = gSignCapsuleHeaderGuild;
		DEBUG ((EFI_D_ERROR, "SUBFUNCTION_BIOS_VERIFY\n"));

        BiosVerifyPtr = (BIOS_VERIFY_PARAMETER *)(UINTN) RegEbx;
        Status = EFI_SUCCESS;
        switch(BiosVerifyPtr->SubFun) {
          case VERIFY_INIT:
            err = 0xFF;
            CapsuleBuffer = (BIOS_VERIFY_CAPSULE_BUFFER *)(UINTN) (BiosVerifyPtr->Buffer);
            if (CompareGuid(&SignGuid, &(CapsuleBuffer->Header.CapsuleGuid))) {
              PubDerKey     = CapsuleBuffer->Data;
              DataSize      = CapsuleBuffer->PubkeySize;
              SHA256 (PubDerKey, DataSize, PubkeyHash1);
              Status = GetPubkey (
                         (UINT8 *)(UINTN)PcdGet32(PcdFlashFvRecoveryBase),
                         (UINT8 *)(UINTN)((PcdGet32(PcdFlashFvRecoveryBase) - 16 + PcdGet32(PcdFlashFvRecoverySize))),
                         &PubkeyHash2
                         );
              if (EFI_ERROR(Status)) {
                Status = GetPubkey (
                           (UINT8 *)(UINTN)PcdGet32(PcdFlashFvMainBase),
                           (UINT8 *)(UINTN)((PcdGet32(PcdFlashFvMainBase) + PcdGet32(PcdFlashFvRecoverySize))),
                            &PubkeyHash2
                            );
                ASSERT (Status == EFI_SUCCESS);
              }
              if (!CompareMem(PubkeyHash2, PubkeyHash1, SHA256_DIGEST_LENGTH)) {
                err = EVP_VerifyInit(&md_ctx, EVP_sha256());
              }
            }
            DEBUG ((EFI_D_ERROR, "init Err:%d\n", err));
            break;
          case VERIFY_UPDATE:
            Data = (UINT8 *)(UINTN)BiosVerifyPtr->Buffer;
            err  = EVP_VerifyUpdate(&md_ctx, Data, BiosVerifyPtr->Size);
            DEBUG ((EFI_D_ERROR, "update Err:%d\n", err));
            break;
          case VERIFY_FINAL:
            CapsuleBuffer = (BIOS_VERIFY_CAPSULE_BUFFER *)(UINTN) (BiosVerifyPtr->Buffer);
            PubDerKey     = CapsuleBuffer->Data;
            DataSize      = CapsuleBuffer->PubkeySize;
            PubRsa = d2i_RSAPublicKey(NULL, &PubDerKey, DataSize);
            if((EvpPubKey = EVP_PKEY_new())==NULL){
              err = 3;
              goto ErrorExit;
            }
            if(EVP_PKEY_assign_RSA(EvpPubKey, PubRsa)!=1){
              err = 4;
              goto ErrorExit;
            }
            DataSize      = ALIGN_VALUE(DataSize, 4);
            Data          = &CapsuleBuffer->Data[DataSize];
            err = EVP_VerifyFinal(&md_ctx, Data, CapsuleBuffer->SignSize, EvpPubKey);
          ErrorExit:
            DEBUG ((EFI_D_ERROR, "final Err:%d\n", err));
            if (EvpPubKey != NULL) {
              EVP_PKEY_free (EvpPubKey);
            }
            if (err == 1) {//cert sign verify pass
              EVP_MD_CTX_cleanup(&md_ctx);
            }
            break;

          default:
            err = 0xff;
            break;
        }
        if (err != 1) {
          EVP_MD_CTX_cleanup(&md_ctx);
          Status = RETURN_ABORTED;
        }
      }
      break;
    case SUBFUNCTION_CLEAR:
	  break;

    case SUBFUNCTION_DISABLE_USB_POWERBUTTON:
      AcpiPmEnData = IoRead16(AcpiIoBase+2);
      IoWrite16(AcpiIoBase+2, AcpiPmEnData & (UINT16)~BIT8);  // disable power button.
      EnvPrepared = TRUE;
      PatchForUsb(1);
      break;

    
    case SUBFUNCTION_ENABLE_USB_POWERBUTTON:
      if(EnvPrepared){
        IoWrite16(AcpiIoBase, BIT8);           // clear power button status.
        IoWrite16(AcpiIoBase+2, AcpiPmEnData); // restore
        EnvPrepared = FALSE;
      }
      PatchForUsb(0);
      break;

        
    case SUBFUNCTION_UPDATE_SMBIOS_DATA:
      Status = HandleSmbiosDataRequest((UPDATE_SMBIOS_PARAMETER*)(UINTN)RegEbx);
      break;

    case SUBFUNCTION_PROGRAM_FLASH:
      BlockParam = (BIOS_UPDATE_BLOCK_PARAMETER*)(UINTN)RegEbx;
      // DEBUG((EFI_D_INFO, "%lX -> +%X L:%X\n", BlockParam->Buffer, BlockParam->Offset, BlockParam->Size));
      
	  if((BlockParam->Offset != PcdGet32(PcdFvPswBase)-PcdGet32(PcdFlashAreaBaseAddress))
		  &&(BlockParam->Offset != (PcdGet32(PcdFvPswBase)-PcdGet32(PcdFlashAreaBaseAddress)+SIZE_4KB)))
		  {
      		Status = ProgramFlash (
                 BlockParam->Offset, 
                 BlockParam->Size, 
                 BlockParam->Buffer
                 );
	  	}
      break;

    case SUBFUNCTION_WRITE_FLASH:
      BlockParam = (BIOS_UPDATE_BLOCK_PARAMETER*)(UINTN)RegEbx;      
      Status = WriteFlash (
                 BlockParam->Offset, 
                 BlockParam->Size, 
                 BlockParam->Buffer
                 );
      break;

    case SUBFUNCTION_ERASE_FLASH:
      BlockParam = (BIOS_UPDATE_BLOCK_PARAMETER*)(UINTN)RegEbx;      
      Status = EraseFlash (
                 BlockParam->Offset, 
                 BlockParam->Size
                 );
      break;
    
    case SUBFUNCTION_OA3_UPDATE:
      {
        ROM_HOLE_PARAMETER        *RomBlock;

        RomBlock = (ROM_HOLE_PARAMETER *)(UINTN)RegEbx;
        Status = UpdateOA30TableToMem (RomBlock->Buffer);
        if(!EFI_ERROR(Status) || Status == EFI_NOT_FOUND) {
          DEBUG((EFI_D_ERROR, "UpdateOa30ToMem: %r\n", Status));

        Status = mMediaAccess->Erase(
                                 mMediaAccess,
                                 (UINT32)(UINTN)(FLASH_REGION_NVSTORAGE_SUBREGION_NV_SMBIOS_STORE_BASE + EFI_PAGE_SIZE),
                                 EFI_PAGE_SIZE,
                                 SPI_MEDIA_TYPE
                                 );
        DEBUG((EFI_D_ERROR, " Erase Done\n"));
        Status = mMediaAccess->Write(
                                 mMediaAccess,
                                 (UINT32)(UINTN)(FLASH_REGION_NVSTORAGE_SUBREGION_NV_SMBIOS_STORE_BASE + EFI_PAGE_SIZE),
                                 (void *)(UINTN)(RomBlock->Buffer),
                                 EFI_PAGE_SIZE,
                                 SPI_MEDIA_TYPE
                                 );
       DEBUG((EFI_D_ERROR, " Write Done\n"));
        }
      }
      break;


    case SUBFUNCTION_WRITECMOS:
      BlockParam = (BIOS_UPDATE_BLOCK_PARAMETER*)(UINTN)RegEbx; 
      CmosData = (UINT8)(BlockParam->Size & 0xF);
      CmosData = (((~CmosData) & 0xF) << 4)|CmosData;
      CmosWrite(RECOVERY_STEP_CMOS_OFFSET, CmosData);
      DEBUG((EFI_D_INFO, "Cmos(%X)=%X\n", RECOVERY_STEP_CMOS_OFFSET, CmosData));
      Status = EFI_SUCCESS;
      break;

    case SUBFUNCTION_CHECK_BIOS_LOCK:
      Status = EFI_SUCCESS;
	  Size = sizeof (SETUP_DATA);
	    Status = mSmmVariable->SmmGetVariable (
	            PLATFORM_SETUP_VARIABLE_NAME, 
	            &gPlatformSetupVariableGuid, 
	            NULL, 
	            &Size, 
	            &SetupData
	            );

  	DEBUG((EFI_D_INFO,"GetVariable SetupData.BIOSLock %x\n",SetupData.BIOSLock));

    if(SetupData.BIOSLock == 1){
        Status = EFI_ACCESS_DENIED;
        }
      break; 

    case SUBFUNCTION_PATCH_FOR_USB:
      Status = EFI_SUCCESS;
      break;

	case SUBFUNCTION_CHECK_FOR_PASSWORD:
		if(BootMode!=BOOT_IN_RECOVERY_MODE){
		  Status = SaveSetupPassword();	
		  DEBUG ((EFI_D_ERROR, "SmiFlash SaveSetupPassword: %r\n",Status));
	    }
        break;
	case SUBFUNCTION_SET_FOR_FIRST_POWERON:
	  	SetFirstPowerOnAfterFlashSpi();
        break;
		 
    case SUBFUNCTION_READ_FLASH_BLOCK:
      BlockParam = (BIOS_UPDATE_BLOCK_PARAMETER*)(UINTN)RegEbx;
      // DEBUG((EFI_D_INFO, "+%X -> %lX L:%X\n", BlockParam->Offset, BlockParam->Buffer, BlockParam->Size));
      if(BlockParam->Offset + BlockParam->Size > PcdGet32(PcdFlashAreaSize)){
        Status = EFI_INVALID_PARAMETER;
        break;
      }
      Size = BlockParam->Size;
      Status = mMediaAccess->Read (
                               mMediaAccess,
                               PcdGet32(PcdFlashAreaBaseAddress) + BlockParam->Offset,
                               (UINT8*)(UINTN)BlockParam->Buffer,
                               &Size,
                               SPI_MEDIA_TYPE
                               );
      break;


    case SUBFUNCTION_CHECK_BIOS_ID:
      Status = CheckBiosId((BIOS_ID_IMAGE*)(UINTN)RegEbx);
      DEBUG ((EFI_D_ERROR, "CheckBiosId: %r\n",Status));
      break;

    
    default:
      Status = RETURN_INVALID_PARAMETER;
      break;
  }

  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "RC:%r\n", Status));
  }

// error in AH.
  RegEax &= 0xFFFF00FF;
  RegEax |= ((Status & 0xFF) << 8);
  Status = mSmmCpu->WriteSaveState (
                      mSmmCpu,
                      sizeof (UINT32),
                      EFI_SMM_SAVE_STATE_REGISTER_RAX,
                      CpuIndex,
                      &RegEax
                      );
  ASSERT_EFI_ERROR (Status);

  return Status;
}


EFI_STATUS
SmiFlashEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS            Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL  *SwDispatch = NULL;
  EFI_SMM_SW_REGISTER_CONTEXT   SwContext;
  EFI_HANDLE                    Handle;


  DEBUG((EFI_D_INFO, "SmiFlash\n"));

  Status = GetBiosId(&gBiosIdImage);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmCpuProtocolGuid,
                    NULL,
                    (VOID**)&mSmmCpu
                    );
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid,
                    NULL,
                    (VOID**)&SwDispatch
                    );
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmNvMediaAccessProtocolGuid,
                    NULL,
                    (VOID**)&mMediaAccess
                    );
  ASSERT_EFI_ERROR (Status);   

  Status = gSmst->SmmLocateProtocol (
                    &gEfiPnpSmbiosProtocolGuid,
                    NULL,
                    (VOID**)&mPnpSmbiosProtocol
                    );

  
  //
	 // Locate SmmVariableProtocol.
	 //
	 Status = gBS->LocateProtocol (
	   &gEfiSmmVariableProtocolGuid, 
	   NULL, 
	   (VOID**)&mSmmVariable
	   );
	 DEBUG ((EFI_D_ERROR, "SmmVariableProtocol Status = %r\n", Status));
	 if (EFI_ERROR (Status)) {
	   return Status;
	 }
     
  SwContext.SwSmiInputValue = SW_SMI_FLASH_SERVICES;
  Status = SwDispatch->Register (
                         SwDispatch,
                         FlashInterface,
                         &SwContext,
                         &Handle
                         );
  ASSERT_EFI_ERROR (Status);

  gResvPage = AllocateReservedZeroMemoryBelow4G(gBS, SIZE_4KB);
  ASSERT(gResvPage != NULL);
  
  GetRsdpTable();
  CheckOa3();
  PatchForUsb(2);
  
  return Status;
}

