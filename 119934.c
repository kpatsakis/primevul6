int wc_RsaSetRNG(RsaKey* key, WC_RNG* rng)
{
    if (key == NULL)
        return BAD_FUNC_ARG;

    key->rng = rng;

    return 0;
}