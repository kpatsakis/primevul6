static int wc_RsaFunctionAsync(const byte* in, word32 inLen, byte* out,
                          word32* outLen, int type, RsaKey* key, WC_RNG* rng)
{
    int ret = 0;

    (void)rng;

#ifdef WOLFSSL_ASYNC_CRYPT_TEST
    if (wc_AsyncTestInit(&key->asyncDev, ASYNC_TEST_RSA_FUNC)) {
        WC_ASYNC_TEST* testDev = &key->asyncDev.test;
        testDev->rsaFunc.in = in;
        testDev->rsaFunc.inSz = inLen;
        testDev->rsaFunc.out = out;
        testDev->rsaFunc.outSz = outLen;
        testDev->rsaFunc.type = type;
        testDev->rsaFunc.key = key;
        testDev->rsaFunc.rng = rng;
        return WC_PENDING_E;
    }
#endif /* WOLFSSL_ASYNC_CRYPT_TEST */

    switch(type) {
#ifndef WOLFSSL_RSA_PUBLIC_ONLY
    case RSA_PRIVATE_DECRYPT:
    case RSA_PRIVATE_ENCRYPT:
    #ifdef HAVE_CAVIUM
        key->dataLen = key->n.raw.len;
        ret = NitroxRsaExptMod(in, inLen,
                               key->d.raw.buf, key->d.raw.len,
                               key->n.raw.buf, key->n.raw.len,
                               out, outLen, key);
    #elif defined(HAVE_INTEL_QA)
        #ifdef RSA_LOW_MEM
            ret = IntelQaRsaPrivate(&key->asyncDev, in, inLen,
                                    &key->d.raw, &key->n.raw,
                                    out, outLen);
        #else
            ret = IntelQaRsaCrtPrivate(&key->asyncDev, in, inLen,
                                &key->p.raw, &key->q.raw,
                                &key->dP.raw, &key->dQ.raw,
                                &key->u.raw,
                                out, outLen);
        #endif
    #else /* WOLFSSL_ASYNC_CRYPT_TEST */
        ret = wc_RsaFunctionSync(in, inLen, out, outLen, type, key, rng);
    #endif
        break;
#endif

    case RSA_PUBLIC_ENCRYPT:
    case RSA_PUBLIC_DECRYPT:
    #ifdef HAVE_CAVIUM
        key->dataLen = key->n.raw.len;
        ret = NitroxRsaExptMod(in, inLen,
                               key->e.raw.buf, key->e.raw.len,
                               key->n.raw.buf, key->n.raw.len,
                               out, outLen, key);
    #elif defined(HAVE_INTEL_QA)
        ret = IntelQaRsaPublic(&key->asyncDev, in, inLen,
                               &key->e.raw, &key->n.raw,
                               out, outLen);
    #else /* WOLFSSL_ASYNC_CRYPT_TEST */
        ret = wc_RsaFunctionSync(in, inLen, out, outLen, type, key, rng);
    #endif
        break;

    default:
        ret = RSA_WRONG_TYPE_E;
    }

    return ret;
}