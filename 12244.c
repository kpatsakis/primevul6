hgr_get_ll(int *new_ll)
{
    win_T	*wp;
    qf_info_T	*qi;

    // If the current window is a help window, then use it
    if (bt_help(curwin->w_buffer))
	wp = curwin;
    else
	// Find an existing help window
	wp = qf_find_help_win();

    if (wp == NULL)	    // Help window not found
	qi = NULL;
    else
	qi = wp->w_llist;

    if (qi == NULL)
    {
	// Allocate a new location list for help text matches
	if ((qi = qf_alloc_stack(QFLT_LOCATION)) == NULL)
	    return NULL;
	*new_ll = TRUE;
    }

    return qi;
}