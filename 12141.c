qf_parse_match(
	char_u		*linebuf,
	int		linelen,
	efm_T		*fmt_ptr,
	regmatch_T	*regmatch,
	qffields_T	*fields,
	int		qf_multiline,
	int		qf_multiscan,
	char_u		**tail)
{
    int		idx = fmt_ptr->prefix;
    int		i;
    int		midx;
    int		status;

    if ((idx == 'C' || idx == 'Z') && !qf_multiline)
	return QF_FAIL;
    if (vim_strchr((char_u *)"EWIN", idx) != NULL)
	fields->type = idx;
    else
	fields->type = 0;

    // Extract error message data from matched line.
    // We check for an actual submatch, because "\[" and "\]" in
    // the 'errorformat' may cause the wrong submatch to be used.
    for (i = 0; i < FMT_PATTERNS; i++)
    {
	status = QF_OK;
	midx = (int)fmt_ptr->addr[i];
	if (i == 0 && midx > 0)				// %f
	    status = qf_parse_fmt_f(regmatch, midx, fields, idx);
	else if (i == FMT_PATTERN_M)
	{
	    if (fmt_ptr->flags == '+' && !qf_multiscan)	// %+
		status = copy_nonerror_line(linebuf, linelen, fields);
	    else if (midx > 0)				// %m
		status = qf_parse_fmt_m(regmatch, midx, fields);
	}
	else if (i == FMT_PATTERN_R && midx > 0)	// %r
	    status = qf_parse_fmt_r(regmatch, midx, tail);
	else if (midx > 0)				// others
	    status = (qf_parse_fmt[i])(regmatch, midx, fields);

	if (status != QF_OK)
	    return status;
    }

    return QF_OK;
}