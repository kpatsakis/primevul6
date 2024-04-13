set_errorlist(
	win_T	*wp,
	list_T	*list,
	int	action,
	char_u	*title,
	dict_T	*what)
{
    qf_info_T	*qi = &ql_info;
    int		retval = OK;

    if (wp != NULL)
    {
	qi = ll_get_or_alloc_list(wp);
	if (qi == NULL)
	    return FAIL;
    }

    if (action == 'f')
    {
	// Free the entire quickfix or location list stack
	qf_free_stack(wp, qi);
	return OK;
    }

    // A dict argument cannot be specified with a non-empty list argument
    if (list->lv_len != 0 && what != NULL)
    {
	semsg(_(e_invalid_argument_str),
			 _("cannot have both a list and a \"what\" argument"));
	return FAIL;
    }

    incr_quickfix_busy();

    if (what != NULL)
	retval = qf_set_properties(qi, what, action, title);
    else
    {
	retval = qf_add_entries(qi, qi->qf_curlist, list, title, action);
	if (retval == OK)
	    qf_list_changed(qf_get_curlist(qi));
    }

    decr_quickfix_busy();

    return retval;
}