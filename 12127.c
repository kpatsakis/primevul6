qf_find_win_with_normal_buf(void)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	if (bt_normal(wp->w_buffer))
	    return wp;

    return NULL;
}