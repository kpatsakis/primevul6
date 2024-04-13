qf_goto_win_with_qfl_file(int qf_fnum)
{
    win_T	*win;
    win_T	*altwin;

    win = curwin;
    altwin = NULL;
    for (;;)
    {
	if (win->w_buffer->b_fnum == qf_fnum)
	    break;
	if (win->w_prev == NULL)
	    win = lastwin;	// wrap around the top
	else
	    win = win->w_prev;	// go to previous window

	if (IS_QF_WINDOW(win))
	{
	    // Didn't find it, go to the window before the quickfix
	    // window, unless 'switchbuf' contains 'uselast': in this case we
	    // try to jump to the previously used window first.
	    if ((swb_flags & SWB_USELAST) && win_valid(prevwin))
		win = prevwin;
	    else if (altwin != NULL)
		win = altwin;
	    else if (curwin->w_prev != NULL)
		win = curwin->w_prev;
	    else
		win = curwin->w_next;
	    break;
	}

	// Remember a usable window.
	if (altwin == NULL && !win->w_p_pvw && bt_normal(win->w_buffer))
	    altwin = win;
    }

    win_goto(win);
}