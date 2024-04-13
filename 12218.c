qf_winid(qf_info_T *qi)
{
    win_T	*win;

    // The quickfix window can be opened even if the quickfix list is not set
    // using ":copen". This is not true for location lists.
    if (qi == NULL)
	return 0;
    win = qf_find_win(qi);
    if (win != NULL)
	return win->w_id;
    return 0;
}