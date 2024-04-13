int wc_CheckRsaKey(RsaKey* key)
{
#if defined(WOLFSSL_CRYPTOCELL)
    return 0;
#endif
#ifdef WOLFSSL_SMALL_STACK
    mp_int *k = NULL, *tmp = NULL;
#else
    mp_int k[1], tmp[1];
#endif
    int ret = 0;

#ifdef WOLFSSL_SMALL_STACK
    k = (mp_int*)XMALLOC(sizeof(mp_int) * 2, NULL, DYNAMIC_TYPE_RSA);
    if (k == NULL)
        return MEMORY_E;
    tmp = k + 1;
#endif

    if (mp_init_multi(k, tmp, NULL, NULL, NULL, NULL) != MP_OKAY)
        ret = MP_INIT_E;

    if (ret == 0) {
        if (key == NULL)
            ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        if (mp_set_int(k, 0x2342) != MP_OKAY)
            ret = MP_READ_E;
    }
#ifdef WOLFSSL_HAVE_SP_RSA
    if (ret == 0) {
        switch (mp_count_bits(&key->n)) {
    #ifndef WOLFSSL_SP_NO_2048
            case 2048:
                ret = sp_ModExp_2048(k, &key->e, &key->n, tmp);
                if (ret != 0)
                    ret = MP_EXPTMOD_E;
                if (ret == 0) {
                    ret = sp_ModExp_2048(tmp, &key->d, &key->n, tmp);
                    if (ret != 0)
                        ret = MP_EXPTMOD_E;
                }
                break;
    #endif /* WOLFSSL_SP_NO_2048 */
    #ifndef WOLFSSL_SP_NO_3072
            case 3072:
                ret = sp_ModExp_3072(k, &key->e, &key->n, tmp);
                if (ret != 0)
                    ret = MP_EXPTMOD_E;
                if (ret == 0) {
                  ret = sp_ModExp_3072(tmp, &key->d, &key->n, tmp);
                  if (ret != 0)
                      ret = MP_EXPTMOD_E;
                }
                break;
    #endif /* WOLFSSL_SP_NO_3072 */
    #ifdef WOLFSSL_SP_4096
            case 4096:
                ret = sp_ModExp_4096(k, &key->e, &key->n, tmp);
                if (ret != 0)
                    ret = MP_EXPTMOD_E;
                if (ret == 0) {
                  ret = sp_ModExp_4096(tmp, &key->d, &key->n, tmp);
                  if (ret != 0)
                      ret = MP_EXPTMOD_E;
                }
                break;
    #endif /* WOLFSSL_SP_4096 */
                default:
                /* If using only single prcsision math then issue key size error, 
                    otherwise fall-back to multi-precision math calculation */
                #ifdef WOLFSSL_SP_MATH
                    ret = WC_KEY_SIZE_E;
                #endif
                    break;
        }
    }
#endif /* WOLFSSL_HAVE_SP_RSA */

#ifndef WOLFSSL_SP_MATH
    if (ret == 0) {
        if (mp_exptmod(k, &key->e, &key->n, tmp) != MP_OKAY)
            ret = MP_EXPTMOD_E;
    }

    if (ret == 0) {
        if (mp_exptmod(tmp, &key->d, &key->n, tmp) != MP_OKAY)
            ret = MP_EXPTMOD_E;
    }
#endif /* !WOLFSSL_SP_MATH */

    if (ret == 0) {
        if (mp_cmp(k, tmp) != MP_EQ)
            ret = RSA_KEY_PAIR_E;
    }

    /* Check d is less than n. */
    if (ret == 0 ) {
        if (mp_cmp(&key->d, &key->n) != MP_LT) {
            ret = MP_EXPTMOD_E;
        }
    }
    /* Check p*q = n. */
    if (ret == 0 ) {
        if (mp_mul(&key->p, &key->q, tmp) != MP_OKAY) {
            ret = MP_EXPTMOD_E;
        }
    }
    if (ret == 0 ) {
        if (mp_cmp(&key->n, tmp) != MP_EQ) {
            ret = MP_EXPTMOD_E;
        }
    }

    /* Check dP, dQ and u if they exist */
    if (ret == 0 && !mp_iszero(&key->dP)) {
        if (mp_sub_d(&key->p, 1, tmp) != MP_OKAY) {
            ret = MP_EXPTMOD_E;
        }
        /* Check dP <= p-1. */
        if (ret == 0) {
            if (mp_cmp(&key->dP, tmp) != MP_LT) {
                ret = MP_EXPTMOD_E;
            }
        }
        /* Check e*dP mod p-1 = 1. (dP = 1/e mod p-1) */
        if (ret == 0) {
            if (mp_mulmod(&key->dP, &key->e, tmp, tmp) != MP_OKAY) {
                ret = MP_EXPTMOD_E;
            }
        }
        if (ret == 0 ) {
            if (!mp_isone(tmp)) {
                ret = MP_EXPTMOD_E;
            }
        }

        if (ret == 0) {
            if (mp_sub_d(&key->q, 1, tmp) != MP_OKAY) {
                ret = MP_EXPTMOD_E;
            }
        }
        /* Check dQ <= q-1. */
        if (ret == 0) {
            if (mp_cmp(&key->dQ, tmp) != MP_LT) {
                ret = MP_EXPTMOD_E;
            }
        }
        /* Check e*dP mod p-1 = 1. (dQ = 1/e mod q-1) */
        if (ret == 0) {
            if (mp_mulmod(&key->dQ, &key->e, tmp, tmp) != MP_OKAY) {
                ret = MP_EXPTMOD_E;
            }
        }
        if (ret == 0 ) {
            if (!mp_isone(tmp)) {
                ret = MP_EXPTMOD_E;
            }
        }

        /* Check u <= p. */
        if (ret == 0) {
            if (mp_cmp(&key->u, &key->p) != MP_LT) {
                ret = MP_EXPTMOD_E;
            }
        }
        /* Check u*q mod p = 1. (u = 1/q mod p) */
        if (ret == 0) {
            if (mp_mulmod(&key->u, &key->q, &key->p, tmp) != MP_OKAY) {
                ret = MP_EXPTMOD_E;
            }
        }
        if (ret == 0 ) {
            if (!mp_isone(tmp)) {
                ret = MP_EXPTMOD_E;
            }
        }
    }

    mp_forcezero(tmp);
    mp_clear(tmp);
    mp_clear(k);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(k, NULL, DYNAMIC_TYPE_RSA);
#endif

    return ret;
}