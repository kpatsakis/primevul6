hgr_search_file(
	qf_list_T *qfl,
	char_u *fname,
	vimconv_T *p_vc,
	regmatch_T *p_regmatch)
{
    FILE	*fd;
    long	lnum;

    fd = mch_fopen((char *)fname, "r");
    if (fd == NULL)
	return;

    lnum = 1;
    while (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
    {
	char_u    *line = IObuff;

	// Convert a line if 'encoding' is not utf-8 and
	// the line contains a non-ASCII character.
	if (p_vc->vc_type != CONV_NONE && has_non_ascii(IObuff))
	{
	    line = string_convert(p_vc, IObuff, NULL);
	    if (line == NULL)
		line = IObuff;
	}

	if (vim_regexec(p_regmatch, line, (colnr_T)0))
	{
	    int	l = (int)STRLEN(line);

	    // remove trailing CR, LF, spaces, etc.
	    while (l > 0 && line[l - 1] <= ' ')
		line[--l] = NUL;

	    if (qf_add_entry(qfl,
			NULL,	// dir
			fname,
			NULL,
			0,
			line,
			lnum,
			0,
			(int)(p_regmatch->startp[0] - line)
			+ 1,	// col
			(int)(p_regmatch->endp[0] - line)
			+ 1,	// end_col
			FALSE,	// vis_col
			NULL,	// search pattern
			0,	// nr
			1,	// type
			TRUE	// valid
			) == QF_FAIL)
	    {
		got_int = TRUE;
		if (line != IObuff)
		    vim_free(line);
		break;
	    }
	}
	if (line != IObuff)
	    vim_free(line);
	++lnum;
	line_breakcheck();
    }
    fclose(fd);
}