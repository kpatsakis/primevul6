int wc_InitRsaHw(RsaKey* key)
{
    unsigned char* m; /* RSA modulous */
    word32 e = 0;     /* RSA public exponent */
    int mSz;
    int eSz;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    mSz = mp_unsigned_bin_size(&(key->n));
    m = (unsigned char*)XMALLOC(mSz, key->heap, DYNAMIC_TYPE_KEY);
    if (m == NULL) {
        return MEMORY_E;
    }

    if (mp_to_unsigned_bin(&(key->n), m) != MP_OKAY) {
        WOLFSSL_MSG("Unable to get RSA key modulus");
        XFREE(m, key->heap, DYNAMIC_TYPE_KEY);
        return MP_READ_E;
    }

    eSz = mp_unsigned_bin_size(&(key->e));
    if (eSz > MAX_E_SIZE) {
        WOLFSSL_MSG("Exponent of size 4 bytes expected");
        XFREE(m, key->heap, DYNAMIC_TYPE_KEY);
        return BAD_FUNC_ARG;
    }

    if (mp_to_unsigned_bin(&(key->e), (byte*)&e + (MAX_E_SIZE - eSz))
                != MP_OKAY) {
        XFREE(m, key->heap, DYNAMIC_TYPE_KEY);
        WOLFSSL_MSG("Unable to get RSA key exponent");
        return MP_READ_E;
    }

    /* check for existing mod buffer to avoid memory leak */
    if (key->mod != NULL) {
        XFREE(key->mod, key->heap, DYNAMIC_TYPE_KEY);
    }

    key->pubExp = e;
    key->mod    = m;

    if (XSecure_RsaInitialize(&(key->xRsa), key->mod, NULL,
                (byte*)&(key->pubExp)) != XST_SUCCESS) {
        WOLFSSL_MSG("Unable to initialize RSA on hardware");
        XFREE(m, key->heap, DYNAMIC_TYPE_KEY);
        return BAD_STATE_E;
    }

#ifdef WOLFSSL_XILINX_PATCH
   /* currently a patch of xsecure_rsa.c for 2048 bit keys */
   if (wc_RsaEncryptSize(key) == 256) {
       if (XSecure_RsaSetSize(&(key->xRsa), 2048) != XST_SUCCESS) {
           WOLFSSL_MSG("Unable to set RSA key size on hardware");
           XFREE(m, key->heap, DYNAMIC_TYPE_KEY);
           return BAD_STATE_E;
       }
   }
#endif
    return 0;
} /* WOLFSSL_XILINX_CRYPT*/