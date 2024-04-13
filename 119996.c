int wc_RsaUnPad_ex(byte* pkcsBlock, word32 pkcsBlockLen, byte** out,
                   byte padValue, int padType, enum wc_HashType hType,
                   int mgf, byte* optLabel, word32 labelLen, int saltLen,
                   int bits, void* heap)
{
    int ret;

    switch (padType) {
        case WC_RSA_PKCSV15_PAD:
            /*WOLFSSL_MSG("wolfSSL Using RSA PKCSV15 un-padding");*/
            ret = RsaUnPad(pkcsBlock, pkcsBlockLen, out, padValue);
            break;

    #ifndef WC_NO_RSA_OAEP
        case WC_RSA_OAEP_PAD:
            WOLFSSL_MSG("wolfSSL Using RSA OAEP un-padding");
            ret = RsaUnPad_OAEP((byte*)pkcsBlock, pkcsBlockLen, out,
                                        hType, mgf, optLabel, labelLen, heap);
            break;
    #endif

    #ifdef WC_RSA_PSS
        case WC_RSA_PSS_PAD:
            WOLFSSL_MSG("wolfSSL Using RSA PSS un-padding");
            ret = RsaUnPad_PSS((byte*)pkcsBlock, pkcsBlockLen, out, hType, mgf,
                                                           saltLen, bits, heap);
            break;
    #endif

    #ifdef WC_RSA_NO_PADDING
        case WC_RSA_NO_PAD:
            WOLFSSL_MSG("wolfSSL Using NO un-padding");

            /* In the case of no padding being used check that input is exactly
             * the RSA key length */
            if (bits <= 0 || pkcsBlockLen !=
                         ((word32)(bits+WOLFSSL_BIT_SIZE-1)/WOLFSSL_BIT_SIZE)) {
                WOLFSSL_MSG("Bad input size");
                ret = RSA_PAD_E;
            }
            else {
                if (out != NULL) {
                    *out = pkcsBlock;
                }
                ret = pkcsBlockLen;
            }
            break;
    #endif /* WC_RSA_NO_PADDING */

        default:
            WOLFSSL_MSG("Unknown RSA UnPad Type");
            ret = RSA_PAD_E;
    }

    /* silence warning if not used with padding scheme */
    (void)hType;
    (void)mgf;
    (void)optLabel;
    (void)labelLen;
    (void)saltLen;
    (void)bits;
    (void)heap;

    return ret;
}