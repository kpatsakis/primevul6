qf_alloc_stack(qfltype_T qfltype)
{
    qf_info_T *qi;

    qi = ALLOC_CLEAR_ONE_ID(qf_info_T, aid_qf_qfinfo);
    if (qi != NULL)
    {
	qi->qf_refcount++;
	qi->qfl_type = qfltype;
	qi->qf_bufnr = INVALID_QFBUFNR;
    }
    return qi;
}