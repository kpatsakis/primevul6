adjust_for_sel(cmdarg_T *cap)
{
    if (VIsual_active && cap->oap->inclusive && *p_sel == 'e'
	    && gchar_cursor() != NUL && LT_POS(VIsual, curwin->w_cursor))
    {
	if (has_mbyte)
	    inc_cursor();
	else
	    ++curwin->w_cursor.col;
	cap->oap->inclusive = FALSE;
    }
}