ll_get_or_alloc_list(win_T *wp)
{
    if (IS_LL_WINDOW(wp))
	// For a location list window, use the referenced location list
	return wp->w_llist_ref;

    // For a non-location list window, w_llist_ref should not point to a
    // location list.
    ll_free_all(&wp->w_llist_ref);

    if (wp->w_llist == NULL)
	wp->w_llist = qf_alloc_stack(QFLT_LOCATION);	// new location list
    return wp->w_llist;
}