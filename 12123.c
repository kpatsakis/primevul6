f_getqflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    if (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	return;

    get_qf_loc_list(TRUE, NULL, &argvars[0], rettv);
# endif
}