nv_esc(cmdarg_T *cap)
{
    int		no_reason;

    no_reason = (cap->oap->op_type == OP_NOP
		&& cap->opcount == 0
		&& cap->count0 == 0
		&& cap->oap->regname == 0
		&& !p_im);

    if (cap->arg)		// TRUE for CTRL-C
    {
	if (restart_edit == 0
#ifdef FEAT_CMDWIN
		&& cmdwin_type == 0
#endif
		&& !VIsual_active
		&& no_reason)
	{
	    if (anyBufIsChanged())
		msg(_("Type  :qa!  and press <Enter> to abandon all changes and exit Vim"));
	    else
		msg(_("Type  :qa  and press <Enter> to exit Vim"));
	}

	// Don't reset "restart_edit" when 'insertmode' is set, it won't be
	// set again below when halfway a mapping.
	if (!p_im)
	    restart_edit = 0;
#ifdef FEAT_CMDWIN
	if (cmdwin_type != 0)
	{
	    cmdwin_result = K_IGNORE;
	    got_int = FALSE;	// don't stop executing autocommands et al.
	    return;
	}
#endif
    }
#ifdef FEAT_CMDWIN
    else if (cmdwin_type != 0 && ex_normal_busy)
    {
	// When :normal runs out of characters while in the command line window
	// vgetorpeek() will return ESC.  Exit the cmdline window to break the
	// loop.
	cmdwin_result = K_IGNORE;
	return;
    }
#endif

    if (VIsual_active)
    {
	end_visual_mode();	// stop Visual
	check_cursor_col();	// make sure cursor is not beyond EOL
	curwin->w_set_curswant = TRUE;
	redraw_curbuf_later(INVERTED);
    }
    else if (no_reason)
	vim_beep(BO_ESC);
    clearop(cap->oap);

    // A CTRL-C is often used at the start of a menu.  When 'insertmode' is
    // set return to Insert mode afterwards.
    if (restart_edit == 0 && goto_im() && ex_normal_busy == 0)
	restart_edit = 'a';
}