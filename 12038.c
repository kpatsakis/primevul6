efm_regpat_bufsz(char_u *efm)
{
    int sz;
    int i;

    sz = (FMT_PATTERNS * 3) + ((int)STRLEN(efm) << 2);
    for (i = FMT_PATTERNS; i > 0; )
	sz += (int)STRLEN(fmt_pat[--i].pattern);
#ifdef BACKSLASH_IN_FILENAME
    sz += 12; // "%f" can become twelve chars longer (see efm_to_regpat)
#else
    sz += 2; // "%f" can become two chars longer
#endif

    return sz;
}