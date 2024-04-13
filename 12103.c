qf_getprop_keys2flags(dict_T *what, int loclist)
{
    int		flags = QF_GETLIST_NONE;

    if (dict_has_key(what, "all"))
    {
	flags |= QF_GETLIST_ALL;
	if (!loclist)
	    // File window ID is applicable only to location list windows
	    flags &= ~ QF_GETLIST_FILEWINID;
    }

    if (dict_has_key(what, "title"))
	flags |= QF_GETLIST_TITLE;

    if (dict_has_key(what, "nr"))
	flags |= QF_GETLIST_NR;

    if (dict_has_key(what, "winid"))
	flags |= QF_GETLIST_WINID;

    if (dict_has_key(what, "context"))
	flags |= QF_GETLIST_CONTEXT;

    if (dict_has_key(what, "id"))
	flags |= QF_GETLIST_ID;

    if (dict_has_key(what, "items"))
	flags |= QF_GETLIST_ITEMS;

    if (dict_has_key(what, "idx"))
	flags |= QF_GETLIST_IDX;

    if (dict_has_key(what, "size"))
	flags |= QF_GETLIST_SIZE;

    if (dict_has_key(what, "changedtick"))
	flags |= QF_GETLIST_TICK;

    if (loclist && dict_has_key(what, "filewinid"))
	flags |= QF_GETLIST_FILEWINID;

    if (dict_has_key(what, "qfbufnr"))
	flags |= QF_GETLIST_QFBUFNR;

    if (dict_has_key(what, "quickfixtextfunc"))
	flags |= QF_GETLIST_QFTF;

    return flags;
}