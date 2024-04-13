qf_getprop_defaults(qf_info_T *qi, int flags, int locstack, dict_T *retdict)
{
    int		status = OK;

    if (flags & QF_GETLIST_TITLE)
	status = dict_add_string(retdict, "title", (char_u *)"");
    if ((status == OK) && (flags & QF_GETLIST_ITEMS))
    {
	list_T	*l = list_alloc();
	if (l != NULL)
	    status = dict_add_list(retdict, "items", l);
	else
	    status = FAIL;
    }
    if ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_number(retdict, "nr", 0);
    if ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_number(retdict, "winid", qf_winid(qi));
    if ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = dict_add_string(retdict, "context", (char_u *)"");
    if ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_number(retdict, "id", 0);
    if ((status == OK) && (flags & QF_GETLIST_IDX))
	status = dict_add_number(retdict, "idx", 0);
    if ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_number(retdict, "size", 0);
    if ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_number(retdict, "changedtick", 0);
    if ((status == OK) && locstack && (flags & QF_GETLIST_FILEWINID))
	status = dict_add_number(retdict, "filewinid", 0);
    if ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    if ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = dict_add_string(retdict, "quickfixtextfunc", (char_u *)"");

    return status;
}