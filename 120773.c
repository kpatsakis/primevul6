int VKO_compute_key(unsigned char *shared_key,
                    const EC_POINT *pub_key, const EC_KEY *priv_key,
                    const unsigned char *ukm, const size_t ukm_size,
                    const int vko_dgst_nid)
{
    unsigned char *databuf = NULL;
    BIGNUM *scalar = NULL, *X = NULL, *Y = NULL;
    const EC_GROUP *grp = NULL;
    EC_POINT *pnt = NULL;
    BN_CTX *ctx = NULL;
    EVP_MD_CTX *mdctx = NULL;
    const EVP_MD *md = NULL;
    int buf_len, half_len;
    int ret = 0;

    if ((ctx = BN_CTX_secure_new()) == NULL) {
        GOSTerr(GOST_F_VKO_COMPUTE_KEY, ERR_R_MALLOC_FAILURE);
        return 0;
    }
    BN_CTX_start(ctx);

    md = EVP_get_digestbynid(vko_dgst_nid);
    if (!md) {
        GOSTerr(GOST_F_VKO_COMPUTE_KEY, GOST_R_INVALID_DIGEST_TYPE);
        goto err;
    }

    grp = EC_KEY_get0_group(priv_key);
    scalar = BN_CTX_get(ctx);
    X = BN_CTX_get(ctx);

    if ((Y = BN_CTX_get(ctx)) == NULL
        || (pnt = EC_POINT_new(grp)) == NULL
        || BN_lebin2bn(ukm, ukm_size, scalar) == NULL
        || !BN_mod_mul(scalar, scalar, EC_KEY_get0_private_key(priv_key),
                       EC_GROUP_get0_order(grp), ctx))
        goto err;

#if 0
    /*-
     * These two curves have cofactor 4; the rest have cofactor 1.
     * But currently gost_ec_point_mul takes care of the cofactor clearing,
     * hence this code is not needed.
     */
    switch (EC_GROUP_get_curve_name(grp)) {
        case NID_id_tc26_gost_3410_2012_256_paramSetA:
        case NID_id_tc26_gost_3410_2012_512_paramSetC:
            if (!BN_lshift(scalar, scalar, 2))
                goto err;
            break;
    }
#endif

    if (!gost_ec_point_mul(grp, pnt, NULL, pub_key, scalar, ctx)) {
        GOSTerr(GOST_F_VKO_COMPUTE_KEY, GOST_R_ERROR_POINT_MUL);
        goto err;
    }
    if (!EC_POINT_get_affine_coordinates(grp, pnt, X, Y, ctx)) {
        GOSTerr(GOST_F_VKO_COMPUTE_KEY, ERR_R_EC_LIB);
        goto err;
    }

    half_len = BN_num_bytes(EC_GROUP_get0_field(grp));
    buf_len = 2 * half_len;
    if ((databuf = OPENSSL_malloc(buf_len)) == NULL) {
        GOSTerr(GOST_F_VKO_COMPUTE_KEY, ERR_R_MALLOC_FAILURE);
        goto err;
    }

    /*
     * Serialize elliptic curve point same way as we do it when saving key
     */
    if (BN_bn2lebinpad(X, databuf, half_len) != half_len
        || BN_bn2lebinpad(Y, databuf + half_len, half_len) != half_len)
        goto err;

    if ((mdctx = EVP_MD_CTX_new()) == NULL) {
        GOSTerr(GOST_F_VKO_COMPUTE_KEY, ERR_R_MALLOC_FAILURE);
        goto err;
    }

    if (EVP_MD_CTX_init(mdctx) == 0
        || EVP_DigestInit_ex(mdctx, md, NULL) == 0
        || EVP_DigestUpdate(mdctx, databuf, buf_len) == 0
        || EVP_DigestFinal_ex(mdctx, shared_key, NULL) == 0) {
        GOSTerr(GOST_F_VKO_COMPUTE_KEY, ERR_R_EVP_LIB);
        goto err;
    }

    ret = (EVP_MD_size(md) > 0) ? EVP_MD_size(md) : 0;

 err:
    BN_CTX_end(ctx);
    BN_CTX_free(ctx);
    EC_POINT_free(pnt);
    EVP_MD_CTX_free(mdctx);
    OPENSSL_free(databuf);

    return ret;
}