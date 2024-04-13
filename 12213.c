get_qfline_items(qfline_T *qfp, list_T *list)
{
    int		bufnum;
    dict_T	*dict;
    char_u	buf[2];

    // Handle entries with a non-existing buffer number.
    bufnum = qfp->qf_fnum;
    if (bufnum != 0 && (buflist_findnr(bufnum) == NULL))
	bufnum = 0;

    if ((dict = dict_alloc()) == NULL)
	return FAIL;
    if (list_append_dict(list, dict) == FAIL)
	return FAIL;

    buf[0] = qfp->qf_type;
    buf[1] = NUL;
    if (dict_add_number(dict, "bufnr", (long)bufnum) == FAIL
	    || dict_add_number(dict, "lnum",     (long)qfp->qf_lnum) == FAIL
	    || dict_add_number(dict, "end_lnum", (long)qfp->qf_end_lnum) == FAIL
	    || dict_add_number(dict, "col",      (long)qfp->qf_col) == FAIL
	    || dict_add_number(dict, "end_col",  (long)qfp->qf_end_col) == FAIL
	    || dict_add_number(dict, "vcol",     (long)qfp->qf_viscol) == FAIL
	    || dict_add_number(dict, "nr",       (long)qfp->qf_nr) == FAIL
	    || dict_add_string(dict, "module", qfp->qf_module) == FAIL
	    || dict_add_string(dict, "pattern", qfp->qf_pattern) == FAIL
	    || dict_add_string(dict, "text", qfp->qf_text) == FAIL
	    || dict_add_string(dict, "type", buf) == FAIL
	    || dict_add_number(dict, "valid", (long)qfp->qf_valid) == FAIL)
	return FAIL;

    return OK;
}