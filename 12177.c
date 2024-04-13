get_qf_loc_list(int is_qf, win_T *wp, typval_T *what_arg, typval_T *rettv)
{
    if (what_arg->v_type == VAR_UNKNOWN)
    {
	if (rettv_list_alloc(rettv) == OK)
	    if (is_qf || wp != NULL)
		(void)get_errorlist(NULL, wp, -1, 0, rettv->vval.v_list);
    }
    else
    {
	if (rettv_dict_alloc(rettv) == OK)
	    if (is_qf || (wp != NULL))
	    {
		if (what_arg->v_type == VAR_DICT)
		{
		    dict_T	*d = what_arg->vval.v_dict;

		    if (d != NULL)
			qf_get_properties(wp, d, rettv->vval.v_dict);
		}
		else
		    emsg(_(e_dictionary_required));
	    }
    }
}