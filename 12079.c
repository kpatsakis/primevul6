qf_parse_fmt_l(regmatch_T *rmp, int midx, qffields_T *fields)
{
    if (rmp->startp[midx] == NULL)
	return QF_FAIL;
    fields->lnum = atol((char *)rmp->startp[midx]);
    return QF_OK;
}