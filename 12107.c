qf_getprop_idx(qf_list_T *qfl, int eidx, dict_T *retdict)
{
    if (eidx == 0)
    {
	eidx = qfl->qf_index;
	if (qf_list_empty(qfl))
	    // For empty lists, current index is set to 0
	    eidx = 0;
    }
    return dict_add_number(retdict, "idx", eidx);
}