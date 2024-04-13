qf_find_buf(qf_info_T *qi)
{
    tabpage_T	*tp;
    win_T	*win;

    if (qi->qf_bufnr != INVALID_QFBUFNR)
    {
	buf_T	*qfbuf;
	qfbuf = buflist_findnr(qi->qf_bufnr);
	if (qfbuf != NULL)
	    return qfbuf;
	// buffer is no longer present
	qi->qf_bufnr = INVALID_QFBUFNR;
    }

    FOR_ALL_TAB_WINDOWS(tp, win)
	if (is_qf_win(win, qi))
	    return win->w_buffer;

    return NULL;
}