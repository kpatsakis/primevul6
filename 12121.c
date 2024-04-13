qf_init(win_T	    *wp,
	char_u	    *efile,
	char_u	    *errorformat,
	int	    newlist,		// TRUE: start a new error list
	char_u	    *qf_title,
	char_u	    *enc)
{
    qf_info_T	    *qi = &ql_info;

    if (wp != NULL)
    {
	qi = ll_get_or_alloc_list(wp);
	if (qi == NULL)
	    return FAIL;
    }

    return qf_init_ext(qi, qi->qf_curlist, efile, curbuf, NULL, errorformat,
	    newlist, (linenr_T)0, (linenr_T)0, qf_title, enc);
}