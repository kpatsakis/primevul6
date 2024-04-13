vgr_qflist_valid(
	win_T	    *wp,
	qf_info_T   *qi,
	int_u	    qfid,
	char_u	    *title)
{
    // Verify that the quickfix/location list was not freed by an autocmd
    if (!qflist_valid(wp, qfid))
    {
	if (wp != NULL)
	{
	    // An autocmd has freed the location list.
	    emsg(_(e_current_location_list_was_changed));
	    return FALSE;
	}
	else
	{
	    // Quickfix list is not found, create a new one.
	    qf_new_list(qi, title);
	    return TRUE;
	}
    }

    if (qf_restore_list(qi, qfid) == FAIL)
	return FALSE;

    return TRUE;
}