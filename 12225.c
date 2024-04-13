qf_parse_get_fields(
	char_u		*linebuf,
	int		linelen,
	efm_T		*fmt_ptr,
	qffields_T	*fields,
	int		qf_multiline,
	int		qf_multiscan,
	char_u		**tail)
{
    regmatch_T	regmatch;
    int		status = QF_FAIL;
    int		r;

    if (qf_multiscan &&
		vim_strchr((char_u *)"OPQ", fmt_ptr->prefix) == NULL)
	return QF_FAIL;

    fields->namebuf[0] = NUL;
    fields->module[0] = NUL;
    fields->pattern[0] = NUL;
    if (!qf_multiscan)
	fields->errmsg[0] = NUL;
    fields->lnum = 0;
    fields->end_lnum = 0;
    fields->col = 0;
    fields->end_col = 0;
    fields->use_viscol = FALSE;
    fields->enr = -1;
    fields->type = 0;
    *tail = NULL;

    // Always ignore case when looking for a matching error.
    regmatch.rm_ic = TRUE;
    regmatch.regprog = fmt_ptr->prog;
    r = vim_regexec(&regmatch, linebuf, (colnr_T)0);
    fmt_ptr->prog = regmatch.regprog;
    if (r)
	status = qf_parse_match(linebuf, linelen, fmt_ptr, &regmatch,
		fields, qf_multiline, qf_multiscan, tail);

    return status;
}