qf_goto_cwindow(qf_info_T *qi, int resize, int sz, int vertsplit)
{
    win_T	*win;

    win = qf_find_win(qi);
    if (win == NULL)
	return FAIL;

    win_goto(win);
    if (resize)
    {
	if (vertsplit)
	{
	    if (sz != win->w_width)
		win_setwidth(sz);
	}
	else if (sz != win->w_height && win->w_height
		       + win->w_status_height + tabline_height() < cmdline_row)
	    win_setheight(sz);
    }

    return OK;
}