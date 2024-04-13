qf_buf_add_line(
	buf_T		*buf,		// quickfix window buffer
	linenr_T	lnum,
	qfline_T	*qfp,
	char_u		*dirname,
	int		first_bufline,
	char_u		*qftf_str)
{
    int		len;
    buf_T	*errbuf;

    // If the 'quickfixtextfunc' function returned a non-empty custom string
    // for this entry, then use it.
    if (qftf_str != NULL && *qftf_str != NUL)
	vim_strncpy(IObuff, qftf_str, IOSIZE - 1);
    else
    {
	if (qfp->qf_module != NULL)
	{
	    vim_strncpy(IObuff, qfp->qf_module, IOSIZE - 1);
	    len = (int)STRLEN(IObuff);
	}
	else if (qfp->qf_fnum != 0
		&& (errbuf = buflist_findnr(qfp->qf_fnum)) != NULL
		&& errbuf->b_fname != NULL)
	{
	    if (qfp->qf_type == 1)	// :helpgrep
		vim_strncpy(IObuff, gettail(errbuf->b_fname), IOSIZE - 1);
	    else
	    {
		// Shorten the file name if not done already.
		// For optimization, do this only for the first entry in a
		// buffer.
		if (first_bufline && (errbuf->b_sfname == NULL
				|| mch_isFullName(errbuf->b_sfname)))
		{
		    if (*dirname == NUL)
			mch_dirname(dirname, MAXPATHL);
		    shorten_buf_fname(errbuf, dirname, FALSE);
		}
		vim_strncpy(IObuff, errbuf->b_fname, IOSIZE - 1);
	    }
	    len = (int)STRLEN(IObuff);
	}
	else
	    len = 0;

	if (len < IOSIZE - 1)
	    IObuff[len++] = '|';

	if (qfp->qf_lnum > 0)
	{
	    qf_range_text(qfp, IObuff + len, IOSIZE - len);
	    len += (int)STRLEN(IObuff + len);

	    vim_snprintf((char *)IObuff + len, IOSIZE - len, "%s",
		    (char *)qf_types(qfp->qf_type, qfp->qf_nr));
	    len += (int)STRLEN(IObuff + len);
	}
	else if (qfp->qf_pattern != NULL)
	{
	    qf_fmt_text(qfp->qf_pattern, IObuff + len, IOSIZE - len);
	    len += (int)STRLEN(IObuff + len);
	}
	if (len < IOSIZE - 2)
	{
	    IObuff[len++] = '|';
	    IObuff[len++] = ' ';
	}

	// Remove newlines and leading whitespace from the text.
	// For an unrecognized line keep the indent, the compiler may
	// mark a word with ^^^^.
	qf_fmt_text(len > 3 ? skipwhite(qfp->qf_text) : qfp->qf_text,
		IObuff + len, IOSIZE - len);
    }

    if (ml_append_buf(buf, lnum, IObuff,
		(colnr_T)STRLEN(IObuff) + 1, FALSE) == FAIL)
	return FAIL;

    return OK;
}