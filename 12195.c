qf_parse_fmt_s(regmatch_T *rmp, int midx, qffields_T *fields)
{
    int		len;

    if (rmp->startp[midx] == NULL || rmp->endp[midx] == NULL)
	return QF_FAIL;
    len = (int)(rmp->endp[midx] - rmp->startp[midx]);
    if (len > CMDBUFFSIZE - 5)
	len = CMDBUFFSIZE - 5;
    STRCPY(fields->pattern, "^\\V");
    STRNCAT(fields->pattern, rmp->startp[midx], len);
    fields->pattern[len + 3] = '\\';
    fields->pattern[len + 4] = '$';
    fields->pattern[len + 5] = NUL;
    return QF_OK;
}