int wc_SignatureGetSize(enum wc_SignatureType sig_type,
    const void* key, word32 key_len)
{
    int sig_len = BAD_FUNC_ARG;

    /* Suppress possible unused args if all signature types are disabled */
    (void)key;
    (void)key_len;

    switch(sig_type) {
        case WC_SIGNATURE_TYPE_ECC:
#ifdef HAVE_ECC
            /* Sanity check that void* key is at least ecc_key in size */
            if (key_len >= sizeof(ecc_key)) {
                sig_len = wc_ecc_sig_size((ecc_key*)key);
            }
            else {
                WOLFSSL_MSG("wc_SignatureGetSize: Invalid ECC key size");
            }
#else
            sig_len = SIG_TYPE_E;
#endif
            break;

        case WC_SIGNATURE_TYPE_RSA_W_ENC:
        case WC_SIGNATURE_TYPE_RSA:
#ifndef NO_RSA
            /* Sanity check that void* key is at least RsaKey in size */
            if (key_len >= sizeof(RsaKey)) {
                sig_len = wc_RsaEncryptSize((RsaKey*)key);
            }
            else {
                WOLFSSL_MSG("wc_SignatureGetSize: Invalid RsaKey key size");
            }
#else
            sig_len = SIG_TYPE_E;
#endif
            break;

        case WC_SIGNATURE_TYPE_NONE:
        default:
            sig_len = BAD_FUNC_ARG;
            break;
    }
    return sig_len;
}