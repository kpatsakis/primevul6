static int dsa_sign_setup(DSA *dsa, BN_CTX *ctx_in, BIGNUM **kinvp,
                          BIGNUM **rp)
{
    BN_CTX *ctx;
    BIGNUM k, kq, *K, *kinv = NULL, *r = NULL;
    int ret = 0;

    if (!dsa->p || !dsa->q || !dsa->g) {
        DSAerr(DSA_F_DSA_SIGN_SETUP, DSA_R_MISSING_PARAMETERS);
        return 0;
    }

    BN_init(&k);
    BN_init(&kq);

    if (ctx_in == NULL) {
        if ((ctx = BN_CTX_new()) == NULL)
            goto err;
    } else
        ctx = ctx_in;

    if ((r = BN_new()) == NULL)
        goto err;

    /* Get random k */
    do
        if (!BN_rand_range(&k, dsa->q))
            goto err;
    while (BN_is_zero(&k)) ;

    if (dsa->flags & DSA_FLAG_CACHE_MONT_P) {
        if (!BN_MONT_CTX_set_locked(&dsa->method_mont_p,
                                    CRYPTO_LOCK_DSA, dsa->p, ctx))
            goto err;
    }

    /* Compute r = (g^k mod p) mod q */

    if ((dsa->flags & DSA_FLAG_NO_EXP_CONSTTIME) == 0) {
        if (!BN_copy(&kq, &k))
            goto err;

        /*
         * We do not want timing information to leak the length of k, so we
         * compute g^k using an equivalent exponent of fixed length. (This
         * is a kludge that we need because the BN_mod_exp_mont() does not
         * let us specify the desired timing behaviour.)
         */

        if (!BN_add(&kq, &kq, dsa->q))
            goto err;
        if (BN_num_bits(&kq) <= BN_num_bits(dsa->q)) {
            if (!BN_add(&kq, &kq, dsa->q))
                goto err;
        }

        K = &kq;

        BN_set_flags(K, BN_FLG_CONSTTIME);
    } else {
        K = &k;
    }

    DSA_BN_MOD_EXP(goto err, dsa, r, dsa->g, K, dsa->p, ctx,
                   dsa->method_mont_p);
    if (!BN_mod(r, r, dsa->q, ctx))
        goto err;

    /* Compute  part of 's = inv(k) (m + xr) mod q' */
    if ((kinv = BN_mod_inverse(NULL, &k, dsa->q, ctx)) == NULL)
        goto err;

    if (*kinvp != NULL)
        BN_clear_free(*kinvp);
    *kinvp = kinv;
    kinv = NULL;
    if (*rp != NULL)
        BN_clear_free(*rp);
    *rp = r;
    ret = 1;
 err:
    if (!ret) {
        DSAerr(DSA_F_DSA_SIGN_SETUP, ERR_R_BN_LIB);
        if (r != NULL)
            BN_clear_free(r);
    }
    if (ctx_in == NULL)
        BN_CTX_free(ctx);
    BN_clear_free(&k);
    BN_clear_free(&kq);
    return (ret);
}