nv_scroll(cmdarg_T *cap)
{
    int		used = 0;
    long	n;
#ifdef FEAT_FOLDING
    linenr_T	lnum;
#endif
    int		half;

    cap->oap->motion_type = MLINE;
    setpcmark();

    if (cap->cmdchar == 'L')
    {
	validate_botline();	    // make sure curwin->w_botline is valid
	curwin->w_cursor.lnum = curwin->w_botline - 1;
	if (cap->count1 - 1 >= curwin->w_cursor.lnum)
	    curwin->w_cursor.lnum = 1;
	else
	{
#ifdef FEAT_FOLDING
	    if (hasAnyFolding(curwin))
	    {
		// Count a fold for one screen line.
		for (n = cap->count1 - 1; n > 0
			    && curwin->w_cursor.lnum > curwin->w_topline; --n)
		{
		    (void)hasFolding(curwin->w_cursor.lnum,
						&curwin->w_cursor.lnum, NULL);
		    --curwin->w_cursor.lnum;
		}
	    }
	    else
#endif
		curwin->w_cursor.lnum -= cap->count1 - 1;
	}
    }
    else
    {
	if (cap->cmdchar == 'M')
	{
#ifdef FEAT_DIFF
	    // Don't count filler lines above the window.
	    used -= diff_check_fill(curwin, curwin->w_topline)
							  - curwin->w_topfill;
#endif
	    validate_botline();	    // make sure w_empty_rows is valid
	    half = (curwin->w_height - curwin->w_empty_rows + 1) / 2;
	    for (n = 0; curwin->w_topline + n < curbuf->b_ml.ml_line_count; ++n)
	    {
#ifdef FEAT_DIFF
		// Count half he number of filler lines to be "below this
		// line" and half to be "above the next line".
		if (n > 0 && used + diff_check_fill(curwin, curwin->w_topline
							     + n) / 2 >= half)
		{
		    --n;
		    break;
		}
#endif
		used += plines(curwin->w_topline + n);
		if (used >= half)
		    break;
#ifdef FEAT_FOLDING
		if (hasFolding(curwin->w_topline + n, NULL, &lnum))
		    n = lnum - curwin->w_topline;
#endif
	    }
	    if (n > 0 && used > curwin->w_height)
		--n;
	}
	else // (cap->cmdchar == 'H')
	{
	    n = cap->count1 - 1;
#ifdef FEAT_FOLDING
	    if (hasAnyFolding(curwin))
	    {
		// Count a fold for one screen line.
		lnum = curwin->w_topline;
		while (n-- > 0 && lnum < curwin->w_botline - 1)
		{
		    (void)hasFolding(lnum, NULL, &lnum);
		    ++lnum;
		}
		n = lnum - curwin->w_topline;
	    }
#endif
	}
	curwin->w_cursor.lnum = curwin->w_topline + n;
	if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
    }

    // Correct for 'so', except when an operator is pending.
    if (cap->oap->op_type == OP_NOP)
	cursor_correct();
    beginline(BL_SOL | BL_FIX);
}