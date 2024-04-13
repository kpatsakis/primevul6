nv_g_home_m_cmd(cmdarg_T *cap)
{
    int		i;
    int		flag = FALSE;

    if (cap->nchar == '^')
	flag = TRUE;

    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = FALSE;
    if (curwin->w_p_wrap && curwin->w_width != 0)
    {
	int		width1 = curwin->w_width - curwin_col_off();
	int		width2 = width1 + curwin_col_off2();

	validate_virtcol();
	i = 0;
	if (curwin->w_virtcol >= (colnr_T)width1 && width2 > 0)
	    i = (curwin->w_virtcol - width1) / width2 * width2 + width1;
    }
    else
	i = curwin->w_leftcol;
    // Go to the middle of the screen line.  When 'number' or
    // 'relativenumber' is on and lines are wrapping the middle can be more
    // to the left.
    if (cap->nchar == 'm')
	i += (curwin->w_width - curwin_col_off()
		+ ((curwin->w_p_wrap && i > 0)
		    ? curwin_col_off2() : 0)) / 2;
    coladvance((colnr_T)i);
    if (flag)
    {
	do
	    i = gchar_cursor();
	while (VIM_ISWHITE(i) && oneright() == OK);
	curwin->w_valid &= ~VALID_WCOL;
    }
    curwin->w_set_curswant = TRUE;
}