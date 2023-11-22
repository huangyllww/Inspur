/** @file

Copyright (c) 2006 - 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  TcmPei.c

Abstract:
  Pei part of TCM Module.

Revision History:

Bug 3282 - improve TCM action stability.
TIME: 2012-01-06
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  1. update command flow as porting guide request.
  2. use retry mechanism.
$END------------------------------------------------------------

Bug 3223 - package ZTE SM3 hash source to .efi for ZTE's copyrights.
TIME: 2011-12-16
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  1. use ppi or protocol to let hash be independent.
$END------------------------------------------------------------

Bug 3144 - Add Tcm Measurement Architecture.
TIME: 2011-11-24
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  1. PEI: Measure CRTM Version.
          Measure Main Bios.
  2. DXE: add 'TCPA' acpi table.
          add event log.
          Measure Handoff Tables.
          Measure All Boot Variables.
          Measure Action.
  Note: As software of SM3's hash has not been implemented, so hash
        function is invalid.
$END------------------------------------------------------------

Bug 3075 - Add TCM support.
TIME: 2011-11-14
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  1. Tcm module init version.
     Only support setup function.
$END------------------------------------------------------------

**/




#include <Uefi.h>
#include <PiPei.h>
#include <Guid/TcmEventHob.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/SafeMemLib.h>
#include <Ppi/FirmwareVolumeInfo.h>
#include <Ppi/FirmwareVolume.h>
#include <Library/BaseCryptLib.h>
#include <Library/TMIoLib.h>
#include "TcmComm.h"


BOOLEAN mImageInMemory  = FALSE;

CHAR8 mSCrtmVersion[] = "{D20BC7C6-A1A5-415c-AE85-38290AB6BE04}";

//
// Number of firmware blobs to grow by each time we run out of room
//
#define FIRMWARE_BLOB_GROWTH_STEP 4

//EFI_PLATFORM_FIRMWARE_BLOB gMeasuredFvInfo[FixedPcdGet32(PcdPeiCoreMaxFvSupported)];
TCM_EFI_PLATFORM_FIRMWARE_BLOB *gMeasuredFvInfo;
UINT32 gMeasuredFvIndex = 0;
UINT32 mMeasuredMaxFvIndex = 0;


EFI_STATUS
EFIAPI
FirmwareVolmeInfoPpiNotifyCallback (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  gNotifyList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiFirmwareVolumeInfoPpiGuid,
    FirmwareVolmeInfoPpiNotifyCallback
  }
};

EFI_STATUS
EFIAPI
TcmPeimEntryMP (
  IN EFI_PEI_SERVICES           **PeiServices
  );

EFI_PEI_PPI_DESCRIPTOR  mTpmInitializationDonePpiList = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gPeiTpmInitializationDonePpiGuid,
  NULL
};

/**
  Single function calculates SM3 digest value for all raw data. It
  combines TCM_SM3Start, TCM_SM3Update, TCM_SM3Complete.

  @param  Data          Raw data to be digested.
  @param  DataLen       Size of the raw data.
  @param  Digest        Pointer to a buffer that stores the final digest.

  @retval EFI_SUCCESS   Always successfully calculate the final digest.
**/
EFI_STATUS
EFIAPI
TcmCommHashAll (
  IN  CONST UINT8        *Data,
  IN        UINTN        DataLen,
  OUT       TCM_DIGEST   *Digest
  )
{
  EFI_STATUS                    Status;

  Status = (UINTN)Sm3HashAll(Data, DataLen, (UINT8*)Digest);
  return Status;
}


/**
  Do a hash operation on a data buffer, extend a specific TCM PCR with the hash result,
  and build a GUIDed HOB recording the event which will be passed to the DXE phase and
  added into the Event Log.

  @param[in]      TcmRegs       TCM MMIO Base Address Pointer.
  @param[in]      HashData      Physical address of the start of the data buffer
                                to be hashed, extended, and logged.
  @param[in]      HashDataLen   The length, in bytes, of the buffer referenced by HashData.
  @param[in]      NewEventHdr   Pointer to a TCM_PCR_EVENT_HDR data structure.
  @param[in]      NewEventData  Pointer to the new event data.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
  @retval EFI_DEVICE_ERROR      The command was unsuccessful.

**/
EFI_STATUS
HashLogExtendEvent (
  IN      TCM_PC_REGISTERS   *TcmRegs,
  IN      UINT8              *HashData,
  IN      UINTN              HashDataLen,
  IN      TCM_PCR_EVENT_HDR  *NewEventHdr,
  IN      UINT8              *NewEventData
  )
{
  EFI_STATUS  Status;
  EFI_STATUS  Status1;
  VOID        *HobData;

  HobData = NULL;
  if(HashDataLen != 0){
    Status = TcmCommHashAll (
               HashData,
               HashDataLen,
               &NewEventHdr->Digest
               );
    if(EFI_ERROR(Status)){
      goto ProcExit;
    }
  }

  Status = TcmCommExtend (
             TcmRegs,
             &NewEventHdr->Digest,
             NewEventHdr->PCRIndex,
             NULL
             );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  HobData = BuildGuidHob (
             &gTcmEventEntryHobGuid,
             sizeof(*NewEventHdr) + NewEventHdr->EventSize
             );
  if (HobData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }

  Status1 = CopyMemBS (HobData, sizeof (*NewEventHdr), NewEventHdr, sizeof (*NewEventHdr));
  if (EFI_ERROR(Status1)) {
    DEBUG ((EFI_D_INFO, "%a line %d CopyMemBS failed.\n", __FUNCTION__, __LINE__));
  }

  HobData = (VOID *)((UINT8*)HobData + sizeof (*NewEventHdr));

  Status1 = CopyMemBS (HobData, NewEventHdr->EventSize, NewEventData, NewEventHdr->EventSize);
  if (EFI_ERROR(Status1)) {
    DEBUG ((EFI_D_INFO, "%a line %d CopyMemBS failed.\n", __FUNCTION__, __LINE__));
  }

ProcExit:
  return Status;
}




/**
  Measure CRTM version.

  @param[in]      TcmRegs       TCM MMIO Base Address Pointer.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
  @retval EFI_DEVICE_ERROR      The command was unsuccessful.

**/
EFI_STATUS
EFIAPI
MeasureCRTMVersion (
  IN      TCM_PC_REGISTERS   *TcmRegs
  )
{
  TCM_PCR_EVENT_HDR  TcmEventHdr;

  TcmEventHdr.PCRIndex  = 0;
  TcmEventHdr.EventType = TCM_EV_S_CRTM_VERSION;
  TcmEventHdr.EventSize = sizeof(mSCrtmVersion);
  return HashLogExtendEvent (
           TcmRegs,
           (UINT8*)&mSCrtmVersion,
           TcmEventHdr.EventSize,
           &TcmEventHdr,
           (UINT8*)&mSCrtmVersion
           );
}




/**
  Measure FV image.
  After measure it successfully, add it into the measured FV list.

  @param[in]  FvBase    Base address of FV image.
  @param[in]  FvLength  Length of FV image.

  @retval EFI_SUCCESS           Fv image is measured successfully
                                or it has been already measured.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
  @retval EFI_DEVICE_ERROR      The command was unsuccessful.

**/
EFI_STATUS
EFIAPI
MeasureFvImage (
  IN EFI_PHYSICAL_ADDRESS FvBase,
  IN UINT64               FvLength
  )
{
  UINT32                            Index;
  EFI_STATUS                        Status;
  TCM_EFI_PLATFORM_FIRMWARE_BLOB    FvBlob;
  TCM_PCR_EVENT_HDR                 TcmEventHdr;
  TCM_PC_REGISTERS                  *TcmRegs;

  TcmRegs = (TCM_PC_REGISTERS*)(UINTN)PcdGet64 (PcdTpmBaseAddress);

// Check whether FV is in the measured FV list.
  for(Index = 0; Index < gMeasuredFvIndex; Index ++){
    if(gMeasuredFvInfo[Index].BlobBase == FvBase){
      return EFI_SUCCESS;
    }
  }

// Measure and record the FV to the TCM
  FvBlob.BlobBase   = FvBase;
  FvBlob.BlobLength = FvLength;

  TcmEventHdr.PCRIndex = 0;
  TcmEventHdr.EventType = TCM_EV_EFI_PLATFORM_FIRMWARE_BLOB;
  TcmEventHdr.EventSize = sizeof(FvBlob);

  Status = HashLogExtendEvent (
             TcmRegs,
             (UINT8*)(UINTN)FvBlob.BlobBase,
             (UINTN)FvBlob.BlobLength,
             &TcmEventHdr,
             (UINT8*)&FvBlob
             );
  if(EFI_ERROR(Status)){
    TCM_MEM_LOG(("L%d:%X", __LINE__, Status));
    goto ProcExit;
  }

// Add new FV into the measured FV list.
//  ASSERT(gMeasuredFvIndex < FixedPcdGet32(PcdPeiCoreMaxFvSupported));
//  if(gMeasuredFvIndex < FixedPcdGet32(PcdPeiCoreMaxFvSupported)) {
  if(gMeasuredFvIndex >= mMeasuredMaxFvIndex) {

    gMeasuredFvInfo = ReallocatePool (
                            sizeof (TCM_EFI_PLATFORM_FIRMWARE_BLOB) * mMeasuredMaxFvIndex,
                            sizeof (TCM_EFI_PLATFORM_FIRMWARE_BLOB) * (mMeasuredMaxFvIndex + FIRMWARE_BLOB_GROWTH_STEP),
                            gMeasuredFvInfo
                            );
    if (gMeasuredFvInfo == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ProcExit;
    }
    mMeasuredMaxFvIndex = mMeasuredMaxFvIndex + FIRMWARE_BLOB_GROWTH_STEP;
  }
  gMeasuredFvInfo[gMeasuredFvIndex].BlobBase   = FvBase;
  gMeasuredFvInfo[gMeasuredFvIndex].BlobLength = FvLength;
  gMeasuredFvIndex++;

ProcExit:
  return Status;
}





/**
  Measure main BIOS.

  @param[in]      TcmRegs       TCM MMIO Base Address Pointer.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
  @retval EFI_DEVICE_ERROR      The command was unsuccessful.

**/
EFI_STATUS
EFIAPI
MeasureMainBios (
  IN      TCM_PC_REGISTERS   *TcmRegs
  )
{
  EFI_STATUS                        Status;
  UINT32                            FvInstances;
  EFI_PEI_FV_HANDLE                 VolumeHandle;
  EFI_FV_INFO                       VolumeInfo;
  EFI_PEI_FIRMWARE_VOLUME_PPI       *FvPpi;

  FvPpi          = NULL;
  FvInstances    = 0;
  VolumeHandle   = NULL;

  while (TRUE) {
    //
    // Traverse all firmware volume instances of Static Core Root of Trust for Measurement
    // (S-CRTM), this firmware volume measure policy can be modified/enhanced by special
    // platform for special CRTM TPM measuring.
    //
    Status = PeiServicesFfsFindNextVolume (FvInstances, &VolumeHandle);
    if (EFI_ERROR (Status)) {
      break;
    }

    //
    // Measure and record the firmware volume that is dispatched by PeiCore
    //
    Status = PeiServicesFfsGetVolumeInfo (VolumeHandle, &VolumeInfo);
    ASSERT_EFI_ERROR (Status);
    //
    // Locate the corresponding FV_PPI according to founded FV's format guid
    //
    Status = PeiServicesLocatePpi (
               &VolumeInfo.FvFormat,
               0,
               NULL,
               (VOID**)&FvPpi
               );
    if (!EFI_ERROR (Status)) {
      MeasureFvImage((EFI_PHYSICAL_ADDRESS)(UINTN)VolumeInfo.FvStart, VolumeInfo.FvSize);
    }

    FvInstances++;
  }

  return EFI_SUCCESS;
}






/**
  Measure and record the Firmware Volum Information once FvInfoPPI install.

  @param PeiServices       An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation
  @param NotifyDescriptor  Address of the notification descriptor data structure.
  @param Ppi               Address of the PPI that was installed.

  @retval EFI_SUCCESS    The FV Info is measured and recorded to TCM.
  @return if not EFI_SUCCESS, fail to measure FV.

**/
EFI_STATUS
EFIAPI
FirmwareVolmeInfoPpiNotifyCallback (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  )
{
  EFI_PEI_FIRMWARE_VOLUME_INFO_PPI  *Fv;
  EFI_STATUS                        Status;
  EFI_PEI_FIRMWARE_VOLUME_PPI       *FvPpi;

  FvPpi = NULL;
  Fv = (EFI_PEI_FIRMWARE_VOLUME_INFO_PPI *)Ppi;

  //
  // The PEI Core can not dispatch or load files from memory mapped FVs that do not support FvPpi.
  //
  Status = PeiServicesLocatePpi (
             &Fv->FvFormat,
             0,
             NULL,
             (VOID**)&FvPpi
             );
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  //
  // This is an FV from an FFS file, and the parent FV must have already been measured,
  // No need to measure twice, so just returns
  //
  if (Fv->ParentFvName != NULL || Fv->ParentFileName != NULL ) {
    return EFI_SUCCESS;
  }

  return MeasureFvImage((EFI_PHYSICAL_ADDRESS)(UINTN)Fv->FvInfo, Fv->FvInfoSize);
}







/**
  Check if TCM chip is activated or not.

  @param[in]      TcmRegs       TCM MMIO Base Address Pointer.

  @retval TRUE    TCM is activated.
  @retval FALSE   TCM is deactivated.

**/
BOOLEAN
EFIAPI
IsTcmUsable (
  IN TCM_PC_REGISTERS *TcmRegs
  )
{
  EFI_STATUS                        Status;
  BOOLEAN                           Deactivated;
  
  Deactivated = TRUE;

  Status = TcmCommGetCapability(TcmRegs, &Deactivated, NULL, NULL);
  if(EFI_ERROR(Status)){
    TCM_MEM_LOG(("L%d:%X", __LINE__, Status));
    return FALSE;
  }
  if(Deactivated){
    TCM_MEM_LOG(("Deactivated"));
  }
  return (BOOLEAN)(!Deactivated);
}






/**
  Do measurement after memory is ready.

  @param[in]      PeiServices   Describes the list of possible PEI Services.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
  @retval EFI_DEVICE_ERROR      The command was unsuccessful.

**/
EFI_STATUS
EFIAPI
TcmPeimEntryMP (
  IN EFI_PEI_SERVICES           **PeiServices
  )
{
  EFI_STATUS                Status;
  TCM_PC_REGISTERS          *TcmRegs;

  TCM_MEM_LOG(("MP"));

  TcmRegs = (TCM_PC_REGISTERS*)(UINTN)PcdGet64 (PcdTpmBaseAddress);
  Status = TcmPcRequestUseTcm(TcmRegs);
  if(EFI_ERROR(Status)){
    TCM_MEM_LOG(("L%d:%X", __LINE__, Status));
    goto ProcExit;
  }

  if(IsTcmUsable(TcmRegs)){
    Status = MeasureCRTMVersion(TcmRegs);
    if(EFI_ERROR(Status)){
      TCM_MEM_LOG(("L%d:%X", __LINE__, Status));
    }
    Status = MeasureMainBios(TcmRegs);
    if(EFI_ERROR(Status)){
      TCM_MEM_LOG(("L%d:%X", __LINE__, Status));
    }
    Status = PeiServicesNotifyPpi(&gNotifyList[0]);
    if(EFI_ERROR(Status)){
      TCM_MEM_LOG(("L%d:%X", __LINE__, Status));
    }
  }else{
    TCM_MEM_LOG(("L%d:UnUsable", __LINE__));
  }

ProcExit:
  return Status;
}






/**
  Entry point of this module.

  @param[in] FileHandle   Handle of the file being invoked.
  @param[in] PeiServices  Describes the list of possible PEI Services.

  @return Status.

**/
EFI_STATUS
EFIAPI
TcmPeimEntryMA (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                Status=EFI_NOT_FOUND;
  EFI_STATUS                Status2;
  EFI_BOOT_MODE             BootMode;
  volatile TCM_PC_REGISTERS *TcmRegs;
  UINT32                    TcmId;                // 0f00h

  TCM_MEM_LOG(("MA"));

  TcmId = TrustedModuleIoRead32((UINTN)PcdGet64(PcdTpmBaseAddress) + 0xF00);
  DEBUG ((EFI_D_INFO, "TcmId:%X\n", TcmId));
  if (TcmId != 0x00011B4E && TcmId != 0x02011B4E && TcmId != 0x06011B4E) {
    PcdSetBoolS (PcdTcmChipPresent, FALSE);
    goto ProcExit;
  }

  TcmRegs = NULL;
  ZeroMem (&BootMode, sizeof (EFI_BOOT_MODE));

#ifndef TCM_NO_MEASURE
  //
  // RegisterForShadow is to measure CRTMVersion and MainBios data
  //
  Status = (**PeiServices).RegisterForShadow(FileHandle);
  if (Status == EFI_ALREADY_STARTED) {
    mImageInMemory = TRUE;
  } else if (Status == EFI_NOT_FOUND) {
    ASSERT_EFI_ERROR (Status);
  }
#endif

  if(!mImageInMemory){
    Status = PeiServicesGetBootMode(&BootMode);
    ASSERT_EFI_ERROR(Status);
    TCM_MEM_LOG(("BM(%X)", BootMode));

    TcmRegs = (volatile TCM_PC_REGISTERS*)(UINTN)PcdGet64 (PcdTpmBaseAddress);
    Status = TcmPcRequestUseTcm(TcmRegs);
    if(EFI_ERROR (Status)){
      TCM_MEM_LOG(("L%d:%X", __LINE__, Status));
      goto ProcExit;
    }

    Status = TcmCommStartup(TcmRegs, BootMode);
    if(EFI_ERROR(Status)){
      TCM_MEM_LOG(("L%d:%X", __LINE__, Status));
      goto ProcExit;
    }

    Status = TcmCommContinueSelfTest(TcmRegs);
    if(EFI_ERROR(Status)){
      TCM_MEM_LOG(("L%d:%X", __LINE__, Status));
      goto ProcExit;
    }
  }else{
    Status = TcmPeimEntryMP ((EFI_PEI_SERVICES**)PeiServices);
    
    if (EFI_ERROR (Status)) {
      if (Status == EFI_NOT_FOUND) {
        DEBUG ((DEBUG_INFO, "Not Found TCM Chip!\n"));
        goto ProcExit;
      } else {
        ASSERT_EFI_ERROR(Status);
      }
    }
  }

ProcExit:
  //
  // Always install TpmInitializationDonePpi no matter success or fail.
  // Other driver can know TPM initialization state by TpmInitializedPpi.
  //
  Status2 = PeiServicesInstallPpi (&mTpmInitializationDonePpiList);
  ASSERT_EFI_ERROR (Status2);
  return Status;
}
