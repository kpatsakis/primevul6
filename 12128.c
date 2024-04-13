qf_find_nth_adj_entry(
	qf_list_T	*qfl,
	int		bnr,
	pos_T		*pos,
	int		n,
	int		dir,
	int		linewise)
{
    qfline_T	*adj_entry;
    int		errornr;

    // Find an entry closest to the specified position
    adj_entry = qf_find_closest_entry(qfl, bnr, pos, dir, linewise, &errornr);
    if (adj_entry == NULL)
	return 0;

    if (--n > 0)
    {
	// Go to the n'th entry in the current buffer
	if (dir == FORWARD)
	    qf_get_nth_below_entry(adj_entry, n, linewise, &errornr);
	else
	    qf_get_nth_above_entry(adj_entry, n, linewise, &errornr);
    }

    return errornr;
}