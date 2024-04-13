emsg_core(char_u *s)
{
    int		attr;
    char_u	*p;
    int		r;
#ifdef FEAT_EVAL
    int		ignore = FALSE;
    int		severe;
#endif

#ifdef FEAT_EVAL
    // When testing some errors are turned into a normal message.
    if (ignore_error(s))
	// don't call msg() if it results in a dialog
	return msg_use_printf() ? FALSE : msg((char *)s);
#endif

    ++called_emsg;

#ifdef FEAT_EVAL
    // If "emsg_severe" is TRUE: When an error exception is to be thrown,
    // prefer this message over previous messages for the same command.
    severe = emsg_severe;
    emsg_severe = FALSE;
#endif

    if (!emsg_off || vim_strchr(p_debug, 't') != NULL)
    {
#ifdef FEAT_EVAL
	/*
	 * Cause a throw of an error exception if appropriate.  Don't display
	 * the error message in this case.  (If no matching catch clause will
	 * be found, the message will be displayed later on.)  "ignore" is set
	 * when the message should be ignored completely (used for the
	 * interrupt message).
	 */
	if (cause_errthrow(s, severe, &ignore) == TRUE)
	{
	    if (!ignore)
		++did_emsg;
	    return TRUE;
	}

	if (in_assert_fails && emsg_assert_fails_msg == NULL)
	{
	    emsg_assert_fails_msg = vim_strsave(s);
	    emsg_assert_fails_lnum = SOURCING_LNUM;
	    vim_free(emsg_assert_fails_context);
	    emsg_assert_fails_context = vim_strsave(
			 SOURCING_NAME == NULL ? (char_u *)"" : SOURCING_NAME);
	}

	// set "v:errmsg", also when using ":silent! cmd"
	set_vim_var_string(VV_ERRMSG, s, -1);
#endif

	/*
	 * When using ":silent! cmd" ignore error messages.
	 * But do write it to the redirection file.
	 */
	if (emsg_silent != 0)
	{
#ifdef FEAT_EVAL
	    ++did_emsg_silent;
#endif
	    if (emsg_noredir == 0)
	    {
		msg_start();
		p = get_emsg_source();
		if (p != NULL)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		p = get_emsg_lnum();
		if (p != NULL)
		{
		    STRCAT(p, "\n");
		    redir_write(p, -1);
		    vim_free(p);
		}
		redir_write(s, -1);
	    }
#ifdef FEAT_EVAL
	    // Only increment did_emsg_def when :silent! wasn't used inside the
	    // :def function.
	    if (emsg_silent == emsg_silent_def)
		++did_emsg_def;
#endif
#ifdef FEAT_JOB_CHANNEL
	    ch_log(NULL, "ERROR silent: %s", (char *)s);
#endif
	    return TRUE;
	}

	ex_exitval = 1;

	// Reset msg_silent, an error causes messages to be switched back on.
	msg_silent = 0;
	cmd_silent = FALSE;

	if (global_busy)		// break :global command
	    ++global_busy;

	if (p_eb)
	    beep_flush();		// also includes flush_buffers()
	else
	    flush_buffers(FLUSH_MINIMAL);  // flush internal buffers
	++did_emsg;			   // flag for DoOneCmd()
#ifdef FEAT_EVAL
	++uncaught_emsg;
#endif
    }

    emsg_on_display = TRUE;	// remember there is an error message
    ++msg_scroll;		// don't overwrite a previous message
    attr = HL_ATTR(HLF_E);	// set highlight mode for error messages
    if (msg_scrolled != 0)
	need_wait_return = TRUE;    // needed in case emsg() is called after
				    // wait_return has reset need_wait_return
				    // and a redraw is expected because
				    // msg_scrolled is non-zero

#ifdef FEAT_JOB_CHANNEL
    emsg_to_channel_log = TRUE;
#endif
    /*
     * Display name and line number for the source of the error.
     */
    msg_source(attr);

    /*
     * Display the error message itself.
     */
    msg_nowait = FALSE;			// wait for this msg
    r = msg_attr((char *)s, attr);

#ifdef FEAT_JOB_CHANNEL
    emsg_to_channel_log = FALSE;
#endif
    return r;
}