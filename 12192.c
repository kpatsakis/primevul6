qf_setprop_context(qf_list_T *qfl, dictitem_T *di)
{
    typval_T	*ctx;

    free_tv(qfl->qf_ctx);
    ctx =  alloc_tv();
    if (ctx != NULL)
	copy_tv(&di->di_tv, ctx);
    qfl->qf_ctx = ctx;

    return OK;
}