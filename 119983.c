int wc_MakeRsaKey(RsaKey* key, int size, long e, WC_RNG* rng)
{
#ifndef WC_NO_RNG
#ifdef WOLFSSL_SMALL_STACK
    mp_int *p = (mp_int *)XMALLOC(sizeof *p, key->heap, DYNAMIC_TYPE_RSA);
    mp_int *q = (mp_int *)XMALLOC(sizeof *q, key->heap, DYNAMIC_TYPE_RSA);
    mp_int *tmp1 = (mp_int *)XMALLOC(sizeof *tmp1, key->heap, DYNAMIC_TYPE_RSA);
    mp_int *tmp2 = (mp_int *)XMALLOC(sizeof *tmp2, key->heap, DYNAMIC_TYPE_RSA);
    mp_int *tmp3 = (mp_int *)XMALLOC(sizeof *tmp3, key->heap, DYNAMIC_TYPE_RSA);
#else
    mp_int p_buf, *p = &p_buf;
    mp_int q_buf, *q = &q_buf;
    mp_int tmp1_buf, *tmp1 = &tmp1_buf;
    mp_int tmp2_buf, *tmp2 = &tmp2_buf;
    mp_int tmp3_buf, *tmp3 = &tmp3_buf;
#endif
    int err, i, failCount, primeSz, isPrime = 0;
    byte* buf = NULL;

#ifdef WOLFSSL_SMALL_STACK
    if ((p == NULL) ||
	(q == NULL) ||
	(tmp1 == NULL) ||
	(tmp2 == NULL) ||
	(tmp3 == NULL)) {
      err = MEMORY_E;
      goto out;
    }
#endif

    if (key == NULL || rng == NULL) {
        err = BAD_FUNC_ARG;
        goto out;
    }

    if (!RsaSizeCheck(size)) {
        err = BAD_FUNC_ARG;
        goto out;
    }

    if (e < 3 || (e & 1) == 0) {
        err = BAD_FUNC_ARG;
        goto out;
    }

#if defined(WOLFSSL_CRYPTOCELL)

    err = cc310_RSA_GenerateKeyPair(key, size, e);
    goto out;

#endif /*WOLFSSL_CRYPTOCELL*/

#ifdef WOLF_CRYPTO_CB
    if (key->devId != INVALID_DEVID) {
        err = wc_CryptoCb_MakeRsaKey(key, size, e, rng);
        if (err != CRYPTOCB_UNAVAILABLE)
            goto out;
        /* fall-through when unavailable */
    }
#endif

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA) && \
    defined(WC_ASYNC_ENABLE_RSA_KEYGEN)
    if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_RSA) {
    #ifdef HAVE_CAVIUM
        /* TODO: Not implemented */
    #elif defined(HAVE_INTEL_QA)
        err = IntelQaRsaKeyGen(&key->asyncDev, key, size, e, rng);
        goto out;
    #else
        if (wc_AsyncTestInit(&key->asyncDev, ASYNC_TEST_RSA_MAKE)) {
            WC_ASYNC_TEST* testDev = &key->asyncDev.test;
            testDev->rsaMake.rng = rng;
            testDev->rsaMake.key = key;
            testDev->rsaMake.size = size;
            testDev->rsaMake.e = e;
            err = WC_PENDING_E;
            goto out;
        }
    #endif
    }
#endif

    err = mp_init_multi(p, q, tmp1, tmp2, tmp3, NULL);

    if (err == MP_OKAY)
        err = mp_set_int(tmp3, e);

    /* The failCount value comes from NIST FIPS 186-4, section B.3.3,
     * process steps 4.7 and 5.8. */
    failCount = 5 * (size / 2);
    primeSz = size / 16; /* size is the size of n in bits.
                            primeSz is in bytes. */

    /* allocate buffer to work with */
    if (err == MP_OKAY) {
        buf = (byte*)XMALLOC(primeSz, key->heap, DYNAMIC_TYPE_RSA);
        if (buf == NULL)
            err = MEMORY_E;
    }

    /* make p */
    if (err == MP_OKAY) {
        isPrime = 0;
        i = 0;
        do {
#ifdef SHOW_GEN
            printf(".");
            fflush(stdout);
#endif
            /* generate value */
            err = wc_RNG_GenerateBlock(rng, buf, primeSz);
            if (err == 0) {
                /* prime lower bound has the MSB set, set it in candidate */
                buf[0] |= 0x80;
                /* make candidate odd */
                buf[primeSz-1] |= 0x01;
                /* load value */
                err = mp_read_unsigned_bin(p, buf, primeSz);
            }

            if (err == MP_OKAY)
                err = _CheckProbablePrime(p, NULL, tmp3, size, &isPrime, rng);

#ifdef HAVE_FIPS
            i++;
#else
            /* Keep the old retry behavior in non-FIPS build. */
            (void)i;
#endif
        } while (err == MP_OKAY && !isPrime && i < failCount);
    }

    if (err == MP_OKAY && !isPrime)
        err = PRIME_GEN_E;

    /* make q */
    if (err == MP_OKAY) {
        isPrime = 0;
        i = 0;
        do {
#ifdef SHOW_GEN
            printf(".");
            fflush(stdout);
#endif
            /* generate value */
            err = wc_RNG_GenerateBlock(rng, buf, primeSz);
            if (err == 0) {
                /* prime lower bound has the MSB set, set it in candidate */
                buf[0] |= 0x80;
                /* make candidate odd */
                buf[primeSz-1] |= 0x01;
                /* load value */
                err = mp_read_unsigned_bin(q, buf, primeSz);
            }

            if (err == MP_OKAY)
                err = _CheckProbablePrime(p, q, tmp3, size, &isPrime, rng);

#ifdef HAVE_FIPS
            i++;
#else
            /* Keep the old retry behavior in non-FIPS build. */
            (void)i;
#endif
        } while (err == MP_OKAY && !isPrime && i < failCount);
    }

    if (err == MP_OKAY && !isPrime)
        err = PRIME_GEN_E;

    if (buf) {
        ForceZero(buf, primeSz);
        XFREE(buf, key->heap, DYNAMIC_TYPE_RSA);
    }

    if (err == MP_OKAY && mp_cmp(p, q) < 0) {
        err = mp_copy(p, tmp1);
        if (err == MP_OKAY)
            err = mp_copy(q, p);
        if (err == MP_OKAY)
            mp_copy(tmp1, q);
    }

    /* Setup RsaKey buffers */
    if (err == MP_OKAY)
        err = mp_init_multi(&key->n, &key->e, &key->d, &key->p, &key->q, NULL);
    if (err == MP_OKAY)
        err = mp_init_multi(&key->dP, &key->dQ, &key->u, NULL, NULL, NULL);

    /* Software Key Calculation */
    if (err == MP_OKAY)                /* tmp1 = p-1 */
        err = mp_sub_d(p, 1, tmp1);
    if (err == MP_OKAY)                /* tmp2 = q-1 */
        err = mp_sub_d(q, 1, tmp2);
#ifdef WC_RSA_BLINDING
    if (err == MP_OKAY)                /* tmp3 = order of n */
        err = mp_mul(tmp1, tmp2, tmp3);
#else
    if (err == MP_OKAY)                /* tmp3 = lcm(p-1, q-1), last loop */
        err = mp_lcm(tmp1, tmp2, tmp3);
#endif
    /* make key */
    if (err == MP_OKAY)                /* key->e = e */
        err = mp_set_int(&key->e, (mp_digit)e);
#ifdef WC_RSA_BLINDING
    /* Blind the inverse operation with a value that is invertable */
    if (err == MP_OKAY) {
        do {
            err = mp_rand(&key->p, get_digit_count(tmp3), rng);
            if (err == MP_OKAY)
                err = mp_set_bit(&key->p, 0);
            if (err == MP_OKAY)
                err = mp_set_bit(&key->p, size - 1);
            if (err == MP_OKAY)
                err = mp_gcd(&key->p, tmp3, &key->q);
        }
        while ((err == MP_OKAY) && !mp_isone(&key->q));
    }
    if (err == MP_OKAY)
        err = mp_mul_d(&key->p, (mp_digit)e, &key->e);
#endif
    if (err == MP_OKAY)                /* key->d = 1/e mod lcm(p-1, q-1) */
        err = mp_invmod(&key->e, tmp3, &key->d);
#ifdef WC_RSA_BLINDING
    /* Take off blinding from d and reset e */
    if (err == MP_OKAY)
        err = mp_mulmod(&key->d, &key->p, tmp3, &key->d);
    if (err == MP_OKAY)
        err = mp_set_int(&key->e, (mp_digit)e);
#endif
    if (err == MP_OKAY)                /* key->n = pq */
        err = mp_mul(p, q, &key->n);
    if (err == MP_OKAY)                /* key->dP = d mod(p-1) */
        err = mp_mod(&key->d, tmp1, &key->dP);
    if (err == MP_OKAY)                /* key->dQ = d mod(q-1) */
        err = mp_mod(&key->d, tmp2, &key->dQ);
#ifdef WOLFSSL_MP_INVMOD_CONSTANT_TIME
    if (err == MP_OKAY)                /* key->u = 1/q mod p */
        err = mp_invmod(q, p, &key->u);
#else
    if (err == MP_OKAY)
        err = mp_sub_d(p, 2, tmp3);
    if (err == MP_OKAY)                /* key->u = 1/q mod p = q^p-2 mod p */
        err = mp_exptmod(q, tmp3 , p, &key->u);
#endif
    if (err == MP_OKAY)
        err = mp_copy(p, &key->p);
    if (err == MP_OKAY)
        err = mp_copy(q, &key->q);

#ifdef HAVE_WOLF_BIGINT
    /* make sure raw unsigned bin version is available */
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(&key->n, &key->n.raw);
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(&key->e, &key->e.raw);
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(&key->d, &key->d.raw);
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(&key->p, &key->p.raw);
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(&key->q, &key->q.raw);
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(&key->dP, &key->dP.raw);
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(&key->dQ, &key->dQ.raw);
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(&key->u, &key->u.raw);
#endif

    if (err == MP_OKAY)
        key->type = RSA_PRIVATE;

    mp_clear(tmp1);
    mp_clear(tmp2);
    mp_clear(tmp3);
    mp_clear(p);
    mp_clear(q);

#if defined(WOLFSSL_KEY_GEN) && !defined(WOLFSSL_NO_RSA_KEY_CHECK)
    /* Perform the pair-wise consistency test on the new key. */
    if (err == 0)
        err = wc_CheckRsaKey(key);
#endif

    if (err != 0) {
        wc_FreeRsaKey(key);
        goto out;
    }

#if defined(WOLFSSL_XILINX_CRYPT) || defined(WOLFSSL_CRYPTOCELL)
    if (wc_InitRsaHw(key) != 0) {
        return BAD_STATE_E;
    }
#endif

    err = 0;

  out:

#ifdef WOLFSSL_SMALL_STACK
    if (p)
        XFREE(p, key->heap, DYNAMIC_TYPE_RSA);
    if (q)
        XFREE(q, key->heap, DYNAMIC_TYPE_RSA);
    if (tmp1)
        XFREE(tmp1, key->heap, DYNAMIC_TYPE_RSA);
    if (tmp2)
        XFREE(tmp2, key->heap, DYNAMIC_TYPE_RSA);
    if (tmp3)
        XFREE(tmp3, key->heap, DYNAMIC_TYPE_RSA);
#endif

    return err;
#else
    return NOT_COMPILED_IN;
#endif
}