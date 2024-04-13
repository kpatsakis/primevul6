qf_parse_multiline_pfx(
	int idx,
	qf_list_T *qfl,
	qffields_T *fields)
{
    char_u		*ptr;
    int			len;

    if (!qfl->qf_multiignore)
    {
	qfline_T *qfprev = qfl->qf_last;

	if (qfprev == NULL)
	    return QF_FAIL;
	if (*fields->errmsg && !qfl->qf_multiignore)
	{
	    len = (int)STRLEN(qfprev->qf_text);
	    ptr = alloc_id(len + STRLEN(fields->errmsg) + 2,
						aid_qf_multiline_pfx);
	    if (ptr == NULL)
		return QF_FAIL;
	    STRCPY(ptr, qfprev->qf_text);
	    vim_free(qfprev->qf_text);
	    qfprev->qf_text = ptr;
	    *(ptr += len) = '\n';
	    STRCPY(++ptr, fields->errmsg);
	}
	if (qfprev->qf_nr == -1)
	    qfprev->qf_nr = fields->enr;
	if (vim_isprintc(fields->type) && !qfprev->qf_type)
	    // only printable chars allowed
	    qfprev->qf_type = fields->type;

	if (!qfprev->qf_lnum)
	    qfprev->qf_lnum = fields->lnum;
	if (!qfprev->qf_end_lnum)
	    qfprev->qf_end_lnum = fields->end_lnum;
	if (!qfprev->qf_col)
	{
	    qfprev->qf_col = fields->col;
	    qfprev->qf_viscol = fields->use_viscol;
	}
	if (!qfprev->qf_end_col)
	    qfprev->qf_end_col = fields->end_col;
	if (!qfprev->qf_fnum)
	    qfprev->qf_fnum = qf_get_fnum(qfl,
		    qfl->qf_directory,
		    *fields->namebuf || qfl->qf_directory != NULL
		    ? fields->namebuf
		    : qfl->qf_currfile != NULL && fields->valid
		    ? qfl->qf_currfile : 0);
    }
    if (idx == 'Z')
	qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    line_breakcheck();

    return QF_IGNORE_LINE;
}