qf_getprop_qftf(qf_list_T *qfl, dict_T *retdict)
{
    int		status;

    if (qfl->qf_qftf_cb.cb_name != NULL)
    {
	typval_T	tv;

	put_callback(&qfl->qf_qftf_cb, &tv);
	status = dict_add_tv(retdict, "quickfixtextfunc", &tv);
	clear_tv(&tv);
    }
    else
	status = dict_add_string(retdict, "quickfixtextfunc", (char_u *)"");

    return status;
}