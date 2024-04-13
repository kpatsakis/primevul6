qf_parse_fmt_c(regmatch_T *rmp, int midx, qffields_T *fields)
{
    if (rmp->startp[midx] == NULL)
	return QF_FAIL;
    fields->col = (int)atol((char *)rmp->startp[midx]);
    return QF_OK;
}