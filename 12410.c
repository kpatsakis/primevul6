nv_percent(cmdarg_T *cap)
{
    pos_T	*pos;
#if defined(FEAT_FOLDING)
    linenr_T	lnum = curwin->w_cursor.lnum;
#endif

    cap->oap->inclusive = TRUE;
    if (cap->count0)	    // {cnt}% : goto {cnt} percentage in file
    {
	if (cap->count0 > 100)
	    clearopbeep(cap->oap);
	else
	{
	    cap->oap->motion_type = MLINE;
	    setpcmark();
	    // Round up, so 'normal 100%' always jumps at the line line.
	    // Beyond 21474836 lines, (ml_line_count * 100 + 99) would
	    // overflow on 32-bits, so use a formula with less accuracy
	    // to avoid overflows.
	    if (curbuf->b_ml.ml_line_count >= 21474836)
		curwin->w_cursor.lnum = (curbuf->b_ml.ml_line_count + 99L)
							 / 100L * cap->count0;
	    else
		curwin->w_cursor.lnum = (curbuf->b_ml.ml_line_count *
						    cap->count0 + 99L) / 100L;
	    if (curwin->w_cursor.lnum < 1)
		curwin->w_cursor.lnum = 1;
	    if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	    beginline(BL_SOL | BL_FIX);
	}
    }
    else		    // "%" : go to matching paren
    {
	cap->oap->motion_type = MCHAR;
	cap->oap->use_reg_one = TRUE;
	if ((pos = findmatch(cap->oap, NUL)) == NULL)
	    clearopbeep(cap->oap);
	else
	{
	    setpcmark();
	    curwin->w_cursor = *pos;
	    curwin->w_set_curswant = TRUE;
	    curwin->w_cursor.coladd = 0;
	    adjust_for_sel(cap);
	}
    }
#ifdef FEAT_FOLDING
    if (cap->oap->op_type == OP_NOP
	    && lnum != curwin->w_cursor.lnum
	    && (fdo_flags & FDO_PERCENT)
	    && KeyTyped)
	foldOpenCursor();
#endif
}