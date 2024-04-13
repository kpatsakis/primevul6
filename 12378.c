unadjust_for_sel(void)
{
    pos_T	*pp;

    if (*p_sel == 'e' && !EQUAL_POS(VIsual, curwin->w_cursor))
    {
	if (LT_POS(VIsual, curwin->w_cursor))
	    pp = &curwin->w_cursor;
	else
	    pp = &VIsual;
	if (pp->coladd > 0)
	    --pp->coladd;
	else
	if (pp->col > 0)
	{
	    --pp->col;
	    mb_adjustpos(curbuf, pp);
	}
	else if (pp->lnum > 1)
	{
	    --pp->lnum;
	    pp->col = (colnr_T)STRLEN(ml_get(pp->lnum));
	    return TRUE;
	}
    }
    return FALSE;
}