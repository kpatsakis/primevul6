qf_age(exarg_T *eap)
{
    qf_info_T	*qi;
    int		count;

    if ((qi = qf_cmd_get_stack(eap, TRUE)) == NULL)
	return;

    if (eap->addr_count != 0)
	count = eap->line2;
    else
	count = 1;
    while (count--)
    {
	if (eap->cmdidx == CMD_colder || eap->cmdidx == CMD_lolder)
	{
	    if (qi->qf_curlist == 0)
	    {
		emsg(_(e_at_bottom_of_quickfix_stack));
		break;
	    }
	    --qi->qf_curlist;
	}
	else
	{
	    if (qi->qf_curlist >= qi->qf_listcount - 1)
	    {
		emsg(_(e_at_top_of_quickfix_stack));
		break;
	    }
	    ++qi->qf_curlist;
	}
    }
    qf_msg(qi, qi->qf_curlist, "");
    qf_update_buffer(qi, NULL);
}