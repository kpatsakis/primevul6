qf_parse_fmt_p(regmatch_T *rmp, int midx, qffields_T *fields)
{
    char_u	*match_ptr;

    if (rmp->startp[midx] == NULL || rmp->endp[midx] == NULL)
	return QF_FAIL;
    fields->col = 0;
    for (match_ptr = rmp->startp[midx]; match_ptr != rmp->endp[midx];
								++match_ptr)
    {
	++fields->col;
	if (*match_ptr == TAB)
	{
	    fields->col += 7;
	    fields->col -= fields->col % 8;
	}
    }
    ++fields->col;
    fields->use_viscol = TRUE;
    return QF_OK;
}