static int RsaUnPad(const byte *pkcsBlock, unsigned int pkcsBlockLen,
                    byte **output, byte padValue)
{
    int    ret = BAD_FUNC_ARG;
    word16 i;
#ifndef WOLFSSL_RSA_VERIFY_ONLY
    byte   invalid = 0;
#endif

    if (output == NULL || pkcsBlockLen < 2 || pkcsBlockLen > 0xFFFF) {
        return BAD_FUNC_ARG;
    }

    if (padValue == RSA_BLOCK_TYPE_1) {
        /* First byte must be 0x00 and Second byte, block type, 0x01 */
        if (pkcsBlock[0] != 0 || pkcsBlock[1] != RSA_BLOCK_TYPE_1) {
            WOLFSSL_MSG("RsaUnPad error, invalid formatting");
            return RSA_PAD_E;
        }

        /* check the padding until we find the separator */
        for (i = 2; i < pkcsBlockLen && pkcsBlock[i++] == 0xFF; ) { }

        /* Minimum of 11 bytes of pre-message data and must have separator. */
        if (i < RSA_MIN_PAD_SZ || pkcsBlock[i-1] != 0) {
            WOLFSSL_MSG("RsaUnPad error, bad formatting");
            return RSA_PAD_E;
        }

        *output = (byte *)(pkcsBlock + i);
        ret = pkcsBlockLen - i;
    }
#ifndef WOLFSSL_RSA_VERIFY_ONLY
    else {
        word16 j;
        word16 pastSep = 0;

        /* Decrypted with private key - unpad must be constant time. */
        for (i = 0, j = 2; j < pkcsBlockLen; j++) {
           /* Update i if not passed the separator and at separator. */
            i |= (~pastSep) & ctMask16Eq(pkcsBlock[j], 0x00) & (j + 1);
            pastSep |= ctMask16Eq(pkcsBlock[j], 0x00);
        }

        /* Minimum of 11 bytes of pre-message data - including leading 0x00. */
        invalid |= ctMaskLT(i, RSA_MIN_PAD_SZ);
        /* Must have seen separator. */
        invalid |= ~pastSep;
        /* First byte must be 0x00. */
        invalid |= ctMaskNotEq(pkcsBlock[0], 0x00);
        /* Check against expected block type: padValue */
        invalid |= ctMaskNotEq(pkcsBlock[1], padValue);

        *output = (byte *)(pkcsBlock + i);
        ret = ((int)~invalid) & (pkcsBlockLen - i);
    }
#endif

    return ret;
}