screen_puts_mbyte(char_u *s, int l, int attr)
{
    int		cw;

    msg_didout = TRUE;		// remember that line is not empty
    cw = (*mb_ptr2cells)(s);
    if (cw > 1 && (
#ifdef FEAT_RIGHTLEFT
		cmdmsg_rl ? msg_col <= 1 :
#endif
		msg_col == Columns - 1))
    {
	// Doesn't fit, print a highlighted '>' to fill it up.
	msg_screen_putchar('>', HL_ATTR(HLF_AT));
	return s;
    }

    screen_puts_len(s, l, msg_row, msg_col, attr);
#ifdef FEAT_RIGHTLEFT
    if (cmdmsg_rl)
    {
	msg_col -= cw;
	if (msg_col == 0)
	{
	    msg_col = Columns;
	    ++msg_row;
	}
    }
    else
#endif
    {
	msg_col += cw;
	if (msg_col >= Columns)
	{
	    msg_col = 0;
	    ++msg_row;
	}
    }
    return s + l;
}