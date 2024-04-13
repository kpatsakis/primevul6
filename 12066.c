qf_setprop_items_from_lines(
	qf_info_T	*qi,
	int		qf_idx,
	dict_T		*what,
	dictitem_T	*di,
	int		action)
{
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    int		retval = FAIL;

    // Use the user supplied errorformat settings (if present)
    if ((efm_di = dict_find(what, (char_u *)"efm", -1)) != NULL)
    {
	if (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == NULL)
	    return FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    // Only a List value is supported
    if (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == NULL)
	return FAIL;

    if (action == 'r')
	qf_free_items(&qi->qf_lists[qf_idx]);
    if (qf_init_ext(qi, qf_idx, NULL, NULL, &di->di_tv, errorformat,
		FALSE, (linenr_T)0, (linenr_T)0, NULL, NULL) >= 0)
	retval = OK;

    return retval;
}