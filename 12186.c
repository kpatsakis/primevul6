qf_setprop_get_qfidx(
	qf_info_T	*qi,
	dict_T		*what,
	int		action,
	int		*newlist)
{
    dictitem_T	*di;
    int		qf_idx = qi->qf_curlist;    // default is the current list

    if ((di = dict_find(what, (char_u *)"nr", -1)) != NULL)
    {
	// Use the specified quickfix/location list
	if (di->di_tv.v_type == VAR_NUMBER)
	{
	    // for zero use the current list
	    if (di->di_tv.vval.v_number != 0)
		qf_idx = di->di_tv.vval.v_number - 1;

	    if ((action == ' ' || action == 'a') && qf_idx == qi->qf_listcount)
	    {
		// When creating a new list, accept qf_idx pointing to the next
		// non-available list and add the new list at the end of the
		// stack.
		*newlist = TRUE;
		qf_idx = qf_stack_empty(qi) ? 0 : qi->qf_listcount - 1;
	    }
	    else if (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		return INVALID_QFIDX;
	    else if (action != ' ')
		*newlist = FALSE;	// use the specified list
	}
	else if (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != NULL
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	{
	    if (!qf_stack_empty(qi))
		qf_idx = qi->qf_listcount - 1;
	    else if (*newlist)
		qf_idx = 0;
	    else
		return INVALID_QFIDX;
	}
	else
	    return INVALID_QFIDX;
    }

    if (!*newlist && (di = dict_find(what, (char_u *)"id", -1)) != NULL)
    {
	// Use the quickfix/location list with the specified id
	if (di->di_tv.v_type != VAR_NUMBER)
	    return INVALID_QFIDX;

	return qf_id2nr(qi, di->di_tv.vval.v_number);
    }

    return qf_idx;
}