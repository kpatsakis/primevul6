qf_get_nth_valid_entry(qf_list_T *qfl, int n, int fdo)
{
    qfline_T	*qfp;
    int		i, eidx;
    int		prev_fnum = 0;

    // check if the list has valid errors
    if (!qf_list_has_valid_entries(qfl))
	return 1;

    eidx = 0;
    FOR_ALL_QFL_ITEMS(qfl, qfp, i)
    {
	if (qfp->qf_valid)
	{
	    if (fdo)
	    {
		if (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    else
		eidx++;
	}

	if (eidx == n)
	    break;
    }

    if (i <= qfl->qf_count)
	return i;
    else
	return 1;
}