qf_setprop_title(qf_info_T *qi, int qf_idx, dict_T *what, dictitem_T *di)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);

    if (di->di_tv.v_type != VAR_STRING)
	return FAIL;

    vim_free(qfl->qf_title);
    qfl->qf_title = dict_get_string(what, "title", TRUE);
    if (qf_idx == qi->qf_curlist)
	qf_update_win_titlevar(qi);

    return OK;
}