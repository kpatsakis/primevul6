qf_parse_fmt_r(regmatch_T *rmp, int midx, char_u **tail)
{
    if (rmp->startp[midx] == NULL)
	return QF_FAIL;
    *tail = rmp->startp[midx];
    return QF_OK;
}