#ifndef __CRYPTO_LIB_DXE_PROTOCOL_H__
#define __CRYPTO_LIB_DXE_PROTOCOL_H__

#include <Base.h>

typedef
UINTN
(EFIAPI *SHA256_GET_CTX_SIZE) (
  VOID
  );

typedef
BOOLEAN
(EFIAPI *SHA256_INIT) (
  OUT  VOID  *Sha256Context
  );

typedef
BOOLEAN
(EFIAPI *SHA256_UPDATE) (
  IN OUT  VOID        *Sha256Context,
  IN      CONST VOID  *Data,
  IN      UINTN       DataSize
  );

typedef  
BOOLEAN
(EFIAPI *SHA256_FINAL) (
  IN OUT  VOID   *Sha256Context,
  OUT     UINT8  *HashValue
  );  

typedef  
BOOLEAN
(EFIAPI *SHA256) (
  IN      VOID     *DataIn, 
  IN      UINTN    DataSize, 
  IN OUT  UINT8    *HashValue
  );  

typedef  
VOID *
(EFIAPI *RSA_NEW) (
  VOID
  );  


typedef enum {
  LibRsaKeyN,      ///< RSA public Modulus (N)
  LibRsaKeyE,      ///< RSA Public exponent (e)
  LibRsaKeyD,      ///< RSA Private exponent (d)
} LIB_RSA_KEY_TAG;  
  
typedef  
BOOLEAN
(EFIAPI *RSA_SET_KEY) (
  IN OUT  VOID             *RsaContext,
  IN      LIB_RSA_KEY_TAG  KeyTag,
  IN      CONST UINT8      *BigNumber,
  IN      UINTN            BnSize
  );  

typedef  
BOOLEAN
(EFIAPI *RSA_PKCS1_VERIFY) (
  IN  VOID         *RsaContext,
  IN  CONST UINT8  *MessageHash,
  IN  UINTN        HashSize,
  IN  CONST UINT8  *Signature,
  IN  UINTN        SigSize
  );

typedef  
VOID
(EFIAPI *RSA_FREE)(
  IN  VOID  *RsaContext
  );  
  
typedef  
BOOLEAN
(EFIAPI *DATA_RSA_PKCS1_VERIFY) (
  IN VOID     *Data,
  IN UINTN    DataSize,
  IN VOID     *SignData,
  IN UINTN    SignDataSize,
  IN VOID     *KeyN,
  IN UINTN    KeyNSize,
  IN VOID     *KeyE,
  IN UINTN    KeyESize
  );


#define CRYPTO_LIB_VERSION   0x00010000
  

typedef struct {
  UINTN                      Version;
  SHA256_GET_CTX_SIZE        Sha256GetContextSize;
  SHA256_INIT                Sha256Init;
  SHA256_UPDATE              Sha256Update;
  SHA256_FINAL               Sha256Final;
  SHA256                     Sha256;
  RSA_NEW                    RsaNew;
  RSA_SET_KEY                RsaSetKey;
  RSA_PKCS1_VERIFY           RsaPkcs1Verify;
  RSA_FREE                   RsaFree;
  DATA_RSA_PKCS1_VERIFY      DataRsaPkcs1Verify;
} CRYPTO_LIB_PROTOCOL;  
 

extern EFI_GUID gCryptoLibDxeProtocolGuid;
extern EFI_GUID gCryptoLibSmmProtocolGuid;
extern EFI_GUID gCryptoLibPpiGuid;
 
#endif

