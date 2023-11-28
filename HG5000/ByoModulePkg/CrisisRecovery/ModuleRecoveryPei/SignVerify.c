
#include <Guid\CapsuleRecord.h>
#include <Pi\PiHob.h>
#include <Pi\PiPeiCis.h>
#include <Pi\PiFirmwareFile.h>
#include <Ppi\FirmwareVolume.h>
#include <Library\BaseMemoryLib.h>
#include <Ppi\RecoveryModule.h>
#include <Ppi\DeviceRecoveryModule.h>
#include <Uefi\UefiBaseType.h>
#include <Guid\BiosId.h>
#include <Library\PeiServicesTablePointerLib.h>
#include <Library\PcdLib.h>
#include <Library\DebugLib.h>
#include <Library\HobLib.h>
#include <Library\PeiServicesLib.h>
#include <Library\ReportStatusCodeLib.h>
#include <Guid/FirmwareFileSystem2.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <RsaSignDataHob.h>
#include <Library/ByoCommLib.h>
#include <Protocol/CryptoLibDxeProtocol.h>



EFI_STATUS
BiosRsaVerify (
  IN UINT8    *BiosData,
  IN UINT32   BiosDataSize,
  IN UINT8    *SignData,
  IN UINT32   SignDataSize
  )
{
  EFI_STATUS                    Status;
  EFI_PEI_HOB_POINTERS          GuidHob;
  PK_RSA2048_HOB_DATA           *PubKey;
  UINT32                        Crc32;
  CRYPTO_LIB_PROTOCOL           *CryptoLib = NULL;
  BOOLEAN                       Rc;
  

  GuidHob.Raw = GetFirstGuidHob(&gByoSignBiosPubKeyFileNameGuid);
  ASSERT(GuidHob.Raw != NULL);
  if(GuidHob.Raw == NULL){
    Status = EFI_NOT_FOUND;
    goto ProcExit;     
  }
  PubKey = GET_GUID_HOB_DATA(GuidHob.Guid);
  LibCalcCrc32(PubKey->KeyN, PubKey->Hdr.KeySize, &Crc32);
  ASSERT(Crc32 == PubKey->Hdr.Crc32);
  if(Crc32 != PubKey->Hdr.Crc32){
    Status = EFI_ABORTED;
    goto ProcExit; 
  }

  DEBUG((EFI_D_INFO, "%a(%X,%X,%X,%X)\n", __FUNCTION__, BiosData, BiosDataSize, SignData, SignDataSize));

  if(BiosDataSize != PcdGet32(PcdFlashAreaSize) || BiosData == NULL || SignData == NULL){
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;    
  }

  Status = PeiServicesLocatePpi (
             &gCryptoLibPpiGuid,
             0,
             NULL,
             (VOID**)&CryptoLib
             );
  if(EFI_ERROR(Status)){
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }

  DEBUG((EFI_D_INFO, "DataRsaPkcs1Verify\n"));
  Rc = CryptoLib->DataRsaPkcs1Verify(
                    BiosData, 
                    BiosDataSize,
                    SignData, 
                    SignDataSize,
                    PubKey->KeyN, 
                    sizeof(PubKey->KeyN),
                    PubKey->KeyE, 
                    sizeof(PubKey->KeyE)
                    );
  DEBUG((EFI_D_INFO, "DataRsaPkcs1VerifyEnd\n"));  
  if (!Rc) {
    Status = EFI_SECURITY_VIOLATION;
    goto ProcExit;
  } 

  Status = EFI_SUCCESS;

ProcExit:
  return Status;
}








EFI_STATUS 
VerifyBiosSign(
  EFI_PEI_SERVICES                  **PeiServices,
  VOID                              *BiosData, 
  UINTN                              BiosSize, 
  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI *Recovery
  )
{
  EFI_STATUS  Status;
  UINTN       Numbers;
  UINTN       Index;
  UINTN       Size;
  EFI_GUID    DeviceId;
  UINT8       *SignBuffer = NULL;
  UINT64      Address;
  BOOLEAN     Verified = FALSE;


  DEBUG((EFI_D_ERROR, "%a\n", __FUNCTION__));

  Status = Recovery->GetNumberRecoveryCapsules (
                      PeiServices,
                      Recovery,
                      &Numbers
                      ); 
  if(Numbers == 0){
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }  

  for (Index = 1; Index <= Numbers; Index++) {
    Status = Recovery->GetRecoveryCapsuleInfo (
                          PeiServices,
                          Recovery,
                          Index,
                          &Size,
                          &DeviceId
                          );
    if(EFI_ERROR(Status)){
      continue;
    }

    DEBUG((EFI_D_INFO, "[%d] size:%X\n", Index, Size));
    
    if(Size != RSA2048_SIGN_SIZE){
      continue;
    }

    Status = (*PeiServices)->AllocatePages (
                PeiServices,
                EfiBootServicesData,
                EFI_SIZE_TO_PAGES(Size),
                &Address
                );
    if (EFI_ERROR(Status)) {
      DEBUG ((EFI_D_ERROR, "AllocatePages:%r\n", Status));								
      goto ProcExit;
    }

    SignBuffer = (UINT8*)(UINTN)Address;
    Status = Recovery->LoadRecoveryCapsule (
                         PeiServices,
                         Recovery,
                         Index,
                         SignBuffer
                         );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "LoadRecoveryCapsule:%r\n", Status));								
      goto ProcExit;
    }

    Status = BiosRsaVerify((UINT8*)BiosData, BiosSize, SignBuffer, Size);
    if(!EFI_ERROR(Status)){
      Verified = TRUE;
    }
    break;
  }

ProcExit:
  if(Verified){
    return EFI_SUCCESS;
  } else {
    if (EFI_ERROR(Status)){
      return Status;
    } else {
      return EFI_NOT_FOUND;
    }
  }
}


