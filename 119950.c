static int RsaMGF(int type, byte* seed, word32 seedSz, byte* out,
                                                    word32 outSz, void* heap)
{
    int ret;

    switch(type) {
    #ifndef NO_SHA
        case WC_MGF1SHA1:
            ret = RsaMGF1(WC_HASH_TYPE_SHA, seed, seedSz, out, outSz, heap);
            break;
    #endif
    #ifndef NO_SHA256
    #ifdef WOLFSSL_SHA224
        case WC_MGF1SHA224:
            ret = RsaMGF1(WC_HASH_TYPE_SHA224, seed, seedSz, out, outSz, heap);
            break;
    #endif
        case WC_MGF1SHA256:
            ret = RsaMGF1(WC_HASH_TYPE_SHA256, seed, seedSz, out, outSz, heap);
            break;
    #endif
    #ifdef WOLFSSL_SHA384
        case WC_MGF1SHA384:
            ret = RsaMGF1(WC_HASH_TYPE_SHA384, seed, seedSz, out, outSz, heap);
            break;
    #endif
    #ifdef WOLFSSL_SHA512
        case WC_MGF1SHA512:
            ret = RsaMGF1(WC_HASH_TYPE_SHA512, seed, seedSz, out, outSz, heap);
            break;
    #endif
        default:
            WOLFSSL_MSG("Unknown MGF type: check build options");
            ret = BAD_FUNC_ARG;
    }

    /* in case of default avoid unused warning */
    (void)seed;
    (void)seedSz;
    (void)out;
    (void)outSz;
    (void)heap;

    return ret;
}