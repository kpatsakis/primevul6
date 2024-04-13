qf_getprop_items(qf_info_T *qi, int qf_idx, int eidx, dict_T *retdict)
{
    int		status = OK;
    list_T	*l = list_alloc();
    if (l != NULL)
    {
	(void)get_errorlist(qi, NULL, qf_idx, eidx, l);
	dict_add_list(retdict, "items", l);
    }
    else
	status = FAIL;

    return status;
}