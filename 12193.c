qf_parse_fmt_t(regmatch_T *rmp, int midx, qffields_T *fields)
{
    if (rmp->startp[midx] == NULL)
	return QF_FAIL;
    fields->type = *rmp->startp[midx];
    return QF_OK;
}