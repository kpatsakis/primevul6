static WC_INLINE int RsaSizeCheck(int size)
{
    if (size < RSA_MIN_SIZE || size > RSA_MAX_SIZE) {
        return 0;
    }

#ifdef HAVE_FIPS
    /* Key size requirements for CAVP */
    switch (size) {
        case 1024:
        case 2048:
        case 3072:
        case 4096:
            return 1;
    }

    return 0;
#else
    return 1; /* allow unusual key sizes in non FIPS mode */
#endif /* HAVE_FIPS */
}