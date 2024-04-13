qf_parse_fmt_m(regmatch_T *rmp, int midx, qffields_T *fields)
{
    char_u	*p;
    int		len;

    if (rmp->startp[midx] == NULL || rmp->endp[midx] == NULL)
	return QF_FAIL;
    len = (int)(rmp->endp[midx] - rmp->startp[midx]);
    if (len >= fields->errmsglen)
    {
	// len + null terminator
	if ((p = vim_realloc(fields->errmsg, len + 1)) == NULL)
	    return QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = len + 1;
    }
    vim_strncpy(fields->errmsg, rmp->startp[midx], len);
    return QF_OK;
}