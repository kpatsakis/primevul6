qf_free_all(win_T *wp)
{
    int		i;
    qf_info_T	*qi = &ql_info;

    if (wp != NULL)
    {
	// location list
	ll_free_all(&wp->w_llist);
	ll_free_all(&wp->w_llist_ref);
    }
    else
	// quickfix list
	for (i = 0; i < qi->qf_listcount; ++i)
	    qf_free(qf_get_list(qi, i));
}