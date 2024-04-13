ex_cnext(exarg_T *eap)
{
    qf_info_T	*qi;
    int		errornr;
    int		dir;

    if ((qi = qf_cmd_get_stack(eap, TRUE)) == NULL)
	return;

    if (eap->addr_count > 0
	    && (eap->cmdidx != CMD_cdo && eap->cmdidx != CMD_ldo
		&& eap->cmdidx != CMD_cfdo && eap->cmdidx != CMD_lfdo))
	errornr = (int)eap->line2;
    else
	errornr = 1;

    // Depending on the command jump to either next or previous entry/file.
    switch (eap->cmdidx)
    {
	case CMD_cnext: case CMD_lnext: case CMD_cdo: case CMD_ldo:
	    dir = FORWARD;
	    break;
	case CMD_cprevious: case CMD_lprevious: case CMD_cNext:
	case CMD_lNext:
	    dir = BACKWARD;
	    break;
	case CMD_cnfile: case CMD_lnfile: case CMD_cfdo: case CMD_lfdo:
	    dir = FORWARD_FILE;
	    break;
	case CMD_cpfile: case CMD_lpfile: case CMD_cNfile: case CMD_lNfile:
	    dir = BACKWARD_FILE;
	    break;
	default:
	    dir = FORWARD;
	    break;
    }

    qf_jump(qi, dir, errornr, eap->forceit);
}