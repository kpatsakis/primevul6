qf_parse_fmt_f(regmatch_T *rmp, int midx, qffields_T *fields, int prefix)
{
    int c;

    if (rmp->startp[midx] == NULL || rmp->endp[midx] == NULL)
	return QF_FAIL;

    // Expand ~/file and $HOME/file to full path.
    c = *rmp->endp[midx];
    *rmp->endp[midx] = NUL;
    expand_env(rmp->startp[midx], fields->namebuf, CMDBUFFSIZE);
    *rmp->endp[midx] = c;

    // For separate filename patterns (%O, %P and %Q), the specified file
    // should exist.
    if (vim_strchr((char_u *)"OPQ", prefix) != NULL
	    && mch_getperm(fields->namebuf) == -1)
	return QF_FAIL;

    return QF_OK;
}