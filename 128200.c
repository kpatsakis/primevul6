verbose_leave_scroll(void)
{
    if (*p_vfile != NUL)
    {
	if (--msg_silent < 0)
	    msg_silent = 0;
    }
    else
	cmdline_row = msg_row;
}