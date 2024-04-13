verbose_leave(void)
{
    if (*p_vfile != NUL)
	if (--msg_silent < 0)
	    msg_silent = 0;
}