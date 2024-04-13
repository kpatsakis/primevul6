nv_g_dollar_cmd(cmdarg_T *cap)
{
    oparg_T	*oap = cap->oap;
    int		i;
    int		col_off = curwin_col_off();

    oap->motion_type = MCHAR;
    oap->inclusive = TRUE;
    if (curwin->w_p_wrap && curwin->w_width != 0)
    {
	curwin->w_curswant = MAXCOL;    // so we stay at the end
	if (cap->count1 == 1)
	{
	    int		width1 = curwin->w_width - col_off;
	    int		width2 = width1 + curwin_col_off2();

	    validate_virtcol();
	    i = width1 - 1;
	    if (curwin->w_virtcol >= (colnr_T)width1)
		i += ((curwin->w_virtcol - width1) / width2 + 1)
		    * width2;
	    coladvance((colnr_T)i);

	    // Make sure we stick in this column.
	    validate_virtcol();
	    curwin->w_curswant = curwin->w_virtcol;
	    curwin->w_set_curswant = FALSE;
	    if (curwin->w_cursor.col > 0 && curwin->w_p_wrap)
	    {
		// Check for landing on a character that got split at
		// the end of the line.  We do not want to advance to
		// the next screen line.
		if (curwin->w_virtcol > (colnr_T)i)
		    --curwin->w_cursor.col;
	    }
	}
	else if (nv_screengo(oap, FORWARD, cap->count1 - 1) == FAIL)
	    clearopbeep(oap);
    }
    else
    {
	if (cap->count1 > 1)
	    // if it fails, let the cursor still move to the last char
	    (void)cursor_down(cap->count1 - 1, FALSE);

	i = curwin->w_leftcol + curwin->w_width - col_off - 1;
	coladvance((colnr_T)i);

	// if the character doesn't fit move one back
	if (curwin->w_cursor.col > 0
		&& (*mb_ptr2cells)(ml_get_cursor()) > 1)
	{
	    colnr_T vcol;

	    getvvcol(curwin, &curwin->w_cursor, NULL, NULL, &vcol);
	    if (vcol >= curwin->w_leftcol + curwin->w_width - col_off)
		--curwin->w_cursor.col;
	}

	// Make sure we stick in this column.
	validate_virtcol();
	curwin->w_curswant = curwin->w_virtcol;
	curwin->w_set_curswant = FALSE;
    }
}