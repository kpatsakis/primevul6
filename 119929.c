static int RsaPad_PSS(const byte* input, word32 inputLen, byte* pkcsBlock,
        word32 pkcsBlockLen, WC_RNG* rng, enum wc_HashType hType, int mgf,
        int saltLen, int bits, void* heap)
{
    int   ret = 0;
    int   hLen, i, o, maskLen, hiBits;
    byte* m;
    byte* s;
#if defined(WOLFSSL_NO_MALLOC) && !defined(WOLFSSL_STATIC_MEMORY)
    byte msg[RSA_MAX_SIZE/8 + RSA_PSS_PAD_SZ];
#else
    byte* msg = NULL;
#endif
#if defined(WOLFSSL_PSS_LONG_SALT) || defined(WOLFSSL_PSS_SALT_LEN_DISCOVER)
    byte* salt;
#else
    byte salt[WC_MAX_DIGEST_SIZE];
#endif

#if defined(WOLFSSL_PSS_LONG_SALT) || defined(WOLFSSL_PSS_SALT_LEN_DISCOVER)
    if (pkcsBlockLen > RSA_MAX_SIZE/8) {
        return MEMORY_E;
    }
#endif

    hLen = wc_HashGetDigestSize(hType);
    if (hLen < 0)
        return hLen;
    if ((int)inputLen != hLen) {
        return BAD_FUNC_ARG;
    }

    hiBits = (bits - 1) & 0x7;
    if (hiBits == 0) {
        /* Per RFC8017, set the leftmost 8emLen - emBits bits of the
           leftmost octet in DB to zero.
        */
        *(pkcsBlock++) = 0;
        pkcsBlockLen--;
    }

    if (saltLen == RSA_PSS_SALT_LEN_DEFAULT) {
        saltLen = hLen;
        #ifdef WOLFSSL_SHA512
            /* See FIPS 186-4 section 5.5 item (e). */
            if (bits == 1024 && hLen == WC_SHA512_DIGEST_SIZE) {
                saltLen = RSA_PSS_SALT_MAX_SZ;
            }
        #endif
    }
#ifndef WOLFSSL_PSS_LONG_SALT
    else if (saltLen > hLen) {
        return PSS_SALTLEN_E;
    }
#endif
#ifndef WOLFSSL_PSS_SALT_LEN_DISCOVER
    else if (saltLen < RSA_PSS_SALT_LEN_DEFAULT) {
        return PSS_SALTLEN_E;
    }
#else
    else if (saltLen == RSA_PSS_SALT_LEN_DISCOVER) {
        saltLen = (int)pkcsBlockLen - hLen - 2;
        if (saltLen < 0) {
            return PSS_SALTLEN_E;
        }
    }
    else if (saltLen < RSA_PSS_SALT_LEN_DISCOVER) {
        return PSS_SALTLEN_E;
    }
#endif
    if ((int)pkcsBlockLen - hLen < saltLen + 2) {
        return PSS_SALTLEN_E;
    }
    maskLen = pkcsBlockLen - 1 - hLen;

#if defined(WOLFSSL_PSS_LONG_SALT) || defined(WOLFSSL_PSS_SALT_LEN_DISCOVER)
    #if !defined(WOLFSSL_NO_MALLOC) || defined(WOLFSSL_STATIC_MEMORY)
        msg = (byte*)XMALLOC(RSA_PSS_PAD_SZ + inputLen + saltLen, heap,
                                                       DYNAMIC_TYPE_RSA_BUFFER);
        if (msg == NULL) {
            return MEMORY_E;
        }
    #endif
    salt = s = m = msg;
    XMEMSET(m, 0, RSA_PSS_PAD_SZ);
    m += RSA_PSS_PAD_SZ;
    XMEMCPY(m, input, inputLen);
    m += inputLen;
    o = (int)(m - s);
    if (saltLen > 0) {
        ret = wc_RNG_GenerateBlock(rng, m, saltLen);
        if (ret == 0) {
            m += saltLen;
        }
    }
#else
    if (pkcsBlockLen < RSA_PSS_PAD_SZ + inputLen + saltLen) {
    #if !defined(WOLFSSL_NO_MALLOC) || defined(WOLFSSL_STATIC_MEMORY)
        msg = (byte*)XMALLOC(RSA_PSS_PAD_SZ + inputLen + saltLen, heap,
                                                       DYNAMIC_TYPE_RSA_BUFFER);
        if (msg == NULL) {
            return MEMORY_E;
        }
    #endif
        m = msg;
    }
    else {
        m = pkcsBlock;
    }
    s = m;
    XMEMSET(m, 0, RSA_PSS_PAD_SZ);
    m += RSA_PSS_PAD_SZ;
    XMEMCPY(m, input, inputLen);
    m += inputLen;
    o = 0;
    if (saltLen > 0) {
        ret = wc_RNG_GenerateBlock(rng, salt, saltLen);
        if (ret == 0) {
            XMEMCPY(m, salt, saltLen);
            m += saltLen;
        }
    }
#endif
    if (ret == 0) {
        /* Put Hash at end of pkcsBlock - 1 */
        ret = wc_Hash(hType, s, (word32)(m - s), pkcsBlock + maskLen, hLen);
    }
    if (ret == 0) {
       /* Set the last eight bits or trailer field to the octet 0xbc */
        pkcsBlock[pkcsBlockLen - 1] = RSA_PSS_PAD_TERM;

        ret = RsaMGF(mgf, pkcsBlock + maskLen, hLen, pkcsBlock, maskLen, heap);
    }
    if (ret == 0) {
        /* Clear the first high bit when "8emLen - emBits" is non-zero.
           where emBits = n modBits - 1 */
        if (hiBits)
            pkcsBlock[0] &= (1 << hiBits) - 1;

        m = pkcsBlock + maskLen - saltLen - 1;
        *(m++) ^= 0x01;
        for (i = 0; i < saltLen; i++) {
            m[i] ^= salt[o + i];
        }
    }

    #if !defined(WOLFSSL_NO_MALLOC) || defined(WOLFSSL_STATIC_MEMORY)
        if (msg != NULL) {
            XFREE(msg, heap, DYNAMIC_TYPE_RSA_BUFFER);
        }
    #endif
    return ret;
}