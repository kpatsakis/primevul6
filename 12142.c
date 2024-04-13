qf_goto_win_with_ll_file(win_T *use_win, int qf_fnum, qf_info_T *ll_ref)
{
    win_T	*win = use_win;

    if (win == NULL)
    {
	// Find the window showing the selected file in the current tab page.
	FOR_ALL_WINDOWS(win)
	    if (win->w_buffer->b_fnum == qf_fnum)
		break;
	if (win == NULL)
	{
	    // Find a previous usable window
	    win = curwin;
	    do
	    {
		if (bt_normal(win->w_buffer))
		    break;
		if (win->w_prev == NULL)
		    win = lastwin;	// wrap around the top
		else
		    win = win->w_prev; // go to previous window
	    } while (win != curwin);
	}
    }
    win_goto(win);

    // If the location list for the window is not set, then set it
    // to the location list from the location window
    if (win->w_llist == NULL && ll_ref != NULL)
	win_set_loclist(win, ll_ref);
}