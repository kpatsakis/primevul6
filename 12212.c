qf_getprop_qfbufnr(qf_info_T *qi, dict_T *retdict)
{
    int	bufnum = 0;

    if (qi != NULL && buflist_findnr(qi->qf_bufnr) != NULL)
	bufnum = qi->qf_bufnr;

    return dict_add_number(retdict, "qfbufnr", bufnum);
}