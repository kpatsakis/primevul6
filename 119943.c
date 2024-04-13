static int RsaUnPad_OAEP(byte *pkcsBlock, unsigned int pkcsBlockLen,
                            byte **output, enum wc_HashType hType, int mgf,
                            byte* optLabel, word32 labelLen, void* heap)
{
    int hLen;
    int ret;
    byte h[WC_MAX_DIGEST_SIZE]; /* max digest size */
    byte* tmp;
    word32 idx;

    /* no label is allowed, but catch if no label provided and length > 0 */
    if (optLabel == NULL && labelLen > 0) {
        return BUFFER_E;
    }

    hLen = wc_HashGetDigestSize(hType);
    if ((hLen < 0) || (pkcsBlockLen < (2 * (word32)hLen + 2))) {
        return BAD_FUNC_ARG;
    }

    tmp = (byte*)XMALLOC(pkcsBlockLen, heap, DYNAMIC_TYPE_RSA_BUFFER);
    if (tmp == NULL) {
        return MEMORY_E;
    }
    XMEMSET(tmp, 0, pkcsBlockLen);

    /* find seedMask value */
    if ((ret = RsaMGF(mgf, (byte*)(pkcsBlock + (hLen + 1)),
                            pkcsBlockLen - hLen - 1, tmp, hLen, heap)) != 0) {
        XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
        return ret;
    }

    /* xor seedMask value with maskedSeed to get seed value */
    for (idx = 0; idx < (word32)hLen; idx++) {
        tmp[idx] = tmp[idx] ^ pkcsBlock[1 + idx];
    }

    /* get dbMask value */
    if ((ret = RsaMGF(mgf, tmp, hLen, tmp + hLen,
                                       pkcsBlockLen - hLen - 1, heap)) != 0) {
        XFREE(tmp, NULL, DYNAMIC_TYPE_RSA_BUFFER);
        return ret;
    }

    /* get DB value by doing maskedDB xor dbMask */
    for (idx = 0; idx < (pkcsBlockLen - hLen - 1); idx++) {
        pkcsBlock[hLen + 1 + idx] = pkcsBlock[hLen + 1 + idx] ^ tmp[idx + hLen];
    }

    /* done with use of tmp buffer */
    XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);

    /* advance idx to index of PS and msg separator, account for PS size of 0*/
    idx = hLen + 1 + hLen;
    while (idx < pkcsBlockLen && pkcsBlock[idx] == 0) {idx++;}

    /* create hash of label for comparison with hash sent */
    if ((ret = wc_Hash(hType, optLabel, labelLen, h, hLen)) != 0) {
        return ret;
    }

    /* say no to chosen ciphertext attack.
       Comparison of lHash, Y, and separator value needs to all happen in
       constant time.
       Attackers should not be able to get error condition from the timing of
       these checks.
     */
    ret = 0;
    ret |= ConstantCompare(pkcsBlock + hLen + 1, h, hLen);
    ret += pkcsBlock[idx++] ^ 0x01; /* separator value is 0x01 */
    ret += pkcsBlock[0]     ^ 0x00; /* Y, the first value, should be 0 */

    /* Return 0 data length on error. */
    idx = ctMaskSelInt(ctMaskEq(ret, 0), idx, pkcsBlockLen);

    /* adjust pointer to correct location in array and return size of M */
    *output = (byte*)(pkcsBlock + idx);
    return pkcsBlockLen - idx;
}