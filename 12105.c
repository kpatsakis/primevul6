wipe_qf_buffer(qf_info_T *qi)
{
    buf_T	*qfbuf;

    if (qi->qf_bufnr != INVALID_QFBUFNR)
    {
	qfbuf = buflist_findnr(qi->qf_bufnr);
	if (qfbuf != NULL && qfbuf->b_nwindows == 0)
	{
	    // If the quickfix buffer is not loaded in any window, then
	    // wipe the buffer.
	    close_buffer(NULL, qfbuf, DOBUF_WIPE, FALSE, FALSE);
	    qi->qf_bufnr = INVALID_QFBUFNR;
	}
    }
}