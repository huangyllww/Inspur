/** @file
  SM2 Asymmetric Cipher Wrapper Implementation over OpenSSL.

  This file implements following APIs which provide basic capabilities for RSA:
  1) Sm2SetPublicKey()
  2) Sm2SetPrivateKey()
  3) Sm2Verify()
  4) Sm2Sign()
  5) Sm2VerifyDigest()
  6) Sm2SignDigest()

Copyright (c) 2022, Byosoft Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "InternalCryptLib.h"

#include <openssl/bn.h>
#include <openssl/ossl_typ.h>
#include <openssl/objects.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>
#include <crypto/sm2.h>
#include <crypto/sm2err.h>

#define RANDOM_KEY_SUPPORT

#ifdef RANDOM_KEY_SUPPORT

static RAND_METHOD        fake_rand;
static const RAND_METHOD  *saved_rand;

static uint8_t  *fake_rand_bytes       = NULL;
static size_t   fake_rand_bytes_offset = 0;
static size_t   fake_rand_size         = 0;

static int
get_faked_bytes (
  unsigned char  *buf,
  int            num
  )
{
  if (fake_rand_bytes == NULL) {
    return saved_rand->bytes (buf, num);
  }

  if (fake_rand_size == 0) {
    return 0;
  }

  while (num-- > 0) {
    if (fake_rand_bytes_offset >= fake_rand_size) {
      fake_rand_bytes_offset = 0;
    }

    *buf++ = fake_rand_bytes[fake_rand_bytes_offset++];
  }

  return 1;
}

static int
start_fake_rand (
  const char  *hex_bytes
  )
{
  /* save old rand method */
  saved_rand = RAND_get_rand_method ();
  if (saved_rand == NULL) {
    return 0;
  }

  fake_rand = *saved_rand;
  /* use own random function */
  fake_rand.bytes = get_faked_bytes;

  fake_rand_bytes        = OPENSSL_hexstr2buf (hex_bytes, NULL);
  fake_rand_bytes_offset = 0;
  fake_rand_size         = strlen (hex_bytes) / 2;

  /* set new RAND_METHOD */
  return RAND_set_rand_method (&fake_rand);
}

static int
restore_rand (
  void
  )
{
  OPENSSL_free (fake_rand_bytes);
  fake_rand_bytes        = NULL;
  fake_rand_bytes_offset = 0;
  return RAND_set_rand_method (saved_rand);
}

#define START_FAKE_RAND(K)  if((K) != NULL) { start_fake_rand((K)); }
#define END_FAKE_RAND(K)    if((K) != NULL) { restore_rand(); }

#else

#define START_FAKE_RAND(K)
#define END_FAKE_RAND(K)

#endif

/*
  For Debug to dump Memory content
*/
static
VOID
DumpData (
  UINT8  *Data,
  UINTN  DataSize
  )
{
  UINTN  Index;

  DEBUG ((DEBUG_INFO, "  "));
  for (Index = 0; Index < DataSize; Index++) {
    DEBUG ((DEBUG_INFO, "%02x ", Data[Index]));
    if ((Index & 0xF) == 0xF) {
      DEBUG ((DEBUG_INFO, "\n  "));
    }
  }

  DEBUG ((DEBUG_INFO, "\n  "));
}

/**
  Sets public key into the SM2 Key context.

  This function sets Public Key Part of SM2 key into the established key context

  @param[in, out]  Sm2Key      Pointer to Sm2 context being set.
  @param[in]       Px          Hex String of Public Key X.
  @param[in]       Py          Hex String of Public Key Y

  @retval  TRUE   SM2 Public key component was set successfully.
  @retval  FALSE  Invalid parameter or SM2 Public key component fail.

**/
BOOLEAN
EFIAPI
Sm2SetPublicKey (
  IN OUT  VOID  *Sm2Key,
  IN CHAR8      *Px,
  IN CHAR8      *Py
  )
{
  EC_KEY    *key      = (EC_KEY *)Sm2Key;
  EC_GROUP  *Sm2Group = NULL;
  BIGNUM    *X        = NULL;
  BIGNUM    *Y        = NULL;
  BOOLEAN   Result    = FALSE;

  if ((Px == NULL) || (Py == NULL)) {
    return FALSE;
  }

  //
  // Set SM2 public key
  //
  if ((BN_hex2bn (&X, Px) == 0) || (BN_hex2bn (&Y, Py) == 0)) {
    goto done;
  }

  //
  // Set Standard parameter which is defined in SM2 elliptic curve.
  // Reference spec
  //   <<Public key cryptographic algorithm SM2 based on elliptic curves - Part5:Parameter definition>>
  //
  Sm2Group = create_EC_group (
               "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF",
               "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC",
               "28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93",
               "32c4ae2c1f1981195f9904466a39c9948fe30bbff2660be1715a4589334c74c7",
               "bc3736a2f4f6779c59bdcee36b692153d0a9877cc62a474002df32e52139f0a0",
               "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123",
               "1"
               );
  if (Sm2Group == NULL) {
    goto done;
  }

  if (EC_KEY_set_group (key, Sm2Group) == 0) {
    goto done;
  }

  if (EC_KEY_set_public_key_affine_coordinates (key, X, Y) == 0) {
    goto done;
  }

  Result = TRUE;

done:
  if (!Result) {
    BN_free (X);
    BN_free (Y);
    EC_GROUP_free (Sm2Group);
  }

  return Result;
}

/**
  Sets private key into the SM2 Key context.

  This function sets Private Key Part of SM2 key into the established key context
  Note: When Verify SM2-based Signature, Sm2SetPrivateKey() is not required
        When signing with SM2 algo, SM2 Public Key is not needed, however
        ****we still need to call Sm2SetPublicKey to init EC Group*****

  @param[in, out]  Sm2Key      Pointer to SM2 context being set.
  @param[in]       PrivKey          Hex String of SM2 Private Key.

  @retval  TRUE   SM2 Private key component was set successfully.
  @retval  FALSE  Invalid parameter or SM2 Public key component fail.

**/
BOOLEAN
EFIAPI
Sm2SetPrivateKey (
  IN OUT   VOID   *Sm2Key,
  IN CONST CHAR8  *PrivKey
  )
{
  EC_KEY  *Key    = (EC_KEY *)Sm2Key;
  BIGNUM  *BnPriv = NULL;

  BN_hex2bn (&BnPriv, PrivKey);
  if (BnPriv == NULL) {
    return FALSE;
  }

  if (EC_KEY_set_private_key (Key, BnPriv) == 0) {
    BN_free (BnPriv);
    return FALSE;
  }

  return TRUE;
}

/**
  Verifies DER encoded SM3-with-SM2 ECDSA signature against Message by following  <<sm2 cryptography algorithm application specification>>
  Before verification, follow spec pre-process 2 to generate SM3 hash based on Message

  If Sm2Key is NULL, then return FALSE.
  If Message is NULL, then return FALSE.
  If Sig is NULL, then return FALSE.

  @param[in]  Sm2Key       Pointer to Sm2 key for signature verification.
  @param[in]  Message      Pointer to octet message to be checked.
  @param[in]  MessageSize  Size of the message in bytes.
  @param[in]  Sig          Pointer to **DER-encoded** SM3-with-SM2 ECDSA signature to be verified.
  @param[in]  SigSize      Size of signature in bytes.

  @retval  TRUE   Valid signature encoded in PKCS1-v1_5.
  @retval  FALSE  Invalid signature or invalid RSA context.

**/
BOOLEAN
EFIAPI
Sm2Verify (
  IN  VOID         *Sm2Key,
  IN  CONST CHAR8  *userid       OPTIONAL,
  IN  CONST UINT8  *Message,
  IN  UINTN        MessageSize,
  IN  CONST UINT8  *Sig,
  IN  UINTN        SigSize
  )
{
  EC_KEY     *key = (EC_KEY *)Sm2Key;
  ECDSA_SIG  *Sm2Sig;
  BOOLEAN    ok;
  BIGNUM     *r, *s;
  UINT8      Bin[256];
  UINT32     size;
  UINTN      UidLen;

  if ((Sm2Key == NULL) || (Message == NULL) || (Sig == NULL)) {
    return FALSE;
  }

  ok = FALSE;

  Sm2Sig = ECDSA_SIG_new ();
  if (Sm2Sig == NULL) {
    goto done;
  }

  if (d2i_ECDSA_SIG (&Sm2Sig, &Sig, (int)SigSize) == NULL) {
    goto done;
  }

  ECDSA_SIG_get0 (Sm2Sig, (const BIGNUM **)&r, (const BIGNUM **)&s);

  DEBUG_CODE (
    size = BN_bn2bin ((const BIGNUM *)r, Bin);
    DumpData (Bin, size);

    size = BN_bn2bin ((const BIGNUM *)s, Bin);
    DumpData (Bin, size);
    );

  UidLen = ((userid == NULL) ? 0 : AsciiStrLen (userid));
  if (sm2_do_verify (
        key,
        EVP_sm3 (),
        Sm2Sig,
        (const uint8_t *)userid,
        UidLen,
        (const uint8_t *)Message,
        MessageSize
        ) != 0)
  {
    ok = TRUE;
  }

done:
  ECDSA_SIG_free (Sm2Sig);
  return ok;
}

/**
  Generate DER encoded SM3-with-SM2 signature on message by following <<sm2 cryptography algorithm application specification>>
  Before signing, follow pre-process 2 to generate SM3 hash based on given Message

  If Sm2Key is NULL, then return FALSE.
  If userid is NULL, then return FALSE.
  If Signature is NULL, then return FALSE.

  @param[in]  Sm2Key       Pointer to Sm2 Key context for signature.
  @param[in]  userid       Pointer to user id to generate Za during SM3 hash caculation
  @param[in]  K            Pointer to Hex String Random Value in SM2 signing
  @param[in]  Message      Pointer to octet message to be checked.
  @param[in]  MessageSize  Size of the message in bytes.
  @param[in]  Sig          Pointer to data buffer to hold **DER-encoded** SM3-with-SM2 ECDSA signature.
  @param[in]  SigSize      Input:  data buffer size.
                           Output: size of SM3-with-SM2 signature.

  @retval  TRUE   Valid signature encoded in SM3-with-SM2.
  @retval  FALSE  Invalid signature.

**/
BOOLEAN
EFIAPI
Sm2Sign (
  IN  VOID         *Sm2Key,
  IN  const CHAR8  *userid  OPTIONAL,
  IN  const CHAR8  *K       OPTIONAL,
  IN  CONST UINT8  *Message,
  IN  UINTN        MessageSize,
  OUT UINT8        *Sig,
  IN OUT UINTN     *SigSize
  )
{
  EC_KEY     *key = (EC_KEY *)Sm2Key;
  ECDSA_SIG  *Sm2Sig;
  BOOLEAN    ok;
  UINT8      *DataBuf = NULL;
  UINTN      DataSize;
  UINTN      uidlen;

  if ((Sm2Key == NULL) || (Message == NULL) || (Sig == NULL) || (SigSize == NULL)) {
    return FALSE;
  }

  ok = FALSE;

  uidlen = ((userid == NULL) ? 0 : AsciiStrLen (userid));

  START_FAKE_RAND (K);
  Sm2Sig = sm2_do_sign (
             key,
             EVP_sm3 (),
             (const uint8_t *)userid,
             uidlen,
             (const uint8_t *)Message,
             MessageSize
             );
  END_FAKE_RAND (K);

  if (Sm2Sig == NULL) {
    goto done;
  }

  DataSize = i2d_ECDSA_SIG ((const ECDSA_SIG *)Sm2Sig, &DataBuf);
  if (DataSize > *SigSize) {
    *SigSize = DataSize;
    goto done;
  }

  CopyMem (Sig, DataBuf, DataSize);
  *SigSize = DataSize;

  ok = TRUE;

done:
  OPENSSL_free (DataBuf);
  ECDSA_SIG_free (Sm2Sig);
  return ok;
}

/*
  This function verifies SM2 Signature with input Message Digest.
SM2 Signature must be DER-encoded.
  Unlike verifying SM3-with-SM2 siganture inside Sm2Verify(), Sm2VerifyDigest()
provides digest algorithm agility by verifying Message Digest generated from external


  @param[in]  Sm2Key       Pointer to Sm2 Key context for signature.
  @param[in]  Digest       Pointer to digest to be signed with SM2 algorithm
  @param[in]  DigestSize   Size of Digest
  @param[in]  Sig          Pointer to **DER-encoded** SM3-with-SM2 ECDSA signature.
  @param[in]  SigSize      Input:  data buffer size.
                           Output: size of SM2 signature.

  @retval  TRUE   Valid signature encoded in Specific-Hash-Algo-with-SM2.
  @retval  FALSE  Invalid signature.
*/
BOOLEAN
EFIAPI
Sm2VerifyDigest (
  IN  VOID         *Sm2Key,
  IN  CONST UINT8  *Digest,
  IN  UINTN        DigestSize,
  IN  CONST UINT8  *Sig,
  IN  UINTN        SigSize
  )
{
  BOOLEAN  ok = FALSE;

  if ((Sm2Key == NULL) || (Digest == NULL) || (Sig == NULL)) {
    return FALSE;
  }

  if (sm2_verify (
        (const unsigned char *)Digest,
        (int)DigestSize,
        (const unsigned char *)Sig,
        (int)SigSize,
        (EC_KEY *)Sm2Key
        ) != 0)
  {
    ok = TRUE;
  }

  return ok;
}

/*
  This function generates SM2 Signature out of given Message Digest.
SM2 Signature returned is encoded in DER form.
  Unlike  generating Message SM3 digest inside Sm2Sign(), Sm2SignDigest()
provides agility in digest algorithm by signing Message Digest from external

  Note: Random K isn't supported by now.

  @param[in]  Sm2Key       Pointer to Sm2 Key context for signature.
  @param[in]  Digest       Pointer to digest to be signed with SM2 algorithm
  @param[in]  DigestSize   Size of Digest
  @param[in]  Sig          Pointer to **DER-encoded** SM3-with-SM2 ECDSA signature.
  @param[in]  SigSize      Input:  data buffer size.
                           Output: size of SM2 signature.

  @retval  TRUE   Valid signature encoded in Specific-Hash-Algo-with-SM2.
  @retval  FALSE  Invalid signature.

*/
BOOLEAN
EFIAPI
Sm2SignDigest (
  IN  VOID         *Sm2Key,
  IN  CONST UINT8  *Digest,
  IN  UINTN        DigestSize,
  OUT UINT8        *Sig,
  IN OUT UINTN     *SigSize
  )
{
  UINT8    DataBuf[256];
  UINTN    DataSize;
  INTN     Ret;
  BOOLEAN  ok = FALSE;

  if ((Sm2Key == NULL) || (Digest == NULL) || (Sig == NULL) || (SigSize == NULL)) {
    return FALSE;
  }

  DataSize = sizeof (DataBuf);
  Ret      = sm2_sign (
               (const unsigned char *)Digest,
               (int)DigestSize,
               DataBuf,
               (unsigned int *)&DataSize,
               (EC_KEY *)Sm2Key
               );

  if (Ret != 0) {
    if (DataSize > *SigSize) {
      *SigSize = DataSize;
      goto done;
    }

    CopyMem (Sig, DataBuf, DataSize);
    *SigSize = DataSize;
    ok       = TRUE;
  }

done:
  return ok;
}
