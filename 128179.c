msg_screen_putchar(int c, int attr)
{
    msg_didout = TRUE;		// remember that line is not empty
    screen_putchar(c, msg_row, msg_col, attr);
#ifdef FEAT_RIGHTLEFT
    if (cmdmsg_rl)
    {
	if (--msg_col == 0)
	{
	    msg_col = Columns;
	    ++msg_row;
	}
    }
    else
#endif
    {
	if (++msg_col >= Columns)
	{
	    msg_col = 0;
	    ++msg_row;
	}
    }
}