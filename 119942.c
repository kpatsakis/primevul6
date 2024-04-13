static int RsaUnPad_PSS(byte *pkcsBlock, unsigned int pkcsBlockLen,
                        byte **output, enum wc_HashType hType, int mgf,
                        int saltLen, int bits, void* heap)
{
    int   ret;
    byte* tmp;
    int   hLen, i, maskLen;
#ifdef WOLFSSL_SHA512
    int orig_bits = bits;
#endif
#if defined(WOLFSSL_NO_MALLOC) && !defined(WOLFSSL_STATIC_MEMORY)
    byte tmp_buf[RSA_MAX_SIZE/8];
    tmp = tmp_buf;

    if (pkcsBlockLen > RSA_MAX_SIZE/8) {
        return MEMORY_E;
    }
#endif

    hLen = wc_HashGetDigestSize(hType);
    if (hLen < 0)
        return hLen;
    bits = (bits - 1) & 0x7;
    if ((pkcsBlock[0] & (0xff << bits)) != 0) {
        return BAD_PADDING_E;
    }
    if (bits == 0) {
        pkcsBlock++;
        pkcsBlockLen--;
    }
    maskLen = (int)pkcsBlockLen - 1 - hLen;
    if (maskLen < 0) {
        WOLFSSL_MSG("RsaUnPad_PSS: Hash too large");
        return WC_KEY_SIZE_E;
    }

    if (saltLen == RSA_PSS_SALT_LEN_DEFAULT) {
        saltLen = hLen;
        #ifdef WOLFSSL_SHA512
            /* See FIPS 186-4 section 5.5 item (e). */
            if (orig_bits == 1024 && hLen == WC_SHA512_DIGEST_SIZE)
                saltLen = RSA_PSS_SALT_MAX_SZ;
        #endif
    }
#ifndef WOLFSSL_PSS_LONG_SALT
    else if (saltLen > hLen)
        return PSS_SALTLEN_E;
#endif
#ifndef WOLFSSL_PSS_SALT_LEN_DISCOVER
    else if (saltLen < RSA_PSS_SALT_LEN_DEFAULT)
        return PSS_SALTLEN_E;
    if (maskLen < saltLen + 1) {
        return PSS_SALTLEN_E;
    }
#else
    else if (saltLen < RSA_PSS_SALT_LEN_DISCOVER)
        return PSS_SALTLEN_E;
    if (saltLen != RSA_PSS_SALT_LEN_DISCOVER && maskLen < saltLen + 1) {
        return WC_KEY_SIZE_E;
    }
#endif

    if (pkcsBlock[pkcsBlockLen - 1] != RSA_PSS_PAD_TERM) {
        WOLFSSL_MSG("RsaUnPad_PSS: Padding Term Error");
        return BAD_PADDING_E;
    }

#if !defined(WOLFSSL_NO_MALLOC) || defined(WOLFSSL_STATIC_MEMORY)
    tmp = (byte*)XMALLOC(maskLen, heap, DYNAMIC_TYPE_RSA_BUFFER);
    if (tmp == NULL) {
        return MEMORY_E;
    }
#endif

    if ((ret = RsaMGF(mgf, pkcsBlock + maskLen, hLen, tmp, maskLen,
                                                                  heap)) != 0) {
        XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
        return ret;
    }

    tmp[0] &= (1 << bits) - 1;
    pkcsBlock[0] &= (1 << bits) - 1;
#ifdef WOLFSSL_PSS_SALT_LEN_DISCOVER
    if (saltLen == RSA_PSS_SALT_LEN_DISCOVER) {
        for (i = 0; i < maskLen - 1; i++) {
            if (tmp[i] != pkcsBlock[i]) {
                break;
            }
        }
        if (tmp[i] != (pkcsBlock[i] ^ 0x01)) {
            XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
            WOLFSSL_MSG("RsaUnPad_PSS: Padding Error Match");
            return PSS_SALTLEN_RECOVER_E;
        }
        saltLen = maskLen - (i + 1);
    }
    else
#endif
    {
        for (i = 0; i < maskLen - 1 - saltLen; i++) {
            if (tmp[i] != pkcsBlock[i]) {
                XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
                WOLFSSL_MSG("RsaUnPad_PSS: Padding Error Match");
                return PSS_SALTLEN_E;
            }
        }
        if (tmp[i] != (pkcsBlock[i] ^ 0x01)) {
            XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
            WOLFSSL_MSG("RsaUnPad_PSS: Padding Error End");
            return PSS_SALTLEN_E;
        }
    }
    for (i++; i < maskLen; i++)
        pkcsBlock[i] ^= tmp[i];

#if !defined(WOLFSSL_NO_MALLOC) || defined(WOLFSSL_STATIC_MEMORY)
    XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
#endif

    *output = pkcsBlock + maskLen - saltLen;
    return saltLen + hLen;
}