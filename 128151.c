verbose_enter_scroll(void)
{
    if (*p_vfile != NUL)
	++msg_silent;
    else
	// always scroll up, don't overwrite
	msg_scroll = TRUE;
}