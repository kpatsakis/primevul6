qf_find_closest_entry(
	qf_list_T	*qfl,
	int		bnr,
	pos_T		*pos,
	int		dir,
	int		linewise,
	int		*errornr)
{
    qfline_T	*qfp;

    *errornr = 0;

    // Find the first entry in this file
    qfp = qf_find_first_entry_in_buf(qfl, bnr, errornr);
    if (qfp == NULL)
	return NULL;		// no entry in this file

    if (dir == FORWARD)
	qfp = qf_find_entry_after_pos(bnr, pos, linewise, qfp, errornr);
    else
	qfp = qf_find_entry_before_pos(bnr, pos, linewise, qfp, errornr);

    return qfp;
}