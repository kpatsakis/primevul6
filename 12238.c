qf_find_last_entry_on_line(qfline_T *entry, int *errornr)
{
    while (!got_int &&
	    entry->qf_next != NULL
	    && entry->qf_fnum == entry->qf_next->qf_fnum
	    && entry->qf_lnum == entry->qf_next->qf_lnum)
    {
	entry = entry->qf_next;
	++*errornr;
    }

    return entry;
}