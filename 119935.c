static int wc_RsaFunctionSync(const byte* in, word32 inLen, byte* out,
                          word32* outLen, int type, RsaKey* key, WC_RNG* rng)
{
    int    ret = 0;
    word32 keyLen;
    (void)rng;

    keyLen = wc_RsaEncryptSize(key);
    if (keyLen > *outLen) {
        WOLFSSL_MSG("Output buffer is not big enough");
        return BAD_FUNC_ARG;
    }

    if (inLen != keyLen) {
        WOLFSSL_MSG("Expected that inLen equals RSA key length");
        return BAD_FUNC_ARG;
    }

    switch(type) {
    case RSA_PRIVATE_DECRYPT:
    case RSA_PRIVATE_ENCRYPT:
    #ifdef WOLFSSL_XILINX_CRYPTO_OLD
        /* Currently public exponent is loaded by default.
         * In SDK 2017.1 RSA exponent values are expected to be of 4 bytes
         * leading to private key operations with Xsecure_RsaDecrypt not being
         * supported */
        ret = RSA_WRONG_TYPE_E;
    #else
        {
            byte *d;
            int dSz;
            XSecure_Rsa rsa;

            dSz = mp_unsigned_bin_size(&key->d);
            d = (byte*)XMALLOC(dSz, key->heap, DYNAMIC_TYPE_PRIVATE_KEY);
            if (d == NULL) {
                ret = MEMORY_E;
            }
            else {
                ret = mp_to_unsigned_bin(&key->d, d);
                XSecure_RsaInitialize(&rsa, key->mod, NULL, d);
            }

            if (ret == 0) {
                if (XSecure_RsaPrivateDecrypt(&rsa, (u8*)in, inLen, out) !=
                        XST_SUCCESS) {
                    ret = BAD_STATE_E;
                }
            }

            if (d != NULL) {
                XFREE(d, key->heap, DYNAMIC_TYPE_PRIVATE_KEY);
            }
        }
    #endif
        break;
    case RSA_PUBLIC_ENCRYPT:
    case RSA_PUBLIC_DECRYPT:
#ifdef WOLFSSL_XILINX_CRYPTO_OLD
        if (XSecure_RsaDecrypt(&(key->xRsa), in, out) != XST_SUCCESS) {
            ret = BAD_STATE_E;
        }
#else
        /* starting at Xilinx release 2019 the function XSecure_RsaDecrypt was removed */
        if (XSecure_RsaPublicEncrypt(&(key->xRsa), (u8*)in, inLen, out) != XST_SUCCESS) {
            WOLFSSL_MSG("Error happened when calling hardware RSA public operation");
            ret = BAD_STATE_E;
        }
#endif
        break;
    default:
        ret = RSA_WRONG_TYPE_E;
    }

    *outLen = keyLen;

    return ret;
}