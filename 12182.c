qf_getprop_qfidx(qf_info_T *qi, dict_T *what)
{
    int		qf_idx;
    dictitem_T	*di;

    qf_idx = qi->qf_curlist;	// default is the current list
    if ((di = dict_find(what, (char_u *)"nr", -1)) != NULL)
    {
	// Use the specified quickfix/location list
	if (di->di_tv.v_type == VAR_NUMBER)
	{
	    // for zero use the current list
	    if (di->di_tv.vval.v_number != 0)
	    {
		qf_idx = di->di_tv.vval.v_number - 1;
		if (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		    qf_idx = INVALID_QFIDX;
	    }
	}
	else if (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != NULL
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	    // Get the last quickfix list number
	    qf_idx = qi->qf_listcount - 1;
	else
	    qf_idx = INVALID_QFIDX;
    }

    if ((di = dict_find(what, (char_u *)"id", -1)) != NULL)
    {
	// Look for a list with the specified id
	if (di->di_tv.v_type == VAR_NUMBER)
	{
	    // For zero, use the current list or the list specified by 'nr'
	    if (di->di_tv.vval.v_number != 0)
		qf_idx = qf_id2nr(qi, di->di_tv.vval.v_number);
	}
	else
	    qf_idx = INVALID_QFIDX;
    }

    return qf_idx;
}