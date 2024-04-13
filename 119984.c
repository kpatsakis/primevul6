int wc_CheckProbablePrime(const byte* pRaw, word32 pRawSz,
                          const byte* qRaw, word32 qRawSz,
                          const byte* eRaw, word32 eRawSz,
                          int nlen, int* isPrime)
{
    return wc_CheckProbablePrime_ex(pRaw, pRawSz, qRaw, qRawSz,
                          eRaw, eRawSz, nlen, isPrime, NULL);
}