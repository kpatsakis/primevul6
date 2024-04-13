qf_get_valid_size(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    int		i, sz = 0;
    int		prev_fnum = 0;

    if ((qi = qf_cmd_get_stack(eap, FALSE)) == NULL)
	return 0;

    qfl = qf_get_curlist(qi);
    FOR_ALL_QFL_ITEMS(qfl, qfp, i)
    {
	if (qfp->qf_valid)
	{
	    if (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo)
		sz++;	// Count all valid entries
	    else if (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
	    {
		// Count the number of files
		sz++;
		prev_fnum = qfp->qf_fnum;
	    }
	}
    }

    return sz;
}