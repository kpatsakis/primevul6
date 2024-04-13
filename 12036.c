ex_cc(exarg_T *eap)
{
    qf_info_T	*qi;
    int		errornr;

    if ((qi = qf_cmd_get_stack(eap, TRUE)) == NULL)
	return;

    if (eap->addr_count > 0)
	errornr = (int)eap->line2;
    else
    {
	switch (eap->cmdidx)
	{
	    case CMD_cc: case CMD_ll:
		errornr = 0;
		break;
	    case CMD_crewind: case CMD_lrewind: case CMD_cfirst:
	    case CMD_lfirst:
		errornr = 1;
		break;
	    default:
		errornr = 32767;
	}
    }

    // For cdo and ldo commands, jump to the nth valid error.
    // For cfdo and lfdo commands, jump to the nth valid file entry.
    if (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
	    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	errornr = qf_get_nth_valid_entry(qf_get_curlist(qi),
		eap->addr_count > 0 ? (int)eap->line1 : 1,
		eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo);

    qf_jump(qi, 0, errornr, eap->forceit);
}