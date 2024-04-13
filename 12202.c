get_nth_valid_entry(
	qf_list_T	*qfl,
	int		errornr,
	int		dir,
	int		*new_qfidx)
{
    qfline_T		*qf_ptr = qfl->qf_ptr;
    int			qf_idx = qfl->qf_index;
    qfline_T		*prev_qf_ptr;
    int			prev_index;
    char		*err = e_no_more_items;

    while (errornr--)
    {
	prev_qf_ptr = qf_ptr;
	prev_index = qf_idx;

	if (dir == FORWARD || dir == FORWARD_FILE)
	    qf_ptr = get_next_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	else
	    qf_ptr = get_prev_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	if (qf_ptr == NULL)
	{
	    qf_ptr = prev_qf_ptr;
	    qf_idx = prev_index;
	    if (err != NULL)
	    {
		emsg(_(err));
		return NULL;
	    }
	    break;
	}

	err = NULL;
    }

    *new_qfidx = qf_idx;
    return qf_ptr;
}