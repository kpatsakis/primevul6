f_setqflist(typval_T *argvars, typval_T *rettv)
{
    if (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	return;

    set_qf_ll_list(NULL, &argvars[0], &argvars[1], &argvars[2], rettv);
}