qflist_valid(win_T *wp, int_u qf_id)
{
    qf_info_T	*qi = &ql_info;
    int		i;

    if (wp != NULL)
    {
	if (!win_valid(wp))
	    return FALSE;
	qi = GET_LOC_LIST(wp);	    // Location list
	if (qi == NULL)
	    return FALSE;
    }

    for (i = 0; i < qi->qf_listcount; ++i)
	if (qi->qf_lists[i].qf_id == qf_id)
	    return TRUE;

    return FALSE;
}