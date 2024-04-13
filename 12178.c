qf_id2nr(qf_info_T *qi, int_u qfid)
{
    int		qf_idx;

    for (qf_idx = 0; qf_idx < qi->qf_listcount; qf_idx++)
	if (qi->qf_lists[qf_idx].qf_id == qfid)
	    return qf_idx;
    return INVALID_QFIDX;
}