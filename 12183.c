qf_parse_fmt_n(regmatch_T *rmp, int midx, qffields_T *fields)
{
    if (rmp->startp[midx] == NULL)
	return QF_FAIL;
    fields->enr = (int)atol((char *)rmp->startp[midx]);
    return QF_OK;
}