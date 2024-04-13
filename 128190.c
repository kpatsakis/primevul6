redir_write(char_u *str, int maxlen)
{
    char_u	*s = str;
    static int	cur_col = 0;

    // Don't do anything for displaying prompts and the like.
    if (redir_off)
	return;

    // If 'verbosefile' is set prepare for writing in that file.
    if (*p_vfile != NUL && verbose_fd == NULL)
	verbose_open();

    if (redirecting())
    {
	// If the string doesn't start with CR or NL, go to msg_col
	if (*s != '\n' && *s != '\r')
	{
	    while (cur_col < msg_col)
	    {
#ifdef FEAT_EVAL
		if (redir_execute)
		    execute_redir_str((char_u *)" ", -1);
		else if (redir_reg)
		    write_reg_contents(redir_reg, (char_u *)" ", -1, TRUE);
		else if (redir_vname)
		    var_redir_str((char_u *)" ", -1);
		else
#endif
		    if (redir_fd != NULL)
		    fputs(" ", redir_fd);
		if (verbose_fd != NULL)
		    fputs(" ", verbose_fd);
		++cur_col;
	    }
	}

#ifdef FEAT_EVAL
	if (redir_execute)
	    execute_redir_str(s, maxlen);
	else if (redir_reg)
	    write_reg_contents(redir_reg, s, maxlen, TRUE);
	else if (redir_vname)
	    var_redir_str(s, maxlen);
#endif

	// Write and adjust the current column.
	while (*s != NUL && (maxlen < 0 || (int)(s - str) < maxlen))
	{
#ifdef FEAT_EVAL
	    if (!redir_reg && !redir_vname && !redir_execute)
#endif
		if (redir_fd != NULL)
		    putc(*s, redir_fd);
	    if (verbose_fd != NULL)
		putc(*s, verbose_fd);
	    if (*s == '\r' || *s == '\n')
		cur_col = 0;
	    else if (*s == '\t')
		cur_col += (8 - cur_col % 8);
	    else
		++cur_col;
	    ++s;
	}

	if (msg_silent != 0)	// should update msg_col
	    msg_col = cur_col;
    }
}