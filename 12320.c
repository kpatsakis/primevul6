nv_gv_cmd(cmdarg_T *cap)
{
    pos_T	tpos;
    int		i;

    if (checkclearop(cap->oap))
	return;

    if (curbuf->b_visual.vi_start.lnum == 0
	    || curbuf->b_visual.vi_start.lnum > curbuf->b_ml.ml_line_count
	    || curbuf->b_visual.vi_end.lnum == 0)
    {
	beep_flush();
	return;
    }

    // set w_cursor to the start of the Visual area, tpos to the end
    if (VIsual_active)
    {
	i = VIsual_mode;
	VIsual_mode = curbuf->b_visual.vi_mode;
	curbuf->b_visual.vi_mode = i;
# ifdef FEAT_EVAL
	curbuf->b_visual_mode_eval = i;
# endif
	i = curwin->w_curswant;
	curwin->w_curswant = curbuf->b_visual.vi_curswant;
	curbuf->b_visual.vi_curswant = i;

	tpos = curbuf->b_visual.vi_end;
	curbuf->b_visual.vi_end = curwin->w_cursor;
	curwin->w_cursor = curbuf->b_visual.vi_start;
	curbuf->b_visual.vi_start = VIsual;
    }
    else
    {
	VIsual_mode = curbuf->b_visual.vi_mode;
	curwin->w_curswant = curbuf->b_visual.vi_curswant;
	tpos = curbuf->b_visual.vi_end;
	curwin->w_cursor = curbuf->b_visual.vi_start;
    }

    VIsual_active = TRUE;
    VIsual_reselect = TRUE;

    // Set Visual to the start and w_cursor to the end of the Visual
    // area.  Make sure they are on an existing character.
    check_cursor();
    VIsual = curwin->w_cursor;
    curwin->w_cursor = tpos;
    check_cursor();
    update_topline();

    // When called from normal "g" command: start Select mode when
    // 'selectmode' contains "cmd".  When called for K_SELECT, always
    // start Select mode.
    if (cap->arg)
    {
	VIsual_select = TRUE;
	VIsual_select_reg = 0;
    }
    else
	may_start_select('c');
    setmouse();
#ifdef FEAT_CLIPBOARD
    // Make sure the clipboard gets updated.  Needed because start and
    // end are still the same, and the selection needs to be owned
    clip_star.vmode = NUL;
#endif
    redraw_curbuf_later(INVERTED);
    showmode();
}