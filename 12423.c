nv_g_underscore_cmd(cmdarg_T *cap)
{
    char_u  *ptr;

    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = TRUE;
    curwin->w_curswant = MAXCOL;
    if (cursor_down((long)(cap->count1 - 1),
					cap->oap->op_type == OP_NOP) == FAIL)
    {
	clearopbeep(cap->oap);
	return;
    }

    ptr = ml_get_curline();

    // In Visual mode we may end up after the line.
    if (curwin->w_cursor.col > 0 && ptr[curwin->w_cursor.col] == NUL)
	--curwin->w_cursor.col;

    // Decrease the cursor column until it's on a non-blank.
    while (curwin->w_cursor.col > 0
	    && VIM_ISWHITE(ptr[curwin->w_cursor.col]))
	--curwin->w_cursor.col;
    curwin->w_set_curswant = TRUE;
    adjust_for_sel(cap);
}