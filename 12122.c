qf_find_help_win(void)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	if (bt_help(wp->w_buffer))
	    return wp;

    return NULL;
}