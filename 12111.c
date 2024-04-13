qf_get_cur_idx(exarg_T *eap)
{
    qf_info_T	*qi;

    if ((qi = qf_cmd_get_stack(eap, FALSE)) == NULL)
	return 0;

    return qf_get_curlist(qi)->qf_index;
}