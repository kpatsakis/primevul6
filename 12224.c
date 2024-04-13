qf_jump_to_usable_window(int qf_fnum, int newwin, int *opened_window)
{
    win_T	*usable_wp = NULL;
    int		usable_win = FALSE;
    qf_info_T	*ll_ref = NULL;

    // If opening a new window, then don't use the location list referred by
    // the current window.  Otherwise two windows will refer to the same
    // location list.
    if (!newwin)
	ll_ref = curwin->w_llist_ref;

    if (ll_ref != NULL)
    {
	// Find a non-quickfix window with this location list
	usable_wp = qf_find_win_with_loclist(ll_ref);
	if (usable_wp != NULL)
	    usable_win = TRUE;
    }

    if (!usable_win)
    {
	// Locate a window showing a normal buffer
	win_T	*win = qf_find_win_with_normal_buf();
	if (win != NULL)
	    usable_win = TRUE;
    }

    // If no usable window is found and 'switchbuf' contains "usetab"
    // then search in other tabs.
    if (!usable_win && (swb_flags & SWB_USETAB))
	usable_win = qf_goto_tabwin_with_file(qf_fnum);

    // If there is only one window and it is the quickfix window, create a
    // new one above the quickfix window.
    if ((ONE_WINDOW && bt_quickfix(curbuf)) || !usable_win || newwin)
    {
	if (qf_open_new_file_win(ll_ref) != OK)
	    return FAIL;
	*opened_window = TRUE;	// close it when fail
    }
    else
    {
	if (curwin->w_llist_ref != NULL)	// In a location window
	    qf_goto_win_with_ll_file(usable_wp, qf_fnum, ll_ref);
	else					// In a quickfix window
	    qf_goto_win_with_qfl_file(qf_fnum);
    }

    return OK;
}