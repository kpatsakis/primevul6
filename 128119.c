repeat_message(void)
{
    if (State == MODE_ASKMORE)
    {
	msg_moremsg(TRUE);	// display --more-- message again
	msg_row = Rows - 1;
    }
#ifdef FEAT_CON_DIALOG
    else if (State == MODE_CONFIRM)
    {
	display_confirm_msg();	// display ":confirm" message again
	msg_row = Rows - 1;
    }
#endif
    else if (State == MODE_EXTERNCMD)
    {
	windgoto(msg_row, msg_col); // put cursor back
    }
    else if (State == MODE_HITRETURN || State == MODE_SETWSIZE)
    {
	if (msg_row == Rows - 1)
	{
	    // Avoid drawing the "hit-enter" prompt below the previous one,
	    // overwrite it.  Esp. useful when regaining focus and a
	    // FocusGained autocmd exists but didn't draw anything.
	    msg_didout = FALSE;
	    msg_col = 0;
	    msg_clr_eos();
	}
	hit_return_msg();
	msg_row = Rows - 1;
    }
}