qf_parse_fmt_v(regmatch_T *rmp, int midx, qffields_T *fields)
{
    if (rmp->startp[midx] == NULL)
	return QF_FAIL;
    fields->col = (int)atol((char *)rmp->startp[midx]);
    fields->use_viscol = TRUE;
    return QF_OK;
}