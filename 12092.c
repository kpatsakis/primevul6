qf_cmd_get_stack(exarg_T *eap, int print_emsg)
{
    qf_info_T	*qi = &ql_info;

    if (is_loclist_cmd(eap->cmdidx))
    {
	qi = GET_LOC_LIST(curwin);
	if (qi == NULL)
	{
	    if (print_emsg)
		emsg(_(e_no_location_list));
	    return NULL;
	}
    }

    return qi;
}