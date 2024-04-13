msg_puts_display(
    char_u	*str,
    int		maxlen,
    int		attr,
    int		recurse)
{
    char_u	*s = str;
    char_u	*t_s = str;	// string from "t_s" to "s" is still todo
    int		t_col = 0;	// screen cells todo, 0 when "t_s" not used
    int		l;
    int		cw;
    char_u	*sb_str = str;
    int		sb_col = msg_col;
    int		wrap;
    int		did_last_char;

    did_wait_return = FALSE;
    while ((maxlen < 0 || (int)(s - str) < maxlen) && *s != NUL)
    {
	/*
	 * We are at the end of the screen line when:
	 * - When outputting a newline.
	 * - When outputting a character in the last column.
	 */
	if (!recurse && msg_row >= Rows - 1 && (*s == '\n' || (
#ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl
		    ? (
			msg_col <= 1
		      || (*s == TAB && msg_col <= 7)
		      || (has_mbyte && (*mb_ptr2cells)(s) > 1 && msg_col <= 2))
		    :
#endif
		      ((*s != '\r' && msg_col + t_col >= Columns - 1)
		       || (*s == TAB && msg_col + t_col >= ((Columns - 1) & ~7))
		       || (has_mbyte && (*mb_ptr2cells)(s) > 1
					 && msg_col + t_col >= Columns - 2)))))
	{
	    /*
	     * The screen is scrolled up when at the last row (some terminals
	     * scroll automatically, some don't.  To avoid problems we scroll
	     * ourselves).
	     */
	    if (t_col > 0)
		// output postponed text
		t_puts(&t_col, t_s, s, attr);

	    // When no more prompt and no more room, truncate here
	    if (msg_no_more && lines_left == 0)
		break;

	    // Scroll the screen up one line.
	    msg_scroll_up();

	    msg_row = Rows - 2;
	    if (msg_col >= Columns)	// can happen after screen resize
		msg_col = Columns - 1;

	    // Display char in last column before showing more-prompt.
	    if (*s >= ' '
#ifdef FEAT_RIGHTLEFT
		    && !cmdmsg_rl
#endif
	       )
	    {
		if (has_mbyte)
		{
		    if (enc_utf8 && maxlen >= 0)
			// avoid including composing chars after the end
			l = utfc_ptr2len_len(s, (int)((str + maxlen) - s));
		    else
			l = (*mb_ptr2len)(s);
		    s = screen_puts_mbyte(s, l, attr);
		}
		else
		    msg_screen_putchar(*s++, attr);
		did_last_char = TRUE;
	    }
	    else
		did_last_char = FALSE;

	    if (p_more)
		// store text for scrolling back
		store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

	    inc_msg_scrolled();
	    need_wait_return = TRUE; // may need wait_return in main()
	    redraw_cmdline = TRUE;
	    if (cmdline_row > 0 && !exmode_active)
		--cmdline_row;

	    /*
	     * If screen is completely filled and 'more' is set then wait
	     * for a character.
	     */
	    if (lines_left > 0)
		--lines_left;
	    if (p_more && lines_left == 0 && State != MODE_HITRETURN
					    && !msg_no_more && !exmode_active)
	    {
#ifdef FEAT_CON_DIALOG
		if (do_more_prompt(NUL))
		    s = confirm_msg_tail;
#else
		(void)do_more_prompt(NUL);
#endif
		if (quit_more)
		    return;
	    }

	    // When we displayed a char in last column need to check if there
	    // is still more.
	    if (did_last_char)
		continue;
	}

	wrap = *s == '\n'
		    || msg_col + t_col >= Columns
		    || (has_mbyte && (*mb_ptr2cells)(s) > 1
					    && msg_col + t_col >= Columns - 1);
	if (t_col > 0 && (wrap || *s == '\r' || *s == '\b'
						 || *s == '\t' || *s == BELL))
	    // output any postponed text
	    t_puts(&t_col, t_s, s, attr);

	if (wrap && p_more && !recurse)
	    // store text for scrolling back
	    store_sb_text(&sb_str, s, attr, &sb_col, TRUE);

	if (*s == '\n')		    // go to next line
	{
	    msg_didout = FALSE;	    // remember that line is empty
#ifdef FEAT_RIGHTLEFT
	    if (cmdmsg_rl)
		msg_col = Columns - 1;
	    else
#endif
		msg_col = 0;
	    if (++msg_row >= Rows)  // safety check
		msg_row = Rows - 1;
	}
	else if (*s == '\r')	    // go to column 0
	{
	    msg_col = 0;
	}
	else if (*s == '\b')	    // go to previous char
	{
	    if (msg_col)
		--msg_col;
	}
	else if (*s == TAB)	    // translate Tab into spaces
	{
	    do
		msg_screen_putchar(' ', attr);
	    while (msg_col & 7);
	}
	else if (*s == BELL)		// beep (from ":sh")
	    vim_beep(BO_SH);
	else
	{
	    if (has_mbyte)
	    {
		cw = (*mb_ptr2cells)(s);
		if (enc_utf8 && maxlen >= 0)
		    // avoid including composing chars after the end
		    l = utfc_ptr2len_len(s, (int)((str + maxlen) - s));
		else
		    l = (*mb_ptr2len)(s);
	    }
	    else
	    {
		cw = 1;
		l = 1;
	    }

	    // When drawing from right to left or when a double-wide character
	    // doesn't fit, draw a single character here.  Otherwise collect
	    // characters and draw them all at once later.
	    if (
# ifdef FEAT_RIGHTLEFT
		    cmdmsg_rl ||
# endif
		    (cw > 1 && msg_col + t_col >= Columns - 1))
	    {
		if (l > 1)
		    s = screen_puts_mbyte(s, l, attr) - 1;
		else
		    msg_screen_putchar(*s, attr);
	    }
	    else
	    {
		// postpone this character until later
		if (t_col == 0)
		    t_s = s;
		t_col += cw;
		s += l - 1;
	    }
	}
	++s;
    }

    // output any postponed text
    if (t_col > 0)
	t_puts(&t_col, t_s, s, attr);
    if (p_more && !recurse)
	store_sb_text(&sb_str, s, attr, &sb_col, FALSE);

    msg_check();
}