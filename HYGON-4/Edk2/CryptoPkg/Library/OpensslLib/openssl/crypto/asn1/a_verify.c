/*
 * Copyright 1995-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include <time.h>
#include <sys/types.h>

#include "internal/cryptlib.h"

#include <openssl/bn.h>
#include <openssl/x509.h>
#include <openssl/objects.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include "crypto/asn1.h"
#include "crypto/evp.h"

#ifndef NO_ASN1_OLD

int ASN1_verify(i2d_of_void *i2d, X509_ALGOR *a, ASN1_BIT_STRING *signature,
                char *data, EVP_PKEY *pkey)
{
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    const EVP_MD *type;
    unsigned char *p, *buf_in = NULL;
    int ret = -1, i, inl;

    if (ctx == NULL) {
        ASN1err(ASN1_F_ASN1_VERIFY, ERR_R_MALLOC_FAILURE);
        goto err;
    }
    i = OBJ_obj2nid(a->algorithm);
    type = EVP_get_digestbyname(OBJ_nid2sn(i));
    if (type == NULL) {
        ASN1err(ASN1_F_ASN1_VERIFY, ASN1_R_UNKNOWN_MESSAGE_DIGEST_ALGORITHM);
        goto err;
    }

    if (signature->type == V_ASN1_BIT_STRING && signature->flags & 0x7) {
        ASN1err(ASN1_F_ASN1_VERIFY, ASN1_R_INVALID_BIT_STRING_BITS_LEFT);
        goto err;
    }

    inl = i2d(data, NULL);
    if (inl <= 0) {
        ASN1err(ASN1_F_ASN1_VERIFY, ERR_R_INTERNAL_ERROR);
        goto err;
    }
    buf_in = OPENSSL_malloc((unsigned int)inl);
    if (buf_in == NULL) {
        ASN1err(ASN1_F_ASN1_VERIFY, ERR_R_MALLOC_FAILURE);
        goto err;
    }
    p = buf_in;

    i2d(data, &p);
    ret = EVP_VerifyInit_ex(ctx, type, NULL)
        && EVP_VerifyUpdate(ctx, (unsigned char *)buf_in, inl);

    OPENSSL_clear_free(buf_in, (unsigned int)inl);

    if (!ret) {
        ASN1err(ASN1_F_ASN1_VERIFY, ERR_R_EVP_LIB);
        goto err;
    }
    ret = -1;

    if (EVP_VerifyFinal(ctx, (unsigned char *)signature->data,
                        (unsigned int)signature->length, pkey) <= 0) {
        ASN1err(ASN1_F_ASN1_VERIFY, ERR_R_EVP_LIB);
        ret = 0;
        goto err;
    }
    ret = 1;
 err:
    EVP_MD_CTX_free(ctx);
    return ret;
}

#endif

int ASN1_item_verify(const ASN1_ITEM *it, X509_ALGOR *a,
                     ASN1_BIT_STRING *signature, void *asn, EVP_PKEY *pkey)
{
    EVP_MD_CTX *ctx = NULL;
    unsigned char *buf_in = NULL;
    int ret = -1, inl = 0;
    int mdnid, pknid;
    size_t inll = 0;

    if (!pkey) {
        ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ERR_R_PASSED_NULL_PARAMETER);
        return -1;
    }

    if (signature->type == V_ASN1_BIT_STRING && signature->flags & 0x7) {
        ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ASN1_R_INVALID_BIT_STRING_BITS_LEFT);
        return -1;
    }

    ctx = EVP_MD_CTX_new();
    if (ctx == NULL) {
        ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ERR_R_MALLOC_FAILURE);
        goto err;
    }

    /* Convert signature OID into digest and public key OIDs */
    if (!OBJ_find_sigid_algs(OBJ_obj2nid(a->algorithm), &mdnid, &pknid)) {
        ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ASN1_R_UNKNOWN_SIGNATURE_ALGORITHM);
        goto err;
    }
    if (mdnid == NID_undef) {
        if (!pkey->ameth || !pkey->ameth->item_verify) {
            ASN1err(ASN1_F_ASN1_ITEM_VERIFY,
                    ASN1_R_UNKNOWN_SIGNATURE_ALGORITHM);
            goto err;
        }
        ret = pkey->ameth->item_verify(ctx, it, asn, a, signature, pkey);
        /*
         * Return value of 2 means carry on, anything else means we exit
         * straight away: either a fatal error of the underlying verification
         * routine handles all verification.
         */
        if (ret != 2)
            goto err;
        ret = -1;
    } else {
        const EVP_MD *type = EVP_get_digestbynid(mdnid);

        if (type == NULL) {
            ASN1err(ASN1_F_ASN1_ITEM_VERIFY,
                    ASN1_R_UNKNOWN_MESSAGE_DIGEST_ALGORITHM);
            goto err;
        }

        /* Check public key OID matches public key type */
        if (EVP_PKEY_type(pknid) != pkey->ameth->pkey_id) {
            ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ASN1_R_WRONG_PUBLIC_KEY_TYPE);
            goto err;
        }

        if (!EVP_DigestVerifyInit(ctx, NULL, type, NULL, pkey)) {
            ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ERR_R_EVP_LIB);
            ret = 0;
            goto err;
        }

    }

    inl = ASN1_item_i2d(asn, &buf_in, it);
    if (inl <= 0) {
        ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ERR_R_INTERNAL_ERROR);
        goto err;
    }
    if (buf_in == NULL) {
        ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ERR_R_MALLOC_FAILURE);
        goto err;
    }
    inll = inl;

    ret = EVP_DigestVerify(ctx, signature->data, (size_t)signature->length,
                           buf_in, inl);
    if (ret <= 0) {
        ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ERR_R_EVP_LIB);
        goto err;
    }
    ret = 1;
 err:
    OPENSSL_clear_free(buf_in, inll);
    EVP_MD_CTX_free(ctx);
    return ret;
}

#ifndef OPENSSL_NO_SM2  // Support SM2 GMT algoritm only when SM2 is enabled

int ASN1_item_verify_sm2(const ASN1_ITEM *it, X509_ALGOR *a,
                     ASN1_BIT_STRING *signature, void *asn, EVP_PKEY *pkey)
{
    EVP_MD_CTX *ctx = NULL;
    unsigned char *buf_in = NULL;
    int ret = -1, inl = 0;
    int mdnid, pknid;
    size_t inll = 0;
    EVP_MD_CTX *za_ctx = NULL;    
    unsigned char *za = NULL;
    int za_size;

    if (!pkey) {
        ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ERR_R_PASSED_NULL_PARAMETER);
        return -1;
    }

    if (signature->type == V_ASN1_BIT_STRING && signature->flags & 0x7) {
        ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ASN1_R_INVALID_BIT_STRING_BITS_LEFT);
        return -1;
    }

    ctx = EVP_MD_CTX_new();
    if (ctx == NULL) {
        ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ERR_R_MALLOC_FAILURE);
        goto err;
    }

    /* Convert signature OID into digest and public key OIDs */
    if (!OBJ_find_sigid_algs(OBJ_obj2nid(a->algorithm), &mdnid, &pknid)) {
        ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ASN1_R_UNKNOWN_SIGNATURE_ALGORITHM);
        goto err;
    }
    if (mdnid == NID_undef) {
        if (!pkey->ameth || !pkey->ameth->item_verify) {
            ASN1err(ASN1_F_ASN1_ITEM_VERIFY,
                    ASN1_R_UNKNOWN_SIGNATURE_ALGORITHM);
            goto err;
        }
        ret = pkey->ameth->item_verify(ctx, it, asn, a, signature, pkey);
        /*
         * Return value of 2 means carry on, anything else means we exit
         * straight away: either a fatal error of the underlying verification
         * routine handles all verification.
         */
        if (ret != 2)
            goto err;
        ret = -1;
    } else {
        const EVP_MD *type = EVP_get_digestbynid(mdnid);

        if (type == NULL) {
            ASN1err(ASN1_F_ASN1_ITEM_VERIFY,
                    ASN1_R_UNKNOWN_MESSAGE_DIGEST_ALGORITHM);
            goto err;
        }

        /* Check public key OID matches public key type */
        if (EVP_PKEY_type(pknid) != pkey->ameth->pkey_id) {
            ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ASN1_R_WRONG_PUBLIC_KEY_TYPE);
            goto err;
        }

        if (!EVP_DigestVerifyInit(ctx, NULL, type, NULL, pkey)) {
            ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ERR_R_EVP_LIB);
            ret = 0;
            goto err;
        }

        za_ctx = EVP_MD_CTX_new();
        if (za_ctx == NULL) {
            ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ERR_R_MALLOC_FAILURE);
            goto err;
        }
        EVP_DigestInit(za_ctx, type);

        //
        // Allocate Za buffer, same size of mdsize
        //
        za_size = EVP_MD_meth_get_result_size(type);
        if ((za = OPENSSL_malloc(za_size)) == NULL) {
             PKCS7err(ASN1_F_ASN1_ITEM_VERIFY, ERR_R_MALLOC_FAILURE);
             goto err;
        }

        //
        //  Wheen using Jit EC SM2 X509 to verify signature
        //  Generate signing Message like this (Jit(Jida))
        //  Za = SM3 (ENTL || IDa || a || b || Xg || Yg || Xa || Ya)
        //  M' = Za || VerifyAttribute , 
        //  while e = SM3 (M') is the final message to get signed
        //
        //
        ZeroMem(za, za_size);

        //
        // 1. Za = SM3 (ENTL || IDa || a || b || Xg || Yg || Xa || Ya)
        //
        sm2_compute_za_digest(za,
                              EVP_MD_CTX_md(za_ctx),
                              SM2_DEFAULT_USERID,
                              16,
                              EVP_PKEY_get0_EC_KEY(pkey)
                              );

        //
        // Set Za to Message Digest Context
        //
        if (!EVP_VerifyUpdate(ctx, za, za_size)) {
            ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ERR_R_EVP_LIB);
            goto err;
        }
    }

    //
    // 2. M' = SM3(Za || VerifyAttribute)
    //
    inl = ASN1_item_i2d(asn, &buf_in, it);
    if (inl <= 0) {
        ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ERR_R_INTERNAL_ERROR);
        goto err;
    }
    if (buf_in == NULL) {
        ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ERR_R_MALLOC_FAILURE);
        goto err;
    }
    inll = inl;

    ret = EVP_DigestVerify(ctx, signature->data, (size_t)signature->length,
                           buf_in, inl);
    if (ret <= 0) {
        ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ERR_R_EVP_LIB);
        goto err;
    }
    ret = 1;
 err:
    OPENSSL_clear_free(buf_in, inll);
    OPENSSL_free(za);
    EVP_MD_CTX_free(ctx);
    EVP_MD_CTX_free(za_ctx);

    return ret;
}

/*
  Determine this pkey is a Sm2 ECC signature verify key
  TODO: further check X509 cert belongs to Jit

*/
int is_sm2_verify_pkey(EVP_PKEY *pkey) {
    EC_GROUP *ecgrp;

    if (EVP_PKEY_id(pkey) == EVP_PKEY_EC) {
        ecgrp = EC_KEY_get0_group(EVP_PKEY_get0_EC_KEY(pkey));

        if (ecgrp && NID_sm2 == EC_GROUP_get_curve_name(ecgrp)) {
            return 1;
        }
    }

    return 0;
}

int ASN1_item_verify_ex(const ASN1_ITEM *it, X509_ALGOR *a,
                        ASN1_BIT_STRING *signature, void *asn, EVP_PKEY *pkey)
{
    int ret = -1;

    if (!pkey) {
        ASN1err(ASN1_F_ASN1_ITEM_VERIFY, ERR_R_PASSED_NULL_PARAMETER);
        return -1;
    }

    if (is_sm2_verify_pkey(pkey)) {
      ret = ASN1_item_verify_sm2(it, a, signature, asn, pkey);
    } else {
      ret = ASN1_item_verify(it, a, signature, asn, pkey);
    }

    return ret;
}

#else

int ASN1_item_verify_ex(const ASN1_ITEM *it, X509_ALGOR *a,
                        ASN1_BIT_STRING *signature, void *asn, EVP_PKEY *pkey)
{
    return ASN1_item_verify(it, a, signature, asn, pkey);
}

#endif //OPENSSL_NO_SM2