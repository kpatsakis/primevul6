nv_pcmark(cmdarg_T *cap)
{
    pos_T	*pos;
#ifdef FEAT_FOLDING
    linenr_T	lnum = curwin->w_cursor.lnum;
    int		old_KeyTyped = KeyTyped;    // getting file may reset it
#endif

    if (!checkclearopq(cap->oap))
    {
	if (cap->cmdchar == TAB && mod_mask == MOD_MASK_CTRL)
	{
	    if (goto_tabpage_lastused() == FAIL)
		clearopbeep(cap->oap);
	    return;
	}
	if (cap->cmdchar == 'g')
	    pos = movechangelist((int)cap->count1);
	else
	    pos = movemark((int)cap->count1);
	if (pos == (pos_T *)-1)		// jump to other file
	{
	    curwin->w_set_curswant = TRUE;
	    check_cursor();
	}
	else if (pos != NULL)		    // can jump
	    nv_cursormark(cap, FALSE, pos);
	else if (cap->cmdchar == 'g')
	{
	    if (curbuf->b_changelistlen == 0)
		emsg(_(e_changelist_is_empty));
	    else if (cap->count1 < 0)
		emsg(_(e_at_start_of_changelist));
	    else
		emsg(_(e_at_end_of_changelist));
	}
	else
	    clearopbeep(cap->oap);
# ifdef FEAT_FOLDING
	if (cap->oap->op_type == OP_NOP
		&& (pos == (pos_T *)-1 || lnum != curwin->w_cursor.lnum)
		&& (fdo_flags & FDO_MARK)
		&& old_KeyTyped)
	    foldOpenCursor();
# endif
    }
}