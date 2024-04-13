nv_cursormark(cmdarg_T *cap, int flag, pos_T *pos)
{
    if (check_mark(pos) == FAIL)
	clearop(cap->oap);
    else
    {
	if (cap->cmdchar == '\''
		|| cap->cmdchar == '`'
		|| cap->cmdchar == '['
		|| cap->cmdchar == ']')
	    setpcmark();
	curwin->w_cursor = *pos;
	if (flag)
	    beginline(BL_WHITE | BL_FIX);
	else
	    check_cursor();
    }
    cap->oap->motion_type = flag ? MLINE : MCHAR;
    if (cap->cmdchar == '`')
	cap->oap->use_reg_one = TRUE;
    cap->oap->inclusive = FALSE;		// ignored if not MCHAR
    curwin->w_set_curswant = TRUE;
}