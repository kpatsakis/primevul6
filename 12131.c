qf_new_list(qf_info_T *qi, char_u *qf_title)
{
    int		i;
    qf_list_T	*qfl;

    // If the current entry is not the last entry, delete entries beyond
    // the current entry.  This makes it possible to browse in a tree-like
    // way with ":grep".
    while (qi->qf_listcount > qi->qf_curlist + 1)
	qf_free(&qi->qf_lists[--qi->qf_listcount]);

    // When the stack is full, remove to oldest entry
    // Otherwise, add a new entry.
    if (qi->qf_listcount == LISTCOUNT)
    {
	qf_free(&qi->qf_lists[0]);
	for (i = 1; i < LISTCOUNT; ++i)
	    qi->qf_lists[i - 1] = qi->qf_lists[i];
	qi->qf_curlist = LISTCOUNT - 1;
    }
    else
	qi->qf_curlist = qi->qf_listcount++;
    qfl = qf_get_curlist(qi);
    CLEAR_POINTER(qfl);
    qf_store_title(qfl, qf_title);
    qfl->qfl_type = qi->qfl_type;
    qfl->qf_id = ++last_qf_id;
}