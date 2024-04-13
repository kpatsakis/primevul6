nv_dollar(cmdarg_T *cap)
{
    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = TRUE;
    // In virtual mode when off the edge of a line and an operator
    // is pending (whew!) keep the cursor where it is.
    // Otherwise, send it to the end of the line.
    if (!virtual_active() || gchar_cursor() != NUL
					       || cap->oap->op_type == OP_NOP)
	curwin->w_curswant = MAXCOL;	// so we stay at the end
    if (cursor_down((long)(cap->count1 - 1),
					 cap->oap->op_type == OP_NOP) == FAIL)
	clearopbeep(cap->oap);
#ifdef FEAT_FOLDING
    else if ((fdo_flags & FDO_HOR) && KeyTyped && cap->oap->op_type == OP_NOP)
	foldOpenCursor();
#endif
}