jump_to_help_window(qf_info_T *qi, int newwin, int *opened_window)
{
    win_T	*wp;
    int		flags;

    if (cmdmod.cmod_tab != 0 || newwin)
	wp = NULL;
    else
	wp = qf_find_help_win();
    if (wp != NULL && wp->w_buffer->b_nwindows > 0)
	win_enter(wp, TRUE);
    else
    {
	// Split off help window; put it at far top if no position
	// specified, the current window is vertically split and narrow.
	flags = WSP_HELP;
	if (cmdmod.cmod_split == 0 && curwin->w_width != Columns
		&& curwin->w_width < 80)
	    flags |= WSP_TOP;
	// If the user asks to open a new window, then copy the location list.
	// Otherwise, don't copy the location list.
	if (IS_LL_STACK(qi) && !newwin)
	    flags |= WSP_NEWLOC;

	if (win_split(0, flags) == FAIL)
	    return FAIL;

	*opened_window = TRUE;

	if (curwin->w_height < p_hh)
	    win_setheight((int)p_hh);

	// When using location list, the new window should use the supplied
	// location list. If the user asks to open a new window, then the new
	// window will get a copy of the location list.
	if (IS_LL_STACK(qi) && !newwin)
	    win_set_loclist(curwin, qi);
    }

    if (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

    return OK;
}