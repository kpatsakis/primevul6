qf_parse_fmt_o(regmatch_T *rmp, int midx, qffields_T *fields)
{
    int		len;

    if (rmp->startp[midx] == NULL || rmp->endp[midx] == NULL)
	return QF_FAIL;
    len = (int)(rmp->endp[midx] - rmp->startp[midx]);
    if (len > CMDBUFFSIZE)
	len = CMDBUFFSIZE;
    STRNCAT(fields->module, rmp->startp[midx], len);
    return QF_OK;
}