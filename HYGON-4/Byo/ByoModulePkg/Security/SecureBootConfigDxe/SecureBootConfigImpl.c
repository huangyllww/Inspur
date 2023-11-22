/** @file
  HII Config Access protocol implementation of SecureBoot configuration module.

Copyright (c) 2011 - 2022, Byosoft Corporation.<BR>

All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include "SecureBootConfigImpl.h"
#include <Library/BaseCryptLib.h>
#include <Library/SetupUiLib.h>
#include <Library/SecureBootRestoreLib.h>
#include <Protocol/SetupItemUpdateNotify.h>
#include <Library/HiiLib.h>
#include <Library/ByoCommLib.h>


EFI_STATUS LibAuthVarSetPhysicalPresent(BOOLEAN Present);

CHAR16              mSecureBootStorageName[] = L"SECUREBOOT_CONFIGURATION";

SECUREBOOT_CONFIG_PRIVATE_DATA         *mPrivateData = NULL;
SECUREBOOT_CONFIG_PRIVATE_DATA         mSecureBootConfigPrivateDateTemplate = {
  SECUREBOOT_CONFIG_PRIVATE_DATA_SIGNATURE,
  {
    SecureBootExtractConfig,
    SecureBootRouteConfig,
    SecureBootCallback
  }
};

HII_VENDOR_DEVICE_PATH          mSecureBootHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    SECUREBOOT_CONFIG_FORM_SET_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};


BOOLEAN mIsEnterSecureBootForm = FALSE;
BOOLEAN mIsSelectedSecureBootModeForm = FALSE;
BOOLEAN mIsSecureBootModeChanged = FALSE;

EFI_HII_DATE       mDate ;
EFI_HII_TIME       mTime ;
//
// OID ASN.1 Value for Hash Algorithms
//
UINT8 mHashOidValue[] = {
  0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x02, 0x05,         // OBJ_md5
  0x2B, 0x0E, 0x03, 0x02, 0x1A,                           // OBJ_sha1
  0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x04,   // OBJ_sha224
  0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01,   // OBJ_sha256
  0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02,   // OBJ_sha384
  0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03,   // OBJ_sha512
  };

HASH_TABLE mHash[] = {
  { L"SHA224", 28, &mHashOidValue[13], 9, NULL,                 NULL,       NULL,         NULL       },
  { L"SHA256", 32, &mHashOidValue[22], 9, Sha256GetContextSize, Sha256Init, Sha256Update, Sha256Final},
  { L"SHA384", 48, &mHashOidValue[31], 9, Sha384GetContextSize, Sha384Init, Sha384Update, Sha384Final},
  { L"SHA512", 64, &mHashOidValue[40], 9, Sha512GetContextSize, Sha512Init, Sha512Update, Sha512Final}
};
//
// Variable Definitions
//
UINT32            mPeCoffHeaderOffset = 0;
WIN_CERTIFICATE   *mCertificate = NULL;
IMAGE_TYPE        mImageType;
UINT8             *mImageBase = NULL;
UINTN             mImageSize = 0;
UINT8             mImageDigest[MAX_DIGEST_SIZE];
UINTN             mImageDigestSize;
EFI_GUID          mCertType;
EFI_IMAGE_SECURITY_DATA_DIRECTORY    *mSecDataDir = NULL;
EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION  mNtHeader;
//
// Possible DER-encoded certificate file suffixes, end with NULL pointer.
//
CHAR16* mDerEncodedSuffix[] = {
  L".cer",
  L".der",
  L".crt",
  NULL
};
CHAR16* mSupportX509Suffix = L"*.cer/der/crt";
//
// Prompt strings during certificate enrollment.
//
CHAR16* mX509EnrollPromptTitle[3] = {
 L""
};
CHAR16* mX509EnrollPromptString[3] = {
 L""
};

SECUREBOOT_CONFIG_PRIVATE_DATA  *gSecureBootPrivateData = NULL;

EFI_STATUS 
SetupItemSecureBootNotify(
  SETUP_ITEM_UPDATE_NOTIFY_PROTOCOL    *This,
  UINTN                                Item, 
  UINTN                                NewStatus
  );

SETUP_ITEM_UPDATE_NOTIFY_PROTOCOL mSetupItemUpdateNotifyProtocol = {
  SetupItemSecureBootNotify
};





VOID
KeepSecureBootStatus (
  VOID
)
{
  
  UpdateSecureBootString(mPrivateData);
  
}
/**
  This code cleans up enrolled file by closing file & free related resources attached to
  enrolled file.

  @param[in] FileContext            FileContext cached in SecureBootConfig driver

**/
VOID
CloseEnrolledFile(
  IN SECUREBOOT_FILE_CONTEXT *FileContext
)
{
  if (FileContext->FHandle != NULL) {
    CloseFile (FileContext->FHandle);
    FileContext->FHandle = NULL;
  }

  if (FileContext->FileName != NULL){
    FreePool(FileContext->FileName);
    FileContext->FileName = NULL;
  }
  FileContext->FileType = UNKNOWN_FILE_TYPE;

}

/**
  This code checks if the FileSuffix is one of the possible DER-encoded certificate suffix.

  @param[in] FileSuffix            The suffix of the input certificate file

  @retval    TRUE           It's a DER-encoded certificate.
  @retval    FALSE          It's NOT a DER-encoded certificate.

**/
BOOLEAN
IsDerEncodeCertificate (
  IN CONST CHAR16         *FileSuffix
)
{
  UINTN     Index;
  for (Index = 0; mDerEncodedSuffix[Index] != NULL; Index++) {
    if (StrCmp (FileSuffix, mDerEncodedSuffix[Index]) == 0) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  This code checks if the file content complies with EFI_VARIABLE_AUTHENTICATION_2 format
The function reads file content but won't open/close given FileHandle.

  @param[in] FileHandle            The FileHandle to be checked

  @retval    TRUE            The content is EFI_VARIABLE_AUTHENTICATION_2 format.
  @retval    FALSE          The content is NOT a EFI_VARIABLE_AUTHENTICATION_2 format.

**/
BOOLEAN
IsAuthentication2Format (
  IN   EFI_FILE_HANDLE    FileHandle
)
{
  EFI_STATUS                     Status;
  EFI_VARIABLE_AUTHENTICATION_2  *Auth2;
  BOOLEAN                        IsAuth2Format;

  IsAuth2Format = FALSE;

  //
  // Read the whole file content
  //
  Status = ReadFileContent(
             FileHandle,
             (VOID **) &mImageBase,
             &mImageSize,
             0
             );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  Auth2 = (EFI_VARIABLE_AUTHENTICATION_2 *)mImageBase;
  if (Auth2->AuthInfo.Hdr.wCertificateType != WIN_CERT_TYPE_EFI_GUID) {
    goto ON_EXIT;
  }

  if (CompareGuid(&gEfiCertPkcs7Guid, &Auth2->AuthInfo.CertType)) {
    IsAuth2Format = TRUE;
  }

ON_EXIT:
  //
  // Do not close File. simply check file content
  //
  if (mImageBase != NULL) {
    FreePool (mImageBase);
    mImageBase = NULL;
  }

  return IsAuth2Format;
}


/**
  Create a time based data payload by concatenating the EFI_VARIABLE_AUTHENTICATION_2
  descriptor with the input data. NO authentication is required in this function.

  @param[in, out]   DataSize       On input, the size of Data buffer in bytes.
                                   On output, the size of data returned in Data
                                   buffer in bytes.
  @param[in, out]   Data           On input, Pointer to data buffer to be wrapped or
                                   pointer to NULL to wrap an empty payload.
                                   On output, Pointer to the new payload date buffer allocated from pool,
                                   it's caller's responsibility to free the memory when finish using it.

  @retval EFI_SUCCESS              Create time based payload successfully.
  @retval EFI_OUT_OF_RESOURCES     There are not enough memory resources to create time based payload.
  @retval EFI_INVALID_PARAMETER    The parameter is invalid.
  @retval Others                   Unexpected error happens.

**/
EFI_STATUS
CreateTimeBasedPayload (
  IN OUT UINTN            *DataSize,
  IN OUT UINT8            **Data
  )
{
  EFI_STATUS                       Status;
  UINT8                            *NewData;
  UINT8                            *Payload;
  UINTN                            PayloadSize;
  EFI_VARIABLE_AUTHENTICATION_2    *DescriptorData;
  UINTN                            DescriptorSize;
  EFI_TIME                         Time;

  if (Data == NULL || DataSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // In Setup mode or Custom mode, the variable does not need to be signed but the
  // parameters to the SetVariable() call still need to be prepared as authenticated
  // variable. So we create EFI_VARIABLE_AUTHENTICATED_2 descriptor without certificate
  // data in it.
  //
  Payload     = *Data;
  PayloadSize = *DataSize;

  DescriptorSize    = OFFSET_OF (EFI_VARIABLE_AUTHENTICATION_2, AuthInfo) + OFFSET_OF (WIN_CERTIFICATE_UEFI_GUID, CertData);
  NewData = (UINT8*) AllocateZeroPool (DescriptorSize + PayloadSize);
  if (NewData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if ((Payload != NULL) && (PayloadSize != 0)) {
    CopyMem (NewData + DescriptorSize, Payload, PayloadSize);
  }

  DescriptorData = (EFI_VARIABLE_AUTHENTICATION_2 *) (NewData);

  ZeroMem (&Time, sizeof (EFI_TIME));
  Status = gRT->GetTime (&Time, NULL);
  if (EFI_ERROR (Status)) {
    FreePool(NewData);
    return Status;
  }
  Time.Pad1       = 0;
  Time.Nanosecond = 0;
  Time.TimeZone   = 0;
  Time.Daylight   = 0;
  Time.Pad2       = 0;
  CopyMem (&DescriptorData->TimeStamp, &Time, sizeof (EFI_TIME));

  DescriptorData->AuthInfo.Hdr.dwLength         = OFFSET_OF (WIN_CERTIFICATE_UEFI_GUID, CertData);
  DescriptorData->AuthInfo.Hdr.wRevision        = 0x0200;
  DescriptorData->AuthInfo.Hdr.wCertificateType = WIN_CERT_TYPE_EFI_GUID;
  CopyGuid (&DescriptorData->AuthInfo.CertType, &gEfiCertPkcs7Guid);

  if (Payload != NULL) {
    FreePool(Payload);
  }

  *DataSize = DescriptorSize + PayloadSize;
  *Data     = NewData;
  return EFI_SUCCESS;
}

/**
  Internal helper function to delete a Variable given its name and GUID, NO authentication
  required.

  @param[in]      VariableName            Name of the Variable.
  @param[in]      VendorGuid              GUID of the Variable.

  @retval EFI_SUCCESS              Variable deleted successfully.
  @retval Others                   The driver failed to start the device.

**/
EFI_STATUS
DeleteVariable (
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid
  )
{
  EFI_STATUS              Status;
  VOID*                   Variable;
  UINT8                   *Data;
  UINTN                   DataSize;
  UINT32                  Attr;

  GetVariable2 (VariableName, VendorGuid, &Variable, NULL);
  if (Variable == NULL) {
    return EFI_SUCCESS;
  }
  FreePool (Variable);

  Data     = NULL;
  DataSize = 0;
  Attr     = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS
             | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;

  Status = CreateTimeBasedPayload (&DataSize, &Data);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
    return Status;
  }

  Status = gRT->SetVariable (
                  VariableName,
                  VendorGuid,
                  Attr,
                  DataSize,
                  Data
                  );
  if (Data != NULL) {
    FreePool (Data);
  }
  return Status;
}

/**

  Set the platform secure boot mode into "Custom" or "Standard" mode.

  @param[in]   SecureBootMode        New secure boot mode: STANDARD_SECURE_BOOT_MODE or
                                     CUSTOM_SECURE_BOOT_MODE.

  @return EFI_SUCCESS                The platform has switched to the special mode successfully.
  @return other                      Fail to operate the secure boot mode.

**/
EFI_STATUS
SetSecureBootMode (
  IN     UINT8         SecureBootMode
  )
{
  return gRT->SetVariable (
                EFI_CUSTOM_MODE_NAME,
                &gEfiCustomModeEnableGuid,
                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                sizeof (UINT8),
                &SecureBootMode
                );
}

/**
  This code checks if the encode type and key strength of X.509
  certificate is qualified.

  @param[in]  X509FileContext     FileContext of X.509 certificate storing
                                  file.
  @param[out] Error               Error type checked in the certificate.

  @return EFI_SUCCESS             The certificate checked successfully.
  @return EFI_INVALID_PARAMETER   The parameter is invalid.
  @return EFI_OUT_OF_RESOURCES    Memory allocation failed.

**/
EFI_STATUS
CheckX509Certificate (
  IN    SECUREBOOT_FILE_CONTEXT*    X509FileContext,
  OUT   ENROLL_KEY_ERROR*           Error
)
{
  EFI_STATUS     Status;
  UINT16*        FilePostFix;
  UINTN          NameLength;
  UINT8*         X509Data;
  UINTN          X509DataSize;
  void*          X509PubKey;
  UINTN          RsaPubKeyModSize;
  UINTN          EcPubKeyXaSize;
  UINTN          EcPubKeyXbSize;

  if (X509FileContext->FileName == NULL) {
    *Error = None_Error;
    return EFI_INVALID_PARAMETER;
  }

  X509Data       = NULL;
  X509DataSize   = 0;
  X509PubKey     = NULL;
  RsaPubKeyModSize = 0;
  EcPubKeyXaSize = 0;
  EcPubKeyXbSize = 0;

  //
  // Parse the file's postfix. Only support DER encoded X.509 certificate files.
  //
  NameLength = StrLen (X509FileContext->FileName);
  if (NameLength <= 4) {
    DEBUG ((DEBUG_ERROR, "Wrong X509 NameLength\n"));
    *Error = Unsupported_Type;
    return EFI_INVALID_PARAMETER;
  }
  FilePostFix = X509FileContext->FileName + NameLength - 4;
  if (!IsDerEncodeCertificate (FilePostFix)) {
    DEBUG ((DEBUG_ERROR, "Unsupported file type, only DER encoded certificate (%s) is supported.\n", mSupportX509Suffix));
    *Error = Unsupported_Type;
    return EFI_INVALID_PARAMETER;
  }
  DEBUG ((DEBUG_INFO, "FileName= %s\n", X509FileContext->FileName));
  DEBUG ((DEBUG_INFO, "FilePostFix = %s\n", FilePostFix));

  //
  // Read the certificate file content
  //
  Status = ReadFileContent (X509FileContext->FHandle, (VOID**) &X509Data, &X509DataSize, 0);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Error occurred while reading the file.\n"));
    goto ON_EXIT;
  }

  //
  // Parse the public key context and check key strength
  //
  if (RsaGetPublicKeyFromX509 (X509Data, X509DataSize, &X509PubKey)) {
    if (X509PubKey != NULL) {
      RsaGetKey (X509PubKey, RsaKeyN, NULL, &RsaPubKeyModSize);
      if (RsaPubKeyModSize < RSA_CER_PUBKEY_MIN_SIZE) {
        DEBUG ((DEBUG_ERROR, "Unqualified RSA PK size, key size should be equal to or greater than 2048 bits.\n"));
        Status = EFI_INVALID_PARAMETER;
        *Error = Unqualified_Key;
      }
      RsaFree (X509PubKey);
    }
  } else if (EcGetPublicKeyFromX509(X509Data, X509DataSize, &X509PubKey)) {
    if (X509PubKey != NULL) {
      EcGetKeyInfo (X509PubKey, NULL, NULL, &EcPubKeyXaSize, &EcPubKeyXbSize);
      if (EcPubKeyXaSize + EcPubKeyXbSize < EC_CER_PUBKEY_MIN_SIZE * 2) {
        DEBUG ((DEBUG_ERROR, "Unqualified EC PK size, key size should be equal to or greater than 2048 bits.\n"));
        Status = EFI_INVALID_PARAMETER;
        *Error = Unqualified_Key;
      }
      EcKeyFree (X509PubKey);
    }
  } else {
    DEBUG ((DEBUG_ERROR, "Error occurred while parsing the pubkey from certificate.\n"));
    Status = EFI_INVALID_PARAMETER;
    *Error = Unsupported_Type;
    goto ON_EXIT;
  }

 ON_EXIT:
  if (X509Data != NULL) {
    FreePool (X509Data);
  }

  return Status;
}

/**
  Generate the PK signature list from the X509 Certificate storing file (.cer)

  @param[in]   X509File              FileHandle of X509 Certificate storing file.
  @param[out]  PkCert                Point to the data buffer to store the signature list.

  @return EFI_UNSUPPORTED            Unsupported Key Length.
  @return EFI_OUT_OF_RESOURCES       There are not enough memory resources to form the signature list.

**/
EFI_STATUS
CreatePkX509SignatureList (
  IN    EFI_FILE_HANDLE             X509File,
  OUT   EFI_SIGNATURE_LIST          **PkCert
  )
{
  EFI_STATUS              Status;
  UINT8                   *X509Data;
  UINTN                   X509DataSize;
  EFI_SIGNATURE_DATA      *PkCertData;

  X509Data = NULL;
  PkCertData = NULL;
  X509DataSize = 0;

  Status = ReadFileContent (X509File, (VOID**) &X509Data, &X509DataSize, 0);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
  ASSERT (X509Data != NULL);

  //
  // Allocate space for PK certificate list and initialize it.
  // Create PK database entry with SignatureHeaderSize equals 0.
  //
  *PkCert = (EFI_SIGNATURE_LIST*) AllocateZeroPool (
              sizeof(EFI_SIGNATURE_LIST) + sizeof(EFI_SIGNATURE_DATA) - 1
              + X509DataSize
              );
  if (*PkCert == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  (*PkCert)->SignatureListSize   = (UINT32) (sizeof(EFI_SIGNATURE_LIST)
                                    + sizeof(EFI_SIGNATURE_DATA) - 1
                                    + X509DataSize);
  (*PkCert)->SignatureSize       = (UINT32) (sizeof(EFI_SIGNATURE_DATA) - 1 + X509DataSize);
  (*PkCert)->SignatureHeaderSize = 0;
  CopyGuid (&(*PkCert)->SignatureType, &gEfiCertX509Guid);
  PkCertData                     = (EFI_SIGNATURE_DATA*) ((UINTN)(*PkCert)
                                                          + sizeof(EFI_SIGNATURE_LIST)
                                                          + (*PkCert)->SignatureHeaderSize);
  CopyGuid (&PkCertData->SignatureOwner, &gEfiGlobalVariableGuid);
  //
  // Fill the PK database with PKpub data from X509 certificate file.
  //
  CopyMem (&(PkCertData->SignatureData[0]), X509Data, X509DataSize);

ON_EXIT:

  if (X509Data != NULL) {
    FreePool (X509Data);
  }

  if (EFI_ERROR(Status) && *PkCert != NULL) {
    FreePool (*PkCert);
    *PkCert = NULL;
  }

  return Status;
}

/**
  Enroll new PK into the System without original PK's authentication.

  The SignatureOwner GUID will be the same with PK's vendorguid.

  @param[in] PrivateData     The module's private data.

  @retval   EFI_SUCCESS            New PK enrolled successfully.
  @retval   EFI_INVALID_PARAMETER  The parameter is invalid.
  @retval   EFI_OUT_OF_RESOURCES   Could not allocate needed resources.

**/
EFI_STATUS
EnrollPlatformKey (
   IN  SECUREBOOT_CONFIG_PRIVATE_DATA*   Private
  )
{
  EFI_STATUS                      Status;
  UINT32                          Attr;
  UINTN                           DataSize;
  EFI_SIGNATURE_LIST              *PkCert;

  PkCert = NULL;

  Status = SetSecureBootMode(CUSTOM_SECURE_BOOT_MODE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Prase the selected PK file and generate PK certificate list.
  //
  Status = CreatePkX509SignatureList (
            Private->FileContext->FHandle,
            &PkCert
            );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
  ASSERT (PkCert != NULL);

  //
  // Set Platform Key variable.
  //
  Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS
          | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
  DataSize = PkCert->SignatureListSize;
  Status = CreateTimeBasedPayload (&DataSize, (UINT8**) &PkCert);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
    goto ON_EXIT;
  }

  Status = gRT->SetVariable(
                  EFI_PLATFORM_KEY_NAME,
                  &gEfiGlobalVariableGuid,
                  Attr,
                  DataSize,
                  PkCert
                  );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_OUT_OF_RESOURCES) {
      DEBUG ((EFI_D_ERROR, "Enroll PK failed with out of resource.\n"));
    }
    goto ON_EXIT;
  }

ON_EXIT:

  if (PkCert != NULL) {
    FreePool(PkCert);
  }

  CloseEnrolledFile(Private->FileContext);

  return Status;
}

/**
  Remove the PK variable.

  @retval EFI_SUCCESS    Delete PK successfully.
  @retval Others         Could not allow to delete PK.

**/
EFI_STATUS
DeletePlatformKey (
  VOID
)
{
  EFI_STATUS Status;

  Status = SetSecureBootMode(CUSTOM_SECURE_BOOT_MODE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = DeleteVariable (
             EFI_PLATFORM_KEY_NAME,
             &gEfiGlobalVariableGuid
             );
  return Status;
}

/**
  Enroll a new KEK item from public key storing file (*.pbk).

  @param[in] PrivateData           The module's private data.

  @retval   EFI_SUCCESS            New KEK enrolled successfully.
  @retval   EFI_INVALID_PARAMETER  The parameter is invalid.
  @retval   EFI_UNSUPPORTED        Unsupported command.
  @retval   EFI_OUT_OF_RESOURCES   Could not allocate needed resources.

**/
EFI_STATUS
EnrollRsa2048ToKek (
  IN SECUREBOOT_CONFIG_PRIVATE_DATA *Private
  )
{
  EFI_STATUS                      Status;
  UINT32                          Attr;
  UINTN                           DataSize;
  EFI_SIGNATURE_LIST              *KekSigList;
  UINTN                           KeyBlobSize;
  UINT8                           *KeyBlob;
  CPL_KEY_INFO                    *KeyInfo;
  EFI_SIGNATURE_DATA              *KEKSigData;
  UINTN                           KekSigListSize;
  UINT8                           *KeyBuffer;
  UINTN                           KeyLenInBytes;

  Attr        = 0;
  DataSize    = 0;
  KeyBuffer   = NULL;
  KeyBlobSize = 0;
  KeyBlob     = NULL;
  KeyInfo     = NULL;
  KEKSigData  = NULL;
  KekSigList  = NULL;
  KekSigListSize = 0;

  //
  // Form the KeKpub certificate list into EFI_SIGNATURE_LIST type.
  // First, We have to parse out public key data from the pbk key file.
  //
  Status = ReadFileContent (
             Private->FileContext->FHandle,
             (VOID**) &KeyBlob,
             &KeyBlobSize,
             0
             );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
  ASSERT (KeyBlob != NULL);
  KeyInfo = (CPL_KEY_INFO *) KeyBlob;
  if (KeyInfo->KeyLengthInBits / 8 != WIN_CERT_UEFI_RSA2048_SIZE) {
    DEBUG ((DEBUG_ERROR, "Unsupported key length, Only RSA2048 is supported.\n"));
    Status = EFI_UNSUPPORTED;
    goto ON_EXIT;
  }

  //
  // Convert the Public key to fix octet string format represented in RSA PKCS#1.
  //
  KeyLenInBytes = KeyInfo->KeyLengthInBits / 8;
  KeyBuffer = AllocateZeroPool (KeyLenInBytes);
  if (KeyBuffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }
  Int2OctStr (
    (UINTN*) (KeyBlob + sizeof (CPL_KEY_INFO)),
    KeyLenInBytes / sizeof (UINTN),
    KeyBuffer,
    KeyLenInBytes
    );
  CopyMem(KeyBlob + sizeof(CPL_KEY_INFO), KeyBuffer, KeyLenInBytes);

  //
  // Form an new EFI_SIGNATURE_LIST.
  //
  KekSigListSize = sizeof(EFI_SIGNATURE_LIST)
                     + sizeof(EFI_SIGNATURE_DATA) - 1
                     + WIN_CERT_UEFI_RSA2048_SIZE;

  KekSigList = (EFI_SIGNATURE_LIST*) AllocateZeroPool (KekSigListSize);
  if (KekSigList == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  KekSigList->SignatureListSize   = sizeof(EFI_SIGNATURE_LIST)
                                  + sizeof(EFI_SIGNATURE_DATA) - 1
                                  + WIN_CERT_UEFI_RSA2048_SIZE;
  KekSigList->SignatureHeaderSize = 0;
  KekSigList->SignatureSize = sizeof(EFI_SIGNATURE_DATA) - 1 + WIN_CERT_UEFI_RSA2048_SIZE;
  CopyGuid (&KekSigList->SignatureType, &gEfiCertRsa2048Guid);

  KEKSigData = (EFI_SIGNATURE_DATA*)((UINT8*)KekSigList + sizeof(EFI_SIGNATURE_LIST));
  CopyGuid (&KEKSigData->SignatureOwner, Private->SignatureGUID);
  CopyMem (
    KEKSigData->SignatureData,
    KeyBlob + sizeof(CPL_KEY_INFO),
    WIN_CERT_UEFI_RSA2048_SIZE
    );

  //
  // Check if KEK entry has been already existed.
  // If true, use EFI_VARIABLE_APPEND_WRITE attribute to append the
  // new KEK to original variable.
  //
  Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS
         | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
  Status = CreateTimeBasedPayload (&KekSigListSize, (UINT8**) &KekSigList);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
    goto ON_EXIT;
  }

  Status = gRT->GetVariable(
                  EFI_KEY_EXCHANGE_KEY_NAME,
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &DataSize,
                  NULL
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Attr |= EFI_VARIABLE_APPEND_WRITE;
  } else if (Status != EFI_NOT_FOUND) {
    goto ON_EXIT;
  }

  //
  // Done. Now we have formed the correct KEKpub database item, just set it into variable storage,
  //
  Status = gRT->SetVariable(
                  EFI_KEY_EXCHANGE_KEY_NAME,
                  &gEfiGlobalVariableGuid,
                  Attr,
                  KekSigListSize,
                  KekSigList
                  );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

ON_EXIT:

  CloseEnrolledFile(Private->FileContext);

  if (Private->SignatureGUID != NULL) {
    FreePool (Private->SignatureGUID);
    Private->SignatureGUID = NULL;
  }

  if (KeyBlob != NULL) {
    FreePool (KeyBlob);
  }
  if (KeyBuffer != NULL) {
    FreePool (KeyBuffer);
  }
  if (KekSigList != NULL) {
    FreePool (KekSigList);
  }

  return Status;
}

/**
  Enroll a new KEK item from X509 certificate file.

  @param[in] PrivateData           The module's private data.

  @retval   EFI_SUCCESS            New X509 is enrolled successfully.
  @retval   EFI_INVALID_PARAMETER  The parameter is invalid.
  @retval   EFI_UNSUPPORTED        Unsupported command.
  @retval   EFI_OUT_OF_RESOURCES   Could not allocate needed resources.

**/
EFI_STATUS
EnrollX509ToKek (
  IN SECUREBOOT_CONFIG_PRIVATE_DATA *Private
  )
{
  EFI_STATUS                        Status;
  UINTN                             X509DataSize;
  VOID                              *X509Data;
  EFI_SIGNATURE_DATA                *KEKSigData;
  EFI_SIGNATURE_LIST                *KekSigList;
  UINTN                             DataSize;
  UINTN                             KekSigListSize;
  UINT32                            Attr;

  X509Data       = NULL;
  X509DataSize   = 0;
  KekSigList     = NULL;
  KekSigListSize = 0;
  DataSize       = 0;
  KEKSigData     = NULL;

  Status = ReadFileContent (
             Private->FileContext->FHandle,
             &X509Data,
             &X509DataSize,
             0
             );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
  ASSERT (X509Data != NULL);

  KekSigListSize = sizeof(EFI_SIGNATURE_LIST) + sizeof(EFI_SIGNATURE_DATA) - 1 + X509DataSize;
  KekSigList = (EFI_SIGNATURE_LIST*) AllocateZeroPool (KekSigListSize);
  if (KekSigList == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  //
  // Fill Certificate Database parameters.
  //
  KekSigList->SignatureListSize   = (UINT32) KekSigListSize;
  KekSigList->SignatureHeaderSize = 0;
  KekSigList->SignatureSize = (UINT32) (sizeof(EFI_SIGNATURE_DATA) - 1 + X509DataSize);
  CopyGuid (&KekSigList->SignatureType, &gEfiCertX509Guid);

  KEKSigData = (EFI_SIGNATURE_DATA*) ((UINT8*) KekSigList + sizeof (EFI_SIGNATURE_LIST));
  CopyGuid (&KEKSigData->SignatureOwner, Private->SignatureGUID);
  CopyMem (KEKSigData->SignatureData, X509Data, X509DataSize);

  //
  // Check if KEK been already existed.
  // If true, use EFI_VARIABLE_APPEND_WRITE attribute to append the
  // new kek to original variable
  //
  Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS
          | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
  Status = CreateTimeBasedPayload (&KekSigListSize, (UINT8**) &KekSigList);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
    goto ON_EXIT;
  }

  Status = gRT->GetVariable(
                  EFI_KEY_EXCHANGE_KEY_NAME,
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &DataSize,
                  NULL
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Attr |= EFI_VARIABLE_APPEND_WRITE;
  } else if (Status != EFI_NOT_FOUND) {
    goto ON_EXIT;
  }

  Status = gRT->SetVariable(
                  EFI_KEY_EXCHANGE_KEY_NAME,
                  &gEfiGlobalVariableGuid,
                  Attr,
                  KekSigListSize,
                  KekSigList
                  );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

ON_EXIT:

  CloseEnrolledFile(Private->FileContext);

  if (Private->SignatureGUID != NULL) {
    FreePool (Private->SignatureGUID);
    Private->SignatureGUID = NULL;
  }

  if (KekSigList != NULL) {
    FreePool (KekSigList);
  }

  return Status;
}

/**
  Enroll new KEK into the System without PK's authentication.
  The SignatureOwner GUID will be Private->SignatureGUID.

  @param[in]  PrivateData     The module's private data.
  @param[out] Error           Error type checked in the certificate.

  @retval   EFI_SUCCESS            New KEK enrolled successful.
  @retval   EFI_INVALID_PARAMETER  The parameter is invalid.
  @retval   others                 Fail to enroll KEK data.

**/
EFI_STATUS
EnrollKeyExchangeKey (
  IN SECUREBOOT_CONFIG_PRIVATE_DATA *Private,
  OUT  ENROLL_KEY_ERROR             *Error
  )
{
  UINT16*     FilePostFix;
  EFI_STATUS  Status;
  UINTN       NameLength;
  
  *Error = Unsupported_Type;

  if ((Private->FileContext->FHandle == NULL) || (Private->FileContext->FileName == NULL) || (Private->SignatureGUID == NULL)) {
    *Error = None_Error;
    return EFI_INVALID_PARAMETER;
  }

  Status = SetSecureBootMode(CUSTOM_SECURE_BOOT_MODE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Parse the file's postfix. Supports DER-encoded X509 certificate,
  // and .pbk as RSA public key file.
  //
  NameLength = StrLen (Private->FileContext->FileName);
  if (NameLength <= 4) {
    return EFI_INVALID_PARAMETER;
  }
  FilePostFix = Private->FileContext->FileName + NameLength - 4;
  if (IsDerEncodeCertificate(FilePostFix)) {
    return EnrollX509ToKek (Private);
  } else if (CompareMem (FilePostFix, L".pbk",4) == 0) {
    return EnrollRsa2048ToKek (Private);
  } else {
    //
    // File type is wrong, simply close it
    //
    CloseEnrolledFile(Private->FileContext);

    return EFI_INVALID_PARAMETER;
  }
}

/**
  Enroll a new X509 certificate into Signature Database (DB or DBX or DBT) without
  KEK's authentication.

  @param[in] PrivateData     The module's private data.
  @param[in] VariableName    Variable name of signature database, must be
                             EFI_IMAGE_SECURITY_DATABASE or EFI_IMAGE_SECURITY_DATABASE1.

  @retval   EFI_SUCCESS            New X509 is enrolled successfully.
  @retval   EFI_OUT_OF_RESOURCES   Could not allocate needed resources.

**/
EFI_STATUS
EnrollX509toSigDB (
  IN SECUREBOOT_CONFIG_PRIVATE_DATA *Private,
  IN CHAR16                         *VariableName
  )
{
  EFI_STATUS                        Status;
  UINTN                             X509DataSize;
  VOID                              *X509Data;
  EFI_SIGNATURE_LIST                *SigDBCert;
  EFI_SIGNATURE_DATA                *SigDBCertData;
  VOID                              *Data;
  UINTN                             DataSize;
  UINTN                             SigDBSize;
  UINT32                            Attr;

  X509DataSize  = 0;
  SigDBSize     = 0;
  DataSize      = 0;
  X509Data      = NULL;
  SigDBCert     = NULL;
  SigDBCertData = NULL;
  Data          = NULL;

  Status = ReadFileContent (
             Private->FileContext->FHandle,
             &X509Data,
             &X509DataSize,
             0
             );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
  ASSERT (X509Data != NULL);

  SigDBSize = sizeof(EFI_SIGNATURE_LIST) + sizeof(EFI_SIGNATURE_DATA) - 1 + X509DataSize;

  Data = AllocateZeroPool (SigDBSize);
  if (Data == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  //
  // Fill Certificate Database parameters.
  //
  SigDBCert = (EFI_SIGNATURE_LIST*) Data;
  SigDBCert->SignatureListSize   = (UINT32) SigDBSize;
  SigDBCert->SignatureHeaderSize = 0;
  SigDBCert->SignatureSize = (UINT32) (sizeof(EFI_SIGNATURE_DATA) - 1 + X509DataSize);
  CopyGuid (&SigDBCert->SignatureType, &gEfiCertX509Guid);

  SigDBCertData = (EFI_SIGNATURE_DATA*) ((UINT8* ) SigDBCert + sizeof (EFI_SIGNATURE_LIST));
  CopyGuid (&SigDBCertData->SignatureOwner, Private->SignatureGUID);
  CopyMem ((UINT8* ) (SigDBCertData->SignatureData), X509Data, X509DataSize);

  //
  // Check if signature database entry has been already existed.
  // If true, use EFI_VARIABLE_APPEND_WRITE attribute to append the
  // new signature data to original variable
  //
  Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS
          | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
  Status = CreateTimeBasedPayload (&SigDBSize, (UINT8**) &Data);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
    goto ON_EXIT;
  }

  Status = gRT->GetVariable(
                  VariableName,
                  &gEfiImageSecurityDatabaseGuid,
                  NULL,
                  &DataSize,
                  NULL
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Attr |= EFI_VARIABLE_APPEND_WRITE;
  } else if (Status != EFI_NOT_FOUND) {
    goto ON_EXIT;
  }

  Status = gRT->SetVariable(
                  VariableName,
                  &gEfiImageSecurityDatabaseGuid,
                  Attr,
                  SigDBSize,
                  Data
                  );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

ON_EXIT:

  CloseEnrolledFile(Private->FileContext);

  if (Private->SignatureGUID != NULL) {
    FreePool (Private->SignatureGUID);
    Private->SignatureGUID = NULL;
  }

  if (Data != NULL) {
    FreePool (Data);
  }

  if (X509Data != NULL) {
    FreePool (X509Data);
  }

  return Status;
}

/**
  Check whether signature is in specified database.

  @param[in]  VariableName        Name of database variable that is searched in.
  @param[in]  Signature           Pointer to signature that is searched for.
  @param[in]  SignatureSize       Size of Signature.

  @return TRUE                    Found the signature in the variable database.
  @return FALSE                   Not found the signature in the variable database.

**/
BOOLEAN
IsSignatureFoundInDatabase (
  IN CHAR16             *VariableName,
  IN UINT8              *Signature,
  IN UINTN              SignatureSize
  )
{
  EFI_STATUS          Status;
  EFI_SIGNATURE_LIST  *CertList;
  EFI_SIGNATURE_DATA  *Cert;
  UINTN               DataSize;
  UINT8               *Data;
  UINTN               Index;
  UINTN               CertCount;
  BOOLEAN             IsFound;

  //
  // Read signature database variable.
  //
  IsFound   = FALSE;
  Data      = NULL;
  DataSize  = 0;
  Status    = gRT->GetVariable (VariableName, &gEfiImageSecurityDatabaseGuid, NULL, &DataSize, NULL);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return FALSE;
  }

  Data = (UINT8 *) AllocateZeroPool (DataSize);
  if (Data == NULL) {
    return FALSE;
  }

  Status = gRT->GetVariable (VariableName, &gEfiImageSecurityDatabaseGuid, NULL, &DataSize, Data);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Enumerate all signature data in SigDB to check if signature exists for executable.
  //
  CertList = (EFI_SIGNATURE_LIST *) Data;
  while ((DataSize > 0) && (DataSize >= CertList->SignatureListSize)) {
    CertCount = (CertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - CertList->SignatureHeaderSize) / CertList->SignatureSize;
    Cert      = (EFI_SIGNATURE_DATA *) ((UINT8 *) CertList + sizeof (EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize);
    if ((CertList->SignatureSize == sizeof(EFI_SIGNATURE_DATA) - 1 + SignatureSize) && (CompareGuid(&CertList->SignatureType, &gEfiCertX509Guid))) {
      for (Index = 0; Index < CertCount; Index++) {
        if (CompareMem (Cert->SignatureData, Signature, SignatureSize) == 0) {
          //
          // Find the signature in database.
          //
          IsFound = TRUE;
          break;
        }
        Cert = (EFI_SIGNATURE_DATA *) ((UINT8 *) Cert + CertList->SignatureSize);
      }

      if (IsFound) {
        break;
      }
    }

    DataSize -= CertList->SignatureListSize;
    CertList  = (EFI_SIGNATURE_LIST *) ((UINT8 *) CertList + CertList->SignatureListSize);
  }

Done:
  if (Data != NULL) {
    FreePool (Data);
  }

  return IsFound;
}

/**
  Calculate the hash of a certificate data with the specified hash algorithm.

  @param[in]    CertData  The certificate data to be hashed.
  @param[in]    CertSize  The certificate size in bytes.
  @param[in]    HashAlg   The specified hash algorithm.
  @param[out]   CertHash  The output digest of the certificate

  @retval TRUE            Successfully got the hash of the CertData.
  @retval FALSE           Failed to get the hash of CertData.

**/
BOOLEAN
CalculateCertHash (
  IN  UINT8                 *CertData,
  IN  UINTN                 CertSize,
  IN  UINT32                HashAlg,
  OUT UINT8                 *CertHash
  )
{
  BOOLEAN                   Status;
  VOID                      *HashCtx;
  UINTN                     CtxSize;
  UINT8                     *TBSCert;
  UINTN                     TBSCertSize;

  HashCtx = NULL;
  Status  = FALSE;

  if (HashAlg >= HASHALG_MAX) {
    return FALSE;
  }

  //
  // Retrieve the TBSCertificate for Hash Calculation.
  //
  if (!X509GetTBSCert (CertData, CertSize, &TBSCert, &TBSCertSize)) {
    return FALSE;
  }

  //
  // 1. Initialize context of hash.
  //
  CtxSize = mHash[HashAlg].GetContextSize ();
  HashCtx = AllocatePool (CtxSize);
  ASSERT (HashCtx != NULL);

  //
  // 2. Initialize a hash context.
  //
  Status = mHash[HashAlg].HashInit (HashCtx);
  if (!Status) {
    goto Done;
  }

  //
  // 3. Calculate the hash.
  //
  Status  = mHash[HashAlg].HashUpdate (HashCtx, TBSCert, TBSCertSize);
  if (!Status) {
    goto Done;
  }

  //
  // 4. Get the hash result.
  //
  ZeroMem (CertHash, mHash[HashAlg].DigestLength);
  Status  = mHash[HashAlg].HashFinal (HashCtx, CertHash);

Done:
  if (HashCtx != NULL) {
    FreePool (HashCtx);
  }

  return Status;
}

/**
  Check whether the hash of an X.509 certificate is in forbidden database (DBX).

  @param[in]  Certificate       Pointer to X.509 Certificate that is searched for.
  @param[in]  CertSize          Size of X.509 Certificate.

  @return TRUE               Found the certificate hash in the forbidden database.
  @return FALSE              Certificate hash is Not found in the forbidden database.

**/
BOOLEAN
IsCertHashFoundInDbx (
  IN  UINT8               *Certificate,
  IN  UINTN               CertSize
  )
{
  BOOLEAN                 IsFound;
  EFI_STATUS              Status;
  EFI_SIGNATURE_LIST      *DbxList;
  EFI_SIGNATURE_DATA      *CertHash;
  UINTN                   CertHashCount;
  UINTN                   Index;
  UINT32                  HashAlg;
  UINT8                   CertDigest[MAX_DIGEST_SIZE];
  UINT8                   *DbxCertHash;
  UINTN                   SiglistHeaderSize;
  UINT8                   *Data;
  UINTN                   DataSize;

  IsFound  = FALSE;
  HashAlg  = HASHALG_MAX;
  Data     = NULL;

  //
  // Read signature database variable.
  //
  DataSize  = 0;
  Status    = gRT->GetVariable (EFI_IMAGE_SECURITY_DATABASE1, &gEfiImageSecurityDatabaseGuid, NULL, &DataSize, NULL);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return FALSE;
  }

  Data = (UINT8 *) AllocateZeroPool (DataSize);
  if (Data == NULL) {
    return FALSE;
  }

  Status = gRT->GetVariable (EFI_IMAGE_SECURITY_DATABASE1, &gEfiImageSecurityDatabaseGuid, NULL, &DataSize, Data);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Check whether the certificate hash exists in the forbidden database.
  //
  DbxList = (EFI_SIGNATURE_LIST *) Data;
  while ((DataSize > 0) && (DataSize >= DbxList->SignatureListSize)) {
    //
    // Determine Hash Algorithm of Certificate in the forbidden database.
    //
    if (CompareGuid (&DbxList->SignatureType, &gEfiCertX509Sha256Guid)) {
      HashAlg = HASHALG_SHA256;
    } else if (CompareGuid (&DbxList->SignatureType, &gEfiCertX509Sha384Guid)) {
      HashAlg = HASHALG_SHA384;
    } else if (CompareGuid (&DbxList->SignatureType, &gEfiCertX509Sha512Guid)) {
      HashAlg = HASHALG_SHA512;
    } else {
      DataSize -= DbxList->SignatureListSize;
      DbxList   = (EFI_SIGNATURE_LIST *) ((UINT8 *) DbxList + DbxList->SignatureListSize);
      continue;
    }

    //
    // Calculate the hash value of current db certificate for comparision.
    //
    if (!CalculateCertHash (Certificate, CertSize, HashAlg, CertDigest)) {
      goto Done;
    }

    SiglistHeaderSize = sizeof (EFI_SIGNATURE_LIST) + DbxList->SignatureHeaderSize;
    CertHash          = (EFI_SIGNATURE_DATA *) ((UINT8 *) DbxList + SiglistHeaderSize);
    CertHashCount     = (DbxList->SignatureListSize - SiglistHeaderSize) / DbxList->SignatureSize;
    for (Index = 0; Index < CertHashCount; Index++) {
      //
      // Iterate each Signature Data Node within this CertList for verify.
      //
      DbxCertHash = CertHash->SignatureData;
      if (CompareMem (DbxCertHash, CertDigest, mHash[HashAlg].DigestLength) == 0) {
        //
        // Hash of Certificate is found in forbidden database.
        //
        IsFound = TRUE;
        goto Done;
      }
      CertHash = (EFI_SIGNATURE_DATA *) ((UINT8 *) CertHash + DbxList->SignatureSize);
    }

    DataSize -= DbxList->SignatureListSize;
    DbxList   = (EFI_SIGNATURE_LIST *) ((UINT8 *) DbxList + DbxList->SignatureListSize);
  }

Done:
  if (Data != NULL) {
    FreePool (Data);
  }

  return IsFound;
}

/**
  Check whether the signature list exists in given variable data.

  It searches the signature list for the certificate hash by CertType.
  If the signature list is found, get the offset of Database for the
  next hash of a certificate.

  @param[in]  Database      Variable data to save signature list.
  @param[in]  DatabaseSize  Variable size.
  @param[in]  SignatureType The type of the signature.
  @param[out] Offset        The offset to save a new hash of certificate.

  @return TRUE       The signature list is found in the forbidden database.
  @return FALSE      The signature list is not found in the forbidden database.
**/
BOOLEAN
GetSignaturelistOffset (
  IN  EFI_SIGNATURE_LIST  *Database,
  IN  UINTN               DatabaseSize,
  IN  EFI_GUID            *SignatureType,
  OUT UINTN               *Offset
  )
{
  EFI_SIGNATURE_LIST      *SigList;
  UINTN                   SiglistSize;

  if ((Database == NULL) || (DatabaseSize == 0)) {
    *Offset = 0;
    return FALSE;
  }

  SigList     = Database;
  SiglistSize = DatabaseSize;
  while ((SiglistSize > 0) && (SiglistSize >= SigList->SignatureListSize)) {
    if (CompareGuid (&SigList->SignatureType, SignatureType)) {
      *Offset = DatabaseSize - SiglistSize;
      return TRUE;
    }
    SiglistSize -= SigList->SignatureListSize;
    SigList      = (EFI_SIGNATURE_LIST *) ((UINT8 *) SigList + SigList->SignatureListSize);
  }
  *Offset = 0;
  return FALSE;
}

/**
  Enroll a new X509 certificate hash into Signature Database (dbx) without
  KEK's authentication.

  @param[in] PrivateData      The module's private data.
  @param[in] HashAlg          The hash algorithm to enroll the certificate.
  @param[in] RevocationDate   The revocation date of the certificate.
  @param[in] RevocationTime   The revocation time of the certificate.
  @param[in] AlwaysRevocation Indicate whether the certificate is always revoked.

  @retval   EFI_SUCCESS            New X509 is enrolled successfully.
  @retval   EFI_INVALID_PARAMETER  The parameter is invalid.
  @retval   EFI_OUT_OF_RESOURCES   Could not allocate needed resources.

**/
EFI_STATUS
EnrollX509HashtoSigDB (
  IN SECUREBOOT_CONFIG_PRIVATE_DATA *Private,
  IN UINT32                         HashAlg,
  IN EFI_HII_DATE                   *RevocationDate,
  IN EFI_HII_TIME                   *RevocationTime,
  IN BOOLEAN                        AlwaysRevocation
  )
{
  EFI_STATUS          Status;
  UINTN               X509DataSize;
  VOID                *X509Data;
  EFI_SIGNATURE_LIST  *SignatureList;
  UINTN               SignatureListSize;
  UINT8               *Data;
  UINT8               *NewData;
  UINTN               DataSize;
  UINTN               DbSize;
  UINT32              Attr;
  EFI_SIGNATURE_DATA  *SignatureData;
  UINTN               SignatureSize;
  EFI_GUID            SignatureType;
  UINTN               Offset;
  UINT8               CertHash[MAX_DIGEST_SIZE];
  UINT16*             FilePostFix;
  UINTN               NameLength;
  EFI_TIME            *Time;

  X509DataSize  = 0;
  DbSize        = 0;
  X509Data      = NULL;
  SignatureData = NULL;
  SignatureList = NULL;
  Data          = NULL;
  NewData       = NULL;

  if ((Private->FileContext->FileName == NULL) || (Private->FileContext->FHandle == NULL) || (Private->SignatureGUID == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SetSecureBootMode (CUSTOM_SECURE_BOOT_MODE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Parse the file's postfix.
  //
  NameLength = StrLen (Private->FileContext->FileName);
  if (NameLength <= 4) {
    return EFI_INVALID_PARAMETER;
  }
  FilePostFix = Private->FileContext->FileName + NameLength - 4;
  if (!IsDerEncodeCertificate(FilePostFix)) {
    //
    // Only supports DER-encoded X509 certificate.
    //
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get the certificate from file and calculate its hash.
  //
  Status = ReadFileContent (
             Private->FileContext->FHandle,
             &X509Data,
             &X509DataSize,
             0
             );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
  ASSERT (X509Data != NULL);

  if (!CalculateCertHash (X509Data, X509DataSize, HashAlg, CertHash)) {
    goto ON_EXIT;
  }

  //
  // Get the variable for enrollment.
  //
  DataSize = 0;
  Status   = gRT->GetVariable (EFI_IMAGE_SECURITY_DATABASE1, &gEfiImageSecurityDatabaseGuid, NULL, &DataSize, NULL);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Data = (UINT8 *) AllocateZeroPool (DataSize);
    if (Data == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Status = gRT->GetVariable (EFI_IMAGE_SECURITY_DATABASE1, &gEfiImageSecurityDatabaseGuid, NULL, &DataSize, Data);
    if (EFI_ERROR (Status)) {
      goto ON_EXIT;
    }
  }

  //
  // Allocate memory for Signature and fill the Signature
  //
  SignatureSize = sizeof(EFI_SIGNATURE_DATA) - 1 + sizeof (EFI_TIME) + mHash[HashAlg].DigestLength;
  SignatureData = (EFI_SIGNATURE_DATA *) AllocateZeroPool (SignatureSize);
  if (SignatureData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyGuid (&SignatureData->SignatureOwner, Private->SignatureGUID);
  CopyMem (SignatureData->SignatureData, CertHash, mHash[HashAlg].DigestLength);

  //
  // Fill the time.
  //
  if (!AlwaysRevocation) {
    Time = (EFI_TIME *)(&SignatureData->SignatureData + mHash[HashAlg].DigestLength);
    Time->Year   = RevocationDate->Year;
    Time->Month  = RevocationDate->Month;
    Time->Day    = RevocationDate->Day;
    Time->Hour   = RevocationTime->Hour;
    Time->Minute = RevocationTime->Minute;
    Time->Second = RevocationTime->Second;
  }

  //
  // Determine the GUID for certificate hash.
  //
  switch (HashAlg) {
  case HASHALG_SHA256:
    SignatureType = gEfiCertX509Sha256Guid;
    break;
  case HASHALG_SHA384:
    SignatureType = gEfiCertX509Sha384Guid;
    break;
  case HASHALG_SHA512:
    SignatureType = gEfiCertX509Sha512Guid;
    break;
  default:
    return FALSE;
  }

  //
  // Add signature into the new variable data buffer
  //
  if (GetSignaturelistOffset((EFI_SIGNATURE_LIST *)Data, DataSize, &SignatureType, &Offset)) {
    //
    // Add the signature to the found signaturelist.
    //
    DbSize  = DataSize + SignatureSize;
    NewData = AllocateZeroPool (DbSize);
    if (NewData == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ON_EXIT;
    }

    SignatureList     = (EFI_SIGNATURE_LIST *)(Data + Offset);
    SignatureListSize = (UINTN) ReadUnaligned32 ((UINT32 *)&SignatureList->SignatureListSize);
    CopyMem (NewData, Data, Offset + SignatureListSize);

    SignatureList = (EFI_SIGNATURE_LIST *)(NewData + Offset);
    WriteUnaligned32 ((UINT32 *) &SignatureList->SignatureListSize, (UINT32)(SignatureListSize + SignatureSize));

    Offset += SignatureListSize;
    CopyMem (NewData + Offset, SignatureData, SignatureSize);
    CopyMem (NewData + Offset + SignatureSize, Data + Offset, DataSize - Offset);

    FreePool (Data);
    Data     = NewData;
    DataSize = DbSize;
  } else {
    //
    // Create a new signaturelist, and add the signature into the signaturelist.
    //
    DbSize  = DataSize + sizeof(EFI_SIGNATURE_LIST) + SignatureSize;
    NewData = AllocateZeroPool (DbSize);
    if (NewData == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ON_EXIT;
    }
    //
    // Fill Certificate Database parameters.
    //
    SignatureList     = (EFI_SIGNATURE_LIST*) (NewData + DataSize);
    SignatureListSize = sizeof(EFI_SIGNATURE_LIST) + SignatureSize;
    WriteUnaligned32 ((UINT32 *) &SignatureList->SignatureListSize, (UINT32) SignatureListSize);
    WriteUnaligned32 ((UINT32 *) &SignatureList->SignatureSize, (UINT32) SignatureSize);
    CopyGuid (&SignatureList->SignatureType, &SignatureType);
    CopyMem ((UINT8* ) SignatureList + sizeof (EFI_SIGNATURE_LIST), SignatureData, SignatureSize);
    if ((DataSize != 0) && (Data != NULL)) {
      CopyMem (NewData, Data, DataSize);
      FreePool (Data);
    }
    Data     = NewData;
    DataSize = DbSize;
  }

  Status = CreateTimeBasedPayload (&DataSize, (UINT8**) &Data);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS
          | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
  Status = gRT->SetVariable(
                  EFI_IMAGE_SECURITY_DATABASE1,
                  &gEfiImageSecurityDatabaseGuid,
                  Attr,
                  DataSize,
                  Data
                  );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

ON_EXIT:

  CloseEnrolledFile(Private->FileContext);

  if (Private->SignatureGUID != NULL) {
    FreePool (Private->SignatureGUID);
    Private->SignatureGUID = NULL;
  }

  if (Data != NULL) {
    FreePool (Data);
  }

  if (SignatureData != NULL) {
    FreePool (SignatureData);
  }

  if (X509Data != NULL) {
    FreePool (X509Data);
  }

  return Status;
}

/**
  Check whether a certificate from a file exists in dbx.

  @param[in] PrivateData     The module's private data.
  @param[in] VariableName    Variable name of signature database, must be
                             EFI_IMAGE_SECURITY_DATABASE1.

  @retval   TRUE             The X509 certificate is found in dbx successfully.
  @retval   FALSE            The X509 certificate is not found in dbx.
**/
BOOLEAN
IsX509CertInDbx (
  IN SECUREBOOT_CONFIG_PRIVATE_DATA *Private,
  IN CHAR16                         *VariableName
  )
{
  EFI_STATUS          Status;
  UINTN               X509DataSize;
  VOID                *X509Data;
  BOOLEAN             IsFound;

  //
  //  Read the certificate from file
  //
  X509DataSize  = 0;
  X509Data      = NULL;
  Status = ReadFileContent (
             Private->FileContext->FHandle,
             &X509Data,
             &X509DataSize,
             0
             );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  //
  // Check the raw certificate.
  //
  IsFound = FALSE;
  if (IsSignatureFoundInDatabase (EFI_IMAGE_SECURITY_DATABASE1, X509Data, X509DataSize)) {
    IsFound = TRUE;
    goto ON_EXIT;
  }

  //
  // Check the hash of certificate.
  //
  if (IsCertHashFoundInDbx (X509Data, X509DataSize)) {
    IsFound = TRUE;
    goto ON_EXIT;
  }

ON_EXIT:
  if (X509Data != NULL) {
    FreePool (X509Data);
  }

  return IsFound;
}

/**
  Reads contents of a PE/COFF image in memory buffer.

  Caution: This function may receive untrusted input.
  PE/COFF image is external input, so this function will make sure the PE/COFF image content
  read is within the image buffer.

  @param  FileHandle      Pointer to the file handle to read the PE/COFF image.
  @param  FileOffset      Offset into the PE/COFF image to begin the read operation.
  @param  ReadSize        On input, the size in bytes of the requested read operation.
                          On output, the number of bytes actually read.
  @param  Buffer          Output buffer that contains the data read from the PE/COFF image.

  @retval EFI_SUCCESS     The specified portion of the PE/COFF image was read and the size
**/
EFI_STATUS
EFIAPI
SecureBootConfigImageRead (
  IN     VOID    *FileHandle,
  IN     UINTN   FileOffset,
  IN OUT UINTN   *ReadSize,
  OUT    VOID    *Buffer
  )
{
  UINTN               EndPosition;

  if (FileHandle == NULL || ReadSize == NULL || Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (MAX_ADDRESS - FileOffset < *ReadSize) {
    return EFI_INVALID_PARAMETER;
  }

  EndPosition = FileOffset + *ReadSize;
  if (EndPosition > mImageSize) {
    *ReadSize = (UINT32)(mImageSize - FileOffset);
  }

  if (FileOffset >= mImageSize) {
    *ReadSize = 0;
  }

  CopyMem (Buffer, (UINT8 *)((UINTN) FileHandle + FileOffset), *ReadSize);

  return EFI_SUCCESS;
}

/**
  Load PE/COFF image information into internal buffer and check its validity.

  @retval   EFI_SUCCESS         Successful
  @retval   EFI_UNSUPPORTED     Invalid PE/COFF file
  @retval   EFI_ABORTED         Serious error occurs, like file I/O error etc.

**/
EFI_STATUS
LoadPeImage (
  VOID
  )
{
  EFI_IMAGE_DOS_HEADER                  *DosHdr;
  EFI_IMAGE_NT_HEADERS32                *NtHeader32;
  EFI_IMAGE_NT_HEADERS64                *NtHeader64;
  PE_COFF_LOADER_IMAGE_CONTEXT          ImageContext;
  EFI_STATUS                            Status;

  NtHeader32 = NULL;
  NtHeader64 = NULL;

  ZeroMem (&ImageContext, sizeof (ImageContext));
  ImageContext.Handle    = (VOID *) mImageBase;
  ImageContext.ImageRead = (PE_COFF_LOADER_READ_FILE) SecureBootConfigImageRead;

  //
  // Get information about the image being loaded
  //
  Status = PeCoffLoaderGetImageInfo (&ImageContext);
  if (EFI_ERROR (Status)) {
    //
    // The information can't be got from the invalid PeImage
    //
    DEBUG ((DEBUG_INFO, "SecureBootConfigDxe: PeImage invalid. \n"));
    return Status;
  }

  //
  // Read the Dos header
  //
  DosHdr = (EFI_IMAGE_DOS_HEADER*)(mImageBase);
  if (DosHdr->e_magic == EFI_IMAGE_DOS_SIGNATURE)
  {
    //
    // DOS image header is present,
    // So read the PE header after the DOS image header
    //
    mPeCoffHeaderOffset = DosHdr->e_lfanew;
  }
  else
  {
    mPeCoffHeaderOffset = 0;
  }

  //
  // Read PE header and check the signature validity and machine compatibility
  //
  NtHeader32 = (EFI_IMAGE_NT_HEADERS32*) (mImageBase + mPeCoffHeaderOffset);
  if (NtHeader32->Signature != EFI_IMAGE_NT_SIGNATURE)
  {
    return EFI_UNSUPPORTED;
  }

  mNtHeader.Pe32 = NtHeader32;

  //
  // Check the architecture field of PE header and get the Certificate Data Directory data
  // Note the size of FileHeader field is constant for both IA32 and X64 arch
  //
  if ((NtHeader32->FileHeader.Machine == EFI_IMAGE_MACHINE_IA32)
      || (NtHeader32->FileHeader.Machine == EFI_IMAGE_MACHINE_EBC)
      || (NtHeader32->FileHeader.Machine == EFI_IMAGE_MACHINE_ARMTHUMB_MIXED)) {
    //
    // 32-bits Architecture
    //
    mImageType = ImageType_IA32;
    mSecDataDir = (EFI_IMAGE_SECURITY_DATA_DIRECTORY*) &(NtHeader32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY]);
  }
  else if ((NtHeader32->FileHeader.Machine == EFI_IMAGE_MACHINE_IA64)
          || (NtHeader32->FileHeader.Machine == EFI_IMAGE_MACHINE_X64)
          || (NtHeader32->FileHeader.Machine == EFI_IMAGE_MACHINE_AARCH64)) {
    //
    // 64-bits Architecture
    //
    mImageType = ImageType_X64;
    NtHeader64 = (EFI_IMAGE_NT_HEADERS64 *) (mImageBase + mPeCoffHeaderOffset);
    mSecDataDir = (EFI_IMAGE_SECURITY_DATA_DIRECTORY*) &(NtHeader64->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY]);
  } else {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
  Calculate hash of Pe/Coff image based on the authenticode image hashing in
  PE/COFF Specification 8.0 Appendix A

  Notes: PE/COFF image has been checked by BasePeCoffLib PeCoffLoaderGetImageInfo() in
  the function LoadPeImage ().

  @param[in]    HashAlg   Hash algorithm type.

  @retval TRUE            Successfully hash image.
  @retval FALSE           Fail in hash image.

**/
BOOLEAN
HashPeImage (
  IN  UINT32                HashAlg
  )
{
  BOOLEAN                   Status;
  EFI_IMAGE_SECTION_HEADER  *Section;
  VOID                      *HashCtx;
  UINTN                     CtxSize;
  UINT8                     *HashBase;
  UINTN                     HashSize;
  UINTN                     SumOfBytesHashed;
  EFI_IMAGE_SECTION_HEADER  *SectionHeader;
  UINTN                     Index;
  UINTN                     Pos;

  HashCtx       = NULL;
  SectionHeader = NULL;
  Status        = FALSE;

  if (HashAlg != HASHALG_SHA256) {
    return FALSE;
  }

  //
  // Initialize context of hash.
  //
  ZeroMem (mImageDigest, MAX_DIGEST_SIZE);

  mImageDigestSize  = SHA256_DIGEST_SIZE;
  mCertType         = gEfiCertSha256Guid;

  CtxSize   = mHash[HashAlg].GetContextSize();

  HashCtx = AllocatePool (CtxSize);
  ASSERT (HashCtx != NULL);

  // 1.  Load the image header into memory.

  // 2.  Initialize a SHA hash context.
  Status = mHash[HashAlg].HashInit(HashCtx);
  if (!Status) {
    goto Done;
  }
  //
  // Measuring PE/COFF Image Header;
  // But CheckSum field and SECURITY data directory (certificate) are excluded
  //

  //
  // 3.  Calculate the distance from the base of the image header to the image checksum address.
  // 4.  Hash the image header from its base to beginning of the image checksum.
  //
  HashBase = mImageBase;
  if (mNtHeader.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // Use PE32 offset.
    //
    HashSize = (UINTN) (&mNtHeader.Pe32->OptionalHeader.CheckSum) - (UINTN) HashBase;
  } else {
    //
    // Use PE32+ offset.
    //
    HashSize = (UINTN) (&mNtHeader.Pe32Plus->OptionalHeader.CheckSum) - (UINTN) HashBase;
  }

  Status  = mHash[HashAlg].HashUpdate(HashCtx, HashBase, HashSize);
  if (!Status) {
    goto Done;
  }
  //
  // 5.  Skip over the image checksum (it occupies a single ULONG).
  // 6.  Get the address of the beginning of the Cert Directory.
  // 7.  Hash everything from the end of the checksum to the start of the Cert Directory.
  //
  if (mNtHeader.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // Use PE32 offset.
    //
    HashBase = (UINT8 *) &mNtHeader.Pe32->OptionalHeader.CheckSum + sizeof (UINT32);
    HashSize = (UINTN) (&mNtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY]) - (UINTN) HashBase;
  } else {
    //
    // Use PE32+ offset.
    //
    HashBase = (UINT8 *) &mNtHeader.Pe32Plus->OptionalHeader.CheckSum + sizeof (UINT32);
    HashSize = (UINTN) (&mNtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY]) - (UINTN) HashBase;
  }

  Status  = mHash[HashAlg].HashUpdate(HashCtx, HashBase, HashSize);
  if (!Status) {
    goto Done;
  }
  //
  // 8.  Skip over the Cert Directory. (It is sizeof(IMAGE_DATA_DIRECTORY) bytes.)
  // 9.  Hash everything from the end of the Cert Directory to the end of image header.
  //
  if (mNtHeader.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // Use PE32 offset
    //
    HashBase = (UINT8 *) &mNtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1];
    HashSize = mNtHeader.Pe32->OptionalHeader.SizeOfHeaders - ((UINTN) (&mNtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1]) - (UINTN) mImageBase);
  } else {
    //
    // Use PE32+ offset.
    //
    HashBase = (UINT8 *) &mNtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1];
    HashSize = mNtHeader.Pe32Plus->OptionalHeader.SizeOfHeaders - ((UINTN) (&mNtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1]) - (UINTN) mImageBase);
  }

  Status  = mHash[HashAlg].HashUpdate(HashCtx, HashBase, HashSize);
  if (!Status) {
    goto Done;
  }
  //
  // 10. Set the SUM_OF_BYTES_HASHED to the size of the header.
  //
  if (mNtHeader.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // Use PE32 offset.
    //
    SumOfBytesHashed = mNtHeader.Pe32->OptionalHeader.SizeOfHeaders;
  } else {
    //
    // Use PE32+ offset
    //
    SumOfBytesHashed = mNtHeader.Pe32Plus->OptionalHeader.SizeOfHeaders;
  }

  //
  // 11. Build a temporary table of pointers to all the IMAGE_SECTION_HEADER
  //     structures in the image. The 'NumberOfSections' field of the image
  //     header indicates how big the table should be. Do not include any
  //     IMAGE_SECTION_HEADERs in the table whose 'SizeOfRawData' field is zero.
  //
  SectionHeader = (EFI_IMAGE_SECTION_HEADER *) AllocateZeroPool (sizeof (EFI_IMAGE_SECTION_HEADER) * mNtHeader.Pe32->FileHeader.NumberOfSections);
  ASSERT (SectionHeader != NULL);
  //
  // 12.  Using the 'PointerToRawData' in the referenced section headers as
  //      a key, arrange the elements in the table in ascending order. In other
  //      words, sort the section headers according to the disk-file offset of
  //      the section.
  //
  Section = (EFI_IMAGE_SECTION_HEADER *) (
               mImageBase +
               mPeCoffHeaderOffset +
               sizeof (UINT32) +
               sizeof (EFI_IMAGE_FILE_HEADER) +
               mNtHeader.Pe32->FileHeader.SizeOfOptionalHeader
               );
  for (Index = 0; Index < mNtHeader.Pe32->FileHeader.NumberOfSections; Index++) {
    Pos = Index;
    while ((Pos > 0) && (Section->PointerToRawData < SectionHeader[Pos - 1].PointerToRawData)) {
      CopyMem (&SectionHeader[Pos], &SectionHeader[Pos - 1], sizeof (EFI_IMAGE_SECTION_HEADER));
      Pos--;
    }
    CopyMem (&SectionHeader[Pos], Section, sizeof (EFI_IMAGE_SECTION_HEADER));
    Section += 1;
  }

  //
  // 13.  Walk through the sorted table, bring the corresponding section
  //      into memory, and hash the entire section (using the 'SizeOfRawData'
  //      field in the section header to determine the amount of data to hash).
  // 14.  Add the section's 'SizeOfRawData' to SUM_OF_BYTES_HASHED .
  // 15.  Repeat steps 13 and 14 for all the sections in the sorted table.
  //
  for (Index = 0; Index < mNtHeader.Pe32->FileHeader.NumberOfSections; Index++) {
    Section = &SectionHeader[Index];
    if (Section->SizeOfRawData == 0) {
      continue;
    }
    HashBase  = mImageBase + Section->PointerToRawData;
    HashSize  = (UINTN) Section->SizeOfRawData;

    Status  = mHash[HashAlg].HashUpdate(HashCtx, HashBase, HashSize);
    if (!Status) {
      goto Done;
    }

    SumOfBytesHashed += HashSize;
  }

  //
  // 16.  If the file size is greater than SUM_OF_BYTES_HASHED, there is extra
  //      data in the file that needs to be added to the hash. This data begins
  //      at file offset SUM_OF_BYTES_HASHED and its length is:
  //             FileSize  -  (CertDirectory->Size)
  //
  if (mImageSize > SumOfBytesHashed) {
    HashBase = mImageBase + SumOfBytesHashed;
    if (mNtHeader.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      //
      // Use PE32 offset.
      //
      HashSize = (UINTN)(
                 mImageSize -
                 mNtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY].Size -
                 SumOfBytesHashed);
    } else {
      //
      // Use PE32+ offset.
      //
      HashSize = (UINTN)(
                 mImageSize -
                 mNtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY].Size -
                 SumOfBytesHashed);
    }

    Status  = mHash[HashAlg].HashUpdate(HashCtx, HashBase, HashSize);
    if (!Status) {
      goto Done;
    }
  }

  Status  = mHash[HashAlg].HashFinal(HashCtx, mImageDigest);

Done:
  if (HashCtx != NULL) {
    FreePool (HashCtx);
  }
  if (SectionHeader != NULL) {
    FreePool (SectionHeader);
  }
  return Status;
}

/**
  Recognize the Hash algorithm in PE/COFF Authenticode and calculate hash of
  Pe/Coff image based on the authenticated image hashing in PE/COFF Specification
  8.0 Appendix A

  @retval EFI_UNSUPPORTED             Hash algorithm is not supported.
  @retval EFI_SUCCESS                 Hash successfully.

**/
EFI_STATUS
HashPeImageByType (
  VOID
  )
{
  UINT8                     Index;
  WIN_CERTIFICATE_EFI_PKCS  *PkcsCertData;

  PkcsCertData = (WIN_CERTIFICATE_EFI_PKCS *) (mImageBase + mSecDataDir->Offset);

  for (Index = 0; Index < HASHALG_MAX; Index++) {
    //
    // Check the Hash algorithm in PE/COFF Authenticode.
    //    According to PKCS#7 Definition:
    //        SignedData ::= SEQUENCE {
    //            version Version,
    //            digestAlgorithms DigestAlgorithmIdentifiers,
    //            contentInfo ContentInfo,
    //            .... }
    //    The DigestAlgorithmIdentifiers can be used to determine the hash algorithm in PE/COFF hashing
    //    This field has the fixed offset (+32) in final Authenticode ASN.1 data.
    //    Fixed offset (+32) is calculated based on two bytes of length encoding.
     //
    if ((*(PkcsCertData->CertData + 1) & TWO_BYTE_ENCODE) != TWO_BYTE_ENCODE) {
      //
      // Only support two bytes of Long Form of Length Encoding.
      //
      continue;
    }

    //
    if (CompareMem (PkcsCertData->CertData + 32, mHash[Index].OidValue, mHash[Index].OidLength) == 0) {
      break;
    }
  }

  if (Index == HASHALG_MAX) {
    return EFI_UNSUPPORTED;
  }

  //
  // HASH PE Image based on Hash algorithm in PE/COFF Authenticode.
  //
  if (!HashPeImage(Index)) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
  Enroll a new signature of executable into Signature Database.

  @param[in] PrivateData     The module's private data.
  @param[in] VariableName    Variable name of signature database, must be
                             EFI_IMAGE_SECURITY_DATABASE, EFI_IMAGE_SECURITY_DATABASE1
                             or EFI_IMAGE_SECURITY_DATABASE2.

  @retval   EFI_SUCCESS            New signature is enrolled successfully.
  @retval   EFI_INVALID_PARAMETER  The parameter is invalid.
  @retval   EFI_UNSUPPORTED        Unsupported command.
  @retval   EFI_OUT_OF_RESOURCES   Could not allocate needed resources.

**/
EFI_STATUS
EnrollAuthentication2Descriptor (
  IN SECUREBOOT_CONFIG_PRIVATE_DATA *Private,
  IN CHAR16                         *VariableName
  )
{
  EFI_STATUS                        Status;
  VOID                              *Data;
  UINTN                             DataSize;
  UINT32                            Attr;

  Data = NULL;

  //
  // DBT only support DER-X509 Cert Enrollment
  //
  if (StrCmp (VariableName, EFI_IMAGE_SECURITY_DATABASE2) == 0) {
    return EFI_UNSUPPORTED;
  }

  //
  // Read the whole file content
  //
  Status = ReadFileContent(
             Private->FileContext->FHandle,
             (VOID **) &mImageBase,
             &mImageSize,
             0
             );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
  ASSERT (mImageBase != NULL);

  Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS
         | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;

  //
  // Check if SigDB variable has been already existed.
  // If true, use EFI_VARIABLE_APPEND_WRITE attribute to append the
  // new signature data to original variable
  //
  DataSize = 0;
  Status = gRT->GetVariable(
                  VariableName,
                  &gEfiImageSecurityDatabaseGuid,
                  NULL,
                  &DataSize,
                  NULL
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Attr |= EFI_VARIABLE_APPEND_WRITE;
  } else if (Status != EFI_NOT_FOUND) {
    goto ON_EXIT;
  }

  //
  // Directly set AUTHENTICATION_2 data to SetVariable
  //
  Status = gRT->SetVariable(
                  VariableName,
                  &gEfiImageSecurityDatabaseGuid,
                  Attr,
                  mImageSize,
                  mImageBase
                  );

  DEBUG((DEBUG_INFO, "Enroll AUTH_2 data to Var:%s Status: %x\n", VariableName, Status));

ON_EXIT:

  CloseEnrolledFile(Private->FileContext);

  if (Data != NULL) {
    FreePool (Data);
  }

  if (mImageBase != NULL) {
    FreePool (mImageBase);
    mImageBase = NULL;
  }

  return Status;

}


/**
  Enroll a new signature of executable into Signature Database.

  @param[in] PrivateData     The module's private data.
  @param[in] VariableName    Variable name of signature database, must be
                             EFI_IMAGE_SECURITY_DATABASE, EFI_IMAGE_SECURITY_DATABASE1
                             or EFI_IMAGE_SECURITY_DATABASE2.

  @retval   EFI_SUCCESS            New signature is enrolled successfully.
  @retval   EFI_INVALID_PARAMETER  The parameter is invalid.
  @retval   EFI_UNSUPPORTED        Unsupported command.
  @retval   EFI_OUT_OF_RESOURCES   Could not allocate needed resources.

**/
EFI_STATUS
EnrollImageSignatureToSigDB (
  IN SECUREBOOT_CONFIG_PRIVATE_DATA *Private,
  IN CHAR16                         *VariableName
  )
{
  EFI_STATUS                        Status;
  EFI_SIGNATURE_LIST                *SigDBCert;
  EFI_SIGNATURE_DATA                *SigDBCertData;
  VOID                              *Data;
  UINTN                             DataSize;
  UINTN                             SigDBSize;
  UINT32                            Attr;
  WIN_CERTIFICATE_UEFI_GUID         *GuidCertData;

  Data = NULL;
  GuidCertData = NULL;

  if (StrCmp (VariableName, EFI_IMAGE_SECURITY_DATABASE2) == 0) {
    return EFI_UNSUPPORTED;
  }

  //
  // Form the SigDB certificate list.
  // Format the data item into EFI_SIGNATURE_LIST type.
  //
  // We need to parse signature data of executable from specified signed executable file.
  // In current implementation, we simply trust the pass-in signed executable file.
  // In reality, it's OS's responsibility to verify the signed executable file.
  //

  //
  // Read the whole file content
  //
  Status = ReadFileContent(
             Private->FileContext->FHandle,
             (VOID **) &mImageBase,
             &mImageSize,
             0
             );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
  ASSERT (mImageBase != NULL);

  Status = LoadPeImage ();
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  if (mSecDataDir->SizeOfCert == 0) {
    if (!HashPeImage (HASHALG_SHA256)) {
      Status =  EFI_SECURITY_VIOLATION;
      goto ON_EXIT;
    }
  } else {

    //
    // Read the certificate data
    //
    mCertificate = (WIN_CERTIFICATE *)(mImageBase + mSecDataDir->Offset);

    if (mCertificate->wCertificateType == WIN_CERT_TYPE_EFI_GUID) {
      GuidCertData = (WIN_CERTIFICATE_UEFI_GUID*) mCertificate;
      if (CompareMem (&GuidCertData->CertType, &gEfiCertTypeRsa2048Sha256Guid, sizeof(EFI_GUID)) != 0) {
        Status = EFI_ABORTED;
        goto ON_EXIT;
      }

      if (!HashPeImage (HASHALG_SHA256)) {
        Status = EFI_ABORTED;
        goto ON_EXIT;;
      }

    } else if (mCertificate->wCertificateType == WIN_CERT_TYPE_PKCS_SIGNED_DATA) {

      Status = HashPeImageByType ();
      if (EFI_ERROR (Status)) {
        goto ON_EXIT;;
      }
    } else {
      Status = EFI_ABORTED;
      goto ON_EXIT;
    }
  }

  //
  // Create a new SigDB entry.
  //
  SigDBSize = sizeof(EFI_SIGNATURE_LIST)
              + sizeof(EFI_SIGNATURE_DATA) - 1
              + (UINT32) mImageDigestSize;

  Data = (UINT8*) AllocateZeroPool (SigDBSize);
  if (Data == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  //
  // Adjust the Certificate Database parameters.
  //
  SigDBCert = (EFI_SIGNATURE_LIST*) Data;
  SigDBCert->SignatureListSize   = (UINT32) SigDBSize;
  SigDBCert->SignatureHeaderSize = 0;
  SigDBCert->SignatureSize       = sizeof(EFI_SIGNATURE_DATA) - 1 + (UINT32) mImageDigestSize;
  CopyGuid (&SigDBCert->SignatureType, &mCertType);

  SigDBCertData = (EFI_SIGNATURE_DATA*)((UINT8*)SigDBCert + sizeof(EFI_SIGNATURE_LIST));
  CopyGuid (&SigDBCertData->SignatureOwner, Private->SignatureGUID);
  CopyMem (SigDBCertData->SignatureData, mImageDigest, mImageDigestSize);

  Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS
          | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
  Status = CreateTimeBasedPayload (&SigDBSize, (UINT8**) &Data);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
    goto ON_EXIT;
  }

  //
  // Check if SigDB variable has been already existed.
  // If true, use EFI_VARIABLE_APPEND_WRITE attribute to append the
  // new signature data to original variable
  //
  DataSize = 0;
  Status = gRT->GetVariable(
                  VariableName,
                  &gEfiImageSecurityDatabaseGuid,
                  NULL,
                  &DataSize,
                  NULL
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Attr |= EFI_VARIABLE_APPEND_WRITE;
  } else if (Status != EFI_NOT_FOUND) {
    goto ON_EXIT;
  }

  //
  // Enroll the variable.
  //
  Status = gRT->SetVariable(
                  VariableName,
                  &gEfiImageSecurityDatabaseGuid,
                  Attr,
                  SigDBSize,
                  Data
                  );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

ON_EXIT:

  CloseEnrolledFile(Private->FileContext);

  if (Private->SignatureGUID != NULL) {
    FreePool (Private->SignatureGUID);
    Private->SignatureGUID = NULL;
  }

  if (Data != NULL) {
    FreePool (Data);
  }

  if (mImageBase != NULL) {
    FreePool (mImageBase);
    mImageBase = NULL;
  }

  return Status;
}

/**
  Enroll signature into DB/DBX/DBT without KEK's authentication.
  The SignatureOwner GUID will be Private->SignatureGUID.

  @param[in] PrivateData     The module's private data.
  @param[in] VariableName    Variable name of signature database, must be
                             EFI_IMAGE_SECURITY_DATABASE or EFI_IMAGE_SECURITY_DATABASE1.
  @param[out] Error          Error type checked in the certificate.

  @retval   EFI_SUCCESS            New signature enrolled successfully.
  @retval   EFI_INVALID_PARAMETER  The parameter is invalid.
  @retval   others                 Fail to enroll signature data.

**/
EFI_STATUS
EnrollSignatureDatabase (
  IN   SECUREBOOT_CONFIG_PRIVATE_DATA     *Private,
  IN   CHAR16                             *VariableName,
  OUT  ENROLL_KEY_ERROR                   *Error
  )
{
  UINT16*      FilePostFix;
  EFI_STATUS   Status;
  UINTN        NameLength;

  *Error = Unsupported_Type;

  if ((Private->FileContext->FileName == NULL) || (Private->FileContext->FHandle == NULL) || (Private->SignatureGUID == NULL)) {
    *Error = None_Error;
    return EFI_INVALID_PARAMETER;
  }

  Status = SetSecureBootMode (CUSTOM_SECURE_BOOT_MODE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Parse the file's postfix.
  //
  NameLength = StrLen (Private->FileContext->FileName);
  if (NameLength <= 4) {
    return EFI_INVALID_PARAMETER;
  }
  FilePostFix = Private->FileContext->FileName + NameLength - 4;
  if (IsDerEncodeCertificate (FilePostFix)) {
    //
    // Supports DER-encoded X509 certificate.
    //
    return EnrollX509toSigDB (Private, VariableName);
  } else if (IsAuthentication2Format(Private->FileContext->FHandle)){
    return EnrollAuthentication2Descriptor(Private, VariableName);
  } else {
    return EnrollImageSignatureToSigDB (Private, VariableName);
  }
}

/**
  List all signatures in specified signature database (e.g. KEK/DB/DBX/DBT)
  by GUID in the page for user to select and delete as needed.

  @param[in]    PrivateData         Module's private data.
  @param[in]    VariableName        The variable name of the vendor's signature database.
  @param[in]    VendorGuid          A unique identifier for the vendor.
  @param[in]    LabelNumber         Label number to insert opcodes.
  @param[in]    FormId              Form ID of current page.
  @param[in]    QuestionIdBase      Base question id of the signature list.

  @retval   EFI_SUCCESS             Success to update the signature list page
  @retval   EFI_OUT_OF_RESOURCES    Unable to allocate required resources.

**/
EFI_STATUS
UpdateDeletePage (
  IN SECUREBOOT_CONFIG_PRIVATE_DATA   *PrivateData,
  IN CHAR16                           *VariableName,
  IN EFI_GUID                         *VendorGuid,
  IN UINT16                           LabelNumber,
  IN EFI_FORM_ID                      FormId,
  IN EFI_QUESTION_ID                  QuestionIdBase
  )
{
  EFI_STATUS                  Status;
  UINT32                      Index;
  UINTN                       CertCount;
  UINTN                       GuidIndex;
  VOID                        *StartOpCodeHandle;
  VOID                        *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL          *StartLabel;
  EFI_IFR_GUID_LABEL          *EndLabel;
  UINTN                       DataSize;
  UINT8                       *Data;
  EFI_SIGNATURE_LIST          *CertList;
  EFI_SIGNATURE_DATA          *Cert;
  UINT32                      ItemDataSize;
  CHAR16                      *GuidStr;
  EFI_STRING_ID               GuidID;
  EFI_STRING_ID               Help;

  Data     = NULL;
  CertList = NULL;
  Cert     = NULL;
  GuidStr  = NULL;
  StartOpCodeHandle = NULL;
  EndOpCodeHandle   = NULL;

  //
  // Initialize the container for dynamic opcodes.
  //
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  if (StartOpCodeHandle == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  if (EndOpCodeHandle == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  //
  // Create Hii Extend Label OpCode.
  //
  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
                                        StartOpCodeHandle,
                                        &gEfiIfrTianoGuid,
                                        NULL,
                                        sizeof (EFI_IFR_GUID_LABEL)
                                        );
  StartLabel->ExtendOpCode  = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number        = LabelNumber;

  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
                                      EndOpCodeHandle,
                                      &gEfiIfrTianoGuid,
                                      NULL,
                                      sizeof (EFI_IFR_GUID_LABEL)
                                      );
  EndLabel->ExtendOpCode  = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number        = LABEL_END;

  //
  // Read Variable.
  //
  DataSize = 0;
  Status = gRT->GetVariable (VariableName, VendorGuid, NULL, &DataSize, Data);
  if (EFI_ERROR (Status) && Status != EFI_BUFFER_TOO_SMALL) {
    goto ON_EXIT;
  }

  Data = (UINT8 *) AllocateZeroPool (DataSize);
  if (Data == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  Status = gRT->GetVariable (VariableName, VendorGuid, NULL, &DataSize, Data);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  GuidStr = AllocateZeroPool (100);
  if (GuidStr == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  //
  // Enumerate all KEK pub data.
  //
  ItemDataSize = (UINT32) DataSize;
  CertList = (EFI_SIGNATURE_LIST *) Data;
  GuidIndex = 0;

  while ((ItemDataSize > 0) && (ItemDataSize >= CertList->SignatureListSize)) {

    if (CompareGuid (&CertList->SignatureType, &gEfiCertRsa2048Guid)) {
      Help = STRING_TOKEN (STR_CERT_TYPE_RSA2048_SHA256_GUID);
    } else if (CompareGuid (&CertList->SignatureType, &gEfiCertX509Guid)) {
      Help = STRING_TOKEN (STR_CERT_TYPE_PCKS7_GUID);
    } else if (CompareGuid (&CertList->SignatureType, &gEfiCertSha1Guid)) {
      Help = STRING_TOKEN (STR_CERT_TYPE_SHA1_GUID);
    } else if (CompareGuid (&CertList->SignatureType, &gEfiCertSha256Guid)) {
      Help = STRING_TOKEN (STR_CERT_TYPE_SHA256_GUID);
    } else if (CompareGuid (&CertList->SignatureType, &gEfiCertSm3Guid)) {
      Help = STRING_TOKEN (STR_CERT_TYPE_SM3_GUID);
    } else if (CompareGuid (&CertList->SignatureType, &gEfiCertX509Sha256Guid)) {
      Help = STRING_TOKEN (STR_CERT_TYPE_X509_SHA256_GUID);
    } else if (CompareGuid (&CertList->SignatureType, &gEfiCertX509Sha384Guid)) {
      Help = STRING_TOKEN (STR_CERT_TYPE_X509_SHA384_GUID);
    } else if (CompareGuid (&CertList->SignatureType, &gEfiCertX509Sha512Guid)) {
      Help = STRING_TOKEN (STR_CERT_TYPE_X509_SHA512_GUID);
    } else if (CompareGuid (&CertList->SignatureType, &gEfiCertX509Sm3Guid)) {
      Help = STRING_TOKEN (STR_CERT_TYPE_X509_SM3_GUID);
    } else {
      //
      // The signature type is not supported in current implementation.
      //
      ItemDataSize -= CertList->SignatureListSize;
      CertList = (EFI_SIGNATURE_LIST *) ((UINT8 *) CertList + CertList->SignatureListSize);
      continue;
    }

    CertCount  = (CertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - CertList->SignatureHeaderSize) / CertList->SignatureSize;
    for (Index = 0; Index < CertCount; Index++) {
      Cert = (EFI_SIGNATURE_DATA *) ((UINT8 *) CertList
                                              + sizeof (EFI_SIGNATURE_LIST)
                                              + CertList->SignatureHeaderSize
                                              + Index * CertList->SignatureSize);
      //
      // Display GUID and help
      //
      GuidToString (&Cert->SignatureOwner, GuidStr, 100);
      GuidID  = HiiSetString (PrivateData->HiiHandle, 0, GuidStr, NULL);
      HiiCreateCheckBoxOpCode (
        StartOpCodeHandle,
        (EFI_QUESTION_ID) (QuestionIdBase + GuidIndex++),
        0,
        0,
        GuidID,
        Help,
        EFI_IFR_FLAG_CALLBACK,
        0,
        NULL
        );
    }

    ItemDataSize -= CertList->SignatureListSize;
    CertList = (EFI_SIGNATURE_LIST *) ((UINT8 *) CertList + CertList->SignatureListSize);
  }

ON_EXIT:
  HiiUpdateForm (
    PrivateData->HiiHandle,
    &gSecureBootConfigFormSetGuid,
    FormId,
    StartOpCodeHandle,
    EndOpCodeHandle
    );

  if (StartOpCodeHandle != NULL) {
    HiiFreeOpCodeHandle (StartOpCodeHandle);
  }

  if (EndOpCodeHandle != NULL) {
    HiiFreeOpCodeHandle (EndOpCodeHandle);
  }

  if (Data != NULL) {
    FreePool (Data);
  }

  if (GuidStr != NULL) {
    FreePool (GuidStr);
  }

  return EFI_SUCCESS;
}

/**
  Delete a KEK entry from KEK database.

  @param[in]    PrivateData         Module's private data.
  @param[in]    QuestionId          Question id of the KEK item to delete.

  @retval   EFI_SUCCESS            Delete kek item successfully.
  @retval   EFI_OUT_OF_RESOURCES   Could not allocate needed resources.

**/
EFI_STATUS
DeleteKeyExchangeKey (
  IN SECUREBOOT_CONFIG_PRIVATE_DATA   *PrivateData,
  IN EFI_QUESTION_ID                  QuestionId
  )
{
  EFI_STATUS                  Status;
  UINTN                       DataSize;
  UINT8                       *Data;
  UINT8                       *OldData;
  UINT32                      Attr;
  UINT32                      Index;
  EFI_SIGNATURE_LIST          *CertList;
  EFI_SIGNATURE_LIST          *NewCertList;
  EFI_SIGNATURE_DATA          *Cert;
  UINTN                       CertCount;
  UINT32                      Offset;
  BOOLEAN                     IsKEKItemFound;
  UINT32                      KekDataSize;
  UINTN                       DeleteKekIndex;
  UINTN                       GuidIndex;

  Data            = NULL;
  OldData         = NULL;
  CertList        = NULL;
  Cert            = NULL;
  Attr            = 0;
  DeleteKekIndex  = QuestionId - OPTION_DEL_KEK_QUESTION_ID;

  Status = SetSecureBootMode(CUSTOM_SECURE_BOOT_MODE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get original KEK variable.
  //
  DataSize = 0;
  Status = gRT->GetVariable (EFI_KEY_EXCHANGE_KEY_NAME, &gEfiGlobalVariableGuid, NULL, &DataSize, NULL);
  if (EFI_ERROR(Status) && Status != EFI_BUFFER_TOO_SMALL) {
    goto ON_EXIT;
  }

  OldData = (UINT8*)AllocateZeroPool(DataSize);
  if (OldData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  Status = gRT->GetVariable (EFI_KEY_EXCHANGE_KEY_NAME, &gEfiGlobalVariableGuid, &Attr, &DataSize, OldData);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  //
  // Allocate space for new variable.
  //
  Data = (UINT8*) AllocateZeroPool (DataSize);
  if (Data == NULL) {
    Status  =  EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  //
  // Enumerate all KEK pub data and erasing the target item.
  //
  IsKEKItemFound = FALSE;
  KekDataSize = (UINT32) DataSize;
  CertList = (EFI_SIGNATURE_LIST *) OldData;
  Offset = 0;
  GuidIndex = 0;
  while ((KekDataSize > 0) && (KekDataSize >= CertList->SignatureListSize)) {
    if (CompareGuid (&CertList->SignatureType, &gEfiCertRsa2048Guid) ||
        CompareGuid (&CertList->SignatureType, &gEfiCertX509Guid)) {
      CopyMem (Data + Offset, CertList, (sizeof(EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize));
      NewCertList = (EFI_SIGNATURE_LIST *)(Data + Offset);
      Offset += (sizeof(EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize);
      Cert      = (EFI_SIGNATURE_DATA *) ((UINT8 *) CertList + sizeof (EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize);
      CertCount  = (CertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - CertList->SignatureHeaderSize) / CertList->SignatureSize;
      for (Index = 0; Index < CertCount; Index++) {
        if (GuidIndex == DeleteKekIndex ) {
          //
          // Find it! Skip it!
          //
          NewCertList->SignatureListSize -= CertList->SignatureSize;
          IsKEKItemFound = TRUE;
        } else {
          //
          // This item doesn't match. Copy it to the Data buffer.
          //
          CopyMem (Data + Offset, Cert, CertList->SignatureSize);
          Offset += CertList->SignatureSize;
        }
        GuidIndex++;
        Cert = (EFI_SIGNATURE_DATA *) ((UINT8*) Cert + CertList->SignatureSize);
      }
    } else {
      //
      // This List doesn't match. Copy it to the Data buffer.
      //
      CopyMem (Data + Offset, CertList, CertList->SignatureListSize);
      Offset += CertList->SignatureListSize;
    }

    KekDataSize -= CertList->SignatureListSize;
    CertList = (EFI_SIGNATURE_LIST*) ((UINT8*) CertList + CertList->SignatureListSize);
  }

  if (!IsKEKItemFound) {
    //
    // Doesn't find the Kek Item!
    //
    Status = EFI_NOT_FOUND;
    goto ON_EXIT;
  }

  //
  // Delete the Signature header if there is no signature in the list.
  //
  KekDataSize = Offset;
  CertList = (EFI_SIGNATURE_LIST*) Data;
  Offset = 0;
  ZeroMem (OldData, KekDataSize);
  while ((KekDataSize > 0) && (KekDataSize >= CertList->SignatureListSize)) {
    CertCount  = (CertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - CertList->SignatureHeaderSize) / CertList->SignatureSize;
    DEBUG ((DEBUG_INFO, "       CertCount = %x\n", CertCount));
    if (CertCount != 0) {
      CopyMem (OldData + Offset, CertList, CertList->SignatureListSize);
      Offset += CertList->SignatureListSize;
    }
    KekDataSize -= CertList->SignatureListSize;
    CertList = (EFI_SIGNATURE_LIST *) ((UINT8 *) CertList + CertList->SignatureListSize);
  }

  DataSize = Offset;
  if ((Attr & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) != 0) {
    Status = CreateTimeBasedPayload (&DataSize, &OldData);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
      goto ON_EXIT;
    }
  }

  Status = gRT->SetVariable(
                  EFI_KEY_EXCHANGE_KEY_NAME,
                  &gEfiGlobalVariableGuid,
                  Attr,
                  DataSize,
                  OldData
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to set variable, Status = %r\n", Status));
    goto ON_EXIT;
  }

ON_EXIT:
  if (Data != NULL) {
    FreePool(Data);
  }

  if (OldData != NULL) {
    FreePool(OldData);
  }

  return UpdateDeletePage (
           PrivateData,
           EFI_KEY_EXCHANGE_KEY_NAME,
           &gEfiGlobalVariableGuid,
           LABEL_KEK_DELETE,
           FORMID_DELETE_KEK_FORM,
           OPTION_DEL_KEK_QUESTION_ID
           );
}

/**
  Delete a signature entry from signature database.

  @param[in]    PrivateData         Module's private data.
  @param[in]    VariableName        The variable name of the vendor's signature database.
  @param[in]    VendorGuid          A unique identifier for the vendor.
  @param[in]    LabelNumber         Label number to insert opcodes.
  @param[in]    FormId              Form ID of current page.
  @param[in]    QuestionIdBase      Base question id of the signature list.
  @param[in]    DeleteIndex         Signature index to delete.

  @retval   EFI_SUCCESS             Delete signature successfully.
  @retval   EFI_NOT_FOUND           Can't find the signature item,
  @retval   EFI_OUT_OF_RESOURCES    Could not allocate needed resources.
**/
EFI_STATUS
DeleteSignature (
  IN SECUREBOOT_CONFIG_PRIVATE_DATA   *PrivateData,
  IN CHAR16                           *VariableName,
  IN EFI_GUID                         *VendorGuid,
  IN UINT16                           LabelNumber,
  IN EFI_FORM_ID                      FormId,
  IN EFI_QUESTION_ID                  QuestionIdBase,
  IN UINTN                            DeleteIndex
  )
{
  EFI_STATUS                  Status;
  UINTN                       DataSize;
  UINT8                       *Data;
  UINT8                       *OldData;
  UINT32                      Attr;
  UINT32                      Index;
  EFI_SIGNATURE_LIST          *CertList;
  EFI_SIGNATURE_LIST          *NewCertList;
  EFI_SIGNATURE_DATA          *Cert;
  UINTN                       CertCount;
  UINT32                      Offset;
  BOOLEAN                     IsItemFound;
  UINT32                      ItemDataSize;
  UINTN                       GuidIndex;

  Data            = NULL;
  OldData         = NULL;
  CertList        = NULL;
  Cert            = NULL;
  Attr            = 0;

  Status = SetSecureBootMode(CUSTOM_SECURE_BOOT_MODE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get original signature list data.
  //
  DataSize = 0;
  Status = gRT->GetVariable (VariableName, VendorGuid, NULL, &DataSize, NULL);
  if (EFI_ERROR (Status) && Status != EFI_BUFFER_TOO_SMALL) {
    goto ON_EXIT;
  }

  OldData = (UINT8 *) AllocateZeroPool (DataSize);
  if (OldData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  Status = gRT->GetVariable (VariableName, VendorGuid, &Attr, &DataSize, OldData);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  //
  // Allocate space for new variable.
  //
  Data = (UINT8*) AllocateZeroPool (DataSize);
  if (Data == NULL) {
    Status  =  EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  //
  // Enumerate all signature data and erasing the target item.
  //
  IsItemFound = FALSE;
  ItemDataSize = (UINT32) DataSize;
  CertList = (EFI_SIGNATURE_LIST *) OldData;
  Offset = 0;
  GuidIndex = 0;
  while ((ItemDataSize > 0) && (ItemDataSize >= CertList->SignatureListSize)) {
    if (CompareGuid (&CertList->SignatureType, &gEfiCertRsa2048Guid) ||
        CompareGuid (&CertList->SignatureType, &gEfiCertX509Guid) ||
        CompareGuid (&CertList->SignatureType, &gEfiCertSha1Guid) ||
        CompareGuid (&CertList->SignatureType, &gEfiCertSha256Guid) ||
        CompareGuid (&CertList->SignatureType, &gEfiCertSm3Guid) ||
        CompareGuid (&CertList->SignatureType, &gEfiCertX509Sha256Guid) ||
        CompareGuid (&CertList->SignatureType, &gEfiCertX509Sha384Guid) ||
        CompareGuid (&CertList->SignatureType, &gEfiCertX509Sha512Guid) ||
        CompareGuid (&CertList->SignatureType, &gEfiCertX509Sm3Guid)
        ) {
      //
      // Copy EFI_SIGNATURE_LIST header then calculate the signature count in this list.
      //
      CopyMem (Data + Offset, CertList, (sizeof(EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize));
      NewCertList = (EFI_SIGNATURE_LIST*) (Data + Offset);
      Offset += (sizeof(EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize);
      Cert      = (EFI_SIGNATURE_DATA *) ((UINT8 *) CertList + sizeof (EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize);
      CertCount  = (CertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - CertList->SignatureHeaderSize) / CertList->SignatureSize;
      for (Index = 0; Index < CertCount; Index++) {
        if (GuidIndex == DeleteIndex) {
          //
          // Find it! Skip it!
          //
          NewCertList->SignatureListSize -= CertList->SignatureSize;
          IsItemFound = TRUE;
        } else {
          //
          // This item doesn't match. Copy it to the Data buffer.
          //
          CopyMem (Data + Offset, (UINT8*)(Cert), CertList->SignatureSize);
          Offset += CertList->SignatureSize;
        }
        GuidIndex++;
        Cert = (EFI_SIGNATURE_DATA *) ((UINT8 *) Cert + CertList->SignatureSize);
      }
    } else {
      //
      // This List doesn't match. Just copy it to the Data buffer.
      //
      CopyMem (Data + Offset, (UINT8*)(CertList), CertList->SignatureListSize);
      Offset += CertList->SignatureListSize;
    }

    ItemDataSize -= CertList->SignatureListSize;
    CertList = (EFI_SIGNATURE_LIST *) ((UINT8 *) CertList + CertList->SignatureListSize);
  }

  if (!IsItemFound) {
    //
    // Doesn't find the signature Item!
    //
    Status = EFI_NOT_FOUND;
    goto ON_EXIT;
  }

  //
  // Delete the EFI_SIGNATURE_LIST header if there is no signature in the list.
  //
  ItemDataSize = Offset;
  CertList = (EFI_SIGNATURE_LIST *) Data;
  Offset = 0;
  ZeroMem (OldData, ItemDataSize);
  while ((ItemDataSize > 0) && (ItemDataSize >= CertList->SignatureListSize)) {
    CertCount  = (CertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - CertList->SignatureHeaderSize) / CertList->SignatureSize;
    DEBUG ((DEBUG_INFO, "       CertCount = %x\n", CertCount));
    if (CertCount != 0) {
      CopyMem (OldData + Offset, (UINT8*)(CertList), CertList->SignatureListSize);
      Offset += CertList->SignatureListSize;
    }
    ItemDataSize -= CertList->SignatureListSize;
    CertList = (EFI_SIGNATURE_LIST *) ((UINT8 *) CertList + CertList->SignatureListSize);
  }

  DataSize = Offset;
  if ((Attr & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) != 0) {
    Status = CreateTimeBasedPayload (&DataSize, &OldData);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
      goto ON_EXIT;
    }
  }

  Status = gRT->SetVariable(
                  VariableName,
                  VendorGuid,
                  Attr,
                  DataSize,
                  OldData
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to set variable, Status = %r\n", Status));
    goto ON_EXIT;
  }

ON_EXIT:
  if (Data != NULL) {
    FreePool(Data);
  }

  if (OldData != NULL) {
    FreePool(OldData);
  }

  return UpdateDeletePage (
           PrivateData,
           VariableName,
           VendorGuid,
           LabelNumber,
           FormId,
           QuestionIdBase
           );
}

/**
  This function to delete signature list or data, according by DelType.

  @param[in]  PrivateData           Module's private data.
  @param[in]  DelType               Indicate delete signature list or data.
  @param[in]  CheckedCount          Indicate how many signature data have
                                    been checked in current signature list.

  @retval   EFI_SUCCESS             Success to update the signature list page
  @retval   EFI_OUT_OF_RESOURCES    Unable to allocate required resources.
**/
EFI_STATUS
DeleteSignatureEx (
  IN SECUREBOOT_CONFIG_PRIVATE_DATA   *PrivateData,
  IN SIGNATURE_DELETE_TYPE            DelType,
  IN UINT32                           CheckedCount
  )
{
  EFI_STATUS          Status;
  EFI_SIGNATURE_LIST  *ListWalker;
  EFI_SIGNATURE_LIST  *NewCertList;
  EFI_SIGNATURE_DATA  *DataWalker;
  CHAR16              VariableName[BUFFER_MAX_SIZE];
  UINT32              VariableAttr;
  UINTN               VariableDataSize;
  UINTN               RemainingSize;
  UINTN               ListIndex;
  UINTN               Index;
  UINTN               Offset;
  UINT8               *VariableData;
  UINT8               *NewVariableData;

  Status              = EFI_SUCCESS;
  VariableAttr        = 0;
  VariableDataSize    = 0;
  ListIndex           = 0;
  Offset              = 0;
  VariableData        = NULL;
  NewVariableData     = NULL;

  if (PrivateData->VariableName == Variable_DB) {
    UnicodeSPrint (VariableName, sizeof (VariableName), EFI_IMAGE_SECURITY_DATABASE);
  } else if (PrivateData->VariableName == Variable_DBX) {
    UnicodeSPrint (VariableName, sizeof (VariableName), EFI_IMAGE_SECURITY_DATABASE1);
  } else if (PrivateData->VariableName == Variable_DBT) {
    UnicodeSPrint (VariableName, sizeof (VariableName), EFI_IMAGE_SECURITY_DATABASE2);
  } else {
    goto ON_EXIT;
  }

  Status = gRT->GetVariable (
                  VariableName,
                  &gEfiImageSecurityDatabaseGuid,
                  &VariableAttr,
                  &VariableDataSize,
                  VariableData
                );
  if (EFI_ERROR (Status) && Status != EFI_BUFFER_TOO_SMALL) {
    goto ON_EXIT;
  }

  VariableData = AllocateZeroPool (VariableDataSize);
  if (VariableData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  Status = gRT->GetVariable (
                  VariableName,
                  &gEfiImageSecurityDatabaseGuid,
                  &VariableAttr,
                  &VariableDataSize,
                  VariableData
                );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  Status = SetSecureBootMode (CUSTOM_SECURE_BOOT_MODE);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  NewVariableData = AllocateZeroPool (VariableDataSize);
  if (NewVariableData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  RemainingSize = VariableDataSize;
  ListWalker = (EFI_SIGNATURE_LIST *)(VariableData);
  if (DelType == Delete_Signature_List_All) {
    VariableDataSize = 0;
  } else {
    //
    //  Traverse to target EFI_SIGNATURE_LIST but others will be skipped.
    //
    while ((RemainingSize > 0) && (RemainingSize >= ListWalker->SignatureListSize) && ListIndex < PrivateData->ListIndex) {
      CopyMem ((UINT8 *)NewVariableData + Offset, ListWalker, ListWalker->SignatureListSize);
      Offset += ListWalker->SignatureListSize;

      RemainingSize -= ListWalker->SignatureListSize;
      ListWalker = (EFI_SIGNATURE_LIST *)((UINT8 *)ListWalker + ListWalker->SignatureListSize);
      ListIndex++;
    }

    //
    //  Handle the target EFI_SIGNATURE_LIST.
    //  If CheckedCount == SIGNATURE_DATA_COUNTS (ListWalker) or DelType == Delete_Signature_List_One
    //  it means delete the whole EFI_SIGNATURE_LIST, So we just skip this EFI_SIGNATURE_LIST.
    //
    if (CheckedCount < SIGNATURE_DATA_COUNTS (ListWalker) && DelType == Delete_Signature_Data) {
      NewCertList = (EFI_SIGNATURE_LIST *)(NewVariableData + Offset);
      //
      // Copy header.
      //
      CopyMem ((UINT8 *)NewVariableData + Offset, ListWalker, sizeof (EFI_SIGNATURE_LIST) + ListWalker->SignatureHeaderSize);
      Offset += sizeof (EFI_SIGNATURE_LIST) + ListWalker->SignatureHeaderSize;

      DataWalker = (EFI_SIGNATURE_DATA *)((UINT8 *)ListWalker + sizeof(EFI_SIGNATURE_LIST) + ListWalker->SignatureHeaderSize);
      for (Index = 0; Index < SIGNATURE_DATA_COUNTS(ListWalker); Index = Index + 1) {
        if (PrivateData->CheckArray[Index]) {
          //
          // Delete checked signature data, and update the size of whole signature list.
          //
          NewCertList->SignatureListSize -= NewCertList->SignatureSize;
        } else {
          //
          // Remain the unchecked signature data.
          //
          CopyMem ((UINT8 *)NewVariableData + Offset, DataWalker, ListWalker->SignatureSize);
          Offset += ListWalker->SignatureSize;
        }
        DataWalker = (EFI_SIGNATURE_DATA *)((UINT8 *)DataWalker + ListWalker->SignatureSize);
      }
    }

    RemainingSize -= ListWalker->SignatureListSize;
    ListWalker = (EFI_SIGNATURE_LIST *)((UINT8 *)ListWalker + ListWalker->SignatureListSize);

    //
    // Copy remaining data, maybe 0.
    //
    CopyMem((UINT8 *)NewVariableData + Offset, ListWalker, RemainingSize);
    Offset += RemainingSize;

    VariableDataSize = Offset;
  }

  if ((VariableAttr & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) != 0) {
    Status = CreateTimeBasedPayload (&VariableDataSize, &NewVariableData);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Fail to create time-based data payload: %r", Status));
      goto ON_EXIT;
    }
  }

  Status = gRT->SetVariable (
                  VariableName,
                  &gEfiImageSecurityDatabaseGuid,
                  VariableAttr,
                  VariableDataSize,
                  NewVariableData
                );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to set variable, Status = %r", Status));
    goto ON_EXIT;
  }

ON_EXIT:
  SECUREBOOT_FREE_NON_NULL (VariableData);
  SECUREBOOT_FREE_NON_NULL (NewVariableData);

  return Status;
}

/**
  Perform secure boot mode transition from User Mode by setting AuditMode 
  or DeployedMode variable.

  @param[in]  NewMode          New secure boot mode.

  @retval   EFI_SUCCESS        Secure Boot mode transition is successful.
**/
EFI_STATUS
TransitionFromUserMode(
  IN  UINT8 NewMode
  )
{
  UINT8      Data;
  EFI_STATUS Status;

  if (NewMode == SECURE_BOOT_MODE_AUDIT_MODE) {
    Data = 1;
    Status = gRT->SetVariable(
                    EFI_AUDIT_MODE_NAME,
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof(UINT8),
                    &Data
                    );
    return Status;
  } else if (NewMode == SECURE_BOOT_MODE_DEPLOYED_MODE) {
    Data = 1;
    Status = gRT->SetVariable(
                    EFI_DEPLOYED_MODE_NAME,
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof(UINT8),
                    &Data
                    );
    return Status;
  }

  //
  // Other case do nothing here. May Goto enroll PK page.
  //
  return EFI_SUCCESS;
}

/**
  Perform secure boot mode transition from Setup Mode by setting AuditMode 
  variable.

  @param[in]  NewMode          New secure boot mode.

  @retval   EFI_SUCCESS        Secure Boot mode transition is successful.
**/
EFI_STATUS
TransitionFromSetupMode(
  IN UINT8 NewMode
  )
{
  UINT8      Data;
  EFI_STATUS Status;

  Status = EFI_INVALID_PARAMETER;

  if (NewMode == SECURE_BOOT_MODE_AUDIT_MODE) {
    Data = 1;
    Status = gRT->SetVariable(
                    EFI_AUDIT_MODE_NAME,
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof(UINT8),
                    &Data
                    );
    return Status;
  }

  //
  // Other case do nothing here. May Goto enroll PK page.
  //
  return EFI_SUCCESS;
}

/**
  Perform secure boot mode transition from Audit Mode. Nothing is done here,
  should goto enroll PK page.

  @param[in]  NewMode          New secure boot mode.

  @retval   EFI_SUCCESS        Secure Boot mode transition is successful.
**/
EFI_STATUS
TransitionFromAuditMode(
  IN UINT8 NewMode
  )
{
  //
  // Other case do nothing here. Should Goto enroll PK page.
  //
  return EFI_SUCCESS;
}

/**
   Perform secure boot mode transition from Deployed Mode by setting Deployed Mode
   variable to 0.

  @param[in]  NewMode          New secure boot mode.

  @retval   EFI_SUCCESS        Secure Boot mode transition is successful.
**/
EFI_STATUS
TransitionFromDeployedMode(
  IN UINT8 NewMode
  )
{
  UINT8      Data;
  EFI_STATUS Status;

  //
  // Platform specific logic. when physical presence,  Allow to set DeployedMode =:0
  // to switch back to UserMode
  //
  if (NewMode == SECURE_BOOT_MODE_USER_MODE) {
    Data = 0;
    Status = gRT->SetVariable(
                    EFI_DEPLOYED_MODE_NAME,
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof(UINT8),
                    &Data
                    );
    DEBUG((EFI_D_INFO, "DeployedMode Status %x\n", Status));
    return Status;
  }
  return EFI_SUCCESS;
}

/**
   Perform main secure boot mode transition.

  @param[in]  CurMode          New secure boot mode.
  @param[in]  NewMode          New secure boot mode.

  @retval   EFI_SUCCESS        Secure Boot mode transition is successful.
**/
EFI_STATUS
SecureBootModeTransition(
  IN  UINT8  CurMode,
  IN  UINT8  NewMode
  )
{
  EFI_STATUS                         Status;

  //
  // Set platform to be customized mode to ensure platform specific mode switch success
  //
  Status = SetSecureBootMode(CUSTOM_SECURE_BOOT_MODE);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  DEBUG ((EFI_D_INFO, "SecureBootModeTransition: %d,%r\n", CurMode,Status));
  //
  // SecureBootMode transition
  //
  switch (CurMode) {
    case SECURE_BOOT_MODE_USER_MODE:
      Status = TransitionFromUserMode(NewMode);
      DEBUG ((EFI_D_INFO, "TransitionFromUserMode: %r\n", Status));
      break;

    case SECURE_BOOT_MODE_SETUP_MODE:
      Status = TransitionFromSetupMode(NewMode);
      DEBUG ((EFI_D_INFO, "TransitionFromSetupMode: %r\n", Status));
      break;

    case SECURE_BOOT_MODE_AUDIT_MODE:
      Status = TransitionFromAuditMode(NewMode);
      DEBUG ((EFI_D_INFO, "TransitionFromAuditMode: %r\n", Status));
      break;

    case SECURE_BOOT_MODE_DEPLOYED_MODE:
      Status = TransitionFromDeployedMode(NewMode);
      DEBUG ((EFI_D_INFO, "TransitionFromDeployedMode: %r\n", Status));
      break;

    default:
      Status = EFI_INVALID_PARAMETER;
      ASSERT(FALSE);
  }

  return Status;
}

/**
   Get current secure boot mode by retrieve data from SetupMode/AuditMode/DeployedMode.

  @param[out]  SecureBootMode                Current secure boot mode.

**/
VOID
ExtractSecureBootModeFromVariable(
  OUT UINT8      *SecureBootMode
  )
{
  UINT8     *SetupMode;
  UINT8     *AuditMode;
  UINT8     *DeployedMode;

  SetupMode        = NULL;
  AuditMode        = NULL;
  DeployedMode     = NULL;

  //
  // Get AuditMode/DeployedMode from variable
  //
  GetVariable2 (EFI_SETUP_MODE_NAME, &gEfiGlobalVariableGuid, (VOID**)&SetupMode, NULL);
  GetVariable2 (EFI_AUDIT_MODE_NAME, &gEfiGlobalVariableGuid, (VOID**)&AuditMode, NULL);
  GetVariable2 (EFI_DEPLOYED_MODE_NAME, &gEfiGlobalVariableGuid, (VOID**)&DeployedMode, NULL);
  if (SetupMode != NULL && AuditMode != NULL && DeployedMode != NULL) {
    if (*SetupMode == 0 && *AuditMode == 0 && *DeployedMode == 0) {
      //
      // User Mode
      //
      *SecureBootMode = SECURE_BOOT_MODE_USER_MODE;
      DEBUG ((EFI_D_INFO, "User Mode\n"));
    } else if (*SetupMode == 1 && *AuditMode == 0 && *DeployedMode == 0) {
      //
      // Setup Mode
      //
      *SecureBootMode = SECURE_BOOT_MODE_SETUP_MODE;
      DEBUG ((EFI_D_INFO, "Setup Mode\n"));
    } else if (*SetupMode == 1 && *AuditMode == 1 && *DeployedMode == 0) {
      //
      // Audit Mode
      //
      *SecureBootMode = SECURE_BOOT_MODE_AUDIT_MODE;
      DEBUG ((EFI_D_INFO, "Audit Mode\n"));
    } else if (*SetupMode == 0 && *AuditMode == 0 && *DeployedMode == 1) {
      //
      // Deployed Mode
      //
      *SecureBootMode = SECURE_BOOT_MODE_DEPLOYED_MODE;
      DEBUG ((EFI_D_INFO, "Deployed Mode\n"));
    } else {
      ASSERT(FALSE);
    }
  }else {
    ASSERT(FALSE);
  }

  if (SetupMode != NULL) {
    FreePool (SetupMode);
  }
  if (DeployedMode != NULL) {
    FreePool (DeployedMode);
  }
  if (AuditMode != NULL) {
    FreePool (AuditMode);
  }
}

VOID
ExtractSecureBootStatusFromVariable(
  OUT UINT8      *SecureBootStatus
  )
{
  UINT8     *SetupMode;
  UINT8     *AuditMode;
  UINT8     *DeployedMode;

  SetupMode        = NULL;
  AuditMode        = NULL;
  DeployedMode     = NULL;

  //
  // Get AuditMode/DeployedMode from variable
  //
  GetVariable2 (EFI_SETUP_MODE_NAME, &gEfiGlobalVariableGuid, (VOID**)&SetupMode, NULL);
  GetVariable2 (EFI_AUDIT_MODE_NAME, &gEfiGlobalVariableGuid, (VOID**)&AuditMode, NULL);
  GetVariable2 (EFI_DEPLOYED_MODE_NAME, &gEfiGlobalVariableGuid, (VOID**)&DeployedMode, NULL);
  if (SetupMode != NULL && AuditMode != NULL && DeployedMode != NULL) {
    if (*SetupMode == 0 && *AuditMode == 0 && *DeployedMode == 0) {
      //
      // User Mode
      //
      *SecureBootStatus = SECURE_BOOT_STATUS_ENABLE;
    } else if (*SetupMode == 1 && *AuditMode == 0 && *DeployedMode == 0) {
      //
      // Setup Mode
      //
      *SecureBootStatus = SECURE_BOOT_STATUS_DISABLE;
    } else if (*SetupMode == 1 && *AuditMode == 1 && *DeployedMode == 0) {
      //
      // Audit Mode
      //
      *SecureBootStatus = SECURE_BOOT_STATUS_DISABLE;
    } else if (*SetupMode == 0 && *AuditMode == 0 && *DeployedMode == 1) {
      //
      // Deployed Mode
      //
      *SecureBootStatus = SECURE_BOOT_STATUS_ENABLE;
    } else {
      ASSERT(FALSE);
    }
  }else {
    ASSERT(FALSE);
  }

  if (SetupMode != NULL) {
    FreePool (SetupMode);
  }
  if (DeployedMode != NULL) {
    FreePool (DeployedMode);
  }
  if (AuditMode != NULL) {
    FreePool (AuditMode);
  }
}

/**

  Update SecureBoot strings based on new Secure Boot Mode State. String includes STR_SECURE_BOOT_STATE_CONTENT
 and STR_CUR_SECURE_BOOT_MODE_CONTENT.

  @param[in]    PrivateData         Module's private data.

  @return EFI_SUCCESS              Update secure boot strings successfully.
  @return other                          Fail to update secure boot strings.

**/
EFI_STATUS
UpdateSecureBootString(
  IN SECUREBOOT_CONFIG_PRIVATE_DATA  *Private
  )
{
  UINT8       CurSecureBootMode;
  UINT8       *SecureBoot;
  EFI_STRING  SecureBootStatus;
  EFI_STRING  SecureBootMode;

  SecureBoot = NULL;
  SecureBootStatus = NULL;
  SecureBootMode = NULL;
  //
  // Get current secure boot state.
  //
  GetVariable2 (EFI_SECURE_BOOT_MODE_NAME, &gEfiGlobalVariableGuid, (VOID**)&SecureBoot, NULL);
  if (SecureBoot == NULL) {
    return EFI_NOT_FOUND;
  }

  if (*SecureBoot == SECURE_BOOT_MODE_ENABLE) {
    SecureBootStatus = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SECURE_BOOT_STATE_ENABLE), NULL);
    HiiSetString (Private->HiiHandle, STRING_TOKEN (STR_SECURE_BOOT_STATE_CONTENT), SecureBootStatus, NULL);
  }else {
    SecureBootStatus = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SECURE_BOOT_STATE_DISABLE), NULL);
    HiiSetString (Private->HiiHandle, STRING_TOKEN (STR_SECURE_BOOT_STATE_CONTENT), SecureBootStatus, NULL);
  }
  //
  // Get current secure boot mode.
  //
  ExtractSecureBootModeFromVariable(&CurSecureBootMode);
  
  if (CurSecureBootMode == SECURE_BOOT_MODE_USER_MODE) {
    SecureBootMode = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_USER_MODE), NULL);
    HiiSetString (Private->HiiHandle, STRING_TOKEN (STR_CUR_SECURE_BOOT_MODE_CONTENT), SecureBootMode, NULL);
  } else if (CurSecureBootMode == SECURE_BOOT_MODE_SETUP_MODE) {
    SecureBootMode = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SETUP_MODE), NULL);
    HiiSetString (Private->HiiHandle, STRING_TOKEN (STR_CUR_SECURE_BOOT_MODE_CONTENT), SecureBootMode, NULL);
  } else if (CurSecureBootMode == SECURE_BOOT_MODE_AUDIT_MODE) {
    SecureBootMode = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_AUDIT_MODE), NULL);
    HiiSetString (Private->HiiHandle, STRING_TOKEN (STR_CUR_SECURE_BOOT_MODE_CONTENT), SecureBootMode, NULL);
  } else if (CurSecureBootMode == SECURE_BOOT_MODE_DEPLOYED_MODE) {
    SecureBootMode = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_DEPLOYED_MODE), NULL);
    HiiSetString (Private->HiiHandle, STRING_TOKEN (STR_CUR_SECURE_BOOT_MODE_CONTENT), SecureBootMode, NULL);
  }
  

  FreePool(SecureBoot);
  FreePool(SecureBootMode);
  return EFI_SUCCESS;
}

/**
  This function extracts configuration from variable.

  @param[in]       Private      Point to SecureBoot configuration driver private data.
  @param[in, out]  ConfigData   Point to SecureBoot configuration private data.

**/
VOID
SecureBootExtractConfigFromVariable (
  IN SECUREBOOT_CONFIG_PRIVATE_DATA  *Private,
  IN OUT SECUREBOOT_CONFIGURATION    *ConfigData
  )
{
  UINT8     *SecureBootEnable;
  UINT8     *SecureBootMode;
  UINT8     *DefaultKey;
  EFI_TIME  CurrTime;

  SecureBootEnable = NULL;
  SecureBootMode   = NULL;
  DefaultKey       = NULL;

  //
  // Initialize the Date and Time using system time.
  //
  ConfigData->CertificateFormat = HASHALG_RAW;
  ConfigData->AlwaysRevocation = TRUE;
  gRT->GetTime (&CurrTime, NULL);
  ConfigData->RevocationDate.Year   = CurrTime.Year;
  ConfigData->RevocationDate.Month  = CurrTime.Month;
  ConfigData->RevocationDate.Day    = CurrTime.Day;
  ConfigData->RevocationTime.Hour   = CurrTime.Hour;
  ConfigData->RevocationTime.Minute = CurrTime.Minute;
  ConfigData->RevocationTime.Second = 0;
  if (Private->FileContext->FHandle != NULL) {
    ConfigData->FileEnrollType = Private->FileContext->FileType;
  } else {
    ConfigData->FileEnrollType = UNKNOWN_FILE_TYPE;
  }
  
  //
  // Control secure boot configure page.
  // PcdEnableSimplifiedSecureBootConfig, if this PCD is enabled, only SecureBootEnable option is provided. Other options are suppressed. 
  // PcdGrayOutSecureBootEnableOption, if this PCD is enabled, SecureBootEnable option is gray.
  //
  ConfigData->EnableSimplifedSecureBootConfig = PcdGetBool(PcdEnableSimplifedSecureBootConfig);
  DEBUG((DEBUG_INFO,"ConfigData->EnableSimplifedSecureBootConfig(%x)\n",ConfigData->EnableSimplifedSecureBootConfig));
  ConfigData->GrayOutSecureBootEnableOption = PcdGetBool(PcdGrayOutSecureBootEnableOption);
  DEBUG((DEBUG_INFO,"ConfigData->GrayOutSecureBootEnableOption(%x)\n",ConfigData->GrayOutSecureBootEnableOption));

  //
  // Check SecureBootEnable & Pk status, fix the inconsistency.
  // If the SecureBootEnable Variable doesn't exist, hide the SecureBoot Enable/Disable
  // Checkbox.
  //
  GetVariable2 (EFI_SECURE_BOOT_ENABLE_NAME, &gEfiSecureBootEnableDisableGuid, (VOID**)&SecureBootEnable, NULL);
  if ((SecureBootEnable == NULL)) {
    ConfigData->HideSecureBoot = TRUE;
  } else {
    ConfigData->HideSecureBoot = FALSE;
  }
  
  //
  // Get the SecureBootMode from CustomMode variable.
  //
  GetVariable2 (EFI_CUSTOM_MODE_NAME, &gEfiCustomModeEnableGuid, (VOID**)&SecureBootMode, NULL);
  if (SecureBootMode == NULL) {
    ConfigData->SecureBootMode = STANDARD_SECURE_BOOT_MODE;
  } else {
    ConfigData->SecureBootMode = *(SecureBootMode);
  }

  //
  // Extract current Secure Boot Mode
  //
  ExtractSecureBootModeFromVariable(&ConfigData->CurSecureBootMode);
  ExtractSecureBootStatusFromVariable(&ConfigData->CurSecureBootStatus);
  
  //
  // If there is no PK then the Delete Pk button will be gray.
  //
  if (ConfigData->CurSecureBootMode == SECURE_BOOT_MODE_SETUP_MODE || ConfigData->CurSecureBootMode == SECURE_BOOT_MODE_AUDIT_MODE) {
    ConfigData->HasPk = FALSE;
    ConfigData->HideSecureBoot = TRUE;
    ConfigData->DeletePk = TRUE;
  } else  {
    ConfigData->HasPk = TRUE;
    ConfigData->HideSecureBoot = FALSE;
    ConfigData->DeletePk = FALSE;
  }

  if (SecureBootEnable != NULL) {
    FreePool (SecureBootEnable);
  }
  if (SecureBootMode != NULL) {
    FreePool (SecureBootMode);
  }
  if (DefaultKey != NULL) {
    FreePool (DefaultKey);
  }
}

/**
  This function allows a caller to extract the current configuration for one
  or more named elements from the target driver.

  @param[in]   This              Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]   Request           A null-terminated Unicode string in
                                 <ConfigRequest> format.
  @param[out]  Progress          On return, points to a character in the Request
                                 string. Points to the string's null terminator if
                                 request was successful. Points to the most recent
                                 '&' before the first failing name/value pair (or
                                 the beginning of the string if the failure is in
                                 the first name/value pair) if the request was not
                                 successful.
  @param[out]  Results           A null-terminated Unicode string in
                                 <ConfigAltResp> format which has all values filled
                                 in for the names in the Request string. String to
                                 be allocated by the called function.

  @retval EFI_SUCCESS            The Results is filled with the requested values.
  @retval EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
  @retval EFI_INVALID_PARAMETER  Request is illegal syntax, or unknown name.
  @retval EFI_NOT_FOUND          Routing data doesn't match any storage in this
                                 driver.

**/
EFI_STATUS
EFIAPI
SecureBootExtractConfig (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL        *This,
  IN CONST EFI_STRING                            Request,
       OUT EFI_STRING                            *Progress,
       OUT EFI_STRING                            *Results
  )
{
  EFI_STATUS                        Status;
  UINTN                             BufferSize;
  UINTN                             Size;
  SECUREBOOT_CONFIGURATION          Configuration;
  EFI_STRING                        ConfigRequest;
  EFI_STRING                        ConfigRequestHdr;
  SECUREBOOT_CONFIG_PRIVATE_DATA    *PrivateData;
  BOOLEAN                           AllocatedRequest;

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  AllocatedRequest = FALSE;
  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  Size             = 0;

  ZeroMem (&Configuration, sizeof (Configuration));
  PrivateData      = SECUREBOOT_CONFIG_PRIVATE_FROM_THIS (This);
  *Progress        = Request;

  if ((Request != NULL) && !HiiIsConfigHdrMatch (Request, &gSecureBootConfigFormSetGuid, mSecureBootStorageName)) {
    return EFI_NOT_FOUND;
  }

  ZeroMem(&Configuration, sizeof(SECUREBOOT_CONFIGURATION));

  //
  // Get Configuration from Variable.
  //
  SecureBootExtractConfigFromVariable (PrivateData, &Configuration);

  BufferSize = sizeof (SECUREBOOT_CONFIGURATION);
  ConfigRequest = Request;
  if ((Request == NULL) || (StrStr (Request, L"OFFSET") == NULL)) {
    //
    // Request is set to NULL or OFFSET is NULL, construct full request string.
    //
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    ConfigRequestHdr = HiiConstructConfigHdr (&gSecureBootConfigFormSetGuid, mSecureBootStorageName, PrivateData->DriverHandle);
    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    ASSERT (ConfigRequest != NULL);
    AllocatedRequest = TRUE;
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)BufferSize);
    FreePool (ConfigRequestHdr);
    ConfigRequestHdr = NULL;
  }

  Status = gHiiConfigRouting->BlockToConfig (
                                gHiiConfigRouting,
                                ConfigRequest,
                                (UINT8 *) &Configuration,
                                BufferSize,
                                Results,
                                Progress
                                );

  //
  // Free the allocated config request string.
  //
  if (AllocatedRequest) {
    FreePool (ConfigRequest);
  }

  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = Request + StrLen (Request);
  }

  return Status;
}

/**
  This function processes the results of changes in configuration.

  @param[in]  This               Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]  Configuration      A null-terminated Unicode string in <ConfigResp>
                                 format.
  @param[out] Progress           A pointer to a string filled in with the offset of
                                 the most recent '&' before the first failing
                                 name/value pair (or the beginning of the string if
                                 the failure is in the first name/value pair) or
                                 the terminating NULL if all was successful.

  @retval EFI_SUCCESS            The Results is processed successfully.
  @retval EFI_INVALID_PARAMETER  Configuration is NULL.
  @retval EFI_NOT_FOUND          Routing data doesn't match any storage in this
                                 driver.

**/
EFI_STATUS
EFIAPI
SecureBootRouteConfig (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL      *This,
  IN CONST EFI_STRING                          Configuration,
       OUT EFI_STRING                          *Progress
  )
{
  SECUREBOOT_CONFIGURATION   IfrNvData;
  UINTN                      BufferSize;
  EFI_STATUS                 Status;
  SECUREBOOT_CONFIG_PRIVATE_DATA    *PrivateData;

  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;
  if (!HiiIsConfigHdrMatch (Configuration, &gSecureBootConfigFormSetGuid, mSecureBootStorageName)) {
    return EFI_NOT_FOUND;
  }

  PrivateData = SECUREBOOT_CONFIG_PRIVATE_FROM_THIS (This);

  //
  // Get Configuration from Variable.
  //
  SecureBootExtractConfigFromVariable (PrivateData, &IfrNvData);

  //
  // Map the Configuration to the configuration block.
  //
  BufferSize = sizeof (SECUREBOOT_CONFIGURATION);
  Status = gHiiConfigRouting->ConfigToBlock (
                                gHiiConfigRouting,
                                Configuration,
                                (UINT8 *)&IfrNvData,
                                &BufferSize,
                                Progress
                                );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  *Progress = Configuration + StrLen (Configuration);
  return EFI_SUCCESS;
}

/**
  This function to load signature list, the update the menu page.

  @param[in]  PrivateData         Module's private data.
  @param[in]  LabelId             Label number to insert opcodes.
  @param[in]  FormId              Form ID of current page.
  @param[in]  QuestionIdBase      Base question id of the signature list.

  @retval   EFI_SUCCESS           Success to update the signature list page
  @retval   EFI_OUT_OF_RESOURCES  Unable to allocate required resources.
**/
EFI_STATUS
LoadSignatureList (
  IN SECUREBOOT_CONFIG_PRIVATE_DATA *PrivateData,
  IN UINT16                         LabelId,
  IN EFI_FORM_ID                    FormId,
  IN EFI_QUESTION_ID                QuestionIdBase
  )
{
  EFI_STATUS            Status;
  EFI_STRING_ID         ListType;
  EFI_STRING            FormatNameString;
  EFI_STRING            FormatHelpString;
  EFI_STRING            FormatTypeString;
  EFI_SIGNATURE_LIST    *ListWalker;
  EFI_IFR_GUID_LABEL    *StartLabel;
  EFI_IFR_GUID_LABEL    *EndLabel;
  EFI_IFR_GUID_LABEL    *StartGoto;
  EFI_IFR_GUID_LABEL    *EndGoto;
  EFI_FORM_ID           DstFormId;
  VOID                  *StartOpCodeHandle;
  VOID                  *EndOpCodeHandle;
  VOID                  *StartGotoHandle;
  VOID                  *EndGotoHandle;
  UINTN                 DataSize;
  UINTN                 RemainingSize;
  UINT16                Index;
  UINT8                 *VariableData;
  CHAR16                VariableName[BUFFER_MAX_SIZE];
  CHAR16                NameBuffer[BUFFER_MAX_SIZE];
  CHAR16                HelpBuffer[BUFFER_MAX_SIZE];

  Status                = EFI_SUCCESS;
  FormatNameString      = NULL;
  FormatHelpString      = NULL;
  StartOpCodeHandle     = NULL;
  EndOpCodeHandle       = NULL;
  StartGotoHandle       = NULL;
  EndGotoHandle         = NULL;
  Index                 = 0;
  VariableData          = NULL;

  //
  // Initialize the container for dynamic opcodes.
  //
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  if (StartOpCodeHandle == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  if (EndOpCodeHandle == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  StartGotoHandle = HiiAllocateOpCodeHandle ();
  if (StartGotoHandle == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  EndGotoHandle = HiiAllocateOpCodeHandle ();
  if (EndGotoHandle == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  //
  // Create Hii Extend Label OpCode.
  //
  StartLabel = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode (
                                       StartOpCodeHandle,
                                       &gEfiIfrTianoGuid,
                                       NULL,
                                       sizeof (EFI_IFR_GUID_LABEL)
                                     );
  StartLabel->ExtendOpCode  = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number        = LabelId;

  EndLabel = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode (
                                     EndOpCodeHandle,
                                     &gEfiIfrTianoGuid,
                                     NULL,
                                     sizeof (EFI_IFR_GUID_LABEL)
                                   );
  EndLabel->ExtendOpCode  = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number        = LABEL_END;

  StartGoto = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode(
                                      StartGotoHandle,
                                      &gEfiIfrTianoGuid,
                                      NULL,
                                      sizeof(EFI_IFR_GUID_LABEL)
                                    );
  StartGoto->ExtendOpCode  = EFI_IFR_EXTEND_OP_LABEL;
  StartGoto->Number        = LABEL_DELETE_ALL_LIST_BUTTON;

  EndGoto = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode(
                                    EndGotoHandle,
                                    &gEfiIfrTianoGuid,
                                    NULL,
                                    sizeof(EFI_IFR_GUID_LABEL)
                                  );
  EndGoto->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndGoto->Number = LABEL_END;

  if (PrivateData->VariableName == Variable_DB) {
    UnicodeSPrint (VariableName, sizeof (VariableName), EFI_IMAGE_SECURITY_DATABASE);
    DstFormId = FORMID_SECURE_BOOT_DB_OPTION_FORM;
  } else if (PrivateData->VariableName == Variable_DBX) {
    UnicodeSPrint (VariableName, sizeof (VariableName), EFI_IMAGE_SECURITY_DATABASE1);
    DstFormId = FORMID_SECURE_BOOT_DBX_OPTION_FORM;
  } else if (PrivateData->VariableName == Variable_DBT) {
    UnicodeSPrint (VariableName, sizeof (VariableName), EFI_IMAGE_SECURITY_DATABASE2);
    DstFormId = FORMID_SECURE_BOOT_DBT_OPTION_FORM;
  } else {
    goto ON_EXIT;
  }

  HiiCreateGotoOpCode (
    StartGotoHandle,
    DstFormId,
    STRING_TOKEN (STR_SECURE_BOOT_DELETE_ALL_LIST),
    STRING_TOKEN (STR_SECURE_BOOT_DELETE_ALL_LIST),
    EFI_IFR_FLAG_CALLBACK,
    KEY_SECURE_BOOT_DELETE_ALL_LIST
  );

  //
  // Read Variable, the variable name save in the PrivateData->VariableName.
  //
  DataSize = 0;
  Status = gRT->GetVariable (VariableName, &gEfiImageSecurityDatabaseGuid, NULL, &DataSize, VariableData);
  if (EFI_ERROR (Status) && Status != EFI_BUFFER_TOO_SMALL) {
    goto ON_EXIT;
  }

  VariableData = AllocateZeroPool (DataSize);
  if (VariableData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }
  Status = gRT->GetVariable (VariableName, &gEfiImageSecurityDatabaseGuid, NULL, &DataSize, VariableData);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  FormatNameString = HiiGetString (PrivateData->HiiHandle, STRING_TOKEN (STR_SIGNATURE_LIST_NAME_FORMAT), NULL);
  FormatHelpString = HiiGetString (PrivateData->HiiHandle, STRING_TOKEN (STR_SIGNATURE_LIST_HELP_FORMAT), NULL);
  if (FormatNameString == NULL || FormatHelpString == NULL) {
    goto ON_EXIT;
  }

  RemainingSize = DataSize;
  ListWalker    = (EFI_SIGNATURE_LIST *)VariableData;
  while ((RemainingSize > 0) && (RemainingSize >= ListWalker->SignatureListSize)) {
    if (CompareGuid (&ListWalker->SignatureType, &gEfiCertRsa2048Guid)) {
      ListType = STRING_TOKEN (STR_LIST_TYPE_RSA2048_SHA256);
    } else if (CompareGuid (&ListWalker->SignatureType, &gEfiCertX509Guid)) {
      ListType = STRING_TOKEN (STR_LIST_TYPE_X509);
    } else if (CompareGuid (&ListWalker->SignatureType, &gEfiCertSha1Guid)) {
      ListType = STRING_TOKEN (STR_LIST_TYPE_SHA1);
    } else if (CompareGuid (&ListWalker->SignatureType, &gEfiCertSha256Guid)) {
      ListType = STRING_TOKEN (STR_LIST_TYPE_SHA256);
    } else if (CompareGuid (&ListWalker->SignatureType, &gEfiCertSm3Guid)) {
      ListType = STRING_TOKEN (STR_LIST_TYPE_SM3);
    } else if (CompareGuid (&ListWalker->SignatureType, &gEfiCertX509Sha256Guid)) {
      ListType = STRING_TOKEN (STR_LIST_TYPE_X509_SHA256);
    } else if (CompareGuid (&ListWalker->SignatureType, &gEfiCertX509Sha384Guid)) {
      ListType = STRING_TOKEN (STR_LIST_TYPE_X509_SHA384);
    } else if (CompareGuid (&ListWalker->SignatureType, &gEfiCertX509Sha512Guid)) {
      ListType = STRING_TOKEN (STR_LIST_TYPE_X509_SHA512);
    } else if (CompareGuid (&ListWalker->SignatureType, &gEfiCertX509Sm3Guid)) {
      ListType = STRING_TOKEN (STR_LIST_TYPE_X509_SM3);
    } else {
      ListType = STRING_TOKEN (STR_LIST_TYPE_UNKNOWN);
    }
    FormatTypeString = HiiGetString (PrivateData->HiiHandle, ListType, NULL);
    if (FormatTypeString == NULL) {
      goto ON_EXIT;
    }

    ZeroMem (NameBuffer, sizeof (NameBuffer));
    UnicodeSPrint (NameBuffer, sizeof (NameBuffer), FormatNameString, Index + 1);

    ZeroMem (HelpBuffer, sizeof (HelpBuffer));
    UnicodeSPrint (HelpBuffer,
      sizeof (HelpBuffer),
      FormatHelpString,
      FormatTypeString,
      SIGNATURE_DATA_COUNTS (ListWalker)
    );
    SECUREBOOT_FREE_NON_NULL (FormatTypeString);
    FormatTypeString = NULL;

    HiiCreateGotoOpCode (
      StartOpCodeHandle,
      SECUREBOOT_DELETE_SIGNATURE_DATA_FORM,
      HiiSetString (PrivateData->HiiHandle, 0, NameBuffer, NULL),
      HiiSetString (PrivateData->HiiHandle, 0, HelpBuffer, NULL),
      EFI_IFR_FLAG_CALLBACK,
      QuestionIdBase + Index++
    );

    RemainingSize -= ListWalker->SignatureListSize;
    ListWalker = (EFI_SIGNATURE_LIST *)((UINT8 *)ListWalker + ListWalker->SignatureListSize);
  }

ON_EXIT:
  HiiUpdateForm (
    PrivateData->HiiHandle,
    &gSecureBootConfigFormSetGuid,
    FormId,
    StartOpCodeHandle,
    EndOpCodeHandle
  );

  HiiUpdateForm (
    PrivateData->HiiHandle,
    &gSecureBootConfigFormSetGuid,
    FormId,
    StartGotoHandle,
    EndGotoHandle
  );

  SECUREBOOT_FREE_NON_OPCODE (StartOpCodeHandle);
  SECUREBOOT_FREE_NON_OPCODE (EndOpCodeHandle);
  SECUREBOOT_FREE_NON_OPCODE (StartGotoHandle);
  SECUREBOOT_FREE_NON_OPCODE (EndGotoHandle);

  SECUREBOOT_FREE_NON_NULL (VariableData);
  SECUREBOOT_FREE_NON_NULL (FormatNameString);
  SECUREBOOT_FREE_NON_NULL (FormatHelpString);

  PrivateData->ListCount = Index;

  return Status;
}

/**
  Parse hash value from EFI_SIGNATURE_DATA, and save in the CHAR16 type array.
  The buffer is callee allocated and should be freed by the caller.

  @param[in]    ListEntry                 The pointer point to the signature list.
  @param[in]    DataEntry                 The signature data we are processing.
  @param[out]   BufferToReturn            Buffer to save the hash value.

  @retval       EFI_INVALID_PARAMETER     Invalid List or Data or Buffer.
  @retval       EFI_OUT_OF_RESOURCES      A memory allocation failed.
  @retval       EFI_SUCCESS               Operation success.
**/
EFI_STATUS
ParseHashValue (
  IN     EFI_SIGNATURE_LIST    *ListEntry,
  IN     EFI_SIGNATURE_DATA    *DataEntry,
     OUT CHAR16                **BufferToReturn
  )
{
  UINTN       Index;
  UINTN       BufferIndex;
  UINTN       TotalSize;
  UINTN       DataSize;
  UINTN       Line;
  UINTN       OneLineBytes;

  //
  //  Assume that, display 8 bytes in one line.
  //
  OneLineBytes = 8;

  if (ListEntry == NULL || DataEntry == NULL || BufferToReturn == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DataSize = ListEntry->SignatureSize - sizeof(EFI_GUID);
  Line = (DataSize + OneLineBytes - 1) / OneLineBytes;

  //
  // Each byte will split two Hex-number, and each line need additional memory to save '\r\n'.
  //
  TotalSize = ((DataSize + Line) * 2 * sizeof(CHAR16));

  *BufferToReturn = AllocateZeroPool(TotalSize);
  if (*BufferToReturn == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0, BufferIndex = 0; Index < DataSize; Index = Index + 1) {
    if ((Index > 0) && (Index % OneLineBytes == 0)) {
      BufferIndex += UnicodeSPrint(&(*BufferToReturn)[BufferIndex], TotalSize - sizeof(CHAR16) * BufferIndex, L"\n");
    }
    BufferIndex += UnicodeSPrint(&(*BufferToReturn)[BufferIndex], TotalSize - sizeof(CHAR16) * BufferIndex, L"%02x", DataEntry->SignatureData[Index]);
  }
  BufferIndex += UnicodeSPrint(&(*BufferToReturn)[BufferIndex], TotalSize - sizeof(CHAR16) * BufferIndex, L"\n");

  return EFI_SUCCESS;
}

/**
  Function to get the common name from the X509 format certificate.
  The buffer is callee allocated and should be freed by the caller.

  @param[in]    ListEntry                 The pointer point to the signature list.
  @param[in]    DataEntry                 The signature data we are processing.
  @param[out]   BufferToReturn            Buffer to save the CN of X509 certificate.

  @retval       EFI_INVALID_PARAMETER     Invalid List or Data or Buffer.
  @retval       EFI_OUT_OF_RESOURCES      A memory allocation failed.
  @retval       EFI_SUCCESS               Operation success.
  @retval       EFI_NOT_FOUND             Not found CN field in the X509 certificate.
**/
EFI_STATUS
GetCommonNameFromX509 (
  IN     EFI_SIGNATURE_LIST    *ListEntry,
  IN     EFI_SIGNATURE_DATA    *DataEntry,
     OUT CHAR16                **BufferToReturn
  )
{
  EFI_STATUS      Status;
  CHAR8           *CNBuffer;
  UINTN           CNBufferSize;

  Status        = EFI_SUCCESS;
  CNBuffer      = NULL;

  CNBuffer = AllocateZeroPool(256);
  if (CNBuffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  CNBufferSize = 256;
  X509GetCommonName (
    (UINT8 *)DataEntry + sizeof(EFI_GUID),
    ListEntry->SignatureSize - sizeof(EFI_GUID),
    CNBuffer,
    &CNBufferSize
  );

  *BufferToReturn = AllocateZeroPool(256 * sizeof(CHAR16));
  if (*BufferToReturn == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  AsciiStrToUnicodeStrS (CNBuffer, *BufferToReturn, 256);

ON_EXIT:
  SECUREBOOT_FREE_NON_NULL (CNBuffer);

  return Status;
}

/**
  Format the help info for the signature data, each help info contain 3 parts.
  1. Owner Guid.
  2. Content, depends on the type of the signature list.
  3. Revocation time.

  @param[in]      PrivateData             Module's private data.
  @param[in]      ListEntry               Point to the signature list.
  @param[in]      DataEntry               Point to the signature data we are processing.
  @param[out]     StringId                Save the string id of help info.

  @retval         EFI_SUCCESS             Operation success.
  @retval         EFI_OUT_OF_RESOURCES    Unable to allocate required resources.
**/
EFI_STATUS
FormatHelpInfo (
  IN     SECUREBOOT_CONFIG_PRIVATE_DATA   *PrivateData,
  IN     EFI_SIGNATURE_LIST               *ListEntry,
  IN     EFI_SIGNATURE_DATA               *DataEntry,
     OUT EFI_STRING_ID                    *StringId
  )
{
  EFI_STATUS      Status;
  EFI_TIME        *Time;
  EFI_STRING_ID   ListTypeId;
  EFI_STRING      FormatHelpString;
  EFI_STRING      FormatTypeString;
  UINTN           DataSize;
  UINTN           HelpInfoIndex;
  UINTN           TotalSize;
  CHAR16          GuidString[BUFFER_MAX_SIZE];
  CHAR16          TimeString[BUFFER_MAX_SIZE];
  CHAR16          *DataString;
  CHAR16          *HelpInfoString;
  BOOLEAN         IsCert;

  Status            = EFI_SUCCESS;
  Time              = NULL;
  FormatTypeString  = NULL;
  HelpInfoIndex     = 0;
  DataString        = NULL;
  HelpInfoString    = NULL;
  IsCert            = FALSE;

  if (CompareGuid(&ListEntry->SignatureType, &gEfiCertRsa2048Guid)) {
    ListTypeId = STRING_TOKEN(STR_LIST_TYPE_RSA2048_SHA256);
    DataSize = ListEntry->SignatureSize - sizeof(EFI_GUID);
    IsCert = TRUE;
  } else if (CompareGuid(&ListEntry->SignatureType, &gEfiCertX509Guid)) {
    ListTypeId = STRING_TOKEN(STR_LIST_TYPE_X509);
    DataSize = ListEntry->SignatureSize - sizeof(EFI_GUID);
    IsCert = TRUE;
  } else if (CompareGuid(&ListEntry->SignatureType, &gEfiCertSha1Guid)) {
    ListTypeId = STRING_TOKEN(STR_LIST_TYPE_SHA1);
    DataSize = 20;
  } else if (CompareGuid(&ListEntry->SignatureType, &gEfiCertSha256Guid)) {
    ListTypeId = STRING_TOKEN(STR_LIST_TYPE_SHA256);
    DataSize = 32;
  }  else if (CompareGuid(&ListEntry->SignatureType, &gEfiCertSm3Guid)) {
    ListTypeId = STRING_TOKEN(STR_LIST_TYPE_SM3);
    DataSize = 32;
  } else if (CompareGuid(&ListEntry->SignatureType, &gEfiCertX509Sha256Guid)) {
    ListTypeId = STRING_TOKEN(STR_LIST_TYPE_X509_SHA256);
    DataSize = 32;
    Time = (EFI_TIME *)(DataEntry->SignatureData + DataSize);
  } else if (CompareGuid(&ListEntry->SignatureType, &gEfiCertX509Sha384Guid)) {
    ListTypeId = STRING_TOKEN(STR_LIST_TYPE_X509_SHA384);
    DataSize = 48;
    Time = (EFI_TIME *)(DataEntry->SignatureData + DataSize);
  } else if (CompareGuid(&ListEntry->SignatureType, &gEfiCertX509Sha512Guid)) {
    ListTypeId = STRING_TOKEN(STR_LIST_TYPE_X509_SHA512);
    DataSize = 64;
    Time = (EFI_TIME *)(DataEntry->SignatureData + DataSize);
  } else if (CompareGuid(&ListEntry->SignatureType, &gEfiCertX509Sm3Guid)) {
    ListTypeId = STRING_TOKEN(STR_LIST_TYPE_X509_SM3);
    DataSize = 32;
    Time = (EFI_TIME *)(DataEntry->SignatureData + DataSize);
  } else {
    Status = EFI_UNSUPPORTED;
    goto ON_EXIT;
  }

  FormatTypeString = HiiGetString (PrivateData->HiiHandle, ListTypeId, NULL);
  if (FormatTypeString == NULL) {
    Status = EFI_NOT_FOUND;
    goto ON_EXIT;
  }

  TotalSize = 1024;
  HelpInfoString = AllocateZeroPool (TotalSize);
  if (HelpInfoString == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  //
  // Format GUID part.
  //
  ZeroMem (GuidString, sizeof (GuidString));
  GuidToString(&DataEntry->SignatureOwner, GuidString, BUFFER_MAX_SIZE);
  FormatHelpString = HiiGetString (PrivateData->HiiHandle, STRING_TOKEN (STR_SIGNATURE_DATA_HELP_FORMAT_GUID), NULL);
  if (FormatHelpString == NULL) {
    Status = EFI_NOT_FOUND;
    goto ON_EXIT;
  }
  HelpInfoIndex += UnicodeSPrint (
                     &HelpInfoString[HelpInfoIndex],
                     TotalSize - sizeof(CHAR16) * HelpInfoIndex,
                     FormatHelpString,
                     GuidString
                   );
  SECUREBOOT_FREE_NON_NULL (FormatHelpString);
  FormatHelpString = NULL;

  //
  // Format content part, it depends on the type of signature list, hash value or CN.
  //
  if (IsCert) {
    GetCommonNameFromX509 (ListEntry, DataEntry, &DataString);
    FormatHelpString = HiiGetString (PrivateData->HiiHandle, STRING_TOKEN (STR_SIGNATURE_DATA_HELP_FORMAT_CN), NULL);
  } else {
    //
    //  Format hash value for each signature data entry.
    //
    ParseHashValue (ListEntry, DataEntry, &DataString);
    FormatHelpString = HiiGetString (PrivateData->HiiHandle, STRING_TOKEN (STR_SIGNATURE_DATA_HELP_FORMAT_HASH), NULL);
  }
  if (FormatHelpString == NULL) {
    Status = EFI_NOT_FOUND;
    goto ON_EXIT;
  }
  HelpInfoIndex += UnicodeSPrint (
                     &HelpInfoString[HelpInfoIndex],
                     TotalSize - sizeof (CHAR16) * HelpInfoIndex,
                     FormatHelpString,
                     FormatTypeString,
                     DataSize,
                     DataString
                   );
  SECUREBOOT_FREE_NON_NULL (FormatHelpString);
  FormatHelpString = NULL;

  //
  // Format revocation time part.
  //
  if (Time != NULL) {
    ZeroMem (TimeString, sizeof (TimeString));
    UnicodeSPrint (
      TimeString,
      sizeof (TimeString),
      L"%d-%d-%d %d:%d:%d",
      Time->Year,
      Time->Month,
      Time->Day,
      Time->Hour,
      Time->Minute,
      Time->Second
    );
    FormatHelpString = HiiGetString (PrivateData->HiiHandle, STRING_TOKEN (STR_SIGNATURE_DATA_HELP_FORMAT_TIME), NULL);
    if (FormatHelpString == NULL) {
      Status = EFI_NOT_FOUND;
      goto ON_EXIT;
    }
    UnicodeSPrint (
      &HelpInfoString[HelpInfoIndex],
      TotalSize - sizeof (CHAR16) * HelpInfoIndex,
      FormatHelpString,
      TimeString
    );
    SECUREBOOT_FREE_NON_NULL (FormatHelpString);
    FormatHelpString = NULL;
  }

  *StringId = HiiSetString (PrivateData->HiiHandle, 0, HelpInfoString, NULL);
ON_EXIT:
  SECUREBOOT_FREE_NON_NULL (DataString);
  SECUREBOOT_FREE_NON_NULL (HelpInfoString);

  SECUREBOOT_FREE_NON_NULL (FormatTypeString);

  return Status;
}

/**
  This function to load signature data under the signature list.

  @param[in]  PrivateData         Module's private data.
  @param[in]  LabelId             Label number to insert opcodes.
  @param[in]  FormId              Form ID of current page.
  @param[in]  QuestionIdBase      Base question id of the signature list.
  @param[in]  ListIndex           Indicate to load which signature list.

  @retval   EFI_SUCCESS           Success to update the signature list page
  @retval   EFI_OUT_OF_RESOURCES  Unable to allocate required resources.
**/
EFI_STATUS
LoadSignatureData (
  IN SECUREBOOT_CONFIG_PRIVATE_DATA *PrivateData,
  IN UINT16                         LabelId,
  IN EFI_FORM_ID                    FormId,
  IN EFI_QUESTION_ID                QuestionIdBase,
  IN UINT16                         ListIndex
  )
{
  EFI_STATUS            Status;
  EFI_SIGNATURE_LIST    *ListWalker;
  EFI_SIGNATURE_DATA    *DataWalker;
  EFI_IFR_GUID_LABEL    *StartLabel;
  EFI_IFR_GUID_LABEL    *EndLabel;
  EFI_STRING_ID         HelpStringId;
  EFI_STRING            FormatNameString;
  VOID                  *StartOpCodeHandle;
  VOID                  *EndOpCodeHandle;
  UINTN                 DataSize;
  UINTN                 RemainingSize;
  UINT16                Index;
  UINT8                 *VariableData;
  CHAR16                VariableName[BUFFER_MAX_SIZE];
  CHAR16                NameBuffer[BUFFER_MAX_SIZE];

  Status              = EFI_SUCCESS;
  FormatNameString    = NULL;
  StartOpCodeHandle   = NULL;
  EndOpCodeHandle     = NULL;
  Index               = 0;
  VariableData        = NULL;

  //
  // Initialize the container for dynamic opcodes.
  //
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  if (StartOpCodeHandle == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  if (EndOpCodeHandle == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  //
  // Create Hii Extend Label OpCode.
  //
  StartLabel = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode (
                                       StartOpCodeHandle,
                                       &gEfiIfrTianoGuid,
                                       NULL,
                                       sizeof (EFI_IFR_GUID_LABEL)
                                     );
  StartLabel->ExtendOpCode  = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number        = LabelId;

  EndLabel = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode (
                                     EndOpCodeHandle,
                                     &gEfiIfrTianoGuid,
                                     NULL,
                                     sizeof (EFI_IFR_GUID_LABEL)
                                   );
  EndLabel->ExtendOpCode  = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number        = LABEL_END;

  if (PrivateData->VariableName == Variable_DB) {
    UnicodeSPrint (VariableName, sizeof (VariableName), EFI_IMAGE_SECURITY_DATABASE);
  } else if (PrivateData->VariableName == Variable_DBX) {
    UnicodeSPrint (VariableName, sizeof (VariableName), EFI_IMAGE_SECURITY_DATABASE1);
  } else if (PrivateData->VariableName == Variable_DBT) {
    UnicodeSPrint (VariableName, sizeof (VariableName), EFI_IMAGE_SECURITY_DATABASE2);
  } else {
    goto ON_EXIT;
  }

  //
  // Read Variable, the variable name save in the PrivateData->VariableName.
  //
  DataSize = 0;
  Status = gRT->GetVariable (VariableName, &gEfiImageSecurityDatabaseGuid, NULL, &DataSize, VariableData);
  if (EFI_ERROR (Status) && Status != EFI_BUFFER_TOO_SMALL) {
    goto ON_EXIT;
  }

  VariableData = AllocateZeroPool (DataSize);
  if (VariableData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }
  Status = gRT->GetVariable (VariableName, &gEfiImageSecurityDatabaseGuid, NULL, &DataSize, VariableData);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  RemainingSize = DataSize;
  ListWalker = (EFI_SIGNATURE_LIST *)VariableData;

  //
  // Skip signature list.
  //
  while ((RemainingSize > 0) && (RemainingSize >= ListWalker->SignatureListSize) && ListIndex-- > 0) {
    RemainingSize -= ListWalker->SignatureListSize;
    ListWalker = (EFI_SIGNATURE_LIST *)((UINT8 *)ListWalker + ListWalker->SignatureListSize);
  }

  FormatNameString = HiiGetString (PrivateData->HiiHandle, STRING_TOKEN (STR_SIGNATURE_DATA_NAME_FORMAT), NULL);
  if (FormatNameString == NULL) {
    goto ON_EXIT;
  }

  DataWalker = (EFI_SIGNATURE_DATA *)((UINT8 *)ListWalker + sizeof(EFI_SIGNATURE_LIST) + ListWalker->SignatureHeaderSize);
  for (Index = 0; Index < SIGNATURE_DATA_COUNTS(ListWalker); Index = Index + 1) {
    //
    // Format name buffer.
    //
    ZeroMem (NameBuffer, sizeof (NameBuffer));
    UnicodeSPrint (NameBuffer, sizeof (NameBuffer), FormatNameString, Index + 1);

    //
    // Format help info buffer.
    //
    Status = FormatHelpInfo (PrivateData, ListWalker, DataWalker, &HelpStringId);
    if (EFI_ERROR (Status)) {
      goto ON_EXIT;
    }

    HiiCreateCheckBoxOpCode (
      StartOpCodeHandle,
      (EFI_QUESTION_ID)(QuestionIdBase + Index),
      0,
      0,
      HiiSetString (PrivateData->HiiHandle, 0, NameBuffer, NULL),
      HelpStringId,
      EFI_IFR_FLAG_CALLBACK,
      0,
      NULL
    );

    ZeroMem(NameBuffer, 100);
    DataWalker = (EFI_SIGNATURE_DATA *)((UINT8 *)DataWalker + ListWalker->SignatureSize);
  }

  //
  // Allocate a buffer to record which signature data will be checked.
  // This memory buffer will be freed when exit from the SECUREBOOT_DELETE_SIGNATURE_DATA_FORM form.
  //
  PrivateData->CheckArray = AllocateZeroPool (SIGNATURE_DATA_COUNTS (ListWalker) * sizeof (BOOLEAN));
ON_EXIT:
  HiiUpdateForm (
    PrivateData->HiiHandle,
    &gSecureBootConfigFormSetGuid,
    FormId,
    StartOpCodeHandle,
    EndOpCodeHandle
  );

  SECUREBOOT_FREE_NON_OPCODE (StartOpCodeHandle);
  SECUREBOOT_FREE_NON_OPCODE (EndOpCodeHandle);

  SECUREBOOT_FREE_NON_NULL (VariableData);
  SECUREBOOT_FREE_NON_NULL (FormatNameString);

  return Status;
}

CHAR16 *gYesNoStrList[2];
CHAR16 *gResetToSetupModeStr;	
CHAR16 *gWishResetToSetupModeStr;
CHAR16 *gRestoreFactoryKeysStr;
CHAR16 *gWishRestoreFactoryKeysStr;
CHAR16 *gEnableSecureBootStr;
CHAR16 *gWishEnableSecureBootStr;
CHAR16 *gDisableSecureBootStr;
CHAR16 *gWishDisableSecureBootStr;
CHAR16 *gPolicyUpdatePromptStr;
CHAR16 *gPromptMsgStr;

VOID InitString(EFI_HII_HANDLE  HiiHandle)
{
  EFI_STRING  Str;

  Str = HiiGetString(HiiHandle, STRING_TOKEN(STR_YES), NULL);
  if(gYesNoStrList[0] != NULL && StrCmp(gYesNoStrList[0], Str) == 0){
    goto ProcExit;
  }	

  DEBUG((EFI_D_INFO, "Update String\n"));

  if(gYesNoStrList[0] != NULL){
    FreePool(gYesNoStrList[0]);
    FreePool(gYesNoStrList[1]);
    FreePool(gResetToSetupModeStr);
    FreePool(gWishResetToSetupModeStr);
    FreePool(gRestoreFactoryKeysStr);
    FreePool(gWishRestoreFactoryKeysStr);
    FreePool(gEnableSecureBootStr);
    FreePool(gWishEnableSecureBootStr);	
    FreePool(gDisableSecureBootStr);
    FreePool(gWishDisableSecureBootStr);
    FreePool(gPolicyUpdatePromptStr);
    FreePool(gPromptMsgStr);			
  }
  gYesNoStrList[0]           = HiiGetString(HiiHandle, STRING_TOKEN(STR_YES), NULL);
  gYesNoStrList[1]           = HiiGetString(HiiHandle, STRING_TOKEN(STR_NO), NULL);	  
  gResetToSetupModeStr       = HiiGetString(HiiHandle, STRING_TOKEN(STR_RESET_TO_SETUP_MODE_TITLE), NULL);
  gWishResetToSetupModeStr   = HiiGetString(HiiHandle, STRING_TOKEN(STR_RESET_TO_SETUP_MODE_CONFIRM), NULL);
  gRestoreFactoryKeysStr     = HiiGetString(HiiHandle, STRING_TOKEN(STR_RESTORE_FACTORY_KEY_TITLE), NULL);
  gWishRestoreFactoryKeysStr = HiiGetString(HiiHandle, STRING_TOKEN(STR_RESTORE_FACTORY_KEY_CONFIRM), NULL);
  gEnableSecureBootStr       = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENABLE_SECURE_BOOT_TITLE), NULL);
  gWishEnableSecureBootStr   = HiiGetString(HiiHandle, STRING_TOKEN(STR_ENABLE_SECURE_BOOT_CONFIRM), NULL);
  gDisableSecureBootStr      = HiiGetString(HiiHandle, STRING_TOKEN(STR_DISABLE_SECURE_BOOT_TITLE), NULL);
  gWishDisableSecureBootStr  = HiiGetString(HiiHandle, STRING_TOKEN(STR_DISABLE_SECURE_BOOT_CONFIRM), NULL);
  gPolicyUpdatePromptStr   = HiiGetString(HiiHandle, STRING_TOKEN(STR_POLICY_UPDATE_PROMPT_CONFIRM), NULL);
  gPromptMsgStr            = HiiGetString(HiiHandle, STRING_TOKEN(STR_PROMPT_MSG_CONFIRM), NULL);

  
ProcExit:
  if(Str != NULL){FreePool(Str);}
  return;	
}

/**
  This function is called to provide results data to the driver.

  @param[in]  This               Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]  Action             Specifies the type of action taken by the browser.
  @param[in]  QuestionId         A unique value which is sent to the original
                                 exporting driver so that it can identify the type
                                 of data to expect.
  @param[in]  Type               The type of value for the question.
  @param[in]  Value              A pointer to the data being sent to the original
                                 exporting driver.
  @param[out] ActionRequest      On return, points to the action requested by the
                                 callback function.

  @retval EFI_SUCCESS            The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES   Not enough storage is available to hold the
                                 variable and its data.
  @retval EFI_DEVICE_ERROR       The variable could not be saved.
  @retval EFI_UNSUPPORTED        The specified Action is not supported by the
                                 callback.

**/
EFI_STATUS
EFIAPI
SecureBootCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL      *This,
  IN     EFI_BROWSER_ACTION                    Action,
  IN     EFI_QUESTION_ID                       QuestionId,
  IN     UINT8                                 Type,
  IN     EFI_IFR_TYPE_VALUE                    *Value,
  OUT EFI_BROWSER_ACTION_REQUEST               *ActionRequest
  )
{
  EFI_INPUT_KEY                   Key;
  EFI_STATUS                      Status;
  RETURN_STATUS                   RStatus;
  SECUREBOOT_CONFIG_PRIVATE_DATA  *Private;
  UINTN                           BufferSize;
  SECUREBOOT_CONFIGURATION        *IfrNvData = NULL;
  UINT16                          LabelId;
  UINT8                           *SecureBootEnable;
  UINT8                           *SecureBootMode;
  CHAR16                          PromptString[100];
  EFI_DEVICE_PATH_PROTOCOL        *File;
  UINTN                           NameLength;
  UINT16                          *FilePostFix;
  BOOLEAN                         GetBrowserDataResult = FALSE;
  ENROLL_KEY_ERROR                EnrollKeyErrorCode;
  SELECTION_TYPE                  Choice;
  EFI_STRING                      StringInfo;
  EFI_STRING                      String;
  SECUREBOOT_CONFIG_PRIVATE_DATA  *PrivateData;
  EFI_STRING                      InputSignatureString;
  EFI_GUID                        InputSignatureGuid;

  Status             = EFI_SUCCESS;
  SecureBootEnable   = NULL;
  SecureBootMode     = NULL;
  File               = NULL;
  EnrollKeyErrorCode = None_Error;
  if ((This == NULL) || (Value == NULL) || (ActionRequest == NULL)) {
    Status = EFI_INVALID_PARAMETER;
    goto EXIT;
  }

  if (Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
    //
    // keep current value as the default value.
    //
    return EFI_SUCCESS;
  }

  Private = SECUREBOOT_CONFIG_PRIVATE_FROM_THIS (This);

  gSecureBootPrivateData = Private;

  //
  // Retrieve uncommitted data from Browser
  //
  BufferSize = sizeof (SECUREBOOT_CONFIGURATION);
  IfrNvData = AllocateZeroPool (BufferSize);
  if (IfrNvData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;    
  }

  GetBrowserDataResult = HiiGetBrowserData (&gSecureBootConfigFormSetGuid, mSecureBootStorageName, BufferSize, (UINT8 *) IfrNvData);

  InitString(Private->HiiHandle);

  LibAuthVarSetPhysicalPresent(TRUE);

  if (Action == EFI_BROWSER_ACTION_FORM_OPEN) {
    if (QuestionId == SECURE_BOOT_ENABLE_DISABLE){
      //
      // Gets the time in the current Browser
      //
      mDate.Year   = IfrNvData->RevocationDate.Year;
      mDate.Month  = IfrNvData->RevocationDate.Month;
      mDate.Day    = IfrNvData->RevocationDate.Day;
      mTime.Hour   = IfrNvData->RevocationTime.Hour;
      mTime.Minute = IfrNvData->RevocationTime.Minute;
      //
      // Secure Boot Policy variable changes after transition. Re-sync CurSecureBootMode
      //
      Status = UpdateSecureBootString(Private);
      SecureBootExtractConfigFromVariable (Private, IfrNvData);
      mIsEnterSecureBootForm = TRUE;
      mIsSelectedSecureBootModeForm = TRUE;
      mIsSecureBootModeChanged = FALSE;
     } else {
      //
      // When entering SecureBoot OPTION Form
      // always close opened file & free resource
      //
      if ((QuestionId == KEY_SECURE_BOOT_PK_OPTION) ||
          (QuestionId == KEY_SECURE_BOOT_KEK_OPTION) ||
          (QuestionId == KEY_SECURE_BOOT_DB_OPTION) ||
          (QuestionId == KEY_SECURE_BOOT_DBX_OPTION) ||
          (QuestionId == KEY_SECURE_BOOT_DBT_OPTION)) {
        CloseEnrolledFile(Private->FileContext);
      } else if (QuestionId == KEY_SECURE_BOOT_DELETE_ALL_LIST) {
        //
        // Update ListCount field in varstore
        // Button "Delete All Signature List" is
        // enable when ListCount is greater than 0.
        //
        IfrNvData->ListCount = Private->ListCount;
      }
    }

    goto EXIT;
  }

  if (Action == EFI_BROWSER_ACTION_RETRIEVE) {
    Status = EFI_UNSUPPORTED;
    goto EXIT;
  }

  if ((Action != EFI_BROWSER_ACTION_CHANGED) &&
      (Action != EFI_BROWSER_ACTION_CHANGING) &&
      (Action != EFI_BROWSER_ACTION_FORM_CLOSE) &&
      (Action != EFI_BROWSER_ACTION_DEFAULT_STANDARD)) {
    Status = EFI_UNSUPPORTED;
    goto EXIT;
  }

  if (Action == EFI_BROWSER_ACTION_CHANGING) {

    switch (QuestionId) {
    case RESTORE_FACTORY_KEY_SECURE_BOOT:

      Choice = UiConfirmDialog(DIALOG_YESNO, gRestoreFactoryKeysStr, NULL, TEXT_ALIGIN_CENTER, gWishRestoreFactoryKeysStr, NULL);
        if(Choice == SELECTION_YES){
        UiConfirmDialog (DIALOG_NO_KEY, gPromptMsgStr, NULL, TEXT_ALIGIN_CENTER, gPolicyUpdatePromptStr, L"", NULL);
        LibNotifySetupItemChanged(gBS, ITEM_SECUREBOOT, STATUS_ENABLE);
        PcdSet16S (PcdPlatformDefaultKey,0x1);
        AuthVarMfgReset();
        UpdateSecureBootString(Private);
        SecureBootExtractConfigFromVariable (Private, IfrNvData);
        }
      
      break;
    case SECURE_BOOT_ENABLE_DISABLE:
        if(Value->u8 == SECURE_BOOT_STATUS_ENABLE ){
        if(IfrNvData->CurSecureBootStatus != SECURE_BOOT_STATUS_ENABLE){

          Choice = UiConfirmDialogEx(DIALOG_ATTRIBUTE_RECOVER,DIALOG_YESNO, gEnableSecureBootStr, NULL, TEXT_ALIGIN_CENTER, gWishEnableSecureBootStr, NULL);
          if(Choice == SELECTION_YES){
            UiConfirmDialogEx (DIALOG_ATTRIBUTE_RECOVER,DIALOG_NO_KEY, gPromptMsgStr, NULL, TEXT_ALIGIN_CENTER, gPolicyUpdatePromptStr, L"", NULL);
            LibNotifySetupItemChanged(gBS, ITEM_SECUREBOOT, STATUS_ENABLE);
            AuthVarMfgReset();
            UpdateSecureBootString(Private);
            SecureBootExtractConfigFromVariable (Private, IfrNvData);
            break;
          }
          Value->u8 = SECURE_BOOT_STATUS_DISABLE;
         } 
        }else if(Value->u8 == SECURE_BOOT_STATUS_DISABLE){
        if(IfrNvData->CurSecureBootStatus != SECURE_BOOT_STATUS_DISABLE){
          Choice = UiConfirmDialogEx(DIALOG_ATTRIBUTE_RECOVER,DIALOG_YESNO, gDisableSecureBootStr, NULL, TEXT_ALIGIN_CENTER, gWishDisableSecureBootStr, NULL);
          if(Choice == SELECTION_YES){
            UiConfirmDialogEx (DIALOG_ATTRIBUTE_RECOVER,DIALOG_NO_KEY, gPromptMsgStr, NULL, TEXT_ALIGIN_CENTER, gPolicyUpdatePromptStr, L"", NULL);
            Status = DeletePlatformKey ();
            PcdSet16S (PcdPlatformDefaultKey,0x0);
            UpdateSecureBootString(Private);
            SecureBootExtractConfigFromVariable (Private, IfrNvData);
            break;
          }
          Value->u8 = SECURE_BOOT_STATUS_ENABLE;
         } 
        }
        break;
    case KEY_SECURE_BOOT_RESET:
        Choice = UiConfirmDialogEx(DIALOG_ATTRIBUTE_RECOVER,DIALOG_YESNO, gResetToSetupModeStr, NULL, TEXT_ALIGIN_CENTER, gWishResetToSetupModeStr, NULL);
        if(Choice == SELECTION_YES){
          UiConfirmDialogEx (DIALOG_ATTRIBUTE_RECOVER,DIALOG_NO_KEY, gPromptMsgStr, NULL, TEXT_ALIGIN_CENTER, gPolicyUpdatePromptStr, L"", NULL);
          Status = RemoveAllCertKey();
          PcdSet16S (PcdPlatformDefaultKey,0x0);
          UpdateSecureBootString(Private);
          SecureBootExtractConfigFromVariable (Private, IfrNvData);
        }      
        break;
    case KEY_SECURE_BOOT_AUDIT_MODE:

    //
    // User Mode/Setup Mode/Deployed Mode ->AuditMode
    //
          UiConfirmDialog (DIALOG_NO_KEY, gPromptMsgStr, NULL, TEXT_ALIGIN_CENTER, gPolicyUpdatePromptStr, L"", NULL);
          if(IfrNvData->CurSecureBootMode == SECURE_BOOT_MODE_USER_MODE){
             Status = DeletePlatformKey ();
             Status = SecureBootModeTransition(IfrNvData->CurSecureBootMode, SECURE_BOOT_MODE_AUDIT_MODE);
  
          }else if(IfrNvData->CurSecureBootMode == SECURE_BOOT_MODE_SETUP_MODE){
             Status = SecureBootModeTransition(IfrNvData->CurSecureBootMode, SECURE_BOOT_MODE_AUDIT_MODE);
          }
          PcdSet16S (PcdPlatformDefaultKey,0x0);
          UpdateSecureBootString(Private);
          SecureBootExtractConfigFromVariable (Private, IfrNvData);
      
      break;
        
    case KEY_SECURE_BOOT_DEPLOYED_MODE:
    //
    // User Mode/Setup Mode/Audit Mode ->Deployed Mode
    //
          UiConfirmDialog (DIALOG_NO_KEY, gPromptMsgStr, NULL, TEXT_ALIGIN_CENTER, gPolicyUpdatePromptStr, L"", NULL);
          LibNotifySetupItemChanged(gBS, ITEM_SECUREBOOT, STATUS_ENABLE);
          if(IfrNvData->CurSecureBootMode == SECURE_BOOT_MODE_USER_MODE){
             Status = SecureBootModeTransition(IfrNvData->CurSecureBootMode, SECURE_BOOT_MODE_DEPLOYED_MODE);
          }else if(IfrNvData->CurSecureBootMode == SECURE_BOOT_MODE_SETUP_MODE){
             AuthVarMfgReset();
            
             Status = SecureBootModeTransition(SECURE_BOOT_MODE_USER_MODE, SECURE_BOOT_MODE_DEPLOYED_MODE);
          }else if(IfrNvData->CurSecureBootMode == SECURE_BOOT_MODE_AUDIT_MODE){
            AuthVarMfgReset();
          }
          PcdSet16S (PcdPlatformDefaultKey,0x0);
          UpdateSecureBootString(Private);
          SecureBootExtractConfigFromVariable (Private, IfrNvData);
      break;
    case KEY_SECURE_BOOT_EXIT_DEPLOYED_MODE:
        
    //
    // Deployed Mode -> User Mode
    //
          UiConfirmDialog (DIALOG_NO_KEY, gPromptMsgStr, NULL, TEXT_ALIGIN_CENTER, gPolicyUpdatePromptStr, L"", NULL);
          Status = SecureBootModeTransition(IfrNvData->CurSecureBootMode, SECURE_BOOT_MODE_USER_MODE);
          PcdSet16S (PcdPlatformDefaultKey,0x0);
          UpdateSecureBootString(Private);
          SecureBootExtractConfigFromVariable (Private, IfrNvData);
      break;
         

    case KEY_SECURE_BOOT_KEK_OPTION:
    case KEY_SECURE_BOOT_DB_OPTION:
    case KEY_SECURE_BOOT_DBX_OPTION:
    case KEY_SECURE_BOOT_DBT_OPTION:
      PrivateData = SECUREBOOT_CONFIG_PRIVATE_FROM_THIS (This);
      //
      // Clear Signature GUID.
      //
      ZeroMem (IfrNvData->SignatureGuid, sizeof (IfrNvData->SignatureGuid));
      if (Private->SignatureGUID == NULL) {
        Private->SignatureGUID = (EFI_GUID *) AllocateZeroPool (sizeof (EFI_GUID));
        if (Private->SignatureGUID == NULL) {
          Status = EFI_OUT_OF_RESOURCES;
          goto EXIT;
        }
      }

      //
      // Cleanup VFRData once leaving PK/KEK/DB/DBX/DBT enroll/delete page
      //
      SecureBootExtractConfigFromVariable (PrivateData, IfrNvData);

      if (QuestionId == KEY_SECURE_BOOT_DB_OPTION) {
        LabelId = SECUREBOOT_ENROLL_SIGNATURE_TO_DB;
      } else if (QuestionId == KEY_SECURE_BOOT_DBX_OPTION) {
        LabelId = SECUREBOOT_ENROLL_SIGNATURE_TO_DBX;
      } else if (QuestionId == KEY_SECURE_BOOT_DBT_OPTION) {
        LabelId = SECUREBOOT_ENROLL_SIGNATURE_TO_DBT;
      } else {
        LabelId = FORMID_ENROLL_KEK_FORM;
      }

      //
      // Refresh selected file.
      //
      CleanUpPage (LabelId, Private);
      break;
    case KEY_SECURE_BOOT_PK_OPTION:
      LabelId = FORMID_ENROLL_PK_FORM;
      //
      // Refresh selected file.
      //
      CleanUpPage (LabelId, Private);
      break;

    case FORMID_ENROLL_PK_FORM:
      ChooseFile (NULL, NULL, UpdatePKFromFile, &File);
      break;

    case FORMID_ENROLL_KEK_FORM:
      ChooseFile (NULL, NULL, UpdateKEKFromFile, &File);
      break;

    case SECUREBOOT_ENROLL_SIGNATURE_TO_DB:
      ChooseFile (NULL, NULL, UpdateDBFromFile, &File);
      break;

    case SECUREBOOT_ENROLL_SIGNATURE_TO_DBX:
      ChooseFile (NULL, NULL, UpdateDBXFromFile, &File);

      if (Private->FileContext->FHandle != NULL) {
        //
        // Parse the file's postfix.
        //
        NameLength = StrLen (Private->FileContext->FileName);
        if (NameLength <= 4) {
          Status = EFI_INVALID_PARAMETER;
          goto EXIT;
        }
        FilePostFix = Private->FileContext->FileName + NameLength - 4;

        if (IsDerEncodeCertificate (FilePostFix)) {
          //
          // Supports DER-encoded X509 certificate.
          //
          IfrNvData->FileEnrollType = X509_CERT_FILE_TYPE;
        } else if (IsAuthentication2Format(Private->FileContext->FHandle)){
          IfrNvData->FileEnrollType = AUTHENTICATION_2_FILE_TYPE;
        } else {
          IfrNvData->FileEnrollType = PE_IMAGE_FILE_TYPE;
        }
        Private->FileContext->FileType = IfrNvData->FileEnrollType;

        //
        // Clean up Certificate Format if File type is not X509 DER
        //
        if (IfrNvData->FileEnrollType != X509_CERT_FILE_TYPE) {
          IfrNvData->CertificateFormat = HASHALG_RAW;
        }
        DEBUG((DEBUG_ERROR, "IfrNvData->FileEnrollType %d\n", Private->FileContext->FileType));
      }

      break;

    case SECUREBOOT_ENROLL_SIGNATURE_TO_DBT:
      ChooseFile (NULL, NULL, UpdateDBTFromFile, &File);
      break;

    case KEY_SECURE_BOOT_DELETE_PK:
      if (Value->u8) {
        StringInfo = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_DISABLEDELETE_NAME_FORMAT), NULL);
        String = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_DISCARDRETURN_NAME_FORMAT), NULL);
        CreatePopUp (
          EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
          &Key,
          StringInfo,
          String,
          NULL
          );
          FreePool(StringInfo);
          FreePool(String);
        if (Key.UnicodeChar == 'y' || Key.UnicodeChar == 'Y') {
          Status = DeletePlatformKey ();
          if (EFI_ERROR (Status)) {
            StringInfo  = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_PHYSICALCUSTOM_NAME_FORMAT), NULL);
            CreatePopUp (
              EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
              &Key,
              StringInfo,
              NULL
              );
            FreePool(StringInfo);
            Value->u8 = 0;
          }else{
            PcdSet16S (PcdPlatformDefaultKey,0x0);
          }
        } else {
          Value->u8 = 0;
        }
      }
      break;

    case KEY_DELETE_KEK:
      UpdateDeletePage (
        Private,
        EFI_KEY_EXCHANGE_KEY_NAME,
        &gEfiGlobalVariableGuid,
        LABEL_KEK_DELETE,
        FORMID_DELETE_KEK_FORM,
        OPTION_DEL_KEK_QUESTION_ID
        );
      break;

    case SECUREBOOT_DELETE_SIGNATURE_FROM_DB:
      UpdateDeletePage (
        Private,
        EFI_IMAGE_SECURITY_DATABASE,
        &gEfiImageSecurityDatabaseGuid,
        LABEL_DB_DELETE,
        SECUREBOOT_DELETE_SIGNATURE_FROM_DB,
        OPTION_DEL_DB_QUESTION_ID
        );
       break;

    //
    // From DBX option to the level-1 form, display signature list.
    //
    case KEY_VALUE_FROM_DBX_TO_LIST_FORM:
      Private->VariableName = Variable_DBX;
      LoadSignatureList (
        Private,
        LABEL_SIGNATURE_LIST_START,
        SECUREBOOT_DELETE_SIGNATURE_LIST_FORM,
        OPTION_SIGNATURE_LIST_QUESTION_ID
      );
      break;

      //
      // Delete all signature list and reload.
      //
    case KEY_SECURE_BOOT_DELETE_ALL_LIST:
      StringInfo = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_DELETELIST_NAME_FORMAT), NULL);
      String = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_CANCELKEY_NAME_FORMAT), NULL);
      CreatePopUp(
        EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
        &Key,
        StringInfo,
        String,
        NULL
      );
      FreePool(StringInfo);
      FreePool(String);
      if (Key.UnicodeChar == L'Y' || Key.UnicodeChar == L'y') {
        DeleteSignatureEx (Private, Delete_Signature_List_All, IfrNvData->CheckedDataCount);
      }

      LoadSignatureList (
        Private,
        LABEL_SIGNATURE_LIST_START,
        SECUREBOOT_DELETE_SIGNATURE_LIST_FORM,
        OPTION_SIGNATURE_LIST_QUESTION_ID
      );
      break;

      //
      // Delete one signature list and reload.
      //
    case KEY_SECURE_BOOT_DELETE_ALL_DATA:
      StringInfo = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_PRESSSIGNATURE_NAME_FORMAT), NULL);
      String = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_CANCELKEY_NAME_FORMAT), NULL);
      CreatePopUp(
        EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
        &Key,
        StringInfo,
        String,
        NULL
      );
      FreePool(StringInfo);
      FreePool(String);
      if (Key.UnicodeChar == L'Y' || Key.UnicodeChar == L'y') {
        DeleteSignatureEx (Private, Delete_Signature_List_One, IfrNvData->CheckedDataCount);
      }

      LoadSignatureList (
        Private,
        LABEL_SIGNATURE_LIST_START,
        SECUREBOOT_DELETE_SIGNATURE_LIST_FORM,
        OPTION_SIGNATURE_LIST_QUESTION_ID
      );
      break;

      //
      // Delete checked signature data and reload.
      //
    case KEY_SECURE_BOOT_DELETE_CHECK_DATA:
      StringInfo = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_PRESSSIGNATURE_NAME_FORMAT), NULL);
      String = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_CANCELKEY_NAME_FORMAT), NULL);
      CreatePopUp(
        EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
        &Key,
        StringInfo,
        String,
        NULL
      );
      FreePool(StringInfo);
      FreePool(String);
      if (Key.UnicodeChar == L'Y' || Key.UnicodeChar == L'y') {
        DeleteSignatureEx (Private, Delete_Signature_Data, IfrNvData->CheckedDataCount);
      }

      LoadSignatureList (
        Private,
        LABEL_SIGNATURE_LIST_START,
        SECUREBOOT_DELETE_SIGNATURE_LIST_FORM,
        OPTION_SIGNATURE_LIST_QUESTION_ID
      );
      break;

    case SECUREBOOT_DELETE_SIGNATURE_FROM_DBT:
      UpdateDeletePage (
        Private,
        EFI_IMAGE_SECURITY_DATABASE2,
        &gEfiImageSecurityDatabaseGuid,
        LABEL_DBT_DELETE,
        SECUREBOOT_DELETE_SIGNATURE_FROM_DBT,
        OPTION_DEL_DBT_QUESTION_ID
        );

      break;

    case KEY_VALUE_SAVE_AND_EXIT_KEK:
      Status = EnrollKeyExchangeKey (Private,&EnrollKeyErrorCode);
      if (EFI_ERROR (Status)) {
        if (EnrollKeyErrorCode != None_Error ) {
          StringInfo = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_ERRORFILE_NAME_FORMAT), NULL);
          String = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_DERENCODED_NAME_FORMAT), NULL);
          CreatePopUp (
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
            &Key,
            StringInfo,
            String,
            NULL
            );
          FreePool(StringInfo);
          FreePool(String);
        } else if (EnrollKeyErrorCode == None_Error){
          StringInfo= HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_FAILED_NAME_FORMAT), NULL);
          String = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_NO_SIGNATURE_PROMPT_FORMAT), NULL);
          CreatePopUp (
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
            &Key,
            StringInfo,
            String,
            NULL
            );
          FreePool(StringInfo);
          FreePool(String);
        }        
      }
      break;

    case KEY_VALUE_SAVE_AND_EXIT_DB:
      Status = EnrollSignatureDatabase (Private, EFI_IMAGE_SECURITY_DATABASE,&EnrollKeyErrorCode);
      if (EFI_ERROR (Status)) {
        if (EnrollKeyErrorCode != None_Error ) {
          StringInfo = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_ERRORFILE_NAME_FORMAT), NULL);
          String = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_EXECUTABLECODE_NAME_FORMAT), NULL);
          CreatePopUp (
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
            &Key,
            StringInfo,
            String,
            NULL
            );
          FreePool(StringInfo);
          FreePool(String);
        } else if (EnrollKeyErrorCode == None_Error){
          StringInfo= HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_FAILED_NAME_FORMAT), NULL);
          String = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_NO_SIGNATURE_PROMPT_FORMAT), NULL);
          CreatePopUp (
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
            &Key,
            StringInfo,
            String,
            NULL
            );
          FreePool(StringInfo);
          FreePool(String);
        }        
      }

      break;
   
   case KEY_VALUE_SAVE_AND_EXIT_DBX:
      if (IsX509CertInDbx (Private, EFI_IMAGE_SECURITY_DATABASE1)) {
        StringInfo = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_ENROLLMENT_NAME_FORMAT), NULL);
        CreatePopUp (
          EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
          &Key,
          StringInfo,
          NULL
          );
         FreePool(StringInfo);
        //
        // Cert already exists in DBX. Close opened file before exit.
        //
        CloseEnrolledFile(Private->FileContext);
        break;
      }

      if ((IfrNvData != NULL) && (IfrNvData->CertificateFormat < HASHALG_MAX)) {
        Status = EnrollX509HashtoSigDB (
                   Private,
                   IfrNvData->CertificateFormat,
                   &IfrNvData->RevocationDate,
                   &IfrNvData->RevocationTime,
                   IfrNvData->AlwaysRevocation
                   );
        IfrNvData->CertificateFormat = HASHALG_RAW;
        //
        // If the Revocation time is modified, the modified time is assigned to the global variable
        //
        mDate.Year   = IfrNvData->RevocationDate.Year;
        mDate.Month  = IfrNvData->RevocationDate.Month;
        mDate.Day    = IfrNvData->RevocationDate.Day;
        mTime.Hour   = IfrNvData->RevocationTime.Hour;
        mTime.Minute = IfrNvData->RevocationTime.Minute;
      } else {
        Status = EnrollSignatureDatabase (Private, EFI_IMAGE_SECURITY_DATABASE1,&EnrollKeyErrorCode);
      }

      if (EFI_ERROR (Status)) {
        if (EnrollKeyErrorCode != None_Error ) {
          StringInfo = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_AUTHCERFICATE_NAME_FORMAT), NULL);
          String = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_ERRORFILE_NAME_FORMAT), NULL);
          CreatePopUp (
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
            &Key,
            StringInfo,
            String,
            NULL
            );
          FreePool(StringInfo);
          FreePool(String);
        } else if (EnrollKeyErrorCode == None_Error){
          StringInfo= HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_FAILED_NAME_FORMAT), NULL);
          String = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_NO_SIGNATURE_PROMPT_FORMAT), NULL);
          CreatePopUp (
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
            &Key,
            StringInfo,
            String,
            NULL
            );
          FreePool(StringInfo);
          FreePool(String);
        }        
      }

      break;

    case KEY_VALUE_SAVE_AND_EXIT_DBT:
      Status = EnrollSignatureDatabase (Private, EFI_IMAGE_SECURITY_DATABASE2,&EnrollKeyErrorCode);
      if (EFI_ERROR (Status)) {
        if (EnrollKeyErrorCode != None_Error ) {
          StringInfo = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_ERRORFILE_NAME_FORMAT), NULL);
          String = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_DERENCODED_NAME_FORMAT), NULL);
          CreatePopUp (
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
            &Key,
            StringInfo,
            String,
            NULL
            );
          FreePool(StringInfo);
          FreePool(String);
        } else if (EnrollKeyErrorCode == None_Error){
          StringInfo= HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_FAILED_NAME_FORMAT), NULL);
          String = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_NO_SIGNATURE_PROMPT_FORMAT), NULL);
          CreatePopUp (
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
            &Key,
            StringInfo,
            String,
            NULL
            );
          FreePool(StringInfo);
          FreePool(String);
        }        
      }
      break;
    case KEY_VALUE_SAVE_AND_EXIT_PK:
      //
      // Check the suffix, encode type and the key strength of PK certificate.
      //
      Status = CheckX509Certificate (Private->FileContext, &EnrollKeyErrorCode);
      switch(EnrollKeyErrorCode){
        case Unsupported_Type:
            StringInfo = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_ERRORFILE_NAME_FORMAT), NULL);
            String     = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_ONLYDER_NAME_FORMAT), NULL);
            break;
        case Unqualified_Key:
            StringInfo= HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_CERFICATEERROR_NAME_FORMAT), NULL);
            String = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_PUBLICKEYLENGTH_NAME_FORMAT), NULL);
            break;
        case None_Error:
            StringInfo= HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_FAILED_NAME_FORMAT), NULL);
            String = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_NO_SIGNATURE_PROMPT_FORMAT), NULL);
            break;
        default:
            StringInfo = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_NULL), NULL);
            String = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_NULL), NULL);
            break;
      }
      if (EFI_ERROR (Status)) {
        if (EnrollKeyErrorCode != None_Error && EnrollKeyErrorCode < Enroll_Error_Max) {
          CreatePopUp (
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
            &Key,
            StringInfo,
            String,
            NULL
            );
          FreePool(StringInfo);
          FreePool(String);
          break;
        } else if (EnrollKeyErrorCode == None_Error){
          CreatePopUp (
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
            &Key,
            StringInfo,
            String,
            NULL
            );
          FreePool(StringInfo);
          FreePool(String);
          break;
        }
      } else {
        Status = EnrollPlatformKey (Private);

        ExtractSecureBootModeFromVariable(&IfrNvData->CurSecureBootMode);
        ExtractSecureBootStatusFromVariable(&IfrNvData->CurSecureBootStatus);
        if (IfrNvData->CurSecureBootMode == SECURE_BOOT_MODE_USER_MODE || IfrNvData->CurSecureBootMode == SECURE_BOOT_MODE_DEPLOYED_MODE) {
          IfrNvData->DeletePk = FALSE;
          IfrNvData->HasPk    = TRUE;
          IfrNvData->HideSecureBoot = FALSE;
          *ActionRequest = EFI_BROWSER_ACTION_REQUEST_FORM_APPLY;
        } else  {
          IfrNvData->DeletePk = TRUE;
          IfrNvData->HasPk    = FALSE;
          IfrNvData->HideSecureBoot = TRUE;
          *ActionRequest = EFI_BROWSER_ACTION_REQUEST_FORM_APPLY;
        }
      }
      if (EFI_ERROR (Status)) {
        UnicodeSPrint (
          PromptString,
          sizeof (PromptString),
          L"Error status: %x.",
          Status
          );
        StringInfo = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_FAILED_NAME_FORMAT), NULL);
        CreatePopUp (
          EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
          &Key,
          StringInfo,
          PromptString,
          NULL
          );
        FreePool(StringInfo);
      }else{
        LibNotifySetupItemChanged(gBS, ITEM_SECUREBOOT, STATUS_ENABLE);
      }
      break;

    default:
      if ((QuestionId >= OPTION_DEL_KEK_QUESTION_ID) &&
                 (QuestionId < (OPTION_DEL_KEK_QUESTION_ID + OPTION_CONFIG_RANGE))) {
        if (Value->u8) {
          StringInfo = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_DELETEKEK_NAME_FORMAT), NULL);
          String = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_CANCELKEY_NAME_FORMAT), NULL);
          CreatePopUp(
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
            &Key,
            StringInfo,
            String,
            NULL
            );
          FreePool(StringInfo);
          FreePool(String);
          if (Key.UnicodeChar == L'Y' || Key.UnicodeChar == L'y') {
            DeleteKeyExchangeKey (Private, QuestionId);
          } else {
            Value->u8 = 0;
          }
        }
      } else if ((QuestionId >= OPTION_DEL_DB_QUESTION_ID) &&
                 (QuestionId < (OPTION_DEL_DB_QUESTION_ID + OPTION_CONFIG_RANGE))) {
        if (Value->u8) {
          StringInfo = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_DELETEDB_NAME_FORMAT), NULL);
          String = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_CANCELKEY_NAME_FORMAT), NULL);
          CreatePopUp(
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
            &Key,
            StringInfo,
            String,
            NULL
            );
          FreePool(StringInfo);
          FreePool(String);
          if (Key.UnicodeChar == L'Y' || Key.UnicodeChar == L'y') {
            DeleteSignature (
              Private,
              EFI_IMAGE_SECURITY_DATABASE,
              &gEfiImageSecurityDatabaseGuid,
              LABEL_DB_DELETE,
              SECUREBOOT_DELETE_SIGNATURE_FROM_DB,
              OPTION_DEL_DB_QUESTION_ID,
              QuestionId - OPTION_DEL_DB_QUESTION_ID
              );
          } else {
            Value->u8 = 0;
          }
        } 
      } else if ((QuestionId >= OPTION_SIGNATURE_LIST_QUESTION_ID) &&
                 (QuestionId < (OPTION_SIGNATURE_LIST_QUESTION_ID + OPTION_CONFIG_RANGE))) {
        LoadSignatureData (
          Private,
          LABEL_SIGNATURE_DATA_START,
          SECUREBOOT_DELETE_SIGNATURE_DATA_FORM,
          OPTION_SIGNATURE_DATA_QUESTION_ID,
          QuestionId - OPTION_SIGNATURE_LIST_QUESTION_ID
        );
        Private->ListIndex = QuestionId - OPTION_SIGNATURE_LIST_QUESTION_ID;
      } else if ((QuestionId >= OPTION_SIGNATURE_DATA_QUESTION_ID) &&
                 (QuestionId < (OPTION_SIGNATURE_DATA_QUESTION_ID + OPTION_CONFIG_RANGE))) {
        if (Private->CheckArray[QuestionId - OPTION_SIGNATURE_DATA_QUESTION_ID]) {
          IfrNvData->CheckedDataCount--;
          Private->CheckArray[QuestionId - OPTION_SIGNATURE_DATA_QUESTION_ID] = FALSE;
        } else {
          IfrNvData->CheckedDataCount++;
          Private->CheckArray[QuestionId - OPTION_SIGNATURE_DATA_QUESTION_ID] = TRUE;
        }
      } else if ((QuestionId >= OPTION_DEL_DBT_QUESTION_ID) &&
                 (QuestionId < (OPTION_DEL_DBT_QUESTION_ID + OPTION_CONFIG_RANGE))) {
        if (Value->u8) {
          StringInfo = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_DELETEDBT_NAME_FORMAT), NULL);
          String = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_SIGNATURE_CANCELKEY_NAME_FORMAT), NULL);
          CreatePopUp(
            EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
            &Key,
            StringInfo,
            String,
            NULL
            );
          FreePool(StringInfo);
          FreePool(String);
          if (Key.UnicodeChar == L'Y' || Key.UnicodeChar == L'y') {
            DeleteSignature (
              Private,
              EFI_IMAGE_SECURITY_DATABASE2,
              &gEfiImageSecurityDatabaseGuid,
              LABEL_DBT_DELETE,
              SECUREBOOT_DELETE_SIGNATURE_FROM_DBT,
              OPTION_DEL_DBT_QUESTION_ID,
              QuestionId - OPTION_DEL_DBT_QUESTION_ID
              );
          } else {
            Value->u8 = 0;
          }
        } 
      }
      break;

    case KEY_VALUE_NO_SAVE_AND_EXIT_PK:
    case KEY_VALUE_NO_SAVE_AND_EXIT_KEK:
    case KEY_VALUE_NO_SAVE_AND_EXIT_DB:
    case KEY_VALUE_NO_SAVE_AND_EXIT_DBX:
    case KEY_VALUE_NO_SAVE_AND_EXIT_DBT:
      CloseEnrolledFile(Private->FileContext);

      if (Private->SignatureGUID != NULL) {
        FreePool (Private->SignatureGUID);
        Private->SignatureGUID = NULL;
      }
      break;
      
    case KEY_SECURE_BOOT_KEK_GUID:
    case KEY_SECURE_BOOT_SIGNATURE_GUID_DB:
    case KEY_SECURE_BOOT_SIGNATURE_GUID_DBX:
    case KEY_SECURE_BOOT_SIGNATURE_GUID_DBT:
      InputSignatureString = HiiGetString (Private->HiiHandle, Value->string, NULL);
      RStatus = StrToGuid (InputSignatureString, &InputSignatureGuid);
      if (RETURN_ERROR (RStatus) || (InputSignatureString[GUID_STRING_LENGTH] != L'\0')) {
        StringInfo = HiiGetString (Private->HiiHandle, STRING_TOKEN (STR_CORRECT_FORMAT), NULL);
        UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, L"", StringInfo, NULL);
        FreePool(StringInfo);
        Status = EFI_INVALID_PARAMETER;
        goto EXIT;
      }
    }
  } else if (Action == EFI_BROWSER_ACTION_CHANGED) {
    switch (QuestionId) {
    case RESTORE_FACTORY_KEY_SECURE_BOOT:
      mIsEnterSecureBootForm = FALSE;
      break;
    case SECURE_BOOT_ENABLE_DISABLE:
      mIsSelectedSecureBootModeForm = FALSE;
      if (mIsSecureBootModeChanged) {
        *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
      }
      mIsSecureBootModeChanged = FALSE;
      break;
    case KEY_SECURE_BOOT_KEK_GUID:
    case KEY_SECURE_BOOT_SIGNATURE_GUID_DB:
    case KEY_SECURE_BOOT_SIGNATURE_GUID_DBX:
    case KEY_SECURE_BOOT_SIGNATURE_GUID_DBT:
      ASSERT (Private->SignatureGUID != NULL);
      RStatus = StrToGuid (IfrNvData->SignatureGuid, Private->SignatureGUID);
      if (RETURN_ERROR (RStatus) || (IfrNvData->SignatureGuid[GUID_STRING_LENGTH] != L'\0')) {
        Status = EFI_INVALID_PARAMETER;
        goto EXIT;
      }

      *ActionRequest = EFI_BROWSER_ACTION_REQUEST_FORM_APPLY;
      break;
    case KEY_SECURE_BOOT_DELETE_PK:
      ExtractSecureBootModeFromVariable(&IfrNvData->CurSecureBootMode);
      ExtractSecureBootStatusFromVariable(&IfrNvData->CurSecureBootStatus);
      if (IfrNvData->CurSecureBootMode == SECURE_BOOT_MODE_USER_MODE || IfrNvData->CurSecureBootMode == SECURE_BOOT_MODE_DEPLOYED_MODE) {
        IfrNvData->DeletePk = FALSE;
        IfrNvData->HasPk    = TRUE;
        IfrNvData->HideSecureBoot = FALSE;
        *ActionRequest = EFI_BROWSER_ACTION_REQUEST_FORM_APPLY;
      } else  {
        IfrNvData->DeletePk = TRUE;
        IfrNvData->HasPk    = FALSE;
        IfrNvData->HideSecureBoot = TRUE;
        *ActionRequest = EFI_BROWSER_ACTION_REQUEST_FORM_APPLY;
      }
      break;
    default:
      break;
    }
  } else if (Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
    if (QuestionId == KEY_HIDE_SECURE_BOOT) {
      GetVariable2 (EFI_SECURE_BOOT_ENABLE_NAME, &gEfiSecureBootEnableDisableGuid, (VOID**)&SecureBootEnable, NULL);
      if (SecureBootEnable == NULL) {
        IfrNvData->HideSecureBoot = TRUE;
      } else {
        FreePool (SecureBootEnable);
        IfrNvData->HideSecureBoot = FALSE;
      }
      Value->b = IfrNvData->HideSecureBoot;
    }
  } else if (Action == EFI_BROWSER_ACTION_FORM_CLOSE) {
    //
    // Force the platform back to Standard Mode once user leave the setup screen.
    //
    GetVariable2 (EFI_CUSTOM_MODE_NAME, &gEfiCustomModeEnableGuid, (VOID**)&SecureBootMode, NULL);
    if (NULL != SecureBootMode && *SecureBootMode == CUSTOM_SECURE_BOOT_MODE) {
      IfrNvData->SecureBootMode = STANDARD_SECURE_BOOT_MODE;
      SetSecureBootMode(STANDARD_SECURE_BOOT_MODE);
    }
    if (SecureBootMode != NULL) {
      FreePool (SecureBootMode);
    }
    //
    // If no changes have been made, it is consistent with Form Open
    // If the time is changed, the time is the same as the modified time
    //
      IfrNvData->RevocationDate.Year   = mDate.Year;
      IfrNvData->RevocationDate.Month  = mDate.Month;
      IfrNvData->RevocationDate.Day    = mDate.Day;
      IfrNvData->RevocationTime.Hour   = mTime.Hour;
      IfrNvData->RevocationTime.Minute = mTime.Minute;
      //
      // Fix Problem:If setup no change,enter DBX delete Signature Option Will remind  modification
      //
      IfrNvData->ListCount = 0;

    if (QuestionId == KEY_SECURE_BOOT_DELETE_ALL_DATA || QuestionId == KEY_SECURE_BOOT_DELETE_CHECK_DATA) {
      //
      // Free memory when exit from the SECUREBOOT_DELETE_SIGNATURE_DATA_FORM form.
      //
      SECUREBOOT_FREE_NON_NULL (Private->CheckArray);
      IfrNvData->CheckedDataCount = 0;
    }
  }

  Status = EFI_SUCCESS;

EXIT:

  if (!EFI_ERROR (Status) && GetBrowserDataResult) {
    BufferSize = sizeof (SECUREBOOT_CONFIGURATION);
    HiiSetBrowserData (&gSecureBootConfigFormSetGuid, mSecureBootStorageName, BufferSize, (UINT8*) IfrNvData, NULL);
  }

  if(IfrNvData != NULL){
    FreePool (IfrNvData);
  }

  if (File != NULL){
    FreePool(File);
    File = NULL;
  }

  LibAuthVarSetPhysicalPresent(FALSE);

  return Status;
}


EFI_STATUS 
SetupItemSecureBootNotify(
  SETUP_ITEM_UPDATE_NOTIFY_PROTOCOL    *This,
  UINTN                                Item, 
  UINTN                                NewStatus
  )
{
  UINTN                           BufferSize;
  SECUREBOOT_CONFIGURATION        IfrNvData;

  if (mPrivateData == NULL) {
    return EFI_UNSUPPORTED;
  }

  if((Item == ITEM_CSM && NewStatus == STATUS_ENABLE) || 
     (Item == ITEM_OPTIMIZED_BOOT && NewStatus == STATUS_DISABLE)) {

    BufferSize = sizeof(SECUREBOOT_CONFIGURATION);
    HiiGetBrowserData(
            &gSecureBootConfigFormSetGuid, 
            mSecureBootStorageName, 
            BufferSize, 
            (UINT8*)&IfrNvData
            );
    if(IfrNvData.CurSecureBootStatus == SECURE_BOOT_STATUS_ENABLE){
      //
      // Enable CSM or turn off optimized boot, need to disable secure boot
      //
      LibAuthVarSetPhysicalPresent(TRUE);
      DeletePlatformKey ();
      PcdSet16S (PcdPlatformDefaultKey,0x0);
      UpdateSecureBootString(mPrivateData);
      SecureBootExtractConfigFromVariable (mPrivateData, &IfrNvData);
      HiiSetBrowserData(
              &gSecureBootConfigFormSetGuid, 
              mSecureBootStorageName, 
              BufferSize, 
              (UINT8*)&IfrNvData, 
              NULL
              );
      LibAuthVarSetPhysicalPresent(FALSE);
    }
  } else if (Item == ITEM_OPTIMIZED_BOOT && NewStatus == STATUS_ENABLE) {
    BufferSize = sizeof(SECUREBOOT_CONFIGURATION);
    HiiGetBrowserData(
            &gSecureBootConfigFormSetGuid, 
            mSecureBootStorageName, 
            BufferSize, 
            (UINT8*)&IfrNvData
            );
    if(IfrNvData.CurSecureBootStatus == SECURE_BOOT_STATUS_DISABLE){
      //
      // Optimized boot enable, need to enable secure boot, and open secure options.
      //
      LibAuthVarSetPhysicalPresent(TRUE);
      AuthVarMfgReset();
      UpdateSecureBootString(mPrivateData);
      SecureBootExtractConfigFromVariable (mPrivateData, &IfrNvData);
      HiiSetBrowserData(
              &gSecureBootConfigFormSetGuid, 
              mSecureBootStorageName, 
              BufferSize, 
              (UINT8*)&IfrNvData, 
              NULL
              );
      LibAuthVarSetPhysicalPresent(FALSE);
    }
  }

  return EFI_SUCCESS;
}

/**
  This function publish the SecureBoot configuration Form.

  @param[in, out]  PrivateData   Points to SecureBoot configuration private data.

  @retval EFI_SUCCESS            HII Form is installed successfully.
  @retval EFI_OUT_OF_RESOURCES   Not enough resource for HII Form installation.
  @retval Others                 Other errors as indicated.

**/
EFI_STATUS
InstallSecureBootConfigForm (
  IN OUT SECUREBOOT_CONFIG_PRIVATE_DATA  *PrivateData
  )
{
  EFI_STATUS                      Status;
  EFI_HII_HANDLE                  HiiHandle;
  EFI_HANDLE                      DriverHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *ConfigAccess;

  DriverHandle = NULL;
  ConfigAccess = &PrivateData->ConfigAccess;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mSecureBootHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  ConfigAccess,
                  &gSetupItemUpdateNotifyProtocolGuid,
                  &mSetupItemUpdateNotifyProtocol,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PrivateData->DriverHandle = DriverHandle;

  //
  // Publish the HII package list
  //
  HiiHandle = HiiAddPackages (
                &gSecureBootConfigFormSetGuid,
                DriverHandle,
                SecureBootConfigDxeStrings,
                SecureBootConfigBin,
                NULL
                );
  if (HiiHandle == NULL) {
    gBS->UninstallMultipleProtocolInterfaces (
           DriverHandle,
           &gEfiDevicePathProtocolGuid,
           &mSecureBootHiiVendorDevicePath,
           &gEfiHiiConfigAccessProtocolGuid,
           ConfigAccess,
           NULL
           );
    return EFI_OUT_OF_RESOURCES;
  }

  PrivateData->HiiHandle = HiiHandle;
  mPrivateData = PrivateData;

  PrivateData->FileContext = AllocateZeroPool (sizeof (SECUREBOOT_FILE_CONTEXT));

  if (PrivateData->FileContext == NULL) {
    UninstallSecureBootConfigForm (PrivateData);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Init OpCode Handle and Allocate space for creation of Buffer
  //
  mStartOpCodeHandle = HiiAllocateOpCodeHandle ();
  if (mStartOpCodeHandle == NULL) {
    UninstallSecureBootConfigForm (PrivateData);
    return EFI_OUT_OF_RESOURCES;
  }

  mEndOpCodeHandle = HiiAllocateOpCodeHandle ();
  if (mEndOpCodeHandle == NULL) {
    UninstallSecureBootConfigForm (PrivateData);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Create Hii Extend Label OpCode as the start opcode
  //
  mStartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
                                         mStartOpCodeHandle,
                                         &gEfiIfrTianoGuid,
                                         NULL,
                                         sizeof (EFI_IFR_GUID_LABEL)
                                         );
  mStartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;

  //
  // Create Hii Extend Label OpCode as the end opcode
  //
  mEndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
                                       mEndOpCodeHandle,
                                       &gEfiIfrTianoGuid,
                                       NULL,
                                       sizeof (EFI_IFR_GUID_LABEL)
                                       );
  mEndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  mEndLabel->Number       = LABEL_END;

  return EFI_SUCCESS;
}

/**
  This function removes SecureBoot configuration Form.

  @param[in, out]  PrivateData   Points to SecureBoot configuration private data.

**/
VOID
UninstallSecureBootConfigForm (
  IN OUT SECUREBOOT_CONFIG_PRIVATE_DATA    *PrivateData
  )
{
  //
  // Uninstall HII package list
  //
  if (PrivateData->HiiHandle != NULL) {
    HiiRemovePackages (PrivateData->HiiHandle);
    PrivateData->HiiHandle = NULL;
  }

  //
  // Uninstall HII Config Access Protocol
  //
  if (PrivateData->DriverHandle != NULL) {
    gBS->UninstallMultipleProtocolInterfaces (
           PrivateData->DriverHandle,
           &gEfiDevicePathProtocolGuid,
           &mSecureBootHiiVendorDevicePath,
           &gEfiHiiConfigAccessProtocolGuid,
           &PrivateData->ConfigAccess,
           NULL
           );
    PrivateData->DriverHandle = NULL;
  }

  if (PrivateData->SignatureGUID != NULL) {
    FreePool (PrivateData->SignatureGUID);
  }

  if (PrivateData->FileContext != NULL) {
    FreePool (PrivateData->FileContext);
  }

  FreePool (PrivateData);
  mPrivateData = NULL;

  if (mStartOpCodeHandle != NULL) {
    HiiFreeOpCodeHandle (mStartOpCodeHandle);
  }

  if (mEndOpCodeHandle != NULL) {
    HiiFreeOpCodeHandle (mEndOpCodeHandle);
  }
}
