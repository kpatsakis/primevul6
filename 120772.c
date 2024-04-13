static int gost_keg(const unsigned char *ukm_source, int pkey_nid,
                    const EC_POINT *pub_key, const EC_KEY *priv_key,
                    unsigned char *keyout)
{
/* Adjust UKM */
    unsigned char real_ukm[16];
    size_t keylen = 0;

    memset(real_ukm, 0, 16);
    if (memcmp(ukm_source, real_ukm, 16) == 0)
        real_ukm[15] = 1;
    else {
        memcpy(real_ukm, ukm_source, 16);
        BUF_reverse(real_ukm, NULL, 16);
    }

    switch (pkey_nid) {
    case NID_id_GostR3410_2012_512:
        keylen =
            VKO_compute_key(keyout, pub_key, priv_key, real_ukm, 16,
                            NID_id_GostR3411_2012_512);
        return (keylen) ? keylen : 0;
        break;

    case NID_id_GostR3410_2012_256:
        {
            unsigned char tmpkey[32];
            keylen =
              VKO_compute_key(tmpkey, pub_key, priv_key, real_ukm, 16,
                  NID_id_GostR3411_2012_256);

            if (keylen == 0)
                return 0;

            if (gost_kdftree2012_256
                (keyout, 64, tmpkey, 32, (const unsigned char *)"kdf tree", 8,
                 ukm_source + 16, 8, 1) > 0)
                keylen = 64;
            else
                keylen = 0;

            OPENSSL_cleanse(tmpkey, 32);
            return (keylen) ? keylen : 0;
        }
    default:
        return 0;
    }
}