qf_get_nth_above_entry(qfline_T *entry, int n, int linewise, int *errornr)
{
    while (n-- > 0 && !got_int)
    {
	if (entry->qf_prev == NULL
		|| entry->qf_prev->qf_fnum != entry->qf_fnum)
	    break;

	entry = entry->qf_prev;
	--*errornr;

	// If multiple entries are on the same line, then use the first entry
	if (linewise)
	    entry = qf_find_first_entry_on_line(entry, errornr);
    }
}