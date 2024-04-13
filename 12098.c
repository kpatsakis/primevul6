qf_history(exarg_T *eap)
{
    qf_info_T	*qi = qf_cmd_get_stack(eap, FALSE);
    int		i;

    if (eap->addr_count > 0)
    {
	if (qi == NULL)
	{
	    emsg(_(e_no_location_list));
	    return;
	}

	// Jump to the specified quickfix list
	if (eap->line2 > 0 && eap->line2 <= qi->qf_listcount)
	{
	    qi->qf_curlist = eap->line2 - 1;
	    qf_msg(qi, qi->qf_curlist, "");
	    qf_update_buffer(qi, NULL);
	}
	else
	    emsg(_(e_invalid_range));

	return;
    }

    if (qf_stack_empty(qi))
	msg(_("No entries"));
    else
	for (i = 0; i < qi->qf_listcount; ++i)
	    qf_msg(qi, i, i == qi->qf_curlist ? "> " : "  ");
}