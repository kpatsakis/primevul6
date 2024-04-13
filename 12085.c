set_qf_ll_list(
    win_T	*wp UNUSED,
    typval_T	*list_arg UNUSED,
    typval_T	*action_arg UNUSED,
    typval_T	*what_arg UNUSED,
    typval_T	*rettv)
{
# ifdef FEAT_QUICKFIX
    char_u	*act;
    int		action = 0;
    static int	recursive = 0;
# endif

    rettv->vval.v_number = -1;

# ifdef FEAT_QUICKFIX
    if (list_arg->v_type != VAR_LIST)
	emsg(_(e_list_required));
    else if (recursive != 0)
	emsg(_(e_autocommand_caused_recursive_behavior));
    else
    {
	list_T  *l = list_arg->vval.v_list;
	dict_T	*what = NULL;
	int	valid_dict = TRUE;

	if (action_arg->v_type == VAR_STRING)
	{
	    act = tv_get_string_chk(action_arg);
	    if (act == NULL)
		return;		// type error; errmsg already given
	    if ((*act == 'a' || *act == 'r' || *act == ' ' || *act == 'f') &&
		    act[1] == NUL)
		action = *act;
	    else
		semsg(_(e_invalid_action_str_1), act);
	}
	else if (action_arg->v_type == VAR_UNKNOWN)
	    action = ' ';
	else
	    emsg(_(e_string_required));

	if (action_arg->v_type != VAR_UNKNOWN
		&& what_arg->v_type != VAR_UNKNOWN)
	{
	    if (what_arg->v_type == VAR_DICT && what_arg->vval.v_dict != NULL)
		what = what_arg->vval.v_dict;
	    else
	    {
		emsg(_(e_dictionary_required));
		valid_dict = FALSE;
	    }
	}

	++recursive;
	if (l != NULL && action && valid_dict
		    && set_errorlist(wp, l, action,
		     (char_u *)(wp == NULL ? ":setqflist()" : ":setloclist()"),
		     what) == OK)
	    rettv->vval.v_number = 0;
	--recursive;
    }
# endif
}