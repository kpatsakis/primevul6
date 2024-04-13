qf_getprop_filewinid(win_T *wp, qf_info_T *qi, dict_T *retdict)
{
    int winid = 0;

    if (wp != NULL && IS_LL_WINDOW(wp))
    {
	win_T	*ll_wp = qf_find_win_with_loclist(qi);
	if (ll_wp != NULL)
	    winid = ll_wp->w_id;
    }

    return dict_add_number(retdict, "filewinid", winid);
}