static int _CheckProbablePrime(mp_int* p, mp_int* q, mp_int* e, int nlen,
                                    int* isPrime, WC_RNG* rng)
{
    int ret;
    mp_int tmp1, tmp2;
    mp_int* prime;

    if (p == NULL || e == NULL || isPrime == NULL)
        return BAD_FUNC_ARG;

    if (!RsaSizeCheck(nlen))
        return BAD_FUNC_ARG;

    *isPrime = MP_NO;

    if (q != NULL) {
        /* 5.4 - check that |p-q| <= (2^(1/2))(2^((nlen/2)-1)) */
        ret = wc_CompareDiffPQ(p, q, nlen);
        if (ret != MP_OKAY) goto notOkay;
        prime = q;
    }
    else
        prime = p;

    ret = mp_init_multi(&tmp1, &tmp2, NULL, NULL, NULL, NULL);
    if (ret != MP_OKAY) goto notOkay;

    /* 4.4,5.5 - Check that prime >= (2^(1/2))(2^((nlen/2)-1))
     *           This is a comparison against lowerBound */
    ret = mp_read_unsigned_bin(&tmp1, lower_bound, nlen/16);
    if (ret != MP_OKAY) goto notOkay;
    ret = mp_cmp(prime, &tmp1);
    if (ret == MP_LT) goto exit;

    /* 4.5,5.6 - Check that GCD(p-1, e) == 1 */
    ret = mp_sub_d(prime, 1, &tmp1);  /* tmp1 = prime-1 */
    if (ret != MP_OKAY) goto notOkay;
    ret = mp_gcd(&tmp1, e, &tmp2);  /* tmp2 = gcd(prime-1, e) */
    if (ret != MP_OKAY) goto notOkay;
    ret = mp_cmp_d(&tmp2, 1);
    if (ret != MP_EQ) goto exit; /* e divides p-1 */

    /* 4.5.1,5.6.1 - Check primality of p with 8 rounds of M-R.
     * mp_prime_is_prime_ex() performs test divisions against the first 256
     * prime numbers. After that it performs 8 rounds of M-R using random
     * bases between 2 and n-2.
     * mp_prime_is_prime() performs the same test divisions and then does
     * M-R with the first 8 primes. Both functions set isPrime as a
     * side-effect. */
    if (rng != NULL)
        ret = mp_prime_is_prime_ex(prime, 8, isPrime, rng);
    else
        ret = mp_prime_is_prime(prime, 8, isPrime);
    if (ret != MP_OKAY) goto notOkay;

exit:
    ret = MP_OKAY;
notOkay:
    mp_clear(&tmp1);
    mp_clear(&tmp2);
    return ret;
}