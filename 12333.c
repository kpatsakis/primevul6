nv_gomark(cmdarg_T *cap)
{
    pos_T	*pos;
    int		c;
#ifdef FEAT_FOLDING
    pos_T	old_cursor = curwin->w_cursor;
    int		old_KeyTyped = KeyTyped;    // getting file may reset it
#endif

    if (cap->cmdchar == 'g')
	c = cap->extra_char;
    else
	c = cap->nchar;
    pos = getmark(c, (cap->oap->op_type == OP_NOP));
    if (pos == (pos_T *)-1)	    // jumped to other file
    {
	if (cap->arg)
	{
	    check_cursor_lnum();
	    beginline(BL_WHITE | BL_FIX);
	}
	else
	    check_cursor();
    }
    else
	nv_cursormark(cap, cap->arg, pos);

    // May need to clear the coladd that a mark includes.
    if (!virtual_active())
	curwin->w_cursor.coladd = 0;
    check_cursor_col();
#ifdef FEAT_FOLDING
    if (cap->oap->op_type == OP_NOP
	    && pos != NULL
	    && (pos == (pos_T *)-1 || !EQUAL_POS(old_cursor, *pos))
	    && (fdo_flags & FDO_MARK)
	    && old_KeyTyped)
	foldOpenCursor();
#endif
}