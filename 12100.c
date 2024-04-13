qf_cmd_get_or_alloc_stack(exarg_T *eap, win_T **pwinp)
{
    qf_info_T	*qi = &ql_info;

    if (is_loclist_cmd(eap->cmdidx))
    {
	qi = ll_get_or_alloc_list(curwin);
	if (qi == NULL)
	    return NULL;
	*pwinp = curwin;
    }

    return qi;
}