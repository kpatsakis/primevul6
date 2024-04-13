nv_goto(cmdarg_T *cap)
{
    linenr_T	lnum;

    if (cap->arg)
	lnum = curbuf->b_ml.ml_line_count;
    else
	lnum = 1L;
    cap->oap->motion_type = MLINE;
    setpcmark();

    // When a count is given, use it instead of the default lnum
    if (cap->count0 != 0)
	lnum = cap->count0;
    if (lnum < 1L)
	lnum = 1L;
    else if (lnum > curbuf->b_ml.ml_line_count)
	lnum = curbuf->b_ml.ml_line_count;
    curwin->w_cursor.lnum = lnum;
    beginline(BL_SOL | BL_FIX);
#ifdef FEAT_FOLDING
    if ((fdo_flags & FDO_JUMP) && KeyTyped && cap->oap->op_type == OP_NOP)
	foldOpenCursor();
#endif
}