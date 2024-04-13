copy_loclist_stack(win_T *from, win_T *to)
{
    qf_info_T	*qi;
    int		idx;

    // When copying from a location list window, copy the referenced
    // location list. For other windows, copy the location list for
    // that window.
    if (IS_LL_WINDOW(from))
	qi = from->w_llist_ref;
    else
	qi = from->w_llist;

    if (qi == NULL)		    // no location list to copy
	return;

    // allocate a new location list
    if ((to->w_llist = qf_alloc_stack(QFLT_LOCATION)) == NULL)
	return;

    to->w_llist->qf_listcount = qi->qf_listcount;

    // Copy the location lists one at a time
    for (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	to->w_llist->qf_curlist = idx;

	if (copy_loclist(qf_get_list(qi, idx),
			qf_get_list(to->w_llist, idx)) == FAIL)
	{
	    qf_free_all(to);
	    return;
	}
    }

    to->w_llist->qf_curlist = qi->qf_curlist;	// current list
}