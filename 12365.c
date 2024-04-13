scroll_redraw(int up, long count)
{
    linenr_T	prev_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    int		prev_topfill = curwin->w_topfill;
#endif
    linenr_T	prev_lnum = curwin->w_cursor.lnum;

    if (up)
	scrollup(count, TRUE);
    else
	scrolldown(count, TRUE);
    if (get_scrolloff_value())
    {
	// Adjust the cursor position for 'scrolloff'.  Mark w_topline as
	// valid, otherwise the screen jumps back at the end of the file.
	cursor_correct();
	check_cursor_moved(curwin);
	curwin->w_valid |= VALID_TOPLINE;

	// If moved back to where we were, at least move the cursor, otherwise
	// we get stuck at one position.  Don't move the cursor up if the
	// first line of the buffer is already on the screen
	while (curwin->w_topline == prev_topline
#ifdef FEAT_DIFF
		&& curwin->w_topfill == prev_topfill
#endif
		)
	{
	    if (up)
	    {
		if (curwin->w_cursor.lnum > prev_lnum
			|| cursor_down(1L, FALSE) == FAIL)
		    break;
	    }
	    else
	    {
		if (curwin->w_cursor.lnum < prev_lnum
			|| prev_topline == 1L
			|| cursor_up(1L, FALSE) == FAIL)
		    break;
	    }
	    // Mark w_topline as valid, otherwise the screen jumps back at the
	    // end of the file.
	    check_cursor_moved(curwin);
	    curwin->w_valid |= VALID_TOPLINE;
	}
    }
    if (curwin->w_cursor.lnum != prev_lnum)
	coladvance(curwin->w_curswant);
    redraw_later(VALID);
}