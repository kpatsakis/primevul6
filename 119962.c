int wc_RsaDirect(byte* in, word32 inLen, byte* out, word32* outSz,
        RsaKey* key, int type, WC_RNG* rng)
{
    int ret;

    if (in == NULL || outSz == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    /* sanity check on type of RSA operation */
    switch (type) {
        case RSA_PUBLIC_ENCRYPT:
        case RSA_PUBLIC_DECRYPT:
        case RSA_PRIVATE_ENCRYPT:
        case RSA_PRIVATE_DECRYPT:
            break;
        default:
            WOLFSSL_MSG("Bad RSA type");
            return BAD_FUNC_ARG;
    }

    if ((ret = wc_RsaEncryptSize(key)) < 0) {
        return BAD_FUNC_ARG;
    }

    if (inLen != (word32)ret) {
        WOLFSSL_MSG("Bad input length. Should be RSA key size");
        return BAD_FUNC_ARG;
    }

    if (out == NULL) {
        *outSz = inLen;
        return LENGTH_ONLY_E;
    }

    switch (key->state) {
        case RSA_STATE_NONE:
        case RSA_STATE_ENCRYPT_PAD:
        case RSA_STATE_ENCRYPT_EXPTMOD:
        case RSA_STATE_DECRYPT_EXPTMOD:
        case RSA_STATE_DECRYPT_UNPAD:
            key->state = (type == RSA_PRIVATE_ENCRYPT ||
                    type == RSA_PUBLIC_ENCRYPT) ? RSA_STATE_ENCRYPT_EXPTMOD:
                                                  RSA_STATE_DECRYPT_EXPTMOD;

            key->dataLen = *outSz;

            ret = wc_RsaFunction(in, inLen, out, &key->dataLen, type, key, rng);
            if (ret >= 0 || ret == WC_PENDING_E) {
                key->state = (type == RSA_PRIVATE_ENCRYPT ||
                    type == RSA_PUBLIC_ENCRYPT) ? RSA_STATE_ENCRYPT_RES:
                                                  RSA_STATE_DECRYPT_RES;
            }
            if (ret < 0) {
                break;
            }

            FALL_THROUGH;

        case RSA_STATE_ENCRYPT_RES:
        case RSA_STATE_DECRYPT_RES:
            ret = key->dataLen;
            break;

        default:
            ret = BAD_STATE_E;
    }

    /* if async pending then skip cleanup*/
    if (ret == WC_PENDING_E
    #ifdef WC_RSA_NONBLOCK
        || ret == FP_WOULDBLOCK
    #endif
    ) {
        return ret;
    }

    key->state = RSA_STATE_NONE;
    wc_RsaCleanup(key);

    return ret;
}