qf_getprop_ctx(qf_list_T *qfl, dict_T *retdict)
{
    int		status;
    dictitem_T	*di;

    if (qfl->qf_ctx != NULL)
    {
	di = dictitem_alloc((char_u *)"context");
	if (di != NULL)
	{
	    copy_tv(qfl->qf_ctx, &di->di_tv);
	    status = dict_add(retdict, di);
	    if (status == FAIL)
		dictitem_free(di);
	}
	else
	    status = FAIL;
    }
    else
	status = dict_add_string(retdict, "context", (char_u *)"");

    return status;
}