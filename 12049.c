qf_setprop_qftf(qf_info_T *qi UNUSED, qf_list_T *qfl, dictitem_T *di)
{
    callback_T	cb;

    free_callback(&qfl->qf_qftf_cb);
    cb = get_callback(&di->di_tv);
    if (cb.cb_name != NULL && *cb.cb_name != NUL)
	set_callback(&qfl->qf_qftf_cb, &cb);

    return OK;
}