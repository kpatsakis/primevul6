qf_get_list_from_lines(dict_T *what, dictitem_T *di, dict_T *retdict)
{
    int		status = FAIL;
    qf_info_T	*qi;
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    list_T	*l;

    // Only a List value is supported
    if (di->di_tv.v_type == VAR_LIST && di->di_tv.vval.v_list != NULL)
    {
	// If errorformat is supplied then use it, otherwise use the 'efm'
	// option setting
	if ((efm_di = dict_find(what, (char_u *)"efm", -1)) != NULL)
	{
	    if (efm_di->di_tv.v_type != VAR_STRING ||
		    efm_di->di_tv.vval.v_string == NULL)
		return FAIL;
	    errorformat = efm_di->di_tv.vval.v_string;
	}

	l = list_alloc();
	if (l == NULL)
	    return FAIL;

	qi = qf_alloc_stack(QFLT_INTERNAL);
	if (qi != NULL)
	{
	    if (qf_init_ext(qi, 0, NULL, NULL, &di->di_tv, errorformat,
			TRUE, (linenr_T)0, (linenr_T)0, NULL, NULL) > 0)
	    {
		(void)get_errorlist(qi, NULL, 0, 0, l);
		qf_free(&qi->qf_lists[0]);
	    }
	    free(qi);
	}
	dict_add_list(retdict, "items", l);
	status = OK;
    }

    return status;
}