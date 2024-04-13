qf_list_entry(qfline_T *qfp, int qf_idx, int cursel)
{
    char_u	*fname;
    buf_T	*buf;
    int		filter_entry;

    fname = NULL;
    if (qfp->qf_module != NULL && *qfp->qf_module != NUL)
	vim_snprintf((char *)IObuff, IOSIZE, "%2d %s", qf_idx,
						(char *)qfp->qf_module);
    else {
	if (qfp->qf_fnum != 0
		&& (buf = buflist_findnr(qfp->qf_fnum)) != NULL)
	{
	    fname = buf->b_fname;
	    if (qfp->qf_type == 1)	// :helpgrep
		fname = gettail(fname);
	}
	if (fname == NULL)
	    sprintf((char *)IObuff, "%2d", qf_idx);
	else
	    vim_snprintf((char *)IObuff, IOSIZE, "%2d %s",
		    qf_idx, (char *)fname);
    }

    // Support for filtering entries using :filter /pat/ clist
    // Match against the module name, file name, search pattern and
    // text of the entry.
    filter_entry = TRUE;
    if (qfp->qf_module != NULL && *qfp->qf_module != NUL)
	filter_entry &= message_filtered(qfp->qf_module);
    if (filter_entry && fname != NULL)
	filter_entry &= message_filtered(fname);
    if (filter_entry && qfp->qf_pattern != NULL)
	filter_entry &= message_filtered(qfp->qf_pattern);
    if (filter_entry)
	filter_entry &= message_filtered(qfp->qf_text);
    if (filter_entry)
	return;

    msg_putchar('\n');
    msg_outtrans_attr(IObuff, cursel ? HL_ATTR(HLF_QFL) : qfFileAttr);

    if (qfp->qf_lnum != 0)
	msg_puts_attr(":", qfSepAttr);
    if (qfp->qf_lnum == 0)
	IObuff[0] = NUL;
    else
	qf_range_text(qfp, IObuff, IOSIZE);
    sprintf((char *)IObuff + STRLEN(IObuff), "%s",
	    (char *)qf_types(qfp->qf_type, qfp->qf_nr));
    msg_puts_attr((char *)IObuff, qfLineAttr);
    msg_puts_attr(":", qfSepAttr);
    if (qfp->qf_pattern != NULL)
    {
	qf_fmt_text(qfp->qf_pattern, IObuff, IOSIZE);
	msg_puts((char *)IObuff);
	msg_puts_attr(":", qfSepAttr);
    }
    msg_puts(" ");

    {
	char_u *tbuf = IObuff;
	size_t	tbuflen = IOSIZE;
	size_t	len = STRLEN(qfp->qf_text) + 3;

	if (len > IOSIZE)
	{
	    tbuf = alloc(len);
	    if (tbuf != NULL)
		tbuflen = len;
	    else
		tbuf = IObuff;
	}

	// Remove newlines and leading whitespace from the text.  For an
	// unrecognized line keep the indent, the compiler may mark a word
	// with ^^^^.
	qf_fmt_text((fname != NULL || qfp->qf_lnum != 0)
				    ? skipwhite(qfp->qf_text) : qfp->qf_text,
				    tbuf, (int)tbuflen);
	msg_prt_line(tbuf, FALSE);

	if (tbuf != IObuff)
	    vim_free(tbuf);
    }
    out_flush();		// show one line at a time
}