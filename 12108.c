qf_get_properties(win_T *wp, dict_T *what, dict_T *retdict)
{
    qf_info_T	*qi = &ql_info;
    qf_list_T	*qfl;
    int		status = OK;
    int		qf_idx = INVALID_QFIDX;
    int		eidx = 0;
    dictitem_T	*di;
    int		flags = QF_GETLIST_NONE;

    if ((di = dict_find(what, (char_u *)"lines", -1)) != NULL)
	return qf_get_list_from_lines(what, di, retdict);

    if (wp != NULL)
	qi = GET_LOC_LIST(wp);

    flags = qf_getprop_keys2flags(what, (wp != NULL));

    if (!qf_stack_empty(qi))
	qf_idx = qf_getprop_qfidx(qi, what);

    // List is not present or is empty
    if (qf_stack_empty(qi) || qf_idx == INVALID_QFIDX)
	return qf_getprop_defaults(qi, flags, wp != NULL, retdict);

    qfl = qf_get_list(qi, qf_idx);

    // If an entry index is specified, use that
    if ((di = dict_find(what, (char_u *)"idx", -1)) != NULL)
    {
	if (di->di_tv.v_type != VAR_NUMBER)
	    return FAIL;
	eidx = di->di_tv.vval.v_number;
    }

    if (flags & QF_GETLIST_TITLE)
	status = qf_getprop_title(qfl, retdict);
    if ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_number(retdict, "nr", qf_idx + 1);
    if ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_number(retdict, "winid", qf_winid(qi));
    if ((status == OK) && (flags & QF_GETLIST_ITEMS))
	status = qf_getprop_items(qi, qf_idx, eidx, retdict);
    if ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = qf_getprop_ctx(qfl, retdict);
    if ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_number(retdict, "id", qfl->qf_id);
    if ((status == OK) && (flags & QF_GETLIST_IDX))
	status = qf_getprop_idx(qfl, eidx, retdict);
    if ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_number(retdict, "size", qfl->qf_count);
    if ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_number(retdict, "changedtick", qfl->qf_changedtick);
    if ((status == OK) && (wp != NULL) && (flags & QF_GETLIST_FILEWINID))
	status = qf_getprop_filewinid(wp, qi, retdict);
    if ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    if ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = qf_getprop_qftf(qfl, retdict);

    return status;
}