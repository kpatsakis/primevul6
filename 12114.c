mark_quickfix_ctx(qf_info_T *qi, int copyID)
{
    int		i;
    int		abort = FALSE;
    typval_T	*ctx;
    callback_T	*cb;

    for (i = 0; i < LISTCOUNT && !abort; ++i)
    {
	ctx = qi->qf_lists[i].qf_ctx;
	if (ctx != NULL && ctx->v_type != VAR_NUMBER
		&& ctx->v_type != VAR_STRING && ctx->v_type != VAR_FLOAT)
	    abort = abort || set_ref_in_item(ctx, copyID, NULL, NULL);

	cb = &qi->qf_lists[i].qf_qftf_cb;
	abort = abort || set_ref_in_callback(cb, copyID);
    }

    return abort;
}