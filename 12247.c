set_ref_in_quickfix(int copyID)
{
    int		abort = FALSE;
    tabpage_T	*tp;
    win_T	*win;

    abort = mark_quickfix_ctx(&ql_info, copyID);
    if (abort)
	return abort;

    abort = set_ref_in_callback(&qftf_cb, copyID);
    if (abort)
	return abort;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	if (win->w_llist != NULL)
	{
	    abort = mark_quickfix_ctx(win->w_llist, copyID);
	    if (abort)
		return abort;
	}
	if (IS_LL_WINDOW(win) && (win->w_llist_ref->qf_refcount == 1))
	{
	    // In a location list window and none of the other windows is
	    // referring to this location list. Mark the location list
	    // context as still in use.
	    abort = mark_quickfix_ctx(win->w_llist_ref, copyID);
	    if (abort)
		return abort;
	}
    }

    return abort;
}