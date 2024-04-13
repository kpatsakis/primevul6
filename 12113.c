qf_set_properties(qf_info_T *qi, dict_T *what, int action, char_u *title)
{
    dictitem_T	*di;
    int		retval = FAIL;
    int		qf_idx;
    int		newlist = FALSE;
    qf_list_T	*qfl;

    if (action == ' ' || qf_stack_empty(qi))
	newlist = TRUE;

    qf_idx = qf_setprop_get_qfidx(qi, what, action, &newlist);
    if (qf_idx == INVALID_QFIDX)	// List not found
	return FAIL;

    if (newlist)
    {
	qi->qf_curlist = qf_idx;
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
    }

    qfl = qf_get_list(qi, qf_idx);
    if ((di = dict_find(what, (char_u *)"title", -1)) != NULL)
	retval = qf_setprop_title(qi, qf_idx, what, di);
    if ((di = dict_find(what, (char_u *)"items", -1)) != NULL)
	retval = qf_setprop_items(qi, qf_idx, di, action);
    if ((di = dict_find(what, (char_u *)"lines", -1)) != NULL)
	retval = qf_setprop_items_from_lines(qi, qf_idx, what, di, action);
    if ((di = dict_find(what, (char_u *)"context", -1)) != NULL)
	retval = qf_setprop_context(qfl, di);
    if ((di = dict_find(what, (char_u *)"idx", -1)) != NULL)
	retval = qf_setprop_curidx(qi, qfl, di);
    if ((di = dict_find(what, (char_u *)"quickfixtextfunc", -1)) != NULL)
	retval = qf_setprop_qftf(qi, qfl, di);

    if (newlist || retval == OK)
	qf_list_changed(qfl);
    if (newlist)
	qf_update_buffer(qi, NULL);

    return retval;
}