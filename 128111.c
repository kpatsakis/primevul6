msg_attr_keep(
    char	*s,
    int		attr,
    int		keep)	    // TRUE: set keep_msg if it doesn't scroll
{
    static int	entered = 0;
    int		retval;
    char_u	*buf = NULL;

    // Skip messages not matching ":filter pattern".
    // Don't filter when there is an error.
    if (!emsg_on_display && message_filtered((char_u *)s))
	return TRUE;

#ifdef FEAT_EVAL
    if (attr == 0)
	set_vim_var_string(VV_STATUSMSG, (char_u *)s, -1);
#endif

    /*
     * It is possible that displaying a messages causes a problem (e.g.,
     * when redrawing the window), which causes another message, etc..	To
     * break this loop, limit the recursiveness to 3 levels.
     */
    if (entered >= 3)
	return TRUE;
    ++entered;

    // Add message to history (unless it's a repeated kept message or a
    // truncated message)
    if ((char_u *)s != keep_msg
	    || (*s != '<'
		&& last_msg_hist != NULL
		&& last_msg_hist->msg != NULL
		&& STRCMP(s, last_msg_hist->msg)))
	add_msg_hist((char_u *)s, -1, attr);

#ifdef FEAT_JOB_CHANNEL
    if (emsg_to_channel_log)
	// Write message in the channel log.
	ch_log(NULL, "ERROR: %s", s);
#endif

    // Truncate the message if needed.
    msg_start();
    buf = msg_strtrunc((char_u *)s, FALSE);
    if (buf != NULL)
	s = (char *)buf;

    msg_outtrans_attr((char_u *)s, attr);
    msg_clr_eos();
    retval = msg_end();

    if (keep && retval && vim_strsize((char_u *)s)
			    < (int)(Rows - cmdline_row - 1) * Columns + sc_col)
	set_keep_msg((char_u *)s, 0);

    need_fileinfo = FALSE;

    vim_free(buf);
    --entered;
    return retval;
}