static int wc_RsaFunctionNonBlock(const byte* in, word32 inLen, byte* out,
                          word32* outLen, int type, RsaKey* key)
{
    int    ret = 0;
    word32 keyLen, len;

    if (key == NULL || key->nb == NULL) {
        return BAD_FUNC_ARG;
    }

    if (key->nb->exptmod.state == TFM_EXPTMOD_NB_INIT) {
        if (mp_init(&key->nb->tmp) != MP_OKAY) {
            ret = MP_INIT_E;
        }

        if (ret == 0) {
            if (mp_read_unsigned_bin(&key->nb->tmp, (byte*)in, inLen) != MP_OKAY) {
                ret = MP_READ_E;
            }
        }
    }

    if (ret == 0) {
        switch(type) {
        case RSA_PRIVATE_DECRYPT:
        case RSA_PRIVATE_ENCRYPT:
            ret = fp_exptmod_nb(&key->nb->exptmod, &key->nb->tmp, &key->d,
                &key->n, &key->nb->tmp);
            if (ret == FP_WOULDBLOCK)
                return ret;
            if (ret != MP_OKAY)
                ret = MP_EXPTMOD_E;
            break;

        case RSA_PUBLIC_ENCRYPT:
        case RSA_PUBLIC_DECRYPT:
            ret = fp_exptmod_nb(&key->nb->exptmod, &key->nb->tmp, &key->e,
                &key->n, &key->nb->tmp);
            if (ret == FP_WOULDBLOCK)
                return ret;
            if (ret != MP_OKAY)
                ret = MP_EXPTMOD_E;
            break;
        default:
            ret = RSA_WRONG_TYPE_E;
            break;
        }
    }

    if (ret == 0) {
        keyLen = wc_RsaEncryptSize(key);
        if (keyLen > *outLen)
            ret = RSA_BUFFER_E;
    }
    if (ret == 0) {
        len = mp_unsigned_bin_size(&key->nb->tmp);

        /* pad front w/ zeros to match key length */
        while (len < keyLen) {
            *out++ = 0x00;
            len++;
        }

        *outLen = keyLen;

        /* convert */
        if (mp_to_unsigned_bin(&key->nb->tmp, out) != MP_OKAY) {
             ret = MP_TO_E;
        }
    }

    mp_clear(&key->nb->tmp);

    return ret;
}