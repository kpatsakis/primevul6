normal_cmd_wait_for_msg(void)
{
    int	save_State = State;

    // Draw the cursor with the right shape here
    if (restart_edit != 0)
	State = MODE_INSERT;

    // If need to redraw, and there is a "keep_msg", redraw before the
    // delay
    if (must_redraw && keep_msg != NULL && !emsg_on_display)
    {
	char_u	*kmsg;

	kmsg = keep_msg;
	keep_msg = NULL;
	// Showmode() will clear keep_msg, but we want to use it anyway.
	// First update w_topline.
	setcursor();
	update_screen(0);
	// now reset it, otherwise it's put in the history again
	keep_msg = kmsg;

	kmsg = vim_strsave(keep_msg);
	if (kmsg != NULL)
	{
	    msg_attr((char *)kmsg, keep_msg_attr);
	    vim_free(kmsg);
	}
    }
    setcursor();
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif
    cursor_on();
    out_flush();
    if (msg_scroll || emsg_on_display)
	ui_delay(1003L, TRUE);	// wait at least one second
    ui_delay(3003L, FALSE);		// wait up to three seconds
    State = save_State;

    msg_scroll = FALSE;
    emsg_on_display = FALSE;
}