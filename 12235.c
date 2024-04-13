qf_setprop_items(qf_info_T *qi, int qf_idx, dictitem_T *di, int action)
{
    int		retval = FAIL;
    char_u	*title_save;

    if (di->di_tv.v_type != VAR_LIST)
	return FAIL;

    title_save = vim_strsave(qi->qf_lists[qf_idx].qf_title);
    retval = qf_add_entries(qi, qf_idx, di->di_tv.vval.v_list,
	    title_save, action == ' ' ? 'a' : action);
    vim_free(title_save);

    return retval;
}