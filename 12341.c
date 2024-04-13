nv_left(cmdarg_T *cap)
{
    long	n;

    if (mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL))
    {
	// <C-Left> and <S-Left> move a word or WORD left
	if (mod_mask & MOD_MASK_CTRL)
	    cap->arg = 1;
	nv_bck_word(cap);
	return;
    }

    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = FALSE;
    for (n = cap->count1; n > 0; --n)
    {
	if (oneleft() == FAIL)
	{
	    // <BS> and <Del> wrap to previous line if 'whichwrap' has 'b'.
	    //		 'h' wraps to previous line if 'whichwrap' has 'h'.
	    //	   CURS_LEFT wraps to previous line if 'whichwrap' has '<'.
	    if (       (((cap->cmdchar == K_BS
				|| cap->cmdchar == Ctrl_H)
			    && vim_strchr(p_ww, 'b') != NULL)
			|| (cap->cmdchar == 'h'
			    && vim_strchr(p_ww, 'h') != NULL)
			|| (cap->cmdchar == K_LEFT
			    && vim_strchr(p_ww, '<') != NULL))
		    && curwin->w_cursor.lnum > 1)
	    {
		--(curwin->w_cursor.lnum);
		coladvance((colnr_T)MAXCOL);
		curwin->w_set_curswant = TRUE;

		// When the NL before the first char has to be deleted we
		// put the cursor on the NUL after the previous line.
		// This is a very special case, be careful!
		// Don't adjust op_end now, otherwise it won't work.
		if (	   (cap->oap->op_type == OP_DELETE
			    || cap->oap->op_type == OP_CHANGE)
			&& !LINEEMPTY(curwin->w_cursor.lnum))
		{
		    char_u *cp = ml_get_cursor();

		    if (*cp != NUL)
		    {
			if (has_mbyte)
			    curwin->w_cursor.col += (*mb_ptr2len)(cp);
			else
			    ++curwin->w_cursor.col;
		    }
		    cap->retval |= CA_NO_ADJ_OP_END;
		}
		continue;
	    }
	    // Only beep and flush if not moved at all
	    else if (cap->oap->op_type == OP_NOP && n == cap->count1)
		beep_flush();
	    break;
	}
    }
#ifdef FEAT_FOLDING
    if (n != cap->count1 && (fdo_flags & FDO_HOR) && KeyTyped
					       && cap->oap->op_type == OP_NOP)
	foldOpenCursor();
#endif
}