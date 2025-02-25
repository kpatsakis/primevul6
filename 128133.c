msg_clr_eos_force(void)
{
    if (msg_use_printf())
    {
	if (full_screen)	// only when termcap codes are valid
	{
	    if (*T_CD)
		out_str(T_CD);	// clear to end of display
	    else if (*T_CE)
		out_str(T_CE);	// clear to end of line
	}
    }
    else
    {
#ifdef FEAT_RIGHTLEFT
	if (cmdmsg_rl)
	{
	    screen_fill(msg_row, msg_row + 1, 0, msg_col + 1, ' ', ' ', 0);
	    screen_fill(msg_row + 1, (int)Rows, 0, (int)Columns, ' ', ' ', 0);
	}
	else
#endif
	{
	    screen_fill(msg_row, msg_row + 1, msg_col, (int)Columns,
								 ' ', ' ', 0);
	    screen_fill(msg_row + 1, (int)Rows, 0, (int)Columns, ' ', ' ', 0);
	}
    }
}