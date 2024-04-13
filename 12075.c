qf_restore_list(qf_info_T *qi, int_u save_qfid)
{
    int curlist;

    if (qf_get_curlist(qi)->qf_id != save_qfid)
    {
	curlist = qf_id2nr(qi, save_qfid);
	if (curlist < 0)
	    // list is not present
	    return FAIL;
	qi->qf_curlist = curlist;
    }
    return OK;
}