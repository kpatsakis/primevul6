copy_loclist(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    // Some of the fields are populated by qf_add_entry()
    to_qfl->qfl_type = from_qfl->qfl_type;
    to_qfl->qf_nonevalid = from_qfl->qf_nonevalid;
    to_qfl->qf_count = 0;
    to_qfl->qf_index = 0;
    to_qfl->qf_start = NULL;
    to_qfl->qf_last = NULL;
    to_qfl->qf_ptr = NULL;
    if (from_qfl->qf_title != NULL)
	to_qfl->qf_title = vim_strsave(from_qfl->qf_title);
    else
	to_qfl->qf_title = NULL;
    if (from_qfl->qf_ctx != NULL)
    {
	to_qfl->qf_ctx = alloc_tv();
	if (to_qfl->qf_ctx != NULL)
	    copy_tv(from_qfl->qf_ctx, to_qfl->qf_ctx);
    }
    else
	to_qfl->qf_ctx = NULL;
    if (from_qfl->qf_qftf_cb.cb_name != NULL)
	copy_callback(&to_qfl->qf_qftf_cb, &from_qfl->qf_qftf_cb);
    else
	to_qfl->qf_qftf_cb.cb_name = NULL;

    if (from_qfl->qf_count)
	if (copy_loclist_entries(from_qfl, to_qfl) == FAIL)
	    return FAIL;

    to_qfl->qf_index = from_qfl->qf_index;	// current index in the list

    // Assign a new ID for the location list
    to_qfl->qf_id = ++last_qf_id;
    to_qfl->qf_changedtick = 0L;

    // When no valid entries are present in the list, qf_ptr points to
    // the first item in the list
    if (to_qfl->qf_nonevalid)
    {
	to_qfl->qf_ptr = to_qfl->qf_start;
	to_qfl->qf_index = 1;
    }

    return OK;
}