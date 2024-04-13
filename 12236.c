get_next_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	int		*qf_index,
	int		dir)
{
    int			idx;
    int			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    do
    {
	if (idx == qfl->qf_count || qf_ptr->qf_next == NULL)
	    return NULL;
	++idx;
	qf_ptr = qf_ptr->qf_next;
    } while ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == FORWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    return qf_ptr;
}