static int dsa_priv_decode(EVP_PKEY *pkey, PKCS8_PRIV_KEY_INFO *p8)
{
    const unsigned char *p, *pm;
    int pklen, pmlen;
    int ptype;
    void *pval;
    ASN1_STRING *pstr;
    X509_ALGOR *palg;
    ASN1_INTEGER *privkey = NULL;
    BN_CTX *ctx = NULL;

    STACK_OF(ASN1_TYPE) *ndsa = NULL;
    DSA *dsa = NULL;

    if (!PKCS8_pkey_get0(NULL, &p, &pklen, &palg, p8))
        return 0;
    X509_ALGOR_get0(NULL, &ptype, &pval, palg);

    /* Check for broken DSA PKCS#8, UGH! */
    if (*p == (V_ASN1_SEQUENCE | V_ASN1_CONSTRUCTED)) {
        ASN1_TYPE *t1, *t2;
        if ((ndsa = d2i_ASN1_SEQUENCE_ANY(NULL, &p, pklen)) == NULL)
            goto decerr;
        if (sk_ASN1_TYPE_num(ndsa) != 2)
            goto decerr;
        /*-
         * Handle Two broken types:
         * SEQUENCE {parameters, priv_key}
         * SEQUENCE {pub_key, priv_key}
         */

        t1 = sk_ASN1_TYPE_value(ndsa, 0);
        t2 = sk_ASN1_TYPE_value(ndsa, 1);
        if (t1->type == V_ASN1_SEQUENCE) {
            p8->broken = PKCS8_EMBEDDED_PARAM;
            pval = t1->value.ptr;
        } else if (ptype == V_ASN1_SEQUENCE)
            p8->broken = PKCS8_NS_DB;
        else
            goto decerr;

        if (t2->type != V_ASN1_INTEGER)
            goto decerr;

        privkey = t2->value.integer;
    } else {
        const unsigned char *q = p;
        if ((privkey = d2i_ASN1_INTEGER(NULL, &p, pklen)) == NULL)
            goto decerr;
        if (privkey->type == V_ASN1_NEG_INTEGER) {
            p8->broken = PKCS8_NEG_PRIVKEY;
            ASN1_STRING_clear_free(privkey);
            if ((privkey = d2i_ASN1_UINTEGER(NULL, &q, pklen)) == NULL)
                goto decerr;
        }
        if (ptype != V_ASN1_SEQUENCE)
            goto decerr;
    }

    pstr = pval;
    pm = pstr->data;
    pmlen = pstr->length;
    if ((dsa = d2i_DSAparams(NULL, &pm, pmlen)) == NULL)
        goto decerr;
    /* We have parameters now set private key */
    if ((dsa->priv_key = BN_secure_new()) == NULL
        || !ASN1_INTEGER_to_BN(privkey, dsa->priv_key)) {
        DSAerr(DSA_F_DSA_PRIV_DECODE, DSA_R_BN_ERROR);
        goto dsaerr;
    }
    /* Calculate public key */
    if ((dsa->pub_key = BN_new()) == NULL) {
        DSAerr(DSA_F_DSA_PRIV_DECODE, ERR_R_MALLOC_FAILURE);
        goto dsaerr;
    }
    if ((ctx = BN_CTX_new()) == NULL) {
        DSAerr(DSA_F_DSA_PRIV_DECODE, ERR_R_MALLOC_FAILURE);
        goto dsaerr;
    }

    if (!BN_mod_exp(dsa->pub_key, dsa->g, dsa->priv_key, dsa->p, ctx)) {
        DSAerr(DSA_F_DSA_PRIV_DECODE, DSA_R_BN_ERROR);
        goto dsaerr;
    }

    EVP_PKEY_assign_DSA(pkey, dsa);
    BN_CTX_free(ctx);
    if (ndsa)
        sk_ASN1_TYPE_pop_free(ndsa, ASN1_TYPE_free);
    else
        ASN1_STRING_clear_free(privkey);

    return 1;

 decerr:
    DSAerr(DSA_F_DSA_PRIV_DECODE, DSA_R_DECODE_ERROR);
 dsaerr:
    BN_CTX_free(ctx);
    ASN1_STRING_clear_free(privkey);
    sk_ASN1_TYPE_pop_free(ndsa, ASN1_TYPE_free);
    DSA_free(dsa);
    return 0;
}