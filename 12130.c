qf_find_win_with_loclist(qf_info_T *ll)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	if (wp->w_llist == ll && !bt_quickfix(wp->w_buffer))
	    return wp;

    return NULL;
}