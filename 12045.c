qf_get_nth_below_entry(qfline_T *entry_arg, int n, int linewise, int *errornr)
{
    qfline_T *entry = entry_arg;

    while (n-- > 0 && !got_int)
    {
	int		first_errornr = *errornr;

	if (linewise)
	    // Treat all the entries on the same line in this file as one
	    entry = qf_find_last_entry_on_line(entry, errornr);

	if (entry->qf_next == NULL
		|| entry->qf_next->qf_fnum != entry->qf_fnum)
	{
	    if (linewise)
		*errornr = first_errornr;
	    break;
	}

	entry = entry->qf_next;
	++*errornr;
    }
}