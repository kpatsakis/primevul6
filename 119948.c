int wc_RsaSetNonBlockTime(RsaKey* key, word32 maxBlockUs, word32 cpuMHz)
{
    if (key == NULL || key->nb == NULL) {
        return BAD_FUNC_ARG;
    }

    /* calculate maximum number of instructions to block */
    key->nb->exptmod.maxBlockInst = cpuMHz * maxBlockUs;

    return 0;
}