nv_csearch(cmdarg_T *cap)
{
    int		t_cmd;

    if (cap->cmdchar == 't' || cap->cmdchar == 'T')
	t_cmd = TRUE;
    else
	t_cmd = FALSE;

    cap->oap->motion_type = MCHAR;
    if (IS_SPECIAL(cap->nchar) || searchc(cap, t_cmd) == FAIL)
	clearopbeep(cap->oap);
    else
    {
	curwin->w_set_curswant = TRUE;
	// Include a Tab for "tx" and for "dfx".
	if (gchar_cursor() == TAB && virtual_active() && cap->arg == FORWARD
		&& (t_cmd || cap->oap->op_type != OP_NOP))
	{
	    colnr_T	scol, ecol;

	    getvcol(curwin, &curwin->w_cursor, &scol, NULL, &ecol);
	    curwin->w_cursor.coladd = ecol - scol;
	}
	else
	    curwin->w_cursor.coladd = 0;
	adjust_for_sel(cap);
#ifdef FEAT_FOLDING
	if ((fdo_flags & FDO_HOR) && KeyTyped && cap->oap->op_type == OP_NOP)
	    foldOpenCursor();
#endif
    }
}