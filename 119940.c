static int wc_RsaFunctionSync(const byte* in, word32 inLen, byte* out,
                          word32* outLen, int type, RsaKey* key, WC_RNG* rng)
{
    struct msghdr   msg;
    struct cmsghdr* cmsg;
    struct iovec      iov;
    byte*  keyBuf   = NULL;
    word32 keyBufSz = 0;
    char cbuf[CMSG_SPACE(4) + CMSG_SPACE(sizeof(struct af_alg_iv) + 1)] = {0};
    int    ret = 0;
    int    op  = 0;    /* decryption vs encryption flag */
    word32 keyLen;

    /* input and output buffer need to be aligned */
    ALIGN64 byte outBuf[MAX_XILINX_RSA_KEY];
    ALIGN64 byte inBuf[MAX_XILINX_RSA_KEY];

    XMEMSET(&msg, 0, sizeof(struct msghdr));
    (void)rng;

    keyLen = wc_RsaEncryptSize(key);
    if (keyLen > *outLen) {
        ERROR_OUT(RSA_BUFFER_E);
    }

    if (keyLen > MAX_XILINX_RSA_KEY) {
        WOLFSSL_MSG("RSA key size larger than supported");
        ERROR_OUT(BAD_FUNC_ARG);
    }

    if ((keyBuf = (byte*)XMALLOC(keyLen * 2, key->heap, DYNAMIC_TYPE_KEY))
            == NULL) {
        ERROR_OUT(MEMORY_E);
    }

    if ((ret = mp_to_unsigned_bin(&(key->n), keyBuf)) != MP_OKAY) {
        ERROR_OUT(MP_TO_E);
    }

    switch(type) {
        case RSA_PRIVATE_DECRYPT:
        case RSA_PRIVATE_ENCRYPT:
            op = 1; /* set as decrypt */
            {
                keyBufSz = mp_unsigned_bin_size(&(key->d));
                if ((mp_to_unsigned_bin(&(key->d), keyBuf + keyLen))
                        != MP_OKAY) {
                    ERROR_OUT(MP_TO_E);
                }
            }
            break;

        case RSA_PUBLIC_DECRYPT:
        case RSA_PUBLIC_ENCRYPT: {
            word32 exp = 0;
            word32 eSz = mp_unsigned_bin_size(&(key->e));
            if ((mp_to_unsigned_bin(&(key->e), (byte*)&exp +
                            (sizeof(word32) - eSz))) != MP_OKAY) {
                ERROR_OUT(MP_TO_E);
            }
            keyBufSz = sizeof(word32);
            XMEMCPY(keyBuf + keyLen, (byte*)&exp, keyBufSz);
            break;
        }

        default:
            ERROR_OUT(RSA_WRONG_TYPE_E);
    }
    keyBufSz += keyLen; /* add size of modulus */

    /* check for existing sockets before creating new ones */
    if (key->alFd > 0) {
        close(key->alFd);
        key->alFd = WC_SOCK_NOTSET;
    }
    if (key->rdFd > 0) {
        close(key->rdFd);
        key->rdFd = WC_SOCK_NOTSET;
    }

    /* create new sockets and set the key to use */
    if ((key->alFd = wc_Afalg_Socket()) < 0) {
        WOLFSSL_MSG("Unable to create socket");
        ERROR_OUT(key->alFd);
    }
    if ((key->rdFd = wc_Afalg_CreateRead(key->alFd, WC_TYPE_ASYMKEY,
                    WC_NAME_RSA)) < 0) {
        WOLFSSL_MSG("Unable to bind and create read/send socket");
        ERROR_OUT(key->rdFd);
    }
    if ((ret = setsockopt(key->alFd, SOL_ALG, ALG_SET_KEY, keyBuf,
                    keyBufSz)) < 0) {
        WOLFSSL_MSG("Error setting RSA key");
        ERROR_OUT(ret);
    }

    msg.msg_control    = cbuf;
    msg.msg_controllen = sizeof(cbuf);
    cmsg = CMSG_FIRSTHDR(&msg);
    if ((ret = wc_Afalg_SetOp(cmsg, op)) < 0) {
        ERROR_OUT(ret);
    }

    /* set flag in IV spot, needed for Xilinx hardware acceleration use */
    cmsg = CMSG_NXTHDR(&msg, cmsg);
    if ((ret = wc_Afalg_SetIv(cmsg, (byte*)XILINX_RSA_FLAG,
                    sizeof(XILINX_RSA_FLAG))) != 0) {
        ERROR_OUT(ret);
    }

    /* compose and send msg */
    XMEMCPY(inBuf, (byte*)in, inLen); /* for alignment */
    iov.iov_base = inBuf;
    iov.iov_len  = inLen;
    msg.msg_iov  = &iov;
    msg.msg_iovlen = 1;
    if ((ret = sendmsg(key->rdFd, &msg, 0)) <= 0) {
        ERROR_OUT(WC_AFALG_SOCK_E);
    }

    if ((ret = read(key->rdFd, outBuf, inLen)) <= 0) {
        ERROR_OUT(WC_AFALG_SOCK_E);
    }
    XMEMCPY(out, outBuf, ret);
    *outLen = keyLen;

done:
    /* clear key data and free buffer */
    if (keyBuf != NULL) {
        ForceZero(keyBuf, keyBufSz);
    }
    XFREE(keyBuf, key->heap, DYNAMIC_TYPE_KEY);

    if (key->alFd > 0) {
        close(key->alFd);
        key->alFd = WC_SOCK_NOTSET;
    }
    if (key->rdFd > 0) {
        close(key->rdFd);
        key->rdFd = WC_SOCK_NOTSET;
    }

    return ret;
}