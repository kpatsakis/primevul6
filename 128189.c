msg_puts_attr_len(char *str, int maxlen, int attr)
{
    /*
     * If redirection is on, also write to the redirection file.
     */
    redir_write((char_u *)str, maxlen);

    /*
     * Don't print anything when using ":silent cmd".
     */
    if (msg_silent != 0)
	return;

    // if MSG_HIST flag set, add message to history
    if ((attr & MSG_HIST) && maxlen < 0)
    {
	add_msg_hist((char_u *)str, -1, attr);
	attr &= ~MSG_HIST;
    }

    // When writing something to the screen after it has scrolled, requires a
    // wait-return prompt later.  Needed when scrolling, resetting
    // need_wait_return after some prompt, and then outputting something
    // without scrolling
    // Not needed when only using CR to move the cursor.
    if (msg_scrolled != 0 && !msg_scrolled_ign && STRCMP(str, "\r") != 0)
	need_wait_return = TRUE;
    msg_didany = TRUE;		// remember that something was outputted

    /*
     * If there is no valid screen, use fprintf so we can see error messages.
     * If termcap is not active, we may be writing in an alternate console
     * window, cursor positioning may not work correctly (window size may be
     * different, e.g. for Win32 console) or we just don't know where the
     * cursor is.
     */
    if (msg_use_printf())
	msg_puts_printf((char_u *)str, maxlen);
    else
	msg_puts_display((char_u *)str, maxlen, attr, FALSE);

    need_fileinfo = FALSE;
}