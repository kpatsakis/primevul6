int pkey_gost_ec_derive(EVP_PKEY_CTX *ctx, unsigned char *key, size_t *keylen)
{
    /*
     * Public key of peer in the ctx field peerkey
     * Our private key in the ctx pkey
     * ukm is in the algorithm specific context data
     */
    EVP_PKEY *my_key = EVP_PKEY_CTX_get0_pkey(ctx);
    EVP_PKEY *peer_key = EVP_PKEY_CTX_get0_peerkey(ctx);
    struct gost_pmeth_data *data = EVP_PKEY_CTX_get_data(ctx);
    int dgst_nid = NID_undef;

    if (!data || data->shared_ukm_size == 0) {
        GOSTerr(GOST_F_PKEY_GOST_EC_DERIVE, GOST_R_UKM_NOT_SET);
        return 0;
    }

    /* VKO */
    if (data->vko_dgst_nid) {
        if (!key) {
            *keylen = data->vko_dgst_nid == NID_id_GostR3411_2012_256? 32 : 64;
            return 1;
        }
        *keylen = VKO_compute_key(key,
                                  EC_KEY_get0_public_key(EVP_PKEY_get0(peer_key)),
                                  (EC_KEY *)EVP_PKEY_get0(my_key),
                                  data->shared_ukm, data->shared_ukm_size,
                                  data->vko_dgst_nid);
        return (*keylen) ? 1 : 0;
    }

    /*
     * shared_ukm_size = 8 stands for pre-2018 cipher suites
     * It means 32 bytes of key length, 8 byte UKM, 32-bytes hash
     *
     * shared_ukm_size = 32 stands for post-2018 cipher suites
     * It means 64 bytes of shared_key, 16 bytes of UKM and either
     * 64 bytes of hash or 64 bytes of TLSTREE output
     * */

    switch (data->shared_ukm_size) {
    case 8:
        {
            if (key == NULL) {
                *keylen = 32;
                return 1;
            }
            EVP_PKEY_get_default_digest_nid(my_key, &dgst_nid);
            if (dgst_nid == NID_id_GostR3411_2012_512)
                dgst_nid = NID_id_GostR3411_2012_256;

            *keylen =
                VKO_compute_key(key,
                                EC_KEY_get0_public_key(EVP_PKEY_get0(peer_key)),
                                (EC_KEY *)EVP_PKEY_get0(my_key),
                                data->shared_ukm, 8, dgst_nid);
            return (*keylen) ? 1 : 0;
        }
        break;
    case 32:
        {
            if (key == NULL) {
                *keylen = 64;
                return 1;
            }

            *keylen = gost_keg(data->shared_ukm, EVP_PKEY_id(my_key),
                               EC_KEY_get0_public_key(EVP_PKEY_get0(peer_key)),
                               (EC_KEY *)EVP_PKEY_get0(my_key), key);
            return (*keylen) ? 1 : 0;
        }

        break;
    default:
        return 0;
    }
}