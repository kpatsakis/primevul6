qf_setprop_curidx(qf_info_T *qi, qf_list_T *qfl, dictitem_T *di)
{
    int		denote = FALSE;
    int		newidx;
    int		old_qfidx;
    qfline_T	*qf_ptr;

    // If the specified index is '$', then use the last entry
    if (di->di_tv.v_type == VAR_STRING
	    && di->di_tv.vval.v_string != NULL
	    && STRCMP(di->di_tv.vval.v_string, "$") == 0)
	newidx = qfl->qf_count;
    else
    {
	// Otherwise use the specified index
	newidx = tv_get_number_chk(&di->di_tv, &denote);
	if (denote)
	    return FAIL;
    }

    if (newidx < 1)		// sanity check
	return FAIL;
    if (newidx > qfl->qf_count)
	newidx = qfl->qf_count;

    old_qfidx = qfl->qf_index;
    qf_ptr = get_nth_entry(qfl, newidx, &newidx);
    if (qf_ptr == NULL)
	return FAIL;
    qfl->qf_ptr = qf_ptr;
    qfl->qf_index = newidx;

    // If the current list is modified and it is displayed in the quickfix
    // window, then Update it.
    if (qf_get_curlist(qi)->qf_id == qfl->qf_id)
	qf_win_pos_update(qi, old_qfidx);

    return OK;
}