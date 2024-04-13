static int wc_RsaFunctionSync(const byte* in, word32 inLen, byte* out,
                          word32* outLen, int type, RsaKey* key, WC_RNG* rng)
{
#ifndef WOLFSSL_SP_MATH
#ifdef WOLFSSL_SMALL_STACK
    mp_int* tmp;
#ifdef WC_RSA_BLINDING
    mp_int* rnd;
    mp_int* rndi;
#endif
#else
    mp_int tmp[1];
#ifdef WC_RSA_BLINDING
    mp_int rnd[1], rndi[1];
#endif
#endif
    int    ret = 0;
    word32 keyLen = 0;
#endif

#ifdef WOLFSSL_HAVE_SP_RSA
#ifndef WOLFSSL_SP_NO_2048
    if (mp_count_bits(&key->n) == 2048) {
        switch(type) {
#ifndef WOLFSSL_RSA_PUBLIC_ONLY
        case RSA_PRIVATE_DECRYPT:
        case RSA_PRIVATE_ENCRYPT:
    #ifdef WC_RSA_BLINDING
            if (rng == NULL)
                return MISSING_RNG_E;
    #endif
    #ifndef RSA_LOW_MEM
            if ((mp_count_bits(&key->p) == 1024) &&
                                             (mp_count_bits(&key->q) == 1024)) {
                return sp_RsaPrivate_2048(in, inLen, &key->d, &key->p, &key->q,
                                          &key->dP, &key->dQ, &key->u, &key->n,
                                          out, outLen);
            }
            break;
    #else
            return sp_RsaPrivate_2048(in, inLen, &key->d, NULL, NULL, NULL,
                                      NULL, NULL, &key->n, out, outLen);
    #endif
#endif
        case RSA_PUBLIC_ENCRYPT:
        case RSA_PUBLIC_DECRYPT:
            return sp_RsaPublic_2048(in, inLen, &key->e, &key->n, out, outLen);
        }
    }
#endif
#ifndef WOLFSSL_SP_NO_3072
    if (mp_count_bits(&key->n) == 3072) {
        switch(type) {
#ifndef WOLFSSL_RSA_PUBLIC_ONLY
        case RSA_PRIVATE_DECRYPT:
        case RSA_PRIVATE_ENCRYPT:
    #ifdef WC_RSA_BLINDING
            if (rng == NULL)
                return MISSING_RNG_E;
    #endif
    #ifndef RSA_LOW_MEM
            if ((mp_count_bits(&key->p) == 1536) &&
                                             (mp_count_bits(&key->q) == 1536)) {
                return sp_RsaPrivate_3072(in, inLen, &key->d, &key->p, &key->q,
                                          &key->dP, &key->dQ, &key->u, &key->n,
                                          out, outLen);
            }
            break;
    #else
            return sp_RsaPrivate_3072(in, inLen, &key->d, NULL, NULL, NULL,
                                      NULL, NULL, &key->n, out, outLen);
    #endif
#endif
        case RSA_PUBLIC_ENCRYPT:
        case RSA_PUBLIC_DECRYPT:
            return sp_RsaPublic_3072(in, inLen, &key->e, &key->n, out, outLen);
        }
    }
#endif
#ifdef WOLFSSL_SP_4096
    if (mp_count_bits(&key->n) == 4096) {
        switch(type) {
#ifndef WOLFSSL_RSA_PUBLIC_ONLY
        case RSA_PRIVATE_DECRYPT:
        case RSA_PRIVATE_ENCRYPT:
    #ifdef WC_RSA_BLINDING
            if (rng == NULL)
                return MISSING_RNG_E;
    #endif
    #ifndef RSA_LOW_MEM
            if ((mp_count_bits(&key->p) == 2048) &&
                                             (mp_count_bits(&key->q) == 2048)) {
                return sp_RsaPrivate_4096(in, inLen, &key->d, &key->p, &key->q,
                                          &key->dP, &key->dQ, &key->u, &key->n,
                                          out, outLen);
            }
            break;
    #else
            return sp_RsaPrivate_4096(in, inLen, &key->d, NULL, NULL, NULL,
                                      NULL, NULL, &key->n, out, outLen);
    #endif
#endif
        case RSA_PUBLIC_ENCRYPT:
        case RSA_PUBLIC_DECRYPT:
            return sp_RsaPublic_4096(in, inLen, &key->e, &key->n, out, outLen);
        }
    }
#endif
#endif /* WOLFSSL_HAVE_SP_RSA */

#ifdef WOLFSSL_SP_MATH
    (void)rng;
    WOLFSSL_MSG("SP Key Size Error");
    return WC_KEY_SIZE_E;
#else
    (void)rng;

#ifdef WOLFSSL_SMALL_STACK
    tmp = (mp_int*)XMALLOC(sizeof(mp_int), key->heap, DYNAMIC_TYPE_RSA);
    if (tmp == NULL)
        return MEMORY_E;
#ifdef WC_RSA_BLINDING
    rnd = (mp_int*)XMALLOC(sizeof(mp_int) * 2, key->heap, DYNAMIC_TYPE_RSA);
    if (rnd == NULL) {
        XFREE(tmp, key->heap, DYNAMIC_TYPE_RSA);
        return MEMORY_E;
    }
    rndi = rnd + 1;
#endif /* WC_RSA_BLINDING */
#endif /* WOLFSSL_SMALL_STACK */

    if (mp_init(tmp) != MP_OKAY)
        ret = MP_INIT_E;

#ifdef WC_RSA_BLINDING
    if (ret == 0) {
        if (type == RSA_PRIVATE_DECRYPT || type == RSA_PRIVATE_ENCRYPT) {
            if (mp_init_multi(rnd, rndi, NULL, NULL, NULL, NULL) != MP_OKAY) {
                mp_clear(tmp);
                ret = MP_INIT_E;
            }
        }
    }
#endif

#ifndef TEST_UNPAD_CONSTANT_TIME
    if (ret == 0 && mp_read_unsigned_bin(tmp, (byte*)in, inLen) != MP_OKAY)
        ret = MP_READ_E;

    if (ret == 0) {
        switch(type) {
    #ifndef WOLFSSL_RSA_PUBLIC_ONLY
        case RSA_PRIVATE_DECRYPT:
        case RSA_PRIVATE_ENCRYPT:
        {
        #if defined(WC_RSA_BLINDING) && !defined(WC_NO_RNG)
            /* blind */
            ret = mp_rand(rnd, get_digit_count(&key->n), rng);

            /* rndi = 1/rnd mod n */
            if (ret == 0 && mp_invmod(rnd, &key->n, rndi) != MP_OKAY)
                ret = MP_INVMOD_E;

            /* rnd = rnd^e */
            if (ret == 0 && mp_exptmod(rnd, &key->e, &key->n, rnd) != MP_OKAY)
                ret = MP_EXPTMOD_E;

            /* tmp = tmp*rnd mod n */
            if (ret == 0 && mp_mulmod(tmp, rnd, &key->n, tmp) != MP_OKAY)
                ret = MP_MULMOD_E;
        #endif /* WC_RSA_BLINDING && !WC_NO_RNG */

        #ifdef RSA_LOW_MEM      /* half as much memory but twice as slow */
            if (ret == 0 && mp_exptmod(tmp, &key->d, &key->n, tmp) != MP_OKAY)
                ret = MP_EXPTMOD_E;
        #else
            if (ret == 0) {
            #ifdef WOLFSSL_SMALL_STACK
                mp_int* tmpa;
                mp_int* tmpb = NULL;
            #else
                mp_int tmpa[1], tmpb[1];
            #endif
                int cleara = 0, clearb = 0;

            #ifdef WOLFSSL_SMALL_STACK
                tmpa = (mp_int*)XMALLOC(sizeof(mp_int) * 2,
                        key->heap, DYNAMIC_TYPE_RSA);
                if (tmpa != NULL)
                    tmpb = tmpa + 1;
                else
                    ret = MEMORY_E;
            #endif

                if (ret == 0) {
                    if (mp_init(tmpa) != MP_OKAY)
                        ret = MP_INIT_E;
                    else
                        cleara = 1;
                }

                if (ret == 0) {
                    if (mp_init(tmpb) != MP_OKAY)
                        ret = MP_INIT_E;
                    else
                        clearb = 1;
                }

                /* tmpa = tmp^dP mod p */
                if (ret == 0 && mp_exptmod(tmp, &key->dP, &key->p,
                                                               tmpa) != MP_OKAY)
                    ret = MP_EXPTMOD_E;

                /* tmpb = tmp^dQ mod q */
                if (ret == 0 && mp_exptmod(tmp, &key->dQ, &key->q,
                                                               tmpb) != MP_OKAY)
                    ret = MP_EXPTMOD_E;

                /* tmp = (tmpa - tmpb) * qInv (mod p) */
                if (ret == 0 && mp_sub(tmpa, tmpb, tmp) != MP_OKAY)
                    ret = MP_SUB_E;

                if (ret == 0 && mp_mulmod(tmp, &key->u, &key->p,
                                                                tmp) != MP_OKAY)
                    ret = MP_MULMOD_E;

                /* tmp = tmpb + q * tmp */
                if (ret == 0 && mp_mul(tmp, &key->q, tmp) != MP_OKAY)
                    ret = MP_MUL_E;

                if (ret == 0 && mp_add(tmp, tmpb, tmp) != MP_OKAY)
                    ret = MP_ADD_E;

            #ifdef WOLFSSL_SMALL_STACK
                if (tmpa != NULL)
            #endif
                {
                    if (cleara)
                        mp_clear(tmpa);
                    if (clearb)
                        mp_clear(tmpb);
            #ifdef WOLFSSL_SMALL_STACK
                    XFREE(tmpa, key->heap, DYNAMIC_TYPE_RSA);
            #endif
                }
            } /* tmpa/b scope */
        #endif   /* RSA_LOW_MEM */

        #ifdef WC_RSA_BLINDING
            /* unblind */
            if (ret == 0 && mp_mulmod(tmp, rndi, &key->n, tmp) != MP_OKAY)
                ret = MP_MULMOD_E;
        #endif   /* WC_RSA_BLINDING */

            break;
        }
    #endif
        case RSA_PUBLIC_ENCRYPT:
        case RSA_PUBLIC_DECRYPT:
            if (mp_exptmod_nct(tmp, &key->e, &key->n, tmp) != MP_OKAY)
                ret = MP_EXPTMOD_E;
            break;
        default:
            ret = RSA_WRONG_TYPE_E;
            break;
        }
    }

    if (ret == 0) {
        keyLen = wc_RsaEncryptSize(key);
        if (keyLen > *outLen)
            ret = RSA_BUFFER_E;
    }

#ifndef WOLFSSL_XILINX_CRYPT
    if (ret == 0) {
        *outLen = keyLen;
        if (mp_to_unsigned_bin_len(tmp, out, keyLen) != MP_OKAY)
             ret = MP_TO_E;
    }
#endif
#else
    (void)type;
    (void)key;
    (void)keyLen;
    XMEMCPY(out, in, inLen);
    *outLen = inLen;
#endif

    mp_clear(tmp);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(tmp, key->heap, DYNAMIC_TYPE_RSA);
#endif
#ifdef WC_RSA_BLINDING
    if (type == RSA_PRIVATE_DECRYPT || type == RSA_PRIVATE_ENCRYPT) {
        mp_clear(rndi);
        mp_clear(rnd);
    }
#ifdef WOLFSSL_SMALL_STACK
    XFREE(rnd, key->heap, DYNAMIC_TYPE_RSA);
#endif
#endif /* WC_RSA_BLINDING */
    return ret;
#endif /* WOLFSSL_SP_MATH */
}