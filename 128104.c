msg_advance(int col)
{
    if (msg_silent != 0)	// nothing to advance to
    {
	msg_col = col;		// for redirection, may fill it up later
	return;
    }
    if (col >= Columns)		// not enough room
	col = Columns - 1;
#ifdef FEAT_RIGHTLEFT
    if (cmdmsg_rl)
	while (msg_col > Columns - col)
	    msg_putchar(' ');
    else
#endif
	while (msg_col < col)
	    msg_putchar(' ');
}