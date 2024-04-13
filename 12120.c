qf_update_buffer(qf_info_T *qi, qfline_T *old_last)
{
    buf_T	*buf;
    win_T	*win;
    aco_save_T	aco;

    // Check if a buffer for the quickfix list exists.  Update it.
    buf = qf_find_buf(qi);
    if (buf != NULL)
    {
	linenr_T	old_line_count = buf->b_ml.ml_line_count;
	int		qf_winid = 0;

	if (IS_LL_STACK(qi))
	{
	    if (curwin->w_llist == qi)
		win = curwin;
	    else
	    {
		// Find the file window (non-quickfix) with this location list
		win = qf_find_win_with_loclist(qi);
		if (win == NULL)
		    // File window is not found. Find the location list window.
		    win = qf_find_win(qi);
		if (win == NULL)
		    return;
	    }
	    qf_winid = win->w_id;
	}

	if (old_last == NULL)
	    // set curwin/curbuf to buf and save a few things
	    aucmd_prepbuf(&aco, buf);

	qf_update_win_titlevar(qi);

	qf_fill_buffer(qf_get_curlist(qi), buf, old_last, qf_winid);
	++CHANGEDTICK(buf);

	if (old_last == NULL)
	{
	    (void)qf_win_pos_update(qi, 0);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}

	// Only redraw when added lines are visible.  This avoids flickering
	// when the added lines are not visible.
	if ((win = qf_find_win(qi)) != NULL && old_line_count < win->w_botline)
	    redraw_buf_later(buf, UPD_NOT_VALID);
    }
}