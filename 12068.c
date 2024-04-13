qf_find_entry_after_pos(
	int		bnr,
	pos_T		*pos,
	int		linewise,
	qfline_T	*qfp,
	int		*errornr)
{
    if (qf_entry_after_pos(qfp, pos, linewise))
	// First entry is after position 'pos'
	return qfp;

    // Find the entry just before or at the position 'pos'
    while (qfp->qf_next != NULL
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_on_or_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    if (qfp->qf_next == NULL || qfp->qf_next->qf_fnum != bnr)
	// No entries found after position 'pos'
	return NULL;

    // Use the entry just after position 'pos'
    qfp = qfp->qf_next;
    ++*errornr;

    return qfp;
}