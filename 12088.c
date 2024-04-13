qf_jump_goto_line(
	linenr_T	qf_lnum,
	int		qf_col,
	char_u		qf_viscol,
	char_u		*qf_pattern)
{
    linenr_T		i;

    if (qf_pattern == NULL)
    {
	// Go to line with error, unless qf_lnum is 0.
	i = qf_lnum;
	if (i > 0)
	{
	    if (i > curbuf->b_ml.ml_line_count)
		i = curbuf->b_ml.ml_line_count;
	    curwin->w_cursor.lnum = i;
	}
	if (qf_col > 0)
	{
	    curwin->w_cursor.coladd = 0;
	    if (qf_viscol == TRUE)
		coladvance(qf_col - 1);
	    else
		curwin->w_cursor.col = qf_col - 1;
	    curwin->w_set_curswant = TRUE;
	    check_cursor();
	}
	else
	    beginline(BL_WHITE | BL_FIX);
    }
    else
    {
	pos_T save_cursor;

	// Move the cursor to the first line in the buffer
	save_cursor = curwin->w_cursor;
	curwin->w_cursor.lnum = 0;
	if (!do_search(NULL, '/', '/', qf_pattern, (long)1, SEARCH_KEEP, NULL))
	    curwin->w_cursor = save_cursor;
    }
}