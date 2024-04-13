nv_edit(cmdarg_T *cap)
{
    // <Insert> is equal to "i"
    if (cap->cmdchar == K_INS || cap->cmdchar == K_KINS)
	cap->cmdchar = 'i';

    // in Visual mode "A" and "I" are an operator
    if (VIsual_active && (cap->cmdchar == 'A' || cap->cmdchar == 'I'))
    {
#ifdef FEAT_TERMINAL
	if (term_in_normal_mode())
	{
	    end_visual_mode();
	    clearop(cap->oap);
	    term_enter_job_mode();
	    return;
	}
#endif
	v_visop(cap);
    }

    // in Visual mode and after an operator "a" and "i" are for text objects
    else if ((cap->cmdchar == 'a' || cap->cmdchar == 'i')
	    && (cap->oap->op_type != OP_NOP || VIsual_active))
    {
#ifdef FEAT_TEXTOBJ
	nv_object(cap);
#else
	clearopbeep(cap->oap);
#endif
    }
#ifdef FEAT_TERMINAL
    else if (term_in_normal_mode())
    {
	clearop(cap->oap);
	term_enter_job_mode();
	return;
    }
#endif
    else if (!curbuf->b_p_ma && !p_im)
    {
	// Only give this error when 'insertmode' is off.
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	clearop(cap->oap);
	if (cap->cmdchar == K_PS)
	    // drop the pasted text
	    bracketed_paste(PASTE_INSERT, TRUE, NULL);
    }
    else if (cap->cmdchar == K_PS && VIsual_active)
    {
	pos_T old_pos = curwin->w_cursor;
	pos_T old_visual = VIsual;
	int old_visual_mode = VIsual_mode;

	// In Visual mode the selected text is deleted.
	if (VIsual_mode == 'V' || curwin->w_cursor.lnum != VIsual.lnum)
	{
	    shift_delete_registers();
	    cap->oap->regname = '1';
	}
	else
	    cap->oap->regname = '-';
	cap->cmdchar = 'd';
	cap->nchar = NUL;
	nv_operator(cap);
	do_pending_operator(cap, 0, FALSE);
	cap->cmdchar = K_PS;

	if (*ml_get_cursor() != NUL)
	{
	    if (old_visual_mode == 'V')
	    {
		// In linewise Visual mode insert before the beginning of the
		// next line.
		// When the last line in the buffer was deleted then create a
		// new line, otherwise there is not need to move cursor.
		// Detect this by checking if cursor moved above Visual area.
		if (curwin->w_cursor.lnum < old_pos.lnum
				&& curwin->w_cursor.lnum < old_visual.lnum)
		{
		    if (u_save_cursor() == OK)
		    {
			ml_append(curwin->w_cursor.lnum, (char_u *)"", 0,
									FALSE);
			appended_lines(curwin->w_cursor.lnum++, 1L);
		    }
		}
	    }
	    // When the last char in the line was deleted then append.
	    // Detect this by checking if cursor moved before Visual area.
	    else if (curwin->w_cursor.col < old_pos.col
				&& curwin->w_cursor.col < old_visual.col)
		inc_cursor();
	}

	// Insert to replace the deleted text with the pasted text.
	invoke_edit(cap, FALSE, cap->cmdchar, FALSE);
    }
    else if (!checkclearopq(cap->oap))
    {
	switch (cap->cmdchar)
	{
	    case 'A':	// "A"ppend after the line
		set_cursor_for_append_to_line();
		break;

	    case 'I':	// "I"nsert before the first non-blank
		if (vim_strchr(p_cpo, CPO_INSEND) == NULL)
		    beginline(BL_WHITE);
		else
		    beginline(BL_WHITE|BL_FIX);
		break;

	    case K_PS:
		// Bracketed paste works like "a"ppend, unless the cursor is in
		// the first column, then it inserts.
		if (curwin->w_cursor.col == 0)
		    break;
		// FALLTHROUGH

	    case 'a':	// "a"ppend is like "i"nsert on the next character.
		// increment coladd when in virtual space, increment the
		// column otherwise, also to append after an unprintable char
		if (virtual_active()
			&& (curwin->w_cursor.coladd > 0
			    || *ml_get_cursor() == NUL
			    || *ml_get_cursor() == TAB))
		    curwin->w_cursor.coladd++;
		else if (*ml_get_cursor() != NUL)
		    inc_cursor();
		break;
	}

	if (curwin->w_cursor.coladd && cap->cmdchar != 'A')
	{
	    int save_State = State;

	    // Pretend Insert mode here to allow the cursor on the
	    // character past the end of the line
	    State = MODE_INSERT;
	    coladvance(getviscol());
	    State = save_State;
	}

	invoke_edit(cap, FALSE, cap->cmdchar, FALSE);
    }
    else if (cap->cmdchar == K_PS)
	// drop the pasted text
	bracketed_paste(PASTE_INSERT, TRUE, NULL);
}