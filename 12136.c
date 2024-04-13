get_errorlist(
	qf_info_T	*qi_arg,
	win_T		*wp,
	int		qf_idx,
	int		eidx,
	list_T		*list)
{
    qf_info_T	*qi = qi_arg;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    int		i;

    if (qi == NULL)
    {
	qi = &ql_info;
	if (wp != NULL)
	{
	    qi = GET_LOC_LIST(wp);
	    if (qi == NULL)
		return FAIL;
	}
    }

    if (eidx < 0)
	return OK;

    if (qf_idx == INVALID_QFIDX)
	qf_idx = qi->qf_curlist;

    if (qf_idx >= qi->qf_listcount)
	return FAIL;

    qfl = qf_get_list(qi, qf_idx);
    if (qf_list_empty(qfl))
	return FAIL;

    FOR_ALL_QFL_ITEMS(qfl, qfp, i)
    {
	if (eidx > 0)
	{
	    if (eidx == i)
		return get_qfline_items(qfp, list);
	}
	else if (get_qfline_items(qfp, list) == FAIL)
	    return FAIL;
    }

    return OK;
}