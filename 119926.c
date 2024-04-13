int wc_RsaPSS_CheckPadding_ex(const byte* in, word32 inSz, byte* sig,
                              word32 sigSz, enum wc_HashType hashType,
                              int saltLen, int bits)
{
    int ret = 0;
#ifndef WOLFSSL_PSS_LONG_SALT
    byte sigCheck[WC_MAX_DIGEST_SIZE*2 + RSA_PSS_PAD_SZ];
#else
    byte *sigCheck = NULL;
#endif

    (void)bits;

    if (in == NULL || sig == NULL ||
                               inSz != (word32)wc_HashGetDigestSize(hashType)) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        if (saltLen == RSA_PSS_SALT_LEN_DEFAULT) {
            saltLen = inSz;
            #ifdef WOLFSSL_SHA512
                /* See FIPS 186-4 section 5.5 item (e). */
                if (bits == 1024 && inSz == WC_SHA512_DIGEST_SIZE) {
                    saltLen = RSA_PSS_SALT_MAX_SZ;
                }
            #endif
        }
#ifndef WOLFSSL_PSS_LONG_SALT
        else if ((word32)saltLen > inSz) {
            ret = PSS_SALTLEN_E;
        }
#endif
#ifndef WOLFSSL_PSS_SALT_LEN_DISCOVER
        else if (saltLen < RSA_PSS_SALT_LEN_DEFAULT) {
            ret = PSS_SALTLEN_E;
        }
#else
        else if (saltLen == RSA_PSS_SALT_LEN_DISCOVER) {
            saltLen = sigSz - inSz;
            if (saltLen < 0) {
                ret = PSS_SALTLEN_E;
            }
        }
        else if (saltLen < RSA_PSS_SALT_LEN_DISCOVER) {
            ret = PSS_SALTLEN_E;
        }
#endif
    }

    /* Sig = Salt | Exp Hash */
    if (ret == 0) {
        if (sigSz != inSz + saltLen) {
            ret = PSS_SALTLEN_E;
        }
    }

#ifdef WOLFSSL_PSS_LONG_SALT
    if (ret == 0) {
        sigCheck = (byte*)XMALLOC(RSA_PSS_PAD_SZ + inSz + saltLen, NULL,
                                                       DYNAMIC_TYPE_RSA_BUFFER);
        if (sigCheck == NULL) {
            ret = MEMORY_E;
        }
    }
#endif

    /* Exp Hash = HASH(8 * 0x00 | Message Hash | Salt) */
    if (ret == 0) {
        XMEMSET(sigCheck, 0, RSA_PSS_PAD_SZ);
        XMEMCPY(sigCheck + RSA_PSS_PAD_SZ, in, inSz);
        XMEMCPY(sigCheck + RSA_PSS_PAD_SZ + inSz, sig, saltLen);
        ret = wc_Hash(hashType, sigCheck, RSA_PSS_PAD_SZ + inSz + saltLen,
                      sigCheck, inSz);
    }
    if (ret == 0) {
        if (XMEMCMP(sigCheck, sig + saltLen, inSz) != 0) {
            WOLFSSL_MSG("RsaPSS_CheckPadding: Padding Error");
            ret = BAD_PADDING_E;
        }
    }

#ifdef WOLFSSL_PSS_LONG_SALT
    if (sigCheck != NULL) {
        XFREE(sigCheck, NULL, DYNAMIC_TYPE_RSA_BUFFER);
    }
#endif
    return ret;
}