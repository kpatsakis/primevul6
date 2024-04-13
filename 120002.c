static int RsaPad(const byte* input, word32 inputLen, byte* pkcsBlock,
                           word32 pkcsBlockLen, byte padValue, WC_RNG* rng)
{
    if (input == NULL || inputLen == 0 || pkcsBlock == NULL ||
                                                        pkcsBlockLen == 0) {
        return BAD_FUNC_ARG;
    }

    if (pkcsBlockLen - RSA_MIN_PAD_SZ < inputLen) {
        WOLFSSL_MSG("RsaPad error, invalid length");
        return RSA_PAD_E;
    }
    pkcsBlock[0] = 0x0;       /* set first byte to zero and advance */
    pkcsBlock++; pkcsBlockLen--;
    pkcsBlock[0] = padValue;  /* insert padValue */

    if (padValue == RSA_BLOCK_TYPE_1) {

        /* pad with 0xff bytes */
        XMEMSET(&pkcsBlock[1], 0xFF, pkcsBlockLen - inputLen - 2);
    }
    else {
#if !defined(WOLFSSL_RSA_VERIFY_ONLY) && !defined(WC_NO_RNG)
        /* pad with non-zero random bytes */
        word32 padLen, i;
        int    ret;
        padLen = pkcsBlockLen - inputLen - 1;
        ret    = wc_RNG_GenerateBlock(rng, &pkcsBlock[1], padLen);
        if (ret != 0) {
            return ret;
        }

        /* remove zeros */
        for (i = 1; i < padLen; i++) {
            if (pkcsBlock[i] == 0) pkcsBlock[i] = 0x01;
        }
#else
        (void)rng;
        return RSA_WRONG_TYPE_E;
#endif
    }

    pkcsBlock[pkcsBlockLen-inputLen-1] = 0;     /* separator */
    XMEMCPY(pkcsBlock+pkcsBlockLen-inputLen, input, inputLen);

    return 0;
}